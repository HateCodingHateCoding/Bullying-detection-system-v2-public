/************************************************************************
* @file  hw_ccmu2.h
* @brief hw_ccmu2 hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
*************************************************************************
*/
#ifndef _HW_CCMU2_H_
#define _HW_CCMU2_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_CCMU2
 * @{
 * */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */

typedef struct {
    __IOM uint32_t CCMU2_WDG_RST_CTRL;                      /*0x0000,             */
    __IOM uint32_t CCMU2_RESETN_EN;                         /*0x0004,             */
    __IOM uint32_t CCMU2_CPUS_FORCE_RESETN;                 /*0x0008,             */
    __IOM uint32_t CCMU2_CPUF_FORCE_RESETN;                 /*0x000c,             */
    __IOM uint32_t CPUS_PAD_LOCKUP_EN;                      /*0x0010,             */
    __IOM uint32_t CPUF_PAD_LOCKUP_EN;                      /*0x0014,             */
          uint32_t RSVD[4];                                 /*0x0018-0x0024,      */
    __IOM uint32_t CCMU2_CPUS_RST_FLAG;                     /*0x0028,             */
    __IOM uint32_t CCMU2_CPUF_RST_FLAG;                     /*0x002c,             */
    __IOM uint32_t CCMU2_CLK_EN;                            /*0x0030,             */
          uint32_t RSVD1;                                   /*0x0034,             */
    __IOM uint32_t CCMU2_SYSCLK_SRC_SEL0;                   /*0x0038,             */
    __IOM uint32_t CCMU2_SYSCLK_SRC_SEL1;                   /*0x003C,             */
          uint32_t RSVD2[4];                                /*0x0040-0x004C,      */
    __IOM uint32_t CCMU2_SYSCLK_DIV;                        /*0x0050,             */
          uint32_t RSVD3;                                   /*0x0054,             */
    __IOM uint32_t CCMU2_APB1_PCLK_DIV;                     /*0x0058,             */
          uint32_t RSVD4[7];                                /*0x005C-0x0074,      */
    __IOM uint32_t CCMU2_GPIO_FCLK_SEL;                     /*0x0078,             */
          uint32_t RSVD5[9];                                /*0x007C-0x009C,      */
    __IOM uint32_t CCMU2_MCO_CLK_CTRL;                      /*0x00A0,             */
    __IOM uint32_t CCMU2_EFUSEC_CTRL;                       /*0x00A4,             */
    __IOM uint32_t CCMU2_MMWAVE_CTRL;                       /*0x00A8,             */
    __IOM uint32_t CCMU2_FREECNT_CTRL;                      /*0x00AC,             */
    __IOM uint32_t CCMU2_CAN_CTRL;                          /*0x00B0,             */
          uint32_t RSVD6;                                   /*0x00B4,             */
    __IOM uint32_t PWM_CTRL;                                /*0x00B8,             */
} ccmu2_dev_t;

#define CCMU2_DEV                       ((ccmu2_dev_t *)  CCMU2_BASE_ADDR) /*!< 0x4001 E000 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x0000 CCMU2_WDG_RST_CTRL
*null
*/
#define WDG1_RST_CTRL_Msk                                   (0x01 << WDG1_RST_CTRL_Pos)
#define WDG1_RST_CTRL_Pos                                   1
#define WDG1_RST_CTRL_PosL                                  1
#define WDG1_RST_CTRL_PosH                                  1

/*!< 0x0000 CCMU2_WDG_RST_CTRL
*null
*/
#define WDG0_RST_CTRL_Msk                                   (0x01 << WDG0_RST_CTRL_Pos)
#define WDG0_RST_CTRL_Pos                                   0
#define WDG0_RST_CTRL_PosL                                  0
#define WDG0_RST_CTRL_PosH                                  0


/*!< 0x0004 CCMU2_RESETN_EN
*null
*/
#define SYS_RESET_EN_Msk                                    (0x01 << SYS_RESET_EN_Pos)
#define SYS_RESET_EN_Pos                                    31
#define SYS_RESET_EN_PosL                                   31
#define SYS_RESET_EN_PosH                                   31

/*!< 0x0004 CCMU2_RESETN_EN
*null
*/
#define DATACUBE_HRESETN_EN_Msk                             (0x01 << DATACUBE_HRESETN_EN_Pos)
#define DATACUBE_HRESETN_EN_Pos                             13
#define DATACUBE_HRESETN_EN_PosL                            13
#define DATACUBE_HRESETN_EN_PosH                            13

/*!< 0x0004 CCMU2_RESETN_EN
*null
*/
#define SYSCFG_DATACUBE_RESETN_EN_Msk                       (0x01 << SYSCFG_DATACUBE_RESETN_EN_Pos)
#define SYSCFG_DATACUBE_RESETN_EN_Pos                       12
#define SYSCFG_DATACUBE_RESETN_EN_PosL                      12
#define SYSCFG_DATACUBE_RESETN_EN_PosH                      12

/*!< 0x0004 CCMU2_RESETN_EN
*null
*/
#define RFDIG_RESETN_DIS_Msk                                (0x01 << RFDIG_RESETN_DIS_Pos)
#define RFDIG_RESETN_DIS_Pos                                7
#define RFDIG_RESETN_DIS_PosL                               7
#define RFDIG_RESETN_DIS_PosH                               7

/*!< 0x0004 CCMU2_RESETN_EN
*null
*/
#define MMW_RESETN_DIS_Msk                                  (0x01 << MMW_RESETN_DIS_Pos)
#define MMW_RESETN_DIS_Pos                                  5
#define MMW_RESETN_DIS_PosL                                 5
#define MMW_RESETN_DIS_PosH                                 5

/*!< 0x0004 CCMU2_RESETN_EN
*null
*/
#define GPIO_RESETN_EN_Msk                                  (0x01 << GPIO_RESETN_EN_Pos)
#define GPIO_RESETN_EN_Pos                                  2
#define GPIO_RESETN_EN_PosL                                 2
#define GPIO_RESETN_EN_PosH                                 2


/*!< 0x0008 CCMU2_CPUS_FORCE_RESETN
*null
*/
#define SYSCFG_CPUS_FORCE_RESETN_Msk                        (0x01 << SYSCFG_CPUS_FORCE_RESETN_Pos)
#define SYSCFG_CPUS_FORCE_RESETN_Pos                        0
#define SYSCFG_CPUS_FORCE_RESETN_PosL                       0
#define SYSCFG_CPUS_FORCE_RESETN_PosH                       0


/*!< 0x000c CCMU2_CPUF_FORCE_RESETN
*null
*/
#define SYSCFG_CPUF_FORCE_RESETN_Msk                        (0x01 << SYSCFG_CPUF_FORCE_RESETN_Pos)
#define SYSCFG_CPUF_FORCE_RESETN_Pos                        0
#define SYSCFG_CPUF_FORCE_RESETN_PosL                       0
#define SYSCFG_CPUF_FORCE_RESETN_PosH                       0


/*!< 0x0010 CPUS_PAD_LOCKUP_EN
*null
*/
#define CPUS_PAD_LOCKUP_EN_Msk                              (0x01 << CPUS_PAD_LOCKUP_EN_Pos)
#define CPUS_PAD_LOCKUP_EN_Pos                              0
#define CPUS_PAD_LOCKUP_EN_PosL                             0
#define CPUS_PAD_LOCKUP_EN_PosH                             0


/*!< 0x0014 CPUF_PAD_LOCKUP_EN
*null
*/
#define CPUF_PAD_LOCKUP_EN_Msk                              (0x01 << CPUF_PAD_LOCKUP_EN_Pos)
#define CPUF_PAD_LOCKUP_EN_Pos                              0
#define CPUF_PAD_LOCKUP_EN_PosL                             0
#define CPUF_PAD_LOCKUP_EN_PosH                             0


/*!< 0x0028 CCMU2_CPUS_RST_FLAG
*null
*/
#define CPUS_RST_Msk                                        (0x01 << CPUS_RST_Pos)
#define CPUS_RST_Pos                                        5
#define CPUS_RST_PosL                                       5
#define CPUS_RST_PosH                                       5

/*!< 0x0028 CCMU2_CPUS_RST_FLAG
*null
*/
#define FORCE_CPUS_RST_FLAG_Msk                             (0x01 << FORCE_CPUS_RST_FLAG_Pos)
#define FORCE_CPUS_RST_FLAG_Pos                             4
#define FORCE_CPUS_RST_FLAG_PosL                            4
#define FORCE_CPUS_RST_FLAG_PosH                            4

/*!< 0x0028 CCMU2_CPUS_RST_FLAG
*null
*/
#define WDT1_RST_FLAG_Msk                                   (0x01 << WDT1_RST_FLAG_Pos)
#define WDT1_RST_FLAG_Pos                                   3
#define WDT1_RST_FLAG_PosL                                  3
#define WDT1_RST_FLAG_PosH                                  3

/*!< 0x0028 CCMU2_CPUS_RST_FLAG
*null
*/
#define WDT0_RST_FLAG_Msk                                   (0x01 << WDT0_RST_FLAG_Pos)
#define WDT0_RST_FLAG_Pos                                   2
#define WDT0_RST_FLAG_PosL                                  2
#define WDT0_RST_FLAG_PosH                                  2

/*!< 0x0028 CCMU2_CPUS_RST_FLAG
*null
*/
#define SWP32S_RST_FLAG_Msk                                 (0x01 << SWP32S_RST_FLAG_Pos)
#define SWP32S_RST_FLAG_Pos                                 1
#define SWP32S_RST_FLAG_PosL                                1
#define SWP32S_RST_FLAG_PosH                                1

/*!< 0x0028 CCMU2_CPUS_RST_FLAG
*null
*/
#define SOCLDO_RST_FLAG_Msk                                 (0x01 << SOCLDO_RST_FLAG_Pos)
#define SOCLDO_RST_FLAG_Pos                                 0
#define SOCLDO_RST_FLAG_PosL                                0
#define SOCLDO_RST_FLAG_PosH                                0


/*!< 0x002c CCMU2_CPUS_RST_FLAG
*null
*/
#define CPUF_RST_Msk                                        (0x01 << CPUF_RST_Pos)
#define CPUF_RST_Pos                                        5
#define CPUF_RST_PosL                                       5
#define CPUF_RST_PosH                                       5

/*!< 0x002c CCMU2_CPUS_RST_FLAG
*null
*/
#define FORCE_CPUF_RST_FLAG_Msk                             (0x01 << FORCE_CPUF_RST_FLAG_Pos)
#define FORCE_CPUF_RST_FLAG_Pos                             4
#define FORCE_CPUF_RST_FLAG_PosL                            4
#define FORCE_CPUF_RST_FLAG_PosH                            4

/*!< 0x002c CCMU2_CPUS_RST_FLAG
*null
*/
#define WDT1_RST_FLAG_Msk                                   (0x01 << WDT1_RST_FLAG_Pos)
#define WDT1_RST_FLAG_Pos                                   3
#define WDT1_RST_FLAG_PosL                                  3
#define WDT1_RST_FLAG_PosH                                  3

/*!< 0x002c CCMU2_CPUS_RST_FLAG
*null
*/
#define WDT0_RST_FLAG_Msk                                   (0x01 << WDT0_RST_FLAG_Pos)
#define WDT0_RST_FLAG_Pos                                   2
#define WDT0_RST_FLAG_PosL                                  2
#define WDT0_RST_FLAG_PosH                                  2

/*!< 0x002c CCMU2_CPUS_RST_FLAG
*null
*/
#define SWP32F_RST_FLAG_Msk                                 (0x01 << SWP32F_RST_FLAG_Pos)
#define SWP32F_RST_FLAG_Pos                                 1
#define SWP32F_RST_FLAG_PosL                                1
#define SWP32F_RST_FLAG_PosH                                1

/*!< 0x002c CCMU2_CPUS_RST_FLAG
*null
*/
#define SOCLDO_RST_FLAG_Msk                                 (0x01 << SOCLDO_RST_FLAG_Pos)
#define SOCLDO_RST_FLAG_Pos                                 0
#define SOCLDO_RST_FLAG_PosL                                0
#define SOCLDO_RST_FLAG_PosH                                0


/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_DATACUBE_HCLK_EN_Msk                         (0x01 << SYSCFG_DATACUBE_HCLK_EN_Pos)
#define SYSCFG_DATACUBE_HCLK_EN_Pos                         13
#define SYSCFG_DATACUBE_HCLK_EN_PosL                        13
#define SYSCFG_DATACUBE_HCLK_EN_PosH                        13

/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_DATACUBE_FCLK_EN_Msk                         (0x01 << SYSCFG_DATACUBE_FCLK_EN_Pos)
#define SYSCFG_DATACUBE_FCLK_EN_Pos                         12
#define SYSCFG_DATACUBE_FCLK_EN_PosL                        12
#define SYSCFG_DATACUBE_FCLK_EN_PosH                        12

/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_RFDIG_FCLK_EN_Msk                            (0x01 << SYSCFG_RFDIG_FCLK_EN_Pos)
#define SYSCFG_RFDIG_FCLK_EN_Pos                            7
#define SYSCFG_RFDIG_FCLK_EN_PosL                           7
#define SYSCFG_RFDIG_FCLK_EN_PosH                           7

/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_MMW_HCLK_EN_Msk                              (0x01 << SYSCFG_MMW_HCLK_EN_Pos)
#define SYSCFG_MMW_HCLK_EN_Pos                              6
#define SYSCFG_MMW_HCLK_EN_PosL                             6
#define SYSCFG_MMW_HCLK_EN_PosH                             6

/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_MMW_FCLK_EN_Msk                              (0x01 << SYSCFG_MMW_FCLK_EN_Pos)
#define SYSCFG_MMW_FCLK_EN_Pos                              5
#define SYSCFG_MMW_FCLK_EN_PosL                             5
#define SYSCFG_MMW_FCLK_EN_PosH                             5

/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_EFUSECTRL_FCLK_EN_Msk                        (0x01 << SYSCFG_EFUSECTRL_FCLK_EN_Pos)
#define SYSCFG_EFUSECTRL_FCLK_EN_Pos                        4
#define SYSCFG_EFUSECTRL_FCLK_EN_PosL                       4
#define SYSCFG_EFUSECTRL_FCLK_EN_PosH                       4

/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_RCCAL_FCLK_EN_Msk                            (0x01 << SYSCFG_RCCAL_FCLK_EN_Pos)
#define SYSCFG_RCCAL_FCLK_EN_Pos                            3
#define SYSCFG_RCCAL_FCLK_EN_PosL                           3
#define SYSCFG_RCCAL_FCLK_EN_PosH                           3

/*!< 0x0030 CCMU2_CLK_EN
*null
*/
#define SYSCFG_GPIO_FCLK_EN_Msk                             (0x01 << SYSCFG_GPIO_FCLK_EN_Pos)
#define SYSCFG_GPIO_FCLK_EN_Pos                             2
#define SYSCFG_GPIO_FCLK_EN_PosL                            2
#define SYSCFG_GPIO_FCLK_EN_PosH                            2


/*!< 0x0038 CCMU2_SYSCLK_SRC_SEL0
*null
*/
#define SYSCFG_SYSCLK_SRC_SEL0_Msk                          (0x03 << SYSCFG_SYSCLK_SRC_SEL0_Pos)
#define SYSCFG_SYSCLK_SRC_SEL0_Pos                          0
#define SYSCFG_SYSCLK_SRC_SEL0_PosL                         0
#define SYSCFG_SYSCLK_SRC_SEL0_PosH                         1


/*!< 0x003c CCMU2_SYSCLK_SRC_SEL1
*null
*/
#define SYSCFG_SYSCLK_SRC_SEL1_Msk                          (0x03 << SYSCFG_SYSCLK_SRC_SEL1_Pos)
#define SYSCFG_SYSCLK_SRC_SEL1_Pos                          0
#define SYSCFG_SYSCLK_SRC_SEL1_PosL                         0
#define SYSCFG_SYSCLK_SRC_SEL1_PosH                         1


/*!< 0x0050 CCMU2_SYSCLK_DIV
*null
*/
#define SYSCFG_SYS1CLK_DIV_Msk                              (0x03 << SYSCFG_SYS1CLK_DIV_Pos)
#define SYSCFG_SYS1CLK_DIV_Pos                              4
#define SYSCFG_SYS1CLK_DIV_PosL                             4
#define SYSCFG_SYS1CLK_DIV_PosH                             5

/*!< 0x0050 CCMU2_SYSCLK_DIV
*null
*/
#define SYSCFG_SYSCLK_DIV_Msk                               (0x03 << SYSCFG_SYSCLK_DIV_Pos)
#define SYSCFG_SYSCLK_DIV_Pos                               0
#define SYSCFG_SYSCLK_DIV_PosL                              0
#define SYSCFG_SYSCLK_DIV_PosH                              1


/*!< 0x0058 CCMU2_APB1_PCLK_DIV
*null
*/
#define APB1_PCLK_DIV_Msk                                   (0x07 << APB1_PCLK_DIV_Pos)
#define APB1_PCLK_DIV_Pos                                   0
#define APB1_PCLK_DIV_PosL                                  0
#define APB1_PCLK_DIV_PosH                                  2


/*!< 0x0078 CCMU2_GPIO_FCLK_SRC_SEL
*null
*/
#define SYSCFG_GPIO_FCLK_SRC_SEL_Msk                        (0x03 << SYSCFG_GPIO_FCLK_SRC_SEL_Pos)
#define SYSCFG_GPIO_FCLK_SRC_SEL_Pos                        0
#define SYSCFG_GPIO_FCLK_SRC_SEL_PosL                       0
#define SYSCFG_GPIO_FCLK_SRC_SEL_PosH                       1


/*!< 0x00A0 CCMU2_MCO_CLK_CTRL
*null
*/
#define MCO_CLK_DIV_Msk                                     (0x07 << MCO_CLK_DIV_Pos)
#define MCO_CLK_DIV_Pos                                     8
#define MCO_CLK_DIV_PosL                                    8
#define MCO_CLK_DIV_PosH                                    10

/*!< 0x00A0 CCMU2_MCO_CLK_CTRL
*null
*/
#define MCO_CLK_SRC_SEL_Msk                                 (0x0F << MCO_CLK_SRC_SEL_Pos)
#define MCO_CLK_SRC_SEL_Pos                                 4
#define MCO_CLK_SRC_SEL_PosL                                4
#define MCO_CLK_SRC_SEL_PosH                                7

/*!< 0x00A0 CCMU2_MCO_CLK_CTRL
*null
*/
#define MCO_CLK_EN_Msk                                      (0x01 << MCO_CLK_EN_Pos)
#define MCO_CLK_EN_Pos                                      0
#define MCO_CLK_EN_PosL                                     0
#define MCO_CLK_EN_PosH                                     0


/*!< 0x00a4 CCMU2_EFUSEC_CTRL
*null
*/
#define EFUSE_CLK_DIV_Msk                                   (0x07 << EFUSE_CLK_DIV_Pos)
#define EFUSE_CLK_DIV_Pos                                   4
#define EFUSE_CLK_DIV_PosL                                  4
#define EFUSE_CLK_DIV_PosH                                  6

/*!< 0x00a4 CCMU2_EFUSEC_CTRL
*null
*/
#define EFUSE_CLK_SRC_SEL_Msk                               (0x03 << EFUSE_CLK_SRC_SEL_Pos)
#define EFUSE_CLK_SRC_SEL_Pos                               0
#define EFUSE_CLK_SRC_SEL_PosL                              0
#define EFUSE_CLK_SRC_SEL_PosH                              1


/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define MMW_SOC_80M_POSTDIV_Msk                             (0x03 << MMW_SOC_80M_POSTDIV_Pos)
#define MMW_SOC_80M_POSTDIV_Pos                             27
#define MMW_SOC_80M_POSTDIV_PosL                            27
#define MMW_SOC_80M_POSTDIV_PosH                            28

/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define MMW_SOC_80M_SRC_SEL_Msk                             (0x03 << MMW_SOC_80M_SRC_SEL_Pos)
#define MMW_SOC_80M_SRC_SEL_Pos                             25
#define MMW_SOC_80M_SRC_SEL_PosL                            25
#define MMW_SOC_80M_SRC_SEL_PosH                            26

/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define BB_CLK_SEL_EN_Msk                                   (0x01 << BB_CLK_SEL_EN_Pos)
#define BB_CLK_SEL_EN_Pos                                   24
#define BB_CLK_SEL_EN_PosL                                  24
#define BB_CLK_SEL_EN_PosH                                  24

/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define FRAMEC_FCLK_POSTDIV_Msk                             (0x03 << FRAMEC_FCLK_POSTDIV_Pos)
#define FRAMEC_FCLK_POSTDIV_Pos                             18
#define FRAMEC_FCLK_POSTDIV_PosL                            18
#define FRAMEC_FCLK_POSTDIV_PosH                            19

/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define FRAME_FCLK_EN_Msk                                   (0x01 << FRAME_FCLK_EN_Pos)
#define FRAME_FCLK_EN_Pos                                   17
#define FRAME_FCLK_EN_PosL                                  17
#define FRAME_FCLK_EN_PosH                                  17

/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define FRAMEC_FCLK_SEL_Msk                                 (0x01 << FRAMEC_FCLK_SEL_Pos)
#define FRAMEC_FCLK_SEL_Pos                                 16
#define FRAMEC_FCLK_SEL_PosL                                16
#define FRAMEC_FCLK_SEL_PosH                                16

/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define MMWHCLK_DIV_Msk                                     (0x03 << MMWHCLK_DIV_Pos)
#define MMWHCLK_DIV_Pos                                     8
#define MMWHCLK_DIV_PosL                                    8
#define MMWHCLK_DIV_PosH                                    9

/*!< 0x00a8 CCMU2_MMWAVE_CTRL
*null
*/
#define SYSCFG_RFDIG_FCLK_OVRD_Msk                          (0x01 << SYSCFG_RFDIG_FCLK_OVRD_Pos)
#define SYSCFG_RFDIG_FCLK_OVRD_Pos                          7
#define SYSCFG_RFDIG_FCLK_OVRD_PosL                         7
#define SYSCFG_RFDIG_FCLK_OVRD_PosH                         7


/*!< 0x00ac CCMU2_FREECNT_CTRL
*null
*/
#define FREECNT_CLR_Msk                                     (0x01 << FREECNT_CLR_Pos)
#define FREECNT_CLR_Pos                                     5
#define FREECNT_CLR_PosL                                    5
#define FREECNT_CLR_PosH                                    5

/*!< 0x00ac CCMU2_FREECNT_CTRL
*null
*/
#define FREECNT_EN_Msk                                      (0x01 << FREECNT_EN_Pos)
#define FREECNT_EN_Pos                                      4
#define FREECNT_EN_PosL                                     4
#define FREECNT_EN_PosH                                     4

/*!< 0x00ac CCMU2_FREECNT_CTRL
*null
*/
#define FREECNT_CLK_DIV_Msk                                 (0x03 << FREECNT_CLK_DIV_Pos)
#define FREECNT_CLK_DIV_Pos                                 0
#define FREECNT_CLK_DIV_PosL                                0
#define FREECNT_CLK_DIV_PosH                                1


/*!< 0x00b0 CCMU2_CAN_CTRL
*null
*/
#define CAN_IP_SEL_Msk                                      (0x01 << CAN_IP_SEL_Pos)
#define CAN_IP_SEL_Pos                                      31
#define CAN_IP_SEL_PosL                                     31
#define CAN_IP_SEL_PosH                                     31

/*!< 0x00b0 CCMU2_CAN_CTRL
*null
*/
#define CAN_FCLK_POSTDIV_Msk                                (0x03 << CAN_FCLK_POSTDIV_Pos)
#define CAN_FCLK_POSTDIV_Pos                                18
#define CAN_FCLK_POSTDIV_PosL                               18
#define CAN_FCLK_POSTDIV_PosH                               19

/*!< 0x00b0 CCMU2_CAN_CTRL
*null
*/
#define CAN_FCLK_SRC_SEL_Msk                                (0x03 << CAN_FCLK_SRC_SEL_Pos)
#define CAN_FCLK_SRC_SEL_Pos                                16
#define CAN_FCLK_SRC_SEL_PosL                               16
#define CAN_FCLK_SRC_SEL_PosH                               17

/*!< 0x00b0 CCMU2_CAN_CTRL
*null
*/
#define CAN_RESETN_Msk                                      (0x01 << CAN_RESETN_Pos)
#define CAN_RESETN_Pos                                      8
#define CAN_RESETN_PosL                                     8
#define CAN_RESETN_PosH                                     8

/*!< 0x00b0 CCMU2_CAN_CTRL
*null
*/
#define CAN_TMR_FCLK_EN_Msk                                 (0x01 << CAN_TMR_FCLK_EN_Pos)
#define CAN_TMR_FCLK_EN_Pos                                 5
#define CAN_TMR_FCLK_EN_PosL                                5
#define CAN_TMR_FCLK_EN_PosH                                5

/*!< 0x00b0 CCMU2_CAN_CTRL
*null
*/
#define CAN_FCLK_EN_Msk                                     (0x01 << CAN_FCLK_EN_Pos)
#define CAN_FCLK_EN_Pos                                     4
#define CAN_FCLK_EN_PosL                                    4
#define CAN_FCLK_EN_PosH                                    4

/*!< 0x00b0 CCMU2_CAN_CTRL
*null
*/
#define SYSCFG_CAN_FD_EN_Msk                                (0x01 << SYSCFG_CAN_FD_EN_Pos)
#define SYSCFG_CAN_FD_EN_Pos                                0
#define SYSCFG_CAN_FD_EN_PosL                               0
#define SYSCFG_CAN_FD_EN_PosH                               0


/*!< 0x00b8 PWM_CTRL
*null
*/
#define PWM_CLK_POSTDIV_Msk                                 (0x03 << PWM_CLK_POSTDIV_Pos)
#define PWM_CLK_POSTDIV_Pos                                 8
#define PWM_CLK_POSTDIV_PosL                                8
#define PWM_CLK_POSTDIV_PosH                                9

/*!< 0x00b8 PWM_CTRL
*null
*/
#define PWM_CLK_SRC_SEL_Msk                                 (0x03 << PWM_CLK_SRC_SEL_Pos)
#define PWM_CLK_SRC_SEL_Pos                                 4
#define PWM_CLK_SRC_SEL_PosL                                4
#define PWM_CLK_SRC_SEL_PosH                                5

/*!< 0x00b8 PWM_CTRL
*null
*/
#define PWM_CLK_EN_Msk                                      (0x01 << PWM_CLK_EN_Pos)
#define PWM_CLK_EN_Pos                                      1
#define PWM_CLK_EN_PosL                                     1
#define PWM_CLK_EN_PosH                                     1

/*!< 0x00b8 PWM_CTRL
*null
*/
#define PWM_RESETN_EN_Msk                                   (0x01 << PWM_RESETN_EN_Pos)
#define PWM_RESETN_EN_Pos                                   0
#define PWM_RESETN_EN_PosL                                  0
#define PWM_RESETN_EN_PosH                                  0


/**
* @}
* End of group HW_CCMU2
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_CCMU2_H_ */