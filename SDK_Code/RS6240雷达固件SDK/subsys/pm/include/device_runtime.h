/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DEVICE_RUNTIME_H
#define _DEVICE_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <device.h>
#include "pm.h"

#if (CONFIG_PM && CONFIG_PM_DEVICE_RUNTIME)
/**
 * @brief Enable device runtime PM
 *
 * This function will enable runtime PM on the given device. If the device is
 * in #PM_DEVICE_STATE_ACTIVE state, the device will be suspended.

 *
 * @param dev Device instance.
 *
 * @retval 0 If the device runtime PM is enabled successfully.
 * @retval -errno Other negative errno, result of suspending the device.
 */
int pm_device_runtime_enable(struct pm_device *dev);

/**
 * @brief Disable device runtime PM
 *
 * If the device is currently suspended it will be resumed.
 *
 * @param dev Device instance.
 *
 * @retval 0 If the device runtime PM is disabled successfully.
 * @retval -errno Other negative errno, result of resuming the device.
 */
int pm_device_runtime_disable(struct pm_device *dev);

/**
 * @brief Resume a device based on usage count.
 *
 * This function will resume the device if the device is suspended (usage count
 * equal to 0). In case of a resume failure, usage count and device state will
 * be left unchanged. In all other cases, usage count will be incremented.
 *
 * @param dev Device instance.
 *
 * @retval 0 If it succeeds. In case device runtime PM is not enabled or not
 * available this function will be a no-op and will also return 0.
 * @retval -errno Other negative errno, result of the PM action callback.
 */
int pm_device_runtime_get(struct pm_device *dev);

/**
 * @brief Suspend a device based on usage count.
 *
 * This function will suspend the device if the device is no longer required
 * (usage count equal to 0). In case of suspend failure, usage count and device
 * state will be left unchanged. In all other cases, usage count will be
 * decremented (down to 0).
 *
 * @param dev Device instance.
 *
 * @retval 0 If it succeeds. In case device runtime PM is not enabled or not
 * available this function will be a no-op and will also return 0.
 * @retval -errno Other negative errno, result of the action callback.
 */
int pm_device_runtime_put(struct pm_device *dev);

/**
 * @brief Check if device runtime is enabled for a given device.
 *
 * @param dev Device instance.
 *
 * @retval true If device has device runtime PM enabled.
 * @retval false If the device has device runtime PM disabled.
 *
 * @see pm_device_runtime_enable()
 */
bool pm_device_runtime_is_enabled(struct pm_device *dev);

#else
static inline int pm_device_runtime_enable(struct pm_device *dev)
{
	(void)dev;
	return 0;
}

static inline int pm_device_runtime_disable(struct pm_device *dev)
{
	(void)dev;
	return 0;
}

static inline int pm_device_runtime_get(struct pm_device *dev)
{
	(void)dev;
	return 0;
}

static inline int pm_device_runtime_put(struct pm_device *dev)
{
	(void)dev;
	return 0;
}

static inline bool pm_device_runtime_is_enabled(struct pm_device *dev)
{
	(void)dev;
	return false;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DEVICE_RUNTIME_H */

