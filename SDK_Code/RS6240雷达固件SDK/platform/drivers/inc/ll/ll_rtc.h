
/**
 **************************************************************************************************
 * @brief   psic_ll_rtc.h
 * @brief   real time clock low layer define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_RTC_H
#define _PSIC_LL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

#include "psic_types.h"
#include "psic_hw.h"
#include "hw_misc.h"
#include "hw_rtc.h"


/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup psic_ll_rtc
  * @{
  */


/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
 /* CR */
#define LL_RTC_CR_CLEAR_UPDATE_REQ							(0x00U)    /*!< RTC_CR Upate Request Time and Calendar. */
#define LL_RTC_CR_RECV_UPDATE_REQ							(0x03U)    /*!< RTC_CR Upate Request Time and Calendar. */


/* MR */
#define LL_RTC_MR_CALRMODE_GREGORIAN						(0x00U)    /*!< RTC_MR  Gregorian calendar. */
#define LL_RTC_MR_CALRMODE_PERSIAN							(0x01U)    /*!< RTC_MR  Persian calendar. */

#define LL_RTC_MR_HOURMODE_24								(0x00U)    /*!< RTC_MR  24 hour mode. */
#define LL_RTC_MR_HOURMODE_12								(0x01U)    /*!< RTC_MR  12 hour mode. */


/* RTC_TIMR */
#define LL_RTC_TIME_AM										(0x00)    /*!< RTC_TIME Current Time is AM */
#define LL_RTC_TIME_PM										(0x01)    /*!< RTC_TIME Current Time is PM */


/* RTC_CALR */
#define LL_RTC_CALR_CENT_PERSIAN_MAX						(0x14U)    /*!< RTC_CALR Max century value (Persian) BCD is 14*/
#define LL_RTC_CALR_CENT_PERSIAN_MIN						(0x13U)    /*!< RTC_CALR Max century value (Persian) BCD is 13*/

#define LL_RTC_CALR_CENT_GREGORIAN_MAX						(0x20U)    /*!< RTC_CALR Max cent value (Gregorian) BCD is 20*/
#define LL_RTC_CALR_CENT_GREGORIAN_MIN						(0x19U)    /*!< RTC_CALR Max cent value (Gregorian) BCD is 19*/


/* RTC_SR */
#define LL_RTC_SR_TIME_DATE_ERR_BIT							(0x05U)   /*!< SR ISR Status bit is for invail date or time that are not-BCD */
#define LL_RTC_SR_CALR_EVT_BIT								(0x04U)   /*!< SR ISR Status bit is for calendar event occurred since last clear*/
#define LL_RTC_SR_TIME_EVT_BIT								(0x03U)   /*!< SR ISR Status bit is for time event occurred since last clear*/
#define LL_RTC_SR_SEC_EVT_BIT								(0x02U)   /*!< SR ISR Status bit is for second event occurred since last clear*/
#define LL_RTC_SR_ALARM_FLAG_BIT							(0x01U)   /*!< SR ISR Status bit is for alarm occurred matching conditions*/
#define LL_RTC_SR_ACK_UPDATE_BIT							(0x00U)   /*!< SR ISR Status bit is for acking that the calendar and time is ready to update*/


/*RTC_TIMALR*/
#define LL_RTC_TIMALR_SEC_EN_MASK							(HW_BIT(SECALEN_Pos))
#define LL_RTC_TIMALR_MIN_EN_MASK							(HW_BIT(MINALEN_Pos))
#define LL_RTC_TIMALR_HOUR_EN_MASK							(HW_BIT(HOURALEN_Pos))
/* RTC_CALALR */
#define LL_RTC_CALALR_DAY_EN_MASK							(HW_BIT(DAYALEN_Pos))
#define LL_RTC_CALALR_DATE_EN_MASK							(HW_BIT(DATEALEN_Pos))
#define LL_RTC_CALALR_MONTH_EN_MASK							(HW_BIT(MONALEN_Pos))


/* RTC_IER */
#define LL_RTC_IER_ALL_INT_ENABLE							(0x3FU)    /*!< Enable all interrrupt*/
/* RTC_IDR */
#define LL_RTC_IER_ALL_INT_DISABLE							(0x3FU)    /*!< Disable all interrrupt*/
/* RTC_WPMR */
/* Write protection key is "RTC" in ASCII */
#define LL_RTC_WPMR_WPKEY_ENABLE							(0x52544301U)    /*!< Protection key*/
#define LL_RTC_WPMR_WPKEY_DISABLE							(0x52544300U)    /*!< Protection key*/

#define LL_RTC_WEEKDAY_SUNDAY								(0x07U)

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup RTC Function.
  * @{
  */
/**
  * @{
  */
/**
 * @brief  (CR) Stop the rtc time counting, receive RTC update request time and calendar.
 * @param  rtc is RTC Instance
 * @retval None
 **/
__HW_STATIC_INLINE
void LL_RTC_Update_Datetime_Req(rtc_dev_t *rtc)
{
	HW_SET_MSK_VAL(rtc->CR, UPDRQE_Msk, UPDRQE_Pos, LL_RTC_CR_RECV_UPDATE_REQ);
}
/**
 * @brief  (CR) Close rtc update request time and calendar, Rerun the rtc time counting
 * @param  rtc is RTC Instance
 * @Note   if UPDCAL and UPDTIM has been previously written to 1, stops the update procedure
 * @retval None
 **/
__HW_STATIC_INLINE
void LL_RTC_Update_Datetime_Clr_Req(rtc_dev_t *rtc)
{
	HW_SET_MSK_VAL(rtc->CR, UPDRQE_Msk, UPDRQE_Pos, LL_RTC_CR_CLEAR_UPDATE_REQ);
}


/**
  * @brief  (MR) retrieve time mode (12 hour mode or 24 hour mode).
  * @param  rtc is RTC Instance
  * @retval 0:12 hour mode
  * 		1:24 hour mode
  */
__HW_STATIC_INLINE
bool LL_RTC_Get_HourMode(rtc_dev_t *rtc)
{
	return (bool)HW_GET_MSK_VAL(rtc->MR, HRMOD_Msk, HRMOD_Pos);
}
/**
  * @brief   (MR) set time mode (12 hour mode or 24 hour mode).
  * @param1  rtc is RTC Instance
  * @param2  hour mode
  			 @arg @ref LL_RTC_MR_HOURMODE_24
  			 @arg @ref LL_RTC_MR_HOURMODE_12
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_HourMode(rtc_dev_t *rtc, bool hour_mode)
{
	HW_SET_MSK_VAL(rtc->MR, HRMOD_Msk, HRMOD_Pos, hour_mode);
}
/**
  * @brief  Retrieve calendar mode (gregorian or persian)
  * @param  rtc is RTC Instance
  * @retval 0:gregorian
  * 		1:persian
  */
__HW_STATIC_INLINE
bool LL_RTC_Get_CalendarMode(rtc_dev_t *rtc)
{
	return (bool)HW_GET_MSK_VAL(rtc->MR, CALRMOD_Msk, CALRMOD_Pos);
}
/**
  * @brief   Set calendar mode (gregorian or persian)
  * @param1  rtc is RTC Instance
  * @param2  calendar mode
  * 		 @arg @ref LL_RTC_MR_CALRMODE_GREGORIAN
  * 		 @arg @ref LL_RTC_MR_CALRMODE_PERSIAN
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_CalendarMode(rtc_dev_t *rtc, bool calr_mode)
{
	HW_SET_MSK_VAL(rtc->MR, CALRMOD_Msk, CALRMOD_Pos, calr_mode);
}



/**
  * @brief  (TIMR) retrieve Hour in BCD format
  * @param  rtc is RTC Instance
  * @note   The range that can be set is 1–12 (BCD) in 12-hour mode or 0–23 (BCD) in 24-hour mode.
  * @retval hour in BCD format (01 - 12) BCD
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Hour(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->TIMR, HOUR_Msk, HOUR_Pos);
}
/**
  * @brief  (TIMR) set Hour in BCD format
  * @param1  rtc is RTC Instance
  * @param2  hour in BCD format (01 - 12) BCD
  * @note   The range that can be set is 1–12 (BCD) in 12-hour mode or 0–23 (BCD) in 24-hour mode.
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Hour(rtc_dev_t *rtc, uint8_t bcd_hour)
{
	HW_SET_MSK_VAL(rtc->TIMR, HOUR_Msk, HOUR_Pos, bcd_hour);
}
/**
  * @brief  (TIMR) retrieve Minute in BCD format
  * @param  rtc is RTC Instance
  * @retval minute in BCD format (00 - 59) BCD
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Minute(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->TIMR, MIN_Msk, MIN_Pos);
}
/**
  * @brief  (TIMR) set Minute in BCD format
  * @param1  rtc is RTC Instance
  * @param2  minute in BCD format (00 - 59) BCD
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Minute(rtc_dev_t *rtc, uint8_t bcd_min)
{
	HW_SET_MSK_VAL(rtc->TIMR, MIN_Msk, MIN_Pos, bcd_min);
}
/**
  * @brief  (TIMR) retrieve Second in BCD format
  * @param  rtc is RTC Instance
  * @retval second in BCD format (00 - 59) BCD
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Second(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->TIMR, SEC_Msk, SEC_Pos);
}
/**
  * @brief  (TIMR) set Second in BCD format
  * @param1  rtc is RTC Instance
  * @param2  second in BCD format (00 - 59) BCD
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Second(rtc_dev_t *rtc, uint8_t bcd_sec)
{
	HW_SET_MSK_VAL(rtc->TIMR, SEC_Msk, SEC_Pos, bcd_sec);
}
/**
  * @brief  (TIMR) retrieve AM PM  Flag bit
  * @param  rtc is RTC Instance
  * @retval 0:AM 1:PM
  */
__HW_STATIC_INLINE
bool LL_RTC_Get_AMPM(rtc_dev_t *rtc)
{
	return (bool)HW_GET_MSK_VAL(rtc->TIMR, AMPM_Msk, AMPM_Pos);
}
/**
  * @brief  (TIMR) set AM PM  Flag bit
  * @param1  rtc is RTC Instance
  * @param2  ampm
  * 		 @arg @ref LL_RTC_TIME_AM
  * 		 @arg @ref LL_RTC_TIME_PM
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_AMPM(rtc_dev_t *rtc, bool am_pm)
{
	HW_SET_MSK_VAL(rtc->TIMR, AMPM_Msk, AMPM_Pos, am_pm);
}



/**
  * @brief  (CALR) retrieve Century in BCD format
  * @param  rtc is RTC Instance
  * @retval Cent in BCD format (19 - 20) BCD format
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Cent(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALR, CENT_Msk, CENT_Pos);
}
/**
  * @brief  (CALR) set Century in BCD format
  * @param1  rtc is RTC Instance
  * @param2  century in BCD format (19 - 20) BCD format
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Cent(rtc_dev_t *rtc, uint8_t bcd_cent)
{
	HW_SET_MSK_VAL(rtc->CALR, CENT_Msk, CENT_Pos, bcd_cent);
}
/**
  * @brief  (CALR) retrieve Year in BCD format
  * @param  rtc is RTC Instance
  * @retval Year in BCD format (00 - 99) BCD
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Year(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALR, YEAR_Msk, YEAR_Pos);
}
/**
  * @brief  (CALR) set Year in BCD format
  * @param1  rtc is RTC Instance
  * @param2  Year in BCD format (00 - 99) BCD
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Year(rtc_dev_t *rtc, uint8_t bcd_year)
{
	HW_SET_MSK_VAL(rtc->CALR, YEAR_Msk, YEAR_Pos, bcd_year);
}
/**
  * @brief  (CALR) retrieve Month in BCD format
  * @param  rtc is RTC Instance
  * @retval Month in BCD format (01 - 12) BCD
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Month(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALR, MONTH_Msk, MONTH_Pos);
}
/**
  * @brief  (CALR) set Month in BCD format
  * @param1  rtc is RTC Instance
  * @param2  Month in BCD format (01 - 12) BCD
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Month(rtc_dev_t *rtc, uint8_t bcd_month)
{
	HW_SET_MSK_VAL(rtc->CALR, MONTH_Msk, MONTH_Pos, bcd_month);
}
/**
  * @brief  (CALR) retrieve Day in BCD format
  * @param  rtc is RTC Instance
  * @retval weak day in BCD format (01 - 07) BCD
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_WDay(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALR, DAY_Msk, DAY_Pos);
}
/**
  * @brief  (CALR) set Day in BCD format
  * @param1  rtc is RTC Instance
  * @param2  weak day in BCD format (01 - 07) BCD
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_WDay(rtc_dev_t *rtc, uint8_t bcd_wday)
{
	HW_SET_MSK_VAL(rtc->CALR, DAY_Msk, DAY_Pos, bcd_wday);
}
/**
  * @brief  (CALR) retrieved Date in BCD format
  * @param  rtc is RTC Instance
  * @retval mday in BCD format (01 - 31) BCD
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_MDay(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALR, DATE_Msk, DATE_Pos);
}
/**
  * @brief  (CALR) set Date in BCD format
  * @param1  rtc is RTC Instance
  * @param2  mday in BCD format (01 - 31) BCD
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_MDay(rtc_dev_t *rtc, uint8_t bcd_mday)
{
	HW_SET_MSK_VAL(rtc->CALR, DATE_Msk, DATE_Pos, bcd_mday);
}



/**
  * @brief  (TIMALR) disable hour alarm
  * @param  rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Hour_Alarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_BIT(rtc->TIMALR, HOURALEN_Pos);
}
/**
  * @brief  (TIMALR) enable hour alarm
  * @param  rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Hour_Alarm_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->TIMALR, HOURALEN_Pos);
}
/**
  * @brief  (TIMALR) set hour alarm value
  * @param  rtc is RTC Instance
  * @param2  set hour value(BCD)
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Hour_Alarm(rtc_dev_t *rtc, uint8_t value)
{
	HW_SET_MSK_VAL(rtc->TIMALR, HOURAL_Msk, HOURAL_Pos, value);
}
/**
  * @brief  (TIMALR) get hour alarm value
  * @param  rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Hour_Alarm(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->TIMALR, HOURAL_Msk, HOURAL_Pos);
}
/**
  * @brief  (TIMALR) set alarm hour mode value
  * @param  rtc is RTC Instance
  * @param1  set hour value 0:AM 1:PM
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_AMPM_Alarm(rtc_dev_t *rtc, uint8_t value)
{
	HW_SET_MSK_VAL(rtc->TIMALR, AMPMIND_Msk, AMPMIND_Pos, value);
}
/**
  * @brief  (TIMALR) get hour alarm value
  * @param  rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_AMPM_Alarm(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->TIMALR, AMPMIND_Msk, AMPMIND_Pos);
}
/**
  * @brief	(TIMALR) disable minute alarm
  * @param	rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Minute_Alarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_BIT(rtc->TIMALR, MINALEN_Pos);
}
/**
  * @brief  (TIMALR) enable minute alarm
  * @param  rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Minute_Alarm_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->TIMALR, MINALEN_Pos);
}
/**
  * @brief  (TIMALR) set minute alarm value
  * @param  rtc is RTC Instance
  * @param2  set minute value(BCD)
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Minute_Alarm(rtc_dev_t *rtc, uint8_t value)
{
	HW_SET_MSK_VAL(rtc->TIMALR, MINAL_Msk, MINAL_Pos, value);
}
/**
  * @brief  (TIMALR) get minute alarm value
  * @param  rtc is RTC Instance
  * @param2  set minute value(BCD)
  * @retval  None
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Minute_Alarm(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->TIMALR, MINAL_Msk, MINAL_Pos);
}
/**
  * @brief	(TIMALR) disable second alarm
  * @param	rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Second_Alarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_BIT(rtc->TIMALR, SECALEN_Pos);
}
/**
  * @brief  (TIMALR) enable second alarm
  * @param  rtc is RTC Instance
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Second_Alarm_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->TIMALR, SECALEN_Pos);
}
/**
  * @brief  (TIMALR) set second alarm value
  * @param1  rtc is RTC Instance
  * @param2  set second value(BCD)
  * @retval  None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Second_Alarm(rtc_dev_t *rtc, uint8_t value)
{
	HW_SET_MSK_VAL(rtc->TIMALR, SECAL_Msk, SECAL_Pos, value);
}
/**
  * @brief  (TIMALR) get second alarm value
  * @param1  rtc is RTC Instance
  * @param2  second value(BCD)
  * @retval  None
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Second_Alarm(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->TIMALR, SECAL_Msk, SECAL_Pos);
}
/**
  * @brief  (TIMALR) disable time alarm
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_TimeAlarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_MSK(rtc->TIMALR, ((1U << HOURALEN_Pos) | \
							(1U << MINALEN_Pos) | \
							(1U << SECALEN_Pos)));
}
/**
  * @brief  (TIMALR) enable time alarm.
  * @param1  rtc is RTC Instance
  * @param2  which alarm enbale
  * 		@arg @ref LL_RTC_TIMALR_ALARM_SEC_EN_MASK
  * 		@arg @ref LL_RTC_TIMALR_ALARM_MIN_EN_MASK
  * 		@arg @ref LL_RTC_TIMALR_ALARM_HOUR_EN_MASK
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_TimeAlarm_Enable(rtc_dev_t *rtc, uint32_t time_mask)
{
	HW_SET_MSK(rtc->TIMALR, (time_mask) & ((1U << HOURALEN_Pos) | \
											(1U << MINALEN_Pos) | \
											(1U << SECALEN_Pos)));
}



/**
  * @brief	(CALALR) disable day alarm.
  * @param	rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Day_Alarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_BIT(rtc->CALALR, DAYALEN_Pos);
}
/**
  * @brief  (CALALR) enable day alarm.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Day_Alarm_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->CALALR, DAYALEN_Pos);
}
/**
  * @brief  (CALALR) set day alarm value.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Day_Alarm(rtc_dev_t *rtc, uint8_t value)
{
	HW_SET_MSK_VAL(rtc->CALALR, DAYAL_Msk, DAYAL_Pos, value);
}
/**
  * @brief  (CALALR) set day alarm value.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Day_Alarm(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALALR, DAYAL_Msk, DAYAL_Pos);
}
/**
  * @brief	(CALALR) disable date alarm.
  * @param	rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Date_Alarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_BIT(rtc->CALALR, DATEALEN_Pos);
}
/**
  * @brief  (CALALR) enable date alarm.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Date_Alarm_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->CALALR, DATEALEN_Pos);
}
/**
  * @brief  (CALALR) set date alarm value.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Date_Alarm(rtc_dev_t *rtc, uint8_t value)
{
	HW_SET_MSK_VAL(rtc->CALALR, DATEAL_Msk, DATEAL_Pos, value);
}
/**
  * @brief  (CALALR) get date alarm value.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Date_Alarm(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALALR, DATEAL_Msk, DATEAL_Pos);
}
/**
  * @brief	(CALALR) disable month alarm.
  * @param	rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Month_Alarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_BIT(rtc->CALALR, MONALEN_Pos);
}
/**
  * @brief  (CALALR) enable month alarm.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Month_Alarm_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->CALALR, MONALEN_Pos);
}
/**
  * @brief  (TIMALR) set date alarm value.
  * @param  rtc is RTC Instance
  */
__HW_STATIC_INLINE
void LL_RTC_Set_Month_Alarm(rtc_dev_t *rtc, uint8_t value)
{
	HW_SET_MSK_VAL(rtc->CALALR, MONAL_Msk, MONAL_Pos, value);
}
/**
  * @brief  (TIMALR) get date alarm value.
  * @param  rtc is RTC Instance
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_Get_Month_Alarm(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->CALALR, MONAL_Msk, MONAL_Pos);
}
/**
  * @brief  (CALALR) disable calendar alarm.
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_CalAlarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_MSK(rtc->CALALR, ((1U << DAYALEN_Pos) | \
							(1U << DATEALEN_Pos) | \
							(1U << MONALEN_Pos)));
}
/**
  * @brief  (CALALR) enable calendar alarm enable.
  * @param1  rtc is RTC Instance
  * @param2  which alarm enbale
  *			@arg @ref LL_RTC_CALALR_ALARM_DAY_EN_MASK
  * 		@arg @ref LL_RTC_CALALR_ALARM_DATE_EN_MASK
  * 		@arg @ref LL_RTC_CALALR_ALARM_MONTH_EN_MASK
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_CalAlarm_Enable(rtc_dev_t *rtc, uint32_t calr_mask)
{
	HW_SET_MSK(rtc->CALALR, (calr_mask) & ((1U << DAYALEN_Pos) | \
										  (1U << DATEALEN_Pos) | \
										  (1U << MONALEN_Pos)));
}


/**
  * @brief  (CALALR and TIMALR) disable alarm
  * @param  rtc is RTC Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Alarm_Disable(rtc_dev_t *rtc)
{
	HW_CLR_MSK(rtc->TIMALR, ((1U << HOURALEN_Pos) | \
							 (1U << MINALEN_Pos) |  \
							(1U << SECALEN_Pos)));
	HW_CLR_MSK(rtc->CALALR, ((1U << DAYALEN_Pos) | \
							(1U << DATEALEN_Pos) | \
							(1U << MONALEN_Pos)));
}
/**
  * @brief  (CALALR and TIMALR) enable alarm.
  * @param1  rtc is RTC Instance
  * @param2  which time alarm enbale
  * 		@arg @ref LL_RTC_TIMALR_SEC_EN_MASK
  * 		@arg @ref LL_RTC_TIMALR_MIN_EN_MASK
  * 		@arg @ref LL_RTC_TIMALR_HOUR_EN_MASK
  * @param2  which calendar alarm enbale
  * 		@arg @ref LL_RTC_CALALR_DAY_EN_MASK
  * 		@arg @ref LL_RTC_CALALR_DATE_EN_MASK
  * 		@arg @ref LL_RTC_CALALR_MONTH_EN_MASK
  * @retval None
  */
__HW_STATIC_INLINE
void LL_RTC_Alarm_Enable(rtc_dev_t *rtc, uint32_t time_mask, uint32_t calr_mask)
{
	HW_SET_MSK(rtc->TIMALR, (time_mask) & ((1U << HOURALEN_Pos) | \
											(1U << MINALEN_Pos) | \
											(1U << SECALEN_Pos)));
	HW_SET_MSK(rtc->CALALR, (calr_mask) & ((1U << DAYALEN_Pos) | \
											(1U << DATEALEN_Pos) | \
											(1U << MONALEN_Pos)));
}




/**
  * @brief  (SR) rtc get all interrupt status.
  * @param1  rtc is RTC Instance
  * @retval all interrupt status.
  */
__HW_STATIC_INLINE
uint32_t LL_RTC_GetStatus(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->SR, ALLSTATE_Msk, ALLSTATE_Pos);
}
/**
* @brief  (SR) rtc get interrupt status.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_TimeDataErr(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->SR, TDERR_Pos);
}
/**
* @brief  (SR) rtc get interrupt status.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_CalendarEvent(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->SR, CALEV_Pos);
}
/**
* @brief  (SR) rtc get interrupt status.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_TimeEvent(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->SR, TIMEV_Pos);
}
/**
* @brief  (SR) rtc get interrupt status.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_SecondEvent(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->SR, SECEV_Pos);
}
/**
* @brief  (SR) rtc get interrupt status.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_AlarmIT(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->SR, ALFLAG_Pos);
}
/**
* @brief  (SR) rtc get interrupt status.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_UpdAckIT(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->SR, ACKUPD_Pos);
}



/**
 * @brief  (SCCR) rtc time or date error interrupt clear.
 * @param1	rtc is RTC Instance
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_TimeDateErr_Intr_Clear(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->SCCR, TDERRCLR_Pos);
}
 /**
 * @brief  (SCCR) rtc calendar interrupt clear.
 * @param1	rtc is RTC Instance
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_Calendar_Intr_Clear(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->SCCR, CALCLR_Pos);
}
/**
* @brief  (SCCR) rtc time interrupt clear.
* @param1  rtc is RTC Instance
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_Time_Intr_Clear(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->SCCR, TIMCLR_Pos);
}
/**
* @brief  (SCCR) rtc second interrupt clear.
* @param1  rtc is RTC Instance
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_Second_Intr_Clear(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->SCCR, SECCLR_Pos);
}
/**
* @brief  (SCCR) rtc alarm interrupt clear.
* @param1  rtc is RTC Instance
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_Alarm_Intr_Clear(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->SCCR, ALCLR_Pos);
}
/**
* @brief  (SCCR) rtc update ack interrput clear.
* @param1  rtc is RTC Instance
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_UpdAck_Intr_Clear(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->SCCR, ACKCLR_Pos);
}



/**
 * @brief  (IER) Enable All rtc interrupt generation.
 * @param1  rtc is RTC Instance
 * @Note  RTC interrupt enable all.
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_All_IntrEnable(rtc_dev_t *rtc)
{
	HW_SET_MSK_VAL(rtc->IER, ALLINEN_Msk, ALLINEN_Pos, LL_RTC_IER_ALL_INT_ENABLE);
}
/**
 * @brief  (IER) Enabled rtc time and date error interrupt generation.
 * @param1  rtc is RTC Instance
 * @Note    RTC interrupt enable all.
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_TimeDateErr_Intr_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IER, TDERREN_Pos);
}
/**
 * @brief  (IER) Enable rtc calendar interrupt generation.
 * @param1  rtc is RTC Instance
 * @Note    RTC interrupt enable all.
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_Calendar_Intr_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IER, CALEN_Pos);
}
/**
 * @brief  (IER) Enable rtc Time interrupt generation.
 * @param1  rtc is RTC Instance
 * @Note    RTC interrupt enable all.
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_Time_Intr_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IER, TIMEN_Pos);
}
/**
 * @brief  (IER) Enable rtc second interrupt generation.
 * @param1  rtc is RTC Instance
 * @Note    RTC interrupt enable all.
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_Second_Intr_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IER, SECEN_Pos);
}
/**
 * @brief  (IER) Enable rtc alarm interrupt generation.
 * @param1  rtc is RTC Instance
 * @Note    RTC interrupt enable all.
 * @retval  None
 */
__HW_STATIC_INLINE
void LL_RTC_Alarm_Intr_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IER, ALEN_Pos);
}
/**
* @brief  (IER) Enable rtc update acknowledge interrupt generation.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt enable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_UpdAck_Intr_Enable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IER, ACKEN_Pos);
}


/**
* @brief  (IDR) Enable All rtc interrupt generation.
* @param1  rtc is RTC Instance
* @Note  RTC interrupt disable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_All_IntrDisable(rtc_dev_t *rtc)
{
	HW_SET_MSK_VAL(rtc->IDR, ALLINDIS_Msk, ALLINDIS_Pos, LL_RTC_IER_ALL_INT_ENABLE);
}
/**
* @brief  (IDR) Disabled rtc time and date error interrupt gdiseration.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt disable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_TimeDateErr_Intr_Disable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IDR, TDERRDIS_Pos);
}
/**
* @brief  (IDR) Disable rtc caldisdar interrupt gdiseration.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt disable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_Calendar_Intr_Disable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IDR, CALDIS_Pos);
}
/**
* @brief  (IDR) Disable rtc Timee interrupt gdiseration.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt disable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_Time_Intr_Disable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IDR, TIMDIS_Pos);
}
/**
* @brief  (IDR) Disable rtc second interrupt gdiseration.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt disable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_Second_Intr_Disable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IDR, SECDIS_Pos);
}
/**
* @brief  (IDR) Disable rtc alarm interrupt gdiseration.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt disable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_Alarm_Intr_Disable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IDR, ALDIS_Pos);
}
/**
* @brief  (IDR) Disable rtc update acknowledge interrupt gdiseration.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt disable all.
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_UpdAck_Intr_Disable(rtc_dev_t *rtc)
{
	HW_SET_BIT(rtc->IDR, ACKDIS_Pos);
}



/**
* @brief  (IMR) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt disable all.
* @retval  All interrupt mask
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_GetITMsk(rtc_dev_t *rtc)
{
	return HW_GET_MSK_VAL(rtc->IMR, ALLINMSK_Msk, ALLINMSK_Pos);
}
/**
* @brief  (IMR) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_TimeDataErrITEn(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->IMR, TDERRCLR_Pos);
}
/**
* @brief  (IMR) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_CalendarEventITEn(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->IMR, CALCLR_Pos);
}
/**
* @brief  (IMR) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_TimeEventITEn(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->IMR, TIMMK_Pos);
}
/**
* @brief  (IMR) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_SecondEventITEn(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->IMR, SECMK_Pos);
}
/**
* @brief  (IMR) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_AlarmITEn(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->IMR, ALRMK_Pos);
}
/**
* @brief  (IMR) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_UpdAckITEn(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->IMR, ACKMK_Pos);
}



/**
* @brief  (VER) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_VaildCalendarAlarm(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->VER, NVCALALR_Pos);
}
/**
* @brief  (VER) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_VaildTimeAlarm(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->VER, NVTIMALR_Pos);
}
/**
* @brief  (VER) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_VaildCalendar(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->VER, NVCAL_Pos);
}
/**
* @brief  (VER) rtc get interrupt mask.
* @param1  rtc is RTC Instance
* @Note    RTC interrupt .
* @retval
*/
__HW_STATIC_INLINE
uint32_t LL_RTC_Is_VaildTime(rtc_dev_t *rtc)
{
	return HW_TEST_BIT(rtc->VER, NVTIM_Pos);
}


/**
* @brief  (WPR) rtc disable write protection.
* @param1  rtc is RTC Instance
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_WRPMR_Disable(rtc_dev_t *rtc)
{
	HW_SET_MSK_VAL(rtc->WPMR, WPKEY_Msk, WPKEY_Pos, LL_RTC_WPMR_WPKEY_DISABLE);
}
/**
* @brief  (WPR) rtc enable write protection.
* @param1  rtc is RTC Instance
* @retval  None
*/
__HW_STATIC_INLINE
void LL_RTC_WRPMR_Enable(rtc_dev_t *rtc)
{
	HW_SET_MSK_VAL(rtc->WPMR, WPKEY_Msk, WPKEY_Pos, LL_RTC_WPMR_WPKEY_ENABLE);
}
/**
  * @}
  */
/** @defgroup RTC Function
  * @}
  */


/**
  * @}  psic_ll_rtc
  */
/**
  * @}  PSIC_LL_Driver
  */
#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_RTC_H */

/**
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
