/*
 * **Copyright (c) 2021 - 2023, POSSUMIC Technology Co., Ltd**
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of POSSUMIC Technology Co. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */
#include <stddef.h>
#include <stdio.h>


#include "version.h"
#include "irq.h"
#include "hal_efuse.h"
#include "pm.h"
#include "mmw_ctrl_os_api.h"
#include "mmw_ctrl.h"

#ifndef CONFIG_MMW_FW_SRAM_SWITCH_TO_CPU
#define CONFIG_MMW_FW_SRAM_SWITCH_TO_CPU                    0  //  unsupport in freeRTOS now.
#endif

#ifndef CONFIG_MMW_FW_SRAM_SWITCH_TO_CPU_SIZE
#define CONFIG_MMW_FW_SRAM_SWITCH_TO_CPU_SIZE               0x10000
#endif

#if CONFIG_MMW_FW_SRAM_SWITCH_TO_CPU
#error "sram swith is unsupport in freeRTOS now."
#endif

mmw_thread *thread_create(const char *name, char *stack,
                        uint32_t stack_size,
                        thread_entry_t entry,
                        void *param, void *p2, void *p3,
                        int prio, uint32_t options)
{
    TaskHandle_t thread_new = NULL;
	if (pdPASS != xTaskCreate((TaskFunction_t)entry, name, stack_size/4, param, prio, &thread_new)) {
		return NULL;
	}
    return (mmw_thread *)thread_new;
}

int thread_delete(mmw_thread *thread_handle)
{
	vTaskDelete((TaskHandle_t)thread_handle);//(TaskHandle_t *)
	return 0;
}

int sem_init(void **hand, uint32_t init_cnt, uint32_t limit)
{
	int err = -1;
	OSI_Semaphore_t *new_sem = OSI_Malloc(sizeof(OSI_Semaphore_t));
	if (new_sem != NULL) {
		err = OSI_SemaphoreCreate(new_sem, init_cnt, limit);
		if (err) {
			OSI_Free(new_sem);
			new_sem = NULL;
		}
	}
	*hand = (void *)new_sem;
	return err;
}

int sem_take(void *hand, uint32_t time_out)
{
	return OSI_SemaphoreWait((OSI_Semaphore_t *)hand, (OSI_Time_t)time_out);
}

int sem_gave(void *hand)
{
	return OSI_SemaphoreRelease((OSI_Semaphore_t *)hand);
}

int sem_deinit(void *hand)
{
	int err = -1;
	if (hand != NULL) {
		err = OSI_SemaphoreDelete((OSI_Semaphore_t *)hand);
		OSI_Free(hand);
	}
	return err;
}

int mutex_init(void **hand)
{
	int err = -1;
	OSI_Mutex_t *new_mutex = OSI_Malloc(sizeof(OSI_Mutex_t));
	if (new_mutex != NULL) {
		err = OSI_MutexCreate(new_mutex);
		if (err) {
			OSI_Free(new_mutex);
			new_mutex = NULL;
		}
	}
	*hand = (void *)new_mutex;
	return err;
}

int mutex_lock(void *hand, uint32_t time_out)
{
	return OSI_MutexLock((OSI_Mutex_t *)hand, (OSI_Time_t)time_out);
}

int mutex_unlock(void *hand)
{
	return OSI_MutexUnlock((OSI_Mutex_t *)hand);
}

int mutex_deinit(void *hand)
{
	int err = -1;
	if (hand != NULL) {
		err = OSI_MutexDelete((OSI_Mutex_t *)hand);
		OSI_Free(hand);
	}
	return err;
}

int timer_init(void **hand, timer_expiry_t expiry_fn)
{
	int err = -1;
	OSI_Timer_t *new_timer = OSI_Malloc(sizeof(OSI_Timer_t));
	if (new_timer != NULL) {
        new_timer->handle = OSI_INVALID_HANDLE;
		err = OSI_TimerCreate(new_timer, OSI_TIMER_ONCE, expiry_fn, NULL, 1);
		if (err) {
			OSI_Free(new_timer);
			new_timer = NULL;
		}
	}
	*hand = (void *)new_timer;
	return err;
}

int timer_start(void *hand, uint32_t us)
{
	int err = OSI_TimerChangePeriod((OSI_Timer_t *)hand, us/1000);
	if (err == OSI_STATUS_OK) {
		err = OSI_TimerStart((OSI_Timer_t *)hand);
	}
	return err;
}

int timer_deinit(void *hand)
{
	int err = -1;
	if (hand != NULL) {
		err = OSI_TimerDelete((OSI_Timer_t *)hand);
		OSI_Free(hand);
	}
	return err;
}

uint32_t get_sys_clock_cycle(void)
{
    return csi_coret_get_value(); //sys_clock_cycle_get_32();
}

char *get_kernel_version(void)
{
    return 0; //VERSION_V_MAJOR;
}

#if (CONFIG_PM)
int power_state_force(enum mmw_os_pm_state state)
{
	//struct pm_state_info pm_state_tmp = {0};

	//pm_state_tmp.state = (enum pm_state)state;

	//pm_state_force(0, &pm_state_tmp, 1);

    return 0;
}
#endif

int irq_connect(uint32_t irq, uint32_t priority, void (*routine) (void *param))
{
    //irq_connect_dynamic(irq, priority, (void (*)(const void *))routine, 0, 0);
    IRQ_Disable(irq);
    IRQ_Attach(irq, (IRQ_Handler_t)routine);
	//IRQ_AttachDevice(irq, (HAL_Dev_t *)temp_device);
	IRQ_Priority(irq, priority);
    IRQ_Enable(irq);
    return 0;
}

#if (CONFIG_PM) && (CONFIG_PM_DEVICE)
struct wakelock mmw_pm_lock;

void pm_constraint_set(enum pm_state state)
{
	pm_policy_wake_lock(&mmw_pm_lock);
}

void pm_constraint_release(enum pm_state state)
{
	pm_policy_wake_unlock(&mmw_pm_lock);
}
#endif

#if (CONFIG_MMW_LOOP_TASK_MONITOR)
OSI_Timer_t mmw_timer;

void mmw_loop_task_exception(void *arg)
{
	/* reset system and bb */
	csi_core_reset();
}

void mmw_loop_task_time_reset(void *arg)
{
	OSI_TimerStart(&mmw_timer);
}

void mmw_loop_task_monitor_init(void)
{
    OSI_TimerCreate(&mmw_timer, OSI_TIMER_ONCE, &mmw_loop_task_exception, NULL, CONFIG_MMW_LOOP_TASK_TIMEOUT_MS);

	mmw_loop_task_time_reset(NULL);
}

void mmw_loop_task_time_stop(void)
{
	OSI_TimerStop(&mmw_timer);
}

#endif

#if (CONFIG_MMW_FW_SRAM_SWITCH_TO_CPU)
static struct k_heap mmw_heap;
void *mmw_heap_init(void *mem, uint32_t bytes)
{
	k_heap_init(&mmw_heap, mem, bytes);

	return (void *)&mmw_heap;
}

void *mmw_heap_malloc(void *heap, size_t bytes)
{
	void *mem;
	struct k_heap **heap_ref;
	size_t __align;

	__align = sizeof(void *) | sizeof(heap_ref);

	mem = k_heap_aligned_alloc(heap, __align, bytes, K_NO_WAIT);
	if (mem == NULL) {
		return NULL;
	}

	heap_ref = mem;
	*heap_ref = heap;
	mem = ++heap_ref;

	return mem;
}
#endif


void mmw_dev_os_api_init(void)
{
    struct mmw_radar_os_iface mmw_os_api;

    mmw_os_api.thread_create = thread_create;
    mmw_os_api.thread_delete = thread_delete;

    mmw_os_api.sem_init = sem_init;
    mmw_os_api.sem_take = sem_take;
    mmw_os_api.sem_gave = sem_gave;
    mmw_os_api.sem_deinit = sem_deinit;

    mmw_os_api.mutex_init = mutex_init;
    mmw_os_api.mutex_lock = mutex_lock;
    mmw_os_api.mutex_unlock = mutex_unlock;
    mmw_os_api.mutex_deinit = mutex_deinit;

    mmw_os_api.usleep = NULL;
    mmw_os_api.msleep = (int (*)(uint32_t))msleep;
    mmw_os_api.busy_wait = NULL;

#if (CONFIG_PM) && (CONFIG_PM_DEVICE)
    mmw_os_api.pm_constraint_set = pm_constraint_set;
    mmw_os_api.pm_constraint_release = pm_constraint_release;
#else
    mmw_os_api.pm_constraint_set = NULL;
    mmw_os_api.pm_constraint_release = NULL;
#endif

    mmw_os_api.timer_init = timer_init;
    mmw_os_api.timer_start = timer_start;
    mmw_os_api.timer_stop = (void (*)(void *))OSI_TimerStop;
    mmw_os_api.timer_deinit = timer_deinit;

	mmw_os_api.get_sys_clock_cycle = get_sys_clock_cycle;

#if (CONFIG_MMW_FW_SRAM_SWITCH_TO_CPU)
    mmw_os_api.heap_init = (void *(*)(void *, uint32_t))mmw_heap_init;
    mmw_os_api.heap_free = (void (*)(void *, void *))k_heap_free;
    mmw_os_api.heap_malloc = (void *(*)(void *, uint32_t))mmw_heap_malloc;
#else
	mmw_os_api.heap_init = (void *(*)(void *, uint32_t))NULL;
    mmw_os_api.heap_free = (void (*)(void *, void *))NULL;
    mmw_os_api.heap_malloc = (void *(*)(void *, uint32_t))NULL;
#endif

    mmw_os_api.malloc = (void *(*)(uint32_t))OSI_Malloc;
    mmw_os_api.free = (void (*)(void *))OSI_Free;

    mmw_os_api.get_kernel_version = get_kernel_version;

    mmw_os_api.print = (void (*)(const char *, ...))printf;

    mmw_os_api.irq_connect = irq_connect;

    mmw_os_api.vsnprintf = vsnprintf;

#if (CONFIG_PM)
    mmw_os_api.power_state_force = NULL;//power_state_force;
#else
	mmw_os_api.power_state_force = NULL;
#endif

	mmw_os_api.is_sched_locked = NULL; //k_is_sched_locked;

#if (CONFIG_DRIVER_EFUSE)
    mmw_os_api.efuse_ctrl_handle = HAL_EFUSE_get_ctrl_handle();
#endif

#if (CONFIG_MMW_LOOP_TASK_MONITOR)
    mmw_os_api.loop_task_time_reset = mmw_loop_task_time_reset;
#else
    mmw_os_api.loop_task_time_reset = NULL;
#endif

    mmw_os_regist(&mmw_os_api);
}

