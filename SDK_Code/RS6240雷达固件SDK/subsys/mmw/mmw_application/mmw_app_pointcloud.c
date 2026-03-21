/**
 ******************************************************************************
 * @file    mmw_point_cloude_process.c
 * @brief   this file contains the function of point cloud:
 *             1 point cloud frame config
 *             2 point cloud initialization config
 *             3 point cloud operation
 * @verbatim    null
 ******************************************************************************
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
 ******************************************************************************
 */
 
 
#include <math.h>
#include "common.h"
#include "mmw_ctrl.h"
#include "mmw_alg_ant_calibration.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_alg_pointcloud_typedef.h"
#include "mmw_app_pointcloud_config.h"
#include "mmw_point_cloud_psic_lib.h"
#include "mmw_app_pointcloud.h"
#include "mmw_point_cloud_ut.h"
#include "mmw_point_cloud_vv.h"
#include "cfar_casogo.h"
#include "mmw_alg_doa.h"
#include "csi_math.h"

/* cfar path num */
#define CFAR_PROCESS_PATH_MAX_NUM   (2)

/* max hardware cfar result per frame */
#define HW_CFAR_RESULT_MAX_NUM (512)

/* Detection parameters */
#define SW_CFAR_RESULT_MAX_NUM     512
#define RANGE_START_SKIP_NUM       5
#define RANGE_END_SKIP_NUM         3

#define CFAR_CA_SNR                7  /* =20*log10(7) */
#define CFAR_NOISE_LEN             4  /* single size=2^4 bin */
#define CFAR_GUARD_LEN             5  /* 5 bin */

uint8_t db_trans_lut[] = {0,2,4,6,8,10,12,14,16,18,20,22,23,25,27,29,31,32,34,36,37,39,41,42,44,45,47,48,50,51,53,54,56,57,59,60,62,63,64,66,67,68,70,71,72,73,75,76,77,79,80,81,82,83,85,86,87,88,89,90,91,93,94,95};
/* this function cloud trans float type varible to db(q4),formula:out = 10 * log10(in) * 2^4 */
int16_t fast_db_trans_f32(float lin_data){
    int16_t db_data = 0;
    uint32_t num;
    uint32_t float_addr = (uint32_t)&lin_data;

    if (lin_data != 0.f) {
        num = readw(float_addr);
        db_data = (((((num >> 23) & 0xFF) - 127) * 6) << 4) + db_trans_lut[(num >> 17) & 0x3F]; // Q4, 20*log10(*)
        db_data = db_data >> 1; /* 10*log10(*) */
    } else {
        db_data = 0;
    }
    return db_data;
}

int8_t mmw_point_cloud_frame_config(void)
{
    int8_t ret;

#if CONFIG_SOC_SERIES_RS613X
    ret = mmw_mode_cfg(MMW_MIMO_1T3R, MMW_WORK_MODE_2DFFT);
#elif (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
    ret = mmw_mode_cfg(MMW_MIMO_2T4R, MMW_WORK_MODE_2DFFT);
#else
    #error "not support board"
#endif    /* #if CONFIG_SOC_SERIES_RS613X */
    if (ret) {
        printf("mode cfg error! %d\n", ret);
    }
    ret = mmw_range_cfg(80*256, 80); /* max20.48m, resol=8cm */
    if (ret) {
        printf("range cfg error! %d\n", ret);
    }
    ret = mmw_velocity_cfg(3200, 100); /* max=3.2m/s, resol=0.1m/s, 32fft */
    if (ret) {
        printf("velocity cfg error! %d\n", ret);
    }
    /* Config RangeRate & frame rates due to different chip */
#if !defined (__MMW_PROCESS_UT) && !defined(__MMW_PROCESS_VV)
    ret = mmw_frame_cfg(50, 0);
#else
#if CONFIG_SOC_SERIES_RS613X
    /* should be at least 20s, to upload all debug data */
    ret = mmw_frame_cfg(10000, 0);
#elif (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
    ret = mmw_frame_cfg(20000, 0);
#else
    #error "not support board"
#endif /* !defined (__MMW_PROCESS_UT) && !defined(__MMW_PROCESS_VV) */

#endif /* #if !defined (__MMW_PROCESS_UT) && !defined(__MMW_PROCESS_VV) */
    if (ret) {
        printf("frame cfg error! %d\n", ret);
    }
    
    mmw_point_cloud_bb_config();

    return ret;
}

void mmw_point_cloud_bb_config(void)
{
	const MmwPointCloudUserCfg_t *ptr_mmw_user_cfg = mmw_point_cloud_get_user_cfg_const();
		
    /* cfar threshold unit is 'voltage', Q3 */
    mmw_cfar_cfg(ptr_mmw_user_cfg->mmw_motion_point_cloud_config.static_rm, floorf(sqrtf(ptr_mmw_user_cfg->mmw_motion_point_cloud_config.threshold_snr_pwr_lin) * 8));
    
    /* set the CFAR peaks limitation */
    mmw_cfar_dop_max_peak_set(32, 8);
	
	/* set autogain */
    mmw_fft_autogain_set(mmw_point_cloud_get_user_cfg_const()->auto_gain_flag);
	
    /* set enable the clutter removal mode, if micro motion point is used, DISABLE clutter remove */
    if (!ptr_mmw_user_cfg->mmw_point_cloud_detection_config.clutter_rm_method) {
        mmw_clutter_remove(MMW_CLUTTER_MODE_DISABLE);
    } else {
        mmw_clutter_remove(MMW_CLUTTER_MODE_ENABLE);
    }
	
	/* reset clutter flag to reset clutter remove function */
    mmw_reset_clut_flag();
	
    mmw_clutter_factor_cfg(ptr_mmw_user_cfg->mmw_point_cloud_detection_config.clutter_cover_time);
	
	/* clutter must update before calling mmw_ctrl_start() */
	mmw_clutter_halt_set(MMW_DISABLE);
	
	mmw_rx_ana_gain_cfg(0xff, ptr_mmw_user_cfg->mmw_point_cloud_detection_config.recep_attenua_config);
	mmw_tx_rf_att_set(ptr_mmw_user_cfg->mmw_point_cloud_detection_config.trans_attenua_config);
	mmw_ana_hpf_bw_cfg(0xff, ptr_mmw_user_cfg->mmw_point_cloud_detection_config.hpf_bandwith_config);
}

void mmw_point_cloud_hw_init()
{

}

void mmw_point_cloud_hw_deinit()
{
    /* disable autogain */
    mmw_fft_autogain_set(MMW_DISABLE); 
}

static void set_ant_geometry(MmwMountType_e mount_type)
{
/* mount_type can be referred to enum define(in mmw_alg_pointcloud_typedef.h,line 12) */
	MmwPointAntGeometry_t *ptr_azi_geometry_struct = mmw_get_g_azi_geometry_struct();
	MmwPointAntGeometry_t *ptr_elev_geometry_struct = mmw_get_g_elev_geometry_struct();
#if CONFIG_SOC_RS6130
    ptr_azi_geometry_struct->geometry_len = 2;
    ptr_elev_geometry_struct->geometry_len = 2;

    if (mount_type == MMW_MOUNT_VERTICAL) {
		ptr_azi_geometry_struct->ptr_geometry[0] = 2;
        ptr_azi_geometry_struct->ptr_geometry[1] = 1;
        ptr_elev_geometry_struct->ptr_geometry[0] = 0;
        ptr_elev_geometry_struct->ptr_geometry[1] = 1;
    } else if (mount_type == MMW_MOUNT_HORIZONTAL) {
        ptr_azi_geometry_struct->ptr_geometry[0] = 1;
        ptr_azi_geometry_struct->ptr_geometry[1] = 0;
        ptr_elev_geometry_struct->ptr_geometry[0] = 2;
        ptr_elev_geometry_struct->ptr_geometry[1] = 1;        
    }
#elif CONFIG_SOC_RS6240
    if (mount_type == MMW_MOUNT_VERTICAL) {
        ptr_azi_geometry_struct->ptr_geometry[0] = 0;
        ptr_azi_geometry_struct->ptr_geometry[1] = 1;
        ptr_azi_geometry_struct->ptr_geometry[2] = 4;
        ptr_azi_geometry_struct->ptr_geometry[3] = 5;

        ptr_elev_geometry_struct->ptr_geometry[0] = 3;
        ptr_elev_geometry_struct->ptr_geometry[1] = 2;
        ptr_elev_geometry_struct->ptr_geometry[2] = 1;
        
        ptr_azi_geometry_struct->geometry_len = 4;
        ptr_elev_geometry_struct->geometry_len = 3;
    } else if (mount_type == MMW_MOUNT_HORIZONTAL) {
		ptr_azi_geometry_struct->ptr_geometry[0] = 1;
        ptr_azi_geometry_struct->ptr_geometry[1] = 2;
        ptr_azi_geometry_struct->ptr_geometry[2] = 3;
        ptr_elev_geometry_struct->ptr_geometry[0] = 0;
        ptr_elev_geometry_struct->ptr_geometry[1] = 1;
        ptr_elev_geometry_struct->ptr_geometry[2] = 4;
        ptr_elev_geometry_struct->ptr_geometry[3] = 5;
        ptr_azi_geometry_struct->geometry_len = 3;
        ptr_elev_geometry_struct->geometry_len = 4;
    }
#elif CONFIG_SOC_RS7241
#if CONFIG_BOARD_MRS7241_P2840_CPUF
    ptr_azi_geometry_struct->ptr_geometry[0] = 4;
    ptr_azi_geometry_struct->ptr_geometry[1] = 5;
    ptr_azi_geometry_struct->ptr_geometry[2] = 6;
    ptr_azi_geometry_struct->ptr_geometry[3] = 7;
    ptr_azi_geometry_struct->ptr_geometry[4] = 0;
    ptr_azi_geometry_struct->ptr_geometry[5] = 1;
    ptr_azi_geometry_struct->ptr_geometry[6] = 2;
    ptr_azi_geometry_struct->ptr_geometry[7] = 3;

    ptr_elev_geometry_struct->ptr_geometry[0] = 0;
    ptr_elev_geometry_struct->ptr_geometry[1] = 0;

    ptr_azi_geometry_struct->geometry_len = 8;
    ptr_elev_geometry_struct->geometry_len = 0;
#elif CONFIG_BOARD_MRS7241_P2828_CPUF
    if (mount_type == MMW_MOUNT_VERTICAL) {
		ptr_azi_geometry_struct->ptr_geometry[0] = 4;
        ptr_azi_geometry_struct->ptr_geometry[1] = 0;
        ptr_azi_geometry_struct->ptr_geometry[2] = 6;
        ptr_azi_geometry_struct->ptr_geometry[3] = 2;
        ptr_azi_geometry_struct->ptr_geometry[4] = 7;
        ptr_azi_geometry_struct->ptr_geometry[5] = 3;

        ptr_elev_geometry_struct->ptr_geometry[0] = 5;
        ptr_elev_geometry_struct->ptr_geometry[1] = 0;
        ptr_azi_geometry_struct->geometry_len = 6;
        ptr_elev_geometry_struct->geometry_len = 2;
    } else if (mount_type == MMW_MOUNT_HORIZONTAL) {
        ptr_azi_geometry_struct->ptr_geometry[0] = 5;
        ptr_azi_geometry_struct->ptr_geometry[1] = 0;

        ptr_elev_geometry_struct->ptr_geometry[0] = 3;
        ptr_elev_geometry_struct->ptr_geometry[1] = 7;
        ptr_elev_geometry_struct->ptr_geometry[2] = 2;
        ptr_elev_geometry_struct->ptr_geometry[3] = 6;
        ptr_elev_geometry_struct->ptr_geometry[4] = 0;
        ptr_elev_geometry_struct->ptr_geometry[5] = 4;
        ptr_azi_geometry_struct->geometry_len = 2;
        ptr_elev_geometry_struct->geometry_len = 6;
    }
#else
    #error "Please Set User-Defined Ant Geometry"
#endif
#else
    #error "Please Set User-Defined Ant Geometry"
#endif
}

/*
 *  @brief This function allocate memories and BB configs before startup, no waveform config is done
 * here, waveform config is done in startup function. This function should be called after frame has been configed
 */
int mmw_point_cloud_init(void)
{
    uint16_t range_fft_num, doppler_fft_num;

    /* Use this function to configure some parameters that cannot be config from shell command to keep up with RadarAnalysis project*/
    mmw_point_cloud_hw_init();
    
    /**
     * mmw_process_lib_init function should be called after waveforms are set.
     * */ 
    mmw_process_lib_init();

    mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
    /**
      * psic lib init function should be called after waveforms are set.
      * In this demo, default use 80% range bin of range_fft_num, because the last 20%
      * range bin may be interferenced by hw filter and aliasing of signal.
      * */
    mmw_psic_lib_init(range_fft_num);

    /**
     * Set g_azi_geometry and g_elev_geometry according to mount type given
     * */
    set_ant_geometry(mmw_point_cloud_get_user_cfg_const()->mount_type);
	
#if (CONFIG_SOC_RS6240 || CONFIG_SOC_RS7241)
	/**
	 * Get calibration data from flash, set calibration data in g_rx_ant_calib_data.
	 * */
	complex16_mdsp calib_data[CALIB_TX_RX_NUM];
	mmw_ant_calib_data_get((uint32_t *)&calib_data[0], sizeof(calib_data));
	csi_q15_to_float((q15_t*)calib_data, (float*)mmw_get_g_rx_ant_calib_data(), CALIB_TX_RX_NUM * 2);
#endif
    return MMW_ERR_CODE_SUCCESS;
}

/*
 * @brief Rest sw/hw configuration configed in mmw_point_cloud_init() function
 */ 
void mmw_point_cloud_deinit()
{
    mmw_psic_lib_deinit();
    mmw_point_cloud_hw_deinit();
}

extern int16_t fast_db_trans(uint32_t lin_data);
int16_t mimo_data_power_avrg(complex16_cube *mimo_data)
{	
	int16_t db_data_q4 = 0;        /* S16Q4 */
	uint64_t power_no_coherent_sum = 0;
	uint32_t power_no_coherent;

	/* the real mimo data is: (mimo_data)/2^15 */
	for (int16_t ant_idx = 0; ant_idx < MMW_ANT_CHANNEL_NUM; ant_idx++) {
		power_no_coherent_sum += (int64_t)mimo_data[ant_idx].real * (int64_t)mimo_data[ant_idx].real 
									+ (int64_t)mimo_data[ant_idx].imag * (int64_t)mimo_data[ant_idx].imag;
	}
	power_no_coherent = power_no_coherent_sum / MMW_ANT_CHANNEL_NUM;	
	
	/*	when calculate no-coherent power, the result is: (2^15 * 2^15 + 2^15 * 2^15)
	*	Matlab conver db data code is: db(power_no_coherent / 2^30, 'voltage') / 2  
	*	--> (db(power_no_coherent / 2^15) + db(1 / 2^15))/ 2
	* 	therefore, the C code is: (fast_db_trans(power_no_coherent) + fast_db_trans(1)) / 2
	* 	the result of fast_db_trans is q4.
	*/
	db_data_q4 = (fast_db_trans(power_no_coherent) + fast_db_trans(1)) / 2;
	return db_data_q4;
}

/*
 * Main point cloud digital signal process, this function is called immedeiately after frame
 * interrupt. This function use software CFAR. This function does:
 * 1. Demonstrate the process of software CFAR, Use API: cfar_ca() does software CFAR detection .
 * 2. Get the result of software CFAR.
 * 3. Compare SNR with SNR threshold to reduce point cloud number.
 * 	  SNR < mmw_point_cloud_get_user_cfg_const()->threshold_snr_pwr(default 12dB); 
 * 4. Doa.
 * 5. The results is stored in ptr_point_cloud_3d, and is returned to caller.
 */
PointCloudBuffer_t* mmw_point_cloud_process_sw_cfar(void)
{
	uint16_t range_fft_num, doppler_fft_num;
	uint32_t veloc_bin_size, velocity_mm, dop_static;
	uint32_t range_bin_size, range_mm;
	uint16_t *range_abs_data = NULL;
	uint16_t *cfar_peaks_out = NULL;
	uint32_t cfar_num = 0, cfar_num_cur_dop = 0, point_num = 0;
	PointCloudBuffer_t *ptr_point_cloud_3d = 0;
	uint8_t cfar_max_num_flag = 0;
	float snr_threshold_db = 0.f, max_range_report_idx = 0.f;
	uint16_t abs_dop_offset;
	
	const MmwPointCloudUserCfg_t *mmw_user_config = mmw_point_cloud_get_user_cfg_const();
	
	uint32_t max_cfar_num = MIN(mmw_user_config->mmw_motion_point_cloud_config.motion_point_cloud_max_cfar_num, SW_CFAR_RESULT_MAX_NUM);
	
	/* CFAR result pointer */
    CFAR_Result *ptr_current_sw_cfar_res = 0;
    CFAR_Info *ptr_current_sw_cfar_info = 0;
	float *ptr_current_sw_cfar_sig_pwr = 0;
	
	mmw_process_mem_alloc((void**)&ptr_point_cloud_3d, sizeof(*ptr_point_cloud_3d));
    memset(ptr_point_cloud_3d, 0, sizeof(*ptr_point_cloud_3d));
	mmw_process_mem_alloc((void**)&ptr_point_cloud_3d->ptr_motion_point_cloud_data, sizeof(*ptr_point_cloud_3d->ptr_motion_point_cloud_data) * max_cfar_num);
	mmw_process_mem_alloc((void **)&ptr_current_sw_cfar_res, sizeof(*ptr_current_sw_cfar_res) * max_cfar_num);
	mmw_process_mem_alloc((void **)&ptr_current_sw_cfar_info, sizeof(*ptr_current_sw_cfar_info) * max_cfar_num);
	mmw_process_mem_alloc((void **)&ptr_current_sw_cfar_sig_pwr, sizeof(*ptr_current_sw_cfar_sig_pwr) * max_cfar_num);
	
	/* If noise is not coveraged, noise should be updated before cfar process first to avoid illegal noise in 1st frame */
	if (!mmw_psic_noise_coverage_flag()) {
		mmw_psic_noise_update();
	}

	int ret = mmw_motion_cube_access_open();
	if (ret) {
		printk("ERR: Open DataCube failed %d\n", ret);
		return 0;
	}
	mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
	max_range_report_idx = range_fft_num * 0.8f;
	mmw_range_get(&range_mm, &range_bin_size);
	mmw_velocity_get(&velocity_mm, &veloc_bin_size);
	range_bin_size = range_mm/range_fft_num;
	dop_static     = doppler_fft_num>>1;
	veloc_bin_size = velocity_mm/dop_static;
	
	range_abs_data = OSI_Malloc(range_fft_num * sizeof(uint16_t));
	cfar_peaks_out = OSI_Malloc((range_fft_num>>1) * sizeof(uint16_t)); //half of range_fft_num
	if (range_abs_data == NULL || cfar_peaks_out == NULL) {
		printk("ERR: CFAR buf malloc failed\n");
		return 0;
	}

	range_fft_num -= (RANGE_START_SKIP_NUM + RANGE_END_SKIP_NUM);
	
	/* CFAR detection for each dop bin */
	for (int dop_idx = 0; dop_idx < doppler_fft_num; ++dop_idx) {
		if (mmw_user_config->mmw_motion_point_cloud_config.static_rm) {
			if ((dop_idx == (dop_static - 1) ||
				 dop_idx == dop_static       ||
				 dop_idx == (dop_static + 1) )) {
				continue;
			}
		}
		if (dop_idx < dop_static) {
			abs_dop_offset = dop_static - dop_idx; 
			if (abs_dop_offset < mmw_user_config->mmw_point_cloud_filter_config.negative_vel_threshold_idx[0] || abs_dop_offset > mmw_user_config->mmw_point_cloud_filter_config.negative_vel_threshold_idx[1]) {
				continue;
			}
		} else if (dop_static < dop_idx) {
			abs_dop_offset = dop_idx - dop_static;
			if (abs_dop_offset < mmw_user_config->mmw_point_cloud_filter_config.positive_vel_threshold_idx[0] || abs_dop_offset > mmw_user_config->mmw_point_cloud_filter_config.positive_vel_threshold_idx[1]) {
				continue;
			} 
		} else {
			if (mmw_user_config->mmw_point_cloud_filter_config.negative_vel_threshold_idx[0] && mmw_user_config->mmw_point_cloud_filter_config.positive_vel_threshold_idx[0]) {
				continue;
			}
		}
		
		/* CFAR-CA detection on range of tx0-rx0. */
		__mmw_fft_abs_range(&range_abs_data[0], RANGE_START_SKIP_NUM, range_fft_num, 0, 0, dop_idx);
		cfar_num_cur_dop = cfar_ca(range_abs_data, range_fft_num, cfar_peaks_out,
							(CFAR_CA_SNR<<2), CFAR_NOISE_LEN, CFAR_GUARD_LEN, PASS_LINE_NONE);											

		/* get current dop sw cfar result */
		for (int cfar_idx = 0; cfar_idx < cfar_num_cur_dop; ++cfar_idx) {	
			uint16_t range_idx = cfar_peaks_out[cfar_idx] + RANGE_START_SKIP_NUM;
			if (range_idx < mmw_user_config->mmw_point_cloud_filter_config.range_threshold_idx[0] || range_idx > MIN(mmw_user_config->mmw_point_cloud_filter_config.range_threshold_idx[1], max_range_report_idx)) {
				continue;
			}
			complex16_cube ant_data[MMW_ANT_CHANNEL_NUM];
			float power_no_coherent_db = 0.f, snr_db = 0.f;
			int16_t noise_pwr_db_q4;

			__mmw_fft_ants(&ant_data[0], dop_idx, range_idx);
			/* calculate snr */
			noise_pwr_db_q4 = mmw_psic_noise_get_db(range_idx);
			power_no_coherent_db = mimo_data_power_avrg(&ant_data[0])  * 0.0625f;  // db_data_q4 / 16
			snr_db = power_no_coherent_db - (noise_pwr_db_q4 * 0.0625f); 
			/* pass snr less then snr_threshold_db */
			if (range_idx * range_bin_size < MMW_DIRECT_WAVE_RANGE_MM) {
				/* due to the influenceof the direct wave,motion point cloud need raise threshold in the first 200 mm */
				snr_threshold_db = fast_db_trans_f32(MMW_DIRECT_WAVE_THRES) * 0.0625f; /* q4 */
			} else {
				snr_threshold_db = fast_db_trans_f32(mmw_user_config->mmw_motion_point_cloud_config.threshold_snr_pwr_lin) * 0.0625f; /* q4 */
			}
			if (snr_db < snr_threshold_db) {
				continue;
			}
			/* save cfar result */
			ptr_current_sw_cfar_res[cfar_num].dop_idx = dop_idx;
			ptr_current_sw_cfar_res[cfar_num].range_idx = range_idx;
			ptr_current_sw_cfar_info[cfar_num].snr = snr_db;
			ptr_current_sw_cfar_sig_pwr[cfar_num] = power_no_coherent_db;
			cfar_num++;
			if (cfar_num >= max_cfar_num) {
				cfar_max_num_flag = 1;
				break;
			}
		}
		if (cfar_max_num_flag) {
			break;
		}
	}
		
	/* DoA*/
	int angle_num;
	MmwAngleInfo_t angle[MMW_DBF_MAX_PEAK];
	for (uint16_t point_idx = 0; point_idx < cfar_num; point_idx++) {
		complex16_cube ant_data[MMW_ANT_CHANNEL_NUM];
		
		uint16_t dop_idx = ptr_current_sw_cfar_res[point_idx].dop_idx;
		uint16_t range_idx = ptr_current_sw_cfar_res[point_idx].range_idx;
		__mmw_fft_ants(&ant_data[0], dop_idx, range_idx);
		angle_num = mmw_angle_estim_by_data(&ant_data[0], &angle[0]);
		if (angle[0].sinValue_Y >= 32767) {
			angle_num = 0;
		}
		/* Save Points*/
		if (angle_num) {
			ptr_point_cloud_3d->ptr_motion_point_cloud_data[point_num].range_idx = range_idx;
			ptr_point_cloud_3d->ptr_motion_point_cloud_data[point_num].doppler_idx = dop_idx;
			csi_q15_to_float(&angle[0].sinValue_X, &ptr_point_cloud_3d->ptr_motion_point_cloud_data[point_num].azi_phase, 1);
			csi_q15_to_float(&angle[0].sinValue_Z, &ptr_point_cloud_3d->ptr_motion_point_cloud_data[point_num].ele_phase, 1);
			ptr_point_cloud_3d->ptr_motion_point_cloud_data[point_num].sig_snr = ptr_current_sw_cfar_info[point_idx].snr;
			ptr_point_cloud_3d->ptr_motion_point_cloud_data[point_num].sig_power = ptr_current_sw_cfar_sig_pwr[point_idx];
			point_num++;
		}
	}
	mmw_motion_cube_access_close();
	ptr_point_cloud_3d->point_cloud_num = point_num;
	
	if (range_abs_data != NULL) {
		OSI_Free(range_abs_data);
	}
	if (cfar_peaks_out != NULL) {
		OSI_Free(cfar_peaks_out);
	}	
	mmw_process_mem_free((void **)&ptr_current_sw_cfar_res);
	mmw_process_mem_free((void **)&ptr_current_sw_cfar_info);
	mmw_process_mem_free((void **)&ptr_current_sw_cfar_sig_pwr);
	
	if (mmw_psic_noise_coverage_flag() && (ptr_point_cloud_3d->point_cloud_num == 0)) {
		mmw_psic_noise_update();
	}
	return ptr_point_cloud_3d;
}

/*
 * This function implements software cfar detection post process if hw cfar buffer is full.
 */
uint32_t mmw_cfar_detection_sw_post_process(CFAR_Result* ptr_cfar_res, uint16_t end_rbin_idx, uint32_t max_sw_cfar_num)
{
    uint32_t cfar_result_num = 0;
    uint16_t range_fft_num, doppler_fft_num, dop_static;
    uint8_t cfar_full_flag = 0;
    float snr_threshold_db = 0.f, snr_db = 0.f;
    uint16_t sig_Q_factor;
    int16_t noise_pwr_db_q4, sig_Q_factor_db_q4;
    int16_t *ptr_dop_fft_db_spectrum = 0;
    uint16_t skip_dop_num = 1;
	uint32_t max_range_mm, range_resolu_mm;
    const MmwPointCloudUserCfg_t *mmw_user_config = mmw_point_cloud_get_user_cfg_const();

    /* get used parameters */
    mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
    dop_static = doppler_fft_num >> 1;
	mmw_range_get(&max_range_mm, &range_resolu_mm);
	
	range_resolu_mm = max_range_mm / range_fft_num;
	
    mmw_motion_cube_access_open();
    if (doppler_fft_num > 0) {
        mmw_process_mem_alloc((void **)&ptr_dop_fft_db_spectrum, sizeof(*ptr_dop_fft_db_spectrum) * doppler_fft_num);
    }

    for (uint16_t range_idx = 0; range_idx <= end_rbin_idx; range_idx++) {

		/* ptr_dop_fft_db_spectrum is Q8 in dB(power) */
        __mmw_fft_logdB_doppler(ptr_dop_fft_db_spectrum, 0, doppler_fft_num, 0, 0, range_idx);
        noise_pwr_db_q4 = mmw_psic_noise_get_db(range_idx);
        sig_Q_factor = mmw_psic_auto_gain_base_get(0) + mmw_psic_auto_gain_rbin_get(range_idx, 0);
        sig_Q_factor_db_q4 = mmw_psic_auto_gain_convert_q_value_to_db(sig_Q_factor);
		if (range_idx * range_resolu_mm < MMW_DIRECT_WAVE_RANGE_MM) {
			/* due to the influenceof the direct wave,motion point cloud need raise threshold in the first 200 mm */
			snr_threshold_db = fast_db_trans_f32(MMW_DIRECT_WAVE_THRES) * 0.0625f; /* q4 */
		}else {
			snr_threshold_db = fast_db_trans_f32(mmw_user_config->mmw_motion_point_cloud_config.threshold_snr_pwr_lin) * 0.0625f; /* q4 */
		}
        for (uint16_t dop_idx = 0; dop_idx < doppler_fft_num; dop_idx++) {

            if (mmw_user_config->mmw_motion_point_cloud_config.static_rm) {
                if ((dop_idx <= (dop_static + skip_dop_num)) && (dop_idx >= (dop_static - skip_dop_num))) {
                    continue;
                }
            }

			/* 0.00390625f = 1/256, 0.0625 = 1/16 */
            snr_db = ptr_dop_fft_db_spectrum[dop_idx] * 0.00390625f - (noise_pwr_db_q4 + sig_Q_factor_db_q4) * 0.0625f;
            if (snr_db < snr_threshold_db) {
                continue;
            }

            ptr_cfar_res[cfar_result_num].range_idx = range_idx;
            ptr_cfar_res[cfar_result_num].dop_idx = dop_idx;
            cfar_result_num++;
            if (cfar_result_num >= max_sw_cfar_num) {
                cfar_full_flag = 1;
                break;
            }
        }
        if (cfar_full_flag) {
            break;
        }
    }
	
    if (doppler_fft_num > 0) {
        mmw_process_mem_free((void **)&ptr_dop_fft_db_spectrum);
    }
    mmw_motion_cube_access_close();
    return cfar_result_num;
}

/*
 * This function implements cfar detection process. firstly, read hardware cfar results, and when the buffer of hardware
 * cfar is full, execute software cfar post process.
 */
uint32_t mmw_cfar_detection_process(CFAR_Result** ptr_hw_cfar_buff, CFAR_Result** ptr_sw_cfar_buff, CFAR_Result** ptr_cfar_path_buff, uint32_t* ptr_cfar_num_buff, uint16_t max_cfar_num)
{
    uint32_t total_cfar_num = 0, hw_cfar_num = 0, max_sw_cfar_num = 0;

    mmw_process_mem_alloc((void**)ptr_hw_cfar_buff, HW_CFAR_RESULT_MAX_NUM * sizeof(CFAR_Result));
    /* 
     * Get cfar result in hw.
     * SNR calculated in hw is not such accuracy, 
     * if more accuracy snr is needed, program should calculate snr by power & noise
     * This function call should be in front of 'mmw_motion_cube_access_open', otherwise an error code should be raised.
    */
    mmw_cfar_result(*ptr_hw_cfar_buff, NULL, &hw_cfar_num, HW_CFAR_RESULT_MAX_NUM);

    if (hw_cfar_num >= HW_CFAR_RESULT_MAX_NUM) { /* hardware cfar buffer is full */
		max_sw_cfar_num = MIN(max_cfar_num, SW_CFAR_RESULT_MAX_NUM);
		mmw_process_mem_alloc((void**)ptr_sw_cfar_buff, max_sw_cfar_num * sizeof(CFAR_Result));
		ptr_cfar_num_buff[0] = mmw_cfar_detection_sw_post_process(*ptr_sw_cfar_buff, (*ptr_hw_cfar_buff)[1].range_idx, max_sw_cfar_num);
        ptr_cfar_path_buff[0] = *ptr_sw_cfar_buff;
		
        for (uint16_t index = 2; index < hw_cfar_num; index++) {
            if ((*ptr_hw_cfar_buff)[index].range_idx != (*ptr_hw_cfar_buff)[1].range_idx) {
                ptr_cfar_path_buff[1] = (*ptr_hw_cfar_buff) + index;
                ptr_cfar_num_buff[1] = hw_cfar_num - index;
                break;
            }
        }
		if (max_cfar_num < ptr_cfar_num_buff[0]) {
			ptr_cfar_num_buff[0] = max_cfar_num;
			ptr_cfar_num_buff[1] = 0;
		} else if (max_cfar_num >= ptr_cfar_num_buff[0] && max_cfar_num < (ptr_cfar_num_buff[0] + ptr_cfar_num_buff[1])) {
			ptr_cfar_num_buff[1] = max_cfar_num - ptr_cfar_num_buff[0]; /* if hw cfar + sw cfar num > max cfar num, save all sw cfar result,discard redundant hw cfar */
		} else {
			;
		}
    } else {
        ptr_cfar_path_buff[1] = *ptr_hw_cfar_buff;
        ptr_cfar_num_buff[1] = MIN(hw_cfar_num, max_cfar_num);
        ptr_cfar_num_buff[0] = 0;
    }

    for (uint16_t path_idx = 0; path_idx < CFAR_PROCESS_PATH_MAX_NUM; path_idx++) {
        total_cfar_num += ptr_cfar_num_buff[path_idx];
    }

    return total_cfar_num;
}

/*
 * Generate point clouds from cfar results, this function is called after hardware or software
 * cfar. This function dose:
 * 1. Run a simple peak search, static pointcloud removal algorithm(mmw_process_1st_pass_filter());
 * 2. Use psic lib to futher reduce point cloud number(mmw_psic_2d_pass_filter());
 * 3. Read beam vector from datacube, and use fft and phase average to get the phase difference or
 * angle fft spectrum;
 * 4. write point clouds to buffer;
 */
void mmw_point_cloud_generate_from_cfar_result(CFAR_Result* ptr_cfar_res, uint32_t cfar_res_num, MmwClusterDetectData_t* ptr_pc_buffer, uint16_t* ptr_pc_num)
{
    uint16_t point_num = *ptr_pc_num;
    uint8_t ret_status;
    uint16_t range_idx, doppler_idx;
    uint32_t max_range_mm, range_resolu_mm;
    uint32_t max_velocity_mm, velocity_resolu_mm;
	uint16_t range_fft_num, doppler_fft_num;
    uint16_t range_idx_last = UINT16_MAX;
    MmwPsicMimoRxNum_t mimo_rx_info;
    MmwPsic2DFFTNum_t fft_num_info;
    float power_no_coherent_sum;
    int16_t db_snr_q4;
    float noise_pwr = 0.f;
    float sin_azi, sin_elev;
	float snr_threshold_lin = 0.f;
    Complexf32_RealImag ptr_ant_data[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
    Complexf32_RealImag ptr_ant_aligned[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
    const MmwPointCloudUserCfg_t *mmw_user_config = mmw_point_cloud_get_user_cfg_const();
	
	if ((!ptr_cfar_res) || (!cfar_res_num)){
        return;
    }
	
    /* get used parameters */
    mmw_fft_num_get(&fft_num_info.range_fft_num, &fft_num_info.dop_fft_num);
    mmw_range_get(&max_range_mm, &range_resolu_mm);
    mmw_velocity_get(&max_velocity_mm, &velocity_resolu_mm);
	mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
    mmw_psic_lib_sdk_get_tx_rx_num(&mimo_rx_info);
	
	range_resolu_mm = max_range_mm / range_fft_num;
	
    for (uint16_t cfar_idx = 0; cfar_idx < cfar_res_num; cfar_idx++) {
        range_idx = ptr_cfar_res[cfar_idx].range_idx;
        doppler_idx = ptr_cfar_res[cfar_idx].dop_idx;

        /* run 1st pass filter, mainly filter out unwanted range */
        if (!mmw_process_1st_pass_filter(range_idx, doppler_idx, &fft_num_info, mmw_user_config)) {
            continue;
        }
        
        /*
         * re-calculate noise for specific range if range changed.
         * mmw_psic_mt_noise_get_db(range_idx) will cost a little
         * time, so it will be calculated only when range_idx changed. hw cfar output
         * is arranged by range, so this method can work well.
         */
        if (range_idx != range_idx_last) {
            noise_pwr = mmw_psic_noise_get_f32(range_idx);
            range_idx_last = range_idx;
        }

        /* 
         * Read ant data here, after simplely bypass some points, because read ant data will cost a little time.
         * Better used float version of this API to align gain between different range and tx
         */
        mmw_process_fft_ant_f32(ptr_ant_data, &mimo_rx_info, doppler_idx, range_idx);

        /* 
         * Run 2nd pass filter here, after HW CFAR, because some points with SNR less than 6dB are retained.
         * 2nd pass filter is necessary. 15.85f is 12dB.
         */
		if (range_idx * range_resolu_mm <= MMW_DIRECT_WAVE_RANGE_MM) {
			/* There is noise in first 4 range bins,ande the threshold is raised by 5db */
			snr_threshold_lin = MMW_DIRECT_WAVE_THRES;
		}else {
			snr_threshold_lin = mmw_user_config->mmw_motion_point_cloud_config.threshold_snr_pwr_lin;
		}
        if(!mmw_process_2nd_pass_filter(ptr_ant_data, &mimo_rx_info, noise_pwr, snr_threshold_lin, &power_no_coherent_sum)){
            continue;
        }
        
        /* 
         * Run Peak grouping to reduce density of point cloud if cluster & track process will be performed,
         * parameter PEAKGROUPING_DOPPLER is recommanded to achieve a balance between performance and compution time.
         */
        PeakGroupingMethod_e grouping_method = mmw_user_config->mmw_motion_point_cloud_config.peak_grouping_method;
        if (grouping_method != PEAKGROUPING_DISABLE && 
            !mmw_process_peak_grouping(range_idx, doppler_idx, &mimo_rx_info, &fft_num_info, power_no_coherent_sum, grouping_method)){
            continue;
        }

#if (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
        /* 
         * Channel calibration should be manualy done for all mimo_rx, user may use their own calibration algorithm here. 
         * RS6130 use hardware calibration data, and is auto-calibrated when generate DataCube
         */
        mmw_process_ant_data_calibrate_f32(ptr_ant_data, ptr_ant_aligned, mmw_get_g_rx_ant_calib_data_const(), mimo_rx_info.mimo_rx_num);
#else
        memcpy(ptr_ant_aligned, ptr_ant_data, sizeof(ptr_ant_aligned));
#endif
        
        /* 
         * Only if the aligned data passes 2nd pass filter, angle process will be applied
         * If compute time is enough, can use additional 'PSIC_2ND_PASS_FILTER_PATH_1' with logic 'AND' to enhance filter level.
        */
        if (
            (mmw_psic_2nd_pass_filter((MmwPsicComplexf32_reim*)ptr_ant_aligned, PSIC_2ND_PASS_FILTER_PATH_0) < mmw_user_config->mmw_motion_point_cloud_config.psic_2nd_pass_thres)
//                && (mmw_psic_2nd_pass_filter((MmwPsicComplexf32_reim*)ptr_ant_aligned, PSIC_2ND_PASS_FILTER_PATH_1) < mmw_radar_get_user_cfg_const()->psic_2nd_pass_thres)
            ) {
                ;
        } else {
            continue;
        }
        
        /* Calcuate azimuth and elevation DoA with conventional angle fft */
        ret_status = mmw_process_angle_process(ptr_ant_aligned, mmw_get_g_azi_geometry_struct_const(), mmw_get_g_elev_geometry_struct_const(), &sin_azi, &sin_elev, 0, 0);
        if (ret_status != MMW_ERR_CODE_SUCCESS) {
            continue;
        }

        db_snr_q4 = fast_db_trans_f32(power_no_coherent_sum / (noise_pwr * mimo_rx_info.mimo_rx_num));

        ptr_pc_buffer[point_num].range_idx = range_idx;
        ptr_pc_buffer[point_num].doppler_idx = doppler_idx;
        ptr_pc_buffer[point_num].azi_phase = sin_azi;
        ptr_pc_buffer[point_num].ele_phase = sin_elev;
        ptr_pc_buffer[point_num].sig_snr = db_snr_q4 * 0.0625f; /* db_snr / 16 */
        ptr_pc_buffer[point_num].sig_power = power_no_coherent_sum;
        /* Congratulations! This is a valid point. */
        point_num++;
    }
    *ptr_pc_num = point_num;
}

/*
 * Main point cloud digital signal process, this function is called immedeiately after frame
 * interrupt. This function dose:
 * 1. Demonstrate how to read cfar result from hardware(software cfar is another option);
 * 2. Due to the limited buffer of hardware cfar, it is necessary to continue implementing
 * software cfar post process on the basis of hardware cfar;
 * 3. Generate point clouds from all cfar results;
 * 4. The results is stored in ptr_point_cloud_3d, and is returned to caller.
 */
PointCloudBuffer_t* mmw_point_cloud_process(void)
{
    uint32_t cfar_num_arr[CFAR_PROCESS_PATH_MAX_NUM] = {0}; /* sw cfar:idx=0, hw cfar:idx=1 */;
	uint32_t total_cfar_num = 0;
	uint16_t current_pc_num = 0;
	
    /* CFAR result pointer */
    CFAR_Result *ptr_cfar_res_arr[CFAR_PROCESS_PATH_MAX_NUM] = {0}; /* sw cfar:idx=0, hw cfar:idx=1 */
    CFAR_Result *ptr_hw_cfar_res = 0, *ptr_sw_cfar_res = 0;
    PointCloudBuffer_t *ptr_point_cloud_3d = 0;
    const MmwPointCloudUserCfg_t *mmw_user_config = mmw_point_cloud_get_user_cfg_const();

    mmw_process_mem_alloc((void**)&ptr_point_cloud_3d, sizeof(*ptr_point_cloud_3d));
    memset(ptr_point_cloud_3d, 0, sizeof(*ptr_point_cloud_3d));
	
	/* Read cfar result from hardware and implement software cfar post process */
	total_cfar_num = mmw_cfar_detection_process(&ptr_hw_cfar_res, &ptr_sw_cfar_res, &ptr_cfar_res_arr[0], &cfar_num_arr[0], mmw_user_config->mmw_motion_point_cloud_config.motion_point_cloud_max_cfar_num);

	mmw_process_mem_alloc((void**)&ptr_point_cloud_3d->ptr_motion_point_cloud_data, sizeof(*ptr_point_cloud_3d->ptr_motion_point_cloud_data) * total_cfar_num);

	/* If noise is not coveraged, noise should be updated before cfar process first to avoid illegal noise in 1st frame */
	if (!mmw_psic_noise_coverage_flag()) {
		mmw_psic_noise_update();
	}

	/* Call this funtion before reading data cube will be more effective */
	mmw_motion_cube_access_open();

	/* Due to the detection of targets from near to far, it is necessary to first generate point clouds from the software cfar results.*/
	for (uint16_t path_idx = 0; path_idx < CFAR_PROCESS_PATH_MAX_NUM; path_idx++) {
		mmw_point_cloud_generate_from_cfar_result(ptr_cfar_res_arr[path_idx], cfar_num_arr[path_idx], ptr_point_cloud_3d->ptr_motion_point_cloud_data, &current_pc_num);
	}
	ptr_point_cloud_3d->point_cloud_num = current_pc_num;

	if (ptr_hw_cfar_res) {
		mmw_process_mem_free((void**)&ptr_hw_cfar_res);
	}
	if (ptr_sw_cfar_res) {
		mmw_process_mem_free((void**)&ptr_sw_cfar_res);
	}
	
	if (mmw_psic_noise_coverage_flag() && (ptr_point_cloud_3d->point_cloud_num == 0)) {
		mmw_psic_noise_update();
	}

#ifdef __MMW_PROCESS_VV
	extern void mmw_radar_point_cloud_printf_config();
	mmw_radar_point_cloud_printf_config();
	mmw_point_cloud_run_vv(ptr_current_cfar_res, ptr_current_cfar_info, cfar_num);
	mmw_point_cloud_vv_upload_mcu_result(ptr_point_cloud_3d);
#endif

	mmw_motion_cube_access_close();
	
    return ptr_point_cloud_3d;
}

void mmw_point_cloud_trans_radar_coord_to_user_coord(float x, float y, float z, float *ptr_cartisian_coord)
{
    #if CONFIG_BOARD_MRS6130_P1812
    if (mmw_point_cloud_get_user_cfg_const()->mount_type == MMW_MOUNT_VERTICAL) {
        ptr_cartisian_coord[0] = x;
        ptr_cartisian_coord[1] = y;
        ptr_cartisian_coord[2] = z;
    } else {
        ptr_cartisian_coord[0] = -x;
        ptr_cartisian_coord[1] = y;
        ptr_cartisian_coord[2] = -z;
    }
#elif CONFIG_BOARD_MRS6130_P1806
    if (mmw_point_cloud_get_user_cfg_const()->mount_type == MMW_MOUNT_VERTICAL) {
        ptr_cartisian_coord[0] = -x;
        ptr_cartisian_coord[1] = y;
        ptr_cartisian_coord[2] = -z;
    } else {
        ptr_cartisian_coord[0] = -x;
        ptr_cartisian_coord[1] = y;
        ptr_cartisian_coord[2] = -z;
    }
#elif (CONFIG_BOARD_MRS6240_P2512_CPUS || CONFIG_BOARD_MRS6240_P2512_CPUF)
    if (mmw_point_cloud_get_user_cfg_const()->mount_type == MMW_MOUNT_VERTICAL) {
        ptr_cartisian_coord[0]  = -x;
        ptr_cartisian_coord[1]  = y;
        ptr_cartisian_coord[2]  = -z;
    } else {
        ptr_cartisian_coord[0]  = x;
        ptr_cartisian_coord[1]  = y;
        ptr_cartisian_coord[2]  = z;
    }
#elif (CONFIG_BOARD_MRS7241_P2840_CPUF || CONFIG_BOARD_MRS7241_P2828_CPUF)
    ptr_cartisian_coord[0] = x;
    ptr_cartisian_coord[1] = y;
    ptr_cartisian_coord[2] = z;
#else
    #error "Please set a valid coordinate transfor function" 
#endif
}
