/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _POLICY_H
#define _POLICY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "slist.h"
#include "pm.h"

#define PM_ALL_SUBSTATES (UINT8_MAX)

struct wakelock {
	sys_snode_t _node; /* For internale use, user should not operate */
	char *name; /* A wake lock name */
	uint16_t ref; /* For internale use, user should not operate */
	uint32_t expires; /* For internale use, user should not operate */
};

#if (CONFIG_PM)
/**
 * @brief PM wake lock initialize.
 *
 * It would be called automatically when enale CONFIG_PM, which means the user
 * should not call.
 *
 * @param none.
 *
 * @return 0 if success, else failure.
 */
int pm_policy_wake_lock_init(void);


/**
 * @brief Check if a wake lock active.
 * If any wake lock active, the system would not enter suspend.
 * @param none.
 *
 * @return ture if active, else false.
 */
bool pm_policy_wake_lock_is_active(void);

/**
 * @brief Get a pm wake lock.
 *
 * Used in conjunction with pm_policy_wake_unlock. If a wake lock getted,
 * the system would never enter idle state, until to put it by call
 * pm_policy_wake_unlock.
 *
 * @param lock wakelock object to get a wake lock.
 *
 * @return none.
 */
void pm_policy_wake_lock(struct wakelock *lock);

/**
 * @brief Put a pm wake lock.
 *
 * Used in conjunction with pm_policy_wake_lock.
 *
 * @param lock wakelock object to put a wake lock.
 *
 * @return none.
 */
void pm_policy_wake_unlock(struct wakelock *lock);


/**
 * @brief Get a pm timeout wake lock.
 *
 * The wake lock would be released when the timeout expired.
 *
 * @param lock wakelock object to get a timeout wake lock.
 * @param timeout time expired in microsecond to release the wake lock.
 *
 * @return none.
 */
void pm_wake_lock_timeout(struct wakelock *lock, uint32_t timeout);
#else
static inline int pm_policy_wake_lock_init(void)
{
	return 0;
}

static inline bool pm_policy_wake_lock_is_active(void)
{
	return false;
}

static inline void pm_policy_wake_lock(struct wakelock *lock)
{
	(void)lock;
}

static inline void pm_policy_wake_unlock(struct wakelock *lock)
{
	(void)lock;
}

static inline void pm_wake_lock_timeout(struct wakelock *lock, uint32_t timeout)
{
	(void)lock;
	(void)timeout;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _POLICY_H */

