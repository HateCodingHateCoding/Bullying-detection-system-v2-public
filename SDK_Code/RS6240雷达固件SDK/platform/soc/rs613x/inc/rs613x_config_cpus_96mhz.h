/**
 ******************************************************************************
 * @file    rs613x_clock_config.h
 * @brief   rs613x clock config define.
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

#ifndef _RS613X_CONFIG_CPUS_96MHZ_H
#define _RS613X_CONFIG_CPUS_96MHZ_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#ifdef __cplusplus
extern "C" {
#endif


#if (CONFIG_SOC_SERIES_RS613X)

 /* Exported types.
 * ----------------------------------------------------------------------------
 */
/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* lsi
 * state: enable (must)
 * freq: 1MHz
 */
#ifndef CONFIG_CLK_LSI_FREQ
#define CONFIG_CLK_LSI_FREQ                                 1000000
#endif

#ifndef CONFIG_CLK_LSI_CALIB
#define CONFIG_CLK_LSI_CALIB                                1
#endif

/* msi
 * state : enable (must)
 * freq : 32MHz
 */
#ifndef CONFIG_CLK_MSI_FREQ
#define CONFIG_CLK_MSI_FREQ                                 32000000
#endif

#ifndef CONFIG_CLK_MSI_CALIB
#define CONFIG_CLK_MSI_CALIB                                1
#endif

/* dcxo
 * state : disable
 */
#ifndef CONFIG_CLK_DCXO_EN
#define CONFIG_CLK_DCXO_EN                                  0
#endif

#ifndef CONFIG_CLK_DCXO_FREQ
#define CONFIG_CLK_DCXO_FREQ                                32000000
#endif

#ifndef CONFIG_CLK_DCXO_OUT_PLL
#define CONFIG_CLK_DCXO_OUT_PLL                             0
#endif

#ifndef CONFIG_CLK_DCXO_OUT_DEV
#define CONFIG_CLK_DCXO_OUT_DEV                             1
#endif

/* pll
 * state: enable
 * freq: 192MHz
 * sys freq: 96MHz
 */
#ifndef CONFIG_CLK_PLL_EN
#define CONFIG_CLK_PLL_EN                                   1
#endif
/* 0: msi; 
 * 1: dcxo
 */
#ifndef CONFIG_CLK_PLL_SRC
#define CONFIG_CLK_PLL_SRC                                  0
#endif

#ifndef CONFIG_CLK_PLL_M_DIVISOR
#define CONFIG_CLK_PLL_M_DIVISOR                            1
#endif

#ifndef CONFIG_CLK_PLL_N_MULTIPLIER
#define CONFIG_CLK_PLL_N_MULTIPLIER                         6
#endif

#ifndef CONFIG_CLK_PLL_OUT_SOC
#define CONFIG_CLK_PLL_OUT_SOC                              1
#endif

#ifndef CONFIG_CLK_PLL_OUT_DEV
#define CONFIG_CLK_PLL_OUT_DEV                              0
#endif

#ifndef CONFIG_CLK_PLL_OUT_DEV1
#define CONFIG_CLK_PLL_OUT_DEV1                             0
#endif

#ifndef CONFIG_CLK_PLL_OUT_CDAA
#define CONFIG_CLK_PLL_OUT_CDAA                             0
#endif

#ifndef CONFIG_CLK_PLL_OUT_SOC_DIVISOR
#define CONFIG_CLK_PLL_OUT_SOC_DIVISOR                      2
#endif

#ifndef CONFIG_CLK_PLL_OUT_DEV_DIVISOR
#define CONFIG_CLK_PLL_OUT_DEV_DIVISOR                      0
#endif

#ifndef CONFIG_CLK_PLL_OUT_DEV1_DIVISOR
#define CONFIG_CLK_PLL_OUT_DEV1_DIVISOR                     0
#endif

#ifndef CONFIG_CLK_PLL_OUT_CDAA_DIVISOR
#define CONFIG_CLK_PLL_OUT_CDAA_DIVISOR                     0
#endif

/* system
 * clock src: pll
 * sys freq: 96MHz
 * AHB freq: 96MHz
 * APB0 freq: 48MHz
 * APB1 freq: 48MHz
 */
/* 0: msi
 * 1: dcxo
 * 2: pll
 */
#ifndef CONFIG_CLK_SYS_SRC
#define CONFIG_CLK_SYS_SRC                                  2
#endif

#ifndef CONFIG_CLK_SYS_DIVISOR
#define CONFIG_CLK_SYS_DIVISOR                              1
#endif

#ifndef CONFIG_CLK_AHB_DIVISOR
#define CONFIG_CLK_AHB_DIVISOR                              1
#endif

#ifndef CONFIG_CLK_APB0_DIVISOR
#define CONFIG_CLK_APB0_DIVISOR                             2
#endif

#ifndef CONFIG_CLK_APB1_DIVISOR
#define CONFIG_CLK_APB1_DIVISOR                             2
#endif

/* lse
 * state: disable
 */
#ifndef CONFIG_CLK_LSE_EN
#define CONFIG_CLK_LSE_EN                                   0
#endif

#ifndef CONFIG_CLK_LSE_FREQ
#define CONFIG_CLK_LSE_FREQ                                 32768
#endif


/* lpclock
 * state: enable (must)
 * clock src: LSI
 * lpclock freq:  1MHz
 * intr clock: enable, 1MHz
 * frac clock: enable, 32768Hz
 */
/* 1: LSI
 * 0: LSE 
 */
#ifndef CONFIG_CLK_LPCLK_SRC
#define CONFIG_CLK_LPCLK_SRC                                1
#endif

#ifndef CONFIG_CLK_LPCLK_INTR_EN
#define CONFIG_CLK_LPCLK_INTR_EN                            1
#endif

#ifndef CONFIG_CLK_LPCLK_INTR_DIVISOR
#define CONFIG_CLK_LPCLK_INTR_DIVISOR                       32
#endif

#ifndef CONFIG_CLK_LPCLK_FRAC_EN
#define CONFIG_CLK_LPCLK_FRAC_EN                            1
#endif

#ifndef CONFIG_CLK_LPCLK_FRAC_QUOTIENT
#define CONFIG_CLK_LPCLK_FRAC_QUOTIENT                      30
#endif

#ifndef CONFIG_CLK_LPCLK_FRAC_REMAINDER
#define CONFIG_CLK_LPCLK_FRAC_REMAINDER                     16920
#endif

#ifndef CONFIG_CLK_LPCLK_FRAC_DENOMINATOR
#define CONFIG_CLK_LPCLK_FRAC_DENOMINATOR                   32768
#endif


/* disable CPUF to AHB H2H  */
#ifndef CONFIG_H2C_DISABLE
#define CONFIG_H2C_DISABLE                                  1
#endif


/*
 * FLASH_CLCOK_SRC_SYS         = 0,
 * FLASH_CLCOK_SRC_MSI         = 1,
 * FLASH_CLCOK_SRC_DCXO        = 2,
 * FLASH_CLCOK_SRC_PLL_DEV1    = 3,
 */
#ifndef CONFIG_XIP_CLK_SRC
#define CONFIG_XIP_CLK_SRC                                  0
#endif

/* 
 * 96mhz / 2 = 48 mhz
 */
#ifndef CONFIG_XIP_CLK_DIV
#define CONFIG_XIP_CLK_DIV                                  2
#endif

/* socldo voltage
 * if sysclock > 112mhz, set to 15
 */
#ifndef CONFIG_PMU_SOCLDO_ACTIVE_VOLTAGE
#define CONFIG_PMU_SOCLDO_ACTIVE_VOLTAGE                    0
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

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RS613X_CONFIG_CPUS_96MHZ_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

