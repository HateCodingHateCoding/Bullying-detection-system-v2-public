/**
 ******************************************************************************
 * @file    hal_clock.h
 * @brief   hal clock define.
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

#ifndef _HAL_CLOCK_H
#define _HAL_CLOCK_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "hal_types.h"
#include "hal_dev.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */

typedef enum {
    CLOCK_LSI                           = 0,
    CLOCK_MSI                           = 1,
    CLOCK_LSE                           = 2,
    CLOCK_DCXO                          = 3,
    CLOCK_LPCLK                         = 4,
    CLOCK_LPCLK_INTE_DIV                = 5,
    CLOCK_LPCLK_FRAC_DIV                = 6,
    CLOCK_PLL                           = 7,
    CLOCK_PLL_SOC                       = 8,
    CLOCK_PLL_DEV                       = 9,
    CLOCK_PLL_DEV1                      = 10,
    CLOCK_PLL_CDAA                      = 11,
    CLOCK_SYS                           = 12,
    CLOCK_AHB                           = 13,
    CLOCK_APB0                          = 14,
    CLOCK_APB1                          = 15,
    CLOCK_CURR_CPU                      = 16,
    CLOCK_CPUS                          = 17,
    CLOCK_CPUF                          = 18,
    CLOCK_MAX                           = 19
} CLOCK_Type_t;

typedef struct {
    #define CLOCK_LSI_2000KHZ           2000000
    #define CLOCK_LSI_1000KHZ           1000000
    #define CLOCK_LSI_500KHZ            500000
    #define CLOCK_LSI_250KHZ            250000
    uint32_t    lsiFreq;

    #define CLOCK_MSI_64MHZ             64000000
    #define CLOCK_MSI_40MHZ             40000000
    #define CLOCK_MSI_32MHZ             32000000
    uint32_t    msiFreq;

    #define CLOCK_LSIMSI_CFG_TRIM_INPUT 0x00
    #define CLOCK_LSIMSI_CFG_TRIM_EFUSE 0x01
    #define CLOCK_LSIMSI_CFG_TRIM_REG   0x02
    #define CLOCK_LSIMSI_CFG_TRIM_CALIB 0x04
    #define CLOCK_LSIMSI_CFG_TRIM_MAX   0x05

    #define CLOCK_LSIMSI_CFG_FREQ_FIXED 0x00
    #define CLOCK_LSIMSI_CFG_FREQ_REFR  0x10
    uint32_t    lsiCfgMode;
    uint32_t    msiCfgMode;

    uint32_t    lsiTrim;
    uint32_t    msiTrim;
} CLOCK_LsiMsiCfg_t;

typedef struct {
    uint32_t    enable;
    uint32_t    freq;
} CLOCK_LseCfg_t;

typedef struct {
    #define CLOCK_LPCLK_SRC_LSE         0
    #define CLOCK_LPCLK_SRC_LSI         1
    uint32_t    clkSrc;
    uint32_t    inteEnable;
    uint32_t    inteDiv;
    uint32_t    fracEnable;
    uint32_t    fracQuotient;
    uint32_t    fracRemainder;
    uint32_t    fracDenominator;
} CLOCK_LpclkCfg_t;

typedef struct {
    uint32_t    enable;
    uint32_t    freq;
    uint32_t    out_pll;
    uint32_t    out_dev;
} CLOCK_DcxoCfg_t;

typedef struct {
    uint32_t    enable;
    #define CLOCK_PLL_SRC_MSI       0
    #define CLOCK_PLL_SRC_DCXO      1
    uint32_t    clkSrc;
    uint32_t    pllDiv;
    uint32_t    pllMul;

    uint32_t    socEnable;
    uint32_t    socDiv;
    uint32_t    devEnable;
    uint32_t    devDiv;
    uint32_t    dev1Enable;
    uint32_t    dev1Div;
    uint32_t    cdaaEnable;
    uint32_t    cdaaDiv;
} CLOCK_PllCfg_t;

typedef struct {
    #define CLOCK_SYS_SRC_MSI     0
    #define CLOCK_SYS_SRC_DCXO    1
    #define CLOCK_SYS_SRC_PLL     2
    uint32_t    clkSrc;
    /* 1 / 2 / 4 / 8 */
    uint32_t    sysDiv;
    /* 1 / 2 / 4 / 8 */
    uint32_t    ahbDiv;
    /* 1 / 2 / 4 / 8 / 16 */
    uint32_t    apb0Div;
    /* 1 / 2 / 4 / 8 / 16 */
    uint32_t    apb1Div;
} CLOCK_SysCfg_t;


typedef enum {
    CLOCK_MCO_SRC_MSI           = 0,
    CLOCK_MCO_SRC_HSE           = 1,
    CLOCK_MCO_SRC_HSE_M50       = 2,
    CLOCK_MCO_SRC_LSI           = 3,
    CLOCK_MCO_SRC_PLL_DEV       = 4,
    CLOCK_MCO_SRC_PLL_DEV1      = 5,
    CLOCK_MCO_SRC_PLL_SOC       = 6,
    CLOCK_MCO_SRC_AHB           = 7,
    CLOCK_MCO_SRC_LSE           = 8,
    CLOCK_MCO_SRC_LSE_M50       = 9,
    CLOCK_MCO_SRC_PLL_CDAA      = 10,
    CLOCK_MCO_SRC_MAX           = 11,
} CLOCK_McoSrc_t;


typedef enum {
    CLOCK_MCO_SRC_DIV1          = 0,
    CLOCK_MCO_SRC_DIV2          = 1,
    CLOCK_MCO_SRC_DIV4          = 2,
    CLOCK_MCO_SRC_DIV8          = 3,
    CLOCK_MCO_SRC_DIV16         = 4,
    CLOCK_MCO_SRC_DIVMAX        = 5,
} CLOCK_McoDiv_t;


/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
HAL_Dev_t * HAL_CLOCK_Init(void);

int32_t HAL_CLOCK_GetFreq(HAL_Dev_t *clock, CLOCK_Type_t clkType);

HAL_Status_t HAL_CLOCK_ConfigLsiMsi(HAL_Dev_t *clock, CLOCK_LsiMsiCfg_t *pLsiMsi);

HAL_Status_t HAL_CLOCK_ConfigLpclk(HAL_Dev_t *clock, CLOCK_LpclkCfg_t *pLpclk);

HAL_Status_t HAL_CLOCK_ConfigLse(HAL_Dev_t *clock, CLOCK_LseCfg_t *pLse);

HAL_Status_t HAL_CLOCK_ConfigDcxo(HAL_Dev_t *clock, CLOCK_DcxoCfg_t *pDcxo);

HAL_Status_t HAL_CLOCK_ConfigPll(HAL_Dev_t *clock, CLOCK_PllCfg_t *pPll);

HAL_Status_t HAL_CLOCK_ConfigSys(HAL_Dev_t *clock, CLOCK_SysCfg_t *pSys);

#if CONFIG_CLOCK_MCO_ENABEL
HAL_Status_t HAL_CLOCK_EnableMCO(CLOCK_McoSrc_t clk, CLOCK_McoDiv_t div);

HAL_Status_t HAL_CLOCK_DisableMCO(void);
#else

#define HAL_CLOCK_EnableMCO(clk, div)                       (HAL_STATUS_NOT_SUPPORTED)

#define HAL_CLOCK_DisableMCO()                              (HAL_STATUS_NOT_SUPPORTED)

#endif

HAL_Status_t HAL_CLOCK_SetupTimeInit(HAL_Dev_t *clock);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_CLOCK_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

