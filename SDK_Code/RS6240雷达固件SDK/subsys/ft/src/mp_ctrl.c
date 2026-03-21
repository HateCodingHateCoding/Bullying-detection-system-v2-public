/*
 * Copyright (c) 2022 Possumic Technology.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mp_ctrl.h"
#include "osi.h"
#include "hal_wkio.h"
#include "hal_gpio.h"
#include "hal_efuse.h"
#include "hal_os.h"
#include "irq.h"
#include "ll_timer.h"
#include "ll_gpadc.h"
#include "ll_rcc_dev.h"

int mp_gpio_ctrl(uint8_t input_pin, uint8_t output_pin, uint8_t intput_mode)
{
	HAL_Dev_t *gpioOutputDev;
	HAL_Dev_t *gpioInputDev;
	HAL_Status_t status = HAL_STATUS_OK;
	GPIO_PinParam_t gpioParam;

	uint8_t outPort = output_pin / 16;
	uint8_t outPin_idx = output_pin % 16;

	gpioOutputDev = HAL_GPIO_Init(outPort);
	if (gpioOutputDev == NULL) {
		printf("Init gpio fail\n");
		return HAL_STATUS_INVALID_PARAM;
	}

	gpioParam.mode = GPIOx_Pn_F1_OUTPUT;
	gpioParam.driving = GPIO_DRIVING_LEVEL_1;

	status = HAL_GPIO_SetPinParam(gpioOutputDev, outPin_idx, &gpioParam);
	if (status != HAL_STATUS_OK) {
		printf("Set config gpio fail %d\n", status);
		return status;
	}

	uint8_t inPort = input_pin / 16;
	uint8_t inPin_idx = input_pin % 16;

	gpioInputDev = HAL_GPIO_Init(inPort);
	if (gpioInputDev == NULL) {
		printf("Init gpio fail\n");
		return HAL_STATUS_INVALID_PARAM;
	}

	gpioParam.mode = GPIOx_Pn_F0_INPUT;
	gpioParam.driving = GPIO_DRIVING_LEVEL_1;

	if(intput_mode == 1) {
		gpioParam.pull = GPIO_PULL_UP;
	} else if(intput_mode == 2) {
		gpioParam.pull  = GPIO_PULL_DOWN;
	} else {
		gpioParam.pull = GPIO_PULL_FLOATING;
	}

	status = HAL_GPIO_SetPinParam(gpioInputDev, inPin_idx, &gpioParam);
	if (status != HAL_STATUS_OK) {
		printf("Set config gpio fail %d\n", status);
		return status;
	}

	HAL_GPIO_WritePin(gpioOutputDev, outPin_idx, 1);

	OSI_delay_busy_wait(10);

	if(HAL_GPIO_ReadPin(gpioInputDev, inPin_idx) != 1) {
		return -1;
	}

	HAL_GPIO_WritePin(gpioOutputDev, outPin_idx, 0);

	OSI_delay_busy_wait(10);

	if(HAL_GPIO_ReadPin(gpioInputDev, inPin_idx) != 0) {
		return -1;
	}

	return 0;
}

int mp_gpio_output_ctrl(uint8_t output_pin, uint8_t output_value)
{
	HAL_Dev_t *gpioDev;
	HAL_Status_t status = HAL_STATUS_OK;
	GPIO_PinParam_t gpioParam;

	uint8_t port = output_pin / 16;
	uint8_t pin_idx = output_pin % 16;

	gpioDev = HAL_GPIO_Init(port);
	if (gpioDev == NULL) {
		printf("Init gpio fail\n");
		return HAL_STATUS_INVALID_PARAM;
	}

	gpioParam.mode = GPIOx_Pn_F1_OUTPUT;
	gpioParam.driving = GPIO_DRIVING_LEVEL_1;

	status = HAL_GPIO_SetPinParam(gpioDev, pin_idx, &gpioParam);
	if (status != HAL_STATUS_OK) {
		printf("Set config gpio fail %d\n", status);
		return status;
	}

	HAL_GPIO_WritePin(gpioDev, pin_idx, output_value);

	return 0;
}


int mp_gpio_input_ctrl(uint8_t input_pin, uint8_t input_value, uint8_t intput_mode)
{
	HAL_Dev_t *gpioDev;
	HAL_Status_t status = HAL_STATUS_OK;
	GPIO_PinParam_t gpioParam;

	uint8_t port = input_pin / 16;
	uint8_t pin_idx = input_pin % 16;

	gpioDev = HAL_GPIO_Init(port);
	if (gpioDev == NULL) {
		printf("Init gpio fail\n");
		return HAL_STATUS_INVALID_PARAM;
	}

	gpioParam.mode = GPIOx_Pn_F0_INPUT;
	gpioParam.driving = GPIO_DRIVING_LEVEL_1;

	if(intput_mode == 1) {
		gpioParam.pull = GPIO_PULL_UP;
	} else if(intput_mode == 2) {
		gpioParam.pull  = GPIO_PULL_DOWN;
	} else {
		gpioParam.pull = GPIO_PULL_FLOATING;
	}

	status = HAL_GPIO_SetPinParam(gpioDev, pin_idx, &gpioParam);
	if (status != HAL_STATUS_OK) {
		printf("Set config gpio fail %d\n", status);
		return status;
	}

	if(HAL_GPIO_ReadPin(gpioDev, pin_idx) == input_value) {
		return 0;
	} else {
		return 1;
	}
}


OSI_Semaphore_t g_wkio_sem;
static void wkio_irq_cb(void *arg)
{
    OSI_SemaphoreRelease(&g_wkio_sem);
}

void mp_pm_wkio(uint8_t pin)
{
	OSI_SemaphoreCreate(&g_wkio_sem, 0, 1);

	HAL_Dev_t *wkioDev;
	HAL_Status_t status = HAL_STATUS_OK;
	WKIO_WakeParam_t wkioIrqParam;

	uint8_t port = pin / 16;
	uint8_t pin_idx =pin % 16;

	wkioDev = HAL_WKIO_Init(port);
	if (wkioDev == NULL) {
		printf("Init wkio fail\n");
		return ;
	}

	status = HAL_WKIO_SetPinMode(wkioDev, pin_idx, HAL_WKIO_MODE_WAKE);
	if (status != HAL_STATUS_OK) {
		printf("Set wkio mode fail %d\n", status);
		return ;
	}

	wkioIrqParam.event = WKIO_WAKE_EVT_LOW_LEVEL;
	wkioIrqParam.pull = HAL_WKIO_MODE_WAKE;
	wkioIrqParam.callback = wkio_irq_cb;
	wkioIrqParam.arg = NULL;

	status = HAL_WKIO_SetWakeParam(wkioDev, pin_idx, &wkioIrqParam);
	if (status != HAL_STATUS_OK) {
		printf("Set wkio wake init fail %d\n", status);
		return ;
	}

	OSI_SemaphoreWait(&g_wkio_sem, OSI_WAIT_FOREVER);
}

void mp_pm_timer(uint32_t time_out_ms)
{
	vTaskDelay(pdMS_TO_TICKS(time_out_ms));
}

void mp_ctrl_tmr0_delay_us(uint32_t wait_us)
{
    LL_TIM_ClockCtrlOn(1, 0, true);
    /* use 40Mhz as MSI clock */
    LL_TIM_BusyWait(wait_us * 40);
    LL_TIM_ClockCtrlOn(1, 0, false);
}

uint32_t mp_ctrl_adc_read_mv(void)
{
    #define GPADC_CH_S8     (8)
    uint32_t ch_val, sum_val = 0;
    bool need_restore = true;
    uint16_t adc_trim06 = 2202;
    uint16_t adc_trim24 = 8985;
    int status = 0;

#if CONFIG_DRIVER_EFUSE
	HAL_Dev_t *efuseDev = HAL_EFUSE_Init();
	if(efuseDev == NULL) {
		printf("efuse device init fail\n");
		return 0;
	}

    status = HAL_EFUSE_GetGpadcTrim(efuseDev, EFUSE_GPADC_VOLT_06, &adc_trim06);
    if (status == 0 && adc_trim06 != 0) {
        adc_trim06 *= 3;
        printf("efuse 06 %d\n", adc_trim06);
    } else {
        adc_trim06 = 2202;
    }

    status = HAL_EFUSE_GetGpadcTrim(efuseDev, EFUSE_GPADC_VOLT_24, &adc_trim24);
    if (status == 0 && adc_trim24 != 0) {
        adc_trim24 *= 3;

        printf("efuse 24 %d\n", adc_trim24);
    } else {
        adc_trim24 = 8985;
    }
#endif

    if (adc_trim24 <= adc_trim06) {
        adc_trim06 = 2202;
        adc_trim24 = 8985;
    }

    LL_GPADC_SetInitConfig();

    /*disable irq for polling mode*/
    if (IRQ_IsEnabled(GPADC_IRQn)) {
        IRQ_Disable(GPADC_IRQn);
        need_restore = true;
    }

    for (int i = 0; i < 16; i++) {
        LL_GPADC_SetChConfig(GPADC_CH_S8);
        ch_val = LL_GPADC_Polling_GetChVal(GPADC_CH_S8);
        ch_val = LL_GPADC_RawVal2mv(ch_val * 3, adc_trim06, 600, adc_trim24, 2400);
        sum_val += ch_val;
    }

    ch_val = sum_val / 16;

    if (need_restore == true) {
        /* restore gpadc irq*/
        IRQ_Enable(GPADC_IRQn);
    }

    return ch_val;
}

int mp_ctrl_dcdc_no_voltage(uint32_t min, uint32_t max)
{
    int ret = 0;
    uint32_t adc_val = 0;

    adc_val = mp_ctrl_adc_read_mv();

    if ((adc_val > min) && (adc_val < max)) {
        ret = 0;
    } else {
    	ret = -1;
    }
    printf("dcdc %d mv\n", adc_val);

    return ret;
}



