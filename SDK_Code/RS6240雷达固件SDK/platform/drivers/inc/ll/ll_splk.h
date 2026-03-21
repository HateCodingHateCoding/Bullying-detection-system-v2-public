/**
 **************************************************************************************************
 * @file    psic_ll_splk.h
 * @brief   spinlock hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_SPLK_H
#define _PSIC_LL_SPLK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "soc.h"
#include "hal_types.h"
#include "hw_splk.h"


#define SPLK_DEV                        ((splk_dev_t *)   SPLK_BASE_ADDR)  /*!< 0x4001 2C00 */
/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup psic_ll_splk
  * @{
  */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup XXXX
  * @{
  */

/** @defgroup spinlock status
  * @{
  */
#define LL_SPINLOCK_LOCK                        0x01
#define LL_SPINLOCK_UNLOCK                      0x00
/**
  * @}
  */

/** @defgroup spinlock ID nums
  * @{
  */
#define SPLK_ID_CLK_PWR                     0
#define SPLK_ID_SRAM_STORE                  1
#define SPLK_ID_FLASH_PSRAM                 2
#define SPLK_ID_DMA                         3

#define SPLK_ID_MMW							9
#define SPLK_ID_MBOX						10
/**
  * @}
  */


/** @defgroup XXX
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */


/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/** @defgroup XXX
  * @{
  */

/** @defgroup XXXX
  * @{
  */
#include "ll_rcc_dev.h"

__HW_STATIC_INLINE
int LL_SPLK_Init(void)
{
    if (LL_RCC_SPLK_IsRelease())
        return HW_SUCCESS;

    LL_RCC_SPLK_EnableBusClock();
    LL_RCC_SPLK_Reset();
    return HW_SUCCESS;
}

__HW_STATIC_INLINE
int LL_SPLK_Reinit(void)
{
    if (LL_RCC_SPLK_IsRelease())
        return HW_SUCCESS;

    LL_RCC_SPLK_Reset();
    return HW_SUCCESS;
}

/**
  * @brief  Taken lock.
  * @param  splk_dev : Spinlock device
  * @param  num This parameter can be a combination of the following values:
  *         @arg @ref (0 <= num < 16)
  * @retval State of spinlock (1 or 0)
  */
__HW_STATIC_INLINE
uint32_t LL_SPLK_TryLock(splk_dev_t *splk_dev, uint32_t num)
{
    return HW_GET_VAL(splk_dev->CTRL[num]);
}

/**
  * @brief  Check if AHB peripheral clock is enabled or not
  * @brief  Taken lock.
  * @param  splk_dev : Spinlock device
  * @param  num This parameter can be a combination of the following values:
  *         @arg @ref (0 <= num < 16)

  */
__HW_STATIC_INLINE
void LL_SPLK_TryUnLock(splk_dev_t *splk_dev, uint32_t num)
{
    HW_SET_VAL(splk_dev->CTRL[num], 0);
}


__HW_STATIC_INLINE
int LL_SPLK_RawLock(uint8_t num, uint8_t block)
{
    if (block) {
        while (HW_GET_VAL(SPLK_DEV->CTRL[num]) == 0) {
        }
        return 1;
    } else {
        return HW_GET_VAL(SPLK_DEV->CTRL[num]);
    }
}


__HW_STATIC_INLINE
void LL_SPLK_RawUnlock(uint8_t num)
{
    HW_SET_VAL(SPLK_DEV->CTRL[num], 0);
}


/**
  * @}
  */


/**
  * @}
  */


/** psic_ll_splk
  * @}
  */


/** PSIC_LL_Driver
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_SPLK_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
