/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "types.h"
#include "ll_utils.h"
#include "mmw_ctrl.h"
#include "mmw_low_power.h"


extern bool g_mmw_data_event;
extern uint32_t mmw_app_low_power_process(void);
extern uint32_t mmw_app_ultra_low_power_process(uint8_t level);

static inline bool mmw_get_fft_wake_up_flag(void)
{
	//return (LL_PMU_GetWupSrcStatus(LL_WUP_SRC_2DFFT_INT_POS) &&
	//	NVIC_GetPendingIRQ(MMW_FFT2D_WUP_IRQn) && arch_irq_is_enabled(MMW_FFT2D_WUP_IRQn));
	return false;
}

#if (CONFIG_PM_DEVICE_RUNTIME_LAZY_MODE_SUPPORT)
bool mmw_data_ready_wakeuped_callback(const struct device *dev)
{
	return LL_PMU_GetWupSrcStatus(LL_WUP_SRC_2DFFT_INT_POS);
}
#endif

#if (CONFIG_PM_DEVICE_QUICK_FALLBACK_MODE)
bool mmw_proc_handle_callback(const struct device *dev)
{
	tracing_io_set(TRACING_IO_PM, 0, 1, 2);

	if (!mmw_get_fft_wake_up_flag() || pm_get_sys_pm_policy() == SYS_PM_DISABLE) {
		return true;
	}

	bool flag = mmw_app_low_power_process();

	if (flag) {
		g_mmw_data_event = false;
		tracing_io_set(TRACING_IO_PM, 0, 1, 3);
	} else {
		tracing_io_set(TRACING_IO_PM, 0, 1, 4);
	}
	return !flag;
}
#endif

#if (CONFIG_PM_WAKEUP_EVENT)
bool is_wakeup_from_mmw_event(uint8_t wakeup_id)
{
	return mmw_get_fft_wake_up_flag();
}

bool mmw_wakeup_event(void)
{
	tracing_io_set(TRACING_IO_PM, 0, 1, 1);

	if (pm_get_sys_pm_policy() == SYS_PM_DISABLE) {
		return true;
	}

	void psic_clock_resume(void);
	void psic_clock_suspend(void);

	psic_clock_resume();
	bool flag = mmw_app_ultra_low_power_process(1);
	psic_clock_suspend();

	if (flag) {
		g_mmw_data_event = false;
		tracing_io_set(TRACING_IO_PM, 0, 1, 3);
	} else {
		tracing_io_set(TRACING_IO_PM, 0, 1, 4);
	}

	return !flag;
}
#endif

void mmw_low_power_init(void)
{
#if (CONFIG_PM_DEVICE_RUNTIME_LAZY_MODE_SUPPORT) && (CONFIG_PM_DEVICE_QUICK_FALLBACK_MODE)
	const struct device *mmw_dev;
	mmw_dev = device_get_binding(DT_LABEL(DT_NODELABEL(mmw)));
	pm_lazy_device_enable(mmw_dev, mmw_data_ready_wakeuped_callback);
	pm_device_set_wakeup_handle(mmw_dev, mmw_proc_handle_callback);
#endif

#if (CONFIG_PM_WAKEUP_EVENT)
	pm_regist_wakeup_event(MMW_FFT2D_WUP_IRQn, is_wakeup_from_mmw_event, mmw_wakeup_event);
#endif
}
