/**
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 */

#include "securec.h"
#include "sle_common.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_uart_server_adv.h"
#include "sle_uart_server.h"
#include "cmsis_os2.h"
#include "ohos_init.h"
#include "ohos_sle_common.h"
#include "ohos_sle_errcode.h"
#include "ohos_sle_ssap_server.h"
#include "ohos_sle_ssap_client.h"
#include "ohos_sle_device_discovery.h"
#include "ohos_sle_connection_manager.h"
#include "iot_uart.h"
#include "pinctrl.h"
#include "uart.h"
#include "errcode.h"

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define UUID_INDEX 14
#define BT_INDEX_5 5
#define BT_INDEX_4 4
#define BT_INDEX_0 0
#define SLE_MTU_SIZE_DEFAULT 1400
/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT 1
/* sle server app uuid for test */
static char g_sleUuidAppUuid[UUID_LEN_2] = {0x12, 0x34};
/* server notify property uuid for test */
static char
    g_slePropertyValue[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/* sle connect acb handle */
static uint16_t g_sleConnHdl = 0;
/* sle server handle */
static uint8_t g_serverId = 0;
/* sle service handle */
static uint16_t g_serviceHandle = 0;
/* sle ntf property handle */
static uint16_t g_propertyHandle = 0;
/* sle pair acb handle */
uint16_t g_slePairHdl;
/* 协商后的实际MTU大小 */
static uint16_t g_negotiatedMtu = 0;
/* SLE连接就绪标志：MTU协商完成且配对完成后才允许发数据 */
static uint8_t g_sleReady = 0;

/*
 * 缓冲区大小：一帧音频最大 1032 字节
 */
#define UART_BUFF_LENGTH 1400
uint8_t g_receiveBuf[UART_BUFF_LENGTH] = {0};

#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16

/* ★★★ 修复：printf 改为 osal_printk，原来的递归宏定义导致什么都打印不出来 ★★★ */
#define printf(fmt, args...) osal_printk(fmt, ##args)

#define SLE_UART_SERVER_LOG "[sle uart server]"
#define SLE_SERVER_INIT_DELAY_MS 1000
#define DELAY_100MS 100
#define TASK_SIZE 4096
#define PRIO 25
#define USLEEP_1000000 1000000

/*
 * UART接收缓冲区大小：必须 >= 一帧音频大小(1032)
 */
#define SLE_UART_TRANSFER_SIZE 1200

static uint8_t g_sleUartBase[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                  0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static uint8_t g_appUartRxBuff[SLE_UART_TRANSFER_SIZE] = {0};

static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_appUartRxBuff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE};

/* 发送统计 */
static uint32_t g_sleSendCount = 0;

/* ======================== 消息队列（异步解耦） ======================== */
/* 每帧最大228字节，队列深度16，可缓冲约16帧 */
#define TX_QUEUE_DEPTH   256
#define TX_MSG_MAX_SIZE  256

typedef struct {
    uint16_t len;
    uint8_t  data[TX_MSG_MAX_SIZE];
} TxMsg;

static osMessageQueueId_t g_txQueue = NULL;
/* 丢弃计数（队列满时） */
static uint32_t g_txQueueDropCount = 0;

/* ---------- UART 帧组装状态机 ----------
 * ESP32 一次 write 1032字节，D02 串口可能因IDLE中断拆成多次回调，
 * 所以必须按 0x55AA...0xAA55 协议组装完整帧再转发SLE。
 */
#define FRAME_BUF_SIZE (AUDIO_FRAME_MAX_SIZE + 16)

typedef enum {
    PARSE_WAIT_HEADER_LOW,   /* 等 0xAA */
    PARSE_WAIT_HEADER_HIGH,  /* 等 0x55 */
    PARSE_READ_SEQ_LOW,
    PARSE_READ_SEQ_HIGH,
    PARSE_READ_LEN_LOW,
    PARSE_READ_LEN_HIGH,
    PARSE_READ_PAYLOAD,
    PARSE_WAIT_TAIL_LOW,     /* 等 0x55 */
    PARSE_WAIT_TAIL_HIGH     /* 等 0xAA */
} FrameParseState;

static uint8_t  g_frameBuf[FRAME_BUF_SIZE];
static uint16_t g_frameIdx = 0;
static uint16_t g_payloadLen = 0;
static uint16_t g_payloadRecv = 0;
static uint16_t g_frameSeq = 0;
static FrameParseState g_parseState = PARSE_WAIT_HEADER_LOW;

/* 收到完整一帧后调用：只入队，不直接发SLE */
static void OnFrameComplete(void)
{
    uint16_t totalLen = g_payloadLen + AUDIO_FRAME_OVERHEAD;

    /* ★ 检查SLE是否就绪 */
    if (!g_sleReady) {
        return;
    }

    if (totalLen > TX_MSG_MAX_SIZE) {
        return;
    }

    /* 入队而非直接发送 */
    TxMsg msg;
    msg.len = totalLen;
    if (memcpy_s(msg.data, TX_MSG_MAX_SIZE, g_frameBuf, totalLen) != EOK) {
        return;
    }

    osStatus_t qs = osMessageQueuePut(g_txQueue, &msg, 0, 0);  /* 不等待 */
    if (qs != osOK) {
        g_txQueueDropCount++;
        if (g_txQueueDropCount % 100 == 1) {
            printf("%s [QUEUE] full, drop esp_seq=%u (total_drop=%u)\r\n",
                   SLE_UART_SERVER_LOG, g_frameSeq, g_txQueueDropCount);
        }
    }
}

/* 逐字节喂入解析器 */
static void FrameParseByte(uint8_t byte)
{
    switch (g_parseState) {
        case PARSE_WAIT_HEADER_LOW:
            /* 0x55AA 小端: 第一个字节是 0xAA */
            if (byte == 0xAA) {
                g_frameIdx = 0;
                g_frameBuf[g_frameIdx++] = byte;
                g_parseState = PARSE_WAIT_HEADER_HIGH;
            }
            break;

        case PARSE_WAIT_HEADER_HIGH:
            if (byte == 0x55) {
                g_frameBuf[g_frameIdx++] = byte;
                g_parseState = PARSE_READ_SEQ_LOW;
            } else if (byte == 0xAA) {
                g_frameIdx = 0;
                g_frameBuf[g_frameIdx++] = byte;
            } else {
                g_parseState = PARSE_WAIT_HEADER_LOW;
            }
            break;

        case PARSE_READ_SEQ_LOW:
            g_frameBuf[g_frameIdx++] = byte;
            g_frameSeq = byte;
            g_parseState = PARSE_READ_SEQ_HIGH;
            break;

        case PARSE_READ_SEQ_HIGH:
            g_frameBuf[g_frameIdx++] = byte;
            g_frameSeq |= ((uint16_t)byte << 8);
            g_parseState = PARSE_READ_LEN_LOW;
            break;

        case PARSE_READ_LEN_LOW:
            g_frameBuf[g_frameIdx++] = byte;
            g_payloadLen = byte;
            g_parseState = PARSE_READ_LEN_HIGH;
            break;

        case PARSE_READ_LEN_HIGH:
            g_frameBuf[g_frameIdx++] = byte;
            g_payloadLen |= ((uint16_t)byte << 8);
            g_payloadRecv = 0;
            if (g_payloadLen > AUDIO_CHUNK_SAMPLES * 2 || g_payloadLen == 0) {
                printf("%s [PARSE] bad payload_len=%u, reset\r\n", SLE_UART_SERVER_LOG, g_payloadLen);
                g_parseState = PARSE_WAIT_HEADER_LOW;
            } else {
                g_parseState = PARSE_READ_PAYLOAD;
            }
            break;

        case PARSE_READ_PAYLOAD:
            if (g_frameIdx < FRAME_BUF_SIZE) {
                g_frameBuf[g_frameIdx++] = byte;
            }
            g_payloadRecv++;
            if (g_payloadRecv >= g_payloadLen) {
                g_parseState = PARSE_WAIT_TAIL_LOW;
            }
            break;

        case PARSE_WAIT_TAIL_LOW:
            g_frameBuf[g_frameIdx++] = byte;
            if (byte == 0x55) {
                g_parseState = PARSE_WAIT_TAIL_HIGH;
            } else {
                printf("%s [PARSE] bad tail low=0x%02x, reset\r\n", SLE_UART_SERVER_LOG, byte);
                g_parseState = PARSE_WAIT_HEADER_LOW;
            }
            break;

        case PARSE_WAIT_TAIL_HIGH:
            g_frameBuf[g_frameIdx++] = byte;
            if (byte == 0xAA) {
                OnFrameComplete();
            } else {
                printf("%s [PARSE] bad tail high=0x%02x, reset\r\n", SLE_UART_SERVER_LOG, byte);
            }
            g_parseState = PARSE_WAIT_HEADER_LOW;
            break;

        default:
            g_parseState = PARSE_WAIT_HEADER_LOW;
            break;
    }
}

static void server_uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    if (length > 0) {
        const uint8_t *data = (const uint8_t *)buffer;
        for (uint16_t i = 0; i < length; i++) {
            FrameParseByte(data[i]);
        }
    }
}

static void UartInitConfig(void)
{
    uart_attr_t attr = {
        .baud_rate = 921600,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE};

    uart_pin_config_t pin_config = {
        .tx_pin = 15,
        .rx_pin = 16,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE};
    uapi_uart_deinit(1);
    uapi_uart_init(1, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
    (void)uapi_uart_register_rx_callback(1,
                                         UART_RX_CONDITION_FULL_OR_IDLE,
                                         1,
                                         server_uart_rx_callback);
}

static void Encode2byteLittle(
    uint8_t *ptr, uint16_t data)
{
    *(uint8_t *)((ptr) + 1) = (uint8_t)((data) >> 0x8);
    *(uint8_t *)(ptr) = (uint8_t)(data);
}

static void sle_uuid_set_base(SleUuid *out)
{
    errcode_t ret;
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sleUartBase, SLE_UUID_LEN);
    if (ret != EOK) {
        printf("%s sle_uuid_set_base memcpy fail\n", SLE_UART_SERVER_LOG);
        out->len = 0;
        return;
    }
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, SleUuid *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    Encode2byteLittle(&out->uuid[UUID_INDEX], u2);
}

static void sle_uart_uuid_print(SleUuid *uuid)
{
    if (uuid == NULL) {
        printf("%suuid_print, uuid is null\r\n", SLE_UART_SERVER_LOG);
        return;
    }

    if (uuid->len == UUID_16BIT_LEN) {
        printf("%s uuid: %02x %02x.\n", SLE_UART_SERVER_LOG,
               uuid->uuid[14], uuid->uuid[15]);
    } else if (uuid->len == UUID_128BIT_LEN) {
        printf("%s uuid: \n", SLE_UART_SERVER_LOG);
        printf("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG, uuid->uuid[0], uuid->uuid[1],
               uuid->uuid[2], uuid->uuid[3]);
        printf("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG, uuid->uuid[4], uuid->uuid[5],
               uuid->uuid[6], uuid->uuid[7]);
        printf("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG, uuid->uuid[8], uuid->uuid[9],
               uuid->uuid[10], uuid->uuid[11]);
        printf("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG, uuid->uuid[12], uuid->uuid[13],
               uuid->uuid[14], uuid->uuid[15]);
    }
}

static void ssaps_mtu_changed_cbk(uint8_t serverId, uint16_t connId, SsapcExchangeInfo *mtu_size,
                                  errcode_t status)
{
    printf("%s ssaps_mtu_changed_cbk server_id:%x, conn_id:%x, mtu_size:%d, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, connId, mtu_size->mtuSize, status);
    g_negotiatedMtu = mtu_size->mtuSize;
    printf("%s [MTU] negotiated MTU = %d\r\n", SLE_UART_SERVER_LOG, g_negotiatedMtu);

    /* ★ MTU协商完成，标记SLE就绪 */
    g_sleReady = 1;
    printf("%s [READY] SLE link ready, can forward data now\r\n", SLE_UART_SERVER_LOG);

    if (g_slePairHdl == 0) {
        g_slePairHdl = connId + 1;
    }
}

static void ssaps_start_service_cbk(uint8_t serverId, uint16_t handle, errcode_t status)
{
    printf("%s start service cbk server_id:%d, handle:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
           serverId, handle, status);
}
static void ssaps_add_service_cbk(uint8_t serverId, SleUuid *uuid, uint16_t handle, errcode_t status)
{
    printf("%s add service cbk server_id:%x, handle:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
           serverId, handle, status);
    sle_uart_uuid_print(uuid);
}
static void ssaps_add_property_cbk(uint8_t serverId, SleUuid *uuid, uint16_t serviceHandle,
                                   uint16_t handle, errcode_t status)
{
    printf("%s add property cbk server_id:%x, service_handle:%x, handle:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, serviceHandle, handle, status);
    sle_uart_uuid_print(uuid);
}
static void ssaps_add_descriptor_cbk(uint8_t serverId, SleUuid *uuid, uint16_t serviceHandle,
                                     uint16_t propertyHandle, errcode_t status)
{
    printf("%s add descriptor cbk server_id:%x, service_handle:%x, property_handle:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, serviceHandle, propertyHandle, status);
    sle_uart_uuid_print(uuid);
}
static void ssaps_delete_all_service_cbk(uint8_t serverId, errcode_t status)
{
    printf("%s delete all service server_id:%x, status:%x\r\n", SLE_UART_SERVER_LOG,
           serverId, status);
}

static errcode_t sle_ssaps_register_cbks(SsapsReadRequestCallback ssaps_read_callback,
                                         SsapsWriteRequestCallback ssaps_write_callback)
{
    ssap_exchange_info_t info = {0};
    errcode_t ret;
    SsapsCallbacks ssaps_cbk = {0};
    ssaps_cbk.addServiceCb = ssaps_add_service_cbk;
    ssaps_cbk.addPropertyCb = ssaps_add_property_cbk;
    ssaps_cbk.addDescriptorCb = ssaps_add_descriptor_cbk;
    ssaps_cbk.startServiceCb = ssaps_start_service_cbk;
    ssaps_cbk.deleteAllServiceCb = ssaps_delete_all_service_cbk;
    
    info.mtu_size = SLE_MTU_SIZE_DEFAULT;
    info.version = 1;
    
    ssaps_cbk.mtuChangedCb = ssaps_mtu_changed_cbk;
    ssaps_cbk.readRequestCb = ssaps_read_callback;
    ssaps_cbk.writeRequestCb = ssaps_write_callback;
    ret = SsapsRegisterCallbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s ssaps_register_callbacks fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    SleUuid service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = SsapsAddServiceSync(g_serverId, &service_uuid, 1, &g_serviceHandle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle uuid add service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t add_property_sync(void)
{
    errcode_t ret;
    SsapsPropertyInfo property = {0};
    property.permissions = SLE_UUID_TEST_PROPERTIES;
    property.operateIndication = SLE_UUID_TEST_OPERATION_INDICATION;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.valueLen = OCTET_BIT_LEN;
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_slePropertyValue));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_slePropertyValue), g_slePropertyValue,
                 sizeof(g_slePropertyValue)) != EOK) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = SsapsAddPropertySync(g_serverId, g_serviceHandle, &property, &g_propertyHandle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle uart add property fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    SsapsDescInfo descriptor = {0};
    uint8_t ntfValue[] = {0x01, 0x02};
    add_property_sync();
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.operateIndication = SLE_UUID_TEST_OPERATION_INDICATION;
    descriptor.value = (uint8_t *)osal_vmalloc(sizeof(ntfValue));
    if (descriptor.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(descriptor.value, sizeof(ntfValue), ntfValue, sizeof(ntfValue)) != EOK) {
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = SsapsAddDescriptorSync(g_serverId, g_serviceHandle, g_propertyHandle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle uart add descriptor fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(descriptor.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uart_server_add(void)
{
    errcode_t ret;
    SleUuid app_uuid = {0};
    printf("%s sle uart add service in\r\n", SLE_UART_SERVER_LOG);
    app_uuid.len = sizeof(g_sleUuidAppUuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sleUuidAppUuid, sizeof(g_sleUuidAppUuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    SsapsRegisterServer(&app_uuid, &g_serverId);
    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        SsapsUnregisterServer(g_serverId);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        SsapsUnregisterServer(g_serverId);
        return ERRCODE_SLE_FAIL;
    }
    printf("%s sle uart add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
           SLE_UART_SERVER_LOG, g_serverId, g_serviceHandle, g_propertyHandle);
    ret = SsapsStartService(g_serverId, g_serviceHandle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle uart add service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    printf("%s sle uart add service out\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

/* 通过handle向client发送数据 - 使用独立内存避免全局缓冲区竞争 */
errcode_t sle_uart_server_send_report_by_handle(const uint8_t *data, uint16_t len)
{
    SsapsNtfInd param = {0};
    uint8_t *txBuf = (uint8_t *)osal_vmalloc(len);
    if (txBuf == NULL) {
        return ERRCODE_SLE_FAIL;
    }

    param.handle = g_propertyHandle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = txBuf;
    param.valueLen = len;
    if (memcpy_s(param.value, len, data, len) != EOK) {
        osal_vfree(txBuf);
        return ERRCODE_SLE_FAIL;
    }
    errcode_t ret = SsapsNotifyIndicate(g_serverId, g_sleConnHdl, &param);
    osal_vfree(txBuf);
    return ret;
}

static void sle_connect_state_changed_cbk(uint16_t connId, const SleAddr *addr, SleAcbStateType conn_state,
                                          SlePairStateType pair_state, SleDiscReasonType disc_reason)
{
    printf("%s connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n",
           SLE_UART_SERVER_LOG, connId, conn_state, pair_state, disc_reason);
    printf("%s addr:%02x:**:**:%02x:%02x\r\n", SLE_UART_SERVER_LOG,
           addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    if (conn_state == OH_SLE_ACB_STATE_CONNECTED) {
        g_sleConnHdl = connId;
        g_sleSendCount = 0;
        g_sleReady = 0;  /* ★ 连接时先标记未就绪，等MTU协商完成 */
        ssap_exchange_info_t parameter = {0};
        parameter.mtu_size = SLE_MTU_SIZE_DEFAULT;
        parameter.version = 1;
        ssaps_set_info(g_serverId, &parameter);
        printf("%s [MTU] requesting MTU = %d\r\n", SLE_UART_SERVER_LOG, SLE_MTU_SIZE_DEFAULT);
    } else if (conn_state == OH_SLE_ACB_STATE_DISCONNECTED) {
        printf("%s [STATS] total SLE packets sent = %u\r\n", SLE_UART_SERVER_LOG, g_sleSendCount);
        g_sleConnHdl = 0;
        g_slePairHdl = 0;
        g_negotiatedMtu = 0;
        g_sleReady = 0;
        g_parseState = PARSE_WAIT_HEADER_LOW;
        /* 断连时清空队列中残留的消息 */
        if (g_txQueue != NULL) {
            TxMsg dummy;
            while (osMessageQueueGet(g_txQueue, &dummy, NULL, 0) == osOK) {}
        }
        SleStartAnnounce(SLE_ADV_HANDLE_DEFAULT);
    }
}

static void sle_pair_complete_cbk(uint16_t connId, const SleAddr *addr, errcode_t status)
{
    printf("%s pair complete conn_id:%02x, status:%x\r\n", SLE_UART_SERVER_LOG,
           connId, status);
    printf("%s pair complete addr: %02x:**:**:**: %02x: %02x\r\n", SLE_UART_SERVER_LOG,
           addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_slePairHdl = connId + 1;
}

static errcode_t sle_conn_register_cbks(void)
{
    errcode_t ret;
    SleConnectionCallbacks conn_cbks = {0};
    conn_cbks.connectStateChangedCb = sle_connect_state_changed_cbk;
    conn_cbks.pairCompleteCb = sle_pair_complete_cbk;
    ret = SleConnectionRegisterCallbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_connection_register_callbacks fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

void ssaps_read_request_callbacks(uint8_t serverId,
                                  uint16_t connId, ssaps_req_read_cb_t *read_cb_para, errcode_t status)
{
    (void)serverId;
    (void)connId;
    (void)read_cb_para;
    (void)status;
}

void ssaps_write_request_callbacks(uint8_t serverId, uint16_t connId,
                                   ssaps_req_write_cb_t *write_cb_para, errcode_t status)
{
    (void)serverId;
    (void)connId;
    (void)status;
    printf("%s [SLE RX] recv %d bytes from client\r\n", SLE_UART_SERVER_LOG, write_cb_para->length);
}

/* 初始化uuid server */
errcode_t sle_uart_server_init()
{
    errcode_t ret;
    ret = sle_uart_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_uart_announce_register_cbks fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_conn_register_cbks fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_ssaps_register_cbks(ssaps_read_request_callbacks, ssaps_write_request_callbacks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_ssaps_register_cbks fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    ret = EnableSle();
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s enable_sle fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    printf("%s init ok\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_enable_server_cbk(void)
{
    errcode_t ret;
    ret = sle_uart_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_uart_server_add fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_uart_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_uart_server_adv_init fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/* 发送完整帧数据到SLE */
uint32_t UartSleSendData(uint8_t *data, uint16_t length)
{
    int ret;
    ret = sle_uart_server_send_report_by_handle(data, length);
    if (ret != 0) {
        printf("%s [SLE TX] send fail, ret=%d\r\n", SLE_UART_SERVER_LOG, ret);
    }
    return ret;
}

static void SleTask(char *arg)
{
    (void)arg;
    usleep(USLEEP_1000000);

    /* ★ 创建消息队列 */
    g_txQueue = osMessageQueueNew(TX_QUEUE_DEPTH, sizeof(TxMsg), NULL);
    if (g_txQueue == NULL) {
        printf("%s [FATAL] create tx queue failed!\r\n", SLE_UART_SERVER_LOG);
        return;
    }
    printf("%s [QUEUE] tx queue created, depth=%d\r\n", SLE_UART_SERVER_LOG, TX_QUEUE_DEPTH);

    UartInitConfig();
    sle_uart_server_init();

    /* ★ 主循环：从队列取帧，发送到SLE */
    TxMsg msg;
    while (1) {
        osStatus_t qs = osMessageQueueGet(g_txQueue, &msg, NULL, osWaitForever);
        if (qs == osOK) {
            g_sleSendCount++;
            /* ★ 去掉每帧的printf，只每500帧打一次统计 */
            if (g_sleSendCount % 500 == 0) {
                printf("%s [SLE TX] sent %u pkts, queue_drop=%u, frame=%u bytes\r\n",
                       SLE_UART_SERVER_LOG, g_sleSendCount, g_txQueueDropCount,msg.len);
            }

            int ret = sle_uart_server_send_report_by_handle(msg.data, msg.len);
            if (ret != 0) {
                printf("%s [SLE TX] send fail, ret=%d\r\n", SLE_UART_SERVER_LOG, ret);
            }
        }
    }
}

static void SleServerExample(void)
{
    osThreadAttr_t attr;

    attr.name = "SleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_SIZE;
    attr.priority = PRIO;
    if (osThreadNew(SleTask, NULL, &attr) == NULL) {
        printf(" Falied to create SleTask!\n");
    } else {
        printf(" create SleTask successfully !\n");
    }
}

SYS_RUN(SleServerExample);
