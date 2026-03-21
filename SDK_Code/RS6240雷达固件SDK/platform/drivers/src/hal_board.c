/**
 ******************************************************************************
 * @file    hal_board.c
 * @brief   hal board define.
 * @verbatim    null
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
#include "hal_gpio.h"
#include "hal_clock.h"

#include "hal_board.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */
static HAL_GetFreqHandle_t boardHandle_GetFreq = NULL;
static HAL_GetTimeHandle_t boardHandle_GetTime = NULL;
static HAL_GetPinMuxHandle_t boardHandle_GetPinMux = NULL;
static HAL_SetPinMuxHandle_t boardHandle_SetPinMux = NULL;
static HAL_BlkDelayHandle_t boardHandle_BlkDelay = NULL;
static HAL_ResetHandle_t boardHandle_Reset = NULL;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Status_t HAL_BOARD_SetPinMux(HAL_Pinmux_t * pConfig)
{
    if (pConfig == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (boardHandle_SetPinMux != NULL) {
        return boardHandle_SetPinMux(pConfig);
    } else {
        return HAL_STATUS_INVALID_STATE;
    }
}

HAL_Status_t HAL_BOARD_GetPinMux(HAL_DevType_t type,
                                                uint8_t idx,
                                                HAL_PinMuxType_t cfgType,
                                                HAL_Pinmux_t * pPinMux)
{
    if (boardHandle_GetPinMux != NULL) {
        return boardHandle_GetPinMux(type, idx, cfgType, pPinMux);
    } else {
        return HAL_STATUS_INVALID_STATE;
    }
}

int32_t HAL_BOARD_GetFreq(CLOCK_Type_t clkType)
{
    if (boardHandle_GetFreq != NULL) {
        return boardHandle_GetFreq(clkType);
    } else {
        return 0;
    }
}

uint32_t HAL_BOARD_GetTime(HAL_TimeType_t timeType)
{
    if (boardHandle_GetTime != NULL) {
        return boardHandle_GetTime(timeType);
    } else {
        return 0;
    }
}

void HAL_BOARD_BlkDelay(uint32_t delayVal, HAL_TimeType_t timeType)
{
    if (boardHandle_BlkDelay != NULL) {
        boardHandle_BlkDelay(delayVal, timeType);
    }
}

uint32_t HAL_BOARD_GetRand(void)
{
    static uint32_t state = 123456789UL;  /* initial seed value */
    uint32_t * tmpTime = (uint32_t *)__TIME__;
    uint32_t * tmpDate = (uint32_t *)__DATE__;
    uint32_t cycle = 0;

    if (boardHandle_GetTime != NULL) {
        cycle = boardHandle_GetTime(HAL_TIME_CYCLE);
    }

    state = state + cycle;
//    state = state * 2862933555777941757UL + 3037000493UL;
    state = state * tmpTime[0] + tmpDate[0];
    uint32_t val = (uint32_t)state;

    return val;
}

void HAL_BOARD_Reset(HAL_ResetType_t resetType)
{
    if (boardHandle_Reset != NULL) {
        boardHandle_Reset(resetType);
    }
}

HAL_Status_t HAL_BOARD_RegisterHandle(HAL_BoardHandleType_t handleType, void *handle)
{
    if (handle == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    switch (handleType) {
    case HAL_BOARD_HANDLE_GET_FREQ:
        boardHandle_GetFreq = handle;
        break;

    case HAL_BOARD_HANDLE_GET_TIME:
        boardHandle_GetTime = handle;
        break;

    case HAL_BOARD_HANDLE_GET_PINMUX:
        boardHandle_GetPinMux = handle;
        break;

    case HAL_BOARD_HANDLE_SET_PINMUX:
        boardHandle_SetPinMux = handle;
        break;

    case HAL_BOARD_HANDLE_BLK_DELAY:
        boardHandle_BlkDelay = handle;
        break;

    case HAL_BOARD_HANDLE_RESET:
        boardHandle_Reset = handle;
        break;

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }


    return HAL_STATUS_OK;
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

