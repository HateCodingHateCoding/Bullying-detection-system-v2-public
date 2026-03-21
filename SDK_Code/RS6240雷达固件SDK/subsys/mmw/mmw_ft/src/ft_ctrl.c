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

#include "ft_ctrl.h"
#include "hif.h"
#include "mmw_app_pointcloud_config.h"

extern uint32_t power_ReadV13(void);
uint32_t ft_ctrl_adc_read_mv(void)
{
    uint32_t ch_val;

    LL_GPADC_SetInitConfig();
	ch_val = power_ReadV13();

    return ch_val;
}

/** @brief init GPIO pin 
 *  @return return 0 on success */
int ft_GPIO_PIN_init(uint8_t pin, uint8_t GPIO_Dir, uint8_t inputmode, HAL_Dev_t *gpioDev, GPIO_Pin_t *GPIO_PIN){
	
	GPIO_Port_t GPIO_Port;
    HAL_Status_t status = HAL_STATUS_OK;
    GPIO_PinParam_t gpioParam;
	
	if (pin < 16) {
		GPIO_Port = GPIO_PORT_A;
		*GPIO_PIN = pin;
	} else if (pin < (16 + 8)) {
		GPIO_Port = GPIO_PORT_B;
		*GPIO_PIN = pin - 16;
	} else {
		return -1;
	}	
	
	gpioDev = HAL_GPIO_Init(GPIO_Port);
	if (gpioDev == NULL) {
		printf("Init gpio fail\n");
		return HAL_STATUS_INVALID_PARAM;
	}
	
	if (GPIO_Dir == FT_GPIO_DIR_INPUT) {
		gpioParam.mode = GPIOx_Pn_F0_INPUT;
		gpioParam.driving = GPIO_DRIVING_LEVEL_1;
		gpioParam.pull = inputmode;
	} else if (GPIO_Dir == FT_GPIO_DIR_OUTPUT) {
		gpioParam.mode = GPIOx_Pn_F1_OUTPUT;
		gpioParam.driving = GPIO_DRIVING_LEVEL_1;
		gpioParam.push = GPIO_PUSH_PULL;
		gpioParam.pull = GPIO_PULL_FLOATING;
	} else {
		return HAL_STATUS_INVALID_PARAM;
	}
	
	status = HAL_GPIO_SetPinParam(gpioDev, *GPIO_PIN, &gpioParam);
	if (status != HAL_STATUS_OK) {
		printf("Set config gpio fail %d\n", status);
		return status;
	}
	return status;
}

HAL_Dev_t *ft_trigger_init(int *status) {
	
	HAL_Dev_t *wkioDev = NULL;
	*status = HAL_STATUS_OK;
	*status = mmw_point_cloud_frame_config();
	mmw_frame_cfg(200, 0);
	if (*status != 0) {
		return wkioDev;
	}
	*status = mmw_point_cloud_init();
	/* limit max cfar num */
	mmw_point_cloud_get_user_cfg()->mmw_motion_point_cloud_config.motion_point_cloud_max_cfar_num = 200;
	
	wkioDev = HAL_WKIO_Init(FT_LED_WKIO_PORT);
    if (wkioDev == NULL) {
        printf("Init wkio fail\n");
		*status = HAL_STATUS_INVALID_PARAM;
        return wkioDev;
    }

    *status = HAL_WKIO_SetPinMode(wkioDev, FT_LED_WKIO_PIN, FT_LED_WKIO_MODE);
    if (*status != HAL_STATUS_OK) {
        printf("Set wkio mode fail %d\n", *status);
        return wkioDev;
    }
	
	return wkioDev;
}

int ft_trigger_deinit(OSI_Semaphore_t *sem, HAL_Dev_t *wkioDev) {
	
	int status = 0;
	
	if (OSI_SemaphoreIsValid(sem)) {
		status = mmw_ctrl_stop();
		if (status != 0) {
			printf("mmw ctrl stop fail %d\n", status);
		} 
		status = OSI_SemaphoreRelease(sem);
		if (status == OSI_STATUS_OK) {
			MMW_LOG_DBG("mmw wave task released\n");
		}
		mmw_point_cloud_deinit();
	}
	
	ft_led_set(wkioDev, 0);
	
	HAL_WKIO_DeInit(wkioDev);
	
	return status;
	
}

uint16_t ft_judge_trigger_detection(PointCloud_rept *ptr_report, PointCloudBuffer_t *ptr_3d_pc, uint16_t force_trigger_cm, uint16_t pc_num, uint16_t pc_num_threshold)
{
	float range_cm;
	float sin_y, cart_x, cart_y;
	uint32_t range_mm, range_reol_mm;
	uint16_t range_fft_num, doppler_fft_num;
	float range_bin_size_cm;
	
	uint16_t valid_pc_num = 0;
	
	if (ptr_3d_pc) {
		mmw_range_get(&range_mm, &range_reol_mm);
		mmw_fft_num_get(&range_fft_num, &doppler_fft_num);
		range_bin_size_cm = (float)range_mm / range_fft_num / 10.f;

		for (int pc_idx = 0; pc_idx < pc_num; pc_idx++) {
			range_cm = ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].range_idx * range_bin_size_cm;
			float sin_y_tmp = 1.f - ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase * ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase - 
                    ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].ele_phase * ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].ele_phase;
			if (sin_y_tmp < 0) {
				continue;
			}
			sin_y = sqrtf(sin_y_tmp);
			cart_x = range_cm * ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase;   // x
            cart_y = range_cm * sin_y;                                                      // y
			
			if (ABS(cart_x) < force_trigger_cm && ABS(cart_y) < force_trigger_cm) {
				float ele_rad = asinf(ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase);
				float azi_rad = ptr_3d_pc->ptr_motion_point_cloud_data[pc_idx].azi_phase / cosf(ele_rad);
				if ( azi_rad < -1 || azi_rad > 1) {
					continue;
				}
				/* deg = rad * 180 / pi = rad * 180 / 3.1415926 = rad * 180 * 0.31830988618379067153803535746773 */
				float azi_deg = asinf(azi_rad) * 180.f * 0.31830988618f;
				float ele_deg = ele_rad * 180.f * 0.31830988618f;
				MMW_LOG_DBG("azi: %f, ele: %f\n", azi_deg, ele_deg);

				/* because point cloud is ranged by range, so only first point is tested */
				if (valid_pc_num == 0) {
					ptr_report->range = range_cm;
					ptr_report->azim_deg = azi_deg;
					ptr_report->elev_deg = ele_deg; // q7
				}
				valid_pc_num++;
				if (valid_pc_num == pc_num_threshold) {
					break;
				}
				
			} else {
				/* If exceeds force trigger range, break the loop */
				break;
			}
		}	

		if (valid_pc_num == pc_num_threshold) {
			/* a valid report is gotten */
			valid_pc_num = 1;
		} else {
			valid_pc_num = 0;
		}
	}
	return valid_pc_num;
}


HAL_Dev_t *ft_led_init(HAL_Status_t *status){
	
	HAL_Dev_t *wkioDev;
	*status = HAL_STATUS_OK;
	
	wkioDev = HAL_WKIO_Init(FT_LED_WKIO_PORT);
    if (wkioDev == NULL) {
        printf("Init wkio fail\n");
		*status = HAL_STATUS_INVALID_PARAM;
        return wkioDev;
    }

    *status = HAL_WKIO_SetPinMode(wkioDev, FT_LED_WKIO_PIN, FT_LED_WKIO_MODE);
    if (*status != HAL_STATUS_OK) {
        printf("Set wkio mode fail %d\n", *status);
		
        return wkioDev;
    }
	
	return wkioDev;
}

int ft_led_set(HAL_Dev_t *wkioDev, WKIO_PinState_t pinState){

	HAL_Status_t status = HAL_STATUS_OK;
	
    if (wkioDev == NULL) {
        printf("Init wkio fail\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    status = HAL_WKIO_WritePin(wkioDev, FT_LED_WKIO_PIN, pinState);
    if (status != HAL_STATUS_OK) {
        printf("Set wkio hold output fail %d\n", status);
        return status;
    }
	return status;
}

void ft_trigger_point_cloud_report(void *ptr_data, uint16_t len)
{
	
    HIF_MsgHdr_t msgHdr = {0x00};
	msgHdr.flag = (1<<2);
	msgHdr.msg_id = 0xC8;
	msgHdr.type = 2;
	
	payload_node_t *payload_head = NULL;
	if(ptr_data) {
		payload_head = HIF_MsgReport_ListAddData(payload_head, ptr_data, len);
	}
	
	HIF_MsgReport_ListStart(&msgHdr, payload_head, NULL);	//blocking mode callback function is NULL
	
}


int ft_power_io_wakeup(uint8_t pin) {
	
	HAL_Dev_t *wkioDev;
    HAL_Status_t status = HAL_STATUS_OK;
    WKIO_WakeParam_t wkioIrqParam;
	
	WKIO_Port_t WKIO_Port;
	WKIO_Pin_t WKIO_PIN;

	if (pin < 18) {
		WKIO_Port = WKIO_PORT_A;
		WKIO_PIN = pin;
	} else {
		return -1;
	}
	
	wkioDev = HAL_WKIO_Init(WKIO_Port);
	if (wkioDev == NULL) {
        LOG_PRINT("Init wkio fail\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    status = HAL_WKIO_SetPinMode(wkioDev, WKIO_PIN, HAL_WKIO_MODE_WAKE);
    if (status != HAL_STATUS_OK) {
        LOG_PRINT("Set wkio mode fail %d\n", status);
        return status;
    }
	
	wkioIrqParam.event = WKIO_WAKE_EVT_FALLING_EDGE;
    wkioIrqParam.pull = HAL_WKIO_PULL_DOWN;

    status = HAL_WKIO_SetWakeParam(wkioDev, WKIO_PIN, &wkioIrqParam);
    if (status != HAL_STATUS_OK) {
        LOG_PRINT("Set wkio wake init fail %d\n", status);
        return status;
    }

    status = HAL_WKIO_EnableWake(wkioDev);
    if (status != HAL_STATUS_OK) {
        LOG_PRINT("Set wkio wake enable fail %d\n", status);
        return status;
    }
	return status;
}

void ft_power_timer_wakeup(uint32_t ms) {
	
	msleep(ms);
	
}