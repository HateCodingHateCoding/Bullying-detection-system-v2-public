/**
 ******************************************************************************
 * @file    pm_shell.c
 * @brief   pm shell command.
 * @verbatim    null
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "config.h"

#if (CONFIG_PM && CONFIG_PM_SHELL)

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "FreeRTOS.h"
#include "task.h"
#include "board_config.h"

#include "shell.h"
#include "shell_cmd_group.h"

#include "osi_common.h"
#include "osi_thread.h"
#include "pm.h"
#include "policy.h"

#define LOG_LEVEL    (3)

#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
static void pm_entry(enum pm_state state);
static void pm_exit(enum pm_state state);

/* Private defines.
 * ----------------------------------------------------------------------------
 */
 
 /* Private macros.
 * ----------------------------------------------------------------------------
 */
#define PM_THREAD_PRIO          (3)
#define PM_THREAD_STACK_SIZE    (2 * 1024)
#define PM_DEV_REG_NUM_MAX      (3)

/* Constants.
 * ------------------------------------------------------------------------------------------------
 */
 
 /* Private variables.
 * ----------------------------------------------------------------------------
 */
static OSI_Thread_t pm_thread;
static uint32_t pm_idle_ms = 1000;
static uint8_t pm_run = 0;
static struct wakelock pm_lock;
static struct wakelock pm_lock_to;
static struct pm_notifier notifier = {
	.state_entry = pm_entry,
	.state_exit = pm_exit,
};
static HAL_Dev_t _pm_dev[PM_DEV_REG_NUM_MAX];

#if CONFIG_PM_DEVICE_RUNTIME
#include "device_runtime.h"
#include "hal_uart.h"

const static UART_InitParam_t UartParam = {
	.baudRate = 115200,
	.parity = UART_PARITY_NONE,
	.stopBits = UART_STOP_BIT_1,
	.dataBits = UART_DATA_WIDTH_8,
	.autoFlowCtrl = UART_FLOW_CTRL_NONE
};
static HAL_Dev_t *uart_dev;
#endif


/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_PM_DEVICE_RUNTIME

static int uart_runtime_init(uint8_t id)
{
	if (uart_dev) {
		return 0;
	}
	uart_dev = HAL_UART_Init(id, &UartParam);
	if (uart_dev == NULL) {
		return -1;
	}
	HAL_UART_Open(uart_dev);
	HAL_UART_SetTransferMode(uart_dev, UART_DIR_TX, UART_TRANS_MODE_NOMA);

	return pm_device_runtime_enable(&uart_dev->pmDev);
}

static int uart_runtime_deinit(uint8_t id)
{
	int ret = 0;
	if (!uart_dev) {
		return 0;
	}

	HAL_UART_Close(uart_dev);
	pm_device_runtime_disable(&uart_dev->pmDev);

	ret = HAL_UART_DeInit(uart_dev);
	uart_dev = NULL;

	return ret;
}

static int uart_runtime_write(uint8_t *data, uint16_t length)
{
	if (uart_dev == NULL)
		return -1;

	pm_device_runtime_get(&uart_dev->pmDev);
	HAL_UART_Transmit(uart_dev, data, length, 100);
	pm_device_runtime_put(&uart_dev->pmDev);

	return 0;
}
#endif

static void pm_entry(enum pm_state state)
{
	LOG_INF("PM suspend enter, %d", state);
}

static void pm_exit(enum pm_state state)
{
	LOG_INF("PM suspend exit, %d", state);
}

static void pm_thread_entry(void *arg)
{
	uint32_t i = 0;

	while (pm_run) {
		LOG_INF("pm test[%d]\n", i++);
		OSI_MSleep(pm_idle_ms);
	}

	OSI_ThreadDelete(&pm_thread);
}

static int cmd_pm_init(Shell *shell, int argc, char *argv[])
{
	LOG_DBG("pm init\n");

	if (pm_run) {
		LOG_WRN("pm init already\n");
		return 0;
	}
	pm_run = 1;
	OSI_Status_t ret = OSI_ThreadCreate(&pm_thread,
                        "pm_thread",
                        pm_thread_entry,
                        NULL,
                        PM_THREAD_PRIO,
                        PM_THREAD_STACK_SIZE / 4);
	if (ret != OSI_STATUS_OK) {
		LOG_ERR("pm thread create fail, %d\n", ret);
		return -1;
	}

	return ret;
}

static int cmd_pm_deinit(Shell *shell, int argc, char *argv[])
{
	pm_run = 0;
	return 0;
}

static int cmd_pm_idle(Shell *shell, int argc, char *argv[])
{
	uint32_t idle_ms;

	if (argc == 1) {
		LOG_INF("PM idle time: %dms\n", pm_idle_ms);
		return 0;
	} else if (argc == 2) {
		idle_ms = atoi(argv[1]);
		LOG_INF("PM idle time change: from %dms to %dms\n", pm_idle_ms, idle_ms);
		pm_idle_ms = idle_ms;
		return 0;
	} else {
		LOG_ERR("Invalid arg\n");
		return -1;
	}
}

static int cmd_pm_wakelock(Shell *shell, int argc, char *argv[])
{
	uint32_t en;
	if (argc == 2) {
		en = atoi(argv[1]);
		if (en) {
			pm_policy_wake_lock(&pm_lock);
			LOG_INF("PM wakelock get, %d\n", pm_lock.ref);
		} else {
			pm_policy_wake_unlock(&pm_lock);
			LOG_INF("PM wakelock put, %d\n", pm_lock.ref);
		}
		return 0;
	} else {
		LOG_ERR("Invalid arg\n");
		return -1;
	}
}

static int cmd_pm_wakelock_timeout(Shell *shell, int argc, char *argv[])
{
	uint32_t timeout_ms;
	if (argc == 2) {
		timeout_ms = atoi(argv[1]);
		LOG_INF("PM wakelock timeout: %dms\n", timeout_ms);
		pm_wake_lock_timeout(&pm_lock_to, timeout_ms);
		return 0;
	} else {
		LOG_ERR("Invalid arg\n");
		return -1;
	}
}

static int cmd_pm_notify(Shell *shell, int argc, char *argv[])
{
	uint32_t en;
	if (argc == 2) {
		en = atoi(argv[1]);
		if (en) {
			LOG_INF("PM notify register\n");
			pm_notifier_register(&notifier);
		} else {
			LOG_INF("PM notify unregister\n");
			pm_notifier_unregister(&notifier);
		}
		return 0;
	} else {
		LOG_ERR("Invalid arg\n");
		return -1;
	}
}

static HAL_Dev_t *pm_dev_find(const char *name)
{
	HAL_Dev_t *dev;
	for (int i = 0; i < PM_DEV_REG_NUM_MAX; i++) {
		dev = &_pm_dev[i];
		if (!strncmp(dev->name, name, strlen(name)))
			return dev;
	}
	return NULL;
}

static HAL_Dev_t *pm_dev_get(const char *name, uint8_t name_len)
{
	HAL_Dev_t *dev;
	for (int i = 0; i < PM_DEV_REG_NUM_MAX; i++) {
		dev = &_pm_dev[i];
		if (dev && !dev->state) {
			dev->state = 1;
			memcpy((char *)&dev->name, name, name_len);
			return dev;
		}
	}
	return NULL;
}

static void pm_dev_put(HAL_Dev_t *dev)
{
	HAL_Dev_t *tdev;
	for (int i = 0; i < PM_DEV_REG_NUM_MAX; i++) {
		tdev = &_pm_dev[i];
		if (tdev->state && !strncmp((char *)&tdev->name, dev->name, strlen(dev->name))) {
			tdev->state = 0;
			memset(&tdev->name, 0, sizeof(tdev->name));
		}
	}
}

static int pm_device_action_cb(void *dev, enum pm_device_action action)
{
	HAL_Dev_t *tdev = (HAL_Dev_t *)dev;

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:
		LOG_INF("[Dev] %s suspend\n", tdev->name);
		break;
	case PM_DEVICE_ACTION_RESUME:
		LOG_INF("[Dev] %s resume\n", tdev->name);
		break;
	default:
		break;
	}
	return 0;
}

static int pm_device_early_action_cb(void *dev, enum pm_device_action action)
{
#if CONFIG_PM_DEVICE_RUNTIME

	char name[CONFIG_DEV_NAME_SIZE + 25] = {0};
	HAL_Dev_t *tdev = (HAL_Dev_t *)dev;

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:
		snprintf(name, sizeof(name), "[Dev] %s early suspend\n", tdev->name);
		uart_runtime_write((uint8_t *)name, sizeof(name));
		break;
	case PM_DEVICE_ACTION_RESUME:
		snprintf(name, sizeof(name), "[Dev] %s early resume\n", tdev->name);
		uart_runtime_write((uint8_t *)name, sizeof(name));
		break;
	default:
		break;
	}
#endif
    return 0;
}

static int cmd_pm_register(Shell *shell, int argc, char *argv[])
{
	uint8_t mul = 0;
	const char *dev_name;
	HAL_Dev_t *dev;

	if (argc >= 2 && argc <= 3) {
		dev_name = argv[1];

		if (strlen(dev_name) > CONFIG_DEV_NAME_SIZE) {
			LOG_ERR("device name length should not longer than %d\n", CONFIG_DEV_NAME_SIZE);
			return -1;
		}
		if (pm_dev_find(dev_name)) {
			LOG_WRN("device %s registered already\n", dev_name);
			return 0;
		}
		dev = pm_dev_get(dev_name, strlen(dev_name) + 1);
		if (dev == NULL) {
			LOG_ERR("No device resource\n");
			return -1;
		}
		if (argc == 3) {
			if (strncmp(argv[2], "mul", 3)) {
				LOG_ERR("Invalid arg %s\n", argv[2]);
				return -1;
			}
			mul = 1;
		}
		return pm_device_mult_register(&dev->pmDev, pm_device_action_cb,
              mul ? pm_device_early_action_cb : NULL);
	} else {
		LOG_ERR("Invalid arg\n");
		return -1;
	}
	return 0;
}

static int cmd_pm_unregister(Shell *shell, int argc, char *argv[])
{
	int ret;
	const char *dev_name;
	HAL_Dev_t *dev;

	if (argc != 2) {
		LOG_ERR("Invalid arg\n");
		return -1;
	}

	dev_name = argv[1];
	dev = pm_dev_find(dev_name);
	if (!dev) {
		LOG_WRN("device %s has not registered\n", dev_name);
		return 0;
	}
	ret = pm_device_unregister(&dev->pmDev);
	pm_dev_put(dev);
	return ret;
}

#if CONFIG_PM_DEVICE_RUNTIME
static int cmd_pm_runtime(Shell *shell, int argc, char *argv[])
{
	if (argc != 3) {
		LOG_ERR("Invalid arg\n");
		return -1;
	}

	if (strncmp(argv[1], "uart1", 4)) {
		LOG_ERR("Invalid arg %s\n", argv[1]);
		return -1;
	}
	if (atoi(argv[2])) {
		LOG_INF("PM device runtime enable\n");
		return uart_runtime_init(1);
	} else {
		LOG_INF("PM device runtime disable\n");
		return uart_runtime_deinit(1);
	}
}
#endif

ShellCommand pmGroup[] = {
    SHELL_CMD_ITEM(init, cmd_pm_init, "init"),
    SHELL_CMD_ITEM(deinit, cmd_pm_deinit, "deinit"),
    SHELL_CMD_ITEM(idle, cmd_pm_idle, "idle [idle_ms]"),
    SHELL_CMD_ITEM(wakelock, cmd_pm_wakelock, "wakelock [en]"),
    SHELL_CMD_ITEM(wakelock_to, cmd_pm_wakelock_timeout, "wakelock_to [timeout_ms]"),
    SHELL_CMD_ITEM(notify, cmd_pm_notify, "notify [en]"),
    SHELL_CMD_ITEM(register, cmd_pm_register, "register <device_name> [mul]"),
    SHELL_CMD_ITEM(unregister, cmd_pm_unregister, "unregister <device_name>"),
#if CONFIG_PM_DEVICE_RUNTIME
    SHELL_CMD_ITEM(runtime, cmd_pm_runtime, "runtime <transport> <en>"),
#endif
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(pm, pmGroup, "PM shell commands");


#endif /* CONFIG_PM_SHELL */
