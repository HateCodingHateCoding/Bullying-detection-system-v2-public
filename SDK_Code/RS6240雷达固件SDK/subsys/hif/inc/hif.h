/**
 ******************************************************************************
 * @file    hif.h
 * @brief   hif define.
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

#ifndef _HIF_H
#define _HIF_H

/* Includes.
 * ----------------------------------------------------------------------------
 */
// #include "hif_config.h"
#include "hif_types.h"
#include "hif_msg.h"
#include "hif_log.h"
#include "hif_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Exported types.
 * ----------------------------------------------------------------------------
 */
typedef enum __attribute__((packed)) {
    HIF_COM_TYPE_DIS                        = 0,
    HIF_COM_TYPE_UART                       = 1,
    HIF_COM_TYPE_IIC                        = 2,
    HIF_COM_TYPE_SPI                        = 3,
} HIF_ComType_t;

typedef enum __attribute__((packed)) {
    HIF_IO_L                                = 0,
    HIF_IO_H                                = 1,
} HIF_IOLevel_t;

typedef enum __attribute__((packed)) {
    HIF_WAKE_TYPE_DIS                       = 0,
    HIF_WAKE_TYPE_COM                       = 1,
    HIF_WAKE_TYPE_IO                        = 2,
} HIF_WakeType_t;

typedef enum __attribute__((packed)) {
    HIF_NOTIFY_TYPE_DIS                     = 0,
    HIF_NOTIFY_TYPE_IO                      = 1,
    HIF_NOTIFY_TYPE_DLY                     = 2,
} HIF_NotifyType_t;

typedef struct {
	#define HOST_POLL_TYPE_NULL        0
    #define HOST_POLL_TYPE_ACK         1
    #define HOST_POLL_TYPE_APP_CUBE    2
	#define HOST_POLL_TYPE_MSG_TLVS    3
	uint8_t  poll_type;
	uint8_t  ack_err_num;
	uint16_t burst_period;
	uint8_t  err_seq[0];
} HOST_POLL_ACK;

typedef struct __host_cube_retry_s {
	uint32_t offset;
	uint32_t len;
} HOST_CUBE_RETRY;

typedef struct {
	uint8_t  poll_type;
	uint8_t  retry_num;
	uint16_t burst_period;
	HOST_CUBE_RETRY  retry[0];
} HOST_POLL_APP_CUBE;

typedef struct __msg_fragment_ack_s {
	uint32_t blank_length;
	uint32_t blank_offset  : 24;
	uint32_t flow_seq      : 6;
	uint32_t reserve       : 2;
} MSG_FRAG_ACK;

typedef struct __host_msg_ack_tlv_s {
	uint8_t  msgid;
#define MSG_ACK_TYPE_APP_DEF           0
#define MSG_ACK_TYPE_FRAGMENT          1
	uint8_t  type;
	uint16_t length;
	uint8_t  data[0];
} MSG_ACK_TLV;

typedef struct {
	uint8_t  poll_type;
	uint8_t  msg_tlv_num;
	uint16_t burst_period;
	MSG_ACK_TLV  ack_info[0];
} HOST_POLL_MSG_TLVS;

typedef struct {
    /* com
     */
    HIF_ComType_t       comType;
    uint32_t            comRate;
    /* uart :
     *      start bit, data bit, stop bit, parity
     * i2c :
     *      address
     * spi :
     *      line
     */
    uint32_t            comParam;

    /* device notify host
     */
    HIF_NotifyType_t    notifyType;
    HIF_IOLevel_t       notifyIoLevel;
    uint32_t            notifyIoNum;

    /* host wakeup device
     */
    HIF_WakeType_t      wakeType;
    HIF_IOLevel_t       wakeIoLevel;
    uint32_t            wakeIoNum;
    uint32_t            wakeComParam;

    /* timeout
     */
    /* begin receive frame magic to all frame
     */
    uint32_t            recvFrameTo;
    /*
     * from the start of sending to successful sending.
     */
    uint32_t            sendFrameTo;
    /* configuration phase
     * after waking up the device but before sending any cmd,
     * if a timeout occurs, this is marked as a false wake-up.
     */
    uint32_t            sendDelay;
    /* configuration phase
     * a series of cmd were sent, but the sleep cmd was not sent for a long time.
     * after this timeout, the system will auto enter sleep mode.
     *
     * calc start from the resoinse to the last cmd.
     */
    uint32_t            sleepTo;

    uint32_t            splitSize;
    uint32_t            splitEna;

    uint32_t            simplex;

    uint32_t            retry_enable;
} HIF_InitParam_t;

typedef enum {
    HIF_SET_COM_TYPE            = 0,
    HIF_SET_COM_RATE            = 1,
    HIF_SET_COM_PARAM           = 2,
    HIF_SET_NOTIFY_TYPE         = 3,
    HIF_SET_NOTIFY_IO_LEVEL     = 4,
    HIF_SET_NOTIFY_IO_NUM       = 5,
    HIF_SET_WAKE_TYPE           = 6,
    HIF_SET_WAKE_IO_LEVEL       = 7,
    HIF_SET_WAKE_IO_NUM         = 8,
    HIF_SET_WAKE_COM_PARAM      = 9,
    HIF_SET_RECV_FRAME_TO       = 10,
    HIF_SET_SEND_FRAME_TO       = 11,
    HIF_SET_SEND_DELAY          = 12,
    HIF_SET_SLEEP_TO            = 13,
    HIF_SET_SPLIT_SIZE          = 14,
    HIF_SET_SPLIT_ENA           = 15,
    HIF_SET_SIMPLEX_MODE        = 16,
    HIF_SET_RETRY               = 17,
    HIF_SET_DEBOUNCE_EN         = 18,
    HIF_SET_DEBOUNCE_TIME       = 19,
} HIF_Attr_t;

#ifndef MIN
#define MIN(x, y)					((x) < (y) ? (x) : (y))
#endif

typedef void (* HIF_MsgTrans_Callback)(hif_queue_item_t *hif_queue_item, int8_t status);

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

#define HIF_MSG_HDL_PACK(msgHdr, msgId, dataLen) \
{                                                       \
    do{                                                 \
        (msgHdr).type   = HIF_MSG_TYPE_TO_HOST;         \
        (msgHdr).flag   = HIF_MSG_FLAG_CHECK_BIT;       \
        (msgHdr).msg_id = msgId;                        \
        (msgHdr).length = dataLen;                      \
    }while(0);                                          \
}


/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int HIF_Init(HIF_InitParam_t *pInitParam);
int HIF_DeInit(void);

int HIF_DefaultInitParam(HIF_InitParam_t * cfgParam, HIF_ComType_t comType, uint32_t comRate);
int HIF_ParamGet(HIF_Attr_t attr, void * arg, uint32_t argSize);
int HIF_ExtControl(HIF_Attr_t attr, void * arg, uint32_t argSize);

int HIF_MsgHdl_Regist(uint8_t msgId, HIF_MsgHdl_t msgHdl);

int HIF_MsgResp(HIF_MsgHdr_t *msg, uint16_t dataLen, uint8_t status);

int HIF_MsgReport(HIF_MsgHdr_t *msg, void *data, HIF_MsgTrans_Callback sentCb);

payload_node_t *HIF_MsgReport_ListAddData(payload_node_t * payload_head, void *data, uint16_t len);
int HIF_MsgReport_ListStart(HIF_MsgHdr_t *msg, payload_node_t * payload_head, HIF_MsgTrans_Callback sentCb);

int HIF_InitStatusReport(uint8_t status);






#ifdef __cplusplus
}
#endif

#endif /* _HIF_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
