/**
 ******************************************************************************
 * @file    osi_timer.c
 * @brief   RTOS interface timer define.
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

#include "osi_timer.h"
#include "osi_util.h"
#include "osi_memory.h"
#include "timers.h"


/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define OS_TIMER_USE_FREERTOS_ORIG_CALLBACK    1
/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* TODO: what block time should be used ? */
#define OSI_TIMER_WAIT_FOREVER    portMAX_DELAY
#define OS_TIMER_WAIT_NONE        0

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
#if OS_TIMER_USE_FREERTOS_ORIG_CALLBACK

/* Timer private data definition */
typedef struct OSI_TimerPriv {
    TimerHandle_t       handle;   /* Timer handle */
    OSI_TimerCallback_t  callback; /* Timer expire callback function */
    void               *argument; /* Argument of timer expire callback function */
} OSI_TimerPriv_t;

#endif

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

#if OS_TIMER_USE_FREERTOS_ORIG_CALLBACK

static void OSI_TimerPrivCallback(TimerHandle_t xTimer)
{
    OSI_TimerPriv_t *priv;

    priv = pvTimerGetTimerID(xTimer);
    if (priv && priv->callback) {
        priv->callback(priv->argument);
    }
}

OSI_Status_t OSI_TimerCreate(OSI_Timer_t *timer, OSI_TimerType_t type,
                         OSI_TimerCallback_t cb, void *arg, uint32_t periodMS)
{
    OSI_TimerPriv_t *priv;

    if (OSI_TimerIsValid(timer)) {
        return OSI_STATUS_ERR_PARAM;
    }

    priv = OSI_Malloc(sizeof(OSI_TimerPriv_t));
    if (priv == NULL) {
        return OSI_STATUS_ERR_NOMEM;
    }

    priv->callback = cb;
    priv->argument = arg;
    priv->handle = xTimerCreate("",
                                OSI_MSecsToTicks(periodMS),
                                type == OSI_TIMER_PERIODIC ? pdTRUE : pdFALSE,
                                priv,
                                OSI_TimerPrivCallback);
    if (priv->handle == NULL) {
        OSI_Free(priv);
        return OSI_STATUS_FAIL;
    }
    timer->handle = priv;
    return OSI_STATUS_OK;
}

static __inline TimerHandle_t OSI_TimerGetKernelHandle(OSI_Timer_t *timer)
{
    OSI_TimerPriv_t *priv = timer->handle;
    return priv->handle;
}

#else /* OS_TIMER_USE_FREERTOS_ORIG_CALLBACK */

OSI_Status_t OSI_TimerCreate(OSI_Timer_t *timer, OSI_TimerType type,
                         OSI_TimerCallback_t cb, void *arg, uint32_t periodMS)
{
    if (OSI_TimerIsValid(timer)) {
        return OSI_STATUS_ERR_PARAM;
    }

    timer->handle = xTimerCreate("",
                                 OS_MSecsToTicks(periodMS),
                                 type == OS_TIMER_PERIODIC ? pdTRUE : pdFALSE,
                                 arg,
                                 cb);
    if (timer->handle == NULL) {
        OS_ERR("err %"OS_HANDLE_F"\n", timer->handle);
        return OSI_STATUS_FAIL;
    }
    return OSI_STATUS_OK;
}

static __inline TimerHandle_t OSI_TimerGetKernelHandle(OSI_Timer_t *timer)
{
    return timer->handle;
}

#endif /* OS_TIMER_USE_FREERTOS_ORIG_CALLBACK */

OSI_Status_t OSI_TimerDelete(OSI_Timer_t *timer)
{
    TimerHandle_t handle;
    BaseType_t ret;

    if (!OSI_TimerIsValid(timer)) {
        return OSI_STATUS_ERR_PARAM;
    }

    handle = OSI_TimerGetKernelHandle(timer);
    ret = xTimerDelete(handle, OSI_TIMER_WAIT_FOREVER);
    if (ret != pdPASS) {
        return OSI_STATUS_FAIL;
    }

#if OS_TIMER_USE_FREERTOS_ORIG_CALLBACK
    OSI_TimerPriv_t *priv = timer->handle;
#endif
    OSI_TimerSetInvalid(timer);
#if OS_TIMER_USE_FREERTOS_ORIG_CALLBACK
    OSI_Free(priv);
#endif
    return OSI_STATUS_OK;
}

OSI_Status_t OSI_TimerStart(OSI_Timer_t *timer)
{
    TimerHandle_t handle;
    BaseType_t ret;
    BaseType_t taskWoken;

    if (!OSI_TimerIsValid(timer)) {
        return OSI_STATUS_ERR_PARAM;
    }

    handle = OSI_TimerGetKernelHandle(timer);

    if (OSI_IsISRContext()) {
        taskWoken = pdFALSE;
        ret = xTimerStartFromISR(handle, &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xTimerStart(handle, OS_TIMER_WAIT_NONE);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_TimerChangePeriod(OSI_Timer_t *timer, uint32_t periodMS)
{
    TimerHandle_t handle;
    BaseType_t ret;
    BaseType_t taskWoken;

    if (!OSI_TimerIsValid(timer)) {
        return OSI_STATUS_ERR_PARAM;
    }

    handle = OSI_TimerGetKernelHandle(timer);

    if (OSI_IsISRContext()) {
        taskWoken = pdFALSE;
        ret = xTimerChangePeriodFromISR(handle, OSI_MSecsToTicks(periodMS), &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xTimerChangePeriod(handle, OSI_MSecsToTicks(periodMS), OS_TIMER_WAIT_NONE);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_TimerStop(OSI_Timer_t *timer)
{
    TimerHandle_t handle;
    BaseType_t ret;
    BaseType_t taskWoken;

    if (!OSI_TimerIsValid(timer)) {
        return OSI_STATUS_ERR_PARAM;
    }

    handle = OSI_TimerGetKernelHandle(timer);

    if (OSI_IsISRContext()) {
        taskWoken = pdFALSE;
        ret = xTimerStopFromISR(handle, &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xTimerStop(handle, OSI_TIMER_WAIT_FOREVER);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_TimerRestart(OSI_Timer_t *timer)
{
    TimerHandle_t handle;
    BaseType_t ret;
    BaseType_t taskWoken;

    if (!OSI_TimerIsValid(timer)) {
        return OSI_STATUS_ERR_PARAM;
    }

    handle = OSI_TimerGetKernelHandle(timer);

    if (OSI_IsISRContext()) {
        taskWoken = pdFALSE;
        ret = xTimerStopFromISR(handle, &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xTimerReset(handle, OSI_TIMER_WAIT_FOREVER);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
    }

    return OSI_STATUS_OK;
}

int OSI_TimerIsActive(OSI_Timer_t *timer)
{
    TimerHandle_t handle;

    if (!OSI_TimerIsValid(timer)) {
        return 0;
    }

    handle = OSI_TimerGetKernelHandle(timer);

    return (xTimerIsTimerActive(handle) != pdFALSE);
}

void OSI_MSleep(OSI_Time_t msec)
{
    vTaskDelay(pdMS_TO_TICKS(msec));
}

void OSI_delay_busy_wait(OSI_Time_t msec)
{
	OSI_Time_t count = 0;
	count = OSI_GetTicks();
	while(OSI_GetTicks() - count < msec);
}


OSI_Time_t OSI_GetTicks(void)
{
    return (OSI_Time_t)xTaskGetTickCount();
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
