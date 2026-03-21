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

#include "hif_com_dma.h"
#include "hif_config.h"
#include "hif_com.h"
#include "core_rv32.h"

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


#if (CONFIG_HIF_SEND_DMA)

static HAL_Dev_t *hifDmaDev = NULL;
static uint8_t dma_channel_id[2] = {0x00};

_Alignas(32) DMA_NodeConf_t dma_item[CONFIG_DMA_ITEM_COUNT] = {0x00};
DMA_NodeConf_t dma_item_end;
DMA_NodeConf_t *item_head = NULL;
DMA_NodeConf_t *item_head_tcb = NULL;


uint8_t block_put = 0;
uint8_t block_get = 0;
uint8_t dma_init = 0;

#define dma_queue_empty(get, put)       ((int8_t)((get) - (put)) >= 0)
#define dma_queue_item_cnt(put, get)    ((int8_t)((put) - (get)))

static void hif_com_dma_done_irq_callback(HAL_Dev_t * pDevice, void *arg);
static void hif_com_dma_err_irq_callback(HAL_Dev_t * pDevice, void *arg);
extern void hif_com_dma_rx_wakeup_callback(HAL_Dev_t * pDevice, void *arg);

int hif_com_tx_dma_init(dma_trigger_e dmaTrigger)
{
    int status = 0;
    HAL_Callback_t temp_cb;
    DMA_ChannelConf_t hifDmaCfg;

    if(dma_init) {
        return HIF_ERRCODE_NOT_READY;
    }

    dma_init = 1;

    hifDmaDev = HAL_DMA_Init(DMA0_ID);
    if (hifDmaDev == NULL) {
        printf("dma init fail...\n");
        return HIF_ERRCODE_NOT_READY;
    }

    hifDmaCfg.direction        = DMA_DIRECTION_MEM2PER;
    hifDmaCfg.priority        = DMA_PRIORITY_7;
    hifDmaCfg.srcBrustNum     = DMA_BURSTNUM_4;
    hifDmaCfg.dstBrustnum     = DMA_BURSTNUM_4;
    hifDmaCfg.srcWidth        = DMA_BITWIDTH_8;
    hifDmaCfg.dstWidth        = DMA_BITWIDTH_8;
    hifDmaCfg.dstHandShake    = dmaTrigger;
    hifDmaCfg.srcIncreMode    = DMA_ADDR_INCREMENT;
    hifDmaCfg.dstIncreMode    = DMA_ADDR_FIXED;
    hifDmaCfg.srcEnReload     = 0;
    hifDmaCfg.dstEnReload     = 0;
    hifDmaCfg.srcEnStatus     = 0;
    hifDmaCfg.dstEnStatus     = 0;

    status = HAL_DMA_Open(hifDmaDev, &hifDmaCfg);
    if (status < 0) {
        printf("dma open channel fail (%d)\n", status);
        return HIF_ERRCODE_NOT_READY;
    } else {
        dma_channel_id[tx_channel_idx] = status;
    }

    temp_cb.arg = NULL;
    temp_cb.cb = hif_com_dma_done_irq_callback;

    status = HAL_DMA_RegisterIRQ(hifDmaDev, dma_channel_id[tx_channel_idx], DMA_EVENT_DONE, &temp_cb);
    if (status != HAL_STATUS_OK) {
        printf("dma register channel irq fail (%d)\n", status);
    }
    status = HAL_DMA_EnableIRQ(hifDmaDev, dma_channel_id[tx_channel_idx], DMA_EVENT_DONE);
    if (status != HAL_STATUS_OK) {
        printf("dma enable channel irq fail (%d)\n", status);
    }

    temp_cb.arg = NULL;
    temp_cb.cb = hif_com_dma_err_irq_callback;

    status = HAL_DMA_RegisterIRQ(hifDmaDev, dma_channel_id[tx_channel_idx], DMA_EVENT_ERR, &temp_cb);
    if (status != HAL_STATUS_OK) {
        printf("dma register channel irq fail (%d)\n", status);
    }

    status = HAL_DMA_EnableIRQ(hifDmaDev, dma_channel_id[tx_channel_idx], DMA_EVENT_ERR);
    if (status != HAL_STATUS_OK) {
        printf("dma enable channel irq fail (%d)\n", status);
    }

    return HIF_ERRCODE_SUCCESS;
}

int hif_com_rx_dma_init(dma_trigger_e dmaTrigger)
{
    int status = 0;
    DMA_ChannelConf_t hifDmaCfg;

    hifDmaCfg.direction        = DMA_DIRECTION_PER2MEM;
    hifDmaCfg.priority        = DMA_PRIORITY_7;
    hifDmaCfg.srcBrustNum     = DMA_BURSTNUM_1;
    hifDmaCfg.dstBrustnum     = DMA_BURSTNUM_1;
    hifDmaCfg.srcWidth        = DMA_BITWIDTH_8;
    hifDmaCfg.dstWidth        = DMA_BITWIDTH_8;
    hifDmaCfg.srcHandShake    = dmaTrigger;
    hifDmaCfg.srcIncreMode    = DMA_ADDR_FIXED;
    hifDmaCfg.dstIncreMode    = DMA_ADDR_INCREMENT;
    hifDmaCfg.srcEnReload     = 0;
    hifDmaCfg.dstEnReload     = 0;
    hifDmaCfg.srcEnStatus     = 0;
    hifDmaCfg.dstEnStatus     = 0;

    status = HAL_DMA_Open(hifDmaDev, &hifDmaCfg);
    if (status < 0) {
        printf("dma open channel fail (%d)\n", status);
        return HIF_ERRCODE_NOT_READY;
    } else {
        dma_channel_id[rx_channel_idx] = status;
    }

#if (CONFIG_PM)
    HAL_Callback_t temp_cb;
    temp_cb.cb = hif_com_dma_rx_wakeup_callback;
    temp_cb.arg = NULL;
    HAL_DMA_ExtendControl(hifDmaDev, dma_channel_id[rx_channel_idx], DMA_EXTATTR_RESUME_CALLBACK, &temp_cb);
#endif

    return HIF_ERRCODE_SUCCESS;
}


int hif_com_dma_block_config(dma_channel_id_e dma_channel, uint32_t dest_addr, uint32_t size)
{
    int status = 0;

    DMA_BlockConf_t DMA_BlockConf = {0x00};
    DMA_BlockConf.srcAddr = DMA_QSPI_TXRX_ADDR;
    DMA_BlockConf.dstAddr = dest_addr;
    DMA_BlockConf.blockSize = size;

    status = HAL_DMA_ConfigBlockTranfer(hifDmaDev, dma_channel_id[dma_channel], &DMA_BlockConf);
	return status;
}


static void hif_com_node_dcache_clean(DMA_NodeConf_t *dma_item)
{
    csi_dcache_clean_range((unsigned long *)dma_item->srcAddr, dma_item->blockSize);
    csi_dcache_clean_range((unsigned long *)dma_item, sizeof(DMA_NodeConf_t));
}

__hif_sram_text DMA_NodeConf_t *hif_com_dma_list_node_add(dma_node_param_t dma_node_param)
{
    if (dma_queue_item_cnt(block_put, block_get) >= CONFIG_DMA_ITEM_COUNT && !dma_node_param.null_tail) {
        return NULL;
    }
    DMA_NodeConf_t *dma_node = NULL;

    if(dma_node_param.null_tail) {
        dma_node = &dma_item_end;
    } else {
        dma_node = &dma_item[block_put & CONFIG_DMA_ITEM_MASK];
    }

    //memset(dma_node, 0, sizeof(DMA_NodeConf_t));
    dma_node->pnextNode = 0;

    dma_node->srcAddr = dma_node_param.src_addr;
    dma_node->dstAddr = dma_node_param.dest_addr;
    dma_node->blockSize = dma_node_param.block_size;
    HAL_DMA_InitNodeCtrl(hifDmaDev, dma_channel_id[tx_channel_idx], dma_node, dma_node_param.int_en);

    if(dma_node_param.head_idx) {
        item_head_tcb = dma_node;
    }

    if(item_head == NULL && !dma_node_param.null_tail) {
        item_head = dma_node;
    } else {
        DMA_NodeConf_t *pdma_node = &dma_item[(block_put - 1) & CONFIG_DMA_ITEM_MASK];
        pdma_node->pnextNode = dma_node;
        csi_dcache_clean_range((unsigned long *)pdma_node, sizeof(DMA_NodeConf_t));
    }

    hif_com_node_dcache_clean(dma_node);
    if(!dma_node_param.null_tail) {
        block_put ++;
    }

    return dma_node;
}

__hif_sram_text static void hif_com_node_dcache_invalid(DMA_NodeConf_t *dma_item)
{
    csi_dcache_invalid_range((unsigned long *)dma_item, sizeof(DMA_NodeConf_t));
}

__hif_sram_text void hif_com_buffer_dcache_invalid(uint32_t addr, uint32_t size)
{
    csi_dcache_invalid_range((unsigned long *)addr, size);
}

bool burst_end_race;
__hif_sram_text bool hif_com_dma_list_node_add_allow(void)
{
    bool allow = false;
    if(item_head_tcb != NULL) {
        hif_com_node_dcache_invalid(item_head_tcb);
        if(item_head_tcb->nodeCtrl1 & 0x01) {
            allow = false;
        } else {
            allow = true;
        }
    } else {
    	/* burst period read racing with DMA Done irq */
        allow = !burst_end_race;

    }
    return allow;
}

__hif_sram_text void hif_com_dma_reset_racing(void)
{
	burst_end_race = false;
}

__hif_sram_text bool hif_com_is_burst_end_race(void)
{
    return burst_end_race;
}

__hif_sram_text bool hif_com_dma_free_allow(hif_queue_item_t *hif_queue_item)
{
    DMA_NodeConf_t *dma_node = (DMA_NodeConf_t *)hif_queue_item->node_end;
    hif_com_node_dcache_invalid(dma_node);
    if((dma_node->nodeCtrl1 & 0x01) || (block_put == block_get)) {
        return true;
    } else {
        return false;
    }
}

__hif_sram_text int hif_com_dma_list_free(hif_queue_item_t *hif_queue_item)
{
    DMA_NodeConf_t *dma_head = item_head;
    DMA_NodeConf_t *dma_node = (DMA_NodeConf_t *)hif_queue_item->node_end;

    if(block_put == block_get) {
        return 0;
    }

    while(dma_head && dma_head != dma_node) {
        block_get ++;
        dma_head = dma_head->pnextNode;
    }
    if(dma_head) {
        block_get ++;
        dma_head = dma_head->pnextNode;
    }

    item_head = dma_head;

    return 0;
}

__hif_sram_text void hif_com_data_dma_list_config(void)
{
    HAL_DMA_ConfigListTranfer(hifDmaDev, dma_channel_id[tx_channel_idx], item_head);
}

__hif_sram_text uint32_t hif_com_dma_block_size_get(dma_channel_id_e dma_channel)
{
    return HAL_DMA_BlockSizeGet(hifDmaDev, dma_channel_id[dma_channel]);
}

__hif_sram_text int hif_com_dma_transfer_start(dma_channel_id_e dma_channel)
{
    int ret  = 0;

    ret = HAL_DMA_StartTransfer(hifDmaDev, dma_channel_id[dma_channel]);
    if(ret) {
        printf("dma start fail\r\n");
    }

    return ret;
}

__hif_sram_text int hif_com_dma_transfer_stop(dma_channel_id_e dma_channel)
{
    int ret = 0;
    ret = HAL_DMA_AbortTransfer(hifDmaDev, dma_channel_id[dma_channel]);
    if(ret) {
        printf("dma stop fail\r\n");
    }
    return ret;
}

extern bool dma_send_start;
__hif_sram_text void hif_com_dma_param_reset(void)
{
    if(item_head) {
        item_head = NULL;
    }
    item_head_tcb = NULL;
    memset(dma_item, 0, sizeof(DMA_NodeConf_t) * CONFIG_DMA_ITEM_COUNT);
    block_put = 0;
    block_get = 0;
}
extern void hif_dma_send_done(void);
__hif_sram_text static void hif_com_dma_done_irq_callback(HAL_Dev_t * pDevice, void *arg)
{
    LOG_IO(0x1000, 11, 0, 5);
    hif_com_dma_transfer_stop(tx_channel_idx);
    dma_send_start = false;
	hif_com_data_send_done();
    hif_com_dma_param_reset();
	burst_end_race = true;
    //hif_com_data_output_done();
}

static void hif_com_dma_err_irq_callback(HAL_Dev_t * pDevice, void *arg)
{
    hif_com_dma_transfer_stop(tx_channel_idx);
    hif_com_dma_param_reset();
}


int hif_com_dma_deinit(void)
{
    int status = 0;

    if(dma_init == 0) {
        return HIF_ERRCODE_NOT_READY;
    }

    dma_init = 0;
    status = HAL_DMA_Close(hifDmaDev, dma_channel_id[rx_channel_idx]);
    if (status != HAL_STATUS_OK) {
        printf("dma close fail (%d)\n", status);
    }


    status = HAL_DMA_Close(hifDmaDev, dma_channel_id[tx_channel_idx]);
    if (status != HAL_STATUS_OK) {
        printf("dma close fail (%d)\n", status);
    }

    status  =  HAL_DMA_DeInit(hifDmaDev);
    if (status != HAL_STATUS_OK) {
        printf("dma deinit fail (%d)\n", status);
    }

    return 0;
}

#endif /*CONFIG_HIF_SEND_DMA*/


#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
