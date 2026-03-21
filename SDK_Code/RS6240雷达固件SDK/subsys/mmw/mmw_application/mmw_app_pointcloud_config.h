#ifndef __MMW_APP_POINTCLOUD_CONFIG_H__
#define __MMW_APP_POINTCLOUD_CONFIG_H__

#include <stdint.h>
#include "mmw_alg_pointcloud_typedef.h"
#include "float.h"

#define MMW_MAX_MOTION_CFAR_NUM					768					/* the maximum CFAR number of motion point cloud */
#define MMW_MAX_PRESENCE_MOTION_REPORT_NUM		768					/* the maximum report number of presecne point cloud */
#define MMW_START_BIN_IDX						4					/* due to the influenceof the direct wave,micro motion point cloud need skip the first 4 range bin */
#define MMW_DIRECT_WAVE_RANGE_MM				200					/* due to the influenceof the direct wave,motion point cloud need raise threshold in the first 200 mm */
#define MMW_DIRECT_WAVE_THRES					1995.2623149f		/* 10 * log10(3.1622777) = 33dB */

typedef struct{
	float range_threshold_idx[2];			/* report point cloud range threshold,first is left, second is right */
	float positive_vel_threshold_idx[2];	/* report point cloud positive velocity threshold,first is left, second is right */
	float negative_vel_threshold_idx[2];	/* report point cloud negative velocity threshold,first is left, second is right */
} MmwPointCloudFilterConfig_t;

typedef struct{
	/*	if enalbe clutter config,frame_type must be DATA_BOX_2D_FRAME_MODE
		if use micro motion or motion point cloud,clutter_en or clutter_halt_en need disable
		if radar is placed on a mobile platform, clutter need disable
	*/ 
	MmwPointClutterRemoval_e clutter_rm_method;		/* See MmwPointClutterRemoval_e enumerate */
	uint8_t clutter_halt_en;						/* set static clutter update,0 is uodate,1 is not update,2 is auto update */
	uint8_t clutter_cover_time;						/* use greater clutter cover time,accuracy is higher,range is 0~7 */
	uint8_t recep_attenua_config;					/* recep attenua config range is [0, 6, 12, 18, 24](1 LSB = 1 dB),if max trigger range < 5m,need set this */
	uint8_t trans_attenua_config;					/* trans attenua config range is 0~8(1 LSB = 1 dB) */
	uint8_t hpf_bandwith_config;					/* hpf bandwith config,config value [31, 16, 8, 4, 2, 1],corresponding effective value is [0.5, 1, 2, 4, 8, 16](1 LSB = 1 MHz) */
} MmwPointCloudDetectionConfig_t;

typedef struct{
	uint16_t motion_point_cloud_max_cfar_num;			/* set motion point cloud max cfar number,range is 0~768 */
	PeakGroupingMethod_e peak_grouping_method;			/* default method for peak grouping process */
	uint8_t static_rm;									/* after enable,the moving point cloud will not report targets that are relativelys stationary to the radar;0 is disable,1 is enable */
	float threshold_snr_pwr_lin;						/* snr for software snr filter for non-coherent power, unit is W */
	uint8_t psic_2nd_pass_thres;						/* threshold in psic_2nd_pass_filter, 3 is recommanded by default */
} MmwMotionPointCloudConfig_t;

typedef struct {
	MmwPointCloudFilterConfig_t mmw_point_cloud_filter_config;
	MmwPointCloudDetectionConfig_t mmw_point_cloud_detection_config;
	MmwMotionPointCloudConfig_t mmw_motion_point_cloud_config;
	MmwMountType_e mount_type; 				/* mount type for CHIP, different mount type has different 'g_azi_geometry' and 'g_elev_geometry' */
	uint8_t auto_gain_flag;					/* use auto gain in fft or not, recomand to use autogain in application */
} MmwPointCloudUserCfg_t;

/* obtain the algorithm layer configuration */
extern MmwPointCloudUserCfg_t *mmw_point_cloud_get_user_cfg(void);
/* obtain the algorithm layer configuration (cosnt) */
extern const MmwPointCloudUserCfg_t *mmw_point_cloud_get_user_cfg_const(void);

#if (CONFIG_SOC_RS6240 || CONFIG_SOC_RS7241)
/* For RS6240, ant calibration is done by software, so calibration data may be got outside point cloud application */
Complexf32_RealImag *mmw_get_g_rx_ant_calib_data(void);
const Complexf32_RealImag *mmw_get_g_rx_ant_calib_data_const(void);
#endif

/* Get varies ant geometry structure used in point cloud application */
MmwPointAntGeometry_t *mmw_get_g_azi_geometry_struct(void);
MmwPointAntGeometry_t *mmw_get_g_elev_geometry_struct(void);
const MmwPointAntGeometry_t *mmw_get_g_azi_geometry_struct_const(void);
const MmwPointAntGeometry_t *mmw_get_g_elev_geometry_struct_const(void);

#endif
