/**
 ******************************************************************************
 * @file    hif.c
 * @brief   hif define.
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
#include "hif.h"
#include "hif_checksum.h"
#include "hif_mem.h"
#include "hif_com.h"
#include "hif_io.h"
#include "hif_pm.h"
#include "hif_com_dma.h"
#if (CONFIG_HIF == 1)

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
typedef struct __hif_queue {
	hif_queue_item_t *next;
	hif_queue_item_t *tail;
} HIF_QUEUE;

struct __hif_ctrl {
    /* Command tx/rx */
    uint8_t         cmd_event;
    uint8_t         cmd_err;
    uint16_t        cmd_len;
    uint8_t        *cmd_buf;

    /* Message queue */
	uint32_t        burst_period;
	hif_queue_item_t *queue_item_pool;
    hif_queue_item_t tail_null;
	HIF_QUEUE       put;
	HIF_QUEUE       proc;
    HIF_QUEUE       ack_pending;
	HIF_QUEUE       retry;
    HIF_QUEUE       free_pending;
	HIF_QUEUE       free;

    /* Thead Process */
    OSI_Semaphore_t    oper_sem;
    OSI_Semaphore_t    proc_sem;
    OSI_Semaphore_t    exit_sem;
    //struct k_thread proc_thread;
    OSI_Thread_t host_interface_task_handle;

    uint8_t     init_once; /* for some resource just init once */
    uint8_t     inited;
    uint8_t     terminate;

    uint8_t        startup_state;

    uint8_t        notify_mode; /* active and passivity */
    uint8_t        notify_enable;

    uint8_t        notify_allow;
        #define HIF_PM_STATE_SLEEP                0
        #define HIF_PM_STATE_WAKEUP                1
        #define HIF_PM_STATE_ACTIVE                2
        #define HIF_PM_STATE_SUSPEND            3
    uint8_t        device_state;
        #define HIF_PM_DEV_SLF_WAKE                0
        #define HIF_PM_DEV_HST_WAKE                1
    uint8_t        device_wake_mode;
    uint8_t        host_state;
    uint32_t    host_wake_time;
    uint32_t    frame_link_time;

    uint8_t        debounce_en;
    uint32_t    debounce_cnt;
    uint32_t    debounce_time;

    uint8_t        data_wait_send;

    HIF_InitParam_t     cfgParam;
}g_hif_ctrl;

typedef struct __hif_ctrl  HIF_CTRL;

static bool null_tail_enable = false;

static uint32_t hif_task_last_period = HIF_THREAD_CHECK_PERIOD;

static void hif_set_queue_event(void);

#ifndef BIT
#define BIT(n)                      (1U<<(n))
#endif

static uint8_t hif_com_tx_seq;
uint8_t tail_null_buffer[HIF_HEAD_LEN] = {0xA5, 0x4B, 0x03, 0x0C, 0x00, 0x00};

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define HOST_STATE_READY            (1U<<0)
#define HOST_STATE_WAKEUP           (1U<<1)

#define queue_empty(q)              ((q)->next == NULL)
#define queue_item_cnt(put, get)    ((int16_t)((put) - (get)))

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
void hif_device_wakeup_req(void);
void hif_device_suspend_req(void);
static void hif_PM_WakeHost(void);
static void hif_PM_SleepHost(void);
hif_queue_item_t *HIF_GetQueueItem(HIF_QUEUE *queue);
void HIF_PutQueueItem(HIF_QUEUE *queue, hif_queue_item_t *item, bool insert_head);
void HIF_AddQueueList(HIF_QUEUE *queue, HIF_QUEUE *queue_add, bool insert_head);


/* Exported functions.
 * ----------------------------------------------------------------------------
 */

void hif_data_io_set(void)
{
    if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
        hif_io_out_set(g_hif_ctrl.cfgParam.notifyIoNum, g_hif_ctrl.cfgParam.notifyIoLevel);
    }
}

void hif_data_io_clr(void)
{
    if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
        hif_io_out_set(g_hif_ctrl.cfgParam.notifyIoNum, g_hif_ctrl.cfgParam.notifyIoLevel ^ 1);
    }
}

static void hif_msg_checksum32_pack(hif_queue_item_t *hif_queue_item)
{
    uint32_t offset = 0;
    HIF_MsgHdr_t *msg = (HIF_MsgHdr_t *)&hif_queue_item->hdr_byte[HIF_PHY_HEAD_LEN];
    msg->seq = (hif_com_tx_seq ++) & 0x07;

    if(msg->flag & HIF_MSG_FLAG_CHECK_BIT) {
        hif_queue_item->check_len = HIF_CHKEC_LEN;
        payload_node_t *payload_node =  hif_queue_item->payload;
        uint32_t checksum32 = *(uint32_t *)msg;
        while(payload_node) {
            checksum32 += checksum32_calc(payload_node->payload, payload_node->data_len, &offset);
            payload_node = payload_node->next;
        }
        checksum32 =~ checksum32;
        memcpy(hif_queue_item->checksum, &checksum32, HIF_CHKEC_LEN);
    }
}


int HIF_MsgResp(HIF_MsgHdr_t *msg, uint16_t data_len, uint8_t status)
{
    int ret = HIF_ERRCODE_SUCCESS;
    HIF_LOG_IO(HIF_LOGIO_NUM_1, 0, 2);

    if (msg->flag & HIF_MSG_FLAG_REQ_BIT) {
        HIF_MsgGenAck_t *ack = (HIF_MsgGenAck_t *)(msg + 1);
        ack->status = status;
        msg->type   = HIF_MSG_TYPE_TO_HOST;
        msg->flag  &= ~HIF_MSG_FLAG_REQ_BIT;
        msg->frag = HIF_MSG_FRAG_INVALID;
        msg->length = sizeof(HIF_MsgGenAck_t) + data_len;

		hif_queue_item_t *hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.free);
        if (hif_queue_item == NULL) {
            return HIF_ERRCODE_NO_BUFFER;
        }
        payload_node_t *payload_node =  hif_mem_malloc();
        if(payload_node == NULL) {
			HIF_PutQueueItem(&g_hif_ctrl.free, hif_queue_item, true);
            return HIF_ERRCODE_NO_BUFFER;
        }
        payload_node->payload = (uint8_t *)ack;
        payload_node->data_len = msg->length;
		hif_queue_item->payload = payload_node;
        hif_queue_item->send_cb = NULL;

        memcpy(&hif_queue_item->hdr_byte[HIF_PHY_HEAD_LEN], msg, sizeof(HIF_MsgHdr_t));
        hif_msg_checksum32_pack(hif_queue_item);
#if (CONFIG_HIF_SEND_DMA)
        hif_com_dma_reset_racing();
        ret = hif_com_msg_send(hif_queue_item, true);
		if (ret) {
            hif_mem_free_item(hif_queue_item);
            HIF_PutQueueItem(&g_hif_ctrl.free, hif_queue_item, true);
        } else {
            HIF_PutQueueItem(&g_hif_ctrl.proc, hif_queue_item, false);
        }
#else
        ret = hif_com_msg_send(hif_queue_item, true);
        HIF_PutQueueItem(&g_hif_ctrl.free, hif_queue_item, true);
#endif
    }

    return ret;
}

static hif_queue_item_t* hif_msg_queue_send(HIF_MsgHdr_t *msg, void *payload, HIF_MsgTrans_Callback sent_cb)
{
    if (!g_hif_ctrl.inited || g_hif_ctrl.terminate) {
        return NULL;
    }
	hif_queue_item_t *hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.free);
	if (hif_queue_item == NULL) {
		return NULL;
	}
    msg->flag |= HIF_MSG_FLAG_MORE_DATA_BIT;
    memcpy(&hif_queue_item->hdr_byte[HIF_PHY_HEAD_LEN], msg, sizeof(HIF_MsgHdr_t));

    if(msg->length != 0 && payload != NULL) {
        payload_node_t *payload_node =  hif_mem_malloc();
        if(payload_node == NULL) {
			HIF_PutQueueItem(&g_hif_ctrl.free, hif_queue_item, true);
            return NULL;
        }
        payload_node->payload = (uint8_t *)payload;
        payload_node->data_len = msg->length;
		hif_queue_item->payload = payload_node;
    }
    hif_queue_item->send_cb = (void *)sent_cb;

    hif_msg_checksum32_pack(hif_queue_item);
    HIF_PutQueueItem(&g_hif_ctrl.put, hif_queue_item, false);
    hif_set_queue_event();
    return hif_queue_item;
}

static void hif_msg_send_finish_cb(hif_queue_item_t *hif_queue_item, int8_t status)
{
	if (hif_queue_item) {
		OSI_SemaphoreRelease(&hif_queue_item->block_sem);
	}
}

#if (CONFIG_HIF_SEND_SPLIT)
static int hif_msg_split_handler(HIF_MsgHdr_t *msg, void *payload, uint16_t length, HIF_MsgTrans_Callback sent_cb)
{
    uint8_t *pBuff = NULL;
    uint8_t *pData = (uint8_t *)payload;
    uint16_t offset = 0;
    uint16_t dataLen = 0;
    int queue_id = 0;

    if(length){
        while(length){
            if(length > g_hif_ctrl.cfgParam.splitSize) {
                dataLen = g_hif_ctrl.cfgParam.splitSize;
            } else {
                dataLen = length;
            }
            pBuff = &pData[offset];
            msg->length = dataLen;
            offset += msg->length;
            length = length - msg->length;
            if(length) {
                msg->frag = HIF_MSG_FRAG_MASK;
                queue_id = hif_msg_queue_send(msg, pBuff, NULL);
            } else {
                msg->frag = HIF_MSG_FRAG_INVALID;
                queue_id = hif_msg_queue_send(msg, pBuff, sent_cb);
                if(sent_cb == hif_msg_send_finish_cb) {
                    OSI_SemaphoreWait(&g_hif_ctrl.hif_queue[queue_id & HIF_QUEUE_MASK].block_sem, OSI_WAIT_FOREVER);
                }
            }
        }
    } else {
        pBuff = pData;
        msg->frag = HIF_MSG_FRAG_INVALID;
        msg->length = 0;
        queue_id = hif_msg_queue_send(msg, pBuff, sent_cb);
        if(sent_cb == hif_msg_send_finish_cb) {
            OSI_SemaphoreWait(&g_hif_ctrl.hif_queue[queue_id & HIF_QUEUE_MASK].block_sem, OSI_WAIT_FOREVER);
        }
    }
    return HIF_ERRCODE_SUCCESS;
}
#endif


int HIF_MsgReport(HIF_MsgHdr_t *msg, void *data, HIF_MsgTrans_Callback sentCb)
{
    int ret = HIF_ERRCODE_SUCCESS;
    HIF_LOG_IO(HIF_LOGIO_NUM_1, 0, 3);
#if (CONFIG_HIF_SEND_SPLIT)
    if ((g_hif_ctrl.cfgParam.splitEna) && (g_hif_ctrl.cfgParam.splitSize)) {
        if(sentCb) {
            return hif_msg_split_handler(msg, data, msg->length, sentCb);
        } else {
            return hif_msg_split_handler(msg, data, msg->length, &hif_msg_send_finish_cb);
        }
    }
#endif
    if(msg->length > HIF_MSG_SEND_MAX_PAYLOAD) {
        ret = HIF_ERRCODE_INVALID_PARAM;
    } else {
		hif_queue_item_t *item;
		if(sentCb) {
			item = hif_msg_queue_send(msg, data, sentCb);
		} else {
			item = hif_msg_queue_send(msg, data, &hif_msg_send_finish_cb);
			if (item) {
				OSI_SemaphoreWait(&item->block_sem, OSI_WAIT_FOREVER);
			}
		}
		ret = (item == NULL ? HIF_ERRCODE_NO_BUFFER : HIF_ERRCODE_SUCCESS);
    }
    return ret;
}

payload_node_t *HIF_MsgReport_ListAddData(payload_node_t * payload_head, void *data, uint16_t len)
{
    if(len != 0 && data != NULL) {
        payload_node_t *payload_node =  hif_mem_malloc();
        if(payload_node == NULL) {
            return NULL;
        }
        payload_node->payload = (uint8_t *)data;
        payload_node->data_len = len;
        if(payload_head) {
            hif_mem_put(payload_node, payload_head);
            return payload_head;
        } else {
            return payload_node;
        }
    } else {
        return NULL;
    }
}

#if (CONFIG_HIF_SEND_SPLIT)
static int hif_msg_list_split_handler(HIF_MsgHdr_t *msg, payload_node_t * payload_head, HIF_MsgTrans_Callback sent_cb)
{
    uint16_t offset = 0;
    uint16_t dataLen = 0;

    payload_node_t *node = payload_head;
    while(node) {
        if(dataLen + node->data_len > g_hif_ctrl.cfgParam.splitSize) {

        }
        dataLen += node->data_len;
        if(dataLen >= )
        node = node->next;
    }

}
#endif

int HIF_MsgReport_ListStart(HIF_MsgHdr_t *msg, payload_node_t * payload_head, HIF_MsgTrans_Callback sentCb)
{
    HIF_LOG_IO(HIF_LOGIO_NUM_1, 0, 1);
    if (!g_hif_ctrl.inited || g_hif_ctrl.terminate) {
        return HIF_ERRCODE_NOT_READY;
    }
#if (CONFIG_HIF_SEND_SPLIT)
	if ((g_hif_ctrl.cfgParam.splitEna) && (g_hif_ctrl.cfgParam.splitSize)) {
		int ret = hif_msg_list_split_handler(msg, payload_head, sentCb);
		return ret;
	}
#endif
    if(payload_head == NULL) {
        return HIF_ERRCODE_INVALID_PARAM;
    }

	msg->length = 0;
	payload_node_t *payload_node = payload_head;
	while(payload_node) {
		msg->length += payload_node->data_len;
		payload_node = payload_node->next;
	}
	if(msg->length > HIF_MSG_SEND_MAX_PAYLOAD) {
		return HIF_ERRCODE_INVALID_PARAM;
	}

	hif_queue_item_t *hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.free);
	if (hif_queue_item) {
		hif_queue_item->payload = payload_head;
		hif_queue_item->send_cb = (void *)(sentCb == NULL ? &hif_msg_send_finish_cb : sentCb);

		msg->flag |= HIF_MSG_FLAG_MORE_DATA_BIT;
		memcpy(&hif_queue_item->hdr_byte[HIF_PHY_HEAD_LEN], msg, sizeof(HIF_MsgHdr_t));
		hif_msg_checksum32_pack(hif_queue_item);
        HIF_PutQueueItem(&g_hif_ctrl.put, hif_queue_item, false);
		hif_set_queue_event();
		if (sentCb == NULL) {
			OSI_SemaphoreWait(&hif_queue_item->block_sem, OSI_WAIT_FOREVER);
		}
		return HIF_ERRCODE_SUCCESS;
	} else {
        hif_mem_free(payload_head);
		return HIF_ERRCODE_NO_BUFFER;
	}
}

static int hif_Msg_ReportSend(void)
{
    int8_t tx_status = 0;
    hif_queue_item_t *hif_queue_item = NULL;
#if (CONFIG_HIF_SEND_DMA)
	hif_com_dma_reset_racing();

    if(g_hif_ctrl.cfgParam.simplex) {
        while(!queue_empty(&g_hif_ctrl.put)) {
            hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.put);
            tx_status = hif_com_msg_send(hif_queue_item, false);
            if(tx_status == HIF_ERRCODE_ABORTED) {
                HIF_PutQueueItem(&g_hif_ctrl.put, hif_queue_item, true);
                break;
            }
            HIF_PutQueueItem(&g_hif_ctrl.proc, hif_queue_item, false);
        }
        tx_status = hif_com_msg_send(&g_hif_ctrl.tail_null, true);
    } else {
		uint32_t burst_num = ((g_hif_ctrl.cfgParam.comType == HIF_COM_TYPE_UART) ?
			                 1 : g_hif_ctrl.burst_period);
        while(burst_num && !queue_empty(&g_hif_ctrl.retry)) {
            hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.retry);
            if (burst_num == 1 && queue_empty(&g_hif_ctrl.retry)) {
                if (hif_queue_item->hdr_byte[2] & BIT(5)) {
                    hif_queue_item->hdr_byte[2] &= ~ BIT(5);
                    (*(uint32_t *)&hif_queue_item->checksum[0]) += BIT(5);
                }
            }
            tx_status = hif_com_msg_send(hif_queue_item, burst_num == 1);
            if(tx_status == HIF_ERRCODE_ABORTED) {
                HIF_PutQueueItem(&g_hif_ctrl.retry, hif_queue_item, true);
				burst_num = 0;
                break;
            }
            HIF_PutQueueItem(&g_hif_ctrl.proc, hif_queue_item, false);
            burst_num -- ;
        }
        while(burst_num && !queue_empty(&g_hif_ctrl.put)) {
            hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.put);
            if (burst_num == 1 && queue_empty(&g_hif_ctrl.put)) {
                if (hif_queue_item->hdr_byte[2] & BIT(5)) {
                    hif_queue_item->hdr_byte[2] &= ~ BIT(5);
                    (*(uint32_t *)&hif_queue_item->checksum[0]) += BIT(5);
                }
            }
            tx_status = hif_com_msg_send(hif_queue_item, burst_num == 1);
            if(tx_status == HIF_ERRCODE_ABORTED) {
                HIF_PutQueueItem(&g_hif_ctrl.put, hif_queue_item, true);
				burst_num = 0;
                break;
            }
            HIF_PutQueueItem(&g_hif_ctrl.proc, hif_queue_item, false);
            burst_num --;
        }
        if(burst_num) {
            tx_status = hif_com_msg_send(&g_hif_ctrl.tail_null, true);
            null_tail_enable = true;
        }
        if(tx_status == HIF_ERRCODE_ABORTED) {
            burst_num = 0;
        }
		g_hif_ctrl.burst_period = burst_num;
     }
#else
    if(!queue_empty(&g_hif_ctrl.put)) {
        hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.put);
        hif_com_msg_send(hif_queue_item, false);
        HIF_PutQueueItem(&g_hif_ctrl.free_pending, hif_queue_item, false);
    }
#endif
    return tx_status;
}

void hif_dma_send_done(void)
{
    if(null_tail_enable) {
        null_tail_enable = false;
        g_hif_ctrl.burst_period = 0;
    }
}

void hif_proc_queue_done(void)
{
#if (CONFIG_HIF_SEND_DMA)
    hif_queue_item_t *hif_queue_item = NULL;
    do{
        hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.proc);
        if(hif_queue_item) {
            if (hif_com_dma_free_allow(hif_queue_item)) {
    			if(queue_empty(&g_hif_ctrl.proc)) {
    				/* End burst_period if proc queue is empty */
                    g_hif_ctrl.burst_period = 0;
                }
                if (hif_queue_item->send_cb != NULL && g_hif_ctrl.cfgParam.retry_enable) {
    				HIF_PutQueueItem(&g_hif_ctrl.ack_pending, hif_queue_item, false);
                    LOG_IO(0x1000, 11, 0, 4);
    			} else {
    				HIF_PutQueueItem(&g_hif_ctrl.free_pending, hif_queue_item, false);
                    LOG_IO(0x1000, 11, 0, 4);
    			}
        		hif_com_dma_list_free(hif_queue_item);
                OSI_SemaphoreRelease(&g_hif_ctrl.proc_sem);
            } else {
                HIF_PutQueueItem(&g_hif_ctrl.proc, hif_queue_item, true);
                break;
            }
        }
    } while(hif_queue_item);
#else

#endif
}

bool hif_msg_tx_queue_free(void)
{
    bool free_state = false;
	do {
	    hif_queue_item_t *hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.free_pending);
		if (hif_queue_item) {
			HIF_MsgTrans_Callback sent_cb = hif_queue_item->send_cb;
            LOG_IO(0x1000, 11, 0, 3);
			if(sent_cb) {
				sent_cb(hif_queue_item, HIF_ERRCODE_SUCCESS);
			}
			hif_mem_free_item(hif_queue_item);
			HIF_PutQueueItem(&g_hif_ctrl.free, hif_queue_item, false);
#if CONFIG_PM
            free_state = true;
#endif
		} else {
			break;
		}
	} while (true);

    return free_state;
}

void hif_msg_queue_flush(HIF_QUEUE * queue)
{
    do {
        hif_queue_item_t *hif_queue_item = HIF_GetQueueItem(queue);
		if (hif_queue_item) {
	        HIF_MsgTrans_Callback sent_cb = hif_queue_item->send_cb;
	        if(sent_cb) {
	            sent_cb(hif_queue_item, HIF_ERRCODE_TIMEOUT);
	        }
	        hif_mem_free_item(hif_queue_item);
			HIF_PutQueueItem(&g_hif_ctrl.free, hif_queue_item, true);
		} else {
			break;
		}
    } while(true);

}
extern void hif_com_dma_param_reset(void);
void hif_Msg_ReportFlush(void)
{
	//TODO: flush proc and pending queue;
#if (CONFIG_HIF_SEND_DMA)
    hif_com_dma_transfer_stop(tx_channel_idx);
#endif
    hif_msg_queue_flush(&g_hif_ctrl.put);
    hif_msg_queue_flush(&g_hif_ctrl.ack_pending);
    hif_msg_queue_flush(&g_hif_ctrl.retry);
    hif_msg_queue_flush(&g_hif_ctrl.proc);
#if (CONFIG_HIF_SEND_DMA)
    hif_com_dma_param_reset();
#endif
}

void hif_wup_callback(uint32_t param)
{
    if (g_hif_ctrl.debounce_time > 0) {
        g_hif_ctrl.debounce_en = 1;
        g_hif_ctrl.debounce_cnt = 0;
    }

    hif_device_wakeup_req();
}

void hif_device_wakeup_req(void)
{
    g_hif_ctrl.device_wake_mode = HIF_PM_DEV_HST_WAKE;
    if (g_hif_ctrl.device_state == HIF_PM_STATE_SLEEP) {
        hif_pm_lock();
        g_hif_ctrl.device_state = HIF_PM_STATE_ACTIVE;
        OSI_SemaphoreRelease(&g_hif_ctrl.proc_sem);
    }
}

void hif_device_suspend_req(void)
{
    g_hif_ctrl.device_wake_mode = HIF_PM_DEV_SLF_WAKE;
    if (g_hif_ctrl.device_state == HIF_PM_STATE_ACTIVE) {
        g_hif_ctrl.device_state = HIF_PM_STATE_SUSPEND;
        OSI_SemaphoreRelease(&g_hif_ctrl.proc_sem);
    }
}

static int hif_deivce_sleep_check(void)
{
    int sleep_allow = 0;

    /* check host allow device to sleep */
    if (g_hif_ctrl.cfgParam.wakeType == HIF_WAKE_TYPE_COM) {
        /* allow sleep cmd */
        if (g_hif_ctrl.device_wake_mode == HIF_PM_DEV_SLF_WAKE) {
            sleep_allow = 1;
            g_hif_ctrl.device_state = HIF_PM_STATE_SUSPEND;
        } else {
            if (g_hif_ctrl.debounce_en) {
                g_hif_ctrl.debounce_cnt += hif_task_last_period;
                if (g_hif_ctrl.debounce_cnt >= g_hif_ctrl.debounce_time) {
                    g_hif_ctrl.device_state = HIF_PM_STATE_SUSPEND;
                    g_hif_ctrl.device_wake_mode = HIF_PM_DEV_SLF_WAKE;
                    sleep_allow = 1;
                }
            } else {
                g_hif_ctrl.device_state = HIF_PM_STATE_ACTIVE;
                g_hif_ctrl.device_wake_mode = HIF_PM_DEV_SLF_WAKE;
            }
        }

    } else if (g_hif_ctrl.cfgParam.wakeType == HIF_WAKE_TYPE_IO) {
        /* host wake device io */
        if (hif_io_in_value_get() != g_hif_ctrl.cfgParam.wakeIoLevel) {
            sleep_allow = 1;
            g_hif_ctrl.device_state = HIF_PM_STATE_SUSPEND;
        }

    } else {
        /* HIF_DEV_WAKE_MODE_DIS */
        sleep_allow = 1;
        g_hif_ctrl.device_state = HIF_PM_STATE_SUSPEND;
    }

    if(!queue_empty(&g_hif_ctrl.proc) || !queue_empty(&g_hif_ctrl.ack_pending) || !queue_empty(&g_hif_ctrl.free_pending)) {
        sleep_allow = 0;
    }
    if (g_hif_ctrl.data_wait_send) {
        sleep_allow = 0;
    }

    return sleep_allow;
}


static void hif_device_sleep(void)
{
    if ((g_hif_ctrl.device_state == HIF_PM_STATE_SUSPEND)
        || (g_hif_ctrl.device_state == HIF_PM_STATE_SLEEP)) {
        g_hif_ctrl.device_state = HIF_PM_STATE_SLEEP;
        g_hif_ctrl.debounce_en = 0;
        hif_pm_unlock();
    }
}

void hif_wakeup_request(void)
{
    hif_device_wakeup_req();
}

void hif_wakeup_release(void)
{
    hif_device_suspend_req();
}

void hif_host_state_change(uint8_t ready)
{
//    if (ready) {
//        g_hif_ctrl.host_state |= HOST_STATE_READY;
//    } else {
//        g_hif_ctrl.host_state = 0;
//    }
}

static int hif_Host_CheckReady(void)
{
    int hostReady = 0;

    if(g_hif_ctrl.cfgParam.simplex) {
        hostReady = 1;
        return hostReady;
    }

    if (g_hif_ctrl.device_state != HIF_PM_STATE_ACTIVE) {
        hif_pm_lock();
        g_hif_ctrl.device_state = HIF_PM_STATE_ACTIVE;
    }

	if (g_hif_ctrl.host_state == HIF_PM_STATE_ACTIVE) {
		if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
			if (g_hif_ctrl.frame_link_time < g_hif_ctrl.cfgParam.sendFrameTo) {
				hostReady = g_hif_ctrl.burst_period > 0;
				HIF_LOG_IO(HIF_LOGIO_NUM_2, 0, 6);
			} else {
				hif_Msg_ReportFlush();
				hif_PM_SleepHost();
				g_hif_ctrl.host_state = HIF_PM_STATE_SLEEP;
				HIF_LOG_IO(HIF_LOGIO_NUM_2, 0, 5);
			}
		}
	} else if (g_hif_ctrl.host_state == HIF_PM_STATE_SLEEP) {
        /* wake host */
        if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
            /* Set the wake up IO level to wake up the host.
             * start the timer, and wait for the host to send the poll cmd.
             */
            hif_PM_WakeHost();
            g_hif_ctrl.host_wake_time = 0;
            g_hif_ctrl.host_state = HIF_PM_STATE_WAKEUP;
            HIF_LOG_IO(HIF_LOGIO_NUM_2, 0, 1);
        } else if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_DLY) {
            /* Start a delay and send cmd.
             */
            g_hif_ctrl.host_wake_time = 0;
            g_hif_ctrl.host_state = HIF_PM_STATE_WAKEUP;
        } else {
            /* There is no need to wake up the host. send it directly. */
            hostReady = 1;
        }
    } else if (g_hif_ctrl.host_state == HIF_PM_STATE_WAKEUP) {
        g_hif_ctrl.host_wake_time += hif_task_last_period;

        if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
            if (g_hif_ctrl.host_wake_time >= g_hif_ctrl.cfgParam.sendFrameTo) {
                hif_Msg_ReportFlush();
                hif_PM_SleepHost();
                g_hif_ctrl.host_state = HIF_PM_STATE_SLEEP;
                HIF_LOG_IO(HIF_LOGIO_NUM_2, 0, 3);
            } else {
                HIF_LOG_IO(HIF_LOGIO_NUM_2, 0, 4);
            }
        } else if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_DLY) {
            if (g_hif_ctrl.host_wake_time >= g_hif_ctrl.cfgParam.sendDelay) {
                g_hif_ctrl.host_state = HIF_PM_STATE_ACTIVE;
                hostReady = 1;
            }
        }
    }
    return hostReady;
}


static void hif_PM_WakeHost(void)
{
    if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
        hif_io_out_set(g_hif_ctrl.cfgParam.notifyIoNum, g_hif_ctrl.cfgParam.notifyIoLevel);
    }
}

__hif_sram_text static void hif_PM_SleepHost(void)
{
    if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
        hif_io_out_set(g_hif_ctrl.cfgParam.notifyIoNum, g_hif_ctrl.cfgParam.notifyIoLevel ^ 1);
    }
}

__hif_sram_text void hif_Msg_SendDone(void)
{
    if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
        g_hif_ctrl.host_state = HIF_PM_STATE_SLEEP;
        g_hif_ctrl.data_wait_send = 0;
        hif_PM_SleepHost();
        hif_set_queue_event();
    }
}


static void hif_set_queue_event(void)
{
    OSI_SemaphoreRelease(&g_hif_ctrl.proc_sem);
}


int hif_msg_rx_busy(void)
{
    if (!g_hif_ctrl.cmd_event) {
        return 0;
    } else {
        return 1;
    }
}

__hif_sram_text int hif_msg_rx_report(uint8_t status, uint8_t *pbuff, uint16_t len)
{
    if (g_hif_ctrl.cmd_event) {
        return HIF_ERRCODE_NOT_READY;
    }

#if (HIF_HOST_WAKEUP_ACK_ENABLE)
    /* host should keep awake until timeout when initial cmd request */
    hif_host_state_change(true);
#endif
    if (g_hif_ctrl.cfgParam.notifyType == HIF_NOTIFY_TYPE_IO) {
        if (g_hif_ctrl.host_state == HIF_PM_STATE_WAKEUP) {
            hif_PM_SleepHost();
        }
    }

    g_hif_ctrl.cmd_err = status;
    g_hif_ctrl.cmd_buf = pbuff;
    g_hif_ctrl.cmd_len = len;
    g_hif_ctrl.cmd_event = true;

    OSI_SemaphoreRelease(&g_hif_ctrl.proc_sem);

    return HIF_ERRCODE_SUCCESS;
}

void hif_Msg_ReportEnable(HOST_POLL_ACK *poll_ack)
{
    if(g_hif_ctrl.cfgParam.retry_enable) {
		int retry = 0;
        LOG_IO(0x1000, 11, 0, 6);
        do {
            hif_queue_item_t *hif_queue_item = HIF_GetQueueItem(&g_hif_ctrl.ack_pending);
            if (hif_queue_item == NULL) {
                //printk("retry packet is not exsit, is free already?\n");
                break;
            }
            if (retry < poll_ack->ack_err_num) {
                /* Note: ensure ack seq is in order. */
                if (poll_ack->err_seq[retry] == ((hif_queue_item->hdr_byte[5]>>4) & 0x7)) {
                    HIF_PutQueueItem(&g_hif_ctrl.retry, hif_queue_item, false);
                    retry++;
                } else {
                    HIF_PutQueueItem(&g_hif_ctrl.free_pending, hif_queue_item, false);
                }
            } else {
                HIF_PutQueueItem(&g_hif_ctrl.free_pending, hif_queue_item, false);
            }
        } while (true);
    }
}

__attribute__((weak)) void cube_report_retry_handler(HOST_POLL_APP_CUBE *data)
{

}

extern void cube_report_retry_handler(HOST_POLL_APP_CUBE *data);
int msghdl_host_poll(HIF_MsgHdr_t *msg)
{
	if (msg->length) {
		HOST_POLL_ACK *poll_ack = ((HOST_POLL_ACK *)(msg + 1));
		g_hif_ctrl.burst_period = poll_ack->burst_period;
		if (poll_ack->burst_period == 0) {
			g_hif_ctrl.host_state = HIF_PM_STATE_SLEEP;
		} else {
			g_hif_ctrl.host_state = HIF_PM_STATE_ACTIVE;
		}
		if (poll_ack->poll_type == HOST_POLL_TYPE_ACK) {
			hif_Msg_ReportEnable(poll_ack);
		} else if(poll_ack->poll_type == HOST_POLL_TYPE_APP_CUBE) {
            cube_report_retry_handler((HOST_POLL_APP_CUBE *)poll_ack);
        }
	} else {
		g_hif_ctrl.host_state = HIF_PM_STATE_ACTIVE;
        g_hif_ctrl.frame_link_time = 0;
        g_hif_ctrl.burst_period = 1;
	}
    return 0;
}

__hif_sram_text void hif_Msg_ReportCancel(void)
{
    if (g_hif_ctrl.host_state == HIF_PM_STATE_WAKEUP) {
        hif_PM_SleepHost();
        g_hif_ctrl.host_state = HIF_PM_STATE_SLEEP;
    }
}

int HIF_InitStatusReport(uint8_t status)
{
    g_hif_ctrl.startup_state = status;
    return 0;
}

static int hif_msg_send_startup(void)
{
    HIF_MsgHdr_t msg_hdr = {0x00};
    HIF_MSG_HDL_PACK(msg_hdr, HIF_MSG_ID_STARTUP, 1);
    hif_msg_queue_send(&msg_hdr, (void *)&g_hif_ctrl.startup_state, NULL);
	return 0;
}

hif_queue_item_t *HIF_GetQueueItem(HIF_QUEUE *queue)
{
	unsigned long key = __lock_irq();
	hif_queue_item_t *item = queue->next;
    if(item) {
        queue->next = item->next;
        item->next = NULL;
        if(queue->next == NULL) {
            queue->tail = NULL;
        }
    }
	__unlock_irq((unsigned long)key);
	return item;
}

void HIF_PutQueueItem(HIF_QUEUE *queue, hif_queue_item_t *item, bool insert_head)
{
	if (item) {
		unsigned long key = __lock_irq();
		if (insert_head) { /* insert to head */
			item->next = queue->next;
			queue->next = item;
            if(item->next == NULL) {
                queue->tail = item;
            }
		} else { /* insert to tail */
			if (queue->next) {
				queue->tail->next = item;
				queue->tail = item;
			} else {
				queue->next = queue->tail = item;
			}
            item->next = NULL;
		}
		__unlock_irq((unsigned long)key);
	}
}

void HIF_AddQueueList(HIF_QUEUE *queue, HIF_QUEUE *queue_add, bool insert_head)
{
	if (queue_add && queue_add->next) {
		unsigned long key = __lock_irq();
		if (queue->next) { /* not NULL*/
			if (insert_head) { /* insert to head */
				queue_add->tail->next = queue->next;
				queue->next = queue_add->next;
			} else { /* insert to tail */
				queue->tail->next = queue_add->next;
				queue->tail = queue_add->tail;
			}
		} else { /* NULL queue */
			queue->next = queue_add->next;
			queue->tail = queue_add->tail;
		}
		__unlock_irq((unsigned long)key);
	}
}

void HIF_InitQueueItem(HIF_QUEUE *queue)
{
	unsigned long key = __lock_irq();
    queue->next = NULL;
    queue->tail = NULL;
	__unlock_irq((unsigned long)key);
}


static int hif_msg_queue_init(void)
{
    int ret = hif_mem_init();
	if (ret) {
		return ret;
	}

    HIF_InitQueueItem(&g_hif_ctrl.put);
    HIF_InitQueueItem(&g_hif_ctrl.proc);
    HIF_InitQueueItem(&g_hif_ctrl.ack_pending);
    HIF_InitQueueItem(&g_hif_ctrl.retry);
    HIF_InitQueueItem(&g_hif_ctrl.free_pending);
    HIF_InitQueueItem(&g_hif_ctrl.free);
	g_hif_ctrl.queue_item_pool = HIF_MemMalloc(HIF_QUEUE_SIZE * sizeof(hif_queue_item_t));
	if (g_hif_ctrl.queue_item_pool == NULL) {
		hif_mem_deinit();
		return OSI_STATUS_ERR_NOMEM;
	}
    for (int i = 0; i < HIF_QUEUE_SIZE; ++i) {
        OSI_SemaphoreCreate(&g_hif_ctrl.queue_item_pool[i].block_sem, 0, 1);
        HIF_PutQueueItem(&g_hif_ctrl.free, &g_hif_ctrl.queue_item_pool[i], false);
    }
    memcpy(g_hif_ctrl.tail_null.hdr_byte, tail_null_buffer, sizeof(tail_null_buffer));
    return 0;
}

static void hif_msg_queue_deinit(void)
{
    hif_Msg_ReportFlush();
    hif_mem_deinit();
	if (g_hif_ctrl.queue_item_pool) {
	    for (int i = 0; i < HIF_QUEUE_SIZE; ++i) {
	        OSI_SemaphoreDelete(&g_hif_ctrl.queue_item_pool[i].block_sem);
	    }
		HIF_MemFree(g_hif_ctrl.queue_item_pool);
		g_hif_ctrl.queue_item_pool = NULL;
	}
}


static int host_interface_startup(void)
{
    hif_msg_send_startup();

    //hif_watchdog_setup(0);

    return 0;
}

bool hif_operate_lock(void)
{
    if (!g_hif_ctrl.inited || g_hif_ctrl.terminate) {
        return false;
    }
    OSI_SemaphoreWait(&g_hif_ctrl.oper_sem, OSI_WAIT_FOREVER);
    return true;
}

void hif_operate_unlock(void)
{
    OSI_SemaphoreRelease(&g_hif_ctrl.oper_sem);
}


static void host_interface_task (void *arg)
{
//    host_interface_startup(); /* inform host of ready*/
    HIF_LOG_IO(HIF_LOGIO_NUM_0, 0, 3);
    HIF_LOG_IO(HIF_LOGIO_NUM_1, 0, 3);
    HIF_LOG_IO(HIF_LOGIO_NUM_2, 0, 3);
    bool check_period = false;
#if CONFIG_PM
    bool put_retry_empty = false;
#endif
    hif_task_last_period = HIF_THREAD_CHECK_PERIOD;
    /* task loop */
    do {
#if (HIF_WATCHDOG_ENABLE)
        hif_watchdog_feed(0);
#endif

        /* Handle the cmd and send resp */
        if (g_hif_ctrl.cmd_event) {
            if (g_hif_ctrl.debounce_en) {
                g_hif_ctrl.debounce_cnt = 0;
            }
            HIF_MsgHdr_t *cmd_msg = (HIF_MsgHdr_t *)(g_hif_ctrl.cmd_buf + sizeof(HIF_PhyHdr_t));
            if (g_hif_ctrl.cmd_err == 0) {
                if (g_hif_ctrl.cfgParam.comType != HIF_COM_TYPE_UART) {
                    g_hif_ctrl.data_wait_send = 1;
                }
                HIF_MsgHdl_Process(cmd_msg);
            } else {
                hif_Msg_ReportCancel();
                HIF_MsgResp(cmd_msg, 0, g_hif_ctrl.cmd_err);
            }
            g_hif_ctrl.cmd_event = false;
        }

        /* check device has notify msg send to host */
        if (queue_empty(&g_hif_ctrl.put) && queue_empty(&g_hif_ctrl.retry)) {
#if CONFIG_PM
            put_retry_empty = true;
#endif
            /* the device can sleep, check host allows deive to sleep */
            if (hif_deivce_sleep_check()) {
                hif_device_sleep();
                HIF_LOG_IO(HIF_LOGIO_NUM_0, 0, 4);
                OSI_SemaphoreWait(&g_hif_ctrl.proc_sem, OSI_WAIT_FOREVER);
                check_period = false;
            } else {
                HIF_LOG_IO(HIF_LOGIO_NUM_0, 0, 3);
                check_period = true;
            }
        } else {
#if CONFIG_PM
            put_retry_empty = false;
#endif
            /* send data */
            if (hif_Host_CheckReady()) {
                HIF_LOG_IO(HIF_LOGIO_NUM_0, 0, 2);
                hif_Msg_ReportSend();
                check_period = false;
            } else {
                HIF_LOG_IO(HIF_LOGIO_NUM_0, 0, 1);
                check_period = true;
            }
        }

#if CONFIG_PM
        if(hif_msg_tx_queue_free() && put_retry_empty && check_period) {
            hif_task_last_period = 0;
            OSI_SemaphoreRelease(&g_hif_ctrl.proc_sem);
        } else {
            hif_task_last_period = HIF_THREAD_CHECK_PERIOD;
        }
#else
        hif_msg_tx_queue_free();
#endif
        if (check_period) {
            OSI_SemaphoreWait(&g_hif_ctrl.proc_sem, HIF_THREAD_CHECK_PERIOD);
        }
    } while (!g_hif_ctrl.terminate);

	hif_Msg_ReportFlush();
	hif_PM_SleepHost();
	OSI_SemaphoreWait(&g_hif_ctrl.exit_sem, OSI_WAIT_FOREVER);
}


void hif_cfg_reset(uint8_t com_mode)
{
    g_hif_ctrl.cfgParam.comType = com_mode;

    g_hif_ctrl.debounce_time = 2000;
    g_hif_ctrl.debounce_cnt = 0;
    g_hif_ctrl.debounce_en = 0;

    g_hif_ctrl.cfgParam.splitEna = 0;
    g_hif_ctrl.cfgParam.splitSize = HIF_MSG_SEND_MAX_PAYLOAD;

    g_hif_ctrl.cfgParam.notifyType = HIF_NOTIFY_TYPE_DIS;
    g_hif_ctrl.cfgParam.notifyIoNum = 6;
    g_hif_ctrl.cfgParam.notifyIoLevel = HIF_IO_H;

    g_hif_ctrl.cfgParam.sendDelay = 0;
    g_hif_ctrl.cfgParam.sendFrameTo = 100;

    g_hif_ctrl.cfgParam.wakeIoNum = 0;
    g_hif_ctrl.cfgParam.wakeType = HIF_WAKE_TYPE_DIS;
    g_hif_ctrl.cfgParam.wakeComParam = 8;
    g_hif_ctrl.cfgParam.wakeIoLevel = HIF_IO_H;

    if (g_hif_ctrl.cfgParam.comType == HIF_COM_TYPE_UART) {
        g_hif_ctrl.cfgParam.comRate = 921600;

        g_hif_ctrl.cfgParam.wakeType = HIF_WAKE_TYPE_COM;
        g_hif_ctrl.cfgParam.wakeComParam = 8;

    } else if (g_hif_ctrl.cfgParam.comType == HIF_COM_TYPE_IIC) {
        g_hif_ctrl.cfgParam.comRate = 100000;

        g_hif_ctrl.cfgParam.wakeType = HIF_WAKE_TYPE_COM;
        g_hif_ctrl.cfgParam.wakeComParam = 0x3A;

        g_hif_ctrl.cfgParam.notifyType = HIF_NOTIFY_TYPE_IO;
    } else if (g_hif_ctrl.cfgParam.comType == HIF_COM_TYPE_SPI) {
        g_hif_ctrl.cfgParam.comRate = 8000000;

        g_hif_ctrl.cfgParam.wakeType = HIF_WAKE_TYPE_COM;
        g_hif_ctrl.cfgParam.wakeComParam = 8;

        g_hif_ctrl.cfgParam.notifyType = HIF_NOTIFY_TYPE_IO;

    } else {
        g_hif_ctrl.cfgParam.wakeType = HIF_WAKE_TYPE_DIS;
        g_hif_ctrl.cfgParam.wakeComParam = 0;

    }
}


int HIF_ParamGet(HIF_Attr_t attr, void * arg, uint32_t argSize)
{
    int status = HIF_ERRCODE_SUCCESS;

    if (arg == NULL) {
        return HIF_ERRCODE_INVALID_PARAM;
    }

    switch (attr) {
        case HIF_SET_COM_TYPE:
        {
            uint32_t *com_Type = (uint32_t *)arg;
            *com_Type = g_hif_ctrl.cfgParam.comType;
            break;
        }

        case HIF_SET_COM_RATE:
        {
            uint32_t *com_Rate = (uint32_t *)arg;
            *com_Rate = g_hif_ctrl.cfgParam.comRate;
            break;
        }

        case HIF_SET_COM_PARAM:
        {
            uint32_t *com_Param = (uint32_t *)arg;
            *com_Param = g_hif_ctrl.cfgParam.comParam;
            break;
        }

        case HIF_SET_NOTIFY_TYPE:
        {
            HIF_NotifyType_t *notify_Type = (HIF_NotifyType_t *)arg;
            *notify_Type = g_hif_ctrl.cfgParam.notifyType;
            break;
        }

        case HIF_SET_NOTIFY_IO_LEVEL:
        {
            HIF_IOLevel_t *notify_IoLevel = (HIF_IOLevel_t *)arg;
            *notify_IoLevel = g_hif_ctrl.cfgParam.notifyIoLevel;
            break;
        }

        case HIF_SET_NOTIFY_IO_NUM:
        {
            uint32_t *notify_IoNum = (uint32_t *)arg;
            *notify_IoNum = g_hif_ctrl.cfgParam.notifyIoNum;
            break;
        }

        case HIF_SET_WAKE_TYPE:
        {
            HIF_WakeType_t *wake_Type = (HIF_WakeType_t *)arg;
            *wake_Type = g_hif_ctrl.cfgParam.wakeType;
            break;
        }

        case HIF_SET_WAKE_IO_LEVEL:
        {
            HIF_IOLevel_t *wake_IoLevel = (HIF_IOLevel_t *)arg;
            *wake_IoLevel = g_hif_ctrl.cfgParam.wakeIoLevel;
            break;
        }

        case HIF_SET_WAKE_IO_NUM:
        {
            uint32_t *wake_IoNum = (uint32_t *)arg;
            *wake_IoNum = g_hif_ctrl.cfgParam.wakeIoNum;
            break;
        }

        case HIF_SET_WAKE_COM_PARAM:
        {
            uint32_t *wake_ComParam = (uint32_t *)arg;
            *wake_ComParam = g_hif_ctrl.cfgParam.wakeComParam;
            break;
        }

        case HIF_SET_RECV_FRAME_TO:
        {
            uint32_t *recv_FrameTo = (uint32_t *)arg;
            *recv_FrameTo = g_hif_ctrl.cfgParam.recvFrameTo;
            break;
        }

        case HIF_SET_SEND_FRAME_TO:
        {
            uint32_t *send_FrameTo = (uint32_t *)arg;
            *send_FrameTo = g_hif_ctrl.cfgParam.sendFrameTo;
            break;
        }

        case HIF_SET_SEND_DELAY:
        {
            uint32_t *send_Delay = (uint32_t *)arg;
            *send_Delay = g_hif_ctrl.cfgParam.sendDelay;
            break;
        }

        case HIF_SET_SLEEP_TO:
        {
            uint32_t *send_FrameTo = (uint32_t *)arg;
            *send_FrameTo = g_hif_ctrl.cfgParam.sendFrameTo;
            break;
        }

        case HIF_SET_SPLIT_SIZE:
        {
            uint32_t *split_Size  = (uint32_t *)arg;
            *split_Size = g_hif_ctrl.cfgParam.splitSize;
            break;
        }

        case HIF_SET_SPLIT_ENA:
        {
            uint32_t *split_Ena = (uint32_t *)arg;
            *split_Ena = g_hif_ctrl.cfgParam.splitEna;
            break;
        }

        case HIF_SET_SIMPLEX_MODE:
        {
            uint32_t *simplex_Mode = (uint32_t *)arg;
            *simplex_Mode = g_hif_ctrl.cfgParam.simplex;
            break;
        }

        case HIF_SET_RETRY:
        {
            uint32_t *retry = (uint32_t *)arg;
            *retry = g_hif_ctrl.cfgParam.retry_enable;
            break;
        }

        default:
            break;
    }

    return status;

}


int HIF_ExtControl(HIF_Attr_t attr, void * arg, uint32_t argSize)
{
    int status = HIF_ERRCODE_SUCCESS;

    if (arg == NULL) {
        return HIF_ERRCODE_INVALID_PARAM;
    }

    switch (attr) {
        case HIF_SET_COM_TYPE:
        {
            HIF_ComType_t comType = *(HIF_ComType_t *)arg;
            if (comType <= HIF_COM_TYPE_SPI) {
                g_hif_ctrl.cfgParam.comType = comType;
            } else {
                status = HIF_ERRCODE_IO_ERROR;
            }
            break;
        }

        case HIF_SET_COM_RATE:
        {
            uint32_t comRate = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.comRate = comRate;
            break;
        }

        case HIF_SET_COM_PARAM:
        {
            uint32_t comParam = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.comParam = comParam;
            break;
        }

        case HIF_SET_NOTIFY_TYPE:
        {
            HIF_NotifyType_t notifyType = *(HIF_NotifyType_t *)arg;
            if (notifyType <= HIF_NOTIFY_TYPE_DLY) {
                g_hif_ctrl.cfgParam.notifyType = notifyType;
            } else {
                status = HIF_ERRCODE_IO_ERROR;
            }
            break;
        }

        case HIF_SET_NOTIFY_IO_LEVEL:
        {
            HIF_IOLevel_t notifyIoLevel = *(HIF_IOLevel_t *)arg;
            if (notifyIoLevel <= HIF_IO_H) {
                g_hif_ctrl.cfgParam.notifyIoLevel = notifyIoLevel;
            } else {
                status = HIF_ERRCODE_IO_ERROR;
            }
            break;
        }

        case HIF_SET_NOTIFY_IO_NUM:
        {
            uint32_t notifyIoNum = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.notifyIoNum = notifyIoNum;
            break;
        }

        case HIF_SET_WAKE_TYPE:
        {
            HIF_WakeType_t wakeType = *(HIF_WakeType_t *)arg;
            if (wakeType <= HIF_WAKE_TYPE_IO) {
                g_hif_ctrl.cfgParam.wakeType = wakeType;
            } else {
                status = HIF_ERRCODE_IO_ERROR;
            }
            break;
        }

        case HIF_SET_WAKE_IO_LEVEL:
        {
            HIF_IOLevel_t wakeIoLevel = *(HIF_IOLevel_t *)arg;
            if (wakeIoLevel <= HIF_IO_H) {
                g_hif_ctrl.cfgParam.wakeIoLevel = wakeIoLevel;
            } else {
                status = HIF_ERRCODE_IO_ERROR;
            }
            break;
        }

        case HIF_SET_WAKE_IO_NUM:
        {
            uint32_t wakeIoNum = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.wakeIoNum = wakeIoNum;
            break;
        }

        case HIF_SET_WAKE_COM_PARAM:
        {
            uint32_t wakeComParam = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.wakeComParam = wakeComParam;
            break;
        }

        case HIF_SET_RECV_FRAME_TO:
        {
            uint32_t recvFrameTo = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.recvFrameTo = recvFrameTo;
            break;
        }

        case HIF_SET_SEND_FRAME_TO:
        {
            uint32_t sendFrameTo = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.sendFrameTo = sendFrameTo;
            break;
        }

        case HIF_SET_SEND_DELAY:
        {
            uint32_t sendDelay = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.sendDelay = sendDelay;
            break;
        }

        case HIF_SET_SLEEP_TO:
        {
            uint32_t sleepTo = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.sleepTo = sleepTo;
            break;
        }

        case HIF_SET_SPLIT_SIZE:
        {
            uint32_t splitSize  = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.splitSize = splitSize ;
            break;
        }

        case HIF_SET_SPLIT_ENA:
        {
            uint32_t splitEna = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.splitEna = splitEna;
            break;
        }

        case HIF_SET_SIMPLEX_MODE:
        {
            uint32_t simplexMode = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.simplex = simplexMode;
            break;
        }

        case HIF_SET_RETRY:
        {
            uint32_t retry = *(uint32_t *)arg;
            g_hif_ctrl.cfgParam.retry_enable = retry;
            break;
        }

        case HIF_SET_DEBOUNCE_EN:
        {
            uint32_t debounceEn = *(uint32_t *)arg;
            g_hif_ctrl.debounce_en = debounceEn;
            break;
        }

        case HIF_SET_DEBOUNCE_TIME:
        {
            uint32_t debounceTime = *(uint32_t *)arg;
            g_hif_ctrl.debounce_time = debounceTime;
            break;
        }

        default:
            break;
    }

    return status;
}


int HIF_DefaultInitParam(HIF_InitParam_t * cfgParam, HIF_ComType_t comType, uint32_t comRate)
{
    if (cfgParam == NULL) {
        return -1;
    }

    cfgParam->comType       = comType;
    cfgParam->comRate        = comRate;
    if (cfgParam->comType == HIF_COM_TYPE_IIC) {
        cfgParam->comParam   = 0x3A;
    } else {
        cfgParam->comParam   = 0;
    }

    if(cfgParam->comType == HIF_COM_TYPE_UART) {
        cfgParam->notifyType     = HIF_NOTIFY_TYPE_DIS;
    } else {
        cfgParam->notifyType     = HIF_NOTIFY_TYPE_IO;
    }

    cfgParam->notifyIoLevel  = HIF_IO_H;
    cfgParam->notifyIoNum    = 6;
    cfgParam->wakeType       = HIF_WAKE_TYPE_COM;
    cfgParam->wakeIoLevel    = HIF_IO_H;
    cfgParam->wakeIoNum      = 0;
    if ((cfgParam->comType == HIF_COM_TYPE_IIC) && (cfgParam->wakeType == HIF_WAKE_TYPE_COM)) {
        cfgParam->wakeComParam = 0x3A;
    } else {
        cfgParam->wakeComParam   = 8;
    }
    cfgParam->sendDelay      = 0;
    cfgParam->sendFrameTo    = 100;
    cfgParam->sleepTo        = 2000;
    cfgParam->splitSize      = HIF_MSG_SEND_MAX_PAYLOAD;
    cfgParam->splitEna       = 0;

    cfgParam->simplex     = false;
    cfgParam->retry_enable          = false;

    memcpy(&g_hif_ctrl.cfgParam, cfgParam, sizeof(HIF_InitParam_t));

    return 0;
}


int HIF_Init(HIF_InitParam_t *pInitParam)
{
    int ret;
    HIF_LOG_DBG("HIF_Init\n");

    if (g_hif_ctrl.inited) {
        HIF_LOG_INF("already init\n");
        return HIF_ERRCODE_SUCCESS;
    }

    if (g_hif_ctrl.terminate) {
        HIF_LOG_INF("in deinit process\n");
        return HIF_ERRCODE_NOT_READY;
    }

    HIF_LOG_DBG("HIF_MsgHdl_Init\n");
    ret = HIF_MsgHdl_Init();
    if (ret) {
        HIF_LOG_INF("HIF_MsgHdl_Init failed %d!\n", ret);
        goto exit1;
    }

    HIF_LOG_DBG("hif_msg_queue_init\n");
    if (hif_msg_queue_init()) {
        HIF_LOG_INF("hif_msg_queue_init failed!\n");
        goto exit1;
    }

    g_hif_ctrl.data_wait_send = 0;

//    hif_cfg_reset(HIF_COM_TYPE_SPI);

    HIF_LOG_DBG("hif_com_register\n");
    ret = hif_com_register(g_hif_ctrl.cfgParam.comType);
    if (ret) {
        HIF_LOG_DBG("hif_com_register fail\n");
        goto exit1;
    }

    HIF_LOG_DBG("hif_com_init\n");
    if (g_hif_ctrl.cfgParam.comType != HIF_COM_TYPE_DIS) {
        ret = hif_com_init(g_hif_ctrl.cfgParam.comRate);
        if (ret) {
            HIF_LOG_DBG("hif_com_init fail\n");
            goto exit1;
        }
    }

    g_hif_ctrl.device_state = HIF_PM_STATE_SLEEP;
    g_hif_ctrl.host_state = HIF_PM_STATE_SLEEP;
    g_hif_ctrl.inited = true;

    HIF_LOG_DBG("hif_com_pm_init\n");
    if (g_hif_ctrl.cfgParam.wakeType == HIF_WAKE_TYPE_COM) {
        ret = hif_com_pm_init(hif_wup_callback, &g_hif_ctrl.cfgParam.wakeComParam);
        if (ret) {
            HIF_LOG_DBG("hif_com_pm_init fail\n");
            goto exit1;
        }
    } else if (g_hif_ctrl.cfgParam.wakeType == HIF_WAKE_TYPE_IO) {
        hif_io_pm_cfg_t hif_io_pm = {
            .io_num     = g_hif_ctrl.cfgParam.wakeIoNum,
            .pull_mode    = 0,
        };
        if (g_hif_ctrl.cfgParam.wakeIoLevel == HIF_IO_L) {
            hif_io_pm.trig_type = HIF_IO_TRIG_LEVEL_LOW;
        } else {
            hif_io_pm.trig_type = HIF_IO_TRIG_LEVEL_HIGH;
        }
        ret = hif_io_in_pm_init(hif_wup_callback, &hif_io_pm);
        if (ret) {
            HIF_LOG_DBG("hif_io_in_pm_init fail\n");
            goto exit1;
        }
    }

    HIF_LOG_DBG("hif_wkio_hold_init\n");
    hif_wkio_hold_init(g_hif_ctrl.cfgParam.notifyIoNum);
    hif_data_io_clr();

#if (HIF_WATCHDOG_ENABLE)
    hif_watchdog_init();
#endif

    /* Creat Host Protocol handle task */
    HIF_LOG_DBG("init_once\n");
    if (!g_hif_ctrl.init_once) {
        HIF_LOG_DBG("init_once create\n");
        g_hif_ctrl.init_once = true;
        OSI_SemaphoreCreate(&g_hif_ctrl.oper_sem, 1, 1);
        OSI_SemaphoreCreate(&g_hif_ctrl.proc_sem, 0, 1);
        OSI_SemaphoreCreate(&g_hif_ctrl.exit_sem, 0, 1);
    }
    OSI_ThreadCreate(&g_hif_ctrl.host_interface_task_handle, "host interface task", (OSI_ThreadEntry_t)host_interface_task, NULL, configMAX_PRIORITIES/2, HOST_INTERFACE_STACK_SIZE);

    HIF_LOG_INF("host interface init!\n");
    return 0;

exit1:

    hif_com_deinit();

    return ret;
}

int HIF_DeInit(void)
{
    OSI_Status_t status = OSI_STATUS_OK;

    HIF_LOG_DBG("HIF_DeInit\n");

    if (!g_hif_ctrl.inited || g_hif_ctrl.terminate) {
        HIF_LOG_DBG("already deInit\n");
        return 0; /* Not init */
    }

	g_hif_ctrl.terminate = 1;
	OSI_SemaphoreRelease(&g_hif_ctrl.proc_sem);
#if (HIF_WATCHDOG_ENABLE)
    hif_watchdog_deinit();
#endif

    status = OSI_ThreadDelete(&g_hif_ctrl.host_interface_task_handle);
    if (status != OSI_STATUS_OK) {
        return status;
    }
    HIF_LOG_DBG("HIF task delete: %d\n", status);
    status = OSI_SemaphoreDelete(&g_hif_ctrl.oper_sem);
    if (status != OSI_STATUS_OK) {
        return status;
    }
    HIF_LOG_DBG("HIF semphore oper_sem delete: %d\n", status);
    status = OSI_SemaphoreDelete(&g_hif_ctrl.proc_sem);
    if (status != OSI_STATUS_OK) {
        return status;
    }
    HIF_LOG_DBG("HIF semphore proc_sem delete: %d\n", status);
    status = OSI_SemaphoreDelete(&g_hif_ctrl.exit_sem);
    if (status != OSI_STATUS_OK) {
        return status;
    }
    HIF_LOG_DBG("HIF semphore exit_sem delete: %d\n", status);

    hif_com_deinit();
    g_hif_ctrl.data_wait_send = 0;
    hif_msg_queue_deinit();
    HIF_MsgHdl_Deinit();

    hif_pm_unlock();

    memset(&g_hif_ctrl, 0, sizeof(g_hif_ctrl));

    HIF_LOG_DBG("HIF_DeInit done\n");

    return 0;
}

#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
