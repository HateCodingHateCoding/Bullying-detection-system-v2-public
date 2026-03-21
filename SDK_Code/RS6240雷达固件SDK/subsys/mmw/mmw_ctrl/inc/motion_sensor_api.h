/*
 *  Copyright (c) 2022 Possumic <www.possumic.com>
 */


#ifndef MOTION_SENSOR_API_H
#define MOTION_SENSOR_API_H

#include <stdint.h>
#include "motion_sensor_manage.h"



int mmwc_print_register(void * printcb);
int mmw_setup(void *sys_if, void *sys_cfg);
int mmw_close(void);
int mmw_sensor_start(void);


/**
 * @brief init motion sensor hardware.
 *
 * This function initialise all hardware and RF configureation needed by algorithm
 *
 * It must be called before @ref motion_sensor_init() function.
 *
 * @return FUNC_OK: hardware initialise success;
 *	       FUNC_ERR: hardware initialise failed
 */
uint32_t motion_sensor_hw_init(void);


/**
 * @brief init motion sensor.
 *
 * Trans detection range(mm) to range index according to fixed frame config.
 * Allocate memory used by this sdk module.
 * This function should be called befor all other APIs except @ref motion_sensor_hw_init() in this SDK.
 * If allocate memory failed, FUNC_ERR will be returned, else FUNC_OK will be returned.
 *
 * ATTENTION:
 * If @ref motion_sensor_deinit is called, this function must be called again to re-allocate memories.
 * If any configure in @ref ptr_default_config is changed, @ref motion_sensor_deinit() and @ref motion_sensor_init() must be called,
 * to re-aloocate memories.
 *
 * @param ptr_default_config: the user-input configration
 * @return FUNC_OK: allocate memory OK, initialize passed
 *	       FUNC_ERR: allocate memory failed,
 *
*/
uint32_t motion_sensor_init(const MotionSensorConfig_t *ptr_default_config);

/**
 * @brief Stop all frames, and release all memory allocated in @ref motion_sensor_init
 *
 * @return FUNC_OK: memory free OK.
 *	       FUNC_ERR: some memory free failed.
*/
uint32_t motion_sensor_deinit(void);

/**
 * @brief get configuration proposed by user
 *
 * If thif function is called before @ref motion_sensor_init() is called,
 * the default configuration is returned.
 * It is recommanded that use this function to get default configure,
 * and change the configure application want to change with default configure,
 * it is useful when SDK updates @ref MotionSensorConfig_t, that will not
 * affect pre-version application code.
 *
 * @return currently valid configuration structure
*/
MotionSensorConfig_t motion_sensor_state_get_config(void);

/**
 * @brief To minimise access to data cube, This function cache all motion detection datacube data into buffer build in SDK.
 *
 * Call this function once before you call any @ref motion_sensor_dsp_xxx_handler, they share the same datacube cache data.
 *
 * ATTENTION:
 * This function is valid only when @ref FRAME_TYPE_MOTION_DETECTION is triggered.
 * For other frames, this function has no effects but cost time.
*/
void motion_sensor_dsp_cache_data(void);

/**
 * @brief This function will reset output of specific range index on specific path.
 * When this function is called, it will take a short number of frames for the path
 * to be accuracy again, during which the g_ptr_snr of this range index on the path is suppressed
 * (or lower than not suppressed).
 *
 * Because motion path will usually trigger micro motion and presence path.
 * This  function is called when you find motion path triggered, and want to suppress other path
 * to make then not be triggered.
 *
 * Another condition of calling this function is that frame rate is changed by calling @ref motion_sensor_frame_ctrl_set_default_rate().
 * Sometimes not stable or false alarm may occures, call this function after call @ref motion_sensor_frame_ctrl_set_default_rate()
 * may decrease this condition.
 *
 * @param path_idx Path id which you want to suppress, if path_idx == APPROACH_DET_PATH_MAX, all path will be suppressed.
 * @param rbin_idx Range bin index on which you want to suppress.
 * @return FUNC_OK All parameters are valid.
 *	   FUNC_ERR Some of the paramters are not valid.
*/
uint32_t motion_sensor_dsp_suppress_snr(MotionSensorPathID_e path_idx, uint32_t rbin_idx);

/**
 * @brief This function will reset ALL output of specific range index on specific path.
 * This function acts similar to @ref motion_sensor_dsp_suppress_snr(), the different is that
 * this function will reset all memories in a path to default value and rerun initial process for path handler,
 * so it will take a longer frame for path to give an available result.
 *
 *
 * This function MUST be called under the following conditions:
 * 1. A motion frame is triggered on first time after frame is paused, e.g before calling @ref motion_sensor_frame_ctrl_start;
 * 2. A motion frame is triggered after another frame in @ref MotionSensorFrameType_e is triggered. e.g after calling @ref motion_sensor_frame_ctrl_change_frame_type
 * 3. Frame rate of motion frame changed. Frame change by calling @ref motion_sensor_frame_ctrl_set_default_rate() may use @ref motion_sensor_dsp_suppress_snr() function
 * to 'soft' reset the outputs, but when the new frame is triggered by @ref motion_sensor_frame_ctrl_change_frame_type() with debounce time, this function must be used
 * instead of @ref motion_sensor_dsp_suppress_snr().
 *
 * In conclusion, if the period between 2 contious detect frame has a frame period of few seconds, this function must be used, or @ref motion_sensor_dsp_suppress_snr()
 * can be used to reset path.
 *
 * @param path_idx path id which you want to reset, if path_idx == @ref APPROACH_DET_PATH_MAX, all path will be reset.
 * @return FUNC_OK All parameters are valid.
 *	   FUNC_ERR Some of the paramters are not valid.
*/
uint32_t motion_sensor_dsp_reset(MotionSensorPathID_e path_idx);

/**
 * @brief This function will reset output of specific range index on specific path.
 * This function acts similar to @ref motion_sensor_dsp_reset(), the different is that
 * this function will reset memories in specific range index to default value and rerun initial process for path handler,
 *
 * @param path_idx Path id which you want to reset, if path_idx == APPROACH_DET_PATH_MAX, all path will be reset.
 * @param rbin_idx Range bin index on which you want to reset.
 * @return FUNC_OK All parameters are valid.
 *	   FUNC_ERR Some of the paramters are not valid.
*/
uint32_t motion_sensor_dsp_reset_by_rbin_idx(MotionSensorPathID_e path_idx, uint32_t rbin_idx);

/**
 * @brief Motion path handler/Micro motion handler/Presence path handler
 *
 * All the following function will return the Signal-Noise Ratio(SNR) of the detected signal.
 * In motion data path, all 'quick' motion will be detected, e.g walking/runing of a human or other moving things
 * In micro motion data path, all 'quick' ande 'slow' motion will be detected, e.g playing cell phone on sofa, wave hands, shake legs...
 * In presence motion data path, all 'quick', 'slow' and 'presence' move will be detected, e.g breath of a human, wind blow of leaves and so on...
 *
 * The parameter inside these functions is binded to frame rate set by @ref motion_sensor_frame_ctrl_set_default_rate(), so a right frame rate must be configured
 * before acturally start a frame by @ref motion_sensor_frame_ctrl_start() function.
 *
 * Before calling these functions, make sure @ref motion_sensor_dsp_cache_data has been called in every calculation cycle.
 *
 * This function take a short number of frames according to different frame rate configure to prepare data, when the output is not ready, SNR = 0 will be returned.
 * Presence data path takes most frames, following micro motion data path and motion data path.
 *
 * In every data process is callbackd, all available rangebins must send in this function to update data path memory in SDK.
 * If a near bin is triggered by a target and break the loop, the further bins were not updated in application. This may cause unknown error if application want to
 * use these bins before calling @ref motion_sensor_dsp_reset().
 *
 * When @ref motion_sensor_dsp_init() is called, these data paths will be reset to initial value, a few frame should be used to reach a stable state.
 *
 * ATTENTION:
 * When frame rate is @ 1Hz, motion handler will not work, and always return 0.
 * When frame rate is @ 20Hz, presence handler wil work every 2 frame, so a SNR=0 is returned when presence handler is sleeping.
 * micro motion handler works at all frame rates.
 *
 * @param rbin_idx Range index on which motion detection will be deteced.
 * @return SNR of a detected target, if SNR=0, no target is detected on this range bin.
*/
MotionSensorPower_t motion_sensor_dsp_motion_handler(uint32_t rbin_idx);
MotionSensorPower_t motion_sensor_dsp_micro_motion_handler(uint32_t rbin_idx);
MotionSensorPower_t motion_sensor_dsp_presence_handler(uint32_t rbin_idx);

/**
 * @brief Pause motion detections and stop mmwave frames.
 *
 * This function is typically used in condition that the frame is controled by external signal.
 *
 * When you want to restart a frame and contiue detection, call @ref motion_sensor_frame_ctrl_start function
 *
 * @return FUNC_OK: frame pause successful
 *	   FUNC_ERR: frame pause failed
*/
uint32_t motion_sensor_frame_ctrl_pause(void);

/**
 * @brief Start motion detection and trigger a motion detect frames
 *
 * Frame rate is configured in @ref motion_sensor_frame_ctrl_set_default_rate() function.
*/
void motion_sensor_frame_ctrl_start(void);

/**
 * @brief configure frame rate used in motion sensor application
 *
 * The frame rate effects motion/micro-motion/presence handler, so right frame rate must be configured before calling
 * @ref motion_sensor_dsp_motion_handler function and so on.
 *
 * When this function is called, the next frame period will be recalculated according to the configure.
 *
 * ATTENTION:
 * This function can only be called in data_process callback thread, any call in other thread may cause crash of hardware
 *
 * @param new_frame_rate new frame used in application
 * @return FUNC_OK: frame_rate set is valid
 *	   FUNC_ERR: frame_rate set failed
*/
uint32_t motion_sensor_frame_ctrl_set_default_rate(MotionSensorFrameRate_e new_frame_rate);

/**
 * @brief Change frame type in next frame calculation cycle.
 *
 * This function should be carefully used, and actural motion detect frame period should be considered.
 * Actural frame period: the time between 2 motion detect frame is triggered;
 * Desired frame period: thr frame rate set in @ref motion_sensor_frame_set_default_rate() function convert to time.
 *
 * Due to the parameters of @ref motion_sensor_dsp_motion_handler(), for example micro motion and presence data path are the same,
 * is binded to Desired frame period, if the time between 2 @ref motion_sensor_dsp_motion_handler() calls varies from frame period,
 * or the environment changes a lot, the function may not work well.
 *
 * When calling this function, the main point is whether the following between 2 @ref motion_sensor_dsp_motion_handler() calls exists:
 * Case 1: probability of state from DETECTED transfer to UNDETECTED;
 * Case 2: environment has changed a lot.
 *
 * If one of the case may happen, then a call to @ref motion_sensor_dsp_reset() should be called, else a call to @ref motion_sensor_dsp_suppress_snr()
 * is recommanded to smoothly reach the stable condition, or no extra operation is sometimes OK due to application.
 *
 * This function is typically called under these conditions:
 * 1. User want to triger a new frame after @ref delay_ms has passed.
 *  e.g when a target is deteced, system want to sleep for @ref delay_ms and then trigger a new frame,
 * with this function, user can easily handle this without timer interface;
 * 2. User want to change the data type in next frame cycle; e.g when an angle information is needed, frame type
 * should trans from @ref FRAME_TYPE_MOTION_DETECTION to @ref FRAME_TYPE_ANGLE_ESTIMATION_HIGH_PERFORMANCE;
 *
 * @ref FRAME_TYPE_ANGLE_ESTIMATION_HIGH_PERFORMANCE has a huge current consumption, the usage of this frame should be limited
 * to get a better power performance.
 *
 * This function should only be called in user data process callback, if it is called outside, @ref motion_sensor_frame_ctrl_start() and
 * @ref motion_sensor_noise_estimation_update() will trigger default motion detect frame.
 *
 * ATTENTION:
 * This function can only be called in data_process callback thread, any call in other thread may cause crash of hardware
 *
 * @param e_frame_type new frame type which will be triggered in next calculation period.
 * @param delay_ms delay time before next frame is triggered from the start time of current frame. If delay_ms is 0, a new frame is triggered immediately after data process.
 * @return FUNC_OK: frame type configure successfully.
 *	   FUNC_ERR: frame type configure failed.
*/
uint32_t motion_sensor_frame_ctrl_change_frame_type(MotionSensorFrameType_e e_frame_type, uint32_t delay_ms);

/**
 * @brief Get height from pre-calculated angle information.
 *
 * This interface needs shorter time than @ref motion_sensor_meas_get_cartesian_coordinate, which may also calculate height.
 *
 * @param rbin_idx range bin on which the height will be calculated.
 * @param ptr_angle angle information calculated by @ref motion_sensor_meas_get_angle_from_angle_est_frame, with same rbin_idx.
 * @param ptr_height the height output in mm.
 * @return FUNC_OK All parameters are valid.
 *	   FUNC_ERR Some of the paramters are not valid.
*/
uint32_t motion_sensor_meas_get_height(uint32_t rbin_idx, const MotionSensorAngleInfo_t *ptr_angle, int16_t *ptr_height);

/**
 * @brief Calculate the cartesian coordinate defined in reference manual using pre-calcuated angle information
 * @param rbin_idx range bin on which the position will be calculated.
 * @param ptr_angle angle information calculated by @ref motion_sensor_meas_get_angle_from_angle_est_frame.
 * @param ptr_point position in mm
 * @return FUNC_OK All parameters are valid.
 *	   FUNC_ERR Some of the paramters are not valid.
 */
uint32_t motion_sensor_meas_get_cartesian_coordinate(uint32_t rbin_idx, const MotionSensorAngleInfo_t *ptr_angle, MotionSensorPoint3D_t *ptr_point);

/**
 * @brief Get range in mm from range bin index
 * @param rbin_idx range index which will convert to range in mm
 * @return range in mm
*/
uint32_t motion_sensor_meas_get_range_from_rbin_idx(uint32_t rbin_idx);

/**
 * @brief Calculate angle information in specific range bin index by the result of @ref FRAME_TYPE_ANGLE_ESTIMATION_HIGH_PERFORMANCE.
 *
 * This function MUST be called in @ref FRAME_TYPE_ANGLE_ESTIMATION_HIGH_PERFORMANCE frame,
 * which is configured in @ref motion_sensor_frame_ctrl_change_frame_type().
 *
 * Before calling this function, @ref mount_type in @ref MotionSensorConfig_t should be correctly set during initialization.
 *
 * @param rbin_idx range bin on which the angle information will be calculated.
 * @param ptr_angle_val output the angle information, which can be used for height and cartesian calculation.
 * @return g_ptr_snr of selected target, if the power of target is lower than noise, g_ptr_snr is set to 0.
 *
*/
MotionSensorPower_t motion_sensor_meas_get_angle_from_angle_est_frame(uint32_t rbin_idx, MotionSensorAngleInfo_t *ptr_angle_val);

/**
 * @biref Get coresponse range bin index from range in mm.
 *
 * @param range_mm actural range in mm.
 * @return range_index based on 0.
 *
*/
uint32_t motion_sensor_meas_get_rbin_from_range(uint32_t range_mm);


/**
 * @biref Reset all approach detection buffer
 *
 * This function must be called when a a brand new approach detection is rasied.
 * That means the last call of @ref motion_sensor_approach_det_handler returns all states except APPROACH_DET_RUNNING.
 *
*/
void motion_sensor_approach_det_reset(void);


/**
 * @biref Clear all flags set in @ref motion_sensor_approach_det_set_flag()
 *
 * This function must be called when a a new frame comes and this frame is used for approach detection.
 *
*/
void motion_sensor_approach_det_clear_all_flags(void);

/**
 * @biref Set approach detection flag in coresponding rbin_idx, when this rbin_idx is a peak and over the g_ptr_snr threshold.
 *        iterate all available range bins, and set the coresponding flag, then call @ref motion_sensor_approach_det_handler()
 *        to complete an approach detection in one frame.
 *
 * @param rbin_idx: range index which g_ptr_snr is over g_ptr_snr threshold and is a peak point.
 * @param snr_level: the g_ptr_snr level of the detection, must less than 4, fixed 3 is recommanded in most scenarios
 * @return FUNC_OK All parameters are valid.
 *         FUNC_ERR Some of the paramters are not valid.
*/
uint32_t motion_sensor_approach_det_set_flag(uint32_t rbin_idx, uint8_t snr_level);

/**
 * @biref get approach detection flag in coresponding rbin_idx set in @ref motion_sensor_approach_det_set_flag()
 *
 * @param rbin_idx: range index which g_ptr_snr is over g_ptr_snr threshold and is a peak point.
 * @return uint32_t the approach detection flag, if rbin_idx is invalid, 0 is returned.
*/
uint32_t motion_sensor_approach_det_get_flag(uint32_t rbin_idx);

/**
 * @biref run approach detection main function.
 *
 * Before calling this function, make sure function @ref motion_sensor_approach_de_set_flag is correctly called.
 * This function manage high efficent flags buffer, and run approach detection algorithm for each frame,
 * it will return @ref MotionSensorApproachDetState_e state, which can see in enumerates definition.
 *
 * @param approach_det_start_bin: start bin of approach detection algorithm run
 * @param approach_det_end_bin: end bin of approach detection algorithm run
 * @param ptr_trig_idx: pointer of triggered range idx
 *
 * @return  APPROACH_DET_TIMEOUT:       No trace is detected before approach detection runs for @ref time_out_no_trig ms
 *          APPROACH_DET_NO_TARGET:     No detection before approach deteciont runs for @ref time_out_no_det ms, time_out_no_det is reset after at least
 *                                      one detection is detected in approach detection region.
 *          APPROACH_DET_TRIG:          A detection is triggered.
 *          APPROACH_DET_RUNNING:       The algorithm needs more frames to give an approach detection result
 *          APPROACH_DET_ERROR:         Input paras is illegal
*/
MotionSensorApproachDetState_e motion_sensor_approach_det_handler(uint32_t approach_det_start_bin, uint32_t approach_det_end_bin, uint32_t *ptr_trig_idx);

/**
 * @biref Set delay time for approach detection
 *
 * @param no_detection_delay_ms: The maximun time before approach detection algorithm exit during which no g_ptr_snr in any range bin passed g_ptr_snr threshold
 * @param no_trigger_delay_ms: The maximun time before approach detection algorithm exit during which approach detection algorithm is not triggered
 *
*/
void motion_sensor_approach_det_set_delay_time(uint32_t no_detection_delay_ms, uint32_t no_trigger_delay_ms);


/**
 * @biref Set sensitivity for approach detection
 *
 * @param sensitivity: larger sensitivity makes approach detecion more sensitive. default value is 19
 *
*/
void motion_sensor_approach_det_set_sensitivity(uint32_t sensitivity);

/**
 * @brief Upload data to Motion Sensor Demo Tools.
 * This Function will automatic pack the package using user input paras.
 *
 * @param ptr_payload_header: infomation of data
 * @param msg_name: name of the data
 * @param ptr_data: data to be uploaded
*/
uint32_t motion_sensor_upload_visualize_data(const MotionSensorMsgPayloadHeader_t *ptr_payload_header, const char *msg_name, void *ptr_data);
uint32_t motion_sensor_upload_visualize_string(char *string);

/**
 * @brief Call this function at the end of all usart trans process to prevent CPU shunt done before last few bytes trans
*/
void motion_sensor_dbg_wait_for_usart_trans_done(void);

/**
 * @biref Set the SNR saturation threshold for a single detection signals at a rbin_idx in 0.5hz/1hz/2hz frame rate scene.
 *
 * @param rbin_idx: Range bin index on which you want to modify。
 * @param snr_thresholds: set SNR saturation threshold value.
 *
 * @return FUNC_OK All parameters are valid.
 *	       FUNC_ERR Some of the paramters are not valid.
*/
uint32_t motion_sensor_set_snr_saturation_threshold(uint32_t rbin_idx, MotionSensorPower_t snr_thresholds);

/**
 * @biref Get the SNR saturation threshold.
 *
 * @param rbin_idx: Range bin index on which you want to obtain。
 *
 * @return SNR saturation threshold, if threshold<0, The threshold is not initialized on this range bin.
*/
MotionSensorPower_t motion_sensor_get_snr_saturation_threshold(uint32_t rbin_idx);

/**
 * @biref Set the response coefficient for scenes with a frame rate of 0.5hz/1hz/2hz.
 *
 * @param resp_coef: response coefficient configuration range 0~7.
 *
 * @return FUNC_OK All parameters are valid.
 *	       FUNC_ERR paramters are not valid.
*/
uint32_t motion_sensor_set_response_coef(MotionSensorFrameRate_e frame_rate_current, uint8_t resp_coef);

/**
 * @brief Upload threshold of a rbin_idx on a chanel.
 *
 * @param thr: current threshold address
 * @param snr: current frames snr
 * @param thr_para: adaptive threshold adjustment parameters
 * @param path_idx: data processing channel index
*/
void motion_sensor_adaption_thr(MotionSensorPower_t *thr, MotionSensorPower_t snr, MotionSensorAdaptionThrPara_t *thr_para, uint8_t path_idx);

/**
 * @brief Check whether the threshold adjustment is complete.
 *
 * @param thr_para: adaptive threshold adjustment parameters
 * @param cnt: threshold adjusted frame count
 * @param path_idx: data processing channel index
 * @param thr: current threshold address
*/
void motion_sensor_count_process(MotionSensorAdaptionThrPara_t *thr_para, uint32_t *cnt, uint8_t path_idx, MotionSensorPower_t *thr);

#endif

