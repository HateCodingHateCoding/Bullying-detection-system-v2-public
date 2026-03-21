/**
 ******************************************************************************
 * @file    task_port.c
 * @brief   task_port define.
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
#include "arch_riscv_config.h"

#if CONFIG_CPU_KERNEL

#include "FreeRTOS.h"
#include "task.h"

#include "rs6130_config.h"

#if (CONFIG_PM)
#include "pm.h"
#include "ll_pmu.h"
#endif

#include "log.h"
#include "pmu_tmr.h"

#include "hal_board.h"

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

/* Used to keep track of the number of nested calls to taskENTER_CRITICAL().  This
will be set to 0 prior to the first task being started. */
portLONG ulCriticalNesting = 0x9999UL;

/* Used to record one tack want to swtich task after enter critical area, we need know it
 * and implement task switch after exit critical area */
portLONG pendsvflag;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
extern void vPortStartTask(void);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

extern void os_Tick_set_timeout(uint32_t ticks);
extern uint32_t os_Tick_convert(uint64_t cyc);
extern uint32_t os_Tick_trig(void);
extern void os_Tick_reverse(void);

#if (portHAS_STACK_OVERFLOW_CHECKING == 1)
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, StackType_t *pxEndOfStack, TaskFunction_t pxCode, void *pvParameters)
#else
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
#endif
{
    extern int __global_pointer$;
    StackType_t *stk  = NULL;
    uint32_t temp = (uint32_t)pxTopOfStack;
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    uint32_t status = __get_MSTATUS();
#endif

    temp &= 0xFFFFFFF8UL;
    stk = (StackType_t *)temp;
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    *(--stk)  = (uint32_t)status;
#endif
    *(--stk)  = (uint32_t)pxCode;            /* Entry Point */
    *(--stk)  = (uint32_t)0x31313131L;       /* X31         */
    *(--stk)  = (uint32_t)0x30303030L;       /* X30         */
    *(--stk)  = (uint32_t)0x29292929L;       /* X29         */
    *(--stk)  = (uint32_t)0x28282828L;       /* X28         */
    *(--stk)  = (uint32_t)0x27272727L;       /* X27         */
    *(--stk)  = (uint32_t)0x26262626L;       /* X26         */
    *(--stk)  = (uint32_t)0x25252525L;       /* X25         */
    *(--stk)  = (uint32_t)0x24242424L;       /* X24         */
    *(--stk)  = (uint32_t)0x23232323L;       /* X23         */
    *(--stk)  = (uint32_t)0x22222222L;       /* X22         */
    *(--stk)  = (uint32_t)0x21212121L;       /* X21         */
    *(--stk)  = (uint32_t)0x20202020L;       /* X20         */
    *(--stk)  = (uint32_t)0x19191919L;       /* X19         */
    *(--stk)  = (uint32_t)0x18181818L;       /* X18         */
    *(--stk)  = (uint32_t)0x17171717L;       /* X17         */
    *(--stk)  = (uint32_t)0x16161616L;       /* X16         */
    *(--stk)  = (uint32_t)0x15151515L;       /* X15         */
    *(--stk)  = (uint32_t)0x14141414L;       /* X14         */
    *(--stk)  = (uint32_t)0x13131313L;       /* X13         */
    *(--stk)  = (uint32_t)0x12121212L;       /* X12         */
    *(--stk)  = (uint32_t)0x11111111L;       /* X11         */
    *(--stk)  = (uint32_t)pvParameters;      /* X10         */
    *(--stk)  = (uint32_t)0x09090909L;       /* X9          */
    *(--stk)  = (uint32_t)0x08080808L;       /* X8          */
    *(--stk)  = (uint32_t)0x07070707L;       /* X7          */
    *(--stk)  = (uint32_t)0x06060606L;       /* X6          */
    *(--stk)  = (uint32_t)0x05050505L;       /* X5          */
    *(--stk)  = (uint32_t)0x04040404L;       /* X4          */
    *(--stk)  = (uint32_t)&__global_pointer$;/* X3          */
#if CONFIG_AOS_OSAL
    *(--stk)  = (uint32_t)aos_task_exit;     /* X1          */
#else
    *(--stk)  = (uint32_t)vTaskDelete;       /* X1          */
#endif

#if CONFIG_CPU_FPU_ENABLE
    {
        *(--stk)  = (uint32_t)0x0L;                  /* FCSR        */

        int num = 32 / 8 * 32 / sizeof(StackType_t);
        for (int i = 0; i < num; i++) {
            *(--stk)  = (StackType_t)0x12345678L;    /* F31 ~ F0    */
        }
    }
#endif

#if CONFIG_CPU_DSP_ENABLE
    *(--stk)  = (uint32_t)0x0L;              /* P-ext vxsat reg    */
#endif /*CONFIG_CPU_DSP_ENABLE */

#if (portHAS_STACK_OVERFLOW_CHECKING == 1)
    if (stk <= pxEndOfStack) {
        //printk("pxTopOfStack: %p, pxEndOfStack: %p, stk: %p\r\n", pxTopOfStack, pxEndOfStack, stk);
        configASSERT(pdFALSE);
        return NULL;
    }
#endif
    return stk;
}

void xPortTickComp(uint32_t ExpiredTime, uint32_t xExpectedIdleTime)
{
    if (ExpiredTime) {
        if (ExpiredTime >= xExpectedIdleTime)
            ExpiredTime = xExpectedIdleTime;
        if (!os_Tick_trig()) {
            os_Tick_reverse();
        } else {
            ExpiredTime = ExpiredTime - 1;
        }
        vTaskStepTick(ExpiredTime);
    }
}

BaseType_t xPortStartScheduler(void)
{
    ulCriticalNesting = 0UL;

    vPortStartTask();

    return pdFALSE;
}


void vPortEndScheduler(void)
{
    /* Not implemented as there is nothing to return to. */
}

void vPortEnterCritical(void)
{
    portDISABLE_INTERRUPTS();
    ulCriticalNesting++;
}

void vPortExitCritical(void)
{
    if (ulCriticalNesting == 0) {
        while (1)
            ;
    }

    ulCriticalNesting--;
    if (ulCriticalNesting == 0) {
        portENABLE_INTERRUPTS();

        if (pendsvflag) {
            pendsvflag = 0;
            portYIELD();
        }
    }
}


#if configGENERATE_RUN_TIME_STATS
uint32_t xportGetTime(void)
{
//    HAL_BOARD_GetTime(HAL_TIME_US);
    return HAL_BOARD_GetTime(HAL_TIME_US);
}

void vportTaskSwitchOutHook(void)
{
    LOG_IO(LOG_IO_TAG_SCH, 11, 1, 0);

#if 0
    LOG_IO(2, 2, 0, 2);

    TaskHandle_t xTask = xTaskGetCurrentTaskHandle();
    UBaseType_t uxTaskNumber = uxTaskGetTaskNumber(xTask);
    if (uxTaskNumber == 7) {
        LOG_IO(2, 2, 0, 0);
    } else if (uxTaskNumber == 5) {
        LOG_IO(2, 4, 0, 0);
    } else if (uxTaskNumber == 6) {
        LOG_IO(2, 5, 0, 0);
    }
#endif
}

void vportTaskSwitchInHook(void)
{
    LOG_IO(LOG_IO_TAG_SCH, 11, 0, 0);

#if 0
    LOG_IO(2, 2, 0, 1);

    TaskHandle_t xTask = xTaskGetCurrentTaskHandle();
    UBaseType_t uxTaskNumber = uxTaskGetTaskNumber(xTask);
    if (uxTaskNumber == 7) {
        LOG_IO(2, 2, 1, 0);
    } else if (uxTaskNumber == 5) {
        LOG_IO(2, 4, 1, 0);
    } else if (uxTaskNumber == 6) {
        LOG_IO(2, 5, 1, 0);
    }
#endif
}
#endif


#if 0
#define CONFIG_KERNEL_TASK_MAX              12
char * str_TaskState[] = {
    "Running",
    "Ready",
    "Blocked",
    "Suspended",
    "Deleted",
    "Invalid",
};

void os_KernelStat(void)
{
    TaskStatus_t cmdKernel[CONFIG_KERNEL_TASK_MAX];
    UBaseType_t uxArraySize = uxTaskGetNumberOfTasks();

    if (uxArraySize > CONFIG_KERNEL_TASK_MAX) {
        printf("current stask number : %d, Get %d Task\n", uxArraySize, CONFIG_KERNEL_TASK_MAX);
        uxArraySize = CONFIG_KERNEL_TASK_MAX;
    }

    uint32_t pulTotalRunTime = 0;
    TaskStatus_t * pxTaskStatusArray = cmdKernel;
    uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &pulTotalRunTime);
    printf("time: %d\n\r", pulTotalRunTime);
    for (uint32_t i = 0; i < uxArraySize; i++) {
        printf("name:  [%d]  %s\n\r", pxTaskStatusArray->xTaskNumber, pxTaskStatusArray->pcTaskName);
        printf("state:       %s\n\r", str_TaskState[pxTaskStatusArray->eCurrentState]);
        printf("priority:    %d / %d (current / base)\n\r",
            pxTaskStatusArray->uxCurrentPriority, pxTaskStatusArray->uxBasePriority);
        printf("time:        %d\n\r", pxTaskStatusArray->ulRunTimeCounter);
        printf("stack:       %p %d (max used)\n\n\r",
            pxTaskStatusArray->pxStackBase, pxTaskStatusArray->usStackHighWaterMark);

        pxTaskStatusArray++;
    }
}
#endif


#if configUSE_PREEMPTION == 0
void xPortSysTickHandler(void)
{
    portLONG ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    xTaskIncrementTick();
    portCLEAR_INTERRUPT_MASK_FROM_ISR(ulDummy);
}

#else
void xPortSysTickHandler(void)
{
    portLONG ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        if (xTaskIncrementTick() != pdFALSE) {
            portYIELD_FROM_ISR(pdTRUE);
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(ulDummy);
}
#endif

void vPortYieldHandler(void)
{
    uint32_t ulSavedInterruptMask;

    ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();

    vTaskSwitchContext();

    portCLEAR_INTERRUPT_MASK_FROM_ISR(ulSavedInterruptMask);
}

__attribute__((weak)) void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
//    void *pxTopOfStack = (void *)(*(unsigned long *)pxTask);
    //printk("!!! task [%s] stack overflow. pxTop: %p\r\n", pcTaskName, pxTopOfStack);
    for (;;)
        ;
}

__attribute__((weak)) void vApplicationMallocFailedHook(size_t heapSize, size_t mallocSize, size_t freeSize)
{
    printf("\n\nmalloc fail: heap size : %d, malloc size: %d, free size: %d\n\n", heapSize, mallocSize, freeSize);
    for (;;)
        ;
}

#if (configUSE_TICKLESS_IDLE != 0)
__attribute__((weak)) void vPortSuppressTickAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t ticks;
    uint64_t now;

    vPortEnterCritical();
    LOG_IO(LOG_IO_TAG_PM, 2, 0, 2);

    now = pmu_tmr_time_get();

#if (CONFIG_PM)
    pm_set_next_timeout_expiry(xExpectedIdleTime);
    int ret = pm_system_suspend(PM_STATE_STANDBY, LL_PWR_MODE_STANDBY0);
    if (ret != 0) {
        ticks = os_Tick_convert(pmu_tmr_time_get() - now);
        if (ticks < xExpectedIdleTime) {
            os_Tick_set_timeout(xExpectedIdleTime - ticks);
            __WFI();
        }
    }
#else
    os_Tick_set_timeout(xExpectedIdleTime);
    __WFI();
#endif
    ticks = os_Tick_convert(pmu_tmr_time_get() - now);
    xPortTickComp(ticks, xExpectedIdleTime);

    LOG_IO(LOG_IO_TAG_PM, 2, 0, 3);
    vPortExitCritical();
}

__attribute__((weak)) void vPortSuppressTickAndSleepLight(TickType_t xExpectedIdleTime)
{
    uint32_t ticks;
    uint64_t now;

    vPortEnterCritical();
    now = pmu_tmr_time_get();
    os_Tick_set_timeout(xExpectedIdleTime);
    __WFI();
    ticks = os_Tick_convert(pmu_tmr_time_get() - now);
    xPortTickComp(ticks, xExpectedIdleTime);
    vPortExitCritical();
}
#endif

#endif /* CONFIG_CPU_KERNEL */


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
