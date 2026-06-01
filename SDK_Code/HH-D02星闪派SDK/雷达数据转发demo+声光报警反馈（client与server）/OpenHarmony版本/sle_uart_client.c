/**
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
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
#include "radar_sle_proto.h"

/* ★ MTU 1500 */
#define SLE_MTU_SIZE_DEFAULT 1500

#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16
#define SLE_UART_TASK_DELAY_MS 1000
#define SLE_UART_WAIT_SLE_CORE_READY_MS 5000
#ifndef SLE_UART_SERVER_NAME
#define SLE_UART_SERVER_NAME "sle_uart_server"
#endif
#define SLE_UART_CLIENT_LOG "[sle uart client]"
#define UUID_LEN_2 2
#define DELAY_100MS 100
#define TASK_SIZE 0x2000
#define PRIO 25
#define USLEEP_1000000 1000000

#define RADAR_HIF_HDR_LEN   6
#define RADAR_MAX_PAYLOAD   4096
#define RADAR_CHECK32_LEN   4
#define RADAR_MAX_HIF_PDU   (RADAR_HIF_HDR_LEN + RADAR_MAX_PAYLOAD + RADAR_CHECK32_LEN)

/* ★ 转发 UART1 配置 */
#define FORWARD_UART_PORT   1
#define FORWARD_UART_TX_PIN 15
#define FORWARD_UART_RX_PIN 16
#define FORWARD_UART_PIN_MUX 1
#define FORWARD_UART_BAUD   921600
/* ★ 命令 UART2 配置 */
#define CMD_UART_PORT      2
#define CMD_UART_TX_PIN    8
#define CMD_UART_RX_PIN    7
#define CMD_UART_PIN_MUX   2
#define CMD_UART_BAUD      115200
static volatile bool g_alarm_active_pause = false; // 报警期间暂停雷达转发
static uint32_t g_alarm_resume_tick = 0;           // 恢复雷达的时间戳
static volatile bool g_sle_connected = false;
typedef struct {
    uint8_t data[8];
    uint8_t len;
} alarm_cmd_item_t;

static osMessageQueueId_t g_alarm_cmd_q = NULL;
/* ---- 重组状态 ---- */
static uint8_t  g_rx_buf[RADAR_MAX_HIF_PDU];
static uint16_t g_rx_expect    = 0;
static uint16_t g_rx_next_off  = 0;
static uint16_t g_rx_seq       = 0xFFFF;
static bool     g_rx_in_progress = false;

/* ---- 统计 ---- */
static uint32_t g_ok_cnt = 0;
static uint32_t g_fe_cnt = 0;
static uint32_t g_drop_cnt = 0;
static uint32_t g_frame_print_cnt = 0;
static uint32_t g_uart_send_cnt = 0;
static uint32_t g_uart_fail_cnt = 0;

/* ---- 帧队列 ---- */
#define FRAME_SLOT_NUM 4

typedef struct {
    uint8_t  idx;
    uint16_t len;
} frame_q_item_t;

static uint8_t  g_frame_buf[FRAME_SLOT_NUM][RADAR_MAX_HIF_PDU];
static uint16_t g_frame_len[FRAME_SLOT_NUM];
static uint8_t  g_frame_widx = 0;
static osMessageQueueId_t g_frame_q = NULL;

errcode_t sle_uart_client_send_report_by_handle(const uint8_t *data, uint8_t len);

/* ---- SLE ---- */
static char g_sleUuidAppUuid[] = {0x39, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                  0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char g_uartReadBuff[100];
static ssapc_find_service_result_t g_sle_uart_find_service_result = {0};
static sle_announce_seek_callbacks_t g_sle_uart_seek_cbk = {0};
static SleConnectionCallbacks g_sle_uart_connect_cbk = {0};
static ssapc_callbacks_t g_sle_uart_ssapc_cbk = {0};
static SleAddr g_sle_uart_remote_addr = {0};
ssapc_write_param_t g_sle_uart_send_param = {0};
uint16_t g_sle_uart_conn_id = 0;
uint8_t g_client_id = 0;

#define SLE_UART_TRANSFER_SIZE 256
static uint8_t g_app_uart_rx_buff[SLE_UART_TRANSFER_SIZE] = {0};
static uint8_t receive_buf[1500] __attribute__((aligned(4))) = {0};

/* ★ UART1 也需要 rx buffer，哪怕只发不收 */
static uint8_t g_forward_uart_rx_buff[SLE_UART_TRANSFER_SIZE] = {0};
static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE
};
static uart_buffer_config_t g_forward_uart_buffer_config = {
    .rx_buffer = g_forward_uart_rx_buff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE
};

static void radar_rx_reset(void)
{
    g_rx_expect      = 0;
    g_rx_next_off    = 0;
    g_rx_seq         = 0xFFFF;
    g_rx_in_progress = false;
}

uint16_t get_g_sle_uart_conn_id(void)
{
    return g_sle_uart_conn_id;
}

ssapc_write_param_t *get_g_sle_uart_send_param(void)
{
    return &g_sle_uart_send_param;
}

// static void uart_rx_callback(const void *buffer, uint16_t length, bool error)
// {
//     errcode_t ret;
//     if (length > 0) {
//         ret = uart_sle_client_send_data((uint8_t *)buffer, (uint8_t)length);
//         if (ret != 0) {
//             osal_printk("\r\n send_data_fail:%d\r\n", ret);
//         }
//     }
// }
static bool is_alarm_cmd(const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len < 4) return false;
    return (buf[0] == 0xAA && buf[1] == 0x55 && buf[2] == 0x01);
}

static void uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    const uint8_t *buf = (const uint8_t *)buffer;
    alarm_cmd_item_t item = {0};
    (void)error;

    if (length == 0 || buf == NULL) {
        return;
    }

    if (!is_alarm_cmd(buf, length)) {
        return;
    }

    if (length > sizeof(item.data)) {
        return;
    }

    item.len = (uint8_t)length;
    (void)memcpy_s(item.data, sizeof(item.data), buf, length);

    if (g_alarm_cmd_q != NULL) {
        (void)osMessageQueuePut(g_alarm_cmd_q, &item, 0, 0);
    }
}


/* 收香橙派报警命令的 UART2 */
static void UartInitConfig(void)
{
    uart_attr_t attr = {
        .baud_rate = CMD_UART_BAUD,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_pin_config_t pin_config = {
        .tx_pin = CMD_UART_TX_PIN,
        .rx_pin = CMD_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uapi_pin_init();
    uapi_pin_set_mode(CMD_UART_TX_PIN, PIN_MODE_2);
    uapi_pin_set_mode(CMD_UART_RX_PIN, PIN_MODE_2);

    uapi_uart_deinit(CMD_UART_PORT);
    uapi_uart_init(CMD_UART_PORT, &pin_config, &attr, NULL, &g_app_uart_buffer_config);

    (void)uapi_uart_register_rx_callback(CMD_UART_PORT,
                                         UART_RX_CONDITION_FULL_OR_IDLE,
                                         1,
                                         uart_rx_callback);

    osal_printk("[CLIENT] CMD UART init ok, port=%d baud=%d\r\n",
                CMD_UART_PORT, CMD_UART_BAUD);
}

/* ★ 转发用的 UART1 初始化（921600，GPIO15/16） */
static void ForwardUartInit(void)
{
    uapi_pin_init();
    uapi_pin_set_mode(FORWARD_UART_TX_PIN, PIN_MODE_1);
    uapi_pin_set_mode(FORWARD_UART_RX_PIN, PIN_MODE_1);

    uart_attr_t attr = {
        .baud_rate = FORWARD_UART_BAUD,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity    = UART_PARITY_NONE,
    };
    uart_pin_config_t pin_config = {
        .tx_pin  = FORWARD_UART_TX_PIN,
        .rx_pin  = FORWARD_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE,
    };

    uapi_uart_deinit(FORWARD_UART_PORT);
    /* ★ 改动1：传 buffer，不传 NULL */
    errcode_t ret = uapi_uart_init(FORWARD_UART_PORT, &pin_config, &attr, NULL,
                                   &g_forward_uart_buffer_config);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[FORWARD] UART1 init fail ret=%x\r\n", ret);
    } else {
        osal_printk("[FORWARD] UART1 init ok baud=%u\r\n", FORWARD_UART_BAUD);
    }
}

/* ★ 转发裸 HIF 数据到香橙派 */
static void ForwardHifToUart(const uint8_t *data, uint16_t len)
{
    int32_t ret = uapi_uart_write(FORWARD_UART_PORT, data, len, 0);
    /* ★ 超时0时返回实际写出字节数，不是ERRCODE_SUCC */
    if (ret > 0) {
        g_uart_send_cnt++;
        if ((g_uart_send_cnt % 200) == 0) {
            osal_printk("[FORWARD] sent=%u fail=%u\r\n",
                        g_uart_send_cnt, g_uart_fail_cnt);
        }
    } else {
        g_uart_fail_cnt++;
        osal_printk("[FORWARD] uart write fail ret=%x len=%u\r\n", ret, len);
    }
}

void SleUartStartScan(void)
{
    SleSeekParam param = {0};
    param.ownaddrtype = 0;
    param.filterduplicates = 0;
    param.seekfilterpolicy = 0;
    param.seekphys = 1;
    param.seekType[0] = 1;
    param.seekInterval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seekWindow[0] = SLE_SEEK_WINDOW_DEFAULT;
    SleSetSeekParam(&param);
    SleStartSeek();
}

static void sle_uart_client_sample_sle_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_enable_cbk, status error\r\n", SLE_UART_CLIENT_LOG);
    } else {
        osal_msleep(SLE_UART_TASK_DELAY_MS);
        SleUartStartScan();
    }
}

static void sle_uart_client_sample_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s seek_enable_cbk, status error\r\n", SLE_UART_CLIENT_LOG);
    }
}

static void sle_uart_client_sample_seek_result_info_cbk(SleSeekResultInfo *seek_result_data)
{
    if (seek_result_data == NULL) {
        osal_printk("%s seek_result_data NULL\r\n", SLE_UART_CLIENT_LOG);
        return;
    }
    osal_printk("%s scan data :%s\r\n", SLE_UART_CLIENT_LOG, seek_result_data->data);
    if (strstr((const char *)seek_result_data->data, SLE_UART_SERVER_NAME) != NULL) {
        (void)memcpy_s(&g_sle_uart_remote_addr, sizeof(g_sle_uart_remote_addr),
                       &seek_result_data->addr, sizeof(seek_result_data->addr));
        SleStopSeek();
    }
}

static void sle_uart_client_sample_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s seek_disable_cbk, status error = %x\r\n", SLE_UART_CLIENT_LOG, status);
    } else {
        SleConnectRemoteDevice(&g_sle_uart_remote_addr);
    }
}

static void SleUartClientSampleSeekCbkRegister(void)
{
    g_sle_uart_seek_cbk.sle_enable_cb = sle_uart_client_sample_sle_enable_cbk;
    g_sle_uart_seek_cbk.seek_enable_cb = sle_uart_client_sample_seek_enable_cbk;
    g_sle_uart_seek_cbk.seek_result_cb = sle_uart_client_sample_seek_result_info_cbk;
    g_sle_uart_seek_cbk.seek_disable_cb = sle_uart_client_sample_seek_disable_cbk;
    sle_announce_seek_register_callbacks(&g_sle_uart_seek_cbk);
}

static void sle_uart_client_sample_connect_state_changed_cbk(uint16_t conn_id, const SleAddr *addr,
    SleAcbStateType conn_state, SlePairStateType pair_state, SleDiscReasonType disc_reason)
{
    unused(pair_state);
    osal_printk("%s conn state changed disc_reason:0x%x\r\n", SLE_UART_CLIENT_LOG, disc_reason);

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_connected = true;
        g_sle_uart_conn_id = conn_id;

        osal_printk("%s SLE_ACB_STATE_CONNECTED conn_id=%u\r\n",
                    SLE_UART_CLIENT_LOG, conn_id);
        SlePairRemoteDevice(addr);

    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_UART_CLIENT_LOG);

    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_UART_CLIENT_LOG);

        g_sle_connected = false;
        g_sle_uart_conn_id = 0;
        g_sle_uart_send_param.handle = 0;

        SleRemovePairedRemoteDevice(addr);
        SleUartStartScan();

    } else {
        osal_printk("%s status error\r\n", SLE_UART_CLIENT_LOG);
    }
}

static void sle_uart_client_sample_pair_complete_cbk(uint16_t conn_id, const SleAddr *addr,
    errcode_t status)
{
    (void)addr;
    osal_printk("%s pair complete conn_id:%d status:0x%x\r\n",
           SLE_UART_CLIENT_LOG, conn_id, status);
    if (status == ERRCODE_SUCC) {
        SsapcExchangeInfo info = {0};
        info.mtuSize = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        SsapcExchangeInfoReq(0, conn_id, &info);
        osal_printk("%s MTU exchange requested: %u\r\n",
               SLE_UART_CLIENT_LOG, SLE_MTU_SIZE_DEFAULT);
    }
}

static void SleUartClientSampleConnectCbkRegister(void)
{
    g_sle_uart_connect_cbk.connectStateChangedCb = sle_uart_client_sample_connect_state_changed_cbk;
    g_sle_uart_connect_cbk.pairCompleteCb = sle_uart_client_sample_pair_complete_cbk;
    SleConnectionRegisterCallbacks(&g_sle_uart_connect_cbk);
}

static void sle_uart_client_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id,
    ssap_exchange_info_t *param, errcode_t status)
{
    osal_printk("%s exchange_info_cbk client:%d status:%d mtu:%d ver:%d\r\n",
           SLE_UART_CLIENT_LOG, client_id, status, param->mtu_size, param->version);

    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;

    int ret = ssapc_find_structure(client_id, conn_id, &find_param);
    osal_printk("[CLIENT] ssapc_find_structure(property) ret=%d conn=%d\r\n", ret, conn_id);
}

static void sle_uart_client_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_service_result_t *service, errcode_t status)
{
    osal_printk("%s find_structure_cbk client:%d conn:%d status:%d\r\n",
           SLE_UART_CLIENT_LOG, client_id, conn_id, status);

    g_sle_uart_find_service_result.start_hdl = service->start_hdl;
    g_sle_uart_find_service_result.end_hdl = service->end_hdl;
    memcpy_s(&g_sle_uart_find_service_result.uuid, sizeof(sle_uuid_t),
             &service->uuid, sizeof(sle_uuid_t));

    osal_printk("[CLIENT] service range: start=0x%x end=0x%x\r\n",
                service->start_hdl, service->end_hdl);

}

static void sle_uart_client_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("[CLIENT] find_property_cbk client=%d conn=%d status=%d handle=0x%x op_ind=%d desc_cnt=%d\r\n",
                client_id, conn_id, status,
                property->handle,
                property->operate_indication,
                property->descriptors_count);

    g_sle_uart_send_param.handle = property->handle;
    g_sle_uart_send_param.type = SSAP_PROPERTY_TYPE_VALUE;

    osal_printk("[CLIENT] save write handle=0x%x\r\n", g_sle_uart_send_param.handle);
}

static void sle_uart_client_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    unused(conn_id);
    osal_printk("%s find_structure_cmp_cbk client:%d status:%d type:%d\r\n",
           SLE_UART_CLIENT_LOG, client_id, status, structure_result->type);
}

static void sle_uart_client_sample_write_cfm_cb(uint8_t client_id, uint16_t conn_id,
    ssapc_write_result_t *write_result, errcode_t status)
{
    osal_printk("%s write_cfm conn:%d client:%d status:%d handle:%02x\r\n",
           SLE_UART_CLIENT_LOG, conn_id, client_id, status, write_result->handle);
}

/* ============ notification 回调 (避让核心) ============ */
void ssapc_notification_callbacks(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status) {
    (void)client_id; 
    (void)conn_id;

    /* ★ 避让：如果处于报警期，直接丢弃星闪通知数据 */
    if (g_alarm_active_pause) return;

    if (status != ERRCODE_SUCC || data == NULL || data->data == NULL || data->data_len < RADAR_SLE_HDR_LEN) return;

    radar_sle_hdr_t h;
    (void)memcpy_s(&h, sizeof(h), data->data, RADAR_SLE_HDR_LEN);
    if (h.magic != RADAR_SLE_MAGIC) return;

    if (h.type == RADAR_SLE_TYPE_FRAME_END) {
        g_fe_cnt++;
        if ((g_fe_cnt % 50) == 0) osal_printk("[CLIENT] FRAME_END x%u ok=%u drop=%u\r\n", g_fe_cnt, g_ok_cnt, g_drop_cnt);
        radar_rx_reset(); return;
    }

    if (h.flags & RADAR_SLE_FLAG_FIRST) {
        if (g_rx_in_progress) g_drop_cnt++;
        g_rx_seq = h.seq; g_rx_expect = h.total_len; g_rx_next_off = 0; g_rx_in_progress = true;
    }

    if (!g_rx_in_progress || h.seq != g_rx_seq || h.offset != g_rx_next_off) return;

    (void)memcpy_s(g_rx_buf + h.offset, RADAR_MAX_HIF_PDU - h.offset, (uint8_t *)data->data + RADAR_SLE_HDR_LEN, h.chunk_len);
    g_rx_next_off = (uint16_t)(g_rx_next_off + h.chunk_len);

    if ((h.flags & RADAR_SLE_FLAG_LAST) && g_rx_next_off == g_rx_expect) {
        g_ok_cnt++;
        if (g_frame_q != NULL) {
            uint8_t idx = g_frame_widx; g_frame_widx = (uint8_t)((g_frame_widx + 1) % FRAME_SLOT_NUM);
            g_frame_len[idx] = g_rx_expect; (void)memcpy_s(g_frame_buf[idx], RADAR_MAX_HIF_PDU, g_rx_buf, g_rx_expect);
            frame_q_item_t item = { .idx = idx, .len = g_rx_expect }; (void)osMessageQueuePut(g_frame_q, &item, 0, 0);
        }
        if ((g_ok_cnt % 50) == 0) osal_printk("[CLIENT] HIF OK x%u drop=%u\r\n", g_ok_cnt, g_drop_cnt);
        radar_rx_reset();
    }
}

void ssapc_indication_callbacks(uint8_t client_id, uint16_t conn_id,
                                ssapc_handle_value_t *data, errcode_t status)
{
    (void)client_id; (void)conn_id; (void)data; (void)status;
}

static void sle_uart_client_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
                                                      ssapc_indication_callback indication_cb)
{
    g_sle_uart_ssapc_cbk.exchange_info_cb      = sle_uart_client_sample_exchange_info_cbk;
    g_sle_uart_ssapc_cbk.find_structure_cb     = sle_uart_client_sample_find_structure_cbk;
    g_sle_uart_ssapc_cbk.ssapc_find_property_cbk = sle_uart_client_sample_find_property_cbk;
    g_sle_uart_ssapc_cbk.find_structure_cmp_cb = sle_uart_client_sample_find_structure_cmp_cbk;
    g_sle_uart_ssapc_cbk.write_cfm_cb          = sle_uart_client_sample_write_cfm_cb;
    g_sle_uart_ssapc_cbk.notification_cb       = notification_cb;
    g_sle_uart_ssapc_cbk.indication_cb         = indication_cb;
    ssapc_register_callbacks(&g_sle_uart_ssapc_cbk);
}

static errcode_t sle_uuid_client_register(void)
{
    SleUuid app_uuid = {0};
    app_uuid.len = sizeof(g_sleUuidAppUuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sleUuidAppUuid, sizeof(g_sleUuidAppUuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return ssapc_register_client(&app_uuid, &g_client_id);
}

errcode_t sle_uart_client_send_report_by_handle(const uint8_t *data, uint8_t len)
{
    /* 基本判断 */
    if (data == NULL || len == 0 || !g_sle_connected) {
        osal_printk("[CLIENT] send abort: data null or not connected\r\n");
        return ERRCODE_FAIL;
    }

    /* 打印调试信息 */
    osal_printk("[CLIENT] send try: conn=%u handle=0x2 len=%u\r\n",
                g_sle_uart_conn_id, len);

    /* 准备参数 */
    SsapcWriteParam param = {0};
    param.handle = 0x2;               // 强制指定 handle 0x2
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data = receive_buf;         // 使用我们那个 4 字节对齐的全局数组
    param.dataLen = len;

    /* 安全拷贝数据到发送缓冲区 */
    /* 注意：第二个参数填 receive_buf 的总大小 1500 */
    if (memcpy_s(receive_buf, 1500, data, len) != EOK) {
        osal_printk("[CLIENT] send abort: memcpy fail\r\n");
        return ERRCODE_FAIL;
    }

    errcode_t ret = SsapcWriteReq(g_client_id, g_sle_uart_conn_id, &param);
    
    osal_printk("[CLIENT] write ret=%d handle=0x2\r\n", ret);
    return ret;
}

int uart_sle_client_send_data(uint8_t *data, uint8_t length)
{
    osal_mdelay(DELAY_100MS);
    return sle_uart_client_send_report_by_handle(data, length);
}

void SleUartClientInit(void)
{
    uint8_t local_addr[SLE_ADDR_LEN] = {0x13, 0x67, 0x5c, 0x07, 0x00, 0x51};
    SleAddr local_address;
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_uuid_client_register();
    SleUartClientSampleSeekCbkRegister();
    SleUartClientSampleConnectCbkRegister();
    sle_uart_client_sample_ssapc_cbk_register(ssapc_notification_callbacks,
                                              ssapc_indication_callbacks);
    EnableSle();
    SleSetLocalAddr(&local_address);
}

static void SleTask(void *arg)
{
    (void)arg;

    osal_msleep(1000);
    UartInitConfig();
    ForwardUartInit();
    SleUartClientInit();

    if (g_frame_q == NULL) {
        g_frame_q = osMessageQueueNew(8, sizeof(frame_q_item_t), NULL);
        if (g_frame_q == NULL) {
            osal_printk("[CLIENT] frame queue create failed\r\n");
        } else {
            osal_printk("[CLIENT] frame queue ok\r\n");
        }
    }

    if (g_alarm_cmd_q == NULL) {
        g_alarm_cmd_q = osMessageQueueNew(4, sizeof(alarm_cmd_item_t), NULL);
        if (g_alarm_cmd_q == NULL) {
            osal_printk("[CLIENT] alarm cmd queue create failed\r\n");
        } else {
            osal_printk("[CLIENT] alarm cmd queue ok\r\n");
        }
    }

    while (1) {
        /* 检查报警恢复计时 */
        if (g_alarm_active_pause) {
            if (osKernelGetTickCount() >= g_alarm_resume_tick) {
                g_alarm_active_pause = false;
                osal_printk("[CLIENT] Alarm period end, radar data RESUMED.\r\n");
            }
        }

        /* 1. 处理来自香橙派的报警命令 */
        if (g_alarm_cmd_q != NULL) {
            alarm_cmd_item_t cmd_item;
            if (osMessageQueueGet(g_alarm_cmd_q, &cmd_item, NULL, 0) == osOK) {
                
                /* ★ 优化1：先暂停，设置更长的静默期 */
                g_alarm_active_pause = true;
                g_alarm_resume_tick = osKernelGetTickCount() + (30 * 1000); 
                
                osal_printk("[CLIENT] Alarm start: STOPPING RADAR...\r\n");
                
                /* ★ 优化2：增加等待时间到 200ms，确保底层硬件 DMA 传输彻底完成 */
                osal_msleep(200); 

                /* ★ 优化3：发送前清理掉雷达队列里的存量，防止任务切换时干扰 */
                frame_q_item_t dummy;
                while (osMessageQueueGet(g_frame_q, &dummy, NULL, 0) == osOK) {
                    /* 只拿出来，不处理，彻底排空 */
                }

                errcode_t ret = sle_uart_client_send_report_by_handle(cmd_item.data, cmd_item.len);
                if (ret != 0) {
                    osal_printk("[CLIENT] alarm cmd send fail:%d\r\n", ret);
                } else {
                    osal_printk("[CLIENT] alarm cmd forwarded successfully\r\n");
                }
            }
        }

        /* 2. 处理雷达数据转发 (受避让标志控制) */
        if (g_frame_q != NULL) {
            frame_q_item_t item;
            if (osMessageQueueGet(g_frame_q, &item, NULL, 10) == osOK) {
                
                /* ★ 核心避让：如果正在报警，直接把雷达包从队列拿出来丢掉，不写��口也不做复杂处理 */
                if (g_alarm_active_pause) {
                    continue; 
                }

                uint8_t *p = g_frame_buf[item.idx];
                uint16_t len = item.len;
                ForwardHifToUart(p, len);

                g_frame_print_cnt++;
                if ((g_frame_print_cnt % 50) == 0) {
                    osal_printk("[CLIENT] FRAME x%u len=%u hdr=%02X %02X %02X %02X %02X %02X\r\n",
                                g_frame_print_cnt, (unsigned)len,
                                p[0], p[1], p[2], p[3], p[4], p[5]);
                }
            }
        }

        osal_msleep(1);
    }
}

static void SleClientExample(void)
{
    osThreadAttr_t attr = {0};
    attr.name       = "SleTask";
    attr.stack_size = TASK_SIZE;
    attr.priority   = PRIO;

    if (osThreadNew(SleTask, NULL, &attr) == NULL) {
        osal_printk(" Failed to create SleTask!\n");
    } else {
        osal_printk(" create SleTask successfully!\n");
    }
}

SYS_RUN(SleClientExample);