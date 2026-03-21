/**
 ******************************************************************************
 * @file    hal_dev.h
 * @brief   hal device define.
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

#ifndef _HAL_DEV_H
#define _HAL_DEV_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "hal_types.h"
#include "sys/list.h"
#include "osi.h"

#if CONFIG_PM
#include "pm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_DEV_NAME_SIZE
#define CONFIG_DEV_NAME_SIZE            8
#endif


#if CONFIG_PM
struct pm_device;
#endif

typedef enum {
    HAL_DEV_TYPE_NONE               = 0U,
    HAL_DEV_TYPE_PWR_CLK            = 1U,
    HAL_DEV_TYPE_EFUSE              = 2U,
    HAL_DEV_TYPE_WDG                = 3U,
    HAL_DEV_TYPE_DMA                = 4U,
    HAL_DEV_TYPE_IPC                = 5U,
    HAL_DEV_TYPE_GPIO               = 6U,
    HAL_DEV_TYPE_WAKEIO             = 7U,
    HAL_DEV_TYPE_UART               = 8U,
    HAL_DEV_TYPE_SPI                = 9U,
    HAL_DEV_TYPE_I2C                = 10U,
    HAL_DEV_TYPE_CAN                = 11U,
    HAL_DEV_TYPE_FLASH              = 12U,
    HAL_DEV_TYPE_PWM                = 13U,
    HAL_DEV_TYPE_MMW                = 14U,
    HAL_DEV_TYPE_M50                = 15U,
    HAL_DEV_TYPE_MAX
} HAL_DevType_t;

typedef enum {
    HAL_DEV_PM_ACTION_LIGHT_SLEEP   = 0,
    HAL_DEV_PM_ACTION_LIGHT_WAKEUP  = 1,
    HAL_DEV_PM_ACTION_DEEP_SLEEP    = 2,
    HAL_DEV_PM_ACTION_DEEP_WAKEUP   = 3,
} HAL_DevPmAction_t;

typedef enum {
    HAL_DEV_STATE_UNINIT            = 0,
    HAL_DEV_STATE_INIT              = 1,
    HAL_DEV_STATE_CLOSE             = 2,
    HAL_DEV_STATE_OPEN              = 3,

    HAL_DEV_STATE_ACTIVE            = 4,
    HAL_DEV_STATE_LIGHT_SLEEP       = 5,
    HAL_DEV_STATE_DEEP_SLEEP        = 6,
} HAL_DevState_t;

typedef struct {
    slist_t             node;

//    const char *        name;
    void                *reg;
    void                (* irqHandler)(void * param);

    uint16_t            id;
    uint8_t             state;
    uint8_t             irqNum;
    uint8_t             irqPrio;
#if (CONFIG_DEV_NAME_SIZE > 0)
    char                name[CONFIG_DEV_NAME_SIZE + 1];
#endif
#if CONFIG_PM
    struct pm_device    pmDev;
#endif
} HAL_Dev_t;

typedef void (* HAL_CbFunc_t) (HAL_Dev_t * pDevice, void *arg);

typedef struct {
    HAL_CbFunc_t        cb;
    void *              arg;
} HAL_Callback_t;
/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#define HAL_DEV_MemFree(mem)                OSI_Free(mem)

/* Private macro.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t * HAL_DEV_Find(HAL_DevType_t type, uint8_t idx);

HAL_Status_t HAL_DEV_Register(HAL_Dev_t *device, HAL_DevType_t type, uint8_t idx);

HAL_Status_t HAL_DEV_Unregister(HAL_Dev_t *device);

static inline HAL_DevState_t HAL_DEV_GetState(const HAL_Dev_t *device)
{
    return (HAL_DevState_t)(device->state);
}

static inline void HAL_DEV_SetState(HAL_Dev_t *device, HAL_DevState_t state)
{
    device->state = (uint8_t)state;
}

static inline void * HAL_DEV_MemMalloc(uint32_t size)
{
    uint8_t *pbuff = (uint8_t *)OSI_Malloc(size);

    if (pbuff != NULL) {
        memset(pbuff, 0, size);
    }

    return pbuff;
}

static inline HAL_DevType_t HAL_DEV_GetType(HAL_Dev_t *device)
{
    if (device != NULL) {
        return (device->id & 0xFF);
    } else {
        return HAL_DEV_TYPE_MAX;
    }
}

static inline uint8_t HAL_DEV_GetId(HAL_Dev_t *device)
{
    if (device != NULL) {
        return ((device->id >> 8)& 0xFF);
    } else {
        return 0xFF;
    }
}


#if (CONFIG_DEV_NAME_SIZE > 0)
static inline HAL_DevState_t HAL_DEV_SetName(HAL_Dev_t *device, char * name)
{
    int idx = 0;
    if (device == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (name != NULL) {
        for (idx = 0; idx < CONFIG_DEV_NAME_SIZE; idx++) {
            if (name[idx] == 0) {
                break;
            }

            device->name[idx] = name[idx];
        }

        device->name[idx] = 0;
    }

    return HAL_STATUS_OK;
}

static inline char * HAL_DEV_GetName(HAL_Dev_t *device)
{
    return device->name;
}

#else

#define HAL_DEV_SetName(device, name)                       (HAL_STATUS_NOT_SUPPORTED)
#define HAL_DEV_GetName(device)                             NULL

#endif

void HAL_DEV_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_DEV_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
