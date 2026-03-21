/**
 ******************************************************************************
 * @file    template.c
 * @brief   template define.
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

#include "hif_types.h"
#include "hif_config.h"
#include "hif_log.h"
#include "hif.h"
#include "hif_log.h"
#include "hal_uart.h"
#include "hif_com.h"
#if (CONFIG_HIF_SEND_DMA)
#include "hif_com_dma.h"
#endif


#if (CONFIG_HIF == 1)

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#if (CONFIG_HIF_DEVICE_COM_UART)


#if (CONFIG_HIF_DEVICE_WAKEUP_UART)
static pm_wakeup_callback_t hif_uart_pm_wakeup_callback = NULL;
#endif

static void hif_com_uart_tx_isr(HAL_Dev_t * pDevice, void *arg);
static void hif_com_uart_rx_isr(HAL_Dev_t * pDevice, void *arg);
/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t *hif_uart_instance;

UART_InitParam_t hif_uart_cfg = {.baudRate = 921600,
                               .parity = UART_PARITY_NONE,
                               .stopBits = UART_STOP_BIT_1,
                               .dataBits = UART_DATA_WIDTH_8,
                               .autoFlowCtrl = UART_FLOW_CTRL_NONE};

int hif_com_uart_init(uint32_t rate)
{
    HAL_Callback_t temp_cb;

    hif_uart_cfg.baudRate = rate;

    UART_ID_t uart_index = CONFIG_HIF_UART_PORT;

    com_port_set(CONFIG_HIF_UART_PORT);

    hif_uart_instance = HAL_UART_Init(uart_index, &hif_uart_cfg);
    if(hif_uart_instance != NULL) {
        if(HAL_UART_Open(hif_uart_instance) == 0) {
#if (CONFIG_HIF_SEND_DMA)
            if(uart_index == 0) {
                hif_com_tx_dma_init(uart0_tx);
            } else if(uart_index == 1) {
                hif_com_tx_dma_init(uart1_tx);
            } else {
                hif_com_tx_dma_init(uart2_tx);
            }
#else
            temp_cb.cb = hif_com_uart_tx_isr;
            temp_cb.arg = NULL;
            HAL_UART_RegisterIRQ((HAL_Dev_t *)hif_uart_instance, UART_DIR_TX, &temp_cb);
#endif
            temp_cb.cb = hif_com_uart_rx_isr;
            temp_cb.arg = NULL;
            HAL_UART_RegisterIRQ((HAL_Dev_t *)hif_uart_instance, UART_DIR_RX, &temp_cb);

            HAL_UART_EnableRxReadyIRQ(hif_uart_instance);

            return HIF_ERRCODE_SUCCESS;
        }
    }
    return HIF_ERRCODE_NOT_READY;
}

int hif_com_uart_deinit(void)
{
#if (CONFIG_HIF_SEND_DMA)
    if(hif_com_dma_deinit()) {
        return HIF_ERRCODE_NOT_READY;
    }
#endif
    if(HAL_UART_Close(hif_uart_instance) == 0) {
        if(HAL_UART_DeInit(hif_uart_instance) == 0) {
            return HIF_ERRCODE_SUCCESS;
        }
    }
    return HIF_ERRCODE_NOT_READY;
}

extern void hif_Msg_ReportCancel(void);

__hif_sram_text void hif_com_uart_wakeup(void)
{
    uint16_t count = 0;

    hif_Msg_ReportCancel();

    uint8_t tx_data[2] = {HIF_PHY_ACK_MAGIC, HIF_PHY_ACK_MAGIC};

    while(HAL_UART_IsTxReady(hif_uart_instance) && count < sizeof(tx_data)) {
        HAL_UART_PutTxData(hif_uart_instance, tx_data[count ++]);
    }
}

__hif_sram_text int hif_com_uart_tx_start(void)
{
#if (CONFIG_HIF_SEND_DMA)
        hif_com_data_dma_list_config();
        hif_com_dma_transfer_start(tx_channel_idx);
#else
    HAL_UART_EnableTxReadyIRQ(hif_uart_instance);
#endif
    return 0;
}

__hif_sram_text int hif_com_uart_tx_done(void)
{
    HAL_UART_DisableTxReadyIRQ(hif_uart_instance);
    return 0;
}

void hif_com_uart_tx_timeout(void *arg)
{
    hif_com_data_output_done();
    HAL_UART_DisableTxReadyIRQ(hif_uart_instance);
}


__hif_sram_text int hif_com_uart_rx_done(void)
{
    hif_com_data_recv_done();

    return 0;
}

void hif_com_uart_rx_timeout(void *arg)
{
    hif_com_rx_buff_reset();
    hif_Msg_ReportCancel();
}


__hif_sram_text int hif_com_uart_write(uint8_t *pbuff, uint16_t size)
{
    uint16_t count = 0;
    while(HAL_UART_IsTxReady(hif_uart_instance) && count < size) {
        HAL_UART_PutTxData(hif_uart_instance, pbuff[count ++]);
    }

    return count;
}

__hif_sram_text int hif_com_uart_read(uint8_t *pbuff, uint16_t size)
{
    uint16_t count = 0;
    while(HAL_UART_IsRxReady(hif_uart_instance) && count < size) {
        pbuff[count ++] = HAL_UART_GetRxData(hif_uart_instance);
    }
    return count;
}

__hif_sram_text static void hif_com_uart_tx_isr(HAL_Dev_t * pDevice, void *arg)
{
    int status = 0;

    if(HAL_UART_IsTxReady(pDevice)) {
        status = hif_com_data_send(0);
        if (status == HIF_COM_TX_DONE) {
            while (!HAL_UART_IsTxEmpty(pDevice)) {
            }
            hif_com_data_output_done();
            hif_com_data_send_done();
        }
    }
}
__hif_sram_text static void hif_com_uart_rx_isr(HAL_Dev_t * pDevice, void *arg)
{
    if(HAL_UART_IsRxReady(pDevice)){
        hif_com_data_recv();
    }
}


#if (CONFIG_HIF_DEVICE_WAKEUP_UART)

static void hif_uart_callback(HAL_Dev_t * pDevice, void *arg)
{
    if (hif_uart_pm_wakeup_callback != NULL) {
        hif_uart_pm_wakeup_callback(0);
    }
}

int hif_com_uart_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg)
{
    uint8_t enable = 1;
    HAL_Callback_t temp_cb;

    if ((pm_cb == NULL) || (pcfg == NULL)) {
        return HIF_ERRCODE_INVALID_PARAM;
    }
    if(hif_uart_instance) {
        temp_cb.cb = hif_uart_callback;
        temp_cb.arg = NULL;
        HAL_UART_ExtControl(hif_uart_instance, UART_PARAM_WAKEUP_THRESHOLD, pcfg);
        HAL_UART_ExtControl(hif_uart_instance, UART_PARAM_WAKEUP_CALLBACK, &temp_cb);
        HAL_UART_ExtControl(hif_uart_instance, UART_PARAM_WAKEUP_CTRL, &enable);
        hif_uart_pm_wakeup_callback = pm_cb;
    } else {
        return HIF_ERRCODE_NOT_READY;
    }

    return HIF_ERRCODE_SUCCESS;
}

int hif_com_uart_pm_deinit(void)
{
    uint8_t enable = 0;

    if(hif_uart_instance) {
        HAL_UART_ExtControl(hif_uart_instance, UART_PARAM_WAKEUP_CALLBACK, NULL);
        HAL_UART_ExtControl(hif_uart_instance, UART_PARAM_WAKEUP_CTRL, &enable);
    } else {
        return HIF_ERRCODE_NOT_READY;
    }

    return HIF_ERRCODE_SUCCESS;
}

#endif /* CONFIG_HIF_DEVICE_WAKEUP_UART */

#endif

#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
