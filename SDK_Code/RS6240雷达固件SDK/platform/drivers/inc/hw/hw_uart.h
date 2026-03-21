/****************************************************************
* @file  uart.h
* @brief uart hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_UART_H_
#define _HW_UART_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup uart
 * @{
 * */

/***
 * @brief General Purpose I/O
 */



typedef struct {
    __OM  uint32_t SRCR;                                    /*0x0000,null                                               */
    __IOM uint32_t CR;                                      /*0x0004,null                                               */
    __IOM uint32_t DSR;                                     /*0x0008,null                                               */
    __IOM uint32_t IER;                                     /*0x000c,null                                               */
    __OM  uint32_t FCR;                                     /*0x0010,null                                               */
    __IOM uint32_t LPDSR;                                   /*0x0014,null                                               */
    __IOM uint32_t FTR;                                     /*0x0018,null                                               */
    __IM  uint32_t TFTR;                                    /*0x001c,null                                               */
    __OM  uint32_t RFTW;                                    /*0x0020,null                                               */
    __OM  uint32_t DMACR;                                   /*0x0024,null                                               */
    __IOM uint32_t RMCR;                                    /*0x0028,null                                               */
    __IOM uint32_t DCR;                                     /*0x002c,null                                               */
    __IOM uint32_t TAT;                                     /*0x0030,null                                               */
    __IOM uint32_t NRCA;                                    /*0x0034,null                                               */
    __IOM uint32_t NTAR;                                    /*0x0038,null                                               */
    __IM  uint32_t BDD;                                     /*0x003c,null                                               */
    __IM  uint32_t ISR;                                     /*0x0040,null                                               */
    __IM  uint32_t SR;                                      /*0x0044,null                                               */
    __IM  uint32_t FSR;                                     /*0x0048,null                                               */
    __IM  uint32_t RDR;                                     /*0x004c,null                                               */
    __OM  uint32_t TDR;                                     /*0x0050,null                                               */
    __IM  uint32_t UPR;                                     /*0x0054,null                                               */
    __IM  uint32_t UV;                                      /*0x0058,null                                               */
    __IM  uint32_t UTR;                                     /*0x005c,null                                               */
} uart_dev_t;

/*!< 0x0000 SRCR
*null
*/
#define XFR_Msk                                             (0x01 << XFR_Pos)
#define XFR_Pos                                             2
#define XFR_PosL                                            2
#define XFR_PosH                                            2

/*!< 0x0000 SRCR
*/
#define RFR_Msk                                             (0x01 << RFR_Pos)
#define RFR_Pos                                             1
#define RFR_PosL                                            1
#define RFR_PosH                                            1

/*!< 0x0000 SRCR
*null
*/
#define UR_Msk                                              (0x01 << UR_Pos)
#define UR_Pos                                              0
#define UR_PosL                                             0
#define UR_PosH                                             0


/*!< 0x0004 CR
*null
*/
#define BD_CLEAR_Msk                                        (0x01 << BD_CLEAR_Pos)
#define BD_CLEAR_Pos                                        25
#define BD_CLEAR_PosL                                       25
#define BD_CLEAR_PosH                                       25

/*!< 0x0004 CR
*null
*/
#define BD_ENABLE_Msk                                       (0x01 << BD_ENABLE_Pos)
#define BD_ENABLE_Pos                                       24
#define BD_ENABLE_PosL                                      24
#define BD_ENABLE_PosH                                      24

/*!< 0x0004 CR
*null
*/
#define TRANSMIT_MODE_Msk                                   (0x01 << TRANSMIT_MODE_Pos)
#define TRANSMIT_MODE_Pos                                   19
#define TRANSMIT_MODE_PosL                                  19
#define TRANSMIT_MODE_PosH                                  19

/*!< 0x0004 CR
*null
*/
#define SEND_ADDR_Msk                                       (0x01 << SEND_ADDR_Pos)
#define SEND_ADDR_Pos                                       18
#define SEND_ADDR_PosL                                      18
#define SEND_ADDR_PosH                                      18

/*!< 0x0004 CR
*null
*/
#define ADDR_MATCH_Msk                                      (0x01 << ADDR_MATCH_Pos)
#define ADDR_MATCH_Pos                                      17
#define ADDR_MATCH_PosL                                     17
#define ADDR_MATCH_PosH                                     17

/*!< 0x0004 CR
*null
*/
#define DLS_E_Msk                                           (0x01 << DLS_E_Pos)
#define DLS_E_Pos                                           16
#define DLS_E_PosL                                          16
#define DLS_E_PosH                                          16

/*!< 0x0004 CR
*null
*/
#define SIRE_Msk                                            (0x01 << SIRE_Pos)
#define SIRE_Pos                                            14
#define SIRE_PosL                                           14
#define SIRE_PosH                                           14

/*!< 0x0004 CR
*null
*/
#define AFCE_Msk                                            (0x01 << AFCE_Pos)
#define AFCE_Pos                                            13
#define AFCE_PosL                                           13
#define AFCE_PosH                                           13

/*!< 0x0004 CR
*null
*/
#define LOOPBACK_Msk                                        (0x01 << LOOPBACK_Pos)
#define LOOPBACK_Pos                                        12
#define LOOPBACK_PosL                                       12
#define LOOPBACK_PosH                                       12

/*!< 0x0004 CR
*null
*/
#define OUT2_Msk                                            (0x01 << OUT2_Pos)
#define OUT2_Pos                                            11
#define OUT2_PosL                                           11
#define OUT2_PosH                                           11

/*!< 0x0004 CR
*null
*/
#define OUT1_Msk                                            (0x01 << OUT1_Pos)
#define OUT1_Pos                                            10
#define OUT1_PosL                                           10
#define OUT1_PosH                                           10

/*!< 0x0004 CR
*null
*/
#define RTS_Msk                                             (0x01 << RTS_Pos)
#define RTS_Pos                                             9
#define RTS_PosL                                            9
#define RTS_PosH                                            9

/*!< 0x0004 CR
*null
*/
#define DTR_Msk                                             (0x01 << DTR_Pos)
#define DTR_Pos                                             8
#define DTR_PosL                                            8
#define DTR_PosH                                            8

/*!< 0x0004 CR
*null
*/
#define BC_Msk                                              (0x01 << BC_Pos)
#define BC_Pos                                              6
#define BC_PosL                                             6
#define BC_PosH                                             6

/*!< 0x0004 CR
*null
*/
#define SP_Msk                                              (0x01 << SP_Pos)
#define SP_Pos                                              5
#define SP_PosL                                             5
#define SP_PosH                                             5

/*!< 0x0004 CR
*null
*/
#define EPS_Msk                                             (0x01 << EPS_Pos)
#define EPS_Pos                                             4
#define EPS_PosL                                            4
#define EPS_PosH                                            4

/*!< 0x0004 CR
*null
*/
#define PEN_Msk                                             (0x01 << PEN_Pos)
#define PEN_Pos                                             3
#define PEN_PosL                                            3
#define PEN_PosH                                            3

/*!< 0x0004 CR
*null
*/
#define PARITY_Msk                                           (0x03 << PARITY_Pos)
#define PARITY_Pos                                           3
#define PARITY_PosL                                          3
#define PARITY_PosH                                          3


/*!< 0x0004 CR
*null
*/
#define STOP_BIT_Msk                                         (0x01 << STOP_BIT_Pos)
#define STOP_BIT_Pos                                          2
#define STOP_BIT_PosL                                         2
#define STOP_BIT_PosH                                         2

/*!< 0x0004 CR
*null
*/
#define DLS_Msk                                             (0x03 << DLS_Pos)
#define DLS_Pos                                             0
#define DLS_PosL                                            0
#define DLS_PosH                                            1


/*!< 0x0008 DSR
*null
*/
#define DLF_Msk                                             (0x3F << DLF_Pos)
#define DLF_Pos                                             16
#define DLF_PosL                                            16
#define DLF_PosH                                            21

/*!< 0x0008 DSR
*null
*/
#define DLHU_Msk                                            (0xFF << DLHU_Pos)
#define DLHU_Pos                                            8
#define DLHU_PosL                                           8
#define DLHU_PosH                                           15

/*!< 0x0008 DSR
*null
*/
#define DLL_Msk                                             (0xFF << DLL_Pos)
#define DLL_Pos                                             0
#define DLL_PosL                                            0
#define DLL_PosH                                            7


/*!< 0x000c IER
*null
*/
#define PTIME_Msk                                           (0x01 << PTIME_Pos)
#define PTIME_Pos                                           7
#define PTIME_PosL                                          7
#define PTIME_PosH                                          7

/*!< 0x000c IER
*null
*/
#define ELCOLR_Msk                                          (0x01 << ELCOLR_Pos)
#define ELCOLR_Pos                                          4
#define ELCOLR_PosL                                         4
#define ELCOLR_PosH                                         4

/*!< 0x000c IER
*null
*/
#define EDSSI_Msk                                           (0x01 << EDSSI_Pos)
#define EDSSI_Pos                                           3
#define EDSSI_PosL                                          3
#define EDSSI_PosH                                          3

/*!< 0x000c IER
*null
*/
#define ELSI_Msk                                            (0x01 << ELSI_Pos)
#define ELSI_Pos                                            2
#define ELSI_PosL                                           2
#define ELSI_PosH                                           2

/*!< 0x000c IER
*null
*/
#define ETBEI_Msk                                           (0x01 << ETBEI_Pos)
#define ETBEI_Pos                                           1
#define ETBEI_PosL                                          1
#define ETBEI_PosH                                          1

/*!< 0x000c IER
*null
*/
#define ERBFI_Msk                                           (0x01 << ERBFI_Pos)
#define ERBFI_Pos                                           0
#define ERBFI_PosL                                          0
#define ERBFI_PosH                                          0


/*!< 0x0010 FCR
*null
*/
#define RT_Msk                                              (0x03 << RT_Pos)
#define RT_Pos                                              6
#define RT_PosL                                             6
#define RT_PosH                                             7

/*!< 0x0010 FCR
*null
*/
#define TET_Msk                                             (0x03 << TET_Pos)
#define TET_Pos                                             4
#define TET_PosL                                            4
#define TET_PosH                                            5

/*!< 0x0010 FCR
*null
*/
#define XFIFOR_Msk                                          (0x01 << XFIFOR_Pos)
#define XFIFOR_Pos                                          2
#define XFIFOR_PosL                                         2
#define XFIFOR_PosH                                         2

/*!< 0x0010 FCR
*null
*/
#define RFIFOR_Msk                                          (0x01 << RFIFOR_Pos)
#define RFIFOR_Pos                                          1
#define RFIFOR_PosL                                         1
#define RFIFOR_PosH                                         1

/*!< 0x0010 FCR
*null
*/
#define FIFOE_Msk                                           (0x01 << FIFOE_Pos)
#define FIFOE_Pos                                           0
#define FIFOE_PosL                                          0
#define FIFOE_PosH                                          0


/*!< 0x0014 LPDSR
*null
*/
#define LPDLH_Msk                                           (0xFF << LPDLH_Pos)
#define LPDLH_Pos                                           8
#define LPDLH_PosL                                          8
#define LPDLH_PosH                                          15

/*!< 0x0014 LPDSR
*null
*/
#define LPDLL_Msk                                           (0xFF << LPDLL_Pos)
#define LPDLL_Pos                                           0
#define LPDLL_PosL                                          0
#define LPDLL_PosH                                          7


/*!< 0x0018 FTR
*null
*/
#define HTX_Msk                                             (0x01 << HTX_Pos)
#define HTX_Pos                                             1
#define HTX_PosL                                            1
#define HTX_PosH                                            1

/*!< 0x0018 FTR
*null
*/
#define FAR_Msk                                             (0x01 << FAR_Pos)
#define FAR_Pos                                             0
#define FAR_PosL                                            0
#define FAR_PosH                                            0


/*!< 0x001c TFTR
*null
*/
#define TFR_Msk                                             (0xFF << TFR_Pos)
#define TFR_Pos                                             0
#define TFR_PosL                                            0
#define TFR_PosH                                            7


/*!< 0x0020 RFTW
*null
*/
#define RFFE_Msk                                            (0x01 << RFFE_Pos)
#define RFFE_Pos                                            9
#define RFFE_PosL                                           9
#define RFFE_PosH                                           9

/*!< 0x0020 RFTW
*null
*/
#define RFPE_Msk                                            (0x01 << RFPE_Pos)
#define RFPE_Pos                                            8
#define RFPE_PosL                                           8
#define RFPE_PosH                                           8

/*!< 0x0020 RFTW
*null
*/
#define RFWD_Msk                                            (0xFF << RFWD_Pos)
#define RFWD_Pos                                            0
#define RFWD_PosL                                           0
#define RFWD_PosH                                           7


/*!< 0x0024 DMACR
*null
*/
#define DMASA_Msk                                           (0x01 << DMASA_Pos)
#define DMASA_Pos                                           0
#define DMASA_PosL                                          0
#define DMASA_PosH                                          0


/*!< 0x0028 RMCR
*null
*/
#define XFER_MODE_Msk                                       (0x03 << XFER_MODE_Pos)
#define XFER_MODE_Pos                                       3
#define XFER_MODE_PosL                                      3
#define XFER_MODE_PosH                                      4

/*!< 0x0028 RMCR
*null
*/
#define DE_POL_Msk                                          (0x01 << DE_POL_Pos)
#define DE_POL_Pos                                          2
#define DE_POL_PosL                                         2
#define DE_POL_PosH                                         2

/*!< 0x0028 RMCR
*null
*/
#define RE_POL_Msk                                          (0x01 << RE_POL_Pos)
#define RE_POL_Pos                                          1
#define RE_POL_PosL                                         1
#define RE_POL_PosH                                         1

/*!< 0x0028 RMCR
*null
*/
#define RS485_EN_Msk                                        (0x01 << RS485_EN_Pos)
#define RS485_EN_Pos                                        0
#define RS485_EN_PosL                                       0
#define RS485_EN_PosH                                       0


/*!< 0x002c DCR
*null
*/
#define DE_DEASSERTION_TIME_Msk                             (0xFF << DE_DEASSERTION_TIME_Pos)
#define DE_DEASSERTION_TIME_Pos                             24
#define DE_DEASSERTION_TIME_PosL                            24
#define DE_DEASSERTION_TIME_PosH                            31

/*!< 0x002c DCR
*null
*/
#define DE_ASSERTION_TIME_Msk                               (0xFF << DE_ASSERTION_TIME_Pos)
#define DE_ASSERTION_TIME_Pos                               16
#define DE_ASSERTION_TIME_PosL                              16
#define DE_ASSERTION_TIME_PosH                              23

/*!< 0x002c DCR
*null
*/
#define RE_ENABLE_Msk                                       (0x01 << RE_ENABLE_Pos)
#define RE_ENABLE_Pos                                       8
#define RE_ENABLE_PosL                                      8
#define RE_ENABLE_PosH                                      8

/*!< 0x002c DCR
*null
*/
#define DE_ENABLE_Msk                                       (0x01 << DE_ENABLE_Pos)
#define DE_ENABLE_Pos                                       0
#define DE_ENABLE_PosL                                      0
#define DE_ENABLE_PosH                                      0


/*!< 0x0030 TAT
*null
*/
#define RE2DE_Msk                                           (0xFFFF << RE2DE_Pos)
#define RE2DE_Pos                                           16
#define RE2DE_PosL                                          16
#define RE2DE_PosH                                          31

/*!< 0x0030 TAT
*null
*/
#define DE2RE_Msk                                           (0xFFFF << DE2RE_Pos)
#define DE2RE_Pos                                           0
#define DE2RE_PosL                                          0
#define DE2RE_PosH                                          15


/*!< 0x0034 NRCA
*null
*/
#define RAR_Msk                                             (0xFF << RAR_Pos)
#define RAR_Pos                                             0
#define RAR_PosL                                            0
#define RAR_PosH                                            7


/*!< 0x0038 NTAR
*null
*/
#define TAR_Msk                                             (0xFF << TAR_Pos)
#define TAR_Pos                                             0
#define TAR_PosL                                            0
#define TAR_PosH                                            7


/*!< 0x003c BDD
*null
*/
#define BAUD_COUNT_FINISH_Msk                               (0x01 << BAUD_COUNT_FINISH_Pos)
#define BAUD_COUNT_FINISH_Pos                               31
#define BAUD_COUNT_FINISH_PosL                              31
#define BAUD_COUNT_FINISH_PosH                              31

/*!< 0x003c BDD
*null
*/
#define BAUD_COUNT_FULL_Msk                                 (0x01 << BAUD_COUNT_FULL_Pos)
#define BAUD_COUNT_FULL_Pos                                 16
#define BAUD_COUNT_FULL_PosL                                16
#define BAUD_COUNT_FULL_PosH                                16

/*!< 0x003c BDD
*null
*/
#define BAUD_COUNT_Msk                                      (0xFFFF << BAUD_COUNT_Pos)
#define BAUD_COUNT_Pos                                      0
#define BAUD_COUNT_PosL                                     0
#define BAUD_COUNT_PosH                                     15


/*!< 0x0040 ISR
*null
*/
#define FIFOSE_Msk                                          (0x03 << FIFOSE_Pos)
#define FIFOSE_Pos                                          8
#define FIFOSE_PosL                                         8
#define FIFOSE_PosH                                         9

/*!< 0x0040 ISR
*null
*/
#define CHARACTER_TIMEOUT_Msk                               (0x01 << CHARACTER_TIMEOUT_Pos)
#define CHARACTER_TIMEOUT_Pos                               6
#define CHARACTER_TIMEOUT_PosL                              6
#define CHARACTER_TIMEOUT_PosH                              6

/*!< 0x0040 ISR
*null
*/
#define BUSY_DETECT_Msk                                     (0x01 << BUSY_DETECT_Pos)
#define BUSY_DETECT_Pos                                     5
#define BUSY_DETECT_PosL                                    5
#define BUSY_DETECT_PosH                                    5

/*!< 0x0040 ISR
*null
*/
#define RECEIVER_LINE_STATUS_Msk                            (0x01 << RECEIVER_LINE_STATUS_Pos)
#define RECEIVER_LINE_STATUS_Pos                            4
#define RECEIVER_LINE_STATUS_PosL                           4
#define RECEIVER_LINE_STATUS_PosH                           4

/*!< 0x0040 ISR
*null
*/
#define RECEIVED_DATA_AVAILABLE_Msk                         (0x01 << RECEIVED_DATA_AVAILABLE_Pos)
#define RECEIVED_DATA_AVAILABLE_Pos                         3
#define RECEIVED_DATA_AVAILABLE_PosL                        3
#define RECEIVED_DATA_AVAILABLE_PosH                        3

/*!< 0x0040 ISR
*null
*/
#define THR_EMPTY_Msk                                       (0x01 << THR_EMPTY_Pos)
#define THR_EMPTY_Pos                                       2
#define THR_EMPTY_PosL                                      2
#define THR_EMPTY_PosH                                      2

/*!< 0x0040 ISR
*null
*/
#define NO_INTERRUPT_PENDING_Msk                            (0x01 << NO_INTERRUPT_PENDING_Pos)
#define NO_INTERRUPT_PENDING_Pos                            1
#define NO_INTERRUPT_PENDING_PosL                           1
#define NO_INTERRUPT_PENDING_PosH                           1

/*!< 0x0040 ISR
*null
*/
#define MODEM_STATUS_Msk                                    (0x01 << MODEM_STATUS_Pos)
#define MODEM_STATUS_Pos                                    0
#define MODEM_STATUS_PosL                                   0
#define MODEM_STATUS_PosH                                   0


/*!< 0x0040 ISR
*null
*/
#define ISR_Msk                                             (0x3F << ISR_Pos)
#define ISR_Pos                                             0
#define ISR_PosL                                            0
#define ISR_PosH                                            6


/*!< 0x0044 SR
*null
*/
#define DCD_Msk                                             (0x01 << DCD_Pos)
#define DCD_Pos                                             23
#define DCD_PosL                                            23
#define DCD_PosH                                            23

/*!< 0x0044 SR
*null
*/
#define RI_Msk                                              (0x01 << RI_Pos)
#define RI_Pos                                              22
#define RI_PosL                                             22
#define RI_PosH                                             22

/*!< 0x0044 SR
*null
*/
#define DSR_Msk                                             (0x01 << DSR_Pos)
#define DSR_Pos                                             21
#define DSR_PosL                                            21
#define DSR_PosH                                            21

/*!< 0x0044 SR
*null
*/
#define CTS_Msk                                             (0x01 << CTS_Pos)
#define CTS_Pos                                             20
#define CTS_PosL                                            20
#define CTS_PosH                                            20

/*!< 0x0044 SR
*null
*/
#define DDCD_Msk                                            (0x01 << DDCD_Pos)
#define DDCD_Pos                                            19
#define DDCD_PosL                                           19
#define DDCD_PosH                                           19

/*!< 0x0044 SR
*null
*/
#define TERI_Msk                                            (0x01 << TERI_Pos)
#define TERI_Pos                                            18
#define TERI_PosL                                           18
#define TERI_PosH                                           18

/*!< 0x0044 SR
*null
*/
#define DDSR_Msk                                            (0x01 << DDSR_Pos)
#define DDSR_Pos                                            17
#define DDSR_PosL                                           17
#define DDSR_PosH                                           17

/*!< 0x0044 SR
*null
*/
#define DCTS_Msk                                            (0x01 << DCTS_Pos)
#define DCTS_Pos                                            16
#define DCTS_PosL                                           16
#define DCTS_PosH                                           16

/*!< 0x0044 SR
*null
*/
#define ADDR_RCVD_Msk                                       (0x01 << ADDR_RCVD_Pos)
#define ADDR_RCVD_Pos                                       8
#define ADDR_RCVD_PosL                                      8
#define ADDR_RCVD_PosH                                      8

/*!< 0x0044 SR
*null
*/
#define RFE_Msk                                             (0x01 << RFE_Pos)
#define RFE_Pos                                             7
#define RFE_PosL                                            7
#define RFE_PosH                                            7

/*!< 0x0044 SR
*null
*/
#define TMET_Msk                                            (0x01 << TMET_Pos)
#define TMET_Pos                                            6
#define TMET_PosL                                           6
#define TMET_PosH                                           6

/*!< 0x0044 SR
*null
*/
#define THRE_Msk                                            (0x01 << THRE_Pos)
#define THRE_Pos                                            5
#define THRE_PosL                                           5
#define THRE_PosH                                           5

/*!< 0x0044 SR
*null
*/
#define BI_Msk                                              (0x01 << BI_Pos)
#define BI_Pos                                              4
#define BI_PosL                                             4
#define BI_PosH                                             4

/*!< 0x0044 SR
*null
*/
#define FE_Msk                                              (0x01 << FE_Pos)
#define FE_Pos                                              3
#define FE_PosL                                             3
#define FE_PosH                                             3

/*!< 0x0044 SR
*null
*/
#define PE_Msk                                              (0x01 << PE_Pos)
#define PE_Pos                                              2
#define PE_PosL                                             2
#define PE_PosH                                             2

/*!< 0x0044 SR
*null
*/
#define OE_Msk                                              (0x01 << OE_Pos)
#define OE_Pos                                              1
#define OE_PosL                                             1
#define OE_PosH                                             1

/*!< 0x0044 SR
*null
*/
#define DR_Msk                                              (0x01 << DR_Pos)
#define DR_Pos                                              0
#define DR_PosL                                             0
#define DR_PosH                                             0

/*!< 0x0044 SR
*null
*/
#define LS_Msk                                              (0xFF << LS_Pos)
#define LS_Pos                                              0
#define LS_PosL                                             0
#define LS_PosH                                             0


/*!< 0x0048 FSR
*null
*/
#define RFL_Msk                                             (0x7F << RFL_Pos)
#define RFL_Pos                                             16
#define RFL_PosL                                            16
#define RFL_PosH                                            22

/*!< 0x0048 FSR
*null
*/
#define TFL_Msk                                             (0x7F << TFL_Pos)
#define TFL_Pos                                             8
#define TFL_PosL                                            8
#define TFL_PosH                                            14

/*!< 0x0048 FSR
*null
*/
#define RFF_Msk                                             (0x01 << RFF_Pos)
#define RFF_Pos                                             4
#define RFF_PosL                                            4
#define RFF_PosH                                            4

/*!< 0x0048 FSR
*null
*/
#define RFNE_Msk                                            (0x01 << RFNE_Pos)
#define RFNE_Pos                                            3
#define RFNE_PosL                                           3
#define RFNE_PosH                                           3

/*!< 0x0048 FSR
*null
*/
#define TFE_Msk                                             (0x01 << TFE_Pos)
#define TFE_Pos                                             2
#define TFE_PosL                                            2
#define TFE_PosH                                            2

/*!< 0x0048 FSR
*null
*/
#define TFNF_Msk                                            (0x01 << TFNF_Pos)
#define TFNF_Pos                                            1
#define TFNF_PosL                                           1
#define TFNF_PosH                                           1


/*!< 0x004c RDR
*null
*/
#define RBR_Msk                                             (0X1FF << RBR_Pos)
#define RBR_Pos                                             0
#define RBR_PosL                                            0
#define RBR_PosH                                            8


/*!< 0x0050 TDR
*null
*/
#define THR_Msk                                             (0X1FF << THR_Pos)
#define THR_Pos                                             0
#define THR_PosL                                            0
#define THR_PosH                                            8


/*!< 0x0054 UPR
*null
*/
#define FIFO_MODE_Msk                                       (0xFF << FIFO_MODE_Pos)
#define FIFO_MODE_Pos                                       16
#define FIFO_MODE_PosL                                      16
#define FIFO_MODE_PosH                                      23

/*!< 0x0054 UPR
*null
*/
#define DMA_EXTRA_Msk                                       (0x01 << DMA_EXTRA_Pos)
#define DMA_EXTRA_Pos                                       13
#define DMA_EXTRA_PosL                                      13
#define DMA_EXTRA_PosH                                      13

/*!< 0x0054 UPR
*null
*/
#define UART_ADD_ENCODED_PARAMS_Msk                         (0x01 << UART_ADD_ENCODED_PARAMS_Pos)
#define UART_ADD_ENCODED_PARAMS_Pos                         12
#define UART_ADD_ENCODED_PARAMS_PosL                        12
#define UART_ADD_ENCODED_PARAMS_PosH                        12

/*!< 0x0054 UPR
*null
*/
#define SHADOW_Msk                                          (0x01 << SHADOW_Pos)
#define SHADOW_Pos                                          11
#define SHADOW_PosL                                         11
#define SHADOW_PosH                                         11

/*!< 0x0054 UPR
*null
*/
#define FIFO_STAT_Msk                                       (0x01 << FIFO_STAT_Pos)
#define FIFO_STAT_Pos                                       10
#define FIFO_STAT_PosL                                      10
#define FIFO_STAT_PosH                                      10

/*!< 0x0054 UPR
*null
*/
#define FIFO_ACCESS_Msk                                     (0x01 << FIFO_ACCESS_Pos)
#define FIFO_ACCESS_Pos                                     9
#define FIFO_ACCESS_PosL                                    9
#define FIFO_ACCESS_PosH                                    9

/*!< 0x0054 UPR
*null
*/
#define ADDITIONAL_FEAT_Msk                                 (0x01 << ADDITIONAL_FEAT_Pos)
#define ADDITIONAL_FEAT_Pos                                 8
#define ADDITIONAL_FEAT_PosL                                8
#define ADDITIONAL_FEAT_PosH                                8

/*!< 0x0054 UPR
*null
*/
#define SIR_LP_MODE_Msk                                     (0x01 << SIR_LP_MODE_Pos)
#define SIR_LP_MODE_Pos                                     7
#define SIR_LP_MODE_PosL                                    7
#define SIR_LP_MODE_PosH                                    7

/*!< 0x0054 UPR
*null
*/
#define SIR_MODE_Msk                                        (0x01 << SIR_MODE_Pos)
#define SIR_MODE_Pos                                        6
#define SIR_MODE_PosL                                       6
#define SIR_MODE_PosH                                       6

/*!< 0x0054 UPR
*null
*/
#define THRE_MODE_Msk                                       (0x01 << THRE_MODE_Pos)
#define THRE_MODE_Pos                                       5
#define THRE_MODE_PosL                                      5
#define THRE_MODE_PosH                                      5

/*!< 0x0054 UPR
*null
*/
#define AFCE_MODE_Msk                                       (0x01 << AFCE_MODE_Pos)
#define AFCE_MODE_Pos                                       4
#define AFCE_MODE_PosL                                      4
#define AFCE_MODE_PosH                                      4

/*!< 0x0054 UPR
*null
*/
#define APB_DATA_WIDTH_Msk                                  (0x03 << APB_DATA_WIDTH_Pos)
#define APB_DATA_WIDTH_Pos                                  0
#define APB_DATA_WIDTH_PosL                                 0
#define APB_DATA_WIDTH_PosH                                 1


/*!< 0x0058 UV
*null
*/
#define UART_COMPONENT_VERSION_Msk                          (0XFFFFFFF << UART_COMPONENT_VERSION_Pos)
#define UART_COMPONENT_VERSION_Pos                          0
#define UART_COMPONENT_VERSION_PosL                         0
#define UART_COMPONENT_VERSION_PosH                         31


/*!< 0x005c UTR
*null
*/
#define PERIPHERAL_ID_Msk                                   (0XFFFFFFF << PERIPHERAL_ID_Pos)
#define PERIPHERAL_ID_Pos                                   0
#define PERIPHERAL_ID_PosL                                  0
#define PERIPHERAL_ID_PosH                                  31


/**
* @}
* End of group HW_UART
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_UART_H_ */