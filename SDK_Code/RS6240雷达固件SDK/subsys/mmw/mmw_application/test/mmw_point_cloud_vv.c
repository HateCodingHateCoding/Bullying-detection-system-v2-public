/**
 ******************************************************************************
 * @file    main.c
 * @brief   funcions for Verification & Validation. V&V run steps:
 * 			1. upload hardware cfar results.
 *			2. upload mcu results use HIF debug protocol which is different and independent from authentic reporting protocol.
 * 		    3. use debug tools to compare and debug which process has different output.
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
 
#include <stdint.h>
#include "mmw_alg_debug.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_alg_pointcloud_typedef.h"
#include "mmw_point_cloud_psic_lib.h"

/**
 * @brief Upload all necessery debug data for v&v in PC platform
 * 		  1. Upload hardware CFAR result and software snr in message "raw_tar_list"
 * 		  2. Upload phase information
 * */
void mmw_point_cloud_run_vv(const CFAR_Result *ptr_cfar_result, const CFAR_Info *ptr_cfar_info, uint16_t cfar_num)
{
	int16_t *ptr_range_upload_buf = 0;
	int16_t *ptr_range_upload_header = 0;
	float *ptr_phase_upload_tx0_buf = 0;
	float *ptr_phase_upload_tx0_header= 0;

	float *ptr_phase_upload_tx1_buf = 0;
	float *ptr_phase_upload_tx1_header= 0;
	Complexf32_RealImag buffer[12];
	MmwPsicMimoRxNum_t mimo_rx_info;
	
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_rx_info);

	if (cfar_num > 120) {
		cfar_num = 120;			/* max 120 cfar point */
	}
	if (cfar_num) {
		mmw_process_mem_alloc((void**)&ptr_range_upload_header, sizeof(*ptr_range_upload_header) * 4 * cfar_num);
		mmw_process_mem_alloc((void**)&ptr_phase_upload_tx0_header, sizeof(*ptr_phase_upload_tx0_header) * mimo_rx_info.ant_rx_num * cfar_num * 2);
		mmw_process_mem_alloc((void**)&ptr_phase_upload_tx1_header, sizeof(*ptr_phase_upload_tx1_header) * mimo_rx_info.ant_rx_num * cfar_num * 2);
		if ((!ptr_range_upload_header) && (!ptr_phase_upload_tx0_header) && (!ptr_phase_upload_tx1_header)) {
			return;
		}
		ptr_range_upload_buf = ptr_range_upload_header;
		ptr_phase_upload_tx0_buf = ptr_phase_upload_tx0_header;
		ptr_phase_upload_tx1_buf = ptr_phase_upload_tx1_header;
		mmw_motion_cube_access_open();		/* more effective call this function before read data */

		for (uint16_t cfar_idx = 0; cfar_idx < cfar_num; cfar_idx++, ptr_cfar_result++, ptr_cfar_info++) {
			*ptr_range_upload_buf++ = ptr_cfar_result->range_idx;
			*ptr_range_upload_buf++ = ptr_cfar_result->dop_idx;
			*ptr_range_upload_buf++ = ptr_cfar_info->snr;
			*ptr_range_upload_buf++ = ptr_cfar_info->snr;
			
			mmw_process_fft_ant_f32(buffer, &mimo_rx_info, ptr_cfar_result->dop_idx, ptr_cfar_result->range_idx);
			for (uint8_t mimo_rx_idx = 0; mimo_rx_idx < mimo_rx_info.ant_rx_num; mimo_rx_idx++) {
				*ptr_phase_upload_tx0_buf++ = buffer[mimo_rx_idx].imag;
				*ptr_phase_upload_tx0_buf++ = buffer[mimo_rx_idx].real;
			}
			for (uint8_t mimo_rx_idx = mimo_rx_info.ant_rx_num; mimo_rx_idx < mimo_rx_info.ant_rx_num * 2; mimo_rx_idx++) {
				*ptr_phase_upload_tx1_buf++ = buffer[mimo_rx_idx].imag;
				*ptr_phase_upload_tx1_buf++ = buffer[mimo_rx_idx].real;
			}
		}
		
		mmw_motion_cube_access_close();
	}

	mmw_psic_debug_proto_report(ptr_range_upload_header, "raw_tar_list_hw", 1, 4 * cfar_num, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
	mmw_psic_debug_proto_report(ptr_phase_upload_tx0_header, "raw_tar_phase_list_1_hw", 1, mimo_rx_info.ant_rx_num * cfar_num * 2, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
	mmw_psic_debug_proto_report(ptr_phase_upload_tx1_header, "raw_tar_phase_list_2_hw", 1, mimo_rx_info.ant_rx_num * cfar_num * 2, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);

	if (cfar_num) {
		mmw_process_mem_free((void**)&ptr_range_upload_header);
		mmw_process_mem_free((void**)&ptr_phase_upload_tx0_header);
		mmw_process_mem_free((void**)&ptr_phase_upload_tx1_header);
	}
}


void mmw_point_cloud_vv_upload_mcu_result(PointCloudBuffer_t *ptr_point_cloud_buffer)
{
	int16_t *ptr_tar_polar = 0;
	int16_t *ptr_tar_header = 0;
	uint16_t range_fft_num, dop_fft_num;
    float *ptr_noise_pwr_db = 0;
	float *ptr_noise_pwr = 0;

	if (ptr_point_cloud_buffer->point_cloud_num) {
		/* range_idx, dop_idx, sin_azi(Q15), sin_elev(Q15), snr(Q4) */
		mmw_process_mem_alloc((void**)&ptr_tar_polar, ptr_point_cloud_buffer->point_cloud_num * sizeof(*ptr_tar_polar) * 5);
		ptr_tar_header = ptr_tar_polar;
		for (int point_idx = 0; point_idx < ptr_point_cloud_buffer->point_cloud_num; point_idx++) {
			*ptr_tar_polar++ = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_idx].range_idx;
			*ptr_tar_polar++ = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_idx].doppler_idx;
			*ptr_tar_polar++ = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_idx].azi_phase * 32768;
			*ptr_tar_polar++ = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_idx].ele_phase * 32768;
			*ptr_tar_polar++ = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_idx].sig_snr * 16;
		}
		mmw_psic_debug_proto_report((void*)ptr_tar_header, "tar_list_polar",1, ptr_point_cloud_buffer->point_cloud_num * 5, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
		mmw_process_mem_free((void**)&ptr_tar_header);
	} else { 
		mmw_psic_debug_proto_report((void*)ptr_tar_header, "tar_list_polar",1, 0, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
	}		
	
	/* upload mt noise for snr re-calcuate in signal process simulation */
    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
    mmw_process_mem_alloc((void**)&ptr_noise_pwr_db, sizeof(*ptr_noise_pwr_db) * range_fft_num);
	mmw_process_mem_alloc((void**)&ptr_noise_pwr, sizeof(*ptr_noise_pwr) * range_fft_num);
    for (int range_idx = 0; range_idx < range_fft_num; range_idx++) {
        ptr_noise_pwr_db[range_idx] = mmw_psic_noise_get_db_f32(range_idx);
    }
	for (int range_idx = 0; range_idx < range_fft_num; range_idx++) {
        ptr_noise_pwr[range_idx] = mmw_psic_noise_get_f32(range_idx);
    }
    /* upload data once a doppler bin has been read */
    mmw_psic_debug_proto_report((void*)ptr_noise_pwr_db, "noise", 1, range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
	mmw_psic_debug_proto_report((void*)ptr_noise_pwr, "noise_power", 1, range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
    mmw_process_mem_free((void**)&ptr_noise_pwr_db);
	mmw_process_mem_free((void**)&ptr_noise_pwr);
}
