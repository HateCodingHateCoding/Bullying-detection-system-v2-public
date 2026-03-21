/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "sys/slist.h"
#include "pm.h"
#include "policy.h"

#if (CONFIG_PM)
static pm_timer_t pm_wakelock_timer;
static sys_slist_t pm_wakelock = SYS_SLIST_STATIC_INIT(&pm_wakelock);

static void pm_wake_lock_update(void)
{
	struct wakelock *lock, *tmp;
	uint32_t expires = PM_WAIT_FOREVER;

	uint32_t ticks = PM_GetTicks();

	pm_sched_lock();
	SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&pm_wakelock, lock, tmp, _node) {
		if (lock) {
			if (lock->expires != PM_WAIT_FOREVER) {
				if (PM_TimeAfterEqual(ticks, lock->expires)) {
					sys_slist_find_and_remove(&pm_wakelock, &(lock->_node));
					continue;
				} else if (expires == PM_WAIT_FOREVER || PM_TimeAfter(expires, lock->expires)) {
					expires = lock->expires;
				}
			}
		}
	}
	pm_sched_unlock();

	if (expires != PM_WAIT_FOREVER) {
		PM_DBG("wakelock timeout update, %d\n", PM_TicksTOMsecs(expires - PM_GetTicks()));
		pm_timer_update(pm_wakelock_timer, PM_TicksTOMsecs(expires - PM_GetTicks()));
	}
}

static void pm_wake_timer_cb(void *arg)
{
	PM_DBG("wakelock timer cb\n");
	pm_wake_lock_update();
}

void pm_policy_wake_lock(struct wakelock *lock)
{
	uint8_t added = 0;
	sys_snode_t *node;

	pm_sched_lock();
	SYS_SLIST_FOR_EACH_NODE(&pm_wakelock, node) {
		if (&lock->_node == node) {
			added = 1;
			break;
		}
	}

	if (!added) {
		lock->ref = 0;
		lock->expires = PM_WAIT_FOREVER;
		sys_slist_append(&pm_wakelock, &lock->_node);
	}
	lock->ref++;
	pm_sched_unlock();
}

void pm_policy_wake_unlock(struct wakelock *lock)
{
	pm_sched_lock();

	if (lock->ref > 0) {
		lock->ref--;
		if (lock->ref == 0)
			sys_slist_find_and_remove(&pm_wakelock, &(lock->_node));
	}

	pm_sched_unlock();
}

void pm_wake_lock_timeout(struct wakelock *lock, uint32_t timeout)
{
	uint8_t added = 0;
	uint32_t ticks;
	sys_snode_t *node;

	ticks = PM_GetTicks();

	pm_sched_lock();
	SYS_SLIST_FOR_EACH_NODE(&pm_wakelock, node) {
		if (&lock->_node == node) {
			added = 1;
			break;
		}
	}
	lock->ref++;
	lock->expires = PM_MsecsTOTicks(timeout) + ticks;

	if (PM_TimeBeforeEqual(lock->expires, ticks)) {
		PM_DBG("wakelock %s expired\n", lock->name);
		sys_slist_find_and_remove(&pm_wakelock, &(lock->_node));
		pm_sched_unlock();
		return;
	}

	if (lock->expires == PM_WAIT_FOREVER)
		lock->expires++;

	if (!added) {
		PM_DBG("wakelock %s append, timeout=%d\n", lock->name, lock->expires);
		sys_slist_append(&pm_wakelock, &lock->_node);
	}
	pm_sched_unlock();

	pm_wake_lock_update();
}

bool pm_policy_wake_lock_is_active(void)
{
	return !sys_slist_is_empty(&pm_wakelock);
}

int pm_policy_wake_lock_init(void)
{
	pm_wakelock_timer = pm_timer_create(pm_wake_timer_cb, PM_WAIT_FOREVER);
	pm_timer_start(pm_wakelock_timer);

	return 0;
}
#endif