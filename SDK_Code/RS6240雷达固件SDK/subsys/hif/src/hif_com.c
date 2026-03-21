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
#include "hif_com.h"
#include "hif_checksum.h"
#include "hif_mem.h"
#include "hif_config.h"
#include "hif_log.h"
#include "hif_com_uart.h"
#include "hif_com_spi.h"
#include "hif_com_iic.h"
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

/* Private variables.
 * ----------------------------------------------------------------------------
 */
static hif_com_t hif_com;

static OSI_Semaphore_t hif_com_sem;

static hif_trf_t hif_com_rx;
static hif_trf_t hif_com_tx;
static uint8_t hif_com_rx_seq;
static uint8_t hif_com_tx_seq;

static uint8_t hif_com_pad[HIF_HEAD_LEN];

extern int hif_msg_rx_report(uint8_t status, uint8_t *pbuff, uint16_t len);

extern void hif_Msg_SendDone(void);

com_port_t com_port = {0x00};
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int hif_com_register(HIF_ComType_t com_type)
{
    int status = HIF_ERRCODE_SUCCESS;

    switch (com_type) {
    case HIF_COM_TYPE_DIS:
        break;
#if (CONFIG_HIF_DEVICE_COM_UART)
    case HIF_COM_TYPE_UART:
        hif_com.com_init                = hif_com_uart_init;
        hif_com.com_deinit                = hif_com_uart_deinit;

        hif_com.com_wakeup                = hif_com_uart_wakeup;

        hif_com.com_tx_start            = hif_com_uart_tx_start;
        hif_com.com_tx                    = hif_com_uart_write;
#if (CONFIG_HIF_SEND_DMA)
        hif_com.com_tx_end                = NULL;
#else
		hif_com.com_tx_end                = hif_com_uart_tx_done;
#endif
        hif_com.com_tx_timeout            = NULL;//hif_com_uart_tx_timeout
        hif_com.com_rx_start            = NULL;
        hif_com.com_rx                    = hif_com_uart_read;
        hif_com.com_rx_end                = hif_com_uart_rx_done;
        hif_com.com_rx_timeout            = NULL;//hif_com_uart_rx_timeout

#if (CONFIG_HIF_DEVICE_WAKEUP_UART)
        hif_com.com_pm_init                = hif_com_uart_pm_init;
        hif_com.com_pm_deinit            = hif_com_uart_pm_deinit;
#else
        hif_com.com_pm_init                = NULL;
        hif_com.com_pm_deinit            = NULL;
#endif /* CONFIG_HIF_DEVICE_WAKEUP_UART */

        hif_com.com_type                = HIF_COM_TYPE_UART;
        break;
#endif

#if CONFIG_HIF_DEVICE_COM_IIC
    case HIF_COM_TYPE_IIC:
        hif_com.com_init                = hif_com_iic_init;
        hif_com.com_deinit                = hif_com_iic_deinit;

        hif_com.com_wakeup                = hif_com_iic_wakeup;

        hif_com.com_tx_start            = hif_com_iic_tx_start;
        hif_com.com_tx                    = hif_com_iic_write;
        hif_com.com_tx_end                = hif_com_iic_write_end;
        hif_com.com_tx_timeout            = NULL;
        hif_com.com_rx_start            = NULL;
        hif_com.com_rx                    = hif_com_iic_read;
        hif_com.com_rx_end                = hif_com_iic_read_end;
        hif_com.com_rx_timeout            = NULL;//hif_com_uart_rx_timeout

#if (CONFIG_HIF_DEVICE_WAKEUP_IIC)
        hif_com.com_pm_init             = hif_com_iic_pm_init;
        hif_com.com_pm_deinit            = hif_com_iic_pm_deinit;
#else
        hif_com.com_pm_init             = NULL;
        hif_com.com_pm_deinit            = NULL;
#endif /* CONFIG_HIF_DEVICE_WAKEUP_IIC */

        hif_com.com_type                = HIF_COM_TYPE_IIC;
        break;
#endif /* CONFIG_HIF_DEVICE_COM_IIC */


#if CONFIG_HIF_DEVICE_COM_SPI
    case HIF_COM_TYPE_SPI:
        hif_com.com_init                = hif_com_spi_init;
        hif_com.com_deinit                = hif_com_spi_deinit;

        hif_com.com_wakeup                = hif_com_spi_wakeup;

        hif_com.com_tx_start            = hif_com_spi_tx_start;
        hif_com.com_tx                    = hif_com_spi_write;

#if (CONFIG_HIF_SEND_DMA)
        hif_com.com_tx_end                = NULL;
#else
		hif_com.com_tx_end                = hif_com_spi_tx_end;
#endif
        hif_com.com_tx_timeout            = NULL;//hif_com_spi_tx_timeout;
        hif_com.com_rx_start            = NULL;
        hif_com.com_rx                    = hif_com_spi_read;
        hif_com.com_rx_end                = hif_com_spi_rx_end;
        hif_com.com_rx_timeout            = NULL;//hif_com_uart_rx_timeout

#if (CONFIG_HIF_DEVICE_WAKEUP_SPI)
        hif_com.com_pm_init                = hif_com_spi_pm_init;
        hif_com.com_pm_deinit            = hif_com_spi_pm_deinit;
#else
        hif_com.com_pm_init                = NULL;
        hif_com.com_pm_deinit            = NULL;
#endif /* HIF_DEVICE_WAKEUP_SPI_ENABLE */

        hif_com.com_type                = HIF_COM_TYPE_SPI;
        break;
#endif /*CONFIG_HIF_DEVICE_COM_SPI*/

    default:
        status = HIF_ERRCODE_INVALID_PARAM;
    }

    return status;
}


__hif_sram_text void hif_com_rx_buff_reset(void)
{
    hif_com_rx.state = TRF_STATE_RX_WAIT;
    hif_com_rx.status = HIF_CMD_STATUS_SUCCESS;
    hif_com_rx.size = 0;
    hif_com_rx.offset = 0;
}

int hif_com_init(uint32_t rate)
{
    int status = HIF_ERRCODE_SUCCESS;

    hif_com_rx_buff_reset();
    hif_com_rx.pbuff = (uint8_t *)HIF_MemMalloc(HOST_CMD_MAX_PAYLOAD + HIF_MAX_SPACE);
    if (hif_com_rx.pbuff == NULL) {
        return HIF_ERRCODE_NO_BUFFER;
    }

    hif_com_tx.size = 0;

    hif_com_rx_seq = 0;
    hif_com_tx_seq = 0;

    for (uint8_t idx = 0; idx < HIF_HEAD_LEN; idx++) {
        hif_com_pad[idx] = 0x79;
    }

    OSI_SemaphoreCreate(&hif_com_sem, 0, 1);

    status = hif_com.com_init(rate);
    if (status != HIF_ERRCODE_SUCCESS) {
        return status;
    }

    return HIF_ERRCODE_SUCCESS;
}


int hif_com_deinit(void)
{
    HIF_MemFree(hif_com_rx.pbuff);
    OSI_SemaphoreDelete(&hif_com_sem);
    hif_com.com_deinit();

    return HIF_ERRCODE_SUCCESS;
}

static hif_queue_item_t *send_item = NULL;
static uint8_t *send_data = NULL;
static uint16_t send_offset = 0;
payload_node_t *payload_node_idx = NULL;
bool dma_send_start = false;

extern void hif_data_io_set(void);
#if (CONFIG_HIF_SEND_DMA)
__hif_sram_text int hif_com_msg_send(void *param, bool start)
{
    int status = HIF_ERRCODE_SUCCESS;
    payload_node_t *payload_node = NULL;

    send_item = (hif_queue_item_t *)param;
    HIF_MsgHdr_t *msg_hdr = (HIF_MsgHdr_t *)&send_item->hdr_byte[HIF_PHY_HEAD_LEN];

    bool tail_null = false;
    if(msg_hdr->length == 0 && !(msg_hdr->flag & HIF_MSG_FLAG_CHECK_BIT)) {
        tail_null = true;
    }

    if (hif_com.com_type == HIF_COM_TYPE_DIS) {
        return HIF_ERRCODE_NOT_READY;
    }

    if(hif_com.com_type == HIF_COM_TYPE_SPI &&
		!hif_com_dma_list_node_add_allow()) {
        return HIF_ERRCODE_ABORTED;
    }

    /* calc and transmit header */
    uint8_t check8 = HIF_CheckSum8(HIF_PHY_MSG_MAGIC, &send_item->hdr_byte[HIF_PHY_HEAD_LEN], HIF_MSG_HEAD_LEN);
    send_item->hdr_byte[0] = HIF_PHY_MSG_MAGIC;
    send_item->hdr_byte[1] = ~check8;

    if(hif_com.com_type == HIF_COM_TYPE_SPI) {
        send_item->hdr_len = HIF_HEAD_LEN + 1;
        send_item->check_len = HIF_CHKEC_LEN + 1;
    } else {
        send_item->hdr_len = HIF_HEAD_LEN;
    }

    dma_node_param_t dma_node_param = {0x00};
    dma_node_param.src_addr = (uint32_t)send_item->hdr_byte;
    dma_node_param.dest_addr = com_port.dstAddr;
    dma_node_param.block_size = send_item->hdr_len;
    dma_node_param.int_en = 0;
    dma_node_param.head_idx = 1;
    if (tail_null) {
        dma_node_param.int_en = 1;
        dma_node_param.head_idx = 0;
        dma_node_param.null_tail = 1;
    }
    if(hif_com_dma_list_node_add(dma_node_param) == NULL) {
        return HIF_ERRCODE_ABORTED;
    }
    payload_node = send_item->payload;
    while(payload_node) {
        dma_node_param.src_addr = (uint32_t)payload_node->payload;
        dma_node_param.block_size = payload_node->data_len;
        dma_node_param.int_en = 0;
        dma_node_param.head_idx = 0;
        if((payload_node->next == NULL && (msg_hdr->flag & HIF_MSG_FLAG_CHECK_BIT)) || payload_node->next) {
            if(hif_com_dma_list_node_add(dma_node_param) == NULL) {
                return HIF_ERRCODE_ABORTED;
            }
        } else {
            dma_node_param.int_en = 1;
            send_item->node_end = hif_com_dma_list_node_add(dma_node_param);
            if(send_item->node_end == NULL) {
                return HIF_ERRCODE_ABORTED;
            }
        }
        payload_node = payload_node->next;
    }
    if(msg_hdr->flag & HIF_MSG_FLAG_CHECK_BIT) {
        dma_node_param.src_addr = (uint32_t)send_item->checksum;
        dma_node_param.block_size = send_item->check_len;
        dma_node_param.int_en = 1;
        dma_node_param.head_idx = 0;
        send_item->node_end = hif_com_dma_list_node_add(dma_node_param);
        if(send_item->node_end == NULL) {
            return HIF_ERRCODE_ABORTED;
        }
    }

    if (start && dma_send_start == false && hif_com.com_tx_start != NULL) {
        dma_send_start = true;
        status = hif_com.com_tx_start();
        if (status != 0) {
            return HIF_ERRCODE_IO_ERROR;
        }
		hif_data_io_set();
    }

    return status;

}
#else
__hif_sram_text int hif_com_msg_send(void *param, bool start)
{
    int status = HIF_ERRCODE_SUCCESS;
    payload_node_t *payload_node = NULL;

    send_item = (hif_queue_item_t *)param;
    HIF_MsgHdr_t *msg_hdr = (HIF_MsgHdr_t *)&send_item->hdr_byte[HIF_PHY_HEAD_LEN];

    if (hif_com.com_type == HIF_COM_TYPE_DIS) {
        return HIF_ERRCODE_NOT_READY;
    }

    /* calc and transmit header */
    uint8_t check8 = HIF_CheckSum8(HIF_PHY_MSG_MAGIC, &send_item->hdr_byte[HIF_PHY_HEAD_LEN], HIF_MSG_HEAD_LEN);
    send_item->hdr_byte[0] = HIF_PHY_MSG_MAGIC;
    send_item->hdr_byte[1] = ~check8;

    if(hif_com.com_type == HIF_COM_TYPE_SPI) {
        send_item->hdr_len = HIF_HEAD_LEN + 1;

    } else {
        send_item->hdr_len = HIF_HEAD_LEN;
    }

    hif_com_tx.offset = 0;
    hif_com_tx.size += send_item->hdr_len;
    payload_node = send_item->payload;
    while(payload_node) {
        hif_com_tx.size += payload_node->data_len;
        payload_node = payload_node->next;
    }

    if(msg_hdr->flag & HIF_MSG_FLAG_CHECK_BIT) {
        hif_com_tx.size += send_item->check_len;
    }

    send_data = send_item->hdr_byte;
    payload_node_idx = send_item->payload;
    send_offset = 0;
    if (hif_com.com_tx_start != NULL) {
        hif_data_io_set();
        status = hif_com.com_tx_start();
        if (status != 0) {
            return HIF_ERRCODE_IO_ERROR;
        }
    }

    status = OSI_SemaphoreWait(&hif_com_sem, 100);
    if(status) {

    } else {

    }

    return status;

}

#endif

__hif_sram_text int hif_com_data_recv(void)
{
    int status = HIF_COM_RX_CONTINUE;
    int rx_status = 0;
    uint8_t loop = 1;

    do {
        switch (hif_com_rx.state)
        {
        case TRF_STATE_RX_WAIT:
            rx_status = hif_com.com_rx(&hif_com_rx.pbuff[hif_com_rx.offset], 1);
            if (rx_status <= 0) {
                loop = 0;
                break;
            }
            hif_com_rx.offset += 1;

            if (hif_com_rx.offset <= 2) {
                if (hif_com_rx.pbuff[0] != HIF_PHY_MSG_MAGIC) {
                    if (hif_com_rx.offset == 1) {
                        if ((hif_com_rx.pbuff[0] != HIF_PHY_WKP_MAGIC) && (hif_com_rx.pbuff[0] != HIF_PHY_SYNC_MAGIC)) {
                            hif_com_rx.offset = 0;
                        }
                    } else if (hif_com_rx.offset == 2) {
                        if (hif_com_rx.pbuff[1] == 0xFF) {
                            if (hif_com.com_wakeup != NULL) {
                                hif_com.com_wakeup();
                            }
                        }
                        hif_com_rx.offset = 0;
                    }
                } else {
                }
            } else if (hif_com_rx.offset == HIF_HEAD_LEN) {
                uint8_t check8 = ~(HIF_CheckSum8(0, hif_com_rx.pbuff, HIF_HEAD_LEN));
                if (check8 == 0) {
                    hif_com_rx_seq++;
                    HIF_MsgHdr_t *msg = (HIF_MsgHdr_t *)&hif_com_rx.pbuff[HIF_PHY_HEAD_LEN];
                    if (msg->type != HIF_MSG_TYPE_TO_DEVICE ||
                        (msg->flag & (HIF_MSG_FLAG_MAC32_BIT|HIF_MSG_FLAG_EXTEND_BIT))) {
                        hif_com_rx.size = hif_com_rx.offset;
                        hif_com_rx.status = HIF_CMD_STATUS_VERSION;
                        hif_com_rx.state = TRF_STATE_RX_DONE;
                        if (hif_com.com_rx_end != NULL) {
                            hif_com.com_rx_end();
                        }
                    } else if (msg->length > HOST_CMD_MAX_PAYLOAD) {
                        hif_com_rx.size = hif_com_rx.offset;
                        hif_com_rx.status = HIF_CMD_STATUS_TOOLONG;
                        hif_com_rx.state = TRF_STATE_RX_DONE;
                        if (hif_com.com_rx_end != NULL) {
                            hif_com.com_rx_end();
                        }
                    } else {
                        hif_com_rx.size = hif_com_rx.offset + msg->length;
                        if ((msg->flag & HIF_MSG_FLAG_CHECK_BIT)) {
                            hif_com_rx.size += HIF_CHKEC_LEN;
                        }

                        if (hif_com_rx.offset < hif_com_rx.size) {
                            hif_com_rx.state = TRF_STATE_RX_BUSY;
                        } else {
                            hif_com_rx.state = TRF_STATE_RX_DONE;
                            if (hif_com.com_rx_end != NULL) {
                                hif_com.com_rx_end();
                            }
                        }
                    }
                } else { /* head check error */
                    //HIF_LOG_ERR("Rx%d Check Err=0x%02X\n", hif_com_rx_seq, check8);
                    hif_com_rx.offset = 0;
                }
            }
            break;

        case TRF_STATE_RX_BUSY:
            if (hif_com_rx.offset >= hif_com_rx.size) {
                HIF_MsgHdr_t *msg = (HIF_MsgHdr_t *)&hif_com_rx.pbuff[HIF_PHY_HEAD_LEN];
                if ((msg->flag & HIF_MSG_FLAG_CHECK_BIT)) {
                    uint32_t check_calc = ~(HIF_CheckSum32(0, (uint32_t *)&hif_com_rx.pbuff[HIF_PHY_HEAD_LEN], hif_com_rx.size - HIF_PHY_HEAD_LEN - HIF_CHKEC_LEN));
                    uint32_t check_sum;
                    memcpy(&check_sum, &hif_com_rx.pbuff[hif_com_rx.size - HIF_CHKEC_LEN], sizeof(check_sum));
                    if (check_calc == check_sum) {
                        hif_com_rx.status = HIF_CMD_STATUS_SUCCESS;
                    } else {
                        hif_com_rx.status = HIF_CMD_STATUS_CHECK;
                    }
                }
                hif_com_rx.state = TRF_STATE_RX_DONE;
                if (hif_com.com_rx_end != NULL) {
                    hif_com.com_rx_end();
                }
                loop = 0;
            } else {
                rx_status = hif_com.com_rx(&hif_com_rx.pbuff[hif_com_rx.offset], hif_com_rx.size - hif_com_rx.offset);
                if (rx_status <= 0) {
                    loop = 0;
                    break;
                }
                hif_com_rx.offset += rx_status;
            }
            break;

        case TRF_STATE_RX_DONE:
        {
            uint8_t invalid_data;
            rx_status = hif_com.com_rx(&invalid_data, 1);
            if (rx_status <= 0) {
                loop = 0;
                break;
            }
            break;
        }

        default:
            break;
        }

    } while (loop);

    return status;
}


__hif_sram_text void hif_com_data_recv_done(void)
{
    if (hif_com_rx.state == TRF_STATE_RX_DONE) {
        hif_msg_rx_report(hif_com_rx.status, hif_com_rx.pbuff, hif_com_rx.size);
    } else if (hif_com_rx.state == TRF_STATE_RX_BUSY) {
        hif_msg_rx_report(HIF_CMD_STATUS_CHECK, hif_com_rx.pbuff, hif_com_rx.size);
    }

    hif_com_rx_buff_reset();
}

__hif_sram_text int hif_com_data_send(uint8_t start)
{
    int status = HIF_COM_TX_CONTINUE;
    int16_t len = 0;

    if (hif_com_tx.size == 0) {
        hif_com.com_tx(hif_com_pad, HIF_HEAD_LEN);
        status = HIF_COM_TX_NULL;
    } else {
        len = hif_com_tx.size - hif_com_tx.offset;

        if (len > 0) {
            if(hif_com_tx.offset < send_item->hdr_len) {
                len = send_item->hdr_len - hif_com_tx.offset;
                len = hif_com.com_tx(&send_item->hdr_byte[send_offset], len);
                send_offset += len;
                if(send_offset == send_item->hdr_len) {
                    send_offset = 0;
                    status = HIF_COM_TX_SUCCESS;
                }
            } else if(hif_com_tx.size - hif_com_tx.offset <= send_item->check_len) {
                len = hif_com_tx.size - hif_com_tx.offset;
                len = hif_com.com_tx(&send_item->checksum[send_offset], len);
                send_offset += len;
                if(send_offset == send_item->check_len) {
                    send_offset = 0;
                }
            }else {
                if(payload_node_idx) {
                    send_data = payload_node_idx->payload;
                    len = payload_node_idx->data_len - send_offset;
                    len = hif_com.com_tx(&send_data[send_offset], len);
                    send_offset += len;
                    if(send_offset >= payload_node_idx->data_len) {
                        send_offset = 0;
                        payload_node_idx = payload_node_idx->next;
                    }
                }
            }
            hif_com_tx.offset += len;
            len = hif_com_tx.size - hif_com_tx.offset;
        }

        if (len == 0) {
            hif_com_tx.offset = 0;
            hif_com_tx.size = 0;
            if (hif_com.com_tx_end != NULL) {
                hif_com.com_tx_end();
            }

            status = HIF_COM_TX_DONE;
        }
    }


    return status;
}

extern void hif_proc_queue_done(void);
__hif_sram_text void hif_com_data_send_done(void)
{
#if (CONFIG_HIF_SEND_DMA)
	hif_proc_queue_done();
#else
    OSI_SemaphoreRelease(&hif_com_sem);
#endif
}

__hif_sram_text void hif_com_data_output_done(void)
{
    hif_Msg_SendDone();
}

int hif_com_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg)
{
    if (hif_com.com_pm_init == NULL) {
        return HIF_ERRCODE_NOT_READY;
    }

    return hif_com.com_pm_init(pm_cb, pcfg);
}


int hif_com_pm_deinit(void)
{
    if (hif_com.com_pm_deinit == NULL) {
        return HIF_ERRCODE_NOT_READY;
    }

    return hif_com.com_pm_deinit();;
}

void com_port_set(com_type_e com_type)
{
    com_port.com_type = com_type;
#if    (CONFIG_HIF_SEND_DMA)
    switch(com_type) {
        case com_uart0:
            com_port.dstAddr = DMA_UART0_TX_ADDR;
            break;
        case com_uart1:
            com_port.dstAddr = DMA_UART1_TX_ADDR;
            break;
        case com_uart2:
            com_port.dstAddr = DMA_UART2_TX_ADDR;
            break;
        case com_spi:
            com_port.dstAddr = DMA_QSPI_TXRX_ADDR;
            break;
        case com_i2c:
            com_port.dstAddr = 0;
            break;
        default:
            break;
    }
#endif
}

#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
