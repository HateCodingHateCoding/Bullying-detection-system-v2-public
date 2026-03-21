/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pm.h"
#include "device.h"
#include "hal_dev.h"

#if (CONFIG_PM && CONFIG_PM_DEVICE_RUNTIME)
int pm_device_runtime_enable(struct pm_device *dev)
{
	int ret = 0;
	HAL_Dev_t *hal_dev;

	if (dev == NULL) {
		return -1;
	}

	if (dev->state == PM_DEVICE_STATE_ACTIVE) {
		hal_dev = HAL_CONTAINER_OF(dev, HAL_Dev_t, pmDev);
		ret = dev->action(hal_dev, PM_DEVICE_ACTION_SUSPEND);
		if (ret < 0) {
			return ret;
		}
		dev->state = PM_DEVICE_STATE_SUSPENDED;
	}
	dev->flags |= (1 << PM_DEVICE_FLAG_RUNTIME_ENABLED);

	return ret;
}

int pm_device_runtime_disable(struct pm_device *dev)
{
	int ret = 0;
	HAL_Dev_t *hal_dev;

	if (dev == NULL) {
		return -1;
	}

	/* wake up the device if suspended */
	if (dev->state == PM_DEVICE_STATE_SUSPENDED) {
		hal_dev = HAL_CONTAINER_OF(dev, HAL_Dev_t, pmDev);
		ret = dev->action(hal_dev, PM_DEVICE_ACTION_RESUME);
		if (ret < 0) {
			return ret;
		}
		dev->state = PM_DEVICE_STATE_ACTIVE;
	}
	dev->flags &= ~(1 << PM_DEVICE_FLAG_RUNTIME_ENABLED);

	return ret;
}

int pm_device_runtime_get(struct pm_device *dev)
{
	int ret = 0;
	HAL_Dev_t *hal_dev;

	if (dev == NULL) {
		return -1;
	}

	if (!(dev->flags & (1 << PM_DEVICE_FLAG_RUNTIME_ENABLED)))
		return -1;

	if (dev->state == PM_DEVICE_STATE_ACTIVE) {
		return -1;
	}

	/* suspend now */
	hal_dev = HAL_CONTAINER_OF(dev, HAL_Dev_t, pmDev);
	ret = dev->action(hal_dev, PM_DEVICE_ACTION_RESUME);
	if (ret < 0) {
		return ret;
	}
	dev->state = PM_DEVICE_STATE_ACTIVE;

	return ret;

}

int pm_device_runtime_put(struct pm_device *dev)
{
	int ret = 0;
	HAL_Dev_t *hal_dev;

	if (dev == NULL) {
		return -1;
	}

	if (!(dev->flags & (1 << PM_DEVICE_FLAG_RUNTIME_ENABLED)))
		return -1;

	if (dev->state == PM_DEVICE_STATE_SUSPENDED) {
		return -1;
	}

	/* suspend now */
	hal_dev = HAL_CONTAINER_OF(dev, HAL_Dev_t, pmDev);
	ret = dev->action(hal_dev, PM_DEVICE_ACTION_SUSPEND);
	if (ret < 0) {
		return ret;
	}
	dev->state = PM_DEVICE_STATE_SUSPENDED;

	return ret;
}

bool pm_device_runtime_is_enabled(struct pm_device *dev)
{
	return dev && (dev->flags & (1 << PM_DEVICE_FLAG_RUNTIME_ENABLED));
}
#endif
