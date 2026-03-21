#include "mmw_ctrl.h"
#include "mmw_point_cloud_psic_lib.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_app_pointcloud_config.h"
#include "mmw_alg_debug.h"
#include "csi_math.h"
#include "csi_const_structs.h"
#include "log.h"
#include "common.h"

#define HW_CFAR_RESULT_MAX_NUM (512)
#if CONFIG_SOC_SERIES_RS613X
#define MAX_RX_CHANNEL_NUM     (1 * 3)
#elif (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
#define MAX_RX_CHANNEL_NUM     (2 * 4)
#else
#error "Please Select Valid Chip!"
#endif

#define MMW_MAX_ANGLE_NUM      (1)
#define MAX_POINT_CLOUD_NUM    (256)
#define SKIP_NEAREST_RANGE_IDX (3)

#if (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
#define TIMES_CAL_FREQ		   (112)
#elif CONFIG_SOC_SERIES_RS613X
#define TIMES_CAL_FREQ		   (CONFIG_SOC_CLOCK_CFG)
#endif

/*all rx antennas calibrate data*/
#if (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
extern Complexf32_RealImag g_rx_ant_calib_data[MAX_RX_CHANNEL_NUM];
#else
static Complexf32_RealImag g_rx_ant_calib_data[MAX_RX_CHANNEL_NUM] = {
        {1, 0.f},
        {1, 0.f},
        {1, 0.f},
};
#endif

extern const MmwPointAntGeometry_t g_azi_geometry_struct;
extern const MmwPointAntGeometry_t g_elev_geometry_struct;

/**
 * @brief Use datacube as random input, Test precision of csi_cfft_f32 function
 * */
void mmw_process_fft_f32_test()
{
	#define TEST_FFT_INPUT_LEN			(4)
	#define TEST_FFT_POINT				(16)
    uint16_t range_fft_num, dop_fft_num;
    /* length of this buffer is range_fft_num * mimo_rx_num, it's used to store all range bin for each doppler bin */
    Complexf32_RealImag ptr_buffer_f32[8];
	Complexf32_RealImag fft_in_16[TEST_FFT_POINT];
	float scaled_to_value = 1e-8f;
	MmwPsicMimoRxNum_t mimo_info;
	static const csi_cfft_instance_f32 *ptr_csi_cfft_instance_f32 = &csi_cfft_sR_f32_len16;

    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
	memset(fft_in_16, 0, sizeof(fft_in_16));
    mmw_motion_cube_access_open();

	for (uint16_t dop_idx = 0; dop_idx < dop_fft_num; dop_idx++) {
		for (uint16_t range_idx = 0; range_idx < range_fft_num; range_idx++) {
			mmw_process_fft_ant_f32(ptr_buffer_f32, &mimo_info, dop_idx, range_idx);
			memset(fft_in_16, 0, sizeof(fft_in_16));
			fft_in_16[0].real = ptr_buffer_f32[0].real;
			fft_in_16[0].imag = ptr_buffer_f32[1].imag;
			fft_in_16[1].real = ptr_buffer_f32[1].real;
			fft_in_16[1].imag = ptr_buffer_f32[1].imag;
			fft_in_16[2].real = ptr_buffer_f32[4].real;
			fft_in_16[2].imag = ptr_buffer_f32[4].imag;
			fft_in_16[3].real = ptr_buffer_f32[5].real;
			fft_in_16[3].imag = ptr_buffer_f32[5].imag;
	
			for (int fft_input_idx = 0; fft_input_idx < TEST_FFT_INPUT_LEN; fft_input_idx++) {
				/* scale abs(*)to specific value */
				float scale = scaled_to_value / sqrtf(fft_in_16[fft_input_idx].real * fft_in_16[fft_input_idx].real + 
											fft_in_16[fft_input_idx].imag * fft_in_16[fft_input_idx].imag);						
				fft_in_16[fft_input_idx].real = fft_in_16[fft_input_idx].real * scale;
				fft_in_16[fft_input_idx].imag = fft_in_16[fft_input_idx].imag * scale;
			}
			mmw_psic_debug_proto_report((void*)fft_in_16, "fft_test_in", 1, 2 * TEST_FFT_POINT, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
			csi_cfft_f32(ptr_csi_cfft_instance_f32, (float*)fft_in_16, 0, 1);
			mmw_psic_debug_proto_report((void*)fft_in_16, "fft_test_out", 1, 2 * TEST_FFT_POINT, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
		}
	}

    mmw_motion_cube_access_close();
}

/* Buffer to save auto gain for fixed-point data_cube upload.
 * The memory is allocated on first call 'mmw_process_read_gain_factor()' function.
 *  g_fft_auto_gain_buff have 3 parts sequencely in memory:
 * 		  gain_offset_tx0: [0 ~ range_fft_num];
 * 		  gain_offset_tx1: [0 ~ range_fft_num];
 * 		  basic gain:      [basic_gain_tx0, basic_gain_tx1].
 *  float value for each range, e.g tx0:
 *        data / power(2, gain_offset_tx0[range_idx] + basic_gain_tx0)
 * */
uint8_t *g_fft_auto_gain_buff = 0;

/**
 * @brief get gain factor for each rbin/tx_idx, and basic gain into g_fft_auto_gain_buff
 * 		  On first call of this function, g_fft_auto_gain_buff will allocate a memory in heap.
 */
void mmw_process_read_gain_factor(void)
{
    uint16_t rfft_num, dfft_num;
    uint8_t *ptr_current_fft_gain;
	MmwPsicMimoRxNum_t mimo_info;

    mmw_fft_num_get(&rfft_num, &dfft_num);
    mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);

	/* allocate memory in first call */
	if (g_fft_auto_gain_buff == 0) {
		mmw_process_mem_alloc((void**)&g_fft_auto_gain_buff, sizeof(*g_fft_auto_gain_buff) * (rfft_num * mimo_info.ant_tx_num + mimo_info.ant_tx_num));
	}
	ptr_current_fft_gain = g_fft_auto_gain_buff;

	/* Get gain offset for each range bin and corresponding tx index */
    for (uint8_t tx_idx = 0; tx_idx < mimo_info.ant_tx_num; tx_idx++) {
        for (uint16_t rbin_idx = 0; rbin_idx < rfft_num; rbin_idx++) {
            *ptr_current_fft_gain= mmw_psic_auto_gain_rbin_get(rbin_idx, tx_idx);
            ptr_current_fft_gain++;
        }
    }

	/* Get basic gain for each tx */
    for (uint8_t tx_idx = 0; tx_idx < mimo_info.ant_tx_num; tx_idx++) {
        *ptr_current_fft_gain = mmw_psic_auto_gain_base_get(tx_idx);
        ptr_current_fft_gain++;
    }
}

/**
 * @brief Upload gain factor saved in g_fft_auto_gain_buff via HIF interface, together with frame index.
 * @param frame_idx: frame_idx in this frame, used for host to check data validity.
 */
void mmw_process_upload_gain_factor(uint32_t frame_idx)
{
    uint16_t rang_fft_num, dop_fft_num;
    uint8_t *ptr_fft_auto_gain = 0;
	MmwPsicMimoRxNum_t mimo_info;

    mmw_fft_num_get(&rang_fft_num, &dop_fft_num);
    mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);

    /* upload start flag of spectrum */
    if (rang_fft_num > 0) {
        mmw_process_mem_alloc((void **)&ptr_fft_auto_gain, (rang_fft_num * mimo_info.ant_tx_num + mimo_info.ant_tx_num + 4));
        memset(ptr_fft_auto_gain, 0, (rang_fft_num * mimo_info.ant_tx_num + mimo_info.ant_tx_num + 4));
    }

	/* upload frame index (4bytes) */
	ptr_fft_auto_gain[0] = frame_idx & 0xff;
	ptr_fft_auto_gain[1] = (frame_idx >> 8) & 0xff;
	ptr_fft_auto_gain[2] = (frame_idx >> 16) & 0xff;
	ptr_fft_auto_gain[3] = (frame_idx >> 24) & 0xff;
	
	/* upload ptr_fft_gain_arr, each tx has different base gain, each range idx has own gain, */
    for (uint16_t rbin_idx = 0; rbin_idx < (rang_fft_num * mimo_info.ant_tx_num + mimo_info.ant_tx_num); rbin_idx++) {
        ptr_fft_auto_gain[rbin_idx + 4] = g_fft_auto_gain_buff[rbin_idx];
    }
	mmw_psic_debug_proto_report((void*)ptr_fft_auto_gain, "fft auto gain", 1, (rang_fft_num * mimo_info.ant_tx_num + mimo_info.ant_tx_num + 4), PSIC_DBG_PROTO_DATA_FORMAT_BYTE,  PSIC_DBG_PROTO_DATA_UNSIGNED, 0, 0);

    if (rang_fft_num > 0) {
        mmw_process_mem_free((void **)&ptr_fft_auto_gain);
    }
}

/**
 * @brief Upload datacube via HIF interface, together with frame index.
 * 		  upload data is arranged by this way:
 * 		  1. upload frame_idx (uint32), repeak for range_fft_num;
 *        2. upload tx0_rx0_dop0_range0, tx0_rx0_dop0_range1, ... tx0_rx0_dop0_rangeN, N is range fft num;
 * 		  3. upload tx0_rx0_dop1_range0, tx0_rx0_dop1_range1, ... tx0_rx0_dopM_rangeN, N is range fft num, M is doppler fft num
 * 		  4. loop 2~3 for ant num(internal loop) times, to upload all ants datacube for tx0
 * 		  5. loop 2~4 for tx num(outsider loop) times, to upload all ants datacube for both tx
 * @param ant_all_flag: if set to 1, upload all datacube for all ants, otherwise upload only rx0 to save transmition time.
 * @param frame_idx: frame_idx in this frame, used for host to check data validity.
 */
void mmw_process_upload_datacube_q15(uint8_t ant_all_flag, uint32_t frame_idx)
{
	uint16_t rang_fft_num, dop_fft_num;
	Complex16_RealImag *ptr_range_spectrum = 0;
    uint32_t *ptr_frame_cnt_arr = 0;
	MmwPsicMimoRxNum_t mimo_info;

	mmw_fft_num_get(&rang_fft_num, &dop_fft_num);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);

    mmw_motion_cube_access_open();

	/* upload start flag of datacube */
	if (rang_fft_num > 0) {
		mmw_process_mem_alloc((void **)&ptr_range_spectrum, sizeof(Complex16_RealImag) * rang_fft_num);
        ptr_frame_cnt_arr = (uint32_t *)ptr_range_spectrum;
		for (uint16_t range_idx = 0; range_idx < rang_fft_num; range_idx++) {
            *ptr_frame_cnt_arr++ = frame_idx;
        }
		mmw_psic_debug_proto_report((void*)ptr_range_spectrum, "range doppler fft spectrum", 1, rang_fft_num * 2, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
	}

	/* upload data cube, all ants or single ant is judged by 'ant_all_flag' */
    if (!ant_all_flag) {
		mimo_info.ant_tx_num = 1;
		mimo_info.ant_rx_num = 1;
	}
	for (uint8_t tx_idx = 0; tx_idx < mimo_info.ant_tx_num; tx_idx++) {
		for (uint8_t rx_idx = 0; rx_idx < mimo_info.ant_rx_num; rx_idx++) {
			for (uint16_t dop_idx = 0; dop_idx < dop_fft_num; dop_idx++) {
				mmw_fft_range(ptr_range_spectrum, rang_fft_num, tx_idx, rx_idx, dop_idx);
				mmw_psic_debug_proto_report((void*)ptr_range_spectrum, "range doppler fft spectrum", 1, rang_fft_num * 2, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
			}
		}
    }

	if (rang_fft_num > 0) {
		mmw_process_mem_free((void **)&ptr_range_spectrum);
	}
	mmw_motion_cube_access_close();
}


void test_data_cube_q15(void)
{
	static uint32_t frame_idx = 0;
	mmw_process_read_gain_factor();
	mmw_process_upload_gain_factor(frame_idx);
	mmw_process_upload_datacube_q15(1, frame_idx);
	frame_idx++;
}

void test_data_cube_f32(void)
{
    uint16_t range_fft_num, dop_fft_num;
    /* length of this buffer is range_fft_num * mimo_rx_num, it's used to store all range bin for each doppler bin */
    Complexf32_RealImag *ptr_buffer_f32 = 0;
	MmwPsicMimoRxNum_t mimo_info;
	complex16_cube* ptr_buffer_swap_q15 = 0;

    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
    mmw_process_mem_alloc((void**)&ptr_buffer_f32, sizeof(*ptr_buffer_f32) * range_fft_num);
    mmw_process_mem_alloc((void**)&ptr_buffer_swap_q15, sizeof(*ptr_buffer_swap_q15) * range_fft_num);
    mmw_motion_cube_access_open();

	for (uint8_t phy_tx_idx = 0; phy_tx_idx < mimo_info.ant_tx_num; phy_tx_idx++) {
		for (uint8_t phy_rx_idx = 0; phy_rx_idx < mimo_info.ant_rx_num; phy_rx_idx++) {
			for (uint16_t dop_idx = 0; dop_idx < dop_fft_num; dop_idx++) {
				mmw_process_fft_range_f32(ptr_buffer_f32, ptr_buffer_swap_q15, range_fft_num, phy_tx_idx, phy_rx_idx, dop_idx);
				mmw_psic_debug_proto_report((void*)ptr_buffer_f32, "data_cube", 1, 2 * range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
			}
		}
	}
    mmw_motion_cube_access_close();
    mmw_process_mem_free((void**)&ptr_buffer_f32);
    mmw_process_mem_free((void**)&ptr_buffer_swap_q15);
}

void test_data_cube(uint8_t fixed_point)
{
	if (fixed_point) {
		test_data_cube_q15();
	} else {
		test_data_cube_f32();
	}
}

void test_mt_noise(void) {
    uint16_t range_fft_num, dop_fft_num;
    float *ptr_noise_pwr_db = 0;
	float *ptr_noise_pwr = 0;

    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
    mmw_process_mem_alloc((void**)&ptr_noise_pwr_db, sizeof(*ptr_noise_pwr_db) * range_fft_num);
	mmw_process_mem_alloc((void**)&ptr_noise_pwr, sizeof(*ptr_noise_pwr) * range_fft_num);
    mmw_psic_noise_update();
    for (int range_idx = 0; range_idx < range_fft_num; range_idx++) {
        ptr_noise_pwr_db[range_idx] = mmw_psic_noise_get_db_f32(range_idx);
    }
	for (int range_idx = 0; range_idx < range_fft_num; range_idx++)  {
		ptr_noise_pwr[range_idx] = mmw_psic_noise_get_f32(range_idx);
	}
	
    /* upload data once a doppler bin has been read */
    mmw_psic_debug_proto_report((void*)ptr_noise_pwr_db, "noise", 1, range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
	mmw_psic_debug_proto_report((void*)ptr_noise_pwr, "noise_power", 1, range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
    mmw_process_mem_free((void**)&ptr_noise_pwr_db);
	mmw_process_mem_free((void**)&ptr_noise_pwr);
}

typedef struct {
    float sin_azi;
    float sin_elev;
    float azi_pwr;
    float elev_pwr;
} SinDoa_t;

void test_angle_calculate(void) {
    uint16_t range_fft_num, dop_fft_num;

    Complexf32_RealImag *ptr_buffer_raw;
    Complexf32_RealImag ptr_buffer_aligned[MAX_RX_CHANNEL_NUM];
    Complexf32_RealImag *ptr_buffer_f32 = 0;			/* buffer to store all range bins in one doppler bin */

    SinDoa_t *ptr_sin_doa = 0;
    SinDoa_t *ptr_sin_doa_hdr;

    uint8_t ret;
	MmwPsicMimoRxNum_t mimo_info;
    float sin_elev, elev_pwr, sin_azi, azi_pwr;
    complex16_cube* ptr_buffer_swap_q15 = 0;

    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
    mmw_process_mem_alloc((void**)&ptr_sin_doa, sizeof(*ptr_sin_doa) * range_fft_num);	
	mmw_process_mem_alloc((void**)&ptr_buffer_f32, sizeof(*ptr_buffer_f32) * range_fft_num);
	mmw_process_mem_alloc((void**)&ptr_buffer_swap_q15, sizeof(*ptr_buffer_swap_q15) * range_fft_num);

    ptr_sin_doa_hdr = ptr_sin_doa;
    mmw_psic_debug_proto_report((void*)g_rx_ant_calib_data, "test_angle_calculate_calib_param", 1, 2 * MAX_RX_CHANNEL_NUM, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING,
                                    PSIC_DBG_PROTO_DATA_F32, 0, 0);

    mmw_motion_cube_access_open();
	/* 
	 * Read ant data from data cube.
	 * In this test function, we use only ant1 as test input:
	 * treat data_cube as a doppler_num x range_fft_num matrix:
	 * MATLAB code is:
	 * ant_input = reshape(data_cube.', 8, []).';
	 * each row is a test case.
	 * */
	for (uint16_t dop_idx = 0; dop_idx < dop_fft_num; dop_idx++) {
		mmw_process_fft_range_f32(ptr_buffer_f32, ptr_buffer_swap_q15, range_fft_num, 0, 0, dop_idx);
		uint16_t range_idx = 0;
		for (range_idx = 0; range_idx + mimo_info.mimo_rx_num < range_fft_num; range_idx += mimo_info.mimo_rx_num) {

			/* apply calibration param */
			ptr_buffer_raw = &ptr_buffer_f32[range_idx];
#if (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
            /* 
             * Channel calibration should be manualy done for all mimo_rx, user may use their own calibration algorithm here. 
             * RS6130 use hardware calibration data, and is auto-calibrated when generate DataCube
             */
			mmw_process_ant_data_calibrate_f32(ptr_buffer_raw, ptr_buffer_aligned, &g_rx_ant_calib_data[0], mimo_info.mimo_rx_num);
#elif CONFIG_SOC_SERIES_RS613X
			memcpy(ptr_buffer_aligned, ptr_buffer_raw, sizeof(*ptr_buffer_raw) * mimo_info.mimo_rx_num);
#else
    #error "Please Select Valid Chip!"
#endif
			
			ret = mmw_process_angle_process(ptr_buffer_aligned, &g_azi_geometry_struct, &g_elev_geometry_struct, &sin_azi, &sin_elev, &azi_pwr, &elev_pwr);
			if (ret == MMW_ERR_CODE_INVALID_RESULT) {
				sin_azi = 1;
			}

			/* save result in buffer */
			ptr_sin_doa->sin_azi = sin_azi;
			ptr_sin_doa->sin_elev = sin_elev;
			ptr_sin_doa->azi_pwr = azi_pwr;
			ptr_sin_doa->elev_pwr = elev_pwr;
			ptr_sin_doa++;
		}
		/* upload data once a doppler bin has been read */
		mmw_psic_debug_proto_report((void *)ptr_sin_doa_hdr, "test_angle_calculate", 1, (range_fft_num - mimo_info.mimo_rx_num) / mimo_info.mimo_rx_num * 4, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING,
									PSIC_DBG_PROTO_DATA_F32, 0, 0);
		ptr_sin_doa = ptr_sin_doa_hdr;
	}

    mmw_motion_cube_access_close();
	mmw_process_mem_free((void**)&ptr_buffer_swap_q15);
	mmw_process_mem_free((void**)&ptr_buffer_f32);
    mmw_process_mem_free((void**)&ptr_sin_doa_hdr);
}

void mmw_radar_point_cloud_printf_config()
{
	static uint32_t frame_cnt = 0;
	uint16_t sys_data[10];
	uint16_t range_fft_num, dop_fft_num;
	uint32_t period_ms, frame_num;
	uint32_t range_resolution_mm, range_mm;
	uint32_t range_rate_resolution_mm, velocity_mm;
	MmwPsicMimoRxNum_t mimo_info;

	mmw_frame_get(&period_ms, &frame_num);
	mmw_fft_num_get(&range_fft_num, &dop_fft_num);
	mmw_range_get(&range_mm, &range_resolution_mm);
	mmw_velocity_get(&velocity_mm, &range_rate_resolution_mm);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
	
	sys_data[0] = frame_cnt & 0xffff;
	sys_data[1] = frame_cnt >> 16;
	sys_data[2] = 0;
	sys_data[3] = period_ms;
	sys_data[4] = range_fft_num;
	sys_data[5] = range_resolution_mm;
	sys_data[6] = 1;
	sys_data[7] = mimo_info.mimo_rx_num;
	sys_data[8] = range_rate_resolution_mm;
	sys_data[9] = dop_fft_num;
 	frame_cnt++;
	
	mmw_psic_debug_proto_report((void*)sys_data, "frame_def", 1, 10, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);

}

void test_peak_grouping(void)
{
	float *ptr_pwr = 0;			/* used to log power */
	uint8_t *ptr_peak_log = 0;	/* used to log if this is a peak. */
	MmwPsic2DFFTNum_t fft_2d_num;
	MmwPsicMimoRxNum_t mimo_info;

    mmw_fft_num_get(&fft_2d_num.range_fft_num, &fft_2d_num.dop_fft_num);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
	mmw_process_mem_alloc((void**)&ptr_pwr, sizeof(*ptr_pwr) * fft_2d_num.range_fft_num);
	mmw_process_mem_alloc((void**)&ptr_peak_log, sizeof(*ptr_peak_log) * fft_2d_num.range_fft_num);
    mmw_motion_cube_access_open();
	/* 
	 * Read ant data from data cube.
	 * In this test function, we use only abs(rx_0) as detection mat,
	 * and peak_grouping algorithm run on each range/doppler bin.
	 * */
	for (int dop_idx = 0; dop_idx < fft_2d_num.dop_fft_num; dop_idx++) {
		for (int range_idx = 0; range_idx < fft_2d_num.range_fft_num; range_idx++) {
			ptr_pwr[range_idx] = mmw_process_get_power(range_idx, dop_idx, &mimo_info);

			/* Test 4 peakgrouping methods togther, pwr are the same */
			ptr_peak_log[range_idx] = mmw_process_peak_grouping(range_idx, dop_idx, &mimo_info, &fft_2d_num, ptr_pwr[range_idx], PEAKGROUPING_DISABLE);
			ptr_peak_log[range_idx] |= 2 * mmw_process_peak_grouping(range_idx, dop_idx, &mimo_info, &fft_2d_num, ptr_pwr[range_idx], PEAKGROUPING_DOPPLER);
			ptr_peak_log[range_idx] |= 4 * mmw_process_peak_grouping(range_idx, dop_idx, &mimo_info, &fft_2d_num, ptr_pwr[range_idx], PEAKGROUPING_DOPPLER_RANGE);
			ptr_peak_log[range_idx] |= 8 * mmw_process_peak_grouping(range_idx, dop_idx, &mimo_info, &fft_2d_num, ptr_pwr[range_idx], PEAKGROUPING_RECTANGLE);
		}
		mmw_psic_debug_proto_report((void *)ptr_pwr, "test_peak_grouping_pwr", 1, fft_2d_num.range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING,
									PSIC_DBG_PROTO_DATA_F32, 0, 0);
		mmw_psic_debug_proto_report((void *)ptr_peak_log, "test_peak_grouping_peak", 1, fft_2d_num.range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_BYTE,
									PSIC_DBG_PROTO_DATA_UNSIGNED, 0, 0);
	}
    mmw_motion_cube_access_close();
	mmw_process_mem_free((void**)&ptr_pwr);
	mmw_process_mem_free((void**)&ptr_peak_log);
}

void test_2nd_pass_filter(void)
{
	uint16_t range_fft_num, dop_fft_num;
	Complexf32_RealImag ant_data[MAX_RX_CHANNEL_NUM];
	float noise_pwr;
	float *ptr_pwr = 0;
	uint8_t *ptr_2nd_pass_filter = 0;
	MmwPsicMimoRxNum_t mimo_info;
	
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);

    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
	mmw_process_mem_alloc((void**)&ptr_2nd_pass_filter, sizeof(*ptr_2nd_pass_filter) * range_fft_num);
	mmw_motion_cube_access_open();
	/* Read ant data from data cube.*/
	for (int dop_idx = 0; dop_idx < dop_fft_num; dop_idx++) {
		for (int range_idx = 0; range_idx < range_fft_num; range_idx++) {
			mmw_process_fft_ant_f32(ant_data, &mimo_info, dop_idx, range_idx);
			noise_pwr = mmw_psic_noise_get_f32(range_idx);
			ptr_2nd_pass_filter[range_idx] = mmw_process_2nd_pass_filter(ant_data, &mimo_info, noise_pwr, 15.8489f, ptr_pwr);
					}
		mmw_psic_debug_proto_report((void *)ptr_2nd_pass_filter, "test_2nd_pass_filter", 1, range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_BYTE,
									PSIC_DBG_PROTO_DATA_UNSIGNED, 0, 0);
	}

	mmw_motion_cube_access_close();
	mmw_process_mem_free((void**)&ptr_2nd_pass_filter);
}

void test_hw_cfar(void)
{
	uint32_t cfar_num = 0;
    /* CFAR result pointer */
    CFAR_Result *ptr_current_cfar_res = 0;
    CFAR_Info *ptr_current_cfar_info = 0;
	int16_t *ptr_raw_tar_list = 0;
	int16_t *ptr_raw_tar_list_hdr;
	uint32_t velocity_mm, veloc_resol;
	uint32_t range_mm, range_resol;
	    
    mmw_process_mem_alloc((void**)&ptr_current_cfar_res, HW_CFAR_RESULT_MAX_NUM * sizeof(*ptr_current_cfar_res));
    mmw_process_mem_alloc((void**)&ptr_current_cfar_info, HW_CFAR_RESULT_MAX_NUM * sizeof(*ptr_current_cfar_info));
	mmw_velocity_get(&velocity_mm, &veloc_resol);
	mmw_range_get(&range_mm, &range_resol);
	
    /* 
     * Get cfar result in hw.
     * SNR calculated in hw is not such accuracy, 
     * if more accuracy snr is needed, program should calculate snr by power & noise
    */
	mmw_cfar_result(ptr_current_cfar_res, ptr_current_cfar_info, &cfar_num, HW_CFAR_RESULT_MAX_NUM);

	if (cfar_num) {
		/* range_idx, dop(bin/s, Q4), snr(Q4), power(Q4) */
		mmw_process_mem_alloc((void**)&ptr_raw_tar_list, cfar_num * sizeof(*ptr_raw_tar_list) * 4);
		ptr_raw_tar_list_hdr = ptr_raw_tar_list;
		for (int cfar_idx = 0; cfar_idx < cfar_num; cfar_idx++) {
			(*ptr_raw_tar_list++) = ptr_current_cfar_res[cfar_idx].range_idx;
			(*ptr_raw_tar_list++) = ptr_current_cfar_res[cfar_idx].dop_idx;
			(*ptr_raw_tar_list++) = ptr_current_cfar_info[cfar_idx].snr * 16;
			(*ptr_raw_tar_list++) = ptr_current_cfar_info[cfar_idx].snr * 16;
		}
		mmw_psic_debug_proto_report((void*)ptr_raw_tar_list_hdr, "raw_tar_list_hw", 1, 4 *cfar_num, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
		mmw_process_mem_free((void**)&ptr_raw_tar_list_hdr);
	} else {
		mmw_psic_debug_proto_report((void*)ptr_raw_tar_list_hdr, "raw_tar_list_hw", 1, 0, PSIC_DBG_PROTO_DATA_FORMAT_SHORT, PSIC_DBG_PROTO_DATA_SIGNED, 0, 0);
	}

	mmw_process_mem_free((void**)&ptr_current_cfar_res);
	mmw_process_mem_free((void**)&ptr_current_cfar_info);
}


/* 
 * treat datacube as (doppler_num, rfft_num, rx_ant) matrix,
 * use reshape(datacube(:, : , 1).', mimo_rx_num, []) as random inputs,
 * each column which has 8 elements is a single test case
*/
void mmw_psic_lib_test_2nd_pass_filter(void)
{

	uint16_t range_fft_num, dop_fft_num;

    Complexf32_RealImag *ptr_buffer_raw;
    Complexf32_RealImag *ptr_buffer_f32 = 0;			/* buffer to store all range bins in one doppler bin */
	uint8_t *ptr_pass_flag = 0;
	uint8_t *ptr_pass_flag_hdr;
	int16_t peak_idx_diff = 0;
	int32_t *ptr_peak_idx_diff = 0;
	int32_t *ptr_peak_idx_diff_hdr;

    MmwPsicMimoRxNum_t mimo_info;
	complex16_cube* ptr_buffer_swap_q15 = 0;
	
    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
    mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
	mmw_process_mem_alloc((void**)&ptr_buffer_f32, sizeof(*ptr_buffer_f32) * range_fft_num);
	mmw_process_mem_alloc((void**)&ptr_buffer_swap_q15, sizeof(*ptr_buffer_swap_q15) * range_fft_num);
	mmw_process_mem_alloc((void**)&ptr_pass_flag, sizeof(*ptr_pass_flag) * range_fft_num * 2 / mimo_info.mimo_rx_num);
	ptr_pass_flag_hdr = ptr_pass_flag;
	mmw_process_mem_alloc((void**)&ptr_peak_idx_diff, sizeof(*ptr_peak_idx_diff) * range_fft_num * 2 / mimo_info.mimo_rx_num);
	ptr_peak_idx_diff_hdr = ptr_peak_idx_diff;

    mmw_motion_cube_access_open();
	for (uint16_t dop_idx = 0; dop_idx < dop_fft_num; dop_idx++) {
		mmw_process_fft_range_f32(ptr_buffer_f32, ptr_buffer_swap_q15, range_fft_num, 0, 0, dop_idx);
		for (uint16_t range_idx = 0; range_idx < range_fft_num; range_idx += mimo_info.mimo_rx_num) {
			/* apply calibration param */
			ptr_buffer_raw = &ptr_buffer_f32[range_idx];
			peak_idx_diff = mmw_psic_2nd_pass_filter((MmwPsicComplexf32_reim*)ptr_buffer_raw, PSIC_2ND_PASS_FILTER_PATH_0);
			*(ptr_pass_flag++) = peak_idx_diff < mmw_point_cloud_get_user_cfg_const()->mmw_motion_point_cloud_config.psic_2nd_pass_thres;
			*(ptr_peak_idx_diff++) = peak_idx_diff;

			peak_idx_diff = mmw_psic_2nd_pass_filter((MmwPsicComplexf32_reim*)ptr_buffer_raw, PSIC_2ND_PASS_FILTER_PATH_1);
			*(ptr_pass_flag++) = peak_idx_diff < mmw_point_cloud_get_user_cfg_const()->mmw_motion_point_cloud_config.psic_2nd_pass_thres;
			*(ptr_peak_idx_diff++) = peak_idx_diff;
		}
		ptr_pass_flag = ptr_pass_flag_hdr;
		mmw_psic_debug_proto_report((void*)ptr_pass_flag_hdr, "psic_2nd_pass_filter", 1, range_fft_num * 2 / mimo_info.mimo_rx_num, PSIC_DBG_PROTO_DATA_FORMAT_BYTE, PSIC_DBG_PROTO_DATA_UNSIGNED, 0, 0);
		ptr_peak_idx_diff = ptr_peak_idx_diff_hdr;
		mmw_psic_debug_proto_report((void*)ptr_peak_idx_diff_hdr, "psic_2nd_pass_peak_idx_diff", 1, range_fft_num * 2 / mimo_info.mimo_rx_num, PSIC_DBG_PROTO_DATA_FORMAT_LONG, PSIC_DBG_PROTO_DATA_UNSIGNED, 0, 0);
	}

    mmw_motion_cube_access_close();
	mmw_process_mem_free((void**)&ptr_buffer_swap_q15);
	mmw_process_mem_free((void**)&ptr_buffer_f32);
    mmw_process_mem_free((void**)&ptr_pass_flag);
	mmw_process_mem_free((void**)&ptr_peak_idx_diff);
}

/**
 * The test process is used to upload verification data via HIF(Host InterFace)
 * 
 */
void mmw_radar_point_cloud_test_process(void)
{
	/* print radar config to ensure the config is right */
	mmw_radar_point_cloud_printf_config();
	
	//mmw_process_fft_f32_test();

    /* 
	 * Upload data cube(complex), which is the basic input for all test step following
	 * If fixed-point datacube will be used, set parameters to 1.
	 * Fixed-point datacube will cost half transmition time.
	*/
    test_data_cube(0);
		
	/* Upload CFAR result of hardware, to check if hardware cfar matches datacube peak, and have proper snr */
	test_hw_cfar();

    /**
     * Upload mt noise.
     * The verification step of mt noise is to compare with data cube, and check
     * whether mt noise is resonable compared with data cube.
     * */
    test_mt_noise();
	
	/**
     * Upload result of 2nd pass filter .
	 *
     * */
	test_2nd_pass_filter();
	
	/**
	 * Peak grouping test.
	 * Test each range/doppler bin, check whether they are peak via all peakgrouping method.
	 * */
	test_peak_grouping();

    /**
     * Test angle calculate function, treat each data in data cube as a valid target.
     * The verification step of angle calculate function is simply rewrite this process in PC,
     * and check whether te error of these 2 methods are small enough.
     * */
    test_angle_calculate();
	
	/**
	 * Test 2nd pass filter, 
	 * 
	 * treat datacube as (doppler_num, rfft_num, rx_ant) matrix,
	 * use reshape(datacube(:, : , 1).', mimo_rx_num, []) as random inputs,
	 * each column which has 8 elements is a single test case
	*/
	mmw_psic_lib_test_2nd_pass_filter();

    /* auto gain function must be cleared manually, to make auto gain in next frame works well */
    mmw_psic_auto_gain_clear();
}

void mmw_point_cloud_process_debug_time_cal(void)
{
	MmwPsic2DFFTNum_t fft_num_info;
	uint16_t range_idx = 0;
	uint16_t doppler_idx = 0;
	Complexf32_RealImag ptr_ant_data[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
	MmwPsicMimoRxNum_t mimo_rx_info;
	float noise_pwr = 0.f;	
	float power_no_coherent_sum;
	Complexf32_RealImag ptr_ant_aligned[MMW_POINT_CLOUD_MAX_MIMO_RX_NUM];
	float sin_azi, sin_elev;
	uint32_t time_start = 0;
	uint32_t time_used_all[8] = {0,0,0,0,0,0,0,0};
	const MmwPointCloudUserCfg_t *ptr_mmw_user_cfg = mmw_point_cloud_get_user_cfg_const();
	mmw_fft_num_get(&fft_num_info.range_fft_num, &fft_num_info.dop_fft_num);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_rx_info);
	mmw_motion_cube_access_open();
	for(range_idx = 0; range_idx < fft_num_info.range_fft_num; range_idx++) {
		for (doppler_idx = 0; doppler_idx < fft_num_info.dop_fft_num; doppler_idx++) {
			
			/* run 1st pass filter, mainly filter out unwanted range */
			SET_TIME_START(time_start);			
            mmw_process_1st_pass_filter(range_idx, doppler_idx, &fft_num_info, ptr_mmw_user_cfg);
			ADD_TIME_ESCAPE(time_used_all[0], time_start);

			/*
             * re-calculate noise for specific range if range changed.
             * mmw_psic_mt_noise_get_db(range_idx) will cost a little
             * time, so it will be calculated only when range_idx changed. hw cfar output
             * is arranged by range, so this method can work well.
             */
            SET_TIME_START(time_start);	
			noise_pwr = mmw_psic_noise_get_f32(range_idx);
			ADD_TIME_ESCAPE(time_used_all[1], time_start);

			/* 
             * Read ant data here, after simplely bypass some points, because read ant data will cost a little time.
             * Better used float version of this API to align gain between different range and tx
             */
			SET_TIME_START(time_start);
            mmw_process_fft_ant_f32(ptr_ant_data, &mimo_rx_info, doppler_idx, range_idx);
			ADD_TIME_ESCAPE(time_used_all[2], time_start);
			
			/* 
             * Run 2nd pass filter here, after HW CFAR, because some points with SNR less than 6dB are retained.
             * 2nd pass filter is necessary. 15.85f is 12dB.
             */
			SET_TIME_START(time_start);
            mmw_process_2nd_pass_filter(ptr_ant_data, &mimo_rx_info, noise_pwr, ptr_mmw_user_cfg->mmw_motion_point_cloud_config.threshold_snr_pwr_lin, &power_no_coherent_sum);
			ADD_TIME_ESCAPE(time_used_all[3], time_start);;
			
			/* 
             * Run Peak grouping to reduce density of point cloud if cluster & track process will be performed,
             * parameter PEAKGROUPING_DOPPLER is recommanded to achieve a balance between performance and compution time.
             */
			PeakGroupingMethod_e grouping_method = ptr_mmw_user_cfg->mmw_motion_point_cloud_config.peak_grouping_method;
            if (grouping_method != PEAKGROUPING_DISABLE) {
				SET_TIME_START(time_start);	
				mmw_process_peak_grouping(range_idx, doppler_idx, &mimo_rx_info, &fft_num_info, power_no_coherent_sum, grouping_method);
				ADD_TIME_ESCAPE(time_used_all[4], time_start);
			}
			
			SET_TIME_START(time_start);
#if (CONFIG_SOC_SERIES_RS624X || CONFIG_SOC_SERIES_RS724X)
            /* 
             * Channel calibration should be manualy done for all mimo_rx, user may use their own calibration algorithm here. 
             * RS6130 use hardware calibration data, and is auto-calibrated when generate DataCube
             */
            mmw_process_ant_data_calibrate_f32(ptr_ant_data, ptr_ant_aligned, &g_rx_ant_calib_data[0], mimo_rx_info.mimo_rx_num);
#else
			memcpy(ptr_ant_aligned, ptr_ant_data, sizeof(ptr_ant_aligned));
#endif
			ADD_TIME_ESCAPE(time_used_all[5], time_start);
			
			/* 
             * Only if the aligned data passes 2nd pass filter, angle process will be applied
             * If compute time is enough, can use additional 'PSIC_2ND_PASS_FILTER_PATH_1' with logic 'AND' to enhance filter level.
            */
			SET_TIME_START(time_start);
            mmw_psic_2nd_pass_filter((MmwPsicComplexf32_reim*)ptr_ant_aligned, PSIC_2ND_PASS_FILTER_PATH_0);
			ADD_TIME_ESCAPE(time_used_all[6], time_start);
			
			/* Calcuate azimuth and elevation DoA with conventional angle fft */
            SET_TIME_START(time_start);
			mmw_process_angle_process(ptr_ant_aligned, &g_azi_geometry_struct, &g_elev_geometry_struct, &sin_azi, &sin_elev, 0, 0);
			ADD_TIME_ESCAPE(time_used_all[7], time_start);
		}
	}

	uint32_t point_num = fft_num_info.range_fft_num * fft_num_info.dop_fft_num;
	printk("mmw_process_1st_pass_filter cal time/us:%d\n",time_used_all[0] / (TIMES_CAL_FREQ * point_num));
	printk("mmw_psic_noise_get_f32 cal time/us:%d\n",time_used_all[1] / (TIMES_CAL_FREQ * point_num));
	printk("mmw_process_fft_ant_f32 cal time/us:%d\n",time_used_all[2] / (TIMES_CAL_FREQ * point_num));
	printk("mmw_process_2nd_pass_filter cal time/us:%d\n",time_used_all[3] / (TIMES_CAL_FREQ * point_num));
	printk("mmw_process_peak_grouping cal time/us:%d\n",time_used_all[4] / (TIMES_CAL_FREQ * point_num));
	printk("mmw_process_ant_data_calibrate_f32 cal time/us:%d\n",time_used_all[5] / (TIMES_CAL_FREQ * point_num));
	printk("mmw_psic_2nd_pass_filter cal time/us:%d\n",time_used_all[6] / (TIMES_CAL_FREQ * point_num));
	printk("mmw_process_angle_process cal time/us:%d\n",time_used_all[7] / (TIMES_CAL_FREQ * point_num));
	mmw_motion_cube_access_close();
}

extern void read_adc_suppression_from_bb(Complexf32_RealImag* ptr_buffer_f32, uint16_t data_len,
                            uint8_t tx_idx, uint8_t rx_idx);
                                
void mmw_point_cloud_test_dc_suppression()
{
    uint16_t range_fft_num, dop_fft_num;
    /* length of this buffer is range_fft_num * mimo_rx_num, it's used to store all range bin for each doppler bin */
    Complexf32_RealImag *ptr_buffer_f32 = 0;
	MmwPsicMimoRxNum_t mimo_info;
	complex16_cube* ptr_buffer_swap_q15 = 0;

    
    /* Upload 1d-fft data to see if dc is suppressed */
    mmw_fft_num_get(&range_fft_num, &dop_fft_num);
	mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
    mmw_process_mem_alloc((void**)&ptr_buffer_f32, sizeof(*ptr_buffer_f32) * range_fft_num);
    mmw_process_mem_alloc((void**)&ptr_buffer_swap_q15, sizeof(*ptr_buffer_swap_q15) * range_fft_num);
    mmw_motion_cube_access_open();

    mmw_process_fft_range_f32(ptr_buffer_f32, ptr_buffer_swap_q15, range_fft_num, 0, 0, dop_fft_num >> 1);
    for (uint16_t rfft_idx = 0; rfft_idx < range_fft_num; rfft_idx++) {
        ptr_buffer_f32[rfft_idx].real = ptr_buffer_f32[rfft_idx].real * 32768;
        ptr_buffer_f32[rfft_idx].imag = ptr_buffer_f32[rfft_idx].imag * 32768;
    }
    mmw_psic_debug_proto_report((void*)ptr_buffer_f32, "1d-fft", 1, 2 * range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);

    /* Upload dc suppression data to see dc value */
    read_adc_suppression_from_bb(ptr_buffer_f32, range_fft_num, 0, 0);
    for (uint16_t rfft_idx = 0; rfft_idx < range_fft_num; rfft_idx++) {
        ptr_buffer_f32[rfft_idx].real = ptr_buffer_f32[rfft_idx].real * 32768;
        ptr_buffer_f32[rfft_idx].imag = ptr_buffer_f32[rfft_idx].imag * 32768;
    }
    mmw_psic_debug_proto_report((void*)ptr_buffer_f32, "adc-suppression", 1, 2 * range_fft_num, PSIC_DBG_PROTO_DATA_FORMAT_FLOATING, PSIC_DBG_PROTO_DATA_F32, 0, 0);
    mmw_process_mem_free((void**)&ptr_buffer_f32);
    mmw_process_mem_free((void**)&ptr_buffer_swap_q15);

    mmw_motion_cube_access_close();
}