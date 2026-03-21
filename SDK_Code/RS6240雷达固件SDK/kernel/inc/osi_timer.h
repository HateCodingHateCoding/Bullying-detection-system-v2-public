/**
 ******************************************************************************
 * @file    osi_timer.h
 * @brief   rtos interface timer define.
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

#ifndef KERNEL_INC_OSI_TIMER_H_
#define KERNEL_INC_OSI_TIMER_H_

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include <osi_common.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */

/** @brief Timer handle definition */
typedef OSI_Handle_t OSI_TimerHandle_t;

/**
 * @brief Timer object definition
 */
typedef struct OSI_Timer {
    OSI_TimerHandle_t handle;
} OSI_Timer_t;

/**
 * @brief Timer type definition
 *     - one shot timer: Timer will be in the dormant state after it expires.
 *     - periodic timer: Timer will auto-reload after it expires.
 */
typedef enum {
    OSI_TIMER_ONCE       = 0, /* one shot timer */
    OSI_TIMER_PERIODIC   = 1  /* periodic timer */
} OSI_TimerType_t;

/** @brief Timer expire callback function definition */
typedef void (*OSI_TimerCallback_t)(void *arg);

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
/* Parameters used to convert the time values */
#define OSI_MSEC_PER_SEC     1000U       /* milliseconds per second */
#define OSI_USEC_PER_MSEC    1000U       /* microseconds per millisecond */
#define OSI_USEC_PER_SEC     1000000U    /* microseconds per second */


/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Macros used to convert various time units to each other
 *     - Secs stand for seconds
 *     - MSecs stand for milliseconds
 *     - Ticks stand for OS ticks
 *     - Jiffies stand for OS jiffies, which is a synonym for OS ticks
 */

#define OSI_SecsToTicks(sec)     ((OSI_Time_t)(sec) * CONFIG_SYSTICK_HZ)
#define OSI_MSecsToTicks(msec)   ((OSI_Time_t)(msec) * (CONFIG_SYSTICK_HZ / OSI_MSEC_PER_SEC))
#define OSI_TicksToMSecs(t)      ((OSI_Time_t)(t) * (OSI_MSEC_PER_SEC / CONFIG_SYSTICK_HZ))
#define OSI_TicksToSecs(t)       ((OSI_Time_t)(t) / CONFIG_SYSTICK_HZ)


/**
 * @brief Macros used to sleep for the given seconds
 */
#define OSI_Sleep(sec)   OSI_MSleep((sec) * OSI_MSEC_PER_SEC)
#define OSI_SSleep(sec)  OSI_Sleep(sec)

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Get the number of ticks since OS start
 * @return the number of ticks since OS start
 */
OSI_Time_t OSI_GetTicks(void);


/**
 * @brief Get the number of seconds since OS start
 * @return the number of seconds since OS start
 */
OSI_Time_t OSI_GetTime(void);

/**
 * @brief Sleep for the given milliseconds
 * @param[in] msec Milliseconds to sleep
 */
void OSI_MSleep(OSI_Time_t msec);


void OSI_delay_busy_wait(OSI_Time_t msec);

/**
 * @brief Create and initialize a timer object
 *
 * @note Creating a timer does not start the timer running. The OSI_TimerStart()
 *       and OSI_TimerChangePeriod() API functions can all be used to start the
 *       timer running.
 *
 * @param[in] timer Pointer to the timer object
 * @param[in] type Timer type
 * @param[in] cb Timer expire callback function
 * @param[in] arg Argument of timer expire callback function
 * @param[in] periodMS Timer period in milliseconds
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_TimerCreate(OSI_Timer_t *timer, OSI_TimerType_t type,
                         OSI_TimerCallback_t cb, void *arg, OSI_Time_t periodMS);

/**
 * @brief Delete the timer object
 * @param[in] timer Pointer to the timer object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_TimerDelete(OSI_Timer_t *timer);

/**
 * @brief Start a timer running.
 * @note If the timer is already running, this function will re-start the timer.
 * @param[in] timer Pointer to the timer object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_TimerStart(OSI_Timer_t *timer);

/**
 * @brief Change the period of a timer
 *
 * If OSI_TimerChangePeriod() is used to change the period of a timer that is
 * already running, then the timer will use the new period value to recalculate
 * its expiry time. The recalculated expiry time will then be relative to when
 * OSI_TimerChangePeriod() was called, and not relative to when the timer was
 * originally started.

 * If OSI_TimerChangePeriod() is used to change the period of a timer that is
 * not already running, then the timer will use the new period value to
 * calculate an expiry time, and the timer will start running.
 *
 * @param[in] timer Pointer to the timer object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_TimerChangePeriod(OSI_Timer_t *timer, OSI_Time_t periodMS);

/**
 * @brief Stop a timer running.
 * @param[in] timer Pointer to the timer object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_TimerStop(OSI_Timer_t *timer);

OSI_Status_t OSI_TimerRestart(OSI_Timer_t *timer);

/**
 * @brief Check whether the timer is active or not
 *
 * A timer is inactive when it is in one of the following cases:
 *     - The timer has been created, but not started.
 *     - The timer is a one shot timer that has not been restarted since it
 *       expired.
 *
 * @param[in] timer Pointer to the timer object
 * @return 1 on active, 0 on inactive
 */
int OSI_TimerIsActive(OSI_Timer_t *timer);

/**
 * @brief Check whether the timer object is valid or not
 * @param[in] timer Pointer to the timer object
 * @return 1 on valid, 0 on invalid
 */
static __always_inline int OSI_TimerIsValid(OSI_Timer_t *timer) {
    return (timer && timer->handle != OSI_INVALID_HANDLE);
}

/**
 * @brief Set the timer object to invalid state
 * @param[in] timer Pointer to the timer object
 * @return None
 */
static __always_inline void OSI_TimerSetInvalid(OSI_Timer_t *timer) {
    timer->handle = OSI_INVALID_HANDLE;
}


#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INC_OSI_TIMER_H_ */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
