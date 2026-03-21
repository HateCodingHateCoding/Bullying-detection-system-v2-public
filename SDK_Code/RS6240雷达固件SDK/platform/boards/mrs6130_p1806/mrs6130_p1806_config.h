/**
 ******************************************************************************
 * @file    mrs6130_p1806_config.h
 * @brief   mrs6130_p1806 config define.
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


#ifndef _MRS6130_P1806_CONFIG_H
#define _MRS6130_P1806_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */


#if CONFIG_BOARD_MRS6130_P1806

#define CONFIG_BOARD_STRING                             "mrs6130_p1806"
#define CONFIG_SOC_RS6130                               1


#ifndef CONFIG_HEAP_SIZE
#define CONFIG_HEAP_SIZE                                (1024 * 40)
#endif


#ifndef CONFIG_KERNEL
#define CONFIG_KERNEL                                   1
#endif


#if CONFIG_KERNEL
#ifndef CONFIG_INIT_TASK_STACK_SIZE
#define CONFIG_INIT_TASK_STACK_SIZE                     4096
#endif
#endif /* CONFIG_KERNEL */


#ifndef CONFIG_DRIVER_EFUSE
#define CONFIG_DRIVER_EFUSE                             1
#endif

#ifndef CONFIG_DRIVER_FLASH
#define CONFIG_DRIVER_FLASH                             1
#endif

#ifndef CONFIG_PRINTF
#define CONFIG_PRINTF                                   1
#endif


#ifndef CONFIG_SOC_CLOCK_CFG
#define CONFIG_SOC_CLOCK_CFG                            128
#endif


/** Printf **/
#if CONFIG_PRINTF
#ifndef CONFIG_PRINTF_UART_NUM
#define CONFIG_PRINTF_UART_NUM                          0
#endif

#ifndef CONFIG_PRINTF_UART_BAUDRATE
#define CONFIG_PRINTF_UART_BAUDRATE                     115200
#endif

/* depend on UART_DRIVER */
#if (CONFIG_PRINTF >= 1)
#undef  CONFIG_DRIVER_UART
#define CONFIG_DRIVER_UART                              1

#else
#error "CONFIG_PRINTF enable, but CONFIG_PRINTF_UART_NUM is error value!"
#endif
#endif /* CONFIG_PRINTF */

/** Shell **/
#ifndef CONFIG_SHELL
#define CONFIG_SHELL                                    0
#endif


#if CONFIG_SHELL
#ifndef CONFIG_SHELL_UART_NUM
#define CONFIG_SHELL_UART_NUM                           0
#endif

#ifndef CONFIG_SHELL_UART_BAUDRATE
#define CONFIG_SHELL_UART_BAUDRATE                      115200
#endif

/* depend on UART_DRIVER */
#if (CONFIG_SHELL >= 1)
#undef  CONFIG_DRIVER_UART
#define CONFIG_DRIVER_UART                              1

#else
#error "CONFIG_SHELL enable, but CONFIG_SHELL_UART_NUM is error value!"
#endif
#endif /* CONFIG_SHELL */


#ifndef CONFIG_LOG
#define CONFIG_LOG                                      1
#endif


#ifndef CONFIG_XIP
#define CONFIG_XIP                                      1
#endif

#if CONFIG_XIP
#undef CONFIG_SECTION_ATTRIBUTE_SRAM
#define CONFIG_SECTION_ATTRIBUTE_SRAM                   1
#endif



#if CONFIG_HIF
#include "hif_config.h"
#endif

#endif /* CONFIG_BOARD_MRS6130_P1806 */


#endif /* _MRS6130_P1806_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
