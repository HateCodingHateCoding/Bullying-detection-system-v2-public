/****************************************************************
* @file  hw_misc.h
* @brief hw_misc hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_MISC_H_
#define _HW_MISC_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_MISC
 * @{
 * */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */

typedef struct {
    __IOM uint32_t MISCREG_M50_POWER_CTRL;                  /*0x0000,             */
    __IOM uint32_t MISCREG_RTC_CLK_CNT;                     /*0x0004,             */
    __IOM uint32_t RTTRTC_CTRL;                             /*0x0008,             */
    __IOM uint32_t M50_RTC_CTRL;                            /*0x000C,             */
    __IOM uint32_t GPADC_WUP_TMR_CTRL;                      /*0x0010,             */
          uint32_t RSVD[58];                                /*0x0014-0x00F8,      */
    __IOM uint32_t WIO_GPADC_MODE;                          /*0x00FC,             */
    __IOM uint32_t WIO_MODE_SEL[2];                         /*0x0100-0x00104,     */
    __IOM uint32_t WIO_HOLD_OUTPUT;                         /*0x0108,             */
    __IOM uint32_t WIO_HOLD_PULLUPDOWN_SET[2];              /*0x010C-0x0110,      */
    __IOM uint32_t WIO_WUP_TYPE[4];                         /*0x0114-0x0120,      */
    __OM  uint32_t WIO_WUP_CLR;                             /*0x0124,             */
    __IOM uint32_t WIO_WUP_FLAG;                            /*0x0128,             */
    __IOM uint32_t WIO_WUP_DBC_SET;                         /*0x012c,             */
} misc_dev_t;

#define MISCREG1_DEV                      ((misc_dev_t *)   RTCSW_MISC_BASE_ADDR)/*!< 0x4001 8400 */

/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */
/*!< 0x0000 MISCREG_M50_POWER_CTRL
*null
*/
#define M50_SW_UP_FLAG_Msk                                  (0x01 << M50_SW_UP_FLAG_Pos)
#define M50_SW_UP_FLAG_Pos                                  24
#define M50_SW_UP_FLAG_PosL                                 24
#define M50_SW_UP_FLAG_PosH                                 24

/*!< 0x0000 MISCREG_M50_POWER_CTRL
*null
*/
#define M50_SRAM_RETN_MODE_Msk                              (0x01 << M50_SRAM_RETN_MODE_Pos)
#define M50_SRAM_RETN_MODE_Pos                              8
#define M50_SRAM_RETN_MODE_PosL                             8
#define M50_SRAM_RETN_MODE_PosH                             8

/*!< 0x0000 MISCREG_M50_POWER_CTRL
*null
*/
#define M50_SRAM_RELEASE_RET_Msk                            (0x01 << M50_SRAM_RELEASE_RET_Pos)
#define M50_SRAM_RELEASE_RET_Pos                            7
#define M50_SRAM_RELEASE_RET_PosL                           7
#define M50_SRAM_RELEASE_RET_PosH                           7

/*!< 0x0000 MISCREG_M50_POWER_CTRL
*null
*/
#define M50_SW_UP_SETTINGS_Msk                              (0x3F << M50_SW_UP_SETTINGS_Pos)
#define M50_SW_UP_SETTINGS_Pos                              1
#define M50_SW_UP_SETTINGS_PosL                             1
#define M50_SW_UP_SETTINGS_PosH                             6

/*!< 0x0000 MISCREG_M50_POWER_CTRL
*null
*/
#define M50_SW_EN_Msk                                       (0x01 << M50_SW_EN_Pos)
#define M50_SW_EN_Pos                                       0
#define M50_SW_EN_PosL                                      0
#define M50_SW_EN_PosH                                      0


/*!< 0x0004 MISCREG_RTC_CLK_CNT
*null
*/
#define RTC_CLK_CNT_Msk                                     (0xFFFF << RTC_CLK_CNT_Pos)
#define RTC_CLK_CNT_Pos                                     0
#define RTC_CLK_CNT_PosL                                    0
#define RTC_CLK_CNT_PosH                                    15


/*!< 0x0008 RTTRTC_CTRL
*null
*/
#define RTTRTC_CLK_SRC_SEL_Msk                              (0x03 << RTTRTC_CLK_SRC_SEL_Pos)
#define RTTRTC_CLK_SRC_SEL_Pos                              4
#define RTTRTC_CLK_SRC_SEL_PosL                             4
#define RTTRTC_CLK_SRC_SEL_PosH                             5

/*!< 0x0008 RTTRTC_CTRL
*null
*/
#define RTTRTC_CLK_EN_Msk                                   (0x01 << RTTRTC_CLK_EN_Pos)
#define RTTRTC_CLK_EN_Pos                                   1
#define RTTRTC_CLK_EN_PosL                                  1
#define RTTRTC_CLK_EN_PosH                                  1

/*!< 0x0008 RTTRTC_CTRL
*null
*/
#define RTTRTC_RESETN_EN_Msk                                (0x01 << RTTRTC_RESETN_EN_Pos)
#define RTTRTC_RESETN_EN_Pos                                0
#define RTTRTC_RESETN_EN_PosL                               0
#define RTTRTC_RESETN_EN_PosH                               0


/*!< 0x000c M50_RTC_CTRL
*null
*/
#define M50_RTC_CLK_SRC_SEL_Msk                             (0x03 << M50_RTC_CLK_SRC_SEL_Pos)
#define M50_RTC_CLK_SRC_SEL_Pos                             4
#define M50_RTC_CLK_SRC_SEL_PosL                            4
#define M50_RTC_CLK_SRC_SEL_PosH                            5

/*!< 0x000c M50_RTC_CTRL
*null
*/
#define M50_RTC_CLK_EN_Msk                                  (0x01 << M50_RTC_CLK_EN_Pos)
#define M50_RTC_CLK_EN_Pos                                  1
#define M50_RTC_CLK_EN_PosL                                 1
#define M50_RTC_CLK_EN_PosH                                 1

/*!< 0x000c M50_RTC_CTRL
*null
*/
#define M50_RTC_RESETN_EN_Msk                               (0x01 << M50_RTC_RESETN_EN_Pos)
#define M50_RTC_RESETN_EN_Pos                               0
#define M50_RTC_RESETN_EN_PosL                              0
#define M50_RTC_RESETN_EN_PosH                              0


/*!< 0x0010 GPADC_WUP_TMR_CTRL
*null
*/
#define GPADC_WUP_TMR_PERIOD_Msk                            (0xFFFF << GPADC_WUP_TMR_PERIOD_Pos)
#define GPADC_WUP_TMR_PERIOD_Pos                            16
#define GPADC_WUP_TMR_PERIOD_PosL                           16
#define GPADC_WUP_TMR_PERIOD_PosH                           31

/*!< 0x0010 GPADC_WUP_TMR_CTRL
*null
*/
#define GPADC_WUP_TMR_CLR_Msk                               (0x01 << GPADC_WUP_TMR_CLR_Pos)
#define GPADC_WUP_TMR_CLR_Pos                               9
#define GPADC_WUP_TMR_CLR_PosL                              9
#define GPADC_WUP_TMR_CLR_PosH                              9

/*!< 0x0010 GPADC_WUP_TMR_CTRL
*null
*/
#define GPADC_WUP_TMR_EN_Msk                                (0x01 << GPADC_WUP_TMR_EN_Pos)
#define GPADC_WUP_TMR_EN_Pos                                8
#define GPADC_WUP_TMR_EN_PosL                               8
#define GPADC_WUP_TMR_EN_PosH                               8

/*!< 0x0010 GPADC_WUP_TMR_CTRL
*null
*/
#define GPADC_WUP_TMR_CLK_SRC_SEL_Msk                       (0x03 << GPADC_WUP_TMR_CLK_SRC_SEL_Pos)
#define GPADC_WUP_TMR_CLK_SRC_SEL_Pos                       4
#define GPADC_WUP_TMR_CLK_SRC_SEL_PosL                      4
#define GPADC_WUP_TMR_CLK_SRC_SEL_PosH                      5

/*!< 0x0010 GPADC_WUP_TMR_CTRL
*null
*/
#define GPADC_WUP_TMR_CLK_EN_Msk                            (0x01 << GPADC_WUP_TMR_CLK_EN_Pos)
#define GPADC_WUP_TMR_CLK_EN_Pos                            1
#define GPADC_WUP_TMR_CLK_EN_PosL                           1
#define GPADC_WUP_TMR_CLK_EN_PosH                           1

/*!< 0x0010 GPADC_WUP_TMR_CTRL
*null
*/
#define GPADC_WUP_TMR_RESETN_Msk                            (0x01 << GPADC_WUP_TMR_RESETN_Pos)
#define GPADC_WUP_TMR_RESETN_Pos                            0
#define GPADC_WUP_TMR_RESETN_PosL                           0
#define GPADC_WUP_TMR_RESETN_PosH                           0


/*!< 0x00FC WIO_GPADC_MODE
*null
*/
#define WIO_GPADC_MODE_Msk                                  (0x01 << WIO_GPADC_MODE_Pos)
#define WIO_GPADC_MODE_Pos                                  0
#define WIO_GPADC_MODE_PosL                                 0
#define WIO_GPADC_MODE_PosH                                 0


/*!< 0x0100 WIO_MODE_SEL0
*null
*/
#define WIO_MODE_SEL0_Msk                                   (0x03 << WIO_MODE_SEL0_Pos)
#define WIO_MODE_SEL0_Pos                                   0
#define WIO_MODE_SEL0_PosL                                  0
#define WIO_MODE_SEL0_PosH                                  1


/*!< 0x0104 WIO_MODE_SEL1
*null
*/
#define WIO_MODE_SEL1_Msk                                   (0x03 << WIO_MODE_SEL1_Pos)
#define WIO_MODE_SEL1_Pos                                   0
#define WIO_MODE_SEL1_PosL                                  0
#define WIO_MODE_SEL1_PosH                                  1


/*!< 0x0108 WIO_HOLD_OUTPUT
*null
*/
#define WIO_HOLD_OUTPUT_Msk                                 (0x01 << WIO_HOLD_OUTPUT_Pos)
#define WIO_HOLD_OUTPUT_Pos                                 0
#define WIO_HOLD_OUTPUT_PosL                                0
#define WIO_HOLD_OUTPUT_PosH                                0


/*!< 0x010c WIO_HOLD_PULLUPDOWN_SET0
*null
*/
#define WIO_HOLD_PULLUPDOWN_SET0_Msk                        (0x03 << WIO_HOLD_PULLUPDOWN_SET0_Pos)
#define WIO_HOLD_PULLUPDOWN_SET0_Pos                        0
#define WIO_HOLD_PULLUPDOWN_SET0_PosL                       0
#define WIO_HOLD_PULLUPDOWN_SET0_PosH                       1


/*!< 0x0110 WIO_HOLD_PULLUPDOWN_SET1
*null
*/
#define WIO_HOLD_PULLUPDOWN_SET1_Msk                        (0x03 << WIO_HOLD_PULLUPDOWN_SET1_Pos)
#define WIO_HOLD_PULLUPDOWN_SET1_Pos                        0
#define WIO_HOLD_PULLUPDOWN_SET1_PosL                       0
#define WIO_HOLD_PULLUPDOWN_SET1_PosH                       1


/*!< 0x0114 WIO_WUP_TYPE
*null
*/
#define WIO_WUP_TYPE0_Msk                                   (0x07 << WIO_WUP_TYPE0_Pos)
#define WIO_WUP_TYPE0_Pos                                   0
#define WIO_WUP_TYPE0_PosL                                  0
#define WIO_WUP_TYPE0_PosH                                  2


/*!< 0x0118 WIO_WUP_TYPE
*null
*/
#define WIO_WUP_TYPE1_Msk                                   (0x07 << WIO_WUP_TYPE1_Pos)
#define WIO_WUP_TYPE1_Pos                                   0
#define WIO_WUP_TYPE1_PosL                                  0
#define WIO_WUP_TYPE1_PosH                                  2


/*!< 0x011c WIO_WUP_TYPE
*null
*/
#define WIO_WUP_TYPE2_Msk                                   (0x07 << WIO_WUP_TYPE2_Pos)
#define WIO_WUP_TYPE2_Pos                                   0
#define WIO_WUP_TYPE2_PosL                                  0
#define WIO_WUP_TYPE2_PosH                                  2


/*!< 0x0120 WIO_WUP_TYPE
*null
*/
#define WIO_WUP_TYPE0_Msk                                   (0x07 << WIO_WUP_TYPE0_Pos)
#define WIO_WUP_TYPE0_Pos                                   0
#define WIO_WUP_TYPE0_PosL                                  0
#define WIO_WUP_TYPE0_PosH                                  2


/*!< 0x0124 WIO_WUP_CLR
*null
*/
#define WIO_WUP_CLR_Msk                                     (0x01 << WIO_WUP_CLR_Pos)
#define WIO_WUP_CLR_Pos                                     0
#define WIO_WUP_CLR_PosL                                    0
#define WIO_WUP_CLR_PosH                                    0



/*!< 0x012c WIO_WUP_DBC_SET
*null
*/
#define WIO_WUP_DBC_SET_Msk                                 (0x1F << WIO_WUP_DBC_SET_Pos)
#define WIO_WUP_DBC_SET_Pos                                 0
#define WIO_WUP_DBC_SET_PosL                                0
#define WIO_WUP_DBC_SET_PosH                                4


/**
* @}
* End of group HW_MISC
*/

/** @addtopgroup HW_RTTRTC
 * @{
 * */
typedef struct {
	__IOM uint32_t RTT_MR;                      /*!< 0x0000, Mode register */
	__IOM uint32_t RTT_AR0;                     /*!< 0x0004, Alarm register */
	__IM  uint32_t RTT_VR;                      /*!< 0x0008, Counter value register */
	__IM  uint32_t RTT_SR;                      /*!< 0x000C, Status register */
	__IM  uint32_t RTT_ALARM_STATUS0;           /*!< 0x0010, Status register */
	__IM  uint32_t RTT_ALARM_STATUS1;           /*!< 0x0014, Status register */
	__IOM uint32_t RTT_AR1;                     /*!< 0x0018, Alarm register */
//	__IOM uint32_t RTT_MR1;                     /*!< 0x001C, Mode register */
} rtt_dev_t;

#define RTTRTC_DEV                      ((rtt_dev_t *)  (RTC_BASE_ADDR + 0x30)) /*!< 0x4001 8030 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */
/*!< 0x0000 RTT_MR
 * Real-Time Clock 1Hz Clock Selection（write only）
 */
#define RTT_MR_SEL1HZ_Msk                                     (0x01 << RTT_MR_SEL1HZ_Pos)
#define RTT_MR_SEL1HZ_Pos                                     24
#define RTT_MR_SEL1HZ_PosL                                    24
#define RTT_MR_SEL1HZ_PosH                                    24

/*!< 0x0000 RTT_MR
 * Real-time Timer Disable
 */
#define RTT_MR_RTTDIS_Msk                                     (0x01 << RTT_MR_RTTDIS_Pos)
#define RTT_MR_RTTDIS_Pos                                     20
#define RTT_MR_RTTDIS_PosL                                    20
#define RTT_MR_RTTDIS_PosH                                    20

/*!< 0x0000 RTT_MR
 * Real-time Timer Reset
 */
#define RTT_MR_RTTRST_Msk                                     (0x01 << RTT_MR_RTTRST_Pos)
#define RTT_MR_RTTRST_Pos                                     18
#define RTT_MR_RTTRST_PosL                                    18
#define RTT_MR_RTTRST_PosH                                    18

/*!< 0x0000 RTT_MR
 * Real-time Timer Increment Interrupt Enable 0
 */
#define RTT_MR_INC_IEN_Msk                                     (0x01 << RTT_MR_INC_IEN_Pos)
#define RTT_MR_INC_IEN_Pos                                     17
#define RTT_MR_INC_IEN_PosL                                    17
#define RTT_MR_INC_IEN_PosH                                    17

/*!< 0x0000 RTT_MR
 * Real-time Timer alarm Interrupt Enbale 0
 */
#define RTT_MR_ALM_IEN_Msk                                     (0x01 << RTT_MR_ALM_IEN_Pos)
#define RTT_MR_ALM_IEN_Pos                                     16
#define RTT_MR_ALM_IEN_PosL                                    16
#define RTT_MR_ALM_IEN_PosH                                    16

/*!< 0x0000 RTT_MR
 * Real-time Timer Prescaler Value (value == 0 or value > 2, forbidden == 1 or == 2) ????
 */
#define RTT_MR_PREDIV_Msk                                     (0x0F << RTT_MR_PREDIV_Pos)
#define RTT_MR_PREDIV_Pos                                     0
#define RTT_MR_PREDIV_PosL                                    0
#define RTT_MR_PREDIV_PosH                                    15



/*!< 0x0004 RTT_AR0
 * Alarm value
 */
#define RTT_AR_ALVALUE_Msk                                     (0xFFFFFFFF << RTT_AR_ALVALUE_Pos)
#define RTT_AR_ALVALUE_Pos                                     0
#define RTT_AR_ALVALUE_PosL                                    0
#define RTT_AR_ALVALUE_PosH                                    31



/*!< 0x0008 RTT_VR
 * Current Real-time Value
 */
#define RTT_VR_RTVALUE_Msk                                     (0xFFFFFFFF << RTT_VR_RTVALUE_Pos)
#define RTT_VR_RTVALUE_Pos                                     0
#define RTT_VR_RTVALUE_PosL                                    0
#define RTT_VR_RTVALUE_PosH                                    31



/*!< off 0x000C RTT_SR
 * Prescaler Roll-over Status (cleared on read)
 * 0: No prescaler roll-over occurred since the last read of the RTT_SR.
 * 1: Prescaler roll-over occurred since the last read of the RTT_SR.
 */
#define RTT_SR_EXCEED_PRE_Msk                                     (0x01 << RTT_SR_EXCEED_PRE_Pos)
#define RTT_SR_EXCEED_PRE_Pos                                     0
#define RTT_SR_EXCEED_PRE_PosL                                    0
#define RTT_SR_EXCEED_PRE_PosH                                    0



/*!< off 0x0010 RTT_ALSR0
 * Real-time Alarm Status (cleared on read)
 */
#define RTT_ALSR0_ALSTATU_Msk                                     (0x01 << RTT_ALSR0_ALSTATU_Pos)
#define RTT_ALSR0_ALSTATU_Pos                                     0
#define RTT_ALSR0_ALSTATU_PosL                                    0
#define RTT_ALSR0_ALSTATU_PosH                                    0



/*!< off 0x0014 RTT_ALSR1
 * Real-time Alarm Status (cleared on read)
 */
#define RTT_ALSR1_ALSTATU_Msk                                     (0x01 << RTT_ALSR1_ALSTATU_Pos)
#define RTT_ALSR1_ALSTATU_Pos                                     0
#define RTT_ALSR1_ALSTATU_PosL                                    0
#define RTT_ALSR1_ALSTATU_PosH                                    0



/*!< 0x0018 RTT_AR1
 * Current Real-time Value
 */
#define RTT_VR_RTVALUE_Msk                                     (0xFFFFFFFF << RTT_VR_RTVALUE_Pos)
#define RTT_VR_RTVALUE_Pos                                     0
#define RTT_VR_RTVALUE_PosL                                    0
#define RTT_VR_RTVALUE_PosH                                    31




/*!< 0x001C RTT_MR1 Mode register
 * Real-time Timer Increment Interrupt Enable 1
 */
#define RTT_MR1_INCIEN_Msk                                      (0x01 << RTT_MR1_INCIEN__Pos)
#define RTT_MR1_INCIEN__Pos                                     17
#define RTT_MR1_INCIEN__PosL                                    17
#define RTT_MR1_INCIEN__PosH                                    17

/*!< 0x001C RTT_MR1 Mode register
 * Real-time Timer Alarm Interrupt Enbale 1
 */
#define RTT_MR1_ALM_IEN_Msk                                      (0x01 << RTT_MR1_ALM_IEN_Pos)
#define RTT_MR1_ALM_IEN_Pos                                      16
#define RTT_MR1_ALM_IEN_PosL                                     16
#define RTT_MR1_ALM_IEN_PosH                                     16


/**
* @}
* End of group HW_RTTRTC
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_MISC_H_ */
