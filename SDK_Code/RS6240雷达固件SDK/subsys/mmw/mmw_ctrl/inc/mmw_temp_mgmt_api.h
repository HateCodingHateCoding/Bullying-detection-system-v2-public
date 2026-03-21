#ifndef ZEPHYR_INCLUDE_DRIVERS_MMW_TEMP_MGMT_API_H_
#define ZEPHYR_INCLUDE_DRIVERS_MMW_TEMP_MGMT_API_H_

#include "soc.h"
#include "osi_timer.h"
#include "osi_semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*mmw_temp_triger_event_cb)(void *arg);
typedef int (*mmw_app_process_start)(void *arg);
typedef int (*mmw_app_process_stop)(void *arg);

typedef enum {
    TEMP_MGMT_DISABLE = 0,        /* Disable the temperature management module. */
    TEMP_MGMT_HOST_CTRL,          /* The host controls the temperature trigger calibration. */
    TEMP_MGMT_HOST_DETECTION,     /* host detection mode. */
} enTempMgmtMode;

typedef enum {
    TEMP_EVENT_NORM = 0,          /* The temperature is normal. */
    TEMP_EVENT_LOW,               /* The temperature is too low, and the temperature less than the last calibrated temperature minus temperature threshold. */
    TEMP_EVENT_HIGH               /* The temperature is too high, and the temperature exceeds the last calibration temperature plus temperature threshold. */
} enTempEventType;

typedef struct {
    uint32_t calib_mask;
    mmw_temp_triger_event_cb event_process;
    mmw_app_process_start app_start;
    mmw_app_process_stop  app_stop;
    uint8_t triger_calib_event;

    OSI_Semaphore_t    app_proc_sem;
} strTempMgmtInfo;

extern strTempMgmtInfo g_temp_mgmt_info;

#define MMW_TEMP_MGMT_DEBUG    0

/**
 * @brief Read the temperature value obtained by the tsensor.
 *
 * @note The temperature of the chip can be obtained, which can be used to monitor the working status of
 *       the chip or for other application scenarios.Can only trigger temperature acquisition during frame
 *       intervals or when the radar is not working.
 *
 * @return The measured temperature value.
 */
int16_t tsensor_read_temp(void);

/**
 * @brief get the temperature value during the last calibration.
 *
 * @note By comparing the temperature value from the previous calibration with the current temperature,
 *       if it exceeds the threshold, an event will be triggered.
 *
 * @return the temperature value during the last calibration.
 */
int16_t temp_mgmt_get_calib_temp(void);

/**
 * @brief save the temperature value during calibration.
 */
void temp_mgmt_set_calib_temp(int16_t calib_temp);

/**
 * @brief get the temperature value during the last calibration.
 */
int16_t temp_mgmt_get_cur_temp(void);

/**
 * @brief set the current temperature.
 */
void temp_mgmt_set_cur_temp(int16_t cur_temp);

/**
 * @brief set the threshold for updating the high and low temperature of the ITRIM.
 */
void temp_mgmt_set_itrim_temp_thr(int16_t temp_low_thr, int16_t temp_high_thr);

/**
 * @brief Set the mode of the temperature management module.
 *
 * @note Initialize the temperature management module before starting frame.
 *
 * @param temp_mgmt_mode  Temperature management mode.
 *
 * @return Non-zero parameter configuration failed, Zero on success.
*/
uint32_t temp_mgmt_mode_cfg(uint8_t temp_mgmt_mode);
uint8_t temp_mgmt_get_mode(void);

#define TEMP_MGMT_DEF_FRAME_NUM         100
#define TEMP_MGMT_DEF_DETECT_PERIOD     60   /* 60s */
/**
 * @brief Set the detection cycle of the temperature management module.
 *
 * @param frame_num_per_meas  Temperature measurement frame period.
 *
*/
void temp_mgmt_set_detect_period(uint32_t frame_num_per_meas);
uint32_t temp_mgmt_get_detect_period(void);

#define TEMP_MGMT_DEF_TEMP_THR          200    //unit:0.1°
/**
 * @brief Set the temperature trigger threshold.
 *
 * @note The current state is determined based on the last calibration temperature and the temperature trigger threshold.
 *
 * @param temp_thr Temperature trigger threshold.
 *
 * @return none
 */
void temp_mgmt_triger_thr_cfg(uint16_t temp_thr);

/**
 * @brief Get the temperature trigger threshold unit 0.1°.
 *
 * @return the temperature trigger threshold
 */
uint16_t temp_mgmt_get_triger_thr(void);

/**
 * @brief Set the callback function for temperature-triggered events.
 *
 * @note In the temperature detection and processing process, the current state is determined based on the current temperature and
 *       temperature threshold, and the callback function is called if the temperature is too high or too low.
 *
 * @param temp_triger_event_callback Temperature-triggered callback function.
 *
 * @return Non-zero parameter configuration failed, Zero on success.
 */
uint32_t temp_mgmt_triger_event_callback(mmw_temp_triger_event_cb temp_triger_event_callback);

#define TEMP_MGMT_LSI_CALIB_MASK        0x10000
#define TEMP_MGMT_MSI_CALIB_MASK        0x20000
#define TEMP_MGMT_CLK_CALIB_MASK        (TEMP_MGMT_LSI_CALIB_MASK | TEMP_MGMT_MSI_CALIB_MASK)
#define TEMP_MGMT_ALL_FM_CALIB_MASK     0x282      /* Full-function module calibration. */
#define TEMP_MGMT_MIN_FM_CALIB_MASK     0x280      /* The minimum function module calibration. */
/**
 * @brief Trigger the calibration of the temperature-related function module.
 *
 * @note Select the calibrated module according the application scenario, select the minimum subset caliration
 *       for low-power scenarios, and select full module calibration for normal mode.
 *
 * @param calib_mask Calibrated module mask.
 *
 * @return Non-zero parameter configuration failed, Zero on success.
 */
uint32_t temp_mgmt_triger_calib(uint32_t calib_mask);

typedef struct {
    int16_t calib_temp;     /* The temperature value at which the calibration last triggered. unit:0.1ø */
    int16_t cur_temp;       /* Current temperature value. unit:0.1ø */

    uint8_t event_status;   /* Event status: Normal, Temperature too high, temperature too low. */
} strTempMeasInfo;

/**
 * @brief Obtain measurement information from temperature management module.
 *
 * @note Generally, the event result is obtained in the event callback function.
 *
 * @param meas_info Temperature management module measurement information.
 *
 * @return none
 */
void temp_mgmt_get_temp_meas_info(strTempMeasInfo *meas_info);

/**
 * @brief Initialize the temperature management module.
 *
 * @note Configure the initialization parameters of the temperature management module.
 *
 * @param temp_mgmt_mode  Temperature management mode.
 * @param frame_num_per_meas  Temperature measurement frame period.
 * @param temp_thr  Temperature trigger threshold.
 * @param calib_mask Calibrated module mask.
 * @param event_callback Temperature-triggered callback function.
 *
 * @return none
 */
void mmw_temp_mgmt_init(uint8_t temp_mgmt_mode, uint32_t frame_num_per_meas, uint16_t temp_thr, uint32_t calib_mask,
                             mmw_temp_triger_event_cb event_callback);

/**
 * @brief Set the mode of the temperature management module.
 *
 * @note This interface is typically used by the user to configure the mode.
 *
 * @param temp_mgmt_mode  Temperature management mode.
 *
 * @return Non-zero parameter configuration failed, Zero on success.
 */
int mmw_temp_mgmt_mode_cfg(uint8_t temp_mgmt_mode);


void mmw_temp_mgmt_app_process_init(mmw_app_process_start app_start, mmw_app_process_stop app_stop);

#define INVALID_TEMP_VALUE      0xFFFF
/**
 * @brief Trigger the calibration process.
 *
 * @param cur_temp  Current temperature value.
 *
 * @return Non-zero parameter configuration failed, Zero on success.
 */
int temp_mgmt_calib_process(int16_t cur_temp);

static inline int temp_mgmt_app_process_start(void *arg)
{
    if (g_temp_mgmt_info.app_start == NULL) {
        return 0;
    }

    int ret = g_temp_mgmt_info.app_start(arg);

    OSI_SemaphoreRelease(&g_temp_mgmt_info.app_proc_sem);

    return ret;
}

static inline int temp_mgmt_app_process_stop(void *arg)
{
    if (g_temp_mgmt_info.app_stop == NULL) {
        return 0;
    }

    OSI_SemaphoreWait(&g_temp_mgmt_info.app_proc_sem, OSI_WAIT_FOREVER);

    return g_temp_mgmt_info.app_stop(arg);
}

extern OSI_Semaphore_t cb_mmw_event_sema;
static inline void temp_mgmt_set_triger_calib_event(bool triger_calib_event)
{
    g_temp_mgmt_info.triger_calib_event = triger_calib_event;

    if (triger_calib_event) {
        OSI_SemaphoreRelease(&cb_mmw_event_sema);
    }
}

static inline bool temp_mgmt_get_triger_calib_event(void)
{
    return g_temp_mgmt_info.triger_calib_event;
}

static inline void temp_mgmt_take_app_proc_event(void)
{
    OSI_SemaphoreWait(&g_temp_mgmt_info.app_proc_sem, OSI_WAIT_FOREVER);
}

static inline void temp_mgmt_give_app_proc_event(void)
{
    OSI_SemaphoreRelease(&g_temp_mgmt_info.app_proc_sem);
}
#ifdef __cplusplus
}
#endif

#endif
