/**
 ******************************************************************************
 * @file    mmw_point_cloud_psic_lib.h
 * @brief   point cloud cpis lib define.
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

#ifndef _MMW_POINT_CLOUD_PSIC_LIB_H
#define _MMW_POINT_CLOUD_PSIC_LIB_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "mmw_ctrl.h"
#include "common.h"
#include "mmw_alg_pointcloud_typedef.h"

/* Exported types.
 * ----------------------------------------------------------------------------
 */
typedef struct {
    float real;
    float imag;
} MmwPsicComplexf32_reim;

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
/* Exported macro.
 * ----------------------------------------------------------------------------
 */
 
 /* define these macro to malloc max buffer in ant application to avoid frequency call malloc function */
#if (CONFIG_SOC_RS6240 || CONFIG_SOC_RS7241)
#define MMW_POINT_CLOUD_MAX_MIMO_RX_NUM			(8)			/* max: 2T4R */
#elif CONFIG_SOC_RS6130
#define MMW_POINT_CLOUD_MAX_MIMO_RX_NUM			(3)			/* max: 1T3R */
#else
#error "no valid board chosen!"
#endif

/*@ Register field mask                       */
#define field_mask(bitWidth)                            ((0x1UL << (bitWidth)) - 1)

/*@ Write retister field                      */
#define write_field(reg, startBit, bitWidth, val)                           \
    do {                                                                    \
        (reg) &= ~(field_mask(bitWidth) << (startBit));                     \
        (reg) |= (((uint32_t)(val) & field_mask(bitWidth)) << (startBit));  \
    } while (0);

/*@ Read retister field                       */
#define read_field(reg, startBit, bitWidth)   (((reg) >> (startBit)) & field_mask(bitWidth))



/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief reset clutter flag to reset clutter remove filter.
 * This function must be called:
 * 1. Before the first call of mmw_ctrl_start() (must)
 * 2. Environment changed. (Recommand)
 * 3. Frame config changed. (Recommand)
 * */
void mmw_reset_clut_flag(void);

/**
 * @brief Enable/Disable ant align in hw for RS6130, this function must be called before mmw_ctrl_start() called.
 *        Calibration param is read from in mmw_ctrl_open() function, even if mmw_set_ant_align(false) is called.
 *        If ant align is enabled, IQ data read from DataCube is aligned, otherwise un-aligned.
 *        This function only control ant align in RS6130, and has no effect for RS624x whether '1' or '0' is set in this function.
 *        So for RS624x, IQ data read from DataCube is always un-aligned.
 * @param mode: 1: enable hw ant align in RS6130, 0: diable
 * */
void mmw_set_ant_align(uint8_t mode);

/**
 * @brief This function should be called after waveforms are set.
 *        This function allocate all memories used in psic lib
 * @param max_range_det_idx the max range index will be used in this process.
 *        Any access grater than max_range_det_idx will cause memory overflow.
 *        Recommand pass range fft number.
 * */
void mmw_psic_lib_init(uint16_t max_range_det_idx);

/**
 * @brief This function free all memories used in psic lib. After call this function, noise associated functions cannot be used.
 * */
void mmw_psic_lib_deinit(void);

/* ------------- PSIC NOISE FUNCTIONS ------------- */
/**
 * @brief Call this function to update noise calculated in psic lib.
 *        This function may be called when no target is in FOV or @ref mmw_psic_noise_coverage_flag() function returns false. 
 * */
void mmw_psic_noise_update(void);

/**
 * @brief Noise is filtered across different frames using a LPF. This function gets the state of LPF.
 *        LPF is reset when call @ref mmw_psic_lib_init().
 * @return 0: LPF is not coveraged, 1: LPF is coveraged. 
 * */
uint8_t mmw_psic_noise_coverage_flag(void);

/**
 * @brief Get filtered power of noise(Q30) for specific range index;
 * @param range_idx range index which want to get noise power.
 * @return power of specific range index in U32Q30.
 * */
uint32_t mmw_psic_noise_get(uint16_t range_idx);

/**
 * @brief Get filtered power of noise(float) for specific range index;
 * @param range_idx range index which want to get noise power.
 * @return power of specific range index in float.
 */ 
float mmw_psic_noise_get_f32(uint16_t range_idx);

/**
 * @brief Get filtered power of noise(S16Q4) in dB for specific range index;
 * @param range_idx range index which want to get noise power.
 * @return power(dB) of specific range index in S16Q4.
 * */
int16_t mmw_psic_noise_get_db(uint16_t range_idx);

/**
 * @brief Get filtered power of noise(float) in dB for specific range index;
 * @param range_idx range index which want to get noise power.
 * @return power(dB) of specific range index in float.
 */ 
float mmw_psic_noise_get_db_f32(uint16_t range_idx);


/* ------------- PSIC FFT GAIN FUNCTIONS ------------- */

/**
 * @brief When @ref mmw_fft_autogain_set() function is called, IQ data read from data cube is auto left shifted by hardware,
 *        this is called auto gain.
 *        Each IQ data in data cube is controlled by 2 gains, one basic gain associated with tx_idx, one extra gain associated with range_idx and tx_idx,
 *        The restoration method of auto gain is:
 *        1. call @ref mmw_psic_auto_gain_base_get() function to get basic gain.
 *        2. call @ref mmw_psic_auto_gain_rbin_get() function to get extra gain.
 *        3. Gain restoration value is: IQ data / (1 << (basic gain + extra gain)) (Q15)
 *        Please refer to @ref mmw_process_fft_ant_f32() function to see restoration sample.
 * */
 
/**
 * @brief Get extra gain factor for specific range index and tx index.
 *        No param check is run in this function, user must make sure the input is valid
 * @param range_idx: range index on which user wants to get gain
 * @param tx_idx: tx index on which user wants to get gain
 * @return extra gain factor
 * */
extern uint32_t mmw_psic_auto_gain_rbin_get(uint32_t range_idx, uint8_t tx_idx);

/**
 * @brief Get extra gain factor for specific range index and tx index
 * @param range_idx: range index on which user wants to get gain
 * @param tx_idx: tx index on which user wants to get gain
 * @return 20log(1 << extra gain factor) in Q4
 * */
extern int16_t mmw_psic_auto_gain_rbin_get_db(uint32_t range_idx, uint8_t tx_idx);

/**
 * @brief Get basic gain factor for specific tx index.
 *        No param check is run in this function, user must make sure the input is valid
 * @param tx_idx: tx index on which user wants to get gain
 * @return basic gain factor
 * */
extern uint16_t mmw_psic_auto_gain_base_get(uint8_t tx_idx);
/**
 * @brief Get basic gain factor for specific tx index.
 *        No param check is run in this function, user must make sure the input is valid
 * @param tx_idx: tx index on which user wants to get gain
 * @return 20log(1 << basic gain factor) in Q4
 * */
extern int16_t mmw_psic_auto_gain_base_get_db(uint8_t tx_idx);

/** 
 * @brief return 20log10(1<<q_value), Q4
 * @param q_value
 * @return 20log10(1 << q_value), Q4
 */
extern int16_t mmw_psic_auto_gain_convert_q_value_to_db(uint16_t q_value);

/**
 * @brief return 20log((data >> right_shift_level) / 2^15) * 2^4, where data >> right_shift_level is Q15.
 *        This function will adjust gain to make more precise dB trans.
 * @param data data in
 * @param right_shift_level
 * @return Q4 in dB
 */
int16_t mmw_psic_auto_gain_get_db_64(uint64_t data, int16_t right_shift_level);
int16_t mmw_psic_auto_gain_get_db_32(uint32_t data, int16_t right_shift_level);

/**
 * @brief return 20log(data / 2^15) * 2^4.
 * @param data data in
 * @param right_shift_level
 * @return Q4 in dB
 */
int16_t mmw_psic_auto_gain_get_db(uint32_t data);

/**
 * @brief Clear hw auto gain config.
 * This function must be called at the end of mmw_ctrl_callback,
 * Otherwise, hardware auto gain function will not work well.
 */
void mmw_psic_auto_gain_clear(void);

typedef enum {
	PSIC_2ND_PASS_FILTER_PATH_0 = 0,
	PSIC_2ND_PASS_FILTER_PATH_1
} Psic2ndPassFilterPath_e;

/**
 * @brief Use aligned ant data to judge if this is a valid point cloud. Set strict_criterion to use stricter criteria,
 *           which will fill out more fake points.
 * @param  ptr_ant_data_aligned: aligned ant data in float version.
 * @param  path_idx: select algorithm process path for 2nd pass filter, only 0&1 are supported.
 * @return score of 2nd pass filter, less value means larger probabilty to pass 2nd pass filter.
 */
extern int16_t mmw_psic_2nd_pass_filter(MmwPsicComplexf32_reim *ptr_ant_data_aligned, Psic2ndPassFilterPath_e path_idx);

/**
 * @brief Get tx ant num/rx ant num /mimo rx num
 * @param ptr_mimo_rx_nums must be a valid pointer, no check in this function
 */
extern void mmw_psic_lib_sdk_get_tx_rx_num(MmwPsicMimoRxNum_t *ptr_mimo_rx_nums);

/**
 * @brief initial sram in bb, this function should be call before mmw_ctrl_start()
 */
extern void mmw_psic_dc_suppression_init(void);

/**
 * @brief This function is used to update dc suppression signal.
 *        
 *        This function needs clutter removal function is on and clutter update is off,
 *        otherwise it will return MMW_ERR_CODE_UNSUPPORT.
 *        
 *        It's recommanded to call this function when no target is in FOV.
 * 
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
extern int mmw_psic_dc_suppression_update(void);

#ifdef __cplusplus
}
#endif

#endif /* _MMW_POINT_CLOUD_PSIC_LIB_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
