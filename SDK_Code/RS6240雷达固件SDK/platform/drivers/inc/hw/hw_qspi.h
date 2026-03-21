/****************************************************************
* @file  QSPI.h
* @brief QSPI hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_QSPI_H_
#define _HW_QSPI_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup QSPI
 * @{
 * */

/* Type definition for device register
 * ----------------------------------------------------------------------------
 * */
typedef struct {
    __IM  uint32_t VERSION_REG;                             /*0x0000,null                                               */
    __IOM uint32_t CTRL_REG;                                /*0x0004,null                                               */
    __IOM uint32_t CONFIG_REG;                              /*0x0008,null                                               */
    __IOM uint32_t CLOCK_CTRL_REG;                           /*0x000c,null                                               */
    __IOM uint32_t RECEIVE_DATA_NUM_REG;                    /*0x0010,null                                               */
          uint32_t RSVD;
    __IM  uint32_t FIFO_STATUS_REG;                         /*0x0018,null                                               */
    __IOM uint32_t TXF_CTL_REG;                             /*0x001c,null                                               */
    __IOM uint32_t RXF_CTL_REG;                             /*0x0020,null                                               */
    __IOM uint32_t INT_CS_REG;                              /*0x0024,null                                               */
    __IM  uint32_t SSR;                                     /*0x0028,null                                               */
    __IM  uint32_t INT_CLEAR_REG;                           /*0x002c,null                                               */
    __IOM uint32_t DMA_EN_REG;                              /*0x0030,null                                               */
    __IOM uint32_t DMATR_REG;                               /*0x0034,null                                               */
    __IOM uint32_t DMARR_REG;                               /*0x0038,null                                               */
    __IM  uint32_t PC_REG;                                  /*0x003c,null                                               */
          uint32_t DR;                                      /*0x0040,null                                               */
          uint32_t RSVD1[39];
    __IOM uint32_t SAMPLE_CTL_REG;                          /*0x00e0,null                                               */
    __IOM uint32_t QSPI_ENHANCE;                            /*0x00e4,null                                               */
    __IOM uint32_t SLV_TX_WAIT_CYCLES;                      /*0x00e8,null                                               */
} qspi_dev_t;

/*!< 0x0000 VERSION_REG
*null
*/
#define BIGVERSIONID_Msk                                    (0xFFFF << BIGVERSIONID_Pos)
#define BIGVERSIONID_Pos                                    16
#define BIGVERSIONID_PosL                                   16
#define BIGVERSIONID_PosH                                   31

/*!< 0x0000 VERSION_REG
*null
*/
#define LITTLEVERSIONID_Msk                                 (0xFFFF << LITTLEVERSIONID_Pos)
#define LITTLEVERSIONID_Pos                                 0
#define LITTLEVERSIONID_PosL                                0
#define LITTLEVERSIONID_PosH                                15

/*!< 0x0004 CTRL_REG
*null
*/
#define SSI_TYPE_Msk                                        (0x01 << SSI_TYPE_Pos)
#define SSI_TYPE_Pos                                        31
#define SSI_TYPE_PosL                                       31
#define SSI_TYPE_PosH                                       31

/*!< 0x0004 CTRL_REG
*null
*/
#define FULL_DUPLEX_EN_Msk                                  (0x01 << FULL_DUPLEX_EN_Pos)
#define FULL_DUPLEX_EN_Pos                                  30
#define FULL_DUPLEX_EN_PosL                                 30
#define FULL_DUPLEX_EN_PosH                                 30

/*!< 0x0004 CTRL_REG
*null
*/
#define SLV_BLOCK_SEL_Msk                                   (0x01 << SLV_BLOCK_SEL_Pos)
#define SLV_BLOCK_SEL_Pos                                   29
#define SLV_BLOCK_SEL_PosL                                  29
#define SLV_BLOCK_SEL_PosH                                  29

/*!< 0x0004 CTRL_REG
*当作为slave时，先配置为master模式，然后将该位配置为高；然后配置为slave模式，SLV_BLOCK_SEL配置为高；然后配置scph和scpol。
*/
#define FORCE_SLV_CLK_IN_Msk                                (0x01 << FORCE_SLV_CLK_IN_Pos)
#define FORCE_SLV_CLK_IN_Pos                                28
#define FORCE_SLV_CLK_IN_PosL                               28
#define FORCE_SLV_CLK_IN_PosH                               28

/*!< 0x0004 CTRL_REG
*选择对SS_N_IN进行去毛刺的方式。
*/
#define SS_N_IN_DEGLITCH_SEL_Msk                            (0x03 << SS_N_IN_DEGLITCH_SEL_Pos)
#define SS_N_IN_DEGLITCH_SEL_Pos                            24
#define SS_N_IN_DEGLITCH_SEL_PosL                           24
#define SS_N_IN_DEGLITCH_SEL_PosH                           26

/*!< 0x0004 CTRL_REG
*CS Enable. This register is valid only when SSI is configured as a master device. Each bit in this register corresponds to a CS line from the SSI master. When a bit in this register is set, the corresponding CS line from the master is activated when a serial transfer begins. It should be noted that setting or clearing bits in this register have no effect on the corresponding slave select outputs until a transfer is started. Before beginning a transfer, you should enable the bit in this register that corresponds to the slave device with which the master wants to communicate. When not operating in broadcast mode, only one bit in this field should be set.
*/
#define CSE_Msk                                             (0x01 << CSE_Pos)
#define CSE_Pos                                             20
#define CSE_PosL                                            20
#define CSE_PosH                                            20

/*!< 0x0004 CTRL_REG
*Slave Output Enable. Is valid only when the SSI is configured as slave. This bit enables or disables the setting of the ssi_oe_n output from the SSI serial slave. When SLV_OE=1, the ssi_oe_n output can never be active. When the ssi_oe_n output controls the tri-state buffer on the txd output form the slave, a high impedance state is always present on the slave txd output when SLV_OE=1. This is useful when the master transmits in broadcast mode(master transmits data to all slave devices). Only one slave may respond with data on the master rxd line. This bit is enabled after reset and must be disabled by software(when broadcast mode is used), if you do not want this device to respond with data. When SSIC_SLV_SPI_MODE is set to 1 and SPI is programmed to work in Enhanced SPI mode, then for correct operation this bit should be programmed to 0.
*/
#define SLV_OE_Msk                                          (0x01 << SLV_OE_Pos)
#define SLV_OE_Pos                                          17
#define SLV_OE_PosL                                         17
#define SLV_OE_PosH                                         17

/*!< 0x0004 CTRL_REG
*Slave CS Toggle Enable.  While operating in SPI mode with clock phase(SCPH) set to 0, this register controls the behavior of the CS line between data frames.
*/
#define SCSTE_Msk                                           (0x01 << SCSTE_Pos)
#define SCSTE_Pos                                           16
#define SCSTE_PosL                                          16
#define SCSTE_PosH                                          16

/*!< 0x0004 CTRL_REG
*Transfer Type. Config the mode of transfer for serial communication. This field does not affect the transfer duplicity. Only indicates whether the receive or transmit data are valid. 1) For tx-only type, data received from the external device is not valid and is not stored in the receive FIFO memory. It is overwritten on the next transfer. 2) For rx-only type, transmitted are not valid. After the first write to the transmit FIFO, the same word is retransmitted for the duration of the transfer. 3) For tx-and-rx type, both transmit and receive data are valid. The transfer continues until the transmit FIFO is empty. Data received from the external device are stored into the receive FIFO memory, where it can be accessed by the host processor.
*/
#define TTYP_Msk                                            (0x03 << TTYP_Pos)
#define TTYP_Pos                                            10
#define TTYP_PosL                                           10
#define TTYP_PosH                                           11

/*!< 0x0004 CTRL_REG
*Shift Register Loop Test.  Use for testing purposes only. When internally active, connects the transmit shift register output to receive shift register input. Can be used in both serial-slave and serial-master modes. When the SSI is configured as a slave in loopback mode, the CS and ssi_clk signals must be provided by an external source. In this mode, the slave cannot generate these signals because there is nothing to which to loop back.
*/
#define SRLT_Msk                                            (0x01 << SRLT_Pos)
#define SRLT_Pos                                            1
#define SRLT_PosL                                           1
#define SRLT_PosH                                           1

/*!< 0x0004 CTRL_REG
*null
*/
#define SSI_START_Msk                                       (0x01 << SSI_START_Pos)
#define SSI_START_Pos                                       0
#define SSI_START_PosL                                      0
#define SSI_START_PosH                                      0


/*!< 0x0008 CONFIG_REG
*Serial Clock Polarity.  Valid when COM_FRF is set to Motorola SPI.
*/
#define SCPOL_Msk                                           (0x01 << SCPOL_Pos)
#define SCPOL_Pos                                           31
#define SCPOL_PosL                                          31
#define SCPOL_PosH                                          31

/*!< 0x0008 CONFIG_REG
*Serial Clock Phase.  Valid when COM_FRF is set to Motorola SPI.
*/
#define SCPH_Msk                                            (0x01 << SCPH_Pos)
#define SCPH_Pos                                            30
#define SCPH_PosL                                           30
#define SCPH_PosH                                           30

/*!< 0x0008 CONFIG_REG
*Data Frame Width.  Config the data frame width. When the data width is programmed to be less than 32 bits, the receive data is automatically right-justified by the receive logic, with the upper bits of the receive FIFO zero-padded. You must right-justify transmit data before writing into the transmit FIFO. The transmit logic ignores the upper unused bits when transmitting the data. Note: when SPI MODE is set to “Dual”, ”Quad” or “Octal” mode: 1) DFW value must be a multiple of 2 if SPI MODE is “Dual” ; 2) DFW value must be a multiple of 4 if SPI MODE is “Quad”; 3) DFW value must be a multiple of 8 if SPI MODE is “Octal”.
*/
#define DFW_Msk                                             (0x1F << DFW_Pos)
#define DFW_Pos                                             16
#define DFW_PosL                                            16
#define DFW_PosH                                            20

/*!< 0x0008 CONFIG_REG
*Control Frame Length.  Configs the length of the control word for the Microwire frame format, when the COM_FRF is 2’b10.
*/
#define CFL_Msk                                             (0x0F << CFL_Pos)
#define CFL_Pos                                             4
#define CFL_PosL                                            4
#define CFL_PosH                                            7

/*!< 0x0008 CONFIG_REG
*Communication Frame Format.
*/
#define COM_FRF_Msk                                         (0x03 << COM_FRF_Pos)
#define COM_FRF_Pos                                         2
#define COM_FRF_PosL                                        2
#define COM_FRF_PosH                                        3

/*!< 0x0008 CONFIG_REG
*null
*/
#define SPIMODE_Msk                                         (0x03 << SPIMODE_Pos)
#define SPIMODE_Pos                                         0
#define SPIMODE_PosL                                        0
#define SPIMODE_PosH                                        1


/*!< 0x000c CLOCK_CTRL_REG
*Clock Divider Rate, Fsclk_out = Fssi_clk/ CDR. 1)The LSB of CDR(0 bit of CDR) is unaffected by a write operation to ensures an even value. 2) .If set to 0, sclk_out will be disabled.
*/
#define CDR_Msk                                             (0XFFFE << CDR_Pos)
#define CDR_Pos                                             0
#define CDR_PosL                                            0
#define CDR_PosH                                            15


/*!< 0x0010 RECEIVE_DATA_NUM_REG
*Receive data number.  When TTYP = 10 or TTYP = 11, this register field sets the number of data frames to be continuously received by the SSI. The SSI continues to receive serial data until the number of data frames received is equal to this register value plus 1. This register serves no purpose when SSI configured as slave because transfer continues for as long as the slave is selected.
*/
#define REC_DATA_NUM_Msk                                    (0xFFFF << REC_DATA_NUM_Pos)
#define REC_DATA_NUM_Pos                                    0
#define REC_DATA_NUM_PosL                                   0
#define REC_DATA_NUM_PosH                                   15


/*!< 0x0018 FIFO_STATUS_REG
*Receive FIFO Status. Contains the number of valid data entries in the receive FIFO.
*/
#define RXFS_Msk                                            (0x3F << RXFS_Pos)
#define RXFS_Pos                                            16
#define RXFS_PosL                                           16
#define RXFS_PosH                                           21

/*!< 0x0018 FIFO_STATUS_REG
*Transmit FIFO Status.  Contains the number of valid data entries in the transmit FIFO.
*/
#define TXFS_Msk                                            (0x3F << TXFS_Pos)
#define TXFS_Pos                                            0
#define TXFS_PosL                                           0
#define TXFS_PosH                                           5


/*!< 0x001c TXF_CTL_REG
*Transfer FIFO Start Level. Used to control the level of entries in transmit FIFO above which transfer will start on serial line. These field is valid only for Master mode of operation.
*/
#define TXFSL_Msk                                           (0x1F << TXFSL_Pos)
#define TXFSL_Pos                                           16
#define TXFSL_PosL                                          16
#define TXFSL_PosH                                          20

/*!< 0x001c TXF_CTL_REG
*Tx FIFO Interrupt Trigger Level.  Controls the level of entries (or below) at which the transmit FIFO controller triggers an interrupt.
*/
#define TXFITL_Msk                                          (0x1F << TXFITL_Pos)
#define TXFITL_Pos                                          0
#define TXFITL_PosL                                         0
#define TXFITL_PosH                                         4


/*!< 0x0020 RXF_CTL_REG
*RX FIFO Interrupt Trigger Level.  Controls the level of entries (or above) at which the receive FIFO controller triggers an interrupt.
*/
#define RXFITL_Msk                                          (0x1F << RXFITL_Pos)
#define RXFITL_Pos                                          0
#define RXFITL_PosL                                         0
#define RXFITL_PosH                                         4


/*!< 0x0024 RXF_CTL_REG
*Slave CS Posedge Interrupt Mask.
*/
#define SLV_CS_POS_INT_M_Msk                                (0x01 << SLV_CS_POS_INT_M_Pos)
#define SLV_CS_POS_INT_M_Pos                                23
#define SLV_CS_POS_INT_M_PosL                               23
#define SLV_CS_POS_INT_M_PosH                               23

/*!< 0x0024 RXF_CTL_REG
*Multi-Master Contention Interrupt Mask.
*/
#define MSTIM_Msk                                           (0x01 << MSTIM_Pos)
#define MSTIM_Pos                                           21
#define MSTIM_PosL                                          21
#define MSTIM_PosH                                          21

/*!< 0x0024 RXF_CTL_REG
*Receive FIFO Full Interrupt Mask
*/
#define RXFFIM_Msk                                          (0x01 << RXFFIM_Pos)
#define RXFFIM_Pos                                          20
#define RXFFIM_PosL                                         20
#define RXFFIM_PosH                                         20

/*!< 0x0024 RXF_CTL_REG
*Receive FIFO Overflow Interrupt Mask
*/
#define RXFOIM_Msk                                          (0x01 << RXFOIM_Pos)
#define RXFOIM_Pos                                          19
#define RXFOIM_PosL                                         19
#define RXFOIM_PosH                                         19

/*!< 0x0024 RXF_CTL_REG
*Receive FIFO Underflow Interrupt Mask.
*/
#define RXFUIM_Msk                                          (0x01 << RXFUIM_Pos)
#define RXFUIM_Pos                                          18
#define RXFUIM_PosL                                         18
#define RXFUIM_PosH                                         18

/*!< 0x0024 RXF_CTL_REG
*Transmit FIFO Overflow Interrupt Mask.
*/
#define TXFOIM_Msk                                          (0x01 << TXFOIM_Pos)
#define TXFOIM_Pos                                          17
#define TXFOIM_PosL                                         17
#define TXFOIM_PosH                                         17

/*!< 0x0024 RXF_CTL_REG
*Transmit FIFO Empty Interrupt Mask.
*/
#define TXFEIM_Msk                                          (0x01 << TXFEIM_Pos)
#define TXFEIM_Pos                                          16
#define TXFEIM_PosL                                         16
#define TXFEIM_PosH                                         16

/*!< 0x0024 RXF_CTL_REG
*Slave CS Posedge Interrupt Status.
*/
#define SLV_CS_POS_INT_Msk                                  (0x01 << SLV_CS_POS_INT_Pos)
#define SLV_CS_POS_INT_Pos                                  7
#define SLV_CS_POS_INT_PosL                                 7
#define SLV_CS_POS_INT_PosH                                 7

/*!< 0x0024 RXF_CTL_REG
*Multi-Master Contention Interrupt Status. The ssi_mst_intr interrupt is active if MSTIS and MSTIM are 1’b1.
*/
#define MSTIS_Msk                                           (0x01 << MSTIS_Pos)
#define MSTIS_Pos                                           5
#define MSTIS_PosL                                          5
#define MSTIS_PosH                                          5

/*!< 0x0024 RXF_CTL_REG
*Receive FIFO Full Interrupt Status
*/
#define RXFFIS_Msk                                          (0x01 << RXFFIS_Pos)
#define RXFFIS_Pos                                          4
#define RXFFIS_PosL                                         4
#define RXFFIS_PosH                                         4

/*!< 0x0024 RXF_CTL_REG
*Receive FIFO Overflow Interrupt Status. The ssi_rxo_intr interrupt is active if RXFOIS and RXFOIM are 1’b1.
*/
#define RXFOIS_Msk                                          (0x01 << RXFOIS_Pos)
#define RXFOIS_Pos                                          3
#define RXFOIS_PosL                                         3
#define RXFOIS_PosH                                         3

/*!< 0x0024 RXF_CTL_REG
*Receive FIFO Underflow Interrupt Status. The ssi_rxu_intr interrupt is active if RXFUIS and RXFUIM are 1’b1.
*/
#define RXFUIS_Msk                                          (0x01 << RXFUIS_Pos)
#define RXFUIS_Pos                                          2
#define RXFUIS_PosL                                         2
#define RXFUIS_PosH                                         2

/*!< 0x0024 RXF_CTL_REG
*Transmit FIFO Overflow Interrupt Status. The ssi_txo_intr interrupt is active if TXFOIS and TXFOIM are 1’b1.
*/
#define TXFOIS_Msk                                          (0x01 << TXFOIS_Pos)
#define TXFOIS_Pos                                          1
#define TXFOIS_PosL                                         1
#define TXFOIS_PosH                                         1

/*!< 0x0024 RXF_CTL_REG
*Transmit FIFO Empty Interrupt Status. The ssi_txe_intr interrupt is active if TXFEIS and TXFEIM are 1’b1.
*/
#define TXFEIS_Msk                                          (0x01 << TXFEIS_Pos)
#define TXFEIS_Pos                                          0
#define TXFEIS_PosL                                         0
#define TXFEIS_PosH                                         0


/*!< 0x0028 SSR
*Data Collision Error.  This bit will be set if CS input is asserted by other master, when the SSI master is in the middle of the transfer. This bit is cleared when read.
*/
#define DCE_Msk                                             (0x01 << DCE_Pos)
#define DCE_Pos                                             6
#define DCE_PosL                                            6
#define DCE_PosH                                            6

/*!< 0x0028 SSR
*Transmission Error.  Set if the transmit FIFO is empty when a transfer is started. Set only when configured as slave This bit is cleared when read.
*/
#define TXE_Msk                                            (0x01 << TXE_Pos)
#define TXE_Pos                                            5
#define TXE_PosL                                           5
#define TXE_PosH                                           5

/*!< 0x0028 SSR
*null
*/
#define RFF_Msk                                             (0x01 << RFF_Pos)
#define RFF_Pos                                             4
#define RFF_PosL                                            4
#define RFF_PosH                                            4

/*!< 0x0028 SSR
*null
*/
#define RFNE_Msk                                            (0x01 << RFNE_Pos)
#define RFNE_Pos                                            3
#define RFNE_PosL                                           3
#define RFNE_PosH                                           3

/*!< 0x0028 SSR
*null
*/
#define TFE_Msk                                             (0x01 << TFE_Pos)
#define TFE_Pos                                             2
#define TFE_PosL                                            2
#define TFE_PosH                                            2

/*!< 0x0028 SSR
*null
*/
#define TFNF_Msk                                            (0x01 << TFNF_Pos)
#define TFNF_Pos                                            1
#define TFNF_PosL                                           1
#define TFNF_PosH                                           1

/*!< 0x0028 SSR
*null
*/
#define BUSY_Msk                                            (0x01 << BUSY_Pos)
#define BUSY_Pos                                            0
#define BUSY_PosL                                           0
#define BUSY_PosH                                           0


/*!< 0x002c RXF_CTL_REG
*Clear Interrupts.  This register is set if any of the interrupts below are active. A read clears the ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and the ssi_mst_intr interrupts.
*/
#define CI_Msk                                              (0x01 << CI_Pos)
#define CI_Pos                                              31
#define CI_PosL                                             31
#define CI_PosH                                             31

/*!< 0x002c RXF_CTL_REG
*Clear Multi-Master Contention Interrupt. This register reflects the status of the interrupt. A read from this register clears the ssi_mst_intr interrupt.
*/
#define MSTICR_Msk                                          (0x01 << MSTICR_Pos)
#define MSTICR_Pos                                          24
#define MSTICR_PosL                                         24
#define MSTICR_PosH                                         24

/*!< 0x002c RXF_CTL_REG
*Clear Slave CS Posedge Interrupt.  This register reflects the status of the interrupt. A read from this register clears the ssi_rxu_intr interrupt.
*/
#define SLV_CS_POS_INT_C_Msk                                (0x01 << SLV_CS_POS_INT_C_Pos)
#define SLV_CS_POS_INT_C_Pos                                17
#define SLV_CS_POS_INT_C_PosL                               17
#define SLV_CS_POS_INT_C_PosH                               17

/*!< 0x002c RXF_CTL_REG
*Clear Receive FIFO Underflow Interrupt. This register reflects the status of the interrupt. A read from this register clears the ssi_rxu_intr interrupt.
*/
#define RXUICR_Msk                                          (0x01 << RXUICR_Pos)
#define RXUICR_Pos                                          16
#define RXUICR_PosL                                         16
#define RXUICR_PosH                                         16

/*!< 0x002c RXF_CTL_REG
*Clear Receive FIFO Overflow Interrupt. This register reflects the status of the interrupt. A read from this register clears the ssi_rxo_intr interrupt.
*/
#define RXOICR_Msk                                          (0x01 << RXOICR_Pos)
#define RXOICR_Pos                                          8
#define RXOICR_PosL                                         8
#define RXOICR_PosH                                         8

/*!< 0x002c RXF_CTL_REG
*Clear Transmit FIFO Overflow Interrupt. This register reflects the status of the interrupt. A read from this register clears the ssi_txo_intr interrupt.
*/
#define TXOICR_Msk                                          (0x01 << TXOICR_Pos)
#define TXOICR_Pos                                          0
#define TXOICR_PosL                                         0
#define TXOICR_PosH                                         0


/*!< 0x0030 DMA_EN_REG
*DMA Transmit Enable. This bit enable/disable the DMA for transmit FIFO channel.
*/
#define DMATE_Msk                                           (0x01 << DMATE_Pos)
#define DMATE_Pos                                           1
#define DMATE_PosL                                          1
#define DMATE_PosH                                          1

/*!< 0x0030 DMA_EN_REG
*DMA Receive Enable. This bit enable/disable the DMA for receive FIFO channel.
*/
#define DMARE_Msk                                           (0x01 << DMARE_Pos)
#define DMARE_Pos                                           0
#define DMARE_PosL                                          0
#define DMARE_PosH                                          0


/*!< 0x0034 DMATR_REG
*DMA Transmit Request Level. The dma_tx_req signal is generated when the number of valid data entries in the transmit FIFO is equal to or below this field value, and DMATE = 1.
*/
#define DMATRL_Msk                                          (0x1F << DMATRL_Pos)
#define DMATRL_Pos                                          0
#define DMATRL_PosL                                         0
#define DMATRL_PosH                                         4


/*!< 0x0038 DMARR_REG
*DMA Receive Request Level.  The dma_rx_req is generated when the number of valid data entries in the receive FIFO is equal to or above this field value + 1, and DMARE = 1.
*/
#define DMARRL_Msk                                          (0x1F << DMARRL_Pos)
#define DMARRL_Pos                                          0
#define DMARRL_PosL                                         0
#define DMARRL_PosH                                         4


/*!< 0x003c PC_REG
*The register contains the peripheral code, which is fixed into the register when designed the controller.
*/
#define PERIPHERALCODE_Msk                                  (0XFFFFFFFF << PERIPHERALCODE_Pos)
#define PERIPHERALCODE_Pos                                  0
#define PERIPHERALCODE_PosL                                 0
#define PERIPHERALCODE_PosH                                 31


/*!< 0x0040~0x00c4 DBx_REG(for i = 0; i <= 35)
*Data Buffer Register. Data Buffer Register is a 32-bit read/write buffer for the transmit/receive FIFOs. When the register is read, data in the receive FIFO buffer is accessed. When it is written to, data are moved into the transmit FIFO buffer.
*/
#define DBR_Msk                                             (0XFFFFFFFF << DBR_Pos)
#define DBR_Pos                                             0
#define DBR_PosL                                            0
#define DBR_PosH                                            31


/*!< 0x00e0 RXF_CTL_REG
*Receive Data Sampling Edge.
*/
#define RSE_Msk                                             (0x01 << RSE_Pos)
#define RSE_Pos                                             16
#define RSE_PosL                                            16
#define RSE_PosH                                            16

/*!< 0x00e0 RXF_CTL_REG
*Receive Data Sample Delay.  This register is used to delay the sample of the receive data. Each value represents a single ssi_clk delay on the sample of input data port.
*/
#define RSD_Msk                                             (0xFF << RSD_Pos)
#define RSD_Pos                                             0
#define RSD_PosL                                            0
#define RSD_PosH                                            7


/*!< 0x00e4 RXF_CTL_REG
*Enables clock stretching capability in SPI transfers. In case of write, if the FIFO becomes empty SSI will stretch the clock until FIFO has enough data to continue the transfer. In case of read, if the receive FIFO becomes full, SSI will stop the clock until data has been read from the FIFO.
*/
#define CLK_STRETCH_EN_Msk                                  (0x01 << CLK_STRETCH_EN_Pos)
#define CLK_STRETCH_EN_Pos                                  28
#define CLK_STRETCH_EN_PosL                                 28
#define CLK_STRETCH_EN_PosH                                 28

/*!< 0x00e4 RXF_CTL_REG
*null
*/
#define TRANS_FORM_Msk                                      (0x03 << TRANS_FORM_Pos)
#define TRANS_FORM_Pos                                      14
#define TRANS_FORM_PosL                                     14
#define TRANS_FORM_PosH                                     15

/*!< 0x00e4 RXF_CTL_REG
*Dummy cycles in Dual/Quad/Octal mode between control frames transmit and data reception. Specified as number of SPI clock cycles.
*/
#define DUMMY_Msk                                           (0x1F << DUMMY_Pos)
#define DUMMY_Pos                                           8
#define DUMMY_PosL                                          8
#define DUMMY_PosH                                          12

/*!< 0x00e4 RXF_CTL_REG
*This register defines the BIT length of the Address to be transmitted. Only after this much bits are programmed in to the FIFO the transfer can begin.
*/
#define ADDR_B_Msk                                          (0x0F << ADDR_B_Pos)
#define ADDR_B_Pos                                          2
#define ADDR_B_PosL                                         2
#define ADDR_B_PosH                                         5

/*!< 0x00e4 RXF_CTL_REG
*Dual/Quad/Octal mode instruction BIT length.
*/
#define INST_B_Msk                                          (0x03 << INST_B_Pos)
#define INST_B_Pos                                          0
#define INST_B_PosL                                         0
#define INST_B_PosH                                         1


/*!< 0x00e8 SLV_TX_WAIT_CYCLES
*The edge number of sclk_in’s data send edge. After TX_WAIT_CYCLES’s sclk_in data send edge, TX must be send.
*/
#define TX_WAIT_CYCLES_Msk                                  (0x1FFFF << TX_WAIT_CYCLES_Pos)
#define TX_WAIT_CYCLES_Pos                                  0
#define TX_WAIT_CYCLES_PosL                                 0
#define TX_WAIT_CYCLES_PosH                                 16


/**
* @}
* End of group HW_QSPI
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_QSPI_H_ */