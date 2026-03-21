/**
 ******************************************************************************
 * @file    hw_dma.h
 * @brief   dma hardware define.
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#ifndef _HW_DMA_H
#define _HW_DMA_H

 #ifdef __cplusplus
extern "C" {
#endif

/** @addtopgroup DMA
 * @{
 * */


/* Includes.
 * ----------------------------------------------------------------------------
 */
//#include "XXXX.h"

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
#define DMAC_MAX_CHANNEL_NUM                                 8U


/* Exported types.
 * ----------------------------------------------------------------------------
 */
/**<! Type definition for device register*/
typedef struct {
    __IOM uint32_t CTLL;              /* DMA conctrol Low register */
    __IOM uint32_t CTLH;              /* DMA conctrol High register */
    __IOM uint32_t CFGL;              /* DMA configuration Low register */
    __IOM uint32_t CFGH;              /* DMA configuration High register */
    __IOM uint32_t SAR;               /* DMA source address register */
    __IOM uint32_t DAR;               /* DMA destination address register */
    __IOM uint32_t LLP;               /* DMA linked list pointed register */
    __IOM uint32_t SSTAT;             /* DMA source status register */
    __IOM uint32_t DSTAT;             /* DMA destination status register */
    __IOM uint32_t SSTATAR;           /* DMA source status address register */
    __IOM uint32_t DSTATAR;           /* DMA destination status address register */
    __IOM uint32_t SGR;               /* DMA source gather register */
    __IOM uint32_t DSR;               /* DMA destination scatter register */
    __IOM uint32_t reserve1;          /* reserve */
} dma_channel_t;

typedef struct {
    dma_channel_t  CHANNEL[DMAC_MAX_CHANNEL_NUM]; /*Address offset: 0x00+0x38*channel_num*/
    /* int raw status */
    __IOM uint32_t RAWBLOCK;      /*0x01c0,Interrupt events are stored*/
    __IOM uint32_t RAWTFR;        /*0x01c4,Interrupt events are stored*/
    __IOM uint32_t RAWERR;        /*0x01c8,Interrupt events are stored*/
    /* int mask */
    __IOM uint32_t MASKBLOCK;     /*0x01cc,1、The contents of the Raw St*/
    __IOM uint32_t MASKTFR;       /*0x01d0,1、The contents of the Raw St*/
    __IOM uint32_t MASKERR;       /*0x01d4,1、The contents of the Raw St*/
    /* int clear */
    __OM  uint32_t CLEARBLOCK;    /*0x01d8,Each bit in the RawBlock and*/
    __OM  uint32_t CLEARTFR;      /*0x01dc,Each bit in the RawTfr and S*/
    __OM  uint32_t CLEARERR;      /*0x01e0,Each bit in the RawDstTran a*/
    __IM  uint32_t STATUSINT;     /*0x01e4,The contents of each of the */

    __IOM uint32_t DMAEN;         /* 0x01e8,This register is used to ena*/
    __IOM uint32_t CHEN;          /* 0x01ec,1、This is the DW_ahb_dmac Ch*/
    __IOM uint32_t DMAREMAPCTRL;  /* 0x01f0,This register is used to con*/
    __IOM uint32_t DMATEST;       /* 0x01f4,This register is used to put*/
    __IM  uint32_t LPTIME;        /* 0x01f8,This register holds the time*/
          uint32_t reserve1;
    __IOM uint32_t CH_REMAP_LOCK[DMAC_MAX_CHANNEL_NUM * 2];
          /* 0x0200+i*8,this register is 64bit,and [63:1] no define */
} dmac_dev_t;


/* Exported macro.
 * ----------------------------------------------------------------------------
 */
/*!< 0x0000 CTLx( 0<= x <= 7)
 * Block chaining is enabled on the source side only if the LLP_SRC_EN field is high and LLPx.
 * LOC is non-zero.
 * Exists: !DMAH_CHx_HC_LLP && DMAH_CHx_MULTI_BLK_EN
*/
#define LLP_SRC_EN_Msk                                      (0x01 << LLP_SRC_EN_Pos)
#define LLP_SRC_EN_Pos                                      28
#define LLP_SRC_EN_PosL                                     28
#define LLP_SRC_EN_PosH                                     28

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Block chaining is enabled on the destination side only if LLP_DST_EN field is high and LLPx.
*LOC is non-zero. 2、Exists: !DMAH_CHx_HC_LLP &&  DMAH_CHx_MULTI_BLK_EN (0 1)
*/
#define LLP_DST_EN_Msk                                      (0x01 << LLP_DST_EN_Pos)
#define LLP_DST_EN_Pos                                      27
#define LLP_DST_EN_PosL                                     27
#define LLP_DST_EN_PosH                                     27

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Source Master Select. Identifies the Master Interface layer
*where the source device (peripheral or memory) resides.
*The maximum value of this field that can be read back is 3.
*2、Exists: DMAH_CHx_SMS == 4
*/
#define SMS_Msk                                             (0x03 << SMS_Pos)
#define SMS_Pos                                             25
#define SMS_PosL                                            25
#define SMS_PosH                                            26

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Destination Master Select. Identifies the Master Interface layer
*where the destination device (peripheral or memory) resides.
*The maximum value of this field that can be read back is 3.
*2、Exists: DMAH_CHx_DMS == 4
*/
#define DMS_Msk                                             (0x03 << DMS_Pos)
#define DMS_Pos                                             23
#define DMS_PosL                                            23
#define DMS_PosH                                            24

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Transfer Type and Flow Control. Flow control can be assigned to the DW_ahb_dmac,
*the source peripheral, or the destination peripheral.
*2、If the configuration parameter DMAH_CHx_FC is set to DMA_FC_ONLY,
*then TT_FC[2] does not exist and TT_FC[2] always reads back 0.
*/
#define TT_FC_Msk                                           (0x07 << TT_FC_Pos)
#define TT_FC_Pos                                           20
#define TT_FC_PosL                                          20
#define TT_FC_PosH                                          22

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Destination scatter enable. Scatter on the destination side is applicable only
*when the CTLx.DINC bit indicates an incrementing or decrementing address control.
*2、Exists: DMAH_CHx_DST_SCA_EN(1)
*/
#define DST_SCATTER_EN_Msk                                  (0x01 << DST_SCATTER_EN_Pos)
#define DST_SCATTER_EN_Pos                                  18
#define DST_SCATTER_EN_PosL                                 18
#define DST_SCATTER_EN_PosH                                 18

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Source gather enable. Gather on the source side is applicable only when the CTLx.
*SINC bit indicates an incrementing or decrementing address control.
*2、Exists: DMAH_CHx_SRC_GAT_EN(1)
*/
#define SRC_GATHER_EN_Msk                                   (0x01 << SRC_GATHER_EN_Pos)
#define SRC_GATHER_EN_Pos                                   17
#define SRC_GATHER_EN_PosL                                  17
#define SRC_GATHER_EN_PosH                                  17

/*!< 0x0000 CTLx( 0<= x <= 7)
*Source Burst Transaction Length. Number of data items,
*each of width CTLx.SRC_TR_WIDTH, to be read from the source every time
*a burst transferred request is made from either the corresponding hardware
*or software handshaking interface.
*/
#define SRC_MSIZE_Msk                                       (0x07 << SRC_MSIZE_Pos)
#define SRC_MSIZE_Pos                                       14
#define SRC_MSIZE_PosL                                      14
#define SRC_MSIZE_PosH                                      16

/*!< 0x0000 CTLx( 0<= x <= 7)
*Destination Burst Transaction Length. Number of data items,
*each of width CTLx.DST_TR_WIDTH, to be written to the destination every time
*a destination burst transaction request is made from
*either the corresponding hardware or software handshaking interface.
*/
#define DEST_MSIZE_Msk                                      (0x07 << DEST_MSIZE_Pos)
#define DEST_MSIZE_Pos                                      11
#define DEST_MSIZE_PosL                                     11
#define DEST_MSIZE_PosH                                     13

/*!< 0x0000 CTLx( 0<= x <= 7)
*Source Address Increment. Indicates whether to increment or
*decrement the source address on every source transfer.
*If the device is fetching data from a source peripheral FIFO with a fixed address,
*then set this field to "No change".
*/
#define SINC_Msk                                            (0x03 << SINC_Pos)
#define SINC_Pos                                            9
#define SINC_PosL                                           9
#define SINC_PosH                                           10

/*!< 0x0000 CTLx( 0<= x <= 7)
*Destination Address Increment. Indicates whether to increment
*or decrement the destination address on every destination transfer.
*If your device is writing data to a destination peripheral FIFO with a fixed address,
*then set this field to \"No Change\".
*/
#define DINC_Msk                                            (0x03 << DINC_Pos)
#define DINC_Pos                                            7
#define DINC_PosL                                           7
#define DINC_PosH                                           8

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Source Transfer Width. Mapped to AHB bus hsize. For a non-memory peripheral,
*typically the peripheral (source) FIFO width.
*2、Exists: !DMAH_CHx_STW(0).
*/
#define SRC_TR_WIDTH_Msk                                    (0x07 << SRC_TR_WIDTH_Pos)
#define SRC_TR_WIDTH_Pos                                    4
#define SRC_TR_WIDTH_PosL                                   4
#define SRC_TR_WIDTH_PosH                                   6

/*!< 0x0000 CTLx( 0<= x <= 7)
*1、Destination Transfer Width. Mapped to AHB bus hsize. For a non-memory peripheral,
*typically the peripheral (destination) FIFO width.
*2、Exists: !DMAH_CHx_DTW(0).
*/
#define DST_TR_WIDTH_Msk                                    (0x07 << DST_TR_WIDTH_Pos)
#define DST_TR_WIDTH_Pos                                    1
#define DST_TR_WIDTH_PosL                                   1
#define DST_TR_WIDTH_PosH                                   3

/*!< 0x0000 CTLx( 0<= x <= 7)
*Interrupt Enable Bit. If set, then all interrupt-generating sources are enabled.
*Functions as a global mask bit for all interrupts for the channel;
*raw interrupt registers still assert if CTLx.INT_EN=0.
*/
#define INT_EN_Msk                                          (0x01 << INT_EN_Pos)
#define INT_EN_Pos                                          0
#define INT_EN_PosL                                         0
#define INT_EN_PosH                                         0

/*!< 0x0004 CTLx( 0<= x <= 7)
*Done bit. If status write-back is enabled, the upper word of the control register,
*CTLx[63:32], is written to the control register location of the Linked List Item (LLI)
*in system memory at the end of the block transfer with the done bit set.
*Software can poll the LLI CTLx.DONE bit to see when a block transfer is complete.
*The LLI CTLx.DONE bit should be cleared when the linked lists are set up in memory prior
*to enabling the channel. LLI accesses are always 32-bit accesses (Hsize=2) aligned
*to 32-bit boundaries and cannot be changed or programmed to anything other than 32-bit.
*/
#define DONE_Msk                                            (0x01 << DONE_Pos)
#define DONE_Pos                                            12
#define DONE_PosL                                           12
#define DONE_PosH                                           12

/*!< 0x0004 CTLx( 0<= x <= 7)
*Block Transfer Size. 1、When the DW_ahb_dmac is the flow controller,
*the user writes this field before the channel is enabled in order to indicate the block size.
*The number programmed into BLOCK_TS indicates the total number of single transactions
*to perform for every block transfer; a single transaction is mapped to a single AMBA beat.
*2、Range Variable[x]: DMAH_CH0_MAX_BLK_SIZE_INT + 31( DMAH_CH0_MAX_BLK_SIZE_INT 8)，Max:4095
*/
#define BLOCK_TS_Msk                                        (0xFFF << BLOCK_TS_Pos)
#define BLOCK_TS_Pos                                        0
#define BLOCK_TS_PosL                                       0
#define BLOCK_TS_PosH                                       11

/*!< 0x0008 CFGx( 0<= x <= 7)
*1、Automatic Destination Reload. The DARx register can be automatically
*reloaded from its initial value at the end of every block for multi-block transfers.
*A new block transfer is then initiated.
*2、Exists: DMAH_CHx_MULTI_BLK_EN(1)
*/
#define RELOAD_DST_Msk                                      (0x01 << RELOAD_DST_Pos)
#define RELOAD_DST_Pos                                      31
#define RELOAD_DST_PosL                                     31
#define RELOAD_DST_PosH                                     31

/*!< 0x0008 CFGx( 0<= x <= 7)
*1、The SARx register can be automatically reloaded from its initial value
*at the end of every block for multi-block transfers.
*A new block transfer is then initiated.
*2、Exists: DMAH_CHx_MULTI_BLK_EN(1)
*/
#define RELOAD_SRC_Msk                                      (0x01 << RELOAD_SRC_Pos)
#define RELOAD_SRC_Pos                                      30
#define RELOAD_SRC_PosL                                     30
#define RELOAD_SRC_PosH                                     30

/*!< 0x0008 CFGx( 0<= x <= 7)
*Source Handshaking Interface Polarity.
*/
#define SRC_HS_POL_Msk                                      (0x01 << SRC_HS_POL_Pos)
#define SRC_HS_POL_Pos                                      19
#define SRC_HS_POL_PosL                                     19
#define SRC_HS_POL_PosH                                     19

/*!< 0x0008 CFGx( 0<= x <= 7)
*Destination Handshaking Interface Polarity.
*/
#define DST_HS_POL_Msk                                      (0x01 << DST_HS_POL_Pos)
#define DST_HS_POL_Pos                                      18
#define DST_HS_POL_PosL                                     18
#define DST_HS_POL_PosH                                     18

/*!< 0x0008 CFGx( 0<= x <= 7)
*Source Software or Hardware Handshaking Select. This  register selects
*which of the handshaking interfaces -  hardware or software - is active for source
*requests on this channel. If the source peripheral is memory, then this bit is ignored.
*/
#define HS_SEL_SRC_Msk                                      (0x01 << HS_SEL_SRC_Pos)
#define HS_SEL_SRC_Pos                                      11
#define HS_SEL_SRC_PosL                                     11
#define HS_SEL_SRC_PosH                                     11

/*!< 0x0008 CFGx( 0<= x <= 7)
*Destination Software or Hardware Handshaking Select.
*This register selects which of the handshaking interfaces - hardware or software -
*is active for destination requests on this channel.
*If the destination peripheral is memory, then this bit is ignored.
*/
#define HS_SEL_DST_Msk                                      (0x01 << HS_SEL_DST_Pos)
#define HS_SEL_DST_Pos                                      10
#define HS_SEL_DST_PosL                                     10
#define HS_SEL_DST_PosH                                     10

/*!< 0x0008 CFGx( 0<= x <= 7)
*Channel FIFO status. Indicates if there is data left in the  channel FIFO.
*Can be used in conjunction with CFGx.CH_SUSP to cleanly disable a channel.
*/
#define FIFO_EMPTY_Msk                                      (0x01 << FIFO_EMPTY_Pos)
#define FIFO_EMPTY_Pos                                      9
#define FIFO_EMPTY_PosL                                     9
#define FIFO_EMPTY_PosH                                     9

/*!< 0x0008 CFGx( 0<= x <= 7)
*Channel Suspend. Suspends all DMA data transfers from  the source until this bit is cleared.
*There is no guarantee that the current transaction will complete.
*Can also be used in conjunction with CFGx.FIFO_EMPTY to cleanly disable a channel
*without losing any data.
*/
#define CH_SUSP_Msk                                         (0x01 << CH_SUSP_Pos)
#define CH_SUSP_Pos                                         8
#define CH_SUSP_PosL                                        8
#define CH_SUSP_PosH                                        8

/*!< 0x0008 CFGx( 0<= x <= 7)
*Channel Priority. A priority of 7 is the highest priority, and 0 is the lowest.
*This field must be programmed within the range 0 to 7.
*A programmed value outside this range will cause erroneous behavior.
*/
#define CH_PRIOR_Msk                                        (0x07 << CH_PRIOR_Pos)
#define CH_PRIOR_Pos                                        5
#define CH_PRIOR_PosL                                       5
#define CH_PRIOR_PosH                                       7

/*!< 0x000c CFGx( 0<= x <= 7)
*1、Assigns a hardware handshaking interface (0 : 15) to the destination of channel x
*if the CFGx.HS_SEL_DST field is 0; otherwise, this field is ignored.
*The channel can then communicate with the destination peripheral connected to
*that interface through the assigned hardware handshaking interface.
*2、Exists: DMAH_NUM_HS_INT > 0(16) 3、Range Variable[x]: LOG2_DMAH_NUM_HS_INT + 42
*/
#define DEST_PER_Msk                                        (0x0F << DEST_PER_Pos)
#define DEST_PER_Pos                                        11
#define DEST_PER_PosL                                       11
#define DEST_PER_PosH                                       14

/*!< 0x000c CFGx( 0<= x <= 7)
*1、Source Hardware Interface. Assigns a hardware handshaking interface (0:15)
*to the source of channel x if the CFGx.HS_SEL_SRC field is 0;
*otherwise, this field is ignored. The channel can then communicate
*with the source peripheral connected to that interface through the
*assigned hardware handshaking interface.
*2、Exists: DMAH_NUM_HS_INT > 0.
*3、Range Variable[x]: LOG2_DMAH_NUM_HS_INT + 38.
*/
#define SRC_PER_Msk                                         (0x0F << SRC_PER_Pos)
#define SRC_PER_Pos                                         7
#define SRC_PER_PosL                                        7
#define SRC_PER_PosH                                        10

/*!< 0x000c CFGx( 0<= x <= 7)
*1、Source Status Update Enable.
*Source status information is fetched only from the location pointed to by the SSTATARx register,
*stored in the SSTATx register and written out to the SSTATx location of the LLI,
*if SS_UPD_EN is high.
*2、Exists: DMAH_CHx_STAT_SRC(0).
*/
#define SS_UPD_EN_Msk                                       (0x01 << SS_UPD_EN_Pos)
#define SS_UPD_EN_Pos                                       6
#define SS_UPD_EN_PosL                                      6
#define SS_UPD_EN_PosH                                      6

/*!< 0x000c CFGx( 0<= x <= 7)
*1、Destination Status Update Enable. Destination status information
*is fetched only from the location pointed to by the DSTATARx register,
*stored in the DSTATx register and written out to the DSTATx location of the LLI,
*if DS_UPD_EN is high.  2、Exists: DMAH_CHx_STAT_DST(0).
*/
#define DS_UPD_EN_Msk                                       (0x01 << DS_UPD_EN_Pos)
#define DS_UPD_EN_Pos                                       5
#define DS_UPD_EN_PosL                                      5
#define DS_UPD_EN_PosH                                      5

/*!< 0x000c CFGx( 0<= x <= 7)
*1、Protection Control bits used to drive the AHB HPROT[3:1] bus.
*The AMBA Specification recommends that the default of HPROT indicates a non-cached,
*non-buffered, privileged data access. The reset value is used to indicate such an access.
*2、HPROT[0] is tied high because all transfers are data accesses,
*    as there are no opcode fetches.
*3、There is a one-to-one mapping of these register bits
*    to the HPROT[3:1] master interface signals.
*4、Mapping of HPROT bus is as follows:
*		(1) 1'b1 to HPROT[0];
*		(2) CFGx.PROTCTL[1] to HPROT[1];
*		(3) CFGx.PROTCTL[2] to HPROT[2];
*		(4) CFGx.PROTCTL[3] to HPROT[3].
*/
#define PROTCTL_Msk                                         (0x07 << PROTCTL_Pos)
#define PROTCTL_Pos                                         2
#define PROTCTL_PosL                                        2
#define PROTCTL_PosH                                        4

/*!< 0x000c CFGx( 0<= x <= 7)
*FIFO Mode Select. Determines how much space or data needs to be available
*in the FIFO before a burst transaction request is serviced.
*/
#define FIFOMODE_Msk                                       (0x01 << FIFOMODE_Pos)
#define FIFOMODE_Pos                                       1
#define FIFOMODE_PosL                                      1
#define FIFOMODE_PosH                                      1

/*!< 0x000c CFGx( 0<= x <= 7)
*Flow Control Mode. Determines when source transaction requests are serviced
*when the Destination Peripheral is the flow controller.
*/
#define FCMODE_Msk                                          (0x01 << FCMODE_Pos)
#define FCMODE_Pos                                          0
#define FCMODE_PosL                                         0
#define FCMODE_PosH                                         0


/*!< 0x0010 SARx( 0<= x <= 7)
*Current Source Address of DMA transfer. Updated after each source transfer.
*The SINC field in the CTLx register determines whether the address increments,
*decrements, or is left unchanged on every source transfer through the block transfer.
*/
#define SAR_Msk                                             (0XFFFFFFFF << SAR_Pos)
#define SAR_Pos                                             0
#define SAR_PosL                                            0
#define SAR_PosH                                            31


/*!< 0x0014 DARx( 0<= x <= 7)
*Current Destination address of DMA transfer. Updated after each destination transfer.
*The DINC field in the CTLx register determines whether the address increments, decrements,
*or is left unchanged on every destination transfer throughout the block transfer
*/
#define DAR_Msk                                             (0XFFFFFFFF << DAR_Pos)
#define DAR_Pos                                             0
#define DAR_PosL                                            0
#define DAR_PosH                                            31


/*!< 0x0018 LLPx( 0<= x <= 7)
*Starting Address In Memory of next LLI if block chaining is enabled.
*Note that the two LSBs of the starting address are not stored
*because the address is assumed to be aligned to a 32-bit boundary.
*LLI accesses are always 32-bit accesses (Hsize=2) aligned to 32-bit boundaries
*and cannot be changed or programmed to anything other than 32-bit.
*/
#define LOC_Msk                                             (0x3FFFFFFF << LOC_Pos)
#define LOC_Pos                                             2
#define LOC_PosL                                            2
#define LOC_PosH                                            31

/*!< 0x0018 LLPx( 0<= x <= 7)
*1、List Master Select. Identifies the AHB layer/interface where the memory device
*that stores the next linked list item resides. The maximum value of this field
*that can be read back is 3. 2、Exists: !DMAH_CHx_HC_LLP && DMAH_CHx_LMS == 4(1 4)
*/
#define LMS_Msk                                             (0x03 << LMS_Pos)
#define LMS_Pos                                             0
#define LMS_PosL                                            0
#define LMS_PosH                                            1

/*!< 0x001c SSTATx ( 0<= x <= 7)
*Source status information retrieved by hardware from the address pointed to
*by the contents of the STATARx register.
*/
#define SSTAT_Msk                                           (0XFFFFFFFF << SSTAT_Pos)
#define SSTAT_Pos                                           0
#define SSTAT_PosL                                          0
#define SSTAT_PosH                                          31

/*!< 0x0020 DSTATx ( 0<= x <= 7)
*Destination status information retrieved by hardware from the address pointed to
*by the contents of DSTATARx register.
*/
#define DSTAT_Msk                                           (0XFFFFFFFF << DSTAT_Pos)
#define DSTAT_Pos                                           0
#define DSTAT_PosL                                          0
#define DSTAT_PosH                                          31

/*!< 0x0024 SSTATARx ( 0<= x <= 7)
*Pointer from where hardware can fetch the source status information,
*which is registered in the SSTATx register and written out to the SSTATx register
*location of the LLI before the start of the next block.
*/
#define SSTATAR_Msk                                         (0XFFFFFFFF << SSTATAR_Pos)
#define SSTATAR_Pos                                         0
#define SSTATAR_PosL                                        0
#define SSTATAR_PosH                                        31

/*!< 0x0028 DSTATARx( 0<= x <= 7)
*Pointer from where hardware can fetch the destination status information,
*which is registered in the DSTATx register and written out to the DSTATx register
*location of the LLI before the start of the next block.
*/
#define DSTATAR_Msk                                         (0XFFFFFFFF << DSTATAR_Pos)
#define DSTATAR_Pos                                         0
#define DSTATAR_PosL                                        0
#define DSTATAR_PosH                                        31

/*!< 0x002c SGRx ( 0<= x <= 7)
*Source Gather Count. Source contiguous tranfer count between successive gather boundaries.
*/
#define SGC_Msk                                             (0xFF << SGC_Pos)
#define SGC_Pos                                             20
#define SGC_PosL                                            20
#define SGC_PosH                                            27

/*!< 0x002c SGRx ( 0<= x <= 7)
*Source Gather Interval.
*/
#define SGI_Msk                                             (0XFFFF << SGI_Pos)
#define SGI_Pos                                             0
#define SGI_PosL                                            0
#define SGI_PosH                                            19

/*!< 0x0030 DSRx( 0<= x <= 7)
*Destination Scatter Count. Destination contiguous transfer count
*between successive scatter boundaries.
*/
#define DSC_Msk                                             (0xFF << DSC_Pos)
#define DSC_Pos                                             20
#define DSC_PosL                                            20
#define DSC_PosH                                            27

/*!< 0x0030 DSRx( 0<= x <= 7)
*Destination Scatter Interval.
*/
#define DSI_Msk                                             (0XFFFF << DSI_Pos)
#define DSI_Pos                                             0
#define DSI_PosL                                            0
#define DSI_PosH                                            19

/*!< 0x01c0 RawBlock - RawErr
*Raw Status for IntBlock Interrupt.
*/
#define RAW_Msk                                             (0xFF << RAW_Pos)
#define RAW_Pos                                             0
#define RAW_PosL                                            0
#define RAW_PosH                                            7

/*!< 0x01cc MaskBlock - MaskErr
*Interrupt Mask Write Enable
*/
#define INT_MASK_WE_Msk                                     (0xFF << INT_MASK_WE_Pos)
#define INT_MASK_WE_Pos                                     8
#define INT_MASK_WE_PosL                                    8
#define INT_MASK_WE_PosH                                    15

/*!< 0x01cc MaskBlock
*Mask for IntBlock Interrupt
*/
#define INT_MASK_Msk                                        (0xFF << INT_MASK_Pos)
#define INT_MASK_Pos                                        0
#define INT_MASK_PosL                                       0
#define INT_MASK_PosH                                       7

/*!< 0x01d8 ClearBlock
*Clear for IntBlock Interrupt
*/
#define CLEAR_Msk                                           (0xFF << CLEAR_Pos)
#define CLEAR_Pos                                           0
#define CLEAR_PosL                                          0
#define CLEAR_PosH                                          7

/*!< 0x01e4 StatusInt
*OR of the contents of StatusErr.
*/
#define ERR_Msk                                             (0x01 << ERR_Pos)
#define ERR_Pos                                             4
#define ERR_PosL                                            4
#define ERR_PosH                                            4

/*!< 0x01e4 StatusInt
*OR of the contents of StatusBlock register
*/
#define BLOCK_Msk                                           (0x01 << BLOCK_Pos)
#define BLOCK_Pos                                           1
#define BLOCK_PosL                                          1
#define BLOCK_PosH                                          1

/*!< 0x01e4 StatusInt
*OR of the contents of StatusTfr register.
*/
#define TFR_CPLT_Msk                                        (0x01 << TFR_CPLT_Pos)
#define TFR_CPLT_Pos                                        0
#define TFR_CPLT_PosL                                       0
#define TFR_CPLT_PosH                                       0

/*!< 0x01e8 DmaEnReg
*DW_ahb_dmac Enable bit
*/
#define DMA_EN_Msk                                          (0x01 << DMA_EN_Pos)
#define DMA_EN_Pos                                          0
#define DMA_EN_PosL                                         0
#define DMA_EN_PosH                                         0

/*!< 0x01ec ChEnReg
*Channel enable register.
*/
#define CH_EN_WE_Msk                                        (0xFF << CH_EN_WE_Pos)
#define CH_EN_WE_Pos                                        8
#define CH_EN_WE_PosL                                       8
#define CH_EN_WE_PosH                                       15

/*!< 0x01ec ChEnReg
*Channel Enable. The ChEnReg.CH_EN bit is automatically cleared by hardware to disable the channel
*after the last AMBA transfer of the DMA transfer to the destination has completed.
*Software can therefore poll this bit to determine when this channel is free for a new DMA transfer.
*/
#define CH_EN_Msk                                           (0xFF << CH_EN_Pos)
#define CH_EN_Pos                                           0
#define CH_EN_PosL                                          0
#define CH_EN_PosH                                          7

/*!< 0x01f0 DmaRemapCtrl
*Bypass M2’s haddr remap function, high active.  force_dma2_remap_en is invalid
*when this register is asserted.
*/
#define BPS_DMA2_REMPA_EN_Msk                               (0x01 << BPS_DMA2_REMPA_EN_Pos)
#define BPS_DMA2_REMPA_EN_Pos                               21
#define BPS_DMA2_REMPA_EN_PosL                              21
#define BPS_DMA2_REMPA_EN_PosH                              21

/*!< 0x01f0 DmaRemapCtrl
*Bypass M1’s haddr remap function, high active.  force_dma1_remap_en is invalid
*when this register is asserted.
*/
#define BPS_DMA1_REMPA_EN_Msk                               (0x01 << BPS_DMA1_REMPA_EN_Pos)
#define BPS_DMA1_REMPA_EN_Pos                               20
#define BPS_DMA1_REMPA_EN_PosL                              20
#define BPS_DMA1_REMPA_EN_PosH                              20

/*!< 0x01f0 DmaRemapCtrl
*Valid only when force_dma2_remap_en is asserted.
*/
#define FORCE_DMA2_REMAP_F_Msk                              (0x01 << FORCE_DMA2_REMAP_F_Pos)
#define FORCE_DMA2_REMAP_F_Pos                              19
#define FORCE_DMA2_REMAP_F_PosL                             19
#define FORCE_DMA2_REMAP_F_PosH                             19

/*!< 0x01f0 DmaRemapCtrl
*Force master2(M2)’s haddr remap enable, only used when bps_dma2_remap_en is dis-asserted.
*The remap target is determined by force_dma1_remap_f when this register is asserted.
*/
#define FORCE_DMA2_REMAP_EN_Msk                             (0x01 << FORCE_DMA2_REMAP_EN_Pos)
#define FORCE_DMA2_REMAP_EN_Pos                             18
#define FORCE_DMA2_REMAP_EN_PosL                            18
#define FORCE_DMA2_REMAP_EN_PosH                            18

/*!< 0x01f0 DmaRemapCtrl
*Valid only when force_dma1_remap_en is asserted.
*/
#define FORCE_DMA1_REMAP_F_Msk                              (0x01 << FORCE_DMA1_REMAP_F_Pos)
#define FORCE_DMA1_REMAP_F_Pos                              17
#define FORCE_DMA1_REMAP_F_PosL                             17
#define FORCE_DMA1_REMAP_F_PosH                             17

/*!< 0x01f0 DmaRemapCtrl
*Force master1(M1)’s haddr remap enable, only used when bps_dma1_remap_en is dis-asserted.
*The remap target is determined by force_dma1_remap_f when this register is asserted.
*/
#define FORCE_DMA1_REMAP_EN_Msk                             (0x01 << FORCE_DMA1_REMAP_EN_Pos)
#define FORCE_DMA1_REMAP_EN_Pos                             16
#define FORCE_DMA1_REMAP_EN_PosL                            16
#define FORCE_DMA1_REMAP_EN_PosH                            16

/*!< 0x01f0 DmaRemapCtrl
*CH_CPU_SEL[i]
*/
#define CH_CPU_SEL_Msk                                      (0xFF << CH_CPU_SEL_Pos)
#define CH_CPU_SEL_Pos                                      0
#define CH_CPU_SEL_PosL                                     0
#define CH_CPU_SEL_PosH                                     7

/*!< 0x01f4 DmaTestReg
*DMA Test register
*/
#define TEST_SLV_IF_Msk                                     (0x01 << TEST_SLV_IF_Pos)
#define TEST_SLV_IF_Pos                                     0
#define TEST_SLV_IF_PosL                                    0
#define TEST_SLV_IF_PosH                                    0

/*!< 0x01f8 LpReg
*This field holds timeout value of low power counter register.
*/
#define DMA_LP_TIMEOUT_Msk                                  (0x0F << DMA_LP_TIMEOUT_Pos)
#define DMA_LP_TIMEOUT_Pos                                  0
#define DMA_LP_TIMEOUT_PosL                                 0
#define DMA_LP_TIMEOUT_PosH                                 3

/*!< 0x0200+i*8 CH_REMAP_LOCK[i]
*write 0 will set bit0 to 1; read will clear bit0; read value of bit0 is the value before clear.
*/
#define LOCK_CTRL_Msk                                       (0x01 << LOCK_CTRL_Pos)
#define LOCK_CTRL_Pos                                       0
#define LOCK_CTRL_PosL                                      0
#define LOCK_CTRL_PosH                                      0


/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */



/**
* @}
* End of group HW_DMA
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_DMA_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

