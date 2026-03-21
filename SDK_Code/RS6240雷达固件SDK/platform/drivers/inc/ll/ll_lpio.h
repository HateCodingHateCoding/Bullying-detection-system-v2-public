/**
 **************************************************************************************************
 * @file    psic_ll_lpio.h
 * @brief   low power I/O hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_LPIO_H
#define _PSIC_LL_LPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

//#include "hw.h"
#include "types.h"
#include "hw_misc.h"

/** @addtogroup PSIC_LL_Driver
  * @{
  */


/** @defgroup GPIO_LL GPIO
  * @{
  */


/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */


/**
  * @}
  */


/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup GPIO_LL_Exported_Constants GPIO Exported Constants
  * @{
  */

/** @defgroup LPIO_LL_PIN PIN
  * @{
  */
#define LL_LPIO_PIN_0                               (0)
#define LL_LPIO_PIN_1                               (1)
#define LL_LPIO_PIN_2                               (2)
#define LL_LPIO_PIN_3                               (3)
#define LL_LPIO_PIN_4                               (4)
#define LL_LPIO_PIN_5                               (5)
#define LL_LPIO_PIN_6                               (6)
#define LL_LPIO_PIN_7                               (7)
#define LL_LPIO_PIN_8                               (8)
#define LL_LPIO_PIN_9                               (9)
#define LL_LPIO_PIN_10                              (10)
#define LL_LPIO_PIN_11                              (11)
#define LL_LPIO_PIN_12                              (12)
#define LL_LPIO_PIN_13                              (13)
#define LL_LPIO_PIN_14                              (14)
#define LL_LPIO_PIN_15                              (15)
#define LL_LPIO_PIN_16                              (16)
#define LL_LPIO_PIN_17                              (17)

#define LL_LPIO_PIN_MAX                             (17)

#define LL_LPIO_PIN_NUMBER                          (18)

#define LL_LPIO_PIN_MSK_0                           HW_BIT(0)
#define LL_LPIO_PIN_MSK_1                           HW_BIT(1)
#define LL_LPIO_PIN_MSK_2                           HW_BIT(2)
#define LL_LPIO_PIN_MSK_3                           HW_BIT(3)
#define LL_LPIO_PIN_MSK_4                           HW_BIT(4)
#define LL_LPIO_PIN_MSK_5                           HW_BIT(5)
#define LL_LPIO_PIN_MSK_6                           HW_BIT(6)
#define LL_LPIO_PIN_MSK_7                           HW_BIT(7)
#define LL_LPIO_PIN_MSK_8                           HW_BIT(8)
#define LL_LPIO_PIN_MSK_9                           HW_BIT(9)
#define LL_LPIO_PIN_MSK_10                          HW_BIT(10)
#define LL_LPIO_PIN_MSK_11                          HW_BIT(11)
#define LL_LPIO_PIN_MSK_12                          HW_BIT(12)
#define LL_LPIO_PIN_MSK_13                          HW_BIT(13)
#define LL_LPIO_PIN_MSK_14                          HW_BIT(14)
#define LL_LPIO_PIN_MSK_15                          HW_BIT(15)
#define LL_LPIO_PIN_MSK_16                          HW_BIT(16)
#define LL_LPIO_PIN_MSK_17                          HW_BIT(17)

#define LL_LPIO_PIN_MSK                             0x0003FFFF


#define LL_LPIO_PIN_TO_MSK(pin)                     HW_BIT(pin)
/**
  * @}
  */

/** @defgroup LPIO_LL_MODE Mode
  * @{
  */
#define LL_LPIO_MODE_GENE                           (0)
#define LL_LPIO_MODE_HOLD                           (1)
#define LL_LPIO_MODE_WAKE                           (2)
#define LL_LPIO_MODE_DISA                           (3)
/**
  * @}
  */

/** @defgroup LPIO_LL_PULL Pull Up Pull Down
  * @{
  */
#define LL_LPIO_PULL_NO                             (0) /*!< Select I/O no pull */
#define LL_LPIO_PULL_UP                             (1) /*!< Select I/O pull up */
#define LL_LPIO_PULL_DOWN                           (2) /*!< Select I/O pull down */
/**
  * @}
  */

/** @defgroup LPIO_LL_WAKEUP_TYPE
  * @{
  */
#define LL_LPIO_WAKE_RISING                         (0)
#define LL_LPIO_WAKE_FAILLING                       (1)
#define LL_LPIO_WAKE_RISING_FALLING                 (2)
#define LL_LPIO_WAKE_HIGH_LEVEL                     (3)
#define LL_LPIO_WAKE_LOW_LEVEL                      (4)
/**
  * @}
  */

/** @defgroup LPIO_LL_DEBOUNCE_VAL
  * @{
  */
#define LL_LPIO_DBC_VAL                     (0x10)
#define LL_LPIO_DBC_VAL_MSK                 (0x1F)
/**
  * @}
  */

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/** @defgroup LPIO_LL_Exported_Functions GPIO Exported Functions
  * @{
  */

/** @defgroup LPIO_LL_EF_Port_Configuration Port Configuration
  * @{
  */

/**
  * @brief  Configure lpio mode for a dedicated pin on dedicated port.
  * @note   I/O mode can be Input mode, General purpose output, Alternate function mode or Analog.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio LPIO
  * @param  pin This parameter can be one of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_LPIO_MODE_GENE
  *         @arg @ref LL_LPIO_MODE_HOLD
  *         @arg @ref LL_LPIO_MODE_WAKE
  *         @arg @ref LL_LPIO_MODE_DISA
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_SetPinMode(uint32_t pin, uint32_t mode)
{
    HW_SET_MSK_VAL(MISCREG1_DEV->WIO_MODE_SEL[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin), mode);
}

/**
  * @brief  Return lpio mode for a dedicated pin on dedicated port.
  * @note   I/O mode can be Input mode, General purpose output, Alternate function mode or Analog.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio LPIO
  * @param  pin This parameter can be one of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPIO_MODE_GENE
  *         @arg @ref LL_LPIO_MODE_HOLD
  *         @arg @ref LL_LPIO_MODE_WAKE
  *         @arg @ref LL_LPIO_MODE_DISA
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_GetPinMode(uint32_t pin)
{
    return HW_GET_MSK_VAL(MISCREG1_DEV->WIO_MODE_SEL[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin));
}

/**
  * @brief  Configure lpio pull-up or pull-down for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio LPIO
  * @param  pin This parameter can be one of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @param  pull This parameter can be one of the following values:
  *         @arg @ref LL_LPIO_PULL_NO
  *         @arg @ref LL_LPIO_PULL_UP
  *         @arg @ref LL_LPIO_PULL_DOWN
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_SetPinPull(uint32_t pin, uint32_t pull)
{
    HW_SET_MSK_VAL(MISCREG1_DEV->WIO_HOLD_PULLUPDOWN_SET[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin), pull);
}

/**
  * @brief  Return lpio pull-up or pull-down for a dedicated pin on a dedicated port
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio LPIO
  * @param  pin This parameter can be one of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPIO_PULL_NO
  *         @arg @ref LL_LPIO_PULL_UP
  *         @arg @ref LL_LPIO_PULL_DOWN
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_GetPinPull(uint32_t pin)
{
    return HW_GET_MSK_VAL(MISCREG1_DEV->WIO_HOLD_PULLUPDOWN_SET[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin));
}

/**
  * @brief  Configure lpio wakeup type for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio LPIO
  * @param  pin This parameter can be one of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @param  wake_type This parameter can be one of the following values:
  *         @arg @ref LL_LPIO_WAKE_RISING
  *         @arg @ref LL_LPIO_WAKE_FAILLING
  *         @arg @ref LL_LPIO_WAKE_RISING_FALLING
  *         @arg @ref LL_LPIO_WAKE_HIGH_LEVEL
  *         @arg @ref LL_LPIO_WAKE_LOW_LEVEL
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_SetPinWakeType(uint32_t pin, uint32_t wake_type)
{
    HW_SET_MSK_VAL(MISCREG1_DEV->WIO_WUP_TYPE[HW_IDX_4(pin)], HW_MSK_4(pin), HW_POS_4(pin), wake_type);
}

/**
  * @brief  Return lpio wakeup type for a dedicated pin on a dedicated port
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio LPIO
  * @param  pin This parameter can be one of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPIO_WAKE_RISING
  *         @arg @ref LL_LPIO_WAKE_FAILLING
  *         @arg @ref LL_LPIO_WAKE_RISING_FALLING
  *         @arg @ref LL_LPIO_WAKE_HIGH_LEVEL
  *         @arg @ref LL_LPIO_WAKE_LOW_LEVEL
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_GetPinWakeType(uint32_t pin)
{
    return HW_GET_MSK_VAL(MISCREG1_DEV->WIO_WUP_TYPE[HW_IDX_4(pin)], HW_MSK_4(pin), HW_POS_4(pin));
}

/**
  * @brief  Return full wakeup pending register value for a dedicated port.
  * @param  lpio LPIO
  * @retval Input data register value of port
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_GetWakePending(void)
{
    return MISCREG1_DEV->WIO_WUP_FLAG;
}

/**
  * @brief  Return if wakeup pending for several pins of dedicated port is high or low.
  * @param  lpio LPIO
  * @param  pin_mask This parameter can be a combination of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_IsWakePendingSet(uint32_t pin_mask)
{
    return (((MISCREG1_DEV->WIO_WUP_FLAG & pin_mask) == (pin_mask)) ? 1UL : 0UL);
}

__HW_STATIC_INLINE
uint32_t LL_LPIO_IsWakePending(uint32_t pin_mask)
{
    return (((MISCREG1_DEV->WIO_WUP_FLAG & pin_mask) == (0)) ? 0UL : 1UL);
}

/**
  * @brief  Clear several pins wakeup pending on dedicated gpio port.
  * @param  lpio LPIO
  * @param  pin_mask This parameter can be a combination of the following values:
  *         LL_LPIO_PIN_0 to LL_LPIO_PIN_15
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_ClearWakePending(uint32_t pin_mask)
{
    MISCREG1_DEV->WIO_WUP_CLR |= pin_mask;
}

/**
  * @brief  Set several pins to high level on dedicated gpio port.
  * @param  pin_mask This parameter can be a combination of the following values:
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_SetOutputPort(uint32_t pin_mask)
{
    MISCREG1_DEV->WIO_HOLD_OUTPUT = pin_mask;
}

/**
  * @brief  Get several pins to high level on dedicated gpio port.
  * @retval port value
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_GetOutputPort(void)
{
    return MISCREG1_DEV->WIO_HOLD_OUTPUT;
}

/**
  * @brief  Set several pins to low level on dedicated gpio port.
  * @param  pin_mask This parameter can be a combination of the following values:
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_SetOutputPinMask(uint32_t pin_mask)
{
    MISCREG1_DEV->WIO_HOLD_OUTPUT |= pin_mask;
}

/**
  * @brief  Set several pins to low level on dedicated gpio port.
  * @param  pin_mask This parameter can be a combination of the following values:
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_ResetOutputPinMask(uint32_t pin_mask)
{
    MISCREG1_DEV->WIO_HOLD_OUTPUT &= ~pin_mask;
}

/**
  * @brief  Configure lpio wakeup gpadc.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  pin This parameter can be one of the following values:
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_EnabledPinWakeGPADC(uint32_t pin)
{
    HW_SET_BIT(MISCREG1_DEV->WIO_GPADC_MODE, pin);
}

/**
  * @brief  Configure lpio wakeup gpadc.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  pin This parameter can be one of the following values:
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_DisablePinWakeGPADC(uint32_t pin)
{
    HW_CLR_BIT(MISCREG1_DEV->WIO_GPADC_MODE, pin);
}

/**
  * @brief  Return lpio wakeup type for a dedicated pin on a dedicated port
  * @note   Warning: only one pin can be passed as parameter.
  * @param  pin This parameter can be one of the following values:
  * @retval Returned value can be one of the following values:
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_IsEnabledPinWakeGPADC(uint32_t pin)
{
    return HW_TEST_BIT(MISCREG1_DEV->WIO_GPADC_MODE, pin);
}

/**
  * @brief  Configure wpio delay for a dedicated port.
  * @param  delay This parameter is 0 - 31:
  * @retval None
  */
__HW_STATIC_INLINE
void LL_LPIO_SetPinDelay(uint32_t delay)
{
    HW_SET_MSK_VAL(MISCREG1_DEV->WIO_WUP_DBC_SET, WIO_WUP_DBC_SET_Msk, WIO_WUP_DBC_SET_Pos, delay);
}

/**
  * @brief  Get wpio delay for a dedicated port.
  * @param  delay This parameter is 0 - 31:
  * @retval None
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_GetPinDelay(void)
{
    return HW_GET_MSK_VAL(MISCREG1_DEV->WIO_WUP_DBC_SET, WIO_WUP_DBC_SET_Msk, WIO_WUP_DBC_SET_Pos);
}

/**
  * @brief  reset wpio registers.
  * @param  None:
  * @retval None
  */
__HW_STATIC_INLINE
uint32_t LL_LPIO_Reset(void)
{
    MISCREG1_DEV->WIO_GPADC_MODE = 0;
    MISCREG1_DEV->WIO_MODE_SEL[0] = 0;
    MISCREG1_DEV->WIO_MODE_SEL[1] = 0;
    MISCREG1_DEV->WIO_HOLD_OUTPUT = 0;
    MISCREG1_DEV->WIO_HOLD_PULLUPDOWN_SET[0] = 0;
    MISCREG1_DEV->WIO_HOLD_PULLUPDOWN_SET[1] = 0;
    MISCREG1_DEV->WIO_WUP_TYPE[0] = 0;
    MISCREG1_DEV->WIO_WUP_TYPE[1] = 0;
    MISCREG1_DEV->WIO_WUP_TYPE[2] = 0;
    MISCREG1_DEV->WIO_WUP_TYPE[3] = 0;
    MISCREG1_DEV->WIO_WUP_CLR = LL_LPIO_PIN_MSK;
    MISCREG1_DEV->WIO_WUP_DBC_SET = LL_LPIO_DBC_VAL;
	return 0;
}

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_LPIO_H */

