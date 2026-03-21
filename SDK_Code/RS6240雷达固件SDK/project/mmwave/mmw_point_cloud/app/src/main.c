/**
 ******************************************************************************
 * @file    main.c
 * @brief   main define.
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


/* Includes.
 * ----------------------------------------------------------------------------
 */
#include <hal_types.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "common.h"
#include "hif_config.h"
#include "hif.h"
#include "hif_com.h"
#include "mmw_ctrl.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_point_cloud_psic_lib.h"
#include "mmw_app_pointcloud.h"
#include "mmw_point_cloud_ut.h"
#include "mmw_alg_debug.h"
#include "mmw_app_pointcloud_config.h"
#if CONFIG_MMW_MICRO_POINT_CLOUD
#include "mmw_alg_doa.h"
#include "mmw_app_micro_pointcloud.h"
#endif
#include "mmw_report.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

typedef struct micro_report_arg {
	PointCloud3D *buffer;
	uint32_t  point_num;
	int32_t   micro_range_unit;  //cm-q4
} MmwMicroPointCloudOut_t;
/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Skip frame for mmw_point_cloud application */
 #define MMW_POINT_CLOUD_SKIP_FRAME_NUM				2
/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */
 /* 
 * The fisrt and second frame after call mmw_start() contains invalid data, so we should bypass 2st frame
 * This flag is set in main() function, and reset in mmw_ctrl_frame_cb()
 */
static uint8_t s_mmw_point_cloud_skip_frame_status;
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */
 
/** 
 *	@brief  motion point cloud upload to RadarDebugTool
 * 			use API "mmw_psic_debug_proto_report" to upload point cloud data
 * 			the conversion of carti as follow:
 * 			X = range * sin_azi;
 * 		 	Y = range * sqrt(1 - sin_azi * sin_azi - sin_ele * sin_ele);
 * 			Z = range * sin_ele;
 * 			sin_azi, sin_ele is geted from the result of "mmw_point_cloud_process"
 * */
static void mmw_point_cloud_upload(const PointCloudBuffer_t *ptr_3d_pc, uint16_t pc_len)
{
	float range_cm;
	float sin_y;
	float *ptr_X_Y_Z = 0;
	float *buf_x = 0;
	float velocity_mps; // 速度
    float snr_db;       // SNR
	uint32_t range_mm, range_reol_mm;
	uint16_t range_fft_num, doppler_fft_num;
	mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
	float range_bin_size_cm;
	//    使用配置的最大速度 (3.2) 来计算分辨率
    //    注意：这里假设最大速度配置是固定的 3.2
    float max_vel_mps = 3.2f; 
    float velocity_resolution = max_vel_mps / (doppler_fft_num / 2.0f);
	
	mmw_range_get(&range_mm, &range_reol_mm);
	mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
	range_bin_size_cm = (float)range_mm / range_fft_num * 0.1f;
	if (pc_len) {	
		mmw_process_mem_alloc((void**)&ptr_X_Y_Z, sizeof(*ptr_X_Y_Z) * pc_len * 5);
		if (!ptr_X_Y_Z) {
			return;
		}
		buf_x = ptr_X_Y_Z;
		for (uint16_t pc_idx = 0; pc_idx < pc_len; pc_idx++) {
			MmwClusterDetectData_t *pt = &ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx];
			
			range_cm = ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].range_idx * range_bin_size_cm;				
			sin_y = sqrtf(1 - ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase * ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase - 
                    ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].ele_phase * ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].ele_phase);
					
			// --- 计算速度 ---
            // 注释说 doppler_len/2 是静止(0速度)
            // 所以我们需要减去 offset 来得到有符号的索引
            int16_t signed_doppler_idx = (int16_t)pt->doppler_idx - (doppler_fft_num / 2);
            velocity_mps = signed_doppler_idx * velocity_resolution;
			
                    mmw_point_cloud_trans_radar_coord_to_user_coord(
                                        range_cm * ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase, /* x */
                                        range_cm * sin_y,                                                    /* y */
                                        range_cm * ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].ele_phase, /* z */
                                        buf_x
                                        );
										
			snr_db = pt->sig_snr;
			buf_x[3] = velocity_mps;
            buf_x[4] = snr_db;
            buf_x +=5;
        }
		
		mmw_psic_debug_proto_report(ptr_X_Y_Z, "motion_point_cloud", 5, pc_len, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 1, 0);	
		mmw_process_mem_free((void**)&ptr_X_Y_Z);	
	}
	else {
		mmw_psic_debug_proto_report(ptr_X_Y_Z, "motion_point_cloud", 5, 0, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);	
	}	
}

/** 
 *	@brief  micro point cloud upload to RadarDebugTool
 *			if user use micro point cloud, enable macro "CONFIG_MMW_MICRO_POINT_CLOUD" in prj_config.h
 * 			carti is geted from the result of "mmw_micro_point_process"
 * 			the data type of carti is  int16
 * 			in "mmw_psic_debug_proto_report", use "PSIC_DBG_PROTO_DATA_FORMAT_SHORT" to upload micro point cloud
 * */
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
int micro_point_cloud_data_handler(uint32_t range_idx, int veloc_idx,
					uint32_t snr_linear, MmwAngleInfo_t *angle, void *arg)
{
	MmwMicroPointCloudOut_t *report = (MmwMicroPointCloudOut_t *)arg;
	int32_t range_cm_q4 = range_idx * report->micro_range_unit;

    PointCloud_Cart *cart = (PointCloud_Cart *)report->buffer;
    int32_t range_cmq1 = (range_cm_q4 >> 3);
    mmw_micro_frame_trans_radar_coord_to_user_coord(
        (range_cmq1 * angle->sinValue_X) >> 16,
        (range_cmq1 * angle->sinValue_Y) >> 16,
        (range_cmq1 * angle->sinValue_Z) >> 16,
        &cart[report->point_num]
        );
	return (++report->point_num >= MICRO_POINT_MAX);
}

void mmw_micro_point_cloud_upload(PointCloud3D *ptr_3d_mpc, uint16_t mpc_len){
	int16_t *ptr_X_Y_Z = 0;
	int16_t *buf_x = 0;
	if (mpc_len) {	
		mmw_process_mem_alloc((void**)&ptr_X_Y_Z, sizeof(*ptr_X_Y_Z) * mpc_len * 3);		
		if (!ptr_X_Y_Z) {
			return;
		}
		buf_x = ptr_X_Y_Z;
		for (uint16_t pc_idx = 0; pc_idx < mpc_len; pc_idx++) {						
			*(buf_x + pc_idx) = ptr_3d_mpc[pc_idx].cart.x;	
			*(buf_x + pc_idx + mpc_len) = ptr_3d_mpc[pc_idx].cart.y;
			*(buf_x + pc_idx + mpc_len + mpc_len) = ptr_3d_mpc[pc_idx].cart.z;
		}
		
		mmw_psic_debug_proto_report(buf_x, "micro_point_cloud", 3, mpc_len, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);	
		mmw_process_mem_free((void**)&ptr_X_Y_Z);	
	}
	else {
		mmw_psic_debug_proto_report(buf_x, "micro_point_cloud", 3, 0, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
	}		
}
#endif

/** 
 * 	@note if user use micro point cloud, process micro point clould firstly, process motion point cloud secondly.
 **/
static int mmw_ctrl_frame_cb(void *mmw_data, void *arg) {
	
	/* Skip default 2 frames to wait for clutter coverage */
	if (!s_mmw_point_cloud_skip_frame_status) { /* s_mmw_point_cloud_skip_frame_status is 0 indicates that the invalid data frames has skipped */
		mmw_dsp_poweron();
#ifndef __MMW_PROCESS_UT

		PointCloudBuffer_t *ptr_point_cloud_buffer;
		/* if user want to using software CAFR, enable macro "SW_CFAR_ENABLE" in prj_config.h */
#if SW_CFAR_ENABLE
		ptr_point_cloud_buffer = mmw_point_cloud_process_sw_cfar();
#else
		ptr_point_cloud_buffer = mmw_point_cloud_process();
#endif

#if (CONFIG_MMW_MICRO_POINT_CLOUD)
		bool micro_update = mmw_micro_point_frame();

		/* auto gain function must be cleared manually as soon as gain factor is not used,
		   to make auto gain in next frame works well.
		   Hardware reuired gain factor cleared before chirp transmit period
		 */
		mmw_psic_auto_gain_clear();

		MmwMicroPointCloudOut_t mpc_output;
		uint16_t range_fft_num, doppler_fft_num;
		uint32_t range_mm, range_reol_mm;
		mmw_range_get(&range_mm, &range_reol_mm);
		mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
		
		mpc_output.micro_range_unit = (range_mm << 4) / 10 / range_fft_num;
		mpc_output.point_num = 0;
		mpc_output.buffer = 0;
		mmw_process_mem_alloc((void**)&mpc_output.buffer, MICRO_POINT_MAX * sizeof(*mpc_output.buffer));
		/* mmw frames down sampling for micro frame.
		 * During down sampling period, MCU keeps storing micro frame, ‘micro_update’ will keep ‘0’,
		 * therefor process micro points when new micro frame is ready(‘micro_update’ = 1).
		 * */
		if (micro_update) {
			mmw_micro_point_process(micro_point_cloud_data_handler, (void*)&mpc_output);
			mmw_micro_point_cloud_upload(mpc_output.buffer, mpc_output.point_num);	
		}
		mmw_process_mem_free((void**)&mpc_output.buffer);
#else
		/* auto gain function must be cleared manually as soon as gain factor is not used,
		   to make auto gain in next frame works well.
		   Hardware reuired gain factor cleared before chirp transmit period
		 */
		mmw_psic_auto_gain_clear();
#endif
		mmw_point_cloud_upload(ptr_point_cloud_buffer, ptr_point_cloud_buffer->point_cloud_num);
		mmw_process_mem_free((void**) &ptr_point_cloud_buffer->ptr_motion_point_cloud_data);
		mmw_process_mem_free((void**) &ptr_point_cloud_buffer);
#else
		mmw_radar_point_cloud_test_process();
		mmw_point_cloud_process_debug_time_cal();
#endif
	} else {
		s_mmw_point_cloud_skip_frame_status--; /* count skipped frames */
		/* auto gain function must be cleared manually as soon as gain factor is not used,
		   to make auto gain in next frame works well.
		   Hardware reuired gain factor cleared before chirp transmit period
		 */
		mmw_psic_auto_gain_clear();
	}
    return 0;
}

int main(void) {
    int status = 0;
    
    printf("mmw ctrl develop demo\n");

    printf("mmw_ctrl_open\n");
    status = mmw_ctrl_open(true, false, false);
    if (status != 0) {
        printf("mmw_ctrl_open fail %d\n", status);
    }

    printf("start init hif\n");

	/* MRS6130_P1806 set UART default
	 * MRS6130_P1812、MRS624x series set SPI default
	 * */
    HIF_InitParam_t hifInitParam;
#if  CONFIG_BOARD_MRS6130_P1806
	HIF_DefaultInitParam(&hifInitParam, HIF_COM_TYPE_UART, 1000000);
#elif (CONFIG_BOARD_MRS6130_P1812 || CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
	HIF_DefaultInitParam(&hifInitParam, HIF_COM_TYPE_SPI, 56000000);
#else
	#error "not support board"
#endif
   
    status = HIF_Init(&hifInitParam);
	if(status) {
		printf("host interface init fail! ret = %d \r\n", status);
	}

    printf("start init point cloud\n");
	
#if CONFIG_MMW_MICRO_POINT_CLOUD
	mmw_coordinate_config(MMW_COORDINATE_TYPE_CART);
    mmw_angle_mount_type_set(mmw_point_cloud_get_user_cfg_const()->mount_type);
	mmw_micro_point_init();	
#endif
	mmw_point_cloud_frame_config();
    mmw_point_cloud_init();
#if SW_CFAR_ENABLE
    mmw_fft_autogain_set(0); /* SW CFAR only support fix gain */
#endif
    status = mmw_ctrl_callback_cfg(&mmw_ctrl_frame_cb, MMW_DATA_TYPE_2DFFT, NULL);
    if (status != 0) {
        printf("mmw_ctrl_callback_cfg fail %d\n", status);
    }

    status = mmw_ctrl_start();
    if (status != 0) {
        printf("mmw_ctrl_start fail %d\n", status);
    }
	/* reset entery flag in here */
	s_mmw_point_cloud_skip_frame_status = MMW_POINT_CLOUD_SKIP_FRAME_NUM;
	
    return 0;
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
