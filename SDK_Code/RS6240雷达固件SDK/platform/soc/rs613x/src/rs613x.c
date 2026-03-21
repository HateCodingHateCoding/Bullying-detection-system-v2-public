/**
 ******************************************************************************
 * @file    rs613x.c
 * @brief   v define.
 * @verbatim    null
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

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "config.h"


#if (CONFIG_SOC_SERIES_RS613X)

#include "compiler.h"
#include <irq.h>
#include "string.h"

#include "hal_types.h"
#include "hal_dev.h"

#include "osi_port.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_CPU_CACHE_EN
#define CONFIG_CPU_CACHE_EN                             1
#endif

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

static void _soc_clear_bss(void)
{
    extern unsigned int __bss_start__;
    extern unsigned int __bss_end__;

    unsigned int bss_len = (unsigned int)(&__bss_end__) - (unsigned int)(&__bss_start__);
    memset((void *)(&__bss_start__), 0, bss_len);  // NOLINT
}


__keep_sram_text void SOC_CacheConfig(void)
{
    /*
     * config :
     * 0> 0x0000 0000 - 0x0050 0000 : rom/ram-code,     memory, non-cacheable, bufferable
     * 1> 0x0050 0000 - 0x0800 0000 : remap sram-code,  memory, cacheable,     bufferable
     * 2> 0x0800 0000 - 0x1000 0000 : xip-code,         memory, cacheable,     bufferable
     * 3> 0x1000 0000 - 0x1040 0000 : rom-data,         memory, non-cacheable, bufferable
     * 4> 0x1040 0000 - 0x1800 0000 : sram-data,        memory, non-cacheable, bufferable
     * 5> 0x1800 0000 - 0x2000 0000 : xip-data,         memory, cacheable,     bufferable
     * 6> 0x2000 0000 - 0x4000 0000 : sbus-sram,        memory, non-cacheable, bufferable
     * 7> 0x4000 0000 - 0xFFFF FFFF : periph,           device, non-cacheable, non-bufferable
     */
    /* 0> 0x0000 0000 - 0x0050 0000 : rom/ram-code,     memory, non-cacheable, bufferable */
    csi_sysmap_config_region(0, 0x00500000, SYSMAP_SYSMAPCFG_B_Msk);
    /* 1> 0x0050 0000 - 0x0800 0000 : remap sram-code,  memory, cacheable,     bufferable */
    csi_sysmap_config_region(1, 0x08000000, SYSMAP_SYSMAPCFG_B_Msk | SYSMAP_SYSMAPCFG_C_Msk);
    /* 2> 0x0800 0000 - 0x1000 0000 : xip-code,         memory, cacheable,     bufferable */
    csi_sysmap_config_region(2, 0x10000000, SYSMAP_SYSMAPCFG_B_Msk | SYSMAP_SYSMAPCFG_C_Msk);
    /* 3> 0x1000 0000 - 0x1040 0000 : ram-data,         memory, non-cacheable, bufferable */
    csi_sysmap_config_region(3, 0x10400000, SYSMAP_SYSMAPCFG_B_Msk);
    /* 4> 0x1040 0000 - 0x1800 0000 : sram-data,        memory, non-cacheable, bufferable */
    csi_sysmap_config_region(4, 0x18000000, SYSMAP_SYSMAPCFG_B_Msk | SYSMAP_SYSMAPCFG_C_Msk);
    /* 5> 0x1800 0000 - 0x2000 0000 : xip-data,         memory, cacheable,     bufferable */
    csi_sysmap_config_region(5, 0x20000000, SYSMAP_SYSMAPCFG_B_Msk | SYSMAP_SYSMAPCFG_C_Msk);
    /* 6> 0x2000 0000 - 0x4000 0000 : sbus-sram,        memory, non-cacheable, bufferable */
    csi_sysmap_config_region(6, 0x40000000, SYSMAP_SYSMAPCFG_B_Msk);
    /* 7> 0x4000 0000 - 0xFFFF FFFF : periph,           device, non-cacheable, non-bufferable */
    csi_sysmap_config_region(7, 0xFFFFFFFF, SYSMAP_SYSMAPCFG_SO_Msk);

#if (CONFIG_CPU_CACHE_EN == 1)
    csi_icache_enable();
    csi_dcache_enable();
#endif
}


void _soc_init(void)
{
    /* data section init at bootloader */
    csi_icache_disable();
    csi_dcache_disable();

    _soc_clear_bss();

    SOC_CacheConfig();

    IRQ_Init();

    PMU_HandleInit(NULL, &SOC_CacheConfig);
}


#endif /* CONFIG_SOC_RS6130 */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

