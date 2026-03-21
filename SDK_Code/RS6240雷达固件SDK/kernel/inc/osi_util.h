/**
 ******************************************************************************
 * @file    osi_util.h
 * @brief   RTOS interface util define.
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

#ifndef KERNEL_INC_OSI_UTIL_H_
#define KERNEL_INC_OSI_UTIL_H_

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include <string.h>
#include <stdlib.h>

#include "osi_timer.h"
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>

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
#define OSI_TimeAfter(a, b)              ((int32_t)(b) - (int32_t)(a) < 0)
#define OSI_TimeBefore(a, b)             OSI_TimeAfter(b, a)
#define OSI_TimeAfterEqual(a, b)         ((int32_t)(a) - (int32_t)(b) >= 0)
#define OSI_TimeBeforeEqual(a, b)        OSI_TimeAfterEqual(b, a)
/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/* check if in ISR context or not */
static __always_inline int OSI_IsISRContext(void)
{
    extern volatile uint32_t  Irq_NestedLevel;
    return (Irq_NestedLevel != 0);
}

static __always_inline TickType_t OSI_CalcWaitTicks(OSI_Time_t msec)
{
    TickType_t tick;

    if (msec == OSI_WAIT_FOREVER) {
        tick = portMAX_DELAY;
    } else if (msec == 0) {
        tick = 0;
    } else {
        tick = OSI_MSecsToTicks(msec);
        if (tick == 0) {
            tick = 1;
        }
    }
    return tick;
}

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INC_OSI_UTIL_H_ */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
