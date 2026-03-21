#ifndef MMW_HIF_H
#define MMW_HIF_H

#include "hif.h"
#include "mmw_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t (*mmw_cfg_callback)(uint8_t *data);

typedef void (*mmw_get_callback)(HIF_MsgHdr_t *msg);


/*****cfg payload******/
/*| subId | msg data |*/

/******get payload*****/
/*| subId | msg data |*/

/**********resp payload*********/
/*| status | subId | msg data |*/

typedef enum {
	mmw_open		= 0,
	mmw_close		= 1,
	mmw_ver			= 2,
	mmw_start		= 3,
	mmw_stop		= 4,
	mmw_stat		= 5,
	mmw_power_save	= 6,
	mmw_mode		= 7,
	mmw_freq		= 8,
	mmw_range		= 9,
	mmw_veloc		= 10,
	mmw_frame		= 11,
	mmw_intv		= 12,
	mmw_chirp_param	= 13,
	mmw_chirp_num	= 14,
	mmw_fft_len		= 15,
	mmw_coord		= 16,
	mmw_data_cfg	= 17,
	mmw_data_report	= 18,
	mmw_clutter_rm	= 19,
	mmw_cfar		= 20,
	mmw_read		= 21,
	mmw_report		= 22,
	mmw_uart		= 23,
	mmw_baudrate	= 24,
	mmw_noise		= 25,
	mmw_tone		= 26,
	mmw_rx_gain		= 27,
	mmw_calib		= 28,
	mmw_ana_hpf_bw	= 29,
	mmw_dbg			= 30,
	mmw_dop_fft		= 31,
	mmw_mmic_dbg	= 32,
	mmw_ant_calib	= 33,
	mmw_mpc_snr		= 34,
	mmw_mpc_range	= 35,
	mmw_com_type    = 36,
	mmw_report_get  = 37,
	mmw_id_max,
} mmw_sub_id_e;

/*mmw config response payload struct*/
typedef struct {
	uint8_t sub_id;
} cfg_resp_t;

/*get mmw param payload struct*/
typedef struct {
	uint8_t sub_id;
} mmw_get_t;


typedef struct {
	uint8_t sub_id;
	uint8_t use_msi;
	uint8_t pll_lc;
} open_msg_cfg_t;

typedef struct {
	uint8_t sub_id;
} close_msg_cfg_t;

typedef struct {
	uint8_t sub_id;
} start_msg_cfg_t;

typedef struct {
	uint8_t sub_id;
} stop_msg_cfg_t;

typedef struct {//array len = len
	uint8_t sub_id;
	uint8_t len;
	uint8_t reserve[2];
	uint32_t build;
	char ver[0];
} version_resp_t;

typedef enum {
	power_down 		= MMW_STATE_POWER_DOWN,
	power_active	= MMW_STATE_ACTIVE,
	power_sleep		= MMW_STATE_SLEEP,
	power_running	= MMW_STATE_RUNNING,
} power_state_e;

typedef struct {
	uint8_t sub_id;
	uint8_t power_state;
	uint8_t reserve[2];
	uint32_t frame_send;
} state_resp_t;

typedef struct {
	uint8_t sub_id;
	uint8_t enable;
} power_save_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t enable;
} power_save_resp_t;

typedef enum {
	txrx_1T1R = MMW_MIMO_1T1R,
	txrx_1T3R = MMW_MIMO_1T3R,
	txrx_2T3R = MMW_MIMO_2T3R,
	txrx_1T4R = MMW_MIMO_1T4R,
	txrx_2T4R = MMW_MIMO_2T4R,
	txrx_1T2R = MMW_MIMO_1T2R,
} txrx_e;

typedef enum {
	mode_1DFFT = MMW_WORK_MODE_1DFFT,
	mode_2DFFT = MMW_WORK_MODE_2DFFT,
} work_e;

typedef struct {
	uint8_t sub_id;
	uint8_t txrx;
	uint8_t work;
} mode_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t txrx;
	uint8_t work;
} mode_resp_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t start;
	uint32_t max;
} freq_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t start;
	uint32_t max;
} freq_resp_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t range;
	uint32_t resol;
} range_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t range;
	uint32_t resol;
} range_resp_t;


typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t veloc;
	uint32_t resol;
} veloc_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t veloc;
	uint32_t resol;
} veloc_resp_t;


typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t period;
	uint32_t intv_num;
} intv_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t period;
	uint32_t intv_num;
} intv_resp_t;


typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t period;
	uint32_t num;
} frame_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t period;
	uint32_t num;
} frame_resp_t;

typedef struct {
	uint8_t sub_id;
} chirp_param_get_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	SweepChirpInfo chirp_info;
} chirp_param_resp_t;

typedef struct {
	uint8_t sub_id;
	uint8_t num;
} chirp_num_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t num;
} chirp_num_resp_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reverse;
	uint16_t range_len;
	uint16_t doppler_len;
} fft_len_resp_t;

typedef enum {
	polar	= MMW_COORDINATE_TYPE_POLAR,
	cart	= MMW_COORDINATE_TYPE_CART,
} coord_type_e;

typedef struct {
	uint8_t sub_id;
	uint8_t coord_type;
} coord_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t coord_type;
} coord_resp_t;

typedef enum {
	type_datacfg_disable 	= MMW_DATA_TYPE_DISABLE,
	type_1dfft 				= MMW_DATA_TYPE_1DFFT,
	type_2dfft 				= MMW_DATA_TYPE_2DFFT,
	type_3d_point 			= MMW_DATA_TYPE_3D_POINTS,
	type_2d_track			= 8,
	type_motion				= MMW_DATA_TYPE_MOTION,
	type_onoff				= MMW_SLEEP_REQ_EVENT,
	type_restore			= 0xff,
} datacfg_type_e;

typedef struct {
	uint8_t sub_id;
	uint8_t datacfg_type;
} datacfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t datacfg_type;
} datacfg_resp_t;

typedef struct {	//array len = data_num
	uint8_t sub_id;
	uint8_t data_type;
	uint8_t reserve[2];
	uint32_t data_num;
	//PointCloud3D point[1];
} data_report_t;

typedef struct {
	uint8_t sub_id;
	uint8_t static_rm;
	uint8_t reserve[2];
	uint32_t snr;
} cfar_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t static_rm;
	uint8_t reserve[2];
	uint32_t snr;
} cfar_resp_t;


typedef struct {
	uint8_t sub_id;
	uint8_t enable;
	uint8_t clutter_fac;
} clutter_rm_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t enable;
	uint8_t clutter_fac;
} clutter_rm_resp_t;


typedef enum {
	type_rf,
	type_dig,
} gain_type_e;

typedef struct {
	uint8_t sub_id;
	uint8_t gain_type;
	uint8_t ant_id;
	union {
		uint8_t u_gain;
		int8_t i_gain;
	};
} rx_gain_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t ant_id;
	uint8_t bw;
} ana_hpf_bw_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve;
	uint16_t offset;
	uint16_t read_num;
} read_get_t;

typedef struct {//read_num = array len
	uint8_t sub_id;
	uint8_t reserve[3];
	Complex16_RealImag fft_data[1];
} read_resp_t;

typedef enum {
	type_cube 				= GUI_DATA_TYPE_CUBE,
	type_mmic				= GUI_DATA_MMIC,
	type_point				= GUI_DATA_TYPE_POINTS,
	type_track				= GUI_DATA_TYPE_TRACKING,
	type_points_trace		= GUI_DATA_TYPE_POINTS_TRACKING,
	type_report_disable		= GUI_DATA_TYPE_DISABLE,
} report_type_e;

typedef struct {
	uint8_t sub_id;
	uint8_t report_type;
	uint8_t reserve[2];
	uint32_t report_num;
} report_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t report_type;
	uint8_t reserve[2];
	uint32_t report_num;
} report_resp_t;


typedef struct {
	uint8_t sub_id;
	uint8_t enable;
} uart_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t baud;
} baudrate_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t baud;
} baudrate_resp_t;

typedef struct {
	uint8_t sub_id;
	uint8_t halt_type;
	uint8_t reserve[2];
	uint32_t break_point;
} dbg_cfg_t;

typedef enum {
	cali,
	save,
	cali_set,
	m43,
	m62,
	m62_azim,
	m62_elev,
	a43,
	a43_azim,
	a43_elev,
	a43_elev_dbf,
} ant_calib_type_e;

typedef struct {
	uint8_t sub_id;
	uint8_t ant_calib_type;
	union {
		//calib_t calib;
		bool save_or_reset;
		uint32_t calib_data[8];
	};
} ant_calib_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t snr;
} mpc_snr_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t min;
	uint32_t max;
} mpc_range_cfg_t;

typedef struct {
	uint8_t sub_id;
} noise_cfg_t;

typedef struct {	//array len = ft_num
	uint8_t sub_id;
	uint8_t reserve;
	uint16_t ft_num;
	uint16_t noise[0];
} noise_resp_t;

typedef struct {
	uint8_t sub_id;
	uint8_t enable;
	uint8_t tone_txid;
	uint8_t reserve;
	uint32_t tone_freq;
} tone_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t enable;
	uint8_t tone_txid;
	uint8_t reverse;
	uint32_t tone_freq;
} tone_resp_t;

typedef enum {
	type_load,
	type_save,
	type_ant,
	type_chirp,
} calib_type_e;

typedef struct {
	uint8_t onoff;
	uint8_t reserve[3];
	uint32_t mask;
} calib_sw_t;

typedef struct {
	uint8_t sub_id;
	uint8_t calib_type;
	uint8_t reserve[2];
	union {
		uint32_t chip_info[6];
		calib_sw_t calib_sw;
	};
} calib_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	uint32_t flag;
} dop_fft_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t reserve[3];
	int32_t en;
	int32_t channel;
} mmic_dbg_cfg_t;

typedef struct {
	uint8_t sub_id;
	uint8_t type;
} report_com_type_t;

typedef struct {
	uint8_t sub_id;
    uint8_t split_num;
    uint16_t split_size;
	uint32_t frame_len;
} report_cfg_get_t;



/**********************************************/

typedef struct {
	uint8_t msg_sub_id;
	mmw_cfg_callback func;
} mmw_cfg_msg_t;

typedef struct {
	uint8_t msg_sub_id;
	mmw_get_callback func;
} mmw_get_msg_t;

int mmw_config_init(void);


#ifdef __cplusplus
}
#endif


#endif
