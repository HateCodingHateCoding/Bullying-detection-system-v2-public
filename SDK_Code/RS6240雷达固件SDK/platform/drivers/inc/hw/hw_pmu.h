/****************************************************************
* @file  hw_pmu.h
* @brief hw_pmu hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_PMU_H_
#define _HW_PMU_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_PMU
 * @{
 * */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */
typedef struct {
    __IOM uint32_t PMU_EXTLDO_UP_SETTINGS;                  /*0x0000,             */
    __IOM uint32_t PMU_SMPS_UP_SETTINGS;                    /*0x0004,             */
    __IOM uint32_t PMU_SOCLDO_UP_SETTINGS;                  /*0x0008,             */
    __IOM uint32_t PMU_EFUSE_UP_SETTINGS;                   /*0x000c,             */
    __IOM uint32_t PMU_SMPS_DWN_SETTINGS;                   /*0x0010,             */
    __IOM uint32_t PMU_DCXO_SETTINGS1;                      /*0x0014,             */
    __IOM uint32_t PMU_DCXO_SETTINGS2;                      /*0x0018,             */
    __IOM uint32_t PMU_SPLLLDO_UP_SETTINGS;                 /*0x001c,             */
    __IOM uint32_t PMU_SW_SETTINGS;                         /*0x0020,             */
    __IOM uint32_t RTC_SW_UP_SETTINGS;                      /*0x0024,             */
    __IOM uint32_t MSI_UP_SETTINGS;                         /*0x0028,             */
    __IOM uint32_t DCXO_PLL_DOWN_SETTINGS;                  /*0x002C,             */
          uint32_t RSVD[8];                                 /*0x0030-0x004C,      */
    __IOM uint32_t PMU_RTCLDO_CTRL;                         /*0x0050,             */
    __IOM uint32_t PMU_EXTLDO_CTRL;                         /*0x0054,             */
    __IOM uint32_t PMU_INTLDO_CTRL;                         /*0x0058,             */
    __IOM uint32_t PMU_SMPS_CTRL;                           /*0x005c,             */
    __IOM uint32_t PMU_SOCLDO_CTRL;                         /*0x0060,             */
    __IOM uint32_t PMU_BGLDO_CTRL;                          /*0x0064,             */
    __IOM uint32_t PMU_ANA_EN;                              /*0x0068,             */
    __IOM uint32_t PMU_EFUSE_CTRL;                          /*0x006C,             */
          uint32_t RSVD1[4];                                /*0x0070-0x007C,      */
    __IOM uint32_t PMU_ANA_BG_CTRL;                         /*0x0080,             */
          uint32_t RSVD2[2];                                /*0x0084-0X0088,      */
    __IOM uint32_t PMU_SRAMA_SHARE;                         /*0x008C,             */
    __IOM uint32_t PMU_DCXO_CTRL;                           /*0x0090,             */
    __IOM uint32_t PMU_DCXO_CTRL3;                          /*0x0094,             */
          uint32_t RSVD3[2];                                /*0x0098-0x009C,      */
    __IOM uint32_t PMU_SPLL_CTRL;                           /*0x0090,             */
          uint32_t RSVD4[2];                                /*0x00A4-0x00A8,      */
    __IOM uint32_t PMU_M50_CTRL;                            /*0x00AC,             */
    __IOM uint32_t PMU_RC_CTRL;                             /*0x00B0,             */
    __IOM uint32_t PMU_FORCE_WUP_CTRL;                      /*0x00B4,             */
    __IOM uint32_t PMU_FORCE_DOWN_CTRL;                     /*0x00B8,             */
          uint32_t RSVD5;                                   /*0x00BC,             */
    __IOM uint32_t SYS0_SRAM_SW_CTRL;                       /*0x00C0,             */
    __IOM uint32_t SYS1_SRAM_SW_CTRL;                       /*0x00C4,             */
          uint32_t RSVD6[2];                                /*0x00C8-0X00CC,      */
    __IOM uint32_t DCDC_PUP_OUT_CTRL;                       /*0x00D0,             */
    __IOM uint32_t DCDC_TEST_CTRL;                          /*0x00D4,             */
          uint32_t RSVD7;                                   /*0x00D8,             */
    __IOM uint32_t LDO_ON_MODE_SEL;                         /*0x00DC,             */
    __IOM uint32_t PMU_BYPASS_LPMS_LOCK;                    /*0x00E0,             */
    __IOM uint32_t PMU_SYS0_LPMS;                           /*0x00E4,             */
    __IOM uint32_t PMU_SYS1_LPMS;                           /*0x00E8,             */
    __IOM uint32_t PMU_MMW_LMPS;                            /*0x00EC,             */
          uint32_t RSVD8[3];                                /*0x00F0-0x00F8,      */
    __IOM uint32_t PMU_STATUS;                              /*0x00FC,             */
    __IOM uint32_t PMU_HBACK;                               /*0x0100,             */
          uint32_t RSVD9[3];                                /*0x0104-0x010C,      */
    __IOM uint32_t PMU_TSENSOR_MASK_CTRL;                   /*0x0110,             */
    __IOM uint32_t PMU_SYS0_WAKEUP_C_MASK;                  /*0x0114,             */
    __IOM uint32_t PMU_SYS1_WAKEUP_C_MASK;                  /*0x0118,             */
    __IOM uint32_t MMW_PREVENT_DWN0;                        /*0x011C,             */
    __IOM uint32_t MMW_PREVENT_DWN1;                        /*0x0120,             */
          uint32_t RSVD10[2];                               /*0x0124-0X0128,      */
    __IOM uint32_t EFUSE_BOOTSEL_DIS;                       /*0x012C,             */
    __IOM uint32_t PMU_WUP_FLAG;                            /*0x0130,             */
    __IOM uint32_t PMU_SPI_WUP_CTRL;                        /*0x0134,             */
    __IOM uint32_t PMU_IIC_WUP_CTRL;                        /*0x0138,             */
    __IOM uint32_t PMU_UART_WUP_CTRL;                       /*0x013C,             */
    __IOM uint32_t PMU_TMR_WUP_CTRL0;                       /*0x0140,             */
    __IOM uint32_t PMU_TMR_WUP_CTRL1;                       /*0x144h,             */
    __IOM uint32_t PMU_TMR_WUP_INTR_FLAG;                   /*0x148h,             */
    __IOM uint32_t PMU_TMR_WUP_VAL0;                        /*0x014C,             */
    __IOM uint32_t PMU_TMR_WUP_VAL1;                        /*0x0150,             */
    __IOM uint32_t PMU_TMR_WUP_CMP0_VAL0;                   /*0x0154,             */
    __IOM uint32_t PMU_TMR_WUP_CMP0_VAL1;                   /*0x0158,             */
    __IOM uint32_t PMU_TMR_WUP_CMP1_VAL0;                   /*0x015C,             */
    __IOM uint32_t PMU_TMR_WUP_CMP1_VAL1;                   /*0x0160,             */
          uint32_t RSVD11[7];                               /*0x0164-0X017C,      */
    __IOM uint32_t PMU_DCXO_OVRD;                           /*0x0180,             */
          uint32_t RSVD12;                                  /*0x0184,             */
    __IOM uint32_t PMU_SPLL_OVRD;                           /*0x0188,             */


} pmu_dev_t;

#define PMU_DEV                         ((pmu_dev_t *)    PWR_BASE_ADDR)  /*!< 0x4001 A000 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */
/*!< 0x0000 PMU_EXTLDO_UP_SETTINGS
*null
*/
#define EXTLDO_UP_SETTINGS_Msk                              (0xFFFF << EXTLDO_UP_SETTINGS_Pos)
#define EXTLDO_UP_SETTINGS_Pos                              0
#define EXTLDO_UP_SETTINGS_PosL                             0
#define EXTLDO_UP_SETTINGS_PosH                             15


/*!< 0x0004 PMU_SMPS_UP_SETTINGS
*null
*/
#define SMPS_UP_SETTINGS_Msk                                (0xFFFF << SMPS_UP_SETTINGS_Pos)
#define SMPS_UP_SETTINGS_Pos                                0
#define SMPS_UP_SETTINGS_PosL                               0
#define SMPS_UP_SETTINGS_PosH                               15


/*!< 0x0008 PMU_SOCLDO_UP_SETTINGS
*null
*/
#define SOCLDO_TIMING_SETTINGS_Msk                          (0X1FF << SOCLDO_TIMING_SETTINGS_Pos)
#define SOCLDO_TIMING_SETTINGS_Pos                          16
#define SOCLDO_TIMING_SETTINGS_PosL                         16
#define SOCLDO_TIMING_SETTINGS_PosH                         24

/*!< 0x0008 PMU_SOCLDO_UP_SETTINGS
*null
*/
#define SOCLDO_UP_SETTINGS_Msk                              (0xFFFF << SOCLDO_UP_SETTINGS_Pos)
#define SOCLDO_UP_SETTINGS_Pos                              0
#define SOCLDO_UP_SETTINGS_PosL                             0
#define SOCLDO_UP_SETTINGS_PosH                             15


/*!< 0x000c PMU_EFUSE_UP_SETTINGS
*null
*/
#define EFUSE_UP_SETTINGS_Msk                               (0xFFFF << EFUSE_UP_SETTINGS_Pos)
#define EFUSE_UP_SETTINGS_Pos                               0
#define EFUSE_UP_SETTINGS_PosL                              0
#define EFUSE_UP_SETTINGS_PosH                              15


/*!< 0x0010 PMU_SMPS_DWN_SETTINGS
*null
*/
#define SMPS_DWN_SETTINGS_Msk                               (0xFFFF << SMPS_DWN_SETTINGS_Pos)
#define SMPS_DWN_SETTINGS_Pos                               0
#define SMPS_DWN_SETTINGS_PosL                              0
#define SMPS_DWN_SETTINGS_PosH                              15


/*!< 0x0014 PMU_DCXO_SETTINGS1
*null
*/
#define DCXO_UP_SETTINGS_Msk                                (0xFFFF << DCXO_UP_SETTINGS_Pos)
#define DCXO_UP_SETTINGS_Pos                                16
#define DCXO_UP_SETTINGS_PosL                               16
#define DCXO_UP_SETTINGS_PosH                               31

/*!< 0x0014 PMU_DCXO_SETTINGS1
*null
*/
#define DCXOLDO_UP_SETTINGS_Msk                             (0xFFFF << DCXOLDO_UP_SETTINGS_Pos)
#define DCXOLDO_UP_SETTINGS_Pos                             0
#define DCXOLDO_UP_SETTINGS_PosL                            0
#define DCXOLDO_UP_SETTINGS_PosH                            15


/*!< 0x0018 PMU_DCXO_SETTINGS2
*null
*/
#define DCXO_OUT_STABLE_SETTINGS_Msk                        (0x3F << DCXO_OUT_STABLE_SETTINGS_Pos)
#define DCXO_OUT_STABLE_SETTINGS_Pos                        26
#define DCXO_OUT_STABLE_SETTINGS_PosL                       26
#define DCXO_OUT_STABLE_SETTINGS_PosH                       31

/*!< 0x0018 PMU_DCXO_SETTINGS2
*null
*/
#define XTAL_DECTECT_SETTINGS_Msk                           (0x3FF << XTAL_DECTECT_SETTINGS_Pos)
#define XTAL_DECTECT_SETTINGS_Pos                           16
#define XTAL_DECTECT_SETTINGS_PosL                          16
#define XTAL_DECTECT_SETTINGS_PosH                          25

/*!< 0x0018 PMU_DCXO_SETTINGS2
*null
*/
#define DCXO_STARTUP_SETTINGS_Msk                           (0xFFFF << DCXO_STARTUP_SETTINGS_Pos)
#define DCXO_STARTUP_SETTINGS_Pos                           0
#define DCXO_STARTUP_SETTINGS_PosL                          0
#define DCXO_STARTUP_SETTINGS_PosH                          15


/*!< 0x001c PMU_SPLLLDO_UP_SETTINGS
*null
*/
#define SPLL_UP_SETTINGS_Msk                                (0xFFFF << SPLL_UP_SETTINGS_Pos)
#define SPLL_UP_SETTINGS_Pos                                16
#define SPLL_UP_SETTINGS_PosL                               16
#define SPLL_UP_SETTINGS_PosH                               31

/*!< 0x001c PMU_SPLLLDO_UP_SETTINGS
*null
*/
#define SPLLLDO_UP_SETTINGS_Msk                             (0xFFFF << SPLLLDO_UP_SETTINGS_Pos)
#define SPLLLDO_UP_SETTINGS_Pos                             0
#define SPLLLDO_UP_SETTINGS_PosL                            0
#define SPLLLDO_UP_SETTINGS_PosH                            15


/*!< 0x0020 PMU_SW_SETTINGS
*null
*/
#define SW_SETTINGS_Msk                                     (0X1FF << SW_SETTINGS_Pos)
#define SW_SETTINGS_Pos                                     16
#define SW_SETTINGS_PosL                                    16
#define SW_SETTINGS_PosH                                    24

/*!< 0x0020 PMU_SW_SETTINGS
*null
*/
#define SW_UP_SETTINGS_Msk                                  (0x7F << SW_UP_SETTINGS_Pos)
#define SW_UP_SETTINGS_Pos                                  0
#define SW_UP_SETTINGS_PosL                                 0
#define SW_UP_SETTINGS_PosH                                 6


/*!< 0x0024 RTC_SW_UP_SETTINGS
*null
*/
#define RTC_SW2_UP_SETTING_Msk                              (0x3F << RTC_SW2_UP_SETTING_Pos)
#define RTC_SW2_UP_SETTING_Pos                              8
#define RTC_SW2_UP_SETTING_PosL                             8
#define RTC_SW2_UP_SETTING_PosH                             13

/*!< 0x0024 RTC_SW_UP_SETTINGS
*null
*/
#define RTC_SW1_UP_SETTING_Msk                              (0x3F << RTC_SW1_UP_SETTING_Pos)
#define RTC_SW1_UP_SETTING_Pos                              0
#define RTC_SW1_UP_SETTING_PosL                             0
#define RTC_SW1_UP_SETTING_PosH                             5


/*!< 0x0028 MSI_UP_SETTINGS
*null
*/
#define MSI_UP_SETTINGS_Msk                                 (0x0F << MSI_UP_SETTINGS_Pos)
#define MSI_UP_SETTINGS_Pos                                 0
#define MSI_UP_SETTINGS_PosL                                0
#define MSI_UP_SETTINGS_PosH                                3

/*!< 0x002C DCXO_PLL_DOWN_SETTINGS
*null
*/
#define DCXO_DWN_SETTINGS_Msk                                (0x07 << DCXO_DWN_SETTINGS_Pos)
#define DCXO_DWN_SETTINGS_Pos                                4
#define DCXO_DWN_SETTINGS_PosL                               4
#define DCXO_DWN_SETTINGS_PosH                               6

/*!< 0x002C DCXO_PLL_DOWN_SETTINGS
*null
*/
#define PLL_DWN_SETTINGS_Msk                                 (0x07 << PLL_DWN_SETTINGS_Pos)
#define PLL_DWN_SETTINGS_Pos                                 0
#define PLL_DWN_SETTINGS_PosL                                0
#define PLL_DWN_SETTINGS_PosH                                2

/*!< 0x0050 PMU_RTCLDO_CTRL
*null
*/
#define RTCLDO_LP_VSEL_Msk                                  (0x0F << RTCLDO_LP_VSEL_Pos)
#define RTCLDO_LP_VSEL_Pos                                  8
#define RTCLDO_LP_VSEL_PosL                                 8
#define RTCLDO_LP_VSEL_PosH                                 11

/*!< 0x0050 PMU_RTCLDO_CTRL
*null
*/
#define RTCLDO_ACTIVE_VSEL_Msk                              (0x0F << RTCLDO_ACTIVE_VSEL_Pos)
#define RTCLDO_ACTIVE_VSEL_Pos                              0
#define RTCLDO_ACTIVE_VSEL_PosL                             0
#define RTCLDO_ACTIVE_VSEL_PosH                             3


/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_BYPASS_Msk                                   (0x01 << EXTLDO_BYPASS_Pos)
#define EXTLDO_BYPASS_Pos                                   28
#define EXTLDO_BYPASS_PosL                                  28
#define EXTLDO_BYPASS_PosH                                  28

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_MMW_ACTIVE_HP_EN_Msk                         (0x01 << EXTLDO_MMW_ACTIVE_HP_EN_Pos)
#define EXTLDO_MMW_ACTIVE_HP_EN_Pos                         26
#define EXTLDO_MMW_ACTIVE_HP_EN_PosL                        26
#define EXTLDO_MMW_ACTIVE_HP_EN_PosH                        26

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_SYS1_ACTIVE_HP_EN_Msk                        (0x01 << EXTLDO_SYS1_ACTIVE_HP_EN_Pos)
#define EXTLDO_SYS1_ACTIVE_HP_EN_Pos                        25
#define EXTLDO_SYS1_ACTIVE_HP_EN_PosL                       25
#define EXTLDO_SYS1_ACTIVE_HP_EN_PosH                       25

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_SYS0_ACTIVE_HP_EN_Msk                        (0x01 << EXTLDO_SYS0_ACTIVE_HP_EN_Pos)
#define EXTLDO_SYS0_ACTIVE_HP_EN_Pos                        24
#define EXTLDO_SYS0_ACTIVE_HP_EN_PosL                       24
#define EXTLDO_SYS0_ACTIVE_HP_EN_PosH                       24

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_SW_MODE_Msk                                  (0x01 << EXTLDO_SW_MODE_Pos)
#define EXTLDO_SW_MODE_Pos                                  18
#define EXTLDO_SW_MODE_PosL                                 18
#define EXTLDO_SW_MODE_PosH                                 18

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_SLP_HP_EN_Msk                                (0x01 << EXTLDO_SLP_HP_EN_Pos)
#define EXTLDO_SLP_HP_EN_Pos                                17
#define EXTLDO_SLP_HP_EN_PosL                               17
#define EXTLDO_SLP_HP_EN_PosH                               17

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_OFF_Msk                                      (0x01 << EXTLDO_OFF_Pos)
#define EXTLDO_OFF_Pos                                      16
#define EXTLDO_OFF_PosL                                     16
#define EXTLDO_OFF_PosH                                     16

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_LP_VSEL_Msk                                  (0x0F << EXTLDO_LP_VSEL_Pos)
#define EXTLDO_LP_VSEL_Pos                                  8
#define EXTLDO_LP_VSEL_PosL                                 8
#define EXTLDO_LP_VSEL_PosH                                 11

/*!< 0x0054 PMU_EXTLDO_CTRL
*null
*/
#define EXTLDO_VSEL_Msk                                     (0x0F << EXTLDO_VSEL_Pos)
#define EXTLDO_VSEL_Pos                                     0
#define EXTLDO_VSEL_PosL                                    0
#define EXTLDO_VSEL_PosH                                    3


/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define INTLDO_BYPASS_Msk                                   (0x01 << INTLDO_BYPASS_Pos)
#define INTLDO_BYPASS_Pos                                   28
#define INTLDO_BYPASS_PosL                                  28
#define INTLDO_BYPASS_PosH                                  28

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define MMW_INTLDO_ACTIVE_HP_EN_Msk                         (0x01 << MMW_INTLDO_ACTIVE_HP_EN_Pos)
#define MMW_INTLDO_ACTIVE_HP_EN_Pos                         26
#define MMW_INTLDO_ACTIVE_HP_EN_PosL                        26
#define MMW_INTLDO_ACTIVE_HP_EN_PosH                        26

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define SYS1_INTLDO_ACTIVE_HP_EN_Msk                        (0x01 << SYS1_INTLDO_ACTIVE_HP_EN_Pos)
#define SYS1_INTLDO_ACTIVE_HP_EN_Pos                        25
#define SYS1_INTLDO_ACTIVE_HP_EN_PosL                       25
#define SYS1_INTLDO_ACTIVE_HP_EN_PosH                       25

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define SYS0_INTLDO_ACTIVE_HP_EN_Msk                        (0x01 << SYS0_INTLDO_ACTIVE_HP_EN_Pos)
#define SYS0_INTLDO_ACTIVE_HP_EN_Pos                        24
#define SYS0_INTLDO_ACTIVE_HP_EN_PosL                       24
#define SYS0_INTLDO_ACTIVE_HP_EN_PosH                       24

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define INTLDO_SW_MODE_Msk                                  (0x01 << INTLDO_SW_MODE_Pos)
#define INTLDO_SW_MODE_Pos                                  18
#define INTLDO_SW_MODE_PosL                                 18
#define INTLDO_SW_MODE_PosH                                 18

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define INTLDO_SLP_HP_EN_Msk                                (0x01 << INTLDO_SLP_HP_EN_Pos)
#define INTLDO_SLP_HP_EN_Pos                                17
#define INTLDO_SLP_HP_EN_PosL                               17
#define INTLDO_SLP_HP_EN_PosH                               17

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define INTLDO_OFF_Msk                                      (0x01 << INTLDO_OFF_Pos)
#define INTLDO_OFF_Pos                                      16
#define INTLDO_OFF_PosL                                     16
#define INTLDO_OFF_PosH                                     16

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define INTLDO_SLP_VSEL_Msk                                 (0x0F << INTLDO_SLP_VSEL_Pos)
#define INTLDO_SLP_VSEL_Pos                                 8
#define INTLDO_SLP_VSEL_PosL                                8
#define INTLDO_SLP_VSEL_PosH                                11

/*!< 0x0058 PMU_INTLDO_CTRL
*null
*/
#define INTLDO_VSEL_Msk                                     (0x0F << INTLDO_VSEL_Pos)
#define INTLDO_VSEL_Pos                                     0
#define INTLDO_VSEL_PosL                                    0
#define INTLDO_VSEL_PosH                                    3


/*!< 0x005c PMU_SMPS_CTRL
*null
*/
#define SMPS_PFM_EN_Msk                                     (0x01 << SMPS_PFM_EN_Pos)
#define SMPS_PFM_EN_Pos                                     17
#define SMPS_PFM_EN_PosL                                    17
#define SMPS_PFM_EN_PosH                                    17

/*!< 0x005c PMU_SMPS_CTRL
*null
*/
#define SMPS_OFF_Msk                                        (0x01 << SMPS_OFF_Pos)
#define SMPS_OFF_Pos                                        16
#define SMPS_OFF_PosL                                       16
#define SMPS_OFF_PosH                                       16

/*!< 0x005c PMU_SMPS_CTRL
*null
*/
#define SMPS_LP_VSEL_Msk                                    (0x0F << SMPS_LP_VSEL_Pos)
#define SMPS_LP_VSEL_Pos                                    8
#define SMPS_LP_VSEL_PosL                                   8
#define SMPS_LP_VSEL_PosH                                   11

/*!< 0x005c PMU_SMPS_CTRL
*null
*/
#define SMPS_VSEL_Msk                                       (0x0F << SMPS_VSEL_Pos)
#define SMPS_VSEL_Pos                                       0
#define SMPS_VSEL_PosL                                      0
#define SMPS_VSEL_PosH                                      3


/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_STARTIC_Msk                                  (0x01 << SOCLDO_STARTIC_Pos)
#define SOCLDO_STARTIC_Pos                                  30
#define SOCLDO_STARTIC_PosL                                 30
#define SOCLDO_STARTIC_PosH                                 30

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_TESTPK_Msk                                   (0x01 << SOCLDO_TESTPK_Pos)
#define SOCLDO_TESTPK_Pos                                   29
#define SOCLDO_TESTPK_PosL                                  29
#define SOCLDO_TESTPK_PosH                                  29

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_TESTSC_Msk                                   (0x01 << SOCLDO_TESTSC_Pos)
#define SOCLDO_TESTSC_Pos                                   28
#define SOCLDO_TESTSC_PosL                                  28
#define SOCLDO_TESTSC_PosH                                  28

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define MMW_SOCDO_ACTIVE_HP_EN_Msk                          (0x01 << MMW_SOCDO_ACTIVE_HP_EN_Pos)
#define MMW_SOCDO_ACTIVE_HP_EN_Pos                          26
#define MMW_SOCDO_ACTIVE_HP_EN_PosL                         26
#define MMW_SOCDO_ACTIVE_HP_EN_PosH                         26

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SYS1_SOCDO_ACTIVE_HP_EN_Msk                         (0x01 << SYS1_SOCDO_ACTIVE_HP_EN_Pos)
#define SYS1_SOCDO_ACTIVE_HP_EN_Pos                         25
#define SYS1_SOCDO_ACTIVE_HP_EN_PosL                        25
#define SYS1_SOCDO_ACTIVE_HP_EN_PosH                        25

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SYS0_SOCDO_ACTIVE_HP_EN_Msk                         (0x01 << SYS0_SOCDO_ACTIVE_HP_EN_Pos)
#define SYS0_SOCDO_ACTIVE_HP_EN_Pos                         24
#define SYS0_SOCDO_ACTIVE_HP_EN_PosL                        24
#define SYS0_SOCDO_ACTIVE_HP_EN_PosH                        24

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_ACTIVE_FASTUP_EN_Msk                         (0x01 << SOCLDO_ACTIVE_FASTUP_EN_Pos)
#define SOCLDO_ACTIVE_FASTUP_EN_Pos                         19
#define SOCLDO_ACTIVE_FASTUP_EN_PosL                        19
#define SOCLDO_ACTIVE_FASTUP_EN_PosH                        19

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_BYPASS_UP_TIMING_PROC_EN_Msk                 (0x01 << SOCLDO_BYPASS_UP_TIMING_PROC_EN_Pos)
#define SOCLDO_BYPASS_UP_TIMING_PROC_EN_Pos                 18
#define SOCLDO_BYPASS_UP_TIMING_PROC_EN_PosL                18
#define SOCLDO_BYPASS_UP_TIMING_PROC_EN_PosH                18

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_SLP_HP_EN_Msk                                (0x01 << SOCLDO_SLP_HP_EN_Pos)
#define SOCLDO_SLP_HP_EN_Pos                                17
#define SOCLDO_SLP_HP_EN_PosL                               17
#define SOCLDO_SLP_HP_EN_PosH                               17

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_OFF_Msk                                      (0x01 << SOCLDO_OFF_Pos)
#define SOCLDO_OFF_Pos                                      16
#define SOCLDO_OFF_PosL                                     16
#define SOCLDO_OFF_PosH                                     16

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_LP_VSEL_Msk                                  (0x0F << SOCLDO_LP_VSEL_Pos)
#define SOCLDO_LP_VSEL_Pos                                  8
#define SOCLDO_LP_VSEL_PosL                                 8
#define SOCLDO_LP_VSEL_PosH                                 11

/*!< 0x0060 PMU_SOCLDO_CTRL
*null
*/
#define SOCLDO_VSEL_Msk                                     (0x0F << SOCLDO_VSEL_Pos)
#define SOCLDO_VSEL_Pos                                     0
#define SOCLDO_VSEL_PosL                                    0
#define SOCLDO_VSEL_PosH                                    3


/*!< 0x0064 PMU_BGLDO_CTRL
*null
*/
#define BGLDO_FASTON_N_Msk                                  (0x01 << BGLDO_FASTON_N_Pos)
#define BGLDO_FASTON_N_Pos                                  4
#define BGLDO_FASTON_N_PosL                                 4
#define BGLDO_FASTON_N_PosH                                 4

/*!< 0x0064 PMU_BGLDO_CTRL
*null
*/
#define BGLDO_TRIM_Msk                                      (0x0F << BGLDO_TRIM_Pos)
#define BGLDO_TRIM_Pos                                      0
#define BGLDO_TRIM_PosL                                     0
#define BGLDO_TRIM_PosH                                     3


/*!< 0x0068 PMU_ANA_EN
*null
*/
#define SPLL_BB_CLK_SEL_DIS_Msk                             (0x01 << SPLL_BB_CLK_SEL_DIS_Pos)
#define SPLL_BB_CLK_SEL_DIS_Pos                             31
#define SPLL_BB_CLK_SEL_DIS_PosL                            31
#define SPLL_BB_CLK_SEL_DIS_PosH                            31

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define SPLL_LDO_LQ_Msk                                     (0x01 << SPLL_LDO_LQ_Pos)
#define SPLL_LDO_LQ_Pos                                     30
#define SPLL_LDO_LQ_PosL                                    30
#define SPLL_LDO_LQ_PosH                                    30

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define SPLL_EN_Msk                                         (0x01 << SPLL_EN_Pos)
#define SPLL_EN_Pos                                         29
#define SPLL_EN_PosL                                        29
#define SPLL_EN_PosH                                        29

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define SPLLLDO_EN_Msk                                      (0x01 << SPLLLDO_EN_Pos)
#define SPLLLDO_EN_Pos                                      28
#define SPLLLDO_EN_PosL                                     28
#define SPLLLDO_EN_PosH                                     28

/*!< 0x0068 SPLL_OFF
*null
*/
#define SLP_SPLL_OFF_Msk                                    (0x01 << SLP_SPLL_OFF_Pos)
#define SLP_SPLL_OFF_Pos                                    27
#define SLP_SPLL_OFF_PosL                                   27
#define SLP_SPLL_OFF_PosH                                   27

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define SLP_DCXO_OFF_Msk                                    (0x01 << SLP_DCXO_OFF_Pos)
#define SLP_DCXO_OFF_Pos                                    26
#define SLP_DCXO_OFF_PosL                                   26
#define SLP_DCXO_OFF_PosH                                   26

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define DCXO_EN_Msk                                         (0x01 << DCXO_EN_Pos)
#define DCXO_EN_Pos                                         25
#define DCXO_EN_PosL                                        25
#define DCXO_EN_PosH                                        25

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define DCXOLDO_EN_Msk                                      (0x01 << DCXOLDO_EN_Pos)
#define DCXOLDO_EN_Pos                                      24
#define DCXOLDO_EN_PosL                                     24
#define DCXOLDO_EN_PosH                                     24

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define RTCSW2_EN_Msk                                       (0x01 << RTCSW2_EN_Pos)
#define RTCSW2_EN_Pos                                       1
#define RTCSW2_EN_PosL                                      1
#define RTCSW2_EN_PosH                                      1

/*!< 0x0068 PMU_ANA_EN
*null
*/
#define RTCSW1_EN_Msk                                       (0x01 << RTCSW1_EN_Pos)
#define RTCSW1_EN_Pos                                       0
#define RTCSW1_EN_PosL                                      0
#define RTCSW1_EN_PosH                                      0


/*!< 0x006c PMU_EFUSE_CTRL
*null
*/
#define MMW_UP_RD_EFUSE_EN_Msk                              (0x01 << MMW_UP_RD_EFUSE_EN_Pos)
#define MMW_UP_RD_EFUSE_EN_Pos                              2
#define MMW_UP_RD_EFUSE_EN_PosL                             2
#define MMW_UP_RD_EFUSE_EN_PosH                             2

/*!< 0x006c PMU_EFUSE_CTRL
*null
*/
#define SYS1_UP_RD_EFUSE_EN_Msk                             (0x01 << SYS1_UP_RD_EFUSE_EN_Pos)
#define SYS1_UP_RD_EFUSE_EN_Pos                             1
#define SYS1_UP_RD_EFUSE_EN_PosL                            1
#define SYS1_UP_RD_EFUSE_EN_PosH                            1

/*!< 0x006c PMU_EFUSE_CTRL
*null
*/
#define SYS0_UP_RD_EFUSE_EN_Msk                             (0x01 << SYS0_UP_RD_EFUSE_EN_Pos)
#define SYS0_UP_RD_EFUSE_EN_Pos                             0
#define SYS0_UP_RD_EFUSE_EN_PosL                            0
#define SYS0_UP_RD_EFUSE_EN_PosH                            0

/*!< 0x0080 PMU_ANA_BG_CTRL
*null
*/
#define BG_STATUS_Msk                                       (0x01 << BG_STATUS_Pos)
#define BG_STATUS_Pos                                       19
#define BG_STATUS_PosL                                      19
#define BG_STATUS_PosH                                      19

/*!< 0x0080 PMU_ANA_BG_CTRL
*null
*/
#define FORCE_BG_ON_Msk                                     (0x01 << FORCE_BG_ON_Pos)
#define FORCE_BG_ON_Pos                                     18
#define FORCE_BG_ON_PosL                                    18
#define FORCE_BG_ON_PosH                                    18

/*!< 0x0080 PMU_ANA_BG_CTRL
*null
*/
#define V2I_IREF_FS_PD_Msk                                  (0x01 << V2I_IREF_FS_PD_Pos)
#define V2I_IREF_FS_PD_Pos                                  17
#define V2I_IREF_FS_PD_PosL                                 17
#define V2I_IREF_FS_PD_PosH                                 17

/*!< 0x0080 PMU_ANA_BG_CTRL
*null
*/
#define BG_FS_PD_Msk                                        (0x01 << BG_FS_PD_Pos)
#define BG_FS_PD_Pos                                        16
#define BG_FS_PD_PosL                                       16
#define BG_FS_PD_PosH                                       16

/*!< 0x0080 PMU_ANA_BG_CTRL
*null
*/
#define V2I_IREF_ITRIM_Msk                                  (0x3F << V2I_IREF_ITRIM_Pos)
#define V2I_IREF_ITRIM_Pos                                  10
#define V2I_IREF_ITRIM_PosL                                 10
#define V2I_IREF_ITRIM_PosH                                 15

/*!< 0x0080 PMU_ANA_BG_CTRL
*null
*/
#define BGUP_SETTINGS_Msk                                   (0x7F << BGUP_SETTINGS_Pos)
#define BGUP_SETTINGS_Pos                                   0
#define BGUP_SETTINGS_PosL                                  0
#define BGUP_SETTINGS_PosH                                  6


/*!< 0x008C PMU_SRAMA_SHARE
*null
*/
#define SRAMA_MMW_SHARE_EN_Msk                              (0x01 << SRAMA_MMW_SHARE_EN_Pos)
#define SRAMA_MMW_SHARE_EN_Pos                              0
#define SRAMA_MMW_SHARE_EN_PosL                             0
#define SRAMA_MMW_SHARE_EN_PosH                             0


/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_CHIRP_MODE_SEL_Msk                            (0x01 << CK_XO_CHIRP_MODE_SEL_Pos)
#define CK_XO_CHIRP_MODE_SEL_Pos                            19
#define CK_XO_CHIRP_MODE_SEL_PosL                           19
#define CK_XO_CHIRP_MODE_SEL_PosH                           19

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define XO_FREQ_SEL_Msk                                     (0x01 << XO_FREQ_SEL_Pos)
#define XO_FREQ_SEL_Pos                                     18
#define XO_FREQ_SEL_PosL                                    18
#define XO_FREQ_SEL_PosH                                    18

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define DCXO_LDO_LQ_Msk                                     (0x01 << DCXO_LDO_LQ_Pos)
#define DCXO_LDO_LQ_Pos                                     13
#define DCXO_LDO_LQ_PosL                                    13
#define DCXO_LDO_LQ_PosH                                    13

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_IO_FANOUT_EN_Msk                              (0x01 << CK_XO_IO_FANOUT_EN_Pos)
#define CK_XO_IO_FANOUT_EN_Pos                              12
#define CK_XO_IO_FANOUT_EN_PosL                             12
#define CK_XO_IO_FANOUT_EN_PosH                             12

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_M50_ANTI_EMI_EN_Msk                           (0x01 << CK_XO_M50_ANTI_EMI_EN_Pos)
#define CK_XO_M50_ANTI_EMI_EN_Pos                           11
#define CK_XO_M50_ANTI_EMI_EN_PosL                          11
#define CK_XO_M50_ANTI_EMI_EN_PosH                          11

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_M50_EN_Msk                                    (0x01 << CK_XO_M50_EN_Pos)
#define CK_XO_M50_EN_Pos                                    10
#define CK_XO_M50_EN_PosL                                   10
#define CK_XO_M50_EN_PosH                                   10

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_SOC_ANTI_EMI_EN_Msk                           (0x01 << CK_XO_SOC_ANTI_EMI_EN_Pos)
#define CK_XO_SOC_ANTI_EMI_EN_Pos                           9
#define CK_XO_SOC_ANTI_EMI_EN_PosL                          9
#define CK_XO_SOC_ANTI_EMI_EN_PosH                          9

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_SOC_EN_Msk                                    (0x01 << CK_XO_SOC_EN_Pos)
#define CK_XO_SOC_EN_Pos                                    8
#define CK_XO_SOC_EN_PosL                                   8
#define CK_XO_SOC_EN_PosH                                   8

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_SPLL_ANTI_EMI_EN_Msk                          (0x01 << CK_XO_SPLL_ANTI_EMI_EN_Pos)
#define CK_XO_SPLL_ANTI_EMI_EN_Pos                          7
#define CK_XO_SPLL_ANTI_EMI_EN_PosL                         7
#define CK_XO_SPLL_ANTI_EMI_EN_PosH                         7

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define CK_XO_SPLL_EN_Msk                                   (0x01 << CK_XO_SPLL_EN_Pos)
#define CK_XO_SPLL_EN_Pos                                   6
#define CK_XO_SPLL_EN_PosL                                  6
#define CK_XO_SPLL_EN_PosH                                  6

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define DCXO_LDO_BYPASS_EN_Msk                              (0x01 << DCXO_LDO_BYPASS_EN_Pos)
#define DCXO_LDO_BYPASS_EN_Pos                              5
#define DCXO_LDO_BYPASS_EN_PosL                             5
#define DCXO_LDO_BYPASS_EN_PosH                             5

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define DCXO_LDO_TEST_EN_Msk                                (0x01 << DCXO_LDO_TEST_EN_Pos)
#define DCXO_LDO_TEST_EN_Pos                                4
#define DCXO_LDO_TEST_EN_PosL                               4
#define DCXO_LDO_TEST_EN_PosH                               4

/*!< 0x0090 PMU_DCXO_CTRL
*null
*/
#define DCXO_LDO_TRIM_Msk                                   (0x0F << DCXO_LDO_TRIM_Pos)
#define DCXO_LDO_TRIM_Pos                                   0
#define DCXO_LDO_TRIM_PosL                                  0
#define DCXO_LDO_TRIM_PosH                                  3


/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define DCXO_CAP_TRIM_Msk                                   (0xFF << DCXO_CAP_TRIM_Pos)
#define DCXO_CAP_TRIM_Pos                                   24
#define DCXO_CAP_TRIM_PosL                                  24
#define DCXO_CAP_TRIM_PosH                                  31

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define DCXO_AMC_VTRIM_Msk                                  (0x0F << DCXO_AMC_VTRIM_Pos)
#define DCXO_AMC_VTRIM_Pos                                  20
#define DCXO_AMC_VTRIM_PosL                                 20
#define DCXO_AMC_VTRIM_PosH                                 23

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define DCXO_ITRIM_Msk                                      (0x0F << DCXO_ITRIM_Pos)
#define DCXO_ITRIM_Pos                                      16
#define DCXO_ITRIM_PosL                                     16
#define DCXO_ITRIM_PosH                                     19

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define DCXO_AMC_ST_Msk                                     (0x03 << DCXO_AMC_ST_Pos)
#define DCXO_AMC_ST_Pos                                     14
#define DCXO_AMC_ST_PosL                                    14
#define DCXO_AMC_ST_PosH                                    15

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define XTAL_TYPE_OVR_Msk                                   (0x01 << XTAL_TYPE_OVR_Pos)
#define XTAL_TYPE_OVR_Pos                                   4
#define XTAL_TYPE_OVR_PosL                                  4
#define XTAL_TYPE_OVR_PosH                                  4

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define XTAL_TYPE_OVR_EN_Msk                                (0x01 << XTAL_TYPE_OVR_EN_Pos)
#define XTAL_TYPE_OVR_EN_Pos                                3
#define XTAL_TYPE_OVR_EN_PosL                               3
#define XTAL_TYPE_OVR_EN_PosH                               3

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define XTAL_TYPE_DETECT_EN_Msk                             (0x01 << XTAL_TYPE_DETECT_EN_Pos)
#define XTAL_TYPE_DETECT_EN_Pos                             2
#define XTAL_TYPE_DETECT_EN_PosL                            2
#define XTAL_TYPE_DETECT_EN_PosH                            2

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define DCXO_ITRIM_BYPASS_Msk                               (0x01 << DCXO_ITRIM_BYPASS_Pos)
#define DCXO_ITRIM_BYPASS_Pos                               1
#define DCXO_ITRIM_BYPASS_PosL                              1
#define DCXO_ITRIM_BYPASS_PosH                              1

/*!< 0x0094 PMU_DCXO_CTRL3
*null
*/
#define DCXO_AMC_EN_Msk                                     (0x01 << DCXO_AMC_EN_Pos)
#define DCXO_AMC_EN_Pos                                     0
#define DCXO_AMC_EN_PosL                                    0
#define DCXO_AMC_EN_PosH                                    0


/*!< 0x00A0 PMU_SPLL_CTRL
*null
*/
#define SPLL_LDO_TEST_Msk                                   (0x01 << SPLL_LDO_TEST_Pos)
#define SPLL_LDO_TEST_Pos                                   4
#define SPLL_LDO_TEST_PosL                                  4
#define SPLL_LDO_TEST_PosH                                  4

/*!< 0x00A0 PMU_SPLL_CTRL
*null
*/
#define SPLL_LDO_TRIM_Msk                                   (0x0F << SPLL_LDO_TRIM_Pos)
#define SPLL_LDO_TRIM_Pos                                   0
#define SPLL_LDO_TRIM_PosL                                  0
#define SPLL_LDO_TRIM_PosH                                  3


/*!< 0x00AC PMU_M50_CTRL
*null
*/
#define LVLSHPD_POLARITY_Msk                                (0x01 << LVLSHPD_POLARITY_Pos)
#define LVLSHPD_POLARITY_Pos                                3
#define LVLSHPD_POLARITY_PosL                               3
#define LVLSHPD_POLARITY_PosH                               3

/*!< 0x00AC PMU_M50_CTRL
*null
*/
#define FORCE_LVLSHPD_Msk                                   (0x01 << FORCE_LVLSHPD_Pos)
#define FORCE_LVLSHPD_Pos                                   2
#define FORCE_LVLSHPD_PosL                                  2
#define FORCE_LVLSHPD_PosH                                  2

/*!< 0x00AC PMU_M50_CTRL
*null
*/
#define LVLSHPD_SRC_SEL_Msk                                 (0x03 << LVLSHPD_SRC_SEL_Pos)
#define LVLSHPD_SRC_SEL_Pos                                 0
#define LVLSHPD_SRC_SEL_PosL                                0
#define LVLSHPD_SRC_SEL_PosH                                1


/*!< 0x00b0 PMU_RC_CTRL
*null
*/
#define SLP_RCMSI_OFF_Msk                                   (0x01 << SLP_RCMSI_OFF_Pos)
#define SLP_RCMSI_OFF_Pos                                   2
#define SLP_RCMSI_OFF_PosL                                  2
#define SLP_RCMSI_OFF_PosH                                  2

/*!< 0x00b0 PMU_RC_CTRL
*null
*/
#define RCMSI_READY_Msk                                     (0x01 << RCMSI_READY_Pos)
#define RCMSI_READY_Pos                                     1
#define RCMSI_READY_PosL                                    1
#define RCMSI_READY_PosH                                    1

/*!< 0x00b0 PMU_RC_CTRL
*null
*/
#define RCMSI_EN_Msk                                        (0x01 << RCMSI_EN_Pos)
#define RCMSI_EN_Pos                                        0
#define RCMSI_EN_PosL                                       0
#define RCMSI_EN_PosH                                       0


/*!< 0x00b4 PMU_FORCE_WUP_CTRL
*null
*/
#define FORCE_WUP_SYS1_Msk                                  (0x01 << FORCE_WUP_SYS1_Pos)
#define FORCE_WUP_SYS1_Pos                                  1
#define FORCE_WUP_SYS1_PosL                                 1
#define FORCE_WUP_SYS1_PosH                                 1

/*!< 0x00b4 PMU_FORCE_WUP_CTRL
*null
*/
#define FORCE_WUP_SYS0_Msk                                  (0x01 << FORCE_WUP_SYS0_Pos)
#define FORCE_WUP_SYS0_Pos                                  0
#define FORCE_WUP_SYS0_PosL                                 0
#define FORCE_WUP_SYS0_PosH                                 0


/*!< 0x00b8 PMU_FORCE_DOWN_CTRL
*null
*/
#define FORCE_DWN_SYS1_Msk                                  (0x01 << FORCE_DWN_SYS1_Pos)
#define FORCE_DWN_SYS1_Pos                                  1
#define FORCE_DWN_SYS1_PosL                                 1
#define FORCE_DWN_SYS1_PosH                                 1

/*!< 0x00b8 PMU_FORCE_DOWN_CTRL
*null
*/
#define FORCE_DWN_SYS0_Msk                                  (0x01 << FORCE_DWN_SYS0_Pos)
#define FORCE_DWN_SYS0_Pos                                  0
#define FORCE_DWN_SYS0_PosL                                 0
#define FORCE_DWN_SYS0_PosH                                 0


/*!< 0x00C0 SYS0_SRAM_SW_CTRL
*null
*/
#define SRAMA0_RET_EN_Msk                                   (0x3FF << SRAMA0_RET_EN_Pos)
#define SRAMA0_RET_EN_Pos                                   16
#define SRAMA0_RET_EN_PosL                                  16
#define SRAMA0_RET_EN_PosH                                  25


/*!< 0x00c4 SYS1_SRAM_SW_CTRL
*null
*/
#define SRAMA0_RET_EN_Msk                                   (0x3FF << SRAMA0_RET_EN_Pos)
#define SRAMA0_RET_EN_Pos                                   16
#define SRAMA0_RET_EN_PosL                                  16
#define SRAMA0_RET_EN_PosH                                  25


/*!< 0x00D0 DCDC_PUP_OUT_CTRL
*null
*/
#define DCDC_PUP_OUT_FORCE_Msk                              (0x01 << DCDC_PUP_OUT_FORCE_Pos)
#define DCDC_PUP_OUT_FORCE_Pos                              16
#define DCDC_PUP_OUT_FORCE_PosL                             16
#define DCDC_PUP_OUT_FORCE_PosH                             16

/*!< 0x00D0 DCDC_PUP_OUT_CTRL
*null
*/
#define DCDC_PUP_OUT_SRC_SEL_Msk                            (0x07 << DCDC_PUP_OUT_SRC_SEL_Pos)
#define DCDC_PUP_OUT_SRC_SEL_Pos                            8
#define DCDC_PUP_OUT_SRC_SEL_PosL                           8
#define DCDC_PUP_OUT_SRC_SEL_PosH                           10

/*!< 0x00D0 DCDC_PUP_OUT_CTRL
*null
*/
#define DCDC_PUP_OUT_PORT_SEL_Msk                           (0x01 << DCDC_PUP_OUT_PORT_SEL_Pos)
#define DCDC_PUP_OUT_PORT_SEL_Pos                           1
#define DCDC_PUP_OUT_PORT_SEL_PosL                          1
#define DCDC_PUP_OUT_PORT_SEL_PosH                          1

/*!< 0x00D0 DCDC_PUP_OUT_CTRL
*null
*/
#define DCDC_PUP_OUT_EN_Msk                                 (0x01 << DCDC_PUP_OUT_EN_Pos)
#define DCDC_PUP_OUT_EN_Pos                                 0
#define DCDC_PUP_OUT_EN_PosL                                0
#define DCDC_PUP_OUT_EN_PosH                                0


/*!< 0x00d4 PMU_TEST_CTRL
*null
*/
#define TEST_SEL_Msk                                        (0x1F << TEST_SEL_Pos)
#define TEST_SEL_Pos                                        6
#define TEST_SEL_PosL                                       6
#define TEST_SEL_PosH                                       10

/*!< 0x00d4 PMU_TEST_CTRL
*null
*/
#define LDO_SW_MODE_TRIM_Msk                                (0x03 << LDO_SW_MODE_TRIM_Pos)
#define LDO_SW_MODE_TRIM_Pos                                5
#define LDO_SW_MODE_TRIM_PosL                               4
#define LDO_SW_MODE_TRIM_PosH                               5

/*!< 0x00d4 PMU_TEST_CTRL
*null
*/
#define TEST_VREF_Msk                                       (0x01 << TEST_VREF_Pos)
#define TEST_VREF_Pos                                       3
#define TEST_VREF_PosL                                      3
#define TEST_VREF_PosH                                      3

/*!< 0x00d4 PMU_TEST_CTRL
*null
*/
#define TEST_IREF_Msk                                       (0x01 << TEST_IREF_Pos)
#define TEST_IREF_Pos                                       2
#define TEST_IREF_PosL                                      2
#define TEST_IREF_PosH                                      2


/*!< 0x00DC LDO_ON_MODE_SEL
*null
*/
#define LDO_ON_MODE_SEL_Msk                                 (0x01 << LDO_ON_MODE_SEL_Pos)
#define LDO_ON_MODE_SEL_Pos                                 0
#define LDO_ON_MODE_SEL_PosL                                0
#define LDO_ON_MODE_SEL_PosH                                0



/*!< 0x00e4 PMU_SYS0_LPMS
*null
*/
#define SYS0_LPMS_Msk                                       (0x07 << SYS0_LPMS_Pos)
#define SYS0_LPMS_Pos                                       0
#define SYS0_LPMS_PosL                                      0
#define SYS0_LPMS_PosH                                      2


/*!< 0x00e8 PMU_SYS1_LPMS
*null
*/
#define SYS1_LPMS_Msk                                       (0x07 << SYS1_LPMS_Pos)
#define SYS1_LPMS_Pos                                       0
#define SYS1_LPMS_PosL                                      0
#define SYS1_LPMS_PosH                                      2


/*!< 0x00ec PMU_MMW_LPMS
*null
*/
#define MMW_LPMS_Msk                                        (0x07 << MMW_LPMS_Pos)
#define MMW_LPMS_Pos                                        0
#define MMW_LPMS_PosL                                       0
#define MMW_LPMS_PosH                                       2

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SPLL_LOCKED_Msk                                     (0x01 << SPLL_LOCKED_Pos)
#define SPLL_LOCKED_Pos                            		    29
#define SPLL_LOCKED_PosL                                    29
#define SPLL_LOCKED_PosH                                    29

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SPLLLDO_READY_Msk                                   (0x01 << SPLLLDO_READY_Pos)
#define SPLLLDO_READY_Pos                            		28
#define SPLLLDO_READY_PosL                                  28
#define SPLLLDO_READY_PosH                                  28

/*!< 0x00Fc PMU_STATUS
*null
*/
#define XO_AMC_ITRIM_CAL_Msk                                (0x0F << XO_AMC_ITRIM_CAL_Pos)
#define XO_AMC_ITRIM_CAL_Pos                          		19
#define XO_AMC_ITRIM_CAL_PosL                               19
#define XO_AMC_ITRIM_CAL_PosH                               19


/*!< 0x00Fc PMU_STATUS
*null
*/
#define XO_AMC_LOCK_Msk                                		(0x01 << XO_AMC_LOCK_Pos)
#define XO_AMC_LOCK_Pos                                		19
#define XO_AMC_LOCK_PoL                                     19
#define XO_AMC_LOCK_PosH                                    19


/*!< 0x00Fc PMU_STATUS
*null
*/
#define XTAL_TYPE_DETECT_Msk                                (0x01 << XTAL_TYPE_DETECT_Pos)
#define XTAL_TYPE_DETECT_Pos                                18
#define XTAL_TYPE_DETECT_PosL                               18
#define XTAL_TYPE_DETECT_PosH                               18


/*!< 0x00Fc PMU_STATUS
*null
*/
#define DCXO_READY_Msk                                      (0x01 << DCXO_READY_Pos)
#define DCXO_READY_Pos                                      17
#define DCXO_READY_PosL                                     17
#define DCXO_READY_PosH                                     17

/*!< 0x00Fc PMU_STATUS
*null
*/
#define BOOTSEL_Msk                                        (0x01 << BOOTSEL_Pos)
#define BOOTSEL_Pos                                        9
#define BOOTSEL_PosL                                       9
#define BOOTSEL_PosH                                       9

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SYS1_POWERED_DWN_Msk                                (0x01 << SYS1_POWERED_DWN_Pos)
#define SYS1_POWERED_DWN_Pos                                8
#define SYS1_POWERED_DWN_PosL                               8
#define SYS1_POWERED_DWN_PosH                               8

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SYS0_POWERED_DWN_Msk                                (0x01 << SYS0_POWERED_DWN_Pos)
#define SYS0_POWERED_DWN_Pos                                7
#define SYS0_POWERED_DWN_PosL                               7
#define SYS0_POWERED_DWN_PosH                               7

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SYS1_POWERED_UP_Msk                                 (0x01 << SYS1_POWERED_UP_Pos)
#define SYS1_POWERED_UP_Pos                                 6
#define SYS1_POWERED_UP_PosL                                6
#define SYS1_POWERED_UP_PosH                                6

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SYS0_POWERED_UP_Msk                                 (0x01 << SYS0_POWERED_UP_Pos)
#define SYS0_POWERED_UP_Pos                                 5
#define SYS0_POWERED_UP_PosL                                5
#define SYS0_POWERED_UP_PosH                                5

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SYS1_SW_EN_Msk                                 		(0x01 << SYS1_SW_EN_Pos)
#define SYS1_SW_EN_Pos                                      4
#define SYS1_SW_EN_PosL                                     4
#define SYS1_SW_EN_PosH                                     4

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SYS0_SW_EN_Msk                                 		(0x01 << SYS0_SW_EN_Pos)
#define SYS0_SW_EN_Pos                                      3
#define SYS0_SW_EN_PosL                                     3
#define SYS0_SW_EN_PosH                                     3

/*!< 0x00Fc PMU_STATUS
*null
*/
#define RTCSW2_READY_Msk                                    (0x01 << RTCSW2_READY_Pos)
#define RTCSW2_READY_Pos                                    2
#define RTCSW2_READY_PosL                                   2
#define RTCSW2_READY_PosH                                   2

/*!< 0x00Fc PMU_STATUS
*null
*/
#define SW1_READY_Msk                                    	(0x01 << SW1_READY_Pos)
#define SW1_READY_Pos                                    	1
#define SW1_READY_PosL                                      1
#define SW1_READY_PosH                                      1

/*!< 0x00Fc PMU_STATUS
*null
*/
#define RTCSW1_READY_Msk                                    (0x01 << RTCSW1_READY_Pos)
#define RTCSW1_READY_Pos                                    0
#define RTCSW1_READY_PosL                                   0
#define RTCSW1_READY_PosH                                   0

/*!< 0x00Fc PMU_STATUS
*null
*/
#define DCXOLDO_READY_Msk                                   (0x01 << DCXOLDO_READY_Pos)
#define DCXOLDO_READY_Pos                                   16
#define DCXOLDO_READY_PosL                                  16
#define DCXOLDO_READY_PosH                                  16


/*!< 0x0100 PMU_HBACK
*null
*/
#define PMU_HARD_BACK_Msk                                   (0XFFFFFFF << PMU_HARD_BACK_Pos)
#define PMU_HARD_BACK_Pos                                   0
#define PMU_HARD_BACK_PosL                                  0
#define PMU_HARD_BACK_PosH                                  31

/*!< 0x0100 PMU_HBACK
*null
*/
#define PMU_HBACK_SW_FAST_UP_Msk                            (0x01 << PMU_HBACK_SW_FAST_UP_Pos)
#define PMU_HBACK_SW_FAST_UP_Pos                            0
#define PMU_HBACK_SW_FAST_UP_PosL                           0
#define PMU_HBACK_SW_FAST_UP_PosH                           0

/*!< 0x0100 PMU_HBACK
*null
*/
#define PMU_HBACK_DCXO_FAST_DWN_Msk                         (0x01 << PMU_HBACK_DCXO_FAST_DWN_Pos)
#define PMU_HBACK_DCXO_FAST_DWN_Pos                         1
#define PMU_HBACK_DCXO_FAST_DWN_PosL                        1
#define PMU_HBACK_DCXO_FAST_DWN_PosH                        1

/*!< 0x0100 PMU_HBACK
*null
*/
#define PMU_HBACK_PLL_FAST_DWN_Msk                          (0x01 << PMU_HBACK_PLL_FAST_DWN_Pos)
#define PMU_HBACK_PLL_FAST_DWN_Pos                          2
#define PMU_HBACK_PLL_FAST_DWN_PosL                         2
#define PMU_HBACK_PLL_FAST_DWN_PosH                         2

/*!< 0x0100 PMU_HBACK
*null
*/
#define PMU_HBACK_CKIN_32K_SRC_Msk                          (0x01 << PMU_HBACK_CKIN_32K_SRC_Pos)
#define PMU_HBACK_CKIN_32K_SRC_Pos                          4
#define PMU_HBACK_CKIN_32K_SRC_PosL                         4
#define PMU_HBACK_CKIN_32K_SRC_PosH                         4


/*!< 0x0100 PMU_HBACK
*null
*/
#define PMU_HBACK_RTC_IO_ENABLE_Msk                          (0x3FFFF << PMU_HBACK_RTC_IO_ENABLE_Pos)
#define PMU_HBACK_RTC_IO_ENABLE_Pos                          14
#define PMU_HBACK_RTC_IO_ENABLE_PosL                         14
#define PMU_HBACK_RTC_IO_ENABLE_PosH                         31

/*!< 0x0110 PMU_TSENSOR_MASK_CTRL
*null
*/
#define TSENSOR_MASK_MDT_Msk                                (0x01 << TSENSOR_MASK_MDT_Pos)
#define TSENSOR_MASK_MDT_Pos                                1
#define TSENSOR_MASK_MDT_PosL                               1
#define TSENSOR_MASK_MDT_PosH                               1

/*!< 0x0110 PMU_TSENSOR_MASK_CTRL
*null
*/
#define TSENSOR_MASK_MTP_Msk                                (0x01 << TSENSOR_MASK_MTP_Pos)
#define TSENSOR_MASK_MTP_Pos                                0
#define TSENSOR_MASK_MTP_PosL                               0
#define TSENSOR_MASK_MTP_PosH                               0


/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_TMR_CMP1_WUP_MASK_Msk                          (0x01 << SYS0_TMR_CMP1_WUP_MASK_Pos)
#define SYS0_TMR_CMP1_WUP_MASK_Pos                          17
#define SYS0_TMR_CMP1_WUP_MASK_PosL                         17
#define SYS0_TMR_CMP1_WUP_MASK_PosH                         17

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_TMR_CMP0_WUP_MASK_Msk                          (0x01 << SYS0_TMR_CMP0_WUP_MASK_Pos)
#define SYS0_TMR_CMP0_WUP_MASK_Pos                          16
#define SYS0_TMR_CMP0_WUP_MASK_PosL                         16
#define SYS0_TMR_CMP0_WUP_MASK_PosH                         16

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_UART_WUP_MASK_Msk                              (0x01 << SYS0_UART_WUP_MASK_Pos)
#define SYS0_UART_WUP_MASK_Pos                              15
#define SYS0_UART_WUP_MASK_PosL                             15
#define SYS0_UART_WUP_MASK_PosH                             15

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_IIC_WUP_MASK_Msk                               (0x01 << SYS0_IIC_WUP_MASK_Pos)
#define SYS0_IIC_WUP_MASK_Pos                               14
#define SYS0_IIC_WUP_MASK_PosL                              14
#define SYS0_IIC_WUP_MASK_PosH                              14

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_SPI_WUP_MASK_Msk                               (0x01 << SYS0_SPI_WUP_MASK_Pos)
#define SYS0_SPI_WUP_MASK_Pos                               13
#define SYS0_SPI_WUP_MASK_PosL                              13
#define SYS0_SPI_WUP_MASK_PosH                              13

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_RTT1_INT_MASK_Msk                              (0x01 << SYS0_RTT1_INT_MASK_Pos)
#define SYS0_RTT1_INT_MASK_Pos                              12
#define SYS0_RTT1_INT_MASK_PosL                             12
#define SYS0_RTT1_INT_MASK_PosH                             12

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_INTEREND_INT_MASK_Msk                          (0x01 << SYS0_INTEREND_INT_MASK_Pos)
#define SYS0_INTEREND_INT_MASK_Pos                          11
#define SYS0_INTEREND_INT_MASK_PosL                         11
#define SYS0_INTEREND_INT_MASK_PosH                         11

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_TSENSOR_INT_MASK_Msk                           (0x01 << SYS0_TSENSOR_INT_MASK_Pos)
#define SYS0_TSENSOR_INT_MASK_Pos                           10
#define SYS0_TSENSOR_INT_MASK_PosL                          10
#define SYS0_TSENSOR_INT_MASK_PosH                          10

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_MMWDBGEND_INT_MASK_Msk                         (0x01 << SYS0_MMWDBGEND_INT_MASK_Pos)
#define SYS0_MMWDBGEND_INT_MASK_Pos                         9
#define SYS0_MMWDBGEND_INT_MASK_PosL                        9
#define SYS0_MMWDBGEND_INT_MASK_PosH                        9

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_RTCSW2_CMP_INT_MASK_Msk                        (0x01 << SYS0_RTCSW2_CMP_INT_MASK_Pos)
#define SYS0_RTCSW2_CMP_INT_MASK_Pos                        8
#define SYS0_RTCSW2_CMP_INT_MASK_PosL                       8
#define SYS0_RTCSW2_CMP_INT_MASK_PosH                       8

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_MTP_NEG_INT_MASK_Msk                           (0x01 << SYS0_MTP_NEG_INT_MASK_Pos)
#define SYS0_MTP_NEG_INT_MASK_Pos                           7
#define SYS0_MTP_NEG_INT_MASK_PosL                          7
#define SYS0_MTP_NEG_INT_MASK_PosH                          7

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_MDT_INT_MASK_Msk                               (0x01 << SYS0_MDT_INT_MASK_Pos)
#define SYS0_MDT_INT_MASK_Pos                               6
#define SYS0_MDT_INT_MASK_PosL                              6
#define SYS0_MDT_INT_MASK_PosH                              6

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_2DFFT_INT_MASK_Msk                             (0x01 << SYS0_2DFFT_INT_MASK_Pos)
#define SYS0_2DFFT_INT_MASK_Pos                             5
#define SYS0_2DFFT_INT_MASK_PosL                            5
#define SYS0_2DFFT_INT_MASK_PosH                            5

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_MTP_INT_MASK_Msk                               (0x01 << SYS0_MTP_INT_MASK_Pos)
#define SYS0_MTP_INT_MASK_Pos                               4
#define SYS0_MTP_INT_MASK_PosL                              4
#define SYS0_MTP_INT_MASK_PosH                              4

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_M50_INT_MASK_Msk                               (0x01 << SYS0_M50_INT_MASK_Pos)
#define SYS0_M50_INT_MASK_Pos                               3
#define SYS0_M50_INT_MASK_PosL                              3
#define SYS0_M50_INT_MASK_PosH                              3

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_WUPIO_INT_MASK_Msk                             (0x01 << SYS0_WUPIO_INT_MASK_Pos)
#define SYS0_WUPIO_INT_MASK_Pos                             2
#define SYS0_WUPIO_INT_MASK_PosL                            2
#define SYS0_WUPIO_INT_MASK_PosH                            2

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_RTC_INT_MASK_Msk                               (0x01 << SYS0_RTC_INT_MASK_Pos)
#define SYS0_RTC_INT_MASK_Pos                               1
#define SYS0_RTC_INT_MASK_PosL                              1
#define SYS0_RTC_INT_MASK_PosH                              1

/*!< 0x0114 PMU_SYS0_WAKEUP_C_MASK
*null
*/
#define SYS0_RTT_INT_MASK_Msk                               (0x01 << SYS0_RTT_INT_MASK_Pos)
#define SYS0_RTT_INT_MASK_Pos                               0
#define SYS0_RTT_INT_MASK_PosL                              0
#define SYS0_RTT_INT_MASK_PosH                              0


/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_CPUSDIS_WUP_MASK_Msk                           (0x01 << SYS1_CPUSDIS_WUP_MASK_Pos)
#define SYS1_CPUSDIS_WUP_MASK_Pos                           18
#define SYS1_CPUSDIS_WUP_MASK_PosL                          18
#define SYS1_CPUSDIS_WUP_MASK_PosH                          18

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_TMR_CMP1_WUP_MASK_Msk                          (0x01 << SYS1_TMR_CMP1_WUP_MASK_Pos)
#define SYS1_TMR_CMP1_WUP_MASK_Pos                          17
#define SYS1_TMR_CMP1_WUP_MASK_PosL                         17
#define SYS1_TMR_CMP1_WUP_MASK_PosH                         17

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_TMR_CMP0_WUP_MASK_Msk                          (0x01 << SYS1_TMR_CMP0_WUP_MASK_Pos)
#define SYS1_TMR_CMP0_WUP_MASK_Pos                          16
#define SYS1_TMR_CMP0_WUP_MASK_PosL                         16
#define SYS1_TMR_CMP0_WUP_MASK_PosH                         16

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_UART_WUP_MASK_Msk                              (0x01 << SYS1_UART_WUP_MASK_Pos)
#define SYS1_UART_WUP_MASK_Pos                              15
#define SYS1_UART_WUP_MASK_PosL                             15
#define SYS1_UART_WUP_MASK_PosH                             15

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_IIC_WUP_MASK_Msk                               (0x01 << SYS1_IIC_WUP_MASK_Pos)
#define SYS1_IIC_WUP_MASK_Pos                               14
#define SYS1_IIC_WUP_MASK_PosL                              14
#define SYS1_IIC_WUP_MASK_PosH                              14

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_SPI_WUP_MASK_Msk                               (0x01 << SYS1_SPI_WUP_MASK_Pos)
#define SYS1_SPI_WUP_MASK_Pos                               13
#define SYS1_SPI_WUP_MASK_PosL                              13
#define SYS1_SPI_WUP_MASK_PosH                              13

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_RTT1_INT_MASK_Msk                              (0x01 << SYS1_RTT1_INT_MASK_Pos)
#define SYS1_RTT1_INT_MASK_Pos                              12
#define SYS1_RTT1_INT_MASK_PosL                             12
#define SYS1_RTT1_INT_MASK_PosH                             12

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_INTEREND_INT_MASK_Msk                          (0x01 << SYS1_INTEREND_INT_MASK_Pos)
#define SYS1_INTEREND_INT_MASK_Pos                          11
#define SYS1_INTEREND_INT_MASK_PosL                         11
#define SYS1_INTEREND_INT_MASK_PosH                         11

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_TSENSOR_INT_MASK_Msk                           (0x01 << SYS1_TSENSOR_INT_MASK_Pos)
#define SYS1_TSENSOR_INT_MASK_Pos                           10
#define SYS1_TSENSOR_INT_MASK_PosL                          10
#define SYS1_TSENSOR_INT_MASK_PosH                          10

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_MMWDBGEND_INT_MASK_Msk                         (0x01 << SYS1_MMWDBGEND_INT_MASK_Pos)
#define SYS1_MMWDBGEND_INT_MASK_Pos                         9
#define SYS1_MMWDBGEND_INT_MASK_PosL                        9
#define SYS1_MMWDBGEND_INT_MASK_PosH                        9

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_RTCSW2_CMP_INT_MASK_Msk                        (0x01 << SYS1_RTCSW2_CMP_INT_MASK_Pos)
#define SYS1_RTCSW2_CMP_INT_MASK_Pos                        8
#define SYS1_RTCSW2_CMP_INT_MASK_PosL                       8
#define SYS1_RTCSW2_CMP_INT_MASK_PosH                       8

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_MTP_NEG_INT_MASK_Msk                           (0x01 << SYS1_MTP_NEG_INT_MASK_Pos)
#define SYS1_MTP_NEG_INT_MASK_Pos                           7
#define SYS1_MTP_NEG_INT_MASK_PosL                          7
#define SYS1_MTP_NEG_INT_MASK_PosH                          7

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_MDT_INT_MASK_Msk                               (0x01 << SYS1_MDT_INT_MASK_Pos)
#define SYS1_MDT_INT_MASK_Pos                               6
#define SYS1_MDT_INT_MASK_PosL                              6
#define SYS1_MDT_INT_MASK_PosH                              6

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_2DFFT_INT_MASK_Msk                             (0x01 << SYS1_2DFFT_INT_MASK_Pos)
#define SYS1_2DFFT_INT_MASK_Pos                             5
#define SYS1_2DFFT_INT_MASK_PosL                            5
#define SYS1_2DFFT_INT_MASK_PosH                            5

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_MTP_INT_MASK_Msk                               (0x01 << SYS1_MTP_INT_MASK_Pos)
#define SYS1_MTP_INT_MASK_Pos                               4
#define SYS1_MTP_INT_MASK_PosL                              4
#define SYS1_MTP_INT_MASK_PosH                              4

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_M50_INT_MASK_Msk                               (0x01 << SYS1_M50_INT_MASK_Pos)
#define SYS1_M50_INT_MASK_Pos                               3
#define SYS1_M50_INT_MASK_PosL                              3
#define SYS1_M50_INT_MASK_PosH                              3

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_WUPIO_INT_MASK_Msk                             (0x01 << SYS1_WUPIO_INT_MASK_Pos)
#define SYS1_WUPIO_INT_MASK_Pos                             2
#define SYS1_WUPIO_INT_MASK_PosL                            2
#define SYS1_WUPIO_INT_MASK_PosH                            2

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_RTC_INT_MASK_Msk                               (0x01 << SYS1_RTC_INT_MASK_Pos)
#define SYS1_RTC_INT_MASK_Pos                               1
#define SYS1_RTC_INT_MASK_PosL                              1
#define SYS1_RTC_INT_MASK_PosH                              1

/*!< 0x0118 PMU_SYS1_WAKEUP_C_MASK
*null
*/
#define SYS1_RTT_INT_MASK_Msk                               (0x01 << SYS1_RTT_INT_MASK_Pos)
#define SYS1_RTT_INT_MASK_Pos                               0
#define SYS1_RTT_INT_MASK_PosL                              0
#define SYS1_RTT_INT_MASK_PosH                              0


/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_NEG_WUP_PREVENT_SPLL_DWN_EN_Msk                 (0x01 << MTP_NEG_WUP_PREVENT_SPLL_DWN_EN_Pos)
#define MTP_NEG_WUP_PREVENT_SPLL_DWN_EN_Pos                 28
#define MTP_NEG_WUP_PREVENT_SPLL_DWN_EN_PosL                28
#define MTP_NEG_WUP_PREVENT_SPLL_DWN_EN_PosH                28

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_NEG_WUP_PREVENT_DCXO_DWN_EN_Msk                 (0x01 << MTP_NEG_WUP_PREVENT_DCXO_DWN_EN_Pos)
#define MTP_NEG_WUP_PREVENT_DCXO_DWN_EN_Pos                 27
#define MTP_NEG_WUP_PREVENT_DCXO_DWN_EN_PosL                27
#define MTP_NEG_WUP_PREVENT_DCXO_DWN_EN_PosH                27

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_NEG_WUP_PREVENT_MSI_DWN_EN_Msk                  (0x01 << MTP_NEG_WUP_PREVENT_MSI_DWN_EN_Pos)
#define MTP_NEG_WUP_PREVENT_MSI_DWN_EN_Pos                  26
#define MTP_NEG_WUP_PREVENT_MSI_DWN_EN_PosL                 26
#define MTP_NEG_WUP_PREVENT_MSI_DWN_EN_PosH                 26

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_NEG_WUP_PREVENT_SOCLDO_DWN_EN_Msk               (0x01 << MTP_NEG_WUP_PREVENT_SOCLDO_DWN_EN_Pos)
#define MTP_NEG_WUP_PREVENT_SOCLDO_DWN_EN_Pos               25
#define MTP_NEG_WUP_PREVENT_SOCLDO_DWN_EN_PosL              25
#define MTP_NEG_WUP_PREVENT_SOCLDO_DWN_EN_PosH              25

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_NEG_WUP_PREVENT_INTLDO_DWN_EN_Msk               (0x01 << MTP_NEG_WUP_PREVENT_INTLDO_DWN_EN_Pos)
#define MTP_NEG_WUP_PREVENT_INTLDO_DWN_EN_Pos               24
#define MTP_NEG_WUP_PREVENT_INTLDO_DWN_EN_PosL              24
#define MTP_NEG_WUP_PREVENT_INTLDO_DWN_EN_PosH              24

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MDT_WUP_PREVENT_SPLL_DWN_EN_Msk                     (0x01 << MDT_WUP_PREVENT_SPLL_DWN_EN_Pos)
#define MDT_WUP_PREVENT_SPLL_DWN_EN_Pos                     20
#define MDT_WUP_PREVENT_SPLL_DWN_EN_PosL                    20
#define MDT_WUP_PREVENT_SPLL_DWN_EN_PosH                    20

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MDT_WUP_PREVENT_DCXO_DWN_EN_Msk                     (0x01 << MDT_WUP_PREVENT_DCXO_DWN_EN_Pos)
#define MDT_WUP_PREVENT_DCXO_DWN_EN_Pos                     19
#define MDT_WUP_PREVENT_DCXO_DWN_EN_PosL                    19
#define MDT_WUP_PREVENT_DCXO_DWN_EN_PosH                    19

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MDT_WUP_PREVENT_MSI_DWN_EN_Msk                      (0x01 << MDT_WUP_PREVENT_MSI_DWN_EN_Pos)
#define MDT_WUP_PREVENT_MSI_DWN_EN_Pos                      18
#define MDT_WUP_PREVENT_MSI_DWN_EN_PosL                     18
#define MDT_WUP_PREVENT_MSI_DWN_EN_PosH                     18

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MDT_WUP_PREVENT_SOCLDO_DWN_EN_Msk                   (0x01 << MDT_WUP_PREVENT_SOCLDO_DWN_EN_Pos)
#define MDT_WUP_PREVENT_SOCLDO_DWN_EN_Pos                   17
#define MDT_WUP_PREVENT_SOCLDO_DWN_EN_PosL                  17
#define MDT_WUP_PREVENT_SOCLDO_DWN_EN_PosH                  17

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MDT_WUP_PREVENT_INTLDO_DWN_EN_Msk                   (0x01 << MDT_WUP_PREVENT_INTLDO_DWN_EN_Pos)
#define MDT_WUP_PREVENT_INTLDO_DWN_EN_Pos                   16
#define MDT_WUP_PREVENT_INTLDO_DWN_EN_PosL                  16
#define MDT_WUP_PREVENT_INTLDO_DWN_EN_PosH                  16

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define FFT2D_WUP_PREVENT_SPLL_DWN_EN_Msk                   (0x01 << FFT2D_WUP_PREVENT_SPLL_DWN_EN_Pos)
#define FFT2D_WUP_PREVENT_SPLL_DWN_EN_Pos                   12
#define FFT2D_WUP_PREVENT_SPLL_DWN_EN_PosL                  12
#define FFT2D_WUP_PREVENT_SPLL_DWN_EN_PosH                  12

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define FFT2D_WUP_PREVENT_DCXO_DWN_EN_Msk                   (0x01 << FFT2D_WUP_PREVENT_DCXO_DWN_EN_Pos)
#define FFT2D_WUP_PREVENT_DCXO_DWN_EN_Pos                   11
#define FFT2D_WUP_PREVENT_DCXO_DWN_EN_PosL                  11
#define FFT2D_WUP_PREVENT_DCXO_DWN_EN_PosH                  11

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define FFT2D_WUP_PREVENT_MSI_DWN_EN_Msk                    (0x01 << FFT2D_WUP_PREVENT_MSI_DWN_EN_Pos)
#define FFT2D_WUP_PREVENT_MSI_DWN_EN_Pos                    10
#define FFT2D_WUP_PREVENT_MSI_DWN_EN_PosL                   10
#define FFT2D_WUP_PREVENT_MSI_DWN_EN_PosH                   10

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define FFT2D_WUP_PREVENT_SOCLDO_DWN_EN_Msk                 (0x01 << FFT2D_WUP_PREVENT_SOCLDO_DWN_EN_Pos)
#define FFT2D_WUP_PREVENT_SOCLDO_DWN_EN_Pos                 9
#define FFT2D_WUP_PREVENT_SOCLDO_DWN_EN_PosL                9
#define FFT2D_WUP_PREVENT_SOCLDO_DWN_EN_PosH                9

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define FFT2D_WUP_PREVENT_INTLDO_DWN_EN_Msk                 (0x01 << FFT2D_WUP_PREVENT_INTLDO_DWN_EN_Pos)
#define FFT2D_WUP_PREVENT_INTLDO_DWN_EN_Pos                 8
#define FFT2D_WUP_PREVENT_INTLDO_DWN_EN_PosL                8
#define FFT2D_WUP_PREVENT_INTLDO_DWN_EN_PosH                8

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_WUP_PREVENT_SPLL_DWN_EN_Msk                     (0x01 << MTP_WUP_PREVENT_SPLL_DWN_EN_Pos)
#define MTP_WUP_PREVENT_SPLL_DWN_EN_Pos                     4
#define MTP_WUP_PREVENT_SPLL_DWN_EN_PosL                    4
#define MTP_WUP_PREVENT_SPLL_DWN_EN_PosH                    4

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_WUP_PREVENT_DCXO_DWN_EN_Msk                     (0x01 << MTP_WUP_PREVENT_DCXO_DWN_EN_Pos)
#define MTP_WUP_PREVENT_DCXO_DWN_EN_Pos                     3
#define MTP_WUP_PREVENT_DCXO_DWN_EN_PosL                    3
#define MTP_WUP_PREVENT_DCXO_DWN_EN_PosH                    3

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_WUP_PREVENT_MSI_DWN_EN_Msk                      (0x01 << MTP_WUP_PREVENT_MSI_DWN_EN_Pos)
#define MTP_WUP_PREVENT_MSI_DWN_EN_Pos                      2
#define MTP_WUP_PREVENT_MSI_DWN_EN_PosL                     2
#define MTP_WUP_PREVENT_MSI_DWN_EN_PosH                     2

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_WUP_PREVENT_SOCLDO_DWN_EN_Msk                   (0x01 << MTP_WUP_PREVENT_SOCLDO_DWN_EN_Pos)
#define MTP_WUP_PREVENT_SOCLDO_DWN_EN_Pos                   1
#define MTP_WUP_PREVENT_SOCLDO_DWN_EN_PosL                  1
#define MTP_WUP_PREVENT_SOCLDO_DWN_EN_PosH                  1

/*!< 0x011c MMW_PREVENT_DWN0
*null
*/
#define MTP_WUP_PREVENT_INTLDO_DWN_EN_Msk                   (0x01 << MTP_WUP_PREVENT_INTLDO_DWN_EN_Pos)
#define MTP_WUP_PREVENT_INTLDO_DWN_EN_Pos                   0
#define MTP_WUP_PREVENT_INTLDO_DWN_EN_PosL                  0
#define MTP_WUP_PREVENT_INTLDO_DWN_EN_PosH                  0


/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define INTEREND_WUP_PREVENT_SPLL_DWN_EN_Msk                (0x01 << INTEREND_WUP_PREVENT_SPLL_DWN_EN_Pos)
#define INTEREND_WUP_PREVENT_SPLL_DWN_EN_Pos                20
#define INTEREND_WUP_PREVENT_SPLL_DWN_EN_PosL               20
#define INTEREND_WUP_PREVENT_SPLL_DWN_EN_PosH               20

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define INTEREND_WUP_PREVENT_DCXO_DWN_EN_Msk                (0x01 << INTEREND_WUP_PREVENT_DCXO_DWN_EN_Pos)
#define INTEREND_WUP_PREVENT_DCXO_DWN_EN_Pos                19
#define INTEREND_WUP_PREVENT_DCXO_DWN_EN_PosL               19
#define INTEREND_WUP_PREVENT_DCXO_DWN_EN_PosH               19

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define INTEREND_WUP_PREVENT_MSI_DWN_EN_Msk                 (0x01 << INTEREND_WUP_PREVENT_MSI_DWN_EN_Pos)
#define INTEREND_WUP_PREVENT_MSI_DWN_EN_Pos                 18
#define INTEREND_WUP_PREVENT_MSI_DWN_EN_PosL                18
#define INTEREND_WUP_PREVENT_MSI_DWN_EN_PosH                18

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define INTEREND_WUP_PREVENT_SOCLDO_DWN_EN_Msk              (0x01 << INTEREND_WUP_PREVENT_SOCLDO_DWN_EN_Pos)
#define INTEREND_WUP_PREVENT_SOCLDO_DWN_EN_Pos              17
#define INTEREND_WUP_PREVENT_SOCLDO_DWN_EN_PosL             17
#define INTEREND_WUP_PREVENT_SOCLDO_DWN_EN_PosH             17

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define INTEREND_WUP_PREVENT_INTLDO_DWN_EN_Msk              (0x01 << INTEREND_WUP_PREVENT_INTLDO_DWN_EN_Pos)
#define INTEREND_WUP_PREVENT_INTLDO_DWN_EN_Pos              16
#define INTEREND_WUP_PREVENT_INTLDO_DWN_EN_PosL             16
#define INTEREND_WUP_PREVENT_INTLDO_DWN_EN_PosH             16

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define TSENSOR_WUP_PREVENT_SPLL_DWN_EN_Msk                 (0x01 << TSENSOR_WUP_PREVENT_SPLL_DWN_EN_Pos)
#define TSENSOR_WUP_PREVENT_SPLL_DWN_EN_Pos                 12
#define TSENSOR_WUP_PREVENT_SPLL_DWN_EN_PosL                12
#define TSENSOR_WUP_PREVENT_SPLL_DWN_EN_PosH                12

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define TSENSOR_WUP_PREVENT_DCXO_DWN_EN_Msk                 (0x01 << TSENSOR_WUP_PREVENT_DCXO_DWN_EN_Pos)
#define TSENSOR_WUP_PREVENT_DCXO_DWN_EN_Pos                 11
#define TSENSOR_WUP_PREVENT_DCXO_DWN_EN_PosL                11
#define TSENSOR_WUP_PREVENT_DCXO_DWN_EN_PosH                11

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define TSENSOR_WUP_PREVENT_MSI_DWN_EN_Msk                  (0x01 << TSENSOR_WUP_PREVENT_MSI_DWN_EN_Pos)
#define TSENSOR_WUP_PREVENT_MSI_DWN_EN_Pos                  10
#define TSENSOR_WUP_PREVENT_MSI_DWN_EN_PosL                 10
#define TSENSOR_WUP_PREVENT_MSI_DWN_EN_PosH                 10

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define TSENSOR_WUP_PREVENT_SOCLDO_DWN_EN_Msk               (0x01 << TSENSOR_WUP_PREVENT_SOCLDO_DWN_EN_Pos)
#define TSENSOR_WUP_PREVENT_SOCLDO_DWN_EN_Pos               9
#define TSENSOR_WUP_PREVENT_SOCLDO_DWN_EN_PosL              9
#define TSENSOR_WUP_PREVENT_SOCLDO_DWN_EN_PosH              9

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define TSENSOR_WUP_PREVENT_INTLDO_DWN_EN_Msk               (0x01 << TSENSOR_WUP_PREVENT_INTLDO_DWN_EN_Pos)
#define TSENSOR_WUP_PREVENT_INTLDO_DWN_EN_Pos               8
#define TSENSOR_WUP_PREVENT_INTLDO_DWN_EN_PosL              8
#define TSENSOR_WUP_PREVENT_INTLDO_DWN_EN_PosH              8

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define DBG_END_WUP_PREVENT_SPLL_DWN_EN_Msk                 (0x01 << DBG_END_WUP_PREVENT_SPLL_DWN_EN_Pos)
#define DBG_END_WUP_PREVENT_SPLL_DWN_EN_Pos                 4
#define DBG_END_WUP_PREVENT_SPLL_DWN_EN_PosL                4
#define DBG_END_WUP_PREVENT_SPLL_DWN_EN_PosH                4

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define DBG_END_WUP_PREVENT_DCXO_DWN_EN_Msk                 (0x01 << DBG_END_WUP_PREVENT_DCXO_DWN_EN_Pos)
#define DBG_END_WUP_PREVENT_DCXO_DWN_EN_Pos                 3
#define DBG_END_WUP_PREVENT_DCXO_DWN_EN_PosL                3
#define DBG_END_WUP_PREVENT_DCXO_DWN_EN_PosH                3

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define DBG_END_WUP_PREVENT_MSI_DWN_EN_Msk                  (0x01 << DBG_END_WUP_PREVENT_MSI_DWN_EN_Pos)
#define DBG_END_WUP_PREVENT_MSI_DWN_EN_Pos                  2
#define DBG_END_WUP_PREVENT_MSI_DWN_EN_PosL                 2
#define DBG_END_WUP_PREVENT_MSI_DWN_EN_PosH                 2

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define DBG_END_WUP_PREVENT_SOCLDO_DWN_EN_Msk               (0x01 << DBG_END_WUP_PREVENT_SOCLDO_DWN_EN_Pos)
#define DBG_END_WUP_PREVENT_SOCLDO_DWN_EN_Pos               1
#define DBG_END_WUP_PREVENT_SOCLDO_DWN_EN_PosL              1
#define DBG_END_WUP_PREVENT_SOCLDO_DWN_EN_PosH              1

/*!< 0x0120 MMW_PREVENT_DWN1
*null
*/
#define DBG_END_WUP_PREVENT_INTLDO_DWN_EN_Msk               (0x01 << DBG_END_WUP_PREVENT_INTLDO_DWN_EN_Pos)
#define DBG_END_WUP_PREVENT_INTLDO_DWN_EN_Pos               0
#define DBG_END_WUP_PREVENT_INTLDO_DWN_EN_PosL              0
#define DBG_END_WUP_PREVENT_INTLDO_DWN_EN_PosH              0


/*!< 0x012C EFUSE_BOOTSEL_DIS
*null
*/
#define EFUSE_BOOTSEL_DIS_Msk                               (0x01 << EFUSE_BOOTSEL_DIS_Pos)
#define EFUSE_BOOTSEL_DIS_Pos                               1
#define EFUSE_BOOTSEL_DIS_PosL                              1
#define EFUSE_BOOTSEL_DIS_PosH                              1


/*!< 0x0130 PMU_WUP_FLAG
*null
*/
#define UART_WUP_FLAG_Msk                                   (0x01 << UART_WUP_FLAG_Pos)
#define UART_WUP_FLAG_Pos                                   3
#define UART_WUP_FLAG_PosL                                  3
#define UART_WUP_FLAG_PosH                                  3

/*!< 0x0130 PMU_WUP_FLAG
*null
*/
#define IIC_WUP_FLAG_Msk                                    (0x01 << IIC_WUP_FLAG_Pos)
#define IIC_WUP_FLAG_Pos                                    2
#define IIC_WUP_FLAG_PosL                                   2
#define IIC_WUP_FLAG_PosH                                   2

/*!< 0x0130 PMU_WUP_FLAG
*null
*/
#define SPI_WUP_FLAG_Msk                                    (0x01 << SPI_WUP_FLAG_Pos)
#define SPI_WUP_FLAG_Pos                                    1
#define SPI_WUP_FLAG_PosL                                   1
#define SPI_WUP_FLAG_PosH                                   1


/*!< 0x0134 PMU_SPI_WUP_CTRL
*null
*/
#define SPI_WUP_PIN_EN_Msk                                  (0x01 << SPI_WUP_PIN_EN_Pos)
#define SPI_WUP_PIN_EN_Pos                                  15
#define SPI_WUP_PIN_EN_PosL                                 15
#define SPI_WUP_PIN_EN_PosH                                 15

/*!< 0x0134 PMU_SPI_WUP_CTRL
*null
*/
#define SPI_WUP_TARGET_Msk                                  (0x0F << SPI_WUP_TARGET_Pos)
#define SPI_WUP_TARGET_Pos                                  4
#define SPI_WUP_TARGET_PosL                                 4
#define SPI_WUP_TARGET_PosH                                 7

/*!< 0x0134 PMU_SPI_WUP_CTRL
*null
*/
#define SPI_WUP_RESETN_EN_Msk                               (0x01 << SPI_WUP_RESETN_EN_Pos)
#define SPI_WUP_RESETN_EN_Pos                               0
#define SPI_WUP_RESETN_EN_PosL                              0
#define SPI_WUP_RESETN_EN_PosH                              0


/*!< 0x0138 PMU_IIC_WUP_CTRL
*null
*/
#define IIC_SCL_WUP_PIN_SEL_Msk                             (0x07 << IIC_SCL_WUP_PIN_SEL_Pos)
#define IIC_SCL_WUP_PIN_SEL_Pos                             20
#define IIC_SCL_WUP_PIN_SEL_PosL                            20
#define IIC_SCL_WUP_PIN_SEL_PosH                            22

/*!< 0x0138 PMU_IIC_WUP_CTRL
*null
*/
#define IIC_SDA_WUP_PIN_SEL_Msk                             (0x07 << IIC_SDA_WUP_PIN_SEL_Pos)
#define IIC_SDA_WUP_PIN_SEL_Pos                             16
#define IIC_SDA_WUP_PIN_SEL_PosL                            16
#define IIC_SDA_WUP_PIN_SEL_PosH                            18

/*!< 0x0138 PMU_IIC_WUP_CTRL
*null
*/
#define IIC_WUP_PIN_EN_Msk                                  (0x01 << IIC_WUP_PIN_EN_Pos)
#define IIC_WUP_PIN_EN_Pos                                  15
#define IIC_WUP_PIN_EN_PosL                                 15
#define IIC_WUP_PIN_EN_PosH                                 15

/*!< 0x0138 PMU_IIC_WUP_CTRL
*null
*/
#define IIC_WUP_EN_Msk                                      (0x01 << IIC_WUP_EN_Pos)
#define IIC_WUP_EN_Pos                                      8
#define IIC_WUP_EN_PosL                                     8
#define IIC_WUP_EN_PosH                                     8

/*!< 0x0138 PMU_IIC_WUP_CTRL
*null
*/
#define WUP_IIC_SLAVE_ADDR_Msk                              (0x7F << WUP_IIC_SLAVE_ADDR_Pos)
#define WUP_IIC_SLAVE_ADDR_Pos                              1
#define WUP_IIC_SLAVE_ADDR_PosL                             1
#define WUP_IIC_SLAVE_ADDR_PosH                             7

/*!< 0x0138 PMU_IIC_WUP_CTRL
*null
*/
#define IIC_WUP_RESETN_EN_Msk                               (0x01 << IIC_WUP_RESETN_EN_Pos)
#define IIC_WUP_RESETN_EN_Pos                               0
#define IIC_WUP_RESETN_EN_PosL                              0
#define IIC_WUP_RESETN_EN_PosH                              0


/*!< 0x013c PMU_UART_WUP_CTRL
*null
*/
#define UART_WUP_PIN_SEL_Msk                                (0x07 << UART_WUP_PIN_SEL_Pos)
#define UART_WUP_PIN_SEL_Pos                                16
#define UART_WUP_PIN_SEL_PosL                               16
#define UART_WUP_PIN_SEL_PosH                               18

/*!< 0x013c PMU_UART_WUP_CTRL
*null
*/
#define UART_WUP_PIN_EN_Msk                                 (0x01 << UART_WUP_PIN_EN_Pos)
#define UART_WUP_PIN_EN_Pos                                 15
#define UART_WUP_PIN_EN_PosL                                15
#define UART_WUP_PIN_EN_PosH                                15

/*!< 0x013c PMU_UART_WUP_CTRL
*null
*/
#define UART_WUP_TARGET_Msk                                 (0x0F << UART_WUP_TARGET_Pos)
#define UART_WUP_TARGET_Pos                                 4
#define UART_WUP_TARGET_PosL                                4
#define UART_WUP_TARGET_PosH                                7

/*!< 0x013c PMU_UART_WUP_CTRL
*null
*/
#define UART_WUP_RESETN_EN_Msk                              (0x01 << UART_WUP_RESETN_EN_Pos)
#define UART_WUP_RESETN_EN_Pos                              0
#define UART_WUP_RESETN_EN_PosL                             0
#define UART_WUP_RESETN_EN_PosH                             0


/*!< 0x0140 PMU_TMR_WUP_CTRL0
*null
*/
#define TMR_WUP_CLK_SRC_SEL_Msk                             (0x03 << TMR_WUP_CLK_SRC_SEL_Pos)
#define TMR_WUP_CLK_SRC_SEL_Pos                             4
#define TMR_WUP_CLK_SRC_SEL_PosL                            4
#define TMR_WUP_CLK_SRC_SEL_PosH                            5

/*!< 0x0140 PMU_TMR_WUP_CTRL0
*null
*/
#define TMR_WUP_CLK_EN_Msk                                  (0x01 << TMR_WUP_CLK_EN_Pos)
#define TMR_WUP_CLK_EN_Pos                                  1
#define TMR_WUP_CLK_EN_PosL                                 1
#define TMR_WUP_CLK_EN_PosH                                 1

/*!< 0x0140 PMU_TMR_WUP_CTRL0
*null
*/
#define TMR_WUP_RESETN_EN_Msk                               (0x01 << TMR_WUP_RESETN_EN_Pos)
#define TMR_WUP_RESETN_EN_Pos                               0
#define TMR_WUP_RESETN_EN_PosL                              0
#define TMR_WUP_RESETN_EN_PosH                              0

/*!< 0x144h PMU_TMR_WUP_CTRL1
*null
*/
#define TMR_WUP_CMP1_CLR_Msk                                 (0x01 << TMR_WUP_CMP1_CLR_Pos)
#define TMR_WUP_CMP1_CLR_Pos                                 17
#define TMR_WUP_CMP1_CLR_PosL                                17
#define TMR_WUP_CMP1_CLR_PosH                                17

/*!< 0x144h PMU_TMR_WUP_CTRL1
*null
*/
#define TMR_WUP_CMP0_CLR_Msk                                 (0x01 << TMR_WUP_CMP0_CLR_Pos)
#define TMR_WUP_CMP0_CLR_Pos                                 16
#define TMR_WUP_CMP0_CLR_PosL                                16
#define TMR_WUP_CMP0_CLR_PosH                                16

/*!< 0x144h PMU_TMR_WUP_CTRL1
*null
*/
#define TMR_WUP_CMP1_EN_Msk                                 (0x01 << TMR_WUP_CMP1_EN_Pos)
#define TMR_WUP_CMP1_EN_Pos                                 9
#define TMR_WUP_CMP1_EN_PosL                                9
#define TMR_WUP_CMP1_EN_PosH                                9

/*!< 0x144h PMU_TMR_WUP_CTRL1
*null
*/
#define TMR_WUP_CMP0_EN_Msk                                 (0x01 << TMR_WUP_CMP0_EN_Pos)
#define TMR_WUP_CMP0_EN_Pos                                 8
#define TMR_WUP_CMP0_EN_PosL                                8
#define TMR_WUP_CMP0_EN_PosH                                8

/*!< 0x144h PMU_TMR_WUP_CTRL1
*null
*/
#define TMR_WUP_EN_Msk                                      (0x01 << TMR_WUP_EN_Pos)
#define TMR_WUP_EN_Pos                                      0
#define TMR_WUP_EN_PosL                                     0
#define TMR_WUP_EN_PosH                                     0

/*!< 0x148h PMU_TMR_WUP_INTR_FLAG
*null
*/
#define TMR_WUP_CMP1_FLAG_Msk                               (0x01 << TMR_WUP_CMP1_FLAG_Pos)
#define TMR_WUP_CMP1_FLAG_Pos                               1
#define TMR_WUP_CMP1_FLAG_PosL                              1
#define TMR_WUP_CMP1_FLAG_PosH                              1

/*!< 0x148h PMU_TMR_WUP_INTR_FLAG
*null
*/
#define TMR_WUP_CMP0_FLAG_Msk                               (0x01 << TMR_WUP_CMP0_FLAG_Pos)
#define TMR_WUP_CMP0_FLAG_Pos                               0
#define TMR_WUP_CMP0_FLAG_PosL                              0
#define TMR_WUP_CMP0_FLAG_PosH                              0

/*!< 0x14Ch PMU_TMR_WUP_VAL0
*null
*/
#define TMR_WUP_TMR_VAL0_Msk                                (0xFFFFFFFF << TMR_WUP_TMR_VAL0_Pos)
#define TMR_WUP_TMR_VAL0_Pos                                0
#define TMR_WUP_TMR_VAL0_PosL                               0
#define TMR_WUP_TMR_VAL0_PosH                               0

/*!< 0x150h PMU_TMR_WUP_VAL1
*null
*/
#define TMR_WUP_TMR_VAL1_Msk                                (0xFFFFFFFF << TMR_WUP_TMR_VAL1_Pos)
#define TMR_WUP_TMR_VAL1_Pos                                0
#define TMR_WUP_TMR_VAL1_PosL                               0
#define TMR_WUP_TMR_VAL1_PosH                               0


/*!< 0x154h PMU_TMR_WUP_CMP0_VAL0
*null
*/
#define TMR_WUP_CMP0_VAL0_Msk                               (0XFFFFFFF << TMR_WUP_CMP0_VAL0_Pos)
#define TMR_WUP_CMP0_VAL0_Pos                               0
#define TMR_WUP_CMP0_VAL0_PosL                              0
#define TMR_WUP_CMP0_VAL0_PosH                              31


/*!< 0x158h PMU_TMR_WUP_CMP0_VAL1
*null
*/
#define TMR_WUP_CMP0_VAL1_Msk                               (0xFFFF << TMR_WUP_CMP0_VAL1_Pos)
#define TMR_WUP_CMP0_VAL1_Pos                               0
#define TMR_WUP_CMP0_VAL1_PosL                              0
#define TMR_WUP_CMP0_VAL1_PosH                              15

/*!< 0x15Ch PMU_TMR_WUP_CMP1_VAL0
*null
*/
#define TMR_WUP_CMP1_VAL0_Msk                               (0XFFFFFFF << TMR_WUP_CMP1_VAL0_Pos)
#define TMR_WUP_CMP1_VAL0_Pos                               0
#define TMR_WUP_CMP1_VAL0_PosL                              0
#define TMR_WUP_CMP1_VAL0_PosH                              31

/*!< 0x160h PMU_TMR_WUP_CMP1_VAL1
*null
*/
#define TMR_WUP_CMP1_VAL1_Msk                               (0xFFFF << TMR_WUP_CMP1_VAL1_Pos)
#define TMR_WUP_CMP1_VAL1_Pos                               0
#define TMR_WUP_CMP1_VAL1_PosL                              0
#define TMR_WUP_CMP1_VAL1_PosH                              15


/*!< 0x180h PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XO_IO_FANOUT_EN_Msk                       (0x01 << DCXO_OVRD_XO_IO_FANOUT_EN_Pos)
#define DCXO_OVRD_XO_IO_FANOUT_EN_Pos                       18
#define DCXO_OVRD_XO_IO_FANOUT_EN_PosL                      18
#define DCXO_OVRD_XO_IO_FANOUT_EN_PosH                      18

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XO_M50_EN_Msk                             (0x01 << DCXO_OVRD_XO_M50_EN_Pos)
#define DCXO_OVRD_XO_M50_EN_Pos                             17
#define DCXO_OVRD_XO_M50_EN_PosL                            17
#define DCXO_OVRD_XO_M50_EN_PosH                            17

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XO_SOC_EN_Msk                             (0x01 << DCXO_OVRD_XO_SOC_EN_Pos)
#define DCXO_OVRD_XO_SOC_EN_Pos                             16
#define DCXO_OVRD_XO_SOC_EN_PosL                            16
#define DCXO_OVRD_XO_SOC_EN_PosH                            16

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XO_SPLL_EN_Msk                            (0x01 << DCXO_OVRD_XO_SPLL_EN_Pos)
#define DCXO_OVRD_XO_SPLL_EN_Pos                            15
#define DCXO_OVRD_XO_SPLL_EN_PosL                           15
#define DCXO_OVRD_XO_SPLL_EN_PosH                           15

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_LDO_EN_Msk                                (0x01 << DCXO_OVRD_LDO_EN_Pos)
#define DCXO_OVRD_LDO_EN_Pos                                14
#define DCXO_OVRD_LDO_EN_PosL                               14
#define DCXO_OVRD_LDO_EN_PosH                               14

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_LDO_LQ_Msk                                (0x01 << DCXO_OVRD_LDO_LQ_Pos)
#define DCXO_OVRD_LDO_LQ_Pos                                13
#define DCXO_OVRD_LDO_LQ_PosL                               13
#define DCXO_OVRD_LDO_LQ_PosH                               13

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_IN_ISOLATION_Msk                          (0x01 << DCXO_OVRD_IN_ISOLATION_Pos)
#define DCXO_OVRD_IN_ISOLATION_Pos                          12
#define DCXO_OVRD_IN_ISOLATION_PosL                         12
#define DCXO_OVRD_IN_ISOLATION_PosH                         12

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_OUT_ISOLATION_Msk                         (0x01 << DCXO_OVRD_OUT_ISOLATION_Pos)
#define DCXO_OVRD_OUT_ISOLATION_Pos                         11
#define DCXO_OVRD_OUT_ISOLATION_PosL                        11
#define DCXO_OVRD_OUT_ISOLATION_PosH                        11

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XO_AMC_EN_Msk                             (0x01 << DCXO_OVRD_XO_AMC_EN_Pos)
#define DCXO_OVRD_XO_AMC_EN_Pos                             10
#define DCXO_OVRD_XO_AMC_EN_PosL                            10
#define DCXO_OVRD_XO_AMC_EN_PosH                            10

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XO_BUF_EN_Msk                             (0x01 << DCXO_OVRD_XO_BUF_EN_Pos)
#define DCXO_OVRD_XO_BUF_EN_Pos                             9
#define DCXO_OVRD_XO_BUF_EN_PosL                            9
#define DCXO_OVRD_XO_BUF_EN_PosH                            9

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XO_EN_Msk                                 (0x01 << DCXO_OVRD_XO_EN_Pos)
#define DCXO_OVRD_XO_EN_Pos                                 8
#define DCXO_OVRD_XO_EN_PosL                                8
#define DCXO_OVRD_XO_EN_PosH                                8

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_ITRIM_BYPASS_Msk                          (0x01 << DCXO_OVRD_ITRIM_BYPASS_Pos)
#define DCXO_OVRD_ITRIM_BYPASS_Pos                          7
#define DCXO_OVRD_ITRIM_BYPASS_PosL                         7
#define DCXO_OVRD_ITRIM_BYPASS_PosH                         7

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_STARTUP_EN_Msk                            (0x01 << DCXO_OVRD_STARTUP_EN_Pos)
#define DCXO_OVRD_STARTUP_EN_Pos                            6
#define DCXO_OVRD_STARTUP_EN_PosL                           6
#define DCXO_OVRD_STARTUP_EN_PosH                           6

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XTAL_DETECT_EN_Msk                        (0x01 << DCXO_OVRD_XTAL_DETECT_EN_Pos)
#define DCXO_OVRD_XTAL_DETECT_EN_Pos                        5
#define DCXO_OVRD_XTAL_DETECT_EN_PosL                       5
#define DCXO_OVRD_XTAL_DETECT_EN_PosH                       5

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_XTAL_MODE_Msk                             (0x01 << DCXO_OVRD_XTAL_MODE_Pos)
#define DCXO_OVRD_XTAL_MODE_Pos                             4
#define DCXO_OVRD_XTAL_MODE_PosL                            4
#define DCXO_OVRD_XTAL_MODE_PosH                            4

/*!< 0x0180 PMU_DCXO_OVRD
*null
*/
#define DCXO_OVRD_EN_Msk                                    (0x01 << DCXO_OVRD_EN_Pos)
#define DCXO_OVRD_EN_Pos                                    0
#define DCXO_OVRD_EN_PosL                                   0
#define DCXO_OVRD_EN_PosH                                   0


/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_CKCDAA_EN_Msk                             (0x01 << SPLL_OVRD_CKCDAA_EN_Pos)
#define SPLL_OVRD_CKCDAA_EN_Pos                             12
#define SPLL_OVRD_CKCDAA_EN_PosL                            12
#define SPLL_OVRD_CKCDAA_EN_PosH                            12

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_CK160M_EN_Msk                             (0x01 << SPLL_OVRD_CK160M_EN_Pos)
#define SPLL_OVRD_CK160M_EN_Pos                             11
#define SPLL_OVRD_CK160M_EN_PosL                            11
#define SPLL_OVRD_CK160M_EN_PosH                            11

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_CKDEV1_EN_Msk                             (0x01 << SPLL_OVRD_CKDEV1_EN_Pos)
#define SPLL_OVRD_CKDEV1_EN_Pos                             10
#define SPLL_OVRD_CKDEV1_EN_PosL                            10
#define SPLL_OVRD_CKDEV1_EN_PosH                            10

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_CKDEV_EN_Msk                              (0x01 << SPLL_OVRD_CKDEV_EN_Pos)
#define SPLL_OVRD_CKDEV_EN_Pos                              9
#define SPLL_OVRD_CKDEV_EN_PosL                             9
#define SPLL_OVRD_CKDEV_EN_PosH                             9

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_SPLL_EN_Msk                               (0x01 << SPLL_OVRD_SPLL_EN_Pos)
#define SPLL_OVRD_SPLL_EN_Pos                               8
#define SPLL_OVRD_SPLL_EN_PosL                              8
#define SPLL_OVRD_SPLL_EN_PosH                              8

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_LDO_EN_Msk                                (0x01 << SPLL_OVRD_LDO_EN_Pos)
#define SPLL_OVRD_LDO_EN_Pos                                7
#define SPLL_OVRD_LDO_EN_PosL                               7
#define SPLL_OVRD_LDO_EN_PosH                               7

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_LDO_LQ_Msk                                (0x01 << SPLL_OVRD_LDO_LQ_Pos)
#define SPLL_OVRD_LDO_LQ_Pos                                6
#define SPLL_OVRD_LDO_LQ_PosL                               6
#define SPLL_OVRD_LDO_LQ_PosH                               6

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_OUT_ISOLATION_N_Msk                       (0x01 << SPLL_OVRD_OUT_ISOLATION_N_Pos)
#define SPLL_OVRD_OUT_ISOLATION_N_Pos                       5
#define SPLL_OVRD_OUT_ISOLATION_N_PosL                      5
#define SPLL_OVRD_OUT_ISOLATION_N_PosH                      5

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_IN_ISOLATION_N_Msk                        (0x01 << SPLL_OVRD_IN_ISOLATION_N_Pos)
#define SPLL_OVRD_IN_ISOLATION_N_Pos                        4
#define SPLL_OVRD_IN_ISOLATION_N_PosL                       4
#define SPLL_OVRD_IN_ISOLATION_N_PosH                       4

/*!< 0x0188 PMU_SPLL_OVRD
*null
*/
#define SPLL_OVRD_EN_Msk                                    (0x01 << SPLL_OVRD_EN_Pos)
#define SPLL_OVRD_EN_Pos                                    0
#define SPLL_OVRD_EN_PosL                                   0
#define SPLL_OVRD_EN_PosH                                   0

/**
* @}
* End of group HW_PMU
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_PMU_H_ */
