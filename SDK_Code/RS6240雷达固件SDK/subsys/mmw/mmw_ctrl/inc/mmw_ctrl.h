/**
 *   @file  mmw_ctrl.h
 *
 *   @brief
 *      Header file of the Millimeter Wave Controller.
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2022 Possumic, Inc.
 *
 */
#ifndef MMW_CTRL_H
#define MMW_CTRL_H

/* Include LL Files */
#include "types.h"

/* Include Files */
#include <stdbool.h>
#include "mmw_type.h"
#include "mmw_ctrl_os_api.h"
#include "mmw_math.h"
#include "mmw_mdsp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_MMW_CHRIP_CALIB_COMP_EN	(1)

/************************ Internal declaration **********************/
int mmw_sensor_stop(void);
int mmw_sensor_reset(void);

/**************************************************************************
 ************************ MMW Interfaces Definitions **********************
 **************************************************************************/

 /**
 * @brief MMWave error code definition.
 *
 * Zero on success or error code otherwise.
 */
#define MMW_ERR_CODE_SUCCESS			0
#define MMW_ERR_CODE_UNSUPPORT			1
#define MMW_ERR_CODE_INVALID_PARAM		2
#define MMW_ERR_CODE_NOT_READY			3
#define MMW_ERR_CODE_RUNNING			4
#define MMW_ERR_CODE_TIMEOUT			5
#define MMW_ERR_CODE_NO_MEMORY			6
#define MMW_ERR_CODE_EIO				7
#define MMW_ERR_CODE_INVALID_RESULT     8

#define MMW_ERR_SUBCODE_ID				256
#define MMW_ERR_SUBCODE_CHECKSUM		(MMW_ERR_SUBCODE_ID)
#define MMW_ERR_SUBCODE_VERSION			(MMW_ERR_SUBCODE_ID + 1)
#define MMW_ERR_SUBCODE_LOAD			(MMW_ERR_SUBCODE_ID + 2)
#define MMW_ERR_SUBCODE_SAVE			(MMW_ERR_SUBCODE_ID + 3)
#define MMW_ERR_SUBCODE_RESTORE			(MMW_ERR_SUBCODE_ID + 4)
#define MMW_ERR_SUBCODE_COMP			(MMW_ERR_SUBCODE_ID + 5)

/* config vail raw data size */
#define MMW_CHIRP_CALIB_NUM				(257)
#define MMW_CHIRP_CALIB_SIZE			(MMW_CHIRP_CALIB_NUM << 1)

#if (CONFIG_MMW_CHRIP_CALIB_COMP_EN)
/* (163) = RUND(13 + 13 + 5 * (257 - 2), 8) = 26 - 10 + 4 + 5*NUM */
#define MMW_CHIRP_CALIB_COMP_SIZE		((20 + MMW_CHIRP_CALIB_NUM*5) >> 3)
#else
#define MMW_CHIRP_CALIB_COMP_SIZE MMW_CHIRP_CALIB_SIZE
#endif
/* config max rx ant calib data */
#define MMW_ANGLE_CALIB_RX_ANT			(4)

/* config storage data formate size */
#define MMW_STORAGE_MAGIC_SIZE			(2)
#define MMW_STORAGE_VERSION_SIZE		(2)
#define MMW_STORAGE_CHECKSUM_SIZE		(2)

/*config totoal storage size with format and raw data*/
#define MMW_STORAGE_CHIRP_CALIB_FORMAT_SIZE (MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE + MMW_STORAGE_CHECKSUM_SIZE)
#define MMW_STORAGE_CHIRP_CALIB_SIZE	(MMW_CHIRP_CALIB_COMP_SIZE + MMW_STORAGE_CHIRP_CALIB_FORMAT_SIZE)

/* config storage format macro for data check */
#define MMW_STORAGE_CHIRP_CALIB_MAGIC	 (0x5AA5)
#define MMW_STORAGE_CHIRP_CALIB_VERSION  (0x03)

#define CLR_MSK(word, msk)                       ((word) &= (~((uint32_t)(msk))))
#define CLR_BIT(word, num)                       ((word) &= (~(uint32_t)(1U << (num))))
#define TEST_BIT(word, bit)                      ((word) & (1 << (bit)))
#define SET_BIT(word, num)                       ((word) |= (uint32_t)(1U << (num)))

void dbg_uart_enable(uint8_t enable);
void set_dbg_uart_port(uint8_t uart_port, uint8_t io_port);

/**
 * @brief Initialize MMWave Radar Controller.
 *
 * @note The lowest power consumption would be ref_msi True and pll_lc True, but
 *       the pll_lc only False for now.
 *       The initialization would take a few time about 1~2 seconds.
 *
 * @param ref_msi Reference clock selection between MSI and DCXO. Ture for MSI.
 * @param pll_lc Selection of pll clock source between DCXO and Self-osc. DCXO
 *               only for now which is False.
 * @param force_calib Enable force to calibrate the mmw RF.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_ctrl_open(bool ref_msi, bool pll_lc, bool force_calib);

/**
 * @brief Deinitialize MMWave Radar Controller.
 *
 * @note The parameters had set will lost after close. Please set again.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_ctrl_close(void);

/**
 * @brief Enable MMWave Radar Controller to start working.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_ctrl_start(void);

/**
 * @brief Disable MMWave Radar Controller to stop working.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
static inline int mmw_ctrl_stop(void)
{
	return mmw_sensor_stop();
}

/**
 * @brief Reset MMWave Radar Controller.
 *
 * Reset the parameters had set and the states in controller.
 *
 * @note The differece with mmw_ctrl_close and mmw_ctrl_open is the hardware
 *       power kept.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
static inline int mmw_ctrl_reset(void)
{
	return mmw_sensor_reset();
}

/**
 * @brief Get MMWave Radar Controller version infomation.
 *
 * @param version Pointer of 32bit version.
 *                | high <--       32bit       --> low |
 *                |   8bit   | 7bit  |  7bit   | 10bit |
 *                | For chip | Major | Project | Patch |
 * @param build Pointer of firmware building version.
 *
 * @return Pointer of MMWave version in string.
 */
char *mmw_version(uint32_t *version, uint32_t *build);

/**
 * @brief MMWave Radar Controller states.
 */
#define MMW_STATE_POWER_DOWN		0
#define MMW_STATE_ACTIVE			1
#define MMW_STATE_SLEEP				2
#define MMW_STATE_RUNNING			3

/**
 * @brief Get MMWave Radar Controller State.
 *
 * @param frame_sent Pointer of numbers of frames had sent.
 *
 * @return MMWave Radar Controller states (defined MMW_STATE_*).
 */
int mmw_state(uint32_t *frame_sent);

/**
 * @brief MMWave Radar Controller lowpower mode.
 */
#define MMW_LOW_POWER_MODE_DISABLE		0
#define MMW_LOW_POWER_MODE_AUTO			1

/**
 * @brief MMWave Radar Controller enable lowpower auto mode.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_ctrl_low_power(void);

/**
 * @brief MMWave Radar Controller disable lowpower mode and wakeup.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_ctrl_wake_up(void);

/**
 * @brief Get MMWave Radar Controller lowpower mode.
 *
 * @return MMWave Radar Controller lowpower mode (defined
 *         MMW_LOW_POWER_MODE_*).
 */
uint8_t mmw_low_power_get(void);

/**
 * @brief Get MMWave Radar Controller PM lock state for force mode.
 *
 * @return MMWave Zero on unlock, otherwise lock.
 */
uint32_t mmw_pm_lock_get(void);

/**
 * @brief MMWave Radar Controller MIMO mode.
 */
#define MMW_MIMO_1T1R			0
#define MMW_MIMO_1T3R			1
#define MMW_MIMO_2T3R			2
#define MMW_MIMO_1T4R			3
#define MMW_MIMO_2T4R			4
#define MMW_MIMO_1T2R			5
#define MMW_MIMO_LAST           6

/**
 * @brief MMWave Radar Controller Hardware Accelerator(HWA) mode.
 *
 * Hardware Accelerator for 1D/2D FFT with datacube of IF signal.
 *
 * @note mmw_fft_range can not be used to get 1dfft data if MMW_WORK_MODE_2DFFT
 *       is seleted.
 */
#define MMW_WORK_MODE_1DFFT		1
#define MMW_WORK_MODE_2DFFT		2

/**
 * @brief Configure the MIMO mode and HWA mode.
 *
 * @note 1. mmw_fft_range can not be used to get 1dfft data if
 *          MMW_WORK_MODE_2DFFT is seleted.
 *       2. Point cloud only support MMW_WORK_MODE_2DFFT for now.
 *       3. Point cloud only support Lambda/2 between RXs and virtual RXs, for
 *          example, MRS6230-E3325 point cloud mode is only supported with
 *          2T3R, but MRS6130-E1817 is supported with 1T3R and 2T3R.
 *
 * @param txrx_mode MMWave Radar Controller MIMO mode(defined MMW_MIMO_*).
 * @param work_mode MMWave Radar Controller HWA mode (defined MMW_WORK_MODE_*).
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_mode_cfg(uint8_t txrx_mode, uint8_t work_mode);

/**
 * @brief Get MIMO mode and HWA mode configurations .
 *
 * @param txrx_mode Pointer of MMWave Radar Controller MIMO mode(defined
 *                  MMW_MIMO_*).
 * @param work_mode Pointer of MMWave Radar Controller HWA mode(defined
 *                  MMW_WORK_MODE_*).
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_mode_get(uint8_t *txrx_mode, uint8_t *work_mode);

/**
 * @brief Set chirp start frequency and max frequency.
 *
 * Set the chirp frequency limited range. Chirp will start from the start
 * frequency but not end to the max frequency. The end frequency is auto set by
 * other settings. There's a sure that the end frequency never over than the
 * max frequency.
 *
 * @note The chirp end frequency is not the max frequency set in here, but it
 *       is never larger than the max frequency.
 *
 * @param start_MHz The chirp start frequency.
 * @param max_MHz The chirp max frequency.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_freq_cfg(uint32_t start_MHz, uint32_t max_MHz);

/**
 * @brief Get chirp start frequency and max frequency.
 *
 * @param start_MHz The chirp start frequency.
 * @param max_MHz The chirp max frequency.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_freq_get(uint32_t *start_MHz, uint32_t *max_MHz);

/**
 * @brief Set the range parameters about the max detected range and the range
 *        resolution.
 *
 * @note (range_mm / resol_mm) should not over than 1024.
 *
 * @param range_mm The max detected range in millimeter.
 * @param resol_mm The range resolution in millimeter.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_range_cfg (uint32_t range_mm, uint32_t resol_mm);

/**
 * @brief Get the range parameters about the max detected range and the range
 *        resolution.
 *
 * @param range_mm The pointer of max detected range in millimeter.
 * @param resol_mm The pointer of range resolution in millimeter.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_range_get (uint32_t *range_mm, uint32_t *resol_mm);

/**
 * @brief Set the velocity parameters about the max detected velocity and the
 *        velocity resolution.
 *
 * @note (2 * velocity_mm / veloc_resol) should not over than 1024.
 *
 * @param velocity_mm The max detected velocity in millimeter per second.
 * @param veloc_resol The velocity resolution in millimeter per second.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_velocity_cfg (uint32_t velocity_mm, uint32_t veloc_resol);

/**
 * @brief Get the velocity parameters about the max detected velocity and the
 *        velocity resolution.
 *
 * @param velocity_mm The pointer of max detected velocity in millimeter per
 *                    second.
 * @param veloc_resol The pointer of velocity resolution in millimeter per
 *                    second.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_velocity_get (uint32_t *velocity_mm, uint32_t *veloc_resol);

/**
 * @brief Set the frame sending period and number of frames to send.
 *
 * @param period_ms Frame sending period in millisecond.
 * @param frame_num Number of frames to send. 0 is a special number, it is as
 *                  same as Never Stop Sending.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_frame_cfg(uint32_t period_ms,  uint32_t frame_num);

/**
 * @brief Get the frame sending period and number of frames to send.
 *
 * @param period_ms The pointer of Frame sending period in millisecond.
 * @param frame_num The pointer of Number of frames to send. 0 is a special
 *                  number, it is as same as Never Stop Sending.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_frame_get(uint32_t *period_ms,  uint32_t *frame_num);

/**
 * @brief Set the interval period and number of intervals in a frame.
 *
 * @note Only support in MMW_WORK_MODE_1DFFT mode for now.
 *
 * @param period_us Interval sending period in microsecond in a frame.
 * @param interval_num Number of intervals in a frame.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_interval_cfg(uint32_t period_us,  uint32_t interval_num);

/**
 * @brief Get the interval period and number of intervals in a frame.
 *
 * @param period_us The pointer of interval sending period in microsecond in a
 *                  frame.
 * @param interval_num The pointer of number of intervals in a frame.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_interval_get(uint32_t *period_us,  uint32_t *interval_num);

/**
 * @brief Set the number of chirps in a interval.
 *
 * @param num Number of chirps sending in a interval.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_chirp_num_cfg(uint32_t num);

/**
 * @brief Can mmw be configured.
 *
 * @return Radar Operating status. return 4:The radar is operating. return 0: Configurable.
 */
int can_mmw_configured(void);

/**
 * @brief Types of rx smapling rate supported by radar.
 */
#define RX_SAMPLING_RATE_10M             1
#define RX_SAMPLING_RATE_20M             2

/**
 * @brief Set rx adc sampling rate.
 *
 * @param Data Rate, Samples per Second. 1: 10MHz, 2: 20MHz.
 *
 * @return none.
 */
void mmw_set_rx_sampling_rate(uint32_t sampling_rate);

/**
 * @brief Frame Pattern setting structure.
 *
 * Frame Pattern can config parameters quickly, instead of APIs such as mmw_mode_cfg,
 * mmw_freq_cfg, mmw_range_cfg, mmw_velociry_cfg.
 */
typedef struct {
    uint32_t start_freq_MHz;     /* MHz */
    uint32_t range_mm;           /* mm */
    uint32_t range_resol_mm;     /* mm */
    uint32_t velocity_mm;        /* mm/s */
    uint32_t velocity_resol_mm;  /* mm/s */
    uint8_t  mimo_mode;
    uint8_t  work_mode;
    uint8_t  acc_num;
    uint8_t  cfg_flag;
} FRAME_PATTERN;

/**
 * @brief Set parameters of the frame pattern specified by ID.
 *        This API is used to config different frame patterns, such as A/B frame pattern.
 *        For details of A/B frame pattern, generally, we call pattern 0 as frameA and pattern 1 as frameB.
 *        FrameA would be triggered at the start of every frame period, and frameB can be triggered after
 *        frameA by the API "mmw_frame_pattern_trigger" in the same frame period.
 *
 * @note In current version, these APIs with "*_frame_pattern_*" or "frameB" can only used
 *       by SingleCore projects.
 *
 * @param pattern_id ID of the frame pattern to be configured in MMW Controller.
 * @param pattern Pointer of frame pattern which obtains user's parameters.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_frame_pattern_cfg(uint32_t pattern_id, FRAME_PATTERN *pattern);

/**
 * @brief Get parameters of the frame pattern specified by ID.
 *
 * @param pattern_id ID of the frame pattern to get in MMW Controller.
 * @param pattern Pointer of frame pattern to store parameters.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_frame_pattern_get(uint32_t pattern_id, FRAME_PATTERN *pattern);

/**
 * @brief Delete the frame pattern specified by ID.
 *
 * @note Config0 can not be deleted.
 *
 * @param pattern_id ID of the frame pattern configured in MMW Controller.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_frame_pattern_del(uint32_t pattern_id);

/**
 * @brief Select the frame pattern specified by ID to be configured.
 *
 * @param pattern_id ID of the frame pattern to run in MMW Controller.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_frame_pattern_select(uint32_t pattern_id);

/**
 * @brief Get the ID of currently running frame pattern.
 *
 * @param pattern_id ID of the running frame pattern in MMW Controller.
 *
 * @return ID of working frame pattern.
 */
uint32_t mmw_frame_pattern_curid(void);

/**
 * @brief Trigger to run the frame pattern specified by ID, just as FrameB.
 *
 * @param pattern_id ID of the frame pattern to run in MMW Controller.
 *
 * @return ID of working frame pattern.
 */
int mmw_frame_pattern_trigger(uint32_t pattern_id);

/**
 * @brief Chirp setting structure.
 *
 * Chirp setting is auto calculated by other parameters such as the parameters
 * from mmw_range_cfg or other mmwave setting interfaces else.
 */
typedef struct {
	/**
	 * @brief Valid start frequency in MHz.
	 */
	uint32_t valid_start_MHz;

	/**
	 * @brief Valid sweep Bandwidth in MHz.
	 *
	 * The valid end frequency = valid_BW_MHz + valid_start_MHz.
	 */
	uint32_t valid_BW_MHz;

	/**
	 * @brief Sweep Bandwidth in MHz.
	 */
	uint32_t sweep_BW_MHz;

	/**
	 * @brief Chirp slope in MHz per microsecond.
	 */
	uint32_t slope_MHz_us;

	/**
	 * @brief Period of a chirp.
	 *
	 * Time of a chirp from chirp start to chirp stop.
	 *
	 */
	uint32_t period_ns;

	/**
	 * @brief ADC sample number.
	 */
	uint32_t sample_num;

	/**
	 * @brief Chirp number of a interval.
	 *
	 * Alway 1 for now.
	 */
	uint32_t chirp_num;
} CHIRP_INFO;

/**
 * @brief Get chirp setting which is generated by MMWave Radar Controller.
 *
 * @param chirp_info The pointer of chirp setting.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_chirp_get(CHIRP_INFO *chirp_info);

/**
 * @brief Get range FFT and doppler FFT sample number.
 *
 * @param range_fft_num The pointer of range FFT sample number.
 * @param doppler_fft_num The pointer of doppler FFT sample number.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_fft_num_get(uint16_t *range_fft_num, uint16_t *doppler_fft_num);

/**
 * @brief Get TxRx antenna and MIMO number.
 *
 * @param tx_ant_num The pointer of Tx antenna number, can be NULL.
 * @param doppler_fft_num The pointer of Tx antenna number, can be NULL.
 *
 * @return MIMO number, Zero is Invalid number.
 */
uint8_t mmw_mimo_num_get(uint8_t *tx_ant_num, uint8_t *rx_ant_num);

/**
 * @brief MMWave data type.
 */
#define MMW_DATA_TYPE_DISABLE				0
#define MMW_DATA_TYPE_1DFFT					2
#define MMW_DATA_TYPE_2DFFT					3
#define MMW_DATA_TYPE_3D_POINTS				6
#define MMW_DATA_TYPE_MOTION				10
#define MMW_DATA_TYPE_EVENT_ON_OFF			11
#define MMW_NOISE_EST_COMPLETE_EVENT		30
#define MMW_SLEEP_REQ_EVENT					31

/**
 * @typedef MMW_HANDLE
 * @brief Callback type for reporting mmwave data.
 *
 * A function of this type is given to the mmw_ctrl_callback_cfg() function
 * and will be called for mmwave data ready.
 *
 * @param mmw_data Not use for now.
 */
typedef int (*MMW_HANDLE)(void *mmw_data, void *arg);

/**
 * @brief Regist data callback provided mmwave data for data ready.
 *
 * A unique callback for a data type. The callbacks might be triggered in
 * different time as soon as the data is ready.
 *
 * @note Only one callback can be registed for now.
 *
 * @param handle Callback for the 'int_type' data type.
 * @param int_type MMWave data type (defined MMW_DATA_TYPE_*).
 * @param arg The pointer to argument of callback.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_ctrl_callback_cfg (MMW_HANDLE handle, uint8_t int_type, void *arg);

/**
 * @brief Get registed current handler of mmwave  data callback.
 *
 * @param int_type The pointer to get current MMWave data type (defined MMW_DATA_TYPE_*), can be NULL.
 * @param arg The pointer to get current argument of callback, can be NULL.
 *
 * @return current handler of Callback for the 'int_type' data type, NULL when no handle.
 */
MMW_HANDLE mmw_ctrl_callback_get (uint8_t *int_type, void **arg);

/**
 * @brief Get the registed callback data type.
 *
 * @return MMWave data type (defined MMW_DATA_TYPE_*).
 */
uint8_t mmw_ctrl_data_type_get(void);

/**
 * @brief Get the bitmask of data type for data ready.
 *
 * The bit of data type(defined MMW_DATA_TYPE_*) will be set if data is ready.
 *
 * @return Bitmask of data type.
 */
uint32_t mmw_data_get_status(void);

/**
 * @brief register mmw wake-up event handler.
 *
 * The function is used to register the processing method of the speified data type in the wake-up event.
 *
 * @param handle Callback for the int_type data type.
 * @param int_type MMWave data type (defined MMW_DATA_TYPE_*).
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_wakeup_event_callback(MMW_HANDLE handle, uint8_t int_type);

/**
 * @brief Antenna id for TX and RX Antenna.
 *
 * @note MMW_ANT_ID_ALL is get/handle all the TX or RX.
 */
#define MMW_ANT_ID_0						0
#define MMW_ANT_ID_1						1
#define MMW_ANT_ID_2						2
#define MMW_ANT_ID_ALL						0xff


/**
 * @brief Obtain the fft data.
 *
 * @param buffer Pointer of complex16_cube data.
 * @param read_num Number of complex16_cube to read. TXn_RXm has interval *
 *                 range_fft_num of complex16_cube from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param offset Offset number to read start from.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_fft_data(complex16_cube *buffer, uint32_t read_num,
	             uint8_t tx_id, uint8_t rx_id, uint32_t offset);

/**
 * @brief Obtain the specific fft data(bin) of specific Antennas.
 *
 * @param buffer Pointer of complex16_cube datas.
 *        It should be buffer[TXs][RXs] when using MMW_ANT_ID_ALL for tx_id or rx_id.
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*) for one Antennas,
 *        and MMW_ANT_ID_ALL for all TX Antennas.
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*) for one Antennas,
 *        and MMW_ANT_ID_ALL for all RX Antennas.
 * @param interval_idx The index of interval in a frame.
 * @param range_idx The range bin index, that's mean the number of range fft
 *                  sample.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_fft_single_bin(complex16_cube *buffer, uint8_t tx_id,
	             uint8_t rx_id, uint16_t interval_idx, uint16_t range_idx);

/**
 * @brief Obtain the specific fft data(bin) of All Antennas.
 *        This API is more efficient than 'mmw_fft_single_bin'.
 * @param buffer Pointer of complex16_cube datas, it should be buffer[TXs][RXs].
 * @param interval_idx The index of interval in a frame.
 * @param range_idx The range bin index, that's mean the number of range fft
 *                  sample.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_fft_ants(complex16_cube *buffer, uint16_t interval_id, uint16_t range_idx);

/**
 * @brief Obtain the specific fft data(bin) of All Antennas.
 *        This is high perfermance API instead of 'mmw_fft_ants'.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of complex16_cube datas, it should be buffer[TXs][RXs].
 * @param interval_idx The index of interval in a frame.
 * @param range_idx The range bin index, that's mean the number of range fft
 *                  sample.
 */
void  __mmw_fft_ants(complex16_cube *buffer, uint16_t interval_id, uint16_t range_idx);

/**
 * @brief Obtain the amplitude of specific fft data(bin) of All Antennas.
 *        This is high perfermance API to calculate amplitude of mmw_fft_ants.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of amplitude datas, it should be buffer[TXs][RXs].
 * @param interval_idx The index of interval in a frame.
 * @param range_idx The range bin index, that's mean the number of range fft
 *                  sample.
 */
void  __mmw_fft_abs_ants(uint16_t *buffer, uint16_t interval_id, uint16_t range_idx);

/**
 * @brief Obtain the Power(dB*Q8) of specific fft data(bin) of All Antennas.
 *        This is high perfermance API to calculate Power(dB*Q8) of mmw_fft_ants.
 *        Power(dB*Q8) = 20*log10(amplitude/2^15))*2^8.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of Power(dB) datas, it should be buffer[TXs][RXs].
 * @param interval_idx The index of interval in a frame.
 * @param range_idx The range bin index, that's mean the number of range fft
 *                  sample.
 */
void  __mmw_fft_logdB_ants(int16_t *buffer, uint16_t interval_id, uint16_t range_idx);

/**
 * @brief Obtain the (range) fft data of specific interval in a frame.
 *
 * @param buffer Pointer of complex16_cube data.
 * @param read_num Number of complex16_cube to read. TXn_RXm has
 *                 range_fft_num of complex16_cube from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param interval_idx The index of interval in a frame.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_fft_range(complex16_cube *buffer, uint16_t read_num,
	               uint8_t tx_id, uint8_t rx_id, uint16_t interval_idx);

/**
 * @brief Obtain the (range) fft data of specific interval in a frame.
 *        This is high perfermance API instead of 'mmw_fft_range'.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of complex16_cube data.
 * @param start_idx Start range index of complex16_cube data to read.
 * @param read_num Number of complex16_cube to read. TXn_RXm has
 *                 range_fft_num of complex16_cube from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param interval_idx The index of interval in a frame.
 */
void __mmw_fft_range(complex16_cube *buffer, uint16_t start_idx, uint16_t read_num,
	               uint8_t tx_id, uint8_t rx_id, uint16_t interval_idx);

/**
 * @brief Obtain the amplitude of (range) fft data of specific interval in a frame.
 *        This is high perfermance API to calculate amplitude of mmw_fft_range.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of amplitude data.
 * @param start_idx Start range index of amplitude data to read.
 * @param read_num Number of amplitude to read. TXn_RXm has
 *                 range_fft_num of amplitude from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param interval_idx The index of interval in a frame.
 */
void __mmw_fft_abs_range(uint16_t *buffer, uint16_t start_idx, uint16_t read_num,
	               uint8_t tx_id, uint8_t rx_id, uint16_t interval_idx);

/**
 * @brief Obtain the Power(dB*Q8) of (range) fft data of specific interval in a frame.
 *        This is high perfermance API to calculate Power(dB*Q8) of mmw_fft_range.
 *        Power(dB*Q8) = 20*log10(amplitude/2^15))*2^8.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of Power data.
 * @param start_idx Start range index of Power data to read.
 * @param read_num Number of Power to read. TXn_RXm has
 *                 range_fft_num of Power from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param interval_idx The index of interval in a frame.
 */
void __mmw_fft_logdB_range(int16_t *buffer, uint16_t start_idx, uint16_t read_num,
	               uint8_t tx_id, uint8_t rx_id, uint16_t interval_idx);

/**
 * @brief Obtain the doppler fft data of specific range index.
 *
 * @param buffer Pointer of complex16_cube data.
 * @param read_num Number of complex16_cube to read. TXn_RXm has
 *                 doppler_fft_num of complex16_cube from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param range_idx The index of the range bin.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_fft_doppler(complex16_cube *buffer, uint16_t read_num,
	                uint8_t tx_id, uint8_t rx_id, uint16_t range_idx);

/**
 * @brief Obtain the doppler fft data of specific range index.
 *        This is high perfermance API instead of 'mmw_fft_doppler'.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of complex16_cube data.
 * @param start_idx Start doppler index of complex16_cube data to read.
 * @param read_num Number of complex16_cube to read. TXn_RXm has
 *                 doppler_fft_num of complex16_cube from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param range_idx The index of the range bin.
 */
void __mmw_fft_doppler(complex16_cube *buffer, uint16_t start_idx, uint16_t read_num,
	                uint8_t tx_id, uint8_t rx_id, uint16_t range_idx);

/**
 * @brief Obtain the amplitude of doppler fft data with specific range index.
 *        This is high perfermance API to calculate amplitude of mmw_fft_doppler.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of amplitude data.
 * @param start_idx Start doppler index of amplitude data to read.
 * @param read_num Number of amplitude to read. TXn_RXm has
 *                 doppler_fft_num of amplitude from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param range_idx The index of the range bin.
 */
void __mmw_fft_abs_doppler(uint16_t *buffer, uint16_t start_idx, uint16_t read_num,
	                uint8_t tx_id, uint8_t rx_id, uint16_t range_idx);

/**
 * @brief Obtain the Power(dB*Q8) of doppler fft data with specific range index.
 *        This is high perfermance API to calculate Power(dB*Q8) of mmw_fft_doppler.
 *        Power(dB*Q8) = 20*log10(amplitude/2^15))*2^8.
 * @note  This function can only be used after mmw_motion_cube_access_open.
 *        And there is no error check of arguments.
 *
 * @param buffer Pointer of Power data.
 * @param start_idx Start doppler index of Power data to read.
 * @param read_num Number of Power to read. TXn_RXm has
 *                 doppler_fft_num of Power from mmw_fft_num_get().
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param rx_id Antenna id for RX Antenna (defined MMW_ANT_ID_*).
 * @param range_idx The index of the range bin.
 */
void __mmw_fft_logdB_doppler(int16_t *buffer, uint16_t start_idx, uint16_t read_num,
	                uint8_t tx_id, uint8_t rx_id, uint16_t range_idx);

/**
 * @brief Config parameter of obtain 1dfft data in 2dfft mode.
 * @note 1dfft data would save sucess in data_buff if
 *       return of mmw_data_get_status with bit MMW_DATA_TYPE_1DFFT in callback of MMW,
 *       otherwise failed to save data.
 *
 * @param range_start Start idx of range bin to be save (should not be last interval).
 * @param range_num Number of range bin to be saved from range_start.
 * @param start_intv Start idx of interval to be saved (should not be last interval).
 * @param end_intv End idx of interval to be saved (should not be last interval).
 * @param tx_id Tx antana idx specified of range fft data, (defined MMW_ANT_ID_*).
 * @param rx_id Rx antana idx specified of range fft data, (defined MMW_ANT_ID_*).
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_2dfft_obtain_1dfft_cfg(uint32_t range_start, uint32_t range_num,
	                uint32_t start_intv, uint32_t end_intv, uint8_t tx_id, uint8_t rx_id);

/**
 * @brief Enable and set buffer to obtain 1dfft data in 2dfft mode.
 * @note 1dfft data would save sucess in data_buff if
 *       return of mmw_data_get_status with bit MMW_DATA_TYPE_1DFFT in callback of MMW,
 *       otherwise failed to save data.
 *
 * @param data_buff Buffer to save 1dfft(range fft) data , alloced by user.
 *        if data_buff is not NULL, it means Enalbe obtain 1dfft data,
 *        if data_buff is NULL, it means Disable obtain 1dfft data.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_2dfft_set_1dfft_buffer(complex16_cube *data_buff);

/**
 * @brief Get status of obtain 1dfft data in 2dfft mode.
 * @note Call this function to check reason when failed to save
 *       1dfft data (MMW_DATA_TYPE_1DFFT bit not be set in mmw_data_get_status).
 *
 * @param data_len Pointer of data saved length, can be NULL if not need.
 * @param single_read Pointer of single read length, can be NULL if not need.
 *
 * @return bit0: 1 means 1dfft data is enabled, 0 means diabled.
 *         bit8: 1 means 1dfft param is valid, 0 menas parameter is invalid.
 */
uint32_t mmw_2dfft_obtain_1dfft_status(uint32_t *data_len, uint32_t *single_read);

/**
 * @brief Improve performance of Obtain fft data from data cube.
 *
 * @note It's more efficient to get data from cube by 'mmw_fft_*' after
 *       using 'mmw_motion_cube_access_open'. Attention, at the same time,
 *       mmw_motion_cube_access_close must be called after
 *       data cube reading is finished even if open failed.
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_motion_cube_access_open(void);

/**
 * @brief Improve performance of Obtain fft data from data cube.
 *
 * @note It's more efficient to get data from cube by 'mmw_fft_*' after
 *       using 'mmw_motion_cube_access_open'. Attention, at the same time,
 *       mmw_motion_cube_access_close must be called after
 *       data cube reading is finished even if open failed.
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_motion_cube_access_close(void);

/**
 * @brief Element structure of CFAR result.
 */
typedef struct {
    uint16_t range_idx;
    uint16_t dop_idx;
} CFAR_Result;

/**
 * @brief Element structure of CFAR side information.
 */
typedef struct {
	uint8_t  snr; /**< 1LSB = 1dB */
} CFAR_Info;

/**
 * @brief Obtain the CFAR result and side information of a 2DFFT Frame.
 *
 * @param res Pointer of CFAR_Result data buffer.
 * @param info Pointer of CFAR_Info data buffer.
 * @param cfar_num Number of CFAR_Result and CFAR_Info.
 * @param buf_num Number of res and info elements buffer.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_cfar_result(CFAR_Result *res, CFAR_Info *info,
	                uint32_t *cfar_num, uint32_t buf_num);


/**
 * @brief On or off to Clutter removal mode.
 */
#define MMW_CLUTTER_MODE_DISABLE      0
#define MMW_CLUTTER_MODE_ENABLE       1

/**
 * @brief Set enable/disable the clutter removal mode.
 *
 * The clutter removal mode is disabled by default. It should be enabled with
 * 3d point mode.
 *
 * @param clutter_mode Enable or disable of clutter removal mode
 *        (defined MMW_CLUTTER_MODE_*).
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_clutter_remove(uint8_t clutter_mode);

/**
 * @brief Get the state of clutter removal mode.
 *
 * @return Enabled or disabled of clutter removal mode(defined
 *         MMW_CLUTTER_MODE_*).
 */
int mmw_clutter_remove_get(void);

/**
 * @brief On or off to Halt clutter update.
 */
#define MMW_HALT_CLUTTER_UPDATE_DISABLE    		0
#define MMW_HALT_CLUTTER_UPDATE_ENABLE       	1
#define MMW_HALT_CLUTTER_UPDATE_AUTO	        2
/**
 * @brief Set clutter removal funtion NOT update clutter in hardware for all tx/rx .
 * 		  If tx-rx suppression function is used, must halt clutter update in hardware.
 *        Before call mmw_ctrl_start() function, this flag should be set to 0(default)
 *        It's recommanded to set halt to 1 in callback function if developer wants use clutter half function.
 * 
 * @param halt 1: Halt clutter update, 0: Clutter keeps update in every frame
 * 
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_clutter_halt_set(uint8_t halt);

/**
 * @brief Get clutter removal function state
 * 
 * @return 1: clutter update is halt, 0: clutter update is on.
 */
int mmw_clutter_halt_get(void);

/**
 * @brief Set the clutter remove factor, the samller the factor, the faster it will be. The configuration range is 0~15.
 *
 * @param clutter_factor clutter remove factor.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_clutter_factor_cfg(uint8_t clutter_factor);

/**
 * @brief Get the clutter remove factor.

 * @param clutter_factor The pointer of clutter remove factor.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_clutter_factor_get(uint8_t *clutter_factor);

/**
 * @brief Set the CFAR parameters.
 *
 * CFAR to find useful signal for point cloud detection. Set the threshold of
 * CFAR.
 *
 * @param static_remove Enable or Disable remove static clutter.
 * @param snr The delta SNR threshold. Only the delta SNR over than the
 *            threshold would be handled.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_cfar_cfg(bool static_remove, uint32_t snr);

/**
 * @brief Get the CFAR parameters.

 * @param static_remove The pointer of static_remove enable.
 * @param snr The pointer of delta SNR threshold.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_cfar_get(bool *static_remove, uint32_t *snr);

/**
 * @brief Set the CFAR Stationary Filter.
 *
 * CFAR Stationary Filter to filter out non-stationary points.
 *
 * @param enable Enable or Disable stationary filter.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_cfar_stationary(bool enable);

/**
 * @brief Set the CFAR Peaks Limitation.
 *
 * Set the max num of peaks in per range bin of CFAR.
 *
 * @param max_per_range Max peaks in per range bin.
 * @param inc_per_range Max peaks in per range bin when the peaks of
 *                      last range bin reached 'max_per_range'.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_cfar_dop_max_peak_set(uint8_t max_per_range, uint8_t inc_per_range);


/**
 * @brief Get the CFAR Peaks Limitation.
 *
 * Set the max num of peaks in per range bin of CFAR.
 *
 * @param max_per_range The pointer of max peaks in per range bin.
 * @param inc_per_range The pointer of max peaks in per range bin when the peaks of
 *                      last range bin reached 'max_per_range'.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_cfar_dop_max_peak_get(uint8_t *max_per_range, uint8_t *inc_per_range);

/**
 * @brief Enable/Disable auto gain of hardware FFT.
 * 		  This function must be called before mmw_ctrl_start() called.
 *        Once autogain is enabled, user must use mmw_psic_auto_gain_xxx functions to restore gain. Please refer to these functions for detail.
 *        If autogain is disabled, gain factor is always 0.
 *        Autogain enlarge dynamic range for application, but will cost extra compute time and memories.
 * @param enable Enable FFT auto gain.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_fft_autogain_set(uint8_t enable);

/**
 * @brief Get enable status of FFT auto gain.
 *
 * @return non-zero on Enable, zero on Disable.
 */
uint8_t mmw_fft_autogain_get(void);

/**
 * @brief Get hardware fft digital gain for range fft or doppler fft in Volt.
 *        Hardware may reduce standard fft gain to avoid overflow, if fft digital gain
 *        read from this function is N, then the real input data amplitude is fft peak value / N.
 *        If mmw_fft_autogain_set(1) is called, this function always returns 1.
 * 
 * @param fft_type 0: get fft gain for range fft, 1: get fft gain for doppler fft
 * 
 * @return fft digital gain.
 */ 
uint16_t mmw_fft_gain_get(uint8_t fft_type);

#define MMW_CAL_TYPE_RFFT   (0)
#define MMW_CAL_TYPE_DFFT   (1)
#define MMW_WIN_TYPE_RECT   (0)
#define MMW_WIN_TYPE_HANN   (1)

/**
 * @brief Set Range-FFT or Doppler-FFT window type.
 * Default is Hnning window for both Range-FFT and Doppler-FFT.
 * 
 * @param calType 0: Range-FFT, 1: Doppler-FFT, see MMW_CAL_TYPE_*
 * 
 * @param win_type 0: Rectangle window, 1: Hanning window, see MMW_WIN_TYPE_*
 * 
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_win_type_set(uint8_t calType, uint8_t win_type);

/**
 * @brief Get window type on Range-FFT or Doppler-FFT
 * 
 * @param calType 0: Range-FFT, 1: Doppler-FFT, see MMW_CAL_TYPE_*
 * 
 * @return window type of input, see MMW_WIN_TYPE_*
 */
uint8_t mmw_win_type_get(uint8_t calType);

#define MMW_RX_ANA_GAIN_27DB    		27
#define MMW_RX_ANA_GAIN_33DB     		33
#define MMW_RX_ANA_GAIN_39DB    		39
#define MMW_RX_ANA_GAIN_45DB      		45
#define MMW_RX_ANA_GAIN_51DB       		51
/**
 * @brief Configure Rx analog gain.

 * @param rx_ant_id antenna id.
 * @param gain configure gain value.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_rx_ana_gain_cfg(uint8_t rx_ant_id, uint8_t gain);

/**
 * @brief Adjusting the Rx digital gain

 * @param rx_ant_id antenna id.
 * @param gain_delta Gain adjustment amount.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int  mmw_rx_dig_gain_adj(uint8_t rx_ant_id, int8_t gain_delta);

/**
 * @brief Start output the single tone.
 *
 * Output the single tone of specific tx channel for RF performence analysis.
 * It can be used after mmw_open().
 *
 * @param tx_id Antenna id for TX Antenna (defined MMW_ANT_ID_*).
 * @param freq_MHz The frequence of single tone.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_single_tone_start(uint8_t tx_id, uint32_t freq_MHz);

/**
 * @brief Stop the single tone.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_single_tone_stop (void);


/**
 * @brief Type of upload data to the RadarAnanlysisTool.
 */
#define GUI_DATA_TYPE_CUBE		   	 	0
#define GUI_DATA_MMIC					2
#define GUI_DATA_TYPE_POINTS			4
#define GUI_DATA_TYPE_TRACKING          5
#define GUI_DATA_TYPE_POINTS_TRACKING   6
#define GUI_DATA_TYPE_DISABLE			0xff

/**
 * @brief Upload data to RadarAnalysisTool with another UART.
 *
 * @param data_type The type of data to upload (defined GUI_DATA_TYPE_*).
 * @param pkt_len Max length of a packet to upload. It should never larger than
 *                4095.
 * @param frame_num Number of frame to upload.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_report_gui(uint8_t data_type, int pkt_len, uint32_t frame_num);


/**
 * @brief Maximum Baudrate of UART to upload data to RadarAnalysisTool.
 */
#define REPORT_BAUDRATE_MAX				2000000
/**
 * @brief Minimum Baudrate of UART to upload data to RadarAnalysisTool.
 */
#define REPORT_BAUDRATE_MIN				9600

/**
 * @brief Set Baudrate of UART to upload data to RadarAnalysisTool.
 *
 * @param baudrate Baudrate of UART to upload data to RadarAnalysisTool.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_report_baudrate(uint32_t baudrate);

int mmw_os_regist(const struct mmw_radar_os_iface *p);

int mmw_resume(uint8_t level);

int mmw_suspend(void);

/**
 * @brief Start noise estimation with current mmw parameters set.
 *
 * @note 1. The Noise should be estimated again with Difference mmw parameters.
 *          The Noise is same with the same mmw parameters, more specificly,
 *          is same with the same chirp shape.
 *       2. The Noise is only one saved. Noise data will be overwrited if
 *          estimate again.
 *       3. The Noise is used to calculate SNR. So it must be used with 3d
 *          points mode and the SNR is needed.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_noise_estimation(void);

/**
 * @brief Get the noise estimation result in dB(Q4).
 *
 * @param buffer Buffer to obtain noise estimation result.
 * @param range_idx The range bin index of the noise estimation result start
 *                  from.
 * @param num The number of the noise estimation result to obtain
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */

int mmw_noisefloor_get(uint16_t *buffer, uint16_t range_idx, int num);

/**
 * @brief Enable or disable the debug uart of MMW.
 *
 * @param enable True for enable debug uart and False for disable.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */

int mmw_enable_dbg_uart(uint8_t enable);

/**
 * @brief Set debugging serial port ID and uart io mode.
 *
 * @param uart_port uart port id, configurable for 0/1/2 corresponding to uart 0/1/2.
 * @param io_port the io used by uart. uart0 0:pb0   uart1 0:pa8 1:pa0  uart2 0:pa14
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_set_dbg_uart_port(uint8_t uart_port, uint8_t io_port);

/**
 * @brief Enable or Disable calibration function in hardware. If disabled, no calibration process is applied for datacube,
 * 	      otherwise all rxs in datacube is multiplied by calibration data(complex) load from flash or set in mmwc_set_angle_calib_data()
 * 		  at cooresponding position. Tx0/Tx1 shares same Rx calibration data, for maximun length of calibration data is 4.
 *		  This function is enabled after POR, when using RS624x/RS724x it's recommanded to disable this function, because 
 * 		  RS624x/RS724x use software to calibration ants but hardware. 
 * 		  See @ref mmwc_set_angle_calib_data, @ref mmwc_save_angle_calib_data
 *
 * @param enable 0: Disable calibration function, 1: Enable calibration function.
 */
void mmw_ant_hw_calib_cfg(uint8_t enable);

/**
 * @brief set rx ant angle calib data into MMW. Once this function is called, angle calib in DataCube is auto set on.
 * 		  This function may called only in RS6130_SOC, because RS624X SOC use software calibration method. 
 
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmwc_set_angle_calib_data(complex16_cube *calib_data, uint8_t rx_num);

/**
 * @brief save rx ant angle calib data into chipinfo .
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmwc_save_angle_calib_data(uint8_t *calib_data, uint8_t len);

/**
 * @brief load chirp calibration data from stroage and set into MMW.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmwc_set_chirp_calib_data(void);

/**
 * @brief load chirp calibration data from MMW register and save into stroage.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmwc_save_chirp_calib_data(void);

/**
 * @brief save chirp calibration data into MMW register.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_chirp_calibdata_set(uint16_t *cal_data, int num);

/**
 * @brief load chirp calibration data from MMW register .
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_chirp_calibdata_get(uint16_t *cal_data, int num);

/**
 * @brief Restore chirp calibration data.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_chirp_calibdata_restore(uint32_t data_num, uint8_t *compress_data, uint32_t compress_data_len,
                                int16_t *raw_data, uint32_t raw_data_len);

/**
 * @brief Compressing chirp calibration data.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_chirp_calibdata_compress(uint32_t data_num, int16_t *raw_data, uint32_t raw_data_len,
                                 uint8_t *compress_data, uint32_t compress_data_len);

#define MMW_CALIB_SIGMADELTA     0x2
#define MMW_CALIB_AFC			 0x80
#define MMW_CALIB_CLUTTER_EST    0x100
/**
 * @brief Trigger mmw calibration.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_set_calib_sw(uint32_t mask, uint8_t flag);

/**
 * @brief set mmw power up start time in ns.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*),  Zero on success.
 */
int mmw_set_power_up_time(uint32_t power_up_time);

#define HPF_BW_0P5M     31
#define HPF_BW_1M       16
#define HPF_BW_2M       8
#define HPF_BW_4M       4
#define HPF_BW_8M       2
#define HPF_BW_16M      1
/**
 * @brief Configure analog channel HPF bandwidth.

 * @param rx_ant_id antenna id.
 * @param bw hpf bandwidth.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_ana_hpf_bw_cfg(uint8_t rx_ant_id, uint8_t bw);

/**
* @brief mmwave mini dsp turn off when system suspend.
*
* @return MMWave error code (defined MMW_ERR_CODE_*),  Zero on success.
*/
int mmw_dsp_poweron(void);

/**
 * @brief get abs of complx input and db of comple input
 *
 * @param cmplx_in for abs the input complx is U16Q16, output is U30Q30
 * when user input INT type of compx input, the output is U30Q14 (value of input^0.5)
 * @param cmplx_in for db the input complx is U16Q0 ,output is U8Q8
 * when user input INT type of compx input, the output is U8Q8 (value of (20log10(input/2^15))*2^8)
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_dsp_abs_log(complex16_cube cmplx_in, uint32_t *abs_out, uint32_t *db_out);

/**
 * @brief sqrt of root_val.
 *
 * @param *root_val is the root input(U32Q32) and output value (U32Q30).
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_dsp_sqrt(uint32_t *root_val);

enum {
		MMW_NOISE_EST_INIT_OPCODE = 0,
		MMW_NOISE_EST_DEINIT_OPCODE,
		MMW_NOISE_EST_LOWPWR_MODE_OPCODE,
};

enum {
		MMW_ANGLE_CALCULATE_OPCODE = 0x100
};

enum {
		MMW_ANGLE_CALIB_INIT_OPCODE = 0x200,
		MMW_ANGLE_CALIB_DEINIT_OPCODE
};

/**
 * @brief get chipinfo from efuse for packag board type.
 * @return packag board no info.
*/
uint8_t mmw_chip_info_get_pbd_no(void);

/**
 * @brief interaction between mmw firmare and zephyr application (os application)
 *
 * @param fd point to the data sets which to contron via cmd.
 * @param cmd is the opcode of ioctl.
 * @param fd point to the param arg of ioctl (buf set to firmare of copied from firmware)
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
*/
int mmw_ioctl(void *fd, uint16_t cmd, void *buf);

#define CLK_TYPE_LSI      0
#define CLK_TYPE_MSI      1
/**
 * @brief The clock frequency of the LSI/MSI module is passed to the firmware.
 *
 * @param clk_type Clock type(CLK_TYPE_LSI/CLK_TYPE_MSI).
 * @param freq     Clock frequency unit Hz.
*/
void mmw_clock_cfg(uint8_t clk_type, uint32_t freq);



/**
 * @brief Set dopfft on or off in 2DFFT mode.
 *
 * @param  true is on.flase is off
*/
int mmw_dop_fft_set(uint8_t on_off);

uint8_t mmw_dop_fft_get(void);

typedef struct {
	uint32_t	start_freq;
	uint32_t	max_freq;
	uint32_t	ramp_time;
	uint32_t	sample_time;
	CHIRP_INFO  cfg;
} SweepChirpInfo;

/**
 * @brief Get all the information about chirp.
 * @param chParam  A structure used to convey information.
*/
int mmw_sweep_chirp_info_get(SweepChirpInfo *chParam);

/**
 * @brief The firmware memory type.
 *
 * @param mem_type value: 0/0xfffe/0xfffc/0xffee/0xfff0/0xffcc/0xeeee/0xff00/0xcccc.
 * @param mem_param value: 0x01 | 0x02 | 0x04 | 0x08 | 0x10.
*/
int mmw_memory_type_set(uint32_t mem_type, uint32_t mem_param);

int mmw_set_data_cube_size(uint8_t sram_size, uint8_t zip_mode);

/**
 * @brief set mmic parameter
 * @param mmic_en Turn debug mode no or off
 * @param channel Select the antenna
 * @param data_src Reading mode
 * @param data_pos Storage location
 * @param chirp_num Number of chirp
*/
uint32_t mmw_set_mmic_para(uint32_t mmic_en, uint32_t channel, uint32_t data_src, uint32_t data_pos, uint32_t chirp_num);

/**
 * @brief set up the tx mapping
* @param txid 1:tx0, 0:tx1;
*/
void mmw_set_tx_ind(uint32_t txid);


/**
 * @brief Coordination for 3d point cloud.
 */
#define MMW_COORDINATE_TYPE_POLAR		0  /**< polar */
#define MMW_COORDINATE_TYPE_CART		1  /**< cartesian */

#ifndef BIT
#if defined(_ASMLANGUAGE)
#define BIT(n)  (1 << (n))
#else
/**
 * @brief Unsigned integer with bit position @p n set (signed in
 * assembly language).
 */
#define BIT(n)  (1UL << (n))
#endif
#endif

/**
 * @brief Get the coordinate type.
 *
 * @return Coordination type (defined MMW_COORDINATE_TYPE_*).
 */
uint8_t mmw_coordinate_get(void);

/**
 * @brief Select the coordinate for 3d point cloud.
 *
 * @param type Coordination for 3d point cloud (defined MMW_COORDINATE_TYPE_*).
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_coordinate_config(uint8_t type);

/**
 * @brief Get the size of point cloud in this frame.
 *
 * @return Size in byte of point cloud.
 */
int mmw_get_pointcloud_len(void);

/**
 * @brief Get the 3d point cloud data.
 *
 * @param result Vector of PointCloud3D.
 * @param buf_len Length in btye to read.
 *
 * @return MMWave error code (defined MMW_ERR_CODE_*), Zero on success.
 */
int mmw_pointcloud_3d(PointCloud3D *result, uint32_t buf_len);

#define MMW_TX_RF_ATT_0DB    		0
#define MMW_TX_RF_ATT_1DB     		1
#define MMW_TX_RF_ATT_2DB    		2
#define MMW_TX_RF_ATT_3DB      		3
#define MMW_TX_RF_ATT_4DB       	4
#define MMW_TX_RF_ATT_6DB       	6
#define MMW_TX_RF_ATT_8DB       	8
/**
 * @brief Attenuates the intensity of tx
 * @param num .The number of db`s that need to be reduced
*/
int mmw_tx_rf_att_set(int att_db);

/**
 * @brief Get the count of frame process since mmw_ctrl_start.
 *        It would be reset by mmw_ctrl_start.
 *
 * @return the count of frame event.
 */
uint32_t mmw_ctrl_frame_cnt_get(void);


#define HWINFO_STORAGE_TYPE_MSK                     0x8000
#define HWINFO_STORAGE_ID_MSK                       0x7FFF
#define HWINFO_STORAGE_FLASH                        0x8000
#define HWINFO_STORAGE_RAM                          0x0000

#define SYS_REBOOT_WARM 0
#define SYS_REBOOT_COLD 1


bool mmw_event_process_completed(void);

#ifdef __cplusplus
}
#endif


#endif /* MMW_CTRL_H */
