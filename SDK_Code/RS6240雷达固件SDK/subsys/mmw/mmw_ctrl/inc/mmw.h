#ifndef ZEPHYR_INCLUDE_DRIVERS_MMW_H_
#define ZEPHYR_INCLUDE_DRIVERS_MMW_H_


#ifdef __cplusplus
extern "C" {
#endif


typedef int (*mmw_wake_check_cb_t)(void *param);

/**
 * @brief Configure frame status monitoring function switch.
 *
 * @note End of frame detection frame status.
 *
 * @param en  it can be turned on and off.
*/
void mmw_set_frame_state_monitor_sw(uint8_t en);

/**
 * @brief Set the maximum number of abnormal power ctrl states after the frame ends. If this number is exceeded, the system will be reset.
 *
 * @param max_num  Maximum number of abnormal power ctrl states.
*/
void mmw_set_pwr_state_err_max_num(uint16_t max_num);


void mmw_loop_task_monitor_init(void);
void mmw_loop_task_time_reset(void *arg);
void mmw_loop_task_time_stop(void);

void mmw_pm_wake_check_regist(mmw_wake_check_cb_t wake_check_cb);


#ifdef __cplusplus
}
#endif

#endif
