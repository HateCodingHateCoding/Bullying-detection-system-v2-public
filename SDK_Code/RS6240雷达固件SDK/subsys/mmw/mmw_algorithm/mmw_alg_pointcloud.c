/**
 ******************************************************************************
 * @file    mmw_alg_pointcloud.c
 * @brief   this file contains the computing function for point cloud
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
#include "mmw_ctrl.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_point_cloud_psic_lib.h"
#include "csi_math.h"
#include "csi_const_structs.h"
#include "mmw_alg_doa.h"
#include "mmw_app_pointcloud.h"

#if MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 == 4
static const csi_cfft_instance_f32 *s_ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len16;
#elif MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 == 5
static const csi_cfft_instance_f32 *s_ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len32;
#elif MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 == 6
static const csi_cfft_instance_f32 *s_ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len64;
#elif MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 == 7
static const csi_cfft_instance_f32 *s_ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len128;
#elif MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 == 8
static const csi_cfft_instance_f32 *s_ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len256;
#elif MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 == 9
static const csi_cfft_instance_f32 *s_ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len512;
#elif MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 == 10
static const csi_cfft_instance_f32 *s_ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len1024;
#else
#error invalid angle fft len!
#endif

uint8_t mmw_process_mem_alloc(void** ptr, uint32_t len)
{
    if (*ptr) {
        OSI_Free(*ptr);
    }
	if(len) {
		*ptr = OSI_Malloc(len);
		if(*ptr == 0){
			printf("mem allocate failed %s\n", __func__);
			while (1);
		}
	} else {
		*ptr = 0;
	}
    return !!(*ptr);
}

void mmw_process_mem_free(void** ptr_ptr_mem)
{
	if (*ptr_ptr_mem) {
		OSI_Free(*ptr_ptr_mem);
		*ptr_ptr_mem = 0;
	}
}

float mmw_process_db_complex_f32(const Complex16_RealImag *ptr_complex_value)
{
    int16_t ret_value;
    uint32_t db_value, abs_value;
	float db_float;

    mmw_dsp_abs_log(*ptr_complex_value, &abs_value, &db_value);
    ret_value = ((int16_t)(db_value & 0xffff)) << 7;
	csi_q15_to_float(&ret_value, &db_float, 1);
    return db_float;
}

float mmw_process_db_real_f32(uint16_t real_value)
{
    Complex16_RealImag tmp_in_data;

    tmp_in_data.real = real_value;
    tmp_in_data.imag = 0;
	return mmw_process_db_complex_f32(&tmp_in_data);
}
int16_t mmw_process_wrap_idx(int16_t idx, int16_t len) 
{
    int16_t wrap_dop_idx;

    if (idx < 0) {
        wrap_dop_idx = len + idx;
    } else if (idx >= len) {
        wrap_dop_idx = idx - len;
    } else {
        wrap_dop_idx = idx;
    }
    return wrap_dop_idx;
}

float mmw_process_get_power(uint16_t range_idx, uint16_t doppler_idx, const MmwPsicMimoRxNum_t *ptr_mimo_rx_nums)
{
    Complexf32_RealImag ptr_ant_data[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
    float squared_ant_data_f32[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
    float pwr = 0.f;

    mmw_process_fft_ant_f32(ptr_ant_data, ptr_mimo_rx_nums, doppler_idx, range_idx);
    csi_cmplx_mag_squared_f32((float*)ptr_ant_data, squared_ant_data_f32, ptr_mimo_rx_nums->mimo_rx_num);
    for (uint16_t mimo_rx_idx = 0; mimo_rx_idx < ptr_mimo_rx_nums->mimo_rx_num; mimo_rx_idx++) {
        pwr += squared_ant_data_f32[mimo_rx_idx];
    }
    return pwr;
}

uint8_t mmw_process_peak_grouping(uint16_t range_idx, uint16_t doppler_idx, const MmwPsicMimoRxNum_t *ptr_mimo_rx_nums, const MmwPsic2DFFTNum_t *ptr_2d_fft_num, float pwr_ut, PeakGroupingMethod_e peak_group_method)
{
    uint16_t compare_num = 0;
    float neighbour_pwr[8];            /* power near ut, max 8 for rectangle shape */
    uint8_t peak_grouping_pass_fg = 1;

    if (peak_group_method >= PEAKGROUPING_DOPPLER) {
        neighbour_pwr[0] = mmw_process_get_power(range_idx, mmw_process_wrap_idx(doppler_idx - 1, ptr_2d_fft_num->dop_fft_num), ptr_mimo_rx_nums);
        neighbour_pwr[1] = mmw_process_get_power(range_idx, mmw_process_wrap_idx(doppler_idx + 1, ptr_2d_fft_num->dop_fft_num), ptr_mimo_rx_nums);
        compare_num = 2;
    }
    if (peak_group_method >= PEAKGROUPING_DOPPLER_RANGE) {
        neighbour_pwr[2] = range_idx == 0 ? 0 : mmw_process_get_power(range_idx - 1, doppler_idx, ptr_mimo_rx_nums);
        neighbour_pwr[3] = range_idx >= (ptr_2d_fft_num->range_fft_num - 1) ? 0 : mmw_process_get_power(range_idx + 1, doppler_idx, ptr_mimo_rx_nums);
        compare_num = 4;
    }
    if (peak_group_method >= PEAKGROUPING_RECTANGLE) {
        neighbour_pwr[4] = range_idx == 0 ? 0 : mmw_process_get_power(range_idx - 1, mmw_process_wrap_idx(doppler_idx - 1, ptr_2d_fft_num->dop_fft_num), ptr_mimo_rx_nums);
        neighbour_pwr[5] = range_idx == 0 ? 0 : mmw_process_get_power(range_idx - 1, mmw_process_wrap_idx(doppler_idx + 1, ptr_2d_fft_num->dop_fft_num), ptr_mimo_rx_nums);

        neighbour_pwr[6] = range_idx >= (ptr_2d_fft_num->range_fft_num - 1) ? 0 : mmw_process_get_power(range_idx + 1, mmw_process_wrap_idx(doppler_idx - 1, ptr_2d_fft_num->dop_fft_num), ptr_mimo_rx_nums);
        neighbour_pwr[7] = range_idx >= (ptr_2d_fft_num->range_fft_num - 1) ? 0 : mmw_process_get_power(range_idx + 1, mmw_process_wrap_idx(doppler_idx + 1, ptr_2d_fft_num->dop_fft_num), ptr_mimo_rx_nums);
        compare_num = 8;
    }

    /* Find peaks in specific region in 'peak_group_method', if 2 peak power are equal select them both */
    for (uint16_t compare_idx = 0; compare_idx < compare_num; compare_idx++) {
        if (pwr_ut < neighbour_pwr[compare_idx])  {
            peak_grouping_pass_fg = 0;
            break;
        }
    }
    return peak_grouping_pass_fg;
}

/**
 * @brief read ant data, and align q value in autogain feature. Align ant data to min q_value to make S16Q15 data type is also valid. This will lose a little precision.
 */
int8_t mmw_process_fft_ant_f32(Complexf32_RealImag *buffer_f32, const MmwPsicMimoRxNum_t* ptr_mimo_rx_nums, uint16_t dop_idx, uint16_t range_idx)
{
    uint8_t q_tar;  // q_tar is the total q value that shoud be recovered.
    int8_t shift_value;
    uint8_t mimo_rx_idx = 0;
    complex16_cube buffer[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
    Complex32_RealImag buffer_32[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
	int8_t err_code = MMW_ERR_CODE_SUCCESS;

    err_code = mmw_fft_ants(buffer, dop_idx, range_idx);
	if (err_code != MMW_ERR_CODE_SUCCESS) {
		printf("%s call failed, err_code: %d", __func__, err_code);
	}

    /* convert buffer to Q31 */
    for (uint8_t tx_idx = 0; tx_idx < ptr_mimo_rx_nums->ant_tx_num; tx_idx++) {
        q_tar = mmw_psic_auto_gain_base_get(tx_idx) + mmw_psic_auto_gain_rbin_get(range_idx, tx_idx);
        shift_value = 16 - q_tar;  // shift_value is the value that will be left-shifted to get q31 data.
		for (uint8_t ant_idx = 0; ant_idx < ptr_mimo_rx_nums->ant_rx_num; ant_idx++) {
			buffer_32[mimo_rx_idx].real = buffer[mimo_rx_idx].real << shift_value;
			buffer_32[mimo_rx_idx].imag = buffer[mimo_rx_idx].imag << shift_value;
			mimo_rx_idx++;
		}
    }
    csi_q31_to_float((q31_t*)buffer_32, (float*)buffer_f32, ptr_mimo_rx_nums->mimo_rx_num << 1);
	return err_code;
}


int8_t mmw_process_fft_range_f32(Complexf32_RealImag *buffer_f32, complex16_cube* ptr_buffer_swap_q15, uint16_t read_num,
	               uint8_t tx_id, uint8_t rx_id, uint16_t dop_idx)
{
	uint8_t q_tar;  // q_tar is the total q value that shoud be recovered.
    int8_t shift_value;
    complex16_cube *ptr_buffer_q15 = ptr_buffer_swap_q15;
	Complex32_RealImag *ptr_buffer_q31 = (Complex32_RealImag*)buffer_f32;

	if (!ptr_buffer_swap_q15) {
		mmw_process_mem_alloc((void**)&ptr_buffer_q15, read_num * sizeof(*ptr_buffer_q15));
	}

	if (ptr_buffer_q15 && ptr_buffer_q31) {
		/* use fast API, to accelerate read effeciency */
		__mmw_fft_range(ptr_buffer_q15, 0, read_num, tx_id, rx_id, dop_idx);
		 /* convert buffer to Q31 */
		for (int range_idx = 0; range_idx < read_num; range_idx++) {
			q_tar = mmw_psic_auto_gain_base_get(tx_id) + mmw_psic_auto_gain_rbin_get(range_idx, tx_id);
			shift_value = 16 - q_tar;  // shift_value is the value that will be left-shifted to get q31 data.
			if (shift_value > 0) {
				ptr_buffer_q31[range_idx].real = ptr_buffer_q15[range_idx].real << shift_value;
                ptr_buffer_q31[range_idx].imag = ptr_buffer_q15[range_idx].imag << shift_value;
			} else {
                ptr_buffer_q31[range_idx].real = ptr_buffer_q15[range_idx].real >> (-shift_value);
                ptr_buffer_q31[range_idx].imag = ptr_buffer_q15[range_idx].imag >> (-shift_value);			
			}
		}
		csi_q31_to_float((q31_t*)ptr_buffer_q31, (float*)buffer_f32, read_num * 2);
		if (!ptr_buffer_swap_q15) {
			mmw_process_mem_free((void**)&ptr_buffer_q15);
		}

		return MMW_ERR_CODE_SUCCESS;
	} else {
		return MMW_ERR_CODE_NO_MEMORY;
	}

}

uint8_t mmw_process_lib_init(void)
{
	mmw_dsp_poweron();
	/* before angle calculate, user should init angle calib data.
	 * therefore, use "mmw_angle_init" function in process init.
	*/
	mmw_angle_init();
	return MMW_ERR_CODE_SUCCESS;
}


/*
 * @brief run 1st pass filter, include range limit and peakgrouping.
 * if all test passes ptr_ant_data stores the ant IQ data for all mimo num.
*/
uint8_t mmw_process_1st_pass_filter(uint16_t range_idx, uint16_t doppler_idx, const MmwPsic2DFFTNum_t *ptr_2d_fft_num, const MmwPointCloudUserCfg_t* ptr_user_cfg)
{
    uint8_t ret = 1;
	uint16_t half_dop_bin;
	uint16_t abs_vel_dop_idx = 0;
	half_dop_bin = ptr_2d_fft_num->dop_fft_num >> 1;
	
    ret &= (range_idx >= ptr_user_cfg->mmw_point_cloud_filter_config.range_threshold_idx[0]);
    ret &= (range_idx <= MIN(ptr_user_cfg->mmw_point_cloud_filter_config.range_threshold_idx[1], 0.8f * ptr_2d_fft_num->range_fft_num));
	
	if (doppler_idx < half_dop_bin)
	{
		abs_vel_dop_idx = half_dop_bin - doppler_idx;	//cal negative velocity doppler offset from zeros doppler
		ret &= (abs_vel_dop_idx >= ptr_user_cfg->mmw_point_cloud_filter_config.negative_vel_threshold_idx[0]);
		ret &= (abs_vel_dop_idx <= ptr_user_cfg->mmw_point_cloud_filter_config.negative_vel_threshold_idx[1]);
	} else if (half_dop_bin < doppler_idx) {
		abs_vel_dop_idx = doppler_idx - half_dop_bin;
		ret &= (abs_vel_dop_idx >= ptr_user_cfg->mmw_point_cloud_filter_config.positive_vel_threshold_idx[0]);
		ret &= (abs_vel_dop_idx <= ptr_user_cfg->mmw_point_cloud_filter_config.positive_vel_threshold_idx[1]);
	} else {
		ret &= !(ptr_user_cfg->mmw_point_cloud_filter_config.negative_vel_threshold_idx[0] && ptr_user_cfg->mmw_point_cloud_filter_config.positive_vel_threshold_idx[0]);
	}
	
	/* 
	 * Ignore static point cloud, and it's small region neighbour.
	 * Static doppler bin has doppler_idx = half_dop_bin.
	 * Enable this comment when developer wants less false point cloud,
	 * Disable this comment when radar is on a moving platform.
	*/
	if (ptr_user_cfg->mmw_motion_point_cloud_config.static_rm)
	{
		ret &= (doppler_idx < half_dop_bin - 1) || (doppler_idx > half_dop_bin + 1);
	}

    return ret;
}

uint8_t mmw_process_2nd_pass_filter(const Complexf32_RealImag *ptr_ant_data, const MmwPsicMimoRxNum_t* ptr_mimo_rx_nums, float noise_pwr, float threshold_pwr, float* ptr_pwr)
{
	float ptr_squared_ant_data_f32[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
	float pwr = 0.f;
	uint8_t test_2nd_pass_flag = 0;
	uint8_t mimo_rx_idx;
	
	/* non-coherent combination, pwr is the sum of combination, no averange is applied to reduce calculate time */
    csi_cmplx_mag_squared_f32((float*)ptr_ant_data, ptr_squared_ant_data_f32, ptr_mimo_rx_nums->mimo_rx_num);
    for (mimo_rx_idx = 0; mimo_rx_idx < ptr_mimo_rx_nums->mimo_rx_num; mimo_rx_idx++) {
        pwr += ptr_squared_ant_data_f32[mimo_rx_idx];
    }
	if (ptr_pwr) {
        *ptr_pwr = pwr;
    }
	
	if (pwr > threshold_pwr * noise_pwr * ptr_mimo_rx_nums->mimo_rx_num){
		test_2nd_pass_flag = 1;
	} else {
		test_2nd_pass_flag = 0;
	}
	return test_2nd_pass_flag;
}

float mmw_process_snr_modify(float pwr, float noise_pwr_db, const MmwPsicMimoRxNum_t *ptr_mimo_rx_nums)
{
    int16_t pwr_q15;
	float pwr_db;

    /* re-calculate snr by software, which is more accuracy than hw snr for high level application */
    pwr = pwr / ptr_mimo_rx_nums->mimo_rx_num;
    csi_float_to_q15(&pwr, &pwr_q15, 1);
    pwr_db = mmw_process_db_real_f32(pwr_q15);
    pwr_db = pwr_db * 0.5f;
    return (pwr_db > noise_pwr_db) ? (pwr_db - noise_pwr_db) : 0;
}

/* Angle process functions */

static float fft_shift(float idx_rolled, int16_t max_len)
{
    int16_t half_len = max_len >> 1;

    if (idx_rolled >= half_len) {
        idx_rolled = idx_rolled - max_len;
    }
    return idx_rolled;
}

void parabolic_interpolation(float left, float mid, float right, float *ptr_idx_offset, float *ptr_value)
{
    float interpolation_value;

    //a = l/2  + r/2 - mid
    //b = -l/2 + r/2
    //out = -b/2/a = (r/2 - l/2)/((mid - l)  + (mid - r))
    mid = (mid - left) + (mid - right);
	if (mid != 0.f) {
		interpolation_value = (float)(right - left) / (2 * mid);
	} else {
		interpolation_value = 0;
	}

    if (ptr_idx_offset) {
        *ptr_idx_offset = interpolation_value;
    }
    if (ptr_value) {
        *ptr_value = mid - 0.25f * (left - right) * interpolation_value;
    }
}

void mmw_process_ant_data_calibrate_f32(const Complexf32_RealImag *ant_input_data, Complexf32_RealImag *ant_output_data, const Complexf32_RealImag *ant_calibrate_data, uint8_t len)
{
    csi_cmplx_mult_cmplx_f32((const float32_t*)ant_input_data, (const float32_t*)ant_calibrate_data,
                             (float32_t*)ant_output_data, (uint32_t)len);
}

static int8_t _mmw_angle_fft(const Complexf32_RealImag *ant_aligned, Complexf32_RealImag *dbf_fft_out,
                           const uint8_t *ant_geometry, uint32_t dbf_ant_num)
{
    if (s_ptr_csi_cfft_instance_f32) {
        csi_fill_f32(0, (float*)dbf_fft_out, (1 << (MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2 + 1)));
        for (int i = 0; i < dbf_ant_num; ++i) {
            dbf_fft_out[i].real = ant_aligned[ant_geometry[i]].real;
            dbf_fft_out[i].imag = ant_aligned[ant_geometry[i]].imag;
        }
        csi_cfft_f32(s_ptr_csi_cfft_instance_f32, (float*)&dbf_fft_out[0], 0, 1);
        return MMW_ERR_CODE_SUCCESS;
    }
    return MMW_ERR_CODE_NOT_READY;
}

/**
 * peak search, if failed search peaks, output should be INT16_MAX
 *
 */
uint32_t mmw_angle_peak_search(const Complexf32_RealImag *fft_out, float *ptr_peak_out, uint16_t fft_len)
{
    float max_pwr;
    uint32_t max_pwr_idx;
	/* because fft is auto adjust, we can directly use this function to get max peak, peak_out is Q13 */
	csi_cmplx_mag_squared_f32((float*)fft_out, ptr_peak_out, fft_len);
	csi_max_f32(ptr_peak_out, fft_len, &max_pwr, &max_pwr_idx);
    return max_pwr_idx;
}

/* return conj(ptr_reference) * ptr_signal*/
static void complex_reim_conj_mul_f32(const Complexf32_RealImag *ptr_reference, const Complexf32_RealImag *ptr_signal, Complexf32_RealImag* ptr_out)
{
    ptr_out->real = ptr_signal->real * ptr_reference->real + ptr_signal->imag * ptr_reference->imag;
    ptr_out->imag = ptr_signal->real * -ptr_reference->imag + ptr_signal->imag * ptr_reference->real;   
}

/* return conj(ptr_reference) * ptr_signal*/
static float complex_power_f32(const Complexf32_RealImag *ptr_reference)
{
    return ptr_reference->real * ptr_reference->real + ptr_reference->imag * ptr_reference->imag;
}

/**
 * @brief return Angle(conj(ptr_reference) * ptr_signal)
 * @param ptr_reference: reference phase, usually rx0
 * @param ptr_signal: signal
 * @return Angle(conj(ptr_reference) * ptr_signal) in rad
 * */
static float CalcPhaseDiff_f32(const Complexf32_RealImag *ptr_reference, const Complexf32_RealImag *ptr_signal)
{
    Complexf32_RealImag ant_output_data;

    complex_reim_conj_mul_f32(ptr_reference, ptr_signal, &ant_output_data);
	return atan2f(ant_output_data.imag, ant_output_data.real);
}

/**
 * @brief Run conventional angle fft, user can choose their own ants by set param 'ptr_ant_geometry'.
 *        The output of angle fft is sin($\theta$), where sin($\theta)$ = 2 * angle_fft_idx / angle_fft_len for 0.5\lambda space anttena array.
 * 		  The peak index is interpolated by parabolic_interpolation method.
 * 		  pwr will also be calculated, one can use it to update high-precision snr.
 * @param ant_aligned: ant data that has been calibed with phase and power;
 * @param ptr_ant_geometry: indicate the number of ant which will be selected.
 * 		  e.g ptr_ant_geometry = [0, 2, 3], ant_aligned[0], ant_aligned[2], ant_aligned[3] will be used as angle fft input.
 * @param ant_len: length of ptr_ant_geometry.
 * @param ptr_sin_doa: sin_doa ouput, which is calculated by peak index in angle fft. This para should be a valid address
 * @param ptr_pwr: power of peak of angle fft. If set to 0, no power will be sent out.
 * @note  for 6130, use a simply method to calculate sin_doa. use "atan2" to calculate angle.
 * */
uint8_t mmw_process_angle_calculate(const Complexf32_RealImag *ant_aligned, const uint8_t *ptr_ant_geometry, uint8_t ant_len, float *ptr_sin_doa, float *ptr_pwr)
{
    uint8_t ret = MMW_ERR_CODE_NO_MEMORY;
	float power = 0.f;
	
#if CONFIG_SOC_RS6130	
	*ptr_sin_doa = CalcPhaseDiff_f32(&ant_aligned[ptr_ant_geometry[0]], &ant_aligned[ptr_ant_geometry[1]]) * 0.31831f; // sin(\theta) = antan2 / 3.14159 --> antan2 * 0.31831, namely{-pi, pi}
    if(ptr_pwr) {
        for (int ant_idx = 0; ant_idx < ant_len; ant_idx++) {
            power += complex_power_f32(&ant_aligned[ptr_ant_geometry[ant_idx]]);
        }
        *ptr_pwr = power / ant_len;
    }
	ret = MMW_ERR_CODE_SUCCESS;
#elif (CONFIG_SOC_RS6240 || CONFIG_SOC_RS7241)
	Complexf32_RealImag ptr_dbf_fft_out[1 << MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2];
	float ptr_dbf_pwr[1 << MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2];
    int32_t azi_idx;
    uint16_t angle_fft_len = 1 << MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2;
    float azi_idx_interp;

	/* All pointer should be valid. */
	if (ant_len == 2) {
		*ptr_sin_doa = CalcPhaseDiff_f32(&ant_aligned[ptr_ant_geometry[0]], &ant_aligned[ptr_ant_geometry[1]]) * 0.31831f; // sin(\theta) = antan2 / 3.14159 --> antan2 * 0.31831, namely{-pi, pi}
		if(ptr_pwr) {
			for (int ant_idx = 0; ant_idx < ant_len; ant_idx++) {
				power += complex_power_f32(&ant_aligned[ptr_ant_geometry[ant_idx]]);
			}
			*ptr_pwr = power / ant_len;
		}
		ret = MMW_ERR_CODE_SUCCESS;
	} else {
		if (s_ptr_csi_cfft_instance_f32) {
			/* angle fft may failed only if it is not initialized yet. */
			if (MMW_ERR_CODE_SUCCESS == _mmw_angle_fft(&ant_aligned[0], ptr_dbf_fft_out, ptr_ant_geometry, ant_len)) {
				/* find peak and interpolation with parabolic method to get higher-precision data. */
				azi_idx = mmw_angle_peak_search(ptr_dbf_fft_out, ptr_dbf_pwr, angle_fft_len);
				parabolic_interpolation(ptr_dbf_pwr[mmw_process_wrap_idx(azi_idx - 1, angle_fft_len)],
										ptr_dbf_pwr[azi_idx],
										ptr_dbf_pwr[mmw_process_wrap_idx(azi_idx + 1, angle_fft_len)],
										&azi_idx_interp, ptr_pwr);

				/* fft-shift to limit index range to (-angle_fft_len/2, angle_fft_len/2], namely (-\pi, pi] */
				azi_idx_interp = fft_shift(azi_idx_interp + azi_idx, angle_fft_len);

				/* sin(theta) = asind(2 * azi_idx_interp / angle_fft_len) for 0.5\lambda anttena array */
				*ptr_sin_doa = 2 * azi_idx_interp / angle_fft_len;

				/*
				 * restore power gain error by angle fft, one can use this power to re-calculate snr.
				 * For get angle fft gain, only 'ant_len' is devided. (If power is needed to be restored, ant_len^2 should be divided.
				 */
				if (ptr_pwr) {
					*ptr_pwr = *ptr_pwr / ant_len;
				}
				ret = MMW_ERR_CODE_SUCCESS;
			}
		}
	}
#else
	#error "not support board"
#endif
    return ret;
}

uint8_t mmw_process_angle_process(const Complexf32_RealImag *ant_aligned, const MmwPointAntGeometry_t* ptr_geometry_azi, const MmwPointAntGeometry_t* ptr_geometry_elev,
								float *ptr_sin_azi, float *ptr_sin_elev, float *ptr_pwr_azi, float *ptr_pwr_elev)
{
	uint8_t ret;
	float epsilon = 1.f - 1e-5f;
	float tmp;

	if (ptr_geometry_elev->geometry_len) {
		ret = mmw_process_angle_calculate(&ant_aligned[0], ptr_geometry_elev->ptr_geometry, ptr_geometry_elev->geometry_len,
					ptr_sin_elev, ptr_pwr_elev);
		if (ret != MMW_ERR_CODE_SUCCESS) {
			printf("elev angle calc failed: %d\r\n", ret);
			return ret;
		}
		if (fabsf(*ptr_sin_elev) >  epsilon) {
			return MMW_ERR_CODE_INVALID_RESULT;
		}
	} else {
		if (ptr_sin_elev) {
			*ptr_sin_elev = 0;
		}
		if (ptr_pwr_elev) {
			*ptr_pwr_elev = 0;
		}
	}

	if (ptr_geometry_azi->geometry_len) {
		ret = mmw_process_angle_calculate(&ant_aligned[0], ptr_geometry_azi->ptr_geometry, ptr_geometry_azi->geometry_len,
					ptr_sin_azi, ptr_pwr_azi);
		if (ret != MMW_ERR_CODE_SUCCESS) {
			printf("azi angle calc failed: %d\r\n", ret);
			return ret;
		} else {		
			/* Check sin\psi is valid */
			tmp = 1.f - *ptr_sin_azi * *ptr_sin_azi - *ptr_sin_elev * *ptr_sin_elev;
			if (tmp < 1e-5f) {
				return MMW_ERR_CODE_INVALID_RESULT;
			}
		}
	} else {
		if (ptr_sin_azi) {
			*ptr_sin_azi = 0;
		}
		if (ptr_pwr_azi) {
			*ptr_pwr_azi = 0;
		}
	}
	return MMW_ERR_CODE_SUCCESS;
}