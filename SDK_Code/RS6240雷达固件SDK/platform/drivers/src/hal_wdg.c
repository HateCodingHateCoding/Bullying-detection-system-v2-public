/**
 ******************************************************************************
 * @file    hal_wdg.c
 * @brief   hal wath dog define.
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
#include "common.h"
#include "hal_board.h"
#include "hal_wdg.h"
#include "ll_wdg.h"
#include "ll_utils.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_pmu.h"
#include "irq.h"


#ifndef CONFIG_WDG_LOG_LEVEL
#define CONFIG_WDG_LOG_LEVEL            LEVEL_ERR
#endif

#define LOG_MODULE                      "WDG"
#define LOG_LEVEL                       CONFIG_WDG_LOG_LEVEL
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
typedef struct {
    HAL_Dev_t device;
    uint8_t status;
    uint8_t sysId;
    WDG_InitParam_t cfgParam;
} Wdg_Device_t;

typedef enum {
    WDG_CLK_SRC_APB1 = 0,
    WDG_CLK_SRC_MSI,
    WDG_CLK_SRC_DCXO,
    WDG_CLK_SRC_LSI,
    WDG_CLK_SRC_MAX = WDG_CLK_SRC_LSI
} WDG_Clk_Src_t;

typedef enum {
    WDG_CLK_DIV_1 = 0,
    WDG_CLK_DIV_2,
    WDG_CLK_DIV_4,
    WDG_CLK_DIV_8,
    WDG_CLK_DIV_16,
    WDG_CLK_DIV_32,
    WDG_CLK_DIV_64,
    WDG_CLK_DIV_128,
    WDG_CLK_DIV_MAX =  WDG_CLK_DIV_128
} WDG_Clk_Div_t;

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_WDG_PULSE_RELOAD_CNT
#define CONFIG_WDG_PULSE_RELOAD_CNT                         0
#endif

#ifndef CONFIG_WDG_IRQ_PRIO
#define CONFIG_WDG_IRQ_PRIO                                 3
#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */


/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

static void WDG_IRQHandler(void * param);
static HAL_Status_t wdg_HwInit(Wdg_Device_t *pWdgDevice);
#if CONFIG_PM
static int wdg_pm_action(void *dev, enum pm_device_action action);
#endif

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
HAL_Dev_t * HAL_WDG_Init(WDG_ID_t id)
{
    HAL_Dev_t *pDev;
    Wdg_Device_t *pWdgDevice = (Wdg_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_WDG, id);

    if (pWdgDevice == NULL) {
        pWdgDevice = (Wdg_Device_t *)HAL_DEV_MemMalloc(sizeof(Wdg_Device_t));
        if (pWdgDevice == NULL) {
            return NULL;
        }

        pDev = (HAL_Dev_t *)pWdgDevice;
        pWdgDevice->sysId                       = csi_get_cpu_id();
        if (pWdgDevice->sysId == 0) {
#if (CONFIG_SOC_WDG0 == 1)
            pWdgDevice->device.reg              = WDG0_DEV;
            pDev->irqNum                        = WDG0_IRQn;
            pDev->irqPrio                       = CONFIG_WDG_IRQ_PRIO;
            pDev->irqHandler                    = WDG_IRQHandler;
#else
            LOG_WRN("wdg 0 not support\n");
            HAL_DEV_MemFree(pDev);
            return NULL;
#endif
        } else {
#if (CONFIG_SOC_WDG1 == 1)
            pWdgDevice->device.reg              = WDG1_DEV;
            pDev->irqNum                        = WDG1_IRQn;
            pDev->irqPrio                       = CONFIG_WDG_IRQ_PRIO;
            pDev->irqHandler                    = WDG_IRQHandler;
#else
            LOG_WRN("wdg 1 not support\n");
            HAL_DEV_MemFree(pDev);
            return NULL;
#endif
        }
#if CONFIG_PM
            pm_device_register(&pDev->pmDev, wdg_pm_action);
#endif

        HAL_DEV_Register((HAL_Dev_t *)pWdgDevice, HAL_DEV_TYPE_WDG, 0);
    }

    return (HAL_Dev_t *)pWdgDevice;
}

HAL_Status_t HAL_WDG_DeInit(HAL_Dev_t *wdg)
{
    HAL_Status_t status = HAL_STATUS_OK;
    Wdg_Device_t *pWdgDevice = (Wdg_Device_t *)wdg;

    if (pWdgDevice == NULL) {
        LOG_WRN("wdg not initialized\n");
        return HAL_STATUS_INVALID_STATE;
    }

#if CONFIG_PM
    pm_device_unregister(&wdg->pmDev);
#endif

    status = HAL_DEV_Unregister(wdg);
    if (status == HAL_STATUS_OK) {
        HAL_DEV_MemFree(wdg);
    }

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_WDG_Open(HAL_Dev_t *wdg, WDG_InitParam_t * pInitParam)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if ((wdg == NULL) || (wdg->reg == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(wdg);
    if ((state == HAL_DEV_STATE_INIT) || (state == HAL_DEV_STATE_CLOSE)) {
        Wdg_Device_t *pWdgDevice = (Wdg_Device_t *)wdg;
        if (pInitParam != NULL) {
            memcpy(&(pWdgDevice->cfgParam), pInitParam, sizeof(WDG_InitParam_t));
        }
        status = wdg_HwInit(pWdgDevice);
        if (status == HAL_STATUS_OK) {
            IRQ_AttachDevice(pWdgDevice->device.irqNum, (HAL_Dev_t *)pWdgDevice);
            IRQ_Priority(pWdgDevice->device.irqNum, pWdgDevice->device.irqPrio);
            IRQ_Enable(pWdgDevice->device.irqNum);

            LL_WDT_Enable(wdg->reg);

            HAL_DEV_SetState(wdg, HAL_DEV_STATE_OPEN);
        } else {
            status = HAL_STATUS_ERROR;
        }
    } else {
        status = HAL_STATUS_ALREADY;
    }

    return status;
}

HAL_Status_t HAL_WDG_Close(HAL_Dev_t *wdg)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if ((wdg == NULL) || (wdg->reg == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(wdg);
    if (state >= HAL_DEV_STATE_OPEN) {
        Wdg_Device_t *pWdgDevice = (Wdg_Device_t *)wdg;

        if (pWdgDevice->sysId == 0) {
#if (CONFIG_SOC_WDG0 == 1)
            HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, WDG0_RESETN_Msk);
            LL_RCC_WDG0_Reset();
            LL_RCC_WDG0_DisableClock();
#else
            return HAL_STATUS_NOT_SUPPORTED;
#endif
        } else {
#if (CONFIG_SOC_WDG1 == 1)
            HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, WDG1_RESETN_Msk);
            LL_RCC_WDG1_Reset();
            LL_RCC_WDG1_DisableClock();
#else
            return HAL_STATUS_NOT_SUPPORTED;
#endif
        }

        IRQ_Disable(pWdgDevice->device.irqNum);
        IRQ_Detach(pWdgDevice->device.irqNum);

        HAL_DEV_SetState(wdg, HAL_DEV_STATE_CLOSE);
    } else {
        status = HAL_STATUS_ALREADY;
    }

    return status;
}

HAL_Status_t HAL_WDG_Feed(HAL_Dev_t *wdg)
{
    if ((wdg == NULL) || (wdg->reg == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (HAL_DEV_GetState(wdg) == HAL_DEV_STATE_OPEN) {
        LL_WDT_Restart(wdg->reg);
    }

    return HAL_STATUS_OK;
}

static void WDG_IRQHandler(void * param)
{
    if (param == NULL) {
        return;
    }

    Wdg_Device_t *pWdgDevice = (Wdg_Device_t *)param;
    LL_WDT_ClrStatus(((HAL_Dev_t *)param)->reg);

    if (pWdgDevice->cfgParam.callback != NULL) {
        pWdgDevice->cfgParam.callback((HAL_Dev_t *)pWdgDevice, &pWdgDevice->cfgParam);
    }
}

static uint32_t wdg_timeout_convert(uint32_t timeout_ms)
{
    uint32_t reload;
    int32_t freq = HAL_BOARD_GetFreq(CLOCK_LSI) / 1;

    reload = (freq / 1000) * timeout_ms;
    if (freq % 1000)
        reload += 1;

    return reload;
}

static HAL_Status_t wdg_HwInit(Wdg_Device_t *pWdgDevice)
{
    if ((pWdgDevice == NULL) || (pWdgDevice->sysId > 1)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (((pWdgDevice->cfgParam.mode == WDG_MODE_INTERRUPT)
            && (pWdgDevice->cfgParam.callback == NULL))) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (pWdgDevice->sysId == 0) {
        pWdgDevice->device.reg = WDG0_DEV;
        LL_RCC_WDG0_DisableClock();
        LL_RCC_WDG0_SetClockSource(WDG_CLK_SRC_APB1);
        LL_RCC_WDG0_Reset();
        LL_RCC_WDG0_EnableBusClock();
        LL_RCC_WDG0_EnableClock();
        LL_WDG0_SetResetPatition(pWdgDevice->cfgParam.mode);

        LL_WDT_SetTimeoutPeriod(WDG0_DEV, wdg_timeout_convert(pWdgDevice->cfgParam.timeout));
        if (pWdgDevice->cfgParam.mode == WDG_MODE_INTERRUPT) {
            LL_WDT_SetRespMode(WDG0_DEV, 1);
        } else {
            /* reset system */
            LL_WDT_SetRespMode(WDG0_DEV, 0);
        }
        LL_WDT_SetRstPulseLen(WDG0_DEV, CONFIG_WDG_PULSE_RELOAD_CNT);
        LL_WDT_Restart(WDG0_DEV);

        LL_RCC_WDG0_DisableClock();
        LL_RCC_WDG0_SetClockSource(WDG_CLK_SRC_LSI);
        LL_RCC_WDG0_SetPrescaler(WDG_CLK_DIV_1);
        LL_RCC_WDG0_EnableClock();
    } else {
        pWdgDevice->device.reg = WDG1_DEV;
        LL_RCC_WDG1_DisableClock();
        LL_RCC_WDG1_SetClockSource(WDG_CLK_SRC_APB1);
        LL_RCC_WDG1_Reset();
        LL_RCC_WDG1_EnableBusClock();
        LL_RCC_WDG1_EnableClock();
        LL_WDG1_SetResetPatition(pWdgDevice->cfgParam.mode);

        LL_WDT_SetTimeoutPeriod(WDG1_DEV, wdg_timeout_convert(pWdgDevice->cfgParam.timeout));
        if (pWdgDevice->cfgParam.mode == WDG_MODE_INTERRUPT) {
            LL_WDT_SetRespMode(WDG1_DEV, 1);
        } else {
            /* reset system */
            LL_WDT_SetRespMode(WDG1_DEV, 0);
        }
        LL_WDT_SetRstPulseLen(WDG1_DEV, CONFIG_WDG_PULSE_RELOAD_CNT);
        LL_WDT_Restart(WDG1_DEV);

        LL_RCC_WDG1_DisableClock();
        LL_RCC_WDG1_SetClockSource(WDG_CLK_SRC_LSI);
        LL_RCC_WDG1_SetPrescaler(WDG_CLK_DIV_1);
        LL_RCC_WDG1_EnableClock();
    }

    return HAL_STATUS_OK;
}

#if CONFIG_PM
static int wdg_pm_action(void *dev, enum pm_device_action action)
{
    HAL_Status_t status = HAL_STATUS_OK;
    int ret = -1;

    switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
        status = HAL_WDG_Close((HAL_Dev_t *)dev);
        if ((status == HAL_STATUS_OK) || (status == HAL_STATUS_ALREADY)) {
            ret = 0;
        }
        break;
    case PM_DEVICE_ACTION_RESUME:
        status = HAL_WDG_Open((HAL_Dev_t *)dev, NULL);
        if ((status == HAL_STATUS_OK) || (status == HAL_STATUS_ALREADY)) {
            ret = 0;
        }
        break;
    default:
        break;
    }
    return (ret == HAL_STATUS_OK) ? 0 : -1;
}
#endif


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

