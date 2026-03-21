/**
 ******************************************************************************
 * @file       hal_wkio.h
 * @brief      LPIO hadware abstract layer define
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

#ifndef _HAL_WKIO_H_
#define _HAL_WKIO_H_

/* Includes ------------------------------------------------------------------*/
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_gpio.h"


/**
 * @brief WKIO port definition
 */
typedef enum {
#if (CONFIG_SOC_WKIOA == 1)
    WKIO_PORT_A   = 0U,
#endif

#if (CONFIG_SOC_WKIOB == 1)
    WKIO_PORT_B   = 1U,
#endif

    WKIO_PORT_NUM
} WKIO_Port_t;


/**
 * @brief WKIO pin number definition
 */
typedef enum {
    WKIO_PIN_0  = 0U,
    WKIO_PIN_1  = 1U,
    WKIO_PIN_2  = 2U,
    WKIO_PIN_3  = 3U,
    WKIO_PIN_4  = 4U,
    WKIO_PIN_5  = 5U,
    WKIO_PIN_6  = 6U,
    WKIO_PIN_7  = 7U,
    WKIO_PIN_8  = 8U,
    WKIO_PIN_9  = 9U,
    WKIO_PIN_10 = 10U,
    WKIO_PIN_11 = 11U,
    WKIO_PIN_12 = 12U,
    WKIO_PIN_13 = 13U,
    WKIO_PIN_14 = 14U,
    WKIO_PIN_15 = 15U,
    WKIO_PIN_16 = 16U,
    WKIO_PIN_17 = 17U,
    WKIO_PIN_NUM
} WKIO_Pin_t;


/**
  * @brief  WKIO Bit SET and Bit RESET enumeration
  */
typedef enum
{
  WKIO_PIN_RESET = 0U,
  WKIO_PIN_SET
} WKIO_PinState_t;


/**
 * @brief WKIO in pull type definition
 */
typedef enum
{
  HAL_WKIO_PULL_FLOATING    = 0U,
  HAL_WKIO_PULL_UP          = 1U,   /* 35Kohm ~ 75Kohm (3.3V) / 70K ohm ~150K ohm (1.8V) */
  HAL_WKIO_PULL_DOWN        = 2U    /* 30Kohm ~ 75Kohm (3.3V) / 60K ohm ~150K ohm (1.8V) */
} WKIO_PinPull_t;

/**
 * @brief WKIO working mode (function) definition
 */
typedef enum {
  HAL_WKIO_MODE_GPIO        = 0U,   /* gpio */
  HAL_WKIO_MODE_HOLD        = 1U,   /* output */
  HAL_WKIO_MODE_WAKE        = 2U,   /* input wake-up */
  HAL_WKIO_MODE_DISA        = 3U
} WKIO_PinMode_t;


/**
  * @brief WKIO interrupt trigger event definition
  */
typedef enum {
  WKIO_WAKE_EVT_RISING_EDGE  = 0U,
  WKIO_WAKE_EVT_FALLING_EDGE = 1U,
  WKIO_WAKE_EVT_BOTH_EDGE    = 2U,
  WKIO_WAKE_EVT_HIGH_LEVEL   = 3U,
  WKIO_WAKE_EVT_LOW_LEVEL    = 4U,
} WKIO_WakeEvent_t;


/** @brief Type define of WKIO IRQ callback function */
typedef void (* WKIO_WakeCallback_t)( void *arg);

/**
  * @brief WKIO interrupt enable parameters
  */
typedef struct {
  WKIO_WakeEvent_t      event;
  WKIO_PinPull_t        pull;
  WKIO_WakeCallback_t   callback;
  void                  *arg;
} WKIO_WakeParam_t;


/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief  Initialize the WKIO peripheral.
 * @param id  WKIO device id. @ref WKIO_Port_t
 * @return  WKIO device handler pointer
 */
HAL_Dev_t *HAL_WKIO_Init(WKIO_Port_t id);

/**
 * @brief  DeInitialize the WKIO peripheral.
 * @param gpio  WKIO device handler pointer
 * @return  NONE.
 */
void HAL_WKIO_DeInit(HAL_Dev_t *wkio);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_SetPinMode(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinMode_t mode);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_GetPinMode(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinMode_t *mode);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_SetPinPull(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinPull_t pull);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_GetPinPull(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinPull_t *pull);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_WritePin(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_PinState_t pinState);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_WritePort(HAL_Dev_t *wkio, uint32_t pinMask);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_SetWakeParam(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_WakeParam_t *param);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_EnableWake(HAL_Dev_t *wkio);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_DisableWake(HAL_Dev_t *wkio);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_SetWakeEvent(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_WakeEvent_t wakeEvent);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_GetWakeEvent(HAL_Dev_t *wkio, WKIO_Pin_t pin, WKIO_WakeEvent_t *wakeEvent);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WKIO_SetWakeDebounce(HAL_Dev_t *wkio, uint32_t debounce);

/**
 * @brief  .
 * @param gpio  GPIO device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
int32_t HAL_WKIO_ReadWakePin(HAL_Dev_t *wkio, WKIO_Pin_t pin);


#endif /* _HAL_WKIO_H_ */
/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
