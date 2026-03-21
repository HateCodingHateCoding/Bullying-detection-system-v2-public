/**
 ******************************************************************************
 * @file    hal_board.h
 * @brief   hal board define.
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

#ifndef _HAL_BOARD_H
#define _HAL_BOARD_H

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_gpio.h"
#include "hal_clock.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */

typedef struct {
    GPIO_Port_t       port;
    GPIO_Pin_t        pin;
    GPIO_PinParam_t  config;
} HAL_PinmuxConfig_t;

typedef struct {
    HAL_PinmuxConfig_t *    pinConfig;
    uint8_t                 pinCnt;
} HAL_Pinmux_t;

#define DEFINE_PINMUX(name, count, ...)                         \
    const HAL_PinmuxConfig_t pin_##name[count] = {__VA_ARGS__}; \
    const HAL_Pinmux_t pinmux_##name = {                        \
        .pinConfig = (HAL_PinmuxConfig_t *)pin_##name,          \
        .pinCnt = count,                                        \
    };

#define DEFINE_PINMUX_WEAK(name, count, ...)                   \
__attribute__((weak))const HAL_PinmuxConfig_t pin_##name[count] = {__VA_ARGS__}; \
__attribute__((weak))const HAL_Pinmux_t pinmux_##name = {                        \
    .pinConfig = (HAL_PinmuxConfig_t *)pin_##name,                               \
    .pinCnt = count,                                                             \
};

typedef enum {
    HAL_PINMUX_TYPE_INIT                = 0,
    HAL_PINMUX_TYPE_DEINIT              = 1,
    HAL_PINMUX_TYPE_SPI_CS              = 2,
    HAL_PINMUX_TYPE_SPI_CS_DEINIT       = 3,
} HAL_PinMuxType_t;

typedef enum {
    HAL_TIME_CYCLE              = 0,
    HAL_TIME_US                 = 1,
    HAL_TIME_MS                 = 2,
    HAL_TIME_S                  = 3,
} HAL_TimeType_t;

typedef enum {
    HAL_RESET_CPU               = 0,
    HAL_RESET_SYS               = 1,
} HAL_ResetType_t;

typedef int32_t (* HAL_GetFreqHandle_t)(CLOCK_Type_t clkType);
typedef uint32_t (* HAL_GetTimeHandle_t)(HAL_TimeType_t timeType);
typedef HAL_Status_t (* HAL_GetPinMuxHandle_t)(
                            HAL_DevType_t type,
                            uint8_t idx,
                            HAL_PinMuxType_t cfgType,
                            HAL_Pinmux_t * pPinMux);
typedef HAL_Status_t (* HAL_SetPinMuxHandle_t)(
                            HAL_Pinmux_t * pConfig);
typedef void (* HAL_BlkDelayHandle_t)(
                            uint32_t delayVal,
                            HAL_TimeType_t timeType);

typedef void (* HAL_ResetHandle_t)(
                            HAL_ResetType_t resetType);

typedef enum {
    HAL_BOARD_HANDLE_GET_FREQ           = 0,
    HAL_BOARD_HANDLE_GET_TIME           = 1,
    HAL_BOARD_HANDLE_GET_PINMUX         = 2,
    HAL_BOARD_HANDLE_SET_PINMUX         = 3,
    HAL_BOARD_HANDLE_BLK_DELAY          = 4,
    HAL_BOARD_HANDLE_RESET              = 5,
} HAL_BoardHandleType_t;

/* Exported constants.
 * ----------------------------------------------------------------------------
 */


/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Status_t HAL_BOARD_SetPinMux(HAL_Pinmux_t * pConfig);

HAL_Status_t HAL_BOARD_GetPinMux(HAL_DevType_t type,
                uint8_t idx,
                HAL_PinMuxType_t cfgType,
                HAL_Pinmux_t * pPinMux);

int32_t HAL_BOARD_GetFreq(CLOCK_Type_t clkType);

uint32_t HAL_BOARD_GetTime(HAL_TimeType_t timeType);

void HAL_BOARD_BlkDelay(uint32_t delayVal, HAL_TimeType_t timeType);

uint32_t HAL_BOARD_GetRand(void);

void HAL_BOARD_Reset(HAL_ResetType_t resetType);

HAL_Status_t HAL_BOARD_RegisterHandle(
                HAL_BoardHandleType_t handleType,
                void *handle);

#ifdef __cplusplus
}
#endif

#endif  /* _HAL_BOARD_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
