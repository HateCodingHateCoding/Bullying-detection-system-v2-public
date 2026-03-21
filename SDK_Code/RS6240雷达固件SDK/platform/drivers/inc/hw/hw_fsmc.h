/****************************************************************
* @file  flashctrl.h
* @brief flashctrl hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_FLASHCTRL_H_
#define _HW_FLASHCTRL_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup flashctrl
 * @{
 * */

/***
 * @brief General Purpose I/O
 */



typedef struct {
    __IOM uint32_t XIP_ENC_START_ADDR;
    __IOM uint32_t XIP_ENC_END_ADDR;
    __IOM uint32_t XIP_ENC_ENABLE;
} fsmc_xip_enc_t;
typedef struct {
    __IM  uint32_t VERSION;                                 /*0x0000,null                                               */
    __IOM uint32_t CTRL;                                    /*0x0004,null                                               */
    __IOM uint32_t CONFIG;                                  /*0x0008,null                                               */
    __IOM uint32_t CLOCK_CTRL;                              /*0x000c,null                                               */
    __IOM uint32_t RECEIVE_DATA_NUM;                        /*0x0010,null                                               */
          uint32_t RSVD[1];                                 /*0x0014,null                                               */
    __IM  uint32_t FIFO_STATUS;                             /*0x0018,null                                               */
    __IOM uint32_t TXF_CTL;                                 /*0x001c,null                                               */
    __IOM uint32_t RXF_CTL;                                 /*0x0020,null                                               */
    __IOM uint32_t INT_CS;                                  /*0x0024,null                                               */
    __IM  uint32_t SSR;                                     /*0x0028,null                                               */
    __IM  uint32_t INT_CLEAR;                               /*0x002c,null                                               */
    __IOM uint32_t DMA_EN;                                  /*0x0030,null                                               */
    __IOM uint32_t DMATR;                                   /*0x0034,null                                               */
    __IOM uint32_t DMARR;                                   /*0x0038,null                                               */
    __IM  uint32_t PC;                                      /*0x003C,null                                               */
    __IOM uint32_t DB;                                      /*0x0040,null                                               */
          uint32_t RSVD1[39];                               /*0x0044,null                                               */
    __IOM uint32_t SAMPLE_CTL;                              /*0x00E0,null                                               */
    __IOM uint32_t QSPI_ENHANCE;                            /*0x00E4,null                                               */
          uint32_t RSVD2[2];                                /*0x00E8,null                                               */
    __IOM uint32_t XIP_MODE_BITS;                           /*0x00F0,null                                               */
    __IOM uint32_t XIP_INCR_INST;                           /*0x00F4,null                                               */
    __IOM uint32_t XIP_WRAP_INST;                           /*0x00F8,null                                               */
    __IOM uint32_t XIP_CONFIG;                              /*0x00FC,null                                               */
    __IOM uint32_t XIP_SER;                                 /*0x0100,null                                               */
    __IM  uint32_t XRFOIC;                                  /*0x0104,null                                               */
    __IOM uint32_t XIP_TIME_OUT;                            /*0x0108,null                                               */
          uint32_t RSVD3[3];                                /*0x010c,null                                               */
    __IOM uint32_t XIP_ENC_KEY[4];                          /*0x0118,null                                               */
    __IOM fsmc_xip_enc_t XIP_ENC_INFO[3];                   /*0x0128,null                                               */
    __IOM uint32_t XIP_ENC_CTRL;                            /*0x0130,null                                               */
          uint32_t RSVD4[44];                               /*0x010c,null                                               */
    __IOM uint32_t PSRAM_CS_SEL;                            /*0x0200,null                                               */
    __IOM uint32_t PSRAM_INST;                              /*0x0204,null                                               */
} fsmc_dev_t;

/*!< 0x0000 VERSION
*null
*/
#define VERSION_Msk                                         (0XFFFFFFF << VERSION_PosL)
#define VERSION_Pos                                         31
#define VERSION_PosL                                        0
#define VERSION_PosH                                        31


/*!< 0x0004 CTRL
*null
*/
#define SSI_TYPE_Msk                                        (0x01 << SSI_TYPE_PosL)
#define SSI_TYPE_Pos                                        31
#define SSI_TYPE_PosL                                       31
#define SSI_TYPE_PosH                                       31

/*!< 0x0004 CTRL
*null
*/
#define FULL_DUPLEX_EN_Msk                                  (0x01 << FULL_DUPLEX_EN_PosL)
#define FULL_DUPLEX_EN_Pos                                  30
#define FULL_DUPLEX_EN_PosL                                 30
#define FULL_DUPLEX_EN_PosH                                 30

/*!< 0x0004 CTRL
*null
*/
#define CSE_Msk                                             (0x03 << CSE_PosL)
#define CSE_Pos                                             20
#define CSE_PosL                                            20
#define CSE_PosH                                            21

/*!< 0x0004 CTRL
*null
*/
#define SCSTE_Msk                                           (0x01 << SCSTE_PosL)
#define SCSTE_Pos                                           16
#define SCSTE_PosL                                          16
#define SCSTE_PosH                                          16

/*!< 0x0004 CTRL
*null
*/
#define TTYP_Msk                                            (0x03 << TTYP_PosL)
#define TTYP_Pos                                            11
#define TTYP_PosL                                           10
#define TTYP_PosH                                           11

/*!< 0x0004 CTRL
*null
*/
#define SRLT_Msk                                            (0x01 << SRLT_PosL)
#define SRLT_Pos                                            1
#define SRLT_PosL                                           1
#define SRLT_PosH                                           1

/*!< 0x0004 CTRL
*null
*/
#define SSI_START_Msk                                       (0x01 << SSI_START_PosL)
#define SSI_START_Pos                                       0
#define SSI_START_PosL                                      0
#define SSI_START_PosH                                      0


/*!< 0x0008 CONFIG
*null
*/
#define SCPOL_Msk                                           (0x01 << SCPOL_PosL)
#define SCPOL_Pos                                           31
#define SCPOL_PosL                                          31
#define SCPOL_PosH                                          31

/*!< 0x0008 CONFIG
*null
*/
#define SCPH_Msk                                            (0x01 << SCPH_PosL)
#define SCPH_Pos                                            30
#define SCPH_PosL                                           30
#define SCPH_PosH                                           30

/*!< 0x0008 CONFIG
*null
*/
#define DFW_Msk                                             (0x1F << DFW_PosL)
#define DFW_Pos                                             20
#define DFW_PosL                                            16
#define DFW_PosH                                            20

/*!< 0x0008 CONFIG
*null
*/
#define BIGENDIAN_Msk                                       (0x01 << BIGENDIAN_PosL)
#define BIGENDIAN_Pos                                       15
#define BIGENDIAN_PosL                                      15
#define BIGENDIAN_PosH                                      15

/*!< 0x0008 CONFIG
*null
*/
#define CFL_Msk                                             (0x0F << CFL_PosL)
#define CFL_Pos                                             7
#define CFL_PosL                                            4
#define CFL_PosH                                            7

/*!< 0x0008 CONFIG
*null
*/
#define COM_FRF_Msk                                         (0x03 << COM_FRF_PosL)
#define COM_FRF_Pos                                         3
#define COM_FRF_PosL                                        2
#define COM_FRF_PosH                                        3

/*!< 0x0008 CONFIG
*null
*/
#define SPIMODE_Msk                                         (0x03 << SPIMODE_PosL)
#define SPIMODE_Pos                                         1
#define SPIMODE_PosL                                        0
#define SPIMODE_PosH                                        1


/*!< 0x000c CLOCK_CTRL
*null
*/
#define CDR_Msk                                             (0X7FFE << CDR_PosL)
#define CDR_Pos                                             15
#define CDR_PosL                                            0
#define CDR_PosH                                            15


/*!< 0x0010 RECEIVE_DATA_NUM
*null
*/
#define REC_DATA_NUM_Msk                                    (0xFFFF << REC_DATA_NUM_PosL)
#define REC_DATA_NUM_Pos                                    15
#define REC_DATA_NUM_PosL                                   0
#define REC_DATA_NUM_PosH                                   15


/*!< 0x0018 FIFO_STATUS
*null
*/
#define RXFS_Msk                                            (0x3F << RXFS_PosL)
#define RXFS_Pos                                            21
#define RXFS_PosL                                           16
#define RXFS_PosH                                           21

/*!< 0x0018 FIFO_STATUS
*null
*/
#define TXFS_Msk                                            (0x3F << TXFS_PosL)
#define TXFS_Pos                                            5
#define TXFS_PosL                                           0
#define TXFS_PosH                                           5


/*!< 0x001c TXF_CTL
*null
*/
#define TXFSL_Msk                                           (0x1F << TXFSL_PosL)
#define TXFSL_Pos                                           20
#define TXFSL_PosL                                          16
#define TXFSL_PosH                                          20

/*!< 0x001c TXF_CTL
*null
*/
#define TXFITL_Msk                                          (0x1F << TXFITL_PosL)
#define TXFITL_Pos                                          4
#define TXFITL_PosL                                         0
#define TXFITL_PosH                                         4


/*!< 0x0020 RXF_CTL
*null
*/
#define RXFITL_Msk                                          (0x1F << RXFITL_PosL)
#define RXFITL_Pos                                          4
#define RXFITL_PosL                                         0
#define RXFITL_PosH                                         4


/*!< 0x0024 INT_CS
*null
*/
#define XRXOIM_Msk                                          (0x01 << XRXOIM_PosL)
#define XRXOIM_Pos                                          22
#define XRXOIM_PosL                                         22
#define XRXOIM_PosH                                         22

/*!< 0x0024 INT_CS
*null
*/
#define MSTIM_Msk                                           (0x01 << MSTIM_PosL)
#define MSTIM_Pos                                           21
#define MSTIM_PosL                                          21
#define MSTIM_PosH                                          21

/*!< 0x0024 INT_CS
*null
*/
#define RXFFIM_Msk                                          (0x01 << RXFFIM_PosL)
#define RXFFIM_Pos                                          20
#define RXFFIM_PosL                                         20
#define RXFFIM_PosH                                         20

/*!< 0x0024 INT_CS
*null
*/
#define RXFOIM_Msk                                          (0x01 << RXFOIM_PosL)
#define RXFOIM_Pos                                          19
#define RXFOIM_PosL                                         19
#define RXFOIM_PosH                                         19

/*!< 0x0024 INT_CS
*null
*/
#define RXFUIM_Msk                                          (0x01 << RXFUIM_PosL)
#define RXFUIM_Pos                                          18
#define RXFUIM_PosL                                         18
#define RXFUIM_PosH                                         18

/*!< 0x0024 INT_CS
*null
*/
#define TXFOIM_Msk                                          (0x01 << TXFOIM_PosL)
#define TXFOIM_Pos                                          17
#define TXFOIM_PosL                                         17
#define TXFOIM_PosH                                         17

/*!< 0x0024 INT_CS
*null
*/
#define TXFEIM_Msk                                          (0x01 << TXFEIM_PosL)
#define TXFEIM_Pos                                          16
#define TXFEIM_PosL                                         16
#define TXFEIM_PosH                                         16

/*!< 0x0024 INT_CS
*null
*/
#define XRXOIS_Msk                                          (0x01 << XRXOIS_PosL)
#define XRXOIS_Pos                                          6
#define XRXOIS_PosL                                         6
#define XRXOIS_PosH                                         6

/*!< 0x0024 INT_CS
*null
*/
#define MSTIS_Msk                                           (0x01 << MSTIS_PosL)
#define MSTIS_Pos                                           5
#define MSTIS_PosL                                          5
#define MSTIS_PosH                                          5

/*!< 0x0024 INT_CS
*null
*/
#define RXFFIS_Msk                                          (0x01 << RXFFIS_PosL)
#define RXFFIS_Pos                                          4
#define RXFFIS_PosL                                         4
#define RXFFIS_PosH                                         4

/*!< 0x0024 INT_CS
*null
*/
#define RXFOIS_Msk                                          (0x01 << RXFOIS_PosL)
#define RXFOIS_Pos                                          3
#define RXFOIS_PosL                                         3
#define RXFOIS_PosH                                         3

/*!< 0x0024 INT_CS
*null
*/
#define RXFUIS_Msk                                          (0x01 << RXFUIS_PosL)
#define RXFUIS_Pos                                          2
#define RXFUIS_PosL                                         2
#define RXFUIS_PosH                                         2

/*!< 0x0024 INT_CS
*null
*/
#define TXFOIS_Msk                                          (0x01 << TXFOIS_PosL)
#define TXFOIS_Pos                                          1
#define TXFOIS_PosL                                         1
#define TXFOIS_PosH                                         1

/*!< 0x0024 INT_CS
*null
*/
#define TXFEIS_Msk                                          (0x01 << TXFEIS_PosL)
#define TXFEIS_Pos                                          0
#define TXFEIS_PosL                                         0
#define TXFEIS_PosH                                         0


/*!< 0x0028 SSR
*null
*/
#define DCE_Msk                                             (0x01 << DCE_PosL)
#define DCE_Pos                                             6
#define DCE_PosL                                            6
#define DCE_PosH                                            6

/*!< 0x0028 SSR
*null
*/
#define TXE_Msk                                             (0x01 << TXE_PosL)
#define TXE_Pos                                             5
#define TXE_PosL                                            5
#define TXE_PosH                                            5

/*!< 0x0028 SSR
*null
*/
#define RFF_Msk                                             (0x01 << RFF_PosL)
#define RFF_Pos                                             4
#define RFF_PosL                                            4
#define RFF_PosH                                            4

/*!< 0x0028 SSR
*null
*/
#define RFNE_Msk                                            (0x01 << RFNE_PosL)
#define RFNE_Pos                                            3
#define RFNE_PosL                                           3
#define RFNE_PosH                                           3

/*!< 0x0028 SSR
*null
*/
#define TFE_Msk                                             (0x01 << TFE_PosL)
#define TFE_Pos                                             2
#define TFE_PosL                                            2
#define TFE_PosH                                            2

/*!< 0x0028 SSR
*null
*/
#define TFNF_Msk                                            (0x01 << TFNF_PosL)
#define TFNF_Pos                                            1
#define TFNF_PosL                                           1
#define TFNF_PosH                                           1

/*!< 0x0028 SSR
*null
*/
#define BUSY_Msk                                            (0x01 << BUSY_PosL)
#define BUSY_Pos                                            0
#define BUSY_PosL                                           0
#define BUSY_PosH                                           0


/*!< 0x002c INT_CLEAR
*null
*/
#define CI_Msk                                              (0x01 << CI_PosL)
#define CI_Pos                                              31
#define CI_PosL                                             31
#define CI_PosH                                             31

/*!< 0x002c INT_CLEAR
*null
*/
#define MSTICR_Msk      																		(0x01 << MSTICR_PosL)
#define MSTICR_Pos      																		24
#define MSTICR_PosL																					24
#define MSTICR_PosH     																		24

/*!< 0x002c INT_CLEAR
*null
*/
#define RXUICR_Msk        																	(0x01 << RXUICR_PosL)
#define RXUICR_Pos        																	16
#define RXUICR_PosL       																	16
#define RXUICR_PosH       																	16

/*!< 0x002c INT_CLEAR
*null
*/
#define RXOICR_Msk                                          (0x01 << RXOICR_PosL)
#define RXOICR_Pos                                          8
#define RXOICR_PosL                                         8
#define RXOICR_PosH                                         8

/*!< 0x002c INT_CLEAR
*null
*/
#define TXOICR_Msk                                          (0x01 << TXOICR_PosL)
#define TXOICR_Pos                                          0
#define TXOICR_PosL                                         0
#define TXOICR_PosH                                         0


/*!< 0x0030 DMA_EN
*null
*/
#define DMATE_Msk                                           (0x01 << DMATE_PosL)
#define DMATE_Pos                                           1
#define DMATE_PosL                                          1
#define DMATE_PosH                                          1

/*!< 0x0030 DMA_EN
*null
*/
#define DMARE_Msk                                           (0x01 << DMARE_PosL)
#define DMARE_Pos                                           0
#define DMARE_PosL                                          0
#define DMARE_PosH                                          0


/*!< 0x0034 DMATR
*null
*/
#define DMATRL_Msk                                          (0x1F << DMATRL_PosL)
#define DMATRL_Pos                                          4
#define DMATRL_PosL                                         0
#define DMATRL_PosH                                         4


/*!< 0x0038 DMARR
*null
*/
#define DMARRL_Msk                                          (0x1F << DMARRL_PosL)
#define DMARRL_Pos                                          4
#define DMARRL_PosL                                         0
#define DMARRL_PosH                                         4


/*!< 0x003C PC
*null
*/
#define PERIPHERALCODE_Msk                                  (0XFFFFFFF << PERIPHERALCODE_PosL)
#define PERIPHERALCODE_Pos                                  31
#define PERIPHERALCODE_PosL                                 0
#define PERIPHERALCODE_PosH                                 31


/*!< 0x0040 DB
*null
*/
#define DBR_Msk                                             (0XFFFFFFF << DBR_PosL)
#define DBR_Pos                                             31
#define DBR_PosL                                            0
#define DBR_PosH                                            31


/*!< 0x00E0 SAMPLE_CTL
*null
*/
#define RSE_Msk                                             (0x01 << RSE_PosL)
#define RSE_Pos                                             16
#define RSE_PosL                                            16
#define RSE_PosH                                            16

/*!< 0x00E0 SAMPLE_CTL
*null
*/
#define RSD_Msk                                             (0x7 << RSD_PosL)
#define RSD_Pos                                             2
#define RSD_PosL                                            0
#define RSD_PosH                                            2


/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define CLK_STRETCH_EN_Msk                                  (0x01 << CLK_STRETCH_EN_PosL)
#define CLK_STRETCH_EN_Pos                                  28
#define CLK_STRETCH_EN_PosL                                 28
#define CLK_STRETCH_EN_PosH                                 28

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define XIP_PREFETCH_EN_Msk                                 (0x01 << XIP_PREFETCH_EN_PosL)
#define XIP_PREFETCH_EN_Pos                                 27
#define XIP_PREFETCH_EN_PosL                                27
#define XIP_PREFETCH_EN_PosH                                27

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define XIP_CONT_XFER_EN_Msk                                (0x01 << XIP_CONT_XFER_EN_PosL)
#define XIP_CONT_XFER_EN_Pos                                24
#define XIP_CONT_XFER_EN_PosL                               24
#define XIP_CONT_XFER_EN_PosH                               24

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define XIP_DFS_HC_Msk                                      (0x01 << XIP_DFS_HC_PosL)
#define XIP_DFS_HC_Pos                                      23
#define XIP_DFS_HC_PosL                                     23
#define XIP_DFS_HC_PosH                                     23

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define XIP_MBL_Msk                                         (0x03 << XIP_MBL_PosL)
#define XIP_MBL_Pos                                         19
#define XIP_MBL_PosL                                        18
#define XIP_MBL_PosH                                        19

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define XIP_INST_EN_Msk                                     (0x01 << XIP_INST_EN_PosL)
#define XIP_INST_EN_Pos                                     17
#define XIP_INST_EN_PosL                                    17
#define XIP_INST_EN_PosH                                    17

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define XIP_MODE_BIT_EN_Msk                                 (0x01 << XIP_MODE_BIT_EN_PosL)
#define XIP_MODE_BIT_EN_Pos                                 16
#define XIP_MODE_BIT_EN_PosL                                16
#define XIP_MODE_BIT_EN_PosH                                16

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define TRANS_FORM_Msk                                      (0x03 << TRANS_FORM_PosL)
#define TRANS_FORM_Pos                                      15
#define TRANS_FORM_PosL                                     14
#define TRANS_FORM_PosH                                     15

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define DUMMY_Msk                                          (0x1F << DUMMY_PosL)
#define DUMMY_Pos                                          12
#define DUMMY_PosL                                         8
#define DUMMY_PosH                                         12

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define ADDR_B_Msk                                          (0x0F << ADDR_B_PosL)
#define ADDR_B_Pos                                          5
#define ADDR_B_PosL                                         2
#define ADDR_B_PosH                                         5

/*!< 0x00E4 QSPI_ENHANCE
*null
*/
#define INST_B_Msk                                          (0x03 << INST_B_PosL)
#define INST_B_Pos                                          1
#define INST_B_PosL                                         0
#define INST_B_PosH                                         1


/*!< 0x00F0 XIP_MODE_BITS
*null
*/
#define XIP_MODE_BITS_Msk                                   (0xFFFF << XIP_MODE_BITS_PosL)
#define XIP_MODE_BITS_Pos                                   15
#define XIP_MODE_BITS_PosL                                  0
#define XIP_MODE_BITS_PosH                                  15


/*!< 0x00F4 XIP_INCR_INST
*null
*/
#define INCR_INST_Msk                                       (0xFFFF << INCR_INST_PosL)
#define INCR_INST_Pos                                       15
#define INCR_INST_PosL                                      0
#define INCR_INST_PosH                                      15


/*!< 0x00F8 XIP_WRAP_INST
*null
*/
#define WRAP_INST_Msk                                       (0xFFFF << WRAP_INST_PosL)
#define WRAP_INST_Pos                                       15
#define WRAP_INST_PosL                                      0
#define WRAP_INST_PosH                                      15


/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_PREFETCH_EN_Msk                                 (0x01 << XIP_PREFETCH_EN_PosL)
#define XIP_PREFETCH_EN_Pos                                 27
#define XIP_PREFETCH_EN_PosL                                27
#define XIP_PREFETCH_EN_PosH                                27

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_CONT_XFER_EN_Msk                                (0x01 << XIP_CONT_XFER_EN_PosL)
#define XIP_CONT_XFER_EN_Pos                                24
#define XIP_CONT_XFER_EN_PosL                               24
#define XIP_CONT_XFER_EN_PosH                               24

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_DFS_HC_Msk                                      (0x01 << XIP_DFS_HC_PosL)
#define XIP_DFS_HC_Pos                                      23
#define XIP_DFS_HC_PosL                                     23
#define XIP_DFS_HC_PosH                                     23

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_MBL_Msk                                         (0x03 << XIP_MBL_PosL)
#define XIP_MBL_Pos                                         19
#define XIP_MBL_PosL                                        18
#define XIP_MBL_PosH                                        19

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_INST_EN_Msk                                     (0x01 << XIP_INST_EN_PosL)
#define XIP_INST_EN_Pos                                     17
#define XIP_INST_EN_PosL                                    17
#define XIP_INST_EN_PosH                                    17

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_MODE_BIT_EN_Msk                                 (0x01 << XIP_MODE_BIT_EN_PosL)
#define XIP_MODE_BIT_EN_Pos                                 16
#define XIP_MODE_BIT_EN_PosL                                16
#define XIP_MODE_BIT_EN_PosH                                16

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_TRANS_FORM_Msk                                  (0x03 << XIP_TRANS_FORM_PosL)
#define XIP_TRANS_FORM_Pos                                  15
#define XIP_TRANS_FORM_PosL                                 14
#define XIP_TRANS_FORM_PosH                                 15

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_DUMMY_Msk                                       (0x1F << XIP_DUMMY_PosL)
#define XIP_DUMMY_Pos                                       12
#define XIP_DUMMY_PosL                                      8
#define XIP_DUMMY_PosH                                      12

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_MODE_Msk                                        (0x03 << XIP_MODE_PosL)
#define XIP_MODE_Pos                                        7
#define XIP_MODE_PosL                                       6
#define XIP_MODE_PosH                                       7

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_ADDR_B_Msk                                      (0x0F << XIP_ADDR_B_PosL)
#define XIP_ADDR_B_Pos                                      5
#define XIP_ADDR_B_PosL                                     2
#define XIP_ADDR_B_PosH                                     5

/*!< 0x00FC XIP_CONFIG
*null
*/
#define XIP_INST_B_Msk                                      (0x03 << XIP_INST_B_PosL)
#define XIP_INST_B_Pos                                      1
#define XIP_INST_B_PosL                                     0
#define XIP_INST_B_PosH                                     1


/*!< 0x0100 XIP_SER
*null
*/
#define SLAVE_SELECT_ENABLE_FLAG_Msk                           (0x01 << SLAVE_SELECT_ENABLE_FLAG_PosL)
#define SLAVE_SELECT_ENABLE_FLAG_Pos                           0
#define SLAVE_SELECT_ENABLE_FLAG_PosL                          0
#define SLAVE_SELECT_ENABLE_FLAG_PosH                          0


/*!< 0x0104 XRFOIC
*null
*/
#define XRFOICR_Msk                                         (0x01 << XRFOICR_PosL)
#define XRFOICR_Pos                                         0
#define XRFOICR_PosL                                        0
#define XRFOICR_PosH                                        0


/*!< 0x0108 XIP_TIME_OUT
*null
*/
#define XIP_TIME_OUT_Msk                                    (0xFF << XIP_TIME_OUT_PosL)
#define XIP_TIME_OUT_Pos                                    7
#define XIP_TIME_OUT_PosL                                   0
#define XIP_TIME_OUT_PosH                                   7


/*!< 0x0118 XIP_ENC_KEY0
*null
*/
#define ENC_KEY_Msk                                         (0XFFFFFFFF << ENC_KEY_PosL)
#define ENC_KEY_Pos                                         31
#define ENC_KEY_PosL                                        0
#define ENC_KEY_PosH                                        31


/*!< 0x011c XIP_ENC_KEY1
*null
*/
#define ENC_KEY_Msk                                         (0XFFFFFFFF << ENC_KEY_PosL)
#define ENC_KEY_Pos                                         31
#define ENC_KEY_PosL                                        0
#define ENC_KEY_PosH                                        31


/*!< 0x0120 XIP_ENC_KEY2
*null
*/
#define ENC_KEY_Msk                                         (0XFFFFFFFF << ENC_KEY_PosL)
#define ENC_KEY_Pos                                         31
#define ENC_KEY_PosL                                        0
#define ENC_KEY_PosH                                        31


/*!< 0x0124 XIP_ENC_KEY3
*null
*/
#define ENC_KEY_Msk                                         (0XFFFFFFFF << ENC_KEY_PosL)
#define ENC_KEY_Pos                                         31
#define ENC_KEY_PosL                                        0
#define ENC_KEY_PosH                                        31


/*!< 0x0128 XIP_ENC_START_ADDR
*null
*/
#define ENC_START_ADDR_Msk                                  (0x3FFFFF00 << ENC_START_ADDR_PosL)
#define ENC_START_ADDR_Pos                                  31
#define ENC_START_ADDR_PosL                                 0
#define ENC_START_ADDR_PosH                                 31


/*!< 0x012c XIP_ENC_END_ADDR
*null
*/
#define ENC_END_ADDR_Msk                                    (0x3FFFFF00 << ENC_END_ADDR_PosL)
#define ENC_END_ADDR_Pos                                    31
#define ENC_END_ADDR_PosL                                   0
#define ENC_END_ADDR_PosH                                   31


/*!< 0x0130 XIP_ENC_CTRL
*null
*/
#define ENC_AES_SOFT_RST_Msk                                (0x01 << ENC_AES_SOFT_RST_PosL)
#define ENC_AES_SOFT_RST_Pos                                2
#define ENC_AES_SOFT_RST_PosL                               2
#define ENC_AES_SOFT_RST_PosH                               2

/*!< 0x0130 XIP_ENC_CTRL
*null
*/
#define ENC_KEY_SEL_Msk                                     (0x01 << ENC_KEY_SEL_PosL)
#define ENC_KEY_SEL_Pos                                     1
#define ENC_KEY_SEL_PosL                                    1
#define ENC_KEY_SEL_PosH                                    1

/*!< 0x0200 PSRAM_CS_SEL
*null
*/
#define PSRAM_CS_SEL_Msk                                    (0x01 << PSRAM_CS_SEL_PosL)
#define PSRAM_CS_SEL_Pos                                    31
#define PSRAM_CS_SEL_PosL                                   31
#define PSRAM_CS_SEL_PosH                                   31

/*!< 0x0204 PSRAM_INST
*null
*/
#define PSRAM_RX_INST_Msk                                    (0xFF << PSRAM_RX_INST_PosL)
#define PSRAM_RX_INST_Pos                                    0
#define PSRAM_RX_INST_PosL                                   0
#define PSRAM_RX_INST_PosH                                   7

#define PSRAM_TX_INST_Msk                                    (0xFF << PSRAM_TX_INST_PosL)
#define PSRAM_TX_INST_Pos                                    8
#define PSRAM_TX_INST_PosL                                   8
#define PSRAM_TX_INST_PosH                                   15

/**
* @}
* End of group HW_FLASHCTRL
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_FLASHCTRL_H_ */
