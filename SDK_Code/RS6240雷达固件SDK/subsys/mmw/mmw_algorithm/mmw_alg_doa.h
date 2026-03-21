/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef ANGLE_CALCULATE_H
#define ANGLE_CALCULATE_H

#include <stdint.h>

#include "mmw_alg_pointcloud_typedef.h"
#include "mmw_type.h"
#include "mdsp_driver.h"
#include "board_config.h"

#if (CONFIG_SOC_CPUS || CONFIG_BOARD_MRS6130_P1812 || CONFIG_BOARD_MRS6130_P1806 || CONFIG_BOARD_MRS6240_P2512_CPUS)
#define  MDSP_FFT_CALC_ENABLE                 1
#else //CPUF
#define  MDSP_FFT_CALC_ENABLE                 0
#endif

#define MMW_ANT_GEOMETRY_M62        (0)
#define MMW_ANT_GEOMETRY_A43        (1)
#define MMW_ANT_GEOMETRY_M43        (2)
#define MMW_ANT_GEOMETRY_M81        (3)
#define MMW_ANT_GEOMETRY_A81        (4)
#define MMW_ANT_GEOMETRY_A22        (4)

#if (CONFIG_BOARD_MRS6130_P1812 || CONFIG_BOARD_MRS6130_P1806)
#define MMW_PHY_RX_NUM              (3)
#define MMW_PHY_TX_NUM              (1)
#define MMW_ANT_GEOMETRY_TYPE       MMW_ANT_GEOMETRY_A22
#else
#define MMW_PHY_RX_NUM              (4)
#define MMW_PHY_TX_NUM              (2)
#if (CONFIG_BOARD_MRS6242_P2512_CPUF)
#define MMW_ANT_GEOMETRY_TYPE       MMW_ANT_GEOMETRY_A81
#else
#define MMW_ANT_GEOMETRY_TYPE       MMW_ANT_GEOMETRY_A43
#endif
#endif

#define MMW_MIMO_NUM                (MMW_PHY_RX_NUM * MMW_PHY_TX_NUM)
#define MMW_ANT_CHANNEL_NUM         (MMW_MIMO_NUM)
#define CALIB_TX_RX_NUM             (MMW_MIMO_NUM)


#define MMW_DBF_FFT_NUM_LOG2        4
#define MMW_DBF_FFT_NUM             (1<<MMW_DBF_FFT_NUM_LOG2)
#define MMW_DBF_MAX_PEAK            1
#define MMW_DBF_PEAK_SFDR           4  //10dB=10*log(n)

/*mdsp*/
#define ANGLE_VALUE_SHIFT          (7)
#define ANGLE_AZIM_INVALID         (90<<ANGLE_VALUE_SHIFT)
#define ANGLE_ELEV_INVALID         (90<<ANGLE_VALUE_SHIFT)
#define ANGLE_CORRECT_FACTOR       (2608)

#define MMW_ANGLE_MAX_VAL    (90<<7)  /* this would be invalid value of azimuth or elevation */


/**
 * @brief Estimate the angle by using 2DFFT range and doppler index.
 *
 * @param range_idx The index of the range bin.
 * @param dop_idx The index of the doppler bin.
 * @param angles Pointer of MmwAngleInfo Result.
 *
 * @return num of angle result, 0 means result is not valid.
 */
int mmw_angle_estim_by_index(uint16_t range_idx, uint16_t dop_idx, MmwAngleInfo_t *angles);

/**
 * @brief Estimate the angle by using complex data of antenas (8 data).
 *
 * @Note  Antenas data can be got by 'mmw_fft_ants' function.
 *
 * @param ant_data The Pointer of Antenas data.
 * @param angles Pointer of MmwAngleInfo Result.
 *
 * @return num of angle result, 0 means result is not valid.
 */
int mmw_angle_estim_by_data(complex16_cube *ant_data, MmwAngleInfo_t *angles);

/**
 * @brief Set mount type of RS6xxx chip.
 *
 * @param mount_type Type of mount, see enum MmwMountType_e, default MMW_MOUNT_VERTICAL.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_angle_mount_type_set(MmwMountType_e mount_type);

/**
 * @brief Get mount type of RS6xxx chip.
 *
 * @return current mount type.
 */
MmwMountType_e mmw_angle_mount_type_get(void);

/**
 * @brief Initial of the angle calculate module.
 *
 * @Note Call this function once before using mmw_angle_estim_by_index or mmw_angle_estim_by_data
 *       if do not using mmw_detection3d_init.
 *       Addition, mmw_angle_init will be called by mmw_detection3d_init.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_angle_init(void);

/**
 * @brief Before DoA, Apply Antenna calibration data to cube data, such as 1DFFT/2DFFT Data.
 *
 * @param ant_phase_data: all antenna input data for 2T4R.
 * @param ant_calibrate_data: all antenna calibrate data collected for 2T4R in 0 degree direction.
 * @param ant_output_data: all antenna output data for 2T4R after calibration.
 */
void mmw_cube_ant_align_2t4r(const complex16_cube *ant_phase_data,
                const complex16_mdsp *ant_calibrate_data, complex16_mdsp *ant_output_data);

uint32_t mmw_gain_adjust_S16Q15(int32_t real, int32_t image);
void mmw_angle_calculate(const uint32_t *azim_phase, const uint32_t *elev_phase, MmwAngleInfo_t *result);

/* Average Phase*/
void mmw_angle_phase_calc_m43(complex16_mdsp *ant_aligned, uint32_t *azim_phase, uint32_t *elev_phase);
void mmw_angle_elev_phase_m62(complex16_mdsp *ant_aligned, uint32_t *elev_phase);
void mmw_angle_phase_calc_m62(complex16_mdsp *ant_aligned, uint32_t *azim_phase, uint32_t *elev_phase);
void mmw_angle_azim_phase_a43(complex16_mdsp *ant_aligned, uint32_t *azim_phase);
void mmw_angle_elev_phase_a43(complex16_mdsp *ant_aligned, uint32_t *elev_phase);

int  mmw_angle_calc_a22(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result);
int  mmw_angle_calc_m42(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result);
int  mmw_angle_calc_m43(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result);
int  mmw_angle_calc_m62(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result);
int  mmw_angle_calc_a43(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result);

/* DBF */
void mmw_angle_bin_phase(complex16_mdsp *data_dbf, uint32_t peak_idx, complex16_mdsp *phase);
uint32_t mmw_angle_cfar(complex16_mdsp *fft_out, uint32_t noise_level, complex16_mdsp *phase, uint32_t angle_max);
uint32_t mmw_angle_fft(complex16_mdsp *ant_aligned, complex16_mdsp *dbf_fft_out,
                       const uint8_t *ant_geometry, uint32_t dbf_ant_num);

int  mmw_angle_dbf_m62_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max);
int  mmw_angle_dbf_m62(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max);

int  mmw_angle_dbf_m81_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max);
int  mmw_angle_dbf_m81(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max);

int  mmw_angle_dbf_a43_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max);
int  mmw_angle_dbf_a43(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max);

int  mmw_angle_dbf_a81_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max);
int  mmw_angle_dbf_a81(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max);


int mmw_angle_dbf(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max);

void * mmw_angle_calib_data_get(void);

#endif
