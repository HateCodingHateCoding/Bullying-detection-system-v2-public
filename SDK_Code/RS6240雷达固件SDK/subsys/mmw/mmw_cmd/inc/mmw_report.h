
#ifndef MMW_REPORT_H
#define MMW_REPORT_H

#include "hif.h"
#include "mmw_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CUBE_DATA_SIZE	(3072)
#define CUBE_HEAD_LEN	(12)
#define CUBE_SIZE_MAX	(CUBE_HEAD_LEN+CUBE_DATA_SIZE)


typedef struct {
    uint32_t frame_idx;
    uint32_t frame_len;
	uint32_t offset;
} MMW_FRAME_UPLOAD;

typedef struct {
    void    *payload;
    uint32_t length;
} MMW_REPORT_DATA;


int get_tx_rx_num(uint8_t *tx, uint8_t *rx);

void mmw_report_param_get(void);

void mmw_data_cube_spi_report_init(void);

void mmw_data_cube_spi_report_deinit(void);

int mmw_ctrl_data_cube_spi_report_cb(void *mmw_data, void *arg);

void mmw_data_report_hif_init(uint8_t mode, uint32_t rate, uint8_t type);

void mmw_data_report_hif_deinit(void);

int mmw_ctrl_data_cube_uart_report_cb(void *mmw_data, void *arg);

#define MMW_TL_TYPE_POINTS            4
#define MMW_TL_TYPE_TARGET            5

#define MMW_TL_FLAG_POLAR             0  /* polar */
#define MMW_TL_FLAG_CART              BIT(0)  /* cartesian */
#define MMW_TL_FLAG_MICRO             BIT(1)  /* static pointcloud */

/* Skip frame for radar analysis application */
#define RADAR_ANALYSIS_SKIP_FRAME_NUM	  2
typedef struct {
	uint8_t  type;
	uint8_t  flag;
	uint16_t length;
} TL_HEADER;

typedef struct {
	uint16_t track_uid;
	uint8_t  track_state;
	uint8_t  points_num;
	/* 1LSB=1cm of range, or 1LSB=0.01 degree of angle.*/
	int16_t  x;
	int16_t  y;
	int16_t  z;
	/* vx, vy, vz: 1LSB=1cm/s */
	int16_t  vx;
	int16_t  vy;
	int16_t  vz;
} ClusterTrackElm;

typedef struct {
	PointCloud3D    *motion_points;
	PointCloud3D    *micro_points;
	ClusterTrackElm *tracker_objs;
	uint16_t         motion_points_num;
	uint16_t         micro_points_num;
	uint16_t         tracker_objs_num;
} Detection3D_Data;

typedef struct {
	uint32_t  proc_frame_num;
	uint32_t  proc_time_us;
} Detection3D_State;


#define POINT_DATA_SIZE	(3072)

int mmw_ctrl_data_point_report_cb(void *mmw_data, void *arg);

int mmw_objs_data_report(uint32_t frame_idx, MMW_REPORT_DATA *motion, MMW_REPORT_DATA *micro, MMW_REPORT_DATA *track);

/* obtain the skip frame status,it has been completed when returning to 0, skip RADAR_ANALYSIS_SKIP_FRAME_NUM frame after call mmw_ctrl_start() */
void radar_analysis_reset_skip_frame_status(void);


#ifdef __cplusplus
}
#endif


#endif


