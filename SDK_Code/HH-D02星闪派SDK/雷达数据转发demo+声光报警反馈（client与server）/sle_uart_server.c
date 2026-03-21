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
#include <stdbool.h>
#include "iot_gpio.h"
#include "gpio.h"
#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define UUID_INDEX 14
#define BT_INDEX_5 5
#define BT_INDEX_4 4
#define BT_INDEX_0 0

/* ★ MTU 调高到 1500 */
#define SLE_MTU_SIZE_DEFAULT 1500
#define UART_BUFF_LENGTH     1500

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
/*连接状态 flag*/
static volatile bool g_connected = false;

/* ★ MTU 初值 1500 */
static volatile uint16_t g_ssap_mtu = SLE_MTU_SIZE_DEFAULT;

/* ★ 缓冲区跟 MTU 一致 */
uint8_t g_receiveBuf[UART_BUFF_LENGTH] = {0};

#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16
#define printf(fmt, args...) printf(fmt, ##args)
#define SLE_UART_SERVER_LOG "[sle uart server]"
#define SLE_SERVER_INIT_DELAY_MS 1000
#define SLE_UART_TRANSFER_SIZE 256
#define DELAY_100MS 100
#define TASK_SIZE 2048
#define PRIO 25
#define USLEEP_1000000 1000000

/* ===== MP3 UART1 配置 ===== */
#define MP3_UART_PORT      1
#define MP3_UART_TX_PIN    15
#define MP3_UART_RX_PIN    16
#define MP3_UART_BAUD      9600

static uint8_t g_mp3_uart_rx_buff[SLE_UART_TRANSFER_SIZE] = {0};
static uart_buffer_config_t g_mp3_uart_buffer_config = {
    .rx_buffer = g_mp3_uart_rx_buff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE
};

//以下为声光报警程序函数
#define ALARM_TASK_STACK_SIZE 0x1000
#define ALARM_TASK_PRIO       24
//#define ALARM_GPIO            12   /* 这里换成你实际接 AO3400 gate 的 GPIO */

typedef struct {
    uint8_t cmd;
    uint8_t seconds;
} alarm_msg_t;

static osMessageQueueId_t g_alarm_q = NULL;
static volatile bool g_alarm_active = false;

static void AlarmLedOn(void)
{
    uapi_gpio_set_val(GPIO_12, 1);
}

static void AlarmLedOff(void)
{
    uapi_gpio_set_val(GPIO_12, 0);
}

static void AlarmGpioInit(void)
{
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(GPIO_12, PIN_MODE_0);
    uapi_gpio_set_dir(GPIO_12, GPIO_DIRECTION_OUTPUT);
    AlarmLedOff();
}

static void Mp3UartInit(void)
{
    uart_attr_t attr = {
        .baud_rate = MP3_UART_BAUD,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_pin_config_t pin_config = {
        .tx_pin = MP3_UART_TX_PIN,
        .rx_pin = MP3_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uapi_pin_init();
    uapi_pin_set_mode(GPIO_15, PIN_MODE_1);
    uapi_pin_set_mode(GPIO_16, PIN_MODE_1);

    uapi_uart_deinit(MP3_UART_PORT);
    if (uapi_uart_init(MP3_UART_PORT, &pin_config, &attr, NULL, &g_mp3_uart_buffer_config) != ERRCODE_SUCC) {
        osal_printk("[MP3] uart init fail\r\n");
    } else {
        osal_printk("[MP3] uart init ok, port=%d baud=%d\r\n", MP3_UART_PORT, MP3_UART_BAUD);
    }
}

static void Mp3SendCmd(const char *cmd)
{
    if (cmd == NULL) {
        return;
    }

    int32_t ret = uapi_uart_write(MP3_UART_PORT, (const uint8_t *)cmd, strlen(cmd), 0);
    osal_printk("[MP3] send cmd: %s ret=%d\r\n", cmd, (int)ret);

    /* VG009 这类模块两条命令之间留一点间隔更稳 */
    osal_msleep(100);
}

static void Mp3PlayAlarm(void)
{
    /* 先设音量，再播放 0108.MP3 */
    Mp3SendCmd("@Volume,31,$");
    Mp3SendCmd("@Play,0108,$");
}

static void Mp3Stop(void)
{
    Mp3SendCmd("@Stop,$");
}

static void AlarmTask(void *arg)
{
    (void)arg;
    alarm_msg_t msg;

    while (1) {
        if (osMessageQueueGet(g_alarm_q, &msg, NULL, osWaitForever) == osOK) {
            if (msg.cmd == 0x01) {
                uint32_t i;

                /* 如果已经在报警，就忽略新的重复命令 */
                if (g_alarm_active) {
                    osal_printk("[ALARM] already active\r\n");
                    continue;
                }

                g_alarm_active = true;
                osal_printk("[ALARM] start, seconds=%u\r\n", (unsigned)msg.seconds);

                /* ===== 新增：启动语音播报 ===== */
                Mp3PlayAlarm();

                /* ===== 原有：灯带闪烁 ===== */
                for (i = 0; i < (uint32_t)msg.seconds * 2; i++) {
                    AlarmLedOn();
                    osal_msleep(500);
                    AlarmLedOff();
                    osal_msleep(500);
                }

                AlarmLedOff();
                g_alarm_active = false;
                osal_printk("[ALARM] stop\r\n");

                /* ===== 停止播报 ===== */
                Mp3Stop();

            }
        }
    }
}

//以上为声光报警程序函数


static uint8_t g_sleUartBase[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                  0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t g_atPreChar = 0;
static uint32_t g_atUartRecvCnt = 0;

static uint8_t g_appUartRxBuff[SLE_UART_TRANSFER_SIZE] = {0};

errcode_t sle_uart_server_send_report_by_handle(const uint8_t *data, uint16_t len);

static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_appUartRxBuff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE
};

static void server_uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    errcode_t ret = 0;
    if (length > 0) {
        ret = UartSleSendData((uint8_t *)buffer, (uint16_t)length);
        if (ret != 0) {
            osal_printk("\r\nsle_server_send_data_fail:%d\r\n", ret);
        }
    }
}

/* bridge 需要的 3 个 API */
bool sle_uart_server_is_ready(void)
{
    return (g_connected && (g_propertyHandle != 0));
}

uint16_t sle_uart_server_get_mtu(void)
{
    return g_ssap_mtu;
}

errcode_t sle_uart_server_send_bytes(const uint8_t *data, uint16_t len)
{
    if (!sle_uart_server_is_ready()) return ERRCODE_FAIL;
    if (data == NULL || len == 0) return ERRCODE_FAIL;
    return sle_uart_server_send_report_by_handle(data, len);
}

static void UartInitConfig(void)
{
    uart_attr_t attr = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE};

    uart_pin_config_t pin_config = {
        .tx_pin = 0,
        .rx_pin = 0,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE};
    uapi_uart_deinit(0);
    uapi_uart_init(0, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
    (void)uapi_uart_register_rx_callback(0,
                                         UART_RX_CONDITION_FULL_OR_IDLE,
                                         1,
                                         server_uart_rx_callback);
}

static void Encode2byteLittle(uint8_t *ptr, uint16_t data)
{
    *(uint8_t *)((ptr) + 1) = (uint8_t)((data) >> 0x8);
    *(uint8_t *)(ptr) = (uint8_t)(data);
}

static void sle_uuid_set_base(SleUuid *out)
{
    errcode_t ret;
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sleUartBase, SLE_UUID_LEN);
    if (ret != EOK) {
        osal_printk("%s sle_uuid_set_base memcpy fail\n", SLE_UART_SERVER_LOG);
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
        osal_printk("%suuid_print, uuid is null\r\n", SLE_UART_SERVER_LOG);
        return;
    }
    if (uuid->len == UUID_16BIT_LEN) {
        osal_printk("%s uuid: %02x %02x.\n", SLE_UART_SERVER_LOG,
               uuid->uuid[14], uuid->uuid[15]);
    } else if (uuid->len == UUID_128BIT_LEN) {
        osal_printk("%s uuid: \n", SLE_UART_SERVER_LOG);
        osal_printk("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG,
               uuid->uuid[0], uuid->uuid[1], uuid->uuid[2], uuid->uuid[3]);
        osal_printk("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG,
               uuid->uuid[4], uuid->uuid[5], uuid->uuid[6], uuid->uuid[7]);
        osal_printk("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG,
               uuid->uuid[8], uuid->uuid[9], uuid->uuid[10], uuid->uuid[11]);
        osal_printk("%s 0x%02x 0x%02x 0x%02x 0x%02x\n", SLE_UART_SERVER_LOG,
               uuid->uuid[12], uuid->uuid[13], uuid->uuid[14], uuid->uuid[15]);
    }
}

static void ssaps_mtu_changed_cbk(uint8_t serverId, uint16_t connId, SsapcExchangeInfo *mtu_size,
                                  errcode_t status)
{
    osal_printk("%s mtu_changed server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, connId, mtu_size->mtuSize, status);
    g_ssap_mtu = mtu_size->mtuSize;
    if (g_slePairHdl == 0) {
        g_slePairHdl = connId + 1;
    }
}

static void ssaps_start_service_cbk(uint8_t serverId, uint16_t handle, errcode_t status)
{
    osal_printk("%s start service cbk server_id:%d, handle:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, handle, status);
}

static void ssaps_add_service_cbk(uint8_t serverId, SleUuid *uuid, uint16_t handle, errcode_t status)
{
    osal_printk("%s add service cbk server_id:%x, handle:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, handle, status);
    sle_uart_uuid_print(uuid);
}

static void ssaps_add_property_cbk(uint8_t serverId, SleUuid *uuid, uint16_t serviceHandle,
                                   uint16_t handle, errcode_t status)
{
    osal_printk("%s add property cbk server_id:%x, service_handle:%x, handle:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, serviceHandle, handle, status);
    sle_uart_uuid_print(uuid);
}

static void ssaps_add_descriptor_cbk(uint8_t serverId, SleUuid *uuid, uint16_t serviceHandle,
                                     uint16_t propertyHandle, errcode_t status)
{
    osal_printk("%s add descriptor cbk server_id:%x, service_handle:%x, property_handle:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, serviceHandle, propertyHandle, status);
    sle_uart_uuid_print(uuid);
}

static void ssaps_delete_all_service_cbk(uint8_t serverId, errcode_t status)
{
    osal_printk("%s delete all service server_id:%x, status:%x\r\n",
           SLE_UART_SERVER_LOG, serverId, status);
}

static errcode_t sle_ssaps_register_cbks(SsapsReadRequestCallback ssaps_read_callback,
                                         SsapsWriteRequestCallback ssaps_write_callback)
{
    errcode_t ret;
    SsapsCallbacks ssaps_cbk = {0};
    ssaps_cbk.addServiceCb = ssaps_add_service_cbk;
    ssaps_cbk.addPropertyCb = ssaps_add_property_cbk;
    ssaps_cbk.addDescriptorCb = ssaps_add_descriptor_cbk;
    ssaps_cbk.startServiceCb = ssaps_start_service_cbk;
    ssaps_cbk.deleteAllServiceCb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtuChangedCb = ssaps_mtu_changed_cbk;
    ssaps_cbk.readRequestCb = ssaps_read_callback;
    ssaps_cbk.writeRequestCb = ssaps_write_callback;
    ret = SsapsRegisterCallbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s ssaps_register_callbacks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
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
        osal_printk("%s add service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t add_property_sync(void)
{
    errcode_t ret;
    SsapsPropertyInfo property = {0};

    /* ★ 核心修改：在这里加上 | SSAP_PERMISSION_WRITE */
    /* 原本只有 SLE_UUID_TEST_PROPERTIES (通常是 Notify) */
    property.permissions = SLE_UUID_TEST_PROPERTIES | SSAP_PERMISSION_WRITE; 
    
    property.operateIndication = SLE_UUID_TEST_OPERATION_INDICATION;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);

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
        osal_printk("%s add property fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
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
        osal_printk("%s add descriptor fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
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
    osal_printk("%s sle uart add service in\r\n", SLE_UART_SERVER_LOG);
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
    osal_printk("%s add service ok, server_id:%x, service_handle:%x, property_handle:%x\r\n",
           SLE_UART_SERVER_LOG, g_serverId, g_serviceHandle, g_propertyHandle);
    ret = SsapsStartService(g_serverId, g_serviceHandle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s start service fail, ret:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s sle uart add service out\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_uart_server_send_report_by_handle(const uint8_t *data, uint16_t len)
{
    SsapsNtfInd param = {0};

    uint16_t handle = g_propertyHandle;
    uint16_t conn = g_sleConnHdl;
    uint8_t  sid = g_serverId;
    bool     ready = (g_connected && handle != 0);

    if (!ready) {
        return ERRCODE_FAIL;
    }
    if (data == NULL || len == 0 || len > UART_BUFF_LENGTH) {
        return ERRCODE_FAIL;
    }

    param.handle = handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = g_receiveBuf;
    param.valueLen = len;

    if (memcpy_s(param.value, UART_BUFF_LENGTH, data, len) != EOK) {
        return ERRCODE_FAIL;
    }

    errcode_t ret = SsapsNotifyIndicate(sid, conn, &param);
    if (ret != ERRCODE_SUCC) {
        static uint32_t s_fail = 0;
        if ((++s_fail % 50) == 1) {
            osal_printk("[SEND] notify fail: ret=0x%x mtu=%u len=%u\r\n",
                        (unsigned)ret, (unsigned)g_ssap_mtu, (unsigned)len);
        }
    }
    return ret;
}

static void sle_connect_state_changed_cbk(uint16_t connId, const SleAddr *addr,
    SleAcbStateType conn_state, SlePairStateType pair_state, SleDiscReasonType disc_reason)
{
    osal_printk("%s conn_state changed conn_id:0x%02x, state:0x%x, pair:0x%x, reason:0x%x\r\n",
           SLE_UART_SERVER_LOG, connId, conn_state, pair_state, disc_reason);
    osal_printk("%s addr:%02x:**:**:**:%02x:%02x\r\n", SLE_UART_SERVER_LOG,
           addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    if (conn_state == OH_SLE_ACB_STATE_CONNECTED) {
        g_connected = true;
        g_sleConnHdl = connId;
        ssap_exchange_info_t parameter = {0};
        parameter.mtu_size = SLE_MTU_SIZE_DEFAULT;  /* ★ 1500 */
        parameter.version = 1;
        ssaps_set_info(g_serverId, &parameter);
    } else if (conn_state == OH_SLE_ACB_STATE_DISCONNECTED) {
        g_connected = false;
        g_sleConnHdl = 0;
        g_slePairHdl = 0;
        SleStartAnnounce(SLE_ADV_HANDLE_DEFAULT);
    }
}

static void sle_pair_complete_cbk(uint16_t connId, const SleAddr *addr, errcode_t status)
{
    osal_printk("%s pair complete conn_id:%02x, status:%x\r\n",
           SLE_UART_SERVER_LOG, connId, status);
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
        osal_printk("%s conn_register_cbks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

void ssaps_read_request_callbacks(uint8_t serverId, uint16_t connId,
                                  ssaps_req_read_cb_t *read_cb_para, errcode_t status)
{
    (void)serverId; (void)connId; (void)read_cb_para; (void)status;
}

// void ssaps_write_request_callbacks(uint8_t serverId, uint16_t connId,
//                                    ssaps_req_write_cb_t *write_cb_para, errcode_t status)
// {
//     (void)serverId; (void)connId; (void)status;
//     write_cb_para->value[write_cb_para->length - 1] = '\0';
//     osal_printk(" client_send_data: %s\r\n", write_cb_para->value);
// }
void ssaps_write_request_callbacks(uint8_t serverId, uint16_t connId,
                                   ssaps_req_write_cb_t *write_cb_para, errcode_t status)
{
    (void)serverId;
    (void)connId;
    (void)status;

    if (write_cb_para == NULL || write_cb_para->value == NULL || write_cb_para->length < 4) {
        return;
    }

    uint8_t *buf = write_cb_para->value;
    uint16_t len = write_cb_para->length;

    /* 报警命令格式：AA 55 01 1E
       AA 55 : 帧头
       01    : 启动报警
       1E    : 持续 30 秒 */
    if (buf[0] == 0xAA && buf[1] == 0x55 && buf[2] == 0x01) {
        alarm_msg_t msg = {0};
        msg.cmd = buf[2];
        msg.seconds = buf[3];

        if (g_alarm_q != NULL) {
            if (osMessageQueuePut(g_alarm_q, &msg, 0, 0) == osOK) {
                osal_printk("[ALARM] cmd recv, seconds=%u\r\n", (unsigned)msg.seconds);
            } else {
                osal_printk("[ALARM] queue full\r\n");
            }
        }
        return;
    }

    /* 其他普通写数据，按字符串调试打印 */
    if (len < UART_BUFF_LENGTH) {
        write_cb_para->value[len] = '\0';
    } else {
        write_cb_para->value[len - 1] = '\0';
    }
    osal_printk("client_send_data: %s\r\n", write_cb_para->value);
}

errcode_t sle_uart_server_init()
{
    errcode_t ret;
    ret = sle_uart_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) return ret;
    ret = sle_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) return ret;
    ret = sle_ssaps_register_cbks(ssaps_read_request_callbacks, ssaps_write_request_callbacks);
    if (ret != ERRCODE_SLE_SUCCESS) return ret;
    ret = EnableSle();
    if (ret != ERRCODE_SLE_SUCCESS) return ret;
    osal_printk("%s init ok\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_enable_server_cbk(void)
{
    errcode_t ret;
    ret = sle_uart_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) return ret;
    ret = sle_uart_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS) return ret;
    return ERRCODE_SLE_SUCCESS;
}

uint32_t UartSleSendData(uint8_t *data, uint8_t length)
{
    osal_mdelay(DELAY_100MS);
    return sle_uart_server_send_report_by_handle(data, length);
}

// static void SleTask(void *arg)
// {
//     (void)arg;
//     osal_printk("====SleTask Begins====\r\n");
//     osal_msleep(1000);
//     UartInitConfig();
//     sle_uart_server_init();
//     return;
// }
static void SleTask(void *arg)
{
    (void)arg;
    osal_printk("====SleTask Begins====\r\n");
    osal_msleep(1000);

    UartInitConfig();

    /* 报警 GPIO 和 mp3 uart 初始化 */
    AlarmGpioInit();
    Mp3UartInit();
    /* 创建报警消息队列 */
    if (g_alarm_q == NULL) {
        g_alarm_q = osMessageQueueNew(4, sizeof(alarm_msg_t), NULL);
        if (g_alarm_q == NULL) {
            osal_printk("[ALARM] queue create failed\r\n");
        } else {
            osal_printk("[ALARM] queue ok\r\n");
        }
    }

    /* 创建报警线程 */
    {
        osThreadAttr_t alarm_attr = {0};
        alarm_attr.name = "AlarmTask";
        alarm_attr.stack_size = ALARM_TASK_STACK_SIZE;
        alarm_attr.priority = ALARM_TASK_PRIO;

        if (osThreadNew(AlarmTask, NULL, &alarm_attr) == NULL) {
            osal_printk("[ALARM] task create failed\r\n");
        } else {
            osal_printk("[ALARM] task ok\r\n");
        }
    }

    sle_uart_server_init();

    while (1) {
        osal_msleep(1000);
    }
}

static void SleServerExample(void)
{
    osThreadAttr_t attr = {0};
    attr.name = "SleTask";
    attr.stack_size = TASK_SIZE;
    attr.priority = osPriorityNormal;
    if (osThreadNew(SleTask, NULL, &attr) == NULL) {
        osal_printk(" Failed to create SleTask!\n");
    } else {
        osal_printk(" create SleTask successfully!\n");
    }
}

SYS_RUN(SleServerExample);