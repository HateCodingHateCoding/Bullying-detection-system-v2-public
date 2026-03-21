#include <stdio.h>
#include "mmw_hif.h"
#include "hif.h"
#include "hif_config.h"
#include "mmw_ctrl.h"
#include "util.h"
#include "mmw_report.h"
#include <math.h>

/* point cloud include files */
#include "mmw_app_micro_pointcloud.h"
#include "mmw_app_pointcloud.h"
#include "mmw_app_pointcloud_config.h"
#include "mmw_point_cloud_psic_lib.h"

#if (CONFIG_CHIP_INFO)
#include "chip_info.h"
#endif

#if (CONFIG_MMW_POINT_CLOUD)
#include "mmw_ant_calibration.h"
#include "mmw_detection3d.h"
#endif

#if (CONFIG_MMW_LIB_MIMO)
#include "mmw_ant_calibration.h"
#include "angle_calculate.h"
#endif


#if (CONFIG_MMW_OBJ_TRACKING)
#include "mmw_detection3d_user_config.h"
#endif


#if (CONFIG_MMW_POINT_CLOUD)
#include "mmw_ant_calibration.h"
#include "mmw_detection3d.h"
#endif

#define MMW_HIF_DBG    1
#if MMW_HIF_DBG
#define HIF_DBG(fmt, arg...)            printf(fmt, ##arg)
#else
#define HIF_DBG(fmt, arg...)
#endif

uint8_t com_mode = 0;
#if (CONFIG_MMW_SHELL)
extern uint32_t repot_frame_num;
extern uint8_t  gui_data_report;
extern uint8_t g_cfar_method;		/* 0: hw cfar with snr threshold, 1: sw cfar */
#else
uint8_t g_cfar_method = 0;		/* 0: hw cfar with snr threshold, 1: sw cfar */
uint32_t repot_frame_num;
uint8_t  gui_data_report = 0;//GUI_DATA_TYPE_DISABLE;
#endif

static uint8_t mmw_cfg_open_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	open_msg_cfg_t open_msg = {0x00};
	memcpy(&open_msg, data, sizeof(open_msg_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", open_msg.sub_id, open_msg.use_msi, open_msg.pll_lc);

	if(mmw_ctrl_open(open_msg.use_msi, open_msg.pll_lc, 1)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_close_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	HIF_DBG("subId:0x%02x\r\n", data[0]);

	if(mmw_ctrl_close()) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_start_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	HIF_DBG("subId:0x%02x\r\n", data[0]);

	if(gui_data_report >= GUI_DATA_TYPE_POINTS && gui_data_report < GUI_DATA_TYPE_DISABLE) {
		mmw_set_ant_align(1);		/* reset align register */
		mmw_point_cloud_init();
		/* CA-CFAR only support fix gain yet. */
		if (g_cfar_method == 1) {
			mmw_fft_autogain_set(0);
		} else if(g_cfar_method == 0) {
			mmw_fft_autogain_set(1);
		} else {
			ret = MMW_ERR_CODE_INVALID_PARAM;
		}
		mmw_report_param_get();
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
		mmw_micro_point_restart();
#endif
	} else {
		/* For DataCube Application, ant align is not enabled */
		mmw_set_ant_align(0);
	}

	if(mmw_ctrl_start()) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_stop_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	HIF_DBG("subId:0x%02x\r\n", data[0]);

	if(mmw_sensor_stop()) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_power_save_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	power_save_cfg_t power_save = {0x00};

	memcpy(&power_save, data, sizeof(power_save_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", power_save.sub_id, power_save.enable);
	if (power_save.enable) {
		if (mmw_ctrl_low_power()) {
			ret = HIF_CMD_STATUS_IO;
		}
	} else {
		if (mmw_ctrl_wake_up()) {
			ret = HIF_CMD_STATUS_IO;
		}
	}

	return ret;
}

static uint8_t mmw_cfg_mode_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	mode_cfg_t mode_cfg = {0x00};

	memcpy(&mode_cfg, data, sizeof(mode_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", mode_cfg.sub_id, mode_cfg.txrx, mode_cfg.work);
	if(mode_cfg.txrx > txrx_1T2R || (mode_cfg.work != mode_1DFFT && mode_cfg.work != mode_2DFFT)) {
		ret = HIF_CMD_STATUS_PARAM;
	} else {
		if(mmw_mode_cfg (mode_cfg.txrx, mode_cfg.work)) {
			ret = HIF_CMD_STATUS_IO;
		}
	}

	return ret;
}

static uint8_t mmw_cfg_freq_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	freq_cfg_t freq_cfg = {0x00};

	memcpy(&freq_cfg, data, sizeof(freq_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", freq_cfg.sub_id, freq_cfg.start, freq_cfg.max);
	if(mmw_freq_cfg(freq_cfg.start, freq_cfg.max)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_range_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	range_cfg_t range_cfg = {0x00};

	memcpy(&range_cfg, data, sizeof(range_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", range_cfg.sub_id, range_cfg.range, range_cfg.resol);
	if(range_cfg.resol == 0 || (range_cfg.range / range_cfg.resol) > 1024) {
		ret = HIF_CMD_STATUS_PARAM;
	} else {
		if(mmw_range_cfg(range_cfg.range, range_cfg.resol)) {
			ret = HIF_CMD_STATUS_IO;
		}
	}

	return ret;
}

static uint8_t mmw_cfg_veloc_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	veloc_cfg_t veloc_cfg = {0x00};
	memcpy(&veloc_cfg, data, sizeof(veloc_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", veloc_cfg.sub_id, veloc_cfg.veloc, veloc_cfg.resol);
	if(veloc_cfg.resol == 0 || (2 * veloc_cfg.veloc / veloc_cfg.resol) > 1024) {
		ret = HIF_CMD_STATUS_PARAM;
	} else {
		if(mmw_velocity_cfg(veloc_cfg.veloc, veloc_cfg.resol)) {
			ret = HIF_CMD_STATUS_IO;
		}
	}

	return ret;
}

static uint8_t mmw_cfg_frame_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	frame_cfg_t frame_cfg = {0x00};
	memcpy(&frame_cfg, data, sizeof(frame_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", frame_cfg.sub_id, frame_cfg.period, frame_cfg.num);
	if(mmw_frame_cfg(frame_cfg.period, frame_cfg.num)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_intv_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	intv_cfg_t intv_cfg = {0x00};

	memcpy(&intv_cfg, data, sizeof(intv_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", intv_cfg.sub_id, intv_cfg.period, intv_cfg.intv_num);
	if(mmw_interval_cfg(intv_cfg.period, intv_cfg.intv_num)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_chirp_num_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	chirp_num_cfg_t chirp_num = {0x00};

	memcpy(&chirp_num, data, sizeof(chirp_num_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", chirp_num.sub_id, chirp_num.num);
	if(mmw_chirp_num_cfg(chirp_num.num)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_coord_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	coord_cfg_t coord_cfg = {0x00};

	memcpy(&coord_cfg, data, sizeof(coord_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", coord_cfg.sub_id, coord_cfg.coord_type);
	if(coord_cfg.coord_type > cart) {
		ret = HIF_CMD_STATUS_PARAM;
	} else {
		if(mmw_coordinate_config(coord_cfg.coord_type)) {
			ret = HIF_CMD_STATUS_IO;
		}
	}

	return ret;
}

static bool    callback_restore;
static void   *arg_restore;
static uint8_t data_type_restore;
static MMW_HANDLE handler_restore;

uint8_t *payload = NULL;

static void send_callback(hif_queue_item_t *hif_queue_item, int8_t status)
{
	OSI_Free(payload);
}

static int mmw_cfgdata_report(void *mmw_data, void *arg)
{
	int ret = HIF_CMD_STATUS_SUCCESS;
    HIF_MsgHdr_t msgHdr = {0x00};

	int point_num = 0;
	data_report_t data_report = {0x00};
	uint32_t data_status = mmw_data_get_status();

	uint32_t len = sizeof(data_report_t) - sizeof(PointCloud3D);

	if (data_status & BIT(MMW_NOISE_EST_COMPLETE_EVENT)) {
		ret = HIF_CMD_STATUS_IO;
	}

	if (MMW_DATA_TYPE_3D_POINTS == mmw_ctrl_data_type_get() &&
		(data_status & BIT(MMW_DATA_TYPE_3D_POINTS))) {

		point_num = mmw_get_pointcloud_len() / sizeof(PointCloud3D);
		data_report.data_num = point_num;
		uint16_t data_len = sizeof(data_report_t) - sizeof(PointCloud3D) + point_num * sizeof(PointCloud3D);

		if (point_num && data_len <= HOST_CMD_MAX_PAYLOAD) {
			PointCloud3D *point = (PointCloud3D *)OSI_Malloc(point_num * sizeof(PointCloud3D));
			memset(point, 0, point_num*sizeof(PointCloud3D));
			uint8_t coordinate = mmw_coordinate_get();
			if(mmw_pointcloud_3d(point, sizeof(PointCloud3D)*point_num) == 0) {
				if (coordinate == MMW_COORDINATE_TYPE_CART) {
					data_report.data_type = MMW_COORDINATE_TYPE_CART;
				} else {
					data_report.data_type = MMW_COORDINATE_TYPE_POLAR;
				}

				data_report.sub_id = mmw_data_report;

				payload = (uint8_t *)OSI_Malloc(data_len);
				memset(payload, 0, data_len);
				memcpy(payload, &data_report, len);
				memcpy(&payload[len], point, point_num * sizeof(PointCloud3D));
                HIF_MSG_HDL_PACK(msgHdr, HIF_MMW_CFG_MSG_ID, data_len);
				HIF_MsgReport(&msgHdr, payload, send_callback);
			}
			OSI_Free(point);
		}
	}

    return ret;
}


static uint8_t mmw_cfg_data_cfg_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	datacfg_t datacfg = {0x00};

	memcpy(&datacfg, data, sizeof(datacfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", datacfg.sub_id, datacfg.datacfg_type);
	if(datacfg.datacfg_type == type_restore) {
		if (callback_restore) {
			mmw_ctrl_callback_cfg(handler_restore, data_type_restore, arg_restore);
			callback_restore = false;
		}
	} else {
		if(datacfg.datacfg_type == type_datacfg_disable || datacfg.datacfg_type == type_1dfft \
			|| datacfg.datacfg_type == type_2dfft || datacfg.datacfg_type == type_3d_point \
			|| datacfg.datacfg_type == type_2d_track || datacfg.datacfg_type == type_motion \
			|| datacfg.datacfg_type == type_onoff) {
			if (!callback_restore) {
				handler_restore = mmw_ctrl_callback_get(&data_type_restore, &arg_restore);
				callback_restore = true;
			}
			if (datacfg.datacfg_type == type_1dfft) {
				uint8_t txrx = 0;
				uint8_t work = 0;
				mmw_mode_get (&txrx, &work);
				if(mmw_ctrl_callback_cfg(mmw_cfgdata_report,
											((MMW_WORK_MODE_1DFFT == work) ? datacfg.datacfg_type:(datacfg.datacfg_type + 1)),
											NULL)) {
					ret = HIF_CMD_STATUS_IO;
				}
			} else {
				if(mmw_ctrl_callback_cfg(mmw_cfgdata_report, datacfg.datacfg_type, NULL)) {
					ret = HIF_CMD_STATUS_IO;
				}
			}
		} else {
			ret = HIF_CMD_STATUS_UNSUPPORT;
		}
	}

	return ret;
}

static uint8_t mmw_cfg_clutter_rm_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	clutter_rm_cfg_t clutter_rm = {0x00};

	memcpy(&clutter_rm, data, sizeof(clutter_rm_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", clutter_rm.sub_id, clutter_rm.enable, clutter_rm.clutter_fac);
	if(mmw_clutter_factor_cfg(clutter_rm.clutter_fac)) {
		ret = HIF_CMD_STATUS_IO;
	}
	if (!ret) {
		if(mmw_clutter_remove(clutter_rm.enable)) {
			ret = HIF_CMD_STATUS_IO;
		}
	}

	return ret;
}

static uint8_t mmw_cfg_cfar_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;
	float snr_pwr;

	cfar_cfg_t cfar_cfg = {0x00};

	memcpy(&cfar_cfg, data, sizeof(cfar_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", cfar_cfg.sub_id, cfar_cfg.static_rm, cfar_cfg.snr);
    snr_pwr = powf(10, cfar_cfg.snr / 20.f);
	if(mmw_cfar_cfg(cfar_cfg.static_rm, roundf(snr_pwr * 8))) {
		ret = HIF_CMD_STATUS_IO;
	}
    mmw_point_cloud_get_user_cfg()->mmw_motion_point_cloud_config.threshold_snr_pwr_lin = powf(10, cfar_cfg.snr / 10.f);

	return ret;
}

static uint8_t mmw_cfg_report_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;
	uint8_t txrx = 0;
	uint8_t work = 0;

	report_cfg_t report_cfg = {0x00};

	memcpy(&report_cfg, data, sizeof(report_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", report_cfg.sub_id, report_cfg.report_type, report_cfg.report_num);
	if(report_cfg.report_type == type_cube || report_cfg.report_type == type_mmic || report_cfg.report_type == type_point \
		|| report_cfg.report_type == type_track || report_cfg.report_type == type_points_trace \
		|| report_cfg.report_type == type_report_disable) {

        repot_frame_num = report_cfg.report_num;
		gui_data_report = report_cfg.report_type;

		ret = mmw_report_gui(gui_data_report, 2048, repot_frame_num);
		mmw_mode_get (&txrx, &work);
		uint8_t type = work == MMW_WORK_MODE_1DFFT ? MMW_DATA_TYPE_1DFFT : MMW_DATA_TYPE_2DFFT;
		if(gui_data_report == GUI_DATA_TYPE_CUBE && com_mode == 1){
			ret = HIF_CMD_STATUS_PARAM;
		} else if(gui_data_report == GUI_DATA_TYPE_CUBE && com_mode == 0) {
			mmw_ctrl_callback_cfg(&mmw_ctrl_data_cube_uart_report_cb, type, NULL);
		} else if(gui_data_report >= GUI_DATA_TYPE_POINTS && gui_data_report < GUI_DATA_TYPE_DISABLE) {
			mmw_point_cloud_init();
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
			mmw_micro_point_init();
#endif
			mmw_ctrl_callback_cfg(&mmw_ctrl_data_point_report_cb, type, NULL);
		}
	} else {
		ret = HIF_CMD_STATUS_PARAM;
	}

	return ret;
}

static uint8_t mmw_cfg_uart_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	return ret;
}

static uint32_t mmw_cfg_baudrate = 921600;
static uint8_t mmw_cfg_baudrate_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	baudrate_cfg_t baudrate_cfg = {0x00};

	memcpy(&baudrate_cfg, data, sizeof(baudrate_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", baudrate_cfg.sub_id, baudrate_cfg.baud);

	mmw_cfg_baudrate = baudrate_cfg.baud;

	return ret;
}

static uint8_t mmw_cfg_noise_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	noise_cfg_t noise_cfg = {0x00};

	memcpy(&noise_cfg, data, sizeof(noise_cfg_t));
	HIF_DBG("subId:0x%02x\r\n", noise_cfg.sub_id);
	if(mmw_noise_estimation()) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t  tone_cfg_enable = 0;
static uint8_t  tone_cfg_txid   = 0;
static uint32_t tone_cfg_freq   = 0;

static uint8_t mmw_cfg_tone_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	tone_cfg_t tone_cfg = {0x00};

	memcpy(&tone_cfg, data, sizeof(tone_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d arg[2]:%d\r\n", tone_cfg.sub_id, tone_cfg.enable, tone_cfg.tone_txid, tone_cfg.tone_freq);
	if(tone_cfg.enable) {
		if(mmw_single_tone_start(tone_cfg.tone_txid, tone_cfg.tone_freq)) {
			ret = HIF_CMD_STATUS_IO;
		} else {
			tone_cfg_enable = 0x01;
			tone_cfg_txid = tone_cfg.tone_txid;
			tone_cfg_freq = tone_cfg.tone_freq;
		}
	} else {
		if(mmw_single_tone_stop()) {
			ret = HIF_CMD_STATUS_IO;
		} else {
			tone_cfg_enable = 0x00;
		}
	}

	return ret;
}

static uint8_t mmw_cfg_rx_gain_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	rx_gain_cfg_t rx_gain = {0x00};

	rx_gain.sub_id = data[0];
	rx_gain.gain_type = data[1];
	rx_gain.ant_id = data[2];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d", rx_gain.sub_id, rx_gain.gain_type, rx_gain.ant_id);
	if(rx_gain.gain_type != type_rf && rx_gain.gain_type != type_dig) {
		ret = HIF_CMD_STATUS_PARAM;
	} else {
		if(rx_gain.gain_type == type_rf) {
			rx_gain.u_gain = data[3];
			HIF_DBG(" arg[2]:%d\r\n", rx_gain.u_gain);
			if(mmw_rx_ana_gain_cfg(rx_gain.ant_id, rx_gain.u_gain)) {
				ret = HIF_CMD_STATUS_IO;
			}
		} else {
			rx_gain.i_gain = data[3];
			HIF_DBG(" arg[2]:%d\r\n", rx_gain.i_gain);
			if(mmw_rx_dig_gain_adj(rx_gain.ant_id, rx_gain.i_gain)) {
				ret = HIF_CMD_STATUS_IO;
			}
		}
	}

	return ret;
}

int mmw_calibration_data(uint8_t *data, int data_len);

static uint8_t mmw_cfg_calib_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	calib_cfg_t calib_msg = {0x00};
	uint32_t info[18] = { 0 };

	calib_msg.sub_id = data[0];
	calib_msg.calib_type = data[1];
	HIF_DBG("subId:0x%02x arg[0]:%d", calib_msg.sub_id, calib_msg.calib_type);
	if(calib_msg.calib_type == type_load) {
		if(mmwc_set_chirp_calib_data()) {
			ret = HIF_CMD_STATUS_IO;
		}
	} else if(calib_msg.calib_type == type_save) {
		if(mmwc_save_chirp_calib_data()) {
			ret = HIF_CMD_STATUS_IO;
		}
	} else if (calib_msg.calib_type == type_ant) {
			memcpy(calib_msg.chip_info, &data[4], sizeof(calib_msg.chip_info));

			for(uint8_t i=0;i<5;i++) {
				HIF_DBG(" arg[%d]:%d", i+1, calib_msg.chip_info[i]);
			}
			HIF_DBG(" arg[6]:%d", calib_msg.chip_info[5]);

		    info[0]  = calib_msg.chip_info[0];
            info[5]  = calib_msg.chip_info[1];
            info[10] = calib_msg.chip_info[2];
            info[15] = calib_msg.chip_info[3];
            info[16] = calib_msg.chip_info[4];
            info[17] = calib_msg.chip_info[5];
		if(mmw_calibration_data((uint8_t *)&info[0], 72)) {
			ret = HIF_CMD_STATUS_IO;
		}
	} else if (calib_msg.calib_type == type_chirp) {
		memcpy(&calib_msg.calib_sw, &data[4], sizeof(calib_msg.calib_sw));
		HIF_DBG(" arg[1]:%d arg[1]:%d\r\n", calib_msg.calib_sw.onoff, calib_msg.calib_sw.mask);
		if(calib_msg.calib_sw.mask == MMW_CALIB_SIGMADELTA	\
			|| calib_msg.calib_sw.mask == MMW_CALIB_AFC	\
			|| calib_msg.calib_sw.mask == MMW_CALIB_CLUTTER_EST) {
			if(mmw_set_calib_sw(calib_msg.calib_sw.mask, calib_msg.calib_sw.onoff)) {
				ret = HIF_CMD_STATUS_IO;
			}
		} else {
			ret = HIF_CMD_STATUS_PARAM;
		}
	} else {
		ret = HIF_CMD_STATUS_UNSUPPORT;
	}

	return ret;
}

static uint8_t mmw_cfg_ana_hpf_bw_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	ana_hpf_bw_t ana_hpf_bw = {0x00};

	memcpy(&ana_hpf_bw, data, sizeof(ana_hpf_bw_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", ana_hpf_bw.sub_id, ana_hpf_bw.ant_id, ana_hpf_bw.bw);
	if(mmw_ana_hpf_bw_cfg(ana_hpf_bw.ant_id, ana_hpf_bw.bw)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

uint32_t mmw_dbg_step(uint8_t mmw_halt_type, uint32_t mmw_break_point);

static uint8_t mmw_cfg_dbg_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	dbg_cfg_t dbg_cfg = {0x00};
	memcpy(&dbg_cfg, data, sizeof(dbg_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", dbg_cfg.sub_id, dbg_cfg.halt_type, dbg_cfg.break_point);
	if(mmw_dbg_step(dbg_cfg.halt_type, dbg_cfg.break_point)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_dop_fft_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	dop_fft_cfg_t dop_fft = {0x00};

	memcpy(&dop_fft, data, sizeof(dop_fft_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", dop_fft.sub_id, dop_fft.flag);
	mmw_dop_fft_set(dop_fft.flag);

	return ret;
}

static uint8_t mmw_cfg_mmic_dbg_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	mmic_dbg_cfg_t mmic_dbg = {0x00};

	memcpy(&mmic_dbg, data, sizeof(mmic_dbg_cfg_t));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", mmic_dbg.sub_id, mmic_dbg.en, mmic_dbg.channel);
	if(mmw_set_mmic_para(mmic_dbg.en, mmic_dbg.channel, 0, 1, 1)) {
		ret = HIF_CMD_STATUS_IO;
	}

	return ret;
}

static uint8_t mmw_cfg_report_com_type_handler(uint8_t *data)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	report_com_type_t report_com_type = {0x00};

	memcpy(&report_com_type, data, sizeof(report_com_type_t));
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", report_com_type.sub_id, report_com_type.type);
    com_mode = report_com_type.type;

	return ret;
}


/*mmw get param handler*/
static void mmw_get_version_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;
	version_resp_t version_resp = {0x00};

	char *ver_str;
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	ver_str = mmw_version(NULL, &version_resp.build);
	if(ver_str != NULL) {
		version_resp.len = strlen(ver_str);
	} else {
		ret = HIF_CMD_STATUS_IO;
	}
	version_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:0x%08x arg[2]:%s\r\n", version_resp.sub_id, version_resp.len, version_resp.build, ver_str);
	memcpy(respPtr, &version_resp, sizeof(version_resp_t));
	memcpy(&respPtr[sizeof(version_resp_t)], ver_str, version_resp.len);

	HIF_MsgResp(msg, sizeof(version_resp_t) + version_resp.len, ret);
}

static void mmw_get_state_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	state_resp_t state_resp = {0x00};


	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	int val = mmw_state(&state_resp.frame_send);
	if(val >= 0) {
		state_resp.power_state = val;
	} else {
		ret = HIF_CMD_STATUS_IO;
	}
	state_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", state_resp.sub_id, state_resp.power_state, state_resp.frame_send);
	memcpy(respPtr, &state_resp, sizeof(state_resp_t));

	HIF_MsgResp(msg, sizeof(state_resp_t), ret);
}

static void mmw_get_power_save_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	power_save_resp_t power_save_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	uint8_t val = mmw_low_power_get();
	if(val) {
		power_save_resp.enable = 0x01;
	} else {
		power_save_resp.enable = 0x00;
	}
	power_save_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", power_save_resp.sub_id, power_save_resp.enable);
	memcpy(respPtr, &power_save_resp, sizeof(power_save_resp_t));

	HIF_MsgResp(msg, sizeof(power_save_resp_t), ret);
}

static void mmw_get_mode_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	mode_resp_t mode_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	int val = mmw_mode_get(&mode_resp.txrx, &mode_resp.work);
	if (mode_resp.txrx >= MMW_MIMO_LAST || val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}
	mode_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", mode_resp.sub_id, mode_resp.txrx, mode_resp.work);
	memcpy(respPtr, &mode_resp, sizeof(mode_resp_t));

	HIF_MsgResp(msg, sizeof(mode_resp_t), ret);
}

static void mmw_get_freq_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	freq_resp_t freq_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	int val = mmw_freq_get(&freq_resp.start, &freq_resp.max);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}

	freq_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", freq_resp.sub_id, freq_resp.start, freq_resp.max);
	memcpy(respPtr, &freq_resp, sizeof(freq_resp_t));

	HIF_MsgResp(msg, sizeof(freq_resp_t), ret);
}

static void mmw_get_range_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	range_resp_t range_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	int val = mmw_range_get (&range_resp.range, &range_resp.resol);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}
	range_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", range_resp.sub_id, range_resp.range, range_resp.resol);
	memcpy(respPtr, &range_resp, sizeof(range_resp_t));

	HIF_MsgResp(msg, sizeof(range_resp_t), ret);
}

static void mmw_get_veloc_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	veloc_resp_t veloc_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	int val = mmw_velocity_get (&veloc_resp.veloc, &veloc_resp.resol);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}
	veloc_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", veloc_resp.sub_id, veloc_resp.veloc, veloc_resp.resol);
	memcpy(respPtr, &veloc_resp, sizeof(veloc_resp_t));

	HIF_MsgResp(msg, sizeof(veloc_resp_t), ret);
}

static void mmw_get_frame_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	frame_resp_t frame_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);


	int val = mmw_frame_get(&frame_resp.period, &frame_resp.num);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}

	frame_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", frame_resp.sub_id, frame_resp.period, frame_resp.num);
	memcpy(respPtr, &frame_resp, sizeof(frame_resp_t));

	HIF_MsgResp(msg, sizeof(frame_resp_t), ret);
}

static void mmw_get_intv_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	intv_resp_t intv_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);


	int val = mmw_interval_get (&intv_resp.period, &intv_resp.intv_num);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}

	intv_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", intv_resp.sub_id, intv_resp.period, intv_resp.intv_num);
	memcpy(respPtr, &intv_resp, sizeof(intv_resp_t));

	HIF_MsgResp(msg, sizeof(intv_resp_t), ret);
}

static void mmw_get_chirp_param_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	chirp_param_resp_t chirp_param_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	int val = mmw_sweep_chirp_info_get(&chirp_param_resp.chirp_info);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}

	chirp_param_resp.sub_id = data[0];

	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d arg[2]:%d arg[3]:%d arg[4]:%d arg[5]:%d arg[6]:%d arg[7]:%d arg[8]:%d arg[9]:%d arg[10]:%d\r\n", \
		chirp_param_resp.sub_id, chirp_param_resp.chirp_info.start_freq, chirp_param_resp.chirp_info.max_freq, \
		chirp_param_resp.chirp_info.ramp_time, chirp_param_resp.chirp_info.sample_time, \
		chirp_param_resp.chirp_info.cfg.valid_start_MHz, chirp_param_resp.chirp_info.cfg.valid_BW_MHz, \
		chirp_param_resp.chirp_info.cfg.sweep_BW_MHz, chirp_param_resp.chirp_info.cfg.slope_MHz_us, \
		chirp_param_resp.chirp_info.cfg.period_ns, chirp_param_resp.chirp_info.cfg.sample_num, \
		chirp_param_resp.chirp_info.cfg.chirp_num);

	memcpy(respPtr, &chirp_param_resp, sizeof(chirp_param_resp_t));

	HIF_MsgResp(msg, sizeof(chirp_param_resp_t), ret);
}

static void mmw_get_chirp_num_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	chirp_num_resp_t chirp_num_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);
	CHIRP_INFO chirp_info;

	int val = mmw_chirp_get(&chirp_info);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	} else {
		chirp_num_resp.num = chirp_info.chirp_num;
	}

	chirp_num_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", chirp_num_resp.sub_id, chirp_num_resp.num);
	memcpy(respPtr, &chirp_num_resp, sizeof(chirp_num_resp_t));

	HIF_MsgResp(msg, sizeof(chirp_num_resp_t), ret);
}

static void mmw_get_fft_len_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	fft_len_resp_t fft_len_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	int val = mmw_fft_num_get(&fft_len_resp.range_len, &fft_len_resp.doppler_len);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	}

	fft_len_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", fft_len_resp.sub_id, fft_len_resp.range_len, fft_len_resp.doppler_len);
	memcpy(respPtr, &fft_len_resp, sizeof(fft_len_resp_t));

	HIF_MsgResp(msg, sizeof(fft_len_resp_t), ret);
}

static void mmw_get_coord_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	coord_resp_t coord_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	uint8_t val = mmw_coordinate_get();
	if(val > cart) {
		ret = HIF_CMD_STATUS_IO;
	} else {
		coord_resp.coord_type = val;
	}

	coord_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", coord_resp.sub_id, coord_resp.coord_type);
	memcpy(respPtr, &coord_resp, sizeof(coord_resp_t));

	HIF_MsgResp(msg, sizeof(coord_resp_t), ret);
}

static void mmw_get_data_cfg_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	datacfg_resp_t datacfg_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	datacfg_resp.datacfg_type = mmw_ctrl_data_type_get();

	datacfg_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", datacfg_resp.sub_id, datacfg_resp.datacfg_type);
	memcpy(respPtr, &datacfg_resp, sizeof(datacfg_resp_t));

	HIF_MsgResp(msg, sizeof(datacfg_resp_t), ret);
}

static void mmw_get_clutter_rm_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	clutter_rm_resp_t clutter_rm_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	uint8_t  enable = mmw_clutter_remove_get();
	if(enable) {
		clutter_rm_resp.enable = 0x01;
	} else {
		clutter_rm_resp.enable = 0x00;
	}
	mmw_clutter_factor_get(&clutter_rm_resp.clutter_fac);

	clutter_rm_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", clutter_rm_resp.sub_id, clutter_rm_resp.enable, clutter_rm_resp.clutter_fac);
	memcpy(respPtr, &clutter_rm_resp, sizeof(clutter_rm_resp_t));

	HIF_MsgResp(msg, sizeof(clutter_rm_resp_t), ret);
}

static void mmw_get_cfar_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	cfar_resp_t cfar_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);
	bool static_rm = false;

	int val = mmw_cfar_get(&static_rm, &cfar_resp.snr);
	if (val != 0) {
		ret = HIF_CMD_STATUS_IO;
	} else {
		cfar_resp.static_rm = static_rm;
	}

	cfar_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", cfar_resp.sub_id, cfar_resp.static_rm, cfar_resp.snr);
	memcpy(respPtr, &cfar_resp, sizeof(cfar_resp_t));

	HIF_MsgResp(msg, sizeof(cfar_resp_t), ret);
}

static void mmw_get_read_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;
	read_get_t read_get = {0x00};
	read_resp_t read_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	memcpy(&read_get, data, sizeof(read_get));
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", read_get.sub_id, read_get.offset, read_get.read_num);
	read_resp.sub_id = read_get.sub_id;
	uint16_t len = sizeof(read_resp_t)-sizeof(Complex16_RealImag);
	uint16_t data_len = 1 + sizeof(read_resp) - sizeof(Complex16_RealImag) + read_get.read_num * sizeof(Complex16_RealImag);

	if(read_get.read_num && data_len <= HOST_CMD_MAX_PAYLOAD) {
		Complex16_RealImag *realImage = (Complex16_RealImag *)OSI_Malloc(read_get.read_num * sizeof(Complex16_RealImag));
		memset(realImage, 0, read_get.read_num*sizeof(Complex16_RealImag));

		if(mmw_fft_data(realImage, read_get.read_num, 0, 0, read_get.offset)) {
			ret = HIF_CMD_STATUS_IO;
		}

		HIF_DBG("subId:0x%02x", read_resp.sub_id);
		for(uint8_t i = 0; i < (read_get.read_num - 1); i++) {
			HIF_DBG(" arg[%d]:%d arg[%d]:%d", 2 * i, read_resp.fft_data[i].imag, 2 * i + 1, read_resp.fft_data[i].real);
		}
		HIF_DBG(" arg[%d]:%d arg[%d]:%d\r\n", 2 * read_get.read_num - 2, read_resp.fft_data[read_get.read_num-1].imag, \
		2 * read_get.read_num - 1, read_resp.fft_data[read_get.read_num-1].real);

		memcpy(respPtr, &read_resp, len);
		memcpy(&respPtr[len], realImage, read_get.read_num * sizeof(Complex16_RealImag));
		HIF_MsgResp(msg, len + read_get.read_num * sizeof(Complex16_RealImag), ret);
		OSI_Free(realImage);
	} else {
		ret = HIF_CMD_STATUS_PARAM;
		HIF_DBG("subId:0x%02x\r\n\r\n", read_resp.sub_id);
		memcpy(respPtr, &read_resp, len);
		HIF_MsgResp(msg, len, ret);
	}

}

static void mmw_get_report_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	report_resp_t report_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	report_resp.report_type = gui_data_report;
	report_resp.report_num = repot_frame_num;

	report_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d\r\n", report_resp.sub_id, report_resp.report_type, report_resp.report_num);
	memcpy(respPtr, &report_resp, sizeof(report_resp_t));

	HIF_MsgResp(msg, sizeof(report_resp_t), ret);
}

static void mmw_get_baudrate_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	baudrate_resp_t baudrate_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	baudrate_resp.baud = mmw_cfg_baudrate;
	baudrate_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d\r\n", baudrate_resp.sub_id, baudrate_resp.baud);
	memcpy(respPtr, &baudrate_resp, sizeof(baudrate_resp_t));

	HIF_MsgResp(msg, sizeof(baudrate_resp_t), ret);
}

static void mmw_get_noise_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;
	noise_resp_t noise_resp = {0x00};
	uint16_t range_fft_num;
	uint16_t doppler_fft_num;
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
	uint16_t data_len = 1 + sizeof(noise_resp) + 2 * range_fft_num;
	noise_resp.sub_id = data[0];
	noise_resp.ft_num = range_fft_num;

	if(range_fft_num && data_len <= HOST_CMD_MAX_PAYLOAD) {
		uint16_t *noise = (uint16_t *)OSI_Malloc(2 * range_fft_num);
		memset(noise, 0, 2 * range_fft_num);

		memcpy(respPtr, &noise_resp, sizeof(noise_resp_t));

		if(mmw_noisefloor_get(noise, 0, range_fft_num)) {
			ret = HIF_CMD_STATUS_IO;
		}

		HIF_DBG("subId:0x%02x arg[0]:%d", noise_resp.sub_id, noise_resp.ft_num);
		for(uint8_t i = 0; i < (noise_resp.ft_num - 1); i++) {
			HIF_DBG(" arg[%d]:%d", i + 1, noise[i]);
		}
		HIF_DBG(" arg[%d]:%d\r\n", noise_resp.ft_num, noise[noise_resp.ft_num-1]);

		memcpy(&respPtr[sizeof(noise_resp_t)], noise, 2 * range_fft_num);
		HIF_MsgResp(msg, sizeof(noise_resp_t) + 2 * range_fft_num, ret);
		OSI_Free(noise);
	} else {
		ret = HIF_CMD_STATUS_IO;
		HIF_DBG("subId:0x%02x arg[0]:%d\r\n", noise_resp.sub_id, noise_resp.ft_num);
		memcpy(respPtr, &noise_resp, sizeof(noise_resp_t));
		HIF_MsgResp(msg, sizeof(noise_resp_t)+2*range_fft_num, ret);
	}

}

static void mmw_get_tone_handler(HIF_MsgHdr_t *msg)
{
	uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	tone_resp_t tone_resp = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	tone_resp.enable = tone_cfg_enable;
	tone_resp.tone_txid = tone_cfg_txid;
	tone_resp.tone_freq = tone_cfg_freq;
	tone_resp.sub_id = data[0];
	HIF_DBG("subId:0x%02x arg[0]:%d arg[1]:%d arg[2]:%d\r\n", tone_resp.sub_id, tone_resp.enable, tone_resp.tone_txid, tone_resp.tone_freq);
	memcpy(respPtr, &tone_resp, sizeof(tone_resp_t));

	HIF_MsgResp(msg, sizeof(tone_resp_t), ret);
}

static void mmw_get_report_cfg_handler(HIF_MsgHdr_t *msg)
{
    uint8_t ret = HIF_CMD_STATUS_SUCCESS;
    report_cfg_get_t report_cfg_get = {0x00};
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	uint8_t tx_cnt = 0;
	uint8_t rx_cnt= 0;
	uint16_t range_fft_num = 0;
	uint16_t doppler_fft_num = 0;

	ret = get_tx_rx_num(&tx_cnt, &rx_cnt);
	if(ret) {
		goto exit;
	}
	ret = mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
	if(ret) {
		goto exit;
	}

	report_cfg_get.frame_len = tx_cnt * rx_cnt * range_fft_num * doppler_fft_num;
	if((report_cfg_get.frame_len * 4) % CUBE_DATA_SIZE > 0) {
		report_cfg_get.split_num = report_cfg_get.frame_len * 4 / CUBE_DATA_SIZE + 1;
	} else {
		report_cfg_get.split_num = report_cfg_get.frame_len * 4 / CUBE_DATA_SIZE;
	}
    report_cfg_get.split_size = CUBE_DATA_SIZE;
    report_cfg_get.sub_id = data[0];
    memcpy(respPtr, &report_cfg_get, sizeof(report_cfg_get_t));

exit:
    HIF_MsgResp(msg, sizeof(report_cfg_get_t), ret);
}


mmw_cfg_msg_t mmw_cfg_msg[] = {
	{mmw_open, 			mmw_cfg_open_handler},
	{mmw_close, 		mmw_cfg_close_handler},
	{mmw_start, 		mmw_cfg_start_handler},
	{mmw_stop, 			mmw_cfg_stop_handler},
	{mmw_power_save, 	mmw_cfg_power_save_handler},
	{mmw_mode, 			mmw_cfg_mode_handler},
	{mmw_freq, 			mmw_cfg_freq_handler},
	{mmw_range, 		mmw_cfg_range_handler},
	{mmw_veloc, 		mmw_cfg_veloc_handler},
	{mmw_frame, 		mmw_cfg_frame_handler},
	{mmw_intv, 			mmw_cfg_intv_handler},
	{mmw_chirp_num, 	mmw_cfg_chirp_num_handler},
	{mmw_coord, 		mmw_cfg_coord_handler},
	{mmw_data_cfg, 		mmw_cfg_data_cfg_handler},
	{mmw_clutter_rm, 	mmw_cfg_clutter_rm_handler},
	{mmw_cfar, 			mmw_cfg_cfar_handler},
	{mmw_report, 		mmw_cfg_report_handler},
	{mmw_uart, 			mmw_cfg_uart_handler},
	{mmw_baudrate, 		mmw_cfg_baudrate_handler},
	{mmw_noise, 		mmw_cfg_noise_handler},
	{mmw_tone, 			mmw_cfg_tone_handler},
	{mmw_rx_gain, 		mmw_cfg_rx_gain_handler},
	{mmw_calib, 		mmw_cfg_calib_handler},
	{mmw_ana_hpf_bw, 	mmw_cfg_ana_hpf_bw_handler},
	{mmw_dbg, 			mmw_cfg_dbg_handler},
	{mmw_dop_fft, 		mmw_cfg_dop_fft_handler},
	{mmw_mmic_dbg, 		mmw_cfg_mmic_dbg_handler},
    {mmw_com_type,      mmw_cfg_report_com_type_handler},
};


mmw_get_msg_t mmw_get_msg[] = {
	{mmw_ver, 			mmw_get_version_handler},
	{mmw_stat, 			mmw_get_state_handler},
	{mmw_power_save, 	mmw_get_power_save_handler},
	{mmw_mode, 			mmw_get_mode_handler},
	{mmw_freq, 			mmw_get_freq_handler},
	{mmw_range, 		mmw_get_range_handler},
	{mmw_veloc, 		mmw_get_veloc_handler},
	{mmw_frame, 		mmw_get_frame_handler},
	{mmw_intv, 			mmw_get_intv_handler},
	{mmw_chirp_param, 	mmw_get_chirp_param_handler},
	{mmw_chirp_num, 	mmw_get_chirp_num_handler},
	{mmw_fft_len, 		mmw_get_fft_len_handler},
	{mmw_coord, 		mmw_get_coord_handler},
	{mmw_data_cfg, 		mmw_get_data_cfg_handler},
	{mmw_clutter_rm, 	mmw_get_clutter_rm_handler},
	{mmw_cfar, 			mmw_get_cfar_handler},
	{mmw_read, 			mmw_get_read_handler},
	{mmw_report, 		mmw_get_report_handler},
	{mmw_baudrate, 		mmw_get_baudrate_handler},
	{mmw_noise, 		mmw_get_noise_handler},
	{mmw_tone, 			mmw_get_tone_handler},
    {mmw_report_get, 	mmw_get_report_cfg_handler},
};

static int mmw_params_cfg_handler(HIF_MsgHdr_t *msg)
{
	bool cmdExist = false;
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t *respPtr = (uint8_t *)HIF_Msg_AckDataPtr(msg);
	uint8_t subId = data[0];
	cfg_resp_t cfg_resp = {0x00};
	uint8_t ret = 0x00;

	for (uint8_t i = 0; i < sizeof(mmw_cfg_msg) / sizeof(mmw_cfg_msg[0]); i++) {
		if (subId == mmw_cfg_msg[i].msg_sub_id) {
			mmw_cfg_callback func = mmw_cfg_msg[i].func;
			ret = func(data);
			cfg_resp.sub_id = subId;
			memcpy(respPtr, &cfg_resp, sizeof(cfg_resp_t));
			HIF_MsgResp(msg, sizeof(cfg_resp_t), ret);
			cmdExist = true;
			break;
		}
	}
	if(!cmdExist) {
		HIF_MsgResp(msg, msg->length, HIF_CMD_STATUS_UNSUPPORT);
	}
	return 0;
}

static int mmw_params_get_handler(HIF_MsgHdr_t *msg)
{
	bool cmdExist = false;
	uint8_t *data = (uint8_t *)(msg + 1);
	uint8_t subId = data[0];

	for (uint8_t i = 0; i < sizeof(mmw_get_msg) / sizeof(mmw_get_msg[0]); i++) {
		if (subId == mmw_get_msg[i].msg_sub_id) {
			mmw_get_callback func = mmw_get_msg[i].func;
			func(msg);
			cmdExist = true;
			break;
		}
	}
	if(!cmdExist) {
		HIF_MsgResp(msg, msg->length, HIF_CMD_STATUS_UNSUPPORT);
	}
	return 0;
}

int mmw_config_init(void)
{
	int ret = -1;

	HIF_InitParam_t hifInitParam;
	HIF_DefaultInitParam(&hifInitParam, HIF_COM_TYPE_UART, 921600);
	ret = HIF_Init(&hifInitParam);
	if (ret != 0) {
		printk("host_interface_init fail %d\n", ret);
		//return ret;
	}

	HIF_MsgHdl_Regist(HIF_MMW_CFG_MSG_ID, mmw_params_cfg_handler);
	HIF_MsgHdl_Regist(HIF_MMW_GET_MSG_ID, mmw_params_get_handler);
	return 0;
}


