/**
 ******************************************************************************
 * @file    hal clock calib.c
 * @brief   hal clock calib define.
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
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_common.h"

#if CONFIG_DRIVER_CLOCK_CALIB


#include "hal_clock_calib.h"

#include "ll_utils.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_pmu.h"
#include "ll_timer.h"
#include "ll_clock_calib.h"

#ifndef CONFIG_CLOCK_CALIB_LOG_LEVEL
#define CONFIG_CLOCK_CALIB_LOG_LEVEL    LEVEL_DIS
#endif

#define LOG_MODULE                      "CALIB"
#define LOG_LEVEL                       CONFIG_CLOCK_CALIB_LOG_LEVEL
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define CLOCK_CALIB_MODE_LSI_TRACERSE_SEARCH    0
#define CLOCK_CALIB_MODE_LSI_BINARY_SEARCH      1

#define CLOCK_LSI_CALIB_MODE              CLOCK_CALIB_MODE_LSI_BINARY_SEARCH

#define CALIB_LSI_CLOSEST_MAX             0xFFFFFF
#define CALIB_LSI_RTRINM_BIT              (LSI_R_TRIM_PosH - LSI_R_TRIM_PosL + 1)
#define CALIB_LSI_ITRINM_BIT              (4)
#define CALIB_LSI_ITRINM_MAX_NUM          (1 << CALIB_LSI_ITRINM_BIT)
#define CALIB_LSI_ITRINM_MSK              (CALIB_LSI_ITRINM_MAX_NUM - 1)

#define CALIB_MSI_ITRINM_BIT              (MSI_CAP_TRIM_PosH - MSI_CAP_TRIM_PosL + 1)
#define CALIB_MSI_ITRINM_MAX_NUM          (1 << CALIB_MSI_ITRINM_BIT)

#define CALIB_MSI_CLOSEST_MAX             0xFFFFFF

#define CALIB_MSI_RTRINM_BIT              (MSI_FREQ_SEL_PosH - MSI_FREQ_SEL_PosL + 1)
#define CALIB_MSI_RTRINM_MAX_BUM          (1 << CALIB_MSI_RTRINM_BIT)
#define CALIB_MSI_CFTRIM_BIT              (MSI_CFTRIM_TRIM_PosH - MSI_CFTRIM_TRIM_PosL + 1)
#define CALIB_MSI_CFTRIM_MAX_BUM          (1 << CALIB_MSI_CFTRIM_BIT)


#define CALIB_CAL_TARGET_MSI_CNT           (131072)  // 65536
#define CALIB_CAL_TARGET_250KLSI_CNT       (8192)
#define CALIB_CAL_TARGET_500KLSI_CNT       (CALIB_CAL_TARGET_250KLSI_CNT * 2)
#define CALIB_CAL_TARGET_1000KLSI_CNT      (CALIB_CAL_TARGET_250KLSI_CNT * 2)
#define CALIB_CAL_TARGET_2000KLSI_CNT      (CALIB_CAL_TARGET_1000KLSI_CNT * 2)

#define CALIB_CAL_LSI_DELAY_US             (100)
#define CALIB_CAL_LSI_TIMEOUT_US           (500000)

#define CALIB_LSI_BASE_FREQ250K            250000
#define CALIB_LSI_BASE_FREQ500K            500000
#define CALIB_LSI_BASE_FREQ1000K           1000000
#define CALIB_LSI_BASE_FREQ2000K           2000000

#define CALIB_LSI_BASE_FREQ256K_TRIM      3
#define CALIB_LSI_BASE_FREQ500K_TRIM      2
#define CALIB_LSI_BASE_FREQ1000K_TRIM     1
#define CALIB_LSI_BASE_FREQ2000K_TRIM     0

#define CALIB_LSI_IBUF_FREQ250K_TRIM      0x08
#define CALIB_LSI_IBUF_FREQ500K_TRIM      0x0C
#define CALIB_LSI_IBUF_FREQ1000K_TRIM     0x00
#define CALIB_LSI_IBUF_FREQ2000K_TRIM     0x00

#define CALIB_LSI_CAL_FREQ250K_DELAY_US   400
#define CALIB_LSI_CAL_FREQ500K_DELAY_US   200
#define CALIB_LSI_CAL_FREQ1000K_DELAY_US  100
#define CALIB_LSI_CAL_FREQ2000K_DELAY_US  100

/* Private macros.
 * ----------------------------------------------------------------------------
 */

#define CALIB_STD_CLK_FREQ                      32000000

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

static void Calib_TimeStart(uint32_t us);
static int Calib_TimeGetDone(void);
static void Calib_TimeStop(void);
static void Calib_TimeDelay(uint32_t us);

static void Calib_Open(uint32_t stdClk, uint32_t calibClk, uint32_t calibTime);
static void Calib_Close(void);
static uint32_t Calib_Start(void);

static uint32_t Calib_GetLsiCurrFreq(uint32_t calibFreq, uint32_t calibTime);
static uint32_t Calib_GetMsiCurrFreq(uint32_t calibFreq, uint32_t calibTime);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */


uint32_t HAL_CLOCK_MsiGetFreq(void)
{
    return Calib_GetMsiCurrFreq(CALIB_STD_CLK_FREQ, CALIB_CAL_TARGET_MSI_CNT);
}


uint32_t HAL_CLOCK_MsiCalib(uint32_t calibFreq, uint32_t *pTrim)
{
    uint32_t scan_cap_trim = 0;
    uint32_t idx = 0;

    uint32_t cap_freq = 0;

    /* find closest */
    uint32_t closest_trim = 0;
    uint32_t curr_freq = 0;
    uint32_t curr_diff = 0;
    uint32_t cap_diff = CALIB_LSI_CLOSEST_MAX;

    /* default val */
    uint8_t bias_trim = 0x07;
    uint8_t freq_trim = 0x07;
    uint8_t cap_trim = 0x40;
    uint8_t cftrim_trim = 0x10;


    LL_CALIB_MSICfgSet(bias_trim, cap_trim, freq_trim, cftrim_trim);

    /* 1. freq_trim large stride */
//    cap_trim = msi_cfg->freq_trim;
//    closest_trim = msi_cfg->freq_trim;
//    for (idx = 0; idx < CALIB_MSI_RTRINM_BIT; idx++) {
//
//        cap_freq = HAL_AC_GetMsiCurrFreq(src_freq);
//
//        if (cap_freq >= dest_freq) {
//            cap_trim &= ~(1 << (CALIB_MSI_RTRINM_BIT - 1 - idx));
//        } else {
//            cap_trim |= (1 << (CALIB_MSI_RTRINM_BIT - 1 - idx));
//        }
//
//        if (CALIB_MSI_RTRINM_BIT - 2 >= idx) {
//            cap_trim |= (1 << (CALIB_MSI_RTRINM_BIT - 2 - idx));
//        }
//
//        msi_cfg->freq_trim = cap_trim;
//
//        LL_AC_CfgMSIClock(msi_cfg->bias_trim, msi_cfg->cap_trim, msi_cfg->freq_trim, msi_cfg->cftrim_trim);
//
//    }
//
//    cap_freq = HAL_AC_GetMsiCurrFreq(src_freq);
//    curr_diff = (dest_freq >= cap_freq) ? (dest_freq - cap_freq) : (cap_freq - dest_freq);
//    if (curr_diff < cap_diff) {
//        cap_diff = curr_diff;
//        curr_freq = cap_freq;
//        closest_trim = cap_trim;
//    }
//
//    msi_cfg->freq_trim = closest_trim;

#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
    uint32_t itcap_freq[CALIB_MSI_ITRINM_BIT + 1];
    uint32_t cfcap_freq[CALIB_MSI_CFTRIM_BIT];
    uint32_t itcap_freq_final;
    uint32_t cfcap_freq_final;
#endif

    /* 2. cap_trim medium stride */
    scan_cap_trim = cap_trim;
    closest_trim = cap_trim;
    cap_freq = Calib_GetMsiCurrFreq(CALIB_STD_CLK_FREQ, CALIB_CAL_TARGET_MSI_CNT);
    curr_diff = (calibFreq >= cap_freq) ? (calibFreq - cap_freq) : (cap_freq - calibFreq);
    if (curr_diff < cap_diff) {
        cap_diff = curr_diff;
        curr_freq = cap_freq;
        closest_trim = scan_cap_trim;
    }

#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
    itcap_freq[CALIB_MSI_ITRINM_BIT] = cap_freq;
    itcap_freq_final = curr_freq;
#endif


    for (idx = 0; idx < CALIB_MSI_ITRINM_BIT; idx++) {
        if (cap_freq >= calibFreq) {
            scan_cap_trim &= ~(1 << (CALIB_MSI_ITRINM_BIT - 1 - idx));
        } else {
            scan_cap_trim |= (1 << (CALIB_MSI_ITRINM_BIT - 1 - idx));
        }

        if (CALIB_MSI_ITRINM_BIT - 2 >= idx) {
            scan_cap_trim |= (1 << (CALIB_MSI_ITRINM_BIT - 2 - idx));
        }

        cap_trim = scan_cap_trim;

        LL_CALIB_MSICfgSet(bias_trim, cap_trim, freq_trim, cftrim_trim);

        cap_freq = Calib_GetMsiCurrFreq(CALIB_STD_CLK_FREQ, CALIB_CAL_TARGET_MSI_CNT);
#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
        itcap_freq[idx] = cap_freq;
#endif
        curr_diff = (calibFreq >= cap_freq) ? (calibFreq - cap_freq) : (cap_freq - calibFreq);
        if (curr_diff < cap_diff) {
            cap_diff = curr_diff;
            curr_freq = cap_freq;
            closest_trim = scan_cap_trim;
        }
    }

#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
    itcap_freq_final = curr_freq;
#endif


    cap_trim = closest_trim;

    /* 3. cftrim_trim small stride */
    scan_cap_trim = cftrim_trim;
    closest_trim = cftrim_trim;
    for (idx = 0; idx < CALIB_MSI_CFTRIM_BIT; idx++) {
        if (cap_freq >= calibFreq) {
            scan_cap_trim &= ~(1 << (CALIB_MSI_CFTRIM_BIT - 1 - idx));
        } else {
            scan_cap_trim |= (1 << (CALIB_MSI_CFTRIM_BIT - 1 - idx));
        }

        if (CALIB_MSI_CFTRIM_BIT - 2 >= idx) {
            scan_cap_trim |= (1 << (CALIB_MSI_CFTRIM_BIT - 2 - idx));
        }

        cftrim_trim = scan_cap_trim;

        LL_CALIB_MSICfgSet(bias_trim, cap_trim, freq_trim, cftrim_trim);

        cap_freq = Calib_GetMsiCurrFreq(CALIB_STD_CLK_FREQ, CALIB_CAL_TARGET_MSI_CNT);
#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
        cfcap_freq[idx] = cap_freq;
#endif

        curr_diff = (calibFreq >= cap_freq) ? (calibFreq - cap_freq) : (cap_freq - calibFreq);
        if (curr_diff < cap_diff) {
            cap_diff = curr_diff;
            curr_freq = cap_freq;
            closest_trim = scan_cap_trim;
        }
    }

#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
    cfcap_freq_final = curr_freq;
#endif


    cftrim_trim = closest_trim;
    bias_trim = freq_trim;
    LL_CALIB_MSICfgSet(bias_trim, cap_trim, freq_trim, cftrim_trim);

    if (pTrim != NULL) {
        *pTrim = LL_CALIB_MSICfgRead();
    }


    LOG_INF("msi calib freq: %d", curr_freq);
    LOG_INF("msi calib: bias: %d, cap: %d, freq: %d, cf: %d", bias_trim, cap_trim, freq_trim, cftrim_trim);

#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
    LOG_DBG("itcap_freq_final : %d", itcap_freq_final);
    LOG_DBG("cfcap_freq_final : %d", cfcap_freq_final);

    for (uint32_t itidx = 0; itidx <= CALIB_MSI_ITRINM_BIT; itidx++) {
        LOG_DBG("%d : %d", itidx, itcap_freq[itidx]);
    }

    for (uint32_t cfidx = 0; cfidx < CALIB_MSI_CFTRIM_BIT; cfidx++) {
        LOG_DBG("%d : %d", cfidx, cfcap_freq[cfidx]);
    }
#endif

    return curr_freq;
}


uint32_t HAL_CLOCK_LsiGetFreq(void)
{
    uint32_t lsi_idx = 0;
    uint32_t calibTime = 0;

    lsi_idx = LL_CALIB_LSIIdxGet();
    switch (lsi_idx) {
    case CALIB_LSI_BASE_FREQ2000K_TRIM:
        calibTime = CALIB_CAL_TARGET_2000KLSI_CNT;
        break;

    case CALIB_LSI_BASE_FREQ1000K_TRIM:
        calibTime = CALIB_CAL_TARGET_1000KLSI_CNT;
        break;

    case CALIB_LSI_BASE_FREQ500K_TRIM:
        calibTime = CALIB_CAL_TARGET_500KLSI_CNT;
        break;

    case CALIB_LSI_BASE_FREQ256K_TRIM:
        calibTime = CALIB_CAL_TARGET_250KLSI_CNT;
        break;

    default:
        return 0;
    }


    return Calib_GetLsiCurrFreq(CALIB_STD_CLK_FREQ, calibTime);
}


uint32_t HAL_CLOCK_LsiCalib(uint32_t calibFreq, uint32_t *pTrim)
{
    uint32_t cap_freq = 0;
    uint32_t tmp_dest_freq = calibFreq;
    uint32_t delay_us = 0;

    /* find closest */
    uint32_t closest_idx = 0;
    uint32_t curr_diff = 0;
    uint32_t curr_freq = 0;
    uint32_t cap_diff = CALIB_LSI_CLOSEST_MAX;

    uint8_t lsi_ibuf_trim = 0;

    /* default val */
    uint8_t r_trim;
    uint8_t lsi_trim;
    uint8_t decap2 = 0x0;
    uint8_t decap1 = 0x03;

    uint32_t calibTime = 0;

#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
    uint32_t lsi_cap_freq[0x10];
#endif


    if (calibFreq >= CALIB_LSI_BASE_FREQ2000K) {
        r_trim = CALIB_LSI_BASE_FREQ2000K_TRIM;
        lsi_trim  = 1 << 4;
        calibTime = CALIB_CAL_TARGET_2000KLSI_CNT;
        delay_us = CALIB_LSI_CAL_FREQ2000K_DELAY_US;
        lsi_ibuf_trim = CALIB_LSI_IBUF_FREQ2000K_TRIM;

    } else if (calibFreq >= CALIB_LSI_BASE_FREQ1000K) {
        r_trim = CALIB_LSI_BASE_FREQ1000K_TRIM;
        lsi_trim  = 0 << 4;
        calibTime = CALIB_CAL_TARGET_1000KLSI_CNT;
        delay_us = CALIB_LSI_CAL_FREQ1000K_DELAY_US;
        lsi_ibuf_trim = CALIB_LSI_IBUF_FREQ1000K_TRIM;

    } else if (calibFreq >= CALIB_LSI_BASE_FREQ500K) {
        r_trim = CALIB_LSI_BASE_FREQ500K_TRIM;
        lsi_trim  = 3 << 4;
        calibTime = CALIB_CAL_TARGET_500KLSI_CNT;
        delay_us = CALIB_LSI_CAL_FREQ500K_DELAY_US;
        lsi_ibuf_trim = CALIB_LSI_IBUF_FREQ500K_TRIM;

    } else {
        r_trim = CALIB_LSI_BASE_FREQ256K_TRIM;
        lsi_trim  = 2 << 4;
        calibTime = CALIB_CAL_TARGET_250KLSI_CNT;
        delay_us = CALIB_LSI_CAL_FREQ250K_DELAY_US;
        lsi_ibuf_trim = CALIB_LSI_IBUF_FREQ250K_TRIM;
    }

    LL_CALIB_CfgLsiIbufTrim(lsi_ibuf_trim);

#if (CLOCK_LSI_CALIB_MODE == CLOCK_CALIB_MODE_LSI_BINARY_SEARCH)
    uint32_t scan_lsi_trim = 0x08;
    uint32_t offset = 0x08;

    LL_CALIB_LSICfgSet(lsi_trim | ((scan_lsi_trim + offset) & CALIB_LSI_ITRINM_MSK),
                               r_trim,
                               decap1,
                               decap2);

    Calib_TimeDelay(delay_us);

    cap_freq = Calib_GetLsiCurrFreq(CALIB_STD_CLK_FREQ, calibTime);

    curr_diff = (tmp_dest_freq >= cap_freq) ? (tmp_dest_freq - cap_freq) : (cap_freq - tmp_dest_freq);
    if (curr_diff < cap_diff) {
        cap_diff = curr_diff;
        curr_freq = cap_freq;
        closest_idx = scan_lsi_trim;
    }

    for (uint8_t step = 0; step < CALIB_LSI_ITRINM_BIT; step++) {
        if (cap_freq >= tmp_dest_freq) {
            scan_lsi_trim |= (1 << (CALIB_LSI_ITRINM_BIT - 1 - step));
        } else {
            scan_lsi_trim &= ~(1 << (CALIB_LSI_ITRINM_BIT - 1 - step));
        }

        if (CALIB_LSI_RTRINM_BIT - 2 >= step) {
            scan_lsi_trim |= (1 << (CALIB_LSI_ITRINM_BIT - 2 - step));
        }

        LL_CALIB_LSICfgSet(lsi_trim | ((scan_lsi_trim + offset) & CALIB_LSI_ITRINM_MSK),
                               r_trim,
                               decap1,
                               decap2);

        Calib_TimeDelay(delay_us);

        cap_freq = Calib_GetLsiCurrFreq(CALIB_STD_CLK_FREQ, calibTime);
#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
        lsi_cap_freq[step] = cap_freq;
#endif

        curr_diff = (tmp_dest_freq >= cap_freq) ? (tmp_dest_freq - cap_freq) : (cap_freq - tmp_dest_freq);
        if (curr_diff < cap_diff) {
            cap_diff = curr_diff;
            curr_freq = cap_freq;
            closest_idx = scan_lsi_trim;
        }
    }
    lsi_trim |= ((closest_idx + offset) & CALIB_LSI_ITRINM_MSK);

#else

    for (int idx = 0; idx <= 0xF; idx++) {
        LL_CALIB_LSICfgSet(lsi_trim | idx, r_trim, decap1, decap2);

        Calib_TimeDelay(delay_us);

        cap_freq = Calib_GetLsiCurrFreq(CALIB_STD_CLK_FREQ, calibTime);
#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
        lsi_cap_freq[idx] = cap_freq;
#endif

        curr_diff = (tmp_dest_freq >= cap_freq) ? (tmp_dest_freq - cap_freq) : (cap_freq - tmp_dest_freq);
        if (curr_diff < cap_diff) {
            cap_diff = curr_diff;
            curr_freq = cap_freq;
            closest_idx = idx;
        }
    }
    lsi_trim |= closest_idx;
#endif


    LL_CALIB_LSICfgSet(lsi_trim, r_trim, decap1, decap2);

    if (pTrim != NULL) {
        *pTrim = LL_CALIB_LSICfgRead();
    }


    LOG_INF("lsi calib freq: %d", curr_freq);
    LOG_INF("lsi calib: lsi_trim: %d, r_trim: %d, decap1: %d, decap2: %d", lsi_trim, r_trim, decap1, decap2);

#if (CONFIG_CLOCK_CALIB_LOG_LEVEL == LEVEL_DBG)
    for (uint32_t idx = 0; idx < 0x10; idx++) {
        LOG_DBG("%d : %d", idx, lsi_cap_freq[idx]);
    }
#endif


    return curr_freq;
}


static uint32_t Calib_GetLsiCurrFreq(uint32_t calibFreq, uint32_t calibTime)
{
    uint32_t cap_tick = 0;
    uint32_t cap_freq = 0;

    Calib_Open(LL_CALIB_HSRC_DCXO, LL_CALIB_LSRC_LSI, calibTime);

    cap_tick = Calib_Start();

    Calib_Close();

    cap_freq = calibTime * (calibFreq / 1000) / cap_tick;
    cap_freq = cap_freq * 1000;

    return cap_freq;
}


static uint32_t Calib_GetMsiCurrFreq(uint32_t calibFreq, uint32_t calibTime)
{
    uint32_t cap_tick = 0;
    uint32_t cap_freq = 0;

    Calib_Open(LL_CALIB_HSRC_DCXO, LL_CALIB_LSRC_MSI, calibTime);

    cap_tick = Calib_Start();

    Calib_Close();

    cap_freq = calibTime * (calibFreq / 1000) / cap_tick;
    cap_freq = cap_freq * 1000;

    return cap_freq;
}

static void Calib_Open(uint32_t stdClk, uint32_t calibClk, uint32_t calibTime)
{
    LL_CALIB_ReleaseCALFreq();
    LL_CALIB_SetCALHclkSource(stdClk);
    LL_CALIB_SetCALLclkSource(calibClk);
    LL_CALIB_SetCALTargetCnt(calibTime);
    LL_CALIB_EnableCALLclk();
    LL_CALIB_EnableCALHclk();
    /*
     * 32us - low speed clk stabilization time
     * 40us - lsi clk stabilization time
     */
    Calib_TimeDelay(32);
}


static void Calib_Close(void)
{
    LL_CALIB_DisableCALLclk();
    LL_CALIB_DisableCALHclk();
    LL_CALIB_ResetCALFreq();
    LL_CALIB_SetCALTargetCnt(0);
}


static uint32_t Calib_Start(void)
{
    uint32_t cap_tick = 0;

    /* time out */
    Calib_TimeStart(CALIB_CAL_LSI_TIMEOUT_US);

    LL_CALIB_EnableCALCap();
    do {
        if (LL_CALIB_IsCALFinish()) {
            cap_tick = LL_CALIB_GetCALCnt();
            LL_CALIB_ClrCALFinish();
            break;
        } else if (Calib_TimeGetDone()) {
            break;
        }
    } while (1);

    return cap_tick;
}


static void Calib_TimeStart(uint32_t us)
{
    /* use dcxo, 32MHz */
    LL_RCC_TMR0_SetClockSource(2);
    LL_RCC_TMR0_SetPrescaler(1);
    LL_RCC_TMR0_DisableBusClock();
    LL_RCC_TMR0_DisableClock();
    LL_RCC_TMR0_Reset();
    LL_RCC_TMR0_EnableBusClock();
    LL_RCC_TMR0_EnableClock();

    LL_TIM_DisableInterruptMask(TIMR0_DEV);
    LL_TIM_SetUserDefinedMode(TIMR0_DEV);
    LL_TIM_SetLoadCounter(TIMR0_DEV, us * 32);
    LL_TIM_Enable(TIMR0_DEV);
}

static int Calib_TimeGetDone(void)
{
    return LL_TIM_GetInterruptStatus(TIMR0_DEV);
}

static void Calib_TimeStop(void)
{
    LL_TIM_Disable(TIMR0_DEV);

    LL_RCC_TMR0_DisableBusClock();
    LL_RCC_TMR0_DisableClock();
    LL_RCC_TMR0_Reset();
    LL_RCC_TMR0_DisableClock();
    LL_RCC_TMR0_DisableBusClock();
}

static void Calib_TimeDelay(uint32_t us)
{
    Calib_TimeStart(us);
    do {
        if (Calib_TimeGetDone()) {
            Calib_TimeStop();
            break;
        }
    } while (1);
}


#endif /* CONFIG_DRIVER_CLOCK_CALIB */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

