/**
 ******************************************************************************
 * @file    log_config.h
 * @brief   log config define.
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

#ifndef _LOG_CONFIG_H
#define _LOG_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */
/* Exported constants.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_PRINTF
#define CONFIG_PRINTF                                       0
#endif

#ifndef CONFIG_PRINTF_UART
#define CONFIG_PRINTF_UART                                  1
#endif

#ifndef CONFIG_PRINTF_UART_NUM
#define CONFIG_PRINTF_UART_NUM                              0
#endif

#ifndef CONFIG_PRINTF_UART_BAUDRATE
#define CONFIG_PRINTF_UART_BAUDRATE                         115200
#endif


#ifndef CONFIG_PRINTF_EARLY
#define CONFIG_PRINTF_EARLY                                 0
#endif

#ifndef CONFIG_PRINTF_EARLY_UART_NUM
#define CONFIG_PRINTF_EARLY_UART_NUM                        0
#endif

#ifndef CONFIG_PRINTF_EARLY_UART_BAUDRATE
#define CONFIG_PRINTF_EARLY_UART_BAUDRATE                   115200
#endif



/* 'ntoa' conversion buffer size, this must be big enough to hold one converted
 * numeric number including padded zeros (dynamically created on stack)
 * default: 32 byte
 */
#ifndef CONFIG_PRINTF_NTOA_BUFFER_SIZE
#define CONFIG_PRINTF_NTOA_BUFFER_SIZE                      32U
#endif

/* 'ftoa' conversion buffer size, this must be big enough to hold one converted
 * float number including padded zeros (dynamically created on stack)
 * default: 32 byte
 */
#ifndef CONFIG_PRINTF_FTOA_BUFFER_SIZE
#define CONFIG_PRINTF_FTOA_BUFFER_SIZE                      32U
#endif

/* support for the floating point type (%f)
 * default: activated
 */
#ifndef CONFIG_PRINTF_SUPPORT_FLOAT
#define CONFIG_PRINTF_SUPPORT_FLOAT                         1
#endif

/* support for exponential floating point notation (%e/%g)
 * default: activated
 */
#ifndef CONFIG_PRINTF_SUPPORT_EXPONENTIAL
#define CONFIG_PRINTF_SUPPORT_EXPONENTIAL                   1
#endif

/* define the default floating point precision
 * default: 6 digits
 */
#ifndef CONFIG_PRINTF_DEFAULT_FLOAT_PRECISION
#define CONFIG_PRINTF_DEFAULT_FLOAT_PRECISION               6U
#endif

/* define the largest float suitable to print with %f
 * default: 1e9
 */
#ifndef CONFIG_PRINTF_MAX_FLOAT
#define CONFIG_PRINTF_MAX_FLOAT                             1e9
#endif

/* support for the long long types (%llu or %p)
 * default: activated
 */
#ifndef CONFIG_PRINTF_SUPPORT_LONG_LONG
#define CONFIG_PRINTF_SUPPORT_LONG_LONG                     1
#endif

/* support for the ptrdiff_t type (%t)
 * ptrdiff_t is normally defined in <stddef.h> as long or long long type
 * default: activated
 */
#ifndef CONFIG_PRINTF_SUPPORT_PTRDIFF_T
#define CONFIG_PRINTF_SUPPORT_PTRDIFF_T                     1
#endif


/* Exported macro.
 * ----------------------------------------------------------------------------
 */
/* Private macro.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _LOG_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
