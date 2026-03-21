/****************************************************************
* @file  GPADC.h
* @brief GPADC hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_GPADC_H_
#define _HW_GPADC_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup GPADC
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __IOM uint32_t GPADC_CTRL;                              /*0x0000,                                               */
    __IOM uint32_t GPADC_CAL_CTRL;                          /*0x0004,                                               */
    __IOM uint32_t GPADC_MODE;                              /*0x0008,                                               */
    __IOM uint32_t GPADC_CH_ENABLE;                         /*0x000c,                                               */
    __IOM uint32_t GPADC_OVER_SMPL_CTRL;                    /*0x0010,                                               */
    __IOM uint32_t GPADC_SMPL_CTRL;                         /*0x0014,                                               */
    __IOM uint32_t GPADC_CCMU_CTRL;                         /*0x0018,                                               */
          uint32_t RSVD;                                    /*0x0020,                                               */
    __IOM uint32_t GPADC_TIMING_CTRL0;                      /*0x0020,                                               */
    __IOM uint32_t GPADC_TIMING_CTRL1;                      /*0x0024,                                               */
    __IOM uint32_t GPADC_TIMING_CTRL2;                      /*0x0028,                                               */
    __IOM uint32_t GPADC_TIMING_CTRL3;                      /*0x002c,                                               */
    __IOM uint32_t GPADC_INT_EN0;                           /*0x0030,                                               */
    __IOM uint32_t GPADC_INT_EN1;                           /*0x0034,                                               */
    __IOM uint32_t GPADC_INT_FLAG_CLR0;                     /*0x0038,                                               */
    __IOM uint32_t GPADC_INT_FLAG_CLR1;                     /*0x003c,                                               */
    __IOM uint32_t GPADC_CMP_LOW[7];                        /*0x0040~0x0058,                                        */
    __IOM uint32_t GPADC_CMP_LOW_VBAT;                      /*0x005c,                                               */
    __IOM uint32_t GPADC_CMP_HIGH[7];                       /*0x0060~0x0078,                                        */
    __IOM uint32_t GPADC_CMP_HIGH_VBAT;                     /*0x007c,                                               */
    __IOM uint32_t GPADC_FIFO_TRIG;                         /*0x0080,                                               */
    __IOM uint32_t GPADC_DMA_CTRL;                          /*0x0084,                                               */
    __IOM uint32_t GPADC_FIFO_FLUSH;                        /*0x0088,                                               */
    __IM  uint32_t GPADC_FIFO_STATUS;                       /*0x008c,                                               */
    __IM  uint32_t GPADC_FIFO_VAL;                          /*0x0090,                                               */
    __IM  uint32_t GPADC_ULTRA_OVER_SMPL_VAL;               /*0x0094,                                               */
          uint32_t RSVD5;                                   /*0x0098,                                               */
    __IM  uint32_t GPADC_CH_VAL[15];                        /*0x009c~0x00d4,                                        */
    __IOM uint32_t GPADC_OVRD_CTRL0;                        /*0x00f0,                                               */
    __IM  uint32_t GPADC_OVRD_CTRL1;                        /*0x00f4,                                               */
    __IM  uint32_t GPADC_CALOUT_SMPL;                       /*0x00f8,                                               */
} gpadc_dev_t;

#define GPADC_DEV                       ((gpadc_dev_t *)  GPADC_BASE_ADDR) /*!< 0x4001 8800 */
/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_LDO_READY_Msk                                 (0x01 << GPADC_LDO_READY_Pos)
#define GPADC_LDO_READY_Pos                                 31
#define GPADC_LDO_READY_PosL                                31
#define GPADC_LDO_READY_PosH                                31

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_INPUT_SHRINK_BYPASS_Msk                       (0x01 << GPADC_INPUT_SHRINK_BYPASS_Pos)
#define GPADC_INPUT_SHRINK_BYPASS_Pos                       24
#define GPADC_INPUT_SHRINK_BYPASS_PosL                      24
#define GPADC_INPUT_SHRINK_BYPASS_PosH                      24

/*!< 0x0000 GPADC_CTRL
*/
#define AON_GPADC_INPUT_SHRINK_Msk                          (0x0F << AON_GPADC_INPUT_SHRINK_Pos)
#define AON_GPADC_INPUT_SHRINK_Pos                          20
#define AON_GPADC_INPUT_SHRINK_PosL                         20
#define AON_GPADC_INPUT_SHRINK_PosH                         23

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_INPUT_BW_Msk                                  (0x0F << GPADC_INPUT_BW_Pos)
#define GPADC_INPUT_BW_Pos                                  16
#define GPADC_INPUT_BW_PosL                                 16
#define GPADC_INPUT_BW_PosH                                 19

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_CLK_DIV_Msk                                   (0x07 << GPADC_CLK_DIV_Pos)
#define GPADC_CLK_DIV_Pos                                   8
#define GPADC_CLK_DIV_PosL                                  8
#define GPADC_CLK_DIV_PosH                                  10

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_RES_Msk                                       (0x03 << GPADC_RES_Pos)
#define GPADC_RES_Pos                                       6
#define GPADC_RES_PosL                                      6
#define GPADC_RES_PosH                                      7

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_TSEL_Msk                                      (0x03 << GPADC_TSEL_Pos)
#define GPADC_TSEL_Pos                                      4
#define GPADC_TSEL_PosL                                     4
#define GPADC_TSEL_PosH                                     5

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_VREF_BG_SEL_Msk                               (0x01 << GPADC_VREF_BG_SEL_Pos)
#define GPADC_VREF_BG_SEL_Pos                               3
#define GPADC_VREF_BG_SEL_PosL                              3
#define GPADC_VREF_BG_SEL_PosH                              3

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_DIFF_MODE_Msk                                 (0x01 << GPADC_DIFF_MODE_Pos)
#define GPADC_DIFF_MODE_Pos                                 2
#define GPADC_DIFF_MODE_PosL                                2
#define GPADC_DIFF_MODE_PosH                                2

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_FAST_MODE_Msk                                 (0x01 << GPADC_FAST_MODE_Pos)
#define GPADC_FAST_MODE_Pos                                 1
#define GPADC_FAST_MODE_PosL                                1
#define GPADC_FAST_MODE_PosH                                1

/*!< 0x0000 GPADC_CTRL
*/
#define GPADC_EN_Msk                                        (0x01 << GPADC_EN_Pos)
#define GPADC_EN_Pos                                        0
#define GPADC_EN_PosL                                       0
#define GPADC_EN_PosH                                       0

/*!< 0x0004 GPADC_CAL_CTRL
*/
#define GPADC_CAL_BYPASS_Msk                                (0x01 << GPADC_CAL_EN_Pos)
#define GPADC_CAL_BYPASS_Pos                                0
#define GPADC_CAL_BYPASS_PosL                               0
#define GPADC_CAL_BYPASS_PosH                               0

/*!< 0x0004 GPADC_CAL_CTRL
*/
#define GPADC_CAL_EN_Msk                                    (0x01 << GPADC_CAL_EN_Pos)
#define GPADC_CAL_EN_Pos                                    1
#define GPADC_CAL_EN_PosL                                   1
#define GPADC_CAL_EN_PosH                                   1


/*!< 0x0004 GPADC_CAL_CTRL
* #define GPADC_CALOUT_Msk                                    (0x7F << GPADC_CALOUT_Pos)
* #define GPADC_CALOUT_Pos                                    16
* #define GPADC_CALOUT_PosL                                   16
* #define GPADC_CALOUT_PosH                                   22
*/

/*!< 0x0004 GPADC_CAL_CTRL。
*/
#define GPADC_CAL_IN_Msk                                    (0x7F << GPADC_CAL_IN_Pos)
#define GPADC_CAL_IN_Pos                                    8
#define GPADC_CAL_IN_PosL                                   8
#define GPADC_CAL_IN_PosH                                   14


/*!< 0x0008 GPADC_MODE
*/
#define GPADC_MODE_SEL_Msk                                  (0x03 << GPADC_MODE_SEL_Pos)
#define GPADC_MODE_SEL_Pos                                  0
#define GPADC_MODE_SEL_PosL                                 0
#define GPADC_MODE_SEL_PosH                                 1


/*!< 0x000c GPADC_CH_ENABLE
*/
#define GPADC_CH_EN_Msk                                     (0x3ff << GPADC_CH_EN_Pos)
#define GPADC_CH_EN_Pos                                     0
#define GPADC_CH_EN_PosL                                    0
#define GPADC_CH_EN_PosH                                    9

/*!< 0x0010 GPADC_OVER_SMPL_CTRL
*/
#define GPADC_ULTRA_OVER_SMPL_SEL_Msk                       (0x1F << GPADC_ULTRA_OVER_SMPL_SEL_Pos)
#define GPADC_ULTRA_OVER_SMPL_SEL_Pos                       9
#define GPADC_ULTRA_OVER_SMPL_SEL_PosL                      9
#define GPADC_ULTRA_OVER_SMPL_SEL_PosH                      13

/*!< 0x0010 GPADC_OVER_SMPL_CTRL
*/
#define GPADC_ULTRA_OVER_SMPL_EN_Msk                        (0x01 << GPADC_ULTRA_OVER_SMPL_EN_Pos)
#define GPADC_ULTRA_OVER_SMPL_EN_Pos                        8
#define GPADC_ULTRA_OVER_SMPL_EN_PosL                       8
#define GPADC_ULTRA_OVER_SMPL_EN_PosH                       8

/*!< 0x0010 GPADC_OVER_SMPL_CTRL
*null
*/
#define GPADC_OVER_SMPL_SEL_Msk                             (0x03 << GPADC_OVER_SMPL_SEL_Pos)
#define GPADC_OVER_SMPL_SEL_Pos                             1
#define GPADC_OVER_SMPL_SEL_PosL                            1
#define GPADC_OVER_SMPL_SEL_PosH                            2

/*!< 0x0010 GPADC_OVER_SMPL_CTRL
*/
#define GPADC_OVER_SMPL_EN_Msk                              (0x01 << GPADC_OVER_SMPL_EN_Pos)
#define GPADC_OVER_SMPL_EN_Pos                              0
#define GPADC_OVER_SMPL_EN_PosL                             0
#define GPADC_OVER_SMPL_EN_PosH                             0


/*!< 0x0014 GPADC_SMPL_CTRL
*null
*/
#define GPADC_SMPL_DISCARD_NUM_Msk                          (0x07 << GPADC_SMPL_DISCARD_NUM_Pos)
#define GPADC_SMPL_DISCARD_NUM_Pos                          0
#define GPADC_SMPL_DISCARD_NUM_PosL                         0
#define GPADC_SMPL_DISCARD_NUM_PosH                         2


/*!< 0x0018 GPADC_CCMU_CTRL
*null
*/
#define GPADC_RESETN_Msk                                    (0x01 << GPADC_RESETN_Pos)
#define GPADC_RESETN_Pos                                    17
#define GPADC_RESETN_PosL                                   17
#define GPADC_RESETN_PosH                                   17

/*!< 0x0018 GPADC_CCMU_CTRL
*null
*/
#define GPADC_FCLK_EN_Msk                                   (0x01 << GPADC_FCLK_EN_Pos)
#define GPADC_FCLK_EN_Pos                                   16
#define GPADC_FCLK_EN_PosL                                  16
#define GPADC_FCLK_EN_PosH                                  16

/*!< 0x0018 GPADC_CCMU_CTRL
*null
*/
#define GPADC_FCLK_POSTDIV_Msk                              (0x07 << GPADC_FCLK_POSTDIV_Pos)
#define GPADC_FCLK_POSTDIV_Pos                              8
#define GPADC_FCLK_POSTDIV_PosL                             8
#define GPADC_FCLK_POSTDIV_PosH                             10

/*!< 0x0018 GPADC_CCMU_CTRL
*/
#define GPADC_FCLK_SRC_SEL_Msk                              (0x03 << GPADC_FCLK_SRC_SEL_Pos)
#define GPADC_FCLK_SRC_SEL_Pos                              0
#define GPADC_FCLK_SRC_SEL_PosL                             0
#define GPADC_FCLK_SRC_SEL_PosH                             1

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define GPADC_PHASE2_SETTLED_EN_Msk                         (0x01 << GPADC_PHASE2_SETTLED_EN_Pos)
#define GPADC_PHASE2_SETTLED_EN_Pos                         28
#define GPADC_PHASE2_SETTLED_EN_PosL                        28
#define GPADC_PHASE2_SETTLED_EN_PosH                        28

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define GPADC_PHASE1_SETTLED_EN_Msk                         (0x01 << GPADC_PHASE1_SETTLED_EN_Pos)
#define GPADC_PHASE1_SETTLED_EN_Pos                         27
#define GPADC_PHASE1_SETTLED_EN_PosL                        27
#define GPADC_PHASE1_SETTLED_EN_PosH                        27

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define GPADC_PHASE2_LDO_READY_EN_Msk                       (0x01 << GPADC_PHASE2_LDO_READY_EN_Pos)
#define GPADC_PHASE2_LDO_READY_EN_Pos                       26
#define GPADC_PHASE2_LDO_READY_EN_PosL                      26
#define GPADC_PHASE2_LDO_READY_EN_PosH                      26

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define GPADC_PHASE1_LDO_READY_EN_Msk                       (0x01 << GPADC_PHASE1_LDO_READY_EN_Pos)
#define GPADC_PHASE1_LDO_READY_EN_Pos                       25
#define GPADC_PHASE1_LDO_READY_EN_PosL                      25
#define GPADC_PHASE1_LDO_READY_EN_PosH                      25

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define SMPL_FINISH_SIGNAL_LOW_EN_Msk                       (0x1F << SMPL_FINISH_SIGNAL_LOW_EN_Pos)
#define SMPL_FINISH_SIGNAL_LOW_EN_Pos                       20
#define SMPL_FINISH_SIGNAL_LOW_EN_PosL                      20
#define SMPL_FINISH_SIGNAL_LOW_EN_PosH                      24

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define START_SIGNAL_HIGH_EN_Msk                            (0x1F << START_SIGNAL_HIGH_EN_Pos)
#define START_SIGNAL_HIGH_EN_Pos                            15
#define START_SIGNAL_HIGH_EN_PosL                           15
#define START_SIGNAL_HIGH_EN_PosH                           19

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define PHASE3_SIGNAL_HIGH_EN_Msk                           (0x1F << PHASE3_SIGNAL_HIGH_EN_Pos)
#define PHASE3_SIGNAL_HIGH_EN_Pos                           10
#define PHASE3_SIGNAL_HIGH_EN_PosL                          10
#define PHASE3_SIGNAL_HIGH_EN_PosH                          14

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define PHASE2_SIGNAL_HIGH_EN_Msk                           (0x1F << PHASE2_SIGNAL_HIGH_EN_Pos)
#define PHASE2_SIGNAL_HIGH_EN_Pos                           5
#define PHASE2_SIGNAL_HIGH_EN_PosL                          5
#define PHASE2_SIGNAL_HIGH_EN_PosH                          9

/*!< 0x0020 GPADC_TIMING_CTRL0
*/
#define PHASE1_SIGNAL_HIGH_EN_Msk                           (0x1F << PHASE1_SIGNAL_HIGH_EN_Pos)
#define PHASE1_SIGNAL_HIGH_EN_Pos                           0
#define PHASE1_SIGNAL_HIGH_EN_PosL                          0
#define PHASE1_SIGNAL_HIGH_EN_PosH                          4


/*!< 0x0024 GPADC_TIMING_CTRL1
*/
#define GPADC_PHASE2_TO_PHASE3_SETTINGS_Msk                 (0xFFFF << GPADC_PHASE2_TO_PHASE3_SETTINGS_Pos)
#define GPADC_PHASE2_TO_PHASE3_SETTINGS_Pos                 16
#define GPADC_PHASE2_TO_PHASE3_SETTINGS_PosL                16
#define GPADC_PHASE2_TO_PHASE3_SETTINGS_PosH                31

/*!< 0x0024 GPADC_TIMING_CTRL1
*/
#define GPADC_PHASE1_TO_PHASE2_SETTINGS_Msk                 (0xFFFF << GPADC_PHASE1_TO_PHASE2_SETTINGS_Pos)
#define GPADC_PHASE1_TO_PHASE2_SETTINGS_Pos                 0
#define GPADC_PHASE1_TO_PHASE2_SETTINGS_PosL                0
#define GPADC_PHASE1_TO_PHASE2_SETTINGS_PosH                15


/*!< 0x0028 GPADC_TIMING_CTRL2
*/
#define GPADC_SMPLF_TO_START_SETTINGS_Msk                   (0x3FF << GPADC_SMPLF_TO_START_SETTINGS_Pos)
#define GPADC_SMPLF_TO_START_SETTINGS_Pos                   16
#define GPADC_SMPLF_TO_START_SETTINGS_PosL                  16
#define GPADC_SMPLF_TO_START_SETTINGS_PosH                  25

/*!< 0x0028 GPADC_TIMING_CTRL2
*/
#define GPADC_START_TO_SMPL_SETTINGS_Msk                    (0x3FF << GPADC_START_TO_SMPL_SETTINGS_Pos)
#define GPADC_START_TO_SMPL_SETTINGS_Pos                    0
#define GPADC_START_TO_SMPL_SETTINGS_PosL                   0
#define GPADC_START_TO_SMPL_SETTINGS_PosH                   9


/*!< 0x002c GPADC_TIMING_CTRL3
*/
#define GPADC_CAL_SETTINGS_Msk                              (0xFFFF << GPADC_CAL_SETTINGS_Pos)
#define GPADC_CAL_SETTINGS_Pos                              0
#define GPADC_CAL_SETTINGS_PosL                             0
#define GPADC_CAL_SETTINGS_PosH                             15

/*!< 0x0030 GPADC_INT_EN0
*/
#define GPADC_CH_CMP_LOW_INT_EN_Msk                          (0x3FF << GPADC_CH_CMP_LOW_INT_EN_Pos)
#define GPADC_CH_CMP_LOW_INT_EN_Pos                          16
#define GPADC_CH_CMP_LOW_INT_EN_PosL                         16
#define GPADC_CH_CMP_LOW_INT_EN_PosH                         26

/*!< 0x0030 GPADC_INT_EN0
*/
#define GPADC_CH_DATA_INT_EN_Msk                            (0x3FF << GPADC_CH_DATA_INT_EN_Pos)
#define GPADC_CH_DATA_INT_EN_Pos                            0
#define GPADC_CH_DATA_INT_EN_PosL                           0
#define GPADC_CH_DATA_INT_EN_PosH                           10


/*!< 0x0034 GPADC_INT_EN0
*/
#define GPADC_ULTRA_OVR_SMPL_DATA_INT_EN_Msk                (0x01 << GPADC_ULTRA_OVR_SMPL_DATA_INT_EN_Pos)
#define GPADC_ULTRA_OVR_SMPL_DATA_INT_EN_Pos                18
#define GPADC_ULTRA_OVR_SMPL_DATA_INT_EN_PosL               18
#define GPADC_ULTRA_OVR_SMPL_DATA_INT_EN_PosH               18

/*!< 0x0034 GPADC_INT_EN0
*/
#define GPADC_FIFO_TRIG_INT_EN_Msk                          (0x01 << GPADC_FIFO_TRIG_INT_EN_Pos)
#define GPADC_FIFO_TRIG_INT_EN_Pos                          17
#define GPADC_FIFO_TRIG_INT_EN_PosL                         17
#define GPADC_FIFO_TRIG_INT_EN_PosH                         17

/*!< 0x0034 GPADC_INT_EN0
*/
#define GPADC_FIFO_DATA_INT_EN_Msk                          (0x01 << GPADC_FIFO_DATA_INT_EN_Pos)
#define GPADC_FIFO_DATA_INT_EN_Pos                          16
#define GPADC_FIFO_DATA_INT_EN_PosL                         16
#define GPADC_FIFO_DATA_INT_EN_PosH                         16

/*!< 0x0034 GPADC_INT_EN0
*/
#define GPADC_CH_CMP_HIGH_INT_EN_Msk                        (0x3FF << GPADC_CH_CMP_HIGH_INT_EN_Pos)
#define GPADC_CH_CMP_HIGH_INT_EN_Pos                        0
#define GPADC_CH_CMP_HIGH_INT_EN_PosL                       0
#define GPADC_CH_CMP_HIGH_INT_EN_PosH                       10

/*!< 0x0038 GPADC_INT_FLAG_CLR0
*/
#define GPADC_CH_CMP_LOW_INT_CLR_Msk                       (0x3FF << GPADC_CH_CMP_HIGH_INT_CLR_Pos)
#define GPADC_CH_CMP_LOW_INT_CLR_Pos                       0
#define GPADC_CH_CMP_LOW_INT_CLR_PosL                      16
#define GPADC_CH_CMP_LOW_INT_CLR_PosH                      26

/*!< 0x0038 GPADC_INT_FLAG_CLR0
*/
#define GPADC_CH_DATA_INT_CLR_Msk                           (0x3FF << GPADC_CH_DATA_INT_CLR_Pos)
#define GPADC_CH_DATA_INT_CLR_Pos                           0
#define GPADC_CH_DATA_INT_CLR_PosL                          0
#define GPADC_CH_DATA_INT_CLR_PosH                          10

/*!< 0x003c GPADC_INT_FLAG_CLR1
*/
#define GPADC_ULTRA_OVR_SMPL_INT_CLR_Msk                    (0x3FF << GPADC_ULTRA_OVR_SMPL_INT_CLR_Pos)
#define GPADC_ULTRA_OVR_SMPL_INT_CLR_Pos                    18
#define GPADC_ULTRA_OVR_SMPL_INT_CLR_PosL                   18
#define GPADC_ULTRA_OVR_SMPL_INT_CLR_PosH                   18

/*!< 0x003c GPADC_INT_FLAG_CLR1
*/
#define GPADC_FIFO_TRIG_INT_CLR_Msk                         (0x01 << GPADC_FIFO_TRIG_INT_CLR_Pos)
#define GPADC_FIFO_TRIG_INT_CLR_Pos                         17
#define GPADC_FIFO_TRIG_INT_CLR_PosL                        17
#define GPADC_FIFO_TRIG_INT_CLR_PosH                        17

/*!< 0x003c GPADC_INT_FLAG_CLR1
*/
#define GPADC_FIFO_DATA_INT_CLR_Msk                         (0x01 << GPADC_FIFO_DATA_INT_CLR_Pos)
#define GPADC_FIFO_DATA_INT_CLR_Pos                         16
#define GPADC_FIFO_DATA_INT_CLR_PosL                        16
#define GPADC_FIFO_DATA_INT_CLR_PosH                        16

/*!< 0x003c GPADC_INT_FLAG_CLR1
*/
#define GPADC_CH_CMP_HIGH_INT_CLR_Msk                       (0x3FF << GPADC_CH_CMP_HIGH_INT_CLR_Pos)
#define GPADC_CH_CMP_HIGH_INT_CLR_Pos                       0
#define GPADC_CH_CMP_HIGH_INT_CLR_PosL                      0
#define GPADC_CH_CMP_HIGH_INT_CLR_PosH                      10

/*!< 0x0040~0x0058 GPADC_CMP_LOW
*/
#define GPADC_CH_CMP_LOW_L_Msk                                (0xFFFF << GPADC_CH_CMP_LOW_L_Pos)
#define GPADC_CH_CMP_LOW_L_Pos                                0
#define GPADC_CH_CMP_LOW_L_PosL                               0
#define GPADC_CH_CMP_LOW_L_PossH                              16

#define GPADC_CH_CMP_LOW_H_Msk                                (0xFFFF << GPADC_CH_CMP_LOW_H_Pos)
#define GPADC_CH_CMP_LOW_H_Pos                                16
#define GPADC_CH_CMP_LOW_H_PosL                               16
#define GPADC_CH_CMP_LOW_H_PossH                              32

/*!< 0x005C GPADC_CMP_LOW_VBAT
*/
#define GPADC_CH_VBAT_LOW_Msk                                (0xFFFF << GPADC_CH_CMP_LOW_L_Pos)
#define GPADC_CH_VBAT_LOW_Pos                                0
#define GPADC_CH_VBAT_LOW_PosL                               0
#define GPADC_CH_VBAT_LOW_PossH                              16

/*!< 0x0060~0x0078 GPADC_CMP_HIGH
*/
#define GPADC_CH_CMP_HIGH_L_Msk                             (0xFFFF << GPADC_CH_CMP_HIGH_L_Pos)
#define GPADC_CH_CMP_HIGH_L_Pos                             0
#define GPADC_CH_CMP_HIGH_L_PosL                            0
#define GPADC_CH_CMP_HIGH_L_PossH                           16

#define GPADC_CH_CMP_HIGH_H_Msk                             (0xFFFF << GPADC_CH_CMP_HIGH_H_Pos)
#define GPADC_CH_CMP_HIGH_H_Pos                             16
#define GPADC_CH_CMP_HIGH_H_PosL                            16
#define GPADC_CH_CMP_HIGH_H_PossH                           32

/*!< 0x007C GPADC_CMP_HIGH_VBAT
*/
#define GPADC_CMP_HIGH_VBAT_Msk                             (0xFFFF << GPADC_CMP_HIGH_VBAT_Pos)
#define GPADC_CMP_HIGH_VBAT_Pos                             0
#define GPADC_CMP_HIGH_VBAT_PosL                            0
#define GPADC_CMP_HIGH_VBAT_PossH                           16

/*!< 0x0080 GPADC_FIFO_TRIG
*/
#define GPADC_FIFO_TRIG_Msk                               (0x3F << GPADC_FIFO_TRIG_Pos)
#define GPADC_FIFO_TRIG_Pos                                0
#define GPADC_FIFO_TRIG_PosL                               0
#define GPADC_FIFO_TRIG_PossH                              5

/*!< 0x0084 GPADC_DMA_CTRL
*/
#define GPADC_DMA_EN_Msk                                    (0x01 << GPADC_DMA_EN_Pos)
#define GPADC_DMA_EN_Pos                                    0
#define GPADC_DMA_EN_PosL                                   0
#define GPADC_DMA_EN_PosH                                   0


/*!< 0x0088 GPADC_FIFO_FLUSH
*/
#define GPADC_FIFO_FLUSH_Msk                                (0x01 << GPADC_FIFO_FLUSH_Pos)
#define GPADC_FIFO_FLUSH_Pos                                0
#define GPADC_FIFO_FLUSH_PosL                               0
#define GPADC_FIFO_FLUSH_PosH                               0


/*!< 0x008c GPADC_FIFO_STATUS
*/
#define GPADC_FIFO_NUM_Msk                                  (0x3F << GPADC_FIFO_NUM_Pos)
#define GPADC_FIFO_NUM_Pos                                  8
#define GPADC_FIFO_NUM_PosL                                 8
#define GPADC_FIFO_NUM_PosH                                 13

/*!< 0x008c GPADC_FIFO_STATUS
*/
#define GPADC_FIFO_FULL_Msk                                 (0x01 << GPADC_FIFO_FULL_Pos)
#define GPADC_FIFO_FULL_Pos                                 1
#define GPADC_FIFO_FULL_PosL                                1
#define GPADC_FIFO_FULL_PosH                                1

/*!< 0x008c GPADC_FIFO_STATUS
*/
#define GPADC_FIFO_EMPTY_Msk                                (0x01 << GPADC_FIFO_EMPTY_Pos)
#define GPADC_FIFO_EMPTY_Pos                                0
#define GPADC_FIFO_EMPTY_PosL                               0
#define GPADC_FIFO_EMPTY_PosH                               0

/*!< 0x0090 GPADC_FIFO_VAL
*/
#define GPADC_FIFO_VAL_Msk                                 (0xFFFFFFFF << GPADC_FIFO_VAL_Pos)
#define GPADC_FIFO_VAL_Pos                                 0
#define GPADC_FIFO_VAL_PosL                                0
#define GPADC_FIFO_VAL_PosH                                31

/*!< 0x0094 GPADC_ULTRA_OVER_SMPL_VAL
*/
#define GPADC_ULTRA_OVER_SMPL_VAL_Msk                      (0xFFFFFFFF << GPADC_FIFO_VAL_Pos)
#define GPADC_ULTRA_OVER_SMPL_VAL_Pos                       0
#define GPADC_ULTRA_OVER_SMPL_VAL_PosL                      0
#define GPADC_ULTRA_OVER_SMPL_VAL_PosH                      31

/*!< 0x009C - 0x00D4 GPADC_CH_VAL
*/
#define GPADC_CH_VAL_Msk                                    (0xFFFFFFFF << GPADC_FIFO_VAL_Pos)
#define GPADC_CH_VAL_Pos                                     0
#define GPADC_CH_VAL_PosL                                    0
#define GPADC_CH_VAL_PosH                                    31

#ifdef CONFIG_GPADC_SUPPORT_OVRD
/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_MSEL_Msk                                 (0x1F << GPADC_OVRD_MSEL_Pos)
#define GPADC_OVRD_MSEL_Pos                                 10
#define GPADC_OVRD_MSEL_PosL                                10
#define GPADC_OVRD_MSEL_PosH                                14

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_VCM_BUF_EN_Msk                           (0x01 << GPADC_OVRD_VCM_BUF_EN_Pos)
#define GPADC_OVRD_VCM_BUF_EN_Pos                           9
#define GPADC_OVRD_VCM_BUF_EN_PosL                          9
#define GPADC_OVRD_VCM_BUF_EN_PosH                          9

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_VBUF_EN_Msk                              (0x01 << GPADC_OVRD_VBUF_EN_Pos)
#define GPADC_OVRD_VBUF_EN_Pos                              8
#define GPADC_OVRD_VBUF_EN_PosL                             8
#define GPADC_OVRD_VBUF_EN_PosH                             8

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_CMP_EN_Msk                               (0x01 << GPADC_OVRD_CMP_EN_Pos)
#define GPADC_OVRD_CMP_EN_Pos                               7
#define GPADC_OVRD_CMP_EN_PosL                              7
#define GPADC_OVRD_CMP_EN_PosH                              7

/*!< 0x00f0 GPADC_OVRD_CTRL0
*GPADC_OVRD_EN
*/
#define GPADC_OVRD_EN_Msk                                   (0x01 << GPADC_OVRD_EN_Pos)
#define GPADC_OVRD_EN_Pos                                   6
#define GPADC_OVRD_EN_PosL                                  6
#define GPADC_OVRD_EN_PosH                                  6

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_CLK_EN_Msk                               (0x01 << GPADC_OVRD_CLK_EN_Pos)
#define GPADC_OVRD_CLK_EN_Pos                               5
#define GPADC_OVRD_CLK_EN_PosL                              5
#define GPADC_OVRD_CLK_EN_PosH                              5

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_CAL_LOGIC_EN_Msk                         (0x01 << GPADC_OVRD_CAL_LOGIC_EN_Pos)
#define GPADC_OVRD_CAL_LOGIC_EN_Pos                         4
#define GPADC_OVRD_CAL_LOGIC_EN_PosL                        4
#define GPADC_OVRD_CAL_LOGIC_EN_PosH                        4

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_CAL_SEL_Msk                              (0x01 << GPADC_OVRD_CAL_SEL_Pos)
#define GPADC_OVRD_CAL_SEL_Pos                              3
#define GPADC_OVRD_CAL_SEL_PosL                             3
#define GPADC_OVRD_CAL_SEL_PosH                             3

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_CAL_DATA_RST_Msk                         (0x01 << GPADC_OVRD_CAL_DATA_RST_Pos)
#define GPADC_OVRD_CAL_DATA_RST_Pos                         2
#define GPADC_OVRD_CAL_DATA_RST_PosL                        2
#define GPADC_OVRD_CAL_DATA_RST_PosH                        2

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_CAL_EN_Msk                               (0x01 << GPADC_OVRD_CAL_EN_Pos)
#define GPADC_OVRD_CAL_EN_Pos                               1
#define GPADC_OVRD_CAL_EN_PosL                              1
#define GPADC_OVRD_CAL_EN_PosH                              1

/*!< 0x00f0 GPADC_OVRD_CTRL0
*/
#define GPADC_OVRD_EN_Msk                                   (0x01 << GPADC_OVRD_EN_Pos)
#define GPADC_OVRD_EN_Pos                                   0
#define GPADC_OVRD_EN_PosL                                  0
#define GPADC_OVRD_EN_PosH                                  0


/*!< 0x00f4 GPADC_OVRD_CTRL1
*/
#define GPADC_CALOUT_Msk                                    (0x7F << GPADC_CALOUT_Pos)
#define GPADC_CALOUT_Pos                                    16
#define GPADC_CALOUT_PosL                                   16
#define GPADC_CALOUT_PosH                                   22

/*!< 0x00f4 GPADC_OVRD_CTRL1
*/
#define GPADC_DOUT_Msk                                      (0xFF << GPADC_DOUT_Pos)
#define GPADC_DOUT_Pos                                      0
#define GPADC_DOUT_PosL                                     0
#define GPADC_DOUT_PosH                                     11

/*!< 0x00f8 GPADC_CAL_
*/
#define GPADC_DOUT_Msk                                      (0xFF << GPADC_DOUT_Pos)
#define GPADC_DOUT_Pos                                      0
#define GPADC_DOUT_PosL                                     0
#define GPADC_DOUT_PosH                                     11

#endif

/*!< 0x00f8 GPADC_CALOUT_SMPL
*/
#define GPADC_CALOUT_SMPL_Msk                               (0x7F << GPADC_CALOUT_SMPL_Pos)
#define GPADC_CALOUT_SMPL_Pos                               0
#define GPADC_CALOUT_SMPL_PosL                              0
#define GPADC_CALOUT_SMPL_PosH                              6

/**
* @}
* End of group HW_GPADC
*/
#endif
#ifdef __cplusplus
}

#endif /* _HW_GPADC_H_ */
