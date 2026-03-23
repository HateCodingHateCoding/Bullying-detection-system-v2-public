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
#include "string.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "cmsis_os2.h"
#include "securec.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_uart_client.h"
#include "sle_errcode.h"
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

#define SLE_MTU_SIZE_DEFAULT 520
#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define SLE_UART_TASK_DELAY_MS 1000
#ifndef SLE_UART_SERVER_NAME
#define SLE_UART_SERVER_NAME "sle_uart_server"
#endif
#define SLE_UART_CLIENT_LOG "[sle uart client]"
#define UUID_LEN_2 2
#define TASK_SIZE 2048
#define PRIO 25
#define USLEEP_1000000 1000000

#define AUDIO_FRAME_HEADER       0x55AA
#define AUDIO_FRAME_TAIL         0xAA55
#define AUDIO_FRAME_OVERHEAD     8

static char g_sleUuidAppUuid[] = {0x39, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                  0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static sle_announce_seek_callbacks_t g_sle_uart_seek_cbk = {0};
static sle_connection_callbacks_t g_sle_uart_connect_cbk = {0};
static ssapc_callbacks_t g_sle_uart_ssapc_cbk = {0};

static sle_addr_t g_sle_uart_remote_addr = {0};
uint16_t g_sle_uart_conn_id = 0;
uint8_t g_client_id = 0;
static uint8_t g_foundServer = 0;

#define SLE_UART_TRANSFER_SIZE 1200
static uint8_t g_app_uart_rx_buff[SLE_UART_TRANSFER_SIZE] = {0};
static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE};

static uint16_t g_expectedSeq = 0;
static uint32_t g_totalRecvPktCount = 0;
static uint32_t g_lostPktCount = 0;
static uint8_t  g_seqInitialized = 0;
static uint16_t g_negotiatedMtu = 0;
static uint32_t g_notificationCount = 0;

uint16_t get_g_sle_uart_conn_id(void)
{
    return g_sle_uart_conn_id;
}

/* ==================== UART配置 ==================== */

static void uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    (void)buffer;
    (void)length;
    (void)error;
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
    (void)uapi_uart_register_rx_callback(1, UART_RX_CONDITION_FULL_OR_IDLE,
                                         1, uart_rx_callback);
}

/* ==================== Seek回调 ==================== */

void SleUartStartScan(void)
{
    osal_printk("%s SleUartStartScan enter\r\n", SLE_UART_CLIENT_LOG);
    g_foundServer = 0;
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

static void sle_client_enable_cbk(errcode_t status)
{
    osal_printk("%s sle_enable_cbk, status=0x%x\r\n", SLE_UART_CLIENT_LOG, status);
    if (status == 0) {
        osal_msleep(SLE_UART_TASK_DELAY_MS);
        SleUartStartScan();
    }
}

static void sle_client_seek_enable_cbk(errcode_t status)
{
    osal_printk("%s seek_enable_cbk, status=0x%x\r\n", SLE_UART_CLIENT_LOG, status);
}

static void sle_client_seek_result_cbk(sle_seek_result_info_t *seekResultData)
{
    if (seekResultData == NULL || g_foundServer) {
        return;
    }
    if (seekResultData->data != NULL && seekResultData->data_length > 0 &&
        strstr((const char *)seekResultData->data, SLE_UART_SERVER_NAME) != NULL) {
        osal_printk("%s found target server!\r\n", SLE_UART_CLIENT_LOG);
        g_foundServer = 1;
        memcpy_s(&g_sle_uart_remote_addr, sizeof(sle_addr_t),
                 &seekResultData->addr, sizeof(sle_addr_t));
        sle_stop_seek();
    }
}

static void sle_client_seek_disable_cbk(errcode_t status)
{
    osal_printk("%s seek_disable_cbk, status=0x%x\r\n", SLE_UART_CLIENT_LOG, status);
    if (status == 0) {
        osal_printk("%s connecting...\r\n", SLE_UART_CLIENT_LOG);
        sle_connect_remote_device(&g_sle_uart_remote_addr);
    }
}

static void SleUartClientSeekCbkRegister(void)
{
    g_sle_uart_seek_cbk.sle_enable_cb = sle_client_enable_cbk;
    g_sle_uart_seek_cbk.seek_enable_cb = sle_client_seek_enable_cbk;
    g_sle_uart_seek_cbk.seek_result_cb = sle_client_seek_result_cbk;
    g_sle_uart_seek_cbk.seek_disable_cb = sle_client_seek_disable_cbk;
    sle_announce_seek_register_callbacks(&g_sle_uart_seek_cbk);
}

/* ==================== 连接回调 ==================== */

static void sle_client_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                  sle_acb_state_t conn_state,
                                                  sle_pair_state_t pair_state,
                                                  sle_disc_reason_t disc_reason)
{
    (void)pair_state;
    osal_printk("%s conn_state: id=%d, state=0x%x, reason=0x%x\r\n",
                SLE_UART_CLIENT_LOG, conn_id, conn_state, disc_reason);

    g_sle_uart_conn_id = conn_id;

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s CONNECTED\r\n", SLE_UART_CLIENT_LOG);
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        osal_printk("%s [MTU] requesting MTU = %u\r\n", SLE_UART_CLIENT_LOG, SLE_MTU_SIZE_DEFAULT);
        ssapc_exchange_info_req(g_client_id, conn_id, &info);
        sle_pair_remote_device(addr);

        g_expectedSeq = 0;
        g_totalRecvPktCount = 0;
        g_lostPktCount = 0;
        g_seqInitialized = 0;
        g_notificationCount = 0;

    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s DISCONNECTED\r\n", SLE_UART_CLIENT_LOG);
        uint32_t totalExpected = g_totalRecvPktCount + g_lostPktCount;
        osal_printk("%s [PKT LOSS] === SUMMARY ===\r\n", SLE_UART_CLIENT_LOG);
        osal_printk("%s   recv = %u\r\n", SLE_UART_CLIENT_LOG, g_totalRecvPktCount);
        osal_printk("%s   lost = %u\r\n", SLE_UART_CLIENT_LOG, g_lostPktCount);
        if (totalExpected > 0) {
            osal_printk("%s   rate = %u.%02u%%\r\n", SLE_UART_CLIENT_LOG,
                        (g_lostPktCount * 100) / totalExpected,
                        ((g_lostPktCount * 10000) / totalExpected) % 100);
        }
        g_negotiatedMtu = 0;
        SleRemovePairedRemoteDevice((const SleAddr *)addr);
        SleUartStartScan();
    }
}

static void sle_client_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    (void)addr;
    osal_printk("%s pair_complete: conn_id=%d, status=0x%x\r\n", SLE_UART_CLIENT_LOG, conn_id, status);
}

static void SleUartClientConnectCbkRegister(void)
{
    g_sle_uart_connect_cbk.connect_state_changed_cb = sle_client_connect_state_changed_cbk;
    g_sle_uart_connect_cbk.pair_complete_cb = sle_client_pair_complete_cbk;
    sle_connection_register_callbacks(&g_sle_uart_connect_cbk);
}

/* ==================== SSAPC回调 ==================== */

static void sle_client_exchange_info_cbk(uint8_t client_id, uint16_t conn_id,
                                          ssap_exchange_info_t *param, errcode_t status)
{
    osal_printk("%s exchange_info: client=%d conn=%d status=0x%x\r\n",
                SLE_UART_CLIENT_LOG, client_id, conn_id, status);
    osal_printk("%s [MTU] negotiated MTU = %d, version = %d\r\n",
                SLE_UART_CLIENT_LOG, param->mtu_size, param->version);
    g_negotiatedMtu = param->mtu_size;

    ssapc_find_structure_param_t find_param = {0};
    find_param.type = 1;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(client_id, conn_id, &find_param);
}

static void sle_client_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
                                           ssapc_find_service_result_t *service, errcode_t status)
{
    osal_printk("%s find_structure: start=0x%02x end=0x%02x status=0x%x\r\n",
                SLE_UART_CLIENT_LOG, service->start_hdl, service->end_hdl, status);
}

static void sle_client_find_property_cbk(uint8_t client_id, uint16_t conn_id,
                                          ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("%s find_property: handle=%d status=0x%x\r\n",
                SLE_UART_CLIENT_LOG, property->handle, status);
}

static void sle_client_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                               ssapc_find_structure_result_t *result, errcode_t status)
{
    (void)conn_id;
    osal_printk("%s find_structure_cmp: client=%d status=0x%x type=%d\r\n",
                SLE_UART_CLIENT_LOG, client_id, status, result->type);
}

static void sle_client_write_cfm_cbk(uint8_t client_id, uint16_t conn_id,
                                      ssapc_write_result_t *result, errcode_t status)
{
    (void)client_id;
    osal_printk("%s write_cfm: conn=%d status=0x%x\r\n", SLE_UART_CLIENT_LOG, conn_id, status);
}

/* ==================== 帧处理：丢包检测 + UART转发 ==================== */

static void ProcessFrame(uint8_t *data, uint16_t dataLen)
{
    if (dataLen < AUDIO_FRAME_OVERHEAD) {
        return;
    }

    uint16_t header = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
    uint16_t tail = (uint16_t)data[dataLen - 2] | ((uint16_t)data[dataLen - 1] << 8);

    if (header != AUDIO_FRAME_HEADER || tail != AUDIO_FRAME_TAIL) {
        osal_printk("%s [RX] bad frame: hdr=0x%04X tail=0x%04X len=%u\r\n",
                    SLE_UART_CLIENT_LOG, header, tail, dataLen);
        return;
    }

    uint16_t recvSeq = (uint16_t)data[2] | ((uint16_t)data[3] << 8);
    uint16_t payloadLen = (uint16_t)data[4] | ((uint16_t)data[5] << 8);

    g_totalRecvPktCount++;

    /* 丢包检测 */
    if (!g_seqInitialized) {
        g_expectedSeq = recvSeq;
        g_seqInitialized = 1;
        osal_printk("%s [PKT] first seq=%u\r\n", SLE_UART_CLIENT_LOG, recvSeq);
    }

    if (recvSeq != g_expectedSeq) {
        uint16_t gap;
        if (recvSeq > g_expectedSeq) {
            gap = recvSeq - g_expectedSeq;
        } else {
            gap = (uint16_t)(0x10000UL + recvSeq - g_expectedSeq);
        }
        if (gap <= 1000) {
            g_lostPktCount += gap;
            osal_printk("%s [LOSS] exp=%u got=%u gap=%u total_lost=%u\r\n",
                        SLE_UART_CLIENT_LOG, g_expectedSeq, recvSeq, gap, g_lostPktCount);
        }
    }
    g_expectedSeq = recvSeq + 1;

    /* 每200包统计 */
    if (g_totalRecvPktCount % 200 == 0) {
        uint32_t total = g_totalRecvPktCount + g_lostPktCount;
        osal_printk("%s [STATS] recv=%u lost=%u rate=%u.%02u%%\r\n",
                    SLE_UART_CLIENT_LOG,
                    g_totalRecvPktCount, g_lostPktCount,
                    total > 0 ? (g_lostPktCount * 100) / total : 0,
                    total > 0 ? ((g_lostPktCount * 10000) / total) % 100 : 0);
    }

    /* 转发PCM裸数据到UART1 */
    if (payloadLen > 0 && payloadLen <= (dataLen - AUDIO_FRAME_OVERHEAD)) {
        uapi_uart_write(1, &data[6], payloadLen, 0);
    }
}

static void sle_client_notification_cbk(uint8_t client_id, uint16_t conn_id,
                                         ssapc_handle_value_t *data, errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    (void)status;
    g_notificationCount++;
    ProcessFrame(data->data, data->data_len);
}

static void sle_client_indication_cbk(uint8_t client_id, uint16_t conn_id,
                                       ssapc_handle_value_t *data, errcode_t status)
{
    (void)client_id;
    (void)conn_id;
    (void)status;
    ProcessFrame(data->data, data->data_len);
}

static void SleUartClientSsapcCbkRegister(void)
{
    g_sle_uart_ssapc_cbk.exchange_info_cb = sle_client_exchange_info_cbk;
    g_sle_uart_ssapc_cbk.find_structure_cb = sle_client_find_structure_cbk;
    g_sle_uart_ssapc_cbk.ssapc_find_property_cbk = sle_client_find_property_cbk;
    g_sle_uart_ssapc_cbk.find_structure_cmp_cb = sle_client_find_structure_cmp_cbk;
    g_sle_uart_ssapc_cbk.write_cfm_cb = sle_client_write_cfm_cbk;
    g_sle_uart_ssapc_cbk.notification_cb = sle_client_notification_cbk;
    g_sle_uart_ssapc_cbk.indication_cb = sle_client_indication_cbk;
    ssapc_register_callbacks(&g_sle_uart_ssapc_cbk);
    osal_printk("%s ssapc_register_callbacks done\r\n", SLE_UART_CLIENT_LOG);
}

/* ==================== 客户端注册 ==================== */

static errcode_t sle_uuid_client_register(void)
{
    sle_uuid_t app_uuid = {0};
    app_uuid.len = sizeof(g_sleUuidAppUuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sleUuidAppUuid, sizeof(g_sleUuidAppUuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    errcode_t ret = ssapc_register_client(&app_uuid, &g_client_id);
    osal_printk("%s ssapc_register_client ret=0x%x, client_id=%d\r\n", SLE_UART_CLIENT_LOG, ret, g_client_id);
    return ret;
}

/* ==================== 初始化 ==================== */

void SleUartClientInit(void)
{
    uint8_t local_addr[SLE_ADDR_LEN] = {0x13, 0x67, 0x5c, 0x07, 0x00, 0x51};
    sle_addr_t local_address = {0};
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);

    osal_printk("%s === SleUartClientInit start ===\r\n", SLE_UART_CLIENT_LOG);

    sle_uuid_client_register();
    SleUartClientSeekCbkRegister();
    SleUartClientConnectCbkRegister();
    SleUartClientSsapcCbkRegister();

    osal_printk("%s calling enable_sle...\r\n", SLE_UART_CLIENT_LOG);
    errcode_t ret = enable_sle();
    osal_printk("%s enable_sle ret=0x%x\r\n", SLE_UART_CLIENT_LOG, ret);

    sle_set_local_addr(&local_address);

    osal_printk("%s === SleUartClientInit done ===\r\n", SLE_UART_CLIENT_LOG);
}

static void SleTask(char *arg)
{
    (void)arg;
    usleep(USLEEP_1000000);
    UartInitConfig();
    SleUartClientInit();
    return NULL;
}

static void SleClientExample(void)
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
        osal_printk(" Falied to create SleTask!\n");
    } else {
        osal_printk(" create SleTask successfully !\n");
    }
}

SYS_RUN(SleClientExample);
