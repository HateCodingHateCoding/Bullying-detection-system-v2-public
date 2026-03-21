/**
 ******************************************************************************
 * @file    osi_samaphores.c
 * @brief   RTOS interface semaphores define.
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

#include "osi_semaphore.h"
#include "osi_util.h"
#include "semphr.h"

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

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

OSI_Status_t OSI_SemaphoreCreate(OSI_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount)
{
    // if (OSI_SemaphoreIsValid(sem)) {
    //     return OSI_STATUS_ERR_PARAM;
    // }

    sem->handle = xSemaphoreCreateCounting(maxCount, initCount);
    if (sem->handle == NULL) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_SemaphoreCreateBinary(OSI_Semaphore_t *sem)
{
    // if (OSI_SemaphoreIsValid(sem)) {
    //     return OSI_STATUS_ERR_PARAM;
    // }

    sem->handle = xSemaphoreCreateBinary();
    if (sem->handle == NULL) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_SemaphoreDelete(OSI_Semaphore_t *sem)
{
    if (!OSI_SemaphoreIsValid(sem)) {
        return OSI_STATUS_ERR_PARAM;
    }

    vSemaphoreDelete(sem->handle);
    OSI_SemaphoreSetInvalid(sem);
    return OSI_STATUS_OK;
}

OSI_Status_t OSI_SemaphoreWait(OSI_Semaphore_t *sem, OSI_Time_t waitMS)
{
    BaseType_t ret;
    BaseType_t taskWoken;

    if (!OSI_SemaphoreIsValid(sem)) {
        return OSI_STATUS_ERR_PARAM;
    }

    if (OSI_IsISRContext()) {
        if (waitMS != 0) {
            return OSI_STATUS_ERR_ISR;
        }
        taskWoken = pdFALSE;
        ret = xSemaphoreTakeFromISR(sem->handle, &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_ERR_TIMEOUT;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xSemaphoreTake(sem->handle, OSI_CalcWaitTicks(waitMS));
        if (ret != pdPASS) {
            return OSI_STATUS_ERR_TIMEOUT;
        }
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_SemaphoreRelease(OSI_Semaphore_t *sem)
{
    BaseType_t ret;
    BaseType_t taskWoken;

    if (!OSI_SemaphoreIsValid(sem)) {
        return OSI_STATUS_ERR_PARAM;
    }

    if (OSI_IsISRContext()) {
        taskWoken = pdFALSE;
        ret = xSemaphoreGiveFromISR(sem->handle, &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xSemaphoreGive(sem->handle);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
    }

    return OSI_STATUS_OK;
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
