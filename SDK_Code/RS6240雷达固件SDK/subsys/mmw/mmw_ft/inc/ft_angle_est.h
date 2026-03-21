/**
 **************************************************************************************************
 * @brief   factory test angle calib and angle calculate config define.
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
 
#ifndef __FT_ANGLE_EST_H__
#define __FT_ANGLE_EST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include "mmw_type.h"
#include "mmw_ctrl.h"
#include "factory_test_config.h"
#include "mmw_alg_pointcloud_typedef.h"
#include "ft_type.h"

#if CONFIG_SOC_SERIES_RS624X
#define FT_ANGLE_EST_CONFIG_MIMO_MODE 		(MMW_MIMO_2T4R)
#elif CONFIG_SOC_RS6130
#define FT_ANGLE_EST_CONFIG_MIMO_MODE 		(MMW_MIMO_1T3R)
#endif
#define FT_ANGLE_EST_CONFIG_WORK_MODE		(MMW_WORK_MODE_2DFFT)
#define FT_ANGLE_EST_CONFIG_START_FREQ 		(FT_DEFAULT_START_FREQ)

#define FT_ANGLE_EST_CONFIG_VELOC 			(1600)
#define FT_ANGLE_EST_CONFIG_VELOC_RES 		(200)

#define FT_ANGELE_CALCULATE_MODE 	(0x06)
#define FT_ANGELE_CALIB_MODE 		(0x07)

#define GET_ROUND_UP(x, y) (((x) + ((y) >> 1)) / (y))
#define GET_ROUND_DOWN(x, y) (((x) - ((y) >> 1)) / (y))


void ft_angle_est_param_set(struct mmw_angle_est_recive_param_t *param);

int mmw_angle_est_start(struct mmw_angle_est_frame_t *pFrame, uint8_t angle_est_mode);

void ft_angle_est_finish(struct radar_angle_est_t *pdata,struct mmw_angle_est_report_t * report);

void ft_angele_est_report(struct radar_angle_est_t *pdata,struct mmw_angle_est_report_t * report);

int mmw_angle_est_wait_done(struct radar_angle_est_t *angle_est_data);

int mmw_angle_save_calib_data(void);

int mmw_angle_save_calib_data_custom(uint8_t *calib_data);

int mmw_angle_get_calib_data(uint32_t *calib_data);

int mmw_angle_calib_iq_data_get(uint8_t *iq_data, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif  