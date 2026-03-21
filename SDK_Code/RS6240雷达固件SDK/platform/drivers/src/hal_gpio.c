/**
 ******************************************************************************
 * @file       hal_gpio.c
 * @brief      gpio hardware abstract layer define source.
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
#include "ll_gpio.h"
#include "hal_gpio.h"
#include "irq.h"
#include "hal_board.h"
#include "ll_rcc_dev.h"

/* Private defines.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_GPIO_IRQ_PRIO
#define CONFIG_GPIO_IRQ_PRIO                                26
#endif

#ifndef CONFIG_GPIO_PM
#if (CONFIG_PM == 1)
#define CONFIG_GPIO_PM                                      1
#else
#define CONFIG_GPIO_PM                                      0
#endif
#endif

#ifndef CONFIG_GPIO_TO
#define CONFIG_GPIO_TO                                      300
#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

#define PRIV_CFG_MODE               0
#define PRIV_CFG_PULL               1
#define PRIV_CFG_PUSH               2
#define PRIV_CFG_DRIV               3
#define PRIV_CFG_EVENT              4

#define PRIV_CFG_MAX                5

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

typedef struct {
    GPIO_IrqCallback_t  callback;
    void                *arg;
#if (CONFIG_GPIO_PM == 1)
    uint8_t             pinCfg[PRIV_CFG_MAX];
#endif

} Gpio_PrivParam_t;


typedef struct {
    HAL_Dev_t           device;

#if (CONFIG_GPIO_PM == 1)
    uint32_t            portOutput;
    uint32_t            portInterrupt;
#endif
    uint32_t            specFlag;
    uint8_t             pinNum;

    Gpio_PrivParam_t    privParam[0];
} Gpio_Device_t;


/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static void GPIO_IRQHandler(void * param);

#if (CONFIG_GPIO_PM == 1)
static int gpio_pm_action(void *dev, enum pm_device_action action);
#endif

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t * HAL_GPIO_Init(GPIO_Port_t id)
{
    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_GPIO, id);

    if (pGpioDevice == NULL) {
        switch (id) {
#if (CONFIG_SOC_GPIOA == 1)
        case GPIO_PORT_A:
        {
            pGpioDevice = (Gpio_Device_t *)HAL_DEV_MemMalloc(
                sizeof(Gpio_Device_t) + CONFIG_SOC_GPIOA_PIN_NUM * sizeof(Gpio_PrivParam_t));
            if (pGpioDevice == NULL) {
                return NULL;
            }

            pGpioDevice->device.reg         = GPIOA_DEV;
            pGpioDevice->device.irqNum      = GPIOA_IRQn;
            pGpioDevice->device.irqPrio     = CONFIG_GPIO_IRQ_PRIO;
            pGpioDevice->device.irqHandler  = GPIO_IRQHandler;
            pGpioDevice->pinNum             = CONFIG_SOC_GPIOA_PIN_NUM;

#if (CONFIG_GPIO_PM == 1)
            for (uint32_t idx = 0; idx < pGpioDevice->pinNum; idx++) {
                pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_MODE] = (uint8_t)GPIOx_Pn_F15_DIS;
            }
#endif
            break;
        }
#endif

#if (CONFIG_SOC_GPIOB == 1)
        case GPIO_PORT_B:
        {
            pGpioDevice = (Gpio_Device_t *)HAL_DEV_MemMalloc(
                sizeof(Gpio_Device_t) + CONFIG_SOC_GPIOB_PIN_NUM * sizeof(Gpio_PrivParam_t));
            if (pGpioDevice == NULL) {
                return (HAL_Dev_t *)pGpioDevice;
            }

            pGpioDevice->device.reg         = GPIOB_DEV;
            pGpioDevice->device.irqNum      = GPIOB_IRQn;
            pGpioDevice->device.irqPrio     = CONFIG_GPIO_IRQ_PRIO;
            pGpioDevice->device.irqHandler  = GPIO_IRQHandler;
            pGpioDevice->pinNum             = CONFIG_SOC_GPIOB_PIN_NUM;

#if (CONFIG_GPIO_PM == 1)
            for (uint32_t idx = 0; idx < pGpioDevice->pinNum; idx++) {
                pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_MODE] = (uint8_t)GPIOx_Pn_F15_DIS;
            }
#endif

            break;
        }
#endif

        default:
             return NULL;
        }

        LL_RCC_GPIO_SetClockSource(LL_RCC_GPIO_AHB);
        HAL_DEV_Register((HAL_Dev_t *)pGpioDevice, HAL_DEV_TYPE_GPIO, id);

#if (CONFIG_GPIO_PM == 1)
        pm_device_mult_register(&(pGpioDevice->device.pmDev), NULL, gpio_pm_action);
#endif
    }

    return (HAL_Dev_t *)pGpioDevice;
}


void HAL_GPIO_DeInit(HAL_Dev_t *gpio)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if (gpio != NULL) {
        status = HAL_DEV_Unregister(gpio);
        if (status == HAL_STATUS_OK) {
            HAL_DEV_MemFree(gpio);
        }
    }
}


/**
 * @brief Configuration of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin
 * @param param Pointer to GPIO_PinParam_t structure
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_SetPinParam(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_PinParam_t *param)
{
    if ((gpio == NULL) || (param == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_SetPinFuncMode(pGpioDevice->device.reg, pin, param->mode);
    LL_GPIO_SetPinOutputType(pGpioDevice->device.reg, pin, param->push);
    LL_GPIO_SetPinPull(pGpioDevice->device.reg, pin, param->pull);
    LL_GPIO_SetPinStreng(pGpioDevice->device.reg, pin, param->driving);

#if (CONFIG_GPIO_PM == 1)
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_MODE] = (uint8_t)param->mode;
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_PULL] = (uint8_t)param->pull;
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_PUSH] = (uint8_t)param->push;
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_DRIV] = (uint8_t)param->driving;
#endif

    return HAL_STATUS_OK;
}

/**
 * @brief DeConfiguration of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_DisablePin(HAL_Dev_t *gpio, GPIO_Pin_t pin)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_SetPinFuncMode(pGpioDevice->device.reg, pin, GPIOx_Pn_F15_DIS);
    LL_GPIO_SetPinPull(pGpioDevice->device.reg, pin, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinStreng(pGpioDevice->device.reg, pin, LL_GPIO_DRIVING_LVL_0);

#if (CONFIG_GPIO_PM == 1)
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_MODE] = (uint8_t)GPIOx_Pn_F15_DIS;
#endif

    return HAL_STATUS_OK;
}

/**
 * @brief Get the configuration of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin
 * @param param Pointer to GPIO_PinParam_t structure
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_GetPinParam(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_PinParam_t *param)
{
    if ((gpio == NULL) || (param == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    param->mode = LL_GPIO_GetPinFuncMode(pGpioDevice->device.reg, pin);
    param->push = LL_GPIO_GetPinOutputType(pGpioDevice->device.reg, pin);
    param->pull = LL_GPIO_GetPinPull(pGpioDevice->device.reg, pin);
    param->driving = LL_GPIO_GetPinStreng(pGpioDevice->device.reg, pin);


    return HAL_STATUS_OK;
}

/**
 * @brief Set work mode of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin
 * @param GPIO mode
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_SetPinMode(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Mode_t mode)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_SetPinFuncMode(pGpioDevice->device.reg, pin, mode);

#if (CONFIG_GPIO_PM == 1)
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_MODE] = (uint8_t)mode;
#endif

    return HAL_STATUS_OK;
}

/**
 * @brief  Set pull type of the specified GPIO
 * @param  GPIO DEV
 * @param  GPIO pin
 * @param  pull pull type of the specified GPIO:
 *         @arg @ref GPIO_PULL_NONE
 *         @arg @ref GPIO_PULL_UP
 *         @arg @ref GPIO_PULL_DOWN
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_SetPinPull(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Pull_t pull)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_SetPinPull(pGpioDevice->device.reg, pin, pull);

#if (CONFIG_GPIO_PM == 1)
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_PULL] = (uint8_t)pull;
#endif

    return HAL_STATUS_OK;
}

/**
 * @brief  Set push type of the specified GPIO
 * @param  GPIO DEV
 * @param  GPIO pin
 * @param  push type of the specified GPIO:
 *         @arg @ref GPIO_PUSH_PULL
 *         @arg @ref GPIO_OPEN_DRAIN
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_SetPinPush(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Push_t push)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_SetPinOutputType(pGpioDevice->device.reg, pin, push);

#if (CONFIG_GPIO_PM == 1)
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_PUSH] = (uint8_t)push;
#endif

    return HAL_STATUS_OK;
}

/**
 * @brief  Set driving level of the specified GPIO
 * @param  GPIO DEV
 * @param  GPIO pin
 * @param  driving level of the specified GPIO:
 *         @arg @ref GPIO_DRIVING_LEVEL_0
 *         @arg @ref GPIO_DRIVING_LEVEL_1
 *         @arg @ref GPIO_DRIVING_LEVEL_2
 *         @arg @ref GPIO_DRIVING_LEVEL_3
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_SetPinDriving(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Driving_t drinving)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_SetPinStreng(pGpioDevice->device.reg, pin, drinving);

#if (CONFIG_GPIO_PM == 1)
        pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_DRIV] = (uint8_t)drinving;
#endif

    return HAL_STATUS_OK;
}

/**
 * @brief Get the state of the specified GPIO port
 * @param GPIO DEV
 * @return GPIO port state, bit mask of all pins
 */
int32_t HAL_GPIO_ReadPort(HAL_Dev_t *gpio)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    return LL_GPIO_ReadInputPort(pGpioDevice->device.reg);
}

/**
 * @brief Get the state of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin
 * @return GPIO pin state
 */
int32_t HAL_GPIO_ReadPin(HAL_Dev_t *gpio, GPIO_Pin_t pin)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    return (int32_t)HW_GET_MSK_VAL(LL_GPIO_ReadInputPort(pGpioDevice->device.reg), HW_BIT(pin), pin);
}

/**
 * @brief Set the state of the specified GPIO port
 * @param GPIO DEV
 * @param portMask GPIO port state, bit mask of all pins
 * @return HAL_Status_t
 */
HAL_Status_t HAL_GPIO_WritePort(HAL_Dev_t *gpio, uint32_t pinMask)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    LL_GPIO_WriteOutputPort(pGpioDevice->device.reg, pinMask);

    return HAL_STATUS_OK;
}

/**
 * @brief Set the state of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin
 * @param GPIO pin state
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_GPIO_WritePin(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_PinState_t pinState)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (pinState == GPIO_PIN_RESET) {
        LL_GPIO_ResetOutputPin(pGpioDevice->device.reg, HAL_BIT(pin));
    } else {
        LL_GPIO_SetOutputPin(pGpioDevice->device.reg, HAL_BIT(pin));
    }

    return HAL_STATUS_OK;
}

/**
 * @brief  Toggle data value for several pin of dedicated port.
 * @param  GPIO DEV
 * @param  GPIO pin
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_GPIO_TogglePin(HAL_Dev_t *gpio, GPIO_Pin_t pin)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_TogglePin(pGpioDevice->device.reg, HAL_BIT(pin));

    return HAL_STATUS_OK;
}

/**
 * @brief Enable the IRQ of the specified GPIO PIN temporarily.
 * @param GPIO DEV
 * @param  GPIO pin
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_GPIO_EnableIRQ(HAL_Dev_t *gpio, GPIO_Pin_t pin)
{
    uint32_t start_tim = 0;
    uint32_t stop_tim = 0;
    GPIO_IrqEvent_t event = 0;

    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_ClearPinInterruptPending(pGpioDevice->device.reg, HW_BIT(pin));
    start_tim = HAL_BOARD_GetTime(HAL_TIME_US);
    while (LL_GPIO_IsPinInterruptPending(pGpioDevice->device.reg, pin)) {
        stop_tim = HAL_BOARD_GetTime(HAL_TIME_US);
        if (stop_tim - start_tim > CONFIG_GPIO_TO) {
            return HAL_STATUS_ERROR;
        }
    }

    event = LL_GPIO_GetPinInterruptType(pGpioDevice->device.reg, pin);
    if ((event == GPIO_IRQ_EVT_LOW_LEVEL) ||
        (event == GPIO_IRQ_EVT_HIGH_LEVEL)) {
        HAL_SET_BIT(pGpioDevice->specFlag, pin);
    } else {
        HAL_CLR_BIT(pGpioDevice->specFlag, pin);
    }

    if (LL_GPIO_GetPinInterrupt(pGpioDevice->device.reg) == 0) {
        IRQ_AttachDevice(pGpioDevice->device.irqNum, gpio);
        IRQ_Priority(pGpioDevice->device.irqNum, pGpioDevice->device.irqPrio);
        IRQ_Enable(pGpioDevice->device.irqNum);
    }
    LL_GPIO_EnablePinInterrupt(pGpioDevice->device.reg, pin);

    return HAL_STATUS_OK;
}

/**
 * @brief Disable the IRQ of the specified GPIO temporarily and whether clear pin interrupt pending of dedicated port.
 * @param GPIO DEV
 * @param GPIO pin
 * @param isClearPending:
 *     @arg !0 Clear IRQ pending
 *     @arg  0 Not clear IRQ pending
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_GPIO_DisableIRQ(HAL_Dev_t *gpio, GPIO_Pin_t pin, int8_t ClearPending)
{
    uint32_t start_tim = 0;
    uint32_t stop_tim = 0;

    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_DisablePinInterrupt(pGpioDevice->device.reg, pin);

    if (ClearPending != 0) {
        LL_GPIO_ClearPinInterruptPending(pGpioDevice->device.reg, pin);

        start_tim = HAL_BOARD_GetTime(HAL_TIME_US);
        while (LL_GPIO_IsPinInterruptPending(pGpioDevice->device.reg, pin)) {
            stop_tim = HAL_BOARD_GetTime(HAL_TIME_US);
            if (stop_tim - start_tim > CONFIG_GPIO_TO) {
                return HAL_STATUS_ERROR;
            }
        }
    }

    if (LL_GPIO_GetPinInterrupt(pGpioDevice->device.reg) == 0) {
        IRQ_Disable(pGpioDevice->device.irqNum);
    }

    return HAL_STATUS_OK;
}

/**
 * @brief Init the IRQ of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin
 * @param param Pointer to GPIO_IrqParam_t structure
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_GPIO_SetIRQParam(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_IrqParam_t *param)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    if (pin > pGpioDevice->pinNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    pGpioDevice->privParam[pin].callback = param->callback;
    pGpioDevice->privParam[pin].arg = param->arg;

    if ((param->event == GPIO_IRQ_EVT_LOW_LEVEL) ||
        (param->event == GPIO_IRQ_EVT_HIGH_LEVEL)) {
        HAL_SET_BIT(pGpioDevice->specFlag, pin);
    } else {
        HAL_CLR_BIT(pGpioDevice->specFlag, pin);
    }

    LL_GPIO_SetPinInterruptType(pGpioDevice->device.reg, pin, param->event);

#if (CONFIG_GPIO_PM == 1)
    pGpioDevice->privParam[pin].pinCfg[PRIV_CFG_EVENT] = (uint8_t)param->event;
#endif

    return HAL_STATUS_OK;
}


/**
 * @brief Set IRQ trigger event of the specified GPIO
 * @param GPIO DEV
 * @param GPIO pin number
 * @param event IRQ trigger event of the specified GPIO
 * @retval HAL_Status_t
 */
HAL_Status_t HAL_GPIO_SetIRQEvent(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_IrqEvent_t event)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    LL_GPIO_SetPinInterruptType(pGpioDevice->device.reg, pin, event);

    return HAL_STATUS_OK;
}

/**
 * @brief GPIO interrupt handing function
 * @param GPIO port
 * @retval None
 */
static void GPIO_IRQHandler(void * param)
{
    uint32_t start_tim = 0;
    uint32_t stop_tim = 0;

    if (param == NULL) {
        return;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)param;
    GPIO_IrqCallback_t  callback = NULL;

    for (int i = 0; i < pGpioDevice->pinNum; ++i) {
        if (LL_GPIO_IsPinInterruptPending(pGpioDevice->device.reg, HW_BIT(i))) {
            LL_GPIO_ClearPinInterruptPending(pGpioDevice->device.reg, HW_BIT(i));

            start_tim = HAL_BOARD_GetTime(HAL_TIME_US);
            while (LL_GPIO_IsPinInterruptPending(pGpioDevice->device.reg, HW_BIT(i))) {
                stop_tim = HAL_BOARD_GetTime(HAL_TIME_US);
                if (stop_tim - start_tim > CONFIG_GPIO_TO) {
                    break;
                }
            }

            if (!(HAL_GET_BIT(pGpioDevice->specFlag, i))) {
                callback = pGpioDevice->privParam[i].callback;
                if (callback != NULL) {
                    callback(pGpioDevice->privParam[i].arg);
                }
            } else {
                HAL_CLR_BIT(pGpioDevice->specFlag, i);
            }
        }
    }
}

/**
 * @brief Set debounce parameters of the specified GPIO port
 * @param GPIO DEV
 * @param param of debounce:
 * @retval HAL_Status_t
 * @note the default value is 0x10 , it will reset after LL_GPIO_EnablePinInterrupt.
 */
HAL_Status_t HAL_GPIO_SetIRQDebounce(HAL_Dev_t *gpio, uint32_t debounce)
{
    if (gpio == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)gpio;

    LL_GPIO_SetPinDelay(pGpioDevice->device.reg, debounce);

    return HAL_STATUS_OK;
}


#if (CONFIG_GPIO_PM == 1)
__keep_sram_text static int gpio_pm_action(void *dev, enum pm_device_action action)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if (dev == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Gpio_Device_t *pGpioDevice = (Gpio_Device_t *)dev;

    if (action == PM_DEVICE_ACTION_SUSPEND) {
        pGpioDevice->portOutput = LL_GPIO_ReadOutputPort(pGpioDevice->device.reg);
        pGpioDevice->portInterrupt = LL_GPIO_GetPinInterrupt(pGpioDevice->device.reg);
    } else if (action == PM_DEVICE_ACTION_RESUME) {
        for (uint32_t idx = 0; idx < pGpioDevice->pinNum; idx++) {
            uint8_t mode = pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_MODE];
            if ((mode == GPIOx_Pn_F0_INPUT) ||
                (mode == GPIOx_Pn_F1_OUTPUT) ||
                (mode == GPIOx_Pn_F14_EINT)) {
                LL_GPIO_SetPinFuncMode(pGpioDevice->device.reg, idx, pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_MODE]);
                LL_GPIO_SetPinOutputType(pGpioDevice->device.reg, idx,
                                    pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_PUSH]);
                LL_GPIO_SetPinPull(pGpioDevice->device.reg, idx, pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_PULL]);
                LL_GPIO_SetPinStreng(pGpioDevice->device.reg, idx, pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_DRIV]);

                if (mode == GPIOx_Pn_F14_EINT) {
                    uint8_t event = pGpioDevice->privParam[idx].pinCfg[PRIV_CFG_EVENT];
                    if ((event == GPIO_IRQ_EVT_LOW_LEVEL) ||
                        (event == GPIO_IRQ_EVT_HIGH_LEVEL)) {
                        HAL_SET_BIT(pGpioDevice->specFlag, idx);
                    } else {
                        HAL_CLR_BIT(pGpioDevice->specFlag, idx);
                    }

                    LL_GPIO_SetPinInterruptType(pGpioDevice->device.reg, idx, event);
                }
            }

            LL_GPIO_WriteOutputPort(pGpioDevice->device.reg, pGpioDevice->portOutput);
            LL_GPIO_SetPinInterrupt(pGpioDevice->device.reg, pGpioDevice->portInterrupt);
        }
    }

    return status;
}
#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

