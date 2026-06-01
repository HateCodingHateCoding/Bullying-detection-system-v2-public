#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "app_init.h"
#include "common_def.h"
#include "dma.h"
#include "errcode.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "nv.h"
#include "pinctrl.h"
#include "securec.h"
#include "sle_common.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "sle_transmition_manager.h"
#include "soc_osal.h"
#include "spi.h"
#include "systick.h"
#include "uart.h"
#include "watchdog.h"

#include "../../radar_sle_proto.h"
#include "../inc/radar_sle_server.h"
#include "../inc/radar_sle_server_adv.h"

#define RADAR_HIF_HDR_LEN                  6
#define RADAR_MAX_PAYLOAD                  4096
#define RADAR_CHECK32_LEN                  4
#define RADAR_MAX_HIF_PDU                  (RADAR_HIF_HDR_LEN + RADAR_MAX_PAYLOAD + RADAR_CHECK32_LEN)

#define RADAR_SERVER_TASK_STACK_SIZE       0x3000
#define RADAR_SERVER_SEND_STACK_SIZE       0x2000
#define RADAR_SERVER_TASK_PRIO             23
#define RADAR_SERVER_SEND_PRIO             24

#define RADAR_QUEUE_LEN                    8
#define RADAR_DEFAULT_MTU                  1500
#define RADAR_DEFAULT_DATA_LEN             1500
#define RADAR_DEFAULT_CONN_INTERVAL        0x14
#define RADAR_DEFAULT_TIMEOUT_MULTIPLIER   0x1F4
#define RADAR_DEFAULT_SCAN_INTERVAL        400
#define RADAR_DEFAULT_SCAN_WINDOW          20
#define RADAR_DEFAULT_MCS                  10
#define RADAR_SLE_CHUNK_FALLBACK           200
#define RADAR_SLE_CHUNK_MAX                1400
#define RADAR_SPI_DMA_WIDTH                0

#define RADAR_NOTIFY_PIN                   GPIO_03
#define RADAR_SPI_MOSI_PIN                 GPIO_07
#define RADAR_SPI_CLK_PIN                  GPIO_09
#define RADAR_SPI_CS_PIN                   GPIO_10
#define RADAR_SPI_MISO_PIN                 GPIO_11
#define RADAR_SPI_PIN_MODE                 PIN_MODE_3
#define RADAR_SPI_BUS                      SPI_BUS_0

#define RADAR_MP3_UART_BUS                 UART_BUS_1
#define RADAR_MP3_UART_BAUD                9600
#define RADAR_MP3_UART_TX_PIN              GPIO_15
#define RADAR_MP3_UART_RX_PIN              GPIO_16
#define RADAR_MP3_UART_PIN_MODE            PIN_MODE_1
#define RADAR_MP3_UART_RX_BUFSZ            256
#define RADAR_ALARM_GPIO                   GPIO_12
#define RADAR_ALARM_QUEUE_LEN              4
#define RADAR_ALARM_TASK_STACK_SIZE        0x1000
#define RADAR_ALARM_TASK_PRIO              24

#define POLL_TO_HEADER_DELAY_MS            5
#define HEADER_TO_PAYLOAD_DELAY_US         300
#define NOTIFY_IO_TIMEOUT_MS               15
#define RADAR_AFTER_SPI_INIT_DELAY_MS      5000

#define HIF_HDR_TYPE(hdr)                  ((hdr)[2] & 0x03)
#define HIF_HDR_CHECK(hdr)                 (((hdr)[2] >> 4) & 0x01)
#define HIF_HDR_MSGID(hdr)                 ((hdr)[3])
#define HIF_HDR_LENGTH(hdr)                ((uint16_t)((hdr)[4]) | ((uint16_t)((hdr)[5] & 0x0F) << 8))

typedef enum {
    RADAR_QUEUE_HIF = 1,
    RADAR_QUEUE_FRAME_END = 2,
} radar_queue_type_t;

typedef struct {
    uint8_t type;
    uint16_t len;
    uint8_t data[RADAR_MAX_HIF_PDU];
} radar_queue_item_t;

typedef struct {
    uint8_t cmd;
    uint8_t seconds;
} radar_alarm_msg_t;

static char g_radar_sle_uuid_app_uuid[2] = {0x0, 0x0};
static char g_radar_sle_property_value[8] = {0};
static uint16_t g_radar_conn_hdl = 0;
static uint8_t g_radar_server_id = 0;
static uint16_t g_radar_service_handle = 0;
static uint16_t g_radar_property_handle = 0;
static volatile bool g_radar_connected = false;
static volatile uint16_t g_radar_mtu = RADAR_DEFAULT_MTU;
static volatile bool g_radar_phy_set = false;

static unsigned long g_radar_queue_id = 0;
static unsigned long g_alarm_queue_id = 0;
static volatile bool g_alarm_active = false;
static volatile bool g_alarm_latched = false;
static volatile uint8_t g_alarm_latched_seconds = 10;
static uint8_t g_mp3_uart_rxbuf[RADAR_MP3_UART_RX_BUFSZ];
static uart_buffer_config_t g_mp3_uart_buf = {
    .rx_buffer = g_mp3_uart_rxbuf,
    .rx_buffer_size = RADAR_MP3_UART_RX_BUFSZ,
};
static volatile uint8_t g_radar_gpio_flag = 0;
static uint16_t g_radar_sle_seq = 0;

static uint32_t g_hif_rx_ok = 0;
static uint32_t g_hif_rx_drop = 0;
static uint32_t g_hif_hdr_err = 0;
static uint32_t g_spi_flush_cnt = 0;
static uint32_t g_spi_resync_cnt = 0;
static uint32_t g_spi_complete_ack_resync = 0;
static uint32_t g_sle_send_ok = 0;
static uint32_t g_sle_send_fail = 0;
static uint32_t g_sle_frame_end = 0;
static uint16_t g_max_hif_payload = 0;

static uint32_t g_frame_hif_cnt = 0;
static uint32_t g_frame_payload_bytes = 0;
static uint32_t g_frame_max_payload = 0;
static uint32_t g_frame_seq = 0;
static uint64_t g_frame_start_us = 0;
static uint64_t g_last_frame_end_us = 0;

static uint8_t g_flush_rx[512];
static uint8_t g_flush_tx[512];
static uint8_t g_payload_dummy[RADAR_MAX_PAYLOAD + RADAR_CHECK32_LEN];

#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
static sle_link_qos_state_t g_sle_link_state = 0;
#else
extern uint8_t gle_tx_acb_data_num_get(void);
#endif

static void encode2byte_little(uint8_t *ptr, uint16_t data)
{
    ptr[1] = (uint8_t)(data >> 8);
    ptr[0] = (uint8_t)data;
}

static void radar_dump_bytes(const char *tag, const uint8_t *data, uint16_t len)
{
    uint16_t show = (len > 16) ? 16 : len;
    osal_printk("%s len=%u:", tag, len);
    for (uint16_t i = 0; i < show; i++) {
        osal_printk(" %02x", data[i]);
    }
    osal_printk("\r\n");
}

static void radar_frame_begin_if_needed(void)
{
    if (g_frame_hif_cnt == 0 && g_frame_payload_bytes == 0) {
        g_frame_start_us = uapi_systick_get_us();
    }
}

static void radar_frame_record_hif(uint16_t payload_len)
{
    radar_frame_begin_if_needed();
    g_frame_hif_cnt++;
    g_frame_payload_bytes += payload_len;
    if (payload_len > g_frame_max_payload) {
        g_frame_max_payload = payload_len;
    }
}

static void radar_frame_end_log(const char *reason)
{
    uint64_t now = uapi_systick_get_us();
    uint32_t frame_us = (g_frame_start_us == 0) ? 0 : (uint32_t)(now - g_frame_start_us);
    uint32_t gap_us = (g_last_frame_end_us == 0) ? 0 : (uint32_t)(now - g_last_frame_end_us);

    if (g_frame_hif_cnt > 0 || reason[0] != 'i') {
        g_frame_seq++;
        if (g_frame_seq <= 20 || (g_frame_seq % 20) == 0 || g_frame_payload_bytes > 3000 || reason[0] != 'a') {
            osal_printk("[radar spi] frame#%u reason=%s hif=%u bytes=%u max=%u frame_us=%u gap_us=%u gpio=%u ack_resync=%u resync=%u\r\n",
                        g_frame_seq, reason, g_frame_hif_cnt, g_frame_payload_bytes, g_frame_max_payload,
                        frame_us, gap_us, uapi_gpio_get_val(RADAR_NOTIFY_PIN),
                        g_spi_complete_ack_resync, g_spi_resync_cnt);
        }
    }

    g_last_frame_end_us = now;
    g_frame_hif_cnt = 0;
    g_frame_payload_bytes = 0;
    g_frame_max_payload = 0;
    g_frame_start_us = 0;
}

static uint8_t g_radar_sle_uuid_base[] = {
    0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void radar_alarm_led_on(void)
{
    uapi_gpio_set_val(RADAR_ALARM_GPIO, GPIO_LEVEL_HIGH);
}

static void radar_alarm_led_off(void)
{
    uapi_gpio_set_val(RADAR_ALARM_GPIO, GPIO_LEVEL_LOW);
}

static void radar_alarm_gpio_init(void)
{
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(RADAR_ALARM_GPIO, PIN_MODE_0);
    uapi_gpio_set_dir(RADAR_ALARM_GPIO, GPIO_DIRECTION_OUTPUT);
    radar_alarm_led_off();
}

static void radar_mp3_uart_init(void)
{
    uapi_pin_init();
    uapi_pin_set_mode(RADAR_MP3_UART_TX_PIN, RADAR_MP3_UART_PIN_MODE);
    uapi_pin_set_mode(RADAR_MP3_UART_RX_PIN, RADAR_MP3_UART_PIN_MODE);

    uart_attr_t attr = {
        .baud_rate = RADAR_MP3_UART_BAUD,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE,
    };
    uart_pin_config_t pin = {
        .tx_pin = RADAR_MP3_UART_TX_PIN,
        .rx_pin = RADAR_MP3_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE,
    };

    uapi_uart_deinit(RADAR_MP3_UART_BUS);
    if (uapi_uart_init(RADAR_MP3_UART_BUS, &pin, &attr, NULL, &g_mp3_uart_buf) == ERRCODE_SUCC) {
        osal_printk("[radar alarm] mp3 uart ok baud=%u\r\n", RADAR_MP3_UART_BAUD);
    } else {
        osal_printk("[radar alarm] mp3 uart init fail\r\n");
    }
}

static void radar_mp3_send_cmd(const char *cmd)
{
    if (cmd == NULL) {
        return;
    }
    (void)uapi_uart_write(RADAR_MP3_UART_BUS, (const uint8_t *)cmd, (uint16_t)strlen(cmd), 1000);
    osal_msleep(100);
}

static void radar_mp3_play_alarm(void)
{
    radar_mp3_send_cmd("@Volume,31,$");
    radar_mp3_send_cmd("@Play,0108,$");
}

static void radar_mp3_stop(void)
{
    radar_mp3_send_cmd("@Stop,$");
}

static void radar_alarm_thread(void)
{
    radar_alarm_msg_t msg;
    uint32_t msg_size = sizeof(msg);

    while (1) {
        if (osal_msg_queue_read_copy(g_alarm_queue_id, &msg, &msg_size, OSAL_WAIT_FOREVER) != OSAL_SUCCESS) {
            msg_size = sizeof(msg);
            continue;
        }
        msg_size = sizeof(msg);
        if (msg.cmd != 0x01) {
            continue;
        }
        if (msg.seconds == 0) {
            msg.seconds = 10;
        }

        while (1) {
            g_alarm_active = true;
            osal_printk("[radar alarm] start seconds=%u\r\n", msg.seconds);
            radar_mp3_play_alarm();
            for (uint32_t i = 0; i < (uint32_t)msg.seconds; i++) {
                radar_alarm_led_on();
                osal_msleep(500);
                radar_alarm_led_off();
                osal_msleep(500);
            }
            radar_alarm_led_off();
            radar_mp3_stop();
            g_alarm_active = false;
            osal_printk("[radar alarm] stop\r\n");

            if (!g_alarm_latched) {
                break;
            }
            msg.cmd = 0x01;
            msg.seconds = g_alarm_latched_seconds;
            g_alarm_latched = false;
            osal_printk("[radar alarm] replay latched seconds=%u\r\n", msg.seconds);
        }
    }
}

static void radar_alarm_queue_cmd(uint8_t seconds)
{
    unused(seconds);

    radar_alarm_msg_t msg = {
        .cmd = 0x01,
        .seconds = 10,
    };

    if (g_alarm_active) {
        g_alarm_latched_seconds = msg.seconds;
        g_alarm_latched = true;
        osal_printk("[radar alarm] cmd latched seconds=%u\r\n", msg.seconds);
        return;
    }

    if (g_alarm_queue_id == 0) {
        return;
    }
    if (osal_msg_queue_write_copy(g_alarm_queue_id, &msg, sizeof(msg), 0) == OSAL_SUCCESS) {
        osal_printk("[radar alarm] cmd queued seconds=%u\r\n", msg.seconds);
    } else {
        radar_alarm_msg_t dropped;
        uint32_t msg_size = sizeof(dropped);
        (void)osal_msg_queue_read_copy(g_alarm_queue_id, &dropped, &msg_size, 0);
        (void)osal_msg_queue_write_copy(g_alarm_queue_id, &msg, sizeof(msg), 0);
        osal_printk("[radar alarm] queue full, replaced seconds=%u\r\n", msg.seconds);
    }
}

static void radar_sle_uuid_set_base(sle_uuid_t *out)
{
    (void)memcpy_s(out->uuid, SLE_UUID_LEN, g_radar_sle_uuid_base, SLE_UUID_LEN);
    out->len = 2;
}

static void radar_sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    radar_sle_uuid_set_base(out);
    out->len = 2;
    encode2byte_little(&out->uuid[14], u2);
}

static void radar_gpio_isr_handler(pin_t pin, uintptr_t param)
{
    unused(param);
    if (pin == RADAR_NOTIFY_PIN) {
        g_radar_gpio_flag = 1;
    }
    uapi_gpio_clear_interrupt(pin);
}

static void radar_delay_us(uint32_t us)
{
    uint64_t start = uapi_systick_get_us();
    while ((uapi_systick_get_us() - start) < (uint64_t)us) {
    }
}

static bool hif_header_check8_valid(const uint8_t hdr[RADAR_HIF_HDR_LEN])
{
    uint8_t sum = 0;
    for (uint32_t i = 0; i < RADAR_HIF_HDR_LEN; i++) {
        sum += hdr[i];
    }
    return (sum == 0xFF);
}

static bool hif_is_complete_ack(const uint8_t hdr[RADAR_HIF_HDR_LEN])
{
    return (HIF_HDR_TYPE(hdr) == 0x03 && HIF_HDR_MSGID(hdr) == 0x0C && HIF_HDR_LENGTH(hdr) == 0);
}

static bool hif_header_idle(const uint8_t hdr[RADAR_HIF_HDR_LEN])
{
    bool idle_ff = true;
    bool idle_00 = true;
    for (uint32_t i = 0; i < RADAR_HIF_HDR_LEN; i++) {
        idle_ff = idle_ff && (hdr[i] == 0xFF);
        idle_00 = idle_00 && (hdr[i] == 0x00);
    }
    return idle_ff || idle_00;
}

static errcode_t radar_spi_read_header_resync(spi_bus_t bus, spi_xfer_data_t *xfer_hdr,
                                              uint8_t hdr_buf[RADAR_HIF_HDR_LEN], uint32_t idx)
{
    errcode_t ret = uapi_spi_master_writeread(bus, xfer_hdr, 0xFFFFFFFF);
    if (ret != ERRCODE_SUCC) {
        if (g_hif_hdr_err <= 5) {
            osal_printk("[radar spi] header read fail ret=0x%x idx=%u\r\n", ret, idx);
        }
        return ret;
    }
    if (hif_header_idle(hdr_buf) || hif_header_check8_valid(hdr_buf)) {
        return ERRCODE_SUCC;
    }

    uint8_t one_rx = 0;
    uint8_t one_tx = 0;
    spi_xfer_data_t one = {
        .tx_buff = &one_tx,
        .tx_bytes = 1,
        .rx_buff = &one_rx,
        .rx_bytes = 1,
    };

    for (uint32_t i = 0; i < 64; i++) {
        ret = uapi_spi_master_writeread(bus, &one, 0xFFFFFFFF);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        (void)memmove_s(hdr_buf, RADAR_HIF_HDR_LEN, hdr_buf + 1, RADAR_HIF_HDR_LEN - 1);
        hdr_buf[RADAR_HIF_HDR_LEN - 1] = one_rx;
        if (hdr_buf[0] == 0xA5 && hif_header_check8_valid(hdr_buf)) {
            if (hif_is_complete_ack(hdr_buf)) {
                g_spi_complete_ack_resync++;
            } else {
                g_spi_resync_cnt++;
            }
            if ((g_spi_resync_cnt + g_spi_complete_ack_resync) <= 10 ||
                (((g_spi_resync_cnt + g_spi_complete_ack_resync) % 100) == 0)) {
                radar_dump_bytes(hif_is_complete_ack(hdr_buf) ? "[radar spi] resync ack" : "[radar spi] resync hdr",
                                 hdr_buf, RADAR_HIF_HDR_LEN);
            }
            return ERRCODE_SUCC;
        }
        if (hif_header_idle(hdr_buf)) {
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

static bool radar_wait_notify_high(uint32_t timeout_ms)
{
    uint64_t start = uapi_systick_get_us();
    uint64_t timeout_us = (uint64_t)timeout_ms * 1000;
    while ((uapi_systick_get_us() - start) <= timeout_us) {
        if (uapi_gpio_get_val(RADAR_NOTIFY_PIN) == GPIO_LEVEL_HIGH) {
            return true;
        }
        radar_delay_us(50);
    }
    return false;
}

static bool radar_wait_notify_low(uint32_t timeout_ms)
{
    uint64_t start = uapi_systick_get_us();
    uint64_t timeout_us = (uint64_t)timeout_ms * 1000;
    while ((uapi_systick_get_us() - start) <= timeout_us) {
        if (uapi_gpio_get_val(RADAR_NOTIFY_PIN) == GPIO_LEVEL_LOW) {
            return true;
        }
        radar_delay_us(50);
    }
    return false;
}

static void radar_spi_flush_bus(spi_bus_t bus)
{
    spi_xfer_data_t xfer = {0};
    xfer.rx_buff = g_flush_rx;
    xfer.rx_bytes = sizeof(g_flush_rx);
    xfer.tx_buff = g_flush_tx;
    xfer.tx_bytes = sizeof(g_flush_tx);
    g_spi_flush_cnt++;

    for (uint32_t round = 0; round < 20; round++) {
        if (uapi_gpio_get_val(RADAR_NOTIFY_PIN) == GPIO_LEVEL_LOW) {
            return;
        }
        osal_msleep(1);
        if (uapi_spi_master_writeread(bus, &xfer, 0xFFFFFFFF) != ERRCODE_SUCC) {
            continue;
        }
        bool all_idle = true;
        for (uint32_t i = 0; i < sizeof(g_flush_rx); i++) {
            if (g_flush_rx[i] != 0xFF && g_flush_rx[i] != 0x00) {
                all_idle = false;
                break;
            }
        }
        if (all_idle) {
            return;
        }
        for (uint32_t i = 0; i <= sizeof(g_flush_rx) - RADAR_HIF_HDR_LEN; i++) {
            if (g_flush_rx[i] == 0xA5 && hif_header_check8_valid(&g_flush_rx[i]) &&
                hif_is_complete_ack(&g_flush_rx[i])) {
                return;
            }
        }
    }

    uint64_t start = uapi_systick_get_us();
    while ((uapi_systick_get_us() - start) < 200000ULL) {
        if (uapi_gpio_get_val(RADAR_NOTIFY_PIN) == GPIO_LEVEL_LOW) {
            return;
        }
        osal_msleep(5);
    }
}

static uint16_t radar_sle_payload_chunk(void)
{
    uint16_t mtu = g_radar_mtu;
    if (mtu <= (RADAR_SLE_HDR_LEN + 32)) {
        return RADAR_SLE_CHUNK_FALLBACK;
    }
    uint16_t chunk = (uint16_t)(mtu - RADAR_SLE_HDR_LEN - 16);
    return (chunk > RADAR_SLE_CHUNK_MAX) ? RADAR_SLE_CHUNK_MAX : chunk;
}

bool radar_sle_server_is_ready(void)
{
    return (g_radar_connected && g_radar_property_handle != 0);
}

uint16_t radar_sle_server_get_mtu(void)
{
    return g_radar_mtu;
}

uint8_t radar_sle_flow_ctrl_flag(void)
{
#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
    return (g_sle_link_state <= SLE_QOS_FLOWCTRL) ? 1 : 0;
#else
    return gle_tx_acb_data_num_get();
#endif
}

errcode_t radar_sle_server_send_bytes(uint8_t *data, uint16_t len)
{
    if (!radar_sle_server_is_ready() || data == NULL || len == 0) {
        return ERRCODE_FAIL;
    }
    ssaps_ntf_ind_t param = {0};
    param.handle = g_radar_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = data;
    param.value_len = len;
    return ssaps_notify_indicate(g_radar_server_id, g_radar_conn_hdl, &param);
}

static errcode_t radar_sle_send_frame_end(void)
{
    radar_sle_hdr_t hdr = {0};
    hdr.magic = RADAR_SLE_MAGIC;
    hdr.type = RADAR_SLE_TYPE_FRAME_END;
    hdr.flags = RADAR_SLE_FLAG_FIRST | RADAR_SLE_FLAG_LAST;
    hdr.seq = g_radar_sle_seq++;
    return radar_sle_server_send_bytes((uint8_t *)&hdr, RADAR_SLE_HDR_LEN);
}

static errcode_t radar_sle_send_hif_fragmented(uint8_t *data, uint16_t len)
{
    static uint8_t txbuf[RADAR_DEFAULT_MTU];
    uint16_t offset = 0;
    uint16_t seq = g_radar_sle_seq++;
    uint16_t chunk_limit = radar_sle_payload_chunk();

    while (offset < len) {
        uint16_t chunk = (uint16_t)(len - offset);
        if (chunk > chunk_limit) {
            chunk = chunk_limit;
        }
        radar_sle_hdr_t hdr = {0};
        hdr.magic = RADAR_SLE_MAGIC;
        hdr.type = RADAR_SLE_TYPE_HIF_PKT;
        hdr.flags = 0;
        if (offset == 0) {
            hdr.flags |= RADAR_SLE_FLAG_FIRST;
        }
        if ((uint16_t)(offset + chunk) >= len) {
            hdr.flags |= RADAR_SLE_FLAG_LAST;
        }
        hdr.seq = seq;
        hdr.total_len = len;
        hdr.offset = offset;
        hdr.chunk_len = chunk;

        if (memcpy_s(txbuf, sizeof(txbuf), &hdr, RADAR_SLE_HDR_LEN) != EOK ||
            memcpy_s(txbuf + RADAR_SLE_HDR_LEN, sizeof(txbuf) - RADAR_SLE_HDR_LEN, data + offset, chunk) != EOK) {
            return ERRCODE_FAIL;
        }

        while (radar_sle_flow_ctrl_flag() == 0) {
            osal_udelay(100);
        }

        errcode_t ret = radar_sle_server_send_bytes(txbuf, (uint16_t)(RADAR_SLE_HDR_LEN + chunk));
        if (ret != ERRCODE_SUCC) {
            osal_msleep(2);
            ret = radar_sle_server_send_bytes(txbuf, (uint16_t)(RADAR_SLE_HDR_LEN + chunk));
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
        offset = (uint16_t)(offset + chunk);
    }
    return ERRCODE_SUCC;
}

static void radar_send_thread(void)
{
    radar_queue_item_t item;
    uint32_t msg_size = sizeof(radar_queue_item_t);

    while (1) {
        uint32_t ret = osal_msg_queue_read_copy(g_radar_queue_id, &item, &msg_size, OSAL_WAIT_FOREVER);
        msg_size = sizeof(radar_queue_item_t);
        if (ret != OSAL_SUCCESS) {
            continue;
        }
        if (!radar_sle_server_is_ready()) {
            continue;
        }
        if (!g_radar_phy_set && g_radar_connected) {
            (void)sle_set_data_len(g_radar_conn_hdl, RADAR_DEFAULT_DATA_LEN);
#ifdef CONFIG_LARGE_THROUGHPUT_RADAR_SERVER
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
            (void)sle_set_phy_param(g_radar_conn_hdl, &phy_param);
            (void)sle_set_mcs(g_radar_conn_hdl, RADAR_DEFAULT_MCS);
            osal_printk("[radar server] large throughput enabled: mtu=%u chunk=%u\r\n",
                        g_radar_mtu, radar_sle_payload_chunk());
#endif
            g_radar_phy_set = true;
        }

        if (item.type == RADAR_QUEUE_FRAME_END) {
            if (radar_sle_send_frame_end() == ERRCODE_SUCC) {
                g_sle_frame_end++;
            }
            continue;
        }
        if (item.type != RADAR_QUEUE_HIF || item.len == 0 || item.len > RADAR_MAX_HIF_PDU) {
            continue;
        }
        if (radar_sle_send_hif_fragmented(item.data, item.len) == ERRCODE_SUCC) {
            g_sle_send_ok++;
        } else {
            g_sle_send_fail++;
        }
        if (((g_sle_send_ok + g_sle_send_fail) % 100) == 0) {
            osal_printk("[radar server] sle ok=%u fail=%u fe=%u mtu=%u chunk=%u hif_rx=%u drop=%u hdr_err=%u flush=%u max_pld=%u\r\n",
                        g_sle_send_ok, g_sle_send_fail, g_sle_frame_end, g_radar_mtu, radar_sle_payload_chunk(),
                        g_hif_rx_ok, g_hif_rx_drop, g_hif_hdr_err, g_spi_flush_cnt, g_max_hif_payload);
        }
    }
}

static void radar_queue_hif(const uint8_t *hdr, const uint8_t *payload, uint16_t payload_len)
{
    radar_queue_item_t item = {0};
    item.type = RADAR_QUEUE_HIF;
    item.len = (uint16_t)(RADAR_HIF_HDR_LEN + payload_len);
    if (memcpy_s(item.data, sizeof(item.data), hdr, RADAR_HIF_HDR_LEN) != EOK ||
        memcpy_s(item.data + RADAR_HIF_HDR_LEN, sizeof(item.data) - RADAR_HIF_HDR_LEN, payload, payload_len) != EOK) {
        g_hif_rx_drop++;
        return;
    }
    if (osal_msg_queue_write_copy(g_radar_queue_id, &item, sizeof(item), 0) == OSAL_SUCCESS) {
        g_hif_rx_ok++;
    } else {
        g_hif_rx_drop++;
    }
}

static void radar_queue_frame_end(void)
{
    radar_queue_item_t item = {0};
    item.type = RADAR_QUEUE_FRAME_END;
    if (osal_msg_queue_write_copy(g_radar_queue_id, &item, sizeof(item), 0) != OSAL_SUCCESS) {
        g_hif_rx_drop++;
    }
}

static errcode_t radar_spi_init(void)
{
    uapi_pin_init();
    uapi_pin_set_mode(RADAR_NOTIFY_PIN, PIN_MODE_0);
    uapi_gpio_init();
    uapi_gpio_set_dir(RADAR_NOTIFY_PIN, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(RADAR_NOTIFY_PIN, 1, radar_gpio_isr_handler);
    uapi_gpio_enable_interrupt(RADAR_NOTIFY_PIN);

    uapi_pin_set_mode(RADAR_SPI_MOSI_PIN, RADAR_SPI_PIN_MODE);
    uapi_pin_set_mode(RADAR_SPI_CLK_PIN, RADAR_SPI_PIN_MODE);
    uapi_pin_set_mode(RADAR_SPI_CS_PIN, RADAR_SPI_PIN_MODE);
    uapi_pin_set_mode(RADAR_SPI_MISO_PIN, RADAR_SPI_PIN_MODE);

    spi_attr_t spi_conf = {0};
    spi_conf.is_slave = false;
    spi_conf.slave_num = 1u;
    spi_conf.bus_clk = 32000000u;
    spi_conf.freq_mhz = 1;
    spi_conf.clk_polarity = SPI_CFG_CLK_CPOL_0;
    spi_conf.clk_phase = SPI_CFG_CLK_CPHA_0;
    spi_conf.frame_format = SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI;
    spi_conf.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    spi_conf.frame_size = HAL_SPI_FRAME_SIZE_8;
    spi_conf.tmod = HAL_SPI_TRANS_MODE_TXRX;
    spi_conf.ndf = 0;
    spi_conf.sste = SPI_CFG_SSTE_DISABLE;

    spi_extra_attr_t spi_extra = {0};
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    spi_extra.tx_use_dma = true;
    spi_extra.rx_use_dma = true;
#else
    spi_extra.tx_use_dma = false;
    spi_extra.rx_use_dma = false;
#endif
    spi_extra.sspi_param.wait_cycles = 0;

    errcode_t ret = uapi_spi_init(RADAR_SPI_BUS, &spi_conf, &spi_extra);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[radar spi] init fail:%x\r\n", ret);
        return ret;
    }
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    uapi_dma_init();
    uapi_dma_open();
    spi_dma_config_t dma_cfg = {
        .src_width = RADAR_SPI_DMA_WIDTH,
        .dest_width = RADAR_SPI_DMA_WIDTH,
        .burst_length = 0,
        .priority = 0,
    };
    ret = uapi_spi_set_dma_mode(RADAR_SPI_BUS, true, &dma_cfg);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[radar spi] dma mode fail:%x\r\n", ret);
    }
#endif
    osal_printk("[radar spi] init ok\r\n");
    return ERRCODE_SUCC;
}

static void radar_spi_thread(void)
{
    uint8_t poll_cmd[] = {
        0xA5, 0x35, 0x15, 0x0C, 0x04, 0x00,
        0x01, 0x00, 0x00, 0x01,
        0xE9, 0xF3, 0xFB, 0xFE
    };
    uint8_t poll_rx[sizeof(poll_cmd)] = {0};
    spi_xfer_data_t xfer_poll = {
        .tx_buff = poll_cmd,
        .tx_bytes = sizeof(poll_cmd),
        .rx_buff = poll_rx,
        .rx_bytes = sizeof(poll_rx),
    };
    uint8_t hdr_buf[RADAR_HIF_HDR_LEN] = {0};
    uint8_t hdr_tx[RADAR_HIF_HDR_LEN] = {0};
    spi_xfer_data_t xfer_hdr = {
        .tx_buff = hdr_tx,
        .tx_bytes = sizeof(hdr_tx),
        .rx_buff = hdr_buf,
        .rx_bytes = sizeof(hdr_buf),
    };
    static uint8_t payload_buf[RADAR_MAX_PAYLOAD + RADAR_CHECK32_LEN];
    spi_xfer_data_t xfer_payload = {0};

    osal_msleep(1000);
    if (radar_spi_init() != ERRCODE_SUCC) {
        return;
    }
    osal_printk("[radar spi] wait %u ms after init\r\n", RADAR_AFTER_SPI_INIT_DELAY_MS);
    osal_msleep(RADAR_AFTER_SPI_INIT_DELAY_MS);
    if (uapi_gpio_get_val(RADAR_NOTIFY_PIN) == GPIO_LEVEL_HIGH) {
        g_radar_gpio_flag = 1;
        osal_printk("[radar spi] notify high after init\r\n");
    }

    uint32_t idle_cnt = 0;
    uint32_t poll_fail_cnt = 0;
    uint32_t hdr_wait_fail_cnt = 0;
    while (1) {
        uint8_t notify_level = uapi_gpio_get_val(RADAR_NOTIFY_PIN);
        if (notify_level != GPIO_LEVEL_HIGH) {
            g_radar_gpio_flag = 0;
            idle_cnt++;
            if ((idle_cnt % 3000) == 0) {
                osal_printk("[radar spi] wait notify low, gpio=%u hif_ok=%u drop=%u hdr_err=%u flush=%u resync=%u max_pld=%u\r\n",
                            notify_level, g_hif_rx_ok, g_hif_rx_drop, g_hif_hdr_err,
                            g_spi_flush_cnt, g_spi_resync_cnt, g_max_hif_payload);
            }
            osal_msleep(1);
            continue;
        }
        idle_cnt = 0;
        g_radar_gpio_flag = 0;
        errcode_t ret = uapi_spi_master_writeread(RADAR_SPI_BUS, &xfer_poll, 0xFFFFFFFF);
        if (ret != ERRCODE_SUCC) {
            poll_fail_cnt++;
            if (poll_fail_cnt <= 10 || (poll_fail_cnt % 100) == 0) {
                osal_printk("[radar spi] poll write fail cnt=%u ret=0x%x gpio=%u\r\n",
                            poll_fail_cnt, ret, uapi_gpio_get_val(RADAR_NOTIFY_PIN));
            }
            osal_msleep(1);
            continue;
        }
        if (poll_fail_cnt > 0) {
            osal_printk("[radar spi] poll recovered after fail=%u\r\n", poll_fail_cnt);
            poll_fail_cnt = 0;
        }
        osal_msleep(POLL_TO_HEADER_DELAY_MS);
        if (!radar_wait_notify_high(NOTIFY_IO_TIMEOUT_MS)) {
            hdr_wait_fail_cnt++;
            if (hdr_wait_fail_cnt <= 5 || (hdr_wait_fail_cnt % 20) == 0) {
                osal_printk("[radar spi] wait header notify timeout cnt=%u gpio=%u\r\n",
                            hdr_wait_fail_cnt, uapi_gpio_get_val(RADAR_NOTIFY_PIN));
            }
            continue;
        }
        if (hdr_wait_fail_cnt > 0) {
            osal_printk("[radar spi] header notify recovered after timeout=%u\r\n", hdr_wait_fail_cnt);
            hdr_wait_fail_cnt = 0;
        }
        radar_delay_us(500);

        for (uint32_t i = 0; i < 256; i++) {
            if (i > 0) {
                osal_msleep(1);
                radar_delay_us(HEADER_TO_PAYLOAD_DELAY_US);
            }
            ret = radar_spi_read_header_resync(RADAR_SPI_BUS, &xfer_hdr, hdr_buf, i);
            if (ret != ERRCODE_SUCC) {
                continue;
            }
            if (hif_header_idle(hdr_buf)) {
                radar_frame_end_log("idle");
                radar_queue_frame_end();
                (void)radar_wait_notify_low(2);
                break;
            }
            if (hif_is_complete_ack(hdr_buf)) {
                radar_frame_end_log("ack");
                radar_queue_frame_end();
                (void)radar_wait_notify_low(2);
                break;
            }
            if (!hif_header_check8_valid(hdr_buf)) {
                g_hif_hdr_err++;
                if (g_hif_hdr_err <= 10 || (g_hif_hdr_err % 100) == 0) {
                    radar_dump_bytes("[radar spi] bad hdr", hdr_buf, RADAR_HIF_HDR_LEN);
                }
                radar_frame_end_log("bad_hdr");
                radar_spi_flush_bus(RADAR_SPI_BUS);
                break;
            }
            uint16_t hdr_len = HIF_HDR_LENGTH(hdr_buf);
            if (hdr_len == 0) {
                continue;
            }
            uint8_t hdr_check = HIF_HDR_CHECK(hdr_buf);
            uint16_t payload_len = (uint16_t)(hdr_len + (hdr_check ? RADAR_CHECK32_LEN : 0));
            if (payload_len > sizeof(payload_buf)) {
                radar_frame_end_log("payload_oversize");
                radar_spi_flush_bus(RADAR_SPI_BUS);
                g_hif_rx_drop++;
                break;
            }
            if (payload_len > g_max_hif_payload) {
                g_max_hif_payload = payload_len;
            }
            radar_delay_us(HEADER_TO_PAYLOAD_DELAY_US);
            xfer_payload.rx_buff = payload_buf;
            xfer_payload.rx_bytes = payload_len;
            xfer_payload.tx_buff = g_payload_dummy;
            xfer_payload.tx_bytes = payload_len;
            ret = uapi_spi_master_writeread(RADAR_SPI_BUS, &xfer_payload, 0xFFFFFFFF);
            if (ret != ERRCODE_SUCC) {
                osal_printk("[radar spi] payload read fail ret=0x%x len=%u\r\n", ret, payload_len);
                radar_frame_end_log("payload_fail");
                radar_spi_flush_bus(RADAR_SPI_BUS);
                g_hif_rx_drop++;
                break;
            }
            radar_frame_record_hif(payload_len);
            if (g_hif_rx_ok < 5) {
                radar_dump_bytes("[radar spi] hif hdr", hdr_buf, RADAR_HIF_HDR_LEN);
                radar_dump_bytes("[radar spi] hif payload", payload_buf, payload_len);
            }
            radar_queue_hif(hdr_buf, payload_buf, payload_len);
            if ((g_hif_rx_ok % 50) == 0 && g_hif_rx_ok > 0) {
                osal_printk("[radar spi] rx hif_ok=%u drop=%u hdr_err=%u flush=%u resync=%u ack_resync=%u max_pld=%u gpio=%u\r\n",
                            g_hif_rx_ok, g_hif_rx_drop, g_hif_hdr_err, g_spi_flush_cnt,
                            g_spi_resync_cnt, g_spi_complete_ack_resync, g_max_hif_payload,
                            uapi_gpio_get_val(RADAR_NOTIFY_PIN));
            }
        }
    }
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size,
                                  errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    if (status == ERRCODE_SUCC && mtu_size != NULL && mtu_size->mtu_size > 0) {
        g_radar_mtu = mtu_size->mtu_size;
    }
    osal_printk("[radar server] mtu changed mtu=%u status=%x\r\n", g_radar_mtu, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[radar server] start service server=%u handle=%u status=%x\r\n", server_id, handle, status);
}

static void ssaps_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                   errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
}

static void ssaps_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                    errcode_t status)
{
    unused(server_id);
    unused(conn_id);

    if (status != ERRCODE_SUCC || write_cb_para == NULL || write_cb_para->value == NULL || write_cb_para->length < 4) {
        osal_printk("[radar alarm] invalid write status=%x\r\n", status);
        return;
    }

    uint8_t *buf = write_cb_para->value;
    if (buf[0] == 0xAA && buf[1] == 0x55 && buf[2] == 0x01) {
        radar_alarm_queue_cmd(buf[3]);
        if (write_cb_para->need_rsp) {
            ssaps_send_rsp_t rsp = {
                .request_id = write_cb_para->request_id,
                .status = ERRCODE_SLE_SUCCESS,
                .value_len = 0,
                .value = NULL,
            };
            (void)ssaps_send_response(server_id, conn_id, &rsp);
        }
        return;
    }

    osal_printk("[radar server] write ignored len=%u head=%02x %02x %02x\r\n",
                write_cb_para->length, buf[0], buf[1], buf[2]);
    if (write_cb_para->need_rsp) {
        ssaps_send_rsp_t rsp = {
            .request_id = write_cb_para->request_id,
            .status = 0x0E,
            .value_len = 0,
            .value = NULL,
        };
        (void)ssaps_send_response(server_id, conn_id, &rsp);
    }
}

static void radar_ssaps_register_cbks(void)
{
    ssaps_callbacks_t cbk = {0};
    cbk.start_service_cb = ssaps_start_service_cbk;
    cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    cbk.read_request_cb = ssaps_read_request_cbk;
    cbk.write_request_cb = ssaps_write_request_cbk;
    ssaps_register_callbacks(&cbk);
}

static errcode_t radar_sle_service_add(void)
{
    sle_uuid_t uuid = {0};
    radar_sle_uuid_setu2(RADAR_SLE_UUID_SERVER_SERVICE, &uuid);
    return ssaps_add_service_sync(g_radar_server_id, &uuid, 1, &g_radar_service_handle);
}

static errcode_t radar_sle_property_add(void)
{
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x00};

    property.permissions = RADAR_SLE_TEST_PROPERTIES;
    radar_sle_uuid_setu2(RADAR_SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.value = osal_vmalloc(sizeof(g_radar_sle_property_value));
    property.value_len = sizeof(g_radar_sle_property_value);
    property.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE |
                                  SSAP_OPERATE_INDICATION_BIT_NOTIFY;
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    (void)memcpy_s(property.value, sizeof(g_radar_sle_property_value), g_radar_sle_property_value,
                   sizeof(g_radar_sle_property_value));
    errcode_t ret = ssaps_add_property_sync(g_radar_server_id, g_radar_service_handle, &property,
                                            &g_radar_property_handle);
    osal_vfree(property.value);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }

    descriptor.permissions = RADAR_SLE_TEST_DESCRIPTOR;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);
    return ssaps_add_descriptor_sync(g_radar_server_id, g_radar_service_handle, g_radar_property_handle, &descriptor);
}

static errcode_t radar_sle_server_add(void)
{
    sle_uuid_t app_uuid = {0};
    app_uuid.len = sizeof(g_radar_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_radar_sle_uuid_app_uuid,
                 sizeof(g_radar_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_radar_server_id);
    if (radar_sle_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_radar_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (radar_sle_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_radar_server_id);
        return ERRCODE_SLE_FAIL;
    }
    errcode_t ret = ssaps_start_service(g_radar_server_id, g_radar_service_handle);
    osal_printk("[radar server] add service ret=%x server=%u service=%u property=%u\r\n",
                ret, g_radar_server_id, g_radar_service_handle, g_radar_property_handle);
    return ret;
}

static void radar_sle_set_info(void)
{
    ssap_exchange_info_t info = {0};
    info.mtu_size = RADAR_DEFAULT_MTU;
    info.version = 1;
    ssaps_set_info(g_radar_server_id, &info);
}

static void radar_sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                                sle_disc_reason_t disc_reason)
{
    unused(addr);
    osal_printk("[radar server] conn=%u state=%x pair=%x reason=%x\r\n", conn_id, conn_state, pair_state, disc_reason);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_radar_connected = true;
        g_radar_conn_hdl = conn_id;
        g_radar_phy_set = false;
        sle_stop_announce(RADAR_SLE_ADV_HANDLE_DEFAULT);
        radar_sle_set_info();
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_radar_connected = false;
        g_radar_conn_hdl = 0;
        g_radar_phy_set = false;
        sle_start_announce(RADAR_SLE_ADV_HANDLE_DEFAULT);
    }
}

static void radar_sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    unused(addr);
    osal_printk("[radar server] pair conn=%u status=%x\r\n", conn_id, status);
}

static void radar_sle_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(status);
    osal_printk("[radar server] conn update conn=%u interval=%x\r\n", conn_id, param->interval);
}

static void radar_sle_update_req_cbk(uint16_t conn_id, errcode_t status,
                                     const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    osal_printk("[radar server] conn update req interval=%x-%x\r\n", param->interval_min, param->interval_max);
}

static void radar_conn_register_cbks(void)
{
    sle_connection_callbacks_t cbks = {0};
    cbks.connect_state_changed_cb = radar_sle_connect_state_changed_cbk;
    cbks.pair_complete_cb = radar_sle_pair_complete_cbk;
    cbks.connect_param_update_req_cb = radar_sle_update_req_cbk;
    cbks.connect_param_update_cb = radar_sle_update_cbk;
    sle_connection_register_callbacks(&cbks);
}

static void radar_sle_connect_param_init(void)
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

static void radar_sle_set_local_addr_init(void)
{
    sle_addr_t addr = {0};
    uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x77};
    addr.type = 0;
    (void)memcpy_s(addr.addr, SLE_ADDR_LEN, mac, SLE_ADDR_LEN);
    sle_set_local_addr(&addr);
}

static void radar_sle_server_set_nv(void)
{
    uint16_t nv_value_len = 0;
    uint8_t nv_value = 0;
    uapi_nv_read(0x20A0, sizeof(uint16_t), &nv_value_len, &nv_value);
    if (nv_value != 7) {
        nv_value = 7;
        uapi_nv_write(0x20A0, &nv_value, sizeof(nv_value));
    }
    osal_printk("[radar server] nv power=%u\r\n", nv_value);
}

#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
static void radar_sle_send_data_cbk(uint16_t conn_id, sle_link_qos_state_t link_state)
{
    unused(conn_id);
    g_sle_link_state = link_state;
}

static void radar_sle_transmission_register_cbks(void)
{
    sle_transmission_callbacks_t cbk = {0};
    cbk.send_data_cb = radar_sle_send_data_cbk;
    sle_transmission_register_callbacks(&cbk);
}
#endif

errcode_t radar_sle_server_init(void)
{
    uapi_watchdog_disable();
    enable_sle();
    radar_sle_server_set_nv();
    radar_conn_register_cbks();
    radar_ssaps_register_cbks();
    radar_sle_server_add();
    radar_sle_server_adv_init();
    radar_sle_set_info();
    radar_sle_connect_param_init();
    radar_sle_set_local_addr_init();
#ifdef SLE_QOS_FLOWCTRL_FUNCTION_SWITCH
    radar_sle_transmission_register_cbks();
#endif
    osal_printk("[radar server] init ok\r\n");
    return ERRCODE_SLE_SUCCESS;
}

static int radar_server_main(void *arg)
{
    unused(arg);
    osal_msleep(1000);

    uint32_t ret = osal_msg_queue_create("radar_sle_q", RADAR_QUEUE_LEN, &g_radar_queue_id, 0,
                                         sizeof(radar_queue_item_t));
    if (ret != OSAL_SUCCESS) {
        osal_printk("[radar server] queue create fail ret=%u\r\n", ret);
        return -1;
    }

    ret = osal_msg_queue_create("radar_alarm_q", RADAR_ALARM_QUEUE_LEN, &g_alarm_queue_id, 0,
                                sizeof(radar_alarm_msg_t));
    if (ret != OSAL_SUCCESS) {
        osal_printk("[radar alarm] queue create fail ret=%u\r\n", ret);
        return -1;
    }

    radar_alarm_gpio_init();
    radar_mp3_uart_init();

    osal_task *alarm_task = osal_kthread_create((osal_kthread_handler)radar_alarm_thread, NULL,
                                                "RadarAlarm", RADAR_ALARM_TASK_STACK_SIZE);
    if (alarm_task != NULL) {
        osal_kthread_set_priority(alarm_task, RADAR_ALARM_TASK_PRIO);
        osal_kfree(alarm_task);
    }

    osal_task *send_task = osal_kthread_create((osal_kthread_handler)radar_send_thread, NULL,
                                               "RadarSleSend", RADAR_SERVER_SEND_STACK_SIZE);
    if (send_task != NULL) {
        osal_kthread_set_priority(send_task, RADAR_SERVER_SEND_PRIO);
        osal_kfree(send_task);
    }

    osal_task *spi_task = osal_kthread_create((osal_kthread_handler)radar_spi_thread, NULL,
                                              "RadarSpi", RADAR_SERVER_TASK_STACK_SIZE);
    if (spi_task != NULL) {
        osal_kthread_set_priority(spi_task, RADAR_SERVER_TASK_PRIO);
        osal_kfree(spi_task);
    }

    radar_sle_server_init();
    return 0;
}

static void radar_sle_server_entry(void)
{
    osal_task *task = NULL;
    osal_kthread_lock();
    task = osal_kthread_create((osal_kthread_handler)radar_server_main, NULL,
                               "RadarServer", RADAR_SERVER_TASK_STACK_SIZE);
    if (task != NULL) {
        osal_kthread_set_priority(task, RADAR_SERVER_TASK_PRIO);
        osal_kfree(task);
    }
    osal_kthread_unlock();
}

app_run(radar_sle_server_entry);
