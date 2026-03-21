/**
 ******************************************************************************
 * @file    osi_semaphore.h
 * @brief   rtos interface semaphore define.
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

#ifndef KERNEL_INC_OSI_SEMAPHORE_H_
#define KERNEL_INC_OSI_SEMAPHORE_H_

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
 * @brief Semaphore handle definition
 */
typedef OSI_Handle_t OSI_SemaphoreHandle_t;

/**
 * @brief Semaphore object definition
 */
typedef struct OSI_Semaphore {
    OSI_SemaphoreHandle_t handle;
} OSI_Semaphore_t;

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

/**
 * @brief Create and initialize a counting semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @param[in] initCount The count value assigned to the semaphore when it is
 *                      created.
 * @param[in] maxCount The maximum count value that can be reached. When the
 *                     semaphore reaches this value it can no longer be
 *                     released.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_SemaphoreCreate(OSI_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount);

/**
 * @brief Create and initialize a binary semaphore object
 * @note A binary semaphore is equal to a counting semaphore created by calling
         OSI_SemaphoreCreate(sem, 0, 1).
 * @param[in] sem Pointer to the semaphore object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_SemaphoreCreateBinary(OSI_Semaphore_t *sem);

/**
 * @brief Delete the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_SemaphoreDelete(OSI_Semaphore_t *sem);

/**
 * @brief Wait until the semaphore object becomes available
 * @param[in] sem Pointer to the semaphore object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the
 *                   semaphore to become available.
 *                   OSI_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_SemaphoreWait(OSI_Semaphore_t *sem, OSI_Time_t waitMS);

/**
 * @brief Release the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_SemaphoreRelease(OSI_Semaphore_t *sem);

/**
 * @brief Check whether the semaphore object is valid or not
 * @param[in] sem Pointer to the semaphore object
 * @return 1 on valid, 0 on invalid
 */
static __always_inline int OSI_SemaphoreIsValid(OSI_Semaphore_t *sem) {
    return (sem && sem->handle != OSI_INVALID_HANDLE);
}

/**
 * @brief Set the semaphore object to invalid state
 * @param[in] sem Pointer to the semaphore object
 * @return None
 */
static __always_inline void OSI_SemaphoreSetInvalid(OSI_Semaphore_t *sem) {
    sem->handle = OSI_INVALID_HANDLE;
}


#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INC_OSI_SEMAPHORE_H_ */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
