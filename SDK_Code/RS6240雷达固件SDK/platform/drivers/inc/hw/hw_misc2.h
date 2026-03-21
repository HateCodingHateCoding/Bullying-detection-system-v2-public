/****************************************************************
* @file  hw_misc2.h
* @brief hw_misc2 hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_MISC2_H_
#define _HW_MISC2_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_MISC2
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __IOM uint32_t MISCREG2_MMW_SW_UP_SETTINGS;             /*0x0000,null                                               */
    __IOM uint32_t MISCREG2_SW_UP_PROC_SETTINGS;            /*0x0004,null                                               */
          uint32_t RSVD[2];                                 /*0x0008-0x000C,null                                        */
    __IOM uint32_t MISCREG2_MMW_POWERCTRL;                  /*0x0010,null                                               */
          uint32_t RSVD1[2];                                /*0x0014-0x0018,null                                        */
    __IM  uint32_t MISCREG2_MMW_STATUS;                     /*0x001c,null                                               */
          uint32_t RSVD2;                                   /*0x0020,null                                               */
    __IOM uint32_t MISCREG2_SRAMB_ON;                       /*0x0024,null                                               */
          uint32_t RSVD3[3];                                /*0x0028-0x0030,null                                        */
    __IOM uint32_t MISCREG2_SRAMB_RET_ON;                   /*0x0034,null                                               */
          uint32_t RSVD4[6];                                /*0x0038-0x004C,null                                        */
    __IOM uint32_t MISCREG2_MMW_RESET_CTRL;                 /*0x0050,null                                               */
    __IOM uint32_t FRAME_TRI_CTRL;                          /*0x0054,null                                               */
          uint32_t RSVD5[2];                                /*0x0058-0x005C,null                                        */
    __IOM uint32_t MISCREG2_MMW_FRAME_CTRL0;                /*0x0060,null                                               */
    __IOM uint32_t MISCREG2_MMW_FRAME_CTRL1;                /*0x0064,null                                               */
    __IOM uint32_t MISCREG2_MMW_FRAME_CTRL2;                /*0x0068,null                                               */
    __IOM uint32_t MISCREG2_MMW_FRAME_CTRL3;                /*0x006c,null                                               */
    __IOM uint32_t MISCREG2_MMW_FRAME_CTRL4;                /*0x0070,null                                               */
    __IOM uint32_t MISCREG2_MMW_FRAME_CTRL5;                /*0x0074,null                                               */
    __IOM uint32_t MISCREG2_MMW_FRAME_CTRL6;                /*0x0078,null                                               */
          uint32_t MISCREG2_MMW_FRAME_CTRL7;                /*0x007c,null                                               */
    __IOM uint32_t MMW_FRAME_CTRL8;                         /*0x0080,null                                               */
          uint32_t MISCREG2_MMW_FRAME_CTRL9;                /*0x0084,null                                               */
          uint32_t RSVD6[18];                               /*0x0088-0x00CC,null                                        */
    __IOM uint32_t MISCREG2_DPLL_CTRL0;                     /*0x00d0,null                                               */
    __IOM uint32_t MISCREG2_DPLL_CTRL1;                     /*0x00d4,null                                               */
    __IOM uint32_t MISCREG2_DPLL_CTRL2;                     /*0x00d8,null                                               */
    __IOM uint32_t MISCREG2_DPLL_CTRL3;                     /*0x00dc,null                                               */
    __IOM uint32_t MISCREG2_DPLL_CTRL4;                     /*0x00e0,null                                               */
    __IOM uint32_t MISCREG2_DPLL_CTRL5;                     /*0x00e4,null                                               */
    __IOM uint32_t MISCREG2_DPLL_CTRL6;                     /*0x00e8,null                                               */
          uint32_t RSVD7[4];                                /*0x00EC-0x00F8,null                                        */
    __IM  uint32_t MISCREG2_DPLL_STATUS;                    /*0x00fc,null                                               */
    __IOM uint32_t MISCREG2_DPLL_OVERRIDE0;                 /*0x0100,null                                               */
          uint32_t RSVD8[3];                                /*0x0104-0x010C,null                                        */
    __IOM uint32_t MMW_SPLL_CTRL;                           /*0x0110,null                                               */
          uint32_t RSVD9[3];                                /*0x0114-0x011C,null                                        */
    __IOM uint32_t MISCREG2_FREE_RUNNING_CNT_CTRL;          /*0x0120,null                                               */
    __IOM uint32_t MISCREG2_FREE_RUNNING_CNT_CPTR_CTRL;     /*0x0124,null                                               */
    __IOM uint32_t MISCREG2_FREE_RUNNING_CNT_CMP_CTRL[2];   /*0x0128-0x012C,null                                        */
    __IOM uint32_t MISCREG2_FREE_RUNNING_CPTR_VALUE;        /*0x0130,null                                               */
    __IOM uint32_t MISCREG2_FREE_RUNNING_CMP_VALUE;         /*0x0134,null                                               */
} misc2_dev_t;

#define MISCREG2_DEV                      ((misc2_dev_t *)   RTCSW2_MISC_BASE_ADDR)/*!< 0x4001 B000 */

/*!< 0x0000 MISCREG2_MMW_SW_UP_SETTINGS
*null
*/
#define MMWBBSW_UP_SETTINGS_Msk                             (0x7FF << MMWBBSW_UP_SETTINGS_Pos)
#define MMWBBSW_UP_SETTINGS_Pos                             0
#define MMWBBSW_UP_SETTINGS_PosL                            0
#define MMWBBSW_UP_SETTINGS_PosH                            10


/*!< 0x0004 MISCREG2_SW_UP_PROC_SETTINGS
*null
*/
#define SW_UP_TIMING_PROC_SETTINGS_Msk                      (0x0F << SW_UP_TIMING_PROC_SETTINGS_Pos)
#define SW_UP_TIMING_PROC_SETTINGS_Pos                      0
#define SW_UP_TIMING_PROC_SETTINGS_PosL                     0
#define SW_UP_TIMING_PROC_SETTINGS_PosH                     3


/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define RFDIG_LDO_TRIM_Msk                                  (0x0F << RFDIG_LDO_TRIM_Pos)
#define RFDIG_LDO_TRIM_Pos                                  28
#define RFDIG_LDO_TRIM_PosL                                 28
#define RFDIG_LDO_TRIM_PosH                                 31

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define RFDIG_LDO_BYPASS_EN_Msk                             (0x01 << RFDIG_LDO_BYPASS_EN_Pos)
#define RFDIG_LDO_BYPASS_EN_Pos                             26
#define RFDIG_LDO_BYPASS_EN_PosL                            26
#define RFDIG_LDO_BYPASS_EN_PosH                            26

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define RFDIG_LDO_TEST_EN_Msk                               (0x01 << RFDIG_LDO_TEST_EN_Pos)
#define RFDIG_LDO_TEST_EN_Pos                               25
#define RFDIG_LDO_TEST_EN_PosL                              25
#define RFDIG_LDO_TEST_EN_PosH                              25

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define RFDIG_LDO_LQ_EN_Msk                                 (0x01 << RFDIG_LDO_LQ_EN_Pos)
#define RFDIG_LDO_LQ_EN_Pos                                 24
#define RFDIG_LDO_LQ_EN_PosL                                24
#define RFDIG_LDO_LQ_EN_PosH                                24

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define TXRF_LDO_TRIM_Msk                                   (0x0F << TXRF_LDO_TRIM_Pos)
#define TXRF_LDO_TRIM_Pos                                   20
#define TXRF_LDO_TRIM_PosL                                  20
#define TXRF_LDO_TRIM_PosH                                  23

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define SLP_TXRF_LDO_OFF_Msk                                (0x01 << SLP_TXRF_LDO_OFF_Pos)
#define SLP_TXRF_LDO_OFF_Pos                                19
#define SLP_TXRF_LDO_OFF_PosL                               19
#define SLP_TXRF_LDO_OFF_PosH                               19

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define TXRF_LDO_BYPASS_EN_Msk                              (0x01 << TXRF_LDO_BYPASS_EN_Pos)
#define TXRF_LDO_BYPASS_EN_Pos                              18
#define TXRF_LDO_BYPASS_EN_PosL                             18
#define TXRF_LDO_BYPASS_EN_PosH                             18

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define FORCE_TXRF_LDO_ON_Msk                               (0x01 << FORCE_TXRF_LDO_ON_Pos)
#define FORCE_TXRF_LDO_ON_Pos                               17
#define FORCE_TXRF_LDO_ON_PosL                              17
#define FORCE_TXRF_LDO_ON_PosH                              17

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define TXRF_LDO_FLOW_EN_Msk                                (0x01 << TXRF_LDO_FLOW_EN_Pos)
#define TXRF_LDO_FLOW_EN_Pos                                16
#define TXRF_LDO_FLOW_EN_PosL                               16
#define TXRF_LDO_FLOW_EN_PosH                               16

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define SLP_DPLL_OFF_Msk                                    (0x01 << SLP_DPLL_OFF_Pos)
#define SLP_DPLL_OFF_Pos                                    11
#define SLP_DPLL_OFF_PosL                                   11
#define SLP_DPLL_OFF_PosH                                   11

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define MMW_SRAM_ON_Msk                                     (0x01 << MMW_SRAM_ON_Pos)
#define MMW_SRAM_ON_Pos                                     10
#define MMW_SRAM_ON_PosL                                    10
#define MMW_SRAM_ON_PosH                                    10

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define SWSRAMTOP_ON_FLOW_SYS1_Msk                          (0x01 << SWSRAMTOP_ON_FLOW_SYS1_Pos)
#define SWSRAMTOP_ON_FLOW_SYS1_Pos                          9
#define SWSRAMTOP_ON_FLOW_SYS1_PosL                         9
#define SWSRAMTOP_ON_FLOW_SYS1_PosH                         9

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define SWSRAMTOP_ON_FLOW_SYS0_Msk                          (0x01 << SWSRAMTOP_ON_FLOW_SYS0_Pos)
#define SWSRAMTOP_ON_FLOW_SYS0_Pos                          8
#define SWSRAMTOP_ON_FLOW_SYS0_PosL                         8
#define SWSRAMTOP_ON_FLOW_SYS0_PosH                         8

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define FORCE_DPLL_EN_Msk                                   (0x01 << FORCE_DPLL_EN_Pos)
#define FORCE_DPLL_EN_Pos                                   7
#define FORCE_DPLL_EN_PosL                                  7
#define FORCE_DPLL_EN_PosH                                  7

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define FORCE_SWSRAMTOP_ACTIVE_Msk                          (0x01 << FORCE_SWSRAMTOP_ACTIVE_Pos)
#define FORCE_SWSRAMTOP_ACTIVE_Pos                          6
#define FORCE_SWSRAMTOP_ACTIVE_PosL                         6
#define FORCE_SWSRAMTOP_ACTIVE_PosH                         6

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define SWSRAMTOP_WAIT_DPLL_READY_Msk                       (0x01 << SWSRAMTOP_WAIT_DPLL_READY_Pos)
#define SWSRAMTOP_WAIT_DPLL_READY_Pos                       5
#define SWSRAMTOP_WAIT_DPLL_READY_PosL                      5
#define SWSRAMTOP_WAIT_DPLL_READY_PosH                      5

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define FORCE_SWSRAMTOP_SW_EN_Msk                           (0x01 << FORCE_SWSRAMTOP_SW_EN_Pos)
#define FORCE_SWSRAMTOP_SW_EN_Pos                           4
#define FORCE_SWSRAMTOP_SW_EN_PosL                          4
#define FORCE_SWSRAMTOP_SW_EN_PosH                          4

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define MMWBBSW_WAIT_DPLL_READY_Msk                         (0x01 << MMWBBSW_WAIT_DPLL_READY_Pos)
#define MMWBBSW_WAIT_DPLL_READY_Pos                         3
#define MMWBBSW_WAIT_DPLL_READY_PosL                        3
#define MMWBBSW_WAIT_DPLL_READY_PosH                        3

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define FORCE_MMWBBSW_EN_Msk                                (0x01 << FORCE_MMWBBSW_EN_Pos)
#define FORCE_MMWBBSW_EN_Pos                                2
#define FORCE_MMWBBSW_EN_PosL                               2
#define FORCE_MMWBBSW_EN_PosH                               2

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*null
*/
#define FORCE_DOWN_MMW_Msk                                  (0x01 << FORCE_DOWN_MMW_Pos)
#define FORCE_DOWN_MMW_Pos                                  1
#define FORCE_DOWN_MMW_PosL                                 1
#define FORCE_DOWN_MMW_PosH                                 1

/*!< 0x0010 MISCREG2_MMW_POWERCTRL
*/
#define FORCE_WUP_MMW_Msk                                   (0x01 << FORCE_WUP_MMW_Pos)
#define FORCE_WUP_MMW_Pos                                   0
#define FORCE_WUP_MMW_PosL                                  0
#define FORCE_WUP_MMW_PosH                                  0


/*!< 0x001c MISCREG2_MMW_STATUS
*null
*/
#define SWSRAMTOP_STATUS_Msk                                (0x01 << SWSRAMTOP_STATUS_Pos)
#define SWSRAMTOP_STATUS_Pos                                3
#define SWSRAMTOP_STATUS_PosL                               3
#define SWSRAMTOP_STATUS_PosH                               3

/*!< 0x001c MISCREG2_MMW_STATUS
*null
*/
#define MMWBBSW_STATUS_Msk                                  (0x01 << MMWBBSW_STATUS_Pos)
#define MMWBBSW_STATUS_Pos                                  2
#define MMWBBSW_STATUS_PosL                                 2
#define MMWBBSW_STATUS_PosH                                 2

/*!< 0x001c MISCREG2_MMW_STATUS
*null
*/
#define MMW_PWR_CTRL_STATUS_Msk                             (0x03 << MMW_PWR_CTRL_STATUS_Pos)
#define MMW_PWR_CTRL_STATUS_Pos                             0
#define MMW_PWR_CTRL_STATUS_PosL                            0
#define MMW_PWR_CTRL_STATUS_PosH                            1


/*!< 0x0024~0x0030 MISCREG2_SRAMB_ON
*null
*/
#define SRAMB_PU_Msk                                        (0x1F << SRAMB_PU_Pos)
#define SRAMB_PU_Pos                                        0
#define SRAMB_PU_PosL                                       0
#define SRAMB_PU_PosH                                       0


/*!< 0x0034~0x0040 MISCREG2_SRAMB_RET_EN
*null
*/
#define SRAMB_RET_Msk                                       (0x1F << SRAMB_RET_Pos)
#define SRAMB_RET_Pos                                       0
#define SRAMB_RET_PosL                                      0
#define SRAMB_RET_PosH                                      0


/*!< 0x0050 MISCREG2_MMW_RESET_CTRL
*null
*/
#define MMW_HRESET_EN_Msk                                   (0x01 << MMW_HRESET_EN_Pos)
#define MMW_HRESET_EN_Pos                                   0
#define MMW_HRESET_EN_PosL                                  0
#define MMW_HRESET_EN_PosH                                  0


/*!< 0x0054 FRAME_TRI_CTRL
*null
*/
#define FRAME_TRI_PINMUX_SEL_Msk                            (0x01 << FRAME_TRI_PINMUX_SEL_Pos)
#define FRAME_TRI_PINMUX_SEL_Pos                            4
#define FRAME_TRI_PINMUX_SEL_PosL                           4
#define FRAME_TRI_PINMUX_SEL_PosH                           4

/*!< 0x0054 FRAME_TRI_CTRL
*使能从PAD输入FRAME_TRI
*/
#define FRAME_TRI_PINMUX_EN_Msk                             (0x01 << FRAME_TRI_PINMUX_EN_Pos)
#define FRAME_TRI_PINMUX_EN_Pos                             0
#define FRAME_TRI_PINMUX_EN_PosL                            0
#define FRAME_TRI_PINMUX_EN_PosH                            0


/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define MM_FRAME_CLK_SRC_SEL_Msk                            (0x03 << MM_FRAME_CLK_SRC_SEL_Pos)
#define MM_FRAME_CLK_SRC_SEL_Pos                            27
#define MM_FRAME_CLK_SRC_SEL_PosL                           27
#define MM_FRAME_CLK_SRC_SEL_PosH                           28

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define MM_FRAME_CLK_DIV_Msk                                (0x07 << MM_FRAME_CLK_DIV_Pos)
#define MM_FRAME_CLK_DIV_Pos                                24
#define MM_FRAME_CLK_DIV_PosL                               24
#define MM_FRAME_CLK_DIV_PosH                               26

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define MMIC_MODE_Msk                                       (0x01 << MMIC_MODE_Pos)
#define MMIC_MODE_Pos                                       19
#define MMIC_MODE_PosL                                      19
#define MMIC_MODE_PosH                                      19

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define FRAME_SYN_MODE_Msk                                  (0x01 << FRAME_SYN_MODE_Pos)
#define FRAME_SYN_MODE_Pos                                  18
#define FRAME_SYN_MODE_PosL                                 18
#define FRAME_SYN_MODE_PosH                                 18

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define FORCE_MM_FRAME_CLK_EN_Msk                           (0x01 << FORCE_MM_FRAME_CLK_EN_Pos)
#define FORCE_MM_FRAME_CLK_EN_Pos                           17
#define FORCE_MM_FRAME_CLK_EN_PosL                          17
#define FORCE_MM_FRAME_CLK_EN_PosH                          17

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define MM_FRAME_RESET_EN_Msk                               (0x01 << MM_FRAME_RESET_EN_Pos)
#define MM_FRAME_RESET_EN_Pos                               16
#define MM_FRAME_RESET_EN_PosL                              16
#define MM_FRAME_RESET_EN_PosH                              16

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define MDT_IND_EN_Msk                                      (0x01 << MDT_IND_EN_Pos)
#define MDT_IND_EN_Pos                                      14
#define MDT_IND_EN_PosL                                     14
#define MDT_IND_EN_PosH                                     14

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define FFT_IND_EN_Msk                                      (0x01 << FFT_IND_EN_Pos)
#define FFT_IND_EN_Pos                                      13
#define FFT_IND_EN_PosL                                     13
#define FFT_IND_EN_PosH                                     13

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define MTP_IND_EN_Msk                                      (0x01 << MTP_IND_EN_Pos)
#define MTP_IND_EN_Pos                                      12
#define MTP_IND_EN_PosL                                     12
#define MTP_IND_EN_PosH                                     12

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define INTERVAL_END_IND_EN_Msk                             (0x01 << INTERVAL_END_IND_EN_Pos)
#define INTERVAL_END_IND_EN_Pos                             11
#define INTERVAL_END_IND_EN_PosL                            11
#define INTERVAL_END_IND_EN_PosH                            11

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*null
*/
#define MM_IND_EN_Msk                                       (0x01 << MM_IND_EN_Pos)
#define MM_IND_EN_Pos                                       10
#define MM_IND_EN_PosL                                      10
#define MM_IND_EN_PosH                                      10

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define DFFT2_UN_PW_DOWN_Msk                                (0x01 << DFFT2_UN_PW_DOWN_Pos)
#define DFFT2_UN_PW_DOWN_Pos                                8
#define DFFT2_UN_PW_DOWN_PosL                               8
#define DFFT2_UN_PW_DOWN_PosH                               8

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define BB_PW_DOWN_EN_Msk                                   (0x01 << BB_PW_DOWN_EN_Pos)
#define BB_PW_DOWN_EN_Pos                                   7
#define BB_PW_DOWN_EN_PosL                                  7
#define BB_PW_DOWN_EN_PosH                                  7

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define FRAME_ONE_SHORT_EN_Msk                              (0x01 << FRAME_ONE_SHORT_EN_Pos)
#define FRAME_ONE_SHORT_EN_Pos                              6
#define FRAME_ONE_SHORT_EN_PosL                             6
#define FRAME_ONE_SHORT_EN_PosH                             6

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define RCCAL_EN_Msk                                        (0x01 << RCCAL_EN_Pos)
#define RCCAL_EN_Pos                                        5
#define RCCAL_EN_PosL                                       5
#define RCCAL_EN_PosH                                       5

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define MT_WAIT_CPU_EN_Msk                                  (0x01 << MT_WAIT_CPU_EN_Pos)
#define MT_WAIT_CPU_EN_Pos                                  4
#define MT_WAIT_CPU_EN_PosL                                 4
#define MT_WAIT_CPU_EN_PosH                                 4

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define MT_FLAG_Msk                                         (0x01 << MT_FLAG_Pos)
#define MT_FLAG_Pos                                         3
#define MT_FLAG_PosL                                        3
#define MT_FLAG_PosH                                        3

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define MDT_FLAG_Msk                                        (0x01 << MDT_FLAG_Pos)
#define MDT_FLAG_Pos                                        2
#define MDT_FLAG_PosL                                       2
#define MDT_FLAG_PosH                                       2

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define DFFT2_FLAG_Msk                                      (0x01 << DFFT2_FLAG_Pos)
#define DFFT2_FLAG_Pos                                      1
#define DFFT2_FLAG_PosL                                     1
#define DFFT2_FLAG_PosH                                     1

/*!< 0x0060 MISCREG2_MMW_FRAME_CTRL0
*/
#define RTC_FSM_EN_Msk                                      (0x01 << RTC_FSM_EN_Pos)
#define RTC_FSM_EN_Pos                                      0
#define RTC_FSM_EN_PosL                                     0
#define RTC_FSM_EN_PosH                                     0


/*!< 0x0064 MISCREG2_MMW_FRAME_CTRL1
*/
#define INTERVAL_LEN_Msk                                    (0xFFFFF << INTERVAL_LEN_Pos)
#define INTERVAL_LEN_Pos                                    0
#define INTERVAL_LEN_PosL                                   0
#define INTERVAL_LEN_PosH                                   23


/*!< 0x0068 MISCREG2_MMW_FRAME_CTRL2
*/
#define FRAME_LEN_Msk                                       (0xFFFF << FRAME_LEN_Pos)
#define FRAME_LEN_Pos                                       0
#define FRAME_LEN_PosL                                      0
#define FRAME_LEN_PosH                                      15


/*!< 0x006c MISCREG2_MMW_FRAME_CTRL3
*/
#define MDT_IND_NUM_Msk                                     (0x1F << MDT_IND_NUM_Pos)
#define MDT_IND_NUM_Pos                                     20
#define MDT_IND_NUM_PosL                                    20
#define MDT_IND_NUM_PosH                                    24

/*!< 0x006c MISCREG2_MMW_FRAME_CTRL3
*/
#define MDT_MASK_Msk                                        (0x0F << MDT_MASK_Pos)
#define MDT_MASK_Pos                                        16
#define MDT_MASK_PosL                                       16
#define MDT_MASK_PosH                                       19

/*!< 0x006c MISCREG2_MMW_FRAME_CTRL3
*/
#define DFFT2_DOP_NUM_Msk                                   (0x3F << DFFT2_DOP_NUM_Pos)
#define DFFT2_DOP_NUM_Pos                                   10
#define DFFT2_DOP_NUM_PosL                                  10
#define DFFT2_DOP_NUM_PosH                                  15

/*!< 0x006c MISCREG2_MMW_FRAME_CTRL3
*/
#define DFFT2_NUM_Msk                                       (0x3FF << DFFT2_NUM_Pos)
#define DFFT2_NUM_Pos                                       0
#define DFFT2_NUM_PosL                                      0
#define DFFT2_NUM_PosH                                      9


/*!< 0x0070 MISCREG2_MMW_FRAME_CTRL4
*/
#define RCCAL_INTER_LEN_Msk                                 (0x3FF << RCCAL_INTER_LEN_Pos)
#define RCCAL_INTER_LEN_Pos                                 12
#define RCCAL_INTER_LEN_PosL                                12
#define RCCAL_INTER_LEN_PosH                                21

/*!< 0x0070 MISCREG2_MMW_FRAME_CTRL4
*/
#define START_DLY_Msk                                       (0xFF << START_DLY_Pos)
#define START_DLY_Pos                                       0
#define START_DLY_PosL                                      0
#define START_DLY_PosH                                      11


/*!< 0x0074 MISCREG2_MMW_FRAME_CTRL5
*/
#define MTP_IND_MODE_Msk                                    (0x01 << MTP_IND_MODE_Pos)
#define MTP_IND_MODE_Pos                                    28
#define MTP_IND_MODE_PosL                                   28
#define MTP_IND_MODE_PosH                                   28

/*!< 0x0074 MISCREG2_MMW_FRAME_CTRL5
*/
#define MT_MASK_Msk                                         (0x0F << MT_MASK_Pos)
#define MT_MASK_Pos                                         24
#define MT_MASK_PosL                                        24
#define MT_MASK_PosH                                        27

/*!< 0x0074 MISCREG2_MMW_FRAME_CTRL5
*/
#define MTP_IND_NUM_Msk                                     (0xFF << MTP_IND_NUM_Pos)
#define MTP_IND_NUM_Pos                                     12
#define MTP_IND_NUM_PosL                                    12
#define MTP_IND_NUM_PosH                                    23

/*!< 0x0074 MISCREG2_MMW_FRAME_CTRL5
*/
#define MTP_NEG_NUM_Msk                                     (0xFF << MTP_NEG_NUM_Pos)
#define MTP_NEG_NUM_Pos                                     0
#define MTP_NEG_NUM_PosL                                    0
#define MTP_NEG_NUM_PosH                                    11


/*!< 0x0078 MISCREG2_MMW_FRAME_CTRL6
*/
#define MTP_NEG_DEBOUNCE_NUM_Msk                            (0xFFFF << MTP_NEG_DEBOUNCE_NUM_Pos)
#define MTP_NEG_DEBOUNCE_NUM_Pos                            16
#define MTP_NEG_DEBOUNCE_NUM_PosL                           16
#define MTP_NEG_DEBOUNCE_NUM_PosH                           31

/*!< 0x0078 MISCREG2_MMW_FRAME_CTRL6
*/
#define MTP_IND_DEBOUNCE_NUM_Msk                            (0xFFFF << MTP_IND_DEBOUNCE_NUM_Pos)
#define MTP_IND_DEBOUNCE_NUM_Pos                            0
#define MTP_IND_DEBOUNCE_NUM_PosL                           0
#define MTP_IND_DEBOUNCE_NUM_PosH                           15

/*!< 0x007c MISCREG2_MMW_FRAME_CTRL7
*/
#define INTERVAL_END_IND_CLEAR_Msk                          (0x01 << INTERVAL_END_IND_CLEAR_Pos)
#define INTERVAL_END_IND_CLEAR_Pos                          3
#define INTERVAL_END_IND_CLEAR_PosL                         3
#define INTERVAL_END_IND_CLEAR_PosH                         3

/*!< 0x007c MISCREG2_MMW_FRAME_CTRL7
*/
#define MDT_IND_CLEAR_Msk                                   (0x01 << MDT_IND_CLEAR_Pos)
#define MDT_IND_CLEAR_Pos                                   2
#define MDT_IND_CLEAR_PosL                                  2
#define MDT_IND_CLEAR_PosH                                  2

/*!< 0x007c MISCREG2_MMW_FRAME_CTRL7
*/
#define FFT_IND_CLEAR_Msk                                   (0x01 << FFT_IND_CLEAR_Pos)
#define FFT_IND_CLEAR_Pos                                   1
#define FFT_IND_CLEAR_PosL                                  1
#define FFT_IND_CLEAR_PosH                                  1

/*!< 0x007c MISCREG2_MMW_FRAME_CTRL7
*/
#define MTP_IND_CLEAR_Msk                                   (0x01 << MTP_IND_CLEAR_Pos)
#define MTP_IND_CLEAR_Pos                                   0
#define MTP_IND_CLEAR_PosL                                  0
#define MTP_IND_CLEAR_PosH                                  0

/*!< 0x0080 MMW_FRAME_CTRL8
*null
*/
#define WU_CPU_INTERVAL_ID_Msk                              (0x1FFFF << WU_CPU_INTERVAL_ID_Pos)
#define WU_CPU_INTERVAL_ID_Pos                              0
#define WU_CPU_INTERVAL_ID_PosL                             0
#define WU_CPU_INTERVAL_ID_PosH                             16

/*!< 0x0084 MISCREG2_MMW_FRAME_CTRL9
*/
#define MTP_NEG_IND_FLAG_Msk                                (0x01 << MTP_NEG_IND_FLAG_Pos)
#define MTP_NEG_IND_FLAG_Pos                                4
#define MTP_NEG_IND_FLAG_PosL                               4
#define MTP_NEG_IND_FLAG_PosH                               4

/*!< 0x0084 MISCREG2_MMW_FRAME_CTRL9
*/
#define INTERVAL_END_IND_FLAG_Msk                           (0x01 << INTERVAL_END_IND_FLAG_Pos)
#define INTERVAL_END_IND_FLAG_Pos                           3
#define INTERVAL_END_IND_FLAG_PosL                          3
#define INTERVAL_END_IND_FLAG_PosH                          3

/*!< 0x0084 MISCREG2_MMW_FRAME_CTRL9
*/
#define MDT_IND_FLAG_Msk                                    (0x01 << MDT_IND_FLAG_Pos)
#define MDT_IND_FLAG_Pos                                    2
#define MDT_IND_FLAG_PosL                                   2
#define MDT_IND_FLAG_PosH                                   2

/*!< 0x0084 MISCREG2_MMW_FRAME_CTRL9
*/
#define FFT_IND_FLAG_Msk                                    (0x01 << FFT_IND_FLAG_Pos)
#define FFT_IND_FLAG_Pos                                    1
#define FFT_IND_FLAG_PosL                                   1
#define FFT_IND_FLAG_PosH                                   1

/*!< 0x0084 MISCREG2_MMW_FRAME_CTRL9
*/
#define MTP_IND_FLAG_Msk                                    (0x01 << MTP_IND_FLAG_Pos)
#define MTP_IND_FLAG_Pos                                    0
#define MTP_IND_FLAG_PosL                                   0
#define MTP_IND_FLAG_PosH                                   0

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_LPF_SWC_EN_Msk                                 (0x01 << DPLL_LPF_SWC_EN_Pos)
#define DPLL_LPF_SWC_EN_Pos                                 30
#define DPLL_LPF_SWC_EN_PosL                                30
#define DPLL_LPF_SWC_EN_PosH                                30

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_VTUNE_TEST_EN_Msk                              (0x01 << DPLL_VTUNE_TEST_EN_Pos)
#define DPLL_VTUNE_TEST_EN_Pos                              29
#define DPLL_VTUNE_TEST_EN_PosL                             29
#define DPLL_VTUNE_TEST_EN_PosH                             29

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_CRYSTAL_LESS_EN_Msk                            (0x01 << DPLL_CRYSTAL_LESS_EN_Pos)
#define DPLL_CRYSTAL_LESS_EN_Pos                            28
#define DPLL_CRYSTAL_LESS_EN_PosL                           28
#define DPLL_CRYSTAL_LESS_EN_PosH                           28

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_CML2CMOS_ITRM_Msk                              (0x03 << DPLL_CML2CMOS_ITRM_Pos)
#define DPLL_CML2CMOS_ITRM_Pos                              20
#define DPLL_CML2CMOS_ITRM_PosL                             20
#define DPLL_CML2CMOS_ITRM_PosH                             21

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_VCO_ITRIM_Msk                                  (0x0F << DPLL_VCO_ITRIM_Pos)
#define DPLL_VCO_ITRIM_Pos                                  16
#define DPLL_VCO_ITRIM_PosL                                 16
#define DPLL_VCO_ITRIM_PosH                                 19

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_PFD_DELAY_TRIM_Msk                             (0x03 << DPLL_PFD_DELAY_TRIM_Pos)
#define DPLL_PFD_DELAY_TRIM_Pos                             13
#define DPLL_PFD_DELAY_TRIM_PosL                            13
#define DPLL_PFD_DELAY_TRIM_PosH                            14

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_LPF_SWR3_Msk                                   (0x03 << DPLL_LPF_SWR3_Pos)
#define DPLL_LPF_SWR3_Pos                                   11
#define DPLL_LPF_SWR3_PosL                                  11
#define DPLL_LPF_SWR3_PosH                                  12

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_LPF_SWR2_Msk                                   (0x07 << DPLL_LPF_SWR2_Pos)
#define DPLL_LPF_SWR2_Pos                                   8
#define DPLL_LPF_SWR2_PosL                                  8
#define DPLL_LPF_SWR2_PosH                                  10

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_CP_SPLIT_ITRIM_Msk                             (0x03 << DPLL_CP_SPLIT_ITRIM_Pos)
#define DPLL_CP_SPLIT_ITRIM_Pos                             4
#define DPLL_CP_SPLIT_ITRIM_PosL                            4
#define DPLL_CP_SPLIT_ITRIM_PosH                            5

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_CP_SPLIT_EN_Msk                                (0x01 << DPLL_CP_SPLIT_EN_Pos)
#define DPLL_CP_SPLIT_EN_Pos                                3
#define DPLL_CP_SPLIT_EN_PosL                               3
#define DPLL_CP_SPLIT_EN_PosH                               3

/*!< 0x00d0 MISCREG2_DPLL_CTRL0
*/
#define DPLL_CP_ITRIM_Msk                                   (0x07 << DPLL_CP_ITRIM_Pos)
#define DPLL_CP_ITRIM_Pos                                   0
#define DPLL_CP_ITRIM_PosL                                  0
#define DPLL_CP_ITRIM_PosH                                  2

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_FCW_OFFSET_SUB_Msk                         (0x01 << DPLL_AFC_FCW_OFFSET_SUB_Pos)
#define DPLL_AFC_FCW_OFFSET_SUB_Pos                         30
#define DPLL_AFC_FCW_OFFSET_SUB_PosL                        30
#define DPLL_AFC_FCW_OFFSET_SUB_PosH                        30

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_FCW_OFFSET_Msk                             (0x3FF << DPLL_AFC_FCW_OFFSET < 9:0 > _Pos)
#define DPLL_AFC_FCW_OFFSET_Pos                             20
#define DPLL_AFC_FCW_OFFSET_PosL                            20
#define DPLL_AFC_FCW_OFFSET_PosH                            29

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_VREF_TRIM_Msk                              (0x07 << DPLL_AFC_VREF_TRIM < 2:0 > _Pos)
#define DPLL_AFC_VREF_TRIM_Pos                              17
#define DPLL_AFC_VREF_TRIM_PosL                             17
#define DPLL_AFC_VREF_TRIM_PosH                             19

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_HA_EN_Msk                                  (0x01 << DPLL_AFC_HA_EN_Pos)
#define DPLL_AFC_HA_EN_Pos                                  16
#define DPLL_AFC_HA_EN_PosL                                 16
#define DPLL_AFC_HA_EN_PosH                                 16

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_LPF_BUF_EN_Msk                                 (0x01 << DPLL_LPF_BUF_EN_Pos)
#define DPLL_LPF_BUF_EN_Pos                                 15
#define DPLL_LPF_BUF_EN_PosL                                15
#define DPLL_LPF_BUF_EN_PosH                                15

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_EN_Msk                                     (0x01 << DPLL_AFC_EN_Pos)
#define DPLL_AFC_EN_Pos                                     14
#define DPLL_AFC_EN_PosL                                    14
#define DPLL_AFC_EN_PosH                                    14

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_FCW_Msk                                    (0x3FF << DPLL_AFC_FCW < 9:0 > _Pos)
#define DPLL_AFC_FCW_Pos                                    4
#define DPLL_AFC_FCW_PosL                                   4
#define DPLL_AFC_FCW_PosH                                   13

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_D_Msk                                      (0x07 << DPLL_AFC_D < 2:0 > _Pos)
#define DPLL_AFC_D_Pos                                      1
#define DPLL_AFC_D_PosL                                     1
#define DPLL_AFC_D_PosH                                     3

/*!< 0x00d4 MISCREG2_DPLL_CTRL1
*/
#define DPLL_AFC_BYPASS_Msk                                 (0x01 << DPLL_AFC_BYPASS_Pos)
#define DPLL_AFC_BYPASS_Pos                                 0
#define DPLL_AFC_BYPASS_PosL                                0
#define DPLL_AFC_BYPASS_PosH                                0


/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define AMC_RSTN_Msk                                        (0x01 << AMC_RSTN_Pos)
#define AMC_RSTN_Pos                                        25
#define AMC_RSTN_PosL                                       25
#define AMC_RSTN_PosH                                       25

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define AFC_RSTN_Msk                                        (0x01 << AFC_RSTN_Pos)
#define AFC_RSTN_Pos                                        24
#define AFC_RSTN_PosL                                       24
#define AFC_RSTN_PosH                                       24

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define AFC_LOCK_NUM_Msk                                    (0x0F << AFC_LOCK_NUM_Pos)
#define AFC_LOCK_NUM_Pos                                    16
#define AFC_LOCK_NUM_PosL                                   16
#define AFC_LOCK_NUM_PosH                                   19

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_DIG_LDO_BYPASS_EN_Msk                          (0x01 << DPLL_DIG_LDO_BYPASS_EN_Pos)
#define DPLL_DIG_LDO_BYPASS_EN_Pos                          13
#define DPLL_DIG_LDO_BYPASS_EN_PosL                         13
#define DPLL_DIG_LDO_BYPASS_EN_PosH                         13

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_VCO_LDO_BYPASS_HV_EN_Msk                       (0x01 << DPLL_VCO_LDO_BYPASS_HV_EN_Pos)
#define DPLL_VCO_LDO_BYPASS_HV_EN_Pos                       11
#define DPLL_VCO_LDO_BYPASS_HV_EN_PosL                      11
#define DPLL_VCO_LDO_BYPASS_HV_EN_PosH                      11

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_VCO_LDO_BYPASS_LV_EN_Msk                       (0x01 << DPLL_VCO_LDO_BYPASS_LV_EN_Pos)
#define DPLL_VCO_LDO_BYPASS_LV_EN_Pos                       10
#define DPLL_VCO_LDO_BYPASS_LV_EN_PosL                      10
#define DPLL_VCO_LDO_BYPASS_LV_EN_PosH                      10

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_VCO_LDO_TEST_HV_EN_Msk                         (0x01 << DPLL_VCO_LDO_TEST_HV_EN_Pos)
#define DPLL_VCO_LDO_TEST_HV_EN_Pos                         9
#define DPLL_VCO_LDO_TEST_HV_EN_PosL                        9
#define DPLL_VCO_LDO_TEST_HV_EN_PosH                        9

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_VCO_LDO_TEST_LV_EN_Msk                         (0x01 << DPLL_VCO_LDO_TEST_LV_EN_Pos)
#define DPLL_VCO_LDO_TEST_LV_EN_Pos                         8
#define DPLL_VCO_LDO_TEST_LV_EN_PosL                        8
#define DPLL_VCO_LDO_TEST_LV_EN_PosH                        8

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_AMC_VREF_TRIM_Msk                              (0x0F << DPLL_AMC_VREF_TRIM_Pos)
#define DPLL_AMC_VREF_TRIM_Pos                              4
#define DPLL_AMC_VREF_TRIM_PosL                             4
#define DPLL_AMC_VREF_TRIM_PosH                             7

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_AMC_EN_Msk                                     (0x01 << DPLL_AMC_EN_Pos)
#define DPLL_AMC_EN_Pos                                     1
#define DPLL_AMC_EN_PosL                                    1
#define DPLL_AMC_EN_PosH                                    1

/*!< 0x00d8 MISCREG2_DPLL_CTRL2
*/
#define DPLL_AMC_BYPASS_Msk                                 (0x01 << DPLL_AMC_BYPASS_Pos)
#define DPLL_AMC_BYPASS_Pos                                 0
#define DPLL_AMC_BYPASS_PosL                                0
#define DPLL_AMC_BYPASS_PosH                                0


/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_DIG_LDO_TRIM_Msk                               (0x0F << DPLL_DIG_LDO_TRIM_Pos)
#define DPLL_DIG_LDO_TRIM_Pos                               28
#define DPLL_DIG_LDO_TRIM_PosL                              28
#define DPLL_DIG_LDO_TRIM_PosH                              31

/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_DIG_LDO_TEST_EN_Msk                            (0x01 << DPLL_DIG_LDO_TEST_EN_Pos)
#define DPLL_DIG_LDO_TEST_EN_Pos                            25
#define DPLL_DIG_LDO_TEST_EN_PosL                           25
#define DPLL_DIG_LDO_TEST_EN_PosH                           25

/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_DIG_LDO_LQ_Msk                                 (0x01 << DPLL_DIG_LDO_LQ_Pos)
#define DPLL_DIG_LDO_LQ_Pos                                 24
#define DPLL_DIG_LDO_LQ_PosL                                24
#define DPLL_DIG_LDO_LQ_PosH                                24

/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_VCO_LDO_S2_TRIM_Msk                            (0x0F << DPLL_VCO_LDO_S2_TRIM_Pos)
#define DPLL_VCO_LDO_S2_TRIM_Pos                            12
#define DPLL_VCO_LDO_S2_TRIM_PosL                           12
#define DPLL_VCO_LDO_S2_TRIM_PosH                           15

/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_VCO_LDO_S1_TRIM_Msk                            (0x0F << DPLL_VCO_LDO_S1_TRIM_Pos)
#define DPLL_VCO_LDO_S1_TRIM_Pos                            8
#define DPLL_VCO_LDO_S1_TRIM_PosL                           8
#define DPLL_VCO_LDO_S1_TRIM_PosH                           11

/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_VCO_LDO_OUT_LV_FS_EN_Msk                       (0x01 << DPLL_VCO_LDO_OUT_LV_FS_EN_Pos)
#define DPLL_VCO_LDO_OUT_LV_FS_EN_Pos                       3
#define DPLL_VCO_LDO_OUT_LV_FS_EN_PosL                      3
#define DPLL_VCO_LDO_OUT_LV_FS_EN_PosH                      3

/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_VCO_LDO_VREF_FS_EN_Msk                         (0x01 << DPLL_VCO_LDO_VREF_FS_EN_Pos)
#define DPLL_VCO_LDO_VREF_FS_EN_Pos                         1
#define DPLL_VCO_LDO_VREF_FS_EN_PosL                        1
#define DPLL_VCO_LDO_VREF_FS_EN_PosH                        1

/*!< 0x00dc MISCREG2_DPLL_CTRL3
*/
#define DPLL_VCO_LDO_LQ_Msk                                 (0x01 << DPLL_VCO_LDO_LQ_Pos)
#define DPLL_VCO_LDO_LQ_Pos                                 0
#define DPLL_VCO_LDO_LQ_PosL                                0
#define DPLL_VCO_LDO_LQ_PosH                                0


/*!< 0x00e0 MISCREG2_DPLL_CTRL4
*/
#define DPLL_AMC2AFC_SETTINGS_Msk                           (0x3F << DPLL_AMC2AFC_SETTINGS_Pos)
#define DPLL_AMC2AFC_SETTINGS_Pos                           24
#define DPLL_AMC2AFC_SETTINGS_PosL                          24
#define DPLL_AMC2AFC_SETTINGS_PosH                          29

/*!< 0x00e0 MISCREG2_DPLL_CTRL4
*/
#define DPLL_EN2AMC_SETTINGS_Msk                            (0x3F << DPLL_EN2AMC_SETTINGS_Pos)
#define DPLL_EN2AMC_SETTINGS_Pos                            16
#define DPLL_EN2AMC_SETTINGS_PosL                           16
#define DPLL_EN2AMC_SETTINGS_PosH                           21

/*!< 0x00e0 MISCREG2_DPLL_CTRL4
*/
#define DPLL_LDOEN2EN_SETTINGS_Msk                          (0xFF << DPLL_LDOEN2EN_SETTINGS_Pos)
#define DPLL_LDOEN2EN_SETTINGS_Pos                          0
#define DPLL_LDOEN2EN_SETTINGS_PosL                         0
#define DPLL_LDOEN2EN_SETTINGS_PosH                         7

/*!< 0x00e4 MISCREG2_DPLL_CTRL5
*/
#define DPLL_AFC2ACTIVE_SETTINGS_Msk                        (0xFF << DPLL_AFC2ACTIVE_SETTINGS_Pos)
#define DPLL_AFC2ACTIVE_SETTINGS_Pos                        0
#define DPLL_AFC2ACTIVE_SETTINGS_PosL                       0
#define DPLL_AFC2ACTIVE_SETTINGS_PosH                       7

/*!< 0x00e8 MISCREG2_DPLL_CTRL6
*null
*/
#define BB_CLK_SEL_STOP_MMW_DWN_EN_Msk                      (0x01 << BB_CLK_SEL_STOP_MMW_DWN_EN_Pos)
#define BB_CLK_SEL_STOP_MMW_DWN_EN_Pos                      1
#define BB_CLK_SEL_STOP_MMW_DWN_EN_PosL                     1
#define BB_CLK_SEL_STOP_MMW_DWN_EN_PosH                     1

/*!< 0x00e8 MISCREG2_DPLL_CTRL6
*null
*/
#define DPLL_BB_CLK_SEL_DIS_Msk                             (0x01 << DPLL_BB_CLK_SEL_DIS_Pos)
#define DPLL_BB_CLK_SEL_DIS_Pos                             0
#define DPLL_BB_CLK_SEL_DIS_PosL                            0
#define DPLL_BB_CLK_SEL_DIS_PosH                            0


/*!< 0x00fc MISCREG2_DPLL_STATUS
*/
#define AFC_CAL_OUT_Msk                                     (0x3FF << AFC_CAL_OUT_Pos)
#define AFC_CAL_OUT_Pos                                     16
#define AFC_CAL_OUT_PosL                                    16
#define AFC_CAL_OUT_PosH                                    25

/*!< 0x00fc MISCREG2_DPLL_STATUS
*/
#define AFC_LOCK_Msk                                        (0x01 << AFC_LOCK_Pos)
#define AFC_LOCK_Pos                                        13
#define AFC_LOCK_PosL                                       13
#define AFC_LOCK_PosH                                       13

/*!< 0x00fc MISCREG2_DPLL_STATUS
*/
#define AMC_LOCK_Msk                                        (0x01 << AMC_LOCK_Pos)
#define AMC_LOCK_Pos                                        12
#define AMC_LOCK_PosL                                       12
#define AMC_LOCK_PosH                                       12

/*!< 0x00fc MISCREG2_DPLL_STATUS
*/
#define AMC_CALOUT_Msk                                      (0x0F << AMC_CALOUT_Pos)
#define AMC_CALOUT_Pos                                      8
#define AMC_CALOUT_PosL                                     8
#define AMC_CALOUT_PosH                                     11

/*!< 0x00fc MISCREG2_DPLL_STATUS
*/
#define DPLL_LDO_READY_Msk                                  (0x01 << DPLL_LDO_READY_Pos)
#define DPLL_LDO_READY_Pos                                  1
#define DPLL_LDO_READY_PosL                                 1
#define DPLL_LDO_READY_PosH                                 1

/*!< 0x00fc MISCREG2_DPLL_STATUS
*/
#define DPLL_ACTIVE_Msk                                     (0x01 << DPLL_ACTIVE_Pos)
#define DPLL_ACTIVE_Pos                                     0
#define DPLL_ACTIVE_PosL                                    0
#define DPLL_ACTIVE_PosH                                    0


/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_OUT_ISOLATION_N_Msk                   (0x01 << DPLL_OVERRIDE_OUT_ISOLATION_N_Pos)
#define DPLL_OVERRIDE_OUT_ISOLATION_N_Pos                   22
#define DPLL_OVERRIDE_OUT_ISOLATION_N_PosL                  22
#define DPLL_OVERRIDE_OUT_ISOLATION_N_PosH                  22

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_IN_ISOLATION_N_Msk                    (0x01 << DPLL_OVERRIDE_IN_ISOLATION_N_Pos)
#define DPLL_OVERRIDE_IN_ISOLATION_N_Pos                    21
#define DPLL_OVERRIDE_IN_ISOLATION_N_PosL                   21
#define DPLL_OVERRIDE_IN_ISOLATION_N_PosH                   21

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_VCO_LDO_VREF_FS_EN_Msk                (0x01 << DPLL_OVERRIDE_VCO_LDO_VREF_FS_EN_Pos)
#define DPLL_OVERRIDE_VCO_LDO_VREF_FS_EN_Pos                20
#define DPLL_OVERRIDE_VCO_LDO_VREF_FS_EN_PosL               20
#define DPLL_OVERRIDE_VCO_LDO_VREF_FS_EN_PosH               20

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_VCO_LDO_OUT_LV_FS_EN_Msk              (0x01 << DPLL_OVERRIDE_VCO_LDO_OUT_LV_FS_EN_Pos)
#define DPLL_OVERRIDE_VCO_LDO_OUT_LV_FS_EN_Pos              19
#define DPLL_OVERRIDE_VCO_LDO_OUT_LV_FS_EN_PosL             19
#define DPLL_OVERRIDE_VCO_LDO_OUT_LV_FS_EN_PosH             19

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_VCO_LDO_LQ_Msk                        (0x01 << DPLL_OVERRIDE_VCO_LDO_LQ_Pos)
#define DPLL_OVERRIDE_VCO_LDO_LQ_Pos                        18
#define DPLL_OVERRIDE_VCO_LDO_LQ_PosL                       18
#define DPLL_OVERRIDE_VCO_LDO_LQ_PosH                       18

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_VCO_LDO_EN_Msk                        (0x01 << DPLL_OVERRIDE_VCO_LDO_EN_Pos)
#define DPLL_OVERRIDE_VCO_LDO_EN_Pos                        17
#define DPLL_OVERRIDE_VCO_LDO_EN_PosL                       17
#define DPLL_OVERRIDE_VCO_LDO_EN_PosH                       17

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_XO_FMCW_EN_Msk                        (0x01 << DPLL_OVERRIDE_XO_FMCW_EN_Pos)
#define DPLL_OVERRIDE_XO_FMCW_EN_Pos                        16
#define DPLL_OVERRIDE_XO_FMCW_EN_PosL                       16
#define DPLL_OVERRIDE_XO_FMCW_EN_PosH                       16

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_VCO_EN_Msk                            (0x01 << DPLL_OVERRIDE_VCO_EN_Pos)
#define DPLL_OVERRIDE_VCO_EN_Pos                            15
#define DPLL_OVERRIDE_VCO_EN_PosL                           15
#define DPLL_OVERRIDE_VCO_EN_PosH                           15

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_PFD_EN_Msk                            (0x01 << DPLL_OVERRIDE_PFD_EN_Pos)
#define DPLL_OVERRIDE_PFD_EN_Pos                            14
#define DPLL_OVERRIDE_PFD_EN_PosL                           14
#define DPLL_OVERRIDE_PFD_EN_PosH                           14

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_FDIV_EN_Msk                           (0x01 << DPLL_OVERRIDE_FDIV_EN_Pos)
#define DPLL_OVERRIDE_FDIV_EN_Pos                           13
#define DPLL_OVERRIDE_FDIV_EN_PosL                          13
#define DPLL_OVERRIDE_FDIV_EN_PosH                          13

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_IB_EN_Msk                             (0x01 << DPLL_OVERRIDE_IB_EN_Pos)
#define DPLL_OVERRIDE_IB_EN_Pos                             12
#define DPLL_OVERRIDE_IB_EN_PosL                            12
#define DPLL_OVERRIDE_IB_EN_PosH                            12

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_LPF_BUF_EN_Msk                        (0x01 << DPLL_OVERRIDE_LPF_BUF_EN_Pos)
#define DPLL_OVERRIDE_LPF_BUF_EN_Pos                        11
#define DPLL_OVERRIDE_LPF_BUF_EN_PosL                       11
#define DPLL_OVERRIDE_LPF_BUF_EN_PosH                       11

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_DIG_LDO_LQ_Msk                        (0x01 << DPLL_OVERRIDE_DIG_LDO_LQ_Pos)
#define DPLL_OVERRIDE_DIG_LDO_LQ_Pos                        10
#define DPLL_OVERRIDE_DIG_LDO_LQ_PosL                       10
#define DPLL_OVERRIDE_DIG_LDO_LQ_PosH                       10

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_DIG_LDO_EN_Msk                        (0x01 << DPLL_OVERRIDE_DIG_LDO_EN_Pos)
#define DPLL_OVERRIDE_DIG_LDO_EN_Pos                        9
#define DPLL_OVERRIDE_DIG_LDO_EN_PosL                       9
#define DPLL_OVERRIDE_DIG_LDO_EN_PosH                       9

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_CP_EN_Msk                             (0x01 << DPLL_OVERRIDE_CP_EN_Pos)
#define DPLL_OVERRIDE_CP_EN_Pos                             8
#define DPLL_OVERRIDE_CP_EN_PosL                            8
#define DPLL_OVERRIDE_CP_EN_PosH                            8

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_AMC_RSTN_Msk                          (0x01 << DPLL_OVERRIDE_AMC_RSTN_Pos)
#define DPLL_OVERRIDE_AMC_RSTN_Pos                          7
#define DPLL_OVERRIDE_AMC_RSTN_PosL                         7
#define DPLL_OVERRIDE_AMC_RSTN_PosH                         7

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_AMC_EN_Msk                            (0x01 << DPLL_OVERRIDE_AMC_EN_Pos)
#define DPLL_OVERRIDE_AMC_EN_Pos                            6
#define DPLL_OVERRIDE_AMC_EN_PosL                           6
#define DPLL_OVERRIDE_AMC_EN_PosH                           6

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_AFC_RSTN_Msk                          (0x01 << DPLL_OVERRIDE_AFC_RSTN_Pos)
#define DPLL_OVERRIDE_AFC_RSTN_Pos                          5
#define DPLL_OVERRIDE_AFC_RSTN_PosL                         5
#define DPLL_OVERRIDE_AFC_RSTN_PosH                         5

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVERRIDE_AFC_EN_Msk                            (0x01 << DPLL_OVERRIDE_AFC_EN_Pos)
#define DPLL_OVERRIDE_AFC_EN_Pos                            4
#define DPLL_OVERRIDE_AFC_EN_PosL                           4
#define DPLL_OVERRIDE_AFC_EN_PosH                           4

/*!< 0x0100 MISCREG2_DPLL_OVERRIDE0
*null
*/
#define DPLL_OVRRIDE_EN_Msk                                 (0x01 << DPLL_OVRRIDE_EN_Pos)
#define DPLL_OVRRIDE_EN_Pos                                 0
#define DPLL_OVRRIDE_EN_PosL                                0
#define DPLL_OVRRIDE_EN_PosH                                0


/*!< 0x0110 MMW_SPLL_CTRL
*/
#define MMW_SPLL_EN_Msk                                     (0x01 << MMW_SPLL_EN_Pos)
#define MMW_SPLL_EN_Pos                                     1
#define MMW_SPLL_EN_PosL                                    1
#define MMW_SPLL_EN_PosH                                    1

/*!< 0x0110 MMW_SPLL_CTRL
*/
#define MMW_SPLL_LDO_EN_Msk                                 (0x01 << MMW_SPLL_LDO_EN_Pos)
#define MMW_SPLL_LDO_EN_Pos                                 0
#define MMW_SPLL_LDO_EN_PosL                                0
#define MMW_SPLL_LDO_EN_PosH                                0


/*!< 0x0120 MISCREG2_FREE_RUNNING_CNT_CTRL
*null
*/
#define FORCE_SYNC_Msk                                      (0x01 << FORCE_SYNC_Pos)
#define FORCE_SYNC_Pos                                      31
#define FORCE_SYNC_PosL                                     31
#define FORCE_SYNC_PosH                                     31

/*!< 0x0120 MISCREG2_FREE_RUNNING_CNT_CTRL
*null
*/
#define FREE_RUNNING_CNT_EN_Msk                             (0x01 << FREE_RUNNING_CNT_EN_Pos)
#define FREE_RUNNING_CNT_EN_Pos                             0
#define FREE_RUNNING_CNT_EN_PosL                            0
#define FREE_RUNNING_CNT_EN_PosH                            0


/*!< 0x0124 MISCREG2_FREE_RUNNING_CNT_CPTR_CTRL
*null
*/
#define CAPTURE_MODE_Msk                                    (0x03 << CAPTURE_MODE_Pos)
#define CAPTURE_MODE_Pos                                    4
#define CAPTURE_MODE_PosL                                   4
#define CAPTURE_MODE_PosH                                   5

/*!< 0x0124 MISCREG2_FREE_RUNNING_CNT_CPTR_CTRL
*/
#define CAPTURE_SRC_SEL_Msk                                 (0x07 << CAPTURE_SRC_SEL_Pos)
#define CAPTURE_SRC_SEL_Pos                                 1
#define CAPTURE_SRC_SEL_PosL                                1
#define CAPTURE_SRC_SEL_PosH                                3

/*!< 0x0124 MISCREG2_FREE_RUNNING_CNT_CPTR_CTRL
*/
#define FREE_RUNNING_CNT_CPTR_EN_Msk                        (0x01 << FREE_RUNNING_CNT_CPTR_EN_Pos)
#define FREE_RUNNING_CNT_CPTR_EN_Pos                        0
#define FREE_RUNNING_CNT_CPTR_EN_PosL                       0
#define FREE_RUNNING_CNT_CPTR_EN_PosH                       0

/*!< 0x0128 MISCREG2_FREE_RUNNING_CNT_CMP_CTRL
*/
#define CMP_FLAG_Msk                                        (0x01 << CMP_FLAG_Pos)
#define CMP_FLAG_Pos                                        1
#define CMP_FLAG_PosL                                       1
#define CMP_FLAG_PosH                                       1

/*!< 0x0128 MISCREG2_FREE_RUNNING_CNT_CMP_CTRL
*null
*/
#define FREE_RUNNING_CNT_CMP_EN_Msk                         (0x01 << FREE_RUNNING_CNT_CMP_EN_Pos)
#define FREE_RUNNING_CNT_CMP_EN_Pos                         0
#define FREE_RUNNING_CNT_CMP_EN_PosL                        0
#define FREE_RUNNING_CNT_CMP_EN_PosH                        0

/*!< 0x012c MISCREG2_FREE_RUNNING_CNT_CMP_CTRL
*null
*/
#define FREE_RUNNING_CNT_Msk                                (0XFFFFFFFF << FREE_RUNNING_CNT_Pos)
#define FREE_RUNNING_CNT_Pos                                0
#define FREE_RUNNING_CNT_PosL                               0
#define FREE_RUNNING_CNT_PosH                               31

/*!< 0x0130 MISCREG2_FREE_RUNNING_CPTR_VALUE
*null
*/
#define FREE_RUNNING_CNT_CPTR_VALUE_Msk                     (0XFFFFFFFF << FREE_RUNNING_CNT_CPTR_VALUE_Pos)
#define FREE_RUNNING_CNT_CPTR_VALUE_Pos                     0
#define FREE_RUNNING_CNT_CPTR_VALUE_PosL                    0
#define FREE_RUNNING_CNT_CPTR_VALUE_PosH                    31

/*!< 0x0134 MISCREG2_FREE_RUNNING_CMP_VALUE
*/
#define FREE_RUNNING_CNT_CMP_VALUE_Msk                      (0XFFFFFFFF << FREE_RUNNING_CNT_CMP_VALUE_Pos)
#define FREE_RUNNING_CNT_CMP_VALUE_Pos                      0
#define FREE_RUNNING_CNT_CMP_VALUE_PosL                     0
#define FREE_RUNNING_CNT_CMP_VALUE_PosH                     31


/**
* @}
* End of group HW_MISC2
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_MISC_H_ */