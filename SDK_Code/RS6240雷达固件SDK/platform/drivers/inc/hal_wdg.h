/**
 ******************************************************************************
 * @file    hal_wdg.h
 * @brief   hal watch dog define.
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

#ifndef _HAL_WDG_H
#define _HAL_WDG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "hal_types.h"
#include "hal_dev.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */
typedef enum {
    WDG_DEFT_ID       = 0U,
    WDG_NUM,
    WDG_INVALID_ID    = 0xFFU
} WDG_ID_t;


typedef enum {
    WDG_MODE_RESET_CPU          = 0,
    WDG_MODE_RESET_SYS          = 1,
    WDG_MODE_INTERRUPT          = 2,
    WDG_MODE_MAX,
} WDG_Mode_t;

typedef struct {
    WDG_Mode_t mode;
    /* ms as unit */
    uint32_t timeout;
    /* take effect when mode is interrupt. */
    HAL_CbFunc_t callback;
} WDG_InitParam_t;

typedef enum {
    WDG_GET_RELOAD_CNT          = 0,
    WDG_SET_RELOAD_CNT          = 1,    /* It need to be turned off wdg before config */
    WDG_GET_MODE                = 2,
    WDG_SET_MODE                = 3,    /* It need to be turned off wdg before config */
    WDG_SET_IRQ_CB              = 4,    /* It need to be turned off wdg before config */
} WDG_Attr_t;

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
/* Exported macro.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */
/**
 * @brief  Initialize the watchdog. 
 * @param pInitParam  init param. 
 * @return  watchdog device handler pointer
 */
HAL_Dev_t * HAL_WDG_Init(WDG_ID_t id);

/**
 * @brief  DeInitialize the watchdog.
 * @param wdg  watchdog device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WDG_DeInit(HAL_Dev_t *wdg);

/**
 * @brief  Enable the watchdog.
 * @param wdg  watchdog device handler pointer
 * @param pInitParam  init param @ref WDG_InitParam_t
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WDG_Open(HAL_Dev_t *wdg, WDG_InitParam_t * pInitParam);

/**
 * @brief  Disable the watchdog.
 * @param wdg  watchdog device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WDG_Close(HAL_Dev_t *wdg);

/**
 * @brief  Feed the watchdog.
 * @param wdg  watchdog device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_WDG_Feed(HAL_Dev_t *wdg);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_WDG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

