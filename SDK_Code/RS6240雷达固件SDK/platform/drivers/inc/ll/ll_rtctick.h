/**
 **************************************************************************************************
 * @file    psic_ll_rtctick.h
 * @brief   rtc tick hadware define for bus.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_RTCTICK_H
#define _PSIC_RTCTICK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

#include "types.h"
#include "hw_pmu.h"
#include "soc.h"


/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup psic_ll_rtctick
  * @{
  */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup define
  * @{
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
void LL_RTCTICK_Enable(void)
{
	HW_SET_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RTCTICK_Disable(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_IsEnabled(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_EN_Msk);
}


__HW_STATIC_INLINE
uint64_t LL_RTCTICK_GetCurrentValue(void)
{
	uint64_t tmr_val = PMU_DEV->PMU_TMR_WUP_VAL1;
	tmr_val <<= 32;
	tmr_val |= PMU_DEV->PMU_TMR_WUP_VAL0;
	return tmr_val;
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_GetCurrentValue32(void)
{
	return PMU_DEV->PMU_TMR_WUP_VAL0;
}


__HW_STATIC_INLINE
uint32_t LL_RTCTICK_GetCurrentValueL(void)
{
	return PMU_DEV->PMU_TMR_WUP_VAL0;
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_GetCurrentValueH(void)
{
	return PMU_DEV->PMU_TMR_WUP_VAL1;
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP0_Enable(void)
{
	HW_SET_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP0_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP0_Disable(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP0_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_CMP0_IsEnabled(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP0_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP0_ClearFlag(void)
{
	HW_SET_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP0_CLR_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_CMP0_GetFlag(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_TMR_WUP_INTR_FLAG, TMR_WUP_CMP0_FLAG_Pos);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP0_SetValue(uint64_t cmp_val)
{
	PMU_DEV->PMU_TMR_WUP_CMP0_VAL0 = (uint32_t)(cmp_val);
	PMU_DEV->PMU_TMR_WUP_CMP0_VAL1 = (uint32_t)(cmp_val >> 32);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP0_SetValueL(uint32_t cmp_val)
{
	PMU_DEV->PMU_TMR_WUP_CMP0_VAL0 = cmp_val;
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP0_SetValueH(uint32_t cmp_val)
{
	PMU_DEV->PMU_TMR_WUP_CMP0_VAL1 = cmp_val;
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_CMP0_GetValueL(void)
{
	return PMU_DEV->PMU_TMR_WUP_CMP0_VAL0;
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_CMP0_GetValueH(void)
{
	return PMU_DEV->PMU_TMR_WUP_CMP0_VAL1;
}

__HW_STATIC_INLINE
uint64_t LL_RTCTICK_CMP0_GetValue(void)
{
	uint64_t tmr_val = PMU_DEV->PMU_TMR_WUP_CMP0_VAL1;
	tmr_val <<= 32;
	tmr_val |= PMU_DEV->PMU_TMR_WUP_CMP0_VAL0;
	return tmr_val;
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP1_Enable(void)
{
	HW_SET_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP1_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP1_Disable(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP1_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_CMP1_IsEnabled(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP1_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP1_ClearFlag(void)
{
	HW_SET_MSK(PMU_DEV->PMU_TMR_WUP_CTRL1, TMR_WUP_CMP1_CLR_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RTCTICK_CMP1_GetFlag(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_TMR_WUP_INTR_FLAG, TMR_WUP_CMP1_FLAG_Pos);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP1_SetValue(uint64_t cmp_val)
{
	PMU_DEV->PMU_TMR_WUP_CMP1_VAL0 = (uint32_t)(cmp_val);
	PMU_DEV->PMU_TMR_WUP_CMP1_VAL1 = (uint32_t)(cmp_val >> 32);
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP1_SetValueL(uint32_t cmp_val)
{
	PMU_DEV->PMU_TMR_WUP_CMP1_VAL0 = cmp_val;
}

__HW_STATIC_INLINE
void LL_RTCTICK_CMP1_SetValueH(uint32_t cmp_val)
{
	PMU_DEV->PMU_TMR_WUP_CMP1_VAL1 = cmp_val;
}

__HW_STATIC_INLINE
uint64_t LL_RTCTICK_CMP1_GetValue(void)
{
	uint64_t tmr_val = PMU_DEV->PMU_TMR_WUP_CMP1_VAL1;
	tmr_val <<= 32;
	tmr_val |= PMU_DEV->PMU_TMR_WUP_CMP1_VAL0;
	return tmr_val;
}

/**
  * @}
  */


/** psic_ll_rtctick
  * @}
  */

/** PSIC_LL_Driver
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_RTCTICK_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
