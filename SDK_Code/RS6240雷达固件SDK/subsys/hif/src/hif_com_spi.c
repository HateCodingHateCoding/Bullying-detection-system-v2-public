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
#include "hal_spi.h"
#include "hif_com.h"
#include "hif_com_spi.h"
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
#if (CONFIG_HIF_DEVICE_COM_SPI)

#if (CONFIG_HIF_DEVICE_WAKEUP_SPI)
static pm_wakeup_callback_t hif_spi_pm_wakeup_callback = NULL;
#endif

static void hif_com_spi_tx_isr(HAL_Dev_t * pDevice, void *arg);
static void hif_com_spi_rx_isr(HAL_Dev_t * pDevice, void *arg);
static void hif_com_spi_cs_isr(HAL_Dev_t * pDevice, void *arg);

void hif_com_dma_rx_wakeup_callback(HAL_Dev_t * pDevice, void *arg);

static trf_state_t hif_com_spi_trf_state;
static uint8_t hif_com_spi_wakeup_state;
/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t *hif_spi_instance;

SPI_InitParam_t hif_spi_cfg = {
                            .mode = SPI_MODE_SLAVE,
                            .sclkMode = SPI_SCLK_MODE0,
                            .ioMode = SPI_IO_MODE_NORMAL,
                            .csMode = SPI_CS_HARD,
                            .dataWidth = SPI_DATAWIDTH_8BIT,
                            .sclk = 56000000,
};

#if (CONFIG_HIF_SEND_DMA)
static uint32_t rx_block_size = 0;
static uint32_t rx_block_offset = 0;
_Alignas(32) uint8_t spi_rx_buffer[SPI_RX_BUFFER_SIZE] = {0x00};
#endif

int hif_com_spi_init(uint32_t rate)
{
    HAL_Callback_t temp_cb;

    com_port_set(com_spi);

    hif_spi_instance = HAL_SPI_Init(SPI0_ID, &hif_spi_cfg, 1);
    if (hif_spi_instance != NULL)
    {
        if(HAL_SPI_Open(hif_spi_instance, 0) == 0) {
#if (CONFIG_HIF_SEND_DMA)
			uint8_t level = 0;

            level = 16;
            HAL_SPI_ExtControl(hif_spi_instance, SPI_PARAM_TX_DMA_LEVEL, &level);

            HAL_SPI_EnableDMA(hif_spi_instance, SPI_DIR_TX);

            if(hif_com_tx_dma_init(spi0_tx) != 0) {
                return HIF_ERRCODE_NOT_READY;
            }

            temp_cb.cb = hif_com_spi_cs_isr;
            temp_cb.arg = NULL;
            HAL_SPI_RegisterIRQ(hif_spi_instance, SPI_IRQ_CS, &temp_cb);

            level = 1;
            HAL_SPI_ExtControl(hif_spi_instance, SPI_PARAM_RX_DMA_LEVEL, &level);

            HAL_SPI_EnableDMA(hif_spi_instance, SPI_DIR_RX);

            hif_com_rx_dma_init(spi0_rx);
            hif_com_dma_block_config(rx_channel_idx, (uint32_t)spi_rx_buffer, sizeof(spi_rx_buffer));

            HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);

            hif_com_dma_transfer_start(rx_channel_idx);

            HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_CS);
#else
            temp_cb.cb = hif_com_spi_tx_isr;
            temp_cb.arg = NULL;
            HAL_SPI_RegisterIRQ(hif_spi_instance, SPI_IRQ_TX, &temp_cb);

            temp_cb.cb = hif_com_spi_rx_isr;
            temp_cb.arg = NULL;
            HAL_SPI_RegisterIRQ(hif_spi_instance, SPI_IRQ_RX, &temp_cb);

            temp_cb.cb = hif_com_spi_cs_isr;
            temp_cb.arg = NULL;
            HAL_SPI_RegisterIRQ(hif_spi_instance, SPI_IRQ_CS, &temp_cb);

            HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);

            HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_RX);
#endif

            hif_com_spi_trf_state = TRF_STATE_RX_BUSY;
            hif_com_spi_wakeup_state = 0;

            return HIF_ERRCODE_SUCCESS;
        }
    }
    return HIF_ERRCODE_NOT_READY;
}

int hif_com_spi_deinit(void)
{
#if (CONFIG_HIF_SEND_DMA)
    if(hif_com_dma_deinit()) {
        return HIF_ERRCODE_NOT_READY;
    }
#endif
    if(HAL_SPI_Close(hif_spi_instance) == 0) {
        if(HAL_SPI_DeInit(hif_spi_instance) == 0) {
            return HIF_ERRCODE_SUCCESS;
        }
    }
    return HIF_ERRCODE_NOT_READY;
}

__hif_sram_text int hif_com_spi_write(uint8_t *pbuff, uint16_t size)
{
    uint32_t fifo_size = 0;

    for (fifo_size = 0; fifo_size < size; fifo_size++) {
        if (HAL_SPI_IsTxReady(hif_spi_instance) == 0) {
            break;
        }

        HAL_SPI_PutTxData(hif_spi_instance, pbuff[fifo_size]);
    }

    return fifo_size;
}

__hif_sram_text int hif_com_spi_read(uint8_t *pbuff, uint16_t size)
{
#if (!CONFIG_HIF_SEND_DMA)
    uint32_t fifo_size = 0;

    fifo_size = HAL_SPI_GetFifoLevel(hif_spi_instance, SPI_FIFO_RX);
    if (fifo_size >= size) {
        fifo_size = size;
    }

    for (uint16_t idx = 0; idx < fifo_size; idx++) {
        pbuff[idx] = HAL_SPI_GetRxData(hif_spi_instance);
    }

    return fifo_size;
#else
    uint32_t read_size = MIN(size, rx_block_size);
    memcpy(pbuff, &spi_rx_buffer[rx_block_offset], read_size);
    rx_block_offset  += read_size;
    rx_block_size -= read_size;

    return read_size;
#endif

}

extern void hif_Msg_ReportCancel(void);

__hif_sram_text void hif_com_spi_wakeup(void)
{
    HAL_SPI_StopTrans(hif_spi_instance);
    HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_TX);
    uint8_t tx_data[4] = {HIF_PHY_ACK_MAGIC, HIF_PHY_ACK_MAGIC, 0, 0};
    hif_com_spi_write(tx_data, sizeof(tx_data));

    hif_Msg_ReportCancel();

    hif_com_spi_trf_state = TRF_STATE_TX_BUSY;
    hif_com_spi_wakeup_state = 1;
}

__hif_sram_text int hif_com_spi_tx_start(void)
{
#if (CONFIG_HIF_SEND_DMA)
    hif_com_dma_transfer_stop(rx_channel_idx);

    hif_com_data_dma_list_config();

    hif_com_spi_trf_state = TRF_STATE_TX_BUSY;
    HAL_SPI_StopTrans(hif_spi_instance);
    HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_TX);
    return hif_com_dma_transfer_start(tx_channel_idx);
#else
    if (hif_com_spi_trf_state != TRF_STATE_IDLE) {
        return HIF_ERRCODE_IO_ERROR;
    }

    hif_com_spi_trf_state = TRF_STATE_TX_BUSY;
    HAL_SPI_StopTrans(hif_spi_instance);
    HAL_SPI_DisableIRQ(hif_spi_instance, SPI_IRQ_TX);
    HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_TX);

    hif_com_data_send(0);

    HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_TX);
	return HIF_ERRCODE_SUCCESS;
#endif
}

__hif_sram_text int hif_com_spi_tx_end(void)
{
#if (CONFIG_HIF_SEND_DMA)
        //LL_SPI_ITEnable(QSPI_DEV, LL_QSPI_ITE_HIGH_EDG);
    hif_com_dma_transfer_stop(rx_channel_idx);
    hif_com_dma_block_config(rx_channel_idx, (uint32_t)spi_rx_buffer, sizeof(spi_rx_buffer));
    hif_com_dma_transfer_start(rx_channel_idx);
#else
    hif_com_spi_trf_state = TRF_STATE_TX_DONE;
#endif

    return 0;
}

void hif_com_spi_tx_timeout(void *arg)
{
    hif_com_data_output_done();
    HAL_SPI_StopTrans(hif_spi_instance);
    HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);
    HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_RX);
    hif_com_spi_trf_state = TRF_STATE_RX_BUSY;
}

void hif_com_spi_rx_timeout(void *arg)
{
    hif_com_rx_buff_reset();
    hif_Msg_ReportCancel();
    HAL_SPI_StopTrans(hif_spi_instance);
    HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);
    HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_RX);
    hif_com_spi_trf_state = TRF_STATE_IDLE;
    HAL_SPI_DisableIRQ(hif_spi_instance, SPI_IRQ_CS);
}


__hif_sram_text int hif_com_spi_rx_end(void)
{
    HAL_SPI_StopTrans(hif_spi_instance);
    HAL_SPI_DisableIRQ(hif_spi_instance, SPI_IRQ_RX);
    hif_com_data_recv_done();
    hif_com_spi_trf_state = TRF_STATE_IDLE;

    return 0;
}

__hif_sram_text static void hif_com_spi_tx_isr(HAL_Dev_t * pDevice, void *arg)
{
    if (hif_com_spi_wakeup_state) {
        hif_com_spi_trf_state = TRF_STATE_TX_DONE;
        HAL_SPI_DisableIRQ(hif_spi_instance, SPI_IRQ_TX);
        HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_CS);
    } else {
        if (hif_com_spi_trf_state == TRF_STATE_TX_DONE) {
            HAL_SPI_DisableIRQ(hif_spi_instance, SPI_IRQ_TX);
            HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_CS);
        } else {
            hif_com_data_send(0);
        }
    }
}
__hif_sram_text static void hif_com_spi_rx_isr(HAL_Dev_t * pDevice, void *arg)
{
    hif_com_data_recv();
}
extern void hif_com_buffer_dcache_invalid(uint32_t addr, uint32_t size);
__hif_sram_text static void hif_com_spi_cs_isr(HAL_Dev_t * pDevice, void *arg)
{
    if (hif_com_spi_wakeup_state) {
        if (hif_com_spi_trf_state == TRF_STATE_TX_BUSY) {
            HAL_SPI_StopTrans(hif_spi_instance);
            HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);
#if (CONFIG_HIF_SEND_DMA)
            hif_com_dma_transfer_stop(rx_channel_idx);
            hif_com_dma_block_config(rx_channel_idx, (uint32_t)spi_rx_buffer, sizeof(spi_rx_buffer));
            hif_com_dma_transfer_start(rx_channel_idx);
#endif
            hif_com_spi_trf_state = TRF_STATE_RX_BUSY;
            hif_com_spi_wakeup_state = 0;
        }
    } else {
#if (CONFIG_HIF_SEND_DMA)
        if (hif_com_spi_trf_state == TRF_STATE_TX_BUSY) {
            if(HAL_SPI_GetFifoLevel(hif_spi_instance, SPI_FIFO_TX) == 0) {
                LOG_IO(0x1000, 11, 0, 2);
                hif_com_data_output_done();
                HAL_SPI_StopTrans(hif_spi_instance);
                HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);
                hif_com_dma_block_config(rx_channel_idx, (uint32_t)spi_rx_buffer, sizeof(spi_rx_buffer));
                hif_com_dma_transfer_start(rx_channel_idx);
                hif_com_spi_trf_state = TRF_STATE_RX_BUSY;
            }
            hif_com_data_send_done();
        }
#else
        if (hif_com_spi_trf_state == TRF_STATE_TX_DONE) {
            if(HAL_SPI_GetFifoLevel(hif_spi_instance, SPI_FIFO_TX) == 0) {
                hif_com_data_output_done();
                HAL_SPI_StopTrans(hif_spi_instance);
                HAL_SPI_EnableIRQ(hif_spi_instance, SPI_IRQ_RX);
                HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);
                hif_com_spi_trf_state = TRF_STATE_RX_BUSY;
                hif_com_data_send_done();
            }
        }
#endif
        else if (hif_com_spi_trf_state == TRF_STATE_RX_BUSY) {
#if (CONFIG_HIF_SEND_DMA)
            rx_block_offset = 0;
            rx_block_size = hif_com_dma_block_size_get(rx_channel_idx);
            if(rx_block_size == SPI_RX_BUFFER_SIZE) {
                rx_block_size = 0;
                HAL_SPI_StopTrans(hif_spi_instance);
                HAL_SPI_StartTrans(hif_spi_instance, SPI_DIR_RX);
                hif_com_dma_transfer_stop(rx_channel_idx);
                hif_com_dma_block_config(rx_channel_idx, (uint32_t)spi_rx_buffer, sizeof(spi_rx_buffer));
                hif_com_dma_transfer_start(rx_channel_idx);
            }
            //LOG_IO(0x1000, 11, 0, rx_block_size+1);
            if(rx_block_size) {
                LOG_IO(0x1000, 11, 1, 0);
                hif_com_buffer_dcache_invalid((uint32_t)spi_rx_buffer, rx_block_size);
                hif_com_data_recv();
                LOG_IO(0x1000, 11, 0, 0);
            }
#endif
        }
    }
}


#if (CONFIG_PM)
void hif_com_dma_rx_wakeup_callback(HAL_Dev_t * pDevice, void *arg)
{
    hif_com_dma_transfer_stop(rx_channel_idx);
    hif_com_dma_block_config(rx_channel_idx, (uint32_t)spi_rx_buffer, sizeof(spi_rx_buffer));
    hif_com_dma_transfer_start(rx_channel_idx);
}
#endif


#if (CONFIG_HIF_DEVICE_WAKEUP_SPI)

static void hif_spi_callback(HAL_Dev_t * pDevice, void *arg)
{
    if (hif_spi_pm_wakeup_callback != NULL) {
        hif_spi_pm_wakeup_callback(0);
    }
}

int hif_com_spi_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg)
{
    uint8_t enable = 1;
    HAL_Callback_t temp_cb;

    if ((pm_cb == NULL) || (pcfg == NULL)) {
        return HIF_ERRCODE_INVALID_PARAM;
    }
    if(hif_spi_instance) {
        temp_cb.cb = hif_spi_callback;
        temp_cb.arg = NULL;
        HAL_SPI_ExtControl(hif_spi_instance, SPI_PARAM_WAKEUP_THRESHOLD, pcfg);
        HAL_SPI_ExtControl(hif_spi_instance, SPI_PARAM_WAKEUP_CALLBACK, &temp_cb);
        HAL_SPI_ExtControl(hif_spi_instance, SPI_PARAM_WAKEUP_CTRL, &enable);
        hif_spi_pm_wakeup_callback = pm_cb;
    } else {
        return HIF_ERRCODE_NOT_READY;
    }

    return HIF_ERRCODE_SUCCESS;
}

int hif_com_spi_pm_deinit(void)
{
    uint8_t enable = 0;

    if(hif_spi_instance) {
        HAL_SPI_ExtControl(hif_spi_instance, SPI_PARAM_WAKEUP_CALLBACK, NULL);
        HAL_SPI_ExtControl(hif_spi_instance, SPI_PARAM_WAKEUP_CTRL, &enable);
    } else {
        return HIF_ERRCODE_NOT_READY;
    }

    return HIF_ERRCODE_SUCCESS;
}

#endif /* HIF_DEVICE_WAKEUP_SPI_ENABLE */

#endif /* CONFIG_HIF_DEVICE_COM_SPI */
#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
