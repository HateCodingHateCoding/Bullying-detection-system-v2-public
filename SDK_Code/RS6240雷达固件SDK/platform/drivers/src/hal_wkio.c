/**
 ******************************************************************************
 * @file       hal_wkio.c
 * @brief      LPIO hardware abstract layer define source.
 * @verbatim   null
 ******************************************************************************
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
 ******************************************************************************
 */
/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "hal_types.h"
#include "hal_dev.h"
#include "ll_lpio.h"
#include "ll_pmu.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_utils.h"
#include "hal_wkio.h"
#include "irq.h"
#include "hal_gpio.h"
#include "hal_board.h"

/* Private defines.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_WKIO_IRQ_PRIO
#define CONFIG_WKIO_IRQ_PRIO                                7
#endif

#ifndef CONFIG_WKIO_PM
#if (CONFIG_PM == 1)
#define CONFIG_WKIO_PM                                      1
#else
#define CONFIG_WKIO_PM                                      0
#endif
#endif

#ifndef CONFIG_WKIO_SRC
#define CONFIG_WKIO_SRC                                     2  /* lpclk frac */
#endif

#ifndef CONFIG_WKIO_TO
#define CONFIG_WKIO_TO                                      300
#endif

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

typedef struct {
    WKIO_WakeCallback_t   callback;
    void                *arg;
} Wkio_PrivParam_t;


typedef struct {
    HAL_Dev_t           device;

    uint32_t            irqState;
    uint32_t            specFlag;
    uint8_t             pinNum;

    Wkio_PrivParam_t    privParam[0];
} Wkio_Device_t;

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#if (CONFIG_SECTION_ATTRIBUTE_SRAM && CONFIG_HIF)
#define __wkio_sram_text 			__sram_text
#define __wkio_sram_rodata 			__sram_rodata
#define __wkio_sram_data 			__sram_data
#define __wkio_sram_bss 			__sram_bss
#else
#define __wkio_sram_text
#define __wkio_sram_rodata
#define __wkio_sram_data
#define __wkio_sram_bss
#endif



/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
static uint32_t wkio_pmu_dis_mask;
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static void WKIO_IRQHandler(void * param);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Init wkio
 * @param WKIO DEV
 * @retval HAL_Dev_t*
 */

HAL_Dev_t *HAL_WKIO_Init(WKIO_Port_t id)
{
    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_WAKEIO, id);

    if (pWkioDevice == NULL) {
        switch (id) {
#if (CONFIG_SOC_WKIOA == 1)
        case WKIO_PORT_A:
        {
            pWkioDevice = (Wkio_Device_t *)HAL_DEV_MemMalloc(
                sizeof(Wkio_Device_t) + CONFIG_SOC_WKIOA_PIN_NUM * sizeof(Wkio_PrivParam_t));
            if (pWkioDevice == NULL) {
                return NULL;
            }

            pWkioDevice->device.reg         = NULL;
            pWkioDevice->device.irqNum      = WIO_IRQn;
            pWkioDevice->device.irqPrio     = CONFIG_WKIO_IRQ_PRIO;
            pWkioDevice->device.irqHandler  = WKIO_IRQHandler;
            pWkioDevice->pinNum             = CONFIG_SOC_WKIOA_PIN_NUM;

            break;
        }
#endif

#if (CONFIG_SOC_WKIOB == 1)
        case WKIO_PORT_B:
        {
            pWkioDevice = (Wkio_Device_t *)HAL_DEV_MemMalloc(
                sizeof(Wkio_Device_t) + CONFIG_SOC_WKIOB_PIN_NUM * sizeof(Wkio_PrivParam_t));
            if (pWkioDevice == NULL) {
                return NULL;
            }

            pWkioDevice->device.reg         = NULL;
            pWkioDevice->device.irqNum      = WIO_IRQn;
            pWkioDevice->device.irqPrio     = CONFIG_WKIO_IRQ_PRIO;
            pWkioDevice->device.irqHandler  = WKIO_IRQHandler;
            pWkioDevice->pinNum             = CONFIG_SOC_WKIOB_PIN_NUM;

            break;
        }
#endif

        default:
             return NULL;
        }

        /* config wakeup sys */
        if (LL_SYS_GetCurrentSysId()) {
            LL_WKUP_EnableSrc(LL_WKUP_SRC_WPIO, 1);
        } else {
            LL_WKUP_EnableSrc(LL_WKUP_SRC_WPIO, 0);
        }

        LL_PMU_SW_RTC1SoftEnable();
        LL_RCC_RTC_SetClockSource(CONFIG_WKIO_SRC);
        //LL_RCC_RTC_SetPrescaler(4);
        LL_RCC_RTC_DisableBusClock();
        LL_RCC_RTC_DisableClock();
        LL_RCC_RTC_Reset();
        LL_RCC_RTC_EnableBusClock();
        LL_RCC_RTC_EnableClock();

        HAL_DEV_Register((HAL_Dev_t *)pWkioDevice, HAL_DEV_TYPE_WAKEIO, id);

    }


    return (HAL_Dev_t *)pWkioDevice;
}

static void WKIO_IRQHandler(void * param)
{
    uint32_t start_tim = 0;
    uint32_t stop_tim = 0;

    if (param == NULL) {
        return;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)param;
    WKIO_WakeCallback_t  callback = NULL;

    for (int i = 0; i < pWkioDevice->pinNum; ++i) {
        if (LL_LPIO_GetWakePending() & HAL_BIT(i)) {
            LL_LPIO_ClearWakePending(HAL_BIT(i));

            start_tim = HAL_BOARD_GetTime(HAL_TIME_US);
            while (LL_LPIO_GetWakePending() & HW_BIT(i)) {
                stop_tim = HAL_BOARD_GetTime(HAL_TIME_US);
                if(stop_tim - start_tim > CONFIG_WKIO_TO) {
                    break;
                }
            }

            if (!(pWkioDevice->specFlag & HAL_BIT(i))) {
                callback = pWkioDevice->privParam[i].callback;
                if (callback != NULL) {
                    callback(pWkioDevice->privParam[i].arg);
                }
            } else {
                HAL_CLR_BIT(pWkioDevice->specFlag, i);
            }
        }
    }
}

/**
 * @brief Deinit wkio
 * @param WKIO DEV
 * @retval
 */
void HAL_WKIO_DeInit(HAL_Dev_t *wkio)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if (wkio != NULL) {
        status = HAL_DEV_Unregister(wkio);
        if (status == HAL_STATUS_OK) {
            HAL_DEV_MemFree(wkio);
        }
    }
}

/**
 * @brief Record the wkio pin that needs to be powered down
 * @param WKIO pin
* @param 1/0
 * @retval None
 */
static void HAL_WKIO_PMU_Dis(WKIO_Pin_t pin, uint32_t en)
{
    if (en == 1) {
        wkio_pmu_dis_mask &= ~(1 << pin);
        LL_PMU_SetRtcWioDisableMask(wkio_pmu_dis_mask);
    } else if (en == 0) {
        wkio_pmu_dis_mask |= (1 << pin);
        LL_PMU_SetRtcWioDisableMask(wkio_pmu_dis_mask);
    }
}

/**
 * @brief Set work mode of the specified WKIO
 * @param WKIO DEV
 * @param WKIO pin
 * @param mode work mode of the specified WKIO
 * @return HAL_Status_t
 */
HAL_Status_t HAL_WKIO_SetPinMode(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinMode_t mode)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_LPIO_SetPinMode(pin, mode);
    if (mode == HAL_WKIO_MODE_HOLD) {
        HAL_WKIO_SetPinPull(wkio, pin, HAL_WKIO_PULL_FLOATING);
    }

    if (mode == HAL_WKIO_MODE_DISA) {
        HAL_WKIO_PMU_Dis(pin, 0);    // turn off the power of the IO pin
    } else {
        HAL_WKIO_PMU_Dis(pin, 1);    // turn on the power of the IO pin
    }

    return HAL_STATUS_OK;
}

/**
 * @brief Get the mode of the specified WKIO
 * @param WKIO DEV
 * @param WKIO pin
 * @return mode
 */
HAL_Status_t HAL_WKIO_GetPinMode(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinMode_t *mode)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    *mode = (WKIO_PinMode_t)LL_LPIO_GetPinMode(pin);

    return HAL_STATUS_OK;
}

/**
 * @brief  Set pull type of the specified WKIO
 * @param  WKIO DEV
 * @param  WKIO pin
 * @param  pull pull type of the specified WKIO:
 *         @arg @ref HAL_HAL_WKIO_PULL_FLOATING
 *         @arg @ref HAL_WKIO_PULL_UP
 *         @arg @ref HAL_WKIO_PULL_DOWN
 * @return None
 */
HAL_Status_t HAL_WKIO_SetPinPull(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinPull_t pull)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_LPIO_SetPinPull(pin, pull);

    return HAL_STATUS_OK;
}

/**
 * @brief Get the pull of the specified WKIO
 * @param WKIO DEV
 * @param WKIO pin
 * @return pull
 */
HAL_Status_t HAL_WKIO_GetPinPull(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinPull_t *pull)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    *pull = (WKIO_PinPull_t)LL_LPIO_GetPinPull(pin);

    return HAL_STATUS_OK;
}

/**
 * @brief Set the state of the specified WKIO
 * @param WKIO DEV
 * @param WKIO pin
 * @param WKIO pin state
 * @retval HAL_Status_t
 */
__wkio_sram_text HAL_Status_t HAL_WKIO_WritePin(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinState_t pinState)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (pinState == WKIO_PIN_RESET) {
        LL_LPIO_ResetOutputPinMask(HW_BIT(pin));
    } else {
        LL_LPIO_SetOutputPinMask(HW_BIT(pin));
    }

    return HAL_STATUS_OK;
}

/**
 * @brief Set the state of the ALL WKIO
 * @param WKIO DEV
 * @param WKIO pin mask
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_WKIO_WritePort(HAL_Dev_t *wkio, uint32_t pinMask)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_LPIO_SetOutputPort(pinMask);

    return HAL_STATUS_OK;
}

/**
 * @brief Init the IRQ of the specified WKIO
 * @param WKIO DEV
 * @param WKIO pin
 * @param param Pointer to WKIO_WakeParam_t structure
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_WKIO_SetWakeParam(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_WakeParam_t *param)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;

    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    pWkioDevice->privParam[pin].callback = param->callback;
    pWkioDevice->privParam[pin].arg = param->arg;

    if (param->event == WKIO_WAKE_EVT_LOW_LEVEL) {
        HAL_SET_BIT(pWkioDevice->specFlag, pin);
    } else {
        HAL_CLR_BIT(pWkioDevice->specFlag, pin);
    }

    LL_LPIO_SetPinWakeType(pin, param->event);

    if (pWkioDevice->irqState == 0) {
        IRQ_Disable(pWkioDevice->device.irqNum);
        IRQ_Attach(pWkioDevice->device.irqNum, pWkioDevice->device.irqHandler);
        IRQ_AttachDevice(pWkioDevice->device.irqNum, wkio);
        IRQ_Priority(pWkioDevice->device.irqNum, pWkioDevice->device.irqPrio);
    }

    IRQ_Enable(pWkioDevice->device.irqNum);
    HAL_SET_BIT(pWkioDevice->irqState, pin);

    return HAL_STATUS_OK;
}

/**
 * @brief Enable the IRQ of the  WKIO
 * @param WKIO DEV
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_WKIO_EnableWake(HAL_Dev_t *wkio)
{
    uint32_t start_tim = 0;
    uint32_t stop_tim = 0;

    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;

    for (int i = 0; i < pWkioDevice->pinNum; ++i) {
        if (LL_LPIO_GetWakePending() & HW_BIT(i)) {
            LL_LPIO_ClearWakePending(HW_BIT(i));

            start_tim = HAL_BOARD_GetTime(HAL_TIME_US);
            while (LL_LPIO_GetWakePending() & HW_BIT(i)) {
                stop_tim = HAL_BOARD_GetTime(HAL_TIME_US);
                if(stop_tim - start_tim > CONFIG_WKIO_TO) {
                    return HAL_STATUS_INVALID_STATE;
                }
            }
        }
    }

    IRQ_Enable(pWkioDevice->device.irqNum);

    return HAL_STATUS_OK;
}

/**
 * @brief Disable the IRQ of the WKIO
 * @param WKIO DEV
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_WKIO_DisableWake(HAL_Dev_t *wkio)
{
    uint32_t start_tim = 0;
    uint32_t stop_tim = 0;

    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;

    for (int i = 0; i < pWkioDevice->pinNum; ++i) {
        if (LL_LPIO_GetWakePending() & HW_BIT(i)) {
            LL_LPIO_ClearWakePending(HW_BIT(i));

            start_tim = HAL_BOARD_GetTime(HAL_TIME_US);
            while (LL_LPIO_GetWakePending() & HW_BIT(i)) {
                stop_tim = HAL_BOARD_GetTime(HAL_TIME_US);
                if(stop_tim - start_tim > CONFIG_WKIO_TO) {
                    return HAL_STATUS_INVALID_STATE;
                }
            }
        }
    }

    IRQ_Disable(pWkioDevice->device.irqNum);

    return HAL_STATUS_OK;
}

/**
 * @brief Set IRQ trigger event of the specified WKIO
 * @param WKIO port
 * @param WKIO pin number
 * @param event IRQ trigger:
 *         @arg @ref WKIO_WAKE_EVT_RISING_EDGE
 *         @arg @ref WKIO_WAKE_EVT_FALLING_EDGE
 *         @arg @ref WKIO_WAKE_EVT_BOTH_EDGE
 *         @arg @ref WKIO_WAKE_EVT_HIGH_LEVEL
 *         @arg @ref WKIO_WAKE_EVT_LOW_LEVEL
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_WKIO_SetWakeEvent(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_WakeEvent_t waketype)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_LPIO_SetPinWakeType(pin, waketype);

    return HAL_STATUS_OK;
}

/**
 * @brief Get IRQ trigger event of the specified WKIO
 * @param WKIO DEV
 * @param WKIO pin number
 * @retval WKIO_WakeEvent_t
 */
HAL_Status_t HAL_WKIO_GetWakeEvent(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_WakeEvent_t *wakeEvent)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    *wakeEvent = (WKIO_WakeEvent_t)LL_LPIO_GetPinWakeType(pin);

    return HAL_STATUS_OK;
}


/**
 * @brief Get the state of the specified WKIO
 * @param WKIO DEV
 * @param WKIO pin
 * @return WKIO_PinState_t
 */
int32_t HAL_WKIO_ReadWakePin(HAL_Dev_t *wkio, WKIO_Pin_t pin)
{
    GPIO_PinParam_t gpioParam;
    HAL_Status_t status = HAL_STATUS_OK;
    uint32_t mode = 0;
    uint32_t wakeType = 0;
    uint32_t pull = 0;
    uint32_t input = 0;
    HAL_Dev_t *gpioDev = NULL;

    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Wkio_Device_t *pWkioDevice = (Wkio_Device_t *)wkio;
    if (pin > pWkioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    uint8_t devId = HAL_DEV_GetId(wkio);
#if (CONFIG_SOC_WKIOA == 1)
    if (devId == WKIO_PORT_A) {
        gpioDev = HAL_DEV_Find(HAL_DEV_TYPE_GPIO, GPIO_PORT_A);
    }
#endif

#if (CONFIG_SOC_WKIOB == 1)
    if (devId == WKIO_PORT_B) {
        gpioDev = HAL_DEV_Find(HAL_DEV_TYPE_GPIO, GPIO_PORT_B);
    }
#endif

    if (gpioDev == NULL) {
        return HAL_STATUS_INVALID_STATE;
    }


    HAL_WKIO_GetWakeEvent(wkio, pin, &wakeType);
    HAL_WKIO_GetPinPull(wkio, pin, &pull);
    HAL_WKIO_GetPinMode(wkio, pin, &mode);
    HAL_WKIO_SetPinMode(wkio, pin, HAL_WKIO_MODE_GPIO);

    gpioParam.mode = GPIOx_Pn_F0_INPUT;
    gpioParam.driving = GPIO_DRIVING_LEVEL_0;
    gpioParam.pull = HAL_WKIO_PULL_FLOATING;
    status = HAL_GPIO_SetPinParam(gpioDev, pin, &gpioParam);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    input = HAL_GPIO_ReadPin(gpioDev, pin);
    HAL_GPIO_DisablePin(gpioDev, pin);

    HAL_WKIO_SetPinMode(wkio, pin, mode);
    HAL_WKIO_SetPinPull(wkio, pin, pull);
    HAL_WKIO_SetWakeEvent(wkio, pin, wakeType);

    return input;
}

/**
 * @brief Set debounce parameters of the specified WKIO port
 * @param WKIO port
 * @param param of debounce:
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_WKIO_SetWakeDebounce(HAL_Dev_t *wkio, uint32_t debounce)
{
    if (wkio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_LPIO_SetPinDelay(debounce);

    return HAL_STATUS_OK;
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
