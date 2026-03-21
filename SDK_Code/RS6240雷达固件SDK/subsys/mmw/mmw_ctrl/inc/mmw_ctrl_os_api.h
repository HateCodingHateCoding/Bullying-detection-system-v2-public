/**
 *   @file  mmw_ctrl_os_api.h
 *
 *   @brief
 *      Header file of APIs for rtos.
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2023 Possumic, Inc.
 *
 */
#ifndef MMW_CTRL_OS_API_H
#define MMW_CTRL_OS_API_H
#include <stdarg.h>

#include <config.h>
#include <soc.h>
#include "board.h"
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_clock.h"
#include "version.h"

enum mmw_os_prio_level {
	 MMW_OS_PRIO_IDLE,
	 MMW_OS_PRIO_LOWEST,
	 MMW_OS_PRIO_LOW,
	 MMW_OS_PRIO_NORMAL_BELOW,
	 MMW_OS_PRIO_NORMAL,
	 MMW_OS_PRIO_NORMAL_ABOVE,
	 MMW_OS_PRIO_HIGH,
	 MMW_OS_PRIO_HIGHEST,
	 MMW_OS_PRIO_COOPERATIVE,
};

enum mmw_os_pm_state {
	MMW_OS_PM_STATE_ACTIVE,
	MMW_OS_PM_STATE_RUNTIME_IDLE,
	MMW_OS_PM_STATE_SUSPEND_TO_IDLE,
	MMW_OS_PM_STATE_STANDBY,
	MMW_OS_PM_STATE_SUSPEND_TO_RAM,
	MMW_OS_PM_STATE_SUSPEND_TO_DISK,
	MMW_OS_PM_STATE_SOFT_OFF
};

typedef void (*thread_entry_t)(void *param);
typedef void (*timer_expiry_t)(void *timer);
typedef void (*irq_handler_t)(void);

typedef void *mmw_thread;

struct mmw_radar_os_iface {
    mmw_thread * (*thread_create)(const char *name,
                           char *stack,
                           uint32_t stack_size,
                           thread_entry_t entry,
                           void *p1, void *p2, void *p3,
                           int prio, uint32_t options);
    int (*thread_delete)(mmw_thread *thread);

    int (*sem_init)(void **hand, uint32_t init_cnt, uint32_t limit);
    int (*sem_take)(void *hand, uint32_t time_out);
    int (*sem_gave)(void *hand);
    int (*sem_deinit)(void *hand);

    int (*mutex_init)(void **hand);
    int (*mutex_lock)(void *hand, uint32_t time_out);
    int (*mutex_unlock)(void *hand);
    int (*mutex_deinit)(void *hand);

    int (*usleep)(uint32_t us);
    int (*msleep)(uint32_t ms);
    void (*busy_wait)(uint32_t us);

#if (CONFIG_PM) && (CONFIG_PM_DEVICE)
    void (*pm_constraint_set)(enum pm_state state);
    void (*pm_constraint_release)(enum pm_state state);
#else
    void *pm_constraint_set;
    void *pm_constraint_release;
#endif

    int (*timer_init)(void **hand, timer_expiry_t expiry_fn);
    int (*timer_start)(void *hand, uint32_t us);
    void (*timer_stop)(void *hand);
    int (*timer_deinit)(void *hand);

    uint32_t (*get_sys_clock_cycle)(void);

    void *(*heap_init)(void *mem, uint32_t bytes);
    void *(*heap_malloc)(void *heap, uint32_t bytes);
    void (*heap_free)(void *heap, void *mem);

    void *(*malloc)(uint32_t size);
    void (*free)(void *p);

    char *(*get_kernel_version)(void);

    void (*print)(const char *str, ...);

    int (*power_state_force)(enum mmw_os_pm_state pm_state);

    int (*irq_connect)(uint32_t irq, uint32_t priority, void (*routine)(void *param));

    int (*vsnprintf)(char *str, uint32_t len,
                     const char *format, va_list vargs);

    int (*is_sched_locked)(void);

    void *efuse_ctrl_handle;

	void (*loop_task_time_reset)(void *arg);
};
void mmw_dev_os_api_init(void);
uint32_t msleep(uint32_t ms);

#endif /* MMW_CTRL_OS_API_H */


