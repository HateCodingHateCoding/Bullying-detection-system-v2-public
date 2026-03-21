/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef __MMW_POINTCLOUD_H__
#define __MMW_POINTCLOUD_H__

/* Include Files */
#include "config.h"
#include "mmw_ctrl.h"
#include "mmw_app_pointcloud.h"
#include "mmw_alg_doa.h"

#ifdef __cplusplus
extern "C" {
#endif

/* skip num of range bin from start */
#define MICRO_RANGE_BIN_SKIP_NUM      3
/* max num of range*/
#define MICRO_RANGE_BIN_NUM           256

#define MICRO_CHIRP_NUM_LOG2    3 //4
#define MICRO_CHIRP_NUM         (1<<MICRO_CHIRP_NUM_LOG2)
#define MICRO_DOP_NUM_LOG2      (MICRO_CHIRP_NUM_LOG2 + 1)
#define MICRO_DOP_NUM           (1<<MICRO_DOP_NUM_LOG2)
#define MDOP_FFT_BUF_NUM        (MICRO_DOP_NUM * MMW_ANT_CHANNEL_NUM)

#define NOISE_SEC_LEFT          1
#define NOISE_SEC_RIGHT         1
#define NOISE_SEC_NUM           (1 + NOISE_SEC_LEFT + NOISE_SEC_RIGHT)

typedef struct _micro_pointcloud_t {
	uint32_t    frame_num;      /* micro frame counter */
	uint16_t    dop_cfar_num;
	uint16_t    points_num;
	uint8_t     dop_chirp_num;
	uint8_t     cfar_snr_th;
	uint8_t     cfar_stationary;
	uint8_t     frame_div_idx;
	uint8_t     frame_div_cnt;
	uint8_t     frame_div_cnt_set;
	int16_t     elev_filter_deg_q7[2];
	int16_t     azim_filter_deg_q7[2];

	/* API param */
	uint16_t    range_bin_skip;
	uint16_t    range_bin_max;
	uint16_t    range_min_mm;  /* user param  convert to range_bin_skip */
	uint16_t    range_max_mm;  /* user param  convert to range_bin_max */
	uint16_t    dop_peak_max;
	const int16_t  *hanning_win;
	void           *proc_bufs;      /* buffer for mdop fft and dbf */
} MPC_CTRL;

__STATIC_INLINE void * os_malloc(uint32_t size)
{
	return OSI_Malloc(size);
}
__STATIC_INLINE void os_free(void *buffer)
{
	OSI_Free(buffer);
}

#define MICRO_POINT_MAX         256  //default micro point num

/* initial and reset */
int  mmw_micro_point_init(void);
void mmw_micro_point_deinit(void);
void mmw_micro_point_restart(void);

/* data flow processing */
typedef int (*MicroPointHandle)(uint32_t range_idx, int veloc_idx, uint32_t snr_linear, MmwAngleInfo_t *angle, void *arg);
uint32_t mmw_micro_point_num_get(void);
uint32_t mmw_micro_doppler_num_get(void);
bool     mmw_micro_point_frame(void);
uint32_t mmw_micro_point_process(MicroPointHandle handler, void* arg);

/* parameters config */
int mmw_micro_point_range_set (uint32_t min_range_mm, uint32_t max_range_mm);
int mmw_micro_point_range_get (uint32_t *min_range_mm, uint32_t *max_range_mm);
int mmw_micro_point_angle_set (int16_t min_azim_q7, int16_t max_azim_q7,
                                        int16_t min_elev_q7, int16_t max_elev_q7);
int mmw_micro_point_angle_get (int16_t *min_azim_q7, int16_t *max_azim_q7,
                                        int16_t *min_elev_q7, int16_t *max_elev_q7);
int mmw_micro_cfar_snr_set(uint32_t snr_linear);
uint32_t mmw_micro_cfar_snr_get(void);
int mmw_micro_cfar_stationary_set(bool range_sta, bool dop_sta);
int mmw_micro_cfar_stationary_get(bool *range_sta, bool *dop_sta);
int mmw_micro_frame_rate_set(uint8_t mmw_frame_div_cnt);
uint8_t mmw_micro_frame_rate_get(void);
MPC_CTRL *mmw_micro_g_mpc_ctrl_get(void);

/**
 * @brief Convert radar coordinate to user coordinate defined by dev board.
 * @param x: x in radar coordinate system
 * @param y: y in radar coordinate system
 * @param z: z in radar coordinate system
 * @param ptr_cart: output buffer in user coordinate system
 */
extern void mmw_micro_frame_trans_radar_coord_to_user_coord(int16_t x, int16_t y, int16_t z, PointCloud_Cart* ptr_cart);

#ifdef __cplusplus
}
#endif

#endif /* __MMW_POINTCLOUD_H__ */

