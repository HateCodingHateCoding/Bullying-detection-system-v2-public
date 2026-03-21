/**
 ******************************************************************************
 * @file    hal_uart.c
 * @brief   uart function define.
 * @verbatim    null
 * @note    If using a baud rate of 1M or higher, it is recommended to
 *          enable CONFIG_CLK_DCXO_EN and CONFIG_CLK_DCXO_OUT_PLL and
 *          set CONFIG_CLK_PLL_SRC to 1, in prj_config.h
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_board.h"
#include "hal_uart.h"
#include "ll_uart.h"
#include "ll_rcc_dev.h"
#include "ll_rcc_bus.h"
#include "ll_gpio.h"
#include "hal_dma.h"
#include "ll_rtctick.h"
#include "ll_utils.h"
#include <errno.h>

#define LOG_MODULE                      "UART"

#include "log.h"


/* Private typedef.
 * ----------------------------------------------------------------------------
 */
typedef struct {
    uint32_t IER;
} UART_Register_back_t;

typedef struct {
#if CONFIG_UART_WUP_ENABLED
    uint16_t        threshold;
    uint8_t         ctrl;
    uint8_t         wk_uart_id;
    HAL_Callback_t  wk_cb;
#endif

    UART_WakeupMode_t       wk_mode;
    UART_Register_back_t    register_back;
} UART_Advanced_param_t;

typedef struct {
    HAL_Dev_t            device;
    UART_ID_t            uart_id;
    UART_InitParam_t     init_param;

    uint8_t *txbuf;
    uint16_t tx_size;

    uint8_t *rxbuf;
    uint16_t rx_size;

    UART_TransMode_t tx_mode;
    UART_TransMode_t rx_mode;

#if CONFIG_UART_TRF_IT_ENABLE
    HAL_Callback_t isr_tx_cb;                 /* UART ISR TX callback function */
    HAL_Callback_t isr_rx_cb;                 /* UART ISR RX callback function */
#endif

#if CONFIG_UART_TRF_DMA_ENABLE
    HAL_Callback_t dma_tx_cb;                 /* UART DMA TX callback function */
    HAL_Callback_t dma_rx_cb;                 /* UART DMA RX callback function */
    uint32_t        dma_tx_id;
    uint32_t        dma_rx_id;

    HAL_Dev_t *dma_tx_device;
    HAL_Dev_t *dma_rx_device;
#endif

#if (CONFIG_UART_TRF_IT_ENABLE || CONFIG_UART_TRF_DMA_ENABLE)
    HAL_Semaphore tx_sem;
    HAL_Semaphore rx_sem;
#endif

#if CONFIG_UART_PM
    UART_Advanced_param_t adv_param;
#endif
} uart_device_t;

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define DEVICE_ID_GET(ID)                                       (ID >> 8)
#define DEVICE_TYPE_GET(ID)                                     (ID & 0XFF)

/**
  * @brief UART register definition
  */
#define UART0                               (UART0_DEV)
#define UART1                               (UART1_DEV)
#define UART2                               (UART2_DEV)


/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
uart_dev_t *uart_instance[UART_NUM] = {UART0, UART1, UART2};

static HAL_Dev_t *uart_device[UART_NUM];

uint32_t test_frac_div = 0;
uint32_t test_bus_freq = 0;
uint32_t test_baud_rate = 0;



/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static void UART_IRQ_Handler(void *hdev);

#if CONFIG_UART_WUP_ENABLED
static HAL_Status_t Uart_wakeup_enable(HAL_Dev_t *uart);
#endif


/* Exported functions.
 * ----------------------------------------------------------------------------
 */
static uint32_t Uart_clk_set(UART_ID_t UartID)
{
    uint32_t freq;
    uint32_t baudRate;
    uart_device_t *temp_device = (uart_device_t *)HAL_DEV_Find(HAL_DEV_TYPE_UART, UartID);
    if (temp_device == NULL) {
        return 0;
    }
    baudRate = temp_device->init_param.baudRate;
    // It is best to use DCXO
    if (UartID == UART0_ID) {
        if (baudRate == 2000000) {
            if (HAL_BOARD_GetFreq(CLOCK_DCXO) != 0) {
                LL_RCC_UART0_SetClockSource(LL_RCC_UART0_HSE);
                LL_RCC_UART0_SetPrescaler(LL_RCC_UART0_DIV_1);
                freq = HAL_BOARD_GetFreq(CLOCK_DCXO);
            } else {
                LL_RCC_UART0_SetClockSource(LL_RCC_UART0_MSI);
                LL_RCC_UART0_SetPrescaler(LL_RCC_UART0_DIV_1);
                freq = HAL_BOARD_GetFreq(CLOCK_MSI);
            }
        } else {
            LL_RCC_UART0_SetClockSource(LL_RCC_UART0_APB1);
            LL_RCC_UART0_SetPrescaler(LL_RCC_UART0_DIV_1);
            freq = HAL_BOARD_GetFreq(CLOCK_APB1);
        }
    } else if (UartID == UART1_ID) {
        /*Select UART clock source and Prescaler*/
        if (baudRate == 2000000) {
            if (HAL_BOARD_GetFreq(CLOCK_DCXO) != 0) {
                LL_RCC_UART1_SetClockSource(LL_RCC_UART1_HSE);
                LL_RCC_UART1_SetPrescaler(LL_RCC_UART1_DIV_1);
                freq = HAL_BOARD_GetFreq(CLOCK_DCXO);
            } else {
                LL_RCC_UART1_SetClockSource(LL_RCC_UART1_MSI);
                LL_RCC_UART1_SetPrescaler(LL_RCC_UART1_DIV_1);
                freq = HAL_BOARD_GetFreq(CLOCK_MSI);
            }
        } else {
            LL_RCC_UART1_SetClockSource(LL_RCC_UART1_APB0);
            LL_RCC_UART1_SetPrescaler(LL_RCC_UART1_DIV_1);
            freq = HAL_BOARD_GetFreq(CLOCK_APB0);
        }
    } else {
        if (baudRate == 2000000) {
            if (HAL_BOARD_GetFreq(CLOCK_DCXO) != 0) {
                LL_RCC_UART2_SetClockSource(LL_RCC_UART2_HSE);
                LL_RCC_UART2_SetPrescaler(LL_RCC_UART2_DIV_1);
                freq = HAL_BOARD_GetFreq(CLOCK_DCXO);
            } else {
                LL_RCC_UART2_SetClockSource(LL_RCC_UART2_MSI);
                LL_RCC_UART2_SetPrescaler(LL_RCC_UART2_DIV_1);
                freq = HAL_BOARD_GetFreq(CLOCK_MSI);
            }
        } else {
            LL_RCC_UART2_SetClockSource(LL_RCC_UART2_APB1);
            LL_RCC_UART2_SetPrescaler(LL_RCC_UART2_DIV_1);
            freq = HAL_BOARD_GetFreq(CLOCK_APB1);
        }
    }

    return freq;
}
__HW_STATIC_INLINE uint32_t Uart_clk_get(UART_ID_t UartID)
{
    uint8_t uart_clk_source;
    uint8_t uart_clk_div;
    uint32_t uart_clk_fre = 0;
    uint8_t div_param[] = {1, 2, 4, 8, 16};
    int32_t fre = 0;

    if (UartID == UART1_ID) {
        /*APB0 CLK*/
        uart_clk_source = LL_RCC_UART1_GetClockSource();
        uart_clk_div = LL_RCC_UART1_GetPrescaler();
    } else if (UartID == UART0_ID) {
        /*APB1 CLK*/
        uart_clk_source = LL_RCC_UART0_GetClockSource();
        uart_clk_div = LL_RCC_UART0_GetPrescaler();
    } else {
        /*APB1 CLK*/
        uart_clk_source = LL_RCC_UART2_GetClockSource();
        uart_clk_div = LL_RCC_UART2_GetPrescaler();
    }

    switch (uart_clk_source) {
    case LL_RCC_UART0_APB1:
        fre = HAL_BOARD_GetFreq(CLOCK_APB1);
        uart_clk_fre = fre / div_param[uart_clk_div];
        break;
    case LL_RCC_UART0_MSI:
        uart_clk_fre = 32000000 / div_param[uart_clk_div];
        break;
    case LL_RCC_UART0_HSE:
        break;
    case LL_RCC_UART0_PLL_DEV:
        fre = HAL_BOARD_GetFreq(CLOCK_PLL_SOC);
        uart_clk_fre = fre / div_param[uart_clk_div];
        break;
    case LL_RCC_UART0_PLL_DEV1:
        break;
    default:
        break;
    }

    return uart_clk_fre;
}

__HW_STATIC_INLINE void Uart_clock_enable(UART_ID_t UartID)
{
    if (UartID == UART0_ID) {
        LL_RCC_UART0_EnableBusClock();

        LL_RCC_UART0_EnableClock();

        LL_RCC_UART0_Reset();
    } else if (UartID == UART1_ID) {
        LL_RCC_UART1_EnableBusClock();

        LL_RCC_UART1_EnableClock();

        LL_RCC_UART1_Reset();
    } else {
        LL_RCC_UART2_EnableBusClock();

        LL_RCC_UART2_EnableClock();

        LL_RCC_UART2_Reset();
    }
}

__HW_STATIC_INLINE void Uart_clock_disable(UART_ID_t UartID)
{
    if (UartID == UART0_ID) {
        LL_RCC_UART0_EnableBusClock();

        LL_RCC_UART0_DisableClock();
    } else if (UartID == UART1_ID) {
        LL_RCC_UART1_EnableBusClock();

        LL_RCC_UART1_DisableClock();
    } else {
        LL_RCC_UART2_EnableBusClock();

        LL_RCC_UART2_DisableClock();
    }
}

#if CONFIG_UART_TRF_IT_ENABLE
static void Uart_isr_tx_callback(HAL_Dev_t *dev, void *param)
{
    uart_device_t *temp_dev = (uart_device_t *)dev;

    if (temp_dev->tx_size != 0 && temp_dev->txbuf) {
        while (temp_dev->tx_size) {
            if (HAL_UART_IsTxReady(dev)) {
                HAL_UART_PutTxData((HAL_Dev_t *)dev, *temp_dev->txbuf);
                ++temp_dev->txbuf;
                --temp_dev->tx_size;
            } else {
                break;
            }
        }
        if (temp_dev->tx_size == 0) {  // complete transmiting
            HAL_UART_DisableTxReadyIRQ((HAL_Dev_t *)dev);   // disable interrupt

            OSI_SemaphoreRelease(&temp_dev->tx_sem);   // release tx smeaphore
        }
    } else {
        HAL_UART_DisableTxReadyIRQ((HAL_Dev_t *)dev);   // disable interrupt
    }
}

static void Uart_isr_rx_callback(HAL_Dev_t *dev, void *param)
{
    uart_device_t *temp_dev = (uart_device_t *)dev;

    if (temp_dev->rxbuf && temp_dev->rx_size) {
        while (temp_dev->rx_size) {
            if (HAL_UART_IsRxReady(dev)) {
                *temp_dev->rxbuf = HAL_UART_GetRxData(dev);
                ++temp_dev->rxbuf;
                --temp_dev->rx_size;
            } else {
                break;
            }
        }

        if (temp_dev->rx_size == 0) {  // complete receiving
            HAL_UART_DisableRxReadyIRQ(dev);   // disable interrupt
            OSI_SemaphoreRelease(&temp_dev->rx_sem);   // release Rx smeaphore
        }
    } else {
        HAL_UART_DisableRxReadyIRQ(dev);   // disable interrupt
    }

}

#endif

#if CONFIG_UART_TRF_DMA_ENABLE
static void Uart_dma_callback(HAL_Dev_t *dma, void *param)
{
    if (param != NULL) {
        HAL_SemaphoreRelease((HAL_Semaphore *)param);
    }
}

__HW_STATIC_INLINE HAL_Status_t Uart_dma_config(HAL_Dev_t *hdev, UART_TransDir_t direction)
{
    HAL_Dev_t *temp_dma;
    DMA_ChannelConf_t chan_cfg;
    HAL_Status_t status;
    HAL_Callback_t *tmp_cb;
    uart_device_t *temp_dev = (uart_device_t *)hdev;
    int32_t tmp_id;
    uint8_t handshak[UART_NUM * 2] = {DMA_HANDSHAKE_RXFIFO_UART0, DMA_HANDSHAKE_TXFIFO_UART0,
                                      DMA_HANDSHAKE_RXFIFO_UART1, DMA_HANDSHAKE_TXFIFO_UART1,
                                      DMA_HANDSHAKE_RXFIFO_UART2, DMA_HANDSHAKE_TXFIFO_UART2};

    memset(&chan_cfg, 0, sizeof(DMA_ChannelConf_t));

    /* PSRAM width must be 4 byte */
    chan_cfg.srcBrustNum = DMA_BURSTNUM_1;
    chan_cfg.dstBrustnum = DMA_BURSTNUM_1;
    chan_cfg.srcWidth = DMA_BITWIDTH_8;
    chan_cfg.dstWidth = DMA_BITWIDTH_8;
    /* Channel priority */
    chan_cfg.priority = DMA_PRIORITY_7;

    if (direction) {
        chan_cfg.direction = DMA_DIRECTION_PER2MEM;   // receive data
        chan_cfg.srcHandShake = handshak[temp_dev->uart_id * 2];
        chan_cfg.srcIncreMode = DMA_ADDR_FIXED;
        chan_cfg.dstIncreMode = DMA_ADDR_INCREMENT;

        tmp_cb = &temp_dev->dma_rx_cb;
        temp_dma = temp_dev->dma_rx_device;

        /* Set configuration */
        tmp_id = HAL_DMA_Open(temp_dma, &chan_cfg);
        if (tmp_id < 0) {
            return HAL_STATUS_ERROR;
        }
        temp_dev->dma_rx_id = tmp_id;
    } else {
        chan_cfg.direction = DMA_DIRECTION_MEM2PER;   // transmit data
        chan_cfg.dstHandShake = handshak[temp_dev->uart_id * 2 + 1];
        chan_cfg.srcIncreMode = DMA_ADDR_INCREMENT;
        chan_cfg.dstIncreMode = DMA_ADDR_FIXED;

        tmp_cb = &temp_dev->dma_tx_cb;
        temp_dma = temp_dev->dma_tx_device;

        /* Set configuration */
        tmp_id = HAL_DMA_Open(temp_dma, &chan_cfg);
        if (tmp_id < 0) {
            return HAL_STATUS_ERROR;
        }
        temp_dev->dma_tx_id = tmp_id;
    }

    /* Register callback */
    status = HAL_DMA_RegisterIRQ(temp_dma, tmp_id, DMA_EVENT_DONE, tmp_cb);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    status = HAL_DMA_EnableIRQ(temp_dma, tmp_id, DMA_EVENT_DONE);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}
#endif

__uart_sram_text static void UART_IRQ_Handler(void *hdev)
{
    uint32_t irq_flag;
    uart_device_t *temp_dev = (uart_device_t *)hdev;

    irq_flag = LL_UART_GetIrqFlag(uart_instance[temp_dev->uart_id]);

    switch (irq_flag) {
    case LL_UART_ISR_MODEM_STAT:
        break;
    case LL_UART_ISR_NO_INTR_PEND:
        break;
    case LL_UART_ISR_TDR_EMPTY:
        if (temp_dev->isr_tx_cb.cb != NULL) {
            /* excute tx callback function after transmit all data */
            temp_dev->isr_tx_cb.cb((HAL_Dev_t *)hdev, temp_dev->isr_tx_cb.arg);
        } else {
            HAL_UART_DisableTxReadyIRQ((HAL_Dev_t *)hdev);   // disable interrupt
        }
        break;
    case LL_UART_ISR_RD_AVAILABLE:
    case LL_UART_ISR_CHAR_TO:
        if (temp_dev->isr_rx_cb.cb != NULL) {
            temp_dev->isr_rx_cb.cb((HAL_Dev_t *)hdev, temp_dev->isr_rx_cb.arg);
        } else {
            HAL_UART_DisableRxReadyIRQ((HAL_Dev_t *)hdev);   // disable interrupt
        }
        break;
    case LL_UART_ISR_LINE_STAT:
        LL_UART_GetLineStatus(uart_instance[temp_dev->uart_id]);  // read for clear flag
        break;
    case LL_UART_ISR_BUSY_DETECT:
        break;
    default:
        break;
    }
}


/**
 * @brief Initialize the UART mode according to the specified
 *        parameters in the UART_InitTypeDef and initialize the associated handle.
 * @param huart UART handle.
 * @retval hw status
 */
int Uart_config(HAL_Dev_t *dev)
{
    uint32_t bus_freq;
    uart_device_t *temp_dev = (uart_device_t *)dev;
    HAL_Pinmux_t temp_pinmux;
    HAL_Status_t temp_result;

    /* uart clock configuration */
    bus_freq = Uart_clk_set(temp_dev->uart_id);
    /* uart gpio configuration */
    temp_result = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_UART, temp_dev->uart_id, HAL_PINMUX_TYPE_INIT, &temp_pinmux);
    if (temp_result != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    HAL_BOARD_SetPinMux(&temp_pinmux);

    Uart_clock_enable(temp_dev->uart_id);  // ENABLE UART CLOCK

    LL_UART_Reset(uart_instance[temp_dev->uart_id]);

    LL_UART_SetParity(uart_instance[temp_dev->uart_id], temp_dev->init_param.parity);
    LL_UART_SetStopBitsLength(uart_instance[temp_dev->uart_id], temp_dev->init_param.stopBits);
    LL_UART_SetDataWidth(uart_instance[temp_dev->uart_id], temp_dev->init_param.dataBits);
    LL_UART_SetHWFlowCtrl(uart_instance[temp_dev->uart_id], temp_dev->init_param.autoFlowCtrl);
    if (temp_dev->init_param.autoFlowCtrl) {
        LL_UART_SetRTS(uart_instance[temp_dev->uart_id], LL_UART_FLW_ASSERTED);
    }
    //bus_freq = Uart_clk_get(temp_dev->uart_id);  // calculate uart clock frequence

    LL_UART_SetPrescaler(uart_instance[temp_dev->uart_id], temp_dev->init_param.baudRate, bus_freq);

    LL_UART_SetTXFIFOThreshold(uart_instance[temp_dev->uart_id], LL_UART_RX_FIFO_THRES_1_2);
    LL_UART_SetRXFIFOThreshold(uart_instance[temp_dev->uart_id], LL_UART_RX_FIFO_THRES_1_2);

    LL_UART_RequestTxDataFlush(uart_instance[temp_dev->uart_id]);
    LL_UART_RequestRxDataFlush(uart_instance[temp_dev->uart_id]);
    LL_UART_EnableFIFO(uart_instance[temp_dev->uart_id]);

    return 0;
}

#if CONFIG_UART_WUP_ENABLED
static HAL_Status_t Uart_wakeup_enable(HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    HAL_Pinmux_t temp_pinmux;
    uint8_t i, j = 0;
    uint8_t wakeup_rx_pin_list[9][2] = {{1, GPIOA_P1_F3_UART1_RX},
                                        {3, GPIOA_P3_F5_UART2_RX},
                                        {7, GPIOA_P7_F3_UART2_RX},
                                        {9, GPIOA_P9_F2_UART1_RX},
                                        {12, GPIOA_P12_F2_UART2_RX},
                                        {14, GPIOA_P14_F3_UART2_RX},
                                        {1, GPIOB_P1_F2_UART0_RX},
                                    };
    /* wakeup enable */
    if (temp_dev->adv_param.threshold > 15) {
        return HAL_STATUS_INVALID_STATE;
    }

    /* get wakeup pin */
    HAL_BOARD_GetPinMux(HAL_DEV_TYPE_UART, temp_dev->uart_id, HAL_PINMUX_TYPE_INIT, &temp_pinmux);

    for (i = 0; i < temp_pinmux.pinCnt; i++) {
        for (j = 0; j < 9; j++) {
            if (temp_pinmux.pinConfig[i].config.mode == wakeup_rx_pin_list[j][1] &&
                temp_pinmux.pinConfig[i].pin == wakeup_rx_pin_list[j][0]) {
                break;
            }
        }
        if (j < 9) {
            break;
        }
    }
    if (j >= 9) {
        return HAL_STATUS_INVALID_PARAM;
    }

    temp_dev->adv_param.wk_uart_id = temp_dev->uart_id;

    if (LL_SYS_GetCurrentSysId()) {
        LL_WKUP_EnableSrc(LL_WKUP_SRC_UART, 1);
    } else {
        LL_WKUP_EnableSrc(LL_WKUP_SRC_UART, 0);
    }

    LL_WKUP_UART_ClrFlag();
    LL_WKUP_UART_Reset();

    LL_WKUP_UART_SetPin(j);

    if (temp_dev->adv_param.threshold == 0) {
        LL_WKUP_UART_SetTrget(8);
    } else {
        LL_WKUP_UART_SetTrget(temp_dev->adv_param.threshold);
    }

    LL_WKUP_UART_Enable();

    return HAL_STATUS_OK;
}

static void Uart_wakeup_disable(HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    LL_WKUP_UART_ClrFlag();

    LL_WKUP_UART_Disable();

    temp_dev->adv_param.wk_uart_id = UART_INVALID_ID;
}

#endif
/* Exported functions.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_UART_PM
HAL_Status_t Uart_suspend(HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    HAL_Status_t status = HAL_STATUS_OK;

    if (temp_dev->adv_param.wk_mode == UART_WK_MODE_AUTO) {
        status  = HAL_UART_PM_Suspend(uart);
    }

#if CONFIG_UART_WUP_ENABLED
    LL_WKUP_UART_Reset();
#endif
    return status;
}

HAL_Status_t Uart_resume(HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    HAL_Status_t status = HAL_STATUS_OK;

#if CONFIG_UART_WUP_ENABLED
    if(temp_dev->adv_param.wk_uart_id == temp_dev->uart_id) {
        if (LL_WKUP_UART_GetFlag()) {
            LL_WKUP_UART_ClrFlag();
            if (temp_dev->adv_param.wk_cb.cb != NULL) {
                temp_dev->adv_param.wk_cb.cb(uart, temp_dev->adv_param.wk_cb.arg);
            }
        }
    }
#endif

    if (temp_dev->adv_param.wk_mode == UART_WK_MODE_AUTO) {
        status = HAL_UART_PM_Resume(uart);
    }

    return status;
}

static int Uart_pm_action(void *dev, enum pm_device_action action)
{
    int ret = -1;

    switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
        ret = Uart_suspend((HAL_Dev_t *)dev);
        break;
    case PM_DEVICE_ACTION_RESUME:
        ret = Uart_resume((HAL_Dev_t *)dev);
        break;
    default:
        break;
    }
    return (ret == HAL_STATUS_OK) ? 0 : -1;
}

static void Uart_pm_register(HAL_Dev_t *dev)
{
    struct pm_device *pm_dev = &dev->pmDev;
    pm_dev->action = Uart_pm_action;
    pm_dev->early_action = NULL;
    pm_dev->flags = 0;
    pm_device_register(pm_dev, Uart_pm_action);
}

static void Uart_pm_unregister(HAL_Dev_t *dev)
{
    struct pm_device *pm_dev = &dev->pmDev;
    pm_device_unregister(pm_dev);
}

static void Uart_register_save(HAL_Dev_t *dev)
{
    uart_device_t *temp_dev = (uart_device_t *)dev;

    temp_dev->adv_param.register_back.IER = uart_instance[temp_dev->uart_id]->IER;
}

static void Uart_register_resume(HAL_Dev_t *dev)
{
    uart_device_t *temp_dev = (uart_device_t *)dev;

    uart_instance[temp_dev->uart_id]->IER = temp_dev->adv_param.register_back.IER;
}

HAL_Status_t HAL_UART_PM_Suspend(HAL_Dev_t *uart)
{
    HAL_Status_t status;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (uart->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (uart->state == HAL_DEV_STATE_CLOSE) {
        return HAL_STATUS_ALREADY;
    }

    Uart_register_save(uart);

    status = HAL_UART_Close(uart);

    return status;
}

HAL_Status_t HAL_UART_PM_Resume(HAL_Dev_t *uart)
{
    HAL_Status_t status;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (uart->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (uart->state == HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_ALREADY;
    }

    status = HAL_UART_Open(uart);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    Uart_register_resume(uart);

    return HAL_STATUS_OK;
}


#endif

/**
 * @brief Register pm callback function
 * @param uart - device handle.
 * @param irqCb - callback structure.
 * @retval status
 */
HAL_Status_t HAL_UART_ExtControl(const HAL_Dev_t *uart, UART_ParamType_t ptype, void *param)
{
#if CONFIG_UART_PM
    uart_device_t *temp_dev = (uart_device_t *)uart;
    uint8_t tmp = ptype;

    if (uart == NULL || param == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    switch (tmp) {
#if CONFIG_UART_WUP_ENABLED
    case UART_PARAM_WAKEUP_THRESHOLD:
        temp_dev->adv_param.threshold = *(uint16_t *)param;
        break;
    case UART_PARAM_WAKEUP_CTRL:
        if(*(uint8_t *)param) {
            if(temp_dev->adv_param.ctrl) {
                return HAL_STATUS_ALREADY;
            } else {
                temp_dev->adv_param.ctrl = *(uint8_t *)param;
                Uart_wakeup_enable((HAL_Dev_t *)uart);
            }
        } else {
            Uart_wakeup_disable((HAL_Dev_t *)uart);
        }
        break;
    case UART_PARAM_WAKEUP_CALLBACK:
        temp_dev->adv_param.wk_cb.cb = ((HAL_Callback_t *)param)->cb;
        temp_dev->adv_param.wk_cb.arg = ((HAL_Callback_t *)param)->arg;
        break;
#endif
    case UART_PARAM_WAKEUP_MODE:
        temp_dev->adv_param.wk_mode = *(UART_WakeupMode_t *)param;
        break;
    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }
#endif

    return HAL_STATUS_OK;
}


/**
 * @brief Initialize the UART peripheral.
 * @param id - UART ID
 * @param param - uart parameter
 * @retval device handle(NULL: failed)
 */
HAL_Dev_t * HAL_UART_Init(UART_ID_t id, const UART_InitParam_t *param)
{
    uart_device_t *temp_device = (uart_device_t *)HAL_DEV_Find(HAL_DEV_TYPE_UART, id);

    if (id > UART_NUM) {
        return NULL;
    }
    if (param == NULL) {
        return NULL;
    }
    if (param->baudRate > CONFIG_UART_MAX_BAUDRATE) {
        return NULL;
    }

    if (temp_device == NULL) {
        temp_device = (uart_device_t *)HAL_DEV_MemMalloc(sizeof(uart_device_t));
        if (temp_device == NULL) {
            return (HAL_Dev_t *)temp_device;
        }
        memset(temp_device, 0, sizeof(uart_device_t));
#if CONFIG_UART_WUP_ENABLED
        temp_device->adv_param.wk_uart_id = UART_INVALID_ID;
#endif
        temp_device->device.reg = uart_instance[id];
        if (id == UART0_ID) {
            temp_device->device.irqNum = UART0_IRQn;
            temp_device->device.irqHandler = UART_IRQ_Handler;
        } else if (id == UART1_ID) {
            temp_device->device.irqNum = UART1_IRQn;
            temp_device->device.irqHandler = UART_IRQ_Handler;
        } else {
            temp_device->device.irqNum = UART2_IRQn;
            temp_device->device.irqHandler = UART_IRQ_Handler;
        }
        IRQ_Disable(temp_device->device.irqNum);
        IRQ_Attach(temp_device->device.irqNum, temp_device->device.irqHandler);
        IRQ_AttachDevice(temp_device->device.irqNum, (HAL_Dev_t *)temp_device);
        IRQ_Priority(temp_device->device.irqNum, 5);

        IRQ_Enable(temp_device->device.irqNum);

        uart_device[id] = (HAL_Dev_t *)temp_device;

        HAL_DEV_Register((HAL_Dev_t *)temp_device, HAL_DEV_TYPE_UART, id);
    }

    temp_device->uart_id = id;
    temp_device->tx_mode = UART_TRANS_MODE_NOMA;
    temp_device->rx_mode = UART_TRANS_MODE_NOMA;
    temp_device->txbuf = NULL;
    temp_device->tx_size = 0;
    temp_device->rxbuf = NULL;
    temp_device->rx_size = 0;
#if CONFIG_UART_TRF_IT_ENABLE
    temp_device->isr_tx_cb.cb = NULL;
    temp_device->isr_rx_cb.cb = NULL;
#endif

#if CONFIG_UART_TRF_DMA_ENABLE
    temp_device->dma_tx_cb.cb = NULL;
    temp_device->dma_rx_cb.cb = NULL;
    temp_device->dma_rx_device = NULL;
    temp_device->dma_tx_device = NULL;
#endif

#if CONFIG_UART_PM
    Uart_pm_register((HAL_Dev_t *)temp_device);
#endif

#if (CONFIG_UART_TRF_IT_ENABLE || CONFIG_UART_TRF_DMA_ENABLE)
    HAL_SemaphoreSetInvalid(&temp_device->tx_sem);
    HAL_SemaphoreSetInvalid(&temp_device->rx_sem);
#endif

    memcpy(&temp_device->init_param, param, sizeof(UART_InitParam_t));

    return (HAL_Dev_t *)temp_device;
}

/**
 * @brief Open the UART peripheral.
 * @param uart - uart handle pointer
 * @retval status
 */
HAL_Status_t HAL_UART_Open(HAL_Dev_t *uart)
{
    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (uart->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }

    /* PM lock */

    /* Uart config */
    if (Uart_config(uart) != 0) {
        return HAL_STATUS_ERROR;
    }
    uart->state = HAL_DEV_STATE_OPEN;

    return HAL_STATUS_OK;
}

/**
 * @brief Close the UART peripheral.
 * @param uart - uart handle pointer
 * @retval status
 */
HAL_Status_t HAL_UART_Close(HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    HAL_Pinmux_t temp_pinmux;
    HAL_Status_t temp_result;

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (uart->state != HAL_DEV_STATE_UNINIT || uart->state != HAL_DEV_STATE_INIT) {
        uart->state = HAL_DEV_STATE_CLOSE;
    }

    /* uart gpio deinit */
    temp_result = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_UART, temp_dev->uart_id, HAL_PINMUX_TYPE_DEINIT, &temp_pinmux);
    if (temp_result != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    HAL_BOARD_SetPinMux(&temp_pinmux);

    /* PM unlock */
    Uart_clock_disable(temp_dev->uart_id);
    uart->state = HAL_DEV_STATE_CLOSE;

    return HAL_STATUS_OK;
}

/**
 * @brief DeInitialize the UART peripheral.
 * @param huart UART handle.
 * @retval HAL status
 */
HAL_Status_t HAL_UART_DeInit(HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Uart_clock_disable(temp_dev->uart_id);

#if CONFIG_UART_TRF_IT_ENABLE
    temp_dev->isr_tx_cb.cb = NULL;
    temp_dev->isr_rx_cb.cb = NULL;
#endif

#if CONFIG_UART_TRF_DMA_ENABLE
    /* deinit dma */
    temp_dev->dma_tx_cb.cb = NULL;
    temp_dev->dma_rx_cb.cb = NULL;
#endif

#if (CONFIG_UART_PM)
    Uart_pm_unregister(uart);
#endif


#if (CONFIG_UART_TRF_IT_ENABLE || CONFIG_UART_TRF_DMA_ENABLE)
    HAL_SemaphoreDeinit(&temp_dev->tx_sem);
    HAL_SemaphoreDeinit(&temp_dev->rx_sem);
    HAL_SemaphoreSetInvalid(&temp_dev->tx_sem);
    HAL_SemaphoreSetInvalid(&temp_dev->rx_sem);

#endif
    temp_dev->txbuf = NULL;
    temp_dev->tx_size = 0;
    temp_dev->rxbuf = NULL;
    temp_dev->rx_size = 0;


    HAL_DEV_Unregister(uart);
    HAL_DEV_MemFree(uart);

    return HAL_STATUS_OK;
}

/**
 * @brief Get uart TX ready flag
 * @param uart - device handle.
 * @retval flag
 */
__uart_sram_text bool HAL_UART_IsTxReady(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    return LL_UART_IsActiveFlag_TXFNF(uart_instance[temp_dev->uart_id]);
}

/**
 * @brief Get uart TX empty flag
 * @param uart - device handle.
 * @retval flag
 */
__uart_sram_text bool HAL_UART_IsTxEmpty(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    return LL_UART_IsActiveFlag_TXFE(uart_instance[temp_dev->uart_id]);
}

/**
 * @brief Get uart RX ready flag
 * @param uart - device handle.
 * @retval flag
 */
__uart_sram_text bool HAL_UART_IsRxReady(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    return LL_UART_IsActiveFlag_RXFNE(uart_instance[temp_dev->uart_id]);
}

/**
 * @brief Uart transmit 1 byte function
 * @param uart - device handle.
 * @param data - to be transmit
 * @retval
 */
__uart_sram_text void HAL_UART_PutTxData(const HAL_Dev_t *uart, uint8_t data)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    LL_UART_TransmitData(uart_instance[temp_dev->uart_id], data);
}

/**
 * @brief Uart receive function
 * @param uart - device handle.
 * @retval received data
 */
__uart_sram_text uint8_t HAL_UART_GetRxData(const HAL_Dev_t *uart)
{
    uint8_t temp_data;
    uart_device_t *temp_dev = (uart_device_t *)uart;

    temp_data = (uint8_t)LL_UART_ReceiveData(uart_instance[temp_dev->uart_id]);

    return temp_data;
}

/**
 * @brief Set uart transmisson mode
 * @param uart - device handle.
 * @param transDir - wirte or read
 * @param Mode - transmisson mode
 * @retval status
 */
HAL_Status_t HAL_UART_SetTransferMode(const HAL_Dev_t *uart, UART_TransDir_t transDir, UART_TransMode_t Mode)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (transDir == UART_DIR_TX) {
        temp_dev->tx_mode = Mode;
#if CONFIG_UART_TRF_DMA_ENABLE
        if (Mode == UART_TRANS_MODE_DMA) {
            HAL_Callback_t temp_cb = {.cb = Uart_dma_callback, .arg = ((void*)&temp_dev->tx_sem)};

            HAL_UART_RegisterDMA((HAL_Dev_t *)uart, UART_DIR_TX, &temp_cb);
        }
#endif
    } else {
        temp_dev->rx_mode = Mode;
#if CONFIG_UART_TRF_DMA_ENABLE
        if (Mode == UART_TRANS_MODE_DMA) {
            HAL_Callback_t temp_cb = {.cb = Uart_dma_callback, .arg = ((void*)&temp_dev->rx_sem)};

            HAL_UART_RegisterDMA((HAL_Dev_t *)temp_dev, UART_DIR_RX, &temp_cb);
        }
#endif
    }

    return HAL_STATUS_OK;
}

#if CONFIG_UART_TRF_IT_ENABLE
/**
 * @brief Enable TX interrupt function
 * @param uart - device handle.
 * @retval status
 */
__uart_sram_text HAL_Status_t HAL_UART_EnableTxReadyIRQ(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    LL_UART_EnableIT_FIFOThreshold(uart_instance[temp_dev->uart_id]);
    LL_UART_EnableIT_TD(uart_instance[temp_dev->uart_id]);

    return HAL_STATUS_OK;
}

/**
 * @brief Disable TX interrupt function
 * @param uart - device handle.
 * @retval status
 */
__uart_sram_text HAL_Status_t HAL_UART_DisableTxReadyIRQ(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    LL_UART_DisableIT_FIFOThreshold(uart_instance[temp_dev->uart_id]);
    LL_UART_DisableIT_TD(uart_instance[temp_dev->uart_id]);

    return HAL_STATUS_OK;
}

/**
 * @brief Enable RX interrupt function
 * @param uart - device handle.
 * @retval status
 */
HAL_Status_t HAL_UART_EnableRxReadyIRQ(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    LL_UART_EnableIT_RD(uart_instance[temp_dev->uart_id]);
    LL_UART_EnableIT_Line(uart_instance[temp_dev->uart_id]);

    return HAL_STATUS_OK;
}

/**
 * @brief Disable RX interrupt function
 * @param uart - device handle.
 * @retval status
 */
HAL_Status_t HAL_UART_DisableRxReadyIRQ(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    LL_UART_DisableIT_RD(uart_instance[temp_dev->uart_id]);
    LL_UART_DisableIT_Line(uart_instance[temp_dev->uart_id]);

    return HAL_STATUS_OK;
}

/**
 * @brief Register interrupt callback function
 * @param uart - device handle.
 * @param transDir - transmisson direction.
 * @param irqCb - callback structure.
 * @retval status
 */
HAL_Status_t HAL_UART_RegisterIRQ(HAL_Dev_t *uart, UART_TransDir_t transDir, const HAL_Callback_t *irqCb)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    if (irqCb == NULL || uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (transDir == UART_DIR_TX) {
        temp_dev->isr_tx_cb.cb = irqCb->cb;
        temp_dev->isr_tx_cb.arg = irqCb->arg;
    } else {
        temp_dev->isr_rx_cb.cb = irqCb->cb;
        temp_dev->isr_rx_cb.arg = irqCb->arg;
    }

    return HAL_STATUS_OK;
}

/**
 * @brief Unregister interrupt callback function
 * @param uart - device handle.
 * @param transDir - transmisson direction.
 * @retval status
 */
HAL_Status_t HAL_UART_UnregisterIRQ(const HAL_Dev_t *uart, UART_TransDir_t transDir)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (transDir == UART_DIR_TX) {
        temp_dev->isr_tx_cb.cb = NULL;
        temp_dev->isr_tx_cb.arg = NULL;
    } else {
        temp_dev->isr_rx_cb.cb = NULL;
        temp_dev->isr_rx_cb.arg = NULL;
    }
    return HAL_STATUS_OK;
}


#endif          // CONFIG_UART_TRF_IT_ENABLE


#if CONFIG_UART_TRF_DMA_ENABLE
/**
 * @brief Register dma callback function
 * @param uart - device handle.
 * @param transDir - transmisson direction.
 * @param irqCb - callback structure.
 * @retval status
 */
HAL_Status_t HAL_UART_RegisterDMA(HAL_Dev_t *uart, UART_TransDir_t transDir, HAL_Callback_t *dmaCb)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    if (dmaCb == NULL || uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (transDir == UART_DIR_TX) {
        temp_dev->dma_tx_cb.cb = dmaCb->cb;
        temp_dev->dma_tx_cb.arg = dmaCb->arg;

        temp_dev->dma_tx_device = HAL_DMA_Init(DMA0_ID);
        if (temp_dev->dma_tx_device == NULL) {
            return HAL_STATUS_ERROR;
        }
    } else {
        temp_dev->dma_rx_cb.cb = dmaCb->cb;
        temp_dev->dma_rx_cb.arg = dmaCb->arg;

        temp_dev->dma_rx_device = HAL_DMA_Init(DMA0_ID);
        if (temp_dev->dma_rx_device == NULL) {
            return HAL_STATUS_ERROR;
        }
    }

    if (HAL_STATUS_OK != Uart_dma_config(uart, transDir)) {
        return HAL_STATUS_ERROR;
    }


    return HAL_STATUS_OK;
}

/**
 * @brief Unregister dma callback function
 * @param uart - device handle.
 * @param transDir - transmisson direction.
 * @retval status
 */
HAL_Status_t HAL_UART_UnregisterDMA(const HAL_Dev_t *uart, UART_TransDir_t transDir)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (transDir == UART_DIR_TX) {
        temp_dev->dma_tx_cb.cb = NULL;
        temp_dev->dma_tx_cb.arg = NULL;

        if (HAL_STATUS_OK != HAL_DMA_Close(temp_dev->dma_tx_device, temp_dev->dma_tx_id)) {
            return HAL_STATUS_ERROR;
        }
    } else {
        temp_dev->dma_rx_cb.cb = NULL;
        temp_dev->dma_rx_cb.arg = NULL;

        if (HAL_STATUS_OK != HAL_DMA_Close(temp_dev->dma_rx_device, temp_dev->dma_rx_id)) {
            return HAL_STATUS_ERROR;
        }
    }

    return HAL_STATUS_OK;
}

/**
 * @brief Start dma transmission function
 * @param uart - device handle.
 * @param buf - data pointer.
 * @param size - length of buf.
 * @retval status
 */
HAL_Status_t HAL_UART_StartTransmitDMA(const HAL_Dev_t *uart, const uint8_t *buf, int32_t size)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    DMA_BlockConf_t blockcfg  = { 0 };
    HAL_Status_t status;

    if (uart == NULL || buf == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    csi_dcache_invalid_range((unsigned long *)buf, size);

    blockcfg.srcAddr    =   (uint32_t)buf;
    blockcfg.dstAddr    =   (uint32_t)&uart_instance[temp_dev->uart_id]->TDR;;
    blockcfg.blockSize  =   size;

    status = HAL_DMA_ConfigBlockTranfer(temp_dev->dma_tx_device, temp_dev->dma_tx_id, &blockcfg);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    /*clear FIFO and set FIFO*/
    LL_UART_SetTXFIFOThreshold(uart_instance[temp_dev->uart_id], LL_UART_TX_FIFO_THRES_1_2);
    LL_UART_RequestTxDataFlush(uart_instance[temp_dev->uart_id]);
    LL_UART_EnableFIFO(uart_instance[temp_dev->uart_id]);

    temp_dev->tx_size = size;

    /* launch DMA transfer */
    if (HAL_STATUS_OK != HAL_DMA_StartTransfer(temp_dev->dma_tx_device, temp_dev->dma_tx_id)) {
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

/**
 * @brief stop dma transmission function
 * @param uart - device handle.
 * @retval status
 */
int32_t HAL_UART_StopTransmitDMA(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    uint32_t temp_size = 0;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DMA_AbortTransfer(temp_dev->dma_tx_device, temp_dev->dma_tx_id);

    return temp_size;
}

/**
 * @brief Start dma receive function
 * @param uart - device handle.
 * @param buf - data pointer.
 * @param size - length of buf.
 * @retval status
 */
HAL_Status_t HAL_UART_StartReceiveDMA(const HAL_Dev_t *uart, uint8_t *buf, int32_t size)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    DMA_BlockConf_t blockcfg  = { 0 };
    HAL_Status_t status;

    if (uart == NULL || buf == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    blockcfg.srcAddr    =   (uint32_t)&uart_instance[temp_dev->uart_id]->RDR;
    blockcfg.dstAddr    =   (uint32_t)buf;
    blockcfg.blockSize  =   size;

    status = HAL_DMA_ConfigBlockTranfer(temp_dev->dma_rx_device, temp_dev->dma_rx_id, &blockcfg);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    temp_dev->rx_size = size;
    /*clear FIFO and set FIFO*/
    LL_UART_SetRXFIFOThreshold(uart_instance[temp_dev->uart_id], LL_UART_RX_FIFO_THRES_1_2);
    LL_UART_RequestRxDataFlush(uart_instance[temp_dev->uart_id]);
    LL_UART_EnableFIFO(uart_instance[temp_dev->uart_id]);

    /* launch DMA transfer */
    if (HAL_STATUS_OK != HAL_DMA_StartTransfer(temp_dev->dma_rx_device, temp_dev->dma_rx_id)) {
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;

}

/**
 * @brief stop dma receive function
 * @param uart - device handle.
 * @retval status
 */
int32_t HAL_UART_StopReceiveDMA(const HAL_Dev_t *uart)
{
    uart_device_t *temp_dev = (uart_device_t *)uart;
    uint32_t temp_size;

    if (uart == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DMA_AbortTransfer(temp_dev->dma_rx_device, temp_dev->dma_rx_id);
    temp_size = temp_dev->rx_size;

    return temp_size;
}

#endif      // CONFIG_UART_TRF_DMA_ENABLE

/**
 * @brief Uart transmit amount of data function
 * @param uart - device handle.
 * @param buff - data address
 * @param size - length of buff
 * @param msec - ms as a unit
 * @retval Length of transmitted data(>= 0) or failed status(< 0)
 */
int32_t HAL_UART_Transmit(const HAL_Dev_t *uart, const uint8_t *buff, int32_t size, uint32_t msec)
{
    uint16_t i = size;
    int32_t count = 0;
    uart_device_t *temp_dev = (uart_device_t *)uart;
    uint32_t tick_start = 0;

    if (buff == NULL || uart == NULL || size == 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (temp_dev->tx_mode == UART_TRANS_MODE_NOMA) {
        tick_start = HAL_BOARD_GetTime(HAL_TIME_MS);
        while (i) {
            if (HAL_UART_IsTxReady(uart)) {
                LL_UART_TransmitData(uart_instance[temp_dev->uart_id], buff[count]);
                count++;
                i--;
            } else {
                if(HAL_BOARD_GetTime(HAL_TIME_MS) - tick_start > msec || msec == 0) {
                    break;
                }
            }
        }
    } else if (temp_dev->tx_mode == UART_TRANS_MODE_INTERRUPT) {
#if CONFIG_UART_TRF_IT_ENABLE
        HAL_Callback_t temp_cb = {.cb = Uart_isr_tx_callback, .arg = ((void*)&temp_dev->tx_sem)};
        HAL_Status_t temp_status;

        HAL_UART_RegisterIRQ((HAL_Dev_t *)temp_dev, UART_DIR_TX, &temp_cb);

        if (HAL_SemaphoreIsValid(&temp_dev->tx_sem) == 0) {
            temp_status = HAL_SemaphoreInitBinary(&temp_dev->tx_sem);
            if (temp_status != HAL_STATUS_OK) {
                return HAL_STATUS_ERROR;  // failed to create smeaphore
            }
        }

        temp_dev->txbuf = (uint8_t *)buff;
        temp_dev->tx_size = size;

        HAL_UART_EnableTxReadyIRQ(uart);
        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_dev->tx_sem, msec)) {
            /* overtime */
        }
        HAL_UART_DisableTxReadyIRQ(uart);

        temp_dev->txbuf = NULL;
        count = size - temp_dev->tx_size;
        temp_dev->tx_size = 0;
#endif  // CONFIG_UART_TRF_IT_ENABLE
    } else if (temp_dev->tx_mode == UART_TRANS_MODE_DMA) {
#if CONFIG_UART_TRF_DMA_ENABLE
        HAL_Status_t temp_status;
        /*Check semaphore*/
        if (HAL_SemaphoreIsValid(&temp_dev->tx_sem) == 0) {
            temp_status = HAL_SemaphoreInitBinary(&temp_dev->tx_sem);
            if (temp_status != HAL_STATUS_OK) {
                return HAL_STATUS_ERROR;  // failed to create smeaphore
            }
        }

        temp_status = HAL_UART_StartTransmitDMA(uart, buff, size);

        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_dev->tx_sem, msec)) {
            /*time out*/
            count = HAL_UART_StopTransmitDMA(uart);
            return HAL_STATUS_TIMEOUT;
        } else {
            count = size;
        }
#endif  //CONFIG_UART_TRF_DMA_ENABLE
    }


    return count;
}

/**
 * @brief Uart receive amount of data function
 * @param uart - device handle.
 * @param buff - data address
 * @param size - desired length
 * @param msec - ms as a unit
 * @retval Length of transmitted data(>= 0) or failed status(< 0)
 */
int32_t HAL_UART_Receive(const HAL_Dev_t *uart, uint8_t *buff, int32_t size, uint32_t msec)
{
    uint16_t i = size;
    uint16_t count = 0;
    uart_device_t *temp_dev = (uart_device_t *)uart;
    TickType_t tick_start = 0;

    if (buff == NULL || uart == NULL || size == 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ( (DEVICE_ID_GET(uart->id) > UART_NUM) || (DEVICE_TYPE_GET(uart->id) != HAL_DEV_TYPE_UART) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (temp_dev->rx_mode == UART_TRANS_MODE_NOMA) {
        tick_start = HAL_BOARD_GetTime(HAL_TIME_MS);
        while (i) {
            if (LL_UART_IsActiveFlag_RXFNE(uart_instance[temp_dev->uart_id])) {
                buff[count] = LL_UART_ReceiveData(uart_instance[temp_dev->uart_id]);
                count++;
                i--;
            } else {
                if (HAL_BOARD_GetTime(HAL_TIME_MS) - tick_start > msec || msec == 0) {
                    break;
                }
            }
        }
    } else if (temp_dev->rx_mode == UART_TRANS_MODE_INTERRUPT) {
#if CONFIG_UART_TRF_IT_ENABLE
        HAL_Callback_t temp_cb = {.cb = Uart_isr_rx_callback, .arg = ((void*)&temp_dev->tx_sem)};
        HAL_Status_t temp_status;

        HAL_UART_RegisterIRQ((HAL_Dev_t *)temp_dev, UART_DIR_RX, &temp_cb);

        if (HAL_SemaphoreIsValid(&temp_dev->rx_sem) == 0) {
            temp_status = HAL_SemaphoreInitBinary(&temp_dev->rx_sem);
            if (temp_status != HAL_STATUS_OK) {
                return HAL_STATUS_ERROR;  // failed to create smeaphore
            }
        }
        temp_dev->rxbuf = buff;
        temp_dev->rx_size = size;

        HAL_UART_EnableRxReadyIRQ(uart);
        HAL_SemaphoreWait(&temp_dev->rx_sem, msec);
        HAL_UART_DisableRxReadyIRQ(uart);

        count = size - temp_dev->rx_size;
        temp_dev->rxbuf = NULL;
        temp_dev->rx_size = 0;
#endif  // CONFIG_UART_TRF_IT_ENABLE
    } else if (temp_dev->rx_mode == UART_TRANS_MODE_DMA) {
#if CONFIG_UART_TRF_DMA_ENABLE
        HAL_Status_t temp_status;
        /*Check semaphore*/
        if (HAL_SemaphoreIsValid(&temp_dev->rx_sem) == 0) {
            temp_status = HAL_SemaphoreInitBinary(&temp_dev->rx_sem);
            if (temp_status != HAL_STATUS_OK) {
                return HAL_STATUS_ERROR;  // failed to create smeaphore
            }
        }

        HAL_UART_StartReceiveDMA(uart, buff, size);
        /* wait for transmit done semaphore */
        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_dev->rx_sem, msec)) {
            /*time out*/
            count = HAL_UART_StopReceiveDMA(uart);
            return HAL_STATUS_TIMEOUT;
        } else {
            count = size;
        }
#endif  // CONFIG_UART_TRF_DMA_ENABLE
    }

    return count;
}



/**
 * @brief Start or stop to transmit break characters
 * @param hdev - device handle.
 * @param isset - control command
 *     @arg !0 Start to transmit break characters
 *     @arg  0 Stop to transmit break characters
 * @retval HAL_Status, HAL_OK on success
 */
void HAL_UART_Break_Send_Ctrl(HAL_Dev_t *hdev, const uint8_t isset)
{
    uart_device_t *temp_dev = (uart_device_t *)hdev;

    if (isset) {
        LL_UART_EnableBreakSending(uart_instance[temp_dev->uart_id]);
    } else {
        LL_UART_DisableBreakSending(uart_instance[temp_dev->uart_id]);
    }
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
