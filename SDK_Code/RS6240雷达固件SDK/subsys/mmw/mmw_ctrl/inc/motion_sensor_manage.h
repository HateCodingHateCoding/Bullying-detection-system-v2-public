/*
 *  Copyright (c) 2022 Possumic <www.possumic.com>
 */


#ifndef MOTION_SENSOR_MANAGE_H
#define MOTION_SENSOR_MANAGE_H

#include <stdint.h>

typedef int16_t MotionSensorPower_t;

typedef struct {
    int16_t azimuth; /*! interage to dergee: azimuth*90/1024 */
    int16_t elevation; /*! interage to dergee: elevation*90/1024 */
    int16_t sinValue_Z; // sin value of elevation angle
    int16_t cosValue_Z; // cos value of elevation angle
    int16_t sinValue_X; // sin value of between verticla rxs phase difference
    int16_t sinValue_Y; // cos value of azimuth angle
} MotionSensorAngleInfo_t;

/**
 * @brief path id enumerates.
 *
 * Pleas use thes enumerates to access path for keeping up with SDK
*/
typedef enum {
	SENSOR_PATH_MOTION = 0,
	SENSOR_PATH_MICRO_MOTION,
	SENSOR_PATH_PRESENCE,
	SENSOR_PATH_MAX
} MotionSensorPathID_e;

/**
 * @brief Supported frame rate
*/
typedef enum {
	FRAME_RATE_0P5HZ = 0,
	FRAME_RATE_1HZ,
	FRAME_RATE_2HZ,
	FRAME_RATE_4HZ,
	FRAME_RATE_6HZ,
	FRAME_RATE_10HZ,
	FRAME_RATE_20HZ,
	FRAME_RATE_MAX
} MotionSensorFrameRate_e;

/**
 * @brief radar mouting position, refer to reference manual
*/
typedef enum {
	MOUNT_HORIZONTAL,
	MOUNT_VERTICAL
} MotionSensorMountType_e;

typedef enum{
	APPROACH_DET_TIMEOUT,
	APPROACH_DET_NO_TARGET,
	APPROACH_DET_TRIG,
	APPROACH_DET_RUNNING,
	APPROACH_DET_ERROR,
} MotionSensorApproachDetState_e;

typedef enum{
	MOTION_SENSOR_LEVEL_LOW = 0,
	MOTION_SENSOR_LEVEL_MID,
	MOTION_SENSOR_LEVEL_HIGH,
	MOTION_SENSOR_LEVEL_MAX	
} MotionSensorLevel_e;

typedef struct {
	/**
	 * max detection range in mm, some memory space will be allocated according to this parameter,
	 * this value will be saturated to max supported value during initialization process.
	 * max detection range effects the calculating time of algorithm and memory, so not too
	 * large max detection range is recommand.
	*/
	uint32_t max_det_range_mm;

	/** force triger range in mm, this value has an default range: [0, max_det_range_mm] */
	uint32_t force_trig_range_mm;

	/** default frame rate, see more in function
	 *  @ref motion_sensor_frame_ctrl_set_default_rate
	 */
	MotionSensorFrameRate_e frame_rate;

	/** mouting type, for more information, see reference manual and
	 *  @ref motion_sensor_meas_get_angle_from_angle_est_frame
	 */
	MotionSensorMountType_e mount_type;

	/** whether use all rx channel to run the detection, only 1 channel is
	 *  enabled when run motion detection if this is set to 1
	 */
	uint32_t use_all_rx_channel;

	/* use high performance motion det, so a target in futher range can be detected, but more power consumption */
	uint32_t use_highpf_motion_det;

	/*
	 * MOTION_SENSOR_LEVEL_HIGH means better snr, but more power consumption, 
	 * MOTION_SENSOR_LEVEL_LOW means worse snr, but less power consumption,
	 * MOTION_SENSOR_LEVEL_MID is treated as MOTION_SENSOR_LEVEL_HIGH here
	*/
	MotionSensorLevel_e det_ablility;

	uint32_t start_freq_mhz;
} MotionSensorConfig_t;

typedef enum {
	/** Motion detection frame */
	FRAME_TYPE_MOTION_DETECTION,

	/** angle estimation frame, which will cost a huge power */
	FRAME_TYPE_ANGLE_ESTIMATION_HIGH_PERFORMANCE,

	/** rise frame rate for trace application */
	FRAME_TYPE_APPROACH_DET,

	FRAME_TYPE_MAX
} MotionSensorFrameType_e;

typedef struct MotionSensorPoint3D {
	/* range of x in mm */
	int16_t x;

	/* range of y in mm */
	uint16_t y;

	/* range of z in mm */
	int16_t z;
} MotionSensorPoint3D_t;

typedef enum {
	MOTION_SENSOR_PAYLOAD_DATA_FORMAT_BYTE = 0,
	MOTION_SENSOR_PAYLOAD_DATA_FORMAT_SHORT
}MotionSensorPayloadDataFormat_e;

typedef enum {
	MOTION_SENSOR_PAYLOAD_DATA_UNSIGNED = 0,
	MOTION_SENSOR_PAYLOAD_DATA_SIGNED
}MotionSensorPayloadDataSign_e;

typedef struct{
	uint8_t q_format;
	uint16_t payload_data_dim;
	uint8_t align_mode;
	uint16_t data_len_1d;
	MotionSensorPayloadDataFormat_e payload_data_format;
	MotionSensorPayloadDataSign_e payload_data_sign;
}MotionSensorMsgPayloadHeader_t;

typedef struct{
	uint8_t adaption_thr_en;             // whether threshold adjustment is enable
	uint32_t adaption_thr_frame_num;     // number of frames adjusted to the threshold
	uint32_t adaption_alpha_coef;        // threshold adjustment parameter
	MotionSensorPower_t adaption_offset; // offset of threshold adjustment
} MotionSensorAdaptionThrPara_t;

/**
 * @brief Return the max detection range index
 *
 * The max range index user can access in application: [0, max_det_range_idx).
 * It is calculated from max_det_range_mm in @ref motion_sensor_init()
 *
 * @return max detection range configured.
*/
uint32_t motion_sensor_state_get_max_det_range_idx(void);

/**
 * @brief Return the force trigger range index
 *
 * This function will change force trigger range unit from mm to 'range bin index', and returns to user
 * force trig range idx is limited in [0, max_det_range_idx).
 * Around force trig range index, more accuracy g_ptr_snr and more calculate resource will be calculated, so forece trigger range is necessary for better performance
 *
 * @return force trigger range index
*/
uint32_t motion_sensor_state_get_force_trig_range_idx(void);

/**
 * @brief Get rx ant num used in motion sensor
 * @return rx num
*/
uint32_t motion_sensor_state_get_rx_num(void);

/**
 * @brief Get tx ant num used in motion sensor
 * @return tx num
*/
uint32_t motion_sensor_state_get_tx_num(void);


#endif

