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
#include "errcode.h"
#include "osal_addr.h"
#include "sle_common.h"
#include "sle_uart_server.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "string.h"
#include "sle_uart_server_adv.h"

#include "ohos_sle_common.h"
#include "ohos_sle_errcode.h"
#include "ohos_sle_ssap_server.h"
#include "ohos_sle_ssap_client.h"
#include "ohos_sle_device_discovery.h"
#include "ohos_sle_connection_manager.h"

/* sle device name */
#define NAME_MAX_LENGTH 16
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT                 0x64
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT                 0x64
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT              0xC8
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT              0xC8
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT      0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY                      0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER  10
/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT                    1
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX                      251
/* 广播名称 */
static uint8_t g_sleLocalName[NAME_MAX_LENGTH] = "sle_uart_server";
#define SLE_SERVER_INIT_DELAY_MS    1000
#define printf(fmt, args...) osal_printk(fmt, ##args)
#define SLE_UART_SERVER_LOG "[sle uart server]"

static uint16_t SleSetAdvLocalName(uint8_t *advData, uint16_t maxLen)
{
    errno_t ret;
    uint8_t index = 0;

    uint8_t *localName = g_sleLocalName;
    uint8_t localNameLen = sizeof(g_sleLocalName) - 1;
    printf("%s localNameLen = %d\r\n", SLE_UART_SERVER_LOG, localNameLen);
    printf("%s local_name: ", SLE_UART_SERVER_LOG);
    for (uint8_t i = 0; i < localNameLen; i++) {
        printf("0x%02x ", localName[i]);
    }
    printf("\r\n");
    advData[index++] = localNameLen + 1;
    advData[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    ret = memcpy_s(&advData[index], maxLen - index, localName, localNameLen);
    if (ret != EOK) {
        printf("%s memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    return (uint16_t)index + localNameLen;
}

static uint16_t SleSetAdvData(uint8_t *advData)
{
    size_t len = 0;
    uint16_t idx = 0;
    errno_t  ret = 0;

    len = sizeof(struct SleAdvCommonValue);
    struct SleAdvCommonValue advDiscLevel = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    ret = memcpy_s(&advData[idx], SLE_ADV_DATA_LEN_MAX - idx, &advDiscLevel, len);
    if (ret != EOK) {
        printf("%s adv_disc_level memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    idx += len;

    len = sizeof(struct SleAdvCommonValue);
    struct SleAdvCommonValue advAccessMode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    ret = memcpy_s(&advData[idx], SLE_ADV_DATA_LEN_MAX - idx, &advAccessMode, len);
    if (ret != EOK) {
        printf("%s adv_access_mode memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    idx += len;

    return idx;
}

static uint16_t SleSetScanResponseData(uint8_t *scanRspData)
{
    uint16_t idx = 0;
    errno_t ret;
    size_t scanRspDataLen = sizeof(struct SleAdvCommonValue);

    struct SleAdvCommonValue txPowerLevel = {
        .length = scanRspDataLen - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    ret = memcpy_s(scanRspData, SLE_ADV_DATA_LEN_MAX, &txPowerLevel, scanRspDataLen);
    if (ret != EOK) {
        printf("%s sle scan response data memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    idx += scanRspDataLen;

    /* set local name */
    idx += SleSetAdvLocalName(&scanRspData[idx], SLE_ADV_DATA_LEN_MAX - idx);
    return idx;
}

static int SleSetDefaultAnnounceParam(void)
{
    errno_t ret;
    SleAnnounceParam param = {0};
    uint8_t index;
    unsigned char local_addr[SLE_ADDR_LEN] = { 0x78, 0x70, 0x60, 0x88, 0x96, 0x45 };
    param.announceMode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announceHandle = SLE_ADV_HANDLE_DEFAULT;
    param.announceGtRole = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announceLevel = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announceChannelMap = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announceIntervalMin = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announceIntervalMax = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.connIntervalMin = SLE_CONN_INTV_MIN_DEFAULT;
    param.connIntervalMax = SLE_CONN_INTV_MAX_DEFAULT;
    param.connMaxLatency = SLE_CONN_MAX_LATENCY;
    param.connSupervisionTimeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    param.ownAddr.type = 0;
    ret = memcpy_s(param.ownAddr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    if (ret != EOK) {
        printf("%s SleSetDefaultAnnounceParam data memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    printf("%s sle_uart_local addr: ", SLE_UART_SERVER_LOG);
    for (index = 0; index < SLE_ADDR_LEN; index++) {
        printf("0x%02x ", param.ownAddr.addr[index]);
    }
    printf("\r\n");
    return SleSetAnnounceParam(param.announceHandle, &param);
}

static int SleSetDefaultAnnounceData(void)
{
    errcode_t ret;
    uint8_t announceDataLen = 0;
    uint8_t seekDataLen = 0;
    SleAnnounceData data = {0};
    uint8_t advHandle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announceData[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seekRspData[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t dataIndex = 0;

    announceDataLen = SleSetAdvData(announceData);
    data.announceData = announceData;
    data.announceDataLen = announceDataLen;

    printf("%s data.announce_data_len = %d\r\n", SLE_UART_SERVER_LOG, data.announceDataLen);
    printf("%s data.announce_data: ", SLE_UART_SERVER_LOG);
    for (dataIndex = 0; dataIndex<data.announceDataLen; dataIndex++) {
        printf("0x%02x ", data.announceData[dataIndex]);
    }
    printf("\r\n");

    seekDataLen = SleSetScanResponseData(seekRspData);
    data.seekRspData = seekRspData;
    data.seekRspDataLen = seekDataLen;

    printf("%s data.seek_rsp_data_len = %d\r\n", SLE_UART_SERVER_LOG, data.seekRspDataLen);
    printf("%s data.seek_rsp_data: ", SLE_UART_SERVER_LOG);
    for (dataIndex = 0; dataIndex<data.seekRspDataLen; dataIndex++) {
        printf("0x%02x ", data.seekRspData[dataIndex]);
    }
    printf("\r\n");

    ret = SleSetAnnounceData(advHandle, &data);
    if (ret == ERRCODE_SLE_SUCCESS) {
        printf("%s set announce data success.\r\n", SLE_UART_SERVER_LOG);
    } else {
        printf("%s set adv param fail.\r\n", SLE_UART_SERVER_LOG);
    }
    return ERRCODE_SLE_SUCCESS;
}

static void sle_announce_enable_cbk(uint32_t announceId, errcode_t status)
{
    printf("%s sle announce enable callback id:%02x, state:%x\r\n", SLE_UART_SERVER_LOG, announceId,
        status);
}

static void sle_announce_disable_cbk(uint32_t announceId, errcode_t status)
{
    printf("%s sle announce disable callback id:%02x, state:%x\r\n", SLE_UART_SERVER_LOG, announceId,
        status);
}

static void SleAnnounceTerminalCbk(uint32_t announceId)
{
    printf("%s sle announce terminal callback id:%02x\r\n", SLE_UART_SERVER_LOG, announceId);
}

static void sle_enable_cbk(errcode_t status)
{
    printf("%s sle enable callback status:%x\r\n", SLE_UART_SERVER_LOG, status);
    osal_msleep(SLE_SERVER_INIT_DELAY_MS);
    sle_enable_server_cbk();
}

errcode_t sle_uart_announce_register_cbks(void)
{
    errcode_t ret;
    SleAnnounceSeekCallbacks seek_cbks = {0};
    seek_cbks.sleAnnounceEnableCb = sle_announce_enable_cbk;
    seek_cbks.sleAnnounceDisableCb = sle_announce_disable_cbk;
    seek_cbks.sleAnnounceTerminalCb = SleAnnounceTerminalCbk;
    seek_cbks.sleEnableCb = sle_enable_cbk;
    ret = SleAnnounceSeekRegisterCallbacks(&seek_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_uart_announce_register_cbks,register_callbacks fail :%x\r\n",
        SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_uart_server_adv_init(void)
{
    errcode_t ret;
    SleAddr addr;
    unsigned char local_addr[SLE_ADDR_LEN] = { 0x78, 0x70, 0x60, 0x88, 0x96, 0x46};
    addr.type = 0;
    memcpy_s(&addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    SleSetLocalAddr(&addr);
    SleSetDefaultAnnounceParam();
    SleSetDefaultAnnounceData();
    ret = SleStartAnnounce(SLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_SLE_SUCCESS) {
        printf("%s sle_uart_server_adv_init,sle_start_announce fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}
