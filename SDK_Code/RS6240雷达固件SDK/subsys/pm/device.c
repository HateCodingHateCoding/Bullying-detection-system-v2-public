/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "config.h"

#if (CONFIG_PM)

#include <stddef.h>
#include "include/device.h"
#include "hal_dev.h"
#include "hal_types.h"

#if 0
static const enum pm_device_state action_target_state[] = {
	[PM_DEVICE_ACTION_SUSPEND] = PM_DEVICE_STATE_SUSPENDED,
	[PM_DEVICE_ACTION_RESUME] = PM_DEVICE_STATE_ACTIVE,
};
static const enum pm_device_state action_expected_state[] = {
	[PM_DEVICE_ACTION_SUSPEND] = PM_DEVICE_STATE_ACTIVE,
	[PM_DEVICE_ACTION_RESUME] = PM_DEVICE_STATE_SUSPENDED,
};
#endif

int pm_device_action_run(struct pm_device *pmDev, pm_device_action_cb_t action_cb,
			 enum pm_device_action action)
{
	int ret = 0;
	HAL_Dev_t *dev;

	if (pmDev == NULL) {
		return -1;
	}

#if 0
	/* Validate action against current state */
	if (dev->state == action_target_state[action]) {
		printk("%d\n", __LINE__);
		return -1;
	}
	if (dev->state != action_expected_state[action]) {
		printk("%d\n", __LINE__);
		return -1;
	}
#endif

	if (action_cb == NULL)
		return 0;
	dev = HAL_CONTAINER_OF(pmDev, HAL_Dev_t, pmDev);
//    printf("%s", dev->name);
	ret = action_cb((void *)dev, action);
	//if (ret < 0) {
	//	return ret;
	//}

	//dev->state = action_target_state[action];

	return ret;
}

#endif


