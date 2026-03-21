/**
 ******************************************************************************
 * @file    template.c
 * @brief   template define.
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

#include "osi_mutex.h"
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

OSI_Status_t OSI_MutexCreate(OSI_Mutex_t *mutex)
{
    if (OSI_MutexIsValid(mutex)) {
        return OSI_STATUS_ERR_PARAM;
    }

    mutex->handle = xSemaphoreCreateMutex();
    if (mutex->handle == NULL) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_MutexDelete(OSI_Mutex_t *mutex)
{
    if (!OSI_MutexIsValid(mutex)) {
        return OSI_STATUS_ERR_PARAM;
    }

    vSemaphoreDelete(mutex->handle);
    OSI_MutexSetInvalid(mutex);
    return OSI_STATUS_OK;
}

OSI_Status_t OSI_MutexLock(OSI_Mutex_t *mutex, OSI_Time_t waitMS)
{
    BaseType_t ret;

    if (!OSI_MutexIsValid(mutex)) {
        return OSI_STATUS_ERR_PARAM;
    }

    ret = xSemaphoreTake(mutex->handle, OSI_CalcWaitTicks(waitMS));
    if (ret != pdPASS) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_MutexUnlock(OSI_Mutex_t *mutex)
{
    BaseType_t ret;

    if (!OSI_MutexIsValid(mutex)) {
        return OSI_STATUS_ERR_PARAM;
    }

    ret = xSemaphoreGive(mutex->handle);
    if (ret != pdPASS) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_RecursiveMutexCreate(OSI_Mutex_t *mutex)
{
    if (OSI_MutexIsValid(mutex)) {
        return OSI_STATUS_ERR_PARAM;
    }

    mutex->handle = xSemaphoreCreateRecursiveMutex();
    if (mutex->handle == NULL) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_RecursiveMutexDelete(OSI_Mutex_t *mutex)
{
    return OSI_MutexDelete(mutex);
}

OSI_Status_t OSI_RecursiveMutexLock(OSI_Mutex_t *mutex, OSI_Time_t waitMS)
{
    BaseType_t ret;

    if (!OSI_MutexIsValid(mutex)) {
        return OSI_STATUS_ERR_PARAM;
    }

    ret = xSemaphoreTakeRecursive(mutex->handle, OSI_CalcWaitTicks(waitMS));
    if (ret != pdPASS) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_RecursiveMutexUnlock(OSI_Mutex_t *mutex)
{
    BaseType_t ret;

    if (!OSI_MutexIsValid(mutex)) {
        return OSI_STATUS_ERR_PARAM;
    }

    ret = xSemaphoreGiveRecursive(mutex->handle);
    if (ret != pdPASS) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_ThreadHandle_t OSI_MutexGetOwner(OSI_Mutex_t *mutex)
{
    if (!OSI_MutexIsValid(mutex)) {
        return OSI_INVALID_HANDLE;
    }

    return (OSI_ThreadHandle_t)xSemaphoreGetMutexHolder(mutex->handle);
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
