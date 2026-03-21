/**
 ******************************************************************************
 * @file       hal_gpio.h
 * @brief      GPIO hadware abstract layer define
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

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_


/* Includes ------------------------------------------------------------------*/

#include "hal_types.h"
#include "hal_dev.h"


/**
 * @brief GPIO port definition
 */
typedef enum {
#if (CONFIG_SOC_GPIOA == 1)
    GPIO_PORT_A                     = 0U,
#endif

#if (CONFIG_SOC_GPIOB == 1)
    GPIO_PORT_B                     = 1U,
#endif

#if (CONFIG_SOC_GPIOC == 1)
    GPIO_PORT_C                     = 2U,
#endif

    GPIO_PORT_NUM,
} GPIO_Port_t;


/**
 * @brief GPIO pin number definition
 */
typedef enum {
    GPIO_PIN_0  = 0U,
    GPIO_PIN_1  = 1U,
    GPIO_PIN_2  = 2U,
    GPIO_PIN_3  = 3U,
    GPIO_PIN_4  = 4U,
    GPIO_PIN_5  = 5U,
    GPIO_PIN_6  = 6U,
    GPIO_PIN_7  = 7U,
    GPIO_PIN_8  = 8U,
    GPIO_PIN_9  = 9U,
    GPIO_PIN_10 = 10U,
    GPIO_PIN_11 = 11U,
    GPIO_PIN_12 = 12U,
    GPIO_PIN_13 = 13U,
    GPIO_PIN_14 = 14U,
    GPIO_PIN_15 = 15U,
    GPIO_PIN_NUM
} GPIO_Pin_t;


/**
  * @brief  GPIO Bit SET and Bit RESET enumeration
  */
typedef enum
{
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET
} GPIO_PinState_t;


/**
 * @brief GPIO out driving level definition
 */
typedef enum {
    GPIO_DRIVING_LEVEL_0 = 0U,      /* 20mA(3.3V) / 6mA(1.8V)  */
    GPIO_DRIVING_LEVEL_1 = 1U,      /* 40mA(3.3V) / 12mA(1.8V) */
    GPIO_DRIVING_LEVEL_2 = 2U,      /* 60mA(3.3V) / 18mA(1.8V) */
    GPIO_DRIVING_LEVEL_3 = 3U       /* 80mA(3.3V) / 24mA(1.8V) */
} GPIO_Driving_t;


/**
 * @brief GPIO out push definition
 */
typedef enum {
    GPIO_PUSH_PULL      = 0U,
    GPIO_OPEN_DRAIN     = 1U
} GPIO_Push_t;


/**
 * @brief GPIO in pull type definition
 */
typedef enum {
    GPIO_PULL_FLOATING  = 0U,
    GPIO_PULL_UP        = 1U,   /* 35Kohm ~ 75Kohm (3.3V) / 70K ohm ~150K ohm (1.8V) */
    GPIO_PULL_DOWN      = 2U    /* 30Kohm ~ 75Kohm (3.3V) / 60K ohm ~150K ohm (1.8V) */
} GPIO_Pull_t;


/**
 * @brief GPIO working mode (function) definition
 */
typedef enum {
    GPIOx_Pn_F0_INPUT               = 0U,   /* for all pins */
    GPIOx_Pn_F1_OUTPUT              = 1U,   /* for all pins */
    GPIOx_Pn_F14_EINT               = 14U,  /* for all pins */
    GPIOx_Pn_F15_DIS                = 15U,  /* for all pins */

    GPIOA_P0_F2_SPI_CS              = 2U,
    GPIOA_P0_F3_UART1_TX            = 3U,
    GPIOA_P0_F4_CANFD_TX            = 4U,
    GPIOA_P0_F5_F_SWD_TMS           = 5U,
    GPIOA_P0_F6_PWM0                = 6U,
    GPIOA_P0_F8_DBG0                = 8U,
    GPIOA_P0_F9_MCO_CLK             = 9U,

    GPIOA_P1_F2_SPI_CLK             = 2U,
    GPIOA_P1_F3_UART1_RX            = 3U,
    GPIOA_P1_F4_CANFD_RX            = 4U,
    GPIOA_P1_F5_F_SWD_TCK           = 5U,
    GPIOA_P1_F6_PWM1                = 6U,
    GPIOA_P1_F8_DBG1                = 8U,

    GPIOA_P2_F2_SPI_MOSI            = 2U,
    GPIOA_P2_F3_UART1_RTS           = 3U,
    GPIOA_P2_F4_TWI0_SDA            = 4U,
    GPIOA_P2_F5_UART2_TX            = 5U,
    GPIOA_P2_F6_PWM2                = 6U,
    GPIOA_P2_F8_DBG2                = 8U,
    GPIOA_P2_F9_ADC_CLK             = 9U,

    GPIOA_P3_F2_SPI_MISO            = 2U,
    GPIOA_P3_F3_UART1_CTS           = 3U,
    GPIOA_P3_F4_TWI0_SCL            = 4U,
    GPIOA_P3_F5_UART2_RX            = 5U,
    GPIOA_P3_F6_PWM3                = 6U,
    GPIOA_P3_F8_DBG3                = 8U,
    GPIOA_P3_F9_ADC_VALID           = 9U,

    GPIOA_P4_F2_SPI_HOLD            = 2U,
    GPIOA_P4_F3_TWI0_SDA            = 3U,
    GPIOA_P4_F4_REQ_IRQ             = 4U,
    GPIOA_P4_F5_CANFD_TX            = 5U,
    GPIOA_P4_F6_PWM4                = 6U,
    GPIOA_P4_F7_PSRAM_CS            = 7U,
    GPIOA_P4_F8_DBG4                = 8U,
    GPIOA_P4_F9_ADC11               = 9U,

    GPIOA_P5_F2_SPI_WP              = 2U,
    GPIOA_P5_F3_TWI0_SCL            = 3U,
    GPIOA_P5_F4_PGOOD               = 4U,
    GPIOA_P5_F5_CANFD_TX            = 5U,
    GPIOA_P5_F6_PWM5                = 6U,
    GPIOA_P5_F8_DBG5                = 8U,
    GPIOA_P5_F9_ADC10               = 9U,

    GPIOA_P6_F2_UART1_RTS           = 2U,
    GPIOA_P6_F3_UART2_TX            = 3U,
    GPIOA_P6_F4_TWI1_SDA            = 4U,
    GPIOA_P6_F6_PWM6                = 6U,
    GPIOA_P6_F6_GPADC6              = 7U,
    GPIOA_P6_F8_DBG6                = 8U,
    GPIOA_P6_F9_ADC9                = 9U,

    GPIOA_P7_F2_UART1_CTS           = 2U,
    GPIOA_P7_F3_UART2_RX            = 3U,
    GPIOA_P7_F4_TWI1_SCL            = 4U,
    GPIOA_P7_F5_PSRAM_CS            = 5U,
    GPIOA_P7_F6_PWM7                = 6U,
    GPIOA_P7_F6_GPADC7              = 7U,
    GPIOA_P7_F8_DBG7                = 8U,
    GPIOA_P7_F9_ADC8                = 9U,

    GPIOA_P8_F2_UART1_TX            = 2U,
    GPIOA_P8_F3_TWI1_SCL            = 3U,
    GPIOA_P8_F5_REQ_IRQ             = 5U,
    GPIOA_P8_F6_PWM0                = 6U,
    GPIOA_P8_F6_GPADC0              = 7U,
    GPIOA_P8_F8_DBG8                = 8U,
    GPIOA_P8_F9_ADC7                = 9U,

    GPIOA_P9_F2_UART1_RX            = 2U,
    GPIOA_P9_F3_TWI1_SDA            = 3U,
    GPIOA_P9_F5_PGOOD               = 5U,
    GPIOA_P9_F6_PWM1                = 6U,
    GPIOA_P9_F6_GPADC1              = 7U,
    GPIOA_P9_F8_DBG9                = 8U,
    GPIOA_P9_F9_ADC6                = 9U,

    GPIOA_P10_F2_TWI1_SDA           = 2U,
    GPIOA_P10_F3_CANFD_TX           = 3U,
    GPIOA_P10_F4_S_SWD_TMS          = 4U,
    GPIOA_P10_F5_PGOOD              = 5U,
    GPIOA_P10_F6_PWM2               = 6U,
    GPIOA_P10_F6_GPADC2             = 7U,
    GPIOA_P10_F8_DBG10              = 8U,
    GPIOA_P10_F9_ADC5               = 9U,

    GPIOA_P11_F2_TWI1_SCL           = 2U,
    GPIOA_P11_F3_CANFD_RX           = 3U,
    GPIOA_P11_F4_S_SWD_TCK          = 4U,
    GPIOA_P11_F5_REQ_IRQ            = 5U,
    GPIOA_P11_F6_PWM3               = 6U,
    GPIOA_P11_F6_GPADC3             = 7U,
    GPIOA_P11_F8_DBG11              = 8U,
    GPIOA_P11_F9_ADC4               = 9U,

    GPIOA_P12_F2_UART2_RX           = 2U,
    GPIOA_P12_F3_UART1_RTS          = 3U,
    GPIOA_P12_F4_CANFD_TX           = 4U,
    GPIOA_P12_F5_PGOOD              = 5U,
    GPIOA_P12_F6_PWM4               = 6U,
    GPIOA_P12_F6_GPADC4             = 7U,
    GPIOA_P12_F8_DBG12              = 8U,
    GPIOA_P12_F9_ADC3               = 9U,

    GPIOA_P13_F2_UART2_TX           = 2U,
    GPIOA_P13_F3_MCO_CLK            = 3U,
    GPIOA_P13_F4_CANFD_RX           = 4U,
    GPIOA_P13_F5_REQ_IRQ            = 5U,
    GPIOA_P13_F6_PWM5               = 6U,
    GPIOA_P13_F6_GPADC5             = 7U,
    GPIOA_P13_F8_DBG13              = 8U,
    GPIOA_P13_F9_ADC2               = 9U,

    GPIOA_P14_F2_TWI0_SDA           = 2U,
    GPIOA_P14_F3_UART2_RX           = 3U,
    GPIOA_P14_F4_S_SWD_TMS          = 4U,
    GPIOA_P14_F5_CANFD_TX           = 5U,
    GPIOA_P14_F6_PWM6               = 6U,
    GPIOA_P14_F6_GPADC6             = 7U,
    GPIOA_P14_F8_DBG14              = 8U,
    GPIOA_P14_F9_ADC1               = 9U,

    GPIOA_P15_F2_TWI0_SCL           = 2U,
    GPIOA_P15_F3_UART2_TX           = 3U,
    GPIOA_P15_F4_S_SWD_TCK          = 4U,
    GPIOA_P15_F5_CANFD_RX           = 5U,
    GPIOA_P15_F6_PWM7               = 6U,
    GPIOA_P15_F6_GPADC7             = 7U,
    GPIOA_P15_F8_DBG15              = 8U,
    GPIOA_P15_F9_ADC0               = 9U,

    GPIOB_P0_F2_UART0_TX            = 2U,
    GPIOB_P0_F3_S_SWD_TMS           = 3U,
    GPIOB_P0_F5_F_SWD_TMS           = 5U,
    GPIOB_P0_F6_PWM0                = 6U,
    GPIOB_P0_F8_S_TDI               = 8U,
    GPIOB_P0_F9_F_TDI               = 9U,

    GPIOB_P1_F2_UART0_RX            = 2U,
    GPIOB_P1_F3_S_SWD_TCK           = 3U,
    GPIOB_P1_F5_F_SWD_TCK           = 5U,
    GPIOB_P1_F6_PWM1                = 6U,
    GPIOB_P1_F8_S_TDO               = 8U,
    GPIOB_P1_F9_F_TDO               = 9U,

    GPIOB_P2_F2_UART0_RTS           = 2U,
    GPIOB_P2_F3_FC_WP               = 3U,
    GPIOB_P2_F4_SPI_WP              = 4U,
    GPIOB_P2_F5_ADC0                = 5U,
    GPIOB_P2_F6_PWM2                = 6U,
    GPIOB_P2_F8_S_TMS               = 8U,
    GPIOB_P2_F9_F_TMS               = 9U,

    GPIOB_P3_F2_UART0_CTS           = 2U,
    GPIOB_P3_F3_FC_HOLD             = 3U,
    GPIOB_P3_F4_SPI_HOLD            = 4U,
    GPIOB_P3_F5_ADC1                = 5U,
    GPIOB_P3_F6_PWM3                = 6U,
    GPIOB_P3_F8_S_TCK               = 8U,
    GPIOB_P3_F9_F_TCK               = 9U,

    GPIOB_P4_F2_FC_CS               = 2U,
    GPIOB_P4_F3_UART2_RX            = 3U,
    GPIOB_P4_F4_SPI_CS              = 4U,
    GPIOB_P4_F5_CANFD_TX            = 5U,
    GPIOB_P4_F6_PWM4                = 6U,
    GPIOB_P4_F7_PSRAM_CS            = 7U,
    GPIOB_P4_F8_UART1_RTS           = 8U,
    GPIOB_P4_F9_TWI1_SCL            = 9U,

    GPIOB_P5_F2_FC_CLK              = 2U,
    GPIOB_P5_F3_UART2_TX            = 3U,
    GPIOB_P5_F4_SPI_CLK             = 4U,
    GPIOB_P5_F5_CANFD_RX            = 5U,
    GPIOB_P5_F6_PWM5                = 6U,
    GPIOB_P5_F8_UART1_CTS           = 8U,
    GPIOB_P5_F9_TWI1_SDA            = 9U,

    GPIOB_P6_F2_FC_MOSI             = 2U,
    GPIOB_P6_F3_TWI0_SDA            = 3U,
    GPIOB_P6_F4_SPI_MOSI            = 4U,
    GPIOB_P6_F5_DBG14               = 5U,
    GPIOB_P6_F6_PWM6                = 6U,
    GPIOB_P6_F8_UART1_TX            = 8U,

    GPIOB_P7_F2_FC_MIS0             = 2U,
    GPIOB_P7_F3_TWI0_SCL            = 3U,
    GPIOB_P7_F4_SPI_MISO            = 4U,
    GPIOB_P7_F5_DBG15               = 5U,
    GPIOB_P7_F6_PWM7                = 6U,
    GPIOB_P7_F8_UART1_RX            = 8U,
} GPIO_Mode_t;


/**
 * @brief GPIO initialization parameters
 */
typedef struct {
    GPIO_Mode_t     mode;
    GPIO_Driving_t  driving;
    GPIO_Push_t     push;
    GPIO_Pull_t     pull;
} GPIO_PinParam_t;


/**
 * @brief GPIO interrupt trigger event definition
 */
typedef enum {
    GPIO_IRQ_EVT_RISING_EDGE  = 0U,
    GPIO_IRQ_EVT_FALLING_EDGE = 1U,
    GPIO_IRQ_EVT_BOTH_EDGE    = 2U,
    GPIO_IRQ_EVT_HIGH_LEVEL   = 3U,
    GPIO_IRQ_EVT_LOW_LEVEL    = 4U,
} GPIO_IrqEvent_t;


/** @brief Type define of GPIO IRQ callback function */
typedef void (* GPIO_IrqCallback_t)( void *arg);


/**
* @brief GPIO interrupt enable parameters
*/
typedef struct {
    GPIO_IrqEvent_t     event;
    GPIO_IrqCallback_t  callback;
    void                *arg;
} GPIO_IrqParam_t;

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
/**
 * @brief  Initialize the GPIO peripheral.
 * @param id  GPIO device id. @ref GPIO_Port_t
 * @return  GPIO device handler pointer
 */
HAL_Dev_t * HAL_GPIO_Init(GPIO_Port_t id);

/**
 * @brief  DeInitialize the GPIO peripheral.
 * @param gpio  GPIO device handler pointer
 * @return  NONE.
 */
void HAL_GPIO_DeInit(HAL_Dev_t *gpio);

/**
 * @brief  Set pin param.
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetPinParam(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_PinParam_t *param);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_GetPinParam(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_PinParam_t *param);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_DisablePin(HAL_Dev_t *gpio, GPIO_Pin_t pin);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetPinMode(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Mode_t mode);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetPinPull(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Pull_t pull);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetPinPush(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Push_t push);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetPinDriving(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_Driving_t drinving);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  .
 */
int32_t  HAL_GPIO_ReadPort(HAL_Dev_t *gpio);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  .
 */
int32_t HAL_GPIO_ReadPin(HAL_Dev_t *gpio, GPIO_Pin_t pin);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_WritePort(HAL_Dev_t *gpio,  uint32_t pinMask);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_WritePin(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_PinState_t PinState);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_TogglePin(HAL_Dev_t *gpio, GPIO_Pin_t pin);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetIRQParam(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_IrqParam_t *param);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetIRQEvent(HAL_Dev_t *gpio, GPIO_Pin_t pin, GPIO_IrqEvent_t event);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_SetIRQDebounce(HAL_Dev_t *gpio, uint32_t debounce);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_EnableIRQ(HAL_Dev_t *gpio, GPIO_Pin_t pin);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_GPIO_DisableIRQ(HAL_Dev_t *gpio, GPIO_Pin_t pin, int8_t ClearPending);


#endif /* _HAL_GPIO_H_ */
/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */

