/**
 ******************************************************************************
 * @file    osi_common.h
 * @brief   rtos interface common define.
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

#ifndef KERNEL_INC_OSI_COMMON_H_
#define KERNEL_INC_OSI_COMMON_H_


/* Includes.
 * ----------------------------------------------------------------------------
 */

#include <stdint.h>
#include "types.h"
#include "compiler.h"
#include "kernel_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Thread priority definition
 */
typedef enum {
    OSI_PRIORITY_IDLE            = 0,
    OSI_PRIORITY_LOW             = 1,
    OSI_PRIORITY_BELOW_NORMAL    = 2,
    OSI_PRIORITY_NORMAL          = 3,
    OSI_PRIORITY_ABOVE_NORMAL    = 4,
    OSI_PRIORITY_HIGH            = 5,
    OSI_PRIORITY_REAL_TIME       = 6
} OSI_Priority_t;

/**
 * @brief OSI status definition
 */
typedef enum {
    OSI_STATUS_OK               = 0,    /* success */
    OSI_STATUS_FAIL             = -1,   /* general failure */
    OSI_STATUS_ERR_NOMEM        = -2,   /* out of memory */
    OSI_STATUS_ERR_PARAM        = -3,   /* invalid parameter */
    OSI_STATUS_ERR_TIMEOUT      = -4,   /* operation timeout */
    OSI_STATUS_ERR_ISR          = -5,   /* not allowed in ISR context */
} OSI_Status_t;

/**
 * @brief Type definition of OSI handle
 */
typedef void * OSI_Handle_t;

/**
 * @brief Type definition of OSI time
 */
typedef uint32_t OSI_Time_t;

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#define OSI_INVALID_HANDLE       NULL        /* OSI invalid handle */
#define OSI_WAIT_FOREVER         0xFFFFFFFFU /* Wait forever timeout value */
#define OSI_SEMAPHORE_MAX_COUNT  0xFFFFFFFFU /* Maximum count value for semaphore */


#ifdef __cplusplus
}
#endif


#endif  // KERNEL_INC_OSI_COMMON_H_

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
