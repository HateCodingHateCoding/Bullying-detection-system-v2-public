/****************************************************************
* @file  hw_syscfg.h
* @brief hw_syscfg hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_SYSCFG_H_
#define _HW_SYSCFG_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_SYSCFG
 * @{
 * */


/* Type definition for device register.
 * ----------------------------------------------------------------------
 */

typedef struct {
		  uint32_t RSVD[2];								    /*0x0000-0x0004       */
    __IOM uint32_t SYSCFG_CPUF_H2H_BYPASS_CTRL;             /*0x0008,             */
    __IOM uint32_t CPU_TDT_SEL;                             /*0x000c,             */
    __IM  uint32_t EFUSE_DIS_CPUS_FLAG;                     /*0x0010,             */
    __IM  uint32_t EFUSE_FUNC_MAP;                          /*0x0014,             */
          uint32_t RSVD1[3];                                /*0x0018-0X0020,      */
    __IOM uint32_t SYSCFG_DBGIO_SRC_SEL;                    /*0x0024,             */
    __IOM uint32_t SYSCFG_CDAA_OUT_CTRL;                    /*0x0028,             */
          uint32_t RSVD2;                                   /*0x002C,             */
    __IOM uint32_t SYSCFG_CDMA_DATA_CUBE_SEL;               /*0x0030,             */
          uint32_t RSVD3;                                   /*0x0034,             */
    __IOM uint32_t SYSCFG_RFDIG_CFG_SEL;                    /*0x0038,             */
          uint32_t RSVD4;                                   /*0x003C,             */
    __IOM uint32_t CPU_RAMCFG;                              /*0x0040,             */
    __IOM uint32_t SYSCFG_ROM_CFG;                          /*0x0044,             */
    __IOM uint32_t SOC_SRAM_CFG;                            /*0x0048,             */
    __IOM uint32_t CPUF_SRAM_CFG;                           /*0x004C,             */
          uint32_t RSVD5[8];                                /*0x0050-0x006C,      */
    __IOM uint32_t SYSCFG_FREQ_CAL_CTRL;                    /*0x0070,             */
     __IOM uint32_t FREQ_CAL_VAL;                           /*0x0074,             */
          uint32_t RSVD6[34];                               /*0x0078-0x00FC,      */
    __IOM uint32_t SYSCFG_INT_EN;                           /*0x0100,             */
    __IOM uint32_t SYSCFG_CLR_EN;                           /*0x0104,             */
    __IOM uint32_t SYSCFG_CPUS_INTR_MASK[4];                /*0x0108-0x0114,      */
    __IOM uint32_t SYSCFG_CPUF_INTR_MASK[4];                /*0x0118-0x0124,      */
		  uint32_t RSVD7[14];   							/*0x0128-0x015C,	  */
    __IOM uint32_t CPUF_DEENC_START_ADDR;                   /*0x0160,             */
    __IOM uint32_t CPUF_DEENC_END_ADDR;                     /*0x0164,             */
    __OM  uint32_t CPUF_DEENC_KEY[4];                       /*0x0168-0x0174,      */
    __IOM uint32_t CPUF_DEENC_EN;                           /*0x0178,             */
    __IOM uint32_t CPUF_DEENC_STATUS;                       /*0x017C,             */
          uint32_t RSVD8[32];                               /*0x0180-0x01FC,      */
    __IOM uint32_t DATACUBE_REMAP_EN;                       /*0x0200,             */
    __IOM uint32_t DATACUBE_REMAP_BDADDR;                   /*0x0204,             */
          uint32_t RSVD9;                                   /*0x0208,             */
    __IOM uint32_t CPU_I_REMAP_BDADDR[4];                   /*0x020C-0x00218,     */
          uint32_t RSVD10[5];                               /*0x021C-0x022C,      */
    __IOM uint32_t CPU_I_REMAP_SIZE[2];                     /*0x0230-0x0234,      */
          uint32_t RSVD11[2];                               /*0x0238-0X023C,      */
    __IOM uint32_t CPUS_D_REMAP_BDADDR[3];                  /*0x0240-0X0248,      */
          uint32_t RSVD12;                                  /*0x024C,             */
    __IOM uint32_t CPUS_D_REMAP_SIZE[2];                    /*0x0250-0x0254,      */
          uint32_t RSVD13[2];                               /*0x0258-0x025C,      */
    __IOM uint32_t CPUF_D_REMAP_BDADDR[3];                  /*0x0260-0x0268,      */
          uint32_t RSVD14;                                  /*0x026C,             */
    __IOM uint32_t CPUF_D_REMAP_SIZE[2];                    /*0x0270-0x0274,      */
} syscfg_dev_t;

#define SYSCFG_DEV                      ((syscfg_dev_t *)   SYSCFG_BASE_ADDR)/*!< 0x4001 C000 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x0008 SYSCFG_CPUF_H2H_BYPASS_CTRL
*null
*/
#define CPUF_H2H_BYPASS_EN_Msk                              (0x01 << CPUF_H2H_BYPASS_EN_Pos)
#define CPUF_H2H_BYPASS_EN_Pos                              0
#define CPUF_H2H_BYPASS_EN_PosL                             0
#define CPUF_H2H_BYPASS_EN_PosH                             0


/*!< 0x000c CPU_TDT_SEL
*null
*/
#define CPUF_DBGEN_Msk                                      (0x01 << CPUF_DBGEN_Pos)
#define CPUF_DBGEN_Pos                                      9
#define CPUF_DBGEN_PosL                                     9
#define CPUF_DBGEN_PosH                                     9

/*!< 0x000c CPU_TDT_SEL
*null
*/
#define CPUS_DBGEN_Msk                                      (0x01 << CPUS_DBGEN_Pos)
#define CPUS_DBGEN_Pos                                      8
#define CPUS_DBGEN_PosL                                     8
#define CPUS_DBGEN_PosH                                     8

/*!< 0x000c CPU_TDT_SEL
*null
*/
#define CPUF_TDT_SEL_Msk                                    (0x01 << CPUF_TDT_SEL_Pos)
#define CPUF_TDT_SEL_Pos                                    1
#define CPUF_TDT_SEL_PosL                                   1
#define CPUF_TDT_SEL_PosH                                   1

/*!< 0x000c CPU_TDT_SEL
*null
*/
#define CPUS_TDT_SEL_Msk                                    (0x01 << CPUS_TDT_SEL_Pos)
#define CPUS_TDT_SEL_Pos                                    0
#define CPUS_TDT_SEL_PosL                                   0
#define CPUS_TDT_SEL_PosH                                   0


/*!< 0x0010 EFUSE_DIS_CPUS_FLAG
*null
*/
#define EFUSE_DIS_CPUS_FLAG_Msk                             (0x01 << EFUSE_DIS_CPUS_FLAG_Pos)
#define EFUSE_DIS_CPUS_FLAG_Pos                             0
#define EFUSE_DIS_CPUS_FLAG_PosL                            0
#define EFUSE_DIS_CPUS_FLAG_PosH                            0


/*!< 0x0014 EFUSE_FUNC_MAP
*null
*/
#define EFUSE_KEY_RD_DIS_EN_Msk                             (0x01 << EFUSE_KEY_RD_DIS_EN_Pos)
#define EFUSE_KEY_RD_DIS_EN_Pos                             1
#define EFUSE_KEY_RD_DIS_EN_PosL                            1
#define EFUSE_KEY_RD_DIS_EN_PosH                            1

/*!< 0x0014 EFUSE_FUNC_MAP
*null
*/
#define EFUSE_KEY_WHITEN_EN_Msk                             (0x01 << EFUSE_KEY_WHITEN_EN_Pos)
#define EFUSE_KEY_WHITEN_EN_Pos                             0
#define EFUSE_KEY_WHITEN_EN_PosL                            0
#define EFUSE_KEY_WHITEN_EN_PosH                            0


/*!< 0x0024 SYSCFG_DBGIO_SRC_SEL
*null
*/
#define DBGIO_SRC_SEL_Msk                                   (0x0F << DBGIO_SRC_SEL_Pos)
#define DBGIO_SRC_SEL_Pos                                   0
#define DBGIO_SRC_SEL_PosL                                  0
#define DBGIO_SRC_SEL_PosH                                  3


/*!< 0x0028 SYSCFG_CDAA_OUT_CTRL
*null
*/
#define CDAA_OUT_SEL_Msk                                    (0x03 << CDAA_OUT_SEL_Pos)
#define CDAA_OUT_SEL_Pos                                    4
#define CDAA_OUT_SEL_PosL                                   4
#define CDAA_OUT_SEL_PosH                                   5

/*!< 0x0028 SYSCFG_CDAA_OUT_CTRL
*null
*/
#define CDAA_OUT_EN_Msk                                     (0x01 << CDAA_OUT_EN_Pos)
#define CDAA_OUT_EN_Pos                                     0
#define CDAA_OUT_EN_PosL                                    0
#define CDAA_OUT_EN_PosH                                    0


/*!< 0x0030 CDMA_DATA_CUBE_SEL
*null
*/
#define CDMA_DATA_CUBE_SEL_Msk                              (0xFFFF << CDMA_DATA_CUBE_SEL_Pos)
#define CDMA_DATA_CUBE_SEL_Pos                              0
#define CDMA_DATA_CUBE_SEL_PosL                             0
#define CDMA_DATA_CUBE_SEL_PosH                             15


/*!< 0x0038 SYSCFG_RFDIG_CFG_SEL
*null
*/
#define SPI_AHB_SEL_Msk                                     (0x01 << SPI_AHB_SEL_Pos)
#define SPI_AHB_SEL_Pos                                     0
#define SPI_AHB_SEL_PosL                                    0
#define SPI_AHB_SEL_PosH                                    0


/*!< 0x0040 CPU_RAMCFG
*null
*/
#define CPUF_RAMCFG_Msk                                     (0x1F << CPUF_RAMCFG_Pos)
#define CPUF_RAMCFG_Pos                                     5
#define CPUF_RAMCFG_PosL                                    5
#define CPUF_RAMCFG_PosH                                    9

/*!< 0x0040 CPU_RAMCFG
*null
*/
#define CPUS_RAMCFG_Msk                                     (0x1F << CPUS_RAMCFG_Pos)
#define CPUS_RAMCFG_Pos                                     0
#define CPUS_RAMCFG_PosL                                    0
#define CPUS_RAMCFG_PosH                                    4


/*!< 0x0044 SYSCFG_ROM_CFG
*null
*/
#define ROMCFG_Msk                                          (0x1F << ROMCFG_Pos)
#define ROMCFG_Pos                                          0
#define ROMCFG_PosL                                         0
#define ROMCFG_PosH                                         4


/*!< 0x0048 SOC_SRAM_CFG
*null
*/
#define RFSP_CFG_Msk                                        (0x1F << RFSP_CFG_Pos)
#define RFSP_CFG_Pos                                        16
#define RFSP_CFG_PosL                                       16
#define RFSP_CFG_PosH                                       20

/*!< 0x0048 SOC_SRAM_CFG
*null
*/
#define RAMSP_CFG_Msk                                       (0x1F << RAMSP_CFG_Pos)
#define RAMSP_CFG_Pos                                       0
#define RAMSP_CFG_PosL                                      0
#define RAMSP_CFG_PosH                                      4


/*!< 0x004c CPUF_SRAM_CFG
*null
*/
#define CPUF_RFSP_CFG_Msk                                   (0x1F << CPUF_RFSP_CFG_Pos)
#define CPUF_RFSP_CFG_Pos                                   16
#define CPUF_RFSP_CFG_PosL                                  16
#define CPUF_RFSP_CFG_PosH                                  20


/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define LFCLK_CNT_TARGET_Msk                                (0x3FFFFF << LFCLK_CNT_TARGET_Pos)
#define LFCLK_CNT_TARGET_Pos                                10
#define LFCLK_CNT_TARGET_PosL                               10
#define LFCLK_CNT_TARGET_PosH                               31

/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define CAL_FINISH_FLAG_Msk                                 (0x01 << CAL_FINISH_FLAG_Pos)
#define CAL_FINISH_FLAG_Pos                                 9
#define CAL_FINISH_FLAG_PosL                                9
#define CAL_FINISH_FLAG_PosH                                9

/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define START_Msk                                           (0x01 << START_Pos)
#define START_Pos                                           8
#define START_PosL                                          8
#define START_PosH                                          8

/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define RESETN_EN_Msk                                       (0x01 << RESETN_EN_Pos)
#define RESETN_EN_Pos                                       7
#define RESETN_EN_PosL                                      7
#define RESETN_EN_PosH                                      7

/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define LFCLK_EN_Msk                                        (0x01 << LFCLK_EN_Pos)
#define LFCLK_EN_Pos                                        4
#define LFCLK_EN_PosL                                       4
#define LFCLK_EN_PosH                                       4

/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define LFCLK_SRC_SEL_Msk                                   (0x03 << LFCLK_SRC_SEL_Pos)
#define LFCLK_SRC_SEL_Pos                                   2
#define LFCLK_SRC_SEL_PosL                                  2
#define LFCLK_SRC_SEL_PosH                                  3

/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define HFCLK_EN_Msk                                        (0x01 << HFCLK_EN_Pos)
#define HFCLK_EN_Pos                                        1
#define HFCLK_EN_PosL                                       1
#define HFCLK_EN_PosH                                       1

/*!< 0x0070 SYSCFG_FREQ_CAL_CTRL
*null
*/
#define HFCLK_SRC_SEL_Msk                                   (0x01 << HFCLK_SRC_SEL_Pos)
#define HFCLK_SRC_SEL_Pos                                   0
#define HFCLK_SRC_SEL_PosL                                  0
#define HFCLK_SRC_SEL_PosH                                  0

/*!< 0x0074 SYSCFG_FREQ_CAL_VAL
*null
*/
#define FREQ_CAL_VAL_Msk                                    (0x3FFFFF << FREQ_CAL_VAL_Pos)
#define FREQ_CAL_VAL_Pos                                    0
#define FREQ_CAL_VAL_PosL                                   0
#define FREQ_CAL_VAL_PosH                                   21

/*!< 0x0100 SYSCFG_INT_EN
*null
*/
#define RF_REG_ACK_ERR_INT_EN_Msk                           (0x01 << RF_REG_ACK_ERR_INT_EN_Pos)
#define RF_REG_ACK_ERR_INT_EN_Pos                           5
#define RF_REG_ACK_ERR_INT_EN_PosL                          5
#define RF_REG_ACK_ERR_INT_EN_PosH                          5

/*!< 0x0100 SYSCFG_INT_EN
*null
*/
#define PEAK_OVERFLOW_INT_EN_Msk                            (0x01 << PEAK_OVERFLOW_INT_EN_Pos)
#define PEAK_OVERFLOW_INT_EN_Pos                            4
#define PEAK_OVERFLOW_INT_EN_PosL                           4
#define PEAK_OVERFLOW_INT_EN_PosH                           4

/*!< 0x0100 SYSCFG_INT_EN
*null
*/
#define TSENSOR_OVERFLOW_INT_EN_Msk                         (0x01 << TSENSOR_OVERFLOW_INT_EN_Pos)
#define TSENSOR_OVERFLOW_INT_EN_Pos                         3
#define TSENSOR_OVERFLOW_INT_EN_PosL                        3
#define TSENSOR_OVERFLOW_INT_EN_PosH                        3

/*!< 0x0100 SYSCFG_INT_EN
*null
*/
#define MMW_DBG_END_INT_EN_Msk                              (0x01 << MMW_DBG_END_INT_EN_Pos)
#define MMW_DBG_END_INT_EN_Pos                              2
#define MMW_DBG_END_INT_EN_PosL                             2
#define MMW_DBG_END_INT_EN_PosH                             2

/*!< 0x0100 SYSCFG_INT_EN
*null
*/
#define SPLL_READY_INT_EN_Msk                               (0x01 << SPLL_READY_INT_EN_Pos)
#define SPLL_READY_INT_EN_Pos                               1
#define SPLL_READY_INT_EN_PosL                              1
#define SPLL_READY_INT_EN_PosH                              1

/*!< 0x0100 SYSCFG_INT_EN
*null
*/
#define DCXO_READY_INT_EN_Msk                               (0x01 << DCXO_READY_INT_EN_Pos)
#define DCXO_READY_INT_EN_Pos                               0
#define DCXO_READY_INT_EN_PosL                              0
#define DCXO_READY_INT_EN_PosH                              0


/*!< 0x0104 SYSCFG_INT_EN
*null
*/
#define RF_REG_ACK_ERR_INT_FLAG_Msk                         (0x01 << RF_REG_ACK_ERR_INT_FLAG_Pos)
#define RF_REG_ACK_ERR_INT_FLAG_Pos                         5
#define RF_REG_ACK_ERR_INT_FLAG_PosL                        5
#define RF_REG_ACK_ERR_INT_FLAG_PosH                        5

/*!< 0x0104 SYSCFG_INT_EN
*null
*/
#define MMW_DBG_END_INT_FLAG_Msk                            (0x01 << MMW_DBG_END_INT_FLAG_Pos)
#define MMW_DBG_END_INT_FLAG_Pos                            2
#define MMW_DBG_END_INT_FLAG_PosL                           2
#define MMW_DBG_END_INT_FLAG_PosH                           2

/*!< 0x0104 SYSCFG_INT_EN
*null
*/
#define SPLL_READY_INT_FLAG_Msk                             (0x01 << SPLL_READY_INT_FLAG_Pos)
#define SPLL_READY_INT_FLAG_Pos                             1
#define SPLL_READY_INT_FLAG_PosL                            1
#define SPLL_READY_INT_FLAG_PosH                            1

/*!< 0x0104 SYSCFG_INT_EN
*null
*/
#define DCXO_READY_INT_FLAG_Msk                             (0x01 << DCXO_READY_INT_FLAG_Pos)
#define DCXO_READY_INT_FLAG_Pos                             0
#define DCXO_READY_INT_FLAG_PosL                            0
#define DCXO_READY_INT_FLAG_PosH                            0


/*!< 0x0108 SYSCFG_CPUS_INTR_MASK
*null
*/
#define CPUS_INTR_MASK0_Msk                                 (0x01 << CPUS_INTR_MASK0_Pos)
#define CPUS_INTR_MASK0_Pos                                 0
#define CPUS_INTR_MASK0_PosL                                0
#define CPUS_INTR_MASK0_PosH                                0


/*!< 0x010c SYSCFG_CPUS_INTR_MASK1
*null
*/
#define CPUS_INTR_MASK1_Msk                                 (0x01 << CPUS_INTR_MASK1_Pos)
#define CPUS_INTR_MASK1_Pos                                 0
#define CPUS_INTR_MASK1_PosL                                0
#define CPUS_INTR_MASK1_PosH                                0


/*!< 0x0110 SYSCFG_CPUS_INTR_MASK
*null
*/
#define CPUS_INTR_MASK2_Msk                                 (0x01 << CPUS_INTR_MASK2_Pos)
#define CPUS_INTR_MASK2_Pos                                 0
#define CPUS_INTR_MASK2_PosL                                0
#define CPUS_INTR_MASK2_PosH                                0


/*!< 0x0118 SYSCFG_CPUF_INTR_MASK
*null
*/
#define CPUF_INTR_MASK0_Msk                                 (0x01 << CPUF_INTR_MASK0_Pos)
#define CPUF_INTR_MASK0_Pos                                 0
#define CPUF_INTR_MASK0_PosL                                0
#define CPUF_INTR_MASK0_PosH                                0


/*!< 0x011c SYSCFG_CPUF_INTR_MASK
*null
*/
#define CPUF_INTR_MASK1_Msk                                 (0x01 << CPUF_INTR_MASK1_Pos)
#define CPUF_INTR_MASK1_Pos                                 0
#define CPUF_INTR_MASK1_PosL                                0
#define CPUF_INTR_MASK1_PosH                                0


/*!< 0x0120 SYSCFG_CPUF_INTR_MASK
*null
*/
#define CPUF_INTR_MASK2_Msk                                 (0x01 << CPUF_INTR_MASK2_Pos)
#define CPUF_INTR_MASK2_Pos                                 0
#define CPUF_INTR_MASK2_PosL                                0
#define CPUF_INTR_MASK2_PosH                                0


/*!< 0x160h CPUF_DEENC_START_ADDR
*null
*/
#define CPU_DEENC_START_ADDR_Msk                            (0xFFFFF << CPU_DEENC_START_ADDR_Pos)
#define CPU_DEENC_START_ADDR_Pos                            0
#define CPU_DEENC_START_ADDR_PosL                           0
#define CPU_DEENC_START_ADDR_PosH                           23


/*!< 0x0164 CPUF_DEENC_END_ADDR
*null
*/
#define CPU_DEENC_START_ADDR_Msk                            (0xFFFFF << CPU_DEENC_START_ADDR_Pos)
#define CPU_DEENC_START_ADDR_Pos                            0
#define CPU_DEENC_START_ADDR_PosL                           0
#define CPU_DEENC_START_ADDR_PosH                           23


/*!< 0x0168 CPUF_DEENC_KEY0
*null
*/
#define CPU_DEENC_KEY0_Msk                                  (0XFFFFFFF << CPU_DEENC_KEY0_Pos)
#define CPU_DEENC_KEY0_Pos                                  31
#define CPU_DEENC_KEY0_PosL                                 0
#define CPU_DEENC_KEY0_PosH                                 31


/*!< 0x016c CPUF_DEENC_KEY1
*null
*/
#define CPU_DEENC_KEY1_Msk                                  (0XFFFFFFF << CPU_DEENC_KEY1_Pos)
#define CPU_DEENC_KEY1_Pos                                  0
#define CPU_DEENC_KEY1_PosL                                 0
#define CPU_DEENC_KEY1_PosH                                 31


/*!< 0x0170 CPUF_DEENC_KEY2
*null
*/
#define CPU_DEENC_KEY2_Msk                                  (0XFFFFFFF << CPU_DEENC_KEY2_Pos)
#define CPU_DEENC_KEY2_Pos                                  0
#define CPU_DEENC_KEY2_PosL                                 0
#define CPU_DEENC_KEY2_PosH                                 31


/*!< 0x0174 CPUF_DEENC_KEY3
*null
*/
#define CPU_DEENC_KEY3_Msk                                  (0XFFFFFFF << CPU_DEENC_KEY3_Pos)
#define CPU_DEENC_KEY3_Pos                                  0
#define CPU_DEENC_KEY3_PosL                                 0
#define CPU_DEENC_KEY3_PosH                                 31


/*!< 0x0178 CPUF_DEENC_EN
*null
*/
#define DEENC_ONLY_INSTRUCTION_EN_Msk                       (0x01 << DEENC_ONLY_INSTRUCTION_EN_Pos)
#define DEENC_ONLY_INSTRUCTION_EN_Pos                       4
#define DEENC_ONLY_INSTRUCTION_EN_PosL                      4
#define DEENC_ONLY_INSTRUCTION_EN_PosH                      4

/*!< 0x0178 CPUF_DEENC_EN
*null
*/
#define CPUS_DEENC_EN_Msk                                   (0x01 << CPUS_DEENC_EN_Pos)
#define CPUS_DEENC_EN_Pos                                   1
#define CPUS_DEENC_EN_PosL                                  1
#define CPUS_DEENC_EN_PosH                                  1

/*!< 0x0178 CPUF_DEENC_EN
*null
*/
#define CPUF_DEENC_EN_Msk                                   (0x01 << CPUF_DEENC_EN_Pos)
#define CPUF_DEENC_EN_Pos                                   0
#define CPUF_DEENC_EN_PosL                                  0
#define CPUF_DEENC_EN_PosH                                  0


/*!< 0x017c CPUF_DEENC_STATUS
*null
*/
#define CPUF_DEENC_STATUS_Msk                               (0x01 << CPUF_DEENC_STATUS_Pos)
#define CPUF_DEENC_STATUS_Pos                               0
#define CPUF_DEENC_STATUS_PosL                              0
#define CPUF_DEENC_STATUS_PosH                              0


/*!< 0x0200 DATACUBE_REMAP_EN
*null
*/
#define DATACUBE_REMAP_EN_Msk                               (0x01 << DATACUBE_REMAP_EN_Pos)
#define DATACUBE_REMAP_EN_Pos                               0
#define DATACUBE_REMAP_EN_PosL                              0
#define DATACUBE_REMAP_EN_PosH                              0


/*!< 0x0204 DATACUBE_REMAP_BDADDR
*null
*/
#define DATACUBE_REMAP_BDADDR_Msk                           (0X1FFF << DATACUBE_REMAP_BDADDR_Pos)
#define DATACUBE_REMAP_BDADDR_Pos                           7
#define DATACUBE_REMAP_BDADDR_PosL                          7
#define DATACUBE_REMAP_BDADDR_PosH                          19


/*!< 0x020c CPU_I_REMAP0_BDADDR
*null
*/
#define CPU_I_REMAP0_BDADDR_Msk                             (0X1FFFFFF << CPU_I_REMAP0_BDADDR_Pos)
#define CPU_I_REMAP0_BDADDR_Pos                             7
#define CPU_I_REMAP0_BDADDR_PosL                            7
#define CPU_I_REMAP0_BDADDR_PosH                            31


/*!< 0x0210 CPU_I_REMAP1_BDADDR
*null
*/
#define CPU_I_REMAP1_BDADDR_Msk                             (0X1FFF << CPU_I_REMAP1_BDADDR_Pos)
#define CPU_I_REMAP1_BDADDR_Pos                             7
#define CPU_I_REMAP1_BDADDR_PosL                            7
#define CPU_I_REMAP1_BDADDR_PosH                            19


/*!< 0x0214 CPU_I_REMAP2_BDADDR
*null
*/
#define CPU_I_REMAP2_BDADDR_Msk                             (0X1FFF << CPU_I_REMAP2_BDADDR_Pos)
#define CPU_I_REMAP2_BDADDR_Pos                             7
#define CPU_I_REMAP2_BDADDR_PosL                            7
#define CPU_I_REMAP2_BDADDR_PosH                            19


/*!< 0x0218 CPU_I_REMAP3_BDADDR
*null
*/
#define CPU_I_REMAP3_BDADDR_Msk                             (0X1FFF << CPU_I_REMAP3_BDADDR_Pos)
#define CPU_I_REMAP3_BDADDR_Pos                             7
#define CPU_I_REMAP3_BDADDR_PosL                            7
#define CPU_I_REMAP3_BDADDR_PosH                            19


/*!< 0x0230 CPU_I_REMAP01_SIZE
*null
*/
#define CPU_I_REMAP1_SIZE_Msk                               (0x7FF << CPU_I_REMAP1_SIZE_Pos)
#define CPU_I_REMAP1_SIZE_Pos                               16
#define CPU_I_REMAP1_SIZE_PosL                              16
#define CPU_I_REMAP1_SIZE_PosH                              26

/*!< 0x0230 CPU_I_REMAP01_SIZE
*null
*/
#define CPU_I_REMAP0_SIZE_Msk                               (0x7FF << CPU_I_REMAP0_SIZE_Pos)
#define CPU_I_REMAP0_SIZE_Pos                               0
#define CPU_I_REMAP0_SIZE_PosL                              0
#define CPU_I_REMAP0_SIZE_PosH                              10


/*!< 0x0234 CPU_I_REMAP2_SIZE
*null
*/
#define CPU_I_REMAP3_SIZE_Msk                               (0x7FF << CPU_I_REMAP3_SIZE_Pos)
#define CPU_I_REMAP3_SIZE_Pos                               16
#define CPU_I_REMAP3_SIZE_PosL                              16
#define CPU_I_REMAP3_SIZE_PosH                              26

/*!< 0x0234 CPU_I_REMAP2_SIZE
*null
*/
#define CPU_I_REMAP2_SIZE_Msk                               (0x7FF << CPU_I_REMAP2_SIZE_Pos)
#define CPU_I_REMAP2_SIZE_Pos                               0
#define CPU_I_REMAP2_SIZE_PosL                              0
#define CPU_I_REMAP2_SIZE_PosH                              10


/*!< 0x0240 CPUS_D_REMAP0_BDADDR
*null
*/
#define CPUS_D_REMAP0_BDADDR_Msk                            (0X1FFF << CPUS_D_REMAP0_BDADDR_Pos)
#define CPUS_D_REMAP0_BDADDR_Pos                            7
#define CPUS_D_REMAP0_BDADDR_PosL                           7
#define CPUS_D_REMAP0_BDADDR_PosH                           19


/*!< 0x0244 CPUS_D_REMAP1_BDADDR
*null
*/
#define CPUS_D_REMAP1_BDADDR_Msk                            (0X1FFF << CPUS_D_REMAP1_BDADDR_Pos)
#define CPUS_D_REMAP1_BDADDR_Pos                            7
#define CPUS_D_REMAP1_BDADDR_PosL                           7
#define CPUS_D_REMAP1_BDADDR_PosH                           19


/*!< 0x0248 CPUS_D_REMAP2_BDADDR
*null
*/
#define CPUS_D_REMAP2_BDADDR_Msk                            (0X1FFF << CPUS_D_REMAP2_BDADDR_Pos)
#define CPUS_D_REMAP2_BDADDR_Pos                            7
#define CPUS_D_REMAP2_BDADDR_PosL                           7
#define CPUS_D_REMAP2_BDADDR_PosH                           19


/*!< 0x0250 CPUS_D_REMAP01_SIZE
*null
*/
#define CPUS_D_REMAP1_SIZE_Msk                              (0x7FF << CPUS_D_REMAP1_SIZE_Pos)
#define CPUS_D_REMAP1_SIZE_Pos                              16
#define CPUS_D_REMAP1_SIZE_PosL                             16
#define CPUS_D_REMAP1_SIZE_PosH                             26

/*!< 0x0250 CPUS_D_REMAP01_SIZE
*null
*/
#define CPUS_D_REMAP0_SIZE_Msk                              (0x7FF << CPUS_D_REMAP0_SIZE_Pos)
#define CPUS_D_REMAP0_SIZE_Pos                              0
#define CPUS_D_REMAP0_SIZE_PosL                             0
#define CPUS_D_REMAP0_SIZE_PosH                             10


/*!< 0x0254 CPUS_D_REMAP23_SIZE
*null
*/
#define CPUS_D_REMAP2_SIZE_Msk                              (0x7FF << CPUS_D_REMAP2_SIZE_Pos)
#define CPUS_D_REMAP2_SIZE_Pos                              0
#define CPUS_D_REMAP2_SIZE_PosL                             0
#define CPUS_D_REMAP2_SIZE_PosH                             10


/*!< 0x0260 CPUF_D_REMAP0_BDADDR
*null
*/
#define CPUF_D_REMAP0_BDADDR_Msk                            (0X1FFF << CPUF_D_REMAP0_BDADDR_Pos)
#define CPUF_D_REMAP0_BDADDR_Pos                            7
#define CPUF_D_REMAP0_BDADDR_PosL                           7
#define CPUF_D_REMAP0_BDADDR_PosH                           19


/*!< 0x0264 CPUF_D_REMAP1_BDADDR
*null
*/
#define CPUF_D_REMAP1_BDADDR_Msk                            (0X1FFF << CPUF_D_REMAP1_BDADDR_Pos)
#define CPUF_D_REMAP1_BDADDR_Pos                            7
#define CPUF_D_REMAP1_BDADDR_PosL                           7
#define CPUF_D_REMAP1_BDADDR_PosH                           19


/*!< 0x0268 CPUF_D_REMAP2_BDADDR
*null
*/
#define CPUF_D_REMAP2_BDADDR_Msk                            (0X1FFF << CPUF_D_REMAP2_BDADDR_Pos)
#define CPUF_D_REMAP2_BDADDR_Pos                            7
#define CPUF_D_REMAP2_BDADDR_PosL                           7
#define CPUF_D_REMAP2_BDADDR_PosH                           19


/*!< 0x0270 CPUF_D_REMAP01_SIZE
*null
*/
#define CPUF_D_REMAP1_SIZE_Msk                              (0x7FF << CPUF_D_REMAP1_SIZE_Pos)
#define CPUF_D_REMAP1_SIZE_Pos                              16
#define CPUF_D_REMAP1_SIZE_PosL                             16
#define CPUF_D_REMAP1_SIZE_PosH                             26

/*!< 0x0270 CPUF_D_REMAP01_SIZE
*null
*/
#define CPUF_D_REMAP0_SIZE_Msk                              (0x7FF << CPUF_D_REMAP0_SIZE_Pos)
#define CPUF_D_REMAP0_SIZE_Pos                              0
#define CPUF_D_REMAP0_SIZE_PosL                             0
#define CPUF_D_REMAP0_SIZE_PosH                             10


/*!< 0x0274 CPUF_D_REMAP2_SIZE
*null
*/
#define CPUF_D_REMAP2_SIZE_Msk                              (0x7FF << CPUF_D_REMAP2_SIZE_Pos)
#define CPUF_D_REMAP2_SIZE_Pos                              0
#define CPUF_D_REMAP2_SIZE_PosL                             0
#define CPUF_D_REMAP2_SIZE_PosH                             10


/**
* @}
* End of group HW_SYSCFG
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_SYSCFG_H_ */
