/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef MMW_ANT_CALIBRATION_H
#define MMW_ANT_CALIBRATION_H

/* Include Files */
#include "mmw_type.h"
#include "mmw_alg_doa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Antenna calibration interface on RS624x/RS7241 to general calibration data of antennas.
 *
 */
void mmw_ant_calibration(uint16_t det_start, uint16_t det_end);

/**
 * @brief Save/Load antennas calibration data into/from flash to temporary variable.
 *
 */
int  mmw_ant_calib_data_save(bool reset);
int  mmw_ant_calib_data_load(void);

/**
 * @brief set/get antennas calibration data into/from temporary variable.
 *
 */
int  mmw_ant_calib_data_set(uint32_t *data, uint32_t len);
int  mmw_ant_calib_data_get(uint32_t *data, uint32_t len);

/**
 * @brief For Calibration Test and Debug.
 *
 */
void mmw_ant_angel_test(uint16_t range_idx, uint16_t det_start, uint16_t det_end,
                            int mimo_type, bool dbf, uint32_t test_type);
void mmw_show_angle(MmwAngleInfo_t *result, int range_idx, int dop_idx);

#ifdef __cplusplus
}
#endif


#endif /* MMW_ANT_CALIBRATION_H */
