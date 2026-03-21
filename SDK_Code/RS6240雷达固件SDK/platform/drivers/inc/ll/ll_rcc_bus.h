/**
 **************************************************************************************************
 * @file    psic_ll_rcc_bus.h
 * @brief   reset clock hadware define for bus.
 * @attention
 *          1. sys1 clk source: msi/hse/spll_soc, clk prescaler: 1/2/4/8
 *          2. sys0 clk source: sys1_clk, clk prescaler: 1/2/4/8
 *          3. ahb clk source: sys0_clk, clk prescaler: 1
 *          4. apb0 clk source: ahb_clk, clk prescaler: 1/2/4/8/16
 *          5. apb1 clk source: ahb_clk, clk prescaler: 1/2/4/8/16
 *          6. cpus clk source: sys0_clk, clk prescaler: 1
 *          7. cpuf clk source: sys1_clk, clk prescaler: 1
 *
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_RCC_BUS_H
#define _PSIC_LL_RCC_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "soc.h"
#include "types.h"
#include "hw_ccmu1.h"
#include "hw_ccmu2.h"
#include "hw_pmu.h"
#include "hw_syscfg.h"
#include "hw_swcfg.h"
#include "hw_rcc.h"
#include "hw_misc.h"

/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup psic_ll_rcc
  * @{
  */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup reset clock define
  * @{
  */

#define LL_RCC_LSI_SRC_RC					0
#define LL_RCC_LSI_SRC_INPUT				1

#define LL_RCC_LSI_INPUT_PIN_PA07			0
#define LL_RCC_LSI_INPUT_PIN_PA09			1

#define LL_RCC_LSI_OUTPUT_PIN_PA09			0
#define LL_RCC_LSI_OUTPUT_PIN_PA12			1

#define LL_RCC_LSE_SRC_INPUT				0
#define LL_RCC_LSE_SRC_XO					1

#define LL_RCC_LPCLK_SRC_LSI				1
#define LL_RCC_LPCLK_SRC_LSE				0


#define LL_RCC_MSI_SRC_RC					0
#define LL_RCC_MSI_SRC_INPUT				1

#define LL_RCC_MSI_INPUT_PIN_PA05			0
#define LL_RCC_MSI_INPUT_PIN_PA08			1




/** @defgroup pll clock source
  * @{
  */
#define LL_RCC_PLL_SRC_MSI					0
#define LL_RCC_PLL_SRC_HSE					1
/**
  * @}
  */

#define LL_RCC_SYS_SRC_MSI					0
#define LL_RCC_SYS_SRC_HSE					1
#define LL_RCC_SYS_SRC_PLL					2


/** @defgroup pll clock division
  * @{
  */
#define LL_RCC_PLL_DIV_MUL_COVEN(div_mul)	(div_mul - 1)
/**
  * @}
  */


/** @defgroup CLOCK DIV
  * @{
  */
#define LL_RCC_CLK_DIV_1					0
#define LL_RCC_CLK_DIV_2					1
#define LL_RCC_CLK_DIV_4					2
#define LL_RCC_CLK_DIV_8					3
#define LL_RCC_CLK_DIV_16					4
#define LL_RCC_CLK_DIV_32					5
#define LL_RCC_CLK_DIV_64					6
#define LL_RCC_CLK_DIV_128					7
#define LL_RCC_CLK_DIV_MAX					LL_RCC_CLK_DIV_128
/**
  * @}
  */


/**
  * @}
  */


/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */


/** @defgroup LSE
  * @{
  */
__HW_STATIC_INLINE
void LL_RCC_LSE_Enable(void)
{
	HW_SET_MSK(RCC_DEV->RCC_XO32K, XO32K_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSE_Disable(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_XO32K, XO32K_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LSE_IsReady(void)
{
	return HW_TEST_BIT(RCC_DEV->RCC_STATUS, XO32K_OK_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LSE_CfgClock(uint32_t bias_trim)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_XO32K, XO32K_BIAS_TRIM_Msk, XO32K_BIAS_TRIM_Pos, bias_trim);
}

__HW_STATIC_INLINE
void LL_RCC_LSE_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_XO32K, XO32K_SRC_SEL_Msk, XO32K_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LSE_GetClockSource(void)
{
    return HW_GET_MSK_VAL(RCC_DEV->RCC_XO32K, XO32K_SRC_SEL_Msk, XO32K_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LSE_EnableClkIn(void)
{
	HW_SET_MSK(RCC_DEV->RCC_XO32K, CKIN_32K_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSE_DisableClkIn(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_XO32K, CKIN_32K_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LSE_IsEnabledClkIn(void)
{
    return HW_TEST_BIT(RCC_DEV->RCC_XO32K, CKIN_32K_EN_Pos);
}
/**
  * @}
  */




/** @defgroup LSI
  * @{
  */
__HW_STATIC_INLINE
void LL_RCC_LSI_Enable(void)
{
	HW_SET_MSK(RCC_DEV->RCC_LSI_CTRL, LSI_CG_EN_Msk | ANA_LSI_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_Disable(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_LSI_CTRL, LSI_CG_EN_Msk | ANA_LSI_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_RTCSW2_CG_Enable(void)
{
	HW_SET_MSK(RCC_DEV->RCC_LSI_CTRL, LSI_CG_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_RTCSW2_CG_Disable(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_LSI_CTRL, LSI_CG_EN_Msk);
}


__HW_STATIC_INLINE
uint32_t LL_RCC_LSI_IsReady(void)
{
	return HW_TEST_BIT(RCC_DEV->RCC_STATUS, LSI_OK_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_CfgClock(uint32_t ana_trim, uint32_t r_trim, uint32_t decap2_trim, uint32_t decap1_trim, uint32_t buf_trim)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, ANA_LSI_TRIM_Msk, ANA_LSI_TRIM_Pos, ana_trim);
	HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_R_TRIM_Msk, LSI_R_TRIM_Pos, r_trim);
	HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP2_TRIM_Msk, LSI_DECAP2_TRIM_Pos, decap2_trim);
	HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_DECAP1_TRIM_Msk, LSI_DECAP1_TRIM_Pos, decap1_trim);
	HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LSI_IB_BUF_TRIM_Msk, LSI_IB_BUF_TRIM_Pos, buf_trim);
}

/*
 * 0: lsi rc
 * 1: clk_in
 */
__HW_STATIC_INLINE
void LL_RCC_LSI_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_LPCLK_CTRL, LSI_SRC_SEL_Msk, LSI_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LSI_GetClockSource(void)
{
    return HW_SET_MSK(RCC_DEV->RCC_LPCLK_CTRL, LSI_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_EnableClkIn(void)
{
	HW_SET_MSK(RCC_DEV->RCC_LPCLK_CTRL, LCLKIN_PINMUX_EN_Msk | LCLKIN_CG_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_DisableClkIn(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_LPCLK_CTRL, LCLKIN_PINMUX_EN_Msk | LCLKIN_CG_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LSI_IsEnabledClkIn(void)
{
    return 	HW_TEST_BIT(RCC_DEV->RCC_LPCLK_CTRL, LCLKIN_PINMUX_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_SetClkInPin(uint32_t pin)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_LPCLK_CTRL, LCLKIN_PINMUX_SRC_SEL_Msk, LCLKIN_PINMUX_SRC_SEL_Pos, pin);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_EnableClkOut(void)
{
	HW_SET_MSK(RCC_DEV->RCC_LPCLK_CTRL, LCLKOUT_PINMUX_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_DisableClkOut(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_LPCLK_CTRL, LCLKOUT_PINMUX_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LSI_IsEnabledClkOut(void)
{
    return 	HW_TEST_BIT(RCC_DEV->RCC_LPCLK_CTRL, LCLKOUT_PINMUX_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_SetClkOutPin(uint32_t pin)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_LPCLK_CTRL, LCLKOUT_PINMUX_SEL_Msk, LCLKOUT_PINMUX_SEL_Pos, pin);
}

__HW_STATIC_INLINE
void LL_RCC_LSI_SetClkOutDriver(uint32_t level)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_LPCLK_CTRL, LCLKOUT_PIN_DRIVING_Msk, LCLKOUT_PIN_DRIVING_Pos, level);
}
/**
  * @}
  */




/** @defgroup LPCLK
  * @{
  */

/*
 * 0: lse
 * 1: lsi
 */
__HW_STATIC_INLINE
void LL_RCC_LPCLK_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_LPCLK_CTRL, LPCLK_SRC_SEL_Msk, LPCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LPCLK_GetClockSource(void)
{
    return HW_GET_MSK_VAL(RCC_DEV->RCC_LPCLK_CTRL, LPCLK_SRC_SEL_Msk, LPCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_EnableToPMU(void)
{
	HW_SET_MSK(RCC_DEV->RCC_RC6M_CTRL, RC6M_FORCE_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_DisableToPMU(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_RC6M_CTRL, RC6M_FORCE_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LPCLK_IsEnabledToPMU(void)
{
    return HW_TEST_BIT(RCC_DEV->RCC_RC6M_CTRL, RC6M_FORCE_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_EnableIntegerDivision(void)
{
	HW_SET_BIT(PMU_DEV->PMU_HBACK, 5);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_DisableIntegerDivision(void)
{
	HW_CLR_BIT(PMU_DEV->PMU_HBACK, 5);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_Ckin32kClockSrc(uint32_t source)
{
    HW_SET_MSK_VAL(PMU_DEV->PMU_HBACK, PMU_HBACK_CKIN_32K_SRC_Msk, PMU_HBACK_CKIN_32K_SRC_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LPCLK_IsEnabledIntegerDivision(void)
{
    return HW_TEST_BIT(PMU_DEV->PMU_HBACK, 5);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_SetIntegerDivision(uint32_t division)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LP_CLK_DIV_Msk, LP_CLK_DIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LPCLK_GetIntegerDivision(void)
{
    return HW_GET_MSK_VAL(RCC_DEV->RCC_LSI_CTRL, LP_CLK_DIV_Msk, LP_CLK_DIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_EnableFracDivision(void)
{
	HW_SET_MSK(RCC_DEV->FRAC_DIV_CTRL, LSI_FRAC_DIV_EN_Msk);
	HW_CLR_MSK(RCC_DEV->FRAC_DIV_CTRL, LSI_FRAC_DIV_RESETN_EN_Msk);
	HW_SET_MSK(RCC_DEV->FRAC_DIV_CTRL, LSI_FRAC_DIV_RESETN_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_DisableFracDivision(void)
{
	HW_CLR_MSK(RCC_DEV->FRAC_DIV_CTRL, LSI_FRAC_DIV_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_LPCLK_IsEnabledFracDivision(void)
{
	return HW_TEST_BIT(RCC_DEV->FRAC_DIV_CTRL, LSI_FRAC_DIV_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_SetFracDivision(uint32_t division, uint32_t remainder, uint32_t quotient)
{
	HW_SET_VAL(RCC_DEV->FRAC_DIV_PARAM0, quotient);
	HW_SET_VAL(RCC_DEV->FRAC_DIV_PARAM1, remainder);
	HW_SET_VAL(RCC_DEV->FRAC_DIV_PARAM2, division);
}

__HW_STATIC_INLINE
void LL_RCC_LPCLK_GetFracDivision(uint32_t *division, uint32_t *remainder, uint32_t *quotient)
{
	*quotient = HW_GET_VAL(RCC_DEV->FRAC_DIV_PARAM0);
	*remainder = HW_GET_VAL(RCC_DEV->FRAC_DIV_PARAM1);
	*division = HW_GET_VAL(RCC_DEV->FRAC_DIV_PARAM2);
}
/**
  * @}
  */




/** @defgroup MSI
  * @{
  */
/*
 * 0: msirc
 * 1: clk_in
 */
__HW_STATIC_INLINE
void LL_RCC_MSI_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(RCC_DEV->MSI_SRC_SEL, MSI_SRC_SEL_Msk, MSI_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MSI_GetClockSource(void)
{
	return HW_GET_MSK_VAL(RCC_DEV->MSI_SRC_SEL, MSI_SRC_SEL_Msk, MSI_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_MCI_CfgClock(uint32_t basic_trim, uint32_t cap_trim, uint32_t freq_sel, uint32_t cf_trim)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_BIAS_TRIM_Msk, MSI_BIAS_TRIM_Pos, basic_trim);
	HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CAP_TRIM_Msk, MSI_CAP_TRIM_Pos, cap_trim);
	HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_FREQ_SEL_Msk, MSI_FREQ_SEL_Pos, freq_sel);
	HW_SET_MSK_VAL(RCC_DEV->RCC_MSI_CTRL, MSI_CFTRIM_TRIM_Msk, MSI_CFTRIM_TRIM_Pos, cf_trim);
}

__HW_STATIC_INLINE
void LL_RCC_MSI_EnableClkIn(void)
{
	HW_SET_MSK(RCC_DEV->CLK32M_IN_CTRL, CLK32M_IN_PINMUX_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MSI_DisableClkIn(void)
{
	HW_CLR_MSK(RCC_DEV->CLK32M_IN_CTRL, CLK32M_IN_PINMUX_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MSI_IsEnabledClkIn(void)
{
	return HW_TEST_BIT(RCC_DEV->CLK32M_IN_CTRL, CLK32M_IN_PINMUX_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_MSI_SetClkInPin(uint32_t pin)
{
	HW_SET_MSK_VAL(RCC_DEV->CLK32M_IN_CTRL, CLK32M_IN_PINMUX_SEL_Msk, CLK32M_IN_PINMUX_SEL_Pos, pin);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MSI_GetClkInPin(void)
{
	return HW_GET_MSK_VAL(RCC_DEV->CLK32M_IN_CTRL, CLK32M_IN_PINMUX_SEL_Msk, CLK32M_IN_PINMUX_SEL_Pos);
}

/**
  * @}
  */




/** @defgroup HSE
  * @{
  */
__HW_STATIC_INLINE
void LL_RCC_HSE_EnableToPLL(void)
{
	HW_SET_MSK(PMU_DEV->PMU_DCXO_CTRL, CK_XO_SPLL_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_HSE_DisableToPLL(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_DCXO_CTRL, CK_XO_SPLL_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_HSE_IsEnabledToPLL(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_DCXO_CTRL, CK_XO_SPLL_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_HSE_EnableToSOC(void)
{
	HW_SET_MSK(PMU_DEV->PMU_DCXO_CTRL, CK_XO_SOC_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_HSE_DisableToSOC(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_DCXO_CTRL, CK_XO_SOC_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_HSE_IsEnabledToSOC(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_DCXO_CTRL, CK_XO_SPLL_EN_Pos);
}
/**
  * @}
  */



/** @defgroup PLL
  * @{
  */
/**
 * 1. pll source : 0-msi, 1-hse
 * 2. div : [2:0] + 1, 1 to 8
 * 3. Mult: [5:0] + 1, 1 to 64
 * 4. out freq max: 240MHz~448MHz
 * 5. out channel: pll_soc, pll_dev, pll_dev1, pll_cdaa
 * 6. out channel freq div: [2:0], 1 to 7
 **/
/*
 * 0: msi
 * 1: hse
 */
__HW_STATIC_INLINE
void LL_RCC_PLL_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_SRC_SEL_Msk, SPLL_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_GetClockSource(void)
{
	return HW_GET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_SRC_SEL_Msk, SPLL_SRC_SEL_Pos);
}

/*
 * [2:0]
 * 1 to 8
 */
__HW_STATIC_INLINE
void LL_RCC_PLL_SetClockDivision(uint32_t division)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_PREDIV_Msk, SPLL_PREDIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_GetClockDivision(uint32_t pll_num)
{
	return HW_GET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_PREDIV_Msk, SPLL_PREDIV_Pos);
}

/*
 * [5:0]
 * Multi: 1 to 64
 * max 320MHz
 */
__HW_STATIC_INLINE
void LL_RCC_PLL_SetClockMultiple(uint32_t multiplication)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_FCW_Msk, SPLL_FCW_Pos, multiplication);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_GetClockMultiple(void)
{
	return HW_GET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_FCW_Msk, SPLL_FCW_Pos);
}

/*
 * channel 0
 * pll_soc
 * to sys clk
 */
__HW_STATIC_INLINE
void LL_RCC_PLL_EnableCh0SOC(void)
{
	HW_SET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_160M_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_DisableCh0SOC(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_160M_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_IsEnabledPLLCh0SOC(void)
{
	return HW_GET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_160M_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_SetCh0SOCDivision(uint32_t division)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_160M_POSTDIV_Msk, SPLL_160M_POSTDIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_GetCh0SOCDivision(void)
{
	return HW_GET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_160M_POSTDIV_Msk, SPLL_160M_POSTDIV_Pos);
}

/*
 * channel 1
 * pll_dev
 * to device clk
 */
__HW_STATIC_INLINE
void LL_RCC_PLL_EnableCh1DEV0(void)
{
	HW_SET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_DisableCh1DEV0(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_IsEnabledCh1DEV0(void)
{
	return HW_GET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_SetCh1DEV0Division(uint32_t division)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV_POSTDIV_Msk, SPLL_DEV_POSTDIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_GetCh1DEV0Division(void)
{
    return HW_GET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV_POSTDIV_Msk, SPLL_DEV_POSTDIV_Pos);
}

/*
 * channel 2
 * pll_dev1
 * to device clk
 */
__HW_STATIC_INLINE
void LL_RCC_PLL_EnableCh2DEV1(void)
{
	HW_SET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV1_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_DisableCh2DEV1(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV1_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_IsEnabledCh2DEV1(void)
{
	return HW_GET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV1_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_SetCh2DEV1Division(uint32_t division)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV1_POSTDIV_Msk, SPLL_DEV1_POSTDIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_GetCh2DEV1Division(void)
{
    return HW_GET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_DEV1_POSTDIV_Msk, SPLL_DEV1_POSTDIV_Pos);
}

/*
 * channel 3
 * pll_dev1
 * to device clk
 */
__HW_STATIC_INLINE
void LL_RCC_PLL_EnableCh3CDAA(void)
{
	HW_SET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_CDAA_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_DisableCh3CDAA(void)
{
	HW_CLR_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_CDAA_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_IsEnabledCh3CDAA(void)
{
	return HW_GET_MSK(RCC_DEV->RCC_SPLL_CTRL, SPLL_CDAA_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PLL_SetCh3CDAADivision(uint32_t division)
{
	HW_SET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_CDAA_POSTDIV_Msk, SPLL_CDAA_POSTDIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PLL_GetCh3CDAADivision(void)
{
	return HW_GET_MSK_VAL(RCC_DEV->RCC_SPLL_CTRL, SPLL_CDAA_POSTDIV_Msk, SPLL_CDAA_POSTDIV_Pos);
}
/**
  * @}
  */





/** @defgroup SYS
  * @{
  */
__HW_FORCE_INLIDE void LL_RCC_SYS_SetClockSource(uint32_t sys, uint32_t source)
{
	if (sys == 0) {
		HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_SRC_SEL0, SYSCFG_SYSCLK_SRC_SEL0_Msk, SYSCFG_SYSCLK_SRC_SEL0_Pos, source);
	} else {
		HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_SRC_SEL1, SYSCFG_SYSCLK_SRC_SEL1_Msk, SYSCFG_SYSCLK_SRC_SEL1_Pos, source);
	}
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SYS_GetClockSource(uint32_t sys)
{
	if (sys == 0) {
		return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_SRC_SEL0, SYSCFG_SYSCLK_SRC_SEL0_Msk, SYSCFG_SYSCLK_SRC_SEL0_Pos);
	} else {
		return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_SRC_SEL1, SYSCFG_SYSCLK_SRC_SEL1_Msk, SYSCFG_SYSCLK_SRC_SEL1_Pos);
	}
}

__HW_STATIC_INLINE
void LL_RCC_SYS_SetClockDivision(uint32_t division)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_DIV, SYSCFG_SYSCLK_DIV_Msk, SYSCFG_SYSCLK_DIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SYS_GetClockDivision(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_DIV, SYSCFG_SYSCLK_DIV_Msk, SYSCFG_SYSCLK_DIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_AHB_SetClockDivision(uint32_t division)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_DIV, SYSCFG_SYS1CLK_DIV_Msk, SYSCFG_SYS1CLK_DIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_AHB_GetClockDivision(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_SYSCLK_DIV, SYSCFG_SYS1CLK_DIV_Msk, SYSCFG_SYS1CLK_DIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_APB0_SetClockDivision(uint32_t division)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_APB0_PCLK_DIV, CCMU_APB0_PCLK_DIV_Msk, CCMU_APB0_PCLK_DIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_APB0_GetClockDivision(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_APB0_PCLK_DIV, CCMU_APB0_PCLK_DIV_Msk, CCMU_APB0_PCLK_DIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_APB1_SetClockDivision(uint32_t division)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_APB1_PCLK_DIV, APB1_PCLK_DIV_Msk, APB1_PCLK_DIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_APB1_GetClockDivision(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_APB1_PCLK_DIV, APB1_PCLK_DIV_Msk, APB1_PCLK_DIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_AHB_SetMMWHClockDivision(uint32_t division)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_MMWAVE_CTRL, MMWHCLK_DIV_Msk, MMWHCLK_DIV_Pos, division);
}

/**
  * @}
  */




/** PSIC_LL_Driver
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_RCC_BUS_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
