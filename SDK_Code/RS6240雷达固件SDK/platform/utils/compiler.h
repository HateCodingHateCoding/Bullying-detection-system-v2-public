/**
 ******************************************************************************
 * @file    compiler.h
 * @brief   compiler define.
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

#ifndef PLATFORM_UTILS_COMPILER_H_
#define PLATFORM_UTILS_COMPILER_H_

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_SECTION_ATTRIBUTE_XIP
#define CONFIG_SECTION_ATTRIBUTE_XIP                    0
#endif

#ifndef CONFIG_SECTION_ATTRIBUTE_NONXIP
#define CONFIG_SECTION_ATTRIBUTE_NONXIP                 0
#endif

#ifndef CONFIG_SECTION_ATTRIBUTE_SRAM
#define CONFIG_SECTION_ATTRIBUTE_SRAM                   0
#endif

#ifndef CONFIG_SECTION_ATTRIBUTE_PSRAM
#define CONFIG_SECTION_ATTRIBUTE_PSRAM                  0
#endif


/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#define __packed    __attribute__((__packed__))

#define __inline    inline
#define __inline__  inline

#ifdef __always_inline
#undef __always_inline    /* already defined in <sys/cdefs.h> */
#define __always_inline    inline __attribute__((always_inline))
#endif

#ifndef __noinline
#define __noinline  __attribute__((__noinline__))
#endif

#define __packed    __attribute__((__packed__))
#define __asm       __asm__
#define __weak      __attribute__((weak))



#if (CONFIG_SECTION_ATTRIBUTE_XIP == 1)
#define __xip_text      __attribute__((section (".xip_text")))
#define __xip_rodata    __attribute__((section (".xip_rodata")))
#else
#define __xip_text
#define __xip_rodata
#endif

#if (CONFIG_SECTION_ATTRIBUTE_NONXIP == 1)
#define __nonxip_text   __attribute__((section (".nonxip_text")))
#define __nonxip_rodata __attribute__((section (".nonxip_rodata")))
#define __nonxip_data   __attribute__((section (".nonxip_data")))
#define __nonxip_bss    __attribute__((section (".nonxip_bss")))
#else
#define __nonxip_text
#define __nonxip_rodata
#define __nonxip_data
#define __nonxip_bss
#endif

#define __keep_sram_text     __attribute__((section (".keep_sram_text")))
#define __keep_sram_rodata   __attribute__((section (".keep_sram_rodata")))
#define __keep_sram_data     __attribute__((section (".keep_sram_data")))
#define __keep_sram_bss      __attribute__((section (".keep_sram_bss")))

#if (CONFIG_SECTION_ATTRIBUTE_SRAM == 1)
#define __sram_text     __attribute__((section (".sram_text")))
#define __sram_rodata   __attribute__((section (".sram_rodata")))
#define __sram_data     __attribute__((section (".sram_data")))
#define __sram_bss      __attribute__((section (".sram_bss")))
#else
#define __sram_text
#define __sram_rodata
#define __sram_data
#define __sram_bss
#endif

#if (CONFIG_SECTION_ATTRIBUTE_PSRAM == 1)
#define __psram_text    __attribute__((section (".psram_text")))
#define __psram_rodata  __attribute__((section (".psram_rodata")))
#define __psram_data    __attribute__((section (".psram_data")))
#define __psram_bss     __attribute__((section (".psram_bss")))
#else
#define __psram_text
#define __psram_rodata
#define __psram_data
#define __psram_bss
#endif

#ifndef STACK_SECTION
#define STACK_SECTION             __attribute__((section(".stack")))
#endif

#ifndef HEAP_SECTION
#define HEAP_SECTION              __attribute__((section(".heap")))
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

#endif /* PLATFORM_UTILS_COMPILER_H_   */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
