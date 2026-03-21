#include "mmw_ctrl.h"
#include "mmw_app_pointcloud_config.h"
#include "float.h"
#include "stdbool.h"
#include "stdio.h"

/* This configuration is used for users to config some function after init the track */
MmwPointCloudUserCfg_t g_mmw_det_3d_user_cfg = {
	.mmw_point_cloud_filter_config = 
	{
		.range_threshold_idx = {FLT_MIN, FLT_MAX},
		.negative_vel_threshold_idx = {FLT_MIN, FLT_MAX},
		.positive_vel_threshold_idx = {FLT_MIN, FLT_MAX}
	},
	
	.mmw_point_cloud_detection_config = 
	{
#if CONFIG_MMW_MICRO_POINT_CLOUD
		.clutter_rm_method = POINT_CLUTTER_REMOVAL_NONE,
#else
		.clutter_rm_method = POINT_CLUTTER_REMOVAL_ALL,
#endif
		.clutter_halt_en = MMW_HALT_CLUTTER_UPDATE_DISABLE,
		.clutter_cover_time = 1,
		.recep_attenua_config = MMW_RX_ANA_GAIN_51DB,
		.trans_attenua_config = MMW_TX_RF_ATT_0DB,
		.hpf_bandwith_config = HPF_BW_0P5M
	},
	
	.mmw_motion_point_cloud_config = 
	{
		.motion_point_cloud_max_cfar_num = MMW_MAX_MOTION_CFAR_NUM,
		.peak_grouping_method = PEAKGROUPING_DISABLE,
		.static_rm = MMW_ENABLE,
		.threshold_snr_pwr_lin = 15.8489f,			    /* default 12dB, 12dB = 10log10(15.8489) */
		.psic_2nd_pass_thres = 3
	},
	
#if CONFIG_BOARD_MRS6130_P1806
	.mount_type = MMW_MOUNT_HORIZONTAL,			/* Default is Horizontal Mount for radar coordinate -- develop board is vertical */
#else
	.mount_type = MMW_MOUNT_VERTICAL,			/* Default is Vertical Mount for radar coordinate -- develop board is vertical */
#endif
#if SW_CFAR_ENABLE
	.auto_gain_flag = MMW_DISABLE
#else
	.auto_gain_flag = MMW_ENABLE
#endif
};

#if CONFIG_SOC_RS6240
/* Default rx antennas calibrate data, application will try to read calibration data when call mmw_ctrl_open(),
   if no calibration data is stored in flash, it will use this default calibration value for RS6240
*/
Complexf32_RealImag g_rx_ant_calib_data[] = {
	    {0.656616211f, 0.f},
	    {0.695953369f, -0.265777588f},
	    {-0.992157f, 0.233001709f},
	    {0.678497314f, -0.212799072f},
	    {0.656829834f, -0.000793457031f},
	    {0.704559326f, -0.257202148f},
	    {-0.979400635f, 0.22177124f},
	    {0.677612305f, -0.212738037f}    
	};

    /* length of g_azi_geometry and g_elev_geometry must handle all mount types */
    uint8_t g_azi_geometry[4] = {0, 1, 4, 5};
    uint8_t g_elev_geometry[4] = {3, 2, 1};
    MmwPointAntGeometry_t g_azi_geometry_struct = {
        .ptr_geometry = g_azi_geometry,
        .geometry_len = 4
    };
    MmwPointAntGeometry_t g_elev_geometry_struct = {
        .ptr_geometry = g_elev_geometry,
        .geometry_len = 3
    };
#elif CONFIG_SOC_RS7241
#if CONFIG_BOARD_MRS7241_P2840_CPUF
/* all rx antennas calibrate data */
Complexf32_RealImag g_rx_ant_calib_data[] = {
    {0.7144453f, 0.f},
    {1.0f, -0.2226512f},
    {0.7953076f, -0.2277154f},
    {0.5709669f, 0.1564293f},
    {-0.4964592f, -0.3787882f},
    {-0.8314539f, -0.3826051f},
    {-0.6511436f, -0.2781908f},
    {-0.2843711f, -0.4151297f}
};

    /* length of g_azi_geometry and g_elev_geometry must handle all mount types */
    uint8_t g_azi_geometry[8] = {4, 5, 6, 7, 0, 1, 2, 3};
    uint8_t g_elev_geometry[2] = {0};
    MmwPointAntGeometry_t g_azi_geometry_struct = {
        .ptr_geometry = g_azi_geometry,
        .geometry_len = 8
    };
    MmwPointAntGeometry_t g_elev_geometry_struct = {
        .ptr_geometry = g_elev_geometry,
        .geometry_len = 0
    };
#elif CONFIG_BOARD_MRS7241_P2828_CPUF
/* all rx antennas calibrate data */
Complexf32_RealImag g_rx_ant_calib_data[] = {
    {0.7537241f, 0.f},
    {0.5404806f, -0.2846407f},
    {0.7753062f, 0.0034884f},
    {0.5949060f, -0.1271592f},
    {-0.8966184f, -0.2503326f},
    {-0.7665839f, 0.1365850f},
    {-1.0f, -0.2860116f},
    {-0.7639511f, -0.0676270f}
};

    /* length of g_azi_geometry and g_elev_geometry must handle all mount types */
    uint8_t g_azi_geometry[6] = {4, 0, 6, 2, 7, 3};
    uint8_t g_elev_geometry[6] = {5, 0};
    MmwPointAntGeometry_t g_azi_geometry_struct = {
        .ptr_geometry = g_azi_geometry,
        .geometry_len = 6
    };
    MmwPointAntGeometry_t g_elev_geometry_struct = {
        .ptr_geometry = g_elev_geometry,
        .geometry_len = 2
    };
#else
#error "Please Set User-Defined Ant Geometry!"
#endif
#elif CONFIG_SOC_RS6130
    /* RS6130 has no calibration data, is load from flash in mmw_ctrl_open() function. */
	Complexf32_RealImag g_rx_ant_calib_data[] = {};
    uint8_t g_azi_geometry[2] = {2, 1};
    uint8_t g_elev_geometry[2] = {0, 1};
    MmwPointAntGeometry_t g_azi_geometry_struct = {
        .ptr_geometry = g_azi_geometry,
        .geometry_len = 2
    };
    MmwPointAntGeometry_t g_elev_geometry_struct = {
        .ptr_geometry = g_elev_geometry,
        .geometry_len = 2
    };
#else
    #error "Please Set User-Defined Ant Geometry!"
#endif

MmwPointCloudUserCfg_t *mmw_point_cloud_get_user_cfg(void)
{
	return &g_mmw_det_3d_user_cfg;
}

const MmwPointCloudUserCfg_t *mmw_point_cloud_get_user_cfg_const(void)
{
	return &g_mmw_det_3d_user_cfg;
}

#if (CONFIG_SOC_RS6240 || CONFIG_SOC_RS7241)
const Complexf32_RealImag *mmw_get_g_rx_ant_calib_data_const(void) {
	return &g_rx_ant_calib_data[0];
}
Complexf32_RealImag *mmw_get_g_rx_ant_calib_data(void) {
	return &g_rx_ant_calib_data[0];
}
#endif

MmwPointAntGeometry_t *mmw_get_g_azi_geometry_struct(void){
	return &g_azi_geometry_struct;
}

MmwPointAntGeometry_t *mmw_get_g_elev_geometry_struct(void){
	return &g_elev_geometry_struct;
}

const MmwPointAntGeometry_t *mmw_get_g_azi_geometry_struct_const(void){
	return &g_azi_geometry_struct;
}

const MmwPointAntGeometry_t *mmw_get_g_elev_geometry_struct_const(void){
	return &g_elev_geometry_struct;
}