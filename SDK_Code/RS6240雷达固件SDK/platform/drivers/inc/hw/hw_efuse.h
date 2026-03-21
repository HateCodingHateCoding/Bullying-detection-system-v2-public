/****************************************************************
* @file  efuse_ctrl.h
* @brief efuse_ctrl hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_EFUSE_CTRL_H_
#define _HW_EFUSE_CTRL_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup efuse_ctrl
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __IOM uint32_t EFUSE_PGMEN;                             /*0x0000              */
    __IOM uint32_t EFUSE_RD_CTRL_REG;                       /*0x0004              */
    __IOM uint32_t EFUSE_ADDR;                              /*0x0008,             */
    __IOM uint32_t EFUSE_WDATA;                             /*0x000c,             */
    __IOM uint32_t EFUSE_SETTINGS1;                         /*0x0010              */
    __IOM uint32_t EFUSE_SETTINGS2;                         /*0x0014,             */
    __IM  uint32_t EFUSE_RDATA;                             /*0x0018,             */
    __IM  uint32_t RSVD[2];                                 /*0x001c-0x0020,      */
          uint32_t KEY_ECC_STATUS;                          /*0x0024,             */
          uint32_t RSVD1[2];                                /*0x0028-0x002C,      */
    __IM  uint32_t EFUSE_BURNED_STATUS;                     /*0x0030,             */
    __IM  uint32_t KEY_BURN_STATUS;                         /*0x0034,             */
          uint32_t RSVD2[2];                                /*0x0038-0x003C,      */
    __IO  uint32_t KEY_WDATA[16];                           /*0x0040~0x007c,      */
          uint32_t RSVD4[32];                               /*0x0080~0x00FC,      */
    __IM  uint32_t EFUSE_REGBANK_DATA[128];                 /*0x0100h,            */
} efuse_dev_t;

/*!< 0x0000 EFUSE_PGMEN
*/
#define EFUSE_PGMEN_Msk                                     (0x01 << EFUSE_PGMEN_Pos)
#define EFUSE_PGMEN_Pos                                     0
#define EFUSE_PGMEN_PosL                                    0
#define EFUSE_PGMEN_PosH                                    0


/*!< 0x0004 EFUSE_RD_CTRL_REG
*/
#define EFUSE_RD_MODE_SEL_Msk                               (0x01 << EFUSE_RD_MODE_SEL_Pos)
#define EFUSE_RD_MODE_SEL_Pos                               1
#define EFUSE_RD_MODE_SEL_PosL                              1
#define EFUSE_RD_MODE_SEL_PosH                              1

/*!< 0x0004 CTRL_REG
*/
#define EFUSE_RD_EN_Msk                                     (0x01 << EFUSE_RD_EN_Pos)
#define EFUSE_RD_EN_Pos                                     0
#define EFUSE_RD_EN_PosL                                    0
#define EFUSE_RD_EN_PosH                                    0


/*!< 0x0008 EFUSE_ADDR
*/
#define EFUSE_KEY_PGM_EN_Msk                                (0x01 << EFUSE_KEY_PGM_EN_Pos)
#define EFUSE_KEY_PGM_EN_Pos                                31
#define EFUSE_KEY_PGM_EN_PosL                               31
#define EFUSE_KEY_PGM_EN_PosH                               31

/*!< 0x0008 EFUSE_ADDR
*/
#define EFUSE_ADDR_Msk                                      (0xFFF << EFUSE_ADDR_Pos)
#define EFUSE_ADDR_Pos                                      0
#define EFUSE_ADDR_PosL                                     0
#define EFUSE_ADDR_PosH                                     13


/*!< 0x000c EFUSE_WDATA
*/
#define EFUSE_DATA_Msk                                      (0XFFFFFFFF << EFUSE_DATA_Pos)
#define EFUSE_DATA_Pos                                      0
#define EFUSE_DATA_PosL                                     0
#define EFUSE_DATA_PosH                                     31


/*!< 0x0010 EFUSE_SETTINGS1
*/
#define TAEN_TPGM_Msk                                       (0x7F << TAEN_TPGM_Pos)
#define TAEN_TPGM_Pos                                       24
#define TAEN_TPGM_PosL                                      24
#define TAEN_TPGM_PosH                                      30

/*!< 0x0010 EFUSE_SETTINGS1
*/
#define THP_A_Msk                                           (0x07 << THP_A_Pos)
#define THP_A_Pos                                           20
#define THP_A_PosL                                          20
#define THP_A_PosH                                          22

/*!< 0x0010 EFUSE_SETTINGS1
*/
#define TSPRD_THPRD_Msk                                     (0x0F << TSPRD_THPRD_Pos)
#define TSPRD_THPRD_Pos                                     16
#define TSPRD_THPRD_PosL                                    16
#define TSPRD_THPRD_PosH                                    19

/*!< 0x0010 EFUSE_SETTINGS1
*/
#define TSRRD_THRRD_Msk                                     (0x07 << TSRRD_THRRD_Pos)
#define TSRRD_THRRD_Pos                                     12
#define TSRRD_THRRD_PosL                                    12
#define TSRRD_THRRD_PosH                                    14

/*!< 0x0010 EFUSE_SETTINGS1
*/
#define TRD_Msk                                             (0x07 << TRD_Pos)
#define TRD_Pos                                             8
#define TRD_PosL                                            8
#define TRD_PosH                                            10

/*!< 0x0010 EFUSE_SETTINGS1
*/
#define TAEN_TRD_Msk                                        (0x07 << TAEN_TRD_Pos)
#define TAEN_TRD_Pos                                        4
#define TAEN_TRD_PosL                                       4
#define TAEN_TRD_PosH                                       6

/*!< 0x0010 EFUSE_SETTINGS1
*/
#define THR_A_Msk                                           (0x03 << THR_A_Pos)
#define THR_A_Pos                                           0
#define THR_A_PosL                                          0
#define THR_A_PosH                                          1


/*!< 0x0014 EFUSE_SETTINGS2
*/
#define T_PGM_Msk                                           (0X1FF << T_PGM_Pos)
#define T_PGM_Pos                                           16
#define T_PGM_PosL                                          16
#define T_PGM_PosH                                          24

/*!< 0x0014 EFUSE_SETTINGS2
*/
#define TSP_PGM_Msk                                         (0x0F << TSP_PGM_Pos)
#define TSP_PGM_Pos                                         8
#define TSP_PGM_PosL                                        8
#define TSP_PGM_PosH                                        11

/*!< 0x0014 EFUSE_SETTINGS2
*/
#define TSP_THP_PG_AVDD_Msk                                 (0x7F << TSP_THP_PG_AVDD_Pos)
#define TSP_THP_PG_AVDD_Pos                                 0
#define TSP_THP_PG_AVDD_PosL                                0
#define TSP_THP_PG_AVDD_PosH                                6


/*!< 0x0018 EFUSE_RDATA
*/
#define EFUSE_RDATA_Msk                                     (0XFFFFFFFF << EFUSE_RDATA_Pos)
#define EFUSE_RDATA_Pos                                     0
#define EFUSE_RDATA_PosL                                    0
#define EFUSE_RDATA_PosH                                    31


/*!< 0x0020 PART1_ECC_STATUS
*/
#define PART4_ECC_ERR_FLAG_Msk                              (0x01 << PART4_ECC_ERR_FLAG_Pos)
#define PART4_ECC_ERR_FLAG_Pos                              0
#define PART4_ECC_ERR_FLAG_PosL                             0
#define PART4_ECC_ERR_FLAG_PosH                             0


/*!< 0x0024 KEY_ECC_STATUS
*/
#define KEY_ECC_ERR_LOC_Msk                                 (0xFFFF << KEY_ECC_ERR_LOC_Pos)
#define KEY_ECC_ERR_LOC_Pos                                 16
#define KEY_ECC_ERR_LOC_PosL                                16
#define KEY_ECC_ERR_LOC_PosH                                16

/*!< 0x0024 KEY_ECC_STATUS
*/
#define KEY_ECC_ERR_FLAG_Msk                                 (0x01 << KEY_ECC_ERR_FLAG_Pos)
#define KEY_ECC_ERR_FLAG_Pos                                 0
#define KEY_ECC_ERR_FLAG_PosL                                0
#define KEY_ECC_ERR_FLAG_PosH                                0

/*!< 0x0030 EFUSE_BURNED_STATUS
*/
#define EFUSE_BURNED_Msk                                    (0x01 << EFUSE_BURNED_Pos)
#define EFUSE_BURNED_Pos                                    0
#define EFUSE_BURNED_PosL                                   0
#define EFUSE_BURNED_PosH                                   0


/*!< 0x0034 KEY_BURN_STATUS
*/
#define KEY_BURN_ERR_LOC1_Msk                               (0X3FFF << KEY_BURN_ERR_LOC1_Pos)
#define KEY_BURN_ERR_LOC1_Pos                               18
#define KEY_BURN_ERR_LOC1_PosL                              18
#define KEY_BURN_ERR_LOC1_PosH                              31

/*!< 0x0034 KEY_BURN_STATUS
*/
#define KEY_BURN_ERR_LOC0_Msk                               (0X3FFF << KEY_BURN_ERR_LOC0_Pos)
#define KEY_BURN_ERR_LOC0_Pos                               4
#define KEY_BURN_ERR_LOC0_PosL                              4
#define KEY_BURN_ERR_LOC0_PosH                              17

/*!< 0x0034 KEY_BURN_STATUS
*/
#define KEY_BURN_ERR_NUM_Msk                                (0x07 << KEY_BURN_ERR_NUM_Pos)
#define KEY_BURN_ERR_NUM_Pos                                1
#define KEY_BURN_ERR_NUM_PosL                               1
#define KEY_BURN_ERR_NUM_PosH                               3

/*!< 0x0034 KEY_BURN_STATUS
*/
#define KEY_BURN_STATUS_Msk                                 (0x01 << KEY_BURN_STATUS_Pos)
#define KEY_BURN_STATUS_Pos                                 0
#define KEY_BURN_STATUS_PosL                                0
#define KEY_BURN_STATUS_PosH                                0


/*!< 0x0040~0x007c KEY_WDATA
*/
#define KEY_VALUE_Msk                                       (0xFFFFFFFF << KEY_VALUE_Pos)
#define KEY_VALUE_Pos                                       0
#define KEY_VALUE_PosL                                      0
#define KEY_VALUE_PosH                                      31


/*!< 0x0100h EFUSE_REGBANK_DATA
*/
#define EFUSE_Msk                                           (0XFFFFFFFF << EFUSE_Pos)
#define EFUSE_Pos                                           0
#define EFUSE_PosL                                          0
#define EFUSE_PosH                                          31

/**
* @}
* End of group HW_EFUSE_CTRL
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_EFUSE_CTRL_H_ */
