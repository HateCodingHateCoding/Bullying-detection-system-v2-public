/**
 ******************************************************************************
 * @file    pmu_tmr.c
 * @brief   adapter freertos define.
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
#include <stdio.h>
#include "hal_types.h"

#include "ll_pmu.h"
#include "ll_utils.h"
#include "ll_rtctick.h"
#include "ll_rcc_dev.h"

#include "irq.h"
#include "pmu_tmr.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */


/* Private defines.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_TICK_LPTIME_MIN_VALUE
#define CONFIG_TICK_LPTIME_MIN_VALUE                        3
#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
static uint8_t pmu_sys_id;
static pmu_tmr_cb_t pmu_tmr_cb = NULL;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */


/* Exported functions.
 * ----------------------------------------------------------------------------
 */

static inline void tmr_clear_irq(void)
{
    if (pmu_sys_id) {
	    LL_RTCTICK_CMP1_ClearFlag();
        while ((LL_RTCTICK_CMP1_GetFlag())) {
        }
    } else {
	    LL_RTCTICK_CMP0_ClearFlag();
        while ((LL_RTCTICK_CMP0_GetFlag())) {
        }
    }
}

static void tmr_irq_handler(void * param)
{
    tmr_clear_irq();

    if (pmu_tmr_cb)
        pmu_tmr_cb(param);
}

int pmu_tmr_init(void)
{
    HAL_Status_t status = HAL_STATUS_OK;

    pmu_sys_id = LL_SYS_GetCurrentSysId();

    if (LL_RTCTICK_IsEnabled() == 0) {
        LL_RCC_RTICK_SetClockSource(LL_RCC_RTICK_LPCLK_FDIV);
        LL_RCC_RTICK_SetPrescaler(LL_RCC_RTICK_DIV_1);
        LL_RCC_RTICK_EnableBusClock();
        LL_RCC_RTICK_EnableClock();
        LL_RCC_RTICK_Reset();
        LL_RTCTICK_Enable();

        if (pmu_sys_id) {
            LL_RTCTICK_CMP1_Disable();
            LL_RTCTICK_CMP1_ClearFlag();

            LL_WKUP_EnableOnSys1(LL_WKUP_SRC_RTICK1);
            LL_WKUP_DisableOnSys0(LL_WKUP_SRC_RTICK1);

            LL_RTCTICK_CMP1_Enable();

            status = IRQ_Attach(TMR_WUP1_IRQn, tmr_irq_handler);
            if (status != HAL_STATUS_OK) {
                return status;
            }
            IRQ_Priority(TMR_WUP1_IRQn, 20U);
            IRQ_Enable(TMR_WUP1_IRQn);
        } else {
            LL_RTCTICK_CMP0_Disable();
            LL_RTCTICK_CMP0_ClearFlag();

            LL_WKUP_EnableOnSys0(LL_WKUP_SRC_RTICK0);
            LL_WKUP_DisableOnSys1(LL_WKUP_SRC_RTICK0);

            LL_RTCTICK_CMP0_Enable();

            status = IRQ_Attach(TMR_WUP0_IRQn, tmr_irq_handler);
            if (status != HAL_STATUS_OK) {
                return status;
            }

            IRQ_Priority(TMR_WUP0_IRQn, 20U);
            IRQ_Enable(TMR_WUP0_IRQn);
        }
    }
    return status;
}

void pmu_tmr_callback_set(pmu_tmr_cb_t cb)
{
    pmu_tmr_cb = cb;
}

void pmu_tmr_time_set(uint32_t deltaTime)
{
    uint32_t lptimeL, lptimeH, cmpTime;

    if (deltaTime < CONFIG_TICK_LPTIME_MIN_VALUE) {
        deltaTime = CONFIG_TICK_LPTIME_MIN_VALUE;
    }

    lptimeL = LL_RTCTICK_GetCurrentValueL();
    lptimeH = LL_RTCTICK_GetCurrentValueH();

    cmpTime = lptimeL + deltaTime;
    if ((cmpTime < lptimeL) || (cmpTime < deltaTime)) {
        lptimeH += 1;
    }

    if (pmu_sys_id) {
        LL_RTCTICK_CMP1_SetValueL(cmpTime);
        LL_RTCTICK_CMP1_SetValueH(lptimeH);
        LL_RTCTICK_CMP1_Enable();
    } else {
        LL_RTCTICK_CMP0_SetValueL(cmpTime);
        LL_RTCTICK_CMP0_SetValueH(lptimeH);
        LL_RTCTICK_CMP0_Enable();
    }
}

void pmu_tmr_time_64_set(uint64_t deltaTime)
{
    uint32_t lptimeL, lptimeH, cmpTime, deltaTimeL, deltaTimeH;

    if (deltaTime < CONFIG_TICK_LPTIME_MIN_VALUE) {
        deltaTime = CONFIG_TICK_LPTIME_MIN_VALUE;
    }

    deltaTimeL = deltaTime & 0xFFFFFFFF;
    deltaTimeH = (deltaTime >> 32) & 0xFFFFFFFF;

    lptimeL = LL_RTCTICK_GetCurrentValueL();
    lptimeH = LL_RTCTICK_GetCurrentValueH();

    lptimeH += deltaTimeH;
    cmpTime = lptimeL + deltaTimeL;
    if ((cmpTime < lptimeL) || (cmpTime < deltaTimeL)) {
        lptimeH += 1;
    }

    if (pmu_sys_id) {
        LL_RTCTICK_CMP1_SetValueL(cmpTime);
        LL_RTCTICK_CMP1_SetValueH(lptimeH);
        LL_RTCTICK_CMP1_Enable();
    } else {
        LL_RTCTICK_CMP0_SetValueL(cmpTime);
        LL_RTCTICK_CMP0_SetValueH(lptimeH);
        LL_RTCTICK_CMP0_Enable();
    }
}

uint64_t pmu_tmr_time_get(void)
{
    return LL_RTCTICK_GetCurrentValue();
}

uint32_t pmu_tmr_trig(void)
{
    if (pmu_sys_id) {
        return LL_RTCTICK_CMP1_GetFlag();
    } else {
        return LL_RTCTICK_CMP0_GetFlag();
    }
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
