/**
 **************************************************************************************************
 * @file    ll_gpadc.h
 * @brief   reset clock hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_GPADC_H
#define _PSIC_LL_GPADC_H


#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "soc.h"
#include "types.h"
#include "hw_gpadc.h"

#include <ll_gpio.h>
#include <ll_gpadc.h>
#include <ll_pmu.h>
#include <ll_rcc_dev.h>


/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup ll_gpadc
  * @{
  */

#define LL_GPADC_FLAGS_PHASE3_DELAY                    GPADC_PHASE2_SETTLED_EN_Pos
#define LL_GPADC_FLAGS_PHASE2_DELAY                    GPADC_PHASE1_SETTLED_EN_Pos

#define LL_GPADC_FLAGS_PHASE3_LDO_READY                GPADC_PHASE2_LDO_READY_EN_Pos
#define LL_GPADC_FLAGS_PHASE2_LDO_READY                GPADC_PHASE1_LDO_READY_EN_Pos

#define LL_GPADC_FLAGS_PHASE2_LDO_READY                GPADC_PHASE1_LDO_READY_EN_Pos

typedef struct {
	uint32_t p3_delay_en:1;
	uint32_t p2_delay_en:1;
	uint32_t p3_wait_ldo_ready:1;
	uint32_t p2_wait_ldo_ready:1;
	uint32_t smpl_finish_low_signal:5;
	uint32_t smpl_start_high_signal:5;
	uint32_t smpl_p3_high_signal:5;
	uint32_t smpl_p2_high_signal:5;
	uint32_t smpl_p1_high_signal:5;
} gpadc_t_flags;

typedef struct {
	gpadc_t_flags flags;
	uint16_t phase3_delay_time;
	uint16_t phase2_delay_time;
	uint16_t smpl_finish_2_start_time;
	uint16_t start_2_smpl_time;
} gpadc_timming_t;

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup reset clock define
  * @{
  */
#define LL_GPADC_CLK_DIV_1                    0
#define LL_GPADC_CLK_DIV_2                    1
#define LL_GPADC_CLK_DIV_4                    2
#define LL_GPADC_CLK_DIV_8                    3
#define LL_GPADC_CLK_DIV_16                   4
#define LL_GPADC_CLK_DIV_32                   5
#define LL_GPADC_CLK_DIV_64                   6
#define LL_GPADC_CLK_DIV_128                  7
#define LL_GPADC_CLK_DIV_MAX                  LL_RCC_CLK_DIV_128

#define LL_GPADC_SOURCE_LPCLK                 0
#define LL_GPADC_SOURCE_DCXO                  1
#define LL_GPADC_SOURCE_MSI                   2
#define LL_GPADC_SOURCE_PLL_DEV               3
/**
  * @}
  */

#define LL_GPADC_CHANNEL_COUNT				(0xa)
#define LL_GPADC_ALL_CHANNEL_MSK			(0x3FF)
#define LL_GPADC_VAIL_CH_MSK				(0x2FF)
#define LL_GPADC_CLOCK_DIV_2				(0x0)
#define LL_GPADC_CLOCK_DIV_4				(0x1)
#define LL_GPADC_CLOCK_DIV_6				(0x2)
#define LL_GPADC_CLOCK_DIV_8				(0x3)
#define LL_GPADC_CLOCK_DIV_12				(0x4)
#define LL_GPADC_CLOCK_DIV_14				(0x5)

#define LL_GPADC_BANDWITH_RC_OFF			(0xF)
#define LL_GPADC_BANDWITH_2MHZ				(0xE)
#define LL_GPADC_BANDWITH_1MHZ				(0xD)
#define LL_GPADC_BANDWITH_0_P_5MHZ			(0xB)
#define LL_GPADC_BANDWITH_0_P_25MHZ			(0x7)
#define LL_GPADC_BANDWITH_0_P_13MHZ			(0x0)

#define LL_GPADC_VREF_SOURCE_BG				(0x1)
#define LL_GPADC_VREF_SOURCE_VDDIO			(0x0)

#define LL_GPADC_INPUT_SHRINK_SCALE_2_3    (0x0)
#define LL_GPADC_INPUT_SHRINK_SCALE_1_2    (0x8)
#define LL_GPADC_INPUT_SHRINK_SCALE_1_3    (0xc)
#define LL_GPADC_INPUT_SHRINK_SCALE_1_5    (0xe)
#define LL_GPADC_INPUT_SHRINK_SCALE_1_9    (0xf)

#define LL_GPADC_RES_6bit     (0x0)
#define LL_GPADC_RES_8bit     (0x1)
#define LL_GPADC_RES_10bit    (0x2)
#define LL_GPADC_RES_12bit    (0x3)

#define LL_GPADC_OVER_SMPL_16x	 (0x0)
#define LL_GPADC_OVER_SMPL_32x	 (0x1)
#define LL_GPADC_OVER_SMPL_64x	 (0x2)
#define LL_GPADC_OVER_SMPL_128x	 (0x3)
#define LL_GPADC_OVER_SMPL_4096x (0x4)

#define LL_GPADC_SIGLE_CYCLE_MODE 		(0x0)
#define LL_GPADC_CONTINUE_MODE 		    (0x1)
#define LL_GPADC_BURST_MODE 	        (0x2)

#define LL_GPADC_ULTRA_OVER_SMP_INTR_BIT    (GPADC_ULTRA_OVR_SMPL_INT_CLR_Pos)
#define LL_GPADC_FIFO_TRIG_INTR_BIT         (GPADC_FIFO_TRIG_INT_CLR_Pos)
#define LL_GPADC_FIFO_DATA_INTR_BIT         (GPADC_FIFO_DATA_INT_CLR_Pos)

#define LL_GPADC_INTERNAL_REF_MV            (1200)

#define LL_GPADC_FAST_MODE_FREQ     (32000000)
/**
  * @}
  */
#ifndef BIT
#define BIT HW_BIT
#endif

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/** @defgroup GPADC
  * @{
  */

/*!< 0x0000 GPADC_CTRL *GPADC LDO ready signal */
__HW_STATIC_INLINE
uint32_t LL_GPADC_LDO_IsReady(void)
{
    return HW_TEST_BIT(GPADC_DEV->GPADC_CTRL, GPADC_LDO_READY_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_IsDone(void)
{
    return !HW_TEST_BIT(GPADC_DEV->GPADC_CTRL, GPADC_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Enable(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_CTRL, GPADC_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Disable(void)
{
    HW_CLR_BIT(GPADC_DEV->GPADC_CTRL, GPADC_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Reset(void)
{
  /* define in gpadc module register */
	HW_SET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_RESETN_Msk, GPADC_RESETN_Pos, 0);
	HW_SET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_RESETN_Msk, GPADC_RESETN_Pos, 1);
}

/*!< 0x0000 GPADC_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_Disable_InputShrink_Bypass(void)
{
	HW_CLR_BIT(GPADC_DEV->GPADC_CTRL, GPADC_INPUT_SHRINK_BYPASS_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Enable_InputShrink_Bypass(void)
{
	HW_SET_BIT(GPADC_DEV->GPADC_CTRL, GPADC_INPUT_SHRINK_BYPASS_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_SetInputShrink(uint32_t scale)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CTRL, AON_GPADC_INPUT_SHRINK_Msk, AON_GPADC_INPUT_SHRINK_Pos, scale);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetInputShrink(void)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CTRL, AON_GPADC_INPUT_SHRINK_Msk, AON_GPADC_INPUT_SHRINK_Pos);
}

/*!< 0x0000 GPADC_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_SetInputBw(uint32_t bw)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_INPUT_BW_Msk, GPADC_INPUT_BW_Pos, bw);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetInputBw(void)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_INPUT_BW_Msk, GPADC_INPUT_BW_Pos);
}

/*!< 0x0000 GPADC_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_SetClockDivision(uint32_t division)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_CLK_DIV_Msk, GPADC_CLK_DIV_Pos, division);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetClockDivision(void)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_CLK_DIV_Msk, GPADC_CLK_DIV_Pos);
}

/*!< 0x0000 GPADC_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_SetResolution(uint32_t res)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_RES_Msk, GPADC_RES_Pos, res);
}

/*!< 0x0000 GPADC_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_SetTestSel(uint32_t testSel)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_TSEL_Msk, GPADC_TSEL_Pos, testSel);
}

/*!< 0x0000 GPADC_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_SetVrefBg(uint32_t sources)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_VREF_BG_SEL_Msk, GPADC_VREF_BG_SEL_Pos, sources);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetVrefBg(uint32_t sources)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_VREF_BG_SEL_Msk, GPADC_VREF_BG_SEL_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetResolution(uint32_t res)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CTRL, GPADC_RES_Msk, GPADC_RES_Pos);
}

/*!< 0x0000 GPADC_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_EnableFastMode(void)
{
	HW_SET_BIT(GPADC_DEV->GPADC_CTRL, GPADC_FAST_MODE_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_DisableFastMode(void)
{
	HW_CLR_BIT(GPADC_DEV->GPADC_CTRL, GPADC_FAST_MODE_Pos);
}

/*!< 0x0004 GPADC_CAL_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_EnableCal(void)
{
	HW_CLR_BIT(GPADC_DEV->GPADC_CAL_CTRL, GPADC_CAL_BYPASS_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_DisableCal(void)
{
	HW_SET_BIT(GPADC_DEV->GPADC_CAL_CTRL, GPADC_CAL_BYPASS_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_SetCalStart(void)
{
	HW_SET_BIT(GPADC_DEV->GPADC_CAL_CTRL, GPADC_CAL_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetCalStatus(void)
{
	return HW_GET_BIT(GPADC_DEV->GPADC_CAL_CTRL, GPADC_CAL_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_SetCalInValue(uint32_t cal)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CAL_CTRL, GPADC_CAL_IN_Msk, GPADC_CAL_IN_Pos, cal);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetCalOutValue(void)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CALOUT_SMPL, GPADC_CALOUT_SMPL_Msk, GPADC_CALOUT_SMPL_Pos);
}

/*!< 0x0008 GPADC_MODE
*/
__HW_STATIC_INLINE
void LL_GPADC_SetMode(uint32_t mode)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_MODE, GPADC_MODE_SEL_Msk, GPADC_MODE_SEL_Pos, mode);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetMode(uint32_t mode)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_MODE, GPADC_MODE_SEL_Msk, GPADC_MODE_SEL_Pos);
}

/*!< 0x000c GPADC_CH_ENABLE
*/
__HW_STATIC_INLINE
void LL_GPADC_EnableChannel(uint32_t ch_msk)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CH_ENABLE, GPADC_CH_EN_Msk, GPADC_CH_EN_Pos, ch_msk);
}

__HW_STATIC_INLINE
void LL_GPADC_DisableChannel(uint32_t ch_msk)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CH_ENABLE, GPADC_CH_EN_Msk, GPADC_CH_EN_Pos, ~ch_msk);
}

/*!< 0x0010 GPADC_OVER_SMPL_CTRL
*/
__HW_STATIC_INLINE
void LL_GPADC_EnableUltraOverSmpl(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_ULTRA_OVER_SMPL_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_DisableUltraOverSmpl(void)
{
    HW_CLR_BIT(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_ULTRA_OVER_SMPL_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_SetChForUltraOverSmpl(uint32_t chnl_id)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_ULTRA_OVER_SMPL_SEL_Msk, GPADC_ULTRA_OVER_SMPL_SEL_Pos, chnl_id);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetChForUltraOverSmpl(void)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_ULTRA_OVER_SMPL_SEL_Msk, GPADC_ULTRA_OVER_SMPL_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_EnableOverSmpl(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_OVER_SMPL_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_DisableOverSmpl(void)
{
    HW_CLR_BIT(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_OVER_SMPL_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_SetOverSmpl(uint32_t oversmpl)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_OVER_SMPL_SEL_Msk, GPADC_OVER_SMPL_SEL_Pos, oversmpl);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetOverSmpl(uint32_t oversmpl)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_OVER_SMPL_CTRL, GPADC_OVER_SMPL_SEL_Msk, GPADC_OVER_SMPL_SEL_Pos);
}

/*!< 0x0030 GPADC_INT_EN0
*/
__HW_STATIC_INLINE
void LL_GPADC_Enable_ChCmpLowIntr(uint32_t chnl_id)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_EN0,  GPADC_CH_CMP_LOW_INT_EN_Pos + chnl_id);
}

__HW_STATIC_INLINE
void LL_GPADC_Disable_AllChCmpLowIntr(void)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_INT_EN0, GPADC_CH_CMP_LOW_INT_EN_Msk, GPADC_CH_CMP_LOW_INT_EN_Pos, 0);
}

__HW_STATIC_INLINE
void LL_GPADC_Enable_ChDataIntr(uint32_t chnl_msk)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_INT_EN0, GPADC_CH_DATA_INT_EN_Msk, GPADC_CH_DATA_INT_EN_Pos, chnl_msk);
}

__HW_STATIC_INLINE
void LL_GPADC_Disable_AllChDataIntr(void)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_INT_EN0, GPADC_CH_DATA_INT_EN_Msk, GPADC_CH_DATA_INT_EN_Pos, 0);
}

/*!< 0x0034 GPADC_INT_EN0
*/

__HW_STATIC_INLINE
void LL_GPADC_Enable_UltraOvrSmplData_Intr(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_EN1, GPADC_ULTRA_OVR_SMPL_DATA_INT_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Disable_UltraOvrSmplData_Intr(void)
{
    HW_CLR_BIT(GPADC_DEV->GPADC_INT_EN1, GPADC_ULTRA_OVR_SMPL_DATA_INT_EN_Pos);
}

/*!< 0x0034 GPADC_INT_EN0
*/
__HW_STATIC_INLINE
void LL_GPADC_Enable_FifoTrig_Intr(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_EN1, GPADC_FIFO_TRIG_INT_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Disable_FifoTrig_Intr(void)
{
    HW_CLR_BIT(GPADC_DEV->GPADC_INT_EN1, GPADC_FIFO_TRIG_INT_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Enable_FifoDataIn_Intr(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_EN1, GPADC_FIFO_DATA_INT_EN_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Disable_FifoDataIn_Intr(void)
{
    HW_CLR_BIT(GPADC_DEV->GPADC_INT_EN1, GPADC_FIFO_DATA_INT_EN_Pos);
}

/*!< 0x0034 GPADC_INT_EN0
*/
__HW_STATIC_INLINE
void LL_GPADC_Enable_ChCmpHigh_Intr(uint32_t chnl_id)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_EN1, GPADC_CH_CMP_HIGH_INT_EN_Pos + chnl_id);
}

__HW_STATIC_INLINE
void LL_GPADC_Disable_AllChCmpHigh_Intr(void)
{
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_INT_EN1, GPADC_CH_CMP_HIGH_INT_EN_Msk, GPADC_CH_CMP_HIGH_INT_EN_Pos, 0);
}

/*!< 0x0038 GPADC_INT_FLAG_CLR0
*/
__HW_STATIC_INLINE
uint32_t LL_GPADC_Get_Intr_Flag0(void)
{
    return GPADC_DEV->GPADC_INT_FLAG_CLR0;
}

__HW_STATIC_INLINE
void LL_GPADC_Clr_Intr_Flag0(void)
{
    GPADC_DEV->GPADC_INT_FLAG_CLR0 = 0xFFFFFFFF;
}

__HW_STATIC_INLINE
void LL_GPADC_Clear_ChCmpLow_Intr(uint32_t chnl_id)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_FLAG_CLR0, GPADC_CH_CMP_LOW_INT_CLR_Pos + chnl_id);
}

__HW_STATIC_INLINE
void LL_GPADC_Clear_ChData_Intr(uint32_t chnl_id)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_FLAG_CLR0, GPADC_CH_DATA_INT_CLR_Pos + chnl_id);
}

/*!< 0x003c GPADC_INT_FLAG_CLR1
*/
__HW_STATIC_INLINE
uint32_t LL_GPADC_Get_Intr_Flag1(void)
{
    return GPADC_DEV->GPADC_INT_FLAG_CLR1;
}

__HW_STATIC_INLINE
void LL_GPADC_Clr_Intr_Flag1(void)
{
    GPADC_DEV->GPADC_INT_FLAG_CLR1 = 0xFFFFFFFF;
}

__HW_STATIC_INLINE
void LL_GPADC_Clear_UltraOvrSmpl_Intr(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_FLAG_CLR1, GPADC_ULTRA_OVR_SMPL_INT_CLR_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Clear_FifoTrig_Intr(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_FLAG_CLR1, GPADC_FIFO_TRIG_INT_CLR_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Clear_FifoData_Intr(void)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_FLAG_CLR1, GPADC_FIFO_DATA_INT_CLR_Pos);
}

__HW_STATIC_INLINE
void LL_GPADC_Clear_ChCmpHigh_Intr(uint32_t chnl_id)
{
    HW_SET_BIT(GPADC_DEV->GPADC_INT_FLAG_CLR1, GPADC_CH_CMP_HIGH_INT_CLR_Pos + chnl_id);
}

/*!< 0x0040~0x0058 GPADC_CMP_LOW
*/
__HW_STATIC_INLINE
void LL_GPADC_SetCmpLowValue(uint32_t chnl_id, uint32_t cmpValue)
{
	if (chnl_id & 0x1) {
	    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CMP_LOW[chnl_id >> 1], GPADC_CH_CMP_LOW_H_Msk, GPADC_CH_CMP_LOW_H_Pos, cmpValue);
	} else {
	    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CMP_LOW[chnl_id >> 1], GPADC_CH_CMP_LOW_L_Msk, GPADC_CH_CMP_LOW_L_Pos, cmpValue);
	}
}

/*!< 0x005C GPADC_CH_VBAT_LOW
*/
__HW_STATIC_INLINE
void LL_GPADC_SetCmpLowVBAT(uint32_t cmpValue)
{
	HW_SET_MSK_VAL(GPADC_DEV->GPADC_CMP_LOW_VBAT, GPADC_CH_VBAT_LOW_Msk, GPADC_CH_VBAT_LOW_Pos, cmpValue);
}

/*!< 0x0060~0x0078 GPADC_CMP_HIGH
*/
__HW_STATIC_INLINE
void LL_GPADC_SetCmpHighValue(uint32_t chnl_id, uint32_t cmpValue)
{
	if (chnl_id & 0x1) {
	    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CMP_HIGH[chnl_id >> 1], GPADC_CH_CMP_HIGH_H_Msk, GPADC_CH_CMP_HIGH_H_Pos, cmpValue);
	} else {
	    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CMP_HIGH[chnl_id >> 1], GPADC_CH_CMP_HIGH_L_Msk, GPADC_CH_CMP_HIGH_L_Pos, cmpValue);
	}
}

/*!< 0x007C GPADC_CMP_HIGH_VBAT
*/
__HW_STATIC_INLINE
void LL_GPADC_SetVBATCmpHighValue(uint32_t cmpValue)
{
	HW_SET_MSK_VAL(GPADC_DEV->GPADC_CMP_HIGH_VBAT, GPADC_CMP_HIGH_VBAT_Msk, GPADC_CMP_HIGH_VBAT_Pos, cmpValue);
}

/*!< 0x0080 GPADC_FIFO_TRIG
*/
__HW_STATIC_INLINE
void LL_GPADC_SetFifoTrig_Level(uint32_t level)
{
	HW_SET_MSK_VAL(GPADC_DEV->GPADC_FIFO_TRIG, GPADC_FIFO_TRIG_Msk, GPADC_FIFO_TRIG_Pos, level);
}

/*!< 0x0088 GPADC_FIFO_FLUSH
*/
__HW_STATIC_INLINE
void LL_GPADC_SetFifoFlush(void)
{
	HW_SET_BIT(GPADC_DEV->GPADC_FIFO_FLUSH, GPADC_FIFO_FLUSH_Pos);
}

/*!< 0x008c GPADC_FIFO_STATUS
*/
__HW_STATIC_INLINE
uint32_t LL_GPADC_GetFifoStatus(void)
{
	return GPADC_DEV->GPADC_FIFO_STATUS;
}

/*!< 0x0090 GPADC_FIFO_VAL
*/
__HW_STATIC_INLINE
uint32_t LL_GPADC_GetFifoVal(void)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_FIFO_VAL, GPADC_FIFO_VAL_Msk, GPADC_FIFO_VAL_Pos);
}

/*!< 0x0094 GPADC_ULTRA_OVER_SMPL_VAL
*/
__HW_STATIC_INLINE
uint32_t LL_GPADC_GetUltraOvrSmplVal(void)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_ULTRA_OVER_SMPL_VAL, GPADC_ULTRA_OVER_SMPL_VAL_Msk, GPADC_ULTRA_OVER_SMPL_VAL_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_GetChVal(uint32_t channel_id)
{
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CH_VAL[channel_id], GPADC_CH_VAL_Msk, GPADC_CH_VAL_Pos);
}

__HW_STATIC_INLINE
int LL_GPADC_SetTimming(gpadc_timming_t *timming)
{
    uint32_t timming_flags =    ((timming->flags.p3_delay_en << GPADC_PHASE2_SETTLED_EN_Pos)
                                | (timming->flags.p2_delay_en << GPADC_PHASE1_SETTLED_EN_Pos)
                                | (timming->flags.p3_wait_ldo_ready << GPADC_PHASE2_LDO_READY_EN_Pos)
                                | (timming->flags.p2_wait_ldo_ready << GPADC_PHASE1_LDO_READY_EN_Pos)
                                | (timming->flags.smpl_finish_low_signal << SMPL_FINISH_SIGNAL_LOW_EN_Pos)
                                | (timming->flags.smpl_start_high_signal << START_SIGNAL_HIGH_EN_Pos)
                                | (timming->flags.smpl_p3_high_signal << PHASE3_SIGNAL_HIGH_EN_Pos)
                                | (timming->flags.smpl_p2_high_signal << PHASE2_SIGNAL_HIGH_EN_Pos)
                                | (timming->flags.smpl_p1_high_signal << PHASE1_SIGNAL_HIGH_EN_Pos));
    /* usage for timming 0 setting */
    GPADC_DEV->GPADC_TIMING_CTRL0 = timming_flags;

    /* usage for timming 1 setting */
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL1, GPADC_PHASE2_TO_PHASE3_SETTINGS_Msk, \
                                    GPADC_PHASE2_TO_PHASE3_SETTINGS_Pos, timming->phase3_delay_time);

    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL1, GPADC_PHASE1_TO_PHASE2_SETTINGS_Msk, \
                                    GPADC_PHASE1_TO_PHASE2_SETTINGS_Pos, timming->phase2_delay_time);

    /* usage for timming 2 setting */
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL2, GPADC_SMPLF_TO_START_SETTINGS_Msk, \
                                    GPADC_SMPLF_TO_START_SETTINGS_Pos, timming->smpl_finish_2_start_time);

    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL2, GPADC_START_TO_SMPL_SETTINGS_Msk, \
                                    GPADC_START_TO_SMPL_SETTINGS_Pos, timming->start_2_smpl_time);

    return 0;
}

__HW_STATIC_INLINE
int LL_GPADC_SetTimming_Def(void)
{
    uint32_t timming_flags =    ((1 << GPADC_PHASE2_SETTLED_EN_Pos)
                                | (1 << GPADC_PHASE1_SETTLED_EN_Pos)
                                | (0 << GPADC_PHASE2_LDO_READY_EN_Pos)
                                | (0 << GPADC_PHASE1_LDO_READY_EN_Pos)
                                | ((1U << 3) << SMPL_FINISH_SIGNAL_LOW_EN_Pos)
                                | ((1U << 3) << START_SIGNAL_HIGH_EN_Pos)
                                | ((1U << 3) << PHASE3_SIGNAL_HIGH_EN_Pos)
                                | (0U << PHASE2_SIGNAL_HIGH_EN_Pos)
                                | (0x1F << PHASE1_SIGNAL_HIGH_EN_Pos));
    /* usage for timming 0 setting */
    GPADC_DEV->GPADC_TIMING_CTRL0 = timming_flags;
    /* usage for timming 1 setting */
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL1, GPADC_PHASE2_TO_PHASE3_SETTINGS_Msk, \
                                    GPADC_PHASE2_TO_PHASE3_SETTINGS_Pos, 0x8ff);
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL1, GPADC_PHASE1_TO_PHASE2_SETTINGS_Msk, \
                                    GPADC_PHASE1_TO_PHASE2_SETTINGS_Pos, 0);
    /* usage for timming 2 setting */
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL2, GPADC_SMPLF_TO_START_SETTINGS_Msk, \
                                    GPADC_SMPLF_TO_START_SETTINGS_Pos, 32);
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_TIMING_CTRL2, GPADC_START_TO_SMPL_SETTINGS_Msk, \
                                    GPADC_START_TO_SMPL_SETTINGS_Pos, 64);
    return 0;
}

__HW_STATIC_INLINE
void LL_GPADC_SetInitConfig(void)
{
	LL_PMU_SW_RTC1SoftEnable();
	LL_RCC_GPADC_SetClockSource(LL_GPADC_SOURCE_MSI);
	LL_RCC_GPADC_SetPrescaler(LL_GPADC_CLK_DIV_1);
	LL_RCC_GPADC_DisableBusClock();
	LL_RCC_GPADC_DisableClock();
	LL_RCC_GPADC_Reset();
	LL_RCC_GPADC_EnableBusClock();
	LL_RCC_GPADC_EnableClock();
	/*default timming param*/
	LL_GPADC_SetTimming_Def();

	/* other param */
	LL_GPADC_SetClockDivision(LL_GPADC_CLK_DIV_1);
	LL_GPADC_SetInputBw(LL_GPADC_BANDWITH_1MHZ);
	LL_GPADC_SetVrefBg(LL_GPADC_VREF_SOURCE_BG); /*LL_GPADC_VREF_SOURCE_VDDIO*/

	/* clock should slower than 32MHz */
	LL_GPADC_SetTestSel(0x3); /* test select 0b11 */
	/* when clock slower than 32Mhz use slow mode */
	LL_GPADC_DisableFastMode();

	/* disable auto cal and use default cal value*/
	LL_GPADC_DisableCal();
	LL_GPADC_SetCalInValue(0x40);
}

__HW_STATIC_INLINE
void LL_GPADC_SetChConfig(uint32_t channel_id)
{
	/* disable all gpadc channel before new read start */
	LL_GPADC_DisableChannel(LL_GPADC_ALL_CHANNEL_MSK);
	/* zephyr read had config resolution */
	LL_GPADC_SetResolution(LL_GPADC_RES_12bit);
	LL_GPADC_SetMode(LL_GPADC_SIGLE_CYCLE_MODE);

	LL_GPADC_Disable_InputShrink_Bypass();
	LL_GPADC_SetInputShrink(LL_GPADC_INPUT_SHRINK_SCALE_1_3);
	LL_GPADC_DisableOverSmpl();
	LL_GPADC_Disable_AllChDataIntr();
	LL_GPADC_Clr_Intr_Flag0();
	LL_GPADC_Clr_Intr_Flag1();

	/*LL_GPADC_Enable_ChDataIntr(BIT(8));*/
	LL_GPADC_EnableChannel(HW_BIT(channel_id));
	LL_GPADC_Enable();
}


__HW_STATIC_INLINE
uint32_t LL_GPADC_ChVal_Calibration(uint32_t ch_val)
{
	const uint16_t scaler[] = {13, 27, 53, 107, 213, 212, 639, 1280, 2560, 5120, 10240, 20480};
	uint32_t calibration_value = 0;

	for (int i = 0; i < 12; i++) {
		if (ch_val & BIT(i)) {
			calibration_value += scaler[i];
		}
	}
	ch_val = (calibration_value + 5) / 10;
	return ch_val;
}

__HW_STATIC_INLINE
uint32_t LL_GPADC_Polling_GetChVal(uint32_t channel_id)
{
	uint32_t intr_stat_flag = 0;
	uint32_t raw_value = 0;

	/* polling to read gpadc raw value*/
	do {
		intr_stat_flag = LL_GPADC_Get_Intr_Flag1();
		intr_stat_flag = LL_GPADC_Get_Intr_Flag0();
		if (intr_stat_flag & HW_BIT(channel_id)) {
			raw_value = LL_GPADC_GetChVal(channel_id) & 0xFFF;
			LL_GPADC_Clear_ChData_Intr(channel_id);
			/*adjust adc raw data for calbration*/
			break;
		}
		if (LL_GPADC_IsDone()) {
			raw_value = LL_GPADC_GetChVal(channel_id) & 0xFFF;
			break;
		}
	} while (1);

	raw_value = LL_GPADC_ChVal_Calibration(raw_value);

	return raw_value;
}


__HW_STATIC_INLINE
uint32_t LL_GPADC_RawVal2mv(uint32_t raw_value, uint32_t min_raw,
									uint32_t min_mv, uint32_t max_raw, uint32_t max_mv)
{
	uint32_t delt_value = 0;
	uint32_t final_value = 0;

	//printk("[?V = %d] [0.6V = %d] [2.4V = %d]\n", raw_value, min_raw, max_raw);

	if (max_raw <= min_raw) {
		//printk("errr for 0.6V = 2.4V value\n");
		return -1;
	}

	if (raw_value >= min_raw) {
		delt_value = raw_value - min_raw;
		final_value = ((max_mv - min_mv) * (delt_value)) / (max_raw - min_raw) ;
		final_value += min_mv;
	} else {
		delt_value = min_raw - raw_value;
		final_value = ((max_mv - min_mv) * (delt_value)) / (max_raw - min_raw) ;
		final_value = min_mv - final_value;
	}

	//printk("[%d mV = %d] [0.6V = %d] [2.4V = %d]\n", final_value, raw_value, min_raw, max_raw);

	return final_value;

}

/**
  * @}
  */

/** ll_rcc
  * @}
  */


/** PSIC_LL_Driver
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_RCC_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
