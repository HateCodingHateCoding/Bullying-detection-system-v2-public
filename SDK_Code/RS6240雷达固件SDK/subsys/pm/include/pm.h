/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PM_H
#define _PM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//#include "sys/slist.h"
#include "slist.h"
#include "port.h"
#include "policy.h"
#include "device.h"
#include "config.h"

enum pm_state {
	PM_STATE_ACTIVE,
	PM_STATE_IDLE,
	PM_STATE_STANDBY,
	PM_STATE_SHUTDOWN,
};

struct pm_notifier {
	sys_snode_t _node;
	void (*state_entry)(enum pm_state state);
	void (*state_exit)(enum pm_state state);
};

#if (CONFIG_PM)
/* Following two API, user should not call */
int pm_system_resume(void);
int pm_system_suspend(enum pm_state state, uint8_t substate_id);

/**
 * @brief Register a power management device.
 *
 * Register the given device from the power management device
 * list.
 *
 * @param dev pm_device object to be registered.
 * @param action pm_device_action_cb_t callback to be registered.
 * @return 0 if the device was successfully registered, 1 if the 
 *         device registered already, else error.
 */
int pm_device_register(struct pm_device *dev, pm_device_action_cb_t action);

/**
 * @brief Register a power management device with multipe callback.
 *
 * Register the given device from the power management device
 * list.
 *
 * @param dev pm_device object to be registered.
 * @param action pm_device_action_cb_t callback to be registered.
 * @param early_action pm_device_action_cb_t callback to be registered.
 *        note: If callback return 1, that system would re-suspend directly,
          which mean the normal level device would not be resumed.
          For geneic suspend-resume process, callback should return 0.
 * @return 0 if the device was successfully registered, 1 if the
 *         device registered already, else error.
 */
int pm_device_mult_register(struct pm_device *dev, pm_device_action_cb_t action,
              pm_device_action_cb_t early_action);


/**
 * @brief Unregister a power management device.
 *
 * Unregister the given device from the power management device
 * list.
 *
 * @param dev pm_device object to be unregistered.
 * @return 0 if the device was successfully unregistered, else error.
 */
int pm_device_unregister(struct pm_device *dev);

/**
 * @brief Register a power management notifier.
 *
 * Register the given notifier from the power management notification
 * list.
 *
 * @param notifier pm_notifier object to be registered.
 */
void pm_notifier_register(struct pm_notifier *notifier);

/**
 * @brief Unregister a power management notifier.
 *
 * Remove the given notifier from the power management notification
 * list. After that this object callbacks will not be called.
 *
 * @param notifier pm_notifier object to be unregistered.
 *
 * @return 0 if the notifier was successfully removed, a negative value
 * otherwise.
 */
int pm_notifier_unregister(struct pm_notifier *notifier);

/**
 * @brief Update the power management lantency.
 *
 * Different latency would take effect about pm policy.
 *
 * @param entry_latency worst case latency in microseconds required to exit the active state.
 * @param entry_latency worst case latency in microseconds required to exit the idle state.
 *
 */
void pm_policy_latency_update(uint32_t entry_latency, uint32_t exit_latency);

#else
static inline int pm_system_resume(void)
{
	return 0;
}

static inline int pm_system_suspend(enum pm_state state, uint8_t substate_id)
{
	(void)state;
	(void)substate_id;
	return 0;
}

static inline int pm_device_register(struct pm_device *dev, pm_device_action_cb_t action)
{
	(void)dev;
	(void)action;
	return 0;
}

static inline int pm_device_mult_register(struct pm_device *dev, pm_device_action_cb_t action,
              pm_device_action_cb_t early_action)
{
	(void)dev;
	(void)action;
	(void)early_action;
	return 0;
}

static inline int pm_device_unregister(struct pm_device *dev)
{
	(void)dev;
	return 0;
}

static inline void pm_notifier_register(struct pm_notifier *notifier)
{
	(void)notifier;
}

static inline int pm_notifier_unregister(struct pm_notifier *notifier)
{
	(void)notifier;
	return 0;
}

static inline void pm_policy_latency_update(uint32_t entry_latency, uint32_t exit_latency)
{
	(void)entry_latency;
	(void)exit_latency;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _PM_H */

