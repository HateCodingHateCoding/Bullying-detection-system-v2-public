/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: sle uuid server sample.
 */
#include "app_init.h"
#include "watchdog.h"
#include "tcxo.h"
#include "systick.h"
#include "los_memory.h"
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "soc_osal.h"
#include "common_def.h"

#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_transmition_manager.h"
#include "nv.h"
#include "pinctrl.h"
#include "uart.h"

#include "../inc/sle_speed_server_adv.h"
#include "../inc/sle_speed_server.h"

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define BT_INDEX_4 4
#define BT_INDEX_5 5
#define BT_INDEX_0 0
extern void send_data_thread_function(void);

/* sle server app uuid for test */
static char g_sle_uuid_app_uuid[UUID_LEN_2] = {0x0, 0x0};
/* server notify property uuid for test */
static char g_sle_property_value[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/* sle connect acb handle */
static uint16_t g_sle_conn_hdl = 0;
/* sle server handle */
static uint8_t g_server_id = 0;
/* sle service handle */
static uint16_t g_service_handle = 0;
/* sle ntf property handle */
static uint16_t g_property_handle = 0;
#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
static sle_link_qos_state_t g_sle_link_state = 0; /* sle link state */
#endif

#ifdef CONFIG_LARGE_THROUGHPUT_SERVER
#define PKT_DATA_LEN 1400                          /* 增大包长，接近MTU上限 */
#define SPEED_DEFAULT_CONN_INTERVAL 0x14           /* 连接间隔2.5ms */
#else
#define PKT_DATA_LEN 600
#define SPEED_DEFAULT_CONN_INTERVAL 0x14
#endif


#define SPEED_DEFAULT_KTHREAD_SIZE 0x2000
#define SPEED_DEFAULT_KTHREAD_PROI 24              /* 提高任务优先级 */
#define DEFAULT_SLE_SPEED_DATA_LEN 1500
#define DEFAULT_SLE_SPEED_MTU_SIZE 1500
#define SPEED_DEFAULT_TIMEOUT_MULTIPLIER 0x1f4
#define SPEED_DEFAULT_SCAN_INTERVAL 400
#define SPEED_DEFAULT_SCAN_WINDOW 20
#define BURST_SEND_COUNT 12                        /* 增大批量发送数量 */

/* UART1 配置：TX=GPIO15, RX=GPIO16, 波特率921600 */
#define UART1_BUS            UART_BUS_1
#define UART1_BAUDRATE       921600
#define UART1_TX_PIN         GPIO_15
#define UART1_RX_PIN         GPIO_16
#define UART1_PIN_MODE       PIN_MODE_1
#define UART1_RX_BUF_SIZE    1500

/* 音频帧包头包尾 */
#define AUDIO_HEADER         0x55AA
#define AUDIO_TAIL           0xAA55
#define AUDIO_FRAME_HDR_LEN  6      /* header(2) + seq(2) + payload_len(2) */
#define AUDIO_FRAME_TAIL_LEN 2
#define AUDIO_FRAME_MIN_LEN  (AUDIO_FRAME_HDR_LEN + AUDIO_FRAME_TAIL_LEN)

/* 消息队列 */
#define UART_QUEUE_LEN       32
#define UART_QUEUE_BUF_SIZE  UART1_RX_BUF_SIZE

typedef struct {
    uint8_t  data[UART_QUEUE_BUF_SIZE];
    uint16_t len;
} uart_msg_t;

// static unsigned char g_data[PKT_DATA_LEN];
static uint8_t g_uart1_rx_buf[UART1_RX_BUF_SIZE] = { 0 };
static uart_buffer_config_t g_uart1_buf_config = {
    .rx_buffer = g_uart1_rx_buf,
    .rx_buffer_size = UART1_RX_BUF_SIZE
};
static unsigned long g_uart_queue_id = 0;

/* 帧重组缓冲区 */
#define REASSEMBLE_BUF_SIZE  (UART1_RX_BUF_SIZE * 4)
static uint8_t  g_reassemble_buf[REASSEMBLE_BUF_SIZE];
static uint16_t g_reassemble_len = 0;

/* 统计 */
static uint32_t g_uart_rx_frm_ok  = 0;   /* 成功入队帧数 */
static uint32_t g_uart_rx_frm_drop = 0;  /* 丢弃帧数 */

static void uart1_rx_callback(const void *buffer, uint16_t length, bool error)
{
    if (error || buffer == NULL || length == 0) {
        return;
    }

    const uint8_t *buf = (const uint8_t *)buffer;

    /* 追加到重组缓冲区 */
    if (g_reassemble_len + length > REASSEMBLE_BUF_SIZE) {
        /* 溢出：丢弃旧数据，重新开始 */
        g_reassemble_len = 0;
    }
    if (memcpy_s(g_reassemble_buf + g_reassemble_len,
                 REASSEMBLE_BUF_SIZE - g_reassemble_len,
                 buf, length) != EOK) {
        return;
    }
    g_reassemble_len += length;

    /* 在重组缓冲区中尝试提取完整帧 */
    uint16_t pos = 0;
    while (pos + AUDIO_FRAME_MIN_LEN <= g_reassemble_len) {
        /* 查找帧头 0x55AA（小端：buf[0]=0xAA, buf[1]=0x55 -> 读出 0x55AA） */
        uint16_t hdr;
        memcpy(&hdr, g_reassemble_buf + pos, 2);
        if (hdr != AUDIO_HEADER) {
            pos++;
            continue;
        }
        /* 读 payload_len */
        if (pos + 6 > g_reassemble_len) {
            break;  /* 头部还没收完，等更多数据 */
        }
        uint16_t payload_len;
        memcpy(&payload_len, g_reassemble_buf + pos + 4, 2);
        uint16_t frame_total = (uint16_t)(6 + payload_len + 2); /* hdr+seq+plen + payload + tail */
        if (frame_total > UART_QUEUE_BUF_SIZE) {
            /* 帧长异常，跳过这个包头 */
            pos++;
            continue;
        }
        if (pos + frame_total > g_reassemble_len) {
            break;  /* 帧还没收完，等更多数据 */
        }
        /* 校验帧尾 */
        uint16_t tail;
        memcpy(&tail, g_reassemble_buf + pos + 6 + payload_len, 2);
        if (tail != AUDIO_TAIL) {
            pos++;
            continue;
        }
        /* 完整帧，入队 */
        if (g_uart_queue_id != 0) {
            uart_msg_t msg;
            if (memcpy_s(msg.data, UART_QUEUE_BUF_SIZE,
                         g_reassemble_buf + pos, frame_total) == EOK) {
                msg.len = frame_total;
                uint32_t ret = osal_msg_queue_write_copy(g_uart_queue_id, &msg,
                                                         sizeof(uart_msg_t), 0);
                if (ret == OSAL_SUCCESS) {
                    g_uart_rx_frm_ok++;
                } else {
                    g_uart_rx_frm_drop++;
                }
            }
        }
        pos += frame_total;
    }

    /* 保留未处理的尾部数据 */
    if (pos > 0 && pos < g_reassemble_len) {
        g_reassemble_len -= pos;
        memmove(g_reassemble_buf, g_reassemble_buf + pos, g_reassemble_len);
    } else if (pos >= g_reassemble_len) {
        g_reassemble_len = 0;
    }

    /* 每100帧打印一次统计（无论成功还是丢弃都累计） */
    static uint32_t s_last_print = 0;
    uint32_t total = g_uart_rx_frm_ok + g_uart_rx_frm_drop;
    if (total > 0 && total - s_last_print >= 100) {
        s_last_print = total;
        osal_printk("[uart1] 统计: 成功入队=%lu 丢弃=%lu 缓冲剩余=%u\r\n",
                    g_uart_rx_frm_ok, g_uart_rx_frm_drop, g_reassemble_len);
    }
}

static void uart1_init(void)
{
    uapi_pin_set_mode(UART1_TX_PIN, UART1_PIN_MODE);
    uapi_pin_set_mode(UART1_RX_PIN, UART1_PIN_MODE);

    uart_attr_t attr = {
        .baud_rate = UART1_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity    = UART_PARITY_NONE
    };
    uart_pin_config_t pin_config = {
        .tx_pin  = UART1_TX_PIN,
        .rx_pin  = UART1_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uapi_uart_deinit(UART1_BUS);
    uapi_uart_init(UART1_BUS, &pin_config, &attr, NULL, &g_uart1_buf_config);
    uapi_uart_register_rx_callback(UART1_BUS,
                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   1, uart1_rx_callback);
    osal_printk("[uart1] 初始化完成，波特率921600，TX=GPIO15，RX=GPIO16\r\n");
}

void encode2byte_little(uint8_t* _ptr, uint16_t data)
{
    do {
        *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 8);
        *(uint8_t *)(_ptr) = (uint8_t)(data);
    } while (0);
}

static uint8_t sle_uuid_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                  0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static void sle_uuid_set_base(sle_uuid_t *out)
{
    (void)memcpy_s(out->uuid, SLE_UUID_LEN, sle_uuid_base, SLE_UUID_LEN);
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[14], u2);
}

static void ssaps_read_request_cbk(uint8_t server_id,
                                   uint16_t conn_id,
                                   ssaps_req_read_cb_t *read_cb_para,
                                   errcode_t status)
{
    osal_printk("[speed server] ssaps read request cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n", server_id,
                conn_id, read_cb_para->handle, status);
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)send_data_thread_function, 0, "RadarTask",
                                      SPEED_DEFAULT_KTHREAD_SIZE);
    osal_kthread_set_priority(task_handle, SPEED_DEFAULT_KTHREAD_PROI + 1);
    if (task_handle != NULL) {
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
    printf("kthread success\r\n");
}

static void ssaps_write_request_cbk(uint8_t server_id,
                                    uint16_t conn_id,
                                    ssaps_req_write_cb_t *write_cb_para,
                                    errcode_t status)
{
    osal_printk("[speed server] ssaps write request cbk server_id:%d, conn_id:%d, handle:%d, status:%d\r\n", server_id,
                conn_id, write_cb_para->handle, status);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size, errcode_t status)
{
    osal_printk("[speed server] ssaps write request cbk server_id:%d, conn_id:%d, mtu_size:%d, status:%d\r\n",
                server_id, conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[speed server] start service cbk server_id:%d, handle:%d, status:%d\r\n", server_id, handle, status);
}

static void sle_ssaps_register_cbks(void)
{
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_request_cbk;
    ssaps_cbk.write_request_cb = ssaps_write_request_cbk;
    ssaps_register_callbacks(&ssaps_cbk);
}

errcode_t sle_uuid_server_send_report_by_handle_id(uint8_t *data, uint16_t len, uint16_t connect_id)
{
    ssaps_ntf_ind_t param = {0};
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = data;
    param.value_len = len;
    ssaps_notify_indicate(g_server_id, connect_id, &param);
    return ERRCODE_SLE_SUCCESS;
}

#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
static void sle_send_data_cbk(uint16_t conn_id, sle_link_qos_state_t link_state)
{
    conn_id = conn_id;
    g_sle_link_state = link_state;
    printf("%s enter, gle_tx_acb_data_num_get:%d.\n", __FUNCTION__, link_state);
}

static void sle_transmission_register_cbks(void)
{
    sle_transmission_callbacks_t trans_cbk = {0};
    trans_cbk.send_data_cb = sle_send_data_cbk;
    sle_transmission_register_callbacks(&trans_cbk);
}
#else
extern uint8_t gle_tx_acb_data_num_get(void);
#endif

uint8_t sle_flow_ctrl_flag(void)
{
#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
    return (g_sle_link_state <= SLE_QOS_FLOWCTRL) ? 1 : 0;
#else
    return gle_tx_acb_data_num_get();
#endif
}


void send_data_thread_function(void)
{
    sle_set_data_len(g_sle_conn_hdl, DEFAULT_SLE_SPEED_DATA_LEN);
#ifdef CONFIG_LARGE_THROUGHPUT_SERVER
#define DEFAULT_SLE_SPEED_MCS 10
    sle_set_phy_t phy_parm = {
        .tx_format = SLE_RADIO_FRAME_2,
        .rx_format = SLE_RADIO_FRAME_2,
        .tx_phy = SLE_PHY_4M,
        .rx_phy = SLE_PHY_4M,
        .tx_pilot_density = SLE_PHY_PILOT_DENSITY_NO,
        .rx_pilot_density = SLE_PHY_PILOT_DENSITY_NO,
        .g_feedback = 0,
        .t_feedback = 0,
    };
    sle_set_phy_param(g_sle_conn_hdl, &phy_parm);
    sle_set_mcs(g_sle_conn_hdl, DEFAULT_SLE_SPEED_MCS);
    osal_printk("[uart server] polar MCS10, PHY 4MHZ, NO_PILOT\r\n");
#else
    osal_printk("[uart server] GFSK, PHY 1MHZ\r\n");
#endif

    uart_msg_t msg;
    uint32_t msg_size = sizeof(uart_msg_t);
    uint32_t sle_tx_ok = 0;
    uint32_t sle_tx_fail = 0;

    while (1) {
        /* 从消息队列阻塞读取 UART 数据帧 */
        uint32_t ret = osal_msg_queue_read_copy(g_uart_queue_id, &msg, &msg_size,
                                                OSAL_WAIT_FOREVER);
        msg_size = sizeof(uart_msg_t); /* 每次读完重置 */
        if (ret != OSAL_SUCCESS || msg.len == 0) {
            continue;
        }

        /* 等待流控允许发送，不丢帧 */
        while (sle_flow_ctrl_flag() == 0) {
            osal_udelay(100);
        }

        errcode_t sle_ret = sle_uuid_server_send_report_by_handle_id(
                                (uint8_t *)msg.data, msg.len, g_sle_conn_hdl);
        if (sle_ret != ERRCODE_SLE_SUCCESS) {
            sle_tx_fail++;
        } else {
            sle_tx_ok++;
        }

        /* 每100帧打印一次统计 */
        if ((sle_tx_ok + sle_tx_fail) % 100 == 0 && (sle_tx_ok + sle_tx_fail) > 0) {
            osal_printk("[uart server] SLE统计: 成功=%lu 失败=%lu\r\n", sle_tx_ok, sle_tx_fail);
        }
    }
}

static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[speed server] sle uuid add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SLE_UUID_TEST_PROPERTIES;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.value = osal_vmalloc(sizeof(g_sle_property_value));
    property.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY;
    if (property.value == NULL) {
        osal_printk("[speed server] sle property mem fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value, sizeof(g_sle_property_value)) !=
        EOK) {
        osal_vfree(property.value);
        osal_printk("[speed server] sle property mem cpy fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[speed server] sle uuid add property fail, ret:%x\r\n", ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);

    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[speed server] sle uuid add descriptor fail, ret:%x\r\n", ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}


static errcode_t sle_uuid_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    osal_printk("[speed server] sle uuid add service in\r\n");
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }

    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[speed server] sle uuid add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
                g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[speed server] sle uuid add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[speed server] sle uuid add service out\r\n");
    return ERRCODE_SLE_SUCCESS;
}

static bool g_send_thread_started = false;

static void sle_connect_state_changed_cbk(uint16_t conn_id,
                                          const sle_addr_t *addr,
                                          sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state,
                                          sle_disc_reason_t disc_reason)
{
    osal_printk(
        "[speed server] connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
        disc_reason:0x%x\r\n",
        conn_id, conn_state, pair_state, disc_reason);
    osal_printk("[speed server] connect state changed addr:%02x:**:**:**:%02x:%02x\r\n", addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_sle_conn_hdl = conn_id;
    /*
    sle_connection_param_update_t parame = {0};
    parame.conn_id = conn_id;
    parame.interval_min = SPEED_DEFAULT_CONN_INTERVAL;
    parame.interval_max = SPEED_DEFAULT_CONN_INTERVAL;
    parame.max_latency = 0;
    parame.supervision_timeout = SPEED_DEFAULT_TIMEOUT_MULTIPLIER;
    */
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        // sle_update_connect_param(&parame);
        sle_stop_announce(SLE_ADV_HANDLE_DEFAULT);
        /* 已配对直接启动，未配对等pair_complete再启动 */
        if (pair_state == SLE_PAIR_PAIRED && !g_send_thread_started) {
            g_send_thread_started = true;
            uart1_init();
            osal_task *task_handle = NULL;
            osal_kthread_lock();
            task_handle = osal_kthread_create((osal_kthread_handler)send_data_thread_function, 0, "SendTask",
                                              SPEED_DEFAULT_KTHREAD_SIZE);
            if (task_handle != NULL) {
                osal_kthread_set_priority(task_handle, SPEED_DEFAULT_KTHREAD_PROI);
                osal_kfree(task_handle);
                osal_printk("[speed server] send thread started (already paired)\r\n");
            }
            osal_kthread_unlock();
        }
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_send_thread_started = false;
        /* 清空消息队列，避免重连后发送旧数据 */
        if (g_uart_queue_id != 0) {
            uart_msg_t tmp;
            uint32_t tmp_size = sizeof(uart_msg_t);
            while (osal_msg_queue_read_copy(g_uart_queue_id, &tmp, &tmp_size, 0) == OSAL_SUCCESS) {
                tmp_size = sizeof(uart_msg_t);
            }
        }
        sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    }
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[speed server] pair complete conn_id:%02x, status:%x\r\n", conn_id, status);
    osal_printk("[speed server] pair complete addr:%02x:**:**:**:%02x:%02x\r\n", addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    if (status != 0 || g_send_thread_started) {
        return;
    }
    g_send_thread_started = true;
    /* 首次配对后初始化UART并启动发送线程 */
    uart1_init();
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)send_data_thread_function, 0, "SendTask",
                                      SPEED_DEFAULT_KTHREAD_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SPEED_DEFAULT_KTHREAD_PROI);
        osal_kfree(task_handle);
        osal_printk("[speed server] send thread started (new pair)\r\n");
    }
    osal_kthread_unlock();
}

void sle_sample_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(status);
    osal_printk("[ssap server] updat state changed conn_id:%d, interval = %02x\n", conn_id, param->interval);
}

void sle_sample_update_req_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    osal_printk("[ssap server] sle_sample_update_req_cbk interval_min:%02x, interval_max:%02x\n", param->interval_min,
                param->interval_max);
}

void sle_sample_rssi_cbk(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    osal_printk("[ssap server] conn_id:%d, rssi = %c, status = %x\n", conn_id, rssi, status);
}

static void sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    conn_cbks.connect_param_update_req_cb = sle_sample_update_req_cbk;
    conn_cbks.connect_param_update_cb = sle_sample_update_cbk;
    conn_cbks.read_rssi_cb = sle_sample_rssi_cbk;
    sle_connection_register_callbacks(&conn_cbks);
}

void sle_ssaps_set_info(void)
{
    ssap_exchange_info_t info = {0};
    info.mtu_size = DEFAULT_SLE_SPEED_MTU_SIZE;
    info.version = 1;
    ssaps_set_info(g_server_id, &info);
}

void sle_speed_connect_param_init(void)
{
    sle_default_connect_param_t param = {0};
    param.enable_filter_policy = 0;
    param.gt_negotiate = 0;
    param.initiate_phys = 1;
    param.max_interval = SPEED_DEFAULT_CONN_INTERVAL;
    param.min_interval = SPEED_DEFAULT_CONN_INTERVAL;
    param.scan_interval = SPEED_DEFAULT_SCAN_INTERVAL;
    param.scan_window = SPEED_DEFAULT_SCAN_WINDOW;
    param.timeout = SPEED_DEFAULT_TIMEOUT_MULTIPLIER;
    sle_default_connection_param_set(&param);
}

void sle_set_local_addr_init(void)
{
    sle_addr_t addr = {0};
    uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    addr.type = 0;
    memcpy_s(addr.addr, SLE_ADDR_LEN, mac, SLE_ADDR_LEN);
    sle_set_local_addr(&addr);
}

void sle_speed_server_set_nv(void)
{
    uint16_t nv_value_len = 0;
    uint8_t nv_value = 0;
    uapi_nv_read(0x20A0, sizeof(uint16_t), &nv_value_len, &nv_value);
    if (nv_value != 7) { // 7:btc功率档位
        nv_value = 7;    // 7:btc功率档位
        uapi_nv_write(0x20A0, (uint8_t *)&(nv_value), sizeof(nv_value));
    }
    osal_printk("[speed server] The value of nv is set to %d.\r\n", nv_value);
}


/* 初始化speed server */
errcode_t sle_speed_server_init(void)
{
    uapi_watchdog_disable();
    enable_sle();
    printf("sle enable\r\n");
    sle_speed_server_set_nv();
    sle_conn_register_cbks();
    sle_ssaps_register_cbks();
    sle_uuid_server_add();
    sle_uuid_server_adv_init();
    sle_ssaps_set_info();
#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
    sle_transmission_register_cbks();
#endif
    sle_speed_connect_param_init();
    sle_set_local_addr_init();
    osal_printk("[speed server] init ok\r\n");
    return ERRCODE_SLE_SUCCESS;
}

int sle_speed_init(void)
{
    osal_msleep(1000); /* sleep 1000ms */

    /* 创建消息队列：最多存 UART_QUEUE_LEN 条，每条 sizeof(uart_msg_t) */
    uint32_t ret = osal_msg_queue_create("uart_sle_q", UART_QUEUE_LEN,
                                         &g_uart_queue_id, 0, sizeof(uart_msg_t));
    if (ret != OSAL_SUCCESS) {
        osal_printk("[server] 消息队列创建失败 ret=%u\r\n", ret);
        return -1;
    }
    osal_printk("[server] 消息队列创建成功\r\n");

    /* uart1_init() 延迟到配对成功后调用，避免连接前接收数据 */
    sle_speed_server_init();
    return 0;
}

static void sle_speed_entry(void)
{
    osal_task *task_handle1 = NULL;
    osal_kthread_lock();
    task_handle1 = osal_kthread_create((osal_kthread_handler)sle_speed_init, 0, "speed", SPEED_DEFAULT_KTHREAD_SIZE);
    if (task_handle1 != NULL) {
        osal_kthread_set_priority(task_handle1, SPEED_DEFAULT_KTHREAD_PROI);
        osal_kfree(task_handle1);
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(sle_speed_entry);
