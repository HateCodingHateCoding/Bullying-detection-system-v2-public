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

#include "soc.h"
#include "hal_dev.h"
#include "mmw_ctrl.h"
#include "mmw.h"

typedef int (*mmw_dev_on)(const HAL_Dev_t *dev);
typedef int (*mmw_dev_off)(const HAL_Dev_t *dev);

static mmw_wake_check_cb_t mmw_wake_check_cb = NULL;

extern bool g_mmw_init;
#if (CONFIG_PM && CONFIG_PM_DEVICE)
#if (CONFIG_MMW_DRIVER_PM_WAKE_EVENT_MODE == 1)

#define WAKEUP_EVENT_NOT_HANDLE         0x1
#define WAKEUP_EVENT_TRIG_POST_PROC     0x2

#if (CONFIG_MMW_XIP_RUN_DEBUG)
void mmw_xip_debug_run(uint32_t status)
{
	uint32_t print_count = 0;
	uint8_t *p = (uint8_t *)CONFIG_XIP_RUN_BUF_ADDR;
	//printk("addr %x num %d status %d\n", CONFIG_XIP_RUN_BUF_ADDR, CONFIG_XIP_RUN_BUF_NUM, status);
	for (int i = 0; i < CONFIG_XIP_RUN_BUF_NUM; i++) {
		if (p[i] && print_count < 20) {
			print_count++;
			printk("func[%d] run %d times in resume duration\n", i, p[i]);
		}
	}
	printk("count %d\n", print_count);
	memset(p, 0, CONFIG_XIP_RUN_BUF_NUM);
}
#endif

int mmw_wake_event_callback(void *dev,
				     enum pm_device_action action)
{
//	HW_TRACE_IO(TRACING_IO_IDLE, 3, 1, 0);

	int ret = 0;
	uint32_t status = 0;

	if ((mmw_wake_check_cb == NULL)
		|| (mmw_wake_check_cb(0) == 0)) {
		if (CLIC->CLICINT[FFT2D_IRQn].IP) {
			extern uint32_t mmw_app_ultra_low_power_process(uint8_t level);
			status = mmw_app_ultra_low_power_process(1);

			if ((status & WAKEUP_EVENT_NOT_HANDLE) == 0) {
				extern uint32_t frame_ctrl_clear_frame_int_flag(uint32_t int_mask);
				frame_ctrl_clear_frame_int_flag(2);
//				HW_TRACE_IO(TRACING_IO_IDLE, 0, 1, 5);
			}
#if (CONFIG_MMW_XIP_RUN_DEBUG)
			mmw_xip_debug_run(status);
#endif
			if (status == 0) {
				ret = 1;
				mmw_suspend();
			}
		}
	}

//	HW_TRACE_IO(TRACING_IO_IDLE, 3, 0, 0);
	return ret;
}

//struct pm_policy_wake_event mmw_policy_wake_event;

#endif

extern void mmw_frcnt_irq_handler(void);

int mmw_frcnt_wake_event_cb(void *dev, enum pm_device_action action)
{
	if (CLIC->CLICINT[FRCNT_IRQn].IP) {
        mmw_frcnt_irq_handler();
		return -1;
	} else {
		return 0;
	}
}

int mmw_dev_pm_action_early(void *dev, enum pm_device_action action)
{
	int ret = 0;

	if (action != PM_DEVICE_ACTION_RESUME)
		return 0;

	if (CLIC->CLICINT[FRCNT_IRQn].IP) {
        mmw_frcnt_irq_handler();
		ret = 1;
	}

#if (CONFIG_MMW_DRIVER_PM_WAKE_EVENT_MODE == 1) && CONFIG_PM
	int status;
	if ((mmw_wake_check_cb == NULL)
		|| (mmw_wake_check_cb(0) == 0)) {
		if (CLIC->CLICINT[FFT2D_IRQn].IP) {
			extern uint32_t mmw_app_ultra_low_power_process(uint8_t level);
			status = mmw_app_ultra_low_power_process(1);

			if ((status & WAKEUP_EVENT_NOT_HANDLE) == 0) {
				extern uint32_t frame_ctrl_clear_frame_int_flag(uint32_t int_mask);
				frame_ctrl_clear_frame_int_flag(2);
//				HW_TRACE_IO(TRACING_IO_IDLE, 0, 1, 5);
			}
#if (CONFIG_MMW_XIP_RUN_DEBUG)
			mmw_xip_debug_run(status);
#endif
			if (status == 0) {
				ret = 1;
				mmw_suspend();
			}
		}
	}
#endif

	return ret;
}

//struct pm_policy_wake_event mmw_frcnt_wake_event;
//#define MMW_NODE DT_NODELABEL(mmw)
//#define MMW_IRQ_PRIORITY DT_IRQ(MMW_NODE, priority)

int mmw_dev_pm_action(void *dev, enum pm_device_action action)
{
    extern bool g_mmw_init;

	if (g_mmw_init == false) {
		return 0;
	}

	switch (action) {
	case PM_DEVICE_ACTION_RESUME:
	case PM_DEVICE_ACTION_TURN_ON:
			CLIC->CLICINT[FFT2D_IRQn].IE |= CLIC_INTIE_IE_Msk;
			mmw_resume(1);
			//extern 	void z_riscv_irq_priority_set(unsigned int irq, unsigned int prio, uint32_t flags);
			//z_riscv_irq_priority_set(FFT2D_IRQn, MMW_IRQ_PRIORITY, 0);
		break;
	case PM_DEVICE_ACTION_SUSPEND:
	case PM_DEVICE_ACTION_TURN_OFF:
		mmw_suspend();
		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}

#endif

int mmw_dev_init(HAL_Dev_t *dev)
{
	void mmw_dev_os_api_init(void);

	mmw_dev_os_api_init();

#if (CONFIG_PM && CONFIG_PM_DEVICE)
	pm_device_mult_register(&dev->pmDev, mmw_dev_pm_action,
	                                          mmw_dev_pm_action_early);
#endif

	return 0;
}

void mmw_pm_wake_check_regist(mmw_wake_check_cb_t wake_check_cb)
{
	mmw_wake_check_cb = wake_check_cb;
}
