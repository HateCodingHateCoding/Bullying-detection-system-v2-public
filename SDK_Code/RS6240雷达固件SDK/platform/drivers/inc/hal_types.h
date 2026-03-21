/**
 ******************************************************************************
 * @file    hal_types.h
 * @brief   hal types define.
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

#ifndef _HAL_TYPES_H
#define _HAL_TYPES_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "errno.h"

#include "soc.h"
#include "osi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */

typedef enum {
    HAL_STATE_DISABLE = 0,
    HAL_STATE_ENABLE  = !HAL_STATE_DISABLE
} HAL_State_t;

typedef enum {
    HAL_STATUS_OK                     = 0,          /*!< success. */
    HAL_STATUS_ERROR                  = -EIO,       /*!< general error. */
    HAL_STATUS_BUSY                   = -EBUSY,     /*!< Busy. */
    HAL_STATUS_TIMEOUT                = -ETIMEDOUT, /*!< Operation timed out. */
    HAL_STATUS_INVALID_PARAM          = -EINVAL,    /*!< Invalid parameter. */
    HAL_STATUS_INVALID_STATE          = -EACCES,    /*!< Invalid state. */
    HAL_STATUS_NO_MEM                 = -ENOBUFS,   /*!< No memory for operation. */
    HAL_STATUS_NOT_SUPPORTED          = -ENOTSUP,   /*!< Not supported. */
    HAL_STATUS_ALREADY                = -EALREADY,  /*!< Module already initialized. */
} HAL_Status_t;

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
#ifndef NULL
  #define NULL                                      ((void *)0)
#endif

#ifndef __HAL_INLINE
  #define __HAL_INLINE                              __inline
#endif

#ifndef __HAL_STATIC_INLINE
  #define __HAL_STATIC_INLINE                       static __inline
#endif

#ifndef __HAL_FORCE_INLIDE
  #define __HAL_FORCE_INLIDE                        __attribute__((always_inline)) static __inline__
#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#define HAL_ABS(a)                                   ((a) < (0) ? -(a) : (a))
#define HAL_MIN(a, b)                                ((a) < (b) ? (a) : (b))
#define HAL_MAX(a, b)                                ((a) > (b) ? (a) : (b))
#define HAL_ARRAY_SIZE(arr)                          (sizeof(arr) / sizeof((arr)[0]))
#define HAL_OFFSETOF(type, member)                   ((size_t)&(((type *)0)->member))  // NOLINT
#define HAL_CONTAINER_OF(ptr, type, field)    ((type *)(((char *)(ptr)) - offsetof(type, field)))

#define HAL_BIT(pos)                                 (1U << (pos))
#define HAL_BIT_MSK(pos)                             (HAL_BIT(pos) - 1UL)

#define HAL_SET_MSK(word, vmask)                     ((word) |= (vmask))
#define HAL_CLR_MSK(word, vmask)                     ((word) &= ~(vmask))
#define HAL_GET_MSK(word, vmask)                     ((word) & (vmask))

#define HAL_SET_VAL(word, val)                       ((word) = (val))
#define HAL_GET_VAL(word)                            (word)

#define HAL_SET_BIT(word, num)                       ((word) |= (uint32_t)(1U << (num)))
#define HAL_CLR_BIT(word, num)                       ((word) &= (~(uint32_t)(1U << (num))))
#define HAL_GET_BIT(word, num)                       ((word) &= (uint32_t)(1U << (num)))

#define HAL_SET_MSK_VAL(word, vmask, shift, val)     ((word) = ((word) & ~(vmask)) | ((vmask) & ((val) << (shift))))
#define HAL_GET_MSK_VAL(word, vmask, shift)          (((word) & (vmask)) >> (shift))
#define HAL_TEST_BIT(word, bit)                      ((word) & (1 << (bit)))


#define HAL_READ_32BIT(addr)                        (*((volatile uint32_t *)(addr)))
#define HAL_READ_16BIT(addr)                        (*((volatile uint16_t *)(addr)))
#define HAL_READ_8BIT(addr)                         (*((volatile uint8_t  *)(addr)))

#define HAL_WRITE_32BIT(addr, val)                  (*((volatile uint32_t *)(addr)) = (uint32_t)(val))
#define HAL_WRITE_16BIT(addr, val)                  (*((volatile uint16_t *)(addr)) = (uint16_t)(val))
#define HAL_WRITE_8BIT(addr, val)                   (*((volatile uint8_t  *)(addr)) = (uint8_t)(val))


#define HAL_WAIT_FOREVER                            OSI_WAIT_FOREVER

#define LEVEL_DIS                                   0
#define LEVEL_ERR                                   1
#define LEVEL_WRN                                   2
#define LEVEL_INF                                   3
#define LEVEL_DBG                                   4

/* Private macro.
 * ----------------------------------------------------------------------------
 */

#ifndef HAL_ABORT
#define HAL_ABORT()
#endif

#ifndef HAL_ASSERT_PARAM
#define HAL_ASSERT_PARAM(param)
#endif

/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _HAL_TYPES_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
