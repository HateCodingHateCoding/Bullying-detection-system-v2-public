/**
 ******************************************************************************
 * @file    osi_port.c
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

#include "kernel_config.h"


#if CONFIG_KERNEL_FREERTOS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>

#include "hal_types.h"
#include "hal_dev.h"
#include "hal_clock.h"
#include "hal_board.h"
#include "irq.h"

#include "ll_pmu.h"
#include "ll_utils.h"
#include "ll_rtctick.h"
#include "ll_rcc_dev.h"
#include "ll_splk.h"

#include "osi_port.h"

#include "pmu_tmr.h"

#define LOG_MODULE                      "osi"
#define LOG_LEVEL                       LEVEL_DBG
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

typedef void* os_task_t;

typedef struct {
    /* GPR, FPR, CSR */
    uint32_t CPU[80];

    uint8_t CLICCFG;
    uint8_t CTL[CONFIG_CPU_IRQ_MAX];
    uint8_t ATTR[CONFIG_CPU_IRQ_MAX];
    uint8_t IE[CONFIG_CPU_IRQ_MAX];
    uint32_t MINTTHRESH;
} sys_PmContex_t;

static sys_PmContex_t sys_PmContex;

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define PMU_TMR_CYC_MAX    (0xFFFFFFFFFFFF)

/* Private variables.
 * ----------------------------------------------------------------------------
 */
uint32_t os_system_freq;

static uint8_t os_sys_id;
static uint32_t os_tick_delta_value;
static uint32_t tick_freq;

PMU_HandleCallback_t pmuSuspend;
PMU_HandleCallback_t pmuResume;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
__attribute__((weak)) int g_fr_next_sleep_ticks = -1;

static void os_PM_SysHandle(uint32_t pmState);

extern void sys_sleep(uint32_t sleepMode, uint32_t *pSaveAddr);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */


int OSI_IntrptEnter(void)
{
    return 0;
}

void OSI_IntrptExit(void * param)
{
    portYIELD_FROM_ISR(pdTRUE);
}

void OSI_SystickHandler(void)
{
    extern void xPortSysTickHandler(void);
    xPortSysTickHandler();
}


#if configUSE_IDLE_HOOK
void vApplicationIdleHook(void)
{
//    LOG_IO(1, 3, 1, 0);

    //os_PM_SysHandle(LL_PWR_MODE_STOP0);

//   LOG_IO(1, 3, 0, 0);
}
#endif

#if configCHECK_FOR_STACK_OVERFLOW
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    LOG_PRINT("\n\n\nStack Over Flow\n");
    LOG_PRINT("task-name: %s\n\n", pcTaskGetName(pxTask));
    while (1);
}
#endif

#if configUSE_MALLOC_FAILED_HOOK

#endif


static void os_PM_SysSuspend(void)
{
    LOG_IO(LOG_IO_TAG_PM, 1, 1, 0);

    sys_PmContex.CLICCFG = CLIC->CLICCFG;
    sys_PmContex.MINTTHRESH = CLIC->MINTTHRESH;

    for (uint16_t idx = 0; idx < CONFIG_CPU_IRQ_MAX; idx++) {
        sys_PmContex.CTL[idx] = CLIC->CLICINT[idx].CTL;
        sys_PmContex.ATTR[idx] |= CLIC->CLICINT[idx].ATTR;
        sys_PmContex.IE[idx] = CLIC->CLICINT[idx].IE;
    }

    if (pmuSuspend) {
        pmuSuspend();
    }

#if CONFIG_PRINTF_EARLY
    extern void PrintfEarly_Suspend(void);
    PrintfEarly_Suspend();
#endif
    LOG_IO(LOG_IO_TAG_PM, 1, 0, 0);
}

static void os_PM_SysResume(void)
{
    LOG_IO_RESUME();
    LOG_IO(LOG_IO_TAG_PM, 1, 1, 0);

    CLIC->CLICCFG = sys_PmContex.CLICCFG;
    CLIC->MINTTHRESH = sys_PmContex.MINTTHRESH;

    for (uint16_t idx = 0; idx < CONFIG_CPU_IRQ_MAX; idx++) {
        CLIC->CLICINT[idx].CTL = sys_PmContex.CTL[idx];
        CLIC->CLICINT[idx].ATTR = sys_PmContex.ATTR[idx];
        CLIC->CLICINT[idx].IE = sys_PmContex.IE[idx];
    }

    if (pmuResume) {
        pmuResume();
    }

#if CONFIG_PRINTF_EARLY
    void PrinftEarly_Resume(void);
    PrinftEarly_Resume();
#endif
    LL_SPLK_Init();

    LOG_IO(LOG_IO_TAG_PM, 1, 0, 0);
}

static void os_PM_SysHandle(uint32_t pmState)
{
    if (pmState != LL_PWR_MODE_STOP0) {
        os_PM_SysSuspend();
    }

    if (os_sys_id) {
        LL_PMU_LPMS_SetSys1State(pmState);
    } else {
        LL_PMU_LPMS_SetSys0State(pmState);
    }

    if (LL_PWR_MODE_STOP0 == pmState) {
        sys_sleep(0, sys_PmContex.CPU);
    } else {
        sys_sleep(1, sys_PmContex.CPU);
    }

    if (pmState != LL_PWR_MODE_STOP0) {
        os_PM_SysResume();
    }
}

void PMU_HandleInit(PMU_HandleCallback_t suspendCb, PMU_HandleCallback_t resumeCb)
{
    pmuSuspend = suspendCb;
    pmuResume = resumeCb;
}


static void os_tick_handler(void * param)
{
    LOG_IO(LOG_IO_TAG_PM, 2, 1, 0);

    pmu_tmr_time_set(os_tick_delta_value);

    OSI_SystickHandler();

    LOG_IO(LOG_IO_TAG_PM, 2, 0, 0);
}

/*
 * Take affect only in sys tick freq 32768.
 */
static uint32_t cyc_to_tick(uint64_t cyc) {
    if (cyc >= 0x20C49BA5C3)
        return 0xFFFFFFFF;
    return (cyc * 125 + 2048) >> 12;
}

static uint64_t tick_to_cyc(uint32_t tick) {
    return ((uint64_t)tick * 4096 + 62) / 125;
}

void sys_sleep_notify(uint8_t enter)
{
//    if (enter)
//        LOG_IO(LOG_IO_TAG_PM, 5, 1, 0);
}

void os_Tick_set_timeout(uint32_t ticks)
{
   //pmu_tmr_time_set(tick_to_cyc(ticks));
    pmu_tmr_time_64_set(tick_to_cyc(ticks));
}

uint32_t os_Tick_convert(uint64_t cyc)
{
   return cyc_to_tick(cyc);
   //return (cyc & PMU_TMR_CYC_MAX) *
    //                                  CONFIG_SYSTICK_HZ / tick_freq;
}

uint32_t os_Tick_trig(void)
{
   return pmu_tmr_trig();
}

void os_PM_set_state(uint8_t state, uint8_t substate)
{
    os_PM_SysHandle(substate);
}

void os_Tick_reverse(void)
{
    pmu_tmr_time_set(os_tick_delta_value);
}

HAL_Status_t OSI_Init(void)
{
    IRQ_RegisterExtHandler(NULL, OSI_IntrptExit);

    os_sys_id = LL_SYS_GetCurrentSysId();

    os_system_freq = HAL_BOARD_GetFreq(CLOCK_CURR_CPU);

    if (pmu_tmr_init() != 0) {
        return HAL_STATUS_ERROR;
    }
    tick_freq = HAL_BOARD_GetFreq(CLOCK_LPCLK_FRAC_DIV);
    os_tick_delta_value = tick_freq / CONFIG_SYSTICK_HZ;
    pmu_tmr_callback_set(os_tick_handler);
    pmu_tmr_time_set(os_tick_delta_value);

    return HAL_STATUS_OK;
}

#endif /* CONFIG_KERNEL_FREERTOS */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
