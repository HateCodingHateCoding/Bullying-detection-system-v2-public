/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "soc.h"
#include "osi_common.h"
#include "types.h"
#include "util.h"
#include "kvf.h"

#include "mmw_ctrl.h"
#include "mmw_alg_doa.h"

/* complex16_reim, default calibration data */
#if (CONFIG_BOARD_MRS6240_P2510 || CONFIG_BOARD_MRS6240_P2520 || CONFIG_BOARD_MRS6240_P2512_CPUF || CONFIG_BOARD_MRS6240_P2512_CPUS)
uint32_t ant_calib_data[CALIB_TX_RX_NUM] = {
    0x0000540C, 0xDDFB5915, 0x1DD38101, 0xE4C356D9,
    0xFFE65413, 0xDF145A2F, 0x1C6382A3, 0xE4C556BC
};
#elif (CONFIG_BOARD_MRS6241_P2528)
uint32_t ant_calib_data[CALIB_TX_RX_NUM] = {
    0x0000687D, 0xC72778A8, 0xDFD16124, 0x4655693C,
    0x0C369794, 0x47318938, 0x31BD96E2, 0xC41F8CDE
};
#elif (CONFIG_BOARD_MRS7241_P2528)
uint32_t ant_calib_data[CALIB_TX_RX_NUM] = {
    0x000070D0, 0xD7E252BB, 0xEF73686D, 0xDB48564C,
    0xE85D86CB, 0x18919ABD, 0xF8B78004, 0x0ED596DE
};
#else //RS6130
#define MMW_CALIB_DATA_LEN		(4*16+8)
uint32_t ant_calib_data[CALIB_TX_RX_NUM] = {
	0x0488070d, 0x04ef0800, 0x039207b5
};

#endif

extern int mmw_calibration_data(uint8_t *data, int data_len);

#define calib_printk(...)  printk(__VA_ARGS__)

void mmw_calibration_calc(const complex16_imre *ant_data, complex16_reim *ant_calib_out, uint8_t num)
{
    int32_t max_phase = 1;

    /* calc 1/ant[i], and find max image or real*/
    for (int i = 0; i < num; ++i) {
        complex32_imre out;
        int32_t abs_pow2 = COMPLEX16_POWER(&ant_data[i]);
        complex16imre_conj_mul32(&ant_data[0], &ant_data[i], &out);
        if (abs_pow2) {
            uint32_t abs_tmp;
            out.real  = (((int64_t)out.real)<<13)/abs_pow2;
            abs_tmp   = ABS(out.real);
            max_phase = MAX(abs_tmp, max_phase);

            out.imag = (((int64_t)out.imag)<<13)/abs_pow2;
            abs_tmp   = ABS(out.imag);
            max_phase = MAX(abs_tmp, max_phase);
        }
        /* save */
        ant_calib_out[i].real = out.real;
        ant_calib_out[i].imag = out.imag;
    }
    max_phase = max_phase + 1;

    for (int i = 0; i < num; ++i) {
        calib_printk("out%d: re=%d im=%d\n", i, ant_calib_out[i].real, ant_calib_out[i].imag);
    }
    calib_printk("Calib Max=%d\n", max_phase);

    /* scaler Q15 */
    for (int i = 0; i < num; ++i) {
        int32_t real32 = ant_calib_out[i].real;
        int32_t image32 = ant_calib_out[i].imag;
        ant_calib_out[i].real = (real32<<15)/max_phase;
        ant_calib_out[i].imag = (image32<<15)/max_phase;
        calib_printk("Calib%d: re=%d im=%d\n", i, ant_calib_out[i].real, ant_calib_out[i].imag);
    }
}

uint16_t mmw_find_peak_index(uint16_t det_start, uint16_t det_end, uint16_t doppler_num)
{
    int ret;
    complex16_cube *data_buf;
    uint32_t max_power = 0;
    uint16_t max_idx = 0;

    data_buf = (complex16_cube *)OSI_Malloc(det_end*sizeof(complex16_cube));
    if (data_buf == NULL) {
        calib_printk("ERR databuf is NULL\n");
        return 0;
    }
    ret = mmw_fft_range(data_buf, det_end, 0, 0, doppler_num>>1);
    if (ret) {
        calib_printk("ERR mmw_fft_range failed %d\n", ret);
        goto exit;
    }

    for (int i = det_start; i < det_end; ++i) {
        uint32_t power = COMPLEX16_POWER(&data_buf[i]);
        if (power > max_power) {
            max_power = power;
            max_idx = i;
        }
    }
    calib_printk("start=%d, end=%d, peak idx=%d, power=%d\n",
                 det_start, det_end, max_idx, max_power);
exit:
    OSI_Free(data_buf);
    return max_idx;
}

void mmw_ant_calibration(uint16_t det_start, uint16_t det_end)
{
    int ret;
    uint16_t peak_idx;
    uint16_t range_fft_num = 0;
    uint16_t doppler_fft_num = 0;
    uint32_t u32_data_buf[CALIB_TX_RX_NUM];
    complex16_cube *ant_data = (complex16_cube *)u32_data_buf;
    complex16_mdsp *aligned_data = (complex16_mdsp *)ant_data;

    /* mdsp init */
    mdsp_angle_calc_init();

    /* Find range peak index use ant0 data */
    mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
    if (doppler_fft_num <= 1) {
        calib_printk("ERR doppler_num=%d\n", doppler_fft_num);
        return;
    }

    /* Find peak index at doppler0 */
    det_end = MIN(range_fft_num, det_end);
    peak_idx = mmw_find_peak_index(det_start, det_end, doppler_fft_num);

    /* Get all antenna data of peak index at doppler0 */
    ret = mmw_fft_ants(&ant_data[0], doppler_fft_num>>1, peak_idx);
    if (ret) {
        calib_printk("ERR mmw_fft_single_bin failed %d\n", ret);
        return;
    }

    /* debug print */
    for (int i = 0; i < CALIB_TX_RX_NUM; ++i) {
        calib_printk("Ant%d: re=%d im=%d\n", i, ant_data[i].real, ant_data[i].imag);
    }

    /* calc ant calibration */
    mmw_calibration_calc((complex16_imre *)&ant_data[0], (complex16_reim *)&ant_calib_data[0], CALIB_TX_RX_NUM);

    /* test calib data */
    mmw_cube_ant_align_2t4r(&ant_data[0], (complex16_mdsp *)&ant_calib_data[0], &aligned_data[0]);
    for (int i = 0; i < CALIB_TX_RX_NUM; ++i) {
        calib_printk("Align%d: re=%d im=%d\n", i, aligned_data[i].real, aligned_data[i].imag);
    }
    //mmw_show_angle(&result, peak_idx);
}

int mmw_ant_calib_data_save(bool clear)
{
    if (clear) {
        KVF_Delete(HWINFO_ID_NVM_ANT_CALIB_2T4R);
        calib_printk("Ant calib data reset!\n");
        return 0;
    } else {
        int len = KVF_Save(HWINFO_ID_NVM_ANT_CALIB_2T4R, (uint8_t *)&ant_calib_data[0], sizeof(ant_calib_data));
        calib_printk("Ant calib save %d!\n", len);
        return (len == sizeof(ant_calib_data) ? 0 : -1);
    }
}

int mmw_ant_calib_data_set(uint32_t *data, uint32_t len)
{
    if (data) {
        len = MIN(len, sizeof(ant_calib_data));
        memcpy((uint8_t *)&ant_calib_data[0], (uint8_t *)data, len);
        return len;
    } else {
        return -1;
    }
}

int mmw_ant_calib_data_get(uint32_t *data, uint32_t len)
{
    if (data) {
        len = MIN(len, sizeof(ant_calib_data));
        memcpy((uint8_t *)data, (uint8_t *)&ant_calib_data[0], sizeof(ant_calib_data));
        return len;
    } else {
        return -1;
    }
}

int mmw_ant_calib_data_load(void)
{
#if (CONFIG_SOC_RS6130)
    uint32_t chip_info[MMW_CALIB_DATA_LEN>>2] = { 0 };
    complex16_reim* ptr_calib_data = 0; 

    /* 
     * Ant data save format in flash:
     * RS6130: 
     * complex16_reim ant_0,
     * complex16_reim ant_1,
     * complex16_reim -ant_2
     *
     * RS6240:
     * complex16_reim ant_0,
     * complex16_reim ant_1,
     * complex16_reim ant_2,
     * complex16_reim ant_3,
     * complex16_reim ant_4,
     * complex16_reim ant_5,
     * complex16_reim ant_6,
     * complex16_reim ant_7,
     * complex16_reim ant_8,
     * */
    int len = KVF_Load(HWINFO_ID_NVM_ANT_CALIB_1T3R, (uint8_t *)&chip_info[0], HWINFO_SIZE_NVM_MMW_CALIB);
    ptr_calib_data = (complex16_reim*)&chip_info[2];
    /* ant2 is reversed for rs6130 */
    ptr_calib_data->imag = -ptr_calib_data->imag;
    ptr_calib_data->real = -ptr_calib_data->real;

    if (len == HWINFO_SIZE_NVM_MMW_CALIB) {
		ant_calib_data[0] = chip_info[0];
		ant_calib_data[1] = chip_info[1];
		ant_calib_data[2] = chip_info[2];
        chip_info[5]  = chip_info[1];
        chip_info[10] = chip_info[2];
        chip_info[15] = chip_info[3];
        chip_info[16] = chip_info[4] & 0xFFFF;
        chip_info[17] = chip_info[4] >> 16;
        chip_info[1] = 0;
        chip_info[2] = 0;
        chip_info[3] = 0;
        chip_info[4] = 0;
        /* 
         * Convert calibration data to chip format:
         * chip_info[0] ~ chip_info[15], a 4x4 calibration matrix
         * chip_info[16]: fcw
         * chip_info[17]: phase
        */
        int ret = mmw_calibration_data((uint8_t *)&chip_info[0], MMW_CALIB_DATA_LEN);
        calib_printk("MIMO Calib Load %d, ret=%d\n", len, ret);
        return ret;
    } else {
		chip_info[0]  = ant_calib_data[0];
        chip_info[5]  = ant_calib_data[1];
        chip_info[10] = ant_calib_data[2];
        chip_info[15] = 2048;
        chip_info[16] = (1 << 12);
        chip_info[17] = 15561;
        return mmw_calibration_data((uint8_t *)&chip_info[0], MMW_CALIB_DATA_LEN);
	}
#else
	/* KV-Calibration is only support for RS6130 */
    int len = KVF_Load(HWINFO_ID_NVM_ANT_CALIB_2T4R, (uint8_t *)&ant_calib_data[0], sizeof(ant_calib_data));
    calib_printk("MIMO Calib Load %d!\n", len);
    calib_printk("%x %x %x %x %x %x %x %x\n",
        ant_calib_data[0], ant_calib_data[1],
        ant_calib_data[2], ant_calib_data[3],
        ant_calib_data[4], ant_calib_data[5],
        ant_calib_data[6], ant_calib_data[7]);
    return (len == sizeof(ant_calib_data) ? 0 : -1);
#endif
}

/*
* @breif:the function is used to calibrates all antenna data for 2T4R.
*     the function calculate_angle_complex16_multi_mode0_init must be called before calling this function.
*
*/
void mmw_cube_ant_align_2t4r(const complex16_cube *ant_phase_data,
    const complex16_mdsp *ant_calibrate_data, complex16_mdsp *ant_output_data)
{
#if (FUNC_CALIB_DEBUG_EN)
    int32_t time_start = csi_coret_get_value();
#endif
    /* TODO: mdsp complex multiply will lost 6dB. */
    complex16cube_mul_mdsp16(&ant_phase_data[0], &ant_calibrate_data[0], &ant_output_data[0]);
    complex16cube_mul_mdsp16(&ant_phase_data[1], &ant_calibrate_data[1], &ant_output_data[1]);
    complex16cube_mul_mdsp16(&ant_phase_data[2], &ant_calibrate_data[2], &ant_output_data[2]);
    complex16cube_mul_mdsp16(&ant_phase_data[3], &ant_calibrate_data[3], &ant_output_data[3]);
    complex16cube_mul_mdsp16(&ant_phase_data[4], &ant_calibrate_data[4], &ant_output_data[4]);
    complex16cube_mul_mdsp16(&ant_phase_data[5], &ant_calibrate_data[5], &ant_output_data[5]);
    complex16cube_mul_mdsp16(&ant_phase_data[6], &ant_calibrate_data[6], &ant_output_data[6]);
    complex16cube_mul_mdsp16(&ant_phase_data[7], &ant_calibrate_data[7], &ant_output_data[7]);
#if (FUNC_CALIB_DEBUG_EN)
    time_start = wrap32_diff(csi_coret_get_value(), time_start);
    calib_printk("align-time=%d\n", time_start);
#endif
}

void mmw_show_angle(MmwAngleInfo_t *result, int range_idx, int dop_idx)
{
    uint32_t range_mm;
    uint32_t resol_mm;

    mmw_range_get(&range_mm, &resol_mm);
    resol_mm = resol_mm * range_idx/10;
    calib_printk("Range=%d, Dop=%d, Angle A=%d, E=%d\n",
        range_idx, dop_idx, (result->azimuth*10)>>7, (result->elevation*10)>>7);
    calib_printk("X=%dcm, Y=%dcm, Z=%dcm\n", ((int)resol_mm * result->sinValue_X)>>15,
        ((int)resol_mm * result->sinValue_Y)>>15, ((int)resol_mm * result->sinValue_Z)>>15);
}

void mmw_ant_angel_test(uint16_t range_idx, uint16_t det_start, uint16_t det_end,
                             int mimo_type, bool dbf, uint32_t test_type)
{
    int ret;
    uint32_t u32_data_buf[CALIB_TX_RX_NUM];
    complex16_cube *ant_data = (complex16_cube *)u32_data_buf;
    complex16_mdsp *aligned_data = (complex16_mdsp *)ant_data;

    complex16_mdsp azim_phase;
    complex16_mdsp elev_phase;
    MmwAngleInfo_t result;

    uint16_t range_fft_num = 0;
    uint16_t doppler_fft_num = 0;

    /* mdsp init */
    mdsp_angle_calc_init();

    /* Find range peak index use ant0 data */
    mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
    if (doppler_fft_num <= 1) {
        calib_printk("ERR doppler_num=%d\n", doppler_fft_num);
        return;
    }
    if (range_idx == 0xFFFF) {
        det_end = MIN(range_fft_num, det_end);
        range_idx = mmw_find_peak_index(det_start, det_end, doppler_fft_num);
    }
    if (range_idx >= range_fft_num) {
        calib_printk("ERR range_fft_num=%d\n", range_fft_num);
        return;
    }

#if (FUNC_ANGLE_CALC_TIME)
    int32_t time_start = csi_coret_get_value();

    for (int j = 0; j < 512; ++j) {
#endif
    //ret = mmw_fft_single_bin(&ant_data[0], MMW_ANT_ID_ALL, MMW_ANT_ID_ALL, doppler_fft_num>>1, range_idx);
    ret = mmw_fft_ants(&ant_data[0], doppler_fft_num>>1, range_idx);
    if (ret) {
        calib_printk("ERR mmw_fft_single_bin failed %d\n", ret);
        return;
    }

    for (int i = 0; i < CALIB_TX_RX_NUM; ++i) {
        calib_printk("Ant%d: re=%d im=%d\n", i, ant_data[i].real, ant_data[i].imag);
    }

    /* TODO: mdsp complex multiply will lost 6dB. */
    mmw_cube_ant_align_2t4r(&ant_data[0], (complex16_mdsp *)&ant_calib_data[0], &aligned_data[0]);
    for (int i = 0; i < CALIB_TX_RX_NUM; ++i) {
        calib_printk("Align%d: re=%d im=%d\n", i, aligned_data[i].real, aligned_data[i].imag);
    }

    if (mimo_type == 1) { //m62
        calib_printk("M62 Angle Test!\n");
        if (dbf) {
            mmw_angle_dbf_m62_azim_phase(&aligned_data[0], &azim_phase, 1);
            mmw_angle_elev_phase_m62(&aligned_data[0], (uint32_t *)&elev_phase);
        } else {
            mmw_angle_phase_calc_m62(&aligned_data[0], (uint32_t *)&azim_phase, (uint32_t *)&elev_phase);
        }
    } else if (mimo_type == 2) {
        calib_printk("A43 Angle Test!\n");
        if (dbf) {
            mmw_angle_dbf_a43_azim_phase(&aligned_data[0], &azim_phase, 1);
            mmw_angle_elev_phase_a43(&aligned_data[0], (uint32_t *)&elev_phase);
        } else {
            mmw_angle_azim_phase_a43(&aligned_data[0], (uint32_t *)&azim_phase);
            mmw_angle_elev_phase_a43(&aligned_data[0], (uint32_t *)&elev_phase);
        }
    } else {
        calib_printk("M81 Angle Test!\n");
        mmw_angle_dbf_m81_azim_phase(&aligned_data[0], &azim_phase, 1);
        test_type = 1;
    }

    switch (test_type) {
    case 1:  //azimuth test
        elev_phase.real = 4096;
        elev_phase.imag = 0;
        break;
    case 2:  //elevation test
    case 3:
        azim_phase.real = 4096;
        azim_phase.imag = 0;
        break;
    }
    mmw_angle_calculate((uint32_t *)&azim_phase, (uint32_t *)&elev_phase, &result);

#if (FUNC_ANGLE_CALC_TIME)
    }
    time_start = wrap32_diff(csi_coret_get_value(), time_start);
    calib_printk("total-time=%d, avrg=%d\n", time_start, (time_start>>10));
#endif

    mmw_show_angle(&result, range_idx, doppler_fft_num>>1);
}
