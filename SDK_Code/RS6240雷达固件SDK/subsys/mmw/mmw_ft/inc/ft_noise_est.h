/**
 **************************************************************************************************
 * @brief   factory test noise estmation config define.
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
 
#ifndef __FT_NOISE_EST_H__
#define __FT_NOISE_EST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include "mmw_type.h"
#include "mmw_ctrl.h"
#include "factory_test_config.h"
#include "mmw_alg_pointcloud_typedef.h"
#include "ft_type.h"

enum {
	NOISE_PEAK_PWR_GAP_ID = 0,
	NOISE_SNR_GAP_ID,
	NOISE_MIN_GAP_ID,
	NOISE_AVGE_GAP_ID,
	NOISE_MAX_GAP_ID,
	NOISE_GAP_ID_NUM
};

enum {
	MMW_NOISE_RX_ANT_REPORT_EVT = 0,
	MMW_NOISE_FINAL_REPORT_EVT,
	MMW_NOISE_UNKNOW_REPORT_EVT
};

enum {
	PEAK_NOISE_VAILD = 0,
	PEAK_INVAILD,
	NOISE_INVALID
};


struct mmw_meas_pos_t {
	uint16_t rbin_max_pwr_idx;
	float  rbin_max_pwr;
};

void ft_noise_est_param_set(struct mmw_nosie_est_recive_param_t *param);

int mmw_noise_factor_set(uint16_t mimo_rx_ant_id, struct mmw_noise_factor_t *factor, struct radar_noise_est_t *mradar_est_data);

int mmw_noise_est_range_set(uint16_t range_min, uint16_t range_max, struct mmw_noise_usr_t *usr_param);

void mmw_noise_hw_init_set(uint32_t id);

int mmw_noise_meas_scope_set(uint16_t start_index, uint16_t end_index);

int mmw_noise_assess_gap_param_config(uint8_t gap_id, int16_t gap_val);

int mmw_noise_est_start(struct mmw_noise_est_frame_t *pFrame, struct radar_noise_est_t *mradar_est_data);

int mmw_noise_est_wait_done(struct radar_noise_est_t *mradar_est_data);

int ft_noise_est_init(struct mmw_noise_est_frame_t *pFrame);

void ft_noise_est_deinit(struct radar_noise_est_t *mradar_est_data);

float noise_est_buf_update(Complexf32_RealImag *pBuf_in, uint16_t dop_num);

static void noise_rbin_buf_update(Complexf32_RealImag *pBuf, uint8_t rx_idx, uint16_t rbin_num, uint16_t rbin_off, struct radar_noise_est_t *pdata);

int mmw_radar_find_rbin_postion(Complexf32_RealImag *cube, uint16_t start_offset, uint16_t rbin_num, uint8_t rx_ant_num, struct mmw_meas_pos_t *pos);

static bool mmw_radio_noise_est_pass(uint16_t rx_ant_id, float *avge_noise, struct radar_noise_est_t *mradar_est_data, struct mmw_noise_report_t *report);

void mmw_noise_est_report_data(int event, struct mmw_noise_report_t *report, struct mmw_noise_est_final_report_t *est_report);



#ifdef __cplusplus
}
#endif
#endif  