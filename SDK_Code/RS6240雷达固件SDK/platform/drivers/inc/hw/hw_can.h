/****************************************************************
* @file  CAN.h
* @brief CAN hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
****************************************************************
*/

#ifndef _HW_CAN_H_
#define _HW_CAN_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup CAN
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __IOM uint32_t CFG;                                    /*0x0000,Configure Register                                 */
    __IOM uint32_t CTRL;                                   /*0x0004,Control Register                                   */
    __IOM uint32_t S_BAUDR;                                /*0x0008,Slow Bit Rate set Register.                        */
    __IOM uint32_t F_BAUDR;                                /*0x000c,Fast Bit Rate set Register.                        */
    __IM  uint32_t STATUS;                                 /*0x0010,Status Register.                                   */
    __IOM uint32_t ERR_COUNT;                              /*0x0014,Error Count Register                               */
    __IOM uint32_t IER;                                    /*0x0018,Interrupt Enable Register                          */
    __IOM uint32_t ISR;                                    /*0x001c,Interrupt Status Register                          */
    __IOM uint32_t ACF;                                    /*0x0020,Acceptance Filter ID or Mask*/
    __IOM uint32_t TTCAN_CFG;                              /*0x0024,Time Trigger CAN Configure R*/
    __IOM uint32_t TRIG_TIME;                              /*0x0028,Trigger Time Register                              */
    __IOM uint32_t DMATR_REG;                              /*0x002c,Reference Message Register                         */
    __IM  uint32_t TTS_L;                                  /*0x0030,Transmission Time Stamp                            */
    __IM  uint32_t TTS_H;                                  /*0x0034,Transmission Time Stamp                            */
    __IM  uint32_t VER;                                    /*0x0038,Version Information                                */
          uint32_t RSVD;                                    
    __IM  uint32_t RBUF[20];                               /*0x0040,Receive Buffer Registers and					   */
    __IOM uint32_t TBUF[20];                    		   /*0x0090,Transmit Buffer Registers  						   */
} can_dev_t;

/*!< 0x0000 CFG 
*Acceptance filter EnableEach bit in this register corresponds to a acceptance filter. When a bit in this register is set (1), the corresponding acceptance filter will be activated. flag. Range Variable[x]: ACF_NUMER+15
*/
#define AE_Msk                                              (0X07 << AE_Pos)
#define AE_Pos                                              16
#define AE_PosL                                             16
#define AE_PosH                                             18

/*!< 0x0000 CFG 
*CAN FD ISO mode
*/
#define FD_ISO_Msk                                          (0x01 << FD_ISO_Pos)
#define FD_ISO_Pos                                          7
#define FD_ISO_PosL                                         7
#define FD_ISO_PosH                                         7

/*!< 0x0000 CFG 
*Transmit Priority ID Mode
*/
#define PRMODE_Msk                                          (0x01 << PRMODE_Pos)
#define PRMODE_Pos                                          6
#define PRMODE_PosL                                         6
#define PRMODE_PosH                                         6

/*!< 0x0000 CFG 
*Listen Only Mode
*/
#define LOM_Msk                                             (0x01 << LOM_Pos)
#define LOM_Pos                                             5
#define LOM_PosL                                            5
#define LOM_PosH                                            5

/*!< 0x0000 CFG 
*Transceiver Standby Mode
*/
#define STBY_Msk                                            (0x01 << STBY_Pos)
#define STBY_Pos                                            4
#define STBY_PosL                                           4
#define STBY_PosH                                           4

/*!< 0x0000 CFG 
*Loop Back Mode, External
*/
#define LBME_Msk                                            (0x01 << LBME_Pos)
#define LBME_Pos                                            3
#define LBME_PosL                                           3
#define LBME_PosH                                           3

/*!< 0x0000 CFG 
*Loop Back Mode, Internal
*/
#define LBMI_Msk                                            (0x01 << LBMI_Pos)
#define LBMI_Pos                                            2
#define LBMI_PosL                                           2
#define LBMI_PosH                                           2

/*!< 0x0000 CFG 
*null
*/
#define PSS_Msk                                             (0x01 << PSS_Pos)
#define PSS_Pos                                             1
#define PSS_PosL                                            1
#define PSS_PosH                                            1

/*!< 0x0000 CFG 
*null
*/
#define SSS_Msk                                             (0x01 << SSS_Pos)
#define SSS_Pos                                             0
#define SSS_PosL                                            0
#define SSS_PosH                                            0


/*!< 0x0004 CTRL 
*RESET request bit
*/
#define RESET_Msk                                           (0x01 << RESET_Pos)
#define RESET_Pos                                           31
#define RESET_PosL                                          31
#define RESET_PosH                                          31

/*!< 0x0004 CTRL 
*SELect acceptance MASK
*/
#define SELMASK_Msk                                         (0x01 << SELMASK_Pos)
#define SELMASK_Pos                                         28
#define SELMASK_PosL                                        28
#define SELMASK_PosH                                        28

/*!< 0x0004 CTRL 
*Acceptance filter address
*/
#define ACFADR_Msk                                          (0x0F << ACFADR_Pos)
#define ACFADR_Pos                                          24
#define ACFADR_PosL                                         24
#define ACFADR_PosH                                         27

/*!< 0x0004 CTRL 
*Transmitter Delay Compensation ENable
*/
#define TDCEN_Msk                                           (0x01 << TDCEN_Pos)
#define TDCEN_Pos                                           23
#define TDCEN_PosL                                          23
#define TDCEN_PosH                                          23

/*!< 0x0004 CTRL 
*Secondary Sample Point Offset
*/
#define SSPOFF_Msk                                          (0x7F << SSPOFF_Pos)
#define SSPOFF_Pos                                          16
#define SSPOFF_PosL                                         16
#define SSPOFF_PosH                                         22

/*!< 0x0004 CTRL 
*The controller status is “bus off”
*/
#define BUSOFF_Msk                                          (0x01 << BUSOFF_Pos)
#define BUSOFF_Pos                                          15
#define BUSOFF_PosL                                         15
#define BUSOFF_PosH                                         15

/*!< 0x0004 CTRL 
*Self-ACKnowledge
*/
#define SACK_Msk                                            (0x01 << SACK_Pos)
#define SACK_Pos                                            14
#define SACK_PosL                                           14
#define SACK_PosH                                           14

/*!< 0x0004 CTRL 
*null
*/
#define ROM_Msk                                             (0x01 << ROM_Pos)
#define ROM_Pos                                             13
#define ROM_PosL                                            13
#define ROM_PosH                                            13

/*!< 0x0004 CTRL 
*Receive buffer RELease.  The host controller has read the actual RB slot and releases it. Afterwards the CAN-CTRL core points to the next RB slot. RSTAT gets updated
*/
#define RREL_Msk                                            (0x01 << RREL_Pos)
#define RREL_Pos                                            12
#define RREL_PosL                                           12
#define RREL_PosH                                           12

/*!< 0x0004 CTRL 
*Receive FIFO Threshold Level.
*/
#define RFTL_Msk                                            (0x0F << RFTL_Pos)
#define RFTL_Pos                                            8
#define RFTL_PosL                                           8
#define RFTL_PosH                                           11

/*!< 0x0004 CTRL 
*null
*/
#define RALL_Msk                                            (0x01 << RALL_Pos)
#define RALL_Pos                                            7
#define RALL_PosL                                           7
#define RALL_PosH                                           7

/*!< 0x0004 CTRL 
*null
*/
#define TSNEXT_Msk                                          (0x01 << TSNEXT_Pos)
#define TSNEXT_Pos                                          6
#define TSNEXT_PosL                                         6
#define TSNEXT_PosH                                         6

/*!< 0x0004 CTRL 
*null
*/
#define TBSEL_Msk                                           (0x01 << TBSEL_Pos)
#define TBSEL_Pos                                           5
#define TBSEL_PosL                                          5
#define TBSEL_PosH                                          5

/*!< 0x0004 CTRL 
*null
*/
#define TPE_Msk                                             (0x01 << TPE_Pos)
#define TPE_Pos                                             4
#define TPE_PosL                                            4
#define TPE_PosH                                            4

/*!< 0x0004 CTRL 
*null
*/
#define TPA_Msk                                             (0x01 << TPA_Pos)
#define TPA_Pos                                             3
#define TPA_PosL                                            3
#define TPA_PosH                                            3

/*!< 0x0004 CTRL 
*null
*/
#define TSONE_Msk                                           (0x01 << TSONE_Pos)
#define TSONE_Pos                                           2
#define TSONE_PosL                                          2
#define TSONE_PosH                                          2

/*!< 0x0004 CTRL 
*null
*/
#define TSALL_Msk                                           (0x01 << TSALL_Pos)
#define TSALL_Pos                                           1
#define TSALL_PosL                                          1
#define TSALL_PosH                                          1

/*!< 0x0004 CTRL 
*null
*/
#define TSA_Msk                                             (0x01 << TSA_Pos)
#define TSA_Pos                                             0
#define TSA_PosL                                            0
#define TSA_PosH                                            0


/*!< 0x0008 S_BAUDR 
*Slow prescaler. The prescaler divides the system clock to get the time quanta clock tq_clk. Valid range PRESC=[0x00, 0xff] results in divider values 1 to 256.
*/
#define S_PRESC_Msk                                         (0xFF << S_PRESC_Pos)
#define S_PRESC_Pos                                         24
#define S_PRESC_PosL                                        24
#define S_PRESC_PosH                                        31

/*!< 0x0008 S_BAUDR 
*Synchronization Jump Width (slow speed)
*/
#define S_SJW_Msk                                           (0x7F << S_SJW_Pos)
#define S_SJW_Pos                                           16
#define S_SJW_PosL                                          16
#define S_SJW_PosH                                          22

/*!< 0x0008 S_BAUDR 
*Bit Timing Segment 2 (slow speed).
*/
#define S_SEG_2_Msk                                         (0x7F << S_SEG_2_Pos)
#define S_SEG_2_Pos                                         8
#define S_SEG_2_PosL                                        8
#define S_SEG_2_PosH                                        14

/*!< 0x0008 S_BAUDR 
*Bit Timing Segment 1 (slow speed).
*/
#define S_SEG_1_Msk                                         (0xFF << S_SEG_1_Pos)
#define S_SEG_1_Pos                                         0
#define S_SEG_1_PosL                                        0
#define S_SEG_1_PosH                                        7


/*!< 0x000c F_BAUDR 
*Fast prescaler. The prescaler divides the system clock to get the time quanta clock tq_clk. Valid range PRESC=[0x00, 0xff] results in divider values 1 to 256.
*/
#define F_PRESC_Msk                                         (0xFF << F_PRESC_Pos)
#define F_PRESC_Pos                                         24
#define F_PRESC_PosL                                        24
#define F_PRESC_PosH                                        31

/*!< 0x000c F_BAUDR 
*Synchronization Jump Width (fast speed)
*/
#define F_SJW_Msk                                           (0x0F << F_SJW_Pos)
#define F_SJW_Pos                                           16
#define F_SJW_PosL                                          16
#define F_SJW_PosH                                          19

/*!< 0x000c F_BAUDR 
*Bit Timing Segment 2 (fast speed).
*/
#define F_SEG_2_Msk                                         (0x0F << F_SEG_2_Pos)
#define F_SEG_2_Pos                                         8
#define F_SEG_2_PosL                                        8
#define F_SEG_2_PosH                                        11

/*!< 0x000c F_BAUDR 
*Bit Timing Segment 1 (fast speed).
*/
#define F_SEG_1_Msk                                         (0x1F << F_SEG_1_Pos)
#define F_SEG_1_Pos                                         0
#define F_SEG_1_PosL                                        0
#define F_SEG_1_PosH                                        4


/*!< 0x0010 STATUS 
*Receive buffer OVerflow
*/
#define ROV_Msk                                             (0x01 << ROV_Pos)
#define ROV_Pos                                             18
#define ROV_PosL                                            18
#define ROV_PosH                                            18

/*!< 0x0010 STATUS 
*Receive buffer STATus
*/
#define RSTAT_Msk                                           (0x03 << RSTAT_Pos)
#define RSTAT_Pos                                           16
#define RSTAT_PosL                                          16
#define RSTAT_PosH                                          17

/*!< 0x0010 STATUS 
*Error WARNing limit reached
*/
#define EWARN_Msk                                           (0x01 << EWARN_Pos)
#define EWARN_Pos                                           15
#define EWARN_PosL                                          15
#define EWARN_PosH                                          15

/*!< 0x0010 STATUS 
*Error Passive mode active
*/
#define EPASS_Msk                                           (0x01 << EPASS_Pos)
#define EPASS_Pos                                           14
#define EPASS_PosL                                          14
#define EPASS_PosH                                          14

/*!< 0x0010 STATUS 
*Reception ACTIVE
*/
#define RACTIVE_Msk                                         (0x01 << RACTIVE_Pos)
#define RACTIVE_Pos                                         13
#define RACTIVE_PosL                                        13
#define RACTIVE_PosH                                        13

/*!< 0x0010 STATUS 
*Transmission ACTIVE
*/
#define TACTIVE_Msk                                         (0x01 << TACTIVE_Pos)
#define TACTIVE_Pos                                         12
#define TACTIVE_PosL                                        12
#define TACTIVE_PosH                                        12

/*!< 0x0010 STATUS 
*If the STB is disabled using STB_DISABLE, then TSFF=0 If TTEN=0 or TTTBM=0: Transmit Secondary buffer Full Flag
*/
#define TSFF_Msk                                            (0x01 << TSFF_Pos)
#define TSFF_Pos                                            11
#define TSFF_PosL                                           11
#define TSFF_PosH                                           11

/*!< 0x0010 STATUS 
*Transmission Secondary STATus bits.  It present different status when TTEN and TTTBM configure to different value. If the STB is disabled using STB_DISABLE, then TSSTAT=00.
*/
#define TSSTAT_Msk                                          (0x03 << TSSTAT_Pos)
#define TSSTAT_Pos                                          8
#define TSSTAT_PosL                                         8
#define TSSTAT_PosH                                         9

/*!< 0x0010 STATUS 
*Kind Of ERror (Error code)
*/
#define KOER_Msk                                            (0x07 << KOER_Pos)
#define KOER_Pos                                            5
#define KOER_PosL                                           5
#define KOER_PosH                                           7

/*!< 0x0010 STATUS 
*Arbitration Lost Capture (bit position in the frame where the arbitration has been lost).
*/
#define ALC_Msk                                             (0x1F << ALC_Pos)
#define ALC_Pos                                             0
#define ALC_PosL                                            0
#define ALC_PosH                                            4


/*!< 0x0014 ERR_COUNT 
*1、Programmable Error Warning Limit = (EWL+1)*8 2、1’b1:Listen Only Mode enabled
*/
#define EWL_Msk                                             (0x0F << EWL_Pos)
#define EWL_Pos                                             16
#define EWL_PosL                                            16
#define EWL_PosH                                            19

/*!< 0x0014 ERR_COUNT 
*Transmit Error CouNT (number of errors during transmission).
*/
#define TECNT_Msk                                           (0xFF << TECNT_Pos)
#define TECNT_Pos                                           8
#define TECNT_PosL                                          8
#define TECNT_PosH                                          15

/*!< 0x0014 ERR_COUNT 
*Receive Error CouNT (number of errors during reception).
*/
#define RECNT_Msk                                           (0xFF << RECNT_Pos)
#define RECNT_Pos                                           0
#define RECNT_PosL                                          0
#define RECNT_PosH                                          7


/*!< 0x0018 IER 
*Watch Trigger Interrupt Enable
*/
#define WTIE_Msk                                            (0x01 << WTIE_Pos)
#define WTIE_Pos                                            11
#define WTIE_PosL                                           11
#define WTIE_PosH                                           11

/*!< 0x0018 IER 
*Time Trigger Interrupt Enable
*/
#define TTIE_Msk                                            (0x01 << TTIE_Pos)
#define TTIE_Pos                                            10
#define TTIE_PosL                                           10
#define TTIE_PosH                                           10

/*!< 0x0018 IER 
*Error Passive Interrupt Enable
*/
#define EPIE_Msk                                            (0x01 << EPIE_Pos)
#define EPIE_Pos                                            9
#define EPIE_PosL                                           9
#define EPIE_PosH                                           9

/*!< 0x0018 IER 
*Arbitration Lost Interrupt Enable
*/
#define ALIE_Msk                                            (0x01 << ALIE_Pos)
#define ALIE_Pos                                            8
#define ALIE_PosL                                           8
#define ALIE_PosH                                           8

/*!< 0x0018 IER 
*Bus Error Interrupt Enable
*/
#define BEIE_Msk                                            (0x01 << BEIE_Pos)
#define BEIE_Pos                                            7
#define BEIE_PosL                                           7
#define BEIE_PosH                                           7

/*!< 0x0018 IER 
*Receive Interrupt Enable
*/
#define RIE_Msk                                             (0x01 << RIE_Pos)
#define RIE_Pos                                             6
#define RIE_PosL                                            6
#define RIE_PosH                                            6

/*!< 0x0018 IER 
*RB Overrun Interrupt Enable
*/
#define ROIE_Msk                                            (0x01 << ROIE_Pos)
#define ROIE_Pos                                            5
#define ROIE_PosL                                           5
#define ROIE_PosH                                           5

/*!< 0x0018 IER 
*RB Full Interrupt Enable
*/
#define RFIE_Msk                                            (0x01 << RFIE_Pos)
#define RFIE_Pos                                            4
#define RFIE_PosL                                           4
#define RFIE_PosH                                           4

/*!< 0x0018 IER 
*RB Almost Full Interrupt Enable
*/
#define RAFIE_Msk                                           (0x01 << RAFIE_Pos)
#define RAFIE_Pos                                           3
#define RAFIE_PosL                                          3
#define RAFIE_PosH                                          3

/*!< 0x0018 IER 
*Transmission Primary Interrupt Enable
*/
#define TPIE_Msk                                            (0x01 << TPIE_Pos)
#define TPIE_Pos                                            2
#define TPIE_PosL                                           2
#define TPIE_PosH                                           2

/*!< 0x0018 IER 
*Transmission Secondary Interrupt Enable
*/
#define TSIE_Msk                                            (0x01 << TSIE_Pos)
#define TSIE_Pos                                            1
#define TSIE_PosL                                           1
#define TSIE_PosH                                           1

/*!< 0x0018 IER 
*Error Interrupt Enable
*/
#define EIE_Msk                                             (0x01 << EIE_Pos)
#define EIE_Pos                                             0
#define EIE_PosL                                            0
#define EIE_PosH                                            0


/*!< 0x001c ISR 
*Trigger Error Interrupt Flag
*/
#define TEIS_Msk                                            (0x01 << TEIS_Pos)
#define TEIS_Pos                                            13
#define TEIS_PosL                                           13
#define TEIS_PosH                                           13

/*!< 0x001c ISR 
*Abort Interrupt Flag
*/
#define AIS_Msk                                             (0x01 << AIS_Pos)
#define AIS_Pos                                             12
#define AIS_PosL                                            12
#define AIS_PosH                                            12

/*!< 0x001c ISR 
*Watch Trigger Interrupt Flag
*/
#define WTIS_Msk                                            (0x01 << WTIS_Pos)
#define WTIS_Pos                                            11
#define WTIS_PosL                                           11
#define WTIS_PosH                                           11

/*!< 0x001c ISR 
*Time Trigger Interrupt Flag
*/
#define TTIS_Msk                                            (0x01 << TTIS_Pos)
#define TTIS_Pos                                            10
#define TTIS_PosL                                           10
#define TTIS_PosH                                           10

/*!< 0x001c ISR 
*Error Passive Interrupt Flag
*/
#define EPIS_Msk                                            (0x01 << EPIS_Pos)
#define EPIS_Pos                                            9
#define EPIS_PosL                                           9
#define EPIS_PosH                                           9

/*!< 0x001c ISR 
*Arbitration Lost Interrupt Flag
*/
#define ALIS_Msk                                            (0x01 << ALIS_Pos)
#define ALIS_Pos                                            8
#define ALIS_PosL                                           8
#define ALIS_PosH                                           8

/*!< 0x001c ISR 
*Bus Error Interrupt Flag
*/
#define BEIS_Msk                                            (0x01 << BEIS_Pos)
#define BEIS_Pos                                            7
#define BEIS_PosL                                           7
#define BEIS_PosH                                           7

/*!< 0x001c ISR 
*Receive Interrupt Flag
*/
#define RIS_Msk                                             (0x01 << RIS_Pos)
#define RIS_Pos                                             6
#define RIS_PosL                                            6
#define RIS_PosH                                            6

/*!< 0x001c ISR 
*RB Overrun Interrupt Flag
*/
#define ROIS_Msk                                            (0x01 << ROIS_Pos)
#define ROIS_Pos                                            5
#define ROIS_PosL                                           5
#define ROIS_PosH                                           5

/*!< 0x001c ISR 
*RB Full Interrupt Flag
*/
#define RFIS_Msk                                            (0x01 << RFIS_Pos)
#define RFIS_Pos                                            4
#define RFIS_PosL                                           4
#define RFIS_PosH                                           4

/*!< 0x001c ISR 
*RB Almost Full Interrupt Flag
*/
#define RAFIS_Msk                                           (0x01 << RAFIS_Pos)
#define RAFIS_Pos                                           3
#define RAFIS_PosL                                          3
#define RAFIS_PosH                                          3

/*!< 0x001c ISR 
*Transmission Primary Interrupt Flag
*/
#define TPIS_Msk                                            (0x01 << TPIS_Pos)
#define TPIS_Pos                                            2
#define TPIS_PosL                                           2
#define TPIS_PosH                                           2

/*!< 0x001c ISR 
*Transmission Secondary Interrupt Flag
*/
#define TSIS_Msk                                            (0x01 << TSIS_Pos)
#define TSIS_Pos                                            1
#define TSIS_PosL                                           1
#define TSIS_PosH                                           1

/*!< 0x001c ISR 
*Error Interrupt Flag
*/
#define EIS_Msk                                             (0x01 << EIS_Pos)
#define EIS_Pos                                             0
#define EIS_PosL                                            0
#define EIS_PosH                                            0


/*!< 0x0020 SSR 
*Acceptance mask IDE bit check enable
*/
#define ACFIDE_Msk                                          (0x01 << ACFIDE_Pos)
#define ACFIDE_Pos                                          30
#define ACFIDE_PosL                                         30
#define ACFIDE_PosH                                         30

/*!< 0x0020 SSR 
*Acceptance mask IDE bit value
*/
#define ACFEX_Msk                                           (0x01 << ACFEX_Pos)
#define ACFEX_Pos                                           29
#define ACFEX_PosL                                          29
#define ACFEX_PosH                                          29

#define ACF_MODE_Msk                                        (0x03 << ACF_MODE_Pos)
#define ACF_MODE_Pos                                        29

/*!< 0x0020 SSR 
*1、Acceptance CODE 1’b0: ACC bit value to compare with ID bit of the received message 1’b1: ACC bit value to compare with ID bit of the received message 2、Acceptance MASK 1’b0: acceptance check for these bits of receive identifier enable 1’b1: acceptance check for these bits of receive identifier disabled.
*/
#define ACODE_XORAMASK_X_Msk                          		(0x1FFFFFFF << ACODE_XORAMASK_X_Pos)
#define ACODE_XORAMASK_X_Pos                          		0
#define ACODE_XORAMASK_X_PosL                      			0
#define ACODE_XORAMASK_X_PosH                     		    28


/*!< 0x0024 TTCAN_CFG 
*Transmit Enable Window. For a single shot transmit trigger there is a time of up to 16 ticks of the cycle time where the frame is allowed to start. TWE+1 defines the number of ticks.
*/
#define TEW_Msk                                             (0x0F << TEW_Pos)
#define TEW_Pos                                             28
#define TEW_PosL                                            28
#define TEW_PosH                                            31

/*!< 0x0024 TTCAN_CFG 
*Trigger Type.
*/
#define TTYPE_Msk                                           (0x07 << TTYPE_Pos)
#define TTYPE_Pos                                           24
#define TTYPE_PosL                                          24
#define TTYPE_PosH                                          26

/*!< 0x0024 TTCAN_CFG 
*Transmit Trigger TB slot Pointer
*/
#define TTPTR_Msk                                           (0x3F << TTPTR_Pos)
#define TTPTR_Pos                                           16
#define TTPTR_PosL                                          16
#define TTPTR_PosH                                          21

/*!< 0x0024 TTCAN_CFG 
*TTCAN Timer PRESCaler。 The TTCAN time base is a CAN bittime defined by S_PRES, S_SEG_1 and S_SEG_2. With T_PRESC an additional prescaling factor of 1, 2, 4 or 8 is defined.
*/
#define T_PRESC_Msk                                         (0x03 << T_PRESC_Pos)
#define T_PRESC_Pos                                         14
#define T_PRESC_PosL                                        14
#define T_PRESC_PosH                                        15

/*!< 0x0024 TTCAN_CFG 
*Pointer to a TB message slot
*/
#define TBPTR_Msk                                           (0x3F << TBPTR_Pos)
#define TBPTR_Pos                                           8
#define TBPTR_PosL                                          8
#define TBPTR_PosH                                          13

/*!< 0x0024 TTCAN_CFG 
*Time Trigger Enable
*/
#define TTEN_Msk                                            (0x01 << TTEN_Pos)
#define TTEN_Pos                                            5
#define TTEN_PosL                                           5
#define TTEN_PosH                                           5

/*!< 0x0024 TTCAN_CFG 
*set TB slot to “Empty”
*/
#define TBE_Msk                                             (0x01 << TBE_Pos)
#define TBE_Pos                                             4
#define TBE_PosL                                            4
#define TBE_PosH                                            4

/*!< 0x0024 TTCAN_CFG 
*set TB slot to “Filled”
*/
#define TBF_Msk                                             (0x01 << TBF_Pos)
#define TBF_Pos                                             3
#define TBF_PosL                                            3
#define TBF_PosH                                            3

/*!< 0x0024 TTCAN_CFG 
*TIME-stamping POSition
*/
#define TIMEPOS_Msk                                         (0x01 << TIMEPOS_Pos)
#define TIMEPOS_Pos                                         2
#define TIMEPOS_PosL                                        2
#define TIMEPOS_PosH                                        2

/*!< 0x0024 TTCAN_CFG 
*TIME-stamping ENable
*/
#define TIMEEN_Msk                                          (0x01 << TIMEEN_Pos)
#define TIMEEN_Pos                                          1
#define TIMEEN_PosL                                         1
#define TIMEEN_PosH                                         1

/*!< 0x0024 TTCAN_CFG 
*If TTEN=0 then TTTBM is ignored
*/
#define TTTBM_Msk                                           (0x01 << TTTBM_Pos)
#define TTTBM_Pos                                           0
#define TTTBM_PosL                                          0
#define TTTBM_PosH                                          0


/*!< 0x0028 TRIG_TIME 
*TT_TRIG(15:0) defines the cycle time for a trigger.
*/
#define TT_TRIG_Msk                                         (0xFFFF << TT_TRIG_Pos)
#define TT_TRIG_Pos                                         16
#define TT_TRIG_PosL                                        16
#define TT_TRIG_PosH                                        31

/*!< 0x0028 TRIG_TIME 
*Watch Trigger Time
*/
#define TT_WTRIG_Msk                                        (0xFFFF << TT_WTRIG_Pos)
#define TT_WTRIG_Pos                                        0
#define TT_WTRIG_PosL                                       0
#define TT_WTRIG_PosH                                       15


/*!< 0x002c DMATR_REG 
*REFerence message IDE bit.
*/
#define REF_IDE_Msk                                         (0x01 << REF_IDE_Pos)
#define REF_IDE_Pos                                         31
#define REF_IDE_PosL                                        31
#define REF_IDE_PosH                                        31

/*!< 0x002c DMATR_REG 
*REFerence message IDentifier. REF_ID is used in TTCAN mode to detect a reference message. This holds for time slaves (reception) as well as for the time master (transmission). If the reference message is detected and there are no errors, then the Sync_Mark of this frame will become the Ref_Mark.
*/
#define REF_ID_Msk                                          (0x1FFFFFFF << REF_ID_Pos)
#define REF_ID_Pos                                          0
#define REF_ID_PosL                                         0
#define REF_ID_PosH                                         28


/*!< 0x0030 TTS_L 
*Transmission Time Stamp. TTS holds the time stamp of the last transmitted frame for CiA 603 time stamping. Every new frame overwrites TTS if TTSEN=1.
*/
#define TTS_Msk                                             (0XFFFFFFFF << TTS_Pos)
#define TTS_Pos                                             0
#define TTS_PosL                                            0
#define TTS_PosH                                            31


/*!< 0x0034 TTS_H 
*Transmission Time Stamp.  TTS holds the time stamp of the last transmitted frame for CiA 603 time stamping. Every new frame overwrites TTS if TTSEN=1.
*/
#define TTS_Msk                                             (0XFFFFFFFF << TTS_Pos)
#define TTS_Pos                                             0
#define TTS_PosL                                            0
#define TTS_PosH                                            31


/*!< 0x0038 VER 
*Version of CAN-CTRL, given as decimal value.VER_0 holds the minor version.VER_1 holds the major version.
*/
#define VER_Msk                                             (0xFF << VER_Pos)
#define VER_Pos                                             8
#define VER_PosL                                            8
#define VER_PosH                                            15


/*!< 0x0040 RBUF 
*When writing to this register, you must right justify the data.  RBUF[0] is for Identifier function. RBUF[1] is for Control, Status, TTCAN function. RBUF[2~17] is for Data function. RBUF[18~19] is for CiA 603 function.
*/
#define RECEIVEBUFFERREGISTER_Msk                           (0XFFFFFFFF << RECEIVEBUFFERREGISTER_Pos)
#define RECEIVEBUFFERREGISTER_Pos                           0
#define RECEIVEBUFFERREGISTER_PosL                          0
#define RECEIVEBUFFERREGISTER_PosH                          31


/*!< 0x0090 TBUF 
*When reading from these register, read data are automatically justified. TBUF[0] is for Identifier function. TBUF[1] is for Control, Status, TTCAN function. TBUF[2~17] is for Data function.
*/
#define TRANSMITBUFFERREGISTER_Msk                          (0XFFFFFFFF << TRANSMITBUFFERREGISTER_Pos)
#define TRANSMITBUFFERREGISTER_Pos                          0
#define TRANSMITBUFFERREGISTER_PosL                         0
#define TRANSMITBUFFERREGISTER_PosH                         31


/**
* @}
* End of group HW_CAN
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_CAN_H_ */
