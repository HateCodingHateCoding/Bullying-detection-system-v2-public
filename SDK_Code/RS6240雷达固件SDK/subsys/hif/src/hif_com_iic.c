/**
 ******************************************************************************
 * @file    template.c
 * @brief   template define.
 * @verbatim    null
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "hif_types.h"
#include "hif_config.h"
#include "hif_log.h"
#include "hif.h"
#include "hif_log.h"
#include "hal_i2c.h"
#include "hif_com.h"

#if (CONFIG_HIF == 1)

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */



#if (CONFIG_HIF_DEVICE_COM_IIC)

#if (CONFIG_HIF_DEVICE_WAKEUP_IIC)
static pm_wakeup_callback_t hif_iic_pm_wakeup_callback = NULL;
#endif


static HAL_Dev_t *hif_iic_instance = NULL;

I2C_InitParam_t hif_iic_cfg = {
    .mode      =  I2C_MODE_SLAVE,
    .speed     =  I2C_SPEED_FAST,
    .busErrCb  =  {
        .arg   =  NULL,
        .cb    =  NULL,
    },
};

static void hif_com_iic_isr_handler(HAL_Dev_t * pDevice, void *arg);

static uint8_t hif_com_iic_wakeup_state;
static trf_state_t hif_com_iic_trf_state;

static uint8_t hif_com_iic_param = 0x3A;
/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int hif_com_iic_init(uint32_t rate)
{
    HAL_Callback_t temp_cb;
    uint8_t level = 0;

    if(rate == 100000 || rate == 100) {
        hif_iic_cfg.speed = I2C_SPEED_STANDARD;
    } else if(rate == 400000 || rate == 400) {
        hif_iic_cfg.speed = I2C_SPEED_FAST;
    } else {
        hif_iic_cfg.speed = I2C_SPEED_FAST_PLUS;
    }

    com_port_set(com_i2c);

    hif_iic_instance = HAL_I2C_Init(I2C0_ID, &hif_iic_cfg);
    if(hif_iic_instance != NULL) {
        if(HAL_I2C_Open(hif_iic_instance, I2C_ADDR_WIDTH_7BIT, hif_com_iic_param) == 0) {
            temp_cb.cb = hif_com_iic_isr_handler;
            temp_cb.arg = NULL;
            HAL_I2C_RegisterIRQ(hif_iic_instance, &temp_cb);

            level = 3;
            HAL_I2C_ExtendControl(hif_iic_instance, I2C_EXTATTR_SET_TXFIFO_LEVEL, &level);
            level = 3;
            HAL_I2C_ExtendControl(hif_iic_instance, I2C_EXTATTR_SET_RXFIFO_LEVEL, &level);

            HAL_I2C_DisableIRQ(hif_iic_instance, I2C_IRQSTATUS_START | I2C_IRQSTATUS_STOP
                                            | I2C_IRQSTATUS_ERROR | I2C_IRQSTATUS_TX_FIFO_EMPTY
                                            | I2C_IRQSTATUS_RX_FIFO_FULL);
            HAL_I2C_EnableIRQ(hif_iic_instance, I2C_IRQSTATUS_RX_FIFO_FULL | I2C_IRQSTATUS_READ_REQ | I2C_IRQSTATUS_STOP | I2C_IRQSTATUS_ERROR);

            HAL_I2C_Enable(hif_iic_instance);

            hif_com_iic_wakeup_state  = 0;
            hif_com_iic_trf_state = TRF_STATE_IDLE;

            return HIF_ERRCODE_SUCCESS;
        }
    }
    return HIF_ERRCODE_NOT_READY;
}

int hif_com_iic_deinit(void)
{
    if(HAL_I2C_Close(hif_iic_instance) == 0) {
        if(HAL_I2C_DeInit(hif_iic_instance) == 0) {
            HAL_I2C_DisableIRQ(hif_iic_instance, I2C_IRQSTATUS_START | I2C_IRQSTATUS_STOP
                                            | I2C_IRQSTATUS_ERROR | I2C_IRQSTATUS_TX_FIFO_EMPTY
                                            | I2C_IRQSTATUS_RX_FIFO_FULL);
            return HIF_ERRCODE_SUCCESS;
        }
    }
    return HIF_ERRCODE_NOT_READY;
}

__hif_sram_text int hif_com_iic_write(uint8_t *pbuff, uint16_t size)
{
    uint32_t fifo_size = 0;

    for (fifo_size = 0; fifo_size < size; fifo_size++) {
        if (HAL_I2C_IsTxReady(hif_iic_instance) == 0) {
            break;
        }

        HAL_I2C_PutTxData(hif_iic_instance, pbuff[fifo_size], I2C_CMD_WRITE);
    }

    return fifo_size;
}

__hif_sram_text int hif_com_iic_read(uint8_t *pbuff, uint16_t size)
{
    uint32_t fifo_size = 0;

    for (fifo_size = 0; fifo_size < size; fifo_size++) {
        if (HAL_I2C_IsRxReady(hif_iic_instance) == 0) {
            break;
        }

        pbuff[fifo_size] = HAL_I2C_GetRxData(hif_iic_instance);
    }

    return fifo_size;
}

__hif_sram_text int hif_com_iic_tx_start(void)
{
    HAL_I2C_RecoverBus(hif_iic_instance);

    return HIF_ERRCODE_SUCCESS;
}


__hif_sram_text int hif_com_iic_write_end(void)
{
    if (hif_com_iic_trf_state == TRF_STATE_TX_BUSY) {
        hif_com_iic_trf_state = TRF_STATE_TX_DONE;
    }
    return 0;
}

__hif_sram_text int hif_com_iic_read_end(void)
{
    hif_com_iic_wakeup_state = 0;
    hif_com_data_recv_done();

    return 0;
}

__hif_sram_text void hif_com_iic_wakeup(void)
{
    hif_com_iic_wakeup_state = 1;
}

__hif_sram_text static void hif_com_iic_isr_handler(HAL_Dev_t * pDevice, void *arg)
{
    int status = 0;
    uint32_t iic_status = HAL_I2C_GetIRQStatus(hif_iic_instance);

    if(iic_status & I2C_IRQSTATUS_READ_REQ) {
        if (hif_com_iic_wakeup_state) {
            uint8_t tx_data[2] = {HIF_PHY_ACK_MAGIC, HIF_PHY_ACK_MAGIC};
            hif_com_iic_write(tx_data, sizeof(tx_data));
            hif_com_iic_wakeup_state = 0;
        } else {
            status = hif_com_data_send(1);
            if (status == HIF_COM_TX_SUCCESS) {
                hif_com_iic_trf_state = TRF_STATE_TX_WAIT;
            } else if (status == HIF_COM_TX_CONTINUE) {
                hif_com_iic_trf_state = TRF_STATE_TX_BUSY;
                HAL_I2C_EnableIRQ(hif_iic_instance, I2C_IRQSTATUS_TX_FIFO_EMPTY);
            } else if (status == HIF_COM_TX_DONE) {
                hif_com_iic_trf_state = TRF_STATE_TX_DONE;
            }
        }
    } else if(iic_status & I2C_IRQSTATUS_RX_FIFO_FULL) {
        hif_com_data_recv();
        hif_com_iic_trf_state = TRF_STATE_RX_BUSY;
    } else if(iic_status & I2C_IRQSTATUS_TX_FIFO_EMPTY) {
        status = hif_com_data_send(1);
        if(status == HIF_COM_TX_DONE || status == HIF_COM_TX_NULL) {
            hif_com_iic_trf_state = TRF_STATE_TX_DONE;
        }
    } else {

    }

    if(iic_status & I2C_IRQSTATUS_STOP) {
        if(hif_com_iic_trf_state == TRF_STATE_RX_BUSY) {
            hif_com_data_recv();
        } else if(hif_com_iic_trf_state == TRF_STATE_TX_DONE) {
            HAL_I2C_DisableIRQ(hif_iic_instance, I2C_IRQSTATUS_TX_FIFO_EMPTY);
            hif_com_data_output_done();
            hif_com_data_send_done();
            hif_com_iic_trf_state = TRF_STATE_IDLE;
        }
    }
    HAL_I2C_ClearIRQ(hif_iic_instance, I2C_IRQSTATUS_ALL);
}

void hif_com_iic_param_set(uint8_t param)
{
    hif_com_iic_param = param;
}

uint8_t hif_com_iic_param_get(void)
{
    return hif_com_iic_param;
}

#if (CONFIG_HIF_DEVICE_WAKEUP_IIC)

static void hif_iic_callback(HAL_Dev_t * pDevice, void *arg)
{
    if (hif_iic_pm_wakeup_callback != NULL) {
        hif_iic_pm_wakeup_callback(0);
    }
}

int hif_com_iic_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg)
{
    uint8_t enable = 1;
    HAL_Callback_t temp_cb;

    if ((pm_cb == NULL) || (pcfg == NULL)) {
        return HIF_ERRCODE_INVALID_PARAM;
    }
    if(hif_iic_instance) {
        temp_cb.cb = hif_iic_callback;
        temp_cb.arg = NULL;
        HAL_I2C_ExtendControl(hif_iic_instance, I2C_EXTATTR_WAKEUP_THRESHOLD, pcfg);
        HAL_I2C_ExtendControl(hif_iic_instance, I2C_EXTATTR_WAKEUP_CALLBACK, &temp_cb);
        HAL_I2C_ExtendControl(hif_iic_instance, I2C_EXTATTR_WAKEUP_CTRL_ABLE, &enable);
        hif_iic_pm_wakeup_callback = pm_cb;
    } else {
        return HIF_ERRCODE_NOT_READY;
    }

    return HIF_ERRCODE_SUCCESS;
}

int hif_com_iic_pm_deinit(void)
{
    uint8_t enable = 0;

    if(hif_iic_instance) {
        HAL_I2C_ExtendControl(hif_iic_instance, I2C_EXTATTR_WAKEUP_CALLBACK, NULL);
        HAL_I2C_ExtendControl(hif_iic_instance, I2C_EXTATTR_WAKEUP_CTRL_ABLE, &enable);
    } else {
        return HIF_ERRCODE_NOT_READY;
    }

    return HIF_ERRCODE_SUCCESS;
}

#endif /* CONFIG_HIF_DEVICE_WAKEUP_IIC */

#endif /* CONFIG_HIF_DEVICE_COM_IIC */
#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
