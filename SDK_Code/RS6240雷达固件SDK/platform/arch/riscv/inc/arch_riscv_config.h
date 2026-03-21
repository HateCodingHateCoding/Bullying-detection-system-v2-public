/**
 ******************************************************************************
 * @file    arch_riscv_config.h
 * @brief   arch riscv config define.
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

#ifndef _ARCH_RISCV_CONFIG_H
#define _ARCH_RISCV_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "soc_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

#if CONFIG_CPU_RISCV_E906F
#define CONFIG_CPU_FPU_ENABLE                          1
#define CONFIG_CPU_DSP_ENABLE                          0
#elif CONFIG_CPU_RISCV_E906FP
#define CONFIG_CPU_FPU_ENABLE                          1
#define CONFIG_CPU_DSP_ENABLE                          1
#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_CPU_FPU_ENABLE
#define CONFIG_CPU_FPU_ENABLE                               0
#endif

#ifndef CONFIG_CPU_DSP_ENABLE
#define CONFIG_CPU_DSP_ENABLE                               0
#endif

#ifndef CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
#define CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE                   1
#endif

#ifndef CONFIG_CPU_IRQ_MAX
#define CONFIG_CPU_IRQ_MAX                                  88
#endif

#ifndef CONFIG_CPU_CORE_IRQ_MAX
#define CONFIG_CPU_CORE_IRQ_MAX                             16
#endif

#ifndef CONFIG_CPU_IRQ_NESTED_ENABLE
#define CONFIG_CPU_IRQ_NESTED_ENABLE                        1
#endif

#ifndef CONFIG_CPU_TRAP_STACK_SIZE
#define CONFIG_CPU_TRAP_STACK_SIZE                          512
#endif

#ifndef CONFIG_CPU_IRQ_STACK_SIZE
#define CONFIG_CPU_IRQ_STACK_SIZE                           2048
#endif

#ifndef CONFIG_CPU_KERNEL
#define CONFIG_CPU_KERNEL                                   1
#endif

#ifndef CONFIG_CPU_BAREMATE
#define CONFIG_CPU_BAREMATE                                 0
#endif

#ifndef CONFIG_CPU_BAREMATE_MAIN_STACK_SIZE
#define CONFIG_CPU_BAREMATE_MAIN_STACK_SIZE                 2048
#endif

#ifndef CONFIG_CPU_TRAP_LOG_ENABLE
#define CONFIG_CPU_TRAP_LOG_ENABLE                          1
#endif

#ifndef CONFIG_CPU_TRAP_LOG_STACK_SIZE
#define CONFIG_CPU_TRAP_LOG_STACK_SIZE                      512
#endif


#if ((CONFIG_CPU_FPU_ENABLE == 0)) && (CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE)
#error "This cpu doesn't supprot FPU, but macro 'CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE' is defined, please remove it."
#endif

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _ARCH_RISCV_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

