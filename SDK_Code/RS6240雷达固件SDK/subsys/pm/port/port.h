/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PORT_H
#define _PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "pm.h"
#include "osi_timer.h"

#define PM_DBG_ON    (0)
#if PM_DBG_ON
#define PM_DBG(fmt, arg...)          printf(fmt, ##arg)
#else
#define PM_DBG(fmt, arg...)
#endif

#define PM_WAIT_FOREVER              (0xFFFFFFFFU)
#define PM_TimeAfter(a, b)           ((int)(b) - (int)(a) < 0)
#define PM_TimeBefore(a, b)          PM_TimeAfter(b, a)
#define PM_TimeAfterEqual(a, b)      ((int)(a) - (int)(b) >= 0)
#define PM_TimeBeforeEqual(a, b)          PM_TimeAfterEqual(b, a)

/** implemented by specific SoC or OS */

#define PM_GetTicks()                       OSI_GetTicks()
#define PM_MsecsTOTicks(msec)        OSI_MSecsToTicks(msec)
#define PM_TicksTOMsecs(ticks)        OSI_TicksToMSecs(ticks)

typedef void * pm_timer_t;

typedef void (*timer_cb_t)(void *arg);

typedef void * pm_spinlock_t;

enum pm_state;

int pm_get_current_cpu(void);
void pm_set_next_timeout_expiry(uint32_t  ticks);
uint32_t pm_get_next_timeout_expiry(void);
int pm_set_sys_clock_timeout(uint32_t ticks);
int pm_state_set(enum pm_state state, uint8_t substate_id);

pm_spinlock_t pm_spin_lock(void);
void pm_spin_unlock(pm_spinlock_t key);

pm_timer_t pm_timer_create(timer_cb_t cb, uint32_t timeout_ms);
void pm_timer_start(pm_timer_t timer);
void pm_timer_update(pm_timer_t timer, uint32_t timeout_ms);

void pm_sched_lock(void);

void pm_sched_unlock(void);


#ifdef __cplusplus
}
#endif

#endif /* _PORT_H */

