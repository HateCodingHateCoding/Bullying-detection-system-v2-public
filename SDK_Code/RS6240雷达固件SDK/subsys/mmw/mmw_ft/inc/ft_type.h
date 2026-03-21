/**
 **************************************************************************************************
 * @brief   factory test data type define.
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
 
#ifndef __FT_TYPE_H__
#define __FT_TYPE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include "mmw_type.h"
#include "factory_test_config.h"
#include "mmw_alg_pointcloud_typedef.h"


/* ------- noise estimation struct define ------- */

/* radar performance typical value */
struct mmw_noise_factor_t {
	int16_t pwr_high_avge;			/* default: 4096 */
	int16_t pwr_low_avge;			/* default: 22530 */
	int16_t pwr_avge;				/* default: 54800 */
	int16_t peak_pwr_avge;			/* default: 22582 */
	int16_t peak_snr_avge;			/* default: 256 */
} __packed;

/* radar peformance gap value */
struct mmw_noise_gap_param_t {
	int16_t peak_pwr;
	int16_t rbin_gap_num;
	int16_t snr;
	int16_t min;
	int16_t avge;
	int16_t max;
} __packed;

/* nosie estimation receive param from FT tool */
struct mmw_nosie_est_recive_param_t {
	uint8_t  est_rx_msk;
	uint8_t  reserved;
	uint16_t est_rbin_start_id;
	uint16_t est_rbin_end_id;
	uint16_t range_mm;
	uint16_t range_resolut;
	uint32_t start_freq;
	uint32_t veloc_mm;
	uint16_t vel_resol;
	uint16_t frame_num;
	uint32_t  hw_init_id;
	struct mmw_noise_factor_t factor[CONFIG_MAX_RX_ANT];
	struct mmw_noise_gap_param_t  gap;
} __packed;

/* noise estimation report param to FT tool */
struct mmw_noise_est_final_report_t {
	uint8_t  rx_msk;
	uint8_t  fail_msk;
	uint8_t  invalid_msk;
	uint8_t  reserved;
	uint16_t peak_distance;
	int16_t  noise_db;
	struct mmw_noise_factor_t factor[CONFIG_MAX_RX_ANT];
}__packed;

/* storage noise estimation result of each ant */
struct mmw_noise_report_t {
	uint8_t test_fail_mask;
	uint8_t test_invalid_mask;
	uint8_t test_ant_mask;
	uint8_t fail;
	uint8_t rx_id;
	uint8_t rx_id_mimo;
	uint8_t noise_start_index;
	uint8_t noise_end_index;
	int16_t peak_distance;
	float peak_pwr_db;
	float snr_db;
	float noise_avge_db;
	float noise_low_db;
	float noise_high_db;
};

struct mmw_noise_param_t {
	uint8_t  tx_num;
	uint8_t  rx_num;
	uint16_t range_len;
	uint16_t doppler_len;
	uint16_t range_index_min;
	uint16_t range_index_max;
	uint16_t range_index_dc;
	uint16_t resol_mm;
	uint32_t frame_num;
	uint16_t rbin_max_pwr_idx[CONFIG_MAX_RX_ANT];				/* max power position for each rx */
	float rbin_max_pwr[CONFIG_MAX_RX_ANT];			/* max power for each rx */
};

struct mmw_noise_usr_t {
	uint8_t  est_rx_ant_msk;
	uint16_t range_min_mm;
	uint16_t range_max_mm;
};

struct radar_noise_est_t
{
	float *noise_est_buf;						/* size: [rx_num][range_bin_num], sum of each range bin data for expected dopper bins */				
	struct mmw_noise_param_t 	noise_param;
	struct mmw_noise_factor_t 	factor_param[CONFIG_MAX_RX_ANT];
	struct mmw_noise_usr_t 		usr_param;
} ;

/* ft noise est wave config */
struct mmw_noise_est_frame_t {
	uint8_t trx_mimo;
	uint8_t work_mode;
	uint8_t clutter_mode;
	uint32_t startFreq_MHz;
	uint32_t endFreq_MHz;
	uint32_t range_mm;
	uint32_t resol_mm;
	uint32_t velocity_mm;
	uint32_t veloc_resol;
	uint32_t intvl_period_us;
	uint32_t intvl_num;
	uint32_t frame_period_ms;
	uint32_t frame_num;
};

/* ------- angle estimation struct define ------- */
#if CONFIG_SOC_SERIES_RS624X
#define CALIB_DATA_NUM 		CONFIG_MAX_RX_ANT
#elif CONFIG_SOC_RS6130
#define CALIB_DATA_NUM 		5
#endif

/* angle estimation receive param from FT tool */
struct mmw_angle_est_recive_param_t {
	uint8_t work_mode;
	uint8_t mount_type;
	uint16_t range_mm;
	uint16_t range_res_mm;
	uint32_t start_freq;
} __packed;


struct mmw_angle_usr_t {
	uint8_t  est_rx_ant_msk;
	uint8_t tx_num;
	uint8_t rx_num;
	uint16_t range_len;
	uint16_t doppler_len;
	uint16_t range_min_mm;
	uint16_t range_max_mm;
	uint16_t range_index_min;
	uint16_t range_index_max;
};

/* storage data in angle estimation process */
struct radar_angle_est_t {
	Complexf32_RealImag angle_est_data_buf[CONFIG_MAX_RX_ANT];
	Complex16_RealImag rx_ant_calib_data[CONFIG_MAX_RX_ANT];
	struct mmw_angle_usr_t angle_est_usr_param;
	uint8_t angle_calib_mode; // 0 --> use calib data in flash 
	uint16_t range;
	int azimuth_deg; 	//q10
	int elevation_deg; 	//q10
};

/* angle estimation report param to FT tool */
struct mmw_angle_est_report_t {
	uint8_t  fail_msk;
	uint8_t  est_mask;
	uint16_t range;
	int noise_db;
	int snr;
	int rx_consis;
	int azimuth_deg;
	int elevation_deg;
	uint32_t calib[CALIB_DATA_NUM];	
} __packed;

/* ft angle est wave config */
struct mmw_angle_est_frame_t {
	uint32_t start_freq;
	uint8_t mount_type;
	uint16_t range_mm;
	uint16_t range_res_mm;
};

/* ------- voltage test struct define ------- */
struct dcdc_vol_t {
	uint32_t min;
	uint32_t max;
};

/* ------- IO test struct define ------- */
struct io_ctrl_t {
	uint8_t type;
	uint8_t input;
	uint8_t output;
	uint8_t input_mode;
};

struct io_output_ctrl_t {
	uint8_t type;
	uint8_t output;
	uint8_t value;
};

struct io_input_ctrl_t {
	uint8_t type;
	uint8_t input;
	uint8_t value;
	uint8_t input_mode;
};


#ifdef __cplusplus
}
#endif
#endif  