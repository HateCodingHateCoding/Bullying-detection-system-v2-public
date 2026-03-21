/**
 ******************************************************************************
 * @file    types.h
 * @brief   types define.
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

 #ifndef _TYPES_H
 #define _TYPES_H
 
 /* Includes.
  * ----------------------------------------------------------------------------
  */
 
 
#ifdef __cplusplus
extern "C" {
#endif
 
 
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <soc.h>

#include "cs.h"
#include "assert.h"

/* Exported types.
 * ----------------------------------------------------------------------------
 */


typedef enum {
  HW_RESET = 0,
  HW_SET   = !HW_RESET
}flag_status_t, itr_status_t;


typedef enum {
    HW_DISABLE = 0,
    HW_ENABLE  = !HW_DISABLE
}func_state_t;

typedef enum {
    HW_SUCCESS                     =  0,   /*!< Operation performed successfully.                                */
    HW_NULL                        =  1,   /*!< Null pointer.                                                    */
    HW_NOT_SUPPORTED               =  2,   /*!< Not supported.                                                   */
    HW_FORBIDDEN                   =  3,   /*!< Operation is forbidden.                                          */
    HW_BUSY                        =  4,   /*!< Busy.                                                            */
    HW_NO_MEM                      =  5,   /*!< No memory for operation.                                         */
    HW_TIMEOUT                     =  6,   /*!< Operation timed out.                                             */
    HW_INVALID_PARAM               =  7,   /*!< Invalid parameter.                                               */
    HW_INVALID_STATE               =  8,   /*!< Invalid state, operation disallowed in this state.               */
    HW_INVALID_LENGTH              =  9,   /*!< Invalid length.                                                  */
    HW_INVALID_ADDR                = 10,   /*!< Bad memory address.                                              */
    HW_ALREADY_INITIALIZED         = 11,   /*!< Module already initialized.                                      */

} hw_status_t;



 /* Exported constants.
  * ----------------------------------------------------------------------------
  */
#ifndef NULL
  #define NULL                                      ((void *)0)
#endif

#ifndef __HW_INLINE
  #define __HW_INLINE                              __inline
#endif

#ifndef __HW_STATIC_INLINE
  #define __HW_STATIC_INLINE                       static __inline
#endif

#ifndef __HW_FORCE_INLIDE
  #define __HW_FORCE_INLIDE                        __attribute__((always_inline)) static __inline__
#endif


 /* Exported macro.
  * ----------------------------------------------------------------------------
  */
 

  #define HW_ABS(a)                                   ((a) < (0) ? -(a) : (a))
  #define HW_MIN(a, b)                                ((a) < (b) ? (a) : (b))
  #define HW_MAX(a, b)                                ((a) > (b) ? (a) : (b))
  #define HW_ROUNDED_DIV(a, b) \
          ((((a) < 0) ^ ((b) < 0)) ? (((a) - (b) / 2) / (b)) : (((a) + (b) / 2) / (b)))
  #define HW_CEIL_DIV(a, b)                           ((((a) - 1) / (b)) + 1)
  #define HW_ARRAY_SIZE(arr)                          (sizeof(arr) / sizeof((arr)[0]))
  #define HW_OFFSETOF(type, member)                   ((size_t)&(((type *)0)->member))
  
  
  
  #define HW_BIT(pos)                                 (1U << (pos))
  #define HW_BIT_MSK(pos)                             (HW_BIT(pos) - 1UL)
  
  #define HW_SET_MSK(word, vmask)                     ((word) |= (vmask))
  #define HW_CLR_MSK(word, vmask)                     ((word) &= ~(vmask))
  #define HW_GET_MSK(word, vmask)                     ((word) & (vmask))
  
  #define HW_SET_VAL(word, val)                        ((word) = (val))
  #define HW_GET_VAL(word)                            (word)
  
  #define HW_SET_BIT(word, num)                       ((word) |= (uint32_t)(1U << (num)))
  #define HW_CLR_BIT(word, num)                       ((word) &= (~(uint32_t)(1U << (num))))
  #define HW_GET_BIT(word, num)                       ((word) &= (uint32_t)(1U << (num)))
  
  #define HW_SET_MSK_VAL(word, vmask, shift, val)     ((word) = ((word) & ~(vmask)) | ((vmask) & ((val) << (shift))))
  
  #define HW_GET_MSK_VAL(word, vmask, shift)          (((word) & (vmask)) >> (shift))
  
  
  #define HW_TEST_BIT(word, bit)                      ((word) & (1 << (bit)))
  
  #define HW_IDX_1(num)                            	((num) >> 5)
  #define HW_POS_1(num)                       		((num) & 0x1F)
  #define HW_MSK_1(num)                      	 		(0x1 << ((num) & 0x1F))
  #define HW_IDX_2(num)                            	((num) >> 4)
  #define HW_POS_2(num)                       		(((num) << 1) & 0x1F)
  #define HW_MSK_2(num)                      	 		(0x3 << (((num) << 1) & 0x1F))
  #define HW_IDX_4(num)                            	((num) >> 3)
  #define HW_POS_4(num)                       		(((num) << 2) & 0x1F)
  #define HW_MSK_4(num)                      	 		(0xF << (((num) << 2) & 0x1F))
  #define HW_IDX_8(num)                            	((num) >> 2)
  #define HW_POS_8(num)                       		(((num) << 3) & 0x1F)
  #define HW_MSK_8(num)                      	 		(0xFF << (((num) << 3) & 0x1F))
  #define HW_IDX_16(num)                            	((num) >> 1)
  #define HW_POS_16(num)                       		(((num) << 4) & 0x1F)
  #define HW_MSK_16(num)                      	 	(0xFFFF << (((num) << 4) & 0x1F))
  
  
  
  
  #define HW_READ_32BIT(addr)                        (*((volatile uint32_t *)(addr)))
  #define HW_READ_16BIT(addr)                        (*((volatile uint16_t *)(addr)))
  #define HW_READ_8BIT(addr)                         (*((volatile uint8_t  *)(addr)))
  
  #define HW_WRITE_32BIT(addr, val)                  (*((volatile uint32_t *)(addr)) = (uint32_t)(val))
  #define HW_WRITE_16BIT(addr, val)                  (*((volatile uint16_t *)(addr)) = (uint16_t)(val))
  #define HW_WRITE_8BIT(addr, val)                   (*((volatile uint8_t  *)(addr)) = (uint8_t)(val))
  
  
  #define UNUSED_VARIABLE(X)                          ((void)(X))
  #define UNUSED_PARAMETER(X)                         UNUSED_VARIABLE(X)
  #define UNUSED_RETURN_VALUE(X)                      UNUSED_VARIABLE(X)
  

 /* Private macro.
  * ----------------------------------------------------------------------------
  */
 
 /* Exported functions.
  * ----------------------------------------------------------------------------
  */
 
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* _TYPES_H */
 
 /*
  ******************************************************************************
  * (C) COPYRIGHT POSSUMIC TECHNOLOGY
  * END OF FILE
  */
 