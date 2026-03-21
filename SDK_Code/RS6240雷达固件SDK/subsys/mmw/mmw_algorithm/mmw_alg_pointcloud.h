#ifndef __MMW_ALG_POINTCLOUD_H__
#define __MMW_ALG_POINTCLOUD_H__

#include "mmw_alg_pointcloud_typedef.h"
#include "mmw_app_pointcloud_config.h"

/* Angle fft lenth is power(2, MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2), use macro to select static configuration to reduce memory usage */
#define MMW_ALG_POINTCLOUD_AFFT_LEN_LOG2     (4)

uint8_t mmw_process_mem_alloc(void** ptr, uint32_t len);
void mmw_process_mem_free(void** ptr_ptr_mem);

/**
 * @brief simplely test range_idx is among valid region.
 * 		  Any point cloud is too near or close to range_fft_num will be discard.
 * 		  doppler_idx is used to filter out static point cloud
 * @param range_idx: range index got from cfar, based on 0,
 * @param doppler_idx: dopper index got from cfar, 0 means negetive max range rate, doppler_num/2 means static point cloud, dopper_num-1 means positive max range  rate
 * @param ptr_2d_fft_num: struct of range-fft number, and doppler-fft number, can be got via function 'mmw_fft_num_get()'
 * @param max_det_range_idx: max detection range index, points grater than 'max_det_range_idx' will be ignored by this function
 * @param min_det_range_idx: min detection range index, points less than 'min_det_range_idx' will be ignored by this function
 * @return 0: test Failed, 1: test success.
 */
extern uint8_t mmw_process_1st_pass_filter(uint16_t range_idx, uint16_t doppler_idx, const MmwPsic2DFFTNum_t *ptr_2d_fft_num, const MmwPointCloudUserCfg_t* ptr_user_cfg);

/**
 * @brief Cyclic check:
 *	 if idx >= len, idx = dix - len,  if idx < 0, idx = idx + len, if idx >= 0 & idx < len, idx keeps the same.
 *   It is usually used in get next or previous value in a circular queue.
 * @param idx: index under check.
 * @param len: max value of length.
 * @return circular value of idx.
 */ 
extern int16_t mmw_process_wrap_idx(int16_t idx, int16_t len);

/**
 * @brief Perform a angle calibration in float32 version.
 * 		  Each input(ant_input_data) is multiplied by the corresponding calibraation parameter(ant_calibrate_data):
 * 				  ant_output_data = ant_input_data .* ant_calibrate_data;
 * @param ant_input_data: input data;
 * @param ant_output_data: calibed output data, the memory should be valid, no memory check is done in this function.
 * @param ant_calibrate_data: calibed output data;
 * @param len: length of ant_input_data/ant_output_data/ant_calibrate_data.
 * */
extern void mmw_process_ant_data_calibrate_f32(const Complexf32_RealImag *ant_input_data, Complexf32_RealImag *ant_output_data, const Complexf32_RealImag *ant_calibrate_data, uint8_t len);

/**
 * @brief Read all enabled tx/rx ant data in datacube for a single (range_idx, dop_idx).
 *  	  If auto gain is enabled, restore the gain to get float data. The output in buffer_f32 is data_cube / 2^15.
 *        Before call this function, make sure mmw_motion_cube_access_open() is called.
 * @param buffer_f32: output buffer must be valid address, no address check is done in this function.
 * 					  The ouput format of this function is: [tx0_ant0, tx0_ant1, ... tx1_ant0, tx1_ant1, ...]
 * @param ptr_mimo_rx_nums: information of mimo rxs, this will got in mmw_psic_lib_sdk_get_tx_rx_num() function
 * @param dop_idx: doppler index on which will be read.
 * @param range_idx: range index on which will be read.
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int8_t mmw_process_fft_ant_f32(Complexf32_RealImag *buffer_f32, const MmwPsicMimoRxNum_t* ptr_mimo_rx_nums, uint16_t dop_idx, uint16_t range_idx);

/**
 * @brief Read all specific tx/rx ant data in datacube for all range bins, which have same dopper bin.
 *  	  If auto gain is enabled, restore the gain to get float data. The output in buffer_f32 is data_cube / 2^15.
 *        Before call this function, make sure mmw_motion_cube_access_open() is called.
 * @param buffer_f32: input buffer must be valid address, no address check is done in this function.
 * 					  The ouput format of this function is: [range_bin_0, range_bin_1, ...]
 * @param ptr_buffer_swap_q15: swap buffer for swap in this function, the length of this buffer should be read_num, 
 * 					  if set to 0, mem alloc will be called in this function, and will cost 4x times.
 * @param read_num: range_bin_0 ~ range_bin_$read_num$ will be read out.
 * 					If read_number > range_fft_num, some unwanted data will still continue to read out, until have read $read_num$ data.
 * @param tx_id: specific tx_index to be read, 0 means tx0, 1 means tx1
 * @param rx_id: specific rx_index to be read, 0 means rx0, 1 means rx1, ...
 * @param dop_idx: doppler index on which will be read.
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int8_t mmw_process_fft_range_f32(Complexf32_RealImag *buffer_f32, complex16_cube* ptr_buffer_swap_q15, uint16_t read_num,
	               uint8_t tx_id, uint8_t rx_id, uint16_t dop_idx);

/**
 * @brief Init process lib static variables. Main set the angle fft length.
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 * */
uint8_t mmw_process_lib_init(void);

/**
 * @brief Run peak grouping algorithm to reduce 
 * @param range_idx: The range index of point under test.
 * @param doppler_idx: The doppler index of point under test.
 * 		  dopper index is got from cfar, 0 means negetive max range rate.
 *		  doppler_num/2 means static point cloud, dopper_num-1 means positive max range rate
 * @param ptr_mimo_rx_nums: information of mimo rxs, this will got in mmw_psic_lib_sdk_get_tx_rx_num() function
 * @param ptr_2d_fft_num: struct of range-fft number, and doppler-fft number, can be got via function 'mmw_fft_num_get()'
 * @param ptr_ant_data: the complex value of point(range_idx, doppler_idx) for all mimo ants.
 * @param pwr_ut: sum of all ants power (non-coherent), no averange should be used 
 * @param peak_grouping_method: peak grouping method:
 * 		Let x axis be range from 0 ~ (range_fft_len - 1), y axis be range from 0 ~ (doppler_fft_len - 1) in the following graph,
 * 		mark '*' indicates current peak under test, 
 * 		mark 'o' indicatec the neighbour will be tested in specificated peak group method,
 * 		mark 'x' indicates these neighbour will NOT be tested.
 * 		only power of mark '*' is grater than all power in 'o', the '*' can pass the peak group test.
 * 		
 * 		PEAKGROUPING_DISABLE:
 * 		            x x x
 * 		            x * x
 * 		            x x x
 * 		
 * 		PEAKGROUPING_DOPPLER:
 * 		            x o x
 * 		            x * x
 * 		            x o x
 * 		
 * 		PEAKGROUPING_DOPPLER_RANGE:
 * 		            x o x
 * 		            o * o
 * 		            x o x
 * 		
 * 		PEAKGROUPING_RECTANGLE:
 * 		            o o o
 * 		            o * o
 *            		o o o
 *
 *      If doppler_idx is (doppler_fft_len - 1), then the doppler neighbour is [(doppler_fft_len - 2), (doppler_fft_len - 1), 0],
 * 		If doppler_idx is (0), then the doppler neighbour is [(doppler_fft_len - 1), 0, 1],
 * 		If range_idx is (range_fft_len - 1), then the range neighbour is [(range_fft_len - 2), (range_fft_len - 1), (range_fft_len - 1)],
 * 		If range_idx is (0), then the range neighbour is [0, 0, 1].
 * @return 0: the point under test NOT a peak, 1: the point under test IS a peak.
 * */
uint8_t mmw_process_peak_grouping(uint16_t range_idx, uint16_t doppler_idx, const MmwPsicMimoRxNum_t *ptr_mimo_rx_nums, 
        const MmwPsic2DFFTNum_t *ptr_2d_fft_num, float pwr_ut, PeakGroupingMethod_e peak_group_method);
				
/**
 * @brief Run 2nd pass filter.
 * 		  Calculate the average non-coherent power. 
 * 		  Compare the average power with threshold.
 * @param ptr_ant_data: the complex value of point(range_idx, doppler_idx) for all mimo ants.
 * @param ptr_mimo_rx_nums: information of mimo rxs, this will got in mmw_psic_lib_sdk_get_tx_rx_num() function
 * @param noise_pwr: the power of noise, get from the function "mmw_psic_mt_noise_get_f32".
 * @param threshold_pwr: threshold_pwr*noise is the true power threshold, it is seted 16 usually, which is increased by 12dB.
 * @param ptr_pwr: peak grouping can calculate power of point under test, set non-zero to send out power of point under test.
 * @return 0: test Failed, 1: test success.
 */
uint8_t mmw_process_2nd_pass_filter(const Complexf32_RealImag *ptr_ant_data, const MmwPsicMimoRxNum_t* ptr_mimo_rx_nums, float noise_pwr, float threshold_pwr, float* ptr_pwr);

/**
 * @brief Run conventional angle fft, user can choose their own ants by set param 'ptr_ant_geometry'.
 *        The output of angle fft is sin($\theta$), where sin($\theta)$ = 2 * angle_fft_idx / angle_fft_len for 0.5\lambda space anttena array.
 * 		  The peak index is interpolated by parabolic_interpolation method.
 * 		  pwr will also be calculated, one can use it to update high-precision snr.
 * @param ant_aligned: ant data that has been calibed with phase and power;
 * @param ptr_geometry_azi: indicate the number of ant which will be selected to calculate azimuth doa.
 * 		  e.g ptr_geometry_azi->ptr_ant_geometry = [0, 2, 3] and ptr_geometry_azi->len = 3, ant_aligned[0], ant_aligned[2], ant_aligned[3] will be used as angle fft input, 
 * @param ptr_geometry_elev: indicate the number of ant which will be selected to calculate elevation doa.
 * 		  e.g ptr_geometry_azi->ptr_ant_geometry = [0, 2, 3] and ptr_geometry_azi->len = 3, ant_aligned[0], ant_aligned[2], ant_aligned[3] will be used as angle fft input, 
 * @param ptr_sin_azi: sin(azimuth_doa) ouput in rad, which is calculated by peak index in angle fft. This para should be a valid address
 * @param ptr_sin_elev: sin(elevation_doa) ouput in rad, which is calculated by peak index in angle fft. This para should be a valid address
 * @param ptr_pwr_azi: power of peak of azimuth angle fft. If set to 0, no power will be sent out.
 * @param ptr_pwr_elev: power of peak of elevation angle fft. If set to 0, no power will be sent out.
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero(MMW_ERR_CODE_SUCCESS) on success. 
 * 			Typical error: MMW_ERR_CODE_INVALID_RESULT: sin(azi)>=1, MMW_ERR_CODE_NO_MEMORY: no memory for angle fft
 * */			  
uint8_t mmw_process_angle_process(const Complexf32_RealImag *ant_aligned, const MmwPointAntGeometry_t* ptr_geometry_azi, const MmwPointAntGeometry_t* ptr_geometry_elev, 
								float *ptr_sin_azi, float *ptr_sin_elev, float *ptr_pwr_azi, float *ptr_pwr_elev);

/**
 * @brief Give position in datacube, this function will calculate power of this point by non-coherent method
 * @param range_idx: range_idx based on 0;
 * @param doppler_idx: dopper_idx based on 0, 0 is the max negetive range rate.
 * @param ptr_mimo_rx_nums: information of mimo rxs, this will got in mmw_psic_lib_sdk_get_tx_rx_num() function
 * @return power of specific point.
 * */
float mmw_process_get_power(uint16_t range_idx, uint16_t doppler_idx,  const MmwPsicMimoRxNum_t *ptr_mimo_rx_nums);


#endif/*MMW_POINT_CLOUD_GENERAL_PROCESS_H*/