/**
 ******************************************************************************
 * @file    hif_io.c
 * @brief   hif_io define.
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
#include "hif.h"
#include "hif_config.h"
#include "hif_io.h"
#include "hal_dev.h"
#include "hal_wkio.h"
#include "hal_gpio.h"

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

/* Private variables.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t *wkioHoldDev;

static hif_io_pm_cfg_t hif_io_pm_cfg;
static pm_wakeup_callback_t hif_io_pm_wakeup_callback = NULL;
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static void hif_wkio_isr(void *arg);
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int hif_wkio_hold_init(uint32_t irq_pins)
{
    HAL_Status_t status = HAL_STATUS_OK;

    uint8_t port = irq_pins / 16;
    uint8_t pin_idx = irq_pins % 16;

    wkioHoldDev = HAL_WKIO_Init(port);
    if (wkioHoldDev == NULL) {
        printf("Init wkio fail\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    status = HAL_WKIO_SetPinMode(wkioHoldDev, pin_idx, HAL_WKIO_MODE_HOLD);
    if (status != HAL_STATUS_OK) {
        printf("Set wkio mode fail %d\n", status);
        return status;
    }
    return status;
}

__hif_sram_text void hif_io_out_set(uint8_t io_num, uint8_t io_val)
{
    HAL_Status_t status = HAL_STATUS_OK;

    uint8_t pin_idx = io_num % 16;

    if (wkioHoldDev != NULL) {
        uint8_t mode = (io_val) ? WKIO_PIN_SET : WKIO_PIN_RESET;

        status = HAL_WKIO_WritePin(wkioHoldDev, pin_idx, mode);
        if (status != HAL_STATUS_OK) {
            printf("Set wkio hold output fail %d\n", status);
        }
    }
}

int hif_io_in_value_get(void)
{
    HAL_Dev_t *gpioDev;
    HAL_Status_t status = HAL_STATUS_OK;
    GPIO_PinParam_t gpioParam;

    uint8_t port = hif_io_pm_cfg.io_num / 16;
    uint8_t pin_idx = hif_io_pm_cfg.io_num % 16;

    gpioDev = HAL_GPIO_Init(port);
    if (gpioDev == NULL) {
        printf("Init gpio fail\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    gpioParam.mode = GPIOx_Pn_F0_INPUT;
    gpioParam.driving = GPIO_DRIVING_LEVEL_1;
    gpioParam.pull = hif_io_pm_cfg.pull_mode;

    status = HAL_GPIO_SetPinParam(gpioDev, pin_idx, &gpioParam);
    if (status != HAL_STATUS_OK) {
        printf("Set config gpio fail %d\n", status);
        return status;
    }

    return HAL_GPIO_ReadPin(gpioDev, pin_idx);
}


int hif_io_in_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg)
{
    hif_io_pm_cfg_t *pio_cfg = (hif_io_pm_cfg_t *)pcfg;
    HAL_Dev_t *wkioDev;
    HAL_Status_t status = HAL_STATUS_OK;
    WKIO_WakeParam_t wkioIrqParam;

    uint8_t port = pio_cfg->io_num / 16;
    uint8_t pin_idx = pio_cfg->io_num % 16;

    wkioDev = HAL_WKIO_Init(port);
    if (wkioDev == NULL) {
        printf("Init wkio fail\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    status = HAL_WKIO_SetPinMode(wkioDev, pin_idx, HAL_WKIO_MODE_WAKE);
    if (status != HAL_STATUS_OK) {
        printf("Set wkio mode fail %d\n", status);
        return status;
    }

    hif_io_pm_cfg.io_num = pio_cfg->io_num;
    hif_io_pm_cfg.pull_mode = pio_cfg->pull_mode;
    hif_io_pm_cfg.trig_type = pio_cfg->trig_type;

    wkioIrqParam.event = pio_cfg->trig_type;
    wkioIrqParam.pull = pio_cfg->pull_mode;
    wkioIrqParam.callback = hif_wkio_isr;
    wkioIrqParam.arg = pcfg;

    status = HAL_WKIO_SetWakeParam(wkioDev, pin_idx, &wkioIrqParam);
    if (status != HAL_STATUS_OK) {
        printf("Set wkio wake init fail %d\n", status);
        return status;
    }

    hif_io_pm_wakeup_callback = pm_cb;

    return HIF_ERRCODE_SUCCESS;
}


int hif_io_in_pm_deinit(void)
{
    return HIF_ERRCODE_SUCCESS;
}


static void hif_wkio_isr(void *arg)
{
    if (hif_io_pm_wakeup_callback != NULL) {
        hif_io_pm_wakeup_callback(0);
    }
}

#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
