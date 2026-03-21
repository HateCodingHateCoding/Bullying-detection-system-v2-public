/**
 ******************************************************************************
 * @file    osi_queue.h
 * @brief   rtos interface queue define.
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

#ifndef KERNEL_INC_OSI_QUEUE_H_
#define KERNEL_INC_OSI_QUEUE_H_

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include <osi_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Queue handle definition
 */
typedef OSI_Handle_t OSI_QueueHandle_t;

/**
 * @brief Queue object definition
 */
typedef struct OS_Queue {
    OSI_QueueHandle_t handle;
} OSI_Queue_t;

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Create and initialize a queue object
 * @param[in] queue Pointer to the queue object
 * @param[in] queueLen The maximum number of items that the queue can hold at
 *                     any one time.
 * @param[in] itemSize The size, in bytes, of each data item that can be stored
 *                     in the queue.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_QueueCreate(OSI_Queue_t *queue, uint32_t queueLen, uint32_t itemSize);

/**
 * @brief Delete the queue object
 * @param[in] queue Pointer to the queue object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_QueueDelete(OSI_Queue_t *queue);

/**
 * @brief Send (write) an item to the back of the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the data to be copied into the queue.
 *                 The size of each item the queue can hold is set when the
 *                 queue is created, and that many bytes will be copied from
 *                 item into the queue storage area.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   queue, should the queue already be full.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_QueueSend(OSI_Queue_t *queue, const void *item, OSI_Time_t waitMS);

/**
 * @brief Receive (read) an item from the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the memory into which the received data will be
 *                 copied. The length of the buffer must be at least equal to
 *                 the queue item size which is set when the queue is created.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for data to become available on the
 *                   queue, should the queue already be empty.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_QueueReceive(OSI_Queue_t *queue, void *item, OSI_Time_t waitMS);

/**
 * @brief Check whether the queue object is valid or not
 * @param[in] queue Pointer to the queue object
 * @return 1 on valid, 0 on invalid
 */
static __always_inline int OSI_QueueIsValid(OSI_Queue_t *queue) {
    return (queue->handle != OSI_INVALID_HANDLE);
}

/**
 * @brief Set the queue object to invalid state
 * @param[in] queue Pointer to the queue object
 * @return None
 */
static __always_inline void OSI_QueueSetInvalid(OSI_Queue_t *queue) {
    queue->handle = OSI_INVALID_HANDLE;
}

/**
 * @brief Create and initialize a message queue object
 * @note A message queue is a queue with each data item can store a pointer.
 *       The size of each data item (message) is equal to sizeof(void *).
 * @param[in] queue Pointer to the message queue object
 * @param[in] queueLen The maximum number of items that the message queue can
 *                     hold at any one time.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
static __always_inline OSI_Status_t OSI_MsgQueueCreate(OSI_Queue_t *queue, uint32_t queueLen) {
    return OSI_QueueCreate(queue, queueLen, sizeof(void *));
}

/**
 * @brief Delete the message queue object
 * @param[in] queue Pointer to the message queue object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
static __always_inline OSI_Status_t OSI_MsgQueueDelete(OSI_Queue_t *queue) {
    return OSI_QueueDelete(queue);
}

/**
 * @brief Send (write) a message to the back of the message queue
 * @param[in] queue Pointer to the message queue object
 * @param[in] msg A message, which is a pointer, to be copied into the queue
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   message queue, should the message queue already be full.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
static __always_inline OSI_Status_t OSI_MsgQueueSend(OSI_Queue_t *queue, void *msg, OSI_Time_t waitMS) {
    return OSI_QueueSend(queue, &msg, waitMS);
}

/**
 * @brief Receive (read) a message from the message queue
 * @param[in] queue Pointer to the message queue object
 * @param[in] msg Pointer to the message buffer into which the received message
 *                will be copied. A message is a pointer.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for message to become available on
 *                   the message queue, should the message queue already be
 *                   empty.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
static __always_inline OSI_Status_t OSI_MsgQueueReceive(OSI_Queue_t *queue, void **msg, OSI_Time_t waitMS) {
    return OSI_QueueReceive(queue, msg, waitMS);
}


#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INC_OSI_QUEUE_H_ */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
