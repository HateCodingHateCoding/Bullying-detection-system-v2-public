#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "app_init.h"
#include "common_def.h"
#include "errcode.h"
#include "pinctrl.h"
#include "securec.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_client.h"
#include "sle_transmition_manager.h"
#include "soc_osal.h"
#include "uart.h"

#include "../../radar_sle_proto.h"
#include "../inc/radar_sle_client.h"

#define RADAR_HIF_HDR_LEN                  6
#define RADAR_MAX_PAYLOAD                  4096
#define RADAR_CHECK32_LEN                  4
#define RADAR_MAX_HIF_PDU                  (RADAR_HIF_HDR_LEN + RADAR_MAX_PAYLOAD + RADAR_CHECK32_LEN)

#define RADAR_CLIENT_TASK_STACK_SIZE       0x3000
#define RADAR_CLIENT_UART_STACK_SIZE       0x2000
#define RADAR_CLIENT_TASK_PRIO             26
#define RADAR_CLIENT_UART_PRIO             25

#define RADAR_CLIENT_UART_BUS              UART_BUS_1
#define RADAR_CLIENT_UART_BAUD             921600
#define RADAR_CLIENT_UART_TX_PIN           GPIO_15
#define RADAR_CLIENT_UART_RX_PIN           GPIO_16
#define RADAR_CLIENT_UART_PIN_MODE         PIN_MODE_1
#define RADAR_CLIENT_UART_RX_BUFSZ         1500

#define RADAR_CLIENT_CMD_UART_BUS          UART_BUS_2
#define RADAR_CLIENT_CMD_UART_BAUD         115200
#define RADAR_CLIENT_CMD_UART_TX_PIN       GPIO_08
#define RADAR_CLIENT_CMD_UART_RX_PIN       GPIO_07
#define RADAR_CLIENT_CMD_UART_PIN_MODE     PIN_MODE_2
#define RADAR_CLIENT_CMD_UART_RX_BUFSZ     64

#define RADAR_CLIENT_QUEUE_LEN             8
#define RADAR_CLIENT_MTU                   1500
#define RADAR_SEEK_INTERVAL_DEFAULT        100
#define RADAR_SEEK_WINDOW_DEFAULT          100
#define RADAR_DEFAULT_CONN_INTERVAL        0x14
#define RADAR_DEFAULT_TIMEOUT_MULTIPLIER   0x1F4
#define RADAR_DEFAULT_SCAN_INTERVAL        400
#define RADAR_DEFAULT_SCAN_WINDOW          20
#define RADAR_DEFAULT_MCS                  10

typedef struct {
    uint16_t len;
    uint8_t data[RADAR_MAX_HIF_PDU];
} radar_uart_item_t;

static unsigned long g_radar_uart_queue_id = 0;
static radar_uart_item_t g_uart_queue_item;
static radar_uart_item_t g_uart_tx_item;
static uint8_t g_uart_rxbuf[RADAR_CLIENT_UART_RX_BUFSZ];
static uart_buffer_config_t g_uart_buf = {
    .rx_buffer = g_uart_rxbuf,
    .rx_buffer_size = RADAR_CLIENT_UART_RX_BUFSZ,
};

static uint8_t g_cmd_uart_rxbuf[RADAR_CLIENT_CMD_UART_RX_BUFSZ];
static uart_buffer_config_t g_cmd_uart_buf = {
    .rx_buffer = g_cmd_uart_rxbuf,
    .rx_buffer_size = RADAR_CLIENT_CMD_UART_RX_BUFSZ,
};

static radar_uart_item_t g_alarm_cmd_item = {0};
static volatile bool g_alarm_cmd_pending = false;

static sle_announce_seek_callbacks_t g_seek_cbk = {0};
static sle_connection_callbacks_t g_connect_cbk = {0};
static ssapc_callbacks_t g_ssapc_cbk = {0};
static sle_addr_t g_remote_addr = {0};
static uint16_t g_conn_id = 0;
static uint8_t g_client_id = 0;
static uint16_t g_write_handle = 0;
static bool g_connected = false;
static bool g_write_ready = false;
static uint8_t g_write_buf[RADAR_CLIENT_CMD_UART_RX_BUFSZ];

static uint8_t g_rx_buf[RADAR_MAX_HIF_PDU];
static uint16_t g_rx_expect = 0;
static uint16_t g_rx_next_off = 0;
static uint16_t g_rx_seq = 0xFFFF;
static bool g_rx_in_progress = false;

static uint32_t g_hif_ok = 0;
static uint32_t g_hif_drop = 0;
static uint32_t g_frame_end = 0;
static uint32_t g_uart_ok = 0;
static uint32_t g_uart_fail = 0;
static uint32_t g_ntf_bad = 0;

static void radar_rx_reset(void)
{
    g_rx_expect = 0;
    g_rx_next_off = 0;
    g_rx_seq = 0xFFFF;
    g_rx_in_progress = false;
}

static errcode_t radar_client_send_alarm_cmd(const uint8_t *data, uint8_t len);
static void radar_client_cmd_uart_rx_callback(const void *buffer, uint16_t length, bool error);

static void radar_client_uart_init(void)
{
    uapi_pin_set_mode(RADAR_CLIENT_UART_TX_PIN, RADAR_CLIENT_UART_PIN_MODE);
    uapi_pin_set_mode(RADAR_CLIENT_UART_RX_PIN, RADAR_CLIENT_UART_PIN_MODE);

    uart_attr_t attr = {
        .baud_rate = RADAR_CLIENT_UART_BAUD,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE,
    };
    uart_pin_config_t pin = {
        .tx_pin = RADAR_CLIENT_UART_TX_PIN,
        .rx_pin = RADAR_CLIENT_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE,
    };

    uapi_uart_deinit(RADAR_CLIENT_UART_BUS);
    uapi_uart_init(RADAR_CLIENT_UART_BUS, &pin, &attr, NULL, &g_uart_buf);
    osal_printk("[radar client] UART1 init ok baud=%u\r\n", RADAR_CLIENT_UART_BAUD);
}

static void radar_client_cmd_uart_init(void)
{
    uapi_pin_set_mode(RADAR_CLIENT_CMD_UART_TX_PIN, RADAR_CLIENT_CMD_UART_PIN_MODE);
    uapi_pin_set_mode(RADAR_CLIENT_CMD_UART_RX_PIN, RADAR_CLIENT_CMD_UART_PIN_MODE);

    uart_attr_t attr = {
        .baud_rate = RADAR_CLIENT_CMD_UART_BAUD,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE,
    };
    uart_pin_config_t pin = {
        .tx_pin = RADAR_CLIENT_CMD_UART_TX_PIN,
        .rx_pin = RADAR_CLIENT_CMD_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE,
    };

    uapi_uart_deinit(RADAR_CLIENT_CMD_UART_BUS);
    uapi_uart_init(RADAR_CLIENT_CMD_UART_BUS, &pin, &attr, NULL, &g_cmd_uart_buf);
    (void)uapi_uart_register_rx_callback(RADAR_CLIENT_CMD_UART_BUS,
                                          UART_RX_CONDITION_FULL_OR_IDLE,
                                          1,
                                          radar_client_cmd_uart_rx_callback);
    osal_printk("[radar client] CMD UART init ok baud=%u\r\n", RADAR_CLIENT_CMD_UART_BAUD);
}

static bool radar_client_is_alarm_cmd(const uint8_t *buf, uint16_t len)
{
    return (buf != NULL && len >= 4 && buf[0] == 0xAA && buf[1] == 0x55 && buf[2] == 0x01);
}

static void radar_client_cmd_uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    const uint8_t *buf = (const uint8_t *)buffer;
    (void)error;

    if (!radar_client_is_alarm_cmd(buf, length) || length > sizeof(g_alarm_cmd_item.data)) {
        return;
    }

    if (memcpy_s(g_alarm_cmd_item.data, sizeof(g_alarm_cmd_item.data), buf, length) != EOK) {
        return;
    }
    g_alarm_cmd_item.len = length;
    g_alarm_cmd_pending = true;
}

static errcode_t radar_client_send_alarm_cmd(const uint8_t *data, uint8_t len)
{
    if (!g_connected || !g_write_ready || g_write_handle == 0 || data == NULL || len == 0 || len > sizeof(g_write_buf)) {
        return ERRCODE_FAIL;
    }

    if (memcpy_s(g_write_buf, sizeof(g_write_buf), data, len) != EOK) {
        return ERRCODE_FAIL;
    }

    ssapc_write_param_t param = {0};
    param.handle = g_write_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data = g_write_buf;
    param.data_len = len;
    return ssapc_write_req(g_client_id, g_conn_id, &param);
}

static void radar_client_process_alarm_cmd(void)
{
    radar_uart_item_t item = {0};

    if (!g_alarm_cmd_pending) {
        return;
    }

    (void)memcpy_s(&item, sizeof(item), &g_alarm_cmd_item, sizeof(g_alarm_cmd_item));
    g_alarm_cmd_pending = false;

    if (item.len == 0 || item.len > sizeof(item.data)) {
        return;
    }

    if (radar_client_send_alarm_cmd(item.data, (uint8_t)item.len) == ERRCODE_SUCC) {
        osal_printk("[radar client] alarm cmd forwarded len=%u\r\n", item.len);
    } else {
        g_alarm_cmd_pending = true;
    }
}

static void radar_uart_tx_thread(void)
{
    uint32_t msg_size = sizeof(radar_uart_item_t);

    while (1) {
        radar_client_process_alarm_cmd();
        uint32_t ret = osal_msg_queue_read_copy(g_radar_uart_queue_id, &g_uart_queue_item, &msg_size, 10);
        msg_size = sizeof(radar_uart_item_t);
        if (ret != OSAL_SUCCESS || g_uart_queue_item.len == 0 || g_uart_queue_item.len > RADAR_MAX_HIF_PDU) {
            continue;
        }
        int32_t uart_ret = uapi_uart_write(RADAR_CLIENT_UART_BUS, g_uart_queue_item.data, g_uart_queue_item.len, 1000);
        if (uart_ret > 0) {
            g_uart_ok++;
        } else {
            g_uart_fail++;
            if (g_uart_fail <= 5) {
                osal_printk("[radar client] uart write fail ret=%d len=%u\r\n", uart_ret, g_uart_queue_item.len);
            }
        }
        if (((g_uart_ok + g_uart_fail) % 100) == 0) {
            osal_printk("[radar client] uart ok=%u fail=%u hif=%u drop=%u fe=%u bad_ntf=%u\r\n",
                        g_uart_ok, g_uart_fail, g_hif_ok, g_hif_drop, g_frame_end, g_ntf_bad);
        }
    }
}

static void radar_queue_uart_frame(uint8_t *data, uint16_t len)
{
    g_uart_tx_item.len = len;
    if (memcpy_s(g_uart_tx_item.data, sizeof(g_uart_tx_item.data), data, len) != EOK) {
        g_hif_drop++;
        return;
    }
    if (osal_msg_queue_write_copy(g_radar_uart_queue_id, &g_uart_tx_item, sizeof(g_uart_tx_item), 0) == OSAL_SUCCESS) {
        g_hif_ok++;
    } else {
        g_hif_drop++;
    }
}

static void radar_sle_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
                                      errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    if (status != ERRCODE_SUCC || data == NULL || data->data == NULL || data->data_len < RADAR_SLE_HDR_LEN) {
        g_ntf_bad++;
        return;
    }

    radar_sle_hdr_t hdr;
    if (memcpy_s(&hdr, sizeof(hdr), data->data, RADAR_SLE_HDR_LEN) != EOK || hdr.magic != RADAR_SLE_MAGIC) {
        g_ntf_bad++;
        return;
    }

    if (hdr.type == RADAR_SLE_TYPE_FRAME_END) {
        g_frame_end++;
        radar_rx_reset();
        return;
    }

    if (hdr.type != RADAR_SLE_TYPE_HIF_PKT || hdr.chunk_len == 0 ||
        hdr.total_len > RADAR_MAX_HIF_PDU || data->data_len < (uint16_t)(RADAR_SLE_HDR_LEN + hdr.chunk_len)) {
        g_ntf_bad++;
        return;
    }

    if (hdr.flags & RADAR_SLE_FLAG_FIRST) {
        if (g_rx_in_progress) {
            g_hif_drop++;
        }
        g_rx_seq = hdr.seq;
        g_rx_expect = hdr.total_len;
        g_rx_next_off = 0;
        g_rx_in_progress = true;
    }

    if (!g_rx_in_progress || hdr.seq != g_rx_seq || hdr.offset != g_rx_next_off ||
        (uint32_t)hdr.offset + hdr.chunk_len > RADAR_MAX_HIF_PDU) {
        g_ntf_bad++;
        return;
    }

    if (memcpy_s(g_rx_buf + hdr.offset, RADAR_MAX_HIF_PDU - hdr.offset,
                 data->data + RADAR_SLE_HDR_LEN, hdr.chunk_len) != EOK) {
        radar_rx_reset();
        g_hif_drop++;
        return;
    }
    g_rx_next_off = (uint16_t)(g_rx_next_off + hdr.chunk_len);

    if ((hdr.flags & RADAR_SLE_FLAG_LAST) && g_rx_next_off == g_rx_expect) {
        radar_queue_uart_frame(g_rx_buf, g_rx_expect);
        if ((g_hif_ok % 50) == 0 && g_hif_ok > 0) {
            osal_printk("[radar client] hif ok=%u drop=%u len=%u\r\n", g_hif_ok, g_hif_drop, g_rx_expect);
        }
        radar_rx_reset();
    }
}

static void radar_sle_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
                                    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(data);
    unused(status);
}

static void radar_sle_enable_cbk(errcode_t status)
{
    if (status == ERRCODE_SUCC) {
        radar_sle_start_scan();
    }
}

static void radar_seek_enable_cbk(errcode_t status)
{
    unused(status);
}

static void radar_seek_disable_cbk(errcode_t status)
{
    if (status == ERRCODE_SUCC) {
        sle_connect_remote_device(&g_remote_addr);
    }
}

static void radar_seek_result_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data != NULL) {
        uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x77};
        if (memcmp(seek_result_data->addr.addr, mac, SLE_ADDR_LEN) == 0) {
            (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
            sle_stop_seek();
        }
    }
}

static void radar_seek_cbk_register(void)
{
    g_seek_cbk.sle_enable_cb = radar_sle_enable_cbk;
    g_seek_cbk.seek_enable_cb = radar_seek_enable_cbk;
    g_seek_cbk.seek_disable_cb = radar_seek_disable_cbk;
    g_seek_cbk.seek_result_cb = radar_seek_result_cbk;
}

static void radar_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                            sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                            sle_disc_reason_t disc_reason)
{
    unused(disc_reason);
    osal_printk("[radar client] conn=%u state=%x pair=%x\r\n", conn_id, conn_state, pair_state);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(addr);
        }
        g_conn_id = conn_id;
        g_connected = true;
        g_write_ready = false;
        g_write_handle = 0;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_conn_id = 0;
        g_connected = false;
        g_write_ready = false;
        g_write_handle = 0;
        radar_rx_reset();
        sle_start_seek();
    }
}

static void radar_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(addr);
    osal_printk("[radar client] pair conn=%u status=%x\r\n", conn_id, status);
    if (status != ERRCODE_SUCC) {
        return;
    }
#ifdef CONFIG_LARGE_THROUGHPUT_RADAR_CLIENT
    (void)sle_set_data_len(conn_id, RADAR_CLIENT_MTU);
    sle_set_phy_t phy_param = {
        .tx_format = SLE_RADIO_FRAME_2,
        .rx_format = SLE_RADIO_FRAME_2,
        .tx_phy = SLE_PHY_4M,
        .rx_phy = SLE_PHY_4M,
        .tx_pilot_density = SLE_PHY_PILOT_DENSITY_NO,
        .rx_pilot_density = SLE_PHY_PILOT_DENSITY_NO,
        .g_feedback = 0,
        .t_feedback = 0,
    };
    (void)sle_set_phy_param(conn_id, &phy_param);
    (void)sle_set_mcs(conn_id, RADAR_DEFAULT_MCS);
    osal_printk("[radar client] large throughput enabled\r\n");
#endif
    ssap_exchange_info_t info = {0};
    info.mtu_size = RADAR_CLIENT_MTU;
    info.version = 1;
    ssapc_exchange_info_req(g_client_id, conn_id, &info);
}

static void radar_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(status);
    osal_printk("[radar client] conn update conn=%u interval=%x\r\n", conn_id, param->interval);
}

static void radar_update_req_cbk(uint16_t conn_id, errcode_t status,
                                 const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    osal_printk("[radar client] conn update req interval=%x-%x\r\n", param->interval_min, param->interval_max);
}

static void radar_connect_cbk_register(void)
{
    g_connect_cbk.connect_state_changed_cb = radar_connect_state_changed_cbk;
    g_connect_cbk.pair_complete_cb = radar_pair_complete_cbk;
    g_connect_cbk.connect_param_update_req_cb = radar_update_req_cbk;
    g_connect_cbk.connect_param_update_cb = radar_update_cbk;
}

static void radar_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
                                    errcode_t status)
{
    osal_printk("[radar client] mtu exchange client=%u conn=%u mtu=%u status=%x\r\n",
                client_id, conn_id, param ? param->mtu_size : 0, status);
    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(client_id, conn_id, &find_param);
}

static void radar_find_structure_cbk(uint8_t client_id, uint16_t conn_id, ssapc_find_service_result_t *service,
                                     errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(service);
    unused(status);
}

static void radar_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                         ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(structure_result);
    unused(status);
}

static void radar_find_property_cbk(uint8_t client_id, uint16_t conn_id, ssapc_find_property_result_t *property,
                                    errcode_t status)
{
    unused(client_id);
    unused(conn_id);

    if (status == ERRCODE_SUCC && property != NULL && property->handle != 0) {
        g_write_handle = property->handle;
        g_write_ready = true;
        osal_printk("[radar client] write handle=0x%x ready\r\n", g_write_handle);
    }
}

static void radar_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result,
                                errcode_t status)
{
    osal_printk("[radar client] write cfm client=%u conn=%u handle=0x%x status=%x\r\n",
                client_id, conn_id, write_result ? write_result->handle : 0, status);
}

static void radar_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data,
                               errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(read_data);
    unused(status);
}

static void radar_ssapc_cbk_register(ssapc_notification_callback notification_cb,
                                     ssapc_indication_callback indication_cb)
{
    g_ssapc_cbk.exchange_info_cb = radar_exchange_info_cbk;
    g_ssapc_cbk.find_structure_cb = radar_find_structure_cbk;
    g_ssapc_cbk.find_structure_cmp_cb = radar_find_structure_cmp_cbk;
    g_ssapc_cbk.ssapc_find_property_cbk = radar_find_property_cbk;
    g_ssapc_cbk.write_cfm_cb = radar_write_cfm_cbk;
    g_ssapc_cbk.read_cfm_cb = radar_read_cfm_cbk;
    g_ssapc_cbk.notification_cb = notification_cb;
    g_ssapc_cbk.indication_cb = indication_cb;
}

static void radar_connect_param_init(void)
{
    sle_default_connect_param_t param = {0};
    param.enable_filter_policy = 0;
    param.gt_negotiate = 0;
    param.initiate_phys = 1;
    param.max_interval = RADAR_DEFAULT_CONN_INTERVAL;
    param.min_interval = RADAR_DEFAULT_CONN_INTERVAL;
    param.scan_interval = RADAR_DEFAULT_SCAN_INTERVAL;
    param.scan_window = RADAR_DEFAULT_SCAN_WINDOW;
    param.timeout = RADAR_DEFAULT_TIMEOUT_MULTIPLIER;
    sle_default_connection_param_set(&param);
}

void radar_sle_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    uint8_t local_addr[SLE_ADDR_LEN] = {0x13, 0x67, 0x5c, 0x07, 0x00, 0x52};
    sle_addr_t local_address = {0};
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);

    sle_uuid_t app_uuid = {0};
    app_uuid.len = 2;
    app_uuid.uuid[0] = 0;
    app_uuid.uuid[1] = 0;
    ssapc_register_client(&app_uuid, &g_client_id);

    radar_seek_cbk_register();
    radar_connect_param_init();
    radar_connect_cbk_register();
    radar_ssapc_cbk_register(notification_cb, indication_cb);
    sle_announce_seek_register_callbacks(&g_seek_cbk);
    sle_connection_register_callbacks(&g_connect_cbk);
    ssapc_register_callbacks(&g_ssapc_cbk);
    enable_sle();
    sle_set_local_addr(&local_address);
}

void radar_sle_start_scan(void)
{
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 0;
    param.seek_interval[0] = RADAR_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = RADAR_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

static int radar_client_main(void *arg)
{
    unused(arg);
    osal_msleep(5000);

    uint32_t ret = osal_msg_queue_create("radar_uart_q", RADAR_CLIENT_QUEUE_LEN, &g_radar_uart_queue_id, 0,
                                         sizeof(radar_uart_item_t));
    if (ret != OSAL_SUCCESS) {
        osal_printk("[radar client] queue create fail ret=%u\r\n", ret);
        return -1;
    }
    radar_client_uart_init();
    radar_client_cmd_uart_init();

    osal_task *uart_task = osal_kthread_create((osal_kthread_handler)radar_uart_tx_thread, NULL,
                                               "RadarUartTx", RADAR_CLIENT_UART_STACK_SIZE);
    if (uart_task != NULL) {
        osal_kthread_set_priority(uart_task, RADAR_CLIENT_UART_PRIO);
        osal_kfree(uart_task);
    }

    radar_sle_client_init(radar_sle_notification_cb, radar_sle_indication_cb);
    return 0;
}

static void radar_sle_client_entry(void)
{
    osal_task *task = NULL;
    osal_kthread_lock();
    task = osal_kthread_create((osal_kthread_handler)radar_client_main, NULL,
                               "RadarClient", RADAR_CLIENT_TASK_STACK_SIZE);
    if (task != NULL) {
        osal_kthread_set_priority(task, RADAR_CLIENT_TASK_PRIO);
        osal_kfree(task);
    }
    osal_kthread_unlock();
}

app_run(radar_sle_client_entry);
