/**
 **************************************************************************************************
 * @brief   project config define.
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
 **************************************************************************************************
 */
 
#ifndef __FT_CTRL_H__
#define __FT_CTRL_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include "hal_gpio.h"
#include "irq.h"
#include "hal_efuse.h"
#include "hal_wkio.h"
#include "ll_gpadc.h"
#include "math.h"
#include "mmw_app_pointcloud.h"
#include "factory_test_config.h"


#define FT_GPIO_DIR_INPUT  	(0x00)
#define FT_GPIO_DIR_OUTPUT	(0x01)

typedef struct __packed {
	int16_t range;     /**< 1LSB = 1cm */
	int16_t azim_deg;      /**< azimuth Q7 degree */
	int16_t elev_deg;      /**< elevation Q7 degree */
} PointCloud_rept;



int ft_GPIO_PIN_init(uint8_t pin, uint8_t GPIO_Dir, uint8_t inputmode, HAL_Dev_t *gpioDev, GPIO_Pin_t *GPIO_PIN);

uint32_t ft_ctrl_adc_read_mv(void);

HAL_Dev_t *ft_trigger_init(int *ret);

int ft_trigger_deinit(OSI_Semaphore_t *sem, HAL_Dev_t *wkioDev);

uint16_t ft_judge_trigger_detection(PointCloud_rept *ptr_report, PointCloudBuffer_t *ptr_3d_pc, uint16_t force_trigger_cm, uint16_t pc_num, uint16_t pc_num_threshold);

int ft_led_set(HAL_Dev_t *wkioDev, WKIO_PinState_t pinState);

void ft_trigger_point_cloud_report(void *ptr_data, uint16_t len);

int ft_power_io_wakeup(uint8_t pin);

void ft_power_timer_wakeup(uint32_t ms);

#ifdef __cplusplus
}
#endif
#endif  