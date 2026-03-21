/****************************************************************
* @file  hw_swcfg.h
* @brief hw_swcfg hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_SWCFG_H_
#define _HW_SWCFG_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_SWCFG
 * @{
 * */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */

#define PSIC_XIP_REMAP_GROUP_MAX_NUM    10

typedef struct {
    __IOM uint32_t SWCFG_SRAM_SHARE;                        /*0x0000,             */
    __IOM uint32_t SWCFG_HICM_REMAP;                        /*0x0004,             */
    __IOM uint32_t SWCFG_PKA_DPA_DIS;                       /*0x0008,             */
          uint32_t RSVD;                                    /*0x000C,             */
    __IOM uint32_t SWCFG_XIP_REMAP_EN;                      /*0x0010,             */
    __IOM uint32_t SWCFG_XIP_REMAP_START[PSIC_XIP_REMAP_GROUP_MAX_NUM];               /*0x0014-0x0038,      */
    __IOM uint32_t SWCFG_XIP_REMAP_SIZE[PSIC_XIP_REMAP_GROUP_MAX_NUM];                /*0x003C-0x0060,      */
    __IOM uint32_t SWCFG_XIP_REMAP[PSIC_XIP_REMAP_GROUP_MAX_NUM];                     /*0x0064-0x0088,      */
} swcfg_dev_t;

#define SWCFG_DEV                       ((swcfg_dev_t *)  SWCFG_BASE_ADDR) /*!< 0x4001 7400 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x0000 SWCFG_SRAM_SHARE
*null
*/
#define UIF_DBG23_EN_Msk                                    (0x01 << UIF_DBG23_EN_Pos)
#define UIF_DBG23_EN_Pos                                    5
#define UIF_DBG23_EN_PosL                                   5
#define UIF_DBG23_EN_PosH                                   5

/*!< 0x0000 SWCFG_SRAM_SHARE
*null
*/
#define UIF_DBG01_EN_Msk                                    (0x01 << UIF_DBG01_EN_Pos)
#define UIF_DBG01_EN_Pos                                    4
#define UIF_DBG01_EN_PosL                                   4
#define UIF_DBG01_EN_PosH                                   4

/*!< 0x0000 SWCFG_SRAM_SHARE
*null
*/
#define LA_SRAM_EN_Msk                                      (0x01 << LA_SRAM_EN_Pos)
#define LA_SRAM_EN_Pos                                      3
#define LA_SRAM_EN_PosL                                     3
#define LA_SRAM_EN_PosH                                     3

/*!< 0x0000 SWCFG_SRAM_SHARE
*null
*/
#define M50_SRAM_EN_Msk                                     (0x01 << M50_SRAM_EN_Pos)
#define M50_SRAM_EN_Pos                                     2
#define M50_SRAM_EN_PosL                                    2
#define M50_SRAM_EN_PosH                                    2

/*!< 0x0000 SWCFG_SRAM_SHARE
*null
*/
#define PKA_SRAM_EN_Msk                                     (0x01 << PKA_SRAM_EN_Pos)
#define PKA_SRAM_EN_Pos                                     1
#define PKA_SRAM_EN_PosL                                    1
#define PKA_SRAM_EN_PosH                                    1

/*!< 0x0000 SWCFG_SRAM_SHARE
*null
*/
#define MMW_DBG_EN_Msk                                      (0x01 << MMW_DBG_EN_Pos)
#define MMW_DBG_EN_Pos                                      0
#define MMW_DBG_EN_PosL                                     0
#define MMW_DBG_EN_PosH                                     0


/*!< 0x0004 SWCFG_HICM_REMAP
*null
*/
#define HICM_CPU_FLAG_Msk                                   (0x01 << HICM_CPU_FLAG_Pos)
#define HICM_CPU_FLAG_Pos                                   1
#define HICM_CPU_FLAG_PosL                                  1
#define HICM_CPU_FLAG_PosH                                  1

/*!< 0x0004 SWCFG_HICM_REMAP
*null
*/
#define HICM_HADDR_REMAP_EN_Msk                             (0x01 << HICM_HADDR_REMAP_EN_Pos)
#define HICM_HADDR_REMAP_EN_Pos                             0
#define HICM_HADDR_REMAP_EN_PosL                            0
#define HICM_HADDR_REMAP_EN_PosH                            0


/*!< 0x0008 SWCFG_PKA_DPA_DIS
*null
*/
#define SWCFG_PKA_DPA_DIS_Msk                               (0x01 << SWCFG_PKA_DPA_DIS_Pos)
#define SWCFG_PKA_DPA_DIS_Pos                               0
#define SWCFG_PKA_DPA_DIS_PosL                              0
#define SWCFG_PKA_DPA_DIS_PosH                              0


/*!< 0x0010 SWCFG_XIP_REMAP_EN
*null
*/
#define XIP_HADDR_REMAP_EN_Msk                              (0x01 << XIP_HADDR_REMAP_EN_Pos)
#define XIP_HADDR_REMAP_EN_Pos                              0
#define XIP_HADDR_REMAP_EN_PosL                             0
#define XIP_HADDR_REMAP_EN_PosH                             0


/*!< 0x0014 SWCFG_XIP_REMAP0_START
*null
*/
#define XIP_REMAP0_START_HADDR_Msk                          (0X7FFF << XIP_REMAP0_START_HADDR_Pos)
#define XIP_REMAP0_START_HADDR_Pos                          12
#define XIP_REMAP0_START_HADDR_PosL                         12
#define XIP_REMAP0_START_HADDR_PosH                         26


/*!< 0x0018 SWCFG_XIP_REMAP1_START
*null
*/
#define XIP_REMAP1_START_HADDR_Msk                          (0X7FFF << XIP_REMAP1_START_HADDR_Pos)
#define XIP_REMAP1_START_HADDR_Pos                          12
#define XIP_REMAP1_START_HADDR_PosL                         12
#define XIP_REMAP1_START_HADDR_PosH                         26


/*!< 0x001c SWCFG_XIP_REMAP2_START
*null
*/
#define XIP_REMAP2_START_HADDR_Msk                          (0X7FFF << XIP_REMAP2_START_HADDR_Pos)
#define XIP_REMAP2_START_HADDR_Pos                          12
#define XIP_REMAP2_START_HADDR_PosL                         12
#define XIP_REMAP2_START_HADDR_PosH                         26


/*!< 0x0020 SWCFG_XIP_REMAP3_START
*null
*/
#define XIP_REMAP3_START_HADDR_Msk                          (0X7FFF << XIP_REMAP3_START_HADDR_Pos)
#define XIP_REMAP3_START_HADDR_Pos                          12
#define XIP_REMAP3_START_HADDR_PosL                         12
#define XIP_REMAP3_START_HADDR_PosH                         26


/*!< 0x0024 SWCFG_XIP_REMAP4_START
*null
*/
#define XIP_REMAP4_START_HADDR_Msk                          (0X7FFF << XIP_REMAP4_START_HADDR_Pos)
#define XIP_REMAP4_START_HADDR_Pos                          12
#define XIP_REMAP4_START_HADDR_PosL                         12
#define XIP_REMAP4_START_HADDR_PosH                         26


/*!< 0x0028 SWCFG_XIP_REMAP5_START
*null
*/
#define XIP_REMAP5_START_HADDR_Msk                          (0X7FFF << XIP_REMAP5_START_HADDR_Pos)
#define XIP_REMAP5_START_HADDR_Pos                          12
#define XIP_REMAP5_START_HADDR_PosL                         12
#define XIP_REMAP5_START_HADDR_PosH                         26


/*!< 0x002c SWCFG_XIP_REMAP6_START
*null
*/
#define XIP_REMAP6_START_HADDR_Msk                          (0X7FFF << XIP_REMAP6_START_HADDR_Pos)
#define XIP_REMAP6_START_HADDR_Pos                          12
#define XIP_REMAP6_START_HADDR_PosL                         12
#define XIP_REMAP6_START_HADDR_PosH                         26


/*!< 0x0030 SWCFG_XIP_REMAP7_START
*null
*/
#define XIP_REMAP7_START_HADDR_Msk                          (0X7FFF << XIP_REMAP7_START_HADDR_Pos)
#define XIP_REMAP7_START_HADDR_Pos                          12
#define XIP_REMAP7_START_HADDR_PosL                         12
#define XIP_REMAP7_START_HADDR_PosH                         26


/*!< 0x0034 SWCFG_XIP_REMAP8_START
*null
*/
#define XIP_REMAP8_START_HADDR_Msk                          (0X7FFF << XIP_REMAP8_START_HADDR_Pos)
#define XIP_REMAP8_START_HADDR_Pos                          12
#define XIP_REMAP8_START_HADDR_PosL                         12
#define XIP_REMAP8_START_HADDR_PosH                         26


/*!< 0x0038 SWCFG_XIP_REMAP9_START
*null
*/
#define XIP_REMAP9_START_HADDR_Msk                          (0X7FFF << XIP_REMAP9_START_HADDR_Pos)
#define XIP_REMAP9_START_HADDR_Pos                          12
#define XIP_REMAP9_START_HADDR_PosL                         12
#define XIP_REMAP9_START_HADDR_PosH                         26


/*!< 0x003c SWCFG_XIP_REMAP0_SIZE
*null
*/
#define XIP_REMAP0_SIZE_Msk                                 (0x7FF << XIP_REMAP0_SIZE_Pos)
#define XIP_REMAP0_SIZE_Pos                                 0
#define XIP_REMAP0_SIZE_PosL                                0
#define XIP_REMAP0_SIZE_PosH                                10


/*!< 0x0040 SWCFG_XIP_REMAP1_SIZE
*null
*/
#define XIP_REMAP1_SIZE_Msk                                 (0x7FF << XIP_REMAP1_SIZE_Pos)
#define XIP_REMAP1_SIZE_Pos                                 0
#define XIP_REMAP1_SIZE_PosL                                0
#define XIP_REMAP1_SIZE_PosH                                10


/*!< 0x0044 SWCFG_XIP_REMAP2_SIZE
*null
*/
#define XIP_REMAP2_SIZE_Msk                                 (0x7FF << XIP_REMAP2_SIZE_Pos)
#define XIP_REMAP2_SIZE_Pos                                 0
#define XIP_REMAP2_SIZE_PosL                                0
#define XIP_REMAP2_SIZE_PosH                                10


/*!< 0x0048 SWCFG_XIP_REMAP3_SIZE
*null
*/
#define XIP_REMAP3_SIZE_Msk                                 (0x7FF << XIP_REMAP3_SIZE_Pos)
#define XIP_REMAP3_SIZE_Pos                                 0
#define XIP_REMAP3_SIZE_PosL                                0
#define XIP_REMAP3_SIZE_PosH                                10


/*!< 0x004c SWCFG_XIP_REMAP4_SIZE
*null
*/
#define XIP_REMAP4_SIZE_Msk                                 (0x7FF << XIP_REMAP4_SIZE_Pos)
#define XIP_REMAP4_SIZE_Pos                                 0
#define XIP_REMAP4_SIZE_PosL                                0
#define XIP_REMAP4_SIZE_PosH                                10


/*!< 0x0050 SWCFG_XIP_REMAP5_SIZE
*null
*/
#define XIP_REMAP5_SIZE_Msk                                 (0x7FF << XIP_REMAP5_SIZE_Pos)
#define XIP_REMAP5_SIZE_Pos                                 0
#define XIP_REMAP5_SIZE_PosL                                0
#define XIP_REMAP5_SIZE_PosH                                10


/*!< 0x0054 SWCFG_XIP_REMAP6_SIZE
*null
*/
#define XIP_REMAP6_SIZE_Msk                                 (0x7FF << XIP_REMAP6_SIZE_Pos)
#define XIP_REMAP6_SIZE_Pos                                 0
#define XIP_REMAP6_SIZE_PosL                                0
#define XIP_REMAP6_SIZE_PosH                                10


/*!< 0x0058 SWCFG_XIP_REMAP7_SIZE
*null
*/
#define XIP_REMAP7_SIZE_Msk                                 (0x7FF << XIP_REMAP7_SIZE_Pos)
#define XIP_REMAP7_SIZE_Pos                                 0
#define XIP_REMAP7_SIZE_PosL                                0
#define XIP_REMAP7_SIZE_PosH                                10


/*!< 0x005c SWCFG_XIP_REMAP8_SIZE
*null
*/
#define XIP_REMAP8_SIZE_Msk                                 (0x7FF << XIP_REMAP8_SIZE_Pos)
#define XIP_REMAP8_SIZE_Pos                                 0
#define XIP_REMAP8_SIZE_PosL                                0
#define XIP_REMAP8_SIZE_PosH                                10


/*!< 0x0060 SWCFG_XIP_REMAP9_SIZE
*null
*/
#define XIP_REMAP9_SIZE_Msk                                 (0x7FF << XIP_REMAP9_SIZE_Pos)
#define XIP_REMAP9_SIZE_Pos                                 0
#define XIP_REMAP9_SIZE_PosL                                0
#define XIP_REMAP9_SIZE_PosH                                10


/*!< 0x0064 SWCFG_XIP_REMAP0
*null
*/
#define XIP_REMAP0_HADDR_Msk                                (0xFF << XIP_REMAP0_HADDR_Pos)
#define XIP_REMAP0_HADDR_Pos                                12
#define XIP_REMAP0_HADDR_PosL                               12
#define XIP_REMAP0_HADDR_PosH                               23


/*!< 0x0068 SWCFG_XIP_REMAP1
*null
*/
#define XIP_REMAP1_HADDR_Msk                                (0xFF << XIP_REMAP1_HADDR_Pos)
#define XIP_REMAP1_HADDR_Pos                                12
#define XIP_REMAP1_HADDR_PosL                               12
#define XIP_REMAP1_HADDR_PosH                               23


/*!< 0x006c SWCFG_XIP_REMAP2
*null
*/
#define XIP_REMAP2_HADDR_Msk                                (0xFF << XIP_REMAP2_HADDR_Pos)
#define XIP_REMAP2_HADDR_Pos                                12
#define XIP_REMAP2_HADDR_PosL                               12
#define XIP_REMAP2_HADDR_PosH                               23


/*!< 0x0070 SWCFG_XIP_REMAP3
*null
*/
#define XIP_REMAP3_HADDR_Msk                                (0xFF << XIP_REMAP3_HADDR_Pos)
#define XIP_REMAP3_HADDR_Pos                                12
#define XIP_REMAP3_HADDR_PosL                               12
#define XIP_REMAP3_HADDR_PosH                               23


/*!< 0x0074 SWCFG_XIP_REMAP4
*null
*/
#define XIP_REMAP4_HADDR_Msk                                (0xFF << XIP_REMAP4_HADDR_Pos)
#define XIP_REMAP4_HADDR_Pos                                12
#define XIP_REMAP4_HADDR_PosL                               12
#define XIP_REMAP4_HADDR_PosH                               23


/*!< 0x0078 SWCFG_XIP_REMAP5
*null
*/
#define XIP_REMAP5_HADDR_Msk                                (0xFF << XIP_REMAP5_HADDR_Pos)
#define XIP_REMAP5_HADDR_Pos                                12
#define XIP_REMAP5_HADDR_PosL                               12
#define XIP_REMAP5_HADDR_PosH                               23


/*!< 0x007c SWCFG_XIP_REMAP6
*null
*/
#define XIP_REMAP6_HADDR_Msk                                (0xFF << XIP_REMAP6_HADDR_Pos)
#define XIP_REMAP6_HADDR_Pos                                12
#define XIP_REMAP6_HADDR_PosL                               12
#define XIP_REMAP6_HADDR_PosH                               23


/*!< 0x0080 SWCFG_XIP_REMAP7
*null
*/
#define XIP_REMAP7_HADDR_Msk                                (0xFF << XIP_REMAP7_HADDR_Pos)
#define XIP_REMAP7_HADDR_Pos                                12
#define XIP_REMAP7_HADDR_PosL                               12
#define XIP_REMAP7_HADDR_PosH                               23


/*!< 0x0084 SWCFG_XIP_REMAP8
*null
*/
#define XIP_REMAP8_HADDR_Msk                                (0xFF << XIP_REMAP8_HADDR_Pos)
#define XIP_REMAP8_HADDR_Pos                                12
#define XIP_REMAP8_HADDR_PosL                               12
#define XIP_REMAP8_HADDR_PosH                               23


/*!< 0x0088 SWCFG_XIP_REMAP9
*null
*/
#define XIP_REMAP9_HADDR_Msk                                (0xFF << XIP_REMAP9_HADDR_Pos)
#define XIP_REMAP9_HADDR_Pos                                12
#define XIP_REMAP9_HADDR_PosL                               12
#define XIP_REMAP9_HADDR_PosH                               23


/**
* @}
* End of group HW_SWCFG
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_SWCFG_H_ */