/**
 **************************************************************************************************
 * @brief   psic_ll_timer.h
 * @brief   timer hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_TIMER_H
#define _PSIC_LL_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

#include "types.h"
#include "hw_tim.h"
#include "soc.h"
#include "ll_rcc_dev.h"

#define TIMR0_DEV                        ((timer_dev_t *)    TIMR0_BASE_ADDR)  /*!< 0x4001 0000 */
#define TIMR1_DEV                        ((timer_dev_t *)    TIMR0_BASE_ADDR + 0x18)  /*!< 0x4001 0018 */
#define TIMR2_DEV                        ((timer_dev_t *)    TIMR1_BASE_ADDR)  /*!< 0x4001 0400 */
#define TIMR3_DEV                        ((timer_dev_t *)    TIMR1_BASE_ADDR + 0x18)  /*!< 0x4001 0418 */

/** @addtogroup PSIC_LL_Driver
  * @{
  */


/** @defgroup TIMER_LL TIMER
  * @{
  */


/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */

__HW_STATIC_INLINE
void LL_TIM_EnableInterruptMask(timer_dev_t *timer)
{
    HW_SET_BIT(timer->CTRL, TIMER_INTERRUPT_MASK_Pos);
}

__HW_STATIC_INLINE
void LL_TIM_DisableInterruptMask(timer_dev_t *timer)
{
    HW_CLR_BIT(timer->CTRL, TIMER_INTERRUPT_MASK_Pos);
}

__HW_STATIC_INLINE
void LL_TIM_SetUserDefinedMode(timer_dev_t *timer)
{
    HW_SET_BIT(timer->CTRL, TIMER_MODE_Pos);
}

__HW_STATIC_INLINE
void LL_TIM_SetFreeRuningMode(timer_dev_t *timer)
{
    HW_CLR_BIT(timer->CTRL, TIMER_MODE_Pos);
}

__HW_STATIC_INLINE
void LL_TIM_Enable(timer_dev_t *timer)
{
    HW_SET_BIT(timer->CTRL, TIMER_ENABLE_Pos);
}

__HW_STATIC_INLINE
void LL_TIM_Disable(timer_dev_t *timer)
{
    HW_CLR_BIT(timer->CTRL, TIMER_ENABLE_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_TIM_IsEnable(timer_dev_t *timer)
{
    return HW_GET_MSK_VAL(timer->CTRL, TIMER_ENABLE_Msk, TIMER_ENABLE_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_TIM_ClearInterrupt(timer_dev_t *timer)
{
    return HW_GET_MSK_VAL(timer->CLR, TIMERNEOI_Msk, TIMERNEOI_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_TIM_GetInterruptStatus(timer_dev_t *timer)
{
    return HW_GET_MSK_VAL(timer->ISR, TIMERSRAWINTSTAT_Msk, TIMERSRAWINTSTAT_Pos);
}

__HW_STATIC_INLINE
void LL_TIM_SetLoadCounter(timer_dev_t *timer, uint32_t value)
{
    HW_SET_VAL(timer->LVR, value);
}

__HW_STATIC_INLINE
uint32_t LL_TIM_GetLoadCounter(timer_dev_t *timer)
{
    return HW_GET_VAL(timer->LVR);
}

__HW_STATIC_INLINE
uint32_t LL_TIM_GetCurrentValue(timer_dev_t *timer)
{
    return HW_GET_VAL(timer->CVR);
}

__HW_STATIC_INLINE
void LL_TIM_ClockCtrlOn(uint32_t clk_src, uint32_t clk_div, bool on)
{
    if (on == true) {
        LL_RCC_TMR0_SetClockSource(clk_src);
        LL_RCC_TMR0_SetPrescaler(clk_div);
        LL_RCC_TMR0_DisableBusClock();
        LL_RCC_TMR0_DisableClock();
        LL_RCC_TMR0_Reset();
        LL_RCC_TMR0_EnableBusClock();
        LL_RCC_TMR0_EnableClock();
        //LL_TIM_SetFreeRuningMode(TIMR0_DEV);
        //LL_TIM_DisableInterruptMask(TIMR0_DEV);
    } else {
        LL_RCC_TMR0_DisableBusClock();
        LL_RCC_TMR0_DisableClock();
        LL_RCC_TMR0_Reset();
        LL_RCC_TMR0_DisableClock();
        LL_RCC_TMR0_DisableBusClock();
    }
}

__HW_STATIC_INLINE
void LL_TIM_BusyWait(uint32_t wait_cycle)
{
    LL_TIM_DisableInterruptMask(TIMR0_DEV);
    LL_TIM_SetUserDefinedMode(TIMR0_DEV);
    LL_TIM_SetLoadCounter(TIMR0_DEV, wait_cycle);
    LL_TIM_Enable(TIMR0_DEV);
    while (!LL_TIM_GetInterruptStatus(TIMR0_DEV)) {
        ;
    }
    LL_TIM_Disable(TIMR0_DEV);
}

__HW_STATIC_INLINE
void LL_TIM_freeRunning_Cfg(void)
{
    LL_TIM_DisableInterruptMask(TIMR0_DEV);
    LL_TIM_SetFreeRuningMode(TIMR0_DEV);
    LL_TIM_Enable(TIMR0_DEV);
}

__HW_STATIC_INLINE
uint32_t LL_TIM_ReadCycles(void)
{
	return LL_TIM_GetCurrentValue(TIMR0_DEV);
}

/**
  * @}
  */


/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_TIMER_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */


