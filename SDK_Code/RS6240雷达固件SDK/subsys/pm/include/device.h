/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DEVICE_H
#define _DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stddef.h>
//#include "sys/slist.h"
#include "slist.h"

enum pm_register_level {
	PM_REG_LEVEL_EARLY = 0,
	PM_REG_LEVEL_NORMAL,
	PM_REG_LEVEL_MAX,
};

enum pm_device_flag {
	/** Indicates if device runtime is enabled  */
	PM_DEVICE_FLAG_RUNTIME_ENABLED,
};

enum pm_device_state {
	PM_DEVICE_STATE_ACTIVE,
	PM_DEVICE_STATE_SUSPENDED,
};

/** @brief Device PM actions. */
enum pm_device_action {
	/** Suspend. */
	PM_DEVICE_ACTION_SUSPEND,
	/** Resume. */
	PM_DEVICE_ACTION_RESUME,
	/**
	 * Turn off
	 * @note
	 *      Action triggered only by a power domain.
	 */
	PM_DEVICE_ACTION_TURN_OFF,
	/**
	 * Turn on
	 * @note
	 *      Action triggered only by a power domain.
	 */
	PM_DEVICE_ACTION_TURN_ON,
};

typedef int (*pm_device_action_cb_t)(void *dev,
				     enum pm_device_action action);

struct pm_device {
	sys_snode_t _node[PM_REG_LEVEL_MAX];
	const char *name;
	enum pm_device_flag flags;
	enum pm_device_state state;
	/**
	 * The pm aciton callback for the normal level device.
	 */
	pm_device_action_cb_t action;
	/**
	 * The pm aciton callback for the early level devices.
	 * Two action level supported by pm: normal and early. When pm enter suspend,
	 * it whould suspend the normal level devices firstly and then early level.
	 * When system resume, the early level devices resume firstly, if the action return 1
	 * that system would not continue wake up the remaning devices and suspend previous
	 * devices to enter idle state again, if the action return 0, system would wake all
	 * early level devices and the the normal level devices. It means failure when action
	 * return a negative value.
	 */
	pm_device_action_cb_t early_action;
};

int pm_device_action_run(struct pm_device *pmDev, pm_device_action_cb_t action_cb,
			 enum pm_device_action action);


#ifdef __cplusplus
}
#endif

#endif /* _DEVICE_H */

