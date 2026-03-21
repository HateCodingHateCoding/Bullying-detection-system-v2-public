/****************************************************************
* @file  hw_rcc.h
* @brief hw_rcc hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_RCC_H_
#define _HW_RCC_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_RCC
 * @{
 * */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */

typedef struct {
    __IOM uint32_t RCC_RC6M_CTRL;                           /*0x0000,             */
    __IOM uint32_t RCC_SPLL_CTRL;                           /*0x0004,             */
    __IOM uint32_t RCC_LSI_CTRL;                            /*0x0008,             */
    __IOM uint32_t RCC_CLK_SRC_SEL;                         /*0x000c,             */
    __IOM uint32_t RCC_MSI_CTRL;                            /*0x0010,             */
    __IOM uint32_t RCC_XO32K;                               /*0x0014,             */
    __IOM uint32_t RCC_LPCLK_CTRL;                          /*0x0018,             */
          uint32_t RSVD[2];                                 /*0x001C-0x0020,      */
    __IOM uint32_t FRAC_DIV_CTRL;                           /*0x0024,             */
    __IOM uint32_t FRAC_DIV_PARAM0;                         /*0x0028,             */
    __IOM uint32_t FRAC_DIV_PARAM1;                         /*0x002c,             */
    __IOM uint32_t FRAC_DIV_PARAM2;                         /*0x0030,             */
    __IOM uint32_t CLK32M_IN_CTRL;                          /*0x0034,             */
    __IOM uint32_t MSI_SRC_SEL;                             /*0x0038,             */
    __IOM uint32_t RCC_STATUS;                              /*0x003c,             */
    __IOM uint32_t RCC_FIRST_BOOTADDR;                      /*0x0040,             */
          uint32_t RSVD2[3];                                /*0x0044-0x004C,      */
    __IOM uint32_t RCC_RST_FLAG;                            /*0x0050,             */
          uint32_t RSVD3[7];                                /*0x0054-0x06c        */
    __IOM uint32_t RCC_SYS0_BOOTFLAG;                       /*0x0070,             */
    __IOM uint32_t RCC_SYS0_BOOTADDR;                       /*0x0074,             */
    __IOM uint32_t RCC_SYS0_BOOTPARAM;                      /*0x0078,             */
          uint32_t RSVD4[5];                                /*0x007C-0x008C,      */
    __IOM uint32_t RCC_SYS1_BOOTFLAG;                       /*0x0090,             */
    __IOM uint32_t RCC_SYS1_BOOTADDR;                       /*0x0094,             */
    __IOM uint32_t RCC_SYS1_BOOTPARAM;                      /*0x0098,             */
          uint32_t RSVD5[6];                                /*0x009C-0x00B0,      */
    __IOM uint32_t RCC_SPECIAL_REG[3];                      /*0x00B4-0x00BC,      */
          uint32_t RSVD6[4];                               /*0x00C0-0x00CC,      */
    __IOM uint32_t RCC_CPUS_INTR_MASK;                      /*0x00D0,             */
    __IOM uint32_t RCC_CPUF_INTR_MASK;                      /*0x00D4,             */
} rcc_dev_t;

#define RCC_DEV                         ((rcc_dev_t *)    RCC_BASE_ADDR) /*!< 0x4001 9000 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x0000 RCC_RC6M_CTRL
*null
*/
#define RC6M_FORCE_EN_Msk                                   (0x01 << RC6M_FORCE_EN_Pos)
#define RC6M_FORCE_EN_Pos                                   0
#define RC6M_FORCE_EN_PosL                                  0
#define RC6M_FORCE_EN_PosH                                  0


/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_CDAA_EN_Msk                                    (0x01 << SPLL_CDAA_EN_Pos)
#define SPLL_CDAA_EN_Pos                                    27
#define SPLL_CDAA_EN_PosL                                   27
#define SPLL_CDAA_EN_PosH                                   27

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_CDAA_POSTDIV_Msk                               (0x07 << SPLL_CDAA_POSTDIV_Pos)
#define SPLL_CDAA_POSTDIV_Pos                               24
#define SPLL_CDAA_POSTDIV_PosL                              24
#define SPLL_CDAA_POSTDIV_PosH                              26

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_DEV1_EN_Msk                                    (0x01 << SPLL_DEV1_EN_Pos)
#define SPLL_DEV1_EN_Pos                                    23
#define SPLL_DEV1_EN_PosL                                   23
#define SPLL_DEV1_EN_PosH                                   23

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_DEV1_POSTDIV_Msk                               (0x07 << SPLL_DEV1_POSTDIV_Pos)
#define SPLL_DEV1_POSTDIV_Pos                               20
#define SPLL_DEV1_POSTDIV_PosL                              20
#define SPLL_DEV1_POSTDIV_PosH                              22

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_DEV_EN_Msk                                     (0x01 << SPLL_DEV_EN_Pos)
#define SPLL_DEV_EN_Pos                                     19
#define SPLL_DEV_EN_PosL                                    19
#define SPLL_DEV_EN_PosH                                    19

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_DEV_POSTDIV_Msk                                (0x07 << SPLL_DEV_POSTDIV_Pos)
#define SPLL_DEV_POSTDIV_Pos                                16
#define SPLL_DEV_POSTDIV_PosL                               16
#define SPLL_DEV_POSTDIV_PosH                               18

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_160M_EN_Msk                                    (0x01 << SPLL_160M_EN_Pos)
#define SPLL_160M_EN_Pos                                    15
#define SPLL_160M_EN_PosL                                   15
#define SPLL_160M_EN_PosH                                   15

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_160M_POSTDIV_Msk                               (0x07 << SPLL_160M_POSTDIV_Pos)
#define SPLL_160M_POSTDIV_Pos                               12
#define SPLL_160M_POSTDIV_PosL                              12
#define SPLL_160M_POSTDIV_PosH                              14

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_PREDIV_Msk                                     (0x07 << SPLL_PREDIV_Pos)
#define SPLL_PREDIV_Pos                                     8
#define SPLL_PREDIV_PosL                                    8
#define SPLL_PREDIV_PosH                                    10

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_FCW_Msk                                        (0x3F << SPLL_FCW_Pos)
#define SPLL_FCW_Pos                                        1
#define SPLL_FCW_PosL                                       1
#define SPLL_FCW_PosH                                       6

/*!< 0x0004 RCC_SPLL_CTRL
*null
*/
#define SPLL_SRC_SEL_Msk                                    (0x01 << SPLL_SRC_SEL_Pos)
#define SPLL_SRC_SEL_Pos                                    0
#define SPLL_SRC_SEL_PosL                                   0
#define SPLL_SRC_SEL_PosH                                   0


/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define LSI_IB_BUF_TRIM_Msk                                 (0x0F << LSI_IB_BUF_TRIM_Pos)
#define LSI_IB_BUF_TRIM_Pos                                 24
#define LSI_IB_BUF_TRIM_PosL                                24
#define LSI_IB_BUF_TRIM_PosH                                27

/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define LSI_DECAP1_TRIM_Msk                                 (0x03 << LSI_DECAP1_TRIM_Pos)
#define LSI_DECAP1_TRIM_Pos                                 20
#define LSI_DECAP1_TRIM_PosL                                20
#define LSI_DECAP1_TRIM_PosH                                21

/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define LSI_DECAP2_TRIM_Msk                                 (0x03 << LSI_DECAP2_TRIM_Pos)
#define LSI_DECAP2_TRIM_Pos                                 18
#define LSI_DECAP2_TRIM_PosL                                18
#define LSI_DECAP2_TRIM_PosH                                19

/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define LSI_R_TRIM_Msk                                      (0x03 << LSI_R_TRIM_Pos)
#define LSI_R_TRIM_Pos                                      16
#define LSI_R_TRIM_PosL                                     16
#define LSI_R_TRIM_PosH                                     17

/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define ANA_LSI_TRIM_Msk                                    (0x3F << ANA_LSI_TRIM_Pos)
#define ANA_LSI_TRIM_Pos                                    8
#define ANA_LSI_TRIM_PosL                                   8
#define ANA_LSI_TRIM_PosH                                   13

/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define LP_CLK_DIV_Msk                                      (0x07 << LP_CLK_DIV_Pos)
#define LP_CLK_DIV_Pos                                      4
#define LP_CLK_DIV_PosL                                     4
#define LP_CLK_DIV_PosH                                     6

/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define ANA_LSI_EN_Msk                                      (0x01 << ANA_LSI_EN_Pos)
#define ANA_LSI_EN_Pos                                      1
#define ANA_LSI_EN_PosL                                     1
#define ANA_LSI_EN_PosH                                     1

/*!< 0x0008 RCC_LSI_CTRL
*null
*/
#define LSI_CG_EN_Msk                                       (0x01 << LSI_CG_EN_Pos)
#define LSI_CG_EN_Pos                                       0
#define LSI_CG_EN_PosL                                      0
#define LSI_CG_EN_PosH                                      0


/*!< 0x000c RCC_CLK_SRC_SEL
*null
*/
#define MMFRAME_CLK_EN_Msk                                  (0x01 << MMFRAME_CLK_EN_Pos)
#define MMFRAME_CLK_EN_Pos                                  3
#define MMFRAME_CLK_EN_PosL                                 3
#define MMFRAME_CLK_EN_PosH                                 3

/*!< 0x000c RCC_CLK_SRC_SEL
*null
*/
#define MMFRAME_CLK_SRC_SEL_Msk                             (0x03 << MMFRAME_CLK_SRC_SEL_Pos)
#define MMFRAME_CLK_SRC_SEL_Pos                             1
#define MMFRAME_CLK_SRC_SEL_PosL                            1
#define MMFRAME_CLK_SRC_SEL_PosH                            2


/*!< 0x0010 RCC_MSI_CTRL
*null
*/
#define MSI_CFTRIM_TRIM_Msk                                 (0x1F << MSI_CFTRIM_TRIM_Pos)
#define MSI_CFTRIM_TRIM_Pos                                 24
#define MSI_CFTRIM_TRIM_PosL                                24
#define MSI_CFTRIM_TRIM_PosH                                28

/*!< 0x0010 RCC_MSI_CTRL
*null
*/
#define MSI_FREQ_SEL_Msk                                    (0x0F << MSI_FREQ_SEL_Pos)
#define MSI_FREQ_SEL_Pos                                    16
#define MSI_FREQ_SEL_PosL                                   16
#define MSI_FREQ_SEL_PosH                                   19

/*!< 0x0010 RCC_MSI_CTRL
*null
*/
#define MSI_CAP_TRIM_Msk                                    (0x7F << MSI_CAP_TRIM_Pos)
#define MSI_CAP_TRIM_Pos                                    8
#define MSI_CAP_TRIM_PosL                                   8
#define MSI_CAP_TRIM_PosH                                   14

/*!< 0x0010 RCC_MSI_CTRL
*null
*/
#define MSI_BIAS_TRIM_Msk                                   (0x0F << MSI_BIAS_TRIM_Pos)
#define MSI_BIAS_TRIM_Pos                                   0
#define MSI_BIAS_TRIM_PosL                                  0
#define MSI_BIAS_TRIM_PosH                                  3


/*!< 0x0014 RCC_XO32K
*null
*/
#define XO32K_BIAS_TRIM_Msk                                 (0x03 << XO32K_BIAS_TRIM_Pos)
#define XO32K_BIAS_TRIM_Pos                                 4
#define XO32K_BIAS_TRIM_PosL                                4
#define XO32K_BIAS_TRIM_PosH                                5

/*!< 0x0014 RCC_XO32K
*null
*/
#define XO32K_SRC_SEL_Msk                                   (0x01 << XO32K_SRC_SEL_Pos)
#define XO32K_SRC_SEL_Pos                                   2
#define XO32K_SRC_SEL_PosL                                  2
#define XO32K_SRC_SEL_PosH                                  2

/*!< 0x0014 RCC_XO32K
*null
*/
#define CKIN_32K_EN_Msk                                     (0x01 << CKIN_32K_EN_Pos)
#define CKIN_32K_EN_Pos                                     1
#define CKIN_32K_EN_PosL                                    1
#define CKIN_32K_EN_PosH                                    1

/*!< 0x0014 RCC_XO32K
*null
*/
#define XO32K_EN_Msk                                        (0x01 << XO32K_EN_Pos)
#define XO32K_EN_Pos                                        0
#define XO32K_EN_PosL                                       0
#define XO32K_EN_PosH                                       0


/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LCLKOUT_PIN_DRIVING_Msk                             (0x03 << LCLKOUT_PIN_DRIVING_Pos)
#define LCLKOUT_PIN_DRIVING_Pos                             24
#define LCLKOUT_PIN_DRIVING_PosL                            24
#define LCLKOUT_PIN_DRIVING_PosH                            25

/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LCLKOUT_PINMUX_SEL_Msk                              (0x01 << LCLKOUT_PINMUX_SEL_Pos)
#define LCLKOUT_PINMUX_SEL_Pos                              20
#define LCLKOUT_PINMUX_SEL_PosL                             20
#define LCLKOUT_PINMUX_SEL_PosH                             20

/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LCLKOUT_PINMUX_EN_Msk                               (0x01 << LCLKOUT_PINMUX_EN_Pos)
#define LCLKOUT_PINMUX_EN_Pos                               16
#define LCLKOUT_PINMUX_EN_PosL                              16
#define LCLKOUT_PINMUX_EN_PosH                              16

/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LCLKIN_PINMUX_SRC_SEL_Msk                           (0x01 << LCLKIN_PINMUX_SRC_SEL_Pos)
#define LCLKIN_PINMUX_SRC_SEL_Pos                           12
#define LCLKIN_PINMUX_SRC_SEL_PosL                          12
#define LCLKIN_PINMUX_SRC_SEL_PosH                          12

/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LCLKIN_CG_EN_Msk                                    (0x01 << LCLKIN_CG_EN_Pos)
#define LCLKIN_CG_EN_Pos                                    9
#define LCLKIN_CG_EN_PosL                                   9
#define LCLKIN_CG_EN_PosH                                   9

/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LCLKIN_PINMUX_EN_Msk                                (0x01 << LCLKIN_PINMUX_EN_Pos)
#define LCLKIN_PINMUX_EN_Pos                                8
#define LCLKIN_PINMUX_EN_PosL                               8
#define LCLKIN_PINMUX_EN_PosH                               8

/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LSI_SRC_SEL_Msk                                     (0x01 << LSI_SRC_SEL_Pos)
#define LSI_SRC_SEL_Pos                                     1
#define LSI_SRC_SEL_PosL                                    1
#define LSI_SRC_SEL_PosH                                    1

/*!< 0x0018 RCC_LPCLK_CTRL
*null
*/
#define LPCLK_SRC_SEL_Msk                                   (0x01 << LPCLK_SRC_SEL_Pos)
#define LPCLK_SRC_SEL_Pos                                   0
#define LPCLK_SRC_SEL_PosL                                  0
#define LPCLK_SRC_SEL_PosH                                  0


/*!< 0x0024 FRAC_DIV_CTRL
*null
*/
#define LSI_FRAC_DIV_RESETN_EN_Msk                           (0x01 << LSI_FRAC_DIV_RESETN_EN_Pos)
#define LSI_FRAC_DIV_RESETN_EN_Pos                           1
#define LSI_FRAC_DIV_RESETN_EN_PosL                          1
#define LSI_FRAC_DIV_RESETN_EN_PosH                          1

/*!< 0x0024 FRAC_DIV_CTRL
*null
*/
#define LSI_FRAC_DIV_EN_Msk                                 (0x01 << LSI_FRAC_DIV_EN_Pos)
#define LSI_FRAC_DIV_EN_Pos                                 0
#define LSI_FRAC_DIV_EN_PosL                                0
#define LSI_FRAC_DIV_EN_PosH                                0


/*!< 0x0028 FRAC_DIV_PARAM0
*null
*/
#define QUOTIENT_Msk                                        (0x3FFFFF << QUOTIENT_Pos)
#define QUOTIENT_Pos                                        0
#define QUOTIENT_PosL                                       0
#define QUOTIENT_PosH                                       21


/*!< 0x002c FRAC_DIV_PARAM1
*null
*/
#define REMAINDER_Msk                                       (0x3FFFFF << REMAINDER_Pos)
#define REMAINDER_Pos                                       0
#define REMAINDER_PosL                                      0
#define REMAINDER_PosH                                      21


/*!< 0x0030 FRAC_DIV_PARAM2
*null
*/
#define DENOMINATOR_Msk                                     (0x3FFFFF << DENOMINATOR_Pos)
#define DENOMINATOR_Pos                                     0
#define DENOMINATOR_PosL                                    0
#define DENOMINATOR_PosH                                    21


/*!< 0x0034 CLK32M_IN_CTRL
*null
*/
#define CLK32M_IN_PINMUX_SEL_Msk                            (0x01 << CLK32M_IN_PINMUX_SEL_Pos)
#define CLK32M_IN_PINMUX_SEL_Pos                            4
#define CLK32M_IN_PINMUX_SEL_PosL                           4
#define CLK32M_IN_PINMUX_SEL_PosH                           4

/*!< 0x0034 CLK32M_IN_CTRL
*null
*/
#define CLK32M_IN_PINMUX_EN_Msk                             (0x01 << CLK32M_IN_PINMUX_EN_Pos)
#define CLK32M_IN_PINMUX_EN_Pos                             0
#define CLK32M_IN_PINMUX_EN_PosL                            0
#define CLK32M_IN_PINMUX_EN_PosH                            0

/*!< 0x0038 MSI_SRC_SEL
*null
*/
#define MSI_SRC_SEL_Msk                                     (0x01 << MSI_SRC_SEL_Pos)
#define MSI_SRC_SEL_Pos                                     0
#define MSI_SRC_SEL_PosL                                    0
#define MSI_SRC_SEL_PosH                                    0

/*!< 0x003C RCC_STATUS
*null
*/
#define XO32K_OK_Msk                                     	(0x01 << XO32K_OK_Pos)
#define XO32K_OK_Pos                                     	1
#define XO32K_OK_PosL                                    	1
#define XO32K_OK_PosH                                    	1

/*!< 0x003C RCC_STATUS
*null
*/
#define LSI_OK_Msk                                     		(0x01 << LSI_OK_Pos)
#define LSI_OK_Pos                                     		0
#define LSI_OK_PosL                                    		0
#define LSI_OK_PosH                                    		0

/*!< 0x0040 RCC_FIRST_BOOTADDR
*null
*/
#define RCC_FIRST_BOOTADDR_Msk                              (0XFFFFFFF << RCC_FIRST_BOOTADDR_Pos)
#define RCC_FIRST_BOOTADDR_Pos                              0
#define RCC_FIRST_BOOTADDR_PosL                             0
#define RCC_FIRST_BOOTADDR_PosH                             31


/*!< 0x0050 RCC_RST_FLAG
*null
*/
#define PORRST_FLAG_Msk                                     (0x01 << PORRST_FLAG_Pos)
#define PORRST_FLAG_Pos                                     1
#define PORRST_FLAG_PosL                                    1
#define PORRST_FLAG_PosH                                    1

/*!< 0x0050 RCC_RST_FLAG
*null
*/
#define NRST_FLAG_Msk                                       (0x01 << NRST_FLAG_Pos)
#define NRST_FLAG_Pos                                       0
#define NRST_FLAG_PosL                                      0
#define NRST_FLAG_PosH                                      0


/*!< 0x0070 RCC_SYS0_BOOTFLAG
*null
*/
#define SYS0_BOOTFLAG_Msk                                   (0x07 << SYS0_BOOTFLAG_Pos)
#define SYS0_BOOTFLAG_Pos                                   0
#define SYS0_BOOTFLAG_PosL                                  0
#define SYS0_BOOTFLAG_PosH                                  2


/*!< 0x0074 RCC_SYS0_BOOTADDR
*null
*/
#define SYS0_BOOTADDR_Msk                                   (0XFFFFFFF << SYS0_BOOTADDR_Pos)
#define SYS0_BOOTADDR_Pos                                   0
#define SYS0_BOOTADDR_PosL                                  0
#define SYS0_BOOTADDR_PosH                                  31


/*!< 0x0078 RCC_SYS0_BOOTPARAM
*null
*/
#define SYS0_BOOTPARAM_Msk                                  (0XFFFFFFF << SYS0_BOOTPARAM_Pos)
#define SYS0_BOOTPARAM_Pos                                  0
#define SYS0_BOOTPARAM_PosL                                 0
#define SYS0_BOOTPARAM_PosH                                 31


/*!< 0x0090 RCC_SYS1_BOOTFLAG
*null
*/
#define SYS1_BOOTFLAG_Msk                                   (0x07 << SYS1_BOOTFLAG_Pos)
#define SYS1_BOOTFLAG_Pos                                   0
#define SYS1_BOOTFLAG_PosL                                  0
#define SYS1_BOOTFLAG_PosH                                  2


/*!< 0x0094 RCC_SYS1_BOOTADDR
*null
*/
#define SYS1_BOOTADDR_Msk                                   (0XFFFFFFF << SYS1_BOOTADDR_Pos)
#define SYS1_BOOTADDR_Pos                                   0
#define SYS1_BOOTADDR_PosL                                  0
#define SYS1_BOOTADDR_PosH                                  31


/*!< 0x0098 RCC_SYS1_BOOTPARAM
*null
*/
#define SYS1_BOOTPARAM_Msk                                  (0XFFFFFFF << SYS1_BOOTPARAM_Pos)
#define SYS1_BOOTPARAM_Pos                                  0
#define SYS1_BOOTPARAM_PosL                                 0
#define SYS1_BOOTPARAM_PosH                                 31


/*!< 0x00B4 RCC_SPECIAL_REG0
*null
*/
#define RCC_SPECIAL_REG0_Msk                                (0XFFFFFFF << RCC_SPECIAL_REG0_Pos)
#define RCC_SPECIAL_REG0_Pos                                0
#define RCC_SPECIAL_REG0_PosL                               0
#define RCC_SPECIAL_REG0_PosH                               31


/*!< 0x00b8 RCC_SPECIAL_REG1
*null
*/
#define RCC_SPECIAL_REG1_Msk                                (0XFFFFFFF << RCC_SPECIAL_REG1_Pos)
#define RCC_SPECIAL_REG1_Pos                                0
#define RCC_SPECIAL_REG1_PosL                               0
#define RCC_SPECIAL_REG1_PosH                               31


/*!< 0x00bc RCC_SPECIAL_REG2
*null
*/
#define RCC_SPECIAL_REG2_Msk                                (0XFFFFFFF << RCC_SPECIAL_REG2_Pos)
#define RCC_SPECIAL_REG2_Pos                                0
#define RCC_SPECIAL_REG2_PosL                               0
#define RCC_SPECIAL_REG2_PosH                               31


/*!< 0x00D0 RCC_CPUS_INTR_MASK
*null
*/
#define CPUS_INTR_MASK_EN_Msk                               (0x01 << CPUS_INTR_MASK_EN_Pos)
#define CPUS_INTR_MASK_EN_Pos                               0
#define CPUS_INTR_MASK_EN_PosL                              0
#define CPUS_INTR_MASK_EN_PosH                              0


/*!< 0x00d4 RCC_CPUS_INTR_MASK
*null
*/
#define CPUFF_INTR_MASK_EN_Msk                              (0x01 << CPUFF_INTR_MASK_EN_Pos)
#define CPUFF_INTR_MASK_EN_Pos                              0
#define CPUFF_INTR_MASK_EN_PosL                             0
#define CPUFF_INTR_MASK_EN_PosH                             0


/**
* @}
* End of group HW_RCC
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_RCC_H_ */