/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE private service register sample of client.
 */
#include "app_init.h"
#include "systick.h"
#include "tcxo.h"
#include "los_memory.h"
#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_transmition_manager.h"
#include "pinctrl.h"
#include "uart.h"

#include "../inc/sle_speed_client.h"

/* -------- UART1 配置 -------- */
#define CLIENT_UART_BUS        UART_BUS_1
#define CLIENT_UART_BAUD       921600
#define CLIENT_UART_TX_PIN     GPIO_15
#define CLIENT_UART_RX_PIN     GPIO_16
#define CLIENT_UART_PIN_MODE   PIN_MODE_1
#define CLIENT_UART_RX_BUFSZ   1500

/* -------- 丢包检测 -------- */
#define FRAME_HEADER           0x55AA
#define FRAME_TAIL             0xAA55
#define FRAME_HDR_SIZE         8      /* header(2)+seq(2)+len(2)+tail(2) */

/* -------- UART发送队列 -------- */
#define UART_TX_QUEUE_LEN      16
#define UART_TX_BUF_SIZE       1500
typedef struct {
    uint8_t  data[UART_TX_BUF_SIZE];
    uint16_t len;
} uart_tx_msg_t;
static unsigned long g_uart_tx_queue_id = 0;

static uint8_t  g_client_uart_rxbuf[CLIENT_UART_RX_BUFSZ];
static uart_buffer_config_t g_client_uart_buf = {
    .rx_buffer      = g_client_uart_rxbuf,
    .rx_buffer_size = CLIENT_UART_RX_BUFSZ
};

static int32_t  g_expect_seq   = -1;   /* 期望的下一个序列号，-1表示未初始化 */
static uint32_t g_lost_pkts    = 0;
static uint32_t g_total_rx_pkts = 0;

static void client_uart1_init(void)
{
    uapi_pin_set_mode(CLIENT_UART_TX_PIN, CLIENT_UART_PIN_MODE);
    uapi_pin_set_mode(CLIENT_UART_RX_PIN, CLIENT_UART_PIN_MODE);
    uart_attr_t attr = {
        .baud_rate = CLIENT_UART_BAUD,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity    = UART_PARITY_NONE
    };
    uart_pin_config_t pin = {
        .tx_pin  = CLIENT_UART_TX_PIN,
        .rx_pin  = CLIENT_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };
    uapi_uart_deinit(CLIENT_UART_BUS);
    uapi_uart_init(CLIENT_UART_BUS, &pin, &attr, NULL, &g_client_uart_buf);
    osal_printk("[client] UART1 初始化完成，波特率921600，TX=GPIO15，RX=GPIO16\r\n");
}

static void uart_tx_thread_function(void)
{
    uart_tx_msg_t msg;
    uint32_t msg_size = sizeof(uart_tx_msg_t);
    uint32_t tx_ok = 0;
    uint32_t tx_fail = 0;
    while (1) {
        uint32_t ret = osal_msg_queue_read_copy(g_uart_tx_queue_id, &msg, &msg_size, OSAL_WAIT_FOREVER);
        msg_size = sizeof(uart_tx_msg_t);
        if (ret != OSAL_SUCCESS || msg.len == 0) {
            continue;
        }
        int32_t uart_ret = uapi_uart_write(CLIENT_UART_BUS, msg.data, msg.len, 1000);
        if (uart_ret > 0) {
            tx_ok++;
        } else {
            tx_fail++;
            if (tx_fail <= 3) {
                osal_printk("[client] UART发送失败 ret=%d len=%u\r\n", uart_ret, msg.len);
            }
        }
        if ((tx_ok + tx_fail) % 100 == 0 && (tx_ok + tx_fail) > 0) {
            osal_printk("[client] UART发送统计: 成功=%lu 失败=%lu\r\n", tx_ok, tx_fail);
        }
    }
}

#undef THIS_FILE_ID
#define THIS_FILE_ID BTH_GLE_SAMPLE_UUID_CLIENT

#define SLE_MTU_SIZE_DEFAULT        1500
#define SLE_SEEK_INTERVAL_DEFAULT   100
#define SLE_SEEK_WINDOW_DEFAULT     100
#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16
#define SPEED_DEFAULT_CONN_INTERVAL 0x14
#define SPEED_DEFAULT_TIMEOUT_MULTIPLIER 0x1f4
#define SPEED_DEFAULT_SCAN_INTERVAL 400
#define SPEED_DEFAULT_SCAN_WINDOW 20

static sle_announce_seek_callbacks_t g_seek_cbk = {0};
static sle_connection_callbacks_t    g_connect_cbk = {0};
static ssapc_callbacks_t             g_ssapc_cbk = {0};
static sle_addr_t                    g_remote_addr = {0};
static uint16_t                      g_conn_id = 0;
static ssapc_find_service_result_t   g_find_service_result = {0};


void sle_sample_sle_enable_cbk(errcode_t status)
{
    if (status == 0) {
        sle_start_scan();
    }
}

void sle_sample_seek_enable_cbk(errcode_t status)
{
    if (status == 0) {
        return;
    }
}

void sle_sample_seek_disable_cbk(errcode_t status)
{
    if (status == 0) {
        sle_connect_remote_device(&g_remote_addr);
    }
}

void sle_sample_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data != NULL) {
        uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
        if (memcmp(seek_result_data->addr.addr, mac, SLE_ADDR_LEN) == 0) {
            (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
            sle_stop_seek();
        }
    }
}

static void sle_speed_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(status);
    unused(conn_id);

    if (data == NULL || data->data == NULL || data->data_len == 0) {
        return;
    }

    uint16_t rx_len = data->data_len;
    uint8_t *buf = (uint8_t *)data->data;

    /* ---- 丢包检测 ---- */
    if (rx_len >= FRAME_HDR_SIZE) {
        uint16_t hdr, tail_val, seq, payload_len;
        memcpy(&hdr,         buf,         2);
        memcpy(&seq,         buf + 2,     2);
        memcpy(&payload_len, buf + 4,     2);
        uint16_t tail_offset = 6 + payload_len;
        if ((uint32_t)tail_offset + 2 <= rx_len) {
            memcpy(&tail_val, buf + tail_offset, 2);
        } else {
            tail_val = 0;
        }

        if (hdr == FRAME_HEADER && tail_val == FRAME_TAIL) {
            g_total_rx_pkts++;
            if (g_expect_seq < 0) {
                g_expect_seq = (int32_t)((seq + 1) & 0xFFFF);
            } else {
                if (seq != (uint16_t)g_expect_seq) {
                    uint16_t lost = (uint16_t)((seq - (uint16_t)g_expect_seq) & 0xFFFF);
                    g_lost_pkts += lost;
                }
                g_expect_seq = (int32_t)((seq + 1) & 0xFFFF);
            }
        }
    }

    /* ---- 入队转发（回调里不阻塞）---- */
    static uint32_t s_uart_ok = 0;
    static uint32_t s_uart_fail = 0;
    if (g_uart_tx_queue_id != 0) {
        uart_tx_msg_t msg;
        uint16_t copy_len = (rx_len > UART_TX_BUF_SIZE) ? UART_TX_BUF_SIZE : rx_len;
        if (memcpy_s(msg.data, UART_TX_BUF_SIZE, buf, copy_len) == EOK) {
            msg.len = copy_len;
            if (osal_msg_queue_write_copy(g_uart_tx_queue_id, &msg,
                                          sizeof(uart_tx_msg_t), 0) == OSAL_SUCCESS) {
                s_uart_ok++;
            } else {
                s_uart_fail++;
            }
        }
    }

    /* 每100帧打印一次统计 */
    if ((s_uart_ok + s_uart_fail) % 100 == 0 && (s_uart_ok + s_uart_fail) > 0) {
        osal_printk("[client] 统计: SLE收包=%lu 丢包=%lu 入队成功=%lu 入队失败=%lu\r\n",
                    g_total_rx_pkts, g_lost_pkts, s_uart_ok, s_uart_fail);
    }
}

static void sle_speed_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(status);
    unused(conn_id);
    unused(client_id);
    osal_printk("\n sle_speed_indication_cb sle uart recived data : %s\r\n", data->data);
}

void sle_sample_seek_cbk_register(void)
{
    g_seek_cbk.sle_enable_cb = sle_sample_sle_enable_cbk;
    g_seek_cbk.seek_enable_cb = sle_sample_seek_enable_cbk;
    g_seek_cbk.seek_disable_cb = sle_sample_seek_disable_cbk;
    g_seek_cbk.seek_result_cb = sle_sample_seek_result_info_cbk;
}

void sle_sample_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    osal_printk("[ssap client] conn state changed conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[0],
        addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    osal_printk("[ssap client] conn state changed disc_reason:0x%x\n", disc_reason);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(&g_remote_addr);
        }
        g_conn_id = conn_id;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        /* 断连后重置期望序列号，重新开始扫描 */
        g_expect_seq = -1;
        osal_printk("[ssap client] 断连，重新扫描...\n");
        sle_start_scan();
    }
}

void sle_sample_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[ssap client] pair complete conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[0],
        addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    if (status == 0) {
#ifdef CONFIG_LARGE_THROUGHPUT_CLIENT
        sle_set_data_len(conn_id, SLE_MTU_SIZE_DEFAULT);
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
        sle_set_phy_param(conn_id, &phy_parm);
        sle_set_mcs(conn_id, 10);
        osal_printk("[client] 已设置 4M PHY MCS10\r\n");
#endif
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1, g_conn_id, &info);
    }
}

void sle_sample_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(status);
    osal_printk("[ssap client] updat state changed conn_id:%d, interval = %02x\n", conn_id, param->interval);
}

void sle_sample_update_req_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    osal_printk("[ssap client] sle_sample_update_req_cbk interval_min = %02x, interval_max = %02x\n",
        param->interval_min, param->interval_max);
}

void sle_sample_connect_cbk_register(void)
{
    g_connect_cbk.connect_state_changed_cb = sle_sample_connect_state_changed_cbk;
    g_connect_cbk.pair_complete_cb = sle_sample_pair_complete_cbk;
    g_connect_cbk.connect_param_update_req_cb = sle_sample_update_req_cbk;
    g_connect_cbk.connect_param_update_cb = sle_sample_update_cbk;
}


void sle_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
    errcode_t status)
{
    osal_printk("[ssap client] pair complete client id:%d status:%d\n", client_id, status);
    osal_printk("[ssap client] exchange mtu, mtu size: %d, version: %d.\n",
        param->mtu_size, param->version);

    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

void sle_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id, ssapc_find_service_result_t *service,
    errcode_t status)
{
    osal_printk("[ssap client] find structure cbk client: %d conn_id:%d status: %d \n",
        client_id, conn_id, status);
    osal_printk("[ssap client] find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n",
        service->start_hdl, service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] structure uuid:[0x%02x][0x%02x]\r\n",
            service->uuid.uuid[14], service->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] structure uuid[%d]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }
    g_find_service_result.start_hdl = service->start_hdl;
    g_find_service_result.end_hdl = service->end_hdl;
    memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}

void sle_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    osal_printk("[ssap client] find structure cmp cbk client id:%d status:%d type:%d uuid len:%d \r\n",
        client_id, status, structure_result->type, structure_result->uuid.len);
    if (structure_result->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find structure cmp cbk structure uuid:[0x%02x][0x%02x]\r\n",
            structure_result->uuid.uuid[14], structure_result->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find structure cmp cbk structure uuid[%d]:[0x%02x]\r\n", idx,
                structure_result->uuid.uuid[idx]);
        }
    }
    /* Server为Notify模式，无需Client主动Write，直接等待Notification即可 */
    unused(conn_id);
}

void sle_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("[ssap client] find property cbk, client id: %d, conn id: %d, operate ind: %d, "
        "descriptors count: %d status:%d.\n", client_id, conn_id, property->operate_indication,
        property->descriptors_count, status);
    for (uint16_t idx = 0; idx < property->descriptors_count; idx++) {
        osal_printk("[ssap client] find property cbk, descriptors type [%d]: 0x%02x.\n",
            idx, property->descriptors_type[idx]);
    }
    if (property->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find property cbk, uuid: %02x %02x.\n",
            property->uuid.uuid[14], property->uuid.uuid[15]); /* 14 15: uuid index */
    } else if (property->uuid.len == UUID_128BIT_LEN) {
        for (uint16_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find property cbk, uuid [%d]: %02x.\n",
                idx, property->uuid.uuid[idx]);
        }
    }
}

void sle_sample_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result,
    errcode_t status)
{
    unused(conn_id);
    unused(write_result);
    osal_printk("[ssap client] write cfm cbk, client id: %d status:%d.\n", client_id, status);
}

void sle_sample_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data,
    errcode_t status)
{
    osal_printk("[ssap client] read cfm cbk client id: %d conn id: %d status: %d\n",
        client_id, conn_id, status);
    osal_printk("[ssap client] read cfm cbk handle: %d, type: %d , len: %d\n",
        read_data->handle, read_data->type, read_data->data_len);
    for (uint16_t idx = 0; idx < read_data->data_len; idx++) {
        osal_printk("[ssap client] read cfm cbk[%d] 0x%02x\r\n", idx, read_data->data[idx]);
    }
}

void sle_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
    ssapc_notification_callback indication_cb)
{
    g_ssapc_cbk.exchange_info_cb = sle_sample_exchange_info_cbk;
    g_ssapc_cbk.find_structure_cb = sle_sample_find_structure_cbk;
    g_ssapc_cbk.find_structure_cmp_cb = sle_sample_find_structure_cmp_cbk;
    g_ssapc_cbk.ssapc_find_property_cbk = sle_sample_find_property_cbk;
    g_ssapc_cbk.write_cfm_cb = sle_sample_write_cfm_cbk;
    g_ssapc_cbk.read_cfm_cb = sle_sample_read_cfm_cbk;
    g_ssapc_cbk.notification_cb = notification_cb;
    g_ssapc_cbk.indication_cb = indication_cb;
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

void sle_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    uint8_t local_addr[SLE_ADDR_LEN] = {0x13, 0x67, 0x5c, 0x07, 0x00, 0x51};
    sle_addr_t local_address;
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_sample_seek_cbk_register();
    sle_speed_connect_param_init();
    sle_sample_connect_cbk_register();
    sle_sample_ssapc_cbk_register(notification_cb, indication_cb);
    sle_announce_seek_register_callbacks(&g_seek_cbk);
    sle_connection_register_callbacks(&g_connect_cbk);
    ssapc_register_callbacks(&g_ssapc_cbk);
    enable_sle();
    sle_set_local_addr(&local_address);
}

void sle_start_scan()
{
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 0;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

int sle_speed_init(void)
{
    osal_msleep(5000);  /* sleep 5000ms，等待Server初始化完成 */

    /* 创建UART发送队列 */
    uint32_t ret = osal_msg_queue_create("uart_tx_q", UART_TX_QUEUE_LEN,
                                         &g_uart_tx_queue_id, 0, sizeof(uart_tx_msg_t));
    if (ret != OSAL_SUCCESS) {
        osal_printk("[client] 队列创建失败 ret=%u\r\n", ret);
        return -1;
    }
    osal_printk("[client] UART发送队列创建成功\r\n");

    client_uart1_init();

    /* 启动UART发送线程 */
    osal_task *tx_task = NULL;
    osal_kthread_lock();
    tx_task = osal_kthread_create((osal_kthread_handler)uart_tx_thread_function, 0,
                                   "UartTxTask", 0x1000);
    if (tx_task != NULL) {
        osal_kthread_set_priority(tx_task, 25);
        osal_kfree(tx_task);
    }
    osal_kthread_unlock();

    sle_client_init(sle_speed_notification_cb, sle_speed_indication_cb);
    return 0;
}

#define SLE_SPEED_TASK_PRIO 26
#define SLE_SPEED_STACK_SIZE 0x2000
static void sle_speed_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_speed_init, 0, "RadarTask", SLE_SPEED_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_SPEED_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(sle_speed_entry);
