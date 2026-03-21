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
#include "prj_config.h"
#if (CONFIG_MMW_DRIVER_TEMP_MGMT)
#include "soc.h"
#include "ll_utils.h"
#include "ll_pmu.h"
#include "hal_clock_calib.h"

#include "mmw_ctrl.h"
#include "mmw.h"

#include "mmw_temp_mgmt_api.h"

strTempMgmtInfo g_temp_mgmt_info;

#define TEMP_TRIG_CALIB_MAX_TIME    10
int temp_mgmt_calib_process(int16_t cur_temp)
{
    uint32_t calib_mask = 0;
    int status = 0;

    if (cur_temp == INVALID_TEMP_VALUE) {
        cur_temp = temp_mgmt_get_cur_temp();
    }

    if (g_temp_mgmt_info.calib_mask == 0) {
        return 0;
    }

#if (CONFIG_CLK_LSI_CALIBRATION || CONFIG_CLK_MSI_CALIBRATION)
    uint8_t clk_pwr_sys_id = LL_SYS_GetCurrentSysId();

    calib_mask = ((g_temp_mgmt_info.calib_mask >> 16) & 0x3);

    if (calib_mask > 0) {
#if (MMW_TEMP_MGMT_DEBUG)
        printk("b lsi:%d,msi:%d\n", psic_clk_get(PSIC_CLK_BUS_LSI), psic_clk_get(PSIC_CLK_BUS_MSI));
#endif

        psic_triger_clk_calib_process(clk_pwr_sys_id, calib_mask);

#if (CONFIG_CLK_LSI_CALIBRATION)
        uint32_t lsi_freq = psic_clk_get(PSIC_CLK_BUS_LSI);
        mmw_clock_cfg(CLK_TYPE_LSI, lsi_freq);
#endif

#if (CONFIG_CLK_MSI_CALIBRATION)
        uint32_t msi_freq = psic_clk_get(PSIC_CLK_BUS_MSI);
        mmw_clock_cfg(CLK_TYPE_MSI, msi_freq);
#endif

#if (MMW_TEMP_MGMT_DEBUG)
        printk("n lsi:%d,msi:%d\n", psic_clk_get(PSIC_CLK_BUS_LSI), psic_clk_get(PSIC_CLK_BUS_MSI));
#endif
    }
#endif

    calib_mask = (g_temp_mgmt_info.calib_mask & 0xFFFF);
    if (calib_mask > 0) {
        int idx;
        for (idx = 0; idx < TEMP_TRIG_CALIB_MAX_TIME; idx++) {
            if (temp_mgmt_triger_calib(calib_mask) == 0) {
                break;
            }
        }
        if (idx >= TEMP_TRIG_CALIB_MAX_TIME) {
            csi_core_reset();
        }
    }

    temp_mgmt_set_calib_temp(cur_temp);

    return status;
}

int host_ctrl_temp_triger_event_cb(void *arg)
{
    strTempMeasInfo meas_info = {0};

    temp_mgmt_get_temp_meas_info(&meas_info);

    if (meas_info.event_status != TEMP_EVENT_NORM) {
        if (g_temp_mgmt_info.event_process != NULL) {
            g_temp_mgmt_info.event_process(&(meas_info.cur_temp));
        }
    }
#if (MMW_TEMP_MGMT_DEBUG) && !(CONFIG_MMW_DRIVER_PM_WAKE_EVENT_MODE)
    printk("cur_temp:%d\n", meas_info.cur_temp);
#endif
    return 0;
}

int host_det_temp_triger_event_cb(void *arg)
{
    int16_t cur_temp = tsensor_read_temp();

    int16_t delta_temp = cur_temp - temp_mgmt_get_calib_temp();

    int16_t temp_thr = (int16_t)temp_mgmt_get_triger_thr();

    temp_mgmt_set_cur_temp(cur_temp);

    if ((delta_temp > (int16_t)temp_thr) || (delta_temp < -((int16_t)temp_thr))) {
        if (g_temp_mgmt_info.event_process != NULL) {
            g_temp_mgmt_info.event_process(&cur_temp);
        }
    }

#if (MMW_TEMP_MGMT_DEBUG) && !(CONFIG_MMW_DRIVER_PM_WAKE_EVENT_MODE)
    printk("cur_temp1:%d\n", cur_temp);
#endif

    return 0;
}

int mmw_temp_mgmt_mode_cfg(uint8_t temp_mgmt_mode)
{
    if (temp_mgmt_mode > TEMP_MGMT_HOST_DETECTION) {
        g_temp_mgmt_info.event_process = NULL;
        return -1;
    }

    temp_mgmt_mode_cfg(temp_mgmt_mode);

    mmw_temp_triger_event_cb event_callback = host_ctrl_temp_triger_event_cb;

    if (temp_mgmt_mode == TEMP_MGMT_HOST_DETECTION) {
        event_callback = host_det_temp_triger_event_cb;
    }

    temp_mgmt_triger_event_callback(event_callback);

    return 0;
}

void mmw_temp_mgmt_init(uint8_t temp_mgmt_mode, uint32_t frame_num_per_meas, uint16_t temp_thr, uint32_t calib_mask,
                             mmw_temp_triger_event_cb event_process)
{
    int ret = 0;

    ret = mmw_temp_mgmt_mode_cfg(temp_mgmt_mode);
    if (ret) {
        return;
    }

    temp_mgmt_set_detect_period(frame_num_per_meas);

    temp_mgmt_triger_thr_cfg(temp_thr);

    g_temp_mgmt_info.calib_mask = calib_mask;
    g_temp_mgmt_info.event_process = event_process;
    g_temp_mgmt_info.triger_calib_event = false;

    OSI_SemaphoreCreate(&g_temp_mgmt_info.app_proc_sem, 1, 50);
}

void mmw_temp_mgmt_app_process_init(mmw_app_process_start app_start, mmw_app_process_stop app_stop)
{
    g_temp_mgmt_info.app_start = app_start;
    g_temp_mgmt_info.app_stop = app_stop;
}

#endif //#if (CONFIG_MMW_DRIVER_TEMP_MGMT)