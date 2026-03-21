/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     ck_i2s.c
 * @brief    CSI Source File for I2S Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <csi_config.h>
#include <string.h>
#include <string.h>
#include <drv_i2s.h>
#include <ck_i2s.h>
#if defined CONFIG_DW_DMAC
#include <drv_dmac.h>
#endif

#define ERR_I2S(errno) (CSI_DRV_ERRNO_I2S_BASE | errno)
#define I2S_BUSY_TIMEOUT    0x1000000
#define I2S_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_I2S(DRV_ERROR_PARAMETER))
typedef struct  {
#ifdef CONFIG_LPM
    uint8_t i2s_power_status;
    uint32_t i2s_regs_saved[22];
#endif
    uint32_t base;
    uint32_t irq;
    i2s_event_cb_t cb_event;
    uint32_t src_clk;
    i2s_protocol_e audio_fmt;
    uint32_t sample_rate;
    uint32_t samplewidth;
    i2s_protocol_e protocol;
    uint32_t send_num;
    uint32_t recv_num;
    uint32_t *send_buf;
    uint32_t *recv_buf;
    uint32_t tot_send_num;
    uint32_t tot_recv_num;
    i2s_status_t status;                // status of i2s transfer
    int32_t  idx;
    int32_t dma_tx_ch;
    int32_t dma_rx_ch;
} ck_i2s_priv_t;

static ck_i2s_priv_t i2s_instance[CONFIG_I2S_NUM];

static const i2s_capabilities_t i2s_capabilities = {
    .asynchronous       = 1,    //supports asynchronous Transmit/Receive
    .synchronous        = 0,    //supports synchronous Transmit/Receive
    .protocol_user      = 0,    //supports user defined Protocol
    .protocol_i2s       = 1,    //supports I2S Protocol
    .protocol_justified = 1,    //supports MSB/LSB justified Protocol
    .mono_mode          = 0,    //supports Mono mode
    .companding         = 0,    //supports Companding
    .mclk_pin           = 0,    //supports MCLK (Master Clock) pin
    .event_frame_error  = 1,    //supports Frame error event: ARM_SAI_EVENT_FRAME_ERROR
};
extern int32_t target_i2s_init(int32_t idx, uint32_t *base, uint32_t *irq);
extern int32_t target_get_addr_space(uint32_t addr);

#if defined CONFIG_DW_DMAC
static void dma_event_cb_fun(int32_t ch, dma_event_e event)
{
    int8_t priv_num  = 0;
    ck_i2s_priv_t *i2s_priv = NULL;

    for (priv_num = 0; priv_num < CONFIG_I2S_NUM; priv_num++) {
        i2s_priv = &i2s_instance[priv_num];

        if ((i2s_priv->dma_tx_ch == ch) || (i2s_priv->dma_rx_ch == ch)) {
            break;
        }
    }

    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    if (event == DMA_EVENT_TRANSFER_ERROR) {
        if (i2s_priv->dma_tx_ch == ch) {
            addr->I2S_AUDIOEN &= ~(AUDIOEN_IIS_EN);
            addr->I2S_DMACR   &= ~I2SDMACR_TX_ENABLE;
            i2s_priv->dma_tx_ch = -1;
            i2s_priv->send_buf = NULL;

            csi_dma_stop(i2s_priv->dma_tx_ch);
            csi_dma_release_channel(i2s_priv->dma_tx_ch);
            i2s_priv->status.frame_error = 0U;

            if (i2s_priv->cb_event) {
                i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_FRAME_ERROR);
            }

            return;
        } else {
            addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;
            addr->I2S_DMACR   &= ~I2SDMACR_RX_ENABLE;
            i2s_priv->dma_rx_ch = -1;
            i2s_priv->recv_buf = NULL;

            csi_dma_stop(i2s_priv->dma_rx_ch);
            csi_dma_release_channel(i2s_priv->dma_rx_ch);
            i2s_priv->status.frame_error = 0U;

            if (i2s_priv->cb_event) {
                i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_FRAME_ERROR);
            }

            return;
        }
    } else if (event == DMA_EVENT_TRANSFER_DONE) {
        /* send done */
        if (i2s_priv->dma_tx_ch == ch) {
            i2s_priv->tot_send_num -= i2s_priv->send_num;
            i2s_priv->send_buf += (i2s_priv->send_num / 4);

            if (i2s_priv->tot_send_num == 0) {
                addr->I2S_AUDIOEN &= ~(AUDIOEN_IIS_EN);
                addr->I2S_DMACR   &= ~I2SDMACR_TX_ENABLE;
                i2s_priv->dma_tx_ch = -1;
                i2s_priv->send_buf = NULL;
                i2s_priv->status.tx_busy = 0U;

                csi_dma_stop(i2s_priv->dma_tx_ch);
                csi_dma_release_channel(i2s_priv->dma_tx_ch);

                if (i2s_priv->cb_event) {
                    i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_SEND_COMPLETE);
                }

                return;
            }

            if (i2s_priv->tot_send_num >= 1024 * 4) {
                i2s_priv->send_num = 1024 * 4;
            } else {
                i2s_priv->send_num = i2s_priv->tot_send_num;
            }

            csi_dma_start(i2s_priv->dma_tx_ch, (void *)i2s_priv->send_buf, (void *)&addr->I2S_DR, i2s_priv->send_num / 4);

        } else if (i2s_priv->dma_rx_ch == ch) {
            i2s_priv->tot_recv_num -= i2s_priv->recv_num;
            i2s_priv->recv_buf += (i2s_priv->recv_num / 4);

            if (i2s_priv->tot_recv_num == 0) {
                addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;
                addr->I2S_DMACR   &= ~I2SDMACR_RX_ENABLE;
                i2s_priv->dma_rx_ch = -1;
                i2s_priv->recv_buf = NULL;
                i2s_priv->status.rx_busy = 0U;

                csi_dma_stop(i2s_priv->dma_rx_ch);
                csi_dma_release_channel(i2s_priv->dma_rx_ch);

                if (i2s_priv->cb_event) {
                    i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_RECEIVE_COMPLETE);
                }

                return;
            }

            if (i2s_priv->tot_recv_num >= 1024 * 4) {
                i2s_priv->recv_num = 1024 * 4;
            } else {
                i2s_priv->recv_num = i2s_priv->tot_recv_num;
            }

            uint8_t i;

            for (i = 2; i > 0; i--) {
                if (!((i2s_priv->recv_num / 4) % (2 << i))) {
                    break;
                }
            }

            addr->I2S_DMARDLR = (2 << i) - 1;
            csi_dma_start(i2s_priv->dma_rx_ch, (void *)&addr->I2S_DR, i2s_priv->recv_buf, i2s_priv->recv_num / 4);
        }
    }

}
static int32_t ck_i2s_dma_send(ck_i2s_priv_t *i2s_priv, const void *data, uint32_t data_size)
{
    dma_config_t config;
    int ret;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    if (target_get_addr_space((uint32_t)i2s_priv->send_buf) != ADDR_SPACE_SRAM) {
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_tw = 4; //unit: 4 bytes
    config.dst_tw = 4; //unit: 4 bytes
    config.hs_if  = CKENUM_DMA_IIS_TX;
    config.type   = DMA_MEM2PERH;
    i2s_priv->tot_send_num = data_size;
    i2s_priv->send_buf = (uint32_t *)data;

    if (i2s_priv->dma_tx_ch == -1) {
        i2s_priv->dma_tx_ch = csi_dma_alloc_channel();


        if (i2s_priv->dma_tx_ch == -1) {
            return  -1;
        }
    }

    ret = csi_dma_config_channel(i2s_priv->dma_tx_ch, &config, dma_event_cb_fun);

    if (ret < 0) {
        csi_dma_release_channel(i2s_priv->dma_tx_ch);
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    addr->I2S_DMACR = I2SDMACR_TX_ENABLE;
    addr->I2S_DMATDLR = 0x7;

    addr->I2S_AUDIOEN = AUDIOEN_IIS_EN;

    if (i2s_priv->tot_send_num >= 1024 * 4) {
        i2s_priv->send_num = 1024 * 4;
    } else {
        i2s_priv->send_num = i2s_priv->tot_send_num;
    }

    csi_dma_start(i2s_priv->dma_tx_ch, (void *)i2s_priv->send_buf, (void *)&addr->I2S_DR, i2s_priv->send_num / 4);

    return 0;
}
int32_t ck_i2s_dma_recv(ck_i2s_priv_t *i2s_priv, void *data, uint32_t data_size)
{
    dma_config_t config;
    int ret;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    memset(&config, 0, sizeof(dma_config_t));

    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_tw = 4; //unit: 4 bytes
    config.dst_tw = 4; //unit: 4 bytes
    config.hs_if  = CKENUM_DMA_IIS_RX;
    config.type   = DMA_PERH2MEM;

    if (i2s_priv->dma_rx_ch == -1) {
        i2s_priv->dma_rx_ch = csi_dma_alloc_channel();

        if (i2s_priv->dma_rx_ch == -1) {
            return  -1;
        }
    }

    i2s_priv->tot_recv_num = data_size;
    i2s_priv->recv_buf = data;

    ret = csi_dma_config_channel(i2s_priv->dma_rx_ch, &config, dma_event_cb_fun);

    if (ret < 0) {
        csi_dma_release_channel(i2s_priv->dma_rx_ch);
        return -1;
    }

    if (i2s_priv->tot_recv_num >= 1024 * 4) {
        i2s_priv->recv_num = 1024 * 4;
    } else {
        i2s_priv->recv_num = i2s_priv->tot_recv_num;
    }

    csi_dma_start(i2s_priv->dma_rx_ch, (void *)&addr->I2S_DR, i2s_priv->recv_buf, i2s_priv->recv_num / 4);

    addr->I2S_DMACR = I2SDMACR_RX_ENABLE;
    uint8_t i;

    for (i = 2; i > 0; i--) {
        if (!((i2s_priv->recv_num / 4) % (2 << i))) {
            break;
        }
    }

    addr->I2S_DMARDLR = (2 << i) - 1;

    addr->I2S_AUDIOEN = AUDIOEN_IIS_EN;
    return 0;
}
#endif
static inline void ck_i2s_enable(ck_i2s_priv_t *i2s_priv)
{
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_AUDIOEN |= AUDIOEN_IIS_EN;   /* i2s enable */
}

static inline void ck_i2s_disable(ck_i2s_priv_t *i2s_priv)
{
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;   /* i2s disable */
}

static int32_t ck_i2s_calc_mclk_div(ck_i2s_priv_t *i2s_priv, uint32_t rate, uint32_t samplewidth)
{
    uint32_t mclk;
    uint32_t div;

    if (samplewidth == 16) {
        mclk = 256 * rate;
    } else if (samplewidth == 24) {
        mclk = 384 * rate;
    } else {
        return -1;
    }

    /* div0 = src_clk/(2*mclk) - 1; */
    div = I2S_SRC_CLK / 2 / mclk;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_DIV0_LEVEL = div;
    return 0;
}

static int32_t ck_i2s_calc_spdifclk_div(ck_i2s_priv_t *i2s_priv, uint32_t rate, uint32_t samplewidth)
{
    /* DIV1_LEVEL[0X94] usually is configured as 17 or 11.  Why ? */
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_DIV1_LEVEL = 17;
    return 0;
}

static int32_t ck_i2s_calc_fs_div(ck_i2s_priv_t *i2s_priv, uint32_t samplewidth)
{
    uint32_t multi; /* sclk = multi * fs */
    uint32_t div;

    if ((i2s_priv->protocol == I2S_PROTOCOL_I2S) ||
        (i2s_priv->protocol == I2S_PROTOCOL_MSB_JUSTIFIED)) {
        if (samplewidth == 16) {
            multi = 32;    /* sclk=32fs */
        } else if (samplewidth == 24) {
            multi = 48;    /* sclk=48fs */
        } else {
            return -1;
        }
    } else { /* SND_SOC_DAIFMT_RIGHT_J */
        if (samplewidth == 16) {
            multi = 64;    /* sclk=64fs */
        } else if (samplewidth == 24) {
            multi = 96;    /* sclk=96fs */
        } else {
            return -1;
        }
    }

    /* div2 = sclk/(2*fs) - 1 = multi/2 - 1; */
    div = multi / 2 - 1;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_DIV2_LEVEL = div;
    return 0;
}

static int32_t ck_i2s_calc_refclk_div(ck_i2s_priv_t *i2s_priv, uint32_t rate)
{
    uint32_t ref_clk;
    uint32_t div;

    switch (rate) {
        /* clk_domain_1/2/3: ref_clk = 3072khz */
        case 8000:
        case 16000:
        case 32000:
        case 48000:
        case 96000:
            ref_clk = 3072000;
            break;

        /* clk_domain_4: ref_clk = 2116.8khz */
        case 11025:
        case 22050:
        case 44100:
        case 88200:
            ref_clk = 2116800;
            break;

        default:
            return -1;
    }

    /* div3 = src_clk / (ref_clk*2) - 1; */
    div = I2S_SRC_CLK / 2 / ref_clk - 1;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_DIV3_LEVEL = div;
    return 0;
}

/**
\brief       interrupt service function for receive FIFO full interrupt .
\param[in]   i2s_priv pointer to spi private.
*/
void ck_i2s_intr_rx_full(ck_i2s_priv_t *i2s_priv)
{
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    uint8_t temp;
    uint32_t *pbuffer = i2s_priv->recv_buf;
    uint32_t length = i2s_priv->tot_recv_num;
    uint8_t rxnum;

    rxnum = addr->I2S_RXFLR;
    uint32_t i = 0u;
    rxnum = (rxnum > length) ? length : rxnum;

    for (i = 0; i < rxnum; i++) {
        *pbuffer = addr->I2S_DR;
        pbuffer++;
    }

    length -= rxnum;
    i2s_priv->recv_num += rxnum;

    if (length <= 0) {
        temp = addr->I2S_IMR;
        temp &= ~I2SIMR_RXFIM;
        addr->I2S_IMR = temp;
        i2s_priv->status.tx_busy = 0U;

        i2s_priv->recv_num = 0;
        addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;   /* i2s disable */

        if (i2s_priv->cb_event) {
            i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_RECEIVE_COMPLETE);
        }

        return;
    } else {
        i2s_priv->recv_buf = pbuffer;
        i2s_priv->tot_recv_num = length;

        if (length > CK_I2S_FIFO_MAX_LV) {
            addr->I2S_RXFTLR = CK_I2S_FIFO_MAX_LV / 2 - 1;
        } else {
            addr->I2S_RXFTLR = length - 1;
        }
    }
}

/**
\brief       interrupt service function for transmit FIFO empty interrupt.
\param[in]   i2s_priv pointer to spi private.
*/
void ck_i2s_intr_tx_empty(ck_i2s_priv_t *i2s_priv)
{
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    uint8_t temp;
    uint8_t send_num;
    /* transfer mode:transmit & receive */
    uint32_t i = 0u;

    if (i2s_priv->tot_send_num >= CK_I2S_FIFO_MAX_LV) {
        send_num = CK_I2S_FIFO_MAX_LV;
    } else {
        send_num = i2s_priv->tot_send_num;
    }

    for (i = 0; i < send_num; i++) {
        uint32_t timecount = 0;

        while (!(addr->I2S_SR & 0x4)) {
            timecount++;

            if (timecount >= I2S_BUSY_TIMEOUT) {
                if (i2s_priv->cb_event) {
                    i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_TX_UNDERFLOW);
                    return;
                }
            }
        }

        addr->I2S_DR = *i2s_priv->send_buf;
        i2s_priv->send_buf++;
        i2s_priv->send_num--;
    }

    if (i2s_priv->tot_send_num == 0) {

        temp = addr->I2S_IMR;
        temp &= ~I2SFICR_TXEC;
        addr->I2S_IMR = temp;

        i2s_priv->status.tx_busy = 0U;
        i2s_priv->send_num = 0;
        addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;   /* i2s disable */

        if (i2s_priv->cb_event) {
            i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_SEND_COMPLETE);
            return;
        }
    }

    i2s_priv->tot_send_num -= send_num;
}
void ck_i2s_intr_fifo_error(ck_i2s_priv_t *i2s_priv, uint32_t intr)
{
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    uint32_t timecount = 0;

    while (addr->I2S_SR & I2SSR_TX_BUSY) {
        timecount++;

        if (timecount >= I2S_BUSY_TIMEOUT) {
            if (i2s_priv->cb_event) {
                i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_TX_UNDERFLOW);
                return;
            }
        }
    }

    timecount = 0;

    while (addr->I2S_SR & I2SSR_RX_BUSY) {
        addr->I2S_DR;
        timecount++;

        if (timecount >= I2S_BUSY_TIMEOUT) {
            if (i2s_priv->cb_event) {
                i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_FRAME_ERROR);
                return;
            }
        }
    }

    if (intr & I2SFICR_RXIOC) {
        i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_RX_OVERFLOW);
    }

    if (intr & I2SFICR_RXUIC) {
        i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_FRAME_ERROR);
    }

    if (intr & I2SFICR_TXOIC) {
        i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_TX_UNDERFLOW);
    }
}

/**
\brief       handler the interrupt.
\param[in]   spi      Pointer to \ref SPI_RESOURCES
*/
void ck_i2s_irqhandler(int32_t idx)
{
    ck_i2s_priv_t *i2s_priv = &i2s_instance[idx];;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    uint32_t intr = addr->I2S_ISR;
    addr->I2S_FICR = intr;

    /* deal with receive FIFO full interrupt */
    if (intr & I2SFICR_RXFIC) {
        ck_i2s_intr_rx_full(i2s_priv);
    }

    /* deal with transmit FIFO empty interrupt */
    if (intr & I2SFICR_TXEC) {
        ck_i2s_intr_tx_empty(i2s_priv);
    }

    if (intr & (I2SFICR_RXIOC | I2SFICR_RXUIC | I2SFICR_TXOIC)) {
        ck_i2s_intr_fifo_error(i2s_priv, intr);
    }
}

#ifdef CONFIG_LPM
static void manage_clock(i2s_handle_t handle, uint8_t enable)
{
    drv_clock_manager_config(CLOCK_MANAGER_I2S, enable);
}

static void do_prepare_sleep_action(i2s_handle_t handle)
{
    ck_i2s_priv_t *i2s_priv = handle;
    uint32_t *abase = (uint32_t *)(i2s_priv->base);
    registers_save(i2s_priv->i2s_regs_saved, abase, 9);
    registers_save(&i2s_priv->i2s_regs_saved[9], abase + 12, 1);
    registers_save(&i2s_priv->i2s_regs_saved[10], abase + 19, 5);
    registers_save(&i2s_priv->i2s_regs_saved[15], abase + 30, 3);
    registers_save(&i2s_priv->i2s_regs_saved[18], abase + 36, 4);
}

static void do_wakeup_sleep_action(i2s_handle_t handle)
{
    ck_i2s_priv_t *i2s_priv = handle;
    uint32_t *abase = (uint32_t *)(i2s_priv->base);

    registers_restore(abase + 12, &i2s_priv->i2s_regs_saved[9], 1);
    registers_restore(abase + 19, &i2s_priv->i2s_regs_saved[10], 5);
    registers_restore(abase + 30, &i2s_priv->i2s_regs_saved[15], 3);
    registers_restore(abase + 36, &i2s_priv->i2s_regs_saved[18], 4);
    registers_restore(abase + 1, &i2s_priv->i2s_regs_saved[1], 8);
    registers_restore(abase, i2s_priv->i2s_regs_saved, 1);
}
#endif

/**
  \brief       Initialize I2S Interface. 1. Initializes the resources needed for the I2S interface 2.registers event callback function
  \param[in]   idx must not exceed return value of csi_i2s_get_instance_count()
  \return      pointer to i2s instances
*/
i2s_handle_t csi_i2s_initialize(int32_t idx, i2s_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_i2s_init(idx, &base, &irq);

    if (ret < 0 || ret >= CONFIG_I2S_NUM) {
        return NULL;
    }

    ck_i2s_priv_t *i2s_priv = &i2s_instance[idx];
    i2s_priv->base = base;
    i2s_priv->irq  = irq;
    i2s_priv->idx  = idx;

    i2s_priv->cb_event = cb_event;
    i2s_priv->status.tx_busy = 0U;
    i2s_priv->status.rx_busy = 0U;
    i2s_priv->status.tx_underflow = 0U;
    i2s_priv->status.rx_overflow = 0U;
    i2s_priv->status.frame_error = 0U;
    i2s_priv->dma_rx_ch = -1;
    i2s_priv->dma_tx_ch = -1;
#ifdef CONFIG_LPM
    csi_i2s_power_control(i2s_priv, DRV_POWER_FULL);
#endif
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;   /* i2s disable */
    addr->I2S_FSSTA = 0x0;
    addr->I2S_FUNCMODE = 0x2;
    /* enable error interrupte : receive fifo overflow, receive fifo underflow, transmit fifo overflow*/
    addr->I2S_IMR = I2SIMR_DEFAULT_MASK;

    csi_vic_enable_irq(i2s_priv->irq);
    return i2s_priv;
}

/**
  \brief       De-initialize I2S Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle i2s handle to operate.
  \return      error code
*/
int32_t csi_i2s_uninitialize(i2s_handle_t handle)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;   /* i2s disable */
    csi_vic_disable_irq(i2s_priv->irq);
    i2s_priv->status.tx_busy = 0U;
    i2s_priv->status.rx_busy = 0U;
    i2s_priv->status.tx_underflow = 0U;
    i2s_priv->status.rx_overflow = 0U;
    i2s_priv->status.frame_error = 0U;
#ifdef CONFIG_LPM
    csi_i2s_power_control(i2s_priv, DRV_POWER_OFF);
#endif
    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx i2s index
  \return      \ref i2s_capabilities_t
*/
i2s_capabilities_t csi_i2s_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_I2S_NUM) {
        i2s_capabilities_t ret;
        memset(&ret, 0, sizeof(i2s_capabilities_t));
        return ret;
    }

    return i2s_capabilities;
}
/**
  \brief       control I2S power.
  \param[in]   handle  i2s handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_i2s_power_control(i2s_handle_t handle, csi_power_stat_e state)
{
    I2S_NULL_PARAM_CHK(handle);
#ifdef CONFIG_LPM
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_I2S(DRV_ERROR_UNSUPPORTED);
#endif
}
/**
  \brief       config i2s protocol.
  \param[in]   handle       i2s handle to operate.
  \param[in]   protocol     i2s protocol \ref i2s_protocol_e.
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_config_protocol(i2s_handle_t handle, i2s_protocol_e protocol)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    addr->I2S_IISCNF_OUT &= ~0x3;
    addr->I2S_IISCNF_IN  &= ~0x3;

    switch (protocol) {
        case I2S_PROTOCOL_I2S:                  //i2s bus
            addr->I2S_IISCNF_OUT |= I2SCNF_OUT_AUDFMT_I2S;
            addr->I2S_IISCNF_IN  |= I2SCNF_IN_AUDFMT_I2S;
            break;

        case I2S_PROTOCOL_MSB_JUSTIFIED:        // MSB (left) justified
            addr->I2S_IISCNF_OUT |= I2SCNF_OUT_AUDFMT_LEFT_J;
            addr->I2S_IISCNF_IN  |= I2SCNF_IN_AUDFMT_LEFT_J;
            break;

        case I2S_PROTOCOL_LSB_JUSTIFIED:        // LSB (right) justified
            addr->I2S_IISCNF_OUT |= I2SCNF_OUT_AUDFMT_RIGHT_J;
            addr->I2S_IISCNF_IN  |= I2SCNF_IN_AUDFMT_RIGHT_J;
            break;

        case I2S_PROTOCOL_USER:
            break;

        default:
            return ERR_I2S(I2S_ERROR_PROTOCOL);
    }

    i2s_priv->protocol = protocol;
    return 0;
}

/**
  \brief       config i2s sample width.
  \param[in]   handle       i2s handle to operate.
  \param[in]   samplewidth  size of sample in bits
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_config_sample_width(i2s_handle_t handle, uint32_t samplewidth)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    if (samplewidth != 16 && samplewidth != 24) {
        return ERR_I2S(I2S_ERROR_DATA_SIZE);
    }

    addr->I2S_FSSTA &= ~0x6;
    i2s_priv->samplewidth = samplewidth;

    /* set i2s mode by samplewidth */
    if (samplewidth == 16) {
        addr->I2S_FSSTA |= FSSTA_RES16_FIFO16;
    } else {
        addr->I2S_FSSTA |= FSSTA_RES24_FIFO24;
    }

    return 0;
}

/**
  \brief       config i2s work mode.
  \param[in]   mode         work mode of i2s \ref Work mode of i2s.
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_config_work_mode(i2s_handle_t handle, i2s_mode_e mode)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;    /* disable i2s */

    switch (mode) {
        case I2S_MODE_TX_MASTER:        // i2s transmitter master mode
            addr->I2S_IISCNF_OUT &= ~I2SCNF_OUT_MASTER;
            addr->I2S_FUNCMODE = FUNCMODE_MODE_TX | FUNCMODE_MODE_WEN;
            break;

        case I2S_MODE_TX_SLAVE:         // i2s transmitter slave mode
            addr->I2S_IISCNF_OUT |= I2SCNF_OUT_MASTER;
            addr->I2S_FUNCMODE = FUNCMODE_MODE_TX | FUNCMODE_MODE_WEN;
            break;

        case I2S_MODE_RX_MASTER:        // i2s receiver master mode
            addr->I2S_IISCNF_IN |= I2SCNF_IN_MASTER;
            addr->I2S_FUNCMODE = FUNCMODE_MODE_TX;
            break;

        case I2S_MODE_RX_SLAVE:         // i2s receiver slave mode
            addr->I2S_IISCNF_IN &= ~I2SCNF_IN_MASTER;
            addr->I2S_FUNCMODE = FUNCMODE_MODE_TX;
            break;

        default :
            return ERR_I2S(DRV_ERROR_UNSUPPORTED);
    }

    return 0;
}

/**
  \brief       config i2s sample rate.
  \param[in]   handle       i2s handle to operate.
  \param[in]   rate         audio sample rate.
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_config_sample_rate(i2s_handle_t handle, uint32_t rate)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_t *i2s_priv = handle;

    switch (rate) {
        case 8000:
        case 12000:
        case 16000:
        case 24000:
        case 32000:
        case 48000:
        case 96000:
        case 11025:
        case 22050:
        case 44100:
        case 88200:
            break;

        default:
            return ERR_I2S(I2S_ERROR_AUDIO_FREQ);
    }

    int32_t ret = ck_i2s_calc_mclk_div(i2s_priv, rate, i2s_priv->samplewidth);

    if (ret < 0) {
        return ERR_I2S(I2S_ERROR_AUDIO_FREQ);
    }

    ret = ck_i2s_calc_spdifclk_div(i2s_priv, rate, i2s_priv->samplewidth);

    if (ret < 0) {
        return ERR_I2S(I2S_ERROR_AUDIO_FREQ);
    }

    ret = ck_i2s_calc_fs_div(i2s_priv, i2s_priv->samplewidth);

    if (ret < 0) {
        return ERR_I2S(I2S_ERROR_AUDIO_FREQ);
    }

    ret = ck_i2s_calc_refclk_div(i2s_priv, rate);

    if (ret < 0) {
        return ERR_I2S(I2S_ERROR_AUDIO_FREQ);
    }

    return 0;
}
/**
  \brief       config i2s attributes.
  \param[in]   handle       i2s handle to operate.
  \param[in]   fmt          i2s mode \ref i2s_format_e.
  \param[in]   samplewidth  size of sample in bits
  \param[in]   mode         work mode of i2s \ref Work mode of i2s.
  \param[in]   rate         audio sample rate.
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_config(i2s_handle_t handle, i2s_protocol_e protocol, uint32_t samplewidth, i2s_mode_e mode, uint32_t rate)
{
    I2S_NULL_PARAM_CHK(handle);

    int32_t ret = csi_i2s_config_protocol(handle, protocol);

    if (ret < 0) {
        return ret;
    }

    ret = csi_i2s_config_sample_width(handle, samplewidth);

    if (ret < 0) {
        return ret;
    }

    ret = csi_i2s_config_work_mode(handle, mode);

    if (ret < 0) {
        return ret;
    }

    ret = csi_i2s_config_sample_rate(handle, rate);

    if (ret < 0) {
        return ret;
    }

    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;    /* disable i2s */
    /* clear FIFO intr */
    addr->I2S_FICR = 0x1f;

    return 0;
}
/**
  \brief       sending data to i2s transmitter.
  \param[in]   handle       i2s handle to operate.
  \param[in]   data         Pointer to buffer for data to send
  \param[in]   data_size    size of tranmitter data
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_send(i2s_handle_t handle, const void *data, uint32_t data_size)
{
    I2S_NULL_PARAM_CHK(handle);
    I2S_NULL_PARAM_CHK(data);
    ck_i2s_priv_t *i2s_priv = handle;

    if (data_size == 0) {
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    i2s_priv->status.tx_busy = 1U;
    i2s_priv->send_num = 0;
    i2s_priv->tot_send_num = data_size / 4;
    i2s_priv->send_buf = (uint32_t *)data;
    addr->I2S_AUDIOEN &= ~AUDIOEN_IIS_EN;    /* disable i2s */

#if 1 //interrption mode

#if defined CONFIG_DW_DMAC

    /* using DMA if send data greater than CK_I2S_FIFO_MAX_LV*/
    if (data_size >= CK_I2S_FIFO_MAX_LV) {
        if (ck_i2s_dma_send(i2s_priv, data, data_size) == 0) {
            return 0;
        }
    }

#endif
    /* enable tx empty interrupt */
    addr->I2S_TXFTLR = 0x0;
    addr->I2S_AUDIOEN |= AUDIOEN_IIS_EN;    /* enable i2s */

    /*enable i2s */
    addr->I2S_IMR |= I2SIMR_TXEM;
#else // blocking mode
    /* enable tx empty interrupt */
    addr->I2S_AUDIOEN = 0x1;
    s

    while (i2s_priv->tot_send_num--) {
        while (!(addr->I2S_SR & 0x4));

        addr->I2S_DR = *i2s_priv->send_buf;
        i2s_priv->send_buf++;
    }

    while (addr->I2S_SR & 0x2);

    addr->I2S_AUDIOEN = 0x0;
#endif
    return 0;
}

/**
  \brief       abort send data to I2S transmmitter.
  \param[in]   handle       i2s handle to operate.
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_abort_send(i2s_handle_t handle)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    addr->I2S_AUDIOEN = 0x0;
    i2s_priv->status.tx_busy = 0U;
    i2s_priv->send_num = 0;
    i2s_priv->tot_send_num = 0;
    i2s_priv->send_buf = NULL;
    return 0;
}

/**
  \brief       receiving data from i2s receiver.
  \param[in]   handle       i2s handle to operate.
  \param[in]   data         Pointer to buffer for data to receive from i2s receiver
  \param[in]   data_size    size of receiver data
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_receive(i2s_handle_t handle, void *data, uint32_t data_size)
{
    I2S_NULL_PARAM_CHK(handle);
    I2S_NULL_PARAM_CHK(data);

    if (data_size == 0) {
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);
    addr->I2S_AUDIOEN = 0;    /* disable i2s */

    /* clear receive fifo data*/
    uint32_t timecount = 0;

    while (addr->I2S_RXFLR != 0) {
        addr->I2S_DR;
        timecount++;

        if (timecount >= I2S_BUSY_TIMEOUT) {
            return ERR_I2S(DRV_ERROR_TIMEOUT);
        }
    }

    i2s_priv->status.rx_busy = 1U;
    i2s_priv->recv_num = 0;
    i2s_priv->tot_recv_num = data_size / 4;
    i2s_priv->recv_buf = (uint32_t *)data;
#if 1       //interruption mode
    /* using DMA if receive data greater than CK_I2S_FIFO_MAX_LV*/
#if defined CONFIG_DW_DMAC

    if (data_size >= CK_I2S_FIFO_MAX_LV) {
        if (ck_i2s_dma_recv(i2s_priv, data, data_size) == 0) {
            return 0;
        }
    }

#endif
    addr->I2S_FICR = 0x1f;
    addr->I2S_IMR |= I2SIMR_RXFIM;

    if (data_size > CK_I2S_FIFO_MAX_LV) {
        addr->I2S_RXFTLR = CK_I2S_FIFO_MAX_LV / 2 - 1;
    } else {
        addr->I2S_RXFTLR = data_size - 1;
    }

    addr->I2S_AUDIOEN = 0x1;
#else       // blocking mode
    addr->I2S_AUDIOEN = 0x1;
    data_size = data_size / 4;

    while (data_size--) {
        while (!(addr->I2S_SR & 0x10));

        *(i2s_priv->recv_buf) = addr->I2S_DR;
        i2s_priv->recv_buf++;
    }

    addr->I2S_AUDIOEN = 0x0;

    while (1);

    if (i2s_priv->cb_event) {
        i2s_priv->cb_event(i2s_priv->idx, I2S_EVENT_RECEIVE_COMPLETE);
    }

#endif
    /*enable i2s */
    return 0;
}

/**
  \brief       abort receive data from I2S receiver.
  \param[in]   handle       i2s handle to operate.
  \return      0 for success, negative for error code
*/
int32_t csi_i2s_abort_receive(i2s_handle_t handle)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_t *i2s_priv = handle;
    ck_i2s_reg_t *addr = (ck_i2s_reg_t *)(i2s_priv->base);

    addr->I2S_AUDIOEN = 0x0;
    i2s_priv->status.rx_busy = 0U;
    i2s_priv->recv_num = 0;
    i2s_priv->tot_recv_num = 0;
    i2s_priv->recv_buf = NULL;
    return 0;

}

/**
  \brief       Get i2s status.
  \param[in]   handle i2s handle to operate.
  \return      i2s status \ref i2s_status_e
*/
i2s_status_t csi_i2s_get_status(i2s_handle_t handle)
{
    if (handle == NULL) {
        i2s_status_t status = {0};
        return status;
    }

    ck_i2s_priv_t *i2s_priv = handle;
    return i2s_priv->status;
}
