/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     power_manager.c
 * @brief    source file for power manager.
 * @version  V1.0
 * @date     18. July 2018
 ******************************************************************************/
#include <stdint.h>
#include <soc.h>
#include <io.h>
#include <drv_common.h>
#include <power_manager.h>

int32_t drv_get_boot_type(void)
{
    if (*(volatile uint32_t *)CSKY_PMU_BASE & 0x20) {
        /* boot from low power mode */
        return 1;
    }

    /* normal boot */
    return 0;
}

int32_t drv_clock_manager_config(clock_manager_e device, uint8_t enable)
{
    if (enable) {
        if (device > 31) {
            set_bit(device - 32, (uint32_t *)CLOCK_CONTROL_REGISTER1);
        } else {
            set_bit(device, (uint32_t *)CLOCK_CONTROL_REGISTER0);
        }
    } else {
        if (device > 31) {
            clear_bit(device - 32, (uint32_t *)CLOCK_CONTROL_REGISTER1);
        } else {
            clear_bit(device, (uint32_t *)CLOCK_CONTROL_REGISTER0);
        }
    }

    return 0;
}

void registers_save(uint32_t *mem, uint32_t *addr, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        mem[i] = *((volatile uint32_t *)addr + i);
    }
}

void registers_restore(uint32_t *addr, uint32_t *mem, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        *((volatile uint32_t *)addr + i) = mem[i];
    }

}

typedef struct {
    uint8_t power_status;
} power_ctrl_t;

int32_t drv_soc_power_control(void *handle, csi_power_stat_e state, power_cb_t *cb)
{
    power_ctrl_t *ctrl = handle;

    switch (state) {
        case DRV_POWER_FULL:
            if (ctrl->power_status == DRV_POWER_SUSPEND && cb->wakeup) {
                cb->wakeup(handle);
            } else if (ctrl->power_status != DRV_POWER_SUSPEND && cb->manage_clock) {
                cb->manage_clock(handle, 1);
            }

            break;

        case DRV_POWER_OFF:
            if (cb->manage_clock) {
                cb->manage_clock(handle, 0);
            }

            break;

        case DRV_POWER_SUSPEND:
            if (ctrl->power_status != DRV_POWER_SUSPEND && cb->sleep) {
                cb->sleep(handle);
            }

            break;

        case DRV_POWER_LOW:
            return (DRV_ERROR_UNSUPPORTED);

        default:
            return (DRV_ERROR_PARAMETER);
    }

    ctrl->power_status = state;

    return 0;
}
