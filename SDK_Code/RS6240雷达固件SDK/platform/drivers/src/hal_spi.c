/**
 ******************************************************************************
 * @file        hal_spi.c
 * @brief       spi hardware abstract layer define source.
 * @verbatim    null
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
#include "hal_spi.h"
#include "ll_gpio.h"
#include "ll_rtctick.h"
#include "hal_clock.h"
#include "hal_os.h"
#include "hal_board.h"
#include "ll_utils.h"
#include "ll_rcc_dev.h"
#include "hal_dma.h"
#include <errno.h>

#define LOG_MODULE                      "SPI"
#include "log.h"



/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define SPI_ID_GET(ID)                                          (ID >> 8)
#define SPI_TYPE_GET(ID)                                        (ID & 0XFF)

#define QSPI0                                                   (QSPI_DEV)

#define SPI_TRANSMIT_WAIT_TIME                                  (30)

/* Private defines.
 * ----------------------------------------------------------------------------
 */
/*
*@brief SPI CLOCK source
*/
#define SPI_SRC_CLOCK_AHB_CLK       (0)
#define SPI_SRC_CLOCK_MSI_CLK       (1)
#define SPI_SRC_CLOCK_DCXO_CLK      (2)
#define SPI_SRC_CLOCK_PLL_DEV1_CLK  (3)

#define SPI_CLOCK_DEV_2             (LL_QSPI_CLK_DIV2 << 1)
#define SPI_CLOCK_DEV_4             (LL_QSPI_CLK_DIV4 << 1)
#define SPI_CLOCK_DEV_8             (LL_QSPI_CLK_DIV8 << 1)
#define SPI_CLOCK_DEV_16            (LL_QSPI_CLK_DIV16 << 1)
#define SPI_CLOCK_DEV_32            (LL_QSPI_CLK_DIV32 << 1)
#define SPI_CLOCK_DEV_64            (LL_QSPI_CLK_DIV64 << 1)
#define SPI_CLOCK_DEV_128           (LL_QSPI_CLK_DIV128 << 1)
#define SPI_CLOCK_DEV_256           (LL_QSPI_CLK_DIV256 << 1)

#define SPI_FIFO_DEPTH              (28)

typedef enum {
  HAL_SPI_STATE_RESET      = 0x00U,    /*!< Peripheral not Initialized                         */
  HAL_SPI_STATE_READY      = 0x01U,    /*!< Peripheral Initialized and ready for use           */
  HAL_SPI_STATE_BUSY       = 0x02U,    /*!< an internal process is ongoing                     */
  HAL_SPI_STATE_BUSY_TX    = 0x03U,    /*!< Data Transmission process is ongoing               */
  HAL_SPI_STATE_BUSY_RX    = 0x04U,    /*!< Data Reception process is ongoing                  */
  HAL_SPI_STATE_BUSY_TX_RX = 0x05U,    /*!< Data Transmission and Reception process is ongoing */
  HAL_SPI_STATE_ERROR      = 0x06U,    /*!< SPI error state                                    */
  HAL_SPI_STATE_ABORT      = 0x07U     /*!< SPI abort is ongoing                               */
} SPI_StateTypeDef_e;

typedef struct {
    uint32_t CTRL_REG;
    uint32_t INT_CS_REG;
    uint32_t TXF_CTL_REG;
    uint32_t RXF_CTL_REG;
    uint32_t DMA_EN_REG;
    uint32_t DMATR_REG;
    uint32_t DMARR_REG;
    uint32_t QSPI_ENHANCE;
} SPI_Register_back_t;

typedef struct {
    uint8_t tx_start_level;                 /* tx start transmit level. value:1-32*/
    uint8_t tx_empty_level;                 /* tx fifo empty trigger level.value:1-32*/
    uint8_t rx_full_level;                  /* rx fifo full trigger level.value:1-32*/

    uint8_t tx_dma_level;                   /* tx dma request level.value:1-32*/
    uint8_t rx_dma_level;                   /* rx dma request level.value:1-32*/

    SPI_FrameCtrl_t frame_param;

#if CONFIG_SPI_WUP_ENABLED
    uint16_t        threshold;
    uint8_t         ctrl;
    HAL_Callback_t  wk_cb;
#endif
#if CONFIG_SPI_PM
    SPI_WakeupMode_t            wk_mode;
    SPI_Register_back_t         register_back;
#endif
} SPI_Advanced_param_t;

typedef struct {
    HAL_Dev_t                       device;

    SPI_ID_t                        spi_id;
    SPI_InitParam_t                 init_param;
    SPI_Advanced_param_t            adv_param;
    SPI_StateTypeDef_e              state;
    uint8_t                         slave_index;

    uint8_t*                        txbuf;
    uint16_t                        tx_offset;
    uint16_t                        tx_size;

    uint8_t*                        rxbuf;
    uint16_t                        rx_offset;
    uint16_t                        rx_size;

    SPI_TransMode_t                 tx_mode;
    SPI_TransMode_t                 rx_mode;
    SPI_TransMode_t                 full_mode;

#if CONFIG_SPI_TRANS_IT_ENABLE
    HAL_Callback_t                  isr_tx_cb;                 /* spi ISR TX callback function */
    HAL_Callback_t                  isr_rx_cb;                 /* spi ISR RX callback function */
    HAL_Callback_t                  isr_cs_cb;
#endif

#if CONFIG_SPI_TRANS_DMA_ENABLE
    HAL_Callback_t                  dma_tx_cb;                 /* spi DMA TX callback function */
    HAL_Callback_t                  dma_rx_cb;                 /* spi DMA RX callback function */

    uint32_t                        dma_tx_id;
    uint32_t                        dma_rx_id;

    HAL_Dev_t *                     dma_tx_device;
    HAL_Dev_t *                     dma_rx_device;
#endif

#if (CONFIG_SPI_TRANS_IT_ENABLE || CONFIG_SPI_TRANS_DMA_ENABLE)
    HAL_Semaphore                   tx_sem;
    HAL_Semaphore                   rx_sem;
#endif

    uint8_t                         cs_irq_state;
} spi_device_t;

static HAL_Dev_t *spi_device[SPI_NUM_MAX];

qspi_dev_t *spi_instance[SPI_NUM_MAX] = {QSPI0};




void Spi_irq_handler(void *param);
#if CONFIG_SPI_PM
static void Spi_register_save(HAL_Dev_t *dev);
#endif


#if CONFIG_SPI_WUP_ENABLED
static HAL_Status_t Spi_wakeup_enable(HAL_Dev_t *spi)
{
    spi_device_t *temp_dev = (spi_device_t *)spi;

    /* wakeup enable */
    if (temp_dev->adv_param.threshold > 15) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (LL_SYS_GetCurrentSysId()) {
        LL_WKUP_EnableSrc(LL_WKUP_SRC_SPI, 1);
    } else {
        LL_WKUP_EnableSrc(LL_WKUP_SRC_SPI, 0);
    }

    LL_WKUP_SPI_ClrFlag();
    LL_WKUP_SPI_Reset();

    if (temp_dev->adv_param.threshold == 0) {
        LL_WKUP_SPI_SetTrget(8);
    } else {
        LL_WKUP_SPI_SetTrget(temp_dev->adv_param.threshold);
    }
    LL_WKUP_SPI_Enable();

    return HAL_STATUS_OK;
}

static void Spi_wakeup_disable(HAL_Dev_t *spi)
{

    LL_WKUP_SPI_ClrFlag();
    LL_WKUP_SPI_Disable();
}

#endif

#if CONFIG_SPI_PM

HAL_Status_t Spi_Suspend(HAL_Dev_t *spi)
{
    spi_device_t *temp_dev = (spi_device_t *)spi;
    HAL_Status_t status = HAL_STATUS_OK;

    if (temp_dev->adv_param.wk_mode == SPI_WK_MODE_AUTO) {
        status  = HAL_SPI_PM_Suspend(spi);
    }

#if CONFIG_SPI_WUP_ENABLED
    LL_WKUP_SPI_Reset();
#endif

    return status;
}

HAL_Status_t Spi_Resume(HAL_Dev_t *spi)
{
    spi_device_t *temp_dev = (spi_device_t *)spi;
    HAL_Status_t status = HAL_STATUS_OK;

#if CONFIG_SPI_WUP_ENABLED
    if (temp_dev->adv_param.wk_cb.cb != NULL && LL_WKUP_SPI_GetFlag()) {
        temp_dev->adv_param.wk_cb.cb(spi, temp_dev->adv_param.wk_cb.arg);
        LL_WKUP_SPI_ClrFlag();
    }
#endif

    if (temp_dev->adv_param.wk_mode == SPI_WK_MODE_AUTO) {
        status = HAL_SPI_PM_Resume(spi);
    }

    return status;
}

static int Spi_pm_action(void *dev, enum pm_device_action action)
{
    int ret = -1;

    switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
        ret = Spi_Suspend((HAL_Dev_t *)dev);
        break;
    case PM_DEVICE_ACTION_RESUME:
        ret = Spi_Resume((HAL_Dev_t *)dev);
        break;
    default:
        break;
    }
    return (ret == HAL_STATUS_OK) ? 0 : -1;
}

static void Spi_pm_register(HAL_Dev_t *dev)
{
    struct pm_device *pm_dev = &dev->pmDev;

    pm_dev->action = Spi_pm_action;
    pm_dev->early_action = NULL;
    pm_dev->flags = 0;
    pm_device_register(pm_dev, Spi_pm_action);
}

static void Spi_pm_unregister(HAL_Dev_t *dev)
{
    struct pm_device *pm_dev = &dev->pmDev;

    pm_device_unregister(pm_dev);
}

static void Spi_register_save(HAL_Dev_t *dev)
{
    spi_device_t *temp_dev = (spi_device_t *)dev;

    temp_dev->adv_param.register_back.CTRL_REG = spi_instance[temp_dev->spi_id]->CTRL_REG;
    temp_dev->adv_param.register_back.INT_CS_REG = spi_instance[temp_dev->spi_id]->INT_CS_REG;
    temp_dev->adv_param.register_back.TXF_CTL_REG = spi_instance[temp_dev->spi_id]->TXF_CTL_REG;
    temp_dev->adv_param.register_back.RXF_CTL_REG = spi_instance[temp_dev->spi_id]->RXF_CTL_REG;
    temp_dev->adv_param.register_back.DMATR_REG = spi_instance[temp_dev->spi_id]->DMATR_REG;
    temp_dev->adv_param.register_back.DMARR_REG = spi_instance[temp_dev->spi_id]->DMARR_REG;
    temp_dev->adv_param.register_back.DMA_EN_REG = spi_instance[temp_dev->spi_id]->DMA_EN_REG;
    temp_dev->adv_param.register_back.QSPI_ENHANCE = spi_instance[temp_dev->spi_id]->QSPI_ENHANCE;
}

static void Spi_register_resume(HAL_Dev_t *dev)
{
    spi_device_t *temp_dev = (spi_device_t *)dev;

    spi_instance[temp_dev->spi_id]->CTRL_REG = temp_dev->adv_param.register_back.CTRL_REG;
    spi_instance[temp_dev->spi_id]->INT_CS_REG = temp_dev->adv_param.register_back.INT_CS_REG;
    spi_instance[temp_dev->spi_id]->TXF_CTL_REG = temp_dev->adv_param.register_back.TXF_CTL_REG;
    spi_instance[temp_dev->spi_id]->RXF_CTL_REG = temp_dev->adv_param.register_back.RXF_CTL_REG;
    spi_instance[temp_dev->spi_id]->DMATR_REG = temp_dev->adv_param.register_back.DMATR_REG;
    spi_instance[temp_dev->spi_id]->DMARR_REG = temp_dev->adv_param.register_back.DMARR_REG;
    spi_instance[temp_dev->spi_id]->DMA_EN_REG = temp_dev->adv_param.register_back.DMA_EN_REG;
    spi_instance[temp_dev->spi_id]->QSPI_ENHANCE = temp_dev->adv_param.register_back.QSPI_ENHANCE;
}

HAL_Status_t HAL_SPI_PM_Suspend(HAL_Dev_t *spi)
{
    HAL_Status_t status;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (spi->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (spi->state == HAL_DEV_STATE_CLOSE) {
        return HAL_STATUS_ALREADY;
    }

    Spi_register_save(spi);

    status = HAL_SPI_Close(spi);

    return status;
}

HAL_Status_t HAL_SPI_PM_Resume(HAL_Dev_t *spi)
{
    spi_device_t *temp_dev = (spi_device_t *)spi;
    HAL_Status_t status;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (spi->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (spi->state == HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_ALREADY;
    }

    status = HAL_SPI_Open(spi, temp_dev->slave_index);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    Spi_register_resume(spi);

    return HAL_STATUS_OK;
}


#endif

static HAL_Status_t Spi_clock_init(HAL_Dev_t *spi, uint32_t sclk)
{
    spi_device_t *temp_dev = (spi_device_t *)spi;
    int freq = 0;
    uint16_t temp_spi_div = 0;

    freq = HAL_BOARD_GetFreq(CLOCK_AHB) + 100000;  //四舍五入
    LL_RCC_SPI_SetClockSource(SPI_SRC_CLOCK_AHB_CLK);

    if (freq >= 200000000) {
        LL_RCC_SPI_SetPrescaler(1);    /* div 2 */
        freq /= 2;
    } else {
        LL_RCC_SPI_SetPrescaler(0);    /* div 1 */
    }
    LL_RCC_SPI_DisableBusClock();
    LL_RCC_SPI_DisableClock();
    LL_RCC_SPI_Reset();
    LL_RCC_SPI_EnableBusClock();
    LL_RCC_SPI_EnableClock();

    temp_spi_div = (freq / sclk) & 0xFFFFFFFE;
    LL_SPI_SetPrescaler(spi_instance[temp_dev->spi_id], temp_spi_div);

    return HAL_STATUS_OK;
}

// static int Spi_clock_get(HAL_Dev_t *dev)
// {
//     spi_device_t *temp_handle = (spi_device_t *)dev;
//     uint8_t temp_flag;
//     uint8_t temp_div;
//     int freq = 0;
//     uint8_t temp_spi_div;

//     temp_spi_div = LL_SPI_GetPrescaler(spi_instance[temp_handle->spi_id]);
//     temp_div = LL_RCC_SPI_GetPrescaler();
//     temp_flag = LL_RCC_SPI_GetClockSource();
//     switch (temp_flag) {
//     case SPI_SRC_CLOCK_AHB_CLK:
//         freq = HAL_BOARD_GetFreq(CLOCK_PLL_SOC);
//         break;
//     case SPI_SRC_CLOCK_MSI_CLK:
//         freq = HAL_BOARD_GetFreq(CLOCK_MSI);
//         break;
//     case SPI_SRC_CLOCK_DCXO_CLK:
//         freq = HAL_BOARD_GetFreq(CLOCK_DCXO);
//         break;
//     case SPI_SRC_CLOCK_PLL_DEV1_CLK:
//         freq = HAL_BOARD_GetFreq(CLOCK_PLL_DEV1);
//         break;
//     default:
//         freq = -1;
//         break;
//     }

//     if (freq > 0) {
//         freq = freq / (1 << temp_div) / temp_spi_div;
//     }

//     return freq;
// }

static HAL_Status_t Spi_clock_deinit(void)
{
    LL_RCC_SPI_DisableBusClock();
    LL_RCC_SPI_DisableClock();
    LL_RCC_SPI_Reset();
    LL_RCC_SPI_DisableClock();
    LL_RCC_SPI_DisableBusClock();

    return HW_SUCCESS;
}

static HAL_Status_t Spi_semaphore_create(HAL_Semaphore *sem)
{
    HAL_Status_t temp_status = HAL_STATUS_OK;

    if (HAL_SemaphoreIsValid(sem) == 0) {
        temp_status = HAL_SemaphoreInitBinary(sem);
    }

    return temp_status;
}


/**
  * @brief  This function init the specified spi.
  * @param0 spi handle struct.
  * @note
  *
  * @retval Hardware status.
  * @retval 0 on success, other status is err.
  */
int Spi_confg(HAL_Dev_t *dev, uint8_t index)
{
    spi_device_t *temp_handle = (spi_device_t *)dev;
    SPI_InitParam_t *tmp_param = &temp_handle[index].init_param;
    int temp_freq;
    HAL_Pinmux_t temp_pinmux;
    HAL_Status_t temp_result;
    uint32_t tmp_div;

    /* spi gpio init */
    temp_result = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_SPI, temp_handle->spi_id, HAL_PINMUX_TYPE_INIT, &temp_pinmux);
    if (temp_result != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    HAL_BOARD_SetPinMux(&temp_pinmux);
    /* cs gpio init */
    if (tmp_param->csMode == SPI_CS_SOFT) {
        temp_result = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_SPI, temp_handle->spi_id, HAL_PINMUX_TYPE_SPI_CS, &temp_pinmux);
        if (temp_result != HAL_STATUS_OK) {
            return HAL_STATUS_ERROR;
        }
        HAL_BOARD_SetPinMux(&temp_pinmux);
    }
    /* spi clock init */
    Spi_clock_init(dev, tmp_param->sclk);
    /* spi parameter configuration */
    LL_SPI_Disable(spi_instance[temp_handle->spi_id]);

    /* mode_1 or mode_2 set */
    tmp_div = LL_SPI_GetPrescaler(spi_instance[temp_handle->spi_id]);
    if ((tmp_param->sclk <= 28000000) && (tmp_div >= SPI_CLOCK_DEV_4)) {
        /* mode1 */
        LL_SPI_SlvClkSelect(spi_instance[temp_handle->spi_id], LL_QSPI_SLVCLK_SSICLK);
    } else {
        /* mode2 */
        LL_SPI_SetMode(spi_instance[temp_handle->spi_id], LL_QSPI_MST_MODE);
        LL_SPI_EnableSlvClkIn(spi_instance[temp_handle->spi_id]);
        LL_SPI_SetMode(spi_instance[temp_handle->spi_id], LL_QSPI_SLV_MODE);
        LL_SPI_SlvClkSelect(spi_instance[temp_handle->spi_id], LL_QSPI_SLVCLK_FUNCLK);
        LL_SPI_DisableSlvClkIn(spi_instance[temp_handle->spi_id]);
    }

    LL_SPI_SetMode(spi_instance[temp_handle->spi_id], tmp_param->mode);

    if (tmp_param->sclkMode & 0x02) {
        LL_SPI_SetClockPolarity(spi_instance[temp_handle->spi_id], LL_QSPI_CPOL_1);
    } else {
        LL_SPI_SetClockPolarity(spi_instance[temp_handle->spi_id], LL_QSPI_CPOL_0);
    }
    if (tmp_param->sclkMode & 0x01) {
        LL_SPI_SetClockPhase(spi_instance[temp_handle->spi_id], LL_QSPI_CPHA_1);
    } else {
        LL_SPI_SetClockPhase(spi_instance[temp_handle->spi_id], LL_QSPI_CPHA_0);
    }

    LL_SPI_SetWorkLines(spi_instance[temp_handle->spi_id], tmp_param->ioMode);

    LL_SPI_SetWorkFormat(spi_instance[temp_handle->spi_id], LL_QSPI_FMODE_SPI);
    LL_SPI_SetDataWidth(spi_instance[temp_handle->spi_id], LL_QSPI_DFS_BIT(tmp_param->dataWidth));

    if (tmp_param->ioMode != SPI_IO_MODE_NORMAL) {
        if (tmp_param->mode == SPI_MODE_MASTER) {
            LL_SPI_SetAddrLenght(spi_instance[temp_handle->spi_id], temp_handle->init_param.dataWidth/4);
        } else {
            LL_SPI_SetAddrLenght(spi_instance[temp_handle->spi_id], 0);
        }
        LL_SPI_SetInstructLenght(spi_instance[temp_handle->spi_id], 0);
        LL_SPI_SetTransType(spi_instance[temp_handle->spi_id], LL_QSPI_TRAN_INS_AND_ADDR_TYPE);
        LL_SPI_StretchEnable(spi_instance[temp_handle->spi_id]);
    } else {
        LL_SPI_StretchDisable(spi_instance[temp_handle->spi_id]);
    }

    LL_SPI_SetTxFifoStartLevel(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(1));
    LL_SPI_SetTxFifoThreshold(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(16));
    LL_SPI_SetRxFifoThreshold(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(1));

    LL_SPI_SetDMALevel_TX(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(1));
    LL_SPI_SetDMALevel_RX(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(1));

    if (tmp_param->mode == SPI_MODE_SLAVE) {
        LL_SPI_EnableSlaveSelect(spi_instance[temp_handle->spi_id]);
        LL_SPI_EnableSlvOupt(spi_instance[temp_handle->spi_id]);
    } else {
        LL_SPI_DisableSlaveSelect(spi_instance[temp_handle->spi_id]);
    }

    LL_SPI_DisableSStoggle(spi_instance[temp_handle->spi_id]);
    LL_SPI_SetReciveNum(spi_instance[temp_handle->spi_id], 0);

    temp_freq = HAL_BOARD_GetFreq(CLOCK_PLL_SOC);
    if (temp_freq >= 92000000) {
        LL_SPI_SetRxSampleDelay(spi_instance[temp_handle->spi_id], LL_QSPI_SAMPLE_DELAY_3);
    } else if (temp_freq >= 46000000) {
        LL_SPI_SetRxSampleDelay(spi_instance[temp_handle->spi_id], LL_QSPI_SAMPLE_DELAY_2);
    } else if (temp_freq >= 23000000) {
        LL_SPI_SetRxSampleDelay(spi_instance[temp_handle->spi_id], LL_QSPI_SAMPLE_DELAY_1);
    } else {
        LL_SPI_SetRxSampleDelay(spi_instance[temp_handle->spi_id], 0);
    }

    LL_SPI_EnableRxHalfSampleDelayClk(spi_instance[temp_handle->spi_id]);

    LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_ALL);
    LL_SPI_ITFlagClearALL(spi_instance[temp_handle->spi_id]);

    return HAL_STATUS_OK;
}

__spi_sram_text int Spi_write_data(HAL_Dev_t *dev, uint8_t *pbuff, uint16_t size)
{
    uint32_t fifo_size = 0;
    spi_device_t *temp_handle = (spi_device_t *)dev;

    uint8_t curr_free_num = SPI_FIFO_DEPTH - LL_SPI_GetTxFifoLevel(spi_instance[temp_handle->spi_id]);
    const uint8_t *data_buff = (uint8_t *)pbuff;

    while (fifo_size < size && curr_free_num--) {
        if (temp_handle->init_param.dataWidth == SPI_DATAWIDTH_8BIT) {
            LL_SPI_TransmitData8(spi_instance[temp_handle->spi_id], data_buff);
            fifo_size++;
            data_buff++;
        } else if (temp_handle->init_param.dataWidth == SPI_DATAWIDTH_16BIT) {
            LL_SPI_TransmitData16(spi_instance[temp_handle->spi_id], data_buff);
            fifo_size++;
            data_buff += 2;
        } else {
            LL_SPI_TransmitData32(spi_instance[temp_handle->spi_id], data_buff);
            fifo_size++;
            data_buff += 4;
        }
    }
    return fifo_size;
}

__spi_sram_text int Spi_read_data(HAL_Dev_t *dev, uint8_t *pbuff, uint16_t size)
{
    uint16_t idx = 0;
    spi_device_t *temp_handle = (spi_device_t *)dev;

    uint16_t fifo_size = 0;
    fifo_size = LL_SPI_GetRxFifoLevel(spi_instance[temp_handle->spi_id]);

    if (temp_handle->init_param.dataWidth == 8) {
        if (fifo_size > size) {
            fifo_size = size;
        }
        while (idx < fifo_size) {
            LL_SPI_ReceiveData8(spi_instance[temp_handle->spi_id], &pbuff[idx]);
            idx += 1;
        }
    } else if (temp_handle->init_param.dataWidth == 16) {
        if (2 * fifo_size > size) {
            fifo_size = size;
        } else {
            fifo_size = 2 * fifo_size;
        }
        while (idx < fifo_size) {
            LL_SPI_ReceiveData16(spi_instance[temp_handle->spi_id], &pbuff[idx]);
            idx += 2;
        }
    } else {
        if (4 * fifo_size > size) {
            fifo_size = size;
        } else {
            fifo_size = 4 * fifo_size;
        }
        while (idx < fifo_size) {
            LL_SPI_ReceiveData32(spi_instance[temp_handle->spi_id], &pbuff[idx]);
            idx += 4;
        }
    }

    return idx;
}

#if CONFIG_SPI_TRANS_IT_ENABLE
__spi_sram_text void Spi_isr_tx_callback(HAL_Dev_t *spi, void *param)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    //uint32_t txfifo = 0;
    temp_handle->tx_offset += Spi_write_data(spi, &temp_handle->txbuf[temp_handle->tx_offset],
                                    temp_handle->tx_size - temp_handle->tx_offset);

    if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
        if (temp_handle->tx_offset >= temp_handle->tx_size) {
            OSI_SemaphoreRelease(&temp_handle->tx_sem);  // release tx smeaphore
            LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_TXE);  // disable TXE interrupt
        }
    } else {
        //txfifo = LL_SPI_GetTxFifoLevel(QSPI0);
        if ((temp_handle->tx_offset >= temp_handle->tx_size)) {
            OSI_SemaphoreRelease(&temp_handle->tx_sem);  // release tx smeaphore
            LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_TXE);  // disable TXE interrupt
        }
    }
}

__spi_sram_text void Spi_isr_rx_callback(HAL_Dev_t *spi, void *param)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    temp_handle->rx_offset += Spi_read_data(spi, &temp_handle->rxbuf[temp_handle->rx_offset],
                                temp_handle->rx_size - temp_handle->rx_offset);

    if (temp_handle->rx_offset >= temp_handle->rx_size) {
        OSI_SemaphoreRelease(&temp_handle->rx_sem);  // release tx smeaphore
        LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_RXF);  // disable RXF interrupt
    }
}


#endif  // CONFIG_SPI_TRANS_IT_ENABLE


#if CONFIG_SPI_TRANS_DMA_ENABLE

static void Spi_dma_callback(HAL_Dev_t *dev, void *param)
{
    csi_dcache_clean();
    csi_dcache_invalid();

    if (param != NULL) {
        HAL_SemaphoreRelease((HAL_Semaphore *)param);
    }
}

static HAL_Status_t Spi_dma_config(HAL_Dev_t *dev, SPI_TransDir_t direction)
{
    HAL_Dev_t *temp_dma;
    spi_device_t *temp_handle = (spi_device_t *)dev;
    HAL_Callback_t *tmp_cb;
    HAL_Status_t status;
    int32_t tmp_id;
    DMA_ChannelConf_t chan_cfg;
    uint8_t handshak[SPI_NUM_MAX*2] = {DMA_HANDSHAKE_RXFIFO_SPI0, DMA_HANDSHAKE_TXFIFO_SPI0};

    memset(&chan_cfg, 0, sizeof(DMA_ChannelConf_t));

    /* PSRAM width must be 4 byte */
    chan_cfg.srcBrustNum = DMA_BURSTNUM_1;
    chan_cfg.dstBrustnum = DMA_BURSTNUM_1;
    chan_cfg.srcWidth = DMA_BITWIDTH_8;
    chan_cfg.dstWidth = DMA_BITWIDTH_8;
    /* Channel priority */
    chan_cfg.priority = DMA_PRIORITY_7;

    if (direction) {
        /* receive data */
        chan_cfg.direction = DMA_DIRECTION_PER2MEM;  // receive data
        chan_cfg.srcHandShake = handshak[temp_handle->spi_id * 2];
        chan_cfg.srcIncreMode = DMA_ADDR_FIXED;
        chan_cfg.dstIncreMode = DMA_ADDR_INCREMENT;

        tmp_cb = &temp_handle->dma_rx_cb;
        temp_dma = temp_handle->dma_rx_device;

        /* Set configuration */
        tmp_id = HAL_DMA_Open(temp_dma, &chan_cfg);
        if (tmp_id < 0) {
            return HAL_STATUS_ERROR;
        }
        temp_handle->dma_rx_id = tmp_id;

    } else {
        /* transmit data */
        chan_cfg.direction = DMA_DIRECTION_MEM2PER;  // transmit data
        chan_cfg.dstHandShake = handshak[temp_handle->spi_id * 2 + 1];
        chan_cfg.srcIncreMode = DMA_ADDR_INCREMENT;
        chan_cfg.dstIncreMode = DMA_ADDR_FIXED;

        tmp_cb = &temp_handle->dma_tx_cb;
        temp_dma = temp_handle->dma_tx_device;

        /* Set configuration */
        tmp_id = HAL_DMA_Open(temp_dma, &chan_cfg);
        if (tmp_id < 0) {
            return HAL_STATUS_ERROR;
        }
        temp_handle->dma_tx_id = tmp_id;
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

/**
 * @brief spi receive function
 * @param spi - device handle.
 * @retval received data
 */
__spi_sram_text int32_t HAL_SPI_GetRxData(const HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    uint32_t tmp;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    tmp = LL_SPI_ReceiveData(spi_instance[temp_handle->spi_id]);

    return tmp;
}

/**
 * @brief spi transmit 1 byte function
 * @param spi - device handle.
 * @param data - to be transmit
 * @retval
 */
__spi_sram_text void HAL_SPI_PutTxData(const HAL_Dev_t *spi, uint32_t data)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return;
    }

    LL_SPI_TransmitData(spi_instance[temp_handle->spi_id], data);
}

/**
 * @brief start spi function
 * @param spi - device handle.
 * @param dir - transmission direction
 * @retval status
 */
__spi_sram_text HAL_Status_t HAL_SPI_StartTrans(const HAL_Dev_t *spi, SPI_TransDir_t dir)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (dir == SPI_DIR_TX) {
        LL_SPI_SetTransferMode(spi_instance[temp_handle->spi_id], LL_QSPI_TMODE_TXONLY);
    } else if (dir == SPI_DIR_RX) {
        LL_SPI_SetTransferMode(spi_instance[temp_handle->spi_id], LL_QSPI_TMODE_RXONLY);
    } else {
        LL_SPI_EnableStdDuplex(spi_instance[temp_handle->spi_id]);
        LL_SPI_SetTransferMode(spi_instance[temp_handle->spi_id], LL_QSPI_TMODE_TXRX);
    }

    LL_SPI_Enable(spi_instance[temp_handle->spi_id]);

    return HAL_STATUS_OK;
}

/**
 * @brief stop spi function
 * @param spi - device handle.
 * @retval status
 */
__spi_sram_text HAL_Status_t HAL_SPI_StopTrans(const HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_SPI_Disable(spi_instance[temp_handle->spi_id]);

    return HAL_STATUS_OK;
}

/**
 * @brief Get spi TX ready flag
 * @param spi - device handle.
 * @retval flag
 */
__spi_sram_text int HAL_SPI_IsTxReady(HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    return LL_SPI_BusIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_STATUS_TXNF);
}

/**
 * @brief Get spi TX empty flag
 * @param spi - device handle.
 * @retval flag
 */
int HAL_SPI_IsTxEmpty(HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI) ) {
        return HAL_STATUS_INVALID_PARAM;
    }

    return LL_SPI_BusIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_STATUS_TXE);
}

/**
 * @brief Get spi RX ready flag
 * @param spi - device handle.
 * @retval flag
 */
int HAL_SPI_IsRxReady(HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    return LL_SPI_BusIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_STATUS_RXNE);
}

/**
 * @brief Get spi busy flag
 * @param spi - device handle.
 * @retval flag
 */
int HAL_SPI_IsBusy(HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    return LL_SPI_BusIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_STATUS_BUSY);
}


/**
 * @brief Get spi fifo level
 * @param spi - device handle.
 * @param type - fifo type.
 * @retval fifo level
 */
__spi_sram_text int HAL_SPI_GetFifoLevel(HAL_Dev_t *spi, SPI_FifoType_t type)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (type == SPI_FIFO_TX) {
        return LL_SPI_GetTxFifoLevel(spi_instance[temp_handle->spi_id]);
    } else {
        return LL_SPI_GetRxFifoLevel(spi_instance[temp_handle->spi_id]);
    }
}

/**
 * @brief Get spi CS interrupt flag
 * @param spi - device handle.
 * @retval flag
 */
int HAL_SPI_IsCsInterrupt(HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    return LL_SPI_ITIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_IT_HIGH_EDG);
}

/**
 * @brief Enable cs(soft)
 * @param spi - device handle.
 * @param csId - slave index(start from 0).
 * @retval status
 */
HAL_Status_t HAL_SPI_EnableCS(const HAL_Dev_t *spi, uint8_t csId)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    HAL_Pinmux_t temp_pinmux;
    HAL_Status_t temp_result;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (temp_handle->init_param.csMode != SPI_CS_SOFT) {
        return HAL_STATUS_NOT_SUPPORTED;
    }

    temp_result = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_SPI, temp_handle->spi_id, HAL_PINMUX_TYPE_SPI_CS, &temp_pinmux);
    if (temp_result != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    if (csId > temp_pinmux.pinCnt) {
        return HAL_STATUS_NOT_SUPPORTED;
    }
    HAL_Dev_t *pGpioDevice = HAL_DEV_Find(HAL_DEV_TYPE_GPIO, temp_pinmux.pinConfig[csId].port);
    HAL_GPIO_WritePin(pGpioDevice, temp_pinmux.pinConfig[csId].pin, GPIO_PIN_RESET);


    return HAL_STATUS_OK;
}

/**
 * @brief Disable cs(soft)
 * @param spi - device handle.
 * @param csId - slave index(start from 0).
 * @retval status
 */
HAL_Status_t HAL_SPI_DisableCS(const HAL_Dev_t *spi, uint8_t csId)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    HAL_Pinmux_t temp_pinmux;
    HAL_Status_t temp_result;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (temp_handle->init_param.csMode != SPI_CS_SOFT) {
        return HAL_STATUS_NOT_SUPPORTED;
    }

    temp_result = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_SPI, temp_handle->spi_id, HAL_PINMUX_TYPE_SPI_CS, &temp_pinmux);
    if (temp_result != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    if (csId > temp_pinmux.pinCnt) {
        return HAL_STATUS_NOT_SUPPORTED;
    }

    HAL_Dev_t *pGpioDevice = HAL_DEV_Find(HAL_DEV_TYPE_GPIO, temp_pinmux.pinConfig[csId].port);
    HAL_GPIO_WritePin(pGpioDevice, temp_pinmux.pinConfig[csId].pin, GPIO_PIN_SET);

    return HAL_STATUS_OK;
}

/**
 * @brief Initialize the SPI peripheral.
 * @param id - SPI ID
 * @param param - spi parameter
 * @param param_num - spi parameter num(master:1, multi slave: >=1)
 * @retval device handle(NULL: failed)
 */
HAL_Dev_t * HAL_SPI_Init(SPI_ID_t id, const SPI_InitParam_t *param, const uint8_t param_num)
{
    spi_device_t *temp_device = (spi_device_t *)HAL_DEV_Find(HAL_DEV_TYPE_SPI, id);
    uint8_t i;

    if (param == NULL || param_num == 0) {
        return NULL;
    }

    if (temp_device == NULL) {
        temp_device = (spi_device_t *)HAL_DEV_MemMalloc(sizeof(spi_device_t)*param_num);
        if (temp_device == NULL) {
            return (HAL_Dev_t *)temp_device;
        }
        memset(temp_device, 0, sizeof(spi_device_t));
        temp_device->device.reg = spi_instance[id];
        if (id == SPI0_ID) {
            temp_device->device.irqNum = QSPI0_IRQn;
            temp_device->device.irqHandler = Spi_irq_handler;
        } else {
            temp_device->device.irqNum = QSPI0_IRQn;
            temp_device->device.irqHandler = Spi_irq_handler;
        }
        IRQ_Disable(temp_device->device.irqNum);
        IRQ_Priority(temp_device->device.irqNum, 7);
        IRQ_Attach(temp_device->device.irqNum, temp_device->device.irqHandler);
        IRQ_AttachDevice(temp_device->device.irqNum, (HAL_Dev_t *)temp_device);
        IRQ_Enable(temp_device->device.irqNum);

        spi_device[id] = (HAL_Dev_t *)temp_device;

        HAL_DEV_Register((HAL_Dev_t *)temp_device, HAL_DEV_TYPE_SPI, id);
    }

    /* parameter init */
    for (i = 0; i < param_num; i++) {
        memcpy(&temp_device[i].init_param, param, sizeof(SPI_InitParam_t));
        memset(&temp_device[i].adv_param, 0, sizeof(SPI_Advanced_param_t));
    }
    temp_device->txbuf = NULL;
    temp_device->tx_offset = 0;
    temp_device->tx_size = 0;

    temp_device->tx_mode = SPI_TRANS_MODE_NOMA;
    temp_device->rx_mode = SPI_TRANS_MODE_NOMA;
    temp_device->full_mode = SPI_TRANS_MODE_NOMA;

    temp_device->rxbuf = NULL;
    temp_device->rx_offset = 0;
    temp_device->rx_size = 0;

    temp_device->state = HAL_SPI_STATE_RESET;
    temp_device->spi_id = id;

#if CONFIG_SPI_TRANS_IT_ENABLE
    temp_device->isr_tx_cb.cb = NULL;
    temp_device->isr_rx_cb.cb = NULL;
#endif

#if CONFIG_SPI_TRANS_DMA_ENABLE
    temp_device->dma_tx_cb.cb = NULL;
    temp_device->dma_rx_cb.cb = NULL;
    temp_device->dma_rx_device = NULL;
    temp_device->dma_tx_device = NULL;
#endif

#if CONFIG_SPI_PM
    Spi_pm_register((HAL_Dev_t *)temp_device);
#endif

#if (CONFIG_SPI_TRANS_IT_ENABLE || CONFIG_SPI_TRANS_DMA_ENABLE)
    HAL_SemaphoreSetInvalid(&temp_device->tx_sem);
    HAL_SemaphoreSetInvalid(&temp_device->rx_sem);
#endif
    return (HAL_Dev_t *)temp_device;
}

/**
 * @brief Open the spi peripheral.
 * @param spi - spi handle pointer
 * @param slave_addrIndex - slave index
 * @retval status
 */
HAL_Status_t HAL_SPI_Open(HAL_Dev_t *spi, uint8_t slave_addrIndex)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (spi->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (Spi_confg(spi, 0)) {  // not support multipe slave parameter
        return HAL_STATUS_ERROR;
    }

    temp_handle->slave_index = slave_addrIndex;

    spi->state = HAL_DEV_STATE_OPEN;
    temp_handle->state = HAL_SPI_STATE_READY;

    return HAL_STATUS_OK;
}

/**
 * @brief Close the SPI peripheral.
 * @param spi - spi handle pointer
 * @retval status
 */
HAL_Status_t HAL_SPI_Close(HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

#if CONFIG_SPI_TRANS_DMA_ENABLE
    HAL_SPI_UnregisterDMA(spi, SPI_DIR_TX);
    HAL_SPI_UnregisterDMA(spi, SPI_DIR_RX);
#endif

#if CONFIG_SPI_TRANS_IT_ENABLE
    HAL_SPI_UnregisterIRQ(spi, SPI_DIR_TX);
    HAL_SPI_UnregisterIRQ(spi, SPI_DIR_RX);
#endif

    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (spi->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }
    /* disable spi clock */
    if (temp_handle->spi_id == SPI0_ID) {
        LL_RCC_SPI_DisableClock();
        LL_RCC_SPI_DisableBusClock();
    }

    spi->state = HAL_DEV_STATE_CLOSE;
    temp_handle->state = HAL_SPI_STATE_RESET;

    return HAL_STATUS_OK;
}

/**
 * @brief This function deinit the specified spi.
 * @param spi - handle struct.
 * @retval status.
 */
HAL_Status_t HAL_SPI_DeInit(HAL_Dev_t *spi)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state == HAL_DEV_STATE_UNINIT) {
        return HAL_STATUS_INVALID_STATE;
    }


    LL_SPI_Disable(spi_instance[temp_handle->spi_id]);

    Spi_clock_deinit();
#if CONFIG_SPI_PM
    Spi_pm_unregister(spi);
#endif
    HAL_DEV_Unregister(spi);

#if (CONFIG_SPI_TRANS_IT_ENABLE || CONFIG_SPI_TRANS_DMA_ENABLE)
    HAL_SemaphoreDeinit(&temp_handle->tx_sem);
    HAL_SemaphoreDeinit(&temp_handle->rx_sem);
#endif

    temp_handle->cs_irq_state = 0;
    temp_handle->state = HAL_SPI_STATE_RESET;

    HAL_DEV_MemFree(spi);

    return HW_SUCCESS;
}

/**
 * @brief Congfigure spi advanced parameter.
 * @param spi - handle struct.
 * @param ptype - parameter type.
 * @param param - parameter.
 * @retval status.
 */
HAL_Status_t HAL_SPI_ExtControl(const HAL_Dev_t *spi, SPI_ParamType_t ptype, void *param)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    SPI_FrameCtrl_t *temp_param = (SPI_FrameCtrl_t *)param;
    uint8_t tmp = ptype;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    switch (tmp) {
    case SPI_PARAM_TX_START_LEVEL:
        LL_SPI_SetTxFifoStartLevel(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(*(uint8_t *)param));
        temp_handle->adv_param.tx_start_level = *(uint8_t *)param;
        break;
    case SPI_PARAM_TX_EMPTY_LEVEL:
        LL_SPI_SetTxFifoThreshold(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(*(uint8_t *)param));
        temp_handle->adv_param.tx_empty_level = *(uint8_t *)param;
        break;
    case SPI_PARAM_RX_FULL_LEVEL:
        LL_SPI_SetRxFifoThreshold(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(*(uint8_t *)param));
        temp_handle->adv_param.rx_full_level = *(uint8_t *)param;
        break;
    case SPI_PARAM_TX_DMA_LEVEL:
        LL_SPI_SetDMALevel_TX(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(*(uint8_t *)param));
        temp_handle->adv_param.tx_dma_level = *(uint8_t *)param;
        break;
    case SPI_PARAM_RX_DMA_LEVEL:
        LL_SPI_SetDMALevel_RX(spi_instance[temp_handle->spi_id], LL_QSPI_FIFO_LEVEL(*(uint8_t *)param));
        temp_handle->adv_param.rx_dma_level = *(uint8_t *)param;
        break;
    case SPI_PARAM_FRAME_CTRL:
        LL_SPI_SetInstructLenght(spi_instance[temp_handle->spi_id], temp_param->instruction_len);
        LL_SPI_SetAddrLenght(spi_instance[temp_handle->spi_id], temp_param->address_len);
        LL_SPI_SetTransType(spi_instance[temp_handle->spi_id], temp_param->trans_form);
        memcpy(&temp_handle->adv_param.frame_param, temp_param, sizeof(SPI_FrameCtrl_t));
        break;
#if CONFIG_SPI_WUP_ENABLED
    case SPI_PARAM_WAKEUP_THRESHOLD:
        temp_handle->adv_param.threshold = *(uint16_t *)param;
        break;
    case SPI_PARAM_WAKEUP_CTRL:
        if (*(uint8_t *)param) {
            Spi_wakeup_enable((HAL_Dev_t *)spi);
        } else {
            Spi_wakeup_disable((HAL_Dev_t *)spi);
        }
        break;
    case SPI_PARAM_WAKEUP_CALLBACK:
        temp_handle->adv_param.wk_cb.cb = ((HAL_Callback_t *)param)->cb;
        temp_handle->adv_param.wk_cb.arg = ((HAL_Callback_t *)param)->arg;
        break;
#endif
#if CONFIG_SPI_PM
    case SPI_PARAM_WAKEUP_MODE:
        temp_handle->adv_param.wk_mode = *(SPI_WakeupMode_t *)param;
        break;
#endif
    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}

/**
 * @brief set transmission mode.
 * @param spi - handle struct.
 * @param transDir - direction.
 * @param Mode - transmission mode.
 * @retval status.
 */
HAL_Status_t HAL_SPI_SetTransferMode(const HAL_Dev_t *spi, SPI_TransDir_t transDir, SPI_TransMode_t Mode)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (transDir == SPI_DIR_TX) {
        temp_handle->tx_mode = Mode;
#if CONFIG_SPI_TRANS_DMA_ENABLE
        if (temp_handle->tx_mode == SPI_TRANS_MODE_DMA) {
            HAL_Callback_t temp_cb = {.cb = Spi_dma_callback, .arg = ((void*)&temp_handle->tx_sem)};

            HAL_SPI_RegisterDMA((HAL_Dev_t *)spi, SPI_DIR_TX, &temp_cb);
        }
#endif

#if CONFIG_SPI_TRANS_IT_ENABLE
        if (temp_handle->tx_mode == SPI_TRANS_MODE_INTERRUPT) {
            HAL_Callback_t tx_cb = {.cb = Spi_isr_tx_callback, .arg = ((void*)&temp_handle->tx_sem)};

            HAL_SPI_RegisterIRQ((HAL_Dev_t *)spi, SPI_IRQ_TX, &tx_cb);
        }
#endif

    } else if (transDir == SPI_DIR_RX) {
        temp_handle->rx_mode = Mode;
#if CONFIG_SPI_TRANS_DMA_ENABLE
        if (temp_handle->rx_mode == SPI_TRANS_MODE_DMA) {
            HAL_Callback_t temp_cb = {.cb = Spi_dma_callback, .arg = ((void*)&temp_handle->rx_sem)};

            HAL_SPI_RegisterDMA((HAL_Dev_t *)spi, SPI_DIR_RX, &temp_cb);
        }
#endif

#if CONFIG_SPI_TRANS_IT_ENABLE
        if (temp_handle->rx_mode == SPI_TRANS_MODE_INTERRUPT) {
            HAL_Callback_t rx_cb = {.cb = Spi_isr_rx_callback, .arg = ((void*)&temp_handle->rx_sem)};

            HAL_SPI_RegisterIRQ((HAL_Dev_t *)spi, SPI_IRQ_RX, &rx_cb);
        }
#endif
    } else {
        temp_handle->full_mode = Mode;
#if CONFIG_SPI_TRANS_DMA_ENABLE
        if (temp_handle->full_mode == SPI_TRANS_MODE_DMA) {
            HAL_Callback_t tx_cb = {.cb = Spi_dma_callback, .arg = ((void*)&temp_handle->tx_sem)};
            HAL_Callback_t rx_cb = {.cb = Spi_dma_callback, .arg = ((void*)&temp_handle->rx_sem)};

            HAL_SPI_RegisterDMA((HAL_Dev_t *)spi, SPI_DIR_TX, &tx_cb);
            HAL_SPI_RegisterDMA((HAL_Dev_t *)spi, SPI_DIR_RX, &rx_cb);
        }
#endif

#if CONFIG_SPI_TRANS_IT_ENABLE
        if (temp_handle->full_mode == SPI_TRANS_MODE_INTERRUPT) {
            HAL_Callback_t tx_cb = {.cb = Spi_isr_tx_callback, .arg = ((void*)&temp_handle->tx_sem)};
            HAL_Callback_t rx_cb = {.cb = Spi_isr_rx_callback, .arg = ((void*)&temp_handle->rx_sem)};

            HAL_SPI_RegisterIRQ((HAL_Dev_t *)spi, SPI_IRQ_TX, &tx_cb);
            HAL_SPI_RegisterIRQ((HAL_Dev_t *)spi, SPI_IRQ_RX, &rx_cb);
        }
#endif

    }

    return HAL_STATUS_OK;
}

/**
 * @brief Spi receive amount of data function
 * @param spi - device handle.
 * @param buff - data address
 * @param size - desired length
 * @param msec - ms as a unit
 * @retval Length of transmitted data(>= 0) or failed status(< 0)
 */
int32_t HAL_SPI_Receive(const HAL_Dev_t *spi, uint8_t *buff, int32_t size, uint32_t msec)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    SPI_InitParam_t *temp_param = &temp_handle[0].init_param;  // not support multipe slave parameter
    uint32_t tick_cnt = msec;
    uint32_t tick_start = 0;

    if (spi == NULL || buff == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    temp_handle->rx_offset = 0;
    temp_handle->rx_size = size;
    temp_handle->rxbuf = buff;

    HAL_SPI_StopTrans(spi);
    temp_handle->state = HAL_SPI_STATE_BUSY_RX;

    if (temp_handle->rx_mode == SPI_TRANS_MODE_NOMA) {
        if (temp_param->mode == SPI_MODE_MASTER) {
            if (temp_param->ioMode != SPI_IO_MODE_NORMAL) {
                SPI_FrameCtrl_t adv = {.instruction_len = 0, .address_len = 0, .trans_form = SPI_TRANS_ALL_SPECIFIED};
                HAL_SPI_ExtControl(spi, SPI_PARAM_FRAME_CTRL, &adv);
            }
            LL_SPI_SetReciveNum(spi_instance[temp_handle->spi_id], temp_handle->rx_size);
        }
        tick_start = HAL_BOARD_GetTime(HAL_TIME_MS);
        HAL_SPI_StartTrans(spi, SPI_DIR_RX);
        /* trigger transfer */
        if (temp_param->mode == SPI_MODE_MASTER) {
            if (temp_param->csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
                HAL_MSleep(10);
            }
            LL_SPI_TransmitData(spi_instance[temp_handle->spi_id], 0x00);  // triger sclk
        }

        while (temp_handle->rx_offset <  temp_handle->rx_size) {
            temp_handle->rx_offset += Spi_read_data((HAL_Dev_t *)spi,
                                        &buff[temp_handle->rx_offset],
                                        temp_handle->rx_size - temp_handle->rx_offset);
            if (HAL_BOARD_GetTime(HAL_TIME_MS) - tick_start > tick_cnt) {
                break;
            }
        }
    } else if (temp_handle->rx_mode == SPI_TRANS_MODE_INTERRUPT) {
#if CONFIG_SPI_TRANS_IT_ENABLE
        if (temp_param->mode == SPI_MODE_MASTER) {
            LL_SPI_SetReciveNum(spi_instance[temp_handle->spi_id], temp_handle->rx_size);
        }

        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->rx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }

        if (temp_param->mode == SPI_MODE_MASTER && temp_param->ioMode != SPI_IO_MODE_NORMAL) {
            SPI_FrameCtrl_t adv = {.instruction_len = 0, .address_len = 0, .trans_form = SPI_TRANS_ALL_SPECIFIED};
            HAL_SPI_ExtControl(spi, SPI_PARAM_FRAME_CTRL, &adv);
        }
        HAL_SPI_EnableIRQ(spi, SPI_IRQ_RX);
        HAL_SPI_StartTrans(spi, SPI_DIR_RX);
        /* trigger transfer */
        if (temp_param->mode == SPI_MODE_MASTER) {
            if (temp_param->csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
            LL_SPI_TransmitData(spi_instance[temp_handle->spi_id], 0);  // triger sclk
        }

        HAL_SemaphoreWait(&temp_handle->rx_sem, msec);
        HAL_SPI_DisableIRQ(spi, SPI_IRQ_RX);
        temp_handle->rxbuf = NULL;
        temp_handle->rx_size = 0;

#endif
    } else {
#if CONFIG_SPI_TRANS_DMA_ENABLE
        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->rx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }
        if (temp_param->mode == SPI_MODE_MASTER && temp_param->ioMode != SPI_IO_MODE_NORMAL) {
            SPI_FrameCtrl_t adv = {.instruction_len = 0, .address_len = 0, .trans_form = SPI_TRANS_ALL_SPECIFIED};
            HAL_SPI_ExtControl(spi, SPI_PARAM_FRAME_CTRL, &adv);
        }
        LL_SPI_SetReciveNum(spi_instance[temp_handle->spi_id], temp_handle->rx_size);

        HAL_SPI_EnableDMA(spi, SPI_DIR_RX);
        HAL_SPI_StartTrans(spi, SPI_DIR_RX);
        HAL_SPI_StartDMA(spi, SPI_DIR_RX, buff, size);
        /* trigger transfer */
        if (temp_param->mode == SPI_MODE_MASTER) {
            if (temp_param->csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
            LL_SPI_TransmitData(spi_instance[temp_handle->spi_id], 0);  // triger sclk
        }
        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_handle->rx_sem, msec)) {
            HAL_SPI_StopDMA(spi, SPI_IRQ_RX);
            return HAL_STATUS_TIMEOUT;
        } else {
            temp_handle->rx_offset = size;
        }

#endif
    }

    if (temp_param->mode == SPI_MODE_MASTER) {
        if (temp_param->csMode) {
            HAL_SPI_DisableCS(spi, temp_handle->slave_index);
        }
    }

    HAL_SPI_StopTrans(spi);
    temp_handle->state = HAL_SPI_STATE_READY;

    return temp_handle->rx_offset;
}

/**
 * @brief Spi transmit amount of data function
 * @param spi - device handle.
 * @param buff - data address
 * @param size - length of buff
 * @param msec - ms as a unit
 * @retval Length of transmitted data(>= 0) or failed status(< 0)
 */
int32_t HAL_SPI_Transmit(const HAL_Dev_t *spi, const uint8_t *buff, int32_t size, uint32_t msec)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    uint32_t tick_cnt = msec;
    uint32_t tick_start = 0;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    temp_handle->tx_offset = 0;
    temp_handle->tx_size = size;
    temp_handle->txbuf = (uint8_t *)buff;

    uint32_t finish_flag = 0;

    if (temp_handle->init_param.mode == SPI_MODE_SLAVE) {
        finish_flag = 31;
    }

    HAL_SPI_StopTrans(spi);
    temp_handle->state = HAL_SPI_STATE_BUSY_TX;

    if (temp_handle->tx_mode == SPI_TRANS_MODE_NOMA) {
        if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
            if (temp_handle->init_param.ioMode != SPI_IO_MODE_NORMAL) {
                SPI_FrameCtrl_t adv = {
                        .instruction_len = 0,
                        .address_len = temp_handle->init_param.dataWidth/4,
                        .trans_form = SPI_TRANS_ALL_SPECIFIED
                    };
                HAL_SPI_ExtControl(spi, SPI_PARAM_FRAME_CTRL, &adv);
            }
            if (temp_handle->init_param.csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
        }
        tick_start = HAL_BOARD_GetTime(HAL_TIME_MS);

        HAL_SPI_StartTrans(spi, SPI_DIR_TX);

        while ((temp_handle->tx_offset < temp_handle->tx_size)) {
            temp_handle->tx_offset += Spi_write_data((HAL_Dev_t *)spi,
                                            (uint8_t *)&buff[temp_handle->tx_offset],
                                            temp_handle->tx_size - temp_handle->tx_offset);
            if (HAL_BOARD_GetTime(HAL_TIME_MS) - tick_start > tick_cnt) {
                break;
            }
        }

    } else if (temp_handle->tx_mode == SPI_TRANS_MODE_INTERRUPT) {
#if CONFIG_SPI_TRANS_IT_ENABLE
        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->tx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }

        if ((temp_handle->init_param.mode == SPI_MODE_MASTER) &&
           (temp_handle->init_param.ioMode != SPI_IO_MODE_NORMAL)) {
            SPI_FrameCtrl_t adv = {
                                    .instruction_len = 0,
                                    .address_len = temp_handle->init_param.dataWidth/4,
                                    .trans_form = SPI_TRANS_ALL_SPECIFIED
                                };
            HAL_SPI_ExtControl(spi, SPI_PARAM_FRAME_CTRL, &adv);
        }
        HAL_SPI_EnableIRQ(spi, SPI_IRQ_TX);
        HAL_SPI_StartTrans(spi, SPI_DIR_TX);
        /* trigger transfer */
        if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
            if (temp_handle->init_param.csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
        }

        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_handle->tx_sem, msec)) {
            ;
        }
        HAL_SPI_DisableIRQ(spi, SPI_IRQ_TX);

        temp_handle->rxbuf = NULL;
        temp_handle->rx_size = 0;

#endif
    } else {
#if CONFIG_SPI_TRANS_DMA_ENABLE
        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->tx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }

        if (temp_handle->init_param.mode == SPI_MODE_MASTER && temp_handle->init_param.ioMode != SPI_IO_MODE_NORMAL) {
            SPI_FrameCtrl_t adv = {
                                        .instruction_len = 0,
                                        .address_len = temp_handle->init_param.dataWidth/4,
                                        .trans_form = SPI_TRANS_ALL_SPECIFIED
                                    };
            HAL_SPI_ExtControl(spi, SPI_PARAM_FRAME_CTRL, &adv);
        }
        HAL_SPI_EnableDMA(spi, SPI_DIR_TX);
        HAL_SPI_StartTrans(spi, SPI_DIR_TX);
        HAL_SPI_StartDMA(spi, SPI_DIR_TX, buff, size);
        if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
            if (temp_handle->init_param.csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
        }

        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_handle->tx_sem, msec)) {
            HAL_SPI_StopDMA(spi, SPI_DIR_TX);
            return HAL_STATUS_TIMEOUT;
        } else {
            temp_handle->tx_offset = size;
        }

#endif
    }

    /* check transmit finish*/
    tick_start = HAL_BOARD_GetTime(HAL_TIME_MS);
    while(LL_SPI_GetTxFifoLevel(QSPI0) != finish_flag) {
        if (HAL_BOARD_GetTime(HAL_TIME_MS) - tick_start > SPI_TRANSMIT_WAIT_TIME) {
            break;
        }
    }

    uint32_t less_shift_delay_us = 0;
    less_shift_delay_us = 2 * 32 * OSI_USEC_PER_SEC / HAL_BOARD_GetFreq(CLOCK_PLL_SOC); /* buffer * 32width * us */
    if (less_shift_delay_us == 0) {
        less_shift_delay_us = 1;
    }

    LL_Delay(SPI_TRANSMIT_WAIT_TIME, LL_DELAY_US, HAL_BOARD_GetFreq(CLOCK_PLL_SOC));

    if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
        if (temp_handle->init_param.csMode) {
            HAL_SPI_DisableCS(spi, temp_handle->slave_index);
        }
    }

    HAL_SPI_StopTrans(spi);
    temp_handle->state = HAL_SPI_STATE_READY;
    return temp_handle->tx_offset;
}

/**
 * @brief spi transmit and receive amount of data function
 * @param spi - device handle.
 * @param tx_data - tx buf pointer
 * @param rx_data- rx buf pointer
 * @param size - length of buf
 * @param msec - waiting time(ms)
 * @retval Length of transmitted data(>= 0) or failed status(< 0)
 */
int32_t HAL_SPI_TransmitReceive(const HAL_Dev_t *spi, uint8_t *tx_data, uint8_t *rx_data, int32_t size, uint32_t msec)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    uint32_t count = 0;
    uint32_t tick_cnt = msec;
    uint32_t tick_start = 0;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    temp_handle->state = HAL_SPI_STATE_BUSY_TX_RX;

    temp_handle->tx_size = size;
    temp_handle->tx_offset = 0;
    temp_handle->txbuf = tx_data;
    temp_handle->rx_size = size;
    temp_handle->rx_offset = 0;
    temp_handle->rxbuf = rx_data;

    HAL_SPI_StopTrans(spi);

    if (temp_handle->full_mode == SPI_TRANS_MODE_NOMA) {
        if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
            if (temp_handle->init_param.csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
        }
        tick_start = HAL_BOARD_GetTime(HAL_TIME_MS);
        HAL_SPI_StartTrans(spi, SPI_DIR_FULL);

        while ((temp_handle->tx_offset < temp_handle->tx_size) ||
            (temp_handle->rx_offset < temp_handle->rx_size)) {
            temp_handle->tx_offset += Spi_write_data((HAL_Dev_t *)spi,
                                        &tx_data[temp_handle->tx_offset],
                                        temp_handle->tx_size - temp_handle->tx_offset);
            temp_handle->rx_offset += Spi_read_data((HAL_Dev_t *)spi,
                                        &rx_data[temp_handle->rx_offset],
                                        temp_handle->rx_size - temp_handle->rx_offset);
            if ((HAL_BOARD_GetTime(HAL_TIME_MS) - tick_start) > tick_cnt) {
                break;
            }
        }

    } else if (temp_handle->full_mode == SPI_TRANS_MODE_INTERRUPT) {
#if CONFIG_SPI_TRANS_IT_ENABLE
        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->tx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }
        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->rx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }

        HAL_SPI_StartTrans(spi, SPI_DIR_FULL);
        /* trigger transfer */
        if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
            if (temp_handle->init_param.csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
        }
        HAL_SPI_EnableIRQ(spi, SPI_IRQ_RX);
        HAL_SPI_EnableIRQ(spi, SPI_IRQ_TX);
        /* waiting compelet */
        while (1) {
            if (HAL_STATUS_OK == HAL_SemaphoreWait(&temp_handle->tx_sem, 10)) {
                break;
            }
            count++;
            if (count >= msec) {
                break;      // timeout
            }
        }

        while (1) {
            if (HAL_STATUS_OK == HAL_SemaphoreWait(&temp_handle->rx_sem, 10)) {
                break;
            }
            count++;
            if (count >= msec) {
                break;      // timeout
            }
        }

        HAL_SPI_DisableIRQ(spi, SPI_IRQ_TX);
        HAL_SPI_DisableIRQ(spi, SPI_IRQ_RX);

        temp_handle->rxbuf = NULL;
        temp_handle->rx_size = 0;

#endif
    } else {
#if CONFIG_SPI_TRANS_DMA_ENABLE
        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->tx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }
        if (HAL_STATUS_OK !=  Spi_semaphore_create(&temp_handle->rx_sem)) {
            return HAL_STATUS_ERROR;  // failed to create smeaphore
        }

        HAL_SPI_EnableDMA(spi, SPI_DIR_FULL);

        HAL_SPI_StartTrans(spi, SPI_DIR_FULL);
        if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
            if (temp_handle->init_param.csMode) {
                HAL_SPI_EnableCS(spi, temp_handle->slave_index);
            }
        }
        HAL_SPI_StartDMA(spi, SPI_DIR_RX, rx_data, size);
        HAL_SPI_StartDMA(spi, SPI_DIR_TX, tx_data, size);

        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_handle->tx_sem, msec)) {
            HAL_SPI_StopDMA(spi, SPI_DIR_TX);
            return HAL_STATUS_TIMEOUT;
        }
        if (HAL_STATUS_OK != HAL_SemaphoreWait(&temp_handle->rx_sem, msec)) {
            HAL_SPI_StopDMA(spi, SPI_DIR_RX);
            return HAL_STATUS_TIMEOUT;
        }
        temp_handle->rx_offset = size;

#endif
    }

    while (LL_SPI_BusIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_STATUS_BUSY)) {
        /* waiting for transmission complete */
    }
    LL_Delay(SPI_TRANSMIT_WAIT_TIME, LL_DELAY_US, HAL_BOARD_GetFreq(CLOCK_PLL_SOC));
    if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
        if (temp_handle->init_param.csMode) {
            HAL_SPI_DisableCS(spi, temp_handle->slave_index);
        }
    }

    HAL_SPI_StopTrans(spi);
    temp_handle->state = HAL_SPI_STATE_READY;

    return temp_handle->rx_offset;
}

#if CONFIG_SPI_TRANS_IT_ENABLE

/**
 * @brief Register interrupt callback function
 * @param spi - device handle.
 * @param irq_type - irq type.
 * @param irqCb - callback structure.
 * @retval status
 */
HAL_Status_t HAL_SPI_RegisterIRQ(HAL_Dev_t *spi, SPI_IRQ_t irq_type, const HAL_Callback_t *irqCb)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL || irqCb == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) ||
        (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (irq_type == SPI_IRQ_TX) {
        temp_handle->isr_tx_cb.arg = irqCb->arg;
        temp_handle->isr_tx_cb.cb = irqCb->cb;
    } else if (irq_type == SPI_IRQ_RX) {
        temp_handle->isr_rx_cb.arg = irqCb->arg;
        temp_handle->isr_rx_cb.cb = irqCb->cb;
    } else {
        temp_handle->isr_cs_cb.arg = irqCb->arg;
        temp_handle->isr_cs_cb.cb = irqCb->cb;
    }

    return HAL_STATUS_OK;
}

/**
 * @brief Unregister interrupt callback function
 * @param spi - device handle.
 * @param irq_type - irq type.
 * @retval status
 */
HAL_Status_t HAL_SPI_UnregisterIRQ(const HAL_Dev_t *spi, SPI_IRQ_t irq_type)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (irq_type == SPI_IRQ_TX) {
        temp_handle->isr_tx_cb.arg = NULL;
        temp_handle->isr_tx_cb.cb = NULL;
    } else if (irq_type == SPI_IRQ_RX) {
        temp_handle->isr_rx_cb.arg = NULL;
        temp_handle->isr_rx_cb.cb = NULL;
    } else {
        temp_handle->isr_cs_cb.arg = NULL;
        temp_handle->isr_cs_cb.cb = NULL;
    }

    return HAL_STATUS_OK;

}

/**
 * @brief Enable interrupt function
 * @param spi - device handle.
 * @param irq_type - irq type.
 * @retval status
 */
__spi_sram_text HAL_Status_t HAL_SPI_EnableIRQ(const HAL_Dev_t *spi, SPI_IRQ_t irq_type)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (irq_type == SPI_IRQ_TX) {
        LL_SPI_ITEnable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_TXE);
    } else if (irq_type == SPI_IRQ_RX) {
        LL_SPI_ITEnable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_RXF);
    } else {
        LL_SPI_ITEnable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_HIGH_EDG);
        temp_handle->cs_irq_state = 1;
    }

    return HAL_STATUS_OK;

}

/**
 * @brief Disable interrupt function
 * @param uart - device handle.
 * @param irq_type - irq type.
 * @retval status
 */
__spi_sram_text HAL_Status_t HAL_SPI_DisableIRQ(const HAL_Dev_t *spi, SPI_IRQ_t irq_type)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (irq_type == SPI_IRQ_TX) {
        LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_TXE);
    } else if (irq_type == SPI_IRQ_RX) {
        LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_RXF);
    } else {
        LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_HIGH_EDG);
        temp_handle->cs_irq_state = 0;
    }

    return HAL_STATUS_OK;

}

#endif      // CONFIG_SPI_TRANS_IT_ENABLE

#if CONFIG_SPI_TRANS_DMA_ENABLE
/**
 * @brief Register dma callback function
 * @param spi - device handle.
 * @param transDir - transmisson direction.
 * @param irqCb - callback structure.
 * @retval status
 */
HAL_Status_t HAL_SPI_RegisterDMA(HAL_Dev_t *spi, SPI_TransDir_t transDir, HAL_Callback_t *dmaCb)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL || dmaCb == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (transDir == SPI_DIR_TX) {
        temp_handle->dma_tx_cb.arg = dmaCb->arg;
        temp_handle->dma_tx_cb.cb = dmaCb->cb;

        temp_handle->dma_tx_device = HAL_DMA_Init(DMA0_ID);
        if (temp_handle->dma_tx_device == NULL) {
            return HAL_STATUS_ERROR;
        }
    } else if (transDir == SPI_DIR_RX) {
        temp_handle->dma_rx_cb.arg = dmaCb->arg;
        temp_handle->dma_rx_cb.cb = dmaCb->cb;

        temp_handle->dma_rx_device = HAL_DMA_Init(DMA0_ID);
        if (temp_handle->dma_rx_device == NULL) {
            return HAL_STATUS_ERROR;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    Spi_dma_config(spi, transDir);

    return HAL_STATUS_OK;

}

/**
 * @brief Unregister dma callback function
 * @param spi - device handle.
 * @param transDir - transmisson direction.
 * @retval status
 */
HAL_Status_t HAL_SPI_UnregisterDMA(const HAL_Dev_t *spi, SPI_TransDir_t transDir)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (transDir == SPI_DIR_TX) {
        temp_handle->dma_tx_cb.arg = NULL;
        temp_handle->dma_tx_cb.cb = NULL;

        if (HAL_STATUS_OK != HAL_DMA_Close(temp_handle->dma_tx_device, temp_handle->dma_tx_id)) {
            return HAL_STATUS_ERROR;
        }
    } else if (transDir == SPI_DIR_RX) {
        temp_handle->dma_rx_cb.arg = NULL;
        temp_handle->dma_rx_cb.cb = NULL;

        if (HAL_STATUS_OK != HAL_DMA_Close(temp_handle->dma_rx_device, temp_handle->dma_rx_id)) {
            return HAL_STATUS_ERROR;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;

}

/**
 * @brief Start dma transmission function
 * @param spi - device handle.
 * @param transDir - direction.
 * @param buf - data pointer.
 * @param size - length of buf.
 * @retval status
 */
HAL_Status_t HAL_SPI_StartDMA(const HAL_Dev_t *spi, SPI_TransDir_t transDir, const uint8_t *buf, int32_t size)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    DMA_BlockConf_t blockcfg  = { 0 };
    HAL_Status_t status;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    csi_dcache_invalid_range((unsigned long *)buf, size);

    if (transDir == SPI_DIR_TX) {
        blockcfg.srcAddr    =   (uint32_t)buf;
        blockcfg.dstAddr    =   (uint32_t)&spi_instance[temp_handle->spi_id]->DR;
        blockcfg.blockSize  =   size;

        status = HAL_DMA_ConfigBlockTranfer(temp_handle->dma_tx_device, temp_handle->dma_tx_id, &blockcfg);
        if (status != HAL_STATUS_OK) {
            return HAL_STATUS_ERROR;
        }

        /* launch DMA transfer */
        if (HAL_STATUS_OK != HAL_DMA_StartTransfer(temp_handle->dma_tx_device, temp_handle->dma_tx_id)) {
            return HAL_STATUS_ERROR;
        }

    } else if (transDir == SPI_DIR_RX) {
        if (temp_handle->init_param.mode == SPI_MODE_MASTER) {
            LL_SPI_SetReciveNum(spi_instance[temp_handle->spi_id],  size);
        }

        blockcfg.srcAddr    =   (uint32_t)&spi_instance[temp_handle->spi_id]->DR;
        blockcfg.dstAddr    =   (uint32_t)buf;
        blockcfg.blockSize  =   size;

        status = HAL_DMA_ConfigBlockTranfer(temp_handle->dma_rx_device, temp_handle->dma_rx_id, &blockcfg);
        if (status != HAL_STATUS_OK) {
            return HAL_STATUS_ERROR;
        }

        /* launch DMA transfer */
        if (HAL_STATUS_OK != HAL_DMA_StartTransfer(temp_handle->dma_rx_device, temp_handle->dma_rx_id)) {
            return HAL_STATUS_ERROR;
        }

    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;

}

/**
 * @brief stop dma transmission function
 * @param spi - device handle.
 * @param transDir - direction.
 * @retval status
 */
int32_t HAL_SPI_StopDMA(const HAL_Dev_t *spi, SPI_TransDir_t transDir)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;
    uint32_t temp_size = 0;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (transDir == SPI_DIR_TX) {
        HAL_DMA_AbortTransfer(temp_handle->dma_tx_device, temp_handle->dma_tx_id);
    } else if (transDir == SPI_DIR_RX) {
        HAL_DMA_AbortTransfer(temp_handle->dma_rx_device, temp_handle->dma_rx_id);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return temp_size;
}

#endif /* CONFIG_SPI_TRANS_DMA_ENABLE */

/**
 * @brief Enable dma function
 * @param spi - device handle.
 * @param dir - direction.
 * @retval status
 */
HAL_Status_t HAL_SPI_EnableDMA(const HAL_Dev_t *spi , SPI_TransDir_t dir)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

     if (dir == SPI_DIR_TX) {
        LL_SPI_EnableDMAReq_TX(spi_instance[temp_handle->spi_id]);
     } else if (dir == SPI_DIR_RX) {
        LL_SPI_EnableDMAReq_RX(spi_instance[temp_handle->spi_id]);
     } else {
        LL_SPI_EnableDMAReq_TX(spi_instance[temp_handle->spi_id]);
        LL_SPI_EnableDMAReq_RX(spi_instance[temp_handle->spi_id]);
     }

     return HAL_STATUS_OK;
}

/**
 * @brief Disable dma function
 * @param spi - device handle.
 * @param dir - direction.
 * @retval status
 */
HAL_Status_t HAL_SPI_DisableDMA(const HAL_Dev_t *spi , SPI_TransDir_t dir)
{
    spi_device_t *temp_handle = (spi_device_t *)spi;

    if (spi == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if ((SPI_ID_GET(spi->id) > SPI_NUM_MAX) || (SPI_TYPE_GET(spi->id) != HAL_DEV_TYPE_SPI)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    if (spi->state != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

     if (dir == SPI_DIR_TX) {
        LL_SPI_DisableDMAReq_TX(spi_instance[temp_handle->spi_id]);
     } else if (dir == SPI_DIR_RX) {
        LL_SPI_DisableDMAReq_RX(spi_instance[temp_handle->spi_id]);
     } else {
        LL_SPI_DisableDMAReq_TX(spi_instance[temp_handle->spi_id]);
        LL_SPI_DisableDMAReq_RX(spi_instance[temp_handle->spi_id]);
     }

     return HAL_STATUS_OK;

}

/**
  * @brief  This function spi irq handler.
  * @param0 spi handle struct.
  * @note
  */
__spi_sram_text void Spi_irq_handler(void *hdev)
{
    spi_device_t *temp_handle = (spi_device_t *)hdev;

    if (LL_SPI_ITIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_IT_RXF)
        && LL_SPI_ITIsEnabled(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_RXF)) {
        if (temp_handle->isr_rx_cb.cb != NULL) {
            temp_handle->isr_rx_cb.cb((HAL_Dev_t *)hdev, temp_handle->isr_rx_cb.arg);
        } else {
            /* disable RXF interrupt */
            LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_RXF);
        }
    }

    if (LL_SPI_ITIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_IT_TXE)
        && LL_SPI_ITIsEnabled(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_TXE)) {
        if (temp_handle->isr_tx_cb.cb != NULL) {
            temp_handle->isr_tx_cb.cb((HAL_Dev_t *)hdev, temp_handle->isr_tx_cb.arg);
        } else {
            /* disable TXE interrupt */
            LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_TXE);
        }
    }

    if (LL_SPI_ITIsActiveFlag(spi_instance[temp_handle->spi_id], LL_QSPI_IT_HIGH_EDG)
        && LL_SPI_ITIsEnabled(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_HIGH_EDG)) {
        if (temp_handle->isr_cs_cb.cb != NULL) {
            temp_handle->isr_cs_cb.cb((HAL_Dev_t *)hdev, temp_handle->isr_cs_cb.arg);
        } else {
            LL_SPI_ITDisable(spi_instance[temp_handle->spi_id], LL_QSPI_ITE_HIGH_EDG);
        }
    }

    LL_SPI_ITFlagClearALL(spi_instance[temp_handle->spi_id]);
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
