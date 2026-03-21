/**
 ******************************************************************************
 * @file    osi_mutex.h
 * @brief   rtos interface mutex define.
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

#ifndef KERNEL_INC_OSI_MUTEX_H_
#define KERNEL_INC_OSI_MUTEX_H_

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include <osi_common.h>
#include <osi_thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Mutex handle definition
 */
typedef OSI_Handle_t OSI_MutexHandle_t;

/**
 * @brief Mutex object definition
 */
typedef struct OSI_Mutex {
    OSI_MutexHandle_t handle;
} OSI_Mutex_t;

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Create and initialize a mutex object
 * @note A mutex can only be locked by a single thread at any given time.
 * @param[in] mutex Pointer to the mutex object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_MutexCreate(OSI_Mutex_t *mutex);

/**
 * @brief Delete the mutex object
 * @param[in] mutex Pointer to the mutex object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_MutexDelete(OSI_Mutex_t *mutex);

/**
 * @brief Lock the mutex object
 * @note A mutex can only be locked by a single thread at any given time. If
 *       the mutex is already locked, the caller will be blocked for the
 *       specified time duration.
 * @param[in] mutex Pointer to the mutex object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the mutex
 *                   to become unlocked.
 *                   OSI_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_MutexLock(OSI_Mutex_t *mutex, OSI_Time_t waitMS);

/**
 * @brief Unlock the mutex object previously locked using OSI_MutexLock()
 * @note The mutex should be unlocked from the same thread context from which
 *       it was locked.
 * @param[in] mutex Pointer to the mutex object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_MutexUnlock(OSI_Mutex_t *mutex);

/**
 * @brief Create and initialize a recursive mutex object
 * @note A recursive mutex can be locked repeatedly by one single thread.
 *       The mutex doesn't become available again until the owner has called
 *       OSI_RecursiveMutexUnlock() for each successful OSI_RecursiveMutexLock().
 * @param[in] mutex Pointer to the recursive mutex object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_RecursiveMutexCreate(OSI_Mutex_t *mutex);

/**
 * @brief Delete the recursive mutex object
 * @param[in] mutex Pointer to the recursive mutex object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_RecursiveMutexDelete(OSI_Mutex_t *mutex);

/**
 * @brief Lock the recursive mutex object
 * @note A recursive mutex can be locked repeatedly by one single thread.
 *       If the recursive mutex is already locked by other thread, the caller
 *       will be blocked for the specified time duration.
 * @param[in] mutex Pointer to the recursive mutex object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the
 *                   recursive mutex to become unlocked.
 *                   OSI_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_RecursiveMutexLock(OSI_Mutex_t *mutex, OSI_Time_t waitMS);

/**
 * @brief Unlock the recursive mutex object previously locked using
 *        OSI_RecursiveMutexLock()
 * @note The recursive mutex should be unlocked from the same thread context
 *       from which it was locked.
 * @param[in] mutex Pointer to the mutex object
 * @retval OSI_Status_t, OSI_STATUS_OK on success
 */
OSI_Status_t OSI_RecursiveMutexUnlock(OSI_Mutex_t *mutex);

/**
 * @brief Get the owner of the mutex object
 * @param[in] mutex Pointer to the mutex object
 * @return Thread handle which is the owner of the mutex object
 */
OSI_ThreadHandle_t OSI_MutexGetOwner(OSI_Mutex_t *mutex);

/**
 * @brief Check whether the mutex object is valid or not
 * @param[in] mutex Pointer to the mutex object
 * @return 1 on valid, 0 on invalid
 */
static __always_inline int OSI_MutexIsValid(OSI_Mutex_t *mutex) {
    return (mutex && mutex->handle != OSI_INVALID_HANDLE);
}

/**
 * @brief Set the mutex object to invalid state
 * @param[in] mutex Pointer to the mutex object
 * @return None
 */
static __always_inline void OSI_MutexSetInvalid(OSI_Mutex_t *mutex) {
    mutex->handle = OSI_INVALID_HANDLE;
}


#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INC_OSI_MUTEX_H_ */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
