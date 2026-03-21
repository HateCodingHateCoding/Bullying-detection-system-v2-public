/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "port.h"
#include "osi_timer.h"

static uint32_t next_timeout_expiry;

/* SOC PORT */
int pm_get_current_cpu(void)
{
	return 0;
}

void pm_set_next_timeout_expiry(uint32_t  ticks)
{
	next_timeout_expiry = ticks;
}

uint32_t  pm_get_next_timeout_expiry(void)
{
	return next_timeout_expiry;
}

int pm_set_sys_clock_timeout(uint32_t ticks)
{
	void os_Tick_set_timeout(uint32_t ticks);
	os_Tick_set_timeout(ticks);
	return 0;
}

int pm_state_set(enum pm_state state, uint8_t substate_id)
{
	//printf("pm_state_set, %d\n", substate_id);
	void os_PM_set_state(uint8_t state, uint8_t substate);
	os_PM_set_state(state, substate_id);
	//printf("pm_state_set, wakeup\n");
	return 0;
}

pm_spinlock_t pm_spin_lock(void)
{
	unsigned long key = __lock_irq();
	return (pm_spinlock_t)key;
}

void pm_spin_unlock(pm_spinlock_t key)
{
	__unlock_irq((unsigned long)key);
}

/* OS PORT */
void pm_sched_lock(void)
{
// vPortEnterCritical(  );
}

void pm_sched_unlock(void)
{
//vPortExitCritical(  );
}

static OSI_Timer_t pm_timer;
static timer_cb_t pm_timer_cb = NULL;

static void OSI_TimerCallback(void *arg)
{
	if (pm_timer_cb)
		pm_timer_cb(arg);
}

pm_timer_t pm_timer_create(timer_cb_t cb, uint32_t timeout_ms)
{
	pm_timer_cb = cb;
	OSI_Status_t status = OSI_TimerCreate(&pm_timer, OSI_TIMER_ONCE,
                         OSI_TimerCallback, NULL, timeout_ms);
	return (status == OSI_STATUS_OK) ? (pm_timer_t)&pm_timer : NULL;
}

void pm_timer_start(pm_timer_t timer)
{
	OSI_TimerStart((OSI_Timer_t *)timer);
}

void pm_timer_update(pm_timer_t timer, uint32_t timeout_ms)
{
	OSI_TimerChangePeriod((OSI_Timer_t *)timer, timeout_ms);
}

