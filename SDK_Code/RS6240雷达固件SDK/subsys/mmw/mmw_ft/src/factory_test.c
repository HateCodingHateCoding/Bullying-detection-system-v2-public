/**
 **************************************************************************************************
 * @brief   factory test function define.
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************************************
 */

#include "factory_test.h"
#include "hif.h"
#include "mmw_type.h"
#include "ft_ctrl.h"
#include "mmw_app_pointcloud.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_point_cloud_psic_lib.h"
#include "mmw_report.h"
#include "hif_pm.h"


struct ft_proc_status_t ft_proc_status = {
	.ft_status = 0,			// 1 --> in factory test mode
	.ft_running = 0			// 1 --> ft function is running
};

struct mmw_noise_est_frame_t mradar_frame = {
#if CONFIG_SOC_SERIES_RS624X
	.trx_mimo = MMW_MIMO_2T4R,
#elif CONFIG_SOC_RS6130
	.trx_mimo = MMW_MIMO_1T3R,
#endif	
	.work_mode = MMW_WORK_MODE_2DFFT,
	.clutter_mode = MMW_CLUTTER_MODE_DISABLE,
	.startFreq_MHz = FT_DEFAULT_START_FREQ,
	.endFreq_MHz = 70000,
	.range_mm = FT_DEFAULT_RANGE,
	.resol_mm = FT_DEFAULT_RANGE_RES,
	.velocity_mm = NOISE_FT_DEFAULT_VELOC,
	.veloc_resol = NOISE_FT_DEFAULT_VEL_RES,
	.intvl_period_us = 83,
	.intvl_num = 0,
	.frame_period_ms = NOISE_FT_DEFAULT_FRAME_PERIOD,
	.frame_num = NOISE_FT_DEFAULT_FRAME_NUM + CONFIG_MMW_VENDOR_FILTER_FRAME_NUM,
};

struct mmw_angle_est_frame_t angle_frame = {
	.start_freq = FT_DEFAULT_START_FREQ,
	.mount_type = MMW_MOUNT_HORIZONTAL,
	.range_mm = FT_DEFAULT_RANGE,
	.range_res_mm = FT_DEFAULT_RANGE_RES,
};

uint8_t est_rx_msk = 0;
static int8_t ft_mmw_perf_status = HIF_ERRCODE_NOT_READY;

extern struct radar_noise_est_t mradar_est_data;
extern struct mmw_noise_est_final_report_t noise_est_report;

extern struct radar_angle_est_t angle_est_data;
extern struct mmw_angle_est_report_t angle_est_report;

OSI_Semaphore_t trigger_mmw_wave_sem;
HAL_Dev_t *ft_trigger_wkio_Dev = NULL;

extern void hif_device_suspend_req(void);

static TaskHandle_t    ft_mmw_perf_task_handle    = NULL;
void factory_test_mmw_perf_task(void *param)
{
	int ret;
	int hif_ret = HIF_CMD_STATUS_SUCCESS;
	ft_proc_status.ft_running = 1;
	
	/* get receive HIF msg */
	struct mmw_nosie_est_recive_param_t *nosie_est_recive_param = (struct mmw_nosie_est_recive_param_t *)((uint8_t*)param + 4);
	/* set noise estmation param */
	ft_noise_est_param_set(nosie_est_recive_param);
	/* start noise estimation */
	ret = mmw_noise_est_start(&mradar_frame, &mradar_est_data);
	if (ret == 0) {
		/* wait semaphore release */
		ret = mmw_noise_est_wait_done(&mradar_est_data);
		if (ret == 0) {
			/* get result of noise estimation */
			uint8_t *ack_data = HIF_Msg_AckDataPtr(param);
			memcpy(ack_data, &noise_est_report, sizeof(noise_est_report));
			if (noise_est_report.fail_msk || noise_est_report.invalid_msk) { /* noise est fail */
				hif_ret = HIF_CMD_STATUS_IO;
			}
			HIF_MsgResp(param, sizeof(noise_est_report), hif_ret);
			ft_noise_est_deinit(&mradar_est_data);
		} else { 		/* wait semaphore release timeout */
			hif_ret = HIF_CMD_STATUS_IO;
			HIF_MsgResp(param, 0, hif_ret);
		}
	} else {
		hif_ret = HIF_CMD_STATUS_IO;
		HIF_MsgResp(param, 0, hif_ret);
	}

	ft_proc_status.ft_running = 0;
	vTaskDelete(ft_mmw_perf_task_handle);
}

static TaskHandle_t    ft_mmw_angle_est_task_handle    = NULL;
void factory_test_mmw_angle_est_task(void *param)
{
	uint8_t angle_est_work_mode;
	int ret;
	int hif_ret = HIF_CMD_STATUS_SUCCESS;
	ft_proc_status.ft_running = 1;
	
	/* get receive HIF msg */
	struct mmw_angle_est_recive_param_t *angle_est_recive_param = (struct mmw_angle_est_recive_param_t *)((uint8_t*)param + 4);
	/* set angle estmation param */
	ft_angle_est_param_set(angle_est_recive_param);
	
	angle_est_work_mode = angle_est_recive_param->work_mode;
	/* start angle estimation */
	ret = mmw_angle_est_start(&angle_frame, angle_est_work_mode);
	if (ret == 0) {
		/* wait semaphore release */
		ret = mmw_angle_est_wait_done(&angle_est_data);
		if (ret == 0) {
			/* get result of angle estimation */
			uint8_t *ack_data = HIF_Msg_AckDataPtr(param);
			memcpy(ack_data, &angle_est_report, sizeof(angle_est_report));
			if (angle_est_report.fail_msk) {	/* angle est fail */
				hif_ret = HIF_CMD_STATUS_IO;
			}
			HIF_MsgResp(param, sizeof(angle_est_report), hif_ret);
		} else {    /* wait semaphore release timeout */
			hif_ret = HIF_CMD_STATUS_IO;
			HIF_MsgResp(param, 0, hif_ret);
		}
	} else {
		hif_ret = HIF_CMD_STATUS_IO;
		HIF_MsgResp(param, 0, hif_ret);
	}
	
	/* update calib flag, only calib sucess, usr can save calib data and iq data */
	if(hif_ret == HIF_CMD_STATUS_SUCCESS) {
		ft_mmw_perf_status = HIF_CMD_STATUS_SUCCESS;
	} else {
		ft_mmw_perf_status = HIF_ERRCODE_IO_ERROR;
	}
	
	ft_proc_status.ft_running = 0;
	vTaskDelete(ft_mmw_angle_est_task_handle);
}

static int factory_test_fun_handler(HIF_MsgHdr_t *msg)
{
	int ret = HIF_CMD_STATUS_SUCCESS;

	if (ft_proc_status.ft_status == 1) {
		
		if (ft_proc_status.ft_running == 0) {
			uint8_t msg_id = *((uint8_t *)(msg) + 1);
			switch (msg_id) {
				case HIF_MSG_ID_MMW_PERF_TEST:
					if(pdPASS != xTaskCreate(factory_test_mmw_perf_task, "ft_mmw_perf task", 512, msg, 1, &ft_mmw_perf_task_handle)){
						MMW_LOG_DBG("ft_mmw_perf task create failed\n");
					} else {
						MMW_LOG_DBG("ft_mmw_perf task create success\n");
					}
					break;
				case HIF_MSG_ID_MMW_CALIB_TEST:
					if(pdPASS != xTaskCreate(factory_test_mmw_angle_est_task, "ft_mmw_angle_est task", 512, msg, 1, &ft_mmw_angle_est_task_handle)){
						MMW_LOG_DBG("ft_mmw_calib task create failed\n");
					} else {
						MMW_LOG_DBG("ft_mmw_calib task create success\n");
					}
				break;
			}
		} else {
			printf("factory test is running\n");
			return -1;
		}
	}
	else {
		ret = HIF_CMD_STATUS_IO;
		printf("please enter factory test mode\n");
		HIF_MsgResp(msg, 0, ret);
	}
	return 0;
}

int factory_test_save_calib_handler(HIF_MsgHdr_t *msg){
	
	int ret = HIF_CMD_STATUS_SUCCESS;

	int param_len = msg->length;
	
	if (ft_proc_status.ft_status == true && ft_proc_status.ft_running == false) {
		if(param_len) {
			uint8_t *subid = (uint8_t *)(msg + 1);
			uint8_t *param = (uint8_t *)(subid + 1);
			ft_proc_status.ft_running = true;
			if(*subid == 0x01) {
#if CONFIG_SOC_SERIES_RS624X
				int calib_data_len = CONFIG_MAX_RX_ANT * 4;
#elif CONFIG_SOC_RS6130
				int calib_data_len = (CONFIG_MAX_RX_ANT + 2) * 4;
#endif				
				if(param_len == calib_data_len + 1) {
					int save_ret = mmw_angle_save_calib_data_custom(param);
					if (save_ret == 0) {
						ret = HIF_CMD_STATUS_SUCCESS;
						angle_est_data.angle_calib_mode = 0;
					} else {			/* save error*/
						ret = HIF_CMD_STATUS_SYSERR;
					}
				} else {
					ret = HIF_CMD_STATUS_PARAM;
				}
			} else {
				ret = HIF_CMD_STATUS_UNSUPPORT;
			}
		} else {
			if (ft_mmw_perf_status == HIF_ERRCODE_NOT_READY) {
				ret = HIF_CMD_STATUS_IO;
				printf("not calib\n");
			} else {
				if(ft_mmw_perf_status == HIF_CMD_STATUS_SUCCESS) {
					if (mmw_angle_save_calib_data()) {
						ret = HIF_CMD_STATUS_SYSERR;
					}
				} else {
					ret = HIF_CMD_STATUS_STATE;
				}
				ft_mmw_perf_status = HIF_ERRCODE_NOT_READY;
			}
		}
		ft_proc_status.ft_running = false;
	} else {
		ret = HIF_CMD_STATUS_IO;
		printf("please enter factory test mode\n");
	}
	return HIF_MsgResp(msg, 0, ret);
}

int factory_test_get_calib_handler(HIF_MsgHdr_t *msg){
	
	int ret;
	int hif_ret = HIF_CMD_STATUS_SUCCESS;
	
	if (ft_proc_status.ft_status == true && ft_proc_status.ft_running == false) {
		
		ft_proc_status.ft_running = true;
#if CONFIG_SOC_SERIES_RS624X
		uint8_t calib_data[CONFIG_MAX_RX_ANT * 4];
#elif CONFIG_SOC_RS6130
		uint8_t calib_data[HWINFO_SIZE_NVM_MMW_CALIB];
#endif
		uint8_t *ack_data = HIF_Msg_AckDataPtr(msg);
		ret = mmw_angle_get_calib_data((uint32_t *)&calib_data);
		if (ret == 0) {
			memcpy(ack_data, &calib_data, sizeof(calib_data));
			HIF_MsgResp(msg, sizeof(calib_data), hif_ret);
		} else {
			hif_ret = HIF_CMD_STATUS_IO;
			HIF_MsgResp(msg, 0, hif_ret);
		}
		ft_proc_status.ft_running = false;
	} else {
		hif_ret = HIF_CMD_STATUS_IO;
		printf("please enter factory test mode\n");
		HIF_MsgResp(msg, 0, hif_ret);
	}
	return 0;
}

int factory_test_voltage_handler(HIF_MsgHdr_t *msg){
	
	int hif_ret = HIF_CMD_STATUS_SUCCESS;
	
	if (ft_proc_status.ft_status == true && ft_proc_status.ft_running == false) {
		ft_proc_status.ft_running = true;
		
		struct dcdc_vol_t *vol = (struct dcdc_vol_t *)(msg + 1);
		
		uint8_t *ack_data = HIF_Msg_AckDataPtr(msg);

		uint32_t adc_val = ft_ctrl_adc_read_mv();
		MMW_LOG_DBG("%d\n", adc_val);
	
		if(adc_val > vol->min && adc_val < vol->max) {
			hif_ret = HIF_CMD_STATUS_SUCCESS;
		} else {
			hif_ret = HIF_CMD_STATUS_IO;
		}

		memcpy(ack_data, &adc_val, sizeof(adc_val));
		HIF_MsgResp(msg, sizeof(ack_data), hif_ret);
		ft_proc_status.ft_running = false;
	} else {
		hif_ret = HIF_CMD_STATUS_IO;
		printf("please enter factory test mode\n");
		HIF_MsgResp(msg, 0, hif_ret);
	}
	return 0;
}

int factory_test_io_handler(HIF_MsgHdr_t *msg)
{
	int hif_ret = HIF_CMD_STATUS_SUCCESS;
	if (ft_proc_status.ft_status == true && ft_proc_status.ft_running == false) {
		
		ft_proc_status.ft_running = true;
		
		struct io_ctrl_t *io_ctrl = (struct io_ctrl_t *)(msg + 1);
		struct io_output_ctrl_t *io_output_ctrl = (struct io_output_ctrl_t *)(msg + 1);
		struct io_input_ctrl_t *io_input_ctrl = (struct io_input_ctrl_t *)(msg + 1);
	
		GPIO_Pin_t input_pin, output_pin;
		HAL_Dev_t input_gpioDev, output_gpioDev;
		HAL_Status_t status = HAL_STATUS_OK;

		/* input and output mode */
		if (io_ctrl->type == 0) {
			if(msg->length == 3) {
				io_ctrl->input_mode = 0;
			}
		
			uint8_t read_value;
			/* init input GPIO */
			status = ft_GPIO_PIN_init(io_ctrl->input, FT_GPIO_DIR_INPUT, io_ctrl->input_mode, &input_gpioDev, &input_pin);
			if (status == 0) {		/* init success */
				/* init output GPIO */
				status = ft_GPIO_PIN_init(io_ctrl->output, FT_GPIO_DIR_OUTPUT, 0, &output_gpioDev, &output_pin);
				if (status == 0) {
					for (int output_value = 0; output_value < 2; output_value++) {
						/* take turns to writing output pin low/high level */
						status = HAL_GPIO_WritePin(&output_gpioDev, output_pin, output_value);
						if (status == 0) {	/* write output success */
							/* wait 10ms, read output value */
							msleep(10);
							read_value = HAL_GPIO_ReadPin(&input_gpioDev, input_pin);
							if (read_value != GPIO_PIN_SET) {	/* if read value is not equal to write value, break loop */
								hif_ret = HIF_CMD_STATUS_IO;
								break;
							} 
							msleep(10);
						} else {	/* write output fail */
							hif_ret = HIF_CMD_STATUS_IO;
							break;
						}
					}
				} else {	/* init output fail */
					hif_ret = HIF_CMD_STATUS_IO;
				}
			} else {  	/* init input fail */
				hif_ret = HIF_CMD_STATUS_IO;
			}
		/* output mode */
		} else if (io_ctrl->type == 1) {
			/* init output GPIO */
			status = ft_GPIO_PIN_init(io_output_ctrl->output, FT_GPIO_DIR_OUTPUT, 0, &output_gpioDev, &output_pin);
			if (status == 0) {  /* init success */
				/* write usr param value on output pin */
				status = HAL_GPIO_WritePin(&output_gpioDev, output_pin, io_output_ctrl->value);
				if (status != 0) {	/* write output fail */
					hif_ret = HIF_CMD_STATUS_IO;
				}
			} else {	/* init fail */
				hif_ret = HIF_CMD_STATUS_IO;
			}
		/* input mode */
		} else if (io_ctrl->type == 2) {
			if(msg->length == 3) {
				io_input_ctrl->input_mode = 0;
			}
			/* init input GPIO */
			status = ft_GPIO_PIN_init(io_input_ctrl->input, FT_GPIO_DIR_INPUT, io_input_ctrl->input_mode, &input_gpioDev, &input_pin);
			if (status == 0) { /* init success */
				uint8_t value = io_input_ctrl->value;
				uint8_t read_value;
				read_value = HAL_GPIO_ReadPin(&input_gpioDev, input_pin);
				if (read_value != value) {
					hif_ret = HIF_CMD_STATUS_IO;
				}
			} else {	/* init fail */
				hif_ret = HIF_CMD_STATUS_IO;
			}
		} else {
			hif_ret = HIF_CMD_STATUS_PARAM;
		}
		ft_proc_status.ft_running = false;
		HIF_MsgResp(msg, 0, hif_ret);
	} else {
		hif_ret = HIF_CMD_STATUS_IO;
		printf("please enter factory test mode\n");
		HIF_MsgResp(msg, 0, hif_ret);
	}
	return 0;
}

static int ft_trigger_frame_cb(void *mmw_data, void *arg) {

	mmw_dsp_poweron();
	PointCloud_rept pc_report = {
		.range = 0,
		.azim_deg = 0,
		.elev_deg = 0
	};
    PointCloudBuffer_t *ptr_point_cloud_buffer;
	
	uint32_t trigger_range_cm = FT_TRIGGER_RANGE_MM / 10;
	uint16_t valid_pc_num = 0;
	uint8_t ft_led_flag = 0;
	/* get point cloud result via function mmw_point_cloud_process() */
	ptr_point_cloud_buffer = mmw_point_cloud_process();
	/* check whether exist point cloud within FT_TRIGGER_RANGE_MM */
	valid_pc_num = ft_judge_trigger_detection(&pc_report, ptr_point_cloud_buffer, trigger_range_cm, ptr_point_cloud_buffer->point_cloud_num, FT_MIN_FORCE_TRIGGER_MOVING_PC_NUM);
	MMW_LOG_DBG("%x, %x, %x\n",pc_report.range, pc_report.azim_deg, pc_report.elev_deg);
	
	mmw_psic_auto_gain_clear();
	mmw_process_mem_free((void**) &ptr_point_cloud_buffer->ptr_motion_point_cloud_data);
	mmw_process_mem_free((void**) &ptr_point_cloud_buffer);
	
	if (valid_pc_num != 0) {
		ft_led_flag = 1;
		ft_trigger_point_cloud_report(&pc_report, sizeof(pc_report) * 1);
	}

	ft_led_set(ft_trigger_wkio_Dev, ft_led_flag);
	return 0;
}

int ft_power_mode_2hz_frame_config(void) 
{
	int ret;
	
#if CONFIG_SOC_RS6130
    ret = mmw_mode_cfg(MMW_MIMO_1T3R, MMW_WORK_MODE_1DFFT);
#elif CONFIG_SOC_SERIES_RS624X
    ret = mmw_mode_cfg(MMW_MIMO_2T4R, MMW_WORK_MODE_1DFFT);
#else
    #error "not support board"
#endif	
	if (ret) {
        printf("mode cfg error! %d\n", ret);
    }
    ret = mmw_range_cfg(10240, 80); /* max10.24m, resol=8cm */
    if (ret) {
        printf("range cfg error! %d\n", ret);
    }
    ret = mmw_velocity_cfg(1600, 200); /* max=1.6m/s, resol=0.02m/s, 16fft */
    if (ret) {
        printf("velocity cfg error! %d\n", ret);
    }
    ret = mmw_frame_cfg(500, 0);	/* frame period 500ms */
    if (ret) {
        printf("frame cfg error! %d\n", ret);
    }
	ret = mmw_freq_cfg (58000, 70000);	/* start frequency: 58GHz, end frequency: 70GHz */
	if (ret) {
		printf("mmw_freq_cfg error\n");
	}
    return ret;
}

static int ft_2hz_mode_frame_cb(void *mmw_data, void *arg) {
	return 0;
}

int factory_test_power_handler(HIF_MsgHdr_t *msg)
{
	int ret = HIF_CMD_STATUS_SUCCESS;
	if (ft_proc_status.ft_status == true && ft_proc_status.ft_running == false) {
		ft_proc_status.ft_running = true;
		
		uint8_t *ppwr_ctrl = (uint8_t *)(msg + 1);
		uint8_t test_mode = ppwr_ctrl[0];	// 0 --> standby mode; 1--> 2Hz typical mode.

		/* standby */
		if (test_mode == FT_POWER_MODE_STANDBY) {
			uint8_t wake_mode = ppwr_ctrl[1];
			if (wake_mode == FT_POWER_WAKEUP_MODE_IO) { /* wakeup io */
		
				uint8_t wakeup_io_pin =  ppwr_ctrl[2];
				HAL_Status_t status = HAL_STATUS_OK;
			
				hif_pm_unlock();
				status = ft_power_io_wakeup(wakeup_io_pin);
				if (status != HAL_STATUS_OK) {
					ret = HIF_CMD_STATUS_IO;
				}
				hif_pm_lock();
				HIF_MsgResp(msg, 0, ret);
			} else if (wake_mode == FT_POWER_WAKEUP_MODE_TIMEROUT) { /* timerout */
				uint32_t timerout_ms = ppwr_ctrl[2] * 1000;
			
				hif_pm_unlock();
				ft_power_timer_wakeup(timerout_ms);
				hif_pm_lock();
				HIF_MsgResp(msg, 0, ret);
			} else {
				ret = HIF_CMD_STATUS_PARAM;
				HIF_MsgResp(msg, 0, ret);
			}
		/* mmw 2Hz */
		} else if (test_mode == FT_POWER_MODE_MMW_2HZ) {
			uint8_t enter_flag = ppwr_ctrl[1];
			if (enter_flag == FT_POWER_MMW_2HZ_ENTER) {
				printf("mmw 2hz mode\n");
				int status = 0;
				
				status = ft_power_mode_2hz_frame_config();
				if (status) {
					ret = HIF_CMD_STATUS_IO;
					HIF_MsgResp(msg, 0, ret);
					return -1;
				}
				
				status = mmw_ctrl_callback_cfg(&ft_2hz_mode_frame_cb, MMW_DATA_TYPE_2DFFT, NULL);
				if (status != 0) {
					printf("mmw_ctrl_callback_cfg fail %d\n", status);
				}

				status = mmw_ctrl_start();
				if (status != 0) {
					printf("mmw ctrl start fail %d\n", status);
				} 
				hif_device_suspend_req();
				HIF_MsgResp(msg, 0, ret);

			} else if (enter_flag == FT_POWER_MMW_2HZ_QUIT) {
				printf("quit mmw 2hz mode\n");

				int status = mmw_ctrl_stop();
				if (status != 0) {
					printf("mmw ctrl stop fail %d\n", status);
				} 
				
				HIF_MsgResp(msg, 0, ret);			
			} else {
				ret = HIF_CMD_STATUS_PARAM;
				HIF_MsgResp(msg, 0, ret);
			}

		} else {
			ret = HIF_CMD_STATUS_PARAM;
			HIF_MsgResp(msg, 0, ret);
		}
		ft_proc_status.ft_running = false;
	} else {
		ret = HIF_CMD_STATUS_IO;
		printf("please enter factory test mode\n");
		HIF_MsgResp(msg, 0, ret);
	}
	return 0;
}

static TaskHandle_t    ft_mmw_trigger_task_handle    = NULL;
void factory_test_trigger_task(void *param){
	
	int status = 0;

	ft_proc_status.ft_running = true;
	ft_trigger_wkio_Dev = ft_trigger_init(&status);
	if (status != 0) {
		printf("ft trigger init fail %d\n", status);
	}

	status = mmw_ctrl_callback_cfg(&ft_trigger_frame_cb, MMW_DATA_TYPE_2DFFT, NULL);
	if (status != 0) {
		printf("mmw_ctrl_callback_cfg fail %d\n", status);
	}

	status = OSI_SemaphoreCreate(&trigger_mmw_wave_sem, 0, 1);
	if (status == OSI_STATUS_OK) {
		MMW_LOG_DBG("mmw wave Semaphore create success\n");
	}

	status = mmw_ctrl_start();
	if (status != 0) {
		printf("mmw ctrl start fail %d\n", status);
	}

	HIF_MsgResp(param, 0, status);
	OSI_SemaphoreWait(&trigger_mmw_wave_sem, portMAX_DELAY);
}

int factory_test_trigger_func_handler(HIF_MsgHdr_t *msg)
{
    uint8_t ret = HIF_CMD_STATUS_SUCCESS;

	if (ft_proc_status.ft_status == true) {	
		
		uint8_t test_mode = *((uint8_t *)(msg + 1));	// 1 --> enter; 0 --> quit;
		
		if (test_mode == FT_TRIGGER_MODE_ENTER && ft_proc_status.ft_running == false) {
			if(pdPASS != xTaskCreate(factory_test_trigger_task, "ft_mmw_trigger task", 512, msg, 1, &ft_mmw_trigger_task_handle)){
				ret = HIF_CMD_STATUS_IO;
				HIF_MsgResp(msg, 0, ret);
				MMW_LOG_DBG("ft_mmw_trigger task create failed\n");
			} else {
				MMW_LOG_DBG("ft_mmw_trigger task create success\n");
			}
		} else if (test_mode == FT_TRIGGER_MODE_QUIT && ft_proc_status.ft_running == true) {
			ft_trigger_deinit(&trigger_mmw_wave_sem, ft_trigger_wkio_Dev);
			HIF_MsgResp(msg, 0, ret);
			ft_proc_status.ft_running = false;
			vTaskDelete(ft_mmw_trigger_task_handle);
		} else {
			ret = HIF_CMD_STATUS_IO;
			HIF_MsgResp(msg, 0, ret);
		}
	}
	else {		
		ret = HIF_CMD_STATUS_IO;
		HIF_MsgResp(msg, 0, ret);
		printf("please enter factory test mode\n");
	}
    return 0;
}

static int factory_test_start_handler(HIF_MsgHdr_t *msg)
{
	int ret = HIF_CMD_STATUS_SUCCESS;
	
	uint8_t factory_test_flag = *((uint8_t *)(msg + 1));
	/* check the status of factory test */
	if (!ft_proc_status.ft_status) {			// not in factory test mode
		if (factory_test_flag) {
			ft_proc_status.ft_status = 1;
			printf("enter factory test mode\n");
			ret = HIF_CMD_STATUS_SUCCESS;
		}
		else {
			printf("please enter factory test mode\n");
		}
	}
	else {		// in factory test mode
		if (factory_test_flag) {
			ret = HIF_CMD_STATUS_IO;
			printf("already in factory test mode\n");
		}
		else {
			ft_proc_status.ft_status = 0;
			angle_est_data.angle_calib_mode = 0;
			ret = HIF_CMD_STATUS_SUCCESS;
			if (ft_proc_status.ft_running) {
				ft_trigger_deinit(&trigger_mmw_wave_sem, ft_trigger_wkio_Dev);
				ft_proc_status.ft_running = false;
				if (ft_mmw_trigger_task_handle != NULL) {
					vTaskDelete(ft_mmw_trigger_task_handle);
				}
			}
			printf("quit factory test mode\n");
		}
	}
	return HIF_MsgResp(msg, 0, ret);
}

int factory_test_iq_data_get_handler(HIF_MsgHdr_t *msg){
	
	int ret = HIF_CMD_STATUS_SUCCESS;

    uint8_t *ack_data = HIF_Msg_AckDataPtr(msg);
    uint8_t iq_data[CONFIG_MAX_RX_ANT * 4] = {0x00};
	
	if (ft_proc_status.ft_status == true && ft_proc_status.ft_running == false) {
		ft_proc_status.ft_running = true;
		
		if (ft_mmw_perf_status == HIF_ERRCODE_NOT_READY) {
			ret = HIF_CMD_STATUS_IO;
			printf("not calib\n");
			HIF_MsgResp(msg, 0, ret);
		} else {
			if(ft_mmw_perf_status == HIF_CMD_STATUS_SUCCESS) {
				ret = mmw_angle_calib_iq_data_get(iq_data, sizeof(iq_data));
				memcpy(ack_data, &iq_data, sizeof(iq_data));
				HIF_MsgResp(msg, sizeof(iq_data), ret);
			} else {
				ret = HIF_CMD_STATUS_STATE;
				HIF_MsgResp(msg, 0, ret);
			}
		}
		ft_proc_status.ft_running = false;
	} else {
		ret = HIF_CMD_STATUS_IO;
		printf("please enter factory test mode\n");
		HIF_MsgResp(msg, 0, ret);
	}
	return 0;
}

int factory_test_msg_init(void){
	int status = 0;
	
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_FACTORY_MODE, factory_test_start_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_MMW_PERF_TEST, factory_test_fun_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_MMW_CALIB_TEST, factory_test_fun_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_PARAM_SAVE, factory_test_save_calib_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_PARAM_GET, factory_test_get_calib_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_VOLTAGE_TEST, factory_test_voltage_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_IO_TEST, factory_test_io_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_PWR_CONSUME_TEST, factory_test_power_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_MMW_FUN_TEST, factory_test_trigger_func_handler);
	status = HIF_MsgHdl_Regist(HIF_MSG_ID_MMW_CALIB_IQ_DATA_GET, factory_test_iq_data_get_handler);
	
	return status;
}

