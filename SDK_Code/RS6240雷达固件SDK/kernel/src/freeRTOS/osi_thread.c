/**
 ******************************************************************************
 * @file    osi_thread.c
 * @brief   RTOS interface thread define.
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

#include "osi_thread.h"
#include "osi_timer.h"
#include "osi_util.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define OSI_KERNEL_PRIO(prio) (prio)

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

OSI_Status_t OSI_ThreadCreate(OSI_Thread_t *thread,
                            const char *name,
                            OSI_ThreadEntry_t entry,
                            void *arg,
                            OSI_Priority_t priority,
                            uint32_t stackSize)
{
    BaseType_t ret;

    if (OSI_ThreadIsValid(thread)) {
        return OSI_STATUS_ERR_PARAM;
    }

    ret = xTaskCreate(entry,
                    name,
                    stackSize / sizeof(StackType_t),
                    arg,
                    OSI_KERNEL_PRIO(priority),
                    (TaskHandle_t * const)&thread->handle);
    if (ret != pdPASS) {
        OSI_ThreadSetInvalid(thread);
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_ThreadDelete(OSI_Thread_t *thread)
{
    TaskHandle_t handle;
    TaskHandle_t curHandle;

    if (thread == NULL) {
        vTaskDelete(NULL); /* delete self */
        return OSI_STATUS_OK;
    }

    if (!OSI_ThreadIsValid(thread)) {
        return OSI_STATUS_ERR_PARAM;
    }

    handle = thread->handle;
    curHandle = xTaskGetCurrentTaskHandle();
    if (handle == curHandle) {
        /* delete self */
        OSI_ThreadSetInvalid(thread);
        vTaskDelete(NULL);
    } else {
        /* delete other thread */
        vTaskDelete(handle);
        OSI_ThreadSetInvalid(thread);
    }

    return OSI_STATUS_OK;
}

void OSI_ThreadSleep(OSI_Time_t msec)
{
    vTaskDelay((TickType_t)OSI_MSecsToTicks(msec));
}

void OSI_ThreadYield(void)
{
    taskYIELD();
}

OSI_ThreadHandle_t OSI_ThreadGetCurrentHandle(void)
{
    return (OSI_ThreadHandle_t)xTaskGetCurrentTaskHandle();
}

void OSI_ThreadStartScheduler(void)
{
    vTaskStartScheduler();
}

void OSI_ThreadSuspendScheduler(void)
{
    vTaskSuspendAll();
}

void OSI_ThreadResumeScheduler(void)
{
    xTaskResumeAll();
}

int OSI_ThreadIsSchedulerRunning(void)
{
    return (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
}

#if INCLUDE_uxTaskGetStackHighWaterMark
uint32_t OSI_ThreadGetStackMinFreeSize(OSI_Thread_t *thread)
{
    TaskHandle_t handle;

    if (thread != NULL) {
        if (OSI_ThreadIsValid(thread)) {
            handle = thread->handle;
        } else {
            return 0;
        }
    } else {
        handle = NULL;
    }

    return (uxTaskGetStackHighWaterMark(handle) * sizeof(StackType_t));
}
#endif

// #if (configCHECK_FOR_STACK_OVERFLOW > 0)
// void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
// {

// }
// #endif

void OSI_ThreadList(void)
{

}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
