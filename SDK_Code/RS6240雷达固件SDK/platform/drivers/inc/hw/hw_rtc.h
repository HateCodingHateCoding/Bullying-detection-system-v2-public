
/****************************************************************
* @file  hw_rtc.h
* @brief rtc hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
****************************************************************/
#ifndef _HW_RTC_H_
#define _HW_RTC_H_


#ifdef __cplusplus
  extern "C" {
#endif

/**
 * @addtopgroup HW_RTC
 * @{
 **/


/* Type definition for device register.
 * ----------------------------------------------------------------------
 */
typedef struct {
	__IOM uint32_t CR;                      /*!< 0x0060,  Control register */
	__IOM uint32_t MR;                      /*!< 0x0064,  Mode register */
	__IOM uint32_t TIMR;                    /*!< 0x0068,  Timer register */
	__IOM uint32_t CALR;                    /*!< 0x006C,  Calendar Register */
	__IOM uint32_t TIMALR;                  /*!< 0x0070,  Time Alarm Register */
	__IOM uint32_t CALALR;                  /*!< 0x0074,  Calendar Alarm Register */
	__IM  uint32_t SR;                      /*!< 0x0078,  Status Register */
	__OM  uint32_t SCCR;                    /*!< 0x007C,  Status Clear Command Register */
	__OM  uint32_t IER;                     /*!< 0x0080,  Interrupt Enable Register */
	__OM  uint32_t IDR;                     /*!< 0x0084,  Interrupt Disable Register */
	__IM  uint32_t IMR;                     /*!< 0x0088,  Interrupt Mask Register */
	__IM  uint32_t VER;                     /*!< 0x008C,  Valid Entry Register */
		  uint32_t GPBR;                    /*!< 0X0090,  Not configure register*/
		  uint32_t RSVD0[4];                /*!< 0x0094,  reserve address*/
	__IM  uint32_t TSTR0;                   /*!< 0X0110,  TimeStamp Time Register 0 */
	__IM  uint32_t TSDR0;                   /*!< 0X0114,  TimeStamp Date Register 0 */
	__IM  uint32_t TSSR0;                   /*!< 0X0118,  TimeStamp Source Register 0 */
	__IM  uint32_t TSTR1;                   /*!< 0x0120,  TimeStamp Time Register 0 */
	__IM  uint32_t TSDR1;                   /*!< 0X0124,  TimeStamp Date Register 0 */
	__IM  uint32_t TSSR1;                   /*!< 0X0128,  TimeStamp Source Register 0 */
		  uint32_t RSVD1[7];                /*!< 0x0128,  reserve address*/
	__IOM uint32_t WPMR;                    /*!< 0X0144,  Write Protection Mode Register */
} rtc_dev_t;

#define RTC_DEV                         ((rtc_dev_t *)    (RTC_BASE_ADDR + 0x60)) /*!< 0x4001 8060 */
// #define RTTRTC_DEV                      ((rtt_dev_t *)  (RTC_BASE_ADDR + 0x30)) /*!< 0x4001 8030 */

/*!< 0x0060 CR
* CALEV in RTC_SR depend on the value of CALEVSEL
*/
#define CALEVSEL_Msk                                      (0x03 << CALEVSEL_Pos)
#define CALEVSEL_Pos                                      16
#define CALEVSEL_PosL                                     16
#define CALEVSEL_PosH                                     17

/*!< 0x0060 CR
* CALEV in RTC_SR depend on the value of CALEVSEL
*/
#define TIMEVSEL_Msk                                      (0x03 << TIMEVSEL_Pos)
#define TIMEVSEL_Pos                                      8
#define TIMEVSEL_PosL                                     8
#define TIMEVSEL_PosH                                     9

/*!< 0x0060 CR
* CALEV in RTC_SR depend on the value of CALEVSEL
*/
#define UPDCAL_Msk                                        (0x01 << UPDCAL_Pos)
#define UPDCAL_Pos                                        1
#define UPDCAL_PosL                                       1
#define UPDCAL_PosH                                       1

/*!< 0x0060 CR
* CALEV in RTC_SR depend on the value of CALEVSEL
*/
#define UPDTIM_Msk                                        (0x01 << UPDTIM_Pos)
#define UPDTIM_Pos                                        0
#define UPDTIM_PosL                                       0
#define UPDTIM_PosH                                       0

/*!< 0x0060 CR
 * upgrade requset
*/
#define UPDRQE_Msk                                        (0x03 << UPDRQE_Pos)
#define UPDRQE_Pos                                        0
#define UPDRQE_PosL                                       0
#define UPDRQE_PosH                                       1



/*!< 0x0064 MR
* Period of the output pulse(脉冲)
*/
#define TPERIOD_Msk                                      (0x03 << TPERIOD_Pos)
#define TPERIOD_Pos                                      28
#define TPERIOD_PosL                                     28
#define TPERIOD_PosH                                     29

/*!< 0x0064 MR
* High duration of output pulse (持续)
*/
#define THIGH_Msk                                        (0x07 << THIGH_Pos)
#define THIGH_Pos                                        24
#define THIGH_PosL                                       24
#define THIGH_PosH                                       26

/*!< 0x0064 MR
* RTCOUT0 output source select
*/
#define OUT0_Msk                                         (0x07 << OUT0_Pos)
#define OUT0_Pos                                         16
#define OUT0_PosL                                        16
#define OUT0_PosH                                        18

/*!< 0x0064 MR
* High PMM correction
*/
#define HIGPPM_Msk                                       (0x01 << HIGPPM_Pos)
#define HIGPPM_Pos                                       15
#define HIGPPM_PosL                                      15
#define HIGPPM_PosH                                      15

/*!< 0x0064 MR
* Slow clock correction
*/
#define CORRECTION_Msk                                  (0x7F << CORRECTION_Pos)
#define CORRECTION_Pos                                  8
#define CORRECTION_PosL                                 8
#define CORRECTION_PosH                                 14

/*!< 0x0064 MR
* Negative PPM correction
*/
#define NEGPPM_Msk                                      (0x01 << NEGPPM_Pos)
#define NEGPPM_Pos                                      4
#define NEGPPM_PosL                                     4
#define NEGPPM_PosH                                     4

/*!< 0x0064 MR
* Persian Calendar
*/
#define CALRMOD_Msk                                     (0x01 << CALRMOD_Pos)
#define CALRMOD_Pos                                     1
#define CALRMOD_PosL                                    1
#define CALRMOD_PosH                                    1

/*!< 0x0064 MR
* 12/24 hour mode
*/
#define HRMOD_Msk                                       (0x01 << HRMOD_Pos)
#define HRMOD_Pos                                       0
#define HRMOD_PosL                                      0
#define HRMOD_PosH                                      0




/*!< 0x0068 TIM
* select AM/PM
*/
#define AMPM_Msk                                        (0x01 << AMPM_Pos)
#define AMPM_Pos                                        22
#define AMPM_PosL                                       22
#define AMPM_PosH                                       22

/*!< 0x0068 TIM
* currect hour
*/
#define HOUR_Msk                                        (0x3F << HOUR_Pos)
#define HOUR_Pos                                        16
#define HOUR_PosL                                       16
#define HOUR_PosH                                       21

/*!< 0x0068 TIM
* currect minute
*/
#define MIN_Msk                                        (0x7F << MIN_Pos)
#define MIN_Pos                                        8
#define MIN_PosL                                       8
#define MIN_PosH                                       14

/*!< 0x0068 TIM
* currect second
*/
#define SEC_Msk                                        (0x7F << SEC_Pos)
#define SEC_Pos                                        0
#define SEC_PosL                                       0
#define SEC_PosH                                       6




/*!< 0x006C CAL
* currect data in month 1 ~ 31
*/
#define DATE_Msk                                      (0x3F << DATE_Pos)
#define DATE_Pos                                      24
#define DATE_PosL                                     24
#define DATE_PosH                                     29

/*!< 0x006C CAL
* currect day in week 1 ~ 7
*/
#define DAY_Msk                                       (0x07 << DAY_Pos)
#define DAY_Pos                                       21
#define DAY_PosL                                      21
#define DAY_PosH                                      23

/*!< 0x006C CAL
* currect month  1 ~ 12
*/
#define MONTH_Msk                                     (0x1F << MONTH_Pos)
#define MONTH_Pos                                     16
#define MONTH_PosL                                    16
#define MONTH_PosH                                    20

/*!< 0x006C CAL
* currect year  00 ~ 99
*/
#define YEAR_Msk                                      (0xFF << YEAR_Pos)
#define YEAR_Pos                                      8
#define YEAR_PosL                                     8
#define YEAR_PosH                                     15

/*!< 0x006C CAL
* currect century 19-20(Greg)/13-14(Pers)
*/
#define CENT_Msk                                      (0x7F << CENT_Pos)
#define CENT_Pos                                      0
#define CENT_PosL                                     0
#define CENT_PosH                                     6




/*!< 0x0070 TIMALR
* hour alarm enable
*/
#define HOURALEN_Msk                                        (0x01 << HOURALEN_Pos)
#define HOURALEN_Pos                                        23
#define HOURALEN_PosL                                       23
#define HOURALEN_PosH                                       23

/*!< 0x0070 TIMALR
*  AM/PM Indicator
*/
#define AMPMIND_Msk                                       (0x01 << AMPMIND_Pos)
#define AMPMIND_Pos                                       22
#define AMPMIND_PosL                                      22
#define AMPMIND_PosH                                      22

/*!< 0x0070 TIMALR
* Hour alarm set
*/
#define HOURAL_Msk                                        (0x3F << HOURAL_Pos)
#define HOURAL_Pos                                        16
#define HOURAL_PosL                                       16
#define HOURAL_PosH                                       21

/*!< 0x0070 TIMALR
* Minute Alarm Enable
*/
#define MINALEN_Msk                                         (0x01 << MINALEN_Pos)
#define MINALEN_Pos                                         15
#define MINALEN_PosL                                        15
#define MINALEN_PosH                                        15

/*!< 0x0070 TIMALR
* Minute Alarm
*/
#define MINAL_Msk                                          (0x7F << MINAL_Pos)
#define MINAL_Pos                                          8
#define MINAL_PosL                                         8
#define MINAL_PosH                                         14

/*!< 0x0070 TIMALR
* second alarm enable
*/
#define SECALEN_Msk                                        (0x01 << SECALEN_Pos)
#define SECALEN_Pos                                        7
#define SECALEN_PosL                                       7
#define SECALEN_PosH                                       7

/*!< 0x0070 TIMALR
* second alarm set
*/
#define SECAL_Msk                                         (0x7F << SECAL_Pos)
#define SECAL_Pos                                         0
#define SECAL_PosL                                        0
#define SECAL_PosH                                        6




/*!< 0x0074 CALAL
* data alarm enable
*/
#define DATEALEN_Msk                                       (0x01 << DATEALEN_Pos)
#define DATEALEN_Pos                                       31
#define DATEALEN_PosL                                      31
#define DATEALEN_PosH                                      31

/*!< 0x0074 CALAL
* data alarm set
*/
#define DATEAL_Msk                                        (0x3F << DATEAL_Pos)
#define DATEAL_Pos                                        24
#define DATEAL_PosL                                       24
#define DATEAL_PosH                                       29

/*!< 0x0074 CALAL
* month alarm enable
*/
#define MONALEN_Msk                                        (0x01 << MONALEN_Pos)
#define MONALEN_Pos                                        23
#define MONALEN_PosL                                       23
#define MONALEN_PosH                                       23

/*!< 0x0074 CALAL
* month alarm set
*/
#define MONAL_Msk                                          (0x1F << MONAL_Pos)
#define MONAL_Pos                                          16
#define MONAL_PosL                                         16
#define MONAL_PosH                                         20

/*!< 0x0074 CALAL
* day alarm enable
*/
#define DAYALEN_Msk                                       (0x01 << DAYALEN_Pos)
#define DAYALEN_Pos                                       15
#define DAYALEN_PosL                                      15
#define DAYALEN_PosH                                      15

/*!< 0x0074 CALAL
* day alarm set
*/
#define DAYAL_Msk                                         (0x07 << DAYAL_Pos)
#define DAYAL_Pos                                         8
#define DAYAL_PosL                                        8
#define DAYAL_PosH                                        10



/*!< 0x0078 SR
* timer or data running error 0 correct
*/
#define TDERR_Msk                                       (0x01 << TDERR_Pos)
#define TDERR_Pos                                       5
#define TDERR_PosL                                      5
#define TDERR_PosH                                      5

/*!< 0x0078 SR
* Calendar event
*/
#define CALEV_Msk                                       (0x01 << CALEV_Pos)
#define CALEV_Pos                                       4
#define CALEV_PosL                                      4
#define CALEV_PosH                                      4

/*!< 0x0078 SR
* Timer event
*/
#define TIMEV_Msk                                       (0x01 << TIMEV_Pos)
#define TIMEV_Pos                                       3
#define TIMEV_PosL                                      3
#define TIMEV_PosH                                      3

/*!< 0x0078 SR
* Second event
*/
#define SECEV_Msk                                       (0x01 << SECEV_Pos)
#define SECEV_Pos                                       2
#define SECEV_PosL                                      2
#define SECEV_PosH                                      2

/*!< 0x0078 SR
* Alarm flag
*/
#define ALFLAG_Msk                                      (0x01 << ALFLAG_Pos)
#define ALFLAG_Pos                                      1
#define ALFLAG_PosL                                     1
#define ALFLAG_PosH                                     1

/*!< 0x0078 SR
* Acknowledge for updata
*/
#define ACKUPD_Msk                                       (0x01 << ACKUPD_Pos)
#define ACKUPD_Pos                                       0
#define ACKUPD_PosL                                      0
#define ACKUPD_PosH                                      0

/*!< 0x0078 SR
* All interrupt status
*/
#define ALLSTATE_Msk                                     (0x3F << ALLSTATE_Pos)
#define ALLSTATE_Pos                                     0
//#define ALLSTATE_PosL                                    0
//#define ALLSTATE_PosH                                    0







/*!< 0x007C SCC
* Time and/or Date Free Running Error Clear
*/
#define TDERRCLR_Msk                                      (0x01 << TDERRCLR_Pos)
#define TDERRCLR_Pos                                      5
#define TDERRCLR_PosL                                     5
#define TDERRCLR_PosH                                     5

/*!< 0x007C SCC
* Calendar Clear
*/
#define CALCLR_Msk                                      (0x01 << CALCLR_Pos)
#define CALCLR_Pos                                      4
#define CALCLR_PosL                                     4
#define CALCLR_PosH                                     4

/*!< 0x007C SCC
* Time Clear
*/
#define TIMCLR_Msk                                      (0x01 << TIMCLR_Pos)
#define TIMCLR_Pos                                      3
#define TIMCLR_PosL                                     3
#define TIMCLR_PosH                                     3

/*!< 0x007C SCC
* Second Clear
*/
#define SECCLR_Msk                                      (0x01 << SECCLR_Pos)
#define SECCLR_Pos                                      2
#define SECCLR_PosL                                     2
#define SECCLR_PosH                                     2

/*!< 0x007C SCC
* Alarm Clear
*/
#define ALCLR_Msk                                      (0x01 << ALCLR_Pos)
#define ALCLR_Pos                                      1
#define ALCLR_PosL                                     1
#define ALCLR_PosH                                     1

/*!< 0x007C SCC
* Acknowledge Clear
*/
#define ACKCLR_Msk                                      (0x01 << ACKCLR_Pos)
#define ACKCLR_Pos                                      0
#define ACKCLR_PosL                                     0
#define ACKCLR_PosH                                     0



/*!< 0x0080 IER
* Time and/or Date Error Interrupt Enable
*/
#define TDERREN_Msk                                      (0x01 << TDERREN_Pos)
#define TDERREN_Pos                                      5
#define TDERREN_PosL                                     5
#define TDERREN_PosH                                     5

/*!< 0x0080 IER
* Calendar Event Interrupt Enable
*/
#define CALEN_Msk                                        (0x01 << CALEN_Pos)
#define CALEN_Pos                                        4
#define CALEN_PosL                                       4
#define CALEN_PosH                                       4
/*!< 0x0080 IER
* Time Event Interrupt Enable
*/
#define TIMEN_Msk                                        (0x01 << TIMEN_Pos)
#define TIMEN_Pos                                        3
#define TIMEN_PosL                                       3
#define TIMEN_PosH                                       3
/*!< 0x0080 IER
* Second Event Interrupt Enable
*/
#define SECEN_Msk                                       (0x01 << SECEN_Pos)
#define SECEN_Pos                                       2
#define SECEN_PosL                                      2
#define SECEN_PosH                                      2
/*!< 0x0080 IER
* Alarm Interrupt Enable
*/
#define ALEN_Msk                                       (0x01 << ALEN_Pos)
#define ALEN_Pos                                       1
#define ALEN_PosL                                      1
#define ALEN_PosH                                      1
/*!< 0x0080 IER
* Acknowledge Update Interrupt Enable
*/
#define ACKEN_Msk                                       (0x01 << ACKEN_Pos)
#define ACKEN_Pos                                       0
#define ACKEN_PosL                                      0
#define ACKEN_PosH                                      0
/*!< 0x0080 IER
* All Interrupt Enable
*/
#define ALLINEN_Msk                                     (0x3F << ALLINEN_Pos)
#define ALLINEN_Pos                                     0
//#define ALLINEN_PosL                                    0
//#define ALLINEN_PosH                                    0




/*!< 0x0084 IDR
* Time and/or Date Error Interrupt Disable
*/
#define TDERRDIS_Msk                                      (0x01 << TDERRDIS_Pos)
#define TDERRDIS_Pos                                      5
#define TDERRDIS_PosL                                     5
#define TDERRDIS_PosH                                     5
/*!< 0x0084 IDR
* Calendar Event Interrupt Disable
*/
#define CALDIS_Msk                                       (0x01 << CALDIS_Pos)
#define CALDIS_Pos                                       4
#define CALDIS_PosL                                      4
#define CALDIS_PosH                                      4
/*!< 0x0084 IDR
* Time Event Interrupt Disable
*/
#define TIMDIS_Msk                                       (0x01 << TIMDIS_Pos)
#define TIMDIS_Pos                                       3
#define TIMDIS_PosL                                      3
#define TIMDIS_PosH                                      3
/*!< 0x0084 IDR
* Second Event Interrupt Disable
*/
#define SECDIS_Msk                                       (0x01 << SECDIS_Pos)
#define SECDIS_Pos                                       2
#define SECDIS_PosL                                      2
#define SECDIS_PosH                                      2
/*!< 0x0084 IDR
* Alarm Interrupt Disable
*/
#define ALDIS_Msk                                       (0x01 << ALDIS_Pos)
#define ALDIS_Pos                                       1
#define ALDIS_PosL                                      1
#define ALDIS_PosH                                      1
/*!< 0x0084 IDR
* Acknowledge Update Interrupt Disable
*/
#define ACKDIS_Msk                                       (0x01 << ACKDIS_Pos)
#define ACKDIS_Pos                                       0
#define ACKDIS_PosL                                      0
#define ACKDIS_PosH                                      0
/*!< 0x0084 IDR
* Entire Interrupt Mask
*/
#define ALLINDIS_Msk                                     (0x3F << ALLINDIS_Pos)
#define ALLINDIS_Pos                                     0
//#define ALLINDIS_PosL                                    0
//#define ALLINDIS_PosH                                    0




/*!< 0x0088 IMR
* Time and/or Date Error Mask
*/
#define TDERRMK_Msk                                      (0x01 << TDERRMK_Pos)
#define TDERRMK_Pos                                      5
#define TDERRMK_PosL                                     5
#define TDERRMK_PosH                                     5
/*!< 0x0088 IMR
*  Calendar Event Interrupt Mask
*/
#define CALMK_Msk                                        (0x01 << CALMK_Pos)
#define CALMK_Pos                                        4
#define CALMK_PosL                                       4
#define CALMK_PosH                                       4
/*!< 0x0088 IMR
* Time Event Interrupt Mask
*/
#define TIMMK_Msk                                        (0x01 << TIMMK_Pos)
#define TIMMK_Pos                                        3
#define TIMMK_PosL                                       3
#define TIMMK_PosH                                       3
/*!< 0x0088 IMR
* Second Event Interrupt Mask
*/
#define SECMK_Msk                                        (0x01 << SECMK_Pos)
#define SECMK_Pos                                        2
#define SECMK_PosL                                       2
#define SECMK_PosH                                       2
/*!< 0x0088 IMR
* Alarm Interrupt Mask
*/
#define ALRMK_Msk                                        (0x01 << ALRMK_Pos)
#define ALRMK_Pos                                        1
#define ALRMK_PosL                                       1
#define ALRMK_PosH                                       1
/*!< 0x0088 IMR
* Acknowledge Update Interrupt Mask
*/
#define ACKMK_Msk                                        (0x01 << ACKMK_Pos)
#define ACKMK_Pos                                        0
#define ACKMK_PosL                                       0
#define ACKMK_PosH                                       0
/*!< 0x0084 IDR
* Entire Interrupt Mask
*/
#define ALLINMSK_Msk                                     (0x3F << ALLINMSK_Pos)
#define ALLINMSK_Pos                                     0
//#define ALLINDIS_PosL                                    0
//#define ALLINDIS_PosH                                    0




/*!< 0x008C VER
* Non-valid Calendar Alarm
*/
//#define NVCALALR_Msk                                      (0x01 << NVCALALR_Pos)
#define NVCALALR_Pos                                      3
#define NVCALALR_PosL                                     3
#define NVCALALR_PosH                                     3

/*!< 0x008C VER
* Non-valid Time Alarm
*/
//#define NVTIMALR_Msk                                      (0x01 << NVTIMALR_Pos)
#define NVTIMALR_Pos                                      2
#define NVTIMALR_PosL                                     2
#define NVTIMALR_PosH                                     2

/*!< 0x008C VER
* Non-valid Calendar
*/
//#define NVCAL_Msk                                         (0x01 << NVCAL_Pos)
#define NVCAL_Pos                                         1
#define NVCAL_PosL                                        1
#define NVCAL_PosH                                        1

/*!< 0x008C VER
* Non-valid Time
*/
//#define NVTIM_Msk                                         (0x01 << NVTIM_Pos)
#define NVTIM_Pos                                         0
#define NVTIM_PosL                                        0
#define NVTIM_PosH                                        0




/*!< 0x0110 TSTR0
* RTC_TSTR0 reports the timestamp of the first tamper event after reading RTC_TSSR0. This register is cleared by reading RTC_TSSR0.
** System Mode of the Tamper **
*/
/*!< 0x011C TSTR1
 * System Mode of the Tamper
 */
#define TPBACKUP_Msk                                       (0x01 << TPBACKUP_Pos)
#define TPBACKUP_Pos                                       31
#define TPBACKUP_PosL                                      31
#define TPBACKUP_PosH                                      31

/*!< 0x0110 TSTR0
* Tamper Events Counter
*/
/*!< 0x0120 TSTR1
* Tamper Events Counter
*/
#define TPTEVCNT_Msk                                       (0x0F << TPTEVCNT_Pos)
#define TPTEVCNT_Pos                                       24
#define TPTEVCNT_PosL                                      24
#define TPTEVCNT_PosH                                      27

/*!< 0x0110 TSTR0
 * AM/PM Indicator of the Tamper
*/
/*!< 0x0110 TSTR1
 * AM/PM Indicator of the Tamper
*/
#define TPAMPM_Msk                                        (0x01 << TPAMPM_Pos)
#define TPAMPM_Pos                                        22
#define TPAMPM_PosL                                       22
#define TPAMPM_PosH                                       22

/*!< 0x0110 TSTR0
 * Hours of the Tamper
*/
/*!< 0x0120 TSTR1
 * Hours of the Tamper
*/
#define TPHOUR_Msk                                        (0x3F << TPHOUR_Pos)
#define TPHOUR_Pos                                        16
#define TPHOUR_PosL                                       16
#define TPHOUR_PosH                                       21

/*!< 0x0110 TSTR0
* Minutes of the Tamper
*/
/*!< 0x0120 TSTR1
* Minutes of the Tamper
*/
#define TPMIN_Msk                                         (0x7F << MIN0_Pos)
#define TPMIN_Pos                                         8
#define TPMIN_PosL                                        8
#define TPMIN_PosH                                        14

/*!< 0x0110 TSTR0
* Seconds of the Tamper
*/
/*!< 0x0120 TSTR1
* Seconds of the Tamper
*/
#define TPSEC_Msk                                         (0x7F << TPSEC_Pos)
#define TPSEC_Pos                                         0
#define TPSEC_PosL                                        0
#define TPSEC_PosH                                        6




/*!< 0x0114 TSDR0
* Date of the Tamper
*/
/*!< 0x0120 TSDR1
* Date of the Tamper
*/
#define TPDATE_Msk                                        (0x3F << TPDATE_Pos)
#define TPDATE_Pos                                        24
#define TPDATE_PosL                                       24
#define TPDATE_PosH                                       29

/*!< 0x0114 TSDR0
* Day of the Tamper
*/
/*!< 0x0120 TSDR1
* Day of the Tamper
*/
#define TPDAY_Msk                                        (0x07 << TPDAY_Pos)
#define TPDAY_Pos                                        21
#define TPDAY_PosL                                       21
#define TPDAY_PosH                                       23

/*!< 0x0114 TSDR0
* Month of the Tamper
*/
/*!< 0x0120 TSDR1
* Month of the Tamper
*/
#define TPMONTH_Msk                                      (0x1F << TPMONTH_Pos)
#define TPMONTH_Pos                                      16
#define TPMONTH_PosL                                     16
#define TPMONTH_PosH                                     20

/*!< 0x0114 TSDR0
* Year of the Tamper
*/
/*!< 0x0120 TSDR1
* Year of the Tamper
*/
#define TPYEAR_Msk                                        (0xFF << TPYEAR_Pos)
#define TPYEAR_Pos                                        8
#define TPYEAR_PosL                                       8
#define TPYEAR_PosH                                       15

/*!< 0x0114 TSDR0
* Century of the Tamper
*/
/*!< 0x0120 TSDR1
* Century of the Tamper
*/
#define TPCENT_Msk                                        (0x7F << TPCENT_Pos)
#define TPCENT_Pos                                        0
#define TPCENT_PosL                                       0
#define TPCENT_PosH                                       6




/*!< 0x0118 TSSR0
* Tamper Source
*/
/*!< 0x0124 TSSR1
* Tamper Source
*/
#define TSRC_Msk                                          (0x03 << TSRC_Pos)
#define TSRC_Pos                                          0
#define TSRC_PosL                                         0
#define TSRC_PosH                                         1



/*!< 0x0144 WPMR
* Write Protection Key(0x525443)
*/
#define WPKEY_Msk                                        (0xFFFFFF << WPKEY_Pos)
#define WPKEY_Pos                                        8
#define WPKEY_PosL                                       8
#define WPKEY_PosH                                       31

/*!< 0x0068 TIM
* Write Protection Enable
*/
#define WPEN_Msk                                         (0x01 << WPEN_Pos)
#define WPEN_Pos                                         0
#define WPEN_PosL                                        0
#define WPEN_PosH                                        0


/**
* @}
* End of group HW_RTC
 */

#ifdef __cplusplus
}
#endif

#endif /* _HW_RTC_H_ */
