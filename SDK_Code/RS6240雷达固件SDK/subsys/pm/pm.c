/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal_types.h"

#include "port.h"
#include "pm.h"
#include "device.h"
#include "device_runtime.h"

#include "log.h"

#if (CONFIG_PM)
static sys_slist_t pm_notifiers = SYS_SLIST_STATIC_INIT(&pm_notifiers);
static sys_slist_t pm_suspend[PM_REG_LEVEL_MAX] = {
    [0 ... (PM_REG_LEVEL_MAX - 1)] = SYS_SLIST_STATIC_INIT(NULL)
};
static sys_slist_t pm_register[PM_REG_LEVEL_MAX] = {
    [0 ... (PM_REG_LEVEL_MAX - 1)] = SYS_SLIST_STATIC_INIT(NULL)
};

static struct pm_ctx {
    enum pm_state state;
    uint32_t entry_latency;
    uint32_t exit_latency;
} pm_ins_ctx = {
    .entry_latency = PM_MsecsTOTicks(10),
    .exit_latency = PM_MsecsTOTicks(1),
};

static struct pm_ctx *pm_intance_get(void)
{
    return &pm_ins_ctx;
}

static inline void pm_device_remove(sys_slist_t *list_src, sys_slist_t *list_dst, sys_snode_t *node)
{
    sys_slist_remove(list_src, NULL, node);
    sys_slist_prepend(list_dst, node);
}

static inline bool pm_device_find(sys_slist_t *list, sys_snode_t *node)
{
    sys_snode_t *tmp;

    SYS_SLIST_FOR_EACH_NODE(list, tmp) {
        if (tmp == node) {
            return true;
        }
    }
    return false;
}

static inline void pm_state_notify(bool entering_state)
{
    struct pm_notifier *notifier;
    pm_spinlock_t pm_notifier_key;
    void (*callback)(enum pm_state state);

    struct pm_ctx *pm = pm_intance_get();

    pm_notifier_key = pm_spin_lock();
    SYS_SLIST_FOR_EACH_CONTAINER(&pm_notifiers, notifier, _node) {
        if (entering_state) {
            callback = notifier->state_entry;
        } else {
            callback = notifier->state_exit;
        }

        if (callback) {
            callback(pm->state);
        }
    }
    pm_spin_unlock(pm_notifier_key);
}

static int pm_suspend_devices(void)
{
    int ret;
    pm_spinlock_t key;
    struct pm_device *dev, *tmp;

    sys_slist_t *list = &pm_register[PM_REG_LEVEL_NORMAL];
    sys_slist_t *sus_list = &pm_suspend[PM_REG_LEVEL_NORMAL];

    LOG_IO(LOG_IO_TAG_PM, 0, 0, 1);

    key = pm_spin_lock();

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(list, dev, tmp, _node[PM_REG_LEVEL_NORMAL]) {
        if (dev) {
            pm_device_remove(list, sus_list, &dev->_node[PM_REG_LEVEL_NORMAL]);
            if (pm_device_runtime_is_enabled(dev)) {
                continue;
            }

            LOG_IO(LOG_IO_TAG_PM, 1, 1, 0);

            ret = pm_device_action_run((void *)dev, dev->action, PM_DEVICE_ACTION_SUSPEND);

            LOG_IO(LOG_IO_TAG_PM, 1, 0, 0);

            if (ret < 0) {
                pm_spin_unlock(key);
                return ret;
            }
        }
    }
    LOG_IO(LOG_IO_TAG_PM, 0, 0, 2);

    list = &pm_register[PM_REG_LEVEL_EARLY];
    sus_list = &pm_suspend[PM_REG_LEVEL_EARLY];

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(list, dev, tmp, _node[PM_REG_LEVEL_EARLY]) {
        if (dev) {
            pm_device_remove(list, sus_list, &dev->_node[PM_REG_LEVEL_EARLY]);
            if (pm_device_runtime_is_enabled(dev)) {
                continue;
            }

            LOG_IO(LOG_IO_TAG_PM, 1, 1, 0);

            ret = pm_device_action_run((void *)dev, dev->early_action, PM_DEVICE_ACTION_SUSPEND);

            LOG_IO(LOG_IO_TAG_PM, 1, 0, 0);

            if (ret < 0) {
                pm_spin_unlock(key);
                return ret;
            }
        }
    }

    pm_spin_unlock(key);

    LOG_IO(LOG_IO_TAG_PM, 0, 0, 3);

    return 0;
}

/*
 * return: <0: resume fail, 0: resume success, >0: re-suspend quickly
 *
 */
static int pm_resume_devices(void)
{
    int ret = 0;
    struct pm_device *dev, *tmp;
    sys_slist_t *list = &pm_suspend[PM_REG_LEVEL_EARLY];
    sys_slist_t *reg_list = &pm_register[PM_REG_LEVEL_EARLY];

    LOG_IO(LOG_IO_TAG_PM, 0, 0, 4);

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(list, dev, tmp, _node[PM_REG_LEVEL_EARLY]) {
        if (dev) {
            pm_device_remove(list, reg_list, &dev->_node[PM_REG_LEVEL_EARLY]);
            if (pm_device_runtime_is_enabled(dev)) {
                continue;
            }
            LOG_IO(LOG_IO_TAG_PM, 1, 1, 0);
            ret = pm_device_action_run((void *)dev, dev->early_action, PM_DEVICE_ACTION_RESUME);
            LOG_IO(LOG_IO_TAG_PM, 1, 0, 0);
            if (ret > 0) { /* suspend return */
                goto suspend_dirtect;
            }
        }
    }

    LOG_IO(LOG_IO_TAG_PM, 0, 0, 5);
    list = &pm_suspend[PM_REG_LEVEL_NORMAL];
    reg_list = &pm_register[PM_REG_LEVEL_NORMAL];

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(list, dev, tmp, _node[PM_REG_LEVEL_NORMAL]) {
        if (dev) {
            pm_device_remove(list, reg_list, &dev->_node[PM_REG_LEVEL_NORMAL]);
            if (pm_device_runtime_is_enabled(dev)) {
                continue;
            }

            LOG_IO(LOG_IO_TAG_PM, 1, 1, 0);

            ret = pm_device_action_run((void *)dev, dev->action, PM_DEVICE_ACTION_RESUME);

            LOG_IO(LOG_IO_TAG_PM, 1, 0, 0);

            if (ret < 0) {
                ret--;
            }
        }
    }

    LOG_IO(LOG_IO_TAG_PM, 0, 0, 6);

    return ret;

suspend_dirtect:
    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(reg_list, dev, tmp, _node[PM_REG_LEVEL_EARLY]) {
        if (dev) {
            pm_device_remove(reg_list, list, &dev->_node[PM_REG_LEVEL_EARLY]);
            if (pm_device_runtime_is_enabled(dev)) {
                continue;
            }
            if (pm_device_action_run((void *)dev, dev->early_action, PM_DEVICE_ACTION_SUSPEND) < 0)
                ret = -1;
        }
    }

    LOG_IO(LOG_IO_TAG_PM, 0, 0, 7);

    return ret;
}

static int _pm_device_register(struct pm_device *dev)
{
    pm_spinlock_t key;
    sys_slist_t *list;

    key = pm_spin_lock();

    for (int i = 0; i < PM_REG_LEVEL_MAX; i++) {
        list = &pm_register[i];
        if (pm_device_find(list, &dev->_node[i])) {
            pm_spin_unlock(key);
            return 1;
        }
    }

    dev->flags = 0;
    dev->state = PM_DEVICE_STATE_ACTIVE;

    if (dev && dev->early_action) {
        list = &pm_register[PM_REG_LEVEL_EARLY];
        sys_slist_prepend(list, &dev->_node[PM_REG_LEVEL_EARLY]);
    }
    if (dev && dev->action) {
        list = &pm_register[PM_REG_LEVEL_NORMAL];
        sys_slist_prepend(list, &dev->_node[PM_REG_LEVEL_NORMAL]);
    }

    pm_spin_unlock(key);
    return 0;
}

int pm_device_register(struct pm_device *dev, pm_device_action_cb_t action)
{
    dev->action = action;
    dev->early_action = NULL;
    return _pm_device_register(dev);
}

int pm_device_mult_register(struct pm_device *dev, pm_device_action_cb_t action,
              pm_device_action_cb_t early_action)
{
    dev->action = action;
    dev->early_action = early_action;
    return _pm_device_register(dev);
}

int pm_device_unregister(struct pm_device *dev)
{
    int ret = -1;
    pm_spinlock_t key;
    sys_slist_t *list;

    key = pm_spin_lock();

    for (int i = 0; i < PM_REG_LEVEL_MAX; i++) {
        list = &pm_register[i];
        if (sys_slist_find_and_remove(list, &(dev->_node[i]))) {
            ret = 0;
        }
    }

    pm_spin_unlock(key);

    return ret;
}

void pm_notifier_register(struct pm_notifier *notifier)
{
    pm_spinlock_t key;
    sys_slist_t *list = &pm_notifiers;

    key = pm_spin_lock();

    if (pm_device_find(list, &notifier->_node)) {
        return;
    }
    sys_slist_append(list, &notifier->_node);

    pm_spin_unlock(key);
}

int pm_notifier_unregister(struct pm_notifier *notifier)
{
    int ret = -1;
    pm_spinlock_t key;
    sys_slist_t *list = &pm_notifiers;

    key = pm_spin_lock();

    if (sys_slist_find_and_remove(list, &(notifier->_node))) {
        ret = 0;
    }

    pm_spin_unlock(key);

    return ret;
}

void pm_policy_latency_update(uint32_t entry_latency, uint32_t exit_latency)
{
    struct pm_ctx *pm = pm_intance_get();
    pm->entry_latency = PM_MsecsTOTicks(entry_latency);
    pm->exit_latency = PM_MsecsTOTicks(exit_latency);
}

int pm_system_resume(void)
{
    struct pm_ctx *pm = pm_intance_get();

    pm_state_notify(false);

    pm->state = PM_STATE_ACTIVE;
    return 0;
}

int pm_system_suspend(enum pm_state state, uint8_t substate_id)
{
    struct pm_ctx *pm = pm_intance_get();

    LOG_IO(LOG_IO_TAG_PM, 4, 0, 1);

    if (state == PM_STATE_ACTIVE) {
        PM_DBG("PM state mismatch.\n");
        return -1;
    }

    if ((pm->entry_latency + pm->exit_latency) >
              pm_get_next_timeout_expiry()) {
        PM_DBG("PM latency mismatch.\n");
        return -1;
    }
    LOG_IO(LOG_IO_TAG_PM, 4, 0, 2);

    /* check wakelock */
    if (pm_policy_wake_lock_is_active()) {
        PM_DBG("PM wake lock active.\n");
        return -1;
    }
    LOG_IO(LOG_IO_TAG_PM, 4, 0, 3);

    pm_set_sys_clock_timeout(pm_get_next_timeout_expiry() - pm->exit_latency);

    pm->state = state;
    if (state != PM_STATE_IDLE) {
        if (pm_suspend_devices()) {
            pm_resume_devices();
            return -1;
        }
    }
    LOG_IO(LOG_IO_TAG_PM, 4, 0, 4);

    pm_sched_lock();

    pm_state_notify(true);

re_suspend:
    pm_state_set(state, substate_id);

    if (pm_resume_devices() > 0) {
        LOG_IO(LOG_IO_TAG_PM, 4, 0, 5);
        goto re_suspend;
    }

    LOG_IO(LOG_IO_TAG_PM, 4, 0, 6);
    pm_system_resume();

    pm_sched_unlock();

    LOG_IO(LOG_IO_TAG_PM, 4, 0, 7);

    return 0;
}
#endif
