#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "gpio.h"
#include "pinctrl.h"
#include "hal_gpio.h"
#include "systick.h"
#include "spi.h"
#include "dma.h"
#include "errcode.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "securec.h"

#include "sle_uart_server.h"
#include "radar_sle_proto.h"

/* ============ 配置 ============ */
#define RADAR_TASK_STACK_SIZE    0x2000
#define SLE_SEND_TASK_STACK_SIZE 0x1000
#define RADAR_TASK_PRIO          osPriorityAboveNormal
#define SLE_SEND_TASK_PRIO       osPriorityNormal

#define RADAR_HIF_HDR_LEN       6
#define RADAR_MAX_PAYLOAD       4096
#define RADAR_CHECK32_LEN       4
#define RADAR_MAX_HIF_PDU       (RADAR_HIF_HDR_LEN + RADAR_MAX_PAYLOAD + RADAR_CHECK32_LEN)

#define SLE_FRAG_PAYLOAD_MAX    200
#define SLE_FRAG_DELAY_MS       3
#define SLE_HIF_DELAY_MS        5

#define POLL_TO_HEADER_DELAY_MS    5
#define HEADER_TO_PAYLOAD_DELAY_US 300
#define NOTIFY_IO_TIMEOUT_MS       15

/* HIF Header 解析宏 */
#define HIF_HDR_TYPE(hdr)     ((hdr)[2] & 0x03)
#define HIF_HDR_CHECK(hdr)    (((hdr)[2] >> 4) & 0x01)
#define HIF_HDR_MORE(hdr)     (((hdr)[2] >> 5) & 0x01)
#define HIF_HDR_MSGID(hdr)    ((hdr)[3])
#define HIF_HDR_LENGTH(hdr)   ((uint16_t)((hdr)[4]) | ((uint16_t)((hdr)[5] & 0x0F) << 8))
#define HIF_HDR_SEQ(hdr)      (((hdr)[5] >> 4) & 0x07)

/* ============ 帧双缓冲 ============ */
#define FRAME_BUF_SIZE          (32 * 1024)
#define FRAME_MAX_HIF_PKTS      64

typedef struct {
    uint8_t  hdr[RADAR_HIF_HDR_LEN];
    uint16_t payload_offset;
    uint16_t payload_check_len;
} hif_index_t;

typedef struct {
    uint8_t     buf[FRAME_BUF_SIZE];
    hif_index_t index[FRAME_MAX_HIF_PKTS];
    int         hif_count;
    uint32_t    buf_used;
    bool        complete;
} frame_slot_t;

#define FRAME_SLOT_NUM  2
static frame_slot_t g_frame_slots[FRAME_SLOT_NUM];
static osMessageQueueId_t g_sle_send_q = NULL;

/* ============ 全局状态 ============ */
static volatile char gpio_isr_flag = 0;
static uint16_t g_sle_seq = 0;
static uint32_t g_send_ok = 0;
static uint32_t g_send_fail = 0;
static uint32_t g_frame_drop = 0;
static uint32_t g_hdr_err_cnt = 0;
static uint32_t g_poll_cnt = 0;
static uint32_t g_flush_cnt = 0;

/* SLE 诊断 */
static uint32_t g_sle_notify_ok = 0;
static uint32_t g_sle_notify_fail = 0;
static uint16_t g_sle_max_sent = 0;
static uint16_t g_sle_min_sent = 9999;
static uint16_t g_max_hif_payload = 0;

/* ★ flush 用的静态缓冲区 */
static uint8_t g_flush_buf[512];
static uint8_t g_flush_tx[512];

static void gpio_isr_handler(pin_t pin, uintptr_t param)
{
    (void)param;
    if (pin == GPIO_03) gpio_isr_flag = 1;
    uapi_gpio_clear_interrupt(pin);
}

static void delay_us(uint32_t us)
{
    uint64_t start = uapi_systick_get_us();
    while ((uapi_systick_get_us() - start) < (uint64_t)us) {}
}

static bool hif_header_check8_valid(const uint8_t hdr[6])
{
    uint8_t sum = 0;
    for (int i = 0; i < 6; i++) sum += hdr[i];
    return (sum == 0xFF);
}

static bool hif_is_complete_ack(const uint8_t hdr[6])
{
    return (HIF_HDR_TYPE(hdr) == 0x03 &&
            HIF_HDR_MSGID(hdr) == 0x0C &&
            HIF_HDR_LENGTH(hdr) == 0);
}

static bool wait_notify_io_high(uint32_t timeout_ms)
{
    uint64_t start = uapi_systick_get_us();
    uint64_t timeout_us = (uint64_t)timeout_ms * 1000;
    while (1) {
        if (uapi_gpio_get_val(GPIO_03) == GPIO_LEVEL_HIGH) return true;
        if ((uapi_systick_get_us() - start) > timeout_us) return false;
        delay_us(50);
    }
}

static uint16_t min_u16(uint16_t a, uint16_t b) { return (a < b) ? a : b; }

/* ★★★ flush：把总线上残留数据全部读掉，让 Device 回到空闲 ★★★
 *
 * 策略：
 *   1. 循环读 512 字节 × 最多 20 轮 = 10KB
 *   2. 如果读到全 0xFF 说明 Device 没数据了 → 成功
 *   3. 如果 IO 变低了说明 Device 回到空闲 → 成功
 *   4. 在读的过程中如果碰到了有效的 complete ack header → 也算成功
 */
static void spi_flush_bus(spi_bus_t bus)
{
    spi_xfer_data_t xfer = {0};
    xfer.rx_buff  = g_flush_buf;
    xfer.rx_bytes = sizeof(g_flush_buf);
    xfer.tx_buff  = g_flush_tx;
    xfer.tx_bytes = sizeof(g_flush_tx);

    g_flush_cnt++;

    for (int round = 0; round < 20; round++) {
        /* IO 已经低了 → Device 空闲了 */
        if (uapi_gpio_get_val(GPIO_03) == GPIO_LEVEL_LOW) {
            return;
        }

        osal_msleep(1);
        errcode_t ret = uapi_spi_master_writeread(bus, &xfer, 0xFFFFFFFF);
        if (ret != ERRCODE_SUCC) continue;

        /* 全 0xFF → 无数据了 */
        bool all_idle = true;
        for (int i = 0; i < (int)sizeof(g_flush_buf); i++) {
            if (g_flush_buf[i] != 0xFF && g_flush_buf[i] != 0x00) {
                all_idle = false;
                break;
            }
        }
        if (all_idle) {
            return;
        }

        /* 在数据流中搜索 complete ack: A5 4B 03 0C 00 00 */
        for (int i = 0; i <= (int)(sizeof(g_flush_buf) - 6); i++) {
            if (g_flush_buf[i] == 0xA5 &&
                hif_header_check8_valid(&g_flush_buf[i]) &&
                hif_is_complete_ack(&g_flush_buf[i])) {
                return;
            }
        }
    }

    /* 20 轮还没清完，等 IO 变低（Device 超时后会自己停，通常 < 100ms） */
    uint64_t start = uapi_systick_get_us();
    while ((uapi_systick_get_us() - start) < 200000ULL) {  /* 200ms */
        if (uapi_gpio_get_val(GPIO_03) == GPIO_LEVEL_LOW) return;
        osal_msleep(5);
    }
}

/* ============ SLE 发送线程 ============ */
static errcode_t sle_send_frame_end(void)
{
    radar_sle_hdr_t h = {0};
    h.magic = RADAR_SLE_MAGIC;
    h.type  = RADAR_SLE_TYPE_FRAME_END;
    h.flags = RADAR_SLE_FLAG_FIRST | RADAR_SLE_FLAG_LAST;
    h.seq   = g_sle_seq++;
    return sle_uart_server_send_bytes((const uint8_t *)&h, (uint16_t)sizeof(h));
}

static errcode_t sle_send_hif_fragmented(const uint8_t *data, uint16_t len)
{
    static uint8_t txbuf[512];
    uint16_t offset = 0;
    uint16_t seq = g_sle_seq++;
    uint16_t frag_cnt = 0;

    while (offset < len) {
        uint16_t chunk = min_u16(SLE_FRAG_PAYLOAD_MAX, (uint16_t)(len - offset));
        radar_sle_hdr_t h = {0};
        h.magic     = RADAR_SLE_MAGIC;
        h.type      = RADAR_SLE_TYPE_HIF_PKT;
        h.flags     = 0;
        if (offset == 0)                       h.flags |= RADAR_SLE_FLAG_FIRST;
        if ((uint16_t)(offset + chunk) >= len) h.flags |= RADAR_SLE_FLAG_LAST;
        h.seq       = seq;
        h.total_len = len;
        h.offset    = offset;
        h.chunk_len = chunk;

        (void)memcpy_s(txbuf, sizeof(txbuf), &h, sizeof(h));
        (void)memcpy_s(txbuf + sizeof(h), sizeof(txbuf) - sizeof(h), data + offset, chunk);

        if (frag_cnt > 0) osal_msleep(SLE_FRAG_DELAY_MS);

        uint16_t send_len = (uint16_t)(RADAR_SLE_HDR_LEN + chunk);
        errcode_t ret = sle_uart_server_send_bytes(txbuf, send_len);

        if (ret == ERRCODE_SUCC) {
            g_sle_notify_ok++;
            if (send_len > g_sle_max_sent) g_sle_max_sent = send_len;
            if (send_len < g_sle_min_sent) g_sle_min_sent = send_len;
        } else {
            g_sle_notify_fail++;
            osal_msleep(20);
            ret = sle_uart_server_send_bytes(txbuf, send_len);
            if (ret == ERRCODE_SUCC) {
                g_sle_notify_ok++;
                if (send_len > g_sle_max_sent) g_sle_max_sent = send_len;
                if (send_len < g_sle_min_sent) g_sle_min_sent = send_len;
            } else {
                return ret;
            }
        }

        frag_cnt++;
        offset = (uint16_t)(offset + chunk);
    }
    return ERRCODE_SUCC;
}

static void SleSendTask(void *arg)
{
    (void)arg;
    static uint8_t send_buf[RADAR_MAX_HIF_PDU];
    uint8_t slot_idx = 0;

    osal_printk("[SLE_SEND] task start\r\n");

    while (1) {
        if (osMessageQueueGet(g_sle_send_q, &slot_idx, NULL, osWaitForever) != osOK) continue;
        while (!sle_uart_server_is_ready()) osal_msleep(10);

        frame_slot_t *slot = &g_frame_slots[slot_idx];

        for (int k = 0; k < slot->hif_count; k++) {
            hif_index_t *idx = &slot->index[k];
            uint16_t total = (uint16_t)(RADAR_HIF_HDR_LEN + idx->payload_check_len);
            if (total > RADAR_MAX_HIF_PDU) continue;

            (void)memcpy_s(send_buf, sizeof(send_buf), idx->hdr, RADAR_HIF_HDR_LEN);
            (void)memcpy_s(send_buf + RADAR_HIF_HDR_LEN,
                           sizeof(send_buf) - RADAR_HIF_HDR_LEN,
                           slot->buf + idx->payload_offset,
                           idx->payload_check_len);

            if (sle_send_hif_fragmented(send_buf, total) == ERRCODE_SUCC) {
                g_send_ok++;
            } else {
                g_send_fail++;
            }
            osal_msleep(SLE_HIF_DELAY_MS);
        }

        if (slot->complete) {
            (void)sle_send_frame_end();
            osal_msleep(SLE_FRAG_DELAY_MS);
        }

        if ((g_send_ok % 200) == 0 && g_send_ok > 0) {
            osal_printk("[DIAG] mtu=%u frag=%u | ntf ok=%u fail=%u send[%u~%u] | "
                        "HIF ok=%u fail=%u drop=%u err=%u flush=%u pld=%u poll=%u\r\n",
                        (unsigned)sle_uart_server_get_mtu(),
                        (unsigned)SLE_FRAG_PAYLOAD_MAX,
                        g_sle_notify_ok, g_sle_notify_fail,
                        (unsigned)g_sle_min_sent, (unsigned)g_sle_max_sent,
                        g_send_ok, g_send_fail, g_frame_drop,
                        g_hdr_err_cnt, g_flush_cnt,
                        (unsigned)g_max_hif_payload, g_poll_cnt);
        }
    }
}

/* ============ SPI 接收主任务 ============ */
static void *RadarTask(const char *arg)
{
    (void)arg;
    errcode_t ret;
    uint8_t cur_slot = 0;

    osal_printk("[RADAR] waiting for SLE init...\r\n");
    osDelay(2000);
    osal_printk("[RADAR] RadarTask start\r\n");

    /* ---- GPIO ---- */
    uapi_pin_init();
    uapi_pin_set_mode(GPIO_03, PIN_MODE_0);
    uapi_gpio_init();
    uapi_gpio_set_dir(GPIO_03, GPIO_DIRECTION_INPUT);
    uapi_systick_init();
    uapi_gpio_register_isr_func(GPIO_03, 1, gpio_isr_handler);
    uapi_gpio_enable_interrupt(GPIO_03);

    /* ---- SPI ---- */
    uapi_pin_set_mode(GPIO_07, PIN_MODE_3);
    uapi_pin_set_mode(GPIO_09, PIN_MODE_3);
    uapi_pin_set_mode(GPIO_10, PIN_MODE_3);
    uapi_pin_set_mode(GPIO_11, PIN_MODE_3);

    spi_attr_t spi0_conf;
    spi0_conf.is_slave         = false;
    spi0_conf.slave_num        = 1u;
    spi0_conf.bus_clk          = 32000000u;
    spi0_conf.freq_mhz         = 1;
    spi0_conf.clk_polarity     = SPI_CFG_CLK_CPOL_0;
    spi0_conf.clk_phase        = SPI_CFG_CLK_CPHA_0;
    spi0_conf.frame_format     = SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI;
    spi0_conf.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    spi0_conf.frame_size       = HAL_SPI_FRAME_SIZE_8;
    spi0_conf.tmod             = HAL_SPI_TRANS_MODE_TXRX;
    spi0_conf.ndf              = 0;
    spi0_conf.sste             = SPI_CFG_SSTE_DISABLE;

    spi_extra_attr_t spi0_extra_conf;
    memset(&spi0_extra_conf, 0, sizeof(spi0_extra_conf));
    spi0_extra_conf.tx_use_dma = true;
    spi0_extra_conf.rx_use_dma = true;
    spi0_extra_conf.sspi_param.wait_cycles = 0;

    ret = uapi_spi_init(SPI_BUS_0, &spi0_conf, &spi0_extra_conf);
    if (ret == ERRCODE_SUCC) {
        osal_printk("[RADAR] SPI init ok\r\n");
    } else {
        osal_printk("[RADAR] SPI init fail: 0x%x\r\n", ret);
        return NULL;
    }

    uapi_dma_init();
    uapi_dma_open();
    spi_dma_config_t dma_cfg = {
        .src_width = 0, .dest_width = 0, .burst_length = 0, .priority = 0
    };
    if (uapi_spi_set_dma_mode(SPI_BUS_0, true, &dma_cfg) != ERRCODE_SUCC) {
        osal_printk("[RADAR] DMA mode fail\r\n");
    }

    spi_xfer_data_t xfer_tx = {0};
    uint8_t poll_cmd[] = {
        0xA5, 0x35, 0x15, 0x0C, 0x04, 0x00,
        0x01, 0x00, 0x00, 0x01,
        0xE9, 0xF3, 0xFB, 0xFE
    };
    uint8_t tx_rxdummy[14] = {0};
    xfer_tx.tx_buff  = poll_cmd;
    xfer_tx.tx_bytes = sizeof(poll_cmd);
    xfer_tx.rx_buff  = tx_rxdummy;
    xfer_tx.rx_bytes = sizeof(tx_rxdummy);

    spi_xfer_data_t xfer_hdr = {0};
    uint8_t hdr_buf[6] = {0};
    uint8_t hdr_txdummy[6] = {0};
    xfer_hdr.rx_buff  = hdr_buf;
    xfer_hdr.rx_bytes = 6;
    xfer_hdr.tx_buff  = hdr_txdummy;
    xfer_hdr.tx_bytes = 6;

    spi_xfer_data_t xfer_pld = {0};

    uint8_t *pld_txdummy = (uint8_t *)malloc((size_t)4096 + 4);
    if (pld_txdummy == NULL) {
        osal_printk("[RADAR] malloc fail\r\n");
        return NULL;
    }
    memset(pld_txdummy, 0, (size_t)4096 + 4);

    /* ======== 主循环 ======== */
    while (1) {

        if (gpio_isr_flag != 1) {
            osal_msleep(1);
            continue;
        }
        gpio_isr_flag = 0;
        g_poll_cnt++;

        /* 发 Poll */
        ret = uapi_spi_master_writeread(SPI_BUS_0, &xfer_tx, 0xFFFFFFFF);
        if (ret != ERRCODE_SUCC) continue;

        osal_msleep(POLL_TO_HEADER_DELAY_MS);
        if (!wait_notify_io_high(NOTIFY_IO_TIMEOUT_MS)) continue;
        delay_us(500);   // ★ IO 高之后再等 500us，让 SPI Tx buffer 就绪

        frame_slot_t *slot = &g_frame_slots[cur_slot];
        slot->hif_count = 0;
        slot->buf_used  = 0;
        slot->complete  = false;

        for (int i = 0; i < 256; i++) {

            if (i > 0) 
            {
                osal_msleep(1);
                delay_us(300);  // ★ 多等 300us
            }

            ret = uapi_spi_master_writeread(SPI_BUS_0, &xfer_hdr, 0xFFFFFFFF);
            if (ret != ERRCODE_SUCC) { osal_msleep(1); continue; }

            /* idle */
            if ((hdr_buf[0] == 0xFF && hdr_buf[1] == 0xFF &&
                 hdr_buf[2] == 0xFF && hdr_buf[3] == 0xFF &&
                 hdr_buf[4] == 0xFF && hdr_buf[5] == 0xFF) ||
                (hdr_buf[0] == 0x00 && hdr_buf[1] == 0x00 &&
                 hdr_buf[2] == 0x00 && hdr_buf[3] == 0x00 &&
                 hdr_buf[4] == 0x00 && hdr_buf[5] == 0x00)) {
                slot->complete = true;
                break;
            }

            /* check8 失败 → flush */
            if (!hif_header_check8_valid(hdr_buf)) {
                g_hdr_err_cnt++;

                /* ★★★ 不再 retry 6 字节，直接 flush 整个总线 ★★★
                 *
                 * retry 只读 6 字节没用——总线上可能有几千字节脏数据。
                 * 必须把脏数据全读掉，Device 才能正常发 complete ack，
                 * 然后回到空闲状态。
                 *
                 * 如果不 flush，Device 会卡在 "等 Host 读数据" 的状态，
                 * IO 一直高，然后系统就死了。
                 */
                spi_flush_bus(SPI_BUS_0);

                /* flush 后这一帧的数据肯定不完整了，直接放弃 */
                slot->hif_count = 0;
                slot->buf_used  = 0;
                slot->complete  = false;
                break;
            }

            /* complete ack */
            if (hif_is_complete_ack(hdr_buf)) {
                slot->complete = true;
                break;
            }

            uint16_t hdr_len = HIF_HDR_LENGTH(hdr_buf);
            if (hdr_len == 0) continue;

            uint8_t hdr_check = HIF_HDR_CHECK(hdr_buf);
            delay_us(HEADER_TO_PAYLOAD_DELAY_US);

            uint32_t pld_len = (uint32_t)hdr_len + (hdr_check ? 4 : 0);
            if (pld_len > g_max_hif_payload) g_max_hif_payload = (uint16_t)pld_len;

            if (slot->buf_used + pld_len > FRAME_BUF_SIZE ||
                slot->hif_count >= FRAME_MAX_HIF_PKTS) {
                xfer_pld.rx_buff  = pld_txdummy;
                xfer_pld.rx_bytes = pld_len;
                xfer_pld.tx_buff  = pld_txdummy;
                xfer_pld.tx_bytes = pld_len;
                (void)uapi_spi_master_writeread(SPI_BUS_0, &xfer_pld, 0xFFFFFFFF);
                continue;
            }

            uint8_t *dest = slot->buf + slot->buf_used;
            xfer_pld.rx_buff  = dest;
            xfer_pld.rx_bytes = pld_len;
            xfer_pld.tx_buff  = pld_txdummy;
            xfer_pld.tx_bytes = pld_len;

            ret = uapi_spi_master_writeread(SPI_BUS_0, &xfer_pld, 0xFFFFFFFF);
            if (ret != ERRCODE_SUCC) {
                /* payload 读失败也 flush */
                spi_flush_bus(SPI_BUS_0);
                slot->hif_count = 0;
                slot->buf_used  = 0;
                slot->complete  = false;
                break;
            }

            hif_index_t *idx = &slot->index[slot->hif_count];
            (void)memcpy_s(idx->hdr, RADAR_HIF_HDR_LEN, hdr_buf, RADAR_HIF_HDR_LEN);
            idx->payload_offset    = (uint16_t)slot->buf_used;
            idx->payload_check_len = (uint16_t)pld_len;

            slot->buf_used += pld_len;
            slot->hif_count++;
        }

        /* 投递给 SLE（只投正常帧） */
        if (slot->hif_count > 0 || slot->complete) {
            if (osMessageQueuePut(g_sle_send_q, &cur_slot, 0, 0) == osOK) {
                cur_slot = (uint8_t)((cur_slot + 1) % FRAME_SLOT_NUM);
            } else {
                g_frame_drop++;
            }
        }

        osal_msleep(1);
    }

    free(pld_txdummy);
    return NULL;
}

/* ============ 入口 ============ */
static void RadarEntry(void)
{
    g_sle_send_q = osMessageQueueNew(FRAME_SLOT_NUM, sizeof(uint8_t), NULL);
    if (g_sle_send_q == NULL) {
        osal_printk("[RADAR] queue fail\r\n");
        return;
    }

    osThreadAttr_t sle_attr = {0};
    sle_attr.name       = "SleSendTask";
    sle_attr.stack_size = SLE_SEND_TASK_STACK_SIZE;
    sle_attr.priority   = SLE_SEND_TASK_PRIO;
    if (osThreadNew(SleSendTask, NULL, &sle_attr) == NULL) {
        osal_printk("[RADAR] SleSendTask fail\r\n");
        return;
    }

    osThreadAttr_t attr = {0};
    attr.name       = "RadarTask";
    attr.stack_size = RADAR_TASK_STACK_SIZE;
    attr.priority   = RADAR_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)RadarTask, NULL, &attr) == NULL) {
        osal_printk("[RADAR] RadarTask fail\r\n");
    }
}

SYS_RUN(RadarEntry);