/**
 ******************************************************************************
 * @file    hal_dev.c
 * @brief   hal device define.
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

#include <kernel_config.h>
#include "osi.h"
#include "hal_dev.h"
#include "soc.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define DEV_ID(type, idx)          (((idx & 0xFF) << 8) | ((type) & 0xFF))


/* Private variables.
 * ----------------------------------------------------------------------------
 */
static slist_t HalDeviceList;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t * HAL_DEV_Find(HAL_DevType_t type, uint8_t idx)
{
    HAL_Dev_t *device = NULL;
    uint16_t device_id = DEV_ID(type, idx);

    for (slist_t *scan = HalDeviceList.next; scan != NULL; scan = scan->next) {
        if (((HAL_Dev_t *)scan)->id == device_id) {  // NOLINT
            device = (HAL_Dev_t *)scan;  // NOLINT
            break;
        }
    }

    return device;
}

HAL_Status_t HAL_DEV_Register(HAL_Dev_t *device, HAL_DevType_t type, uint8_t idx)
{
    uint16_t device_id = DEV_ID(type, idx);

    if ((device == NULL) || (type >= HAL_DEV_TYPE_MAX)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    for (slist_t *scan = HalDeviceList.next; scan != NULL; scan = scan->next) {
        if (((HAL_Dev_t *)scan)->id == device_id) {  // NOLINT
            return HAL_STATUS_ALREADY;
        }
    }

    device->id = device_id;
    device->state = HAL_DEV_STATE_INIT;
    slist_add(&device->node, &HalDeviceList);

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_DEV_Unregister(HAL_Dev_t *device)
{
    HAL_Status_t status = HAL_STATUS_ALREADY;

    if (device == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    for (slist_t *scan = HalDeviceList.next; scan != NULL; scan = scan->next) {
        if ((((HAL_Dev_t *)scan) == device) && (device->state != HAL_DEV_STATE_UNINIT)) {  // NOLINT
            slist_del(&device->node, &HalDeviceList);
            device->state = HAL_DEV_STATE_UNINIT;
            status = HAL_STATUS_OK;
            break;
        }
    }

    return status;
}


void HAL_DEV_Init(void)
{
    slist_init(&HalDeviceList);
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

