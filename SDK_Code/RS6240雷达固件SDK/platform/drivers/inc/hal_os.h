/**
 ******************************************************************************
 * @file    hal_os.h
 * @brief   hal os define.
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

#ifndef _HAL_OSI_H
#define _HAL_OSI_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "osi.h"
#include "irq.h"
#include <string.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */
/* Exported constants.
 * ----------------------------------------------------------------------------
 */
/* Exported macro.
 * ----------------------------------------------------------------------------
 */
/* IRQ disable/enable */
#define HAL_DisableIRQ()                __disable_irq()
#define HAL_EnableIRQ()                 __enable_irq()

/* Check if IRQ is disabled */
#define HAL_IsIRQDisabled()             __is_disable_irq()

/* Check if in ISR context or not */
#define HAL_IsISRContext()              OSI_IsISRContext()

/* Critical Sections */
#define HAL_EnterCriticalSection()      __lock_irq()
#define HAL_ExitCriticalSection(flags)  __unlock_irq(flags)

/* Semaphore */
typedef OSI_Semaphore_t HAL_Semaphore;

#define HAL_SemaphoreInit(sem, initCount, maxCount) \
    (OSI_SemaphoreCreate(sem, initCount, maxCount) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_SemaphoreInitBinary(sem) \
    (OSI_SemaphoreCreateBinary(sem) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_SemaphoreDeinit(sem) \
    (OSI_SemaphoreDelete(sem) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_SemaphoreWait(sem, msec) \
    (OSI_SemaphoreWait(sem, msec) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_SemaphoreRelease(sem) \
    (OSI_SemaphoreRelease(sem) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_SemaphoreIsValid(sem) \
    OSI_SemaphoreIsValid(sem)

#define HAL_SemaphoreSetInvalid(sem) \
    OSI_SemaphoreSetInvalid(sem)

/* Mutex */
typedef OSI_Mutex_t HAL_Mutex;

#define HAL_MutexInit(mtx) \
    (OSI_RecursiveMutexCreate(mtx) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_MutexDeinit(mtx) \
    (OSI_RecursiveMutexDelete(mtx) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_MutexLock(mtx, msec) \
    (OSI_RecursiveMutexLock(mtx, msec) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_MutexUnlock(mtx) \
    (OSI_RecursiveMutexUnlock(mtx) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

/* Thread */
#define HAL_ThreadSuspendScheduler()    OSI_ThreadSuspendScheduler()
#define HAL_ThreadResumeScheduler()     OSI_ThreadResumeScheduler()
#define HAL_ThreadIsSchedulerRunning()  OSI_ThreadIsSchedulerRunning()

/* Timer */
typedef OSI_Timer_t HAL_Timer;

#define HAL_TimerInit(timer, type, cb, arg, msec) \
    (OSI_TimerCreate(timer, type, cb, arg, msec) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_TimerDeinit(timer) \
    (OSI_TimerDelete(timer) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_TimerStart(timer) \
    (OSI_TimerStart(timer) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_TimerChangePeriod(timer, msec) \
    (OSI_TimerChangePeriod(timer, msec) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_TimerStop(timer) \
    (OSI_TimerStop(timer) == OSI_STATUS_OK ? HAL_STATUS_OK : HAL_STATUS_ERROR)

#define HAL_TimerIsActive(timer) \
    OSI_TimerIsActive(timer)


/* Time */
#define HAL_Ticks()                 OSI_GetTicks()
#define HAL_MSleep(msec)            OSI_MSleep(msec)
#define HAL_delay_busy_wait(msec)   OSI_delay_busy_wait(msec)

#define HAL_SecsToTicks(sec)        OSI_SecsToTicks(sec)
#define HAL_MSecsToTicks(msec)      OSI_MSecsToTicks(msec)
#define HAL_TicksToMSecs(t)         OSI_TicksToMSecs(t)
#define HAL_TicksToSecs(t)          OSI_TicksToSecs(t)

#define HAL_TimeAfter(a, b)         OSI_TimeAfter(a, b)
#define HAL_TimeBefore(a, b)        OSI_TimeBefore(a, b)
#define HAL_TimeAfterEqual(a, b)    OSI_TimeAfterEqual(a, b)
#define HAL_TimeBeforeEqual(a, b)   OSI_TimeBeforeEqual(a, b)

/* Memory */
#define HAL_Malloc(l)               OSI_Malloc(l)
#define HAL_Free(p)                 OSI_Free(p)
#define HAL_Memcpy(d, s, l)         OSI_Memcpy(d, s, l)
#define HAL_Memset(d, c, l)         OSI_Memset(d, c, l)
#define HAL_Memcmp(a, b, l)         OSI_Memcmp(a, b, l)
#define HAL_Memmove(d, s, n)        OSI_Memmove(d, s, n)

/* Private macro.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _HAL_OSI_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
