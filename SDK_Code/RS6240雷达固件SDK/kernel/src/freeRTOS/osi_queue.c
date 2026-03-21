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

#include "osi_queue.h"
#include "osi_timer.h"
#include "osi_util.h"
#include "queue.h"

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

OSI_Status_t OSI_QueueCreate(OSI_Queue_t *queue, uint32_t queueLen, uint32_t itemSize)
{
    // if (OSI_QueueIsValid(queue)) {
    //     return OSI_STATUS_ERR_PARAM;
    // }

    queue->handle = xQueueCreate(queueLen, itemSize);
    if (queue->handle == NULL) {
        return OSI_STATUS_FAIL;
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_QueueDelete(OSI_Queue_t *queue)
{
    UBaseType_t ret;

    if (OSI_QueueIsValid(queue) == 0) {
        return OSI_STATUS_ERR_PARAM;
    }

    ret = uxQueueMessagesWaiting(queue->handle);
    if (ret > 0) {
        return OSI_STATUS_FAIL;
    }

    vQueueDelete(queue->handle);
    OSI_QueueSetInvalid(queue);

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_QueueSend(OSI_Queue_t *queue, const void *item, OSI_Time_t waitMS)
{
    BaseType_t ret;
    BaseType_t taskWoken;

    if (OSI_QueueIsValid(queue) == 0) {
        return OSI_STATUS_ERR_PARAM;
    }

    if (OSI_IsISRContext()) {
        taskWoken = pdFALSE;
        ret = xQueueSendFromISR(queue->handle, item, &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xQueueSend(queue->handle, item, OSI_CalcWaitTicks(waitMS));
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
    }

    return OSI_STATUS_OK;
}

OSI_Status_t OSI_QueueReceive(OSI_Queue_t *queue, void *item, OSI_Time_t waitMS)
{
    BaseType_t ret;
    BaseType_t taskWoken;

    if (OSI_QueueIsValid(queue) == 0) {
        return OSI_STATUS_ERR_PARAM;
    }

    if (OSI_IsISRContext()) {
        taskWoken = pdFALSE;
        ret = xQueueReceiveFromISR(queue->handle, item, &taskWoken);
        if (ret != pdPASS) {
            return OSI_STATUS_FAIL;
        }
        portEND_SWITCHING_ISR(taskWoken);
    } else {
        ret = xQueueReceive(queue->handle, item, OSI_CalcWaitTicks(waitMS));
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
