/**
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
//#include "shell_port.h"
#include "shell.h"
#include "shell_cmd_group.h"
#include "mmw_ctrl.h"
#include "util.h"
#include "mmw_report.h"
#include "mmw_alg_debug.h"
#include "hif.h"

/* point cloud include files */
#include "mmw_app_micro_pointcloud.h"
#include "mmw_app_pointcloud.h"
#include "mmw_app_pointcloud_config.h"
#include "mmw_point_cloud_psic_lib.h"

#if (CONFIG_CHIP_INFO)
#include "chip_info.h"
#endif

#include "mmw_alg_ant_calibration.h"
#include "mmw_alg_doa.h"


#if (CONFIG_MMW_MICRO_POINT_CLOUD)
uint8_t g_micro_point_en = 1;
#endif

/* Set this to 1 in shell cmd, dc suppression is enabled during datacube and point cloud */
uint8_t g_dc_suppression_flag = 0;

#if (CONFIG_MMW_SHELL)

static uint8_t com_mode = 0;
static uint32_t com_rate;
uint8_t g_cfar_method = 0;		/* 0: hw cfar with snr threshold, 1: sw cfar */

uint32_t mmw_dbg_trace(void);
uint32_t mmw_dbg_step(uint8_t mmw_halt_type, uint32_t mmw_break_point);

#define MMW_PRINT_USE_SHELL   1

#if (MMW_PRINT_USE_SHELL)
#define mmw_print(shell, arg...) shellPrint(shell, arg);
#else
#define mmw_print(fmt, arg...) printf(fmt, ##arg)
#endif

uint32_t repot_frame_num;
uint8_t  gui_data_report = GUI_DATA_TYPE_DISABLE;

uint8_t  gui_data_report_last = GUI_DATA_TYPE_DISABLE;
static uint8_t com_mode_last = 0;
static uint32_t com_rate_last;
static uint8_t mmw_type_last = 0;;

const char *err_code_string[] = {
	"Success!",
	"Unsupported yet!",
	"Invalid param, please refer to Manual!",
	"Not ready, ensure open or wakeup!",
	"Running, please stop sensor first!",
	"Timeout"
};

const char *power_state_code[] = {
	"Power Down",
	"Active",
	"Sleep",
	"Running",
};

#define PRINT_ERR_CODE(shell)	do { \
	if (ret > 0 && ret < ARRAY_SIZE(err_code_string)) \
		mmw_print(shell, "ERR:%s\n", err_code_string[ret]); \
} while (0)

static int mmw_shell_data_show(void *mmw_data, void *arg);
static int cmd_mmw_open(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	bool use_msi = false;
	bool pll_lc  = false;

	if (argc > 1 && strcmp(argv[1], "msi") == 0) {
		use_msi = true;
	}
	if (argc > 2 && strcmp(argv[2], "none") == 0) {
		pll_lc = true;
	}
	ret = mmw_ctrl_open(use_msi, pll_lc, 1);
	PRINT_ERR_CODE(shell);

	return ret;
}
static int cmd_mmw_close(Shell *shell, int argc, char *argv[])
{
	mmw_ctrl_close();
    return 0;
}

static int cmd_mmw_verison(Shell *shell, int argc, char *argv[])
{
	int ret = 0;

	const char *ver_str = NULL;
	int build = 0;
	ver_str = mmw_version(NULL, (uint32_t *)&build);
	if (ver_str != NULL) {
		mmw_print(shell, "MMW version:%s @%8x\n", ver_str, build);
	} else {
		ret = MMW_ERR_CODE_NOT_READY;
		mmw_print(shell, "%s\n", err_code_string[ret]);
	}

	return ret;
}

static int cmd_mmw_start(Shell *shell, int argc, char *argv[])
{
	int ret = 0;
	if(gui_data_report >= GUI_DATA_TYPE_POINTS && gui_data_report < GUI_DATA_TYPE_DISABLE) {
        mmw_set_ant_align(1);       /* reset align register */
		mmw_point_cloud_init();
        /* CA-CFAR only support fix gain yet. */
        if (g_cfar_method == 1) {
            mmw_fft_autogain_set(0);
        } else if(g_cfar_method == 0) {
            mmw_fft_autogain_set(1);
        } else {
            ret = MMW_ERR_CODE_INVALID_PARAM;
        }
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
        mmw_micro_point_restart();
#endif
	} else {
        /* For DataCube Application, ant align is not enabled */
        mmw_set_ant_align(0);
		mmw_fft_autogain_set(0);
    }

	ret = mmw_ctrl_start();
	PRINT_ERR_CODE(shell);

    return ret;
}

static int cmd_mmw_stop(Shell *shell, int argc, char *argv[])
{
	if (gui_data_report >= GUI_DATA_TYPE_POINTS && gui_data_report < GUI_DATA_TYPE_DISABLE) {
		mmw_point_cloud_deinit();
		gui_data_report = GUI_DATA_TYPE_DISABLE;
	}
	mmw_sensor_stop();
	return 0;
}

static int cmd_mmw_state(Shell *shell, int argc, char *argv[])
{
	uint32_t frame_sent = 0;
	int ret = 0;
	ret = mmw_state(&frame_sent);
	if (ret >= 0 && ret < ARRAY_SIZE(power_state_code)) {
		mmw_print(shell, "%s, frame_sent=%d\n", power_state_code[ret], frame_sent);
	} else {
		mmw_print(shell, "Unknown state=%d\n", ret);
	}

	return 0;
}

static int cmd_mmw_powersave(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc > 1) {
		uint8_t enable = (uint8_t)atoi(argv[1]);
		if (enable) {
			ret = mmw_ctrl_low_power();
		} else {
			ret = mmw_ctrl_wake_up();
		}
		PRINT_ERR_CODE(shell);
	} else {
	    mmw_print(shell, "mmw ps %s\n", mmw_low_power_get() ? "enable":"disable");
	}

    return ret;
}

const char *mimo[MMW_MIMO_LAST] = {"1T1R", "1T3R", "2T3R", "1T4R", "2T4R", "1T2R"};
static int cmd_mmw_mode(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	uint8_t txrx;
	uint8_t work;
	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		mmw_mode_get (&txrx, &work);
		if (txrx < MMW_MIMO_LAST) {
			mmw_print(shell, "txrx_mode=%s(%d), work_mode=%s(%d)\n", mimo[txrx], txrx,
				(MMW_WORK_MODE_1DFFT == work ? "1DFFT" : "2DFFT"), work);
		}
		return 0;
	} else if (argc > 2) {
		txrx = (uint8_t)atoi(argv[1]);
		work = (uint8_t)atoi(argv[2]);
		ret  =  mmw_mode_cfg (txrx, work);
		PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "ERR: less param, see help\n");
	}

	return ret;
}

static int cmd_mmw_freq(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	uint32_t start;
	uint32_t max = 0;
	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		mmw_freq_get (&start, &max);
		mmw_print(shell, "FreqStart=%dMHz, Max=%dMHz\n", start, max);
		return 0;
	}
	if (argc > 1) {
		start = (uint32_t)atoi(argv[1]);
		if (argc > 2)
			max = (uint32_t)atoi(argv[2]);
		ret  =  mmw_freq_cfg (start, max);
		PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "ERR: less param, see help\n");
	}

	return ret;
}

static int cmd_mmw_range(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint32_t range;
		uint32_t resol;
		mmw_range_get (&range, &resol);
		mmw_print(shell, "range=%dmm, resol=%dmm\n", range, resol);
		return 0;
	} else if (argc > 2) {
		uint32_t range = (uint32_t)atoi(argv[1]);
		uint32_t resol = (uint32_t)atoi(argv[2]);
		ret = mmw_range_cfg(range, resol);
		PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "ERR: less param, see help\n");
	}

	return ret;
}

static int cmd_mmw_veloc(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint32_t veloc;
		uint32_t resol;
		mmw_velocity_get (&veloc, &resol);
		mmw_print(shell, "veloc=%dmm/s, resol=%dmm/s\n", veloc, resol);
		return 0;
	} else if (argc > 2) {
		uint32_t veloc = (uint32_t)atoi(argv[1]);
		uint32_t resol = (uint32_t)atoi(argv[2]);
		ret = mmw_velocity_cfg(veloc, resol);
		PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "ERR: less param, see help\n");
	}

	return ret;
}

static int cmd_mmw_intv(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint32_t period;
		uint32_t intv_num;
		mmw_interval_get (&period, &intv_num);
		mmw_print(shell, "period=%dus, intv_num=%d\n", period, intv_num);
		return 0;
	} else  if (argc > 2) {
		uint32_t period = (uint32_t)atoi(argv[1]);
		uint32_t intv_num = (uint32_t)atoi(argv[2]);
		ret = mmw_interval_cfg(period, intv_num);
		PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "ERR: less param, see help\n");
	}

	return ret;
}

static int cmd_mmw_frame(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint32_t period;
		uint32_t num;
		mmw_frame_get (&period, &num);
		mmw_print(shell, "period=%dms, frame_num=%d\n", period, num);
		return 0;
	} else if (argc > 2) {
		uint32_t period = (uint32_t)atoi(argv[1]);
		uint32_t num = (uint32_t)strtoul(argv[2], NULL, 10);
		ret = mmw_frame_cfg(period, num);
		PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "ERR: less param, see help\n");
	}

	return ret;
}

static int cmd_mmw_chirp_param(Shell *shell, int argc, char *argv[])
{
	int ret = 0;

	SweepChirpInfo chParam;
	if (argc != 1) {
		return -EINVAL;
	}

	ret = mmw_sweep_chirp_info_get(&chParam);
	if (ret) {
		PRINT_ERR_CODE(shell);
		return ret;
	}

	mmw_print(shell, "Chirp: start_freq=%uMHz\nend_freq=%uMHz\nslope=%uMHz/us\nsweep_BW=%uMHz\n",
		chParam.start_freq, chParam.max_freq, chParam.cfg.slope_MHz_us, chParam.cfg.sweep_BW_MHz);
	mmw_print(shell, "valid_start=%uMHz\nvalid_BW=%uMHz\nsample_time=%uus\n",
		chParam.cfg.valid_start_MHz, chParam.cfg.valid_BW_MHz, chParam.sample_time);
	mmw_print(shell, "ADC num=%u\nrame_time=%uus\nperiod=%uns\n",
		chParam.cfg.sample_num, chParam.ramp_time, chParam.cfg.period_ns);

	return 0;
}

static int cmd_mmw_chirp_num(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		CHIRP_INFO chirp_info;
		ret = mmw_chirp_get(&chirp_info);
		mmw_print(shell, "chirp num=%d\n", chirp_info.chirp_num);
		return 0;
	} else if (argc > 1) {
		uint32_t num = (uint32_t)atoi(argv[1]);
		ret = mmw_chirp_num_cfg(num);
		PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "ERR: less param, see help\n");
	}

	return ret;
}

static int cmd_mmw_fft_len(Shell *shell, int argc, char *argv[])
{
	int ret = 0;

	uint16_t range_len, doppler_len;
	ret = mmw_fft_num_get(&range_len, &doppler_len);
	if (!ret) {
		mmw_print(shell, "Range FFT: %d, Doppler FFT: %d\n", range_len, doppler_len);
	} else {
		PRINT_ERR_CODE(shell);
	}

	return ret;
}

const char *coord_name[2] = {"polar", "cart"};
static int cmd_mmw_coord(Shell *shell, int argc, char *argv[])
{
	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint8_t  coordinate = 0;
		coordinate = mmw_coordinate_get();
		mmw_print(shell, "coord=%s\n", coord_name[coordinate]);
		return 0;
	} else {
		int i = 0;
		int ret = -1;
		for (; i < ARRAY_SIZE(coord_name); i++) {
			if (strcmp(argv[1], coord_name[i]) == 0) {
				ret = mmw_coordinate_config(i);
				break;
			}
		}
		PRINT_ERR_CODE(shell);
		return ret;
	}

	return 0;
}

const char *data_type_name[] = {
	"disable", "adc", "fft", "fft",
	"undef", "undef", "3dpoints",
	"undef", "2dtrack", "undef", "motion",
	"on-off"
};

static bool    callback_restore;
static void   *arg_restore;
static uint8_t data_type_restore;
static MMW_HANDLE handler_restore;

static int cmd_mmw_datacfg(Shell *shell, int argc, char *argv[])
{

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint8_t  data_callback = mmw_ctrl_data_type_get();
		mmw_print(shell, "data callback=%s\n", data_type_name[data_callback]);
		return 0;
	} else if (strcmp(argv[1], "restore") == 0) {
		if (callback_restore) {
			mmw_ctrl_callback_cfg(handler_restore, data_type_restore, arg_restore);
			callback_restore = false;
			mmw_print(shell, "callback restore handler@%p type=%d arg=%p\n",
				handler_restore, data_type_restore, arg_restore);
		}
		return 0;
	} else {
		int i = 0;
		int ret = -1;
		for (; i < ARRAY_SIZE(data_type_name); i++) {
			if (strcmp(argv[1], data_type_name[i]) == 0) {
				if (!callback_restore) {
					handler_restore = mmw_ctrl_callback_get(&data_type_restore, &arg_restore);
					callback_restore = true;
				}
				if (strcmp(argv[1], "fft") == 0) {
					uint8_t txrx = 0;
					uint8_t work = 0;
					mmw_mode_get (&txrx, &work);
					ret = mmw_ctrl_callback_cfg(mmw_shell_data_show,
						                        ((MMW_WORK_MODE_1DFFT == work) ? i:i + 1),
						                        NULL);
				} else {
					ret = mmw_ctrl_callback_cfg(mmw_shell_data_show, i, NULL);
				}
				break;
			}
		}
		PRINT_ERR_CODE(shell);
		return ret;
	}

	return 0;
}

static void print_fft_data(Complex16_RealImag *fft)
{
	printf("%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d,%d %d\n",
		fft[0].imag, fft[0].real, fft[1].imag, fft[1].real,
		fft[2].imag, fft[2].real, fft[3].imag, fft[3].real,
		fft[4].imag, fft[4].real, fft[5].imag, fft[5].real,
		fft[6].imag, fft[6].real, fft[7].imag, fft[7].real);
}

static int cmd_mmw_cfar(Shell *shell, int argc, char *argv[])
{
	float snr_db, snr_pwr;
	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint32_t snr = 0;
		bool static_rm = 0;
		mmw_cfar_get(&static_rm, &snr);	/* snr U5Q3 */
		snr_db = log10f(snr / 8.f) * 20;
		mmw_print(shell, "cfar static_rm=%d snr=%d\n", static_rm, (int16_t)roundf(snr_db));
		return 0;
	} else {
		bool static_rm = (bool)atoi(argv[1]);
		uint32_t snr = (uint32_t)atoi(argv[2]);
		int ret = 0;
		snr_pwr = powf(10, snr / 20.f);
		ret = mmw_cfar_cfg(static_rm, roundf(snr_pwr * 8));
        mmw_point_cloud_get_user_cfg()->mmw_motion_point_cloud_config.static_rm = static_rm;
		mmw_point_cloud_get_user_cfg()->mmw_motion_point_cloud_config.threshold_snr_pwr_lin = powf(10, snr * 0.1f);
        if (argc > 3) {
            /* parameter 3: CFAR method select, 0: snr/1: CA-CFAR */
            if (atoi(argv[3]) > 1) {
                ret = MMW_ERR_CODE_INVALID_PARAM;
            } else {
                g_cfar_method = atoi(argv[3]);
            }
        }
		PRINT_ERR_CODE(shell);
		return ret;
	}

	return 0;
}

static int cmd_mmw_mount(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		switch(mmw_point_cloud_get_user_cfg()->mount_type) {
			case MMW_MOUNT_VERTICAL:
#if CONFIG_BOARD_MRS6130_P1806
				mmw_print(shell, "mount horizon\n");
#else
				mmw_print(shell, "mount vertical\n");
#endif
				break;
			case MMW_MOUNT_HORIZONTAL:
#if CONFIG_BOARD_MRS6130_P1806
				mmw_print(shell, "mount horizon\n");
#else
				mmw_print(shell, "mount vertical\n");
#endif			
				break;
			default:
				mmw_print(shell, "mount ERROR!\n");
				break;
		}
		ret = 0;
	} else if (argc == 2) {
		if (strcmp(argv[1], "vertical") == 0) {
		#if CONFIG_BOARD_MRS6130_P1806
			/* for board P1806, when board is "vertical", chip is "horizontal" */
			mmw_point_cloud_get_user_cfg()->mount_type = MMW_MOUNT_HORIZONTAL;
			mmw_angle_mount_type_set(MMW_MOUNT_HORIZONTAL);
		#else
			mmw_point_cloud_get_user_cfg()->mount_type = MMW_MOUNT_VERTICAL;
			mmw_angle_mount_type_set(MMW_MOUNT_VERTICAL);
		#endif
			ret = 0;
		} else if (strcmp(argv[1], "horizon") == 0) {
		#if CONFIG_BOARD_MRS6130_P1806
			/* for board P1806, when board is "vertical", chip is "horizontal" */
			mmw_point_cloud_get_user_cfg()->mount_type = MMW_MOUNT_VERTICAL;
			mmw_angle_mount_type_set(MMW_MOUNT_VERTICAL);
		#else
			mmw_point_cloud_get_user_cfg()->mount_type = MMW_MOUNT_HORIZONTAL;
			mmw_angle_mount_type_set(MMW_MOUNT_HORIZONTAL);
		#endif
			ret = 0;
		} else {
			/* ceiling is not supportted yet */
			;
		}
	}
	return ret;
}


static int cmd_mmw_dc_suppression(Shell *shell, int argc, char *argv[])
{
    if (argc == 1 || strcmp(argv[1], "get") == 0) {
        mmw_print(shell, "dc suppression: %d", g_dc_suppression_flag);
    } else if (argc == 2) {
        g_dc_suppression_flag = (bool)atoi(argv[1]);
		if (!g_dc_suppression_flag) {
			mmw_clutter_halt_set(MMW_HALT_CLUTTER_UPDATE_DISABLE);
		}
    }
    return 0;
}

static int cmd_mmw_clutter_rm(Shell *shell, int argc, char *argv[])
{
	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		uint8_t clutter_fac = 0;
		uint8_t  enable_mask = mmw_clutter_remove_get();
		mmw_clutter_factor_get(&clutter_fac);
		mmw_print(shell, "clutter_remove=%d, fac=%d\n", !!enable_mask, clutter_fac);
		return 0;
	} else {
		int ret = 0;
		uint8_t enable = (atoi(argv[1]) ? MMW_CLUTTER_MODE_ENABLE  : MMW_CLUTTER_MODE_DISABLE);
		if (argc > 2) {
			uint8_t clutter_fac = atoi(argv[2]);
			ret = mmw_clutter_factor_cfg(clutter_fac);
		}
		if (!ret)
			ret = mmw_clutter_remove(enable);
		PRINT_ERR_CODE(shell);
		return ret;
	}
	return 0;
}

static int cmd_mmw_rx_gain(Shell *shell, int argc, char *argv[])
{
    int ret = -1;

    if (argc < 3) {
		mmw_print(shell, "ERR: less param, see help\n");
        return ret;
	}

	if (strcmp(argv[1], "rf") == 0) {
        uint8_t ant_id = atoi(argv[2]);
		uint8_t gain = atoi(argv[3]);

        ret = mmw_rx_ana_gain_cfg(ant_id, gain);
	} else if (strcmp(argv[1], "dig") == 0) {
        uint8_t ant_id = atoi(argv[2]);
		int8_t gain = atoi(argv[3]);

        ret = mmw_rx_dig_gain_adj(ant_id, gain);
	}

    PRINT_ERR_CODE(shell);

    return ret;
}

static int cmd_mmw_ana_hpf_bw(Shell *shell, int argc, char *argv[])
{
    int ret = -1;

    if (argc < 2) {
		mmw_print(shell, "ERR: less param, see help\n");

        return ret;
	}

	uint8_t ant_id = atoi(argv[1]);
	uint8_t bw = atoi(argv[2]);

    ret = mmw_ana_hpf_bw_cfg(ant_id, bw);

    PRINT_ERR_CODE(shell);

    return ret;
}

static int cmd_mmw_read(Shell *shell, int argc, char *argv[])
{
	int ret = 0;

	uint16_t read_num = 8;
	uint16_t offset = 0;
	Complex16_RealImag fft[8];
	if (argc > 1) {
		read_num = (uint16_t)atoi(argv[1]);
		if (argc > 2) {
			offset = (uint16_t)atoi(argv[2]);
		}
	}
	read_num = read_num>>3;
	for (int i = 0; i < read_num; ++i) {
		ret = mmw_fft_data(&fft[0], 8, 0, 0, offset);
		if (!ret) {
			print_fft_data(fft);
		} else {
			PRINT_ERR_CODE(shell);
			break;
		}
		offset += 8;
	}

	return ret;
}

static int cmd_mmw_report(Shell *shell, int argc, char *argv[])
{
	uint8_t txrx = 0;
	uint8_t work = 0;

	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		char *report_type;
		switch (gui_data_report) {
		case GUI_DATA_TYPE_CUBE:
			report_type = "cube";
			break;
		case GUI_DATA_MMIC:
			report_type = "mmic";
		case GUI_DATA_TYPE_POINTS:
			report_type = "points";
			break;
		case GUI_DATA_TYPE_TRACKING:
			report_type = "track";
			break;
		case GUI_DATA_TYPE_POINTS_TRACKING:
			report_type = "points-track";
			break;
		case GUI_DATA_TYPE_DISABLE:
		default:
			report_type = "disable";
			break;
		}
		mmw_print(shell, "gui report=%s, num=%u\n", report_type, repot_frame_num);
		return 0;
	} else {
		int ret = -1;
		if (argc > 2) {
			repot_frame_num = (uint32_t)strtoul(argv[2], NULL, 10);
//            mmw_print(shell, "num=%u\n", repot_frame_num);
		} else {
			uint32_t period;
			mmw_frame_get (&period, &repot_frame_num);
		}
		if (strcmp(argv[1], "disable") == 0) {
			gui_data_report = GUI_DATA_TYPE_DISABLE;
		} else if (strcmp(argv[1], "cube") == 0) {
			gui_data_report = GUI_DATA_TYPE_CUBE;
		} else if (strcmp(argv[1], "mmic") == 0) {
			gui_data_report = GUI_DATA_MMIC;
		} else if (strcmp(argv[1], "points") == 0) {
			gui_data_report = GUI_DATA_TYPE_POINTS;
		} else if (strcmp(argv[1], "track") == 0) {
			gui_data_report = GUI_DATA_TYPE_TRACKING;
		} else if (strcmp(argv[1], "points-track") == 0) {
			gui_data_report = GUI_DATA_TYPE_POINTS_TRACKING;
		} else {
			mmw_print(shell, "Unsupport Cmd %s\n", argv[1]);
			return ret;
		}
		ret = mmw_report_gui(gui_data_report, 2048, repot_frame_num);
		mmw_mode_get (&txrx, &work);
		uint8_t type = work == MMW_WORK_MODE_1DFFT ? MMW_DATA_TYPE_1DFFT : MMW_DATA_TYPE_2DFFT;

        do {
            if (gui_data_report == GUI_DATA_TYPE_CUBE) {
                if (com_mode == 1) {
                    if ((gui_data_report_last == GUI_DATA_TYPE_POINTS) ||
                        (gui_data_report_last == GUI_DATA_TYPE_TRACKING) ||
                        (gui_data_report_last == GUI_DATA_TYPE_POINTS_TRACKING)) {
                        mmw_data_report_hif_deinit();
                    } else if (gui_data_report_last == GUI_DATA_TYPE_CUBE) {
                        if (com_mode_last != 1) {
                            mmw_data_report_hif_deinit();
                        } else {
                            /* already init */
                            mmw_report_param_get();
                            if(mmw_type_last != type) {
                                mmw_ctrl_callback_cfg(&mmw_ctrl_data_cube_spi_report_cb, type, NULL);
                            }
                            break;
                        }
                    }

                    mmw_report_param_get();
                    mmw_data_report_hif_init(HIF_COM_TYPE_SPI, com_rate, 1);
                    mmw_ctrl_callback_cfg(&mmw_ctrl_data_cube_spi_report_cb, type, NULL);
                } else if (com_mode == 0) {
                    if ((gui_data_report_last == GUI_DATA_TYPE_POINTS) ||
                        (gui_data_report_last == GUI_DATA_TYPE_TRACKING) ||
                        (gui_data_report_last == GUI_DATA_TYPE_POINTS_TRACKING)) {
                        mmw_data_report_hif_deinit();
                    } else if (gui_data_report_last == GUI_DATA_TYPE_CUBE) {
                        if (com_mode_last == 1) {
                            mmw_data_cube_spi_report_deinit();
                        } else {
                            /* already init */
                            if (com_rate == com_rate_last) {
                                mmw_report_param_get();
                                if(mmw_type_last != type) {
                                    mmw_ctrl_callback_cfg(&mmw_ctrl_data_cube_uart_report_cb, type, NULL);
                                }
                                break;
                            }
                            mmw_data_report_hif_deinit();
                        }
                    }

                    mmw_report_param_get();
                    mmw_data_report_hif_init(HIF_COM_TYPE_UART, com_rate, 1);
                    mmw_ctrl_callback_cfg(&mmw_ctrl_data_cube_uart_report_cb, type, NULL);
                }
            } else if ((gui_data_report == GUI_DATA_TYPE_POINTS) ||
                (gui_data_report == GUI_DATA_TYPE_TRACKING) ||
                (gui_data_report == GUI_DATA_TYPE_POINTS_TRACKING)) {
                if (com_mode == 1) {
                    /* spi mode */
                    if ((gui_data_report_last == GUI_DATA_TYPE_POINTS) ||
                        (gui_data_report_last == GUI_DATA_TYPE_TRACKING) ||
                        (gui_data_report_last == GUI_DATA_TYPE_POINTS_TRACKING)) {
                        if (com_mode_last == 1) {
                            mmw_report_param_get();
                            if(mmw_type_last != type) {
                                mmw_ctrl_callback_cfg(&mmw_ctrl_data_point_report_cb, type, NULL);
                            }
                            break;
                        } else if (com_mode_last == 0) {
                            mmw_data_report_hif_deinit();
                        }
                        mmw_data_report_hif_deinit();
                    } else if (gui_data_report_last == GUI_DATA_TYPE_CUBE) {
                        if (com_mode_last == 1) {
                            mmw_data_cube_spi_report_deinit();
                        } else if (com_mode_last == 0) {
                            mmw_data_report_hif_deinit();
                        }
                    }

                    mmw_report_param_get();
                    mmw_data_report_hif_init(HIF_COM_TYPE_SPI, com_rate, 0);
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
                    mmw_micro_point_init();
#endif
                    mmw_ctrl_callback_cfg(&mmw_ctrl_data_point_report_cb, type, NULL);
                } else if (com_mode == 0) {
                    /* uart mode */
                    if ((gui_data_report_last == GUI_DATA_TYPE_POINTS) ||
                        (gui_data_report_last == GUI_DATA_TYPE_TRACKING) ||
                        (gui_data_report_last == GUI_DATA_TYPE_POINTS_TRACKING)) {
                        if (com_mode_last == 1) {
                            mmw_data_report_hif_deinit();
                        } else if (com_mode_last == 0) {
                            if (com_rate == com_rate_last) {
                                mmw_report_param_get();
                                if(mmw_type_last != type) {
                                    mmw_ctrl_callback_cfg(&mmw_ctrl_data_point_report_cb, type, NULL);
                                }
                                break;
                            }
                        }
                        mmw_data_report_hif_deinit();
                    } else if (gui_data_report_last == GUI_DATA_TYPE_CUBE) {
                        if (com_mode_last == 1) {
                            mmw_data_cube_spi_report_deinit();
                        } else if (com_mode_last == 0) {
                            mmw_data_report_hif_deinit();
                        }
                    }

                    mmw_report_param_get();
                    mmw_data_report_hif_init(HIF_COM_TYPE_UART, com_rate, 0);
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
                    mmw_micro_point_init();
#endif
                    mmw_ctrl_callback_cfg(&mmw_ctrl_data_point_report_cb, type, NULL);

                }
            }
        } while (0);

        gui_data_report_last = gui_data_report;
        com_mode_last = com_mode;
        com_rate_last = com_rate;
        mmw_type_last = type;
		/* reset the skipped frames status */
		radar_analysis_reset_skip_frame_status();
		
		PRINT_ERR_CODE(shell);
		return ret;
	}

	return 0;
}

static int cmd_mmw_baudrate(Shell *shell, int argc, char *argv[])
{
	// int ret = 0;

	com_rate = (uint32_t)atoi(argv[1]);

	// ret = HIF_ExtControl(HIF_SET_COM_RATE, &comRate, sizeof(comRate));

	return 0;
}

static int cmd_mmw_dbg(Shell *shell, int argc, char *argv[])
{
	mmw_print(shell, "Trace=0x%08X\n", mmw_dbg_trace());
	if (argc > 1) {
		uint8_t  dbg_mmw_halt_type   = 0;
		uint32_t dbg_mmw_break_point = 0;

		int dbg_mmw_halt = (uint8_t)atoi(argv[1]);
		if (dbg_mmw_halt == 2 && argc > 2) {
			dbg_mmw_halt_type   = 2;
			dbg_mmw_break_point = (uint32_t)strtol(argv[2], NULL, 16);
		} else {
			dbg_mmw_halt_type = !!dbg_mmw_halt;
		}
		mmw_dbg_step(dbg_mmw_halt_type, dbg_mmw_break_point);
	}

	return 0;
}

static int cmd_mmw_ant_calib(Shell *shell, int argc, char *argv[])
{
	if (argc > 1) {
		if (strcmp(argv[1], "calib") == 0) {
			if (argc > 3) {
				uint16_t start = (uint16_t)atoi(argv[2]);
				uint16_t end   = (uint16_t)atoi(argv[3]);
				mmw_ant_calibration(start, end);
			} else {
				mmw_print(shell, "Less param!\n");
			}
		} else if (strcmp(argv[1], "save") == 0) {
			bool save_or_clear = (argc > 2 && (strcmp(argv[2], "clear") == 0));
			mmw_ant_calib_data_save(save_or_clear);
		} else if (strcmp(argv[1], "load") == 0) {
			mmw_ant_calib_data_load();
		} else if (strcmp(argv[1], "calib_get") == 0) {
			uint32_t calib_data[8] = {0};
			mmw_ant_calib_data_get(&calib_data[0], sizeof(calib_data));
			mmw_print(shell, "%d + %dj, %d + %dj, %d + %dj, %d + %dj, %d + %dj, %d + %dj, %d + %dj, %d + %dj\n",
					(int16_t)(calib_data[0] & 0xFFFF), ((int16_t)(calib_data[0] >> 16)),
					(int16_t)(calib_data[1] & 0xFFFF), ((int16_t)(calib_data[1] >> 16)),
					(int16_t)(calib_data[2] & 0xFFFF), ((int16_t)(calib_data[2] >> 16)),
					(int16_t)(calib_data[3] & 0xFFFF), ((int16_t)(calib_data[3] >> 16)),
					(int16_t)(calib_data[4] & 0xFFFF), ((int16_t)(calib_data[4] >> 16)),
					(int16_t)(calib_data[5] & 0xFFFF), ((int16_t)(calib_data[5] >> 16)),
					(int16_t)(calib_data[6] & 0xFFFF), ((int16_t)(calib_data[6] >> 16)),
					(int16_t)(calib_data[7] & 0xFFFF), ((int16_t)(calib_data[7] >> 16)));
		} else if (strcmp(argv[1], "calib_set") == 0) {
			uint32_t calib_data[8];
			uint32_t data_num = 0;
			for (int i = 2; i < argc; ++i, ++data_num) {
				calib_data[data_num] = (uint32_t)strtol(argv[i], NULL, 16);
			}
			mmw_ant_calib_data_set(&calib_data[0], sizeof(uint32_t)*data_num);
		} else if (strcmp(argv[1], "calib_on") == 0) {
			mmw_ant_hw_calib_cfg(1);
		} else if (strcmp(argv[1], "calib_off") == 0) {
			mmw_ant_hw_calib_cfg(0);
		} else {
			int mimo_type;
			uint32_t test_type = 0;
			if (strcmp(argv[1], "m81") == 0) {
				mimo_type = 0;
			} else if (strcmp(argv[1], "m62") == 0) {
				mimo_type = 1;
			} else if (strcmp(argv[1], "m62_azim") == 0) {
				mimo_type = 1;
				test_type = 1;
			} else if (strcmp(argv[1], "m62_elev") == 0) {
				mimo_type = 1;
				test_type = 2;
			} else if (strcmp(argv[1], "a43") == 0) {
				mimo_type = 2;
			} else if (strcmp(argv[1], "a43_azim") == 0) {
				mimo_type = 2;
				test_type = 1;
			} else if (strcmp(argv[1], "a43_elev") == 0) {
				mimo_type = 2;
				test_type = 2;
			} else if (strcmp(argv[1], "a43_elev_dbf") == 0) {
				mimo_type = 2;
				test_type = 3;
			} else {
				mmw_print(shell, "Unsupport %s!\n", argv[1]);
				return 0;
			}

			if (argc > 2) {
				uint16_t index = 0xFFFF;
				uint16_t det_start = 0;
				uint16_t det_end = 0;
				if (argc > 3) {
					det_start = (uint16_t)atoi(argv[2]);
					det_end   = (uint16_t)atoi(argv[3]);
				} else {
					index = (uint16_t)atoi(argv[2]);
				}
				mmw_ant_angel_test(index, det_start, det_end, mimo_type, true, test_type);
			} else {
				mmw_print(shell, "Less param!\n");
			}
		}
	}
	return 0;
}

#if (CONFIG_MMW_MICRO_POINT_CLOUD)

static int cmd_mmw_micro_config(Shell *shell, int argc, char *argv[])
{
    int ret = 0;
    if (argc > 1 && strcmp(argv[1], "get") != 0) {
		g_micro_point_en = !!(uint8_t)atoi(argv[1]);
        PRINT_ERR_CODE(shell);
	} else {
		mmw_print(shell, "micro point en: %d\n", g_micro_point_en);
	}
    return 0;
}

static int cmd_mmw_micro_snr(Shell *shell, int argc, char *argv[])
{
	if (argc > 1 && strcmp(argv[1], "get") != 0) {
		uint8_t snr = (uint8_t)atoi(argv[1]);
		mmw_micro_cfar_snr_set(snr);
	} else {
		mmw_print(shell, "micro snr=%d\n", mmw_micro_cfar_snr_get());
	}

	return 0;
}

static int cmd_mmw_micro_cfar_station(Shell *shell, int argc, char *argv[])
{
	bool range_station = 0, dop_station = 0;
	if (argc > 2 && strcmp(argv[1], "get") != 0) {
		range_station = (bool)atoi(argv[1]);
		dop_station = (bool)atoi(argv[2]);
		mmw_micro_cfar_stationary_set(range_station, dop_station);
	} else {
		mmw_micro_cfar_stationary_get(&range_station, &dop_station);
		mmw_print(shell, "micro cfar range_station=%d dop_station=%d\n",
			range_station, dop_station);
	}
	return 0;
}

static int cmd_mmw_micro_range(Shell *shell, int argc, char *argv[])
{
	if (argc > 2 && strcmp(argv[1], "get") != 0) {
		uint32_t range_min = (uint32_t)atoi(argv[1]);
		uint32_t range_max = (uint32_t)atoi(argv[2]);
		mmw_micro_point_range_set(range_min, range_max);
	} else {
		uint32_t range_min, range_max;
		mmw_micro_point_range_get(&range_min, &range_max);
		mmw_print(shell, "micro range(%dmm, %dmm)\n", range_min, range_max);
	}

	return 0;
}

static int cmd_mmw_micro_angle(Shell *shell, int argc, char *argv[])
{
	int16_t azimuth_min, azimuth_max, elev_min, elev_max;
	mmw_micro_point_angle_get(&azimuth_min, &azimuth_max, &elev_min, &elev_max);
	if (argc > 2 && strcmp(argv[1], "get") != 0) {
		azimuth_min = atoi(argv[1]);
		azimuth_max = atoi(argv[2]);
		azimuth_min = MAX(azimuth_min, -90)<<ANGLE_VALUE_SHIFT;
		azimuth_max = MIN(azimuth_max, 90)<<ANGLE_VALUE_SHIFT;
		if (argc > 4) {
			elev_min = atoi(argv[3]);
			elev_max = atoi(argv[4]);
			elev_min = MAX(elev_min, -90)<<ANGLE_VALUE_SHIFT;
			elev_max = MIN(elev_max, 90)<<ANGLE_VALUE_SHIFT;
		}
		mmw_micro_point_angle_set(azimuth_min, azimuth_max, elev_min, elev_max);
	} else {
		mmw_print(shell, "micro angle azim(%d %d), elev(%d %d)\n",
			azimuth_min>>ANGLE_VALUE_SHIFT, azimuth_max>>ANGLE_VALUE_SHIFT,
			elev_min>>ANGLE_VALUE_SHIFT, elev_max>>ANGLE_VALUE_SHIFT);
	}

	return 0;
}

static int cmd_mmw_micro_frame_rate(Shell *shell, int argc, char *argv[])
{
	if (argc > 1 && strcmp(argv[1], "get") != 0) {
		uint8_t sample_interval = atoi(argv[1]);
		mmw_micro_frame_rate_set(sample_interval);
	} else {
		uint8_t sample_interval = mmw_micro_frame_rate_get();
		mmw_print(shell, "micro frame sample_interval=%d\n", sample_interval);
	}
	return 0;
}

#endif

#if (CONFIG_MMW_OBJ_TRACKING)
static int cmd_mmw_sensitivity(Shell *shell, int argc, char *argv[])
{

	if (argc > 2) {
		if (strcmp(argv[1], "track") == 0) {
			uint32_t sensitivity_level = (uint32_t)atoi(argv[2]);
			uint16_t distqance_range_base_mm, max_asso_range_mm;
			switch (sensitivity_level) {
			case 0:
				distqance_range_base_mm = 1000;
				max_asso_range_mm = 1500;
				break;
			case 1:
				distqance_range_base_mm = 800;
				max_asso_range_mm = 1800;
				break;
			case 2:
				distqance_range_base_mm = 600;
				max_asso_range_mm = 2000;
				break;
			case 3:
				distqance_range_base_mm = 400;
				max_asso_range_mm = 2000;
				break;
			default:
				mmw_print(shell, "Unsupport MMW para %d\n", sensitivity_level);
				return 1;
				break;
            }
			tracking_associate_thresh_set(max_asso_range_mm, 100);
			track_range_thresh_set(distqance_range_base_mm, distqance_range_base_mm >> 1, distqance_range_base_mm >> 1);
			mmw_print(shell, "track sensitivity %d\n", sensitivity_level);
		} else if (strcmp(argv[1], "interf") == 0) {
			uint16_t min_cluster_points_near, min_cluster_points_far, min_asso_points;
			uint32_t interference_level = (uint32_t)atoi(argv[2]);
			switch (interference_level) {
			case 0:
				min_cluster_points_near = 5;
				min_cluster_points_far = 3;
				min_asso_points = 1;
				break;
			case 1:
				min_cluster_points_near = 6;
				min_cluster_points_far = 3;
				min_asso_points = 2;
				break;
			case 2:
				min_cluster_points_near = 7;
				min_cluster_points_far = 4;
				min_asso_points = 3;
				break;
			default:
				mmw_print(shell, "Unsupport MMW para %d\n", interference_level);
				return 1;
				break;
			}
			{
				uint32_t points_near;
				uint32_t points_far;
				float range_factor;
				tracking_object_create_get(&points_near, &points_far, &range_factor);
				tracking_object_create_set(min_cluster_points_near, min_cluster_points_far, range_factor);
			}
			{
				uint32_t min_points;
				uint32_t probe_timeout;
				uint32_t confirm_timeout;
				tracking_object_keep_get(&min_points, &probe_timeout, &confirm_timeout);
				tracking_object_keep_set(min_asso_points, probe_timeout, confirm_timeout);
			}
			mmw_dyn_threshold_enable_set(1);
			mmw_print(shell, "anti interference level %d\n", interference_level);
		} else {
			mmw_print(shell, "Only <track/interf> subcommand is supported!\n");
		}
	} else {
		mmw_print(shell, "A subcommand <track/interf> should be used!\n");
	}

	return 0;

}
#endif

/*
static int cmd_mmw_default(Shell *shell, int argc, char *argv[])
{

	if (argc == 1 || strcmp(argv[1], "help") == 0) {
		shell_help(shell);
		return SHELL_CMD_HELP_PRINTED;
	} else if (strcmp(argv[1], "get_dpll") == 0) {
		uint32_t fcw = *(volatile uint32_t *)(0x4001B0d4);
		printf("0x1 0x%X 0x%X\n", (fcw>>4)&0xff, (fcw>>12)&0x3);
	} else if (strcmp(argv[1], "get_lsi") == 0) {
		printf("%02X%02X%02X%02X\n",
			HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, ANA_LSI_TRIM_Msk, ANA_LSI_TRIM_Pos),
			HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_R_TRIM_Msk, LSI_R_TRIM_Pos),
			HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP2_TRIM_Msk, LSI_DECAP2_TRIM_Pos),
			HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP1_TRIM_Msk, LSI_DECAP1_TRIM_Pos));
	} else if (strcmp(argv[1], "get_msi") == 0) {
		printf("%02X%02X%02X%02X\n",
			HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CFTRIM_TRIM_Msk, MSI_CFTRIM_TRIM_Pos),
			HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CAP_TRIM_Msk, MSI_CAP_TRIM_Pos),
			HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_BIAS_TRIM_Msk, MSI_BIAS_TRIM_Pos),
			HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_FREQ_SEL_Msk, MSI_FREQ_SEL_Pos));
	} else {
		printf("Unsupport MMW Cmd %s\n", argv[1]);
	}

    return 0;
}*/

static int cmd_mmw_noise(Shell *shell, int argc, char *argv[])
{

	if ((argc > 1) && (!strcmp(argv[1], "est"))) {
		mmw_noise_estimation();
		mmw_print(shell, "please wait noise estimate completed...\n");
	} else {
		uint16_t range_fft_num;
		uint16_t doppler_fft_num;
		uint16_t noise[16];
		uint16_t i = 0;

		mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
		mmw_print(shell, "noise num: %d\n", range_fft_num);

		while ((range_fft_num - i) > 0) {
			int ret = mmw_noisefloor_get(noise, i, MIN(16, range_fft_num - i));
			if (ret) {
				mmw_print(shell, "Noise state=%d\n", ret);
				return -EINVAL;
			}
			mmw_print(shell, "%4d:", i);
			mmw_print(shell, "%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d\n",
				noise[0], noise[1], noise[2], noise[3], noise[4], noise[5], noise[6], noise[7],
				noise[8], noise[9], noise[10], noise[11], noise[12], noise[13], noise[14], noise[15]);
			i += 16;
		}
	}

	return 0;
}

static uint8_t  tone_enable = 0;
static uint8_t  tone_txid   = 0;
#if (CONFIG_BOARD_PISC_MRS7241_P2528)
static uint32_t tone_freq   = 76000;
#else
static uint32_t tone_freq   = 58000;
#endif

static int cmd_mmw_tone(Shell *shell, int argc, char *argv[])
{

	int ret;
	if (argc > 1) {
		if (strcmp(argv[1], "start") == 0) {
			if (argc > 2) {
				tone_txid = (uint32_t)atoi(argv[2]);
				if (argc > 3) {
					tone_freq = (uint32_t)atoi(argv[3]);
				}
			}
			ret = mmw_single_tone_start(tone_txid, tone_freq);
			tone_enable = 1;
		} else if (strcmp(argv[1], "stop") == 0) {
			ret = mmw_single_tone_stop();
			tone_enable = 0;
		} else {
			mmw_print(shell, "ERR param, see help\n");
			return 0;
		}
		PRINT_ERR_CODE(shell);
		return ret;
	} else {
		mmw_print(shell, "Single tone %s, txid=%d, freq=%dMHz!\n",
			(tone_enable ? "Enable" : "Disable"), tone_txid, tone_freq);
	}

	return 0;
}

static int cmd_mmw_uart(Shell *shell, int argc, char *argv[])
{

	if ((argc > 1) && (!strcmp(argv[1], "on"))) {
		mmw_enable_dbg_uart(1);
	} else if ((argc > 1) && (!strcmp(argv[1], "off"))) {
		mmw_enable_dbg_uart(0);
	} else {
		return -EINVAL;
	}

	return 0;
}

#define MMW_CALIB_DATA_LEN		(4*16+8)
int mmw_calibration_data(uint8_t *data, int data_len);

static int cmd_mmw_calib(Shell *shell, int argc, char *argv[])
{

    int ret;

	if (argc > 1) {
		if (!strcmp(argv[1], "load")) {
			mmwc_set_chirp_calib_data();
		} else if (!strcmp(argv[1], "save")) {
			mmwc_save_chirp_calib_data();
		} else if (!strcmp(argv[1], "ant")) {
            uint32_t chip_info[MMW_CALIB_DATA_LEN>>2] = { 0 };

            chip_info[0]  = strtoul(argv[2], NULL, 16);
            chip_info[5]  = strtoul(argv[3], NULL, 16);
            chip_info[10] = strtoul(argv[4], NULL, 16);
            chip_info[15] = strtoul(argv[5], NULL, 16);
            chip_info[16] = strtoul(argv[6], NULL, 16);
            chip_info[17] = strtoul(argv[7], NULL, 16);
            ret = mmw_calibration_data((uint8_t *)&chip_info[0], MMW_CALIB_DATA_LEN);
            return ret;
        } else {
			uint8_t on_off = 1;
			if (argc > 2) {
				on_off = (uint8_t)atoi(argv[2]);
			}
			if (!strcmp(argv[1], "chirp"))
				mmw_set_calib_sw(0x2, on_off);
			else {
				uint32_t mask = (uint8_t)atoi(argv[1]);
				mmw_set_calib_sw(mask, on_off);
			}
		}
	} else {
		return -EINVAL;
	}

	return 0;
}

static int cmd_mmw_dop_fft(Shell *shell, int argc, char *argv[])
{

	uint32_t flag = 0;
	if (argc == 1 || strcmp(argv[1], "get") == 0) {
		mmw_print(shell, "dop fft=%d\n", mmw_dop_fft_get());
	} else if (argc == 2) {
		 flag = (uint32_t)atoi(argv[1]);
		if (flag == 0 || flag == 1) {
			mmw_dop_fft_set(flag);
		} else {
			return -EINVAL;
		}
	} else {
		return -EINVAL;
	}

	return 0;
}

static int cmd_mmw_mmic_dbg(Shell *shell, int argc, char *argv[])
{

	int ret = 0;
	int en, rx_num;
	if (argc == 3) {
		en = (uint32_t)atoi(argv[1]);
		rx_num = (uint32_t)atoi(argv[2]);
		ret = mmw_set_mmic_para(en, rx_num, 0, 1, 1);
		if (ret) {
			PRINT_ERR_CODE(shell);
		}
	} else {
		return -EINVAL;
	}

	return 0;
}

static int cmd_com_type(Shell *shell, int argc, char *argv[])
{

	com_mode = (uint8_t)atoi(argv[1]);
	printf("com_mode = %d\r\n", com_mode);
#if 0
	if(com_mode == 0) {
		dbg_uart_enable(1);
		set_dbg_uart_port(1, 1);
	} else {
		dbg_uart_enable(0);
		set_dbg_uart_port(1, 1);
	}
#endif
	return 0;
}

static int cmd_get_report_cfg(Shell *shell, int argc, char *argv[])
{
	uint32_t frame_len = 0;
	uint16_t count = 0;
	uint8_t tx_cnt = 0;
	uint8_t rx_cnt= 0;
	uint16_t range_fft_num = 0;
	uint16_t doppler_fft_num = 0;
	int ret = 0;

	ret = get_tx_rx_num(&tx_cnt, &rx_cnt);
	if(ret) {
		PRINT_ERR_CODE(shell);
		return ret;
	}
	ret = mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
	if(ret) {
		PRINT_ERR_CODE(shell);
		return ret;
	}

	frame_len = tx_cnt * rx_cnt * range_fft_num * doppler_fft_num;
	if((frame_len * 4) % CUBE_DATA_SIZE > 0) {
		count = frame_len * 4 / CUBE_DATA_SIZE + 1;
	} else {
		count = frame_len * 4 / CUBE_DATA_SIZE;
	}

    mmw_print(shell, "total_size=%d\r\nsplit_size=%d\r\nsplit_num=%d\r\n", frame_len, CUBE_DATA_SIZE, count);
    return 0;
}

static int cmd_mmw_tx_rf_att(Shell *shell, int argc, char *argv[])
{
    int ret = 0;
    int att_db = (uint32_t)atoi(argv[1]);

    ret = mmw_tx_rf_att_set(att_db);

    return ret;
}


static int cmd_hif_simplex(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc > 1) {
		uint32_t enable = (uint32_t)atoi(argv[1]);
        ret = HIF_ExtControl(HIF_SET_SIMPLEX_MODE, (void *)&enable, sizeof(enable));

		PRINT_ERR_CODE(shell);
	}

    return ret;
}

static int cmd_hif_Retry(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc > 1) {
		uint32_t enable = (uint32_t)atoi(argv[1]);
        ret = HIF_ExtControl(HIF_SET_RETRY, (void *)&enable, sizeof(enable));
		PRINT_ERR_CODE(shell);
	}

    return ret;
}

static int cmd_mmw_dcxo(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc > 1) {
        ret = 0;
		uint32_t dcxo_time = (uint32_t)atoi(argv[1]);
        mmw_set_power_up_time(dcxo_time);
		PRINT_ERR_CODE(shell);
	}

    return ret;
}

static int cmd_mmw_win(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 2) {
        ret = 0;
        mmw_print(shell, "win type = %d\r\n", mmw_win_type_get((uint32_t)atoi(argv[1])));
	} else if (argc == 3) {
        ret = mmw_win_type_set((uint32_t)atoi(argv[1]), (uint32_t)atoi(argv[2]));
    }

    return ret;    
}

static int cmd_mmw_clutter_halt(Shell *shell, int argc, char *argv[])
{
	int ret = -1;

	if (argc == 2) {
        ret = mmw_clutter_halt_set(atoi(argv[1]));
	} else {
        ret = 0;
        mmw_print(shell, "clutter hald = %d\r\n", mmw_clutter_halt_get());
    }
    return ret;
}

ShellCommand mmwGroup[] = {
    SHELL_CMD_ITEM(open, cmd_mmw_open, "[<xo/msi>] [xo/none]"),
    SHELL_CMD_ITEM(close, cmd_mmw_close, "No param"),
    SHELL_CMD_ITEM(version, cmd_mmw_verison, "No param"),
    SHELL_CMD_ITEM(start, cmd_mmw_start, "No param"),
    SHELL_CMD_ITEM(stop, cmd_mmw_stop, "No param"),
    SHELL_CMD_ITEM(state, cmd_mmw_state, "No param Get power state"),
    SHELL_CMD_ITEM(ps, cmd_mmw_powersave, "<enable>"),
    SHELL_CMD_ITEM(mode, cmd_mmw_mode, "<txrx_mode> <FFT mode>"),
    SHELL_CMD_ITEM(freq, cmd_mmw_freq, "<start_MHz> [max_MHz]"),
    SHELL_CMD_ITEM(range, cmd_mmw_range, "<max_mm> <resolution_mm>"),
    SHELL_CMD_ITEM(veloc, cmd_mmw_veloc, "<max_mm/s> <resolution_mm/s>"),
    SHELL_CMD_ITEM(frame, cmd_mmw_frame, "<period_ms> <frame_num>"),
    SHELL_CMD_ITEM(intv, cmd_mmw_intv, "<period_us> <intv_num>"),
    SHELL_CMD_ITEM(chirp_info, cmd_mmw_chirp_param, "No param"),
    SHELL_CMD_ITEM(chirp_num, cmd_mmw_chirp_num, "<num>"),
    SHELL_CMD_ITEM(fft_len, cmd_mmw_fft_len, "No param"),
    SHELL_CMD_ITEM(coord, cmd_mmw_coord, "[polar/cart]"),
    SHELL_CMD_ITEM(data, cmd_mmw_datacfg, "<diable/fft/3dpoints/restore>"),
    SHELL_CMD_ITEM(clutter_rm, cmd_mmw_clutter_rm, "<enable> [fac]"),
    SHELL_CMD_ITEM(cfar, cmd_mmw_cfar, "<static_rm> <snr(1~255)> <method(0: SNR/1 CACFAR)>"),
    SHELL_CMD_ITEM(cube, cmd_mmw_read, "[read_num] [offset]"),
	SHELL_CMD_ITEM(mount, cmd_mmw_mount, "<ceiling/horizon/vertical>"),
	SHELL_CMD_ITEM(dcxo, cmd_mmw_dcxo, "<time(us)>"),
	SHELL_CMD_ITEM(win, cmd_mmw_win, "<calc type 0:rfft, 1:dfft><win type 0:rectangle, 1: hanning>"),
	SHELL_CMD_ITEM(dc_suppression, cmd_mmw_dc_suppression, "<1/0>"),
	SHELL_CMD_ITEM(clutter_halt, cmd_mmw_clutter_halt, "<1/0>"),

#if (CONFIG_MMW_OBJ_TRACKING)
    SHELL_CMD_ITEM(report, cmd_mmw_report, "<disable/cube/points/track/points-track> [frame_num]"),
#else
    SHELL_CMD_ITEM(report, cmd_mmw_report, "<disable/cube/points> [frame_num]"),
#endif
    SHELL_CMD_ITEM(uart, cmd_mmw_uart, "<on/off>"),
    SHELL_CMD_ITEM(baudrate, cmd_mmw_baudrate, "<baudrate>"),
    SHELL_CMD_ITEM(noise, cmd_mmw_noise, "[est]"),
    SHELL_CMD_ITEM(tone, cmd_mmw_tone, "<start/stop> [txid] [freq_MHz]"),
    SHELL_CMD_ITEM(rx_gain, cmd_mmw_rx_gain, "<rf/dig> <ant_id> <gain>"),
    SHELL_CMD_ITEM(calib, cmd_mmw_calib, "<chirp/load/save/ant>"),
    SHELL_CMD_ITEM(hpf, cmd_mmw_ana_hpf_bw, "<ant_id> <bw>"),
    SHELL_CMD_ITEM(dbg, cmd_mmw_dbg, "[<type> [tag]]"),
    SHELL_CMD_ITEM(dop_fft, cmd_mmw_dop_fft, "<off/on>"),
    SHELL_CMD_ITEM(mmic_dbg, cmd_mmw_mmic_dbg, "[<on/off> [rx_mode]]"),
    SHELL_CMD_ITEM(tx_rf_att, cmd_mmw_tx_rf_att, "<0/1/2/3/4/6/8>"),
    SHELL_CMD_ITEM(ant, cmd_mmw_ant_calib, "<calib_get/calib_set/load/save/calib_on/calib_off/m62/a43><start/index>[end]"),

#if (CONFIG_MMW_MICRO_POINT_CLOUD)
    SHELL_CMD_ITEM(mpoint, cmd_mmw_micro_config, "<0/1>"),
    SHELL_CMD_ITEM(micro_snr, cmd_mmw_micro_snr, "<1~255>"),
    SHELL_CMD_ITEM(micro_station, cmd_mmw_micro_cfar_station, "<range><dop>"),
    SHELL_CMD_ITEM(micro_range, cmd_mmw_micro_range, "<min_mm><max_mm>"),
    SHELL_CMD_ITEM(micro_angle, cmd_mmw_micro_angle, "<azim_min><azim_max>[elev_min][elev_max]"),
    SHELL_CMD_ITEM(micro_frame, cmd_mmw_micro_frame_rate, "<sample_interval>"),
#endif

#if (CONFIG_MMW_OBJ_TRACKING)
    SHELL_CMD_ITEM(sensitivity, cmd_mmw_sensitivity, "<track/interf>"),
#endif
    SHELL_CMD_ITEM(comtype, cmd_com_type, "<No param>"),
    SHELL_CMD_ITEM(getreportcfg, cmd_get_report_cfg, "<No param>"),
    SHELL_CMD_ITEM(hif_simplex, cmd_hif_simplex, "<enable>"),
    SHELL_CMD_ITEM(hif_retry, cmd_hif_Retry, "<enable>"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(mmwc, mmwGroup, "MMW Controller shell commands");


static int cmd_mem_dump(Shell *shell, int argc, char *argv[])
{
	uint32_t *addr = (uint32_t *)strtol(argv[1], NULL, 16);
	uint32_t len   = strtol(argv[2], NULL, 16)/4;

	while (len) {
		int num = MIN(len, 8);
		for (int i = 0; i < num; ++i) {
			mmw_print(shell, "%08X ", *addr++);
		}
		mmw_print(shell, "\n");
		len -= num;
	}
	return 0;
}

SHELL_CMD(memdump, cmd_mem_dump, "Memory dump <addr> <bytes>");


#define SHELL_PRINT_MAX    4
static int mmw_shell_data_show(void *mmw_data, void *arg)
{
	int i, print_num;
	uint32_t data_status = mmw_data_get_status();
	printk("MMW data=0x%x\n", data_status);

	if (data_status & BIT(MMW_NOISE_EST_COMPLETE_EVENT)) {
		printk("noise estimate complete, please mmwc start\n");
	}

	if (MMW_DATA_TYPE_3D_POINTS == mmw_ctrl_data_type_get() &&
		(data_status & BIT(MMW_DATA_TYPE_3D_POINTS))) {
		PointCloud3D array[SHELL_PRINT_MAX];
		print_num = mmw_get_pointcloud_len()/sizeof(PointCloud3D);
		printk("Point Cloud=%d:\n", print_num);
		if (print_num) {
			uint8_t coordinate = mmw_coordinate_get();
			print_num = MIN(print_num, SHELL_PRINT_MAX);
			mmw_pointcloud_3d(&array[0], sizeof(PointCloud3D)*print_num);
			if (coordinate == MMW_COORDINATE_TYPE_CART) {
				for (i = 0; i < print_num; ++i) {
					printk("P%d: x=%dcm\ty=%dcm\tz=%dcm\t%dcm/s\t%d\n", i,
						array[i].cart.x, array[i].cart.y, array[i].cart.z,
						array[i].cart.vel, array[i].cart.snr);
				}
			} else {
				for (i = 0; i < print_num; ++i) {
					printk("P%d: R=%dcm\tA=%d\tE=%d\t%dcm/s\t%d\n", i,
						array[i].polar.range, array[i].polar.azim, array[i].polar.elev,
						array[i].polar.vel, array[i].polar.snr);
				}
			}
		}
	}

    return 0;
}


static int cmd_devmem_read(Shell *shell, int argc, char *argv[])
{
	uint8_t width = 0;
	uint32_t addr = 0;
	uint32_t value;
	int err = 0;

	if (argc == 2) {
		width = 32;
		addr = strtoul(argv[1], NULL, 0);
	} else if (argc == 3) {
		if (!strcmp(argv[1], "-8")) {
			width = 8;
		}
		else if (!strcmp(argv[1], "-16")) {
			width = 16;
		}
		else if (!strcmp(argv[1], "-32")) {
			width = 32;
		}
		else {
			return -EINVAL;
		}
		addr = strtoul(argv[2], NULL, 0);
	} else {
		return -EINVAL;
	}


	switch (width) {
	case 8:
		value = *(volatile uint8_t *)(addr);
		break;
	case 16:
		value = *(volatile uint16_t *)(addr);
		break;
	case 32:
		value = *(volatile uint32_t *)(addr);
		break;
	default:
		shellPrint(shell, "Incorrect data width\n");
		err = -EINVAL;
		break;
	}

	if (err == 0) {
		shellPrint(shell, "Read value 0x%x\n", value);
	}

	return err;
}

static int cmd_devmem_write(Shell *shell, int argc, char *argv[])
{
	uint8_t width = 0;
	uint32_t value = 0;
	uint32_t addr = 0;
	int err = 0;

	if (argc == 3) {
		width = 32;
		addr = strtoul(argv[1], NULL, 0);
		value = strtoul(argv[2], NULL, 0);
	} else if (argc == 4) {
		if (!strcmp(argv[1], "-8")) {
			width = 8;
		}
		else if (!strcmp(argv[1], "-16")) {
			width = 16;
		}
		else if (!strcmp(argv[1], "-32")) {
			width = 32;
		}
		else {
			return -EINVAL;
		}
		addr = strtoul(argv[2], NULL, 0);
		value = strtoul(argv[3], NULL, 0);
	} else {
		return -EINVAL;
	}

	switch (width) {
	case 8:
		*(volatile uint8_t *)(addr) = (uint8_t)value;
		break;
	case 16:
		*(volatile uint16_t *)(addr) = (uint16_t)value;
		break;
	case 32:
		*(volatile uint32_t *)(addr) = (uint32_t)value;
		break;
	default:
		shellPrint(shell, "Incorrect data width\n");
		err = -EINVAL;
		break;
	}

	return err;
}

ShellCommand devmemGroup[] = {
    SHELL_CMD_ITEM(read, 	cmd_devmem_read,  "devmem read [width] <address> Width:-8/-16/-32 bits"),
    SHELL_CMD_ITEM(write, 	cmd_devmem_write, "devmem write [width] <address> <value> Width:-8/-16/-32 bits"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(devmem, devmemGroup, "Read/write physical memory");

#if (POINTCLOUD_SHOW_TIME_INFO)
#define AVERAGE_CALC(cnt, sum)  ((cnt) ? (sum)/(cnt) : 0)
static int cmd_perf_points(Shell *shell, int argc, char *argv[])
{
	TIME_DBG_INFO *item = (TIME_DBG_INFO *)&g_time_dbg_info;
	if (strcmp(argv[1], "clear") == 0) {
		memset(item, 0, sizeof(g_time_dbg_info));
		return 0;
	}
	shellPrint(shell, "Item\tcnt\tmax/us\tmin/us\tavrg/us\t\n");
	for (int i = 0; i < sizeof(g_time_dbg_info)/sizeof(TIME_DBG_INFO); ++i) {
		if (item->label != NULL) {
			shellPrint(shell, "%s\t%u\t%u\t%u\t%u\n",
				item->label, item->cnt, CORET_TO_US(item->max), CORET_TO_US(item->min),
				(uint32_t)AVERAGE_CALC(item->cnt, CORET_TO_US(item->sum)));
		}
		item++;
	}
	return 0;
}
ShellCommand perfGroup[] = {
    SHELL_CMD_ITEM(points, 	cmd_perf_points,  "perf points [clear]"),
    SHELL_CMD_ITEM_END()
};
SHELL_CMD_GROUP(perf, perfGroup, "Get process time information");
#endif

#endif /* CONFIG_MMW_SHELL */

