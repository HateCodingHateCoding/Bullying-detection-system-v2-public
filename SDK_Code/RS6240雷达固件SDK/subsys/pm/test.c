/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if 0
#include <stdio.h>

#include "pm.h"

extern uint32_t msleep(uint32_t ms);

int pm_device_action(struct pm_device *dev,
				     enum pm_device_action action);
int pm_device_early_action(struct pm_device *dev,
				     enum pm_device_action action);

struct pm_device device_a = {
	.name = "device_a",
	.flags = 0,
	.state = PM_DEVICE_STATE_ACTIVE,
	.action = pm_device_action,
	.early_action = pm_device_early_action,
};

struct pm_device device_b = {
	.name = "device_b",
	.flags = 0,
	.state = PM_DEVICE_STATE_ACTIVE,
	.action = pm_device_action,
	.early_action = pm_device_early_action,
};

struct pm_device device_c = {
	.name = "device_c",
	.flags = 0,
	.state = PM_DEVICE_STATE_ACTIVE,
	.action = pm_device_action,
	.early_action = pm_device_early_action,
};

int pm_device_action(struct pm_device *dev,
				     enum pm_device_action action)
{
	printf("%s %s\n", dev->name, action == PM_DEVICE_ACTION_SUSPEND ? "suspend" : "resume");
	return 0;
}

static uint32_t test_flag = 0;
int pm_device_early_action(struct pm_device *dev,
				     enum pm_device_action action)
{
	printf("%s %s\n", dev->name, action == PM_DEVICE_ACTION_SUSPEND ? "early suspend" : "early resume");
	if (dev == &device_b) {
		test_flag++;
		if (test_flag >= 100 && test_flag < 150) {
			if (test_flag == 100)
			msleep(5000);
			return 1;
		} else if (test_flag >= 150) {
			test_flag = 10;
			return 1;
		}
	}
	return 0;
}



void test_pm_run(void)
{
	pm_device_register(&device_a);
	pm_device_register(&device_b);
	pm_device_register(&device_c);

	printf("device_a:%p device_b:%p device_c:%p\n", &device_a, &device_b, &device_c);
	printf("start suspend\n");
	while (1) {
		pm_system_suspend(PM_STATE_STANDBY, 0);

		msleep(50);
	}
}
#endif
