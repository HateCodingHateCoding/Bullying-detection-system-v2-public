/************************************************************************
* @file  hw_ccmu1.h
* @brief hw_ccmu1 hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
*************************************************************************
*/
#ifndef _HW_CCMU1_H_
#define _HW_CCMU1_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_CCMU1
 * @{
 * */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */

typedef struct {
    __IOM uint32_t CCMU_RESETN_EN;                          /*0x0000,             */
    __IOM uint32_t CCMU_APB0_RESETN_EN;                     /*0x0004,             */
    __IOM uint32_t CCMU_APB1_RESETN_EN;                     /*0x0008,             */
          uint32_t RSVD[3];                                 /*0x000C-0x0014,      */
    __IOM uint32_t CCMU_HCLKEN_SEL;                         /*0x0018,             */
    __IOM uint32_t CCMU_HCLK_SW_EN;                         /*0x001C,             */
    __IOM uint32_t CCMU_APB0_PCLKEN_SEL;                    /*0x0020,             */
    __IOM uint32_t CCMU_APB0_SW_PCLKEN;                     /*0x0024,             */
    __IOM uint32_t CCMU_APB1_PCLKEN_SEL;                    /*0x0028,             */
    __IOM uint32_t CCMU_APB1_SW_PCLKEN;                     /*0x002C,             */
    __IOM uint32_t CCMU_AHB_FCLK_EN;                        /*0x0030,             */
    __IOM uint32_t CCMU_APB0_FCLK_EN;                       /*0x0034,             */
    __IOM uint32_t CCMU_APB1_FCLK_EN;                       /*0x0038,             */
          uint32_t RSVD1[5];                                /*0x003C-0x004C,      */
    __IOM uint32_t CCMU_APB0_PCLK_DIV;                      /*0x0050,             */
          uint32_t RSVD2;                                   /*0x0054,             */
    __IOM uint32_t CCMU_ROM_CLK_CTRL;                       /*0x0058,             */
    __IOM uint32_t CCMU_SRAM_CLK_CTRL[2];                   /*0x005C-0X0060,      */
    __IOM uint32_t UART_LP_CTRL;                            /*0x0064,             */
          uint32_t RSVD3[2];                                /*0x0068-0x006C,      */
    __IOM uint32_t CCMU_I2C0_FCLK_SRC_SEL;                  /*0x0070,             */
    __IOM uint32_t CCMU_I2C1_FCLK_SRC_SEL;                  /*0x0074,             */
    __IOM uint32_t CCMU_UART1_FCLK_SRC_SEL;                 /*0x0078,             */
          uint32_t RSVD4[2];                                /*0x007C-0x0080,      */
    __IOM uint32_t CCMU_TMR0_FCLK_SRC_SEL;                  /*0x0084,             */
    __IOM uint32_t CCMU_TMR1_FCLK_SRC_SEL;                  /*0x0088,             */
    __IOM uint32_t CCMU_UART0_FCLK_SRC_SEL;                 /*0x008C,             */
    __IOM uint32_t CCMU_QSPI0_FCLK_SRC_SEL;                 /*0x0090,             */
          uint32_t RSVD5;                                   /*0x0094,             */
    __IOM uint32_t CCMU_FCTRL_FCLK_SRC_SEL;                 /*0x0098,             */
          uint32_t RSVD6;                                   /*0x009C,             */
    __IOM uint32_t CCMU_WDG0_FCLK_SRC_SEL;                  /*0x00A0,             */
    __IOM uint32_t CCMU_WDG1_FCLK_SRC_SEL;                  /*0x00A4,             */
    __IOM uint32_t CCMU_UART2_FCLK_SRC_SEL;                 /*0x00A8,             */
    __IOM uint32_t CCMU_MCAN_FCLK_SRC_SEL;                  /*0x00AC,             */
          uint32_t RSVD7;                                   /*0x00B0,             */
    __IOM uint32_t CCMU_CDAA_FCLK_DIV;                      /*0x00B4,             */
} ccmu1_dev_t;

#define CCMU1_DEV                       ((ccmu1_dev_t *)  CCMU1_BASE_ADDR) /*!< 0x4001 7000 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x0000 CCMU_RESETn_EN
*reset mini DSP module
*/
#define MINIDSP_RESETN_Msk                                  (0x01 << MINIDSP_RESETN_Pos)
#define MINIDSP_RESETN_Pos                                  7
#define MINIDSP_RESETN_PosL                                 7
#define MINIDSP_RESETN_PosH                                 7

/*!< 0x0000 CCMU_RESETn_EN
*null
*/
#define MCAN2_RESETN_Msk                                    (0x01 << MCAN2_RESETN_Pos)
#define MCAN2_RESETN_Pos                                    6
#define MCAN2_RESETN_PosL                                   6
#define MCAN2_RESETN_PosH                                   6

/*!< 0x0000 CCMU_RESETn_EN
*null
*/
#define SS_RESETN_Msk                                       (0x01 << SS_RESETN_Pos)
#define SS_RESETN_Pos                                       4
#define SS_RESETN_PosL                                      4
#define SS_RESETN_PosH                                      4

/*!< 0x0000 CCMU_RESETn_EN
*null
*/
#define QSPI0_RESETN_Msk                                    (0x01 << QSPI0_RESETN_Pos)
#define QSPI0_RESETN_Pos                                    2
#define QSPI0_RESETN_PosL                                   2
#define QSPI0_RESETN_PosH                                   2

/*!< 0x0000 CCMU_RESETn_EN
*null
*/
#define FLASHCTRL_RESETN_Msk                                (0x01 << FLASHCTRL_RESETN_Pos)
#define FLASHCTRL_RESETN_Pos                                1
#define FLASHCTRL_RESETN_PosL                               1
#define FLASHCTRL_RESETN_PosH                               1

/*!< 0x0000 CCMU_RESETn_EN
*null
*/
#define DMAC_RESETN_Msk                                     (0x01 << DMAC_RESETN_Pos)
#define DMAC_RESETN_Pos                                     0
#define DMAC_RESETN_PosL                                    0
#define DMAC_RESETN_PosH                                    0


/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define LA_RESETN_Msk                                       (0x01 << LA_RESETN_Pos)
#define LA_RESETN_Pos                                       10
#define LA_RESETN_PosL                                      10
#define LA_RESETN_PosH                                      10

/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define PKA_RESETN_Msk                                      (0x01 << PKA_RESETN_Pos)
#define PKA_RESETN_Pos                                      9
#define PKA_RESETN_PosL                                     9
#define PKA_RESETN_PosH                                     9

/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define CDAA_RESETN_Msk                                     (0x01 << CDAA_RESETN_Pos)
#define CDAA_RESETN_Pos                                     8
#define CDAA_RESETN_PosL                                    8
#define CDAA_RESETN_PosH                                    8

/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define MAILBOX1_RESETN_Msk                                 (0x01 << MAILBOX1_RESETN_Pos)
#define MAILBOX1_RESETN_Pos                                 4
#define MAILBOX1_RESETN_PosL                                4
#define MAILBOX1_RESETN_PosH                                4

/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define MAILBOX0_RESETN_Msk                                 (0x01 << MAILBOX0_RESETN_Pos)
#define MAILBOX0_RESETN_Pos                                 3
#define MAILBOX0_RESETN_PosL                                3
#define MAILBOX0_RESETN_PosH                                3

/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define UART1_RESETN_Msk                                    (0x01 << UART1_RESETN_Pos)
#define UART1_RESETN_Pos                                    2
#define UART1_RESETN_PosL                                   2
#define UART1_RESETN_PosH                                   2

/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define I2C1_RESETN_Msk                                     (0x01 << I2C1_RESETN_Pos)
#define I2C1_RESETN_Pos                                     1
#define I2C1_RESETN_PosL                                    1
#define I2C1_RESETN_PosH                                    1

/*!< 0x0004 CCMU_APB0_RESETn_EN
*null
*/
#define I2C0_RESETN_Msk                                     (0x01 << I2C0_RESETN_Pos)
#define I2C0_RESETN_Pos                                     0
#define I2C0_RESETN_PosL                                    0
#define I2C0_RESETN_PosH                                    0


/*!< 0x0008 CCMU_APB1_RESETn_EN
*null
*/
#define SPINLOCK_PRESETN_Msk                                (0x01 << SPINLOCK_PRESETN_Pos)
#define SPINLOCK_PRESETN_Pos                                9
#define SPINLOCK_PRESETN_PosL                               9
#define SPINLOCK_PRESETN_PosH                               9

/*!< 0x0008 CCMU_APB1_RESETn_EN
*null
*/
#define UART2_RESETN_Msk                                    (0x01 << UART2_RESETN_Pos)
#define UART2_RESETN_Pos                                    7
#define UART2_RESETN_PosL                                   7
#define UART2_RESETN_PosH                                   7

/*!< 0x0008 CCMU_APB1_RESETn_EN
*null
*/
#define UART0_RESETN_Msk                                    (0x01 << UART0_RESETN_Pos)
#define UART0_RESETN_Pos                                    4
#define UART0_RESETN_PosL                                   4
#define UART0_RESETN_PosH                                   4

/*!< 0x0008 CCMU_APB1_RESETn_EN
*null
*/
#define WDG1_RESETN_Msk                                     (0x01 << WDG1_RESETN_Pos)
#define WDG1_RESETN_Pos                                     3
#define WDG1_RESETN_PosL                                    3
#define WDG1_RESETN_PosH                                    3

/*!< 0x0008 CCMU_APB1_RESETn_EN
*null
*/
#define WDG0_RESETN_Msk                                     (0x01 << WDG0_RESETN_Pos)
#define WDG0_RESETN_Pos                                     2
#define WDG0_RESETN_PosL                                    2
#define WDG0_RESETN_PosH                                    2

/*!< 0x0008 CCMU_APB1_RESETn_EN
*null
*/
#define TMR1_RESETN_Msk                                     (0x01 << TMR1_RESETN_Pos)
#define TMR1_RESETN_Pos                                     1
#define TMR1_RESETN_PosL                                    1
#define TMR1_RESETN_PosH                                    1

/*!< 0x0008 CCMU_APB1_RESETn_EN
*null
*/
#define TMR0_RESETN_Msk                                     (0x01 << TMR0_RESETN_Pos)
#define TMR0_RESETN_Pos                                     0
#define TMR0_RESETN_PosL                                    0
#define TMR0_RESETN_PosH                                    0


/*!< 0x0018 CCMU_HCLKEN_SEL
*null
*/
#define QSPI0_HCLKEN_SEL_Msk                                (0x01 << QSPI0_HCLKEN_SEL_Pos)
#define QSPI0_HCLKEN_SEL_Pos                                2
#define QSPI0_HCLKEN_SEL_PosL                               2
#define QSPI0_HCLKEN_SEL_PosH                               2

/*!< 0x0018 CCMU_HCLKEN_SEL
*null
*/
#define FLASHCTRL_HCLKEN_SEL_Msk                            (0x01 << FLASHCTRL_HCLKEN_SEL_Pos)
#define FLASHCTRL_HCLKEN_SEL_Pos                            1
#define FLASHCTRL_HCLKEN_SEL_PosL                           1
#define FLASHCTRL_HCLKEN_SEL_PosH                           1


/*!< 0x001c CCMU_HCLK_SW_EN
*null
*/
#define MINIDSP_HCLK_EN_Msk                                 (0x01 << MINIDSP_HCLK_EN_Pos)
#define MINIDSP_HCLK_EN_Pos                                 7
#define MINIDSP_HCLK_EN_PosL                                7
#define MINIDSP_HCLK_EN_PosH                                7

/*!< 0x001c CCMU_HCLK_SW_EN
*null
*/
#define MCAN2_HCLK_EN_Msk                                   (0x01 << MCAN2_HCLK_EN_Pos)
#define MCAN2_HCLK_EN_Pos                                   6
#define MCAN2_HCLK_EN_PosL                                  6
#define MCAN2_HCLK_EN_PosH                                  6

/*!< 0x001c CCMU_HCLK_SW_EN
*null
*/
#define ENCRYPT_SS_HCLK_EN_Msk                              (0x01 << ENCRYPT_SS_HCLK_EN_Pos)
#define ENCRYPT_SS_HCLK_EN_Pos                              4
#define ENCRYPT_SS_HCLK_EN_PosL                             4
#define ENCRYPT_SS_HCLK_EN_PosH                             4

/*!< 0x001c CCMU_HCLK_SW_EN
*null
*/
#define QSPI0_HCLK_EN_Msk                                   (0x01 << QSPI0_HCLK_EN_Pos)
#define QSPI0_HCLK_EN_Pos                                   2
#define QSPI0_HCLK_EN_PosL                                  2
#define QSPI0_HCLK_EN_PosH                                  2

/*!< 0x001c CCMU_HCLK_SW_EN
*null
*/
#define FLASHCTRL_HCLK_EN_Msk                               (0x01 << FLASHCTRL_HCLK_EN_Pos)
#define FLASHCTRL_HCLK_EN_Pos                               1
#define FLASHCTRL_HCLK_EN_PosL                              1
#define FLASHCTRL_HCLK_EN_PosH                              1

/*!< 0x001c CCMU_HCLK_SW_EN
*null
*/
#define DMAC_HCLK_EN_Msk                                    (0x01 << DMAC_HCLK_EN_Pos)
#define DMAC_HCLK_EN_Pos                                    0
#define DMAC_HCLK_EN_PosL                                   0
#define DMAC_HCLK_EN_PosH                                   0


/*!< 0x0020 CCMU_APB0_PCLKEN_SEL
*null
*/
#define CDAA_PCLKEN_SEL_Msk                                 (0x01 << CDAA_PCLKEN_SEL_Pos)
#define CDAA_PCLKEN_SEL_Pos                                 8
#define CDAA_PCLKEN_SEL_PosL                                8
#define CDAA_PCLKEN_SEL_PosH                                8

/*!< 0x0020 CCMU_APB0_PCLKEN_SEL
*null
*/
#define MAILBOX1_PCLKEN_SEL_Msk                             (0x01 << MAILBOX1_PCLKEN_SEL_Pos)
#define MAILBOX1_PCLKEN_SEL_Pos                             4
#define MAILBOX1_PCLKEN_SEL_PosL                            4
#define MAILBOX1_PCLKEN_SEL_PosH                            4

/*!< 0x0020 CCMU_APB0_PCLKEN_SEL
*null
*/
#define MAILBOX0_PCLKEN_SEL_Msk                             (0x01 << MAILBOX0_PCLKEN_SEL_Pos)
#define MAILBOX0_PCLKEN_SEL_Pos                             3
#define MAILBOX0_PCLKEN_SEL_PosL                            3
#define MAILBOX0_PCLKEN_SEL_PosH                            3

/*!< 0x0020 CCMU_APB0_PCLKEN_SEL
*null
*/
#define UART1_PCLKEN_SEL_Msk                                (0x01 << UART1_PCLKEN_SEL_Pos)
#define UART1_PCLKEN_SEL_Pos                                2
#define UART1_PCLKEN_SEL_PosL                               2
#define UART1_PCLKEN_SEL_PosH                               2

/*!< 0x0020 CCMU_APB0_PCLKEN_SEL
*null
*/
#define I2C1_PCLKEN_SEL_Msk                                 (0x01 << I2C1_PCLKEN_SEL_Pos)
#define I2C1_PCLKEN_SEL_Pos                                 1
#define I2C1_PCLKEN_SEL_PosL                                1
#define I2C1_PCLKEN_SEL_PosH                                1

/*!< 0x0020 CCMU_APB0_PCLKEN_SEL
*null
*/
#define I2C0_PCLKEN_SEL_Msk                                 (0x01 << I2C0_PCLKEN_SEL_Pos)
#define I2C0_PCLKEN_SEL_Pos                                 0
#define I2C0_PCLKEN_SEL_PosL                                0
#define I2C0_PCLKEN_SEL_PosH                                0


/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define LA_SW_PCLKEN_Msk                                    (0x01 << LA_SW_PCLKEN_Pos)
#define LA_SW_PCLKEN_Pos                                    10
#define LA_SW_PCLKEN_PosL                                   10
#define LA_SW_PCLKEN_PosH                                   10

/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define PKA_SW_PCLKEN_Msk                                   (0x01 << PKA_SW_PCLKEN_Pos)
#define PKA_SW_PCLKEN_Pos                                   9
#define PKA_SW_PCLKEN_PosL                                  9
#define PKA_SW_PCLKEN_PosH                                  9

/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define CDAA_SW_PCLKEN_Msk                                  (0x01 << CDAA_SW_PCLKEN_Pos)
#define CDAA_SW_PCLKEN_Pos                                  8
#define CDAA_SW_PCLKEN_PosL                                 8
#define CDAA_SW_PCLKEN_PosH                                 8

/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define MAILBOX1_SW_PCLKEN_Msk                              (0x01 << MAILBOX1_SW_PCLKEN_Pos)
#define MAILBOX1_SW_PCLKEN_Pos                              4
#define MAILBOX1_SW_PCLKEN_PosL                             4
#define MAILBOX1_SW_PCLKEN_PosH                             4

/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define MAILBOX0_SW_PCLKEN_Msk                              (0x01 << MAILBOX0_SW_PCLKEN_Pos)
#define MAILBOX0_SW_PCLKEN_Pos                              3
#define MAILBOX0_SW_PCLKEN_PosL                             3
#define MAILBOX0_SW_PCLKEN_PosH                             3

/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define UART1_SW_PCLKEN_Msk                                 (0x01 << UART1_SW_PCLKEN_Pos)
#define UART1_SW_PCLKEN_Pos                                 2
#define UART1_SW_PCLKEN_PosL                                2
#define UART1_SW_PCLKEN_PosH                                2

/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define I2C1_SW_PCLKEN_Msk                                  (0x01 << I2C1_SW_PCLKEN_Pos)
#define I2C1_SW_PCLKEN_Pos                                  1
#define I2C1_SW_PCLKEN_PosL                                 1
#define I2C1_SW_PCLKEN_PosH                                 1

/*!< 0x0024 CCMU_APB0_SW_PCLKEN
*null
*/
#define I2C0_SW_PCLKEN_Msk                                  (0x01 << I2C0_SW_PCLKEN_Pos)
#define I2C0_SW_PCLKEN_Pos                                  0
#define I2C0_SW_PCLKEN_PosL                                 0
#define I2C0_SW_PCLKEN_PosH                                 0


/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define RTC_PCLKEN_SEL_Msk                                  (0x01 << RTC_PCLKEN_SEL_Pos)
#define RTC_PCLKEN_SEL_Pos                                  31
#define RTC_PCLKEN_SEL_PosL                                 31
#define RTC_PCLKEN_SEL_PosH                                 31

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define SWCFG_PCLKEN_SEL_Msk                                (0x01 << SWCFG_PCLKEN_SEL_Pos)
#define SWCFG_PCLKEN_SEL_Pos                                10
#define SWCFG_PCLKEN_SEL_PosL                               10
#define SWCFG_PCLKEN_SEL_PosH                               10

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define SPINLOCK_PCLKEN_SEL_Msk                             (0x01 << SPINLOCK_PCLKEN_SEL_Pos)
#define SPINLOCK_PCLKEN_SEL_Pos                             9
#define SPINLOCK_PCLKEN_SEL_PosL                            9
#define SPINLOCK_PCLKEN_SEL_PosH                            9

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define UART2_PCLKEN_SEL_Msk                                (0x01 << UART2_PCLKEN_SEL_Pos)
#define UART2_PCLKEN_SEL_Pos                                7
#define UART2_PCLKEN_SEL_PosL                               7
#define UART2_PCLKEN_SEL_PosH                               7

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define CCMU_PCLKEN_SEL_Msk                                 (0x01 << CCMU_PCLKEN_SEL_Pos)
#define CCMU_PCLKEN_SEL_Pos                                 6
#define CCMU_PCLKEN_SEL_PosL                                6
#define CCMU_PCLKEN_SEL_PosH                                6

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define UART0_PCLKEN_SEL_Msk                                (0x01 << UART0_PCLKEN_SEL_Pos)
#define UART0_PCLKEN_SEL_Pos                                4
#define UART0_PCLKEN_SEL_PosL                               4
#define UART0_PCLKEN_SEL_PosH                               4

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define WDG1_PCLKEN_SEL_Msk                                 (0x01 << WDG1_PCLKEN_SEL_Pos)
#define WDG1_PCLKEN_SEL_Pos                                 3
#define WDG1_PCLKEN_SEL_PosL                                3
#define WDG1_PCLKEN_SEL_PosH                                3

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define WDG0_PCLKEN_SEL_Msk                                 (0x01 << WDG0_PCLKEN_SEL_Pos)
#define WDG0_PCLKEN_SEL_Pos                                 2
#define WDG0_PCLKEN_SEL_PosL                                2
#define WDG0_PCLKEN_SEL_PosH                                2

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define TMR1_PCLKEN_SEL_Msk                                 (0x01 << TMR1_PCLKEN_SEL_Pos)
#define TMR1_PCLKEN_SEL_Pos                                 1
#define TMR1_PCLKEN_SEL_PosL                                1
#define TMR1_PCLKEN_SEL_PosH                                1

/*!< 0x0028 CCMU_APB1_PCLKEN_SEL
*null
*/
#define TMR0_PCLKEN_SEL_Msk                                 (0x01 << TMR0_PCLKEN_SEL_Pos)
#define TMR0_PCLKEN_SEL_Pos                                 0
#define TMR0_PCLKEN_SEL_PosL                                0
#define TMR0_PCLKEN_SEL_PosH                                0


/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define RTC_SW_PCLKEN_Msk                                   (0x01 << RTC_SW_PCLKEN_Pos)
#define RTC_SW_PCLKEN_Pos                                   31
#define RTC_SW_PCLKEN_PosL                                  31
#define RTC_SW_PCLKEN_PosH                                  31

/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define SPINLOCK_SW_PCLKEN_Msk                              (0x01 << SPINLOCK_SW_PCLKEN_Pos)
#define SPINLOCK_SW_PCLKEN_Pos                              9
#define SPINLOCK_SW_PCLKEN_PosL                             9
#define SPINLOCK_SW_PCLKEN_PosH                             9

/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define UART2_SW_PCLKEN_Msk                                 (0x01 << UART2_SW_PCLKEN_Pos)
#define UART2_SW_PCLKEN_Pos                                 7
#define UART2_SW_PCLKEN_PosL                                7
#define UART2_SW_PCLKEN_PosH                                7

/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define UART0_SW_PCLKEN_Msk                                 (0x01 << UART0_SW_PCLKEN_Pos)
#define UART0_SW_PCLKEN_Pos                                 4
#define UART0_SW_PCLKEN_PosL                                4
#define UART0_SW_PCLKEN_PosH                                4

/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define WDG1_SW_PCLKEN_Msk                                  (0x01 << WDG1_SW_PCLKEN_Pos)
#define WDG1_SW_PCLKEN_Pos                                  3
#define WDG1_SW_PCLKEN_PosL                                 3
#define WDG1_SW_PCLKEN_PosH                                 3

/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define WDG0_SW_PCLKEN_Msk                                  (0x01 << WDG0_SW_PCLKEN_Pos)
#define WDG0_SW_PCLKEN_Pos                                  2
#define WDG0_SW_PCLKEN_PosL                                 2
#define WDG0_SW_PCLKEN_PosH                                 2

/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define TMR1_SW_PCLKEN_Msk                                  (0x01 << TMR1_SW_PCLKEN_Pos)
#define TMR1_SW_PCLKEN_Pos                                  1
#define TMR1_SW_PCLKEN_PosL                                 1
#define TMR1_SW_PCLKEN_PosH                                 1

/*!< 0x002c CCMU_APB1_SW_PCLKEN
*null
*/
#define TMR0_SW_PCLKEN_Msk                                  (0x01 << TMR0_SW_PCLKEN_Pos)
#define TMR0_SW_PCLKEN_Pos                                  0
#define TMR0_SW_PCLKEN_PosL                                 0
#define TMR0_SW_PCLKEN_PosH                                 0


/*!< 0x0030 CCMU_AHB_FCLK_EN
*null
*/
#define QSPI0_FCLK_EN_Msk                                   (0x01 << QSPI0_FCLK_EN_Pos)
#define QSPI0_FCLK_EN_Pos                                   2
#define QSPI0_FCLK_EN_PosL                                  2
#define QSPI0_FCLK_EN_PosH                                  2

/*!< 0x0030 CCMU_AHB_FCLK_EN
*null
*/
#define FLASHCTRL_FCLK_EN_Msk                               (0x01 << FLASHCTRL_FCLK_EN_Pos)
#define FLASHCTRL_FCLK_EN_Pos                               1
#define FLASHCTRL_FCLK_EN_PosL                              1
#define FLASHCTRL_FCLK_EN_PosH                              1


/*!< 0x0034 CCMU_APB0_FCLK_EN
*null
*/
#define LA_FCLK_EN_Msk                                      (0x01 << LA_FCLK_EN_Pos)
#define LA_FCLK_EN_Pos                                      10
#define LA_FCLK_EN_PosL                                     10
#define LA_FCLK_EN_PosH                                     10

/*!< 0x0034 CCMU_APB0_FCLK_EN
*null
*/
#define MCAN2_FCLK_EN_Msk                                   (0x01 << MCAN2_FCLK_EN_Pos)
#define MCAN2_FCLK_EN_Pos                                   7
#define MCAN2_FCLK_EN_PosL                                  7
#define MCAN2_FCLK_EN_PosH                                  7

/*!< 0x0034 CCMU_APB0_FCLK_EN
*null
*/
#define UART1_FCLK_EN_Msk                                   (0x01 << UART1_FCLK_EN_Pos)
#define UART1_FCLK_EN_Pos                                   2
#define UART1_FCLK_EN_PosL                                  2
#define UART1_FCLK_EN_PosH                                  2

/*!< 0x0034 CCMU_APB0_FCLK_EN
*null
*/
#define I2C1_FCLK_EN_Msk                                    (0x01 << I2C1_FCLK_EN_Pos)
#define I2C1_FCLK_EN_Pos                                    1
#define I2C1_FCLK_EN_PosL                                   1
#define I2C1_FCLK_EN_PosH                                   1

/*!< 0x0034 CCMU_APB0_FCLK_EN
*null
*/
#define I2C0_FCLK_EN_Msk                                    (0x01 << I2C0_FCLK_EN_Pos)
#define I2C0_FCLK_EN_Pos                                    0
#define I2C0_FCLK_EN_PosL                                   0
#define I2C0_FCLK_EN_PosH                                   0


/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR1_1_EN_2_FCLK_Msk                                (0x01 << TMR1_1_EN_2_FCLK_Pos)
#define TMR1_1_EN_2_FCLK_Pos                                31
#define TMR1_1_EN_2_FCLK_PosL                               31
#define TMR1_1_EN_2_FCLK_PosH                               31

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR0_1_EN_2_FCLK_Msk                                (0x01 << TMR0_1_EN_2_FCLK_Pos)
#define TMR0_1_EN_2_FCLK_Pos                                30
#define TMR0_1_EN_2_FCLK_PosL                               30
#define TMR0_1_EN_2_FCLK_PosH                               30

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR1_2_EN_2_FCLK_Msk                                (0x01 << TMR1_2_EN_2_FCLK_Pos)
#define TMR1_2_EN_2_FCLK_Pos                                29
#define TMR1_2_EN_2_FCLK_PosL                               29
#define TMR1_2_EN_2_FCLK_PosH                               29

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR0_2_EN_2_FCLK_Msk                                (0x01 << TMR0_2_EN_2_FCLK_Pos)
#define TMR0_2_EN_2_FCLK_Pos                                28
#define TMR0_2_EN_2_FCLK_PosL                               28
#define TMR0_2_EN_2_FCLK_PosH                               28

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR1_2_FCLK_EN_Msk                                  (0x01 << TMR1_2_FCLK_EN_Pos)
#define TMR1_2_FCLK_EN_Pos                                  9
#define TMR1_2_FCLK_EN_PosL                                 9
#define TMR1_2_FCLK_EN_PosH                                 9

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR0_2_FCLK_EN_Msk                                  (0x01 << TMR0_2_FCLK_EN_Pos)
#define TMR0_2_FCLK_EN_Pos                                  8
#define TMR0_2_FCLK_EN_PosL                                 8
#define TMR0_2_FCLK_EN_PosH                                 8

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define UART2_FCLK_EN_Msk                                   (0x01 << UART2_FCLK_EN_Pos)
#define UART2_FCLK_EN_Pos                                   7
#define UART2_FCLK_EN_PosL                                  7
#define UART2_FCLK_EN_PosH                                  7

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define UART0_FCLK_EN_Msk                                   (0x01 << UART0_FCLK_EN_Pos)
#define UART0_FCLK_EN_Pos                                   4
#define UART0_FCLK_EN_PosL                                  4
#define UART0_FCLK_EN_PosH                                  4

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define WDG1_FCLK_EN_Msk                                    (0x01 << WDG1_FCLK_EN_Pos)
#define WDG1_FCLK_EN_Pos                                    3
#define WDG1_FCLK_EN_PosL                                   3
#define WDG1_FCLK_EN_PosH                                   3

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define WDG0_FCLK_EN_Msk                                    (0x01 << WDG0_FCLK_EN_Pos)
#define WDG0_FCLK_EN_Pos                                    2
#define WDG0_FCLK_EN_PosL                                   2
#define WDG0_FCLK_EN_PosH                                   2

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR1_1_FCLK_EN_Msk                                  (0x01 << TMR1_1_FCLK_EN_Pos)
#define TMR1_1_FCLK_EN_Pos                                  1
#define TMR1_1_FCLK_EN_PosL                                 1
#define TMR1_1_FCLK_EN_PosH                                 1

/*!< 0x0038 CCMU_APB1_FCLK_EN
*null
*/
#define TMR0_1_FCLK_EN_Msk                                  (0x01 << TMR0_1_FCLK_EN_Pos)
#define TMR0_1_FCLK_EN_Pos                                  0
#define TMR0_1_FCLK_EN_PosL                                 0
#define TMR0_1_FCLK_EN_PosH                                 0


/*!< 0x0050 CCMU_APB0_PCLK_DIV
*null
*/
#define CCMU_APB0_PCLK_DIV_Msk                              (0x07 << CCMU_APB0_PCLK_DIV_Pos)
#define CCMU_APB0_PCLK_DIV_Pos                              0
#define CCMU_APB0_PCLK_DIV_PosL                             0
#define CCMU_APB0_PCLK_DIV_PosH                             2


/*!< 0x0058 CCMU_ROM_CLK_CTRL
*null
*/
#define CCMU_ROM_CLK_CTRL_Msk                               (0x01 << CCMU_ROM_CLK_CTRL_Pos)
#define CCMU_ROM_CLK_CTRL_Pos                               0
#define CCMU_ROM_CLK_CTRL_PosL                              0
#define CCMU_ROM_CLK_CTRL_PosH                              0


/*!< 0x005C-0x0060 CCMU_SRAM_CLK_CTRL
*null
*/
#define CCMU_SRAM_CLK_CTRL_Msk                              (0x01 << CCMU_SRAM_CLK_CTRL_Pos)
#define CCMU_SRAM_CLK_CTRL_Pos                              0
#define CCMU_SRAM_CLK_CTRL_PosL                             0
#define CCMU_SRAM_CLK_CTRL_PosH                             0


/*!< 0x0064 UART_LP_CTRL
*null
*/
#define UART2_PCLK_LP_EN_Msk                                (0x01 << UART2_PCLK_LP_EN_Pos)
#define UART2_PCLK_LP_EN_Pos                                5
#define UART2_PCLK_LP_EN_PosL                               5
#define UART2_PCLK_LP_EN_PosH                               5

/*!< 0x0064 UART_LP_CTRL
*null
*/
#define UART2_FCLK_LP_EN_Msk                                (0x01 << UART2_FCLK_LP_EN_Pos)
#define UART2_FCLK_LP_EN_Pos                                4
#define UART2_FCLK_LP_EN_PosL                               4
#define UART2_FCLK_LP_EN_PosH                               4

/*!< 0x0064 UART_LP_CTRL
*null
*/
#define UART1_PCLK_LP_EN_Msk                                (0x01 << UART1_PCLK_LP_EN_Pos)
#define UART1_PCLK_LP_EN_Pos                                3
#define UART1_PCLK_LP_EN_PosL                               3
#define UART1_PCLK_LP_EN_PosH                               3

/*!< 0x0064 UART_LP_CTRL
*null
*/
#define UART1_FCLK_LP_EN_Msk                                (0x01 << UART1_FCLK_LP_EN_Pos)
#define UART1_FCLK_LP_EN_Pos                                2
#define UART1_FCLK_LP_EN_PosL                               2
#define UART1_FCLK_LP_EN_PosH                               2

/*!< 0x0064 UART_LP_CTRL
*null
*/
#define UART0_PCLK_LP_EN_Msk                                (0x01 << UART0_PCLK_LP_EN_Pos)
#define UART0_PCLK_LP_EN_Pos                                1
#define UART0_PCLK_LP_EN_PosL                               1
#define UART0_PCLK_LP_EN_PosH                               1

/*!< 0x0064 UART_LP_CTRL
*null
*/
#define UART0_FCLK_LP_EN_Msk                                (0x01 << UART0_FCLK_LP_EN_Pos)
#define UART0_FCLK_LP_EN_Pos                                0
#define UART0_FCLK_LP_EN_PosL                               0
#define UART0_FCLK_LP_EN_PosH                               0


/*!< 0x0070 CCMU_I2C0_FCLK_SRC_SEL
*null
*/
#define CCMU_I2C0_FCLK_POSTDIV_Msk                          (0x03 << CCMU_I2C0_FCLK_POSTDIV_Pos)
#define CCMU_I2C0_FCLK_POSTDIV_Pos                          4
#define CCMU_I2C0_FCLK_POSTDIV_PosL                         4
#define CCMU_I2C0_FCLK_POSTDIV_PosH                         5

/*!< 0x0070 CCMU_I2C0_FCLK_SRC_SEL
*null
*/
#define CCMU_I2C0_FCLK_SRC_SEL_Msk                          (0x03 << CCMU_I2C0_FCLK_SRC_SEL_Pos)
#define CCMU_I2C0_FCLK_SRC_SEL_Pos                          0
#define CCMU_I2C0_FCLK_SRC_SEL_PosL                         0
#define CCMU_I2C0_FCLK_SRC_SEL_PosH                         1

/*!< 0x0070 CCMU_I2C1_FCLK_SRC_SEL
*null
*/
#define CCMU_I2C1_FCLK_POSTDIV_Msk                          (0x03 << CCMU_I2C1_FCLK_POSTDIV_Pos)
#define CCMU_I2C1_FCLK_POSTDIV_Pos                          4
#define CCMU_I2C1_FCLK_POSTDIV_PosL                         4
#define CCMU_I2C1_FCLK_POSTDIV_PosH                         5

/*!< 0x0074 CCMU_I2C1_FCLK_SRC_SEL
*null
*/
#define CCMU_I2C1_FCLK_SRC_SEL_Msk                          (0x03 << CCMU_I2C1_FCLK_SRC_SEL_Pos)
#define CCMU_I2C1_FCLK_SRC_SEL_Pos                          0
#define CCMU_I2C1_FCLK_SRC_SEL_PosL                         0
#define CCMU_I2C1_FCLK_SRC_SEL_PosH                         1


/*!< 0x0078 CCMU_UART1_FCLK_SRC_SEL
*null
*/
#define CCMU_UART1_FCLK_POSTDIV_Msk                         (0x03 << CCMU_UART1_FCLK_POSTDIV_Pos)
#define CCMU_UART1_FCLK_POSTDIV_Pos                         4
#define CCMU_UART1_FCLK_POSTDIV_PosL                        4
#define CCMU_UART1_FCLK_POSTDIV_PosH                        5

/*!< 0x0078 CCMU_UART1_FCLK_SRC_SEL
*null
*/
#define CCMU_UART1_FCLK_SRC_SEL_Msk                         (0x07 << CCMU_UART1_FCLK_SRC_SEL_Pos)
#define CCMU_UART1_FCLK_SRC_SEL_Pos                         0
#define CCMU_UART1_FCLK_SRC_SEL_PosL                        0
#define CCMU_UART1_FCLK_SRC_SEL_PosH                        2


/*!< 0x0084 CCMU_TMR0_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR0_2_POSTDIV_Msk                             (0x07 << CCMU_TMR0_2_POSTDIV_Pos)
#define CCMU_TMR0_2_POSTDIV_Pos                             24
#define CCMU_TMR0_2_POSTDIV_PosL                            24
#define CCMU_TMR0_2_POSTDIV_PosH                            26

/*!< 0x0084 CCMU_TMR0_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR0_2_FCLK_SRC_SEL_Msk                        (0x03 << CCMU_TMR0_2_FCLK_SRC_SEL_Pos)
#define CCMU_TMR0_2_FCLK_SRC_SEL_Pos                        16
#define CCMU_TMR0_2_FCLK_SRC_SEL_PosL                       16
#define CCMU_TMR0_2_FCLK_SRC_SEL_PosH                       17

/*!< 0x0084 CCMU_TMR0_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR0_1_POSTDIV_Msk                             (0x07 << CCMU_TMR0_1_POSTDIV_Pos)
#define CCMU_TMR0_1_POSTDIV_Pos                             8
#define CCMU_TMR0_1_POSTDIV_PosL                            8
#define CCMU_TMR0_1_POSTDIV_PosH                            10

/*!< 0x0084 CCMU_TMR0_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR0_1_FCLK_SRC_SEL_Msk                        (0x03 << CCMU_TMR0_1_FCLK_SRC_SEL_Pos)
#define CCMU_TMR0_1_FCLK_SRC_SEL_Pos                        0
#define CCMU_TMR0_1_FCLK_SRC_SEL_PosL                       0
#define CCMU_TMR0_1_FCLK_SRC_SEL_PosH                       1


/*!< 0x0088 CCMU_TMR1_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR1_2_POSTDIV_Msk                             (0x07 << CCMU_TMR1_2_POSTDIV_Pos)
#define CCMU_TMR1_2_POSTDIV_Pos                             24
#define CCMU_TMR1_2_POSTDIV_PosL                            24
#define CCMU_TMR1_2_POSTDIV_PosH                            26

/*!< 0x0088 CCMU_TMR1_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR1_2_FCLK_SRC_SEL_Msk                        (0x03 << CCMU_TMR1_2_FCLK_SRC_SEL_Pos)
#define CCMU_TMR1_2_FCLK_SRC_SEL_Pos                        16
#define CCMU_TMR1_2_FCLK_SRC_SEL_PosL                       16
#define CCMU_TMR1_2_FCLK_SRC_SEL_PosH                       17

/*!< 0x0088 CCMU_TMR1_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR1_1_POSTDIV_Msk                             (0x07 << CCMU_TMR1_1_POSTDIV_Pos)
#define CCMU_TMR1_1_POSTDIV_Pos                             8
#define CCMU_TMR1_1_POSTDIV_PosL                            8
#define CCMU_TMR1_1_POSTDIV_PosH                            10

/*!< 0x0088 CCMU_TMR1_FCLK_SRC_SEL
*null
*/
#define CCMU_TMR1_1_FCLK_SRC_SEL_Msk                        (0x03 << CCMU_TMR1_1_FCLK_SRC_SEL_Pos)
#define CCMU_TMR1_1_FCLK_SRC_SEL_Pos                        0
#define CCMU_TMR1_1_FCLK_SRC_SEL_PosL                       0
#define CCMU_TMR1_1_FCLK_SRC_SEL_PosH                       1

/*!< 0x008c CCMU_UART0_FCLK_SRC_SEL
*null
*/
#define CCMU_UART0_FCLK_POSTDIV_Msk                         (0x03 << CCMU_UART0_FCLK_POSTDIV_Pos)
#define CCMU_UART0_FCLK_POSTDIV_Pos                         4
#define CCMU_UART0_FCLK_POSTDIV_PosL                        4
#define CCMU_UART0_FCLK_POSTDIV_PosH                        5

/*!< 0x008c CCMU_UART0_FCLK_SRC_SEL
*null
*/
#define CCMU_UART0_FCLK_SRC_SEL_Msk                         (0x07 << CCMU_UART0_FCLK_SRC_SEL_Pos)
#define CCMU_UART0_FCLK_SRC_SEL_Pos                         0
#define CCMU_UART0_FCLK_SRC_SEL_PosL                        0
#define CCMU_UART0_FCLK_SRC_SEL_PosH                        2


/*!< 0x0090 CCMU_QSPI0_FCLK_SRC_SEL
*null
*/
#define CCMU_QSPI0_FCLK_POSTDIV_Msk                         (0x03 << CCMU_QSPI0_FCLK_POSTDIV_Pos)
#define CCMU_QSPI0_FCLK_POSTDIV_Pos                         4
#define CCMU_QSPI0_FCLK_POSTDIV_PosL                        4
#define CCMU_QSPI0_FCLK_POSTDIV_PosH                        5

/*!< 0x0090 CCMU_QSPI0_FCLK_SRC_SEL
*null
*/
#define CCMU_QSPI0_FCLK_SRC_SEL_Msk                         (0x03 << CCMU_QSPI0_FCLK_SRC_SEL_Pos)
#define CCMU_QSPI0_FCLK_SRC_SEL_Pos                         0
#define CCMU_QSPI0_FCLK_SRC_SEL_PosL                        0
#define CCMU_QSPI0_FCLK_SRC_SEL_PosH                        1


/*!< 0x0098 CCMU_FCTRL_FCLK_SRC_SEL
*null
*/
#define CCMU_FCTRL_FCLK_POSTDIV_Msk                         (0x03 << CCMU_FCTRL_FCLK_POSTDIV_Pos)
#define CCMU_FCTRL_FCLK_POSTDIV_Pos                         4
#define CCMU_FCTRL_FCLK_POSTDIV_PosL                        4
#define CCMU_FCTRL_FCLK_POSTDIV_PosH                        5

/*!< 0x0098 CCMU_FCTRL_FCLK_SRC_SEL
*null
*/
#define CCMU_FCTRL_FCLK_SRC_SEL_Msk                         (0x03 << CCMU_FCTRL_FCLK_SRC_SEL_Pos)
#define CCMU_FCTRL_FCLK_SRC_SEL_Pos                         0
#define CCMU_FCTRL_FCLK_SRC_SEL_PosL                        0
#define CCMU_FCTRL_FCLK_SRC_SEL_PosH                        1


/*!< 0x00a0 CCMU_WDG0_FCLK_SRC_SEL
*null
*/
#define CCMU_WDG0_POSTDIV_Msk                               (0x07 << CCMU_WDG0_POSTDIV_Pos)
#define CCMU_WDG0_POSTDIV_Pos                               8
#define CCMU_WDG0_POSTDIV_PosL                              8
#define CCMU_WDG0_POSTDIV_PosH                              10

/*!< 0x00a0 CCMU_WDG0_FCLK_SRC_SEL
*null
*/
#define CCMU_WDG0_FCLK_SRC_SEL_Msk                          (0x03 << CCMU_WDG0_FCLK_SRC_SEL_Pos)
#define CCMU_WDG0_FCLK_SRC_SEL_Pos                          0
#define CCMU_WDG0_FCLK_SRC_SEL_PosL                         0
#define CCMU_WDG0_FCLK_SRC_SEL_PosH                         1


/*!< 0x00a4 CCMU_WDG1_FCLK_SRC_SEL
*null
*/
#define CCMU_WDG1_POSTDIV_Msk                               (0x07 << CCMU_WDG1_POSTDIV_Pos)
#define CCMU_WDG1_POSTDIV_Pos                               8
#define CCMU_WDG1_POSTDIV_PosL                              8
#define CCMU_WDG1_POSTDIV_PosH                              10

/*!< 0x00a4 CCMU_WDG1_FCLK_SRC_SEL
*null
*/
#define CCMU_WDG1_FCLK_SRC_SEL_Msk                          (0x03 << CCMU_WDG1_FCLK_SRC_SEL_Pos)
#define CCMU_WDG1_FCLK_SRC_SEL_Pos                          0
#define CCMU_WDG1_FCLK_SRC_SEL_PosL                         0
#define CCMU_WDG1_FCLK_SRC_SEL_PosH                         1


/*!< 0x00a8 CCMU_UART2_FCLK_SRC_SEL
*null
*/
#define CCMU_UART2_FCLK_POSTDIV_Msk                         (0x03 << CCMU_UART2_FCLK_POSTDIV_Pos)
#define CCMU_UART2_FCLK_POSTDIV_Pos                         4
#define CCMU_UART2_FCLK_POSTDIV_PosL                        4
#define CCMU_UART2_FCLK_POSTDIV_PosH                        5

/*!< 0x00a8 CCMU_UART2_FCLK_SRC_SEL
*null
*/
#define CCMU_UART2_FCLK_SRC_SEL_Msk                         (0x07 << CCMU_UART2_FCLK_SRC_SEL_Pos)
#define CCMU_UART2_FCLK_SRC_SEL_Pos                         0
#define CCMU_UART2_FCLK_SRC_SEL_PosL                        0
#define CCMU_UART2_FCLK_SRC_SEL_PosH                        2


/*!< 0x00ac CCMU_MCAN_FCLK_SRC_SEL
*null
*/
#define MCAN2_INT_N_Msk                                     (0x01 << MCAN2_INT_N_Pos)
#define MCAN2_INT_N_Pos                                     8
#define MCAN2_INT_N_PosL                                    8
#define MCAN2_INT_N_PosH                                    8

/*!< 0x00ac CCMU_MCAN_FCLK_SRC_SEL
*null
*/
#define CCMU_MCAN2_FCLK_POSTDIV_Msk                         (0x03 << CCMU_MCAN2_FCLK_POSTDIV_Pos)
#define CCMU_MCAN2_FCLK_POSTDIV_Pos                         4
#define CCMU_MCAN2_FCLK_POSTDIV_PosL                        4
#define CCMU_MCAN2_FCLK_POSTDIV_PosH                        5

/*!< 0x00ac CCMU_MCAN_FCLK_SRC_SEL
*null
*/
#define CCMU_MCAN2_FCLK_SRC_SEL_Msk                         (0x03 << CCMU_MCAN2_FCLK_SRC_SEL_Pos)
#define CCMU_MCAN2_FCLK_SRC_SEL_Pos                         0
#define CCMU_MCAN2_FCLK_SRC_SEL_PosL                        0
#define CCMU_MCAN2_FCLK_SRC_SEL_PosH                        1


/*!< 0x00b4 CCMU_CDAA_FCLK_DIV
*null
*/
#define CCMU_CDAA_FCLK_POSTDIV_Msk                          (0x03 << CCMU_CDAA_FCLK_POSTDIV_Pos)
#define CCMU_CDAA_FCLK_POSTDIV_Pos                          0
#define CCMU_CDAA_FCLK_POSTDIV_PosL                         0
#define CCMU_CDAA_FCLK_POSTDIV_PosH                         1


/**
* @}
* End of group HW_CCMU1
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_CCMU1_H_ */
