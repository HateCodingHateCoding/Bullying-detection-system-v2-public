/**
 ******************************************************************************
 * @file    ll_clock_calib.h
 * @brief   ll clock calib define.
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

#ifndef _LL_CLOCK_CALIB_H
#define _LL_CLOCK_CALIB_H

/* Includes.
 * ----------------------------------------------------------------------------
 */


 #ifdef __cplusplus
extern "C" {
#endif


#include "types.h"
#include "hw_rcc.h"
#include "hw_syscfg.h"
#include "soc.h"


/* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
#define LL_CALIB_LSRC_LSI                   0
#define LL_CALIB_LSRC_LP                    1
#define LL_CALIB_LSRC_MSI                   2
#define LL_CALIB_LSRC_M50_32K               3

#define LL_CALIB_HSRC_MSI                   0
#define LL_CALIB_HSRC_DCXO                  1

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

__HW_STATIC_INLINE
void LL_CALIB_SetCALTargetCnt(uint32_t traget_cnt)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, LFCLK_CNT_TARGET_Msk, LFCLK_CNT_TARGET_Pos, traget_cnt);
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_GetCALTargetCnt(void)
{
    return HW_GET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, LFCLK_CNT_TARGET_Msk, LFCLK_CNT_TARGET_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_IsCALFinish(void)
{
    return HW_GET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, CAL_FINISH_FLAG_Msk, CAL_FINISH_FLAG_Pos);
}

__HW_STATIC_INLINE
void LL_CALIB_ClrCALFinish(void)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, CAL_FINISH_FLAG_Msk, CAL_FINISH_FLAG_Pos, 1);
}

__HW_STATIC_INLINE
void LL_CALIB_EnableCALCap(void)
{
    /*
     * LL_AC_EnableCALCap(void) must be used
     * after LL_AC_IsCALFinish(void)
     */
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, START_Msk, START_Pos, 1);
}

__HW_STATIC_INLINE
void LL_CALIB_DisableCALCap(void)
{
    /*
     * hw set to 0
     * LL_AC_EnableCALCap(void) must be used
     * after LL_AC_IsCALFinish(void)
     */
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_IsEnabledCALCap(void)
{
    /* read away 0 */
    return 0;
}

__HW_STATIC_INLINE
void LL_CALIB_ResetCALFreq(void)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, RESETN_EN_Msk, RESETN_EN_Pos, 0);
}

__HW_STATIC_INLINE
void LL_CALIB_ReleaseCALFreq(void)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, RESETN_EN_Msk, RESETN_EN_Pos, 1);
}

__HW_STATIC_INLINE
void LL_CALIB_EnableCALLclk(void)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, LFCLK_EN_Msk, LFCLK_EN_Pos, 1);
}

__HW_STATIC_INLINE
void LL_CALIB_DisableCALLclk(void)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, LFCLK_EN_Msk, LFCLK_EN_Pos, 0);
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_IsableCALLclk(void)
{
    return HW_GET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, LFCLK_EN_Msk, LFCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_CALIB_SetCALLclkSource(uint32_t source)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, LFCLK_SRC_SEL_Msk, LFCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_GetCALLclkSource(void)
{
    return HW_GET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, LFCLK_SRC_SEL_Msk, LFCLK_SRC_SEL_Pos);
}


__HW_STATIC_INLINE
void LL_CALIB_EnableCALHclk(void)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, HFCLK_EN_Msk, HFCLK_EN_Pos, 1);
}

__HW_STATIC_INLINE
void LL_CALIB_DisableCALHclk(void)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, HFCLK_EN_Msk, HFCLK_EN_Pos, 0);
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_IsableCALHclk(void)
{
    return HW_GET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, HFCLK_EN_Msk, HFCLK_EN_Pos);
}


__HW_STATIC_INLINE
void LL_CALIB_SetCALHclkSource(uint32_t source)
{
    HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, HFCLK_SRC_SEL_Msk, HFCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_GetCALHclkSource(void)
{
    return HW_GET_MSK_VAL(SYSCFG_DEV->SYSCFG_FREQ_CAL_CTRL, HFCLK_SRC_SEL_Msk, HFCLK_SRC_SEL_Pos);
}


__HW_STATIC_INLINE
uint32_t LL_CALIB_GetCALCnt(void)
{
    return HW_GET_MSK_VAL(SYSCFG_DEV->FREQ_CAL_VAL, FREQ_CAL_VAL_Msk, FREQ_CAL_VAL_Pos);
}


__HW_STATIC_INLINE
void LL_CALIB_CfgLsiIbufTrim(uint8_t ibuf_trim)
{
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_IB_BUF_TRIM_Msk, LSI_IB_BUF_TRIM_Pos, ibuf_trim);
}


__HW_STATIC_INLINE
void LL_CALIB_LSICfgWrite(uint32_t trim)
{
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, ANA_LSI_TRIM_Msk, ANA_LSI_TRIM_Pos, ((trim) && 0xFF));
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_R_TRIM_Msk, LSI_R_TRIM_Pos, ((trim >> 8) && 0xFF));
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP1_TRIM_Msk, LSI_DECAP1_TRIM_Pos, 3);
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP2_TRIM_Msk, LSI_DECAP2_TRIM_Pos, 0);
}


__HW_STATIC_INLINE
uint32_t LL_CALIB_LSICfgRead(void)
{
    uint32_t trim = 0;

    trim = HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_R_TRIM_Msk, LSI_R_TRIM_Pos) & 0xFF;
    trim <<= 8;
    trim |= HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, ANA_LSI_TRIM_Msk, ANA_LSI_TRIM_Pos) & 0xFF;

    return trim;
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_LSIIdxGet(void)
{
    return HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_R_TRIM_Msk, LSI_R_TRIM_Pos);
}


__HW_STATIC_INLINE
void LL_CALIB_LSICfgSet(uint32_t lsi_trim, uint32_t r_trim, uint32_t decap1_trim, uint32_t decap2_trim)
{
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, ANA_LSI_TRIM_Msk, ANA_LSI_TRIM_Pos, lsi_trim);
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_R_TRIM_Msk, LSI_R_TRIM_Pos, r_trim);
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP1_TRIM_Msk, LSI_DECAP1_TRIM_Pos, decap1_trim);
    HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP2_TRIM_Msk, LSI_DECAP2_TRIM_Pos, decap2_trim);
}

__HW_STATIC_INLINE
void LL_CALIB_LSICfgGet(uint32_t *lsi_trim, uint32_t *r_trim, uint32_t *decap1_trim, uint32_t *decap2_trim)
{
    *lsi_trim = HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, ANA_LSI_TRIM_Msk, ANA_LSI_TRIM_Pos);
    *r_trim = HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_R_TRIM_Msk, LSI_R_TRIM_Pos);
    *decap1_trim = HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP1_TRIM_Msk, LSI_DECAP1_TRIM_Pos);
    *decap2_trim = HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP2_TRIM_Msk, LSI_DECAP2_TRIM_Pos);
}


__HW_STATIC_INLINE
void LL_CALIB_MSICfgWrite(uint32_t trim)
{
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_BIAS_TRIM_Msk, MSI_BIAS_TRIM_Pos, ((trim) & 0xF));
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CAP_TRIM_Msk, MSI_CAP_TRIM_Pos, ((trim >> 4) & 0x7F));
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_FREQ_SEL_Msk, MSI_FREQ_SEL_Pos, ((trim >> 11) & 0xF));
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CFTRIM_TRIM_Msk, MSI_CFTRIM_TRIM_Pos, ((trim >> 15) & 0x1F));
}

__HW_STATIC_INLINE
uint32_t LL_CALIB_MSICfgRead(void)
{
    uint32_t retTrim = 0;
    uint32_t trim = 0;

    trim = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CFTRIM_TRIM_Msk, MSI_CFTRIM_TRIM_Pos);
    retTrim = (trim  & 0x1F) << 15;
    trim = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_FREQ_SEL_Msk, MSI_FREQ_SEL_Pos);
    retTrim |= (trim  & 0xF) << 11;
    trim = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CAP_TRIM_Msk, MSI_CAP_TRIM_Pos);
    retTrim |= (trim  & 0x7F) << 4;
    trim = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_BIAS_TRIM_Msk, MSI_BIAS_TRIM_Pos);
    retTrim |= trim  & 0xF;

    return retTrim;
}

__HW_STATIC_INLINE
void LL_CALIB_MSICfgSet(uint32_t basic_trim, uint32_t cap_trim, uint32_t freq_sel, uint32_t cftrim_trim)
{
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_BIAS_TRIM_Msk, MSI_BIAS_TRIM_Pos, basic_trim);
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CAP_TRIM_Msk, MSI_CAP_TRIM_Pos, cap_trim);
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_FREQ_SEL_Msk, MSI_FREQ_SEL_Pos, freq_sel);
    HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CFTRIM_TRIM_Msk, MSI_CFTRIM_TRIM_Pos, cftrim_trim);
}

__HW_STATIC_INLINE
void LL_CALIB_MSICfgGet(uint32_t *basic_trim, uint32_t *cap_trim, uint32_t *freq_sel, uint32_t *cftrim_trim)
{
    *basic_trim = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_BIAS_TRIM_Msk, MSI_BIAS_TRIM_Pos);
    *cap_trim = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CAP_TRIM_Msk, MSI_CAP_TRIM_Pos);
    *freq_sel = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_FREQ_SEL_Msk, MSI_FREQ_SEL_Pos);
    *cftrim_trim = HW_GET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CFTRIM_TRIM_Msk, MSI_CFTRIM_TRIM_Pos);
}


#ifdef __cplusplus
}
#endif

#endif /* _LL_CLOCK_CALIB_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

