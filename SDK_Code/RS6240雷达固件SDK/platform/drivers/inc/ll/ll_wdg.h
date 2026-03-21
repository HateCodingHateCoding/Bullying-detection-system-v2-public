/**
 **************************************************************************************************
 * @file        psic_ll_wdg.h
 * @brief       Header file of WDT LL module.
 * @attention   Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_WDG_H
#define _PSIC_LL_WDG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "types.h"
#include "hw_wdg.h"
#include "soc.h"


#define WDG0_DEV                        ((wdg_dev_t *)    WDG0_BASE_ADDR)  /*!< 0x4001 1000 */
#define WDG1_DEV                        ((wdg_dev_t *)    WDG1_BASE_ADDR)  /*!< 0x4001 1400 */

/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup psic_ll_wdg
  * @{
  */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/* -------- WDT_CR : (WDT Offset: 0x00) Control Register -------- */

#define LL_WDT_CR_WDREN                     (1U << WDTENABLE_Pos)       /*!< WDT_CR Watchdog Enable */
#define LL_WDT_CR_RMOD_INTR                 (1U << RESPONSEMODE_Pos)    /*!< WDT_CR Response Mode */
#define LL_WDT_CR_RPL_CYC2                  (0x0U)                      /*!< WDT_CR Rest pulse length with 2 pclock cycles */
#define LL_WDT_CR_RPL_CYC4                  (0x1U)                      /*!< WDT_CR Rest pulse length with 4 pclock cycles */
#define LL_WDT_CR_RPL_CYC8                  (0x2U)                      /*!< WDT_CR Rest pulse length with 8 pclock cycles */
#define LL_WDT_CR_RPL_CYC16                 (0x3U)                      /*!< WDT_CR Rest pulse length with 16 pclock cycles */
#define LL_WDT_CR_RPL_CYC32                 (0x4U)                      /*!< WDT_CR Rest pulse length with 32 pclock cycles */
#define LL_WDT_CR_RPL_CYC64                 (0x5U)                      /*!< WDT_CR Rest pulse length with 64 pclock cycles */
#define LL_WDT_CR_RPL_CYC128                (0x6U)                      /*!< WDT_CR Rest pulse length with 128 pclock cycles */
#define LL_WDT_CR_RPL_CYC256                (0x7U)                      /*!< WDT_CR Rest pulse length with 256 pclock cycles */
#define LL_WDT_CRR_VAL                      (0xa5e7U)

/* WDG0 clock device AND WDG1 clock device in CCMU register */
#define LL_WDT_CLOCK_DEV_WDG0               (1)
#define LL_WDT_CLOCK_DEV_WDG1               (2)

/* foure clock bus for WDG0 and WDG1 to select */
#define LL_WDT_CLOCK_BUS_LSI                (1)
#define LL_WDT_CLOCK_BUS_MSI                (2)
#define LL_WDT_CLOCK_BUS_HSE                (4)
#define LL_WDT_CLOCK_BUS_APB1               (11)

#define LL_WDT_PRESCALER_MAX                (128)

#define LL_WDT_RELOAD_MAX                   (TOP_Msk)

#define LL_WDT_RST_PULSE_LEN_MAX            (RPL_Msk)


#define LL_WDT_TIMEOUT_PERIOD_MIN           (64*1024)

#define LL_WDT_TORR_BASE_NUMBER             (64*1024)     /* TORR : 64*1024*2^(0 - 15) */

#define LL_WDT_TORR_RELOAD_MAX              (TOP_Msk)
/**
  * @}
  */
/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/**
 * \brief Enable watchdog with given mode.
 *
 * \note The Watchdog Mode Register (WDT_MR) can be written only once.
 * Only a processor reset resets it.
 *
 * \param dwMode   WDT mode to be set
 */
__HW_STATIC_INLINE
void LL_WDT_Enable(wdg_dev_t *watchdog)
{
    HW_SET_BIT(watchdog->CR, WDTENABLE_PosL);
}

/**
 * \brief Disable watchdog.
 *
 * \note The Watchdog Mode Register (WDT_MR) can be written only once.
 * Only a processor reset resets it.
 */
__HW_STATIC_INLINE
void LL_WDT_Disable(wdg_dev_t *watchdog)
{
    HW_CLR_BIT(watchdog->CR, WDTENABLE_PosL);
}

/**
 * \brief Get watchdog eanble status.
 */
__HW_STATIC_INLINE
uint32_t LL_WDT_GetEnableStatus(wdg_dev_t *watchdog)
{
  return HW_GET_MSK_VAL(watchdog->CR, WDTENABLE_Msk, WDTENABLE_PosL);
}

/**
 * \brief Watchdog restart.
 */
__HW_STATIC_INLINE
void LL_WDT_Restart(wdg_dev_t *watchdog)
{
    HW_SET_MSK_VAL(watchdog->RST, CRR_Msk, CRR_PosL, LL_WDT_CRR_VAL);
}

/**
 * \brief Set Watchdog Reset pulse length
 */
__HW_STATIC_INLINE
void LL_WDT_SetRstPulseLen(wdg_dev_t *watchdog, uint32_t plen)
{
    HW_SET_MSK_VAL(watchdog->CR, RPL_Msk, RPL_PosL, plen);
}

/**
 * \brief Set Watchdog Response mode
 * mode : 0 - system reset, 1 - interrupt
 */
__HW_STATIC_INLINE
void LL_WDT_SetRespMode(wdg_dev_t *watchdog, uint32_t mode)
{
    if (mode) {
        HW_SET_BIT(watchdog->CR, RESPONSEMODE_PosL);
    } else {
        HW_CLR_BIT(watchdog->CR, RESPONSEMODE_PosL);
    }
}

/**
 * \brief Get current counter value .
 */
__HW_STATIC_INLINE
uint32_t LL_WDT_GetCcvr(wdg_dev_t *watchdog)
{
    return watchdog->CVR & CCVR_Msk;
}

/**
 * \brief Set timeout period
 */
__HW_STATIC_INLINE
void LL_WDT_SetTimeoutPeriod(wdg_dev_t *watchdog, uint32_t top)
{
    HW_SET_MSK_VAL(watchdog->TOP, TOP_Msk, TOP_PosL, top);
}

/**
 * \brief Watchdog get status.
 */
__HW_STATIC_INLINE
uint32_t LL_WDT_GetStatus(wdg_dev_t *watchdog)
{
    return HW_GET_MSK_VAL(watchdog->ISR, STAT_Msk, STAT_PosL);
}

/**
 * \brief Watchdog clear status.
 */
__HW_STATIC_INLINE
void LL_WDT_ClrStatus(wdg_dev_t *watchdog)
{
    HW_GET_BIT(watchdog->ICR, EOI_PosL);
}


/**
 * \brief Watchdog get period.
 *
 * \param dwMs   desired watchdog period in millisecond.
 */
__HW_STATIC_INLINE
uint32_t LL_WDT_GetPeriod(uint32_t dwMs)
{
    if ((dwMs < 4) || (dwMs > 16000)) {
        return 0;
    }
    return ((dwMs << 8) / 1000);
}

/** psic_ll_wdg
  * @}
  */


/** PSIC_LL_Driver
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_WDG_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
