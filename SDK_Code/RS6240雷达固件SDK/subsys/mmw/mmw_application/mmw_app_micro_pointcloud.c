/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/************** Include Files **************/
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "mmw_point_cloud_psic_lib.h"

#include "csi_math.h"
#include "csi_math.h"
#include "csi_const_structs.h"
#include "mmw_alg_ant_calibration.h"
#include "mmw_alg_micro_cube.h"
#include "mmw_app_micro_pointcloud.h"
#include "mmw_alg_debug.h"

#define MPC_SHOW_DEBUG_PRINT_ENABLE     0 /* MPC debug print */
#if (MPC_SHOW_DEBUG_PRINT_ENABLE)
#define mpc_printf(fmt, arg...)         printk("[MPC]"fmt, ##arg)
#else
#define mpc_printf(fmt, arg...)
#endif  //MPC_SHOW_DEBUG_PRINT_ENABLE

/* USE RECTANGLE Window in micro doppler to increase peak in doppler */
#define MICRO_DOP_WIN_TYPE_RECT			(1) //hanning win would lost 6dB.
#define MICRO_DOP_FFT_GAIN_LOG2         (MICRO_DOP_NUM_LOG2/2 - 1) //16chirp = 12dB for fft gain compsation
#define MICRO_DOP_GAIN_LOG2             (2 + (MICRO_DOP_NUM_LOG2 & 0x1)) //16chirp = 12dB for fft gain compsation

#define MICRO_CFAR_SNR_TH               (16)  // 12(10log(16)) + 9.5(10log(9))= 18.5dB, hanning win 6dB lost.
#define MAX_DOP_PEAK_MAX_PER_RBIN       MICRO_DOP_NUM   //default

/* skip num of micro dop bin beside of velocity0 */
#define DOP_BIN_SKIP_0             0 //0 or 1
#define DOP_BIN_SKIP_BESIDE        0 //0~15
/* peak detect on 4 antannes */
#define DETECT_ANT_NUM_TH          ((MMW_ANT_CHANNEL_NUM + 1)>>1)

MPC_CTRL        g_mpc_ctrl;

static void micro_clutter_remove(complex16_cube *data_in, uint32_t chirp_num)
{
	int32_t clutter[2] = { 0 };
	/* calc average of chirps */
	for (int i = 0; i < chirp_num; ++i) {
		clutter[0] += data_in[i].real;
		clutter[1] += data_in[i].imag;
	}
	clutter[0] = clutter[0]/chirp_num;
	clutter[1] = clutter[1]/chirp_num;
	for (int i = 0; i < chirp_num; ++i) {
		data_in[i].real = data_in[i].real - clutter[0];
		data_in[i].imag = data_in[i].imag - clutter[1];
	}
}

#if (MICRO_DOP_WIN_TYPE_RECT == 0)
const int16_t hanning_win_4[] = {
	11321, 29639, 29639, 11321
};
const int16_t hanning_win_8[] = {
	3383, 13539, 24576, 31780, 31780, 24576, 13539, 3383
};
const int16_t hanning_win_16[] = {
	1106, 4276, 9081, 14872, 20868, 26258, 30314, 32489,
	32489, 30314, 26258, 20868, 14872, 9081, 4276, 1106
};
const int16_t hanning_win_32[] = {
	296, 1174, 2601, 4526, 6880, 9578, 12521, 15604,
	18716, 21743, 24576, 27113, 29263, 30947, 32104, 32694,
	32694, 32104, 30947, 29263, 27113, 24576, 21743, 18716,
	15604, 12521, 9578, 6880, 4526, 2601, 1174, 296
};
int micro_dop_win_init(uint32_t chirp_num)
{
	switch(chirp_num) {
	case 4:
		g_mpc_ctrl.hanning_win = &hanning_win_4[0];
		break;
	case 8:
		g_mpc_ctrl.hanning_win = &hanning_win_8[0];
		break;
	case 16:
		g_mpc_ctrl.hanning_win = &hanning_win_16[0];
		break;
	case 32:
		g_mpc_ctrl.hanning_win = &hanning_win_32[0];
		break;
	}
	return 0;
}
#endif

void micro_dop_window(complex16_cube *data_in, uint32_t chirp_num, complex16_mdsp *data_win, uint32_t mdop_num)
{
	int i = 0;
	const int16_t *win = g_mpc_ctrl.hanning_win;
	if (win == NULL) {
		while (i < chirp_num) {
			data_win[i].real = data_in[i].real << MICRO_DOP_FFT_GAIN_LOG2;
			data_win[i].imag = data_in[i].imag << MICRO_DOP_FFT_GAIN_LOG2;
			++i;
		}
	} else {
		while (i < chirp_num) { //"+1" for hanning win would lost 6dB.
			data_win[i].real = (((int32_t)data_in[i].real) * win[i])>>(15 - (MICRO_DOP_FFT_GAIN_LOG2 + 1));
			data_win[i].imag = (((int32_t)data_in[i].imag) * win[i])>>(15 - (MICRO_DOP_FFT_GAIN_LOG2 + 1));
			++i;
		}
	}
	while (i < mdop_num) {
		*(uint32_t *)&data_win[i++] = 0;
	}
}

int micro_dbf_cfar(complex16_mdsp *data_dbf, uint32_t dbf_num, Noisetype noise_level,
                       complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max)
{
	uint32_t point_idx = 0;
	uint32_t angel_num = 0;
	uint32_t max_idx[MMW_DBF_MAX_PEAK + 1];
	uint32_t max_peak[MMW_DBF_MAX_PEAK + 1] = {0};
	uint32_t max_peak_th;

#if (MPC_SHOW_DBF_FFT_DEBUG)
	mpc_printf("dbf noise=%d\n", noise_level);
#endif

	for (int i = 0; i < MMW_DBF_FFT_NUM; ++i) {
		/* noise threshold */
		uint32_t power_pre, power_next;
		uint32_t power = COMPLEX16_POWER(&data_dbf[i]);
		if (power < noise_level) {
			continue ;
		}
		/* stationary detect */
		power_pre  = COMPLEX16_POWER(&data_dbf[(i - 1) & (MMW_DBF_FFT_NUM - 1)]);
		power_next = COMPLEX16_POWER(&data_dbf[(i + 1) & (MMW_DBF_FFT_NUM - 1)]);
		if (power < power_next || power < power_pre) {
			continue ;
		}
		/* save peaks */
		for (int j = 0; j < buf_max; ++j) {
			if (power > max_peak[j]) {
				max_peak[j + 1] = max_peak[j];
				max_idx[j + 1]  = max_idx[j];
				max_peak[j] = power;
				max_idx[j]  = i;
				break;
			}
		}
	}

	max_peak_th = max_peak[0]/MMW_DBF_PEAK_SFDR;  //10dB
	while (max_peak[point_idx] > max_peak_th && point_idx < MMW_DBF_MAX_PEAK) {
		complex16_mdsp azim_phase, elev_phase;
		mmw_angle_bin_phase(&data_dbf[0], max_idx[point_idx], &azim_phase);
		mmw_angle_elev_phase_m62(ant_aligned, (uint32_t *)&elev_phase);
		mmw_angle_calculate((uint32_t *)&azim_phase, (uint32_t *)&elev_phase, angles);
		if (angles->elevation < ANGLE_ELEV_INVALID) {
			if (++angel_num < buf_max) {
				angles++;
			} else {
				break;
			}
		}
		point_idx++;
	}
	return angel_num;
}

static void micro_ant_data_align_2t4r(complex16_mdsp *fft_out, complex16_mdsp *ant_aligned, uint32_t ant_step)
{
	const complex16_mdsp *ant_calibrate_data = (complex16_mdsp *)mmw_angle_calib_data_get();
	for (int i = 0; i < MMW_ANT_CHANNEL_NUM; ++i) {
		complex16mdsp_mul16(fft_out, &ant_calibrate_data[i], &ant_aligned[i]);
		fft_out += ant_step;
	}
}

int micro_dop_cfar_doa(complex16_mdsp *fft_out, uint32_t range_idx, MicroPointHandle handler, void* arg)
{
	int ret = 0;
	uint32_t point_num = 0;
	uint32_t cfar_num = 0;
	uint8_t  range_station = g_mpc_ctrl.cfar_stationary & 0x1;
	uint8_t  dop_station   = (g_mpc_ctrl.cfar_stationary >> 1) & 0x1;
	uint32_t dop_cfar_th   = g_mpc_ctrl.cfar_snr_th;

	Noisetype *noiseth = micro_cube_noise_get(range_idx);

	/* 1. Get left and right data for range stationary detection */
  	complex16_mdsp *fft_range_left = NULL;
	complex16_mdsp *fft_range_right = NULL;
	if (range_station) {
		uint32_t buff_idx = range_idx % 3;
		if (buff_idx == 0) {
			fft_range_left	= fft_out + 2 * MDOP_FFT_BUF_NUM;
			fft_range_right	= fft_out + 1 * MDOP_FFT_BUF_NUM;
		} else if (buff_idx == 2) {
			fft_range_left	= fft_out + 1 * MDOP_FFT_BUF_NUM;
			fft_range_right	= fft_out;
			fft_out = fft_out + 2 * MDOP_FFT_BUF_NUM;
		} else { /* 1 */
			fft_range_left	= fft_out;
			fft_range_right	= fft_out + 2 * MDOP_FFT_BUF_NUM;
			fft_out = fft_out + 1 * MDOP_FFT_BUF_NUM;
		}
	}

	/* 2. CFAR on Doppler FFT result and perform DoA on CFAR result */
	for (int i = (DOP_BIN_SKIP_0 + DOP_BIN_SKIP_BESIDE); i < (MICRO_DOP_NUM - DOP_BIN_SKIP_BESIDE); ++i) {
		uint32_t is_peak = 0;
		uint32_t peak_power = 0;
		uint32_t peak_noise = 0;

		/* Calc left and right index for doppler stationary detection */
		uint32_t dop_prev = 0, dop_next = 0;
		if (dop_station) {
			dop_prev = (((uint32_t)i - 1) & (MICRO_DOP_NUM - 1));
			dop_next = (((uint32_t)i + 1) & (MICRO_DOP_NUM - 1));
		}

		/* Peak detection on each MIMO-RX. */
		for (int j = 0; j < MMW_ANT_CHANNEL_NUM; ++j) {

			/* noise threshold */
			uint32_t ant_base = j*MICRO_DOP_NUM;
			uint32_t data_idx = ant_base + i;
			uint32_t power_pre, power_next;

			/* Power compsation to get real noise level.
			 * Note: we do scale on fft_out because noise is too small and would be cut-off to 0 */
			uint32_t noise_threshold = ((noiseth[j] * dop_cfar_th)>>MICRO_CHIRP_NUM_LOG2);
			uint32_t power = COMPLEX16_POWER(&fft_out[data_idx])<<MICRO_DOP_GAIN_LOG2;

			/* Compare with noise threshold */
			if (power <= noise_threshold) {
				continue ;
			}

			/* Do doppler stationary detection */
			if (dop_station) {
				power_pre  = COMPLEX16_POWER(&fft_out[ant_base + dop_prev]);
				power_next = COMPLEX16_POWER(&fft_out[ant_base + dop_next]);
				power_pre  = (power_pre<<MICRO_DOP_GAIN_LOG2);
				power_next = (power_next<<MICRO_DOP_GAIN_LOG2);
				if (power < power_next || power < power_pre) {
					continue ;
				}
			}

			/* Record max peak power for calc SNR */
			if (power > peak_power) {
				peak_power = power;
				peak_noise = noise_threshold;
			}

			/* Peak filter out by half of MIMO num */
			if (++is_peak >= DETECT_ANT_NUM_TH) {
				if (range_station) {
					/* range stationary detect */
					power_pre  = COMPLEX16_POWER(&fft_range_left[data_idx]);
					power_next = COMPLEX16_POWER(&fft_range_right[data_idx]);
					power_pre  = (power_pre<<MICRO_DOP_GAIN_LOG2);
					power_next = (power_next<<MICRO_DOP_GAIN_LOG2);
					if (power <= power_next || power <= power_pre) {
						is_peak = 0;
					}
				}
				break;
			}
			(void)power_next; (void)power_pre;
		}

		/* DoA on CFAR peak. */
		if (is_peak >= DETECT_ANT_NUM_TH) {
			int angle_num;
			MmwAngleInfo_t angles[MMW_DBF_MAX_PEAK];
			uint32_t ant_aligned[MMW_ANT_CHANNEL_NUM];

			cfar_num++;
			/* SNR calculate, 'peak_noise+1' avoid peak_noise is 0 */
			uint32_t snr_linear = (((uint64_t)peak_power) * 9 * dop_cfar_th)/(peak_noise+1);

			/* Antenna aligned and Anlge DBF */
		#if (CONFIG_SOC_RS6130)
			for (int j = 0; j < MMW_ANT_CHANNEL_NUM; ++j) {
				ant_aligned[j] = *(uint32_t *)&fft_out[i + j*MICRO_DOP_NUM];
			}
		#else
			micro_ant_data_align_2t4r(&fft_out[i], (complex16_mdsp *)&ant_aligned[0], MICRO_DOP_NUM);
		#endif
			angle_num = mmw_angle_dbf((complex16_mdsp *)&ant_aligned[0], &angles[0], MMW_DBF_MAX_PEAK);

			/* Convert to point cloud result data */
			for (int k = 0; k < angle_num; ++k) {
				int veloc_idx;
				/* angle limit */
				if (angles[k].elevation >= g_mpc_ctrl.elev_filter_deg_q7[1] ||
					angles[k].elevation <= g_mpc_ctrl.elev_filter_deg_q7[0] ||
					angles[k].azimuth   >= g_mpc_ctrl.azim_filter_deg_q7[1] ||
					angles[k].azimuth   <= g_mpc_ctrl.azim_filter_deg_q7[0]) {
					continue;
				}
				veloc_idx = (i >= (MICRO_DOP_NUM >> 1) ? i - (MICRO_DOP_NUM) : i);
				ret = handler(range_idx, veloc_idx, snr_linear, &angles[k], arg);
				if (ret || ++point_num > g_mpc_ctrl.dop_peak_max) {
					goto exit;
				}
			}

			/* It's optimization, skip the next point to peak if stationary detetion. */
			i += dop_station;
		}
	}
exit:
	g_mpc_ctrl.dop_cfar_num += cfar_num;
	g_mpc_ctrl.points_num += point_num;
	return ret;
}

/* Note: mmw frames down sampling for micro frame.
 * return: TRUE if new micro frame is ready to process micro points .
 *         FALSE if new micro frame is not ready to process micro points.
 */
bool mmw_micro_point_frame(void)
{
    if (g_mpc_ctrl.frame_num == 0){
        /* set if auto gain is enabled at 1st frame */
        micro_cube_auto_gain_set(mmw_fft_autogain_get());
    }
	/* set frame div cnt */
	if (unlikely(g_mpc_ctrl.frame_div_cnt == 0)) { /* init frame_div_cnt */
		uint32_t period_ms = 100, dummy;
		mmw_frame_get(&period_ms,  &dummy);
		g_mpc_ctrl.frame_div_cnt = (1600/MICRO_CHIRP_NUM + (period_ms>>1))/period_ms;
		mpc_printf("micro frame div %d!\n", g_mpc_ctrl.frame_div_cnt);
	}

	/* down sampling */
	uint8_t frame_div_idx = g_mpc_ctrl.frame_div_idx++;
	if (g_mpc_ctrl.frame_div_idx >= g_mpc_ctrl.frame_div_cnt) {
		g_mpc_ctrl.frame_div_idx = 0;
	}
	if (frame_div_idx) {
		mpc_printf("micro skip frame %d!\n", frame_div_idx);
		return false;
	}

	/* micro chirp store */
	micro_cube_data_store();
	if (unlikely(!micro_cube_ready())) {
		return false;
	}
	return true;
}

uint32_t mmw_micro_point_process(MicroPointHandle handler, void* arg)
{
	uint32_t frame_num;
	uint32_t chirp_num;
	uint16_t range_idx, range_max, range_num;
	uint32_t fft_range_idx;
	uint32_t fft_range_pre;
	uint8_t  range_station = g_mpc_ctrl.cfar_stationary & 0x1;
	uint32_t time_start, time_dop = 0, time_cfar = 0;

	complex16_mdsp *micro_dop_buf = (complex16_mdsp *)g_mpc_ctrl.proc_bufs;
	if (unlikely(micro_dop_buf == NULL || handler == NULL)) {
		mpc_printf("data_bufs %p handler %p!\n", micro_dop_buf, handler);
		return 0;
	}

	/* Update micro point param */
	frame_num = g_mpc_ctrl.frame_num++;
	if (frame_num == 0) {
		uint16_t dop_num;
		uint32_t range_mm, range_bin_size;
		mmw_fft_num_get(&range_num, &dop_num);
		mmw_range_get(&range_mm, &range_bin_size);
		if (g_mpc_ctrl.range_min_mm || g_mpc_ctrl.range_max_mm) {
			g_mpc_ctrl.range_bin_skip = (uint32_t)range_num * g_mpc_ctrl.range_min_mm/range_mm;
			g_mpc_ctrl.range_bin_max  = (uint32_t)range_num * g_mpc_ctrl.range_max_mm/range_mm;
		}

		mpc_printf("[CFG] range=%d, skip=%d, max=%d!\n",
			range_num, g_mpc_ctrl.range_bin_skip, g_mpc_ctrl.range_bin_max);
	}
	micro_cube_range_get(&range_idx, &range_max);
	range_idx = MAX(range_idx, g_mpc_ctrl.range_bin_skip);
	range_max = MIN(range_max, g_mpc_ctrl.range_bin_max);

	/* calc left idx of the first range bin */
	if (range_station) {
		fft_range_idx = (range_idx > 0 ? range_idx - 1 : 0);
		fft_range_pre = fft_range_idx + NOISE_SEC_NUM;
	} else {
		fft_range_idx = range_idx;
	}

	g_mpc_ctrl.points_num   = 0;
	g_mpc_ctrl.dop_cfar_num = 0;
	chirp_num = g_mpc_ctrl.dop_chirp_num;
	do { /* for each range bin */
		SET_TIME_START(time_start);
		if (fft_range_idx < range_max) {

			complex16_mdsp *fft_out;
			if (range_station) {
				/* three ring buffers for range stationary detection */
				fft_out = micro_dop_buf + (fft_range_idx % 3) * MDOP_FFT_BUF_NUM;
			} else {
				fft_out = micro_dop_buf;
			}

			/* For each MIMO-RX */
			uint32_t ant_idx = 0;
			complex16_cube *data_chirps = micro_cube_chirps_get(fft_range_idx);
			do {
				/* 
				 * static clutter remove.
				 * even if rectangle window is used, clutter remove is needed, because 0s are padding
				*/
				micro_clutter_remove(data_chirps, chirp_num);

				/* apply hanning window and padding 0 */
				micro_dop_window(data_chirps, chirp_num, fft_out, MICRO_DOP_NUM);

				/* micro doppler fft */
			#if (MDSP_FFT_CALC_ENABLE)
				RunComplexFFT((uint32_t *)fft_out, MICRO_DOP_NUM_LOG2, (uint32_t *)fft_out);
			#elif (MICRO_DOP_NUM == 32)
				csi_cfft_q15(&csi_cfft_sR_q15_len32, (q15_t *)fft_out, 0, 1);
			#elif (MICRO_DOP_NUM == 16)
				csi_cfft_q15(&csi_cfft_sR_q15_len16, (q15_t *)fft_out, 0, 1);
			#endif

				/* update pointers of antanna data */
				fft_out     += MICRO_DOP_NUM;
				data_chirps += chirp_num;
			} while (++ant_idx < MMW_ANT_CHANNEL_NUM);

			/* Calc left bins of range stationary */
			fft_range_idx++;
			if (range_station && unlikely(fft_range_idx < fft_range_pre)) {
				continue;
			}
		}
		ADD_TIME_ESCAPE(time_dop, time_start);

		/* micro point cloud detection */
		SET_TIME_START(time_start);
		if (micro_dop_cfar_doa(micro_dop_buf, range_idx, handler, arg)) {
			break;
		}
		range_idx++;
		ADD_TIME_ESCAPE(time_cfar, time_start);
	} while (range_idx < range_max);

	UPDATE_TIME_DBG_INFO(time_dop, "mdop", g_time_dbg_info.micro_dop);
	UPDATE_TIME_DBG_INFO(time_cfar, "mcfar", g_time_dbg_info.micro_cfar);
	mpc_printf("F%d cfar=%d point=%d!\n", frame_num, g_mpc_ctrl.dop_cfar_num, g_mpc_ctrl.points_num);
	return g_mpc_ctrl.points_num;
}

uint32_t mmw_micro_point_num_get(void)
{
	return g_mpc_ctrl.points_num;
}

uint32_t mmw_micro_doppler_num_get(void)
{
	return MICRO_DOP_NUM;
}

/* Reset micro point cloud state at mmw restart. */
void mmw_micro_point_restart(void)
{
	g_mpc_ctrl.frame_div_idx = 0;
	g_mpc_ctrl.frame_div_cnt = g_mpc_ctrl.frame_div_cnt_set;
	g_mpc_ctrl.frame_num  = 0;
	g_mpc_ctrl.dop_cfar_num = 0;
	g_mpc_ctrl.points_num = 0;
	g_mpc_ctrl.dop_chirp_num = micro_cube_chirp_num_get();
	micro_cube_reset();
}

int mmw_micro_point_init(void)
{
	/* 1. Alloc memory for micro doppler fft buffer, 1.5K @ fft16, mimo8 */
	if (g_mpc_ctrl.proc_bufs == NULL) {
		uint32_t buff_size = sizeof(complex16_mdsp) * (MDOP_FFT_BUF_NUM*3);
		g_mpc_ctrl.proc_bufs = os_malloc(buff_size);
		if (g_mpc_ctrl.proc_bufs == NULL) {
			mpc_printf("proc_bufs malloc %d failed!\n", buff_size);
			goto err;
		}
	} else { /* inited already, just do nothing */
		return 0;
	}

	/* 2. Initial default parameters of micro point cloud. */
	g_mpc_ctrl.range_bin_max  = MICRO_RANGE_BIN_NUM;
	g_mpc_ctrl.range_bin_skip = MICRO_RANGE_BIN_SKIP_NUM;
	g_mpc_ctrl.dop_peak_max   = MAX_DOP_PEAK_MAX_PER_RBIN;
	g_mpc_ctrl.dop_chirp_num  = MICRO_CHIRP_NUM;
	g_mpc_ctrl.cfar_snr_th    = MICRO_CFAR_SNR_TH;
	g_mpc_ctrl.cfar_stationary = 0x1 | (0x1<<1); /* enable range and doppler stationary */
	/* angle filter */
	g_mpc_ctrl.azim_filter_deg_q7[0] = -ANGLE_AZIM_INVALID;
	g_mpc_ctrl.azim_filter_deg_q7[1] = ANGLE_AZIM_INVALID;
	g_mpc_ctrl.elev_filter_deg_q7[0] = -ANGLE_ELEV_INVALID;
	g_mpc_ctrl.elev_filter_deg_q7[1] = ANGLE_ELEV_INVALID;
#if (MICRO_DOP_WIN_TYPE_RECT == 0)
	micro_dop_win_init(MICRO_CHIRP_NUM);
#endif

	/* 3. Alloc memory for micro cube, 74K @ 8chirps.  */
	uint16_t micro_cube_skip_bin = (MICRO_RANGE_BIN_SKIP_NUM > 0 ? (MICRO_RANGE_BIN_SKIP_NUM - 1) : 0);
	if (micro_cube_init(micro_cube_skip_bin, MICRO_RANGE_BIN_NUM, MICRO_CHIRP_NUM, MMW_ANT_CHANNEL_NUM)) {
		mpc_printf("micro_cube_init failed!\n");
		goto err;
	}

	/* 4. Initial DoA calculate module. */
	mmw_angle_init();
	mpc_printf("mpc_init buf=%p\n", g_mpc_ctrl.proc_bufs);
	return 0;

err:
	if (g_mpc_ctrl.proc_bufs != NULL) {
		os_free(g_mpc_ctrl.proc_bufs);
		g_mpc_ctrl.proc_bufs = NULL;
	}
	return -1;
}

void mmw_micro_point_deinit(void)
{
	/* 1. Free all all resources */
	micro_cube_deinit();
	if (g_mpc_ctrl.proc_bufs) {
		os_free(g_mpc_ctrl.proc_bufs);
	}
	/* 2. Reset all parameters and state */
	memset(&g_mpc_ctrl, 0, sizeof(g_mpc_ctrl));
}

int mmw_micro_point_range_set (uint32_t min_range_mm, uint32_t max_range_mm)
{
	g_mpc_ctrl.range_min_mm = min_range_mm;
	g_mpc_ctrl.range_max_mm = max_range_mm;
	return 0;
}

int mmw_micro_point_range_get (uint32_t *min_range_mm, uint32_t *max_range_mm)
{
	if (min_range_mm == NULL || max_range_mm == NULL)
		return -1;
	*min_range_mm = g_mpc_ctrl.range_min_mm;
	*max_range_mm = g_mpc_ctrl.range_max_mm;
	return 0;
}

int mmw_micro_point_angle_set (int16_t min_azim_q7, int16_t max_azim_q7,
                                        int16_t min_elev_q7, int16_t max_elev_q7)
{
	g_mpc_ctrl.azim_filter_deg_q7[0] = MAX(min_azim_q7, -ANGLE_AZIM_INVALID);
	g_mpc_ctrl.azim_filter_deg_q7[1] = MIN(max_azim_q7, ANGLE_AZIM_INVALID);
	g_mpc_ctrl.elev_filter_deg_q7[0] = MAX(min_elev_q7, -ANGLE_AZIM_INVALID);
	g_mpc_ctrl.elev_filter_deg_q7[1] = MIN(max_elev_q7, ANGLE_AZIM_INVALID);
	return 0;
}

int mmw_micro_point_angle_get (int16_t *min_azim_q7, int16_t *max_azim_q7,
                                        int16_t *min_elev_q7, int16_t *max_elev_q7)
{
	*min_azim_q7 = g_mpc_ctrl.azim_filter_deg_q7[0];
	*max_azim_q7 = g_mpc_ctrl.azim_filter_deg_q7[1];
	*min_elev_q7 = g_mpc_ctrl.elev_filter_deg_q7[0];
	*max_elev_q7 = g_mpc_ctrl.elev_filter_deg_q7[1];
	return 0;
}

int mmw_micro_cfar_snr_set(uint32_t snr_linear)
{
	g_mpc_ctrl.cfar_snr_th = (snr_linear ? snr_linear : 1);
	return 0;
}

uint32_t mmw_micro_cfar_snr_get(void)
{
	return g_mpc_ctrl.cfar_snr_th;
}

int mmw_micro_cfar_stationary_set(bool range_sta, bool dop_sta)
{
	g_mpc_ctrl.cfar_stationary = range_sta | (((uint8_t)dop_sta)<<1);
	return 0;
}

int mmw_micro_cfar_stationary_get(bool *range_sta, bool *dop_sta)
{
	*range_sta = g_mpc_ctrl.cfar_stationary & 0x1;
	*dop_sta   = (g_mpc_ctrl.cfar_stationary>>1) & 0x1;
	return 0;
}

int mmw_micro_frame_rate_set(uint8_t mmw_frame_div_cnt)
{
	g_mpc_ctrl.frame_div_cnt_set = mmw_frame_div_cnt;
	g_mpc_ctrl.frame_div_cnt = mmw_frame_div_cnt;
	g_mpc_ctrl.frame_div_idx = 0;
	micro_cube_reset();
	return 0;
}

uint8_t mmw_micro_frame_rate_get(void)
{
	return g_mpc_ctrl.frame_div_cnt;
}

MPC_CTRL *mmw_micro_g_mpc_ctrl_get(void)
{
	return &g_mpc_ctrl;
}

void mmw_micro_frame_trans_radar_coord_to_user_coord(int16_t x, int16_t y, int16_t z, PointCloud_Cart* ptr_cart)
{
#if CONFIG_BOARD_MRS6130_P1812
    if (mmw_angle_mount_type_get() == MMW_MOUNT_VERTICAL) {
        ptr_cart->x = x;
        ptr_cart->y = y;
        ptr_cart->z = z;
    } else {
        ptr_cart->x = -x;
        ptr_cart->y = y;
        ptr_cart->z = -z;        
    }
#elif CONFIG_BOARD_MRS6130_P1806
    if (mmw_angle_mount_type_get() == MMW_MOUNT_VERTICAL) {
        ptr_cart->x = -x;
        ptr_cart->y = y;
        ptr_cart->z = -z;
    } else {
        ptr_cart->x = -x;
        ptr_cart->y = y;
        ptr_cart->z = -z;
    }
#elif (CONFIG_BOARD_MRS6240_P2512_CPUS || CONFIG_BOARD_MRS6240_P2512_CPUF)
    if (mmw_angle_mount_type_get() == MMW_MOUNT_VERTICAL) {
        ptr_cart->x  = -x;
        ptr_cart->y  = y;
        ptr_cart->z  = -z;
    } else {
        ptr_cart->x  = x;
        ptr_cart->y  = y;
        ptr_cart->z  = z;
    }
#elif (CONFIG_BOARD_MRS7241_P2840_CPUF || CONFIG_BOARD_MRS7241_P2828_CPUF)
    ptr_cart->x = x;
    ptr_cart->y = y;
    ptr_cart->z = z;
#else
    #error "Please set a valid coordinate transfor function" 
#endif    
}