/****************************************************************
* @file  iic.h
* @brief I2C hadware define
* @attention
*          Copyright (c) 2025 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_IIC_H_
#define _HW_IIC_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup IIC
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __IOM uint32_t CR;                                      /*0x0000,I2C Control Register         */
    __IOM uint32_t ENABLE;                                  /*0x0004,I2C ENABLE Register          */
    __IOM uint32_t SS_SCL_CNT_SET;                          /*0x0008,Standard Speed I2C Clock SCL */
    __IOM uint32_t FS_SCL_CNT_SET;                          /*0x000c,Fast Mode or Fast Mode Plus  */
    __IOM uint32_t HS_SCL_CNT_SET;                          /*0x0010,High Speed I2C Clock SCL Cou */
    __IOM uint32_t IC_FS_SPKLEN;                            /*0x0014,I2C SS, FS or FM+ spike supp */
    __IOM uint32_t SDA_HOLD_SETUP;                          /*0x0018,I2C SDA Hold Time ,Setup tim */
    __IOM uint32_t DMA_CR;                                  /*0x001c,DMA Control Register         */
    __IOM uint32_t TL_SET;                                  /*0x0020,I2C FIFO Threshold Set Regis */
    __OM  uint32_t ICR;                                     /*0x0024,Clear Combined and Individua */
    __IOM uint32_t IMR;                                     /*0x0028,I2C Interrupt Mask Register  */
    __IM  uint32_t ISR;                                     /*0x002c,I2C Interrupt Status Registe */
    __IM  uint32_t TX_ABRT_SOURCE;                          /*0x0030,I2C Transmit Abort Source Re */
    __IM  uint32_t SR;                                      /*0x0034,I2C STATUS Register          */
    __IOM uint32_t TAR;                                     /*0x0038,I2C Target Address Register  */
    __IOM uint32_t SAR;                                     /*0x003c,I2C Slave Address Register   */
    __IOM uint32_t HS_MADDR;                                /*0x0040,I2C High Speed Master Mode C */
    __OM  uint32_t DATA_CMD;                                /*0x0044,I2C Rx/Tx Data Buffer and Co */
    __IOM uint32_t SCL_STUCK_AT_LOW_TIMEOUT;                /*0x0048,I2C SCL Stuck at Low Timeout */
    __IOM uint32_t SDA_STUCK_AT_LOW_TIMEOUT;                /*0x004c,I2C SDA Stuck at Low Timeout */
    __IM  uint32_t OE_RD;                                   /*0x0050,I2C SCL and SDA output signa */
    __IOM uint32_t SLV_BCNT_CLR;                            /*0x0054,IIC clear bit count error st */
    __IOM uint32_t SLV_BCNT_TOUT;                           /*0x0058,slave bit count timeout      */
} i2c_dev_t;

#define I2C0_DEV                        ((i2c_dev_t *)    I2C0_BASE_ADDR)   /*!< 0x4000 4000 */
#define I2C1_DEV                        ((i2c_dev_t *)    I2C1_BASE_ADDR)   /*!< 0x4000 4400 */

/*!< 0x0000 CR
*1、ACK General Call. (1) When set to 1, DW_apb_i2c responds with a ACK (by asserting ic_data_oe
) when it receives a General Call. (2) Otherwise, DW_apb_i2c responds with a NACK (by negating
ic_data_oe).  2、The register controls whether DW_apb_i2c responds with a ACK or NACK when it
receives an I2C General Call address.  3、This register is applicable only when the DW_apb_i2c
is in slave mode.  4、Value After Reset: IC_DEFAULT_ACK_GENERAL_CALL
*/
#define ACK_GEN_CALL_Msk                                    (0x01 << ACK_GEN_CALL_Pos)
#define ACK_GEN_CALL_Pos                                    13
#define ACK_GEN_CALL_PosL                                   13
#define ACK_GEN_CALL_PosH                                   13

/*!< 0x0000 CR
*1、Generate NACK. This NACK generation only occurs when DW_apb_i2c is a slave-receiver.  2
、If this register is set to a value of 1, it can only generate a NACK after a data byte is
received; hence, the data transfer is aborted and the data received is not pushed to the receive
buffer.  3、When the register is set to a value of 0, it generates NACK/ACK, depending on normal
criteria. (1) 1: generate NACK after data byte received; (2) 0: generate NACK/ACK normally.
*/
#define NACK_Msk                                            (0x01 << NACK_Pos)
#define NACK_Pos                                            12
#define NACK_PosL                                           12
#define NACK_PosH                                           12

/*!< 0x0000 CR
*1、In Slave mode, this register bit is not applicable. 2、Exists: IC_BUS_CLEAR_FEATURE==1(0).
*/
#define BUS_CLEAR_FEATURE_CTRL_Msk                          (0x01 << BUS_CLEAR_FEATURE_CTRL_Pos)
#define BUS_CLEAR_FEATURE_CTRL_Pos                          11
#define BUS_CLEAR_FEATURE_CTRL_PosL                         11
#define BUS_CLEAR_FEATURE_CTRL_PosH                         11

/*!< 0x0000 CR
*1、In Master mode, (1) 1'b1: issues the STOP_DET interrupt only when master is active. (2) 1'b0
: issues the STOP_DET irrespective of whether master is active or not. 2、Memory Access: "(
IC_STOP_DET_IF_MASTER_ACTIVE==1) ? \"read-write\" : \"read-only\""
*/
#define STOP_DET_IF_MASTER_ACTIVE_Msk                       (0x01 << STOP_DET_IF_MASTER_ACTIVE_Pos)
#define STOP_DET_IF_MASTER_ACTIVE_Pos                       10
#define STOP_DET_IF_MASTER_ACTIVE_PosL                      10
#define STOP_DET_IF_MASTER_ACTIVE_PosH                      10

/*!< 0x0000 CR
*1、This bit controls whether DW_apb_i2c should hold the bus when the Rx FIFO is physically full
to its RX_BUFFER_DEPTH, as described in the IC_RX_FULL_HLD_BUS_EN parameter. 2、Memory Access: "(
IC_RX_FULL_HLD_BUS_EN==1) ? \"read-write\" : \"read-only\"".
*/
#define RX_FIFO_FULL_HLD_CTRL_Msk                           (0x01 << RX_FIFO_FULL_HLD_CTRL_Pos)
#define RX_FIFO_FULL_HLD_CTRL_Pos                           9
#define RX_FIFO_FULL_HLD_CTRL_PosL                          9
#define RX_FIFO_FULL_HLD_CTRL_PosH                          9

/*!< 0x0000 CR
*This bit controls the generation of the TX_EMPTY interrupt, as described in the ISR register.
*/
#define TX_EMPTY_CTRL_Msk                                   (0x01 << TX_EMPTY_CTRL_Pos)
#define TX_EMPTY_CTRL_Pos                                   8
#define TX_EMPTY_CTRL_PosL                                  8
#define TX_EMPTY_CTRL_PosH                                  8

/*!< 0x0000 CR
*During a general call address, this slave does not issue the STOP_DET interrupt if
STOP_DET_IF_ADDRESSED = 1'b1, even if the slave  responds to the general call address by
generating ACK. The STOP_DET interrupt is generated only when the transmitted address matches the
slave address (SAR).
*/
#define STOP_DET_IFADDRESSED_Msk                            (0x01 << STOP_DET_IFADDRESSED_Pos)
#define STOP_DET_IFADDRESSED_Pos                            7
#define STOP_DET_IFADDRESSED_PosL                           7
#define STOP_DET_IFADDRESSED_PosH                           7

/*!< 0x0000 CR
*1、If this bit is set (slave is disabled), DW_apb_i2c functions only as a master and does not
perform any action that requires a slave. 2、Value After Reset: IC_SLAVE_DISABLE
*/
#define IC_SLAVE_DISABLE_Msk                                (0x01 << IC_SLAVE_DISABLE_Pos)
#define IC_SLAVE_DISABLE_Pos                                6
#define IC_SLAVE_DISABLE_PosL                               6
#define IC_SLAVE_DISABLE_PosH                               6

/*!< 0x0000 CR
*1、Determines whether RESTART conditions may be sent when acting as a master. Some older slaves
do not support handling RESTART conditions; however, RESTART conditions are used in several
DW_apb_i2c operations.  2、When RESTART is disabled, the master is prohibited from performing
the following functions:(1) Sending a START BYTE; (2) Performing any high-speed mode operation;
(3) High-speed mode operation;(4) Performing direction changes in combined format mode; (5)
Performing a read operation with a 10-bit address.  3、By replacing RESTART condition followed
by a STOP and a subsequent START condition, split operations are broken down into multiple
DW_apb_i2c transfers. If the above operations are performed, it will result in setting bit 6 (
TX_ABRT) of the ISR register.
4、Value After Reset: IC_RESTART_EN.
*/
#define IC_RESTART_EN_Msk                                   (0x01 << IC_RESTART_EN_Pos)
#define IC_RESTART_EN_Pos                                   5
#define IC_RESTART_EN_PosL                                  5
#define IC_RESTART_EN_PosH                                  5

/*!< 0x0000 CR
*1、If the I2C_DYNAMIC_TAR_UPDATE configuration parameter is set to 'No' (0), this bit is named
IC_10BITADDR_MASTER and controls whether the DW_apb_i2c starts its transfers in 7- or 10-bit
addressing mode when acting as a master.  2、Value After Reset: IC_10BITADDR_MASTER.  3、Exists
: I2C_DYNAMIC_TAR_UPDATE == 0(0).
*/
#define IC_10BITADDR_MASTER_Msk                             (0x01 << IC_10BITADDR_MASTER_Pos)
#define IC_10BITADDR_MASTER_Pos                             4
#define IC_10BITADDR_MASTER_PosL                            4
#define IC_10BITADDR_MASTER_PosH                            4

/*!< 0x0000 CR
*1、7-bit addressing. The DW_apb_i2c ignores  transactions that involve 10-bit addressing; for 7-
bit  addressing, only the lower 7 bits of the IC_SAR register are compared.;2、10-bit addressing
. The DW_apb_i2c responds to only 10-bit addressing transfers that match the full 10 bits of the
IC_SAR register. 3、Value After Reset: IC_10BITADDR_SLAVE.
*/
#define IC_10BITADDR_SLAVE_Msk                              (0x01 << IC_10BITADDR_SLAVE_Pos)
#define IC_10BITADDR_SLAVE_Pos                              3
#define IC_10BITADDR_SLAVE_PosL                             3
#define IC_10BITADDR_SLAVE_PosH                             3

/*!< 0x0000 CR
*1、These bits control at which speed the DW_apb_i2c operates; its setting is relevant only if
one is operating the DW_apb_i2c in master mode. Hardware protects against illegal values being
programmed by software. These bits must be programmed appropriately for slave mode also, as it is
used to capture correct value of spike filter as per the speed mode.  2、This register should be
programmed only with a value in the range of 1 to IC_MAX_SPEED_MODE; otherwise, hardware updates
this register with the value of IC_MAX_SPEED_MODE. (1) standard mode (100 kbit/s); (2) fast mode (
<=400 kbit/s) or fast mode plus (<=1000Kbit/s); (3) high speed mode (3.4 Mbit/s);3、Value After
Reset: IC_MAX_SPEED_MODE
*/
#define SPEED_Msk                                           (0x03 << SPEED_Pos)
#define SPEED_Pos                                           1
#define SPEED_PosL                                          1
#define SPEED_PosH                                          2

/*!< 0x0000 CR
*1、NOTE: Software should ensure that if this bit is written with '1' then bit 6 should also be
written with a '1'. 2、Value After Reset: IC_MASTER_MODE.
*/
#define MASTER_MODE_Msk                                     (0x01 << MASTER_MODE_Pos)
#define MASTER_MODE_Pos                                     0
#define MASTER_MODE_PosL                                    0
#define MASTER_MODE_PosH                                    0

#define MASTER_ENABLE_MASK                                  (MASTER_MODE_Msk | IC_SLAVE_DISABLE_Msk)

/*!< 0x0004 ENABLE
*1、If SDA is stuck at low indicated through the TX_ABORT interrupt (IC_TX_ABRT_SOURCE[17]),
then this bit is used as a control knob to initiate the SDA Recovery Mechanism (that is, send at
most 9 SCL clocks and STOP to release the SDA line) and then this bit gets auto clear.  2、Exists
: IC_BUS_CLEAR_FEATURE==1(0).
*/
#define SDA_STUCK_RECOVERY_ENABLE_Msk                       (0x01 << SDA_STUCK_RECOVERY_ENABLE_Pos)
#define SDA_STUCK_RECOVERY_ENABLE_Pos                       3
#define SDA_STUCK_RECOVERY_ENABLE_PosL                      3
#define SDA_STUCK_RECOVERY_ENABLE_PosH                      3

/*!< 0x0004 ENABLE
*1、In Master mode: (1) 1'b1: Blocks the transmission of data on I2C bus even if Tx FIFO has
data to transmit. (2) 1'b0: The transmission of data starts on I2C bus automatically, as soon as
the first data is available in the Tx FIFO.  2、Note: To block the execution of Master commands,
set the TX_CMD_BLOCK bit only when Tx FIFO is empty (IC_STATUS[2]==1) and Master is in Idle state
(IC_STATUS[5] == 0). Any further commands put in the Tx FIFO are not executed until TX_CMD_BLOCK
bit is unset 3、Value After Reset: IC_TX_CMD_BLOCK_DEFAULT(0)
*/
#define TX_CMD_BLOCK_Msk                                    (0x01 << TX_CMD_BLOCK_Pos)
#define TX_CMD_BLOCK_Pos                                    2
#define TX_CMD_BLOCK_PosL                                   2
#define TX_CMD_BLOCK_PosH                                   2

/*!< 0x0004 ENABLE
*1、When set, the controller initiates the transfer abort. (1) 0: ABORT not initiated or ABORT
done. (2) 1: ABORT operation in progress 2、The software can abort the I2C transfer in master
mode by setting this bit. The software can set this bit only when ENABLE is already set; otherwise
, the controller ignores any write to ABORT bit. The software cannot clear the ABORT bit once set
. In response to an ABORT, the controller issues a STOP and flushes the Tx FIFO after completing
the current transfer, then sets the TX_ABORT interrupt after the abort operation. The ABORT bit
is cleared automatically after the abort operation.  3、For a detailed description on how to
abort I2C transfers, refer to "Aborting I2C Transfers".
*/
#define ABORT_Msk                                           (0x01 << ABORT_Pos)
#define ABORT_Pos                                           1
#define ABORT_PosL                                          1
#define ABORT_PosH                                          1

/*!< 0x0004 ENABLE
*1、Controls whether the DW_apb_i2c is enabled. (0) 0: Disables DW_apb_i2c (TX and RX FIFOs are
held in an erased state); (1) 1: Enables DW_apb_i2cSoftware can disable DW_apb_i2c while it is
active. However, it is important that care be taken to ensure that DW_apb_i2c is disabled properly
. A recommended  procedure is described in "Disabling DW_apb_i2c". 2、When DW_apb_i2c is disabled
, the following occurs: (1) The TX FIFO and RX FIFO get flushed. (2) Status bits in the
IC_INTR_STAT register are still active until DW_apb_i2c goes into IDLE state. 3、If the module
is transmitting, it stops as well as deletes the contents of the transmit buffer after the
current transfer is complete. If the module is receiving, the DW_apb_i2c stops the current
transfer at the end of the current byte and does not acknowledge the transfer. 4、In systems
with asynchronous pclk and ic_clk when IC_CLK_TYPE parameter set to asynchronous (1), there is a
two ic_clk delay when enabling or disabling the DW_apb_i2c. (IC_CLK_TYPE parameter = 1).
*/
#define ENABLE_Msk                                          (0x01 << ENABLE_Pos)
#define ENABLE_Pos                                          0
#define ENABLE_PosL                                         0
#define ENABLE_PosH                                         0


/*!< 0x0008 SS_SCL_CNT_SET
*1、This register must be set before any I2C bus transaction can take place to ensure proper I/O
timing. This register sets the SCL clock high-period count for standard speed. For more
information, refer to "IC_CLK Frequency Configuration". 2、This register can be written only
when the I2C interface is disabled which corresponds to the IC_ENABLE[0] register being set to 0.
Writes at other times have no effect. 3、The minimum valid value is 6; hardware prevents values
less than this being written, and if attempted results in 6 being set.  4、NOTE: This register
must not be programmed to a value higher than 65525, because DW_apb_i2c uses a 16-bit counter to
flag an I2C bus idle condition when this counter reaches a value of IC_SS_SCL_HCNT + 10.
5、Value After Reset: IC_SS_SCL_HIGH_COUNT(16’h0190)
*/
#define IC_SS_SCL_HCNT_Msk                                  (0xFFFF << IC_SS_SCL_HCNT_Pos)
#define IC_SS_SCL_HCNT_Pos                                  16
#define IC_SS_SCL_HCNT_PosL                                 16
#define IC_SS_SCL_HCNT_PosH                                 31

/*!< 0x0008 SS_SCL_CNT_SET
*1、This register must be set before any I2C bus transaction can take place to ensure proper I/O
timing. This register sets the SCL clock low period count for standard speed.  2、This register
can be written only when the I2C interface is disabled which corresponds to the IC_ENABLE[0]
register being set to 0. Writes at other times have no effect. 3、The minimum valid value is 8;
hardware prevents values less than this being written, and if attempted, results in 8 being set.
4、Value After Reset: IC_SS_SCL_LOW_COUNT 5、Memory Access: "(IC_HC_COUNT_VALUES==1) ? \"read-
only\" : \"read-write\""
*/
#define IC_SS_SCL_LCNT_Msk                                  (0xFFFF << IC_SS_SCL_LCNT_Pos)
#define IC_SS_SCL_LCNT_Pos                                  0
#define IC_SS_SCL_LCNT_PosL                                 0
#define IC_SS_SCL_LCNT_PosH                                 15


/*!< 0x000c FS_SCL_CNT_SET
*1、This register sets the SCL clock high-period count for fast mode or fast mode plus.
2、Value After Reset: IC_FS_SCL_HIGH_COUNT 3、Memory Access: "(IC_HC_COUNT_VALUES==1) ? \"read-
only\" : \"read-write\""
*/
#define IC_FS_SCL_HCNT_Msk                                  (0xFFFF << IC_FS_SCL_HCNT_Pos)
#define IC_FS_SCL_HCNT_Pos                                  16
#define IC_FS_SCL_HCNT_PosL                                 16
#define IC_FS_SCL_HCNT_PosH                                 31

/*!< 0x000c FS_SCL_CNT_SET
*1、This register must be set before any I2C bus transaction can take place to ensure proper I/O
timing. This register sets the SCL clock low period count for fast speed. It is used in highspeed
mode to send the Master Code and START BYTE or General CALL.  2、This register can be written
only when the I2C interface is disabled, which corresponds to the IC_ENABLE[0] register being set
to 0. Writes at other times have no effect. 3、The minimum valid value is 8; hardware prevents
values less than this being written, and if attempted results in 8 being set. 4、Value After
Reset: IC_FS_SCL_LOW_COUNT 5、Memory Access: "(IC_HC_COUNT_VALUES==1) ? \"read-only\" : \"read-
write\""
*/
#define IC_FS_SCL_LCNT_Msk                                  (0xFFFF << IC_FS_SCL_LCNT_Pos)
#define IC_FS_SCL_LCNT_Pos                                  0
#define IC_FS_SCL_LCNT_PosL                                 0
#define IC_FS_SCL_LCNT_PosH                                 15


/*!< 0x0010 HS_SCL_CNT_SET
*1、This register must be set before any I2C bus transaction can take place to ensure proper I/O
timing. This register sets the SCL clock high period count for high speed. 2、The SCL High time
depends on the loading of the bus. For 100pF loading, the SCL High time is 60ns; for 400pF loading
, the SCL High time is 120ns.  3、This register can be written only when the I2C interface is
disabled, which corresponds to the IC_ENABLE[0] register being set to 0. Writes at other times
have no effect. 4、The minimum valid value is 6; hardware prevents values less than this being
written, and if attempted results in 6 being set.  5、Value After Reset: IC_HS_SCL_HIGH_COUNT
6、Memory Access: "(IC_HC_COUNT_VALUES==1) ? \"read-only\" : \"read-write\""
*/
#define IC_HS_SCL_HCNT_Msk                                  (0xFFFF << IC_HS_SCL_HCNT_Pos)
#define IC_HS_SCL_HCNT_Pos                                  16
#define IC_HS_SCL_HCNT_PosL                                 16
#define IC_HS_SCL_HCNT_PosH                                 31

/*!< 0x0010 HS_SCL_CNT_SET
*1、This register must be set before any I2C bus transaction can take place to ensure proper I/O
timing. This register sets the SCL clock low period count for high speed. 2、The SCL low time
depends on the loading of the bus. For 100pF loading, the SCL low time is 160ns; for 400pF loading
, the SCL low time is 320ns.  3、This register can be written only when the I2C interface is
disabled, which corresponds to the IC_ENABLE[0] register being set to 0. Writes at other times
have no effect. 4、The minimum valid value is 8; hardware prevents values less than this being
written, and if attempted results in 8 being set.  5、Value After Reset: IC_HS_SCL_LOW_COUNT
6、Memory Access: "(IC_HC_COUNT_VALUES==1) ? \"read-only\" : \"read-write\""
*/
#define IC_HS_SCL_LCNT_Msk                                  (0xFFFF << IC_HS_SCL_LCNT_Pos)
#define IC_HS_SCL_LCNT_Pos                                  0
#define IC_HS_SCL_LCNT_PosL                                 0
#define IC_HS_SCL_LCNT_PosH                                 15


/*!< 0x0014 IC_FS_SPKLEN
*1、This register sets the duration, measured in ic_clk cycles, of the longest spike in the SCL
or SDA lines that will be filtered out by the spike suppression logic 2、The minimum valid value
is 1; hardware prevents values less than this being written, and if attempted results in 1 being
set.  3、Value After Reset: IC_DEFAULT_HS_SPKLEN.
*/
#define IC_HS_SPKLEN_Msk                                    (0xFF << IC_HS_SPKLEN_Pos)
#define IC_HS_SPKLEN_Pos                                    8
#define IC_HS_SPKLEN_PosL                                   8
#define IC_HS_SPKLEN_PosH                                   15

/*!< 0x0014 IC_FS_SPKLEN
*1、This register must be set before any I2C bus transaction can take place to ensure stable
operation. This register sets the duration, measured in ic_clk cycles, of the longest spike in
the SCL or SDA lines that will be filtered out by the spike suppression logic.  2、This register
can be written only when the I2C interface is disabled which corresponds to the IC_ENABLE[0]
register being set to 0. Writes at other times have no effect.  3、The minimum valid value is 1;
hardware prevents values less than this being written, and if attempted results in 1 being set.
4、Value After Reset: IC_DEFAULT_FS_SPKLEN.
*/
#define IC_FS_SPKLEN_Msk                                    (0xFF << IC_FS_SPKLEN_Pos)
#define IC_FS_SPKLEN_Pos                                    0
#define IC_FS_SPKLEN_PosL                                   0
#define IC_FS_SPKLEN_PosH                                   7


/*!< 0x0018 SDA_HOLD_SETUP
*1、SDA Setup. It is recommended that if the required delay is 1000ns, then for an ic_clk
frequency of 10 MHz, IC_SDA_SETUP should be programmed to a value of 11. IC_SDA_SETUP must be
programmed with a minimum value of 2. 2、This register controls the amount of time delay (in
terms of number of ic_clk clock periods) introduced in the rising edge of SCL - relative to SDA
changing - when DW_apb_i2c services a read request in a slave-transmitter operation. The relevant
I2C requirement is tSU:DAT (note 4) as detailed in the I2C Bus Specification.  3、This register
must be programmed with a value equal to or greater than 2. Writes to this register succeed only
when IC_ENABLE[0] = 0. 4、Note: The length of setup time is calculated using [(IC_SDA_SETUP - 1)
* (ic_clk_period)], so if the user requires 10 ic_clk periods of setup time, they should program
a value of 11. The IC_SDA_SETUP register is only used by the DW_apb_i2c when operating as a slave
transmitter. 5、Exists: IC_ULTRA_FAST_MODE==0 6、Value After Reset: IC_DEFAULT_SDA_SETUP
*/
#define SDA_SETUP_Msk                                       (0xFF << SDA_SETUP_Pos)
#define SDA_SETUP_Pos                                       24
#define SDA_SETUP_PosL                                      24
#define SDA_SETUP_PosH                                      31

/*!< 0x0018 SDA_HOLD_SETUP
*1、Sets the required SDA hold time in units of ic_clk period, when DW_apb_i2c acts as a receiver
. 2、Value After Reset: IC_DEFAULT_SDA_RX_HOLD (0)
*/
#define IC_SDA_RX_HOLD_Msk                                  (0xFF << IC_SDA_RX_HOLD_Pos)
#define IC_SDA_RX_HOLD_Pos                                  16
#define IC_SDA_RX_HOLD_PosL                                 16
#define IC_SDA_RX_HOLD_PosH                                 23

/*!< 0x0018 SDA_HOLD_SETUP
*1、Sets the required SDA hold time in units of ic_clk period, when DW_apb_i2c acts as a
transmitter. 2、Value After Reset: IC_DEFAULT_SDA_TX_HOLD (1)
*/
#define IC_SDA_TX_HOLD_Msk                                  (0xFFFF << IC_SDA_TX_HOLD_Pos)
#define IC_SDA_TX_HOLD_Pos                                  0
#define IC_SDA_TX_HOLD_PosL                                 0
#define IC_SDA_TX_HOLD_PosH                                 15


/*!< 0x001c DMA_CR
*1、Transmit Data Level. This bit field controls the level at which a DMA request is made by the
transmit logic. It is equal to the watermark level; that is, the dma_tx_req signal is generated
when the number of valid data entries in the transmit FIFO is equal to or below this field value,
and TDMAE = 1. 2、Range Variable[x]: TX_ABW – 1 (3)
*/
#define DMATDL_Msk                                          (0x07 << DMATDL_Pos)
#define DMATDL_Pos                                          16
#define DMATDL_PosL                                         16
#define DMATDL_PosH                                         18

/*!< 0x001c DMA_CR
*1、Receive Data Level. This bit field controls the level at which a DMA request is made by the
receive logic. The watermark level = DMARDL+1; that is, dma_rx_req is generated when the number
of valid data entries in the receive FIFO is equal to or more than this field value + 1, and
RDMAE =1. For instance, when DMARDL is 0, then dma_rx_req is asserted  when 1 or more data
entries are present in the receive FIFO. 2、Range Variable[x]: RX_ABW - 1(3)
*/
#define DMARDL_Msk                                          (0x07 << DMARDL_Pos)
#define DMARDL_Pos                                          8
#define DMARDL_PosL                                         8
#define DMARDL_PosH                                         10

/*!< 0x001c DMA_CR
*Transmit DMA Enable. This bit enables/disables the transmit FIFO DMA channel.
*/
#define TDMAE_Msk                                           (0x01 << TDMAE_Pos)
#define TDMAE_Pos                                           1
#define TDMAE_PosL                                          1
#define TDMAE_PosH                                          1

/*!< 0x001c DMA_CR
*Receive DMA Enable. This bit enables/disables the receive FIFO DMA channel.
*/
#define RDMAE_Msk                                           (0x01 << RDMAE_Pos)
#define RDMAE_Pos                                           0
#define RDMAE_PosL                                          0
#define RDMAE_PosH                                          0


/*!< 0x0020 TL_SET
*1、Transmit FIFO Threshold Level. Controls the level of entries (or below) that trigger the
TX_EMPTY interrupt (bit 4 in ISR register).  2、The valid range is 0-255, with the additional
restriction that it may not be set to value larger than the depth of the buffer. If an attempt is
made to do that, the actual value set will be the maximum depth of the buffer. （7） 3、A
value of 0 sets the threshold for 0 entries, and a value of 255 sets the threshold for 255 entries
. 4、Value After Reset: IC_TX_TL
*/
#define TX_TL_Msk                                           (0xFF << TX_TL_Pos)
#define TX_TL_Pos                                           8
#define TX_TL_PosL                                          8
#define TX_TL_PosH                                          15

/*!< 0x0020 TL_SET
*1、Receive FIFO Threshold Level.Controls the level of entries (or above) that triggers the
RX_FULL interrupt (bit 2 in ISR register).  2、The valid range is 0-255, with the additional
restriction that hardware does not allow this value to be set to a value larger than the depth of
the buffer. If an attempt is made to do that, the actual value set will be the maximum depth of
the buffer.  3、A value of 0 sets the threshold for 1 entry, and a value of 255 sets the
threshold for 256 entries. 4、Value After Reset: IC_RX_TL
*/
#define RX_TL_Msk                                           (0xFF << RX_TL_Pos)
#define RX_TL_Pos                                           0
#define RX_TL_PosL                                          0
#define RX_TL_PosH                                          7


/*!< 0x0024 ICR
*Write 1 to this bit to clear the combined interrupt, all individual interrupts, and the
TX_ABRT_SOURCE register. This bit does not clear hardware clearable interrupts but software
clearable interrupts. Refer to Bit 9 of the TX_ABRT_SOURCE register for an exception to clearing
TX_ABRT_SOURCE.
*/
#define CLR_INTR_Msk                                        (0x01 << CLR_INTR_Pos)
#define CLR_INTR_Pos                                        31
#define CLR_INTR_PosL                                       31
#define CLR_INTR_PosH                                       31

/*!< 0x0024 ICR
*Write 1 to this bit to clear the SCL_STUCT_AT_LOW interrupt (bit 13) of the ISR register.
*/
#define CLR_SCL_STUCK_DET_Msk                               (0x01 << CLR_SCL_STUCK_DET_Pos)
#define CLR_SCL_STUCK_DET_Pos                               13
#define CLR_SCL_STUCK_DET_PosL                              13
#define CLR_SCL_STUCK_DET_PosH                              13

/*!< 0x0024 ICR
*Write 1 to this bit to clear the RESTART_DET interrupt (bit 12) of the ISR register.
*/
#define CLR_RESTART_DET_Msk                                 (0x01 << CLR_RESTART_DET_Pos)
#define CLR_RESTART_DET_Pos                                 12
#define CLR_RESTART_DET_PosL                                12
#define CLR_RESTART_DET_PosH                                12

/*!< 0x0024 ICR
*Write 1 to this bit to clear the GEN_CALL interrupt (bit 11) of the ISR register.
*/
#define CLR_GEN_CALL_Msk                                    (0x01 << CLR_GEN_CALL_Pos)
#define CLR_GEN_CALL_Pos                                    11
#define CLR_GEN_CALL_PosL                                   11
#define CLR_GEN_CALL_PosH                                   11

/*!< 0x0024 ICR
*null
*/
#define CLR_START_DET_Msk                                   (0x01 << CLR_START_DET_Pos)
#define CLR_START_DET_Pos                                   10
#define CLR_START_DET_PosL                                  10
#define CLR_START_DET_PosH                                  10

/*!< 0x0024 ICR
*Write 1 to this bit to clear the STOP_DET interrupt (bit 9) of the ISR register.
*/
#define CLR_STOP_DET_Msk                                    (0x01 << CLR_STOP_DET_Pos)
#define CLR_STOP_DET_Pos                                    9
#define CLR_STOP_DET_PosL                                   9
#define CLR_STOP_DET_PosH                                   9

/*!< 0x0024 ICR
*Write 1 to this bit clears the ACTIVITY interrupt if the I2C is not active anymore. If the I2C
module is still active on the bus, the ACTIVITY interrupt bit continues to be set. It is
automatically cleared by hardware if the module is disabled and if there is no further activity
on the bus. The value read from this register to get status of the ACTIVITY interrupt (bit 8) of
the ISR register.
*/
#define CLR_ACTIVITY_Msk                                    (0x01 << CLR_ACTIVITY_Pos)
#define CLR_ACTIVITY_Pos                                    8
#define CLR_ACTIVITY_PosL                                   8
#define CLR_ACTIVITY_PosH                                   8

/*!< 0x0024 ICR
*Write 1 to this bit to clear the RX_DONE interrupt (bit 7) of the ISR register.
*/
#define CLR_RX_DONE_Msk                                     (0x01 << CLR_RX_DONE_Pos)
#define CLR_RX_DONE_Pos                                     7
#define CLR_RX_DONE_PosL                                    7
#define CLR_RX_DONE_PosH                                    7

/*!< 0x0024 ICR
*Write 1 to this bit to clear the TX_ABRT interrupt (bit 6) of the ISR registerr, and the
TX_ABRT_SOURCE register. This also releases the TX FIFO from the flushed/reset state, allowing
more writes to the TX FIFO. Refer to Bit 9 of the TX_ABRT_SOURCE register for an exception to
clearing TX_ABRT_SOURCE.
*/
#define CLR_TX_ABRT_Msk                                     (0x01 << CLR_TX_ABRT_Pos)
#define CLR_TX_ABRT_Pos                                     6
#define CLR_TX_ABRT_PosL                                    6
#define CLR_TX_ABRT_PosH                                    6

/*!< 0x0024 ICR
*Write 1 to this bit to clear the RD_REQ interrupt (bit 5) of the ISR register.
*/
#define CLR_RD_REQ_Msk                                      (0x01 << CLR_RD_REQ_Pos)
#define CLR_RD_REQ_Pos                                      5
#define CLR_RD_REQ_PosL                                     5
#define CLR_RD_REQ_PosH                                     5

/*!< 0x0024 ICR
*Write 1 to this bit to clear the TX_OVER interrupt (bit 3) of the ISR register.
*/
#define CLR_TX_OVER_Msk                                     (0x01 << CLR_TX_OVER_Pos)
#define CLR_TX_OVER_Pos                                     3
#define CLR_TX_OVER_PosL                                    3
#define CLR_TX_OVER_PosH                                    3

/*!< 0x0024 ICR
*Write 1 to this bit to clear the RX_OVER interrupt (bit 1) of the ISR register.
*/
#define CLR_RX_OVER_Msk                                     (0x01 << CLR_RX_OVER_Pos)
#define CLR_RX_OVER_Pos                                     1
#define CLR_RX_OVER_PosL                                    1
#define CLR_RX_OVER_PosH                                    1

/*!< 0x0024 ICR
*Write 1 to this bit to clear the RX_UNDER interrupt (bit 0) of the ISR register.
*/
#define CLR_RX_UNDER_Msk                                    (0x01 << CLR_RX_UNDER_Pos)
#define CLR_RX_UNDER_Pos                                    0
#define CLR_RX_UNDER_PosL                                   0
#define CLR_RX_UNDER_PosH                                   0


/*!< 0x0028 IMR
*null
*/
#define M_SLV_RX_BCNT_TO_Msk                                (0x01 << M_SLV_RX_BCNT_TO_Pos)
#define M_SLV_RX_BCNT_TO_Pos                                17
#define M_SLV_RX_BCNT_TO_PosL                               17
#define M_SLV_RX_BCNT_TO_PosH                               17

/*!< 0x0028 IMR
*null
*/
#define M_SLV_TX_BCNT_TO_Msk                                (0x01 << M_SLV_TX_BCNT_TO_Pos)
#define M_SLV_TX_BCNT_TO_Pos                                16
#define M_SLV_TX_BCNT_TO_PosL                               16
#define M_SLV_TX_BCNT_TO_PosH                               16

/*!< 0x0028 IMR
*null
*/
#define M_SLV_RX_BCNT_ERR_Msk                               (0x01 << M_SLV_RX_BCNT_ERR_Pos)
#define M_SLV_RX_BCNT_ERR_Pos                               15
#define M_SLV_RX_BCNT_ERR_PosL                              15
#define M_SLV_RX_BCNT_ERR_PosH                              15

/*!< 0x0028 IMR
*null
*/
#define SLV_TX_BCNT_ERR_Msk                                 (0x01 << SLV_TX_BCNT_ERR_Pos)
#define SLV_TX_BCNT_ERR_Pos                                 14
#define SLV_TX_BCNT_ERR_PosL                                14
#define SLV_TX_BCNT_ERR_PosH                                14

/*!< 0x0028 IMR
*Exists: IC_BUS_CLEAR_FEATURE==1(0)
*/
#define SCL_STUCK_AT_LOW_Msk                                (0x01 << SCL_STUCK_AT_LOW_Pos)
#define SCL_STUCK_AT_LOW_Pos                                13
#define SCL_STUCK_AT_LOW_PosL                               13
#define SCL_STUCK_AT_LOW_PosH                               13

/*!< 0x0028 IMR
*Exists: IC_SLV_RESTART_DET_EN == 1(0).
*/
#define M_RESTART_DET_Msk                                   (0x01 << M_RESTART_DET_Pos)
#define M_RESTART_DET_Pos                                   12
#define M_RESTART_DET_PosL                                  12
#define M_RESTART_DET_PosH                                  12

/*!< 0x0028 IMR
*null
*/
#define M_GEN_CALL_Msk                                      (0x01 << M_GEN_CALL_Pos)
#define M_GEN_CALL_Pos                                      11
#define M_GEN_CALL_PosL                                     11
#define M_GEN_CALL_PosH                                     11

/*!< 0x0028 IMR
*null
*/
#define M_START_DET_Msk                                     (0x01 << M_START_DET_Pos)
#define M_START_DET_Pos                                     10
#define M_START_DET_PosL                                    10
#define M_START_DET_PosH                                    10

/*!< 0x0028 IMR
*null
*/
#define M_STOP_DET_Msk                                      (0x01 << M_STOP_DET_Pos)
#define M_STOP_DET_Pos                                      9
#define M_STOP_DET_PosL                                     9
#define M_STOP_DET_PosH                                     9

/*!< 0x0028 IMR
*null
*/
#define M_ACTIVITY_Msk                                      (0x01 << M_ACTIVITY_Pos)
#define M_ACTIVITY_Pos                                      8
#define M_ACTIVITY_PosL                                     8
#define M_ACTIVITY_PosH                                     8

/*!< 0x0028 IMR
*null
*/
#define M_RX_DONE_Msk                                       (0x01 << M_RX_DONE_Pos)
#define M_RX_DONE_Pos                                       7
#define M_RX_DONE_PosL                                      7
#define M_RX_DONE_PosH                                      7

/*!< 0x0028 IMR
*null
*/
#define M_TX_ABRT_Msk                                       (0x01 << M_TX_ABRT_Pos)
#define M_TX_ABRT_Pos                                       6
#define M_TX_ABRT_PosL                                      6
#define M_TX_ABRT_PosH                                      6

/*!< 0x0028 IMR
*null
*/
#define M_RD_REQ_Msk                                        (0x01 << M_RD_REQ_Pos)
#define M_RD_REQ_Pos                                        5
#define M_RD_REQ_PosL                                       5
#define M_RD_REQ_PosH                                       5

/*!< 0x0028 IMR
*null
*/
#define M_TX_EMPTY_Msk                                      (0x01 << M_TX_EMPTY_Pos)
#define M_TX_EMPTY_Pos                                      4
#define M_TX_EMPTY_PosL                                     4
#define M_TX_EMPTY_PosH                                     4

/*!< 0x0028 IMR
*null
*/
#define M_TX_OVER_Msk                                       (0x01 << M_TX_OVER_Pos)
#define M_TX_OVER_Pos                                       3
#define M_TX_OVER_PosL                                      3
#define M_TX_OVER_PosH                                      3

/*!< 0x0028 IMR
*null
*/
#define M_RX_FULL_Msk                                       (0x01 << M_RX_FULL_Pos)
#define M_RX_FULL_Pos                                       2
#define M_RX_FULL_PosL                                      2
#define M_RX_FULL_PosH                                      2

/*!< 0x0028 IMR
*null
*/
#define M_RX_OVER_Msk                                       (0x01 << M_RX_OVER_Pos)
#define M_RX_OVER_Pos                                       1
#define M_RX_OVER_PosL                                      1
#define M_RX_OVER_PosH                                      1

/*!< 0x0028 IMR
*null
*/
#define M_RX_UNDER_Msk                                      (0x01 << M_RX_UNDER_Pos)
#define M_RX_UNDER_Pos                                      0
#define M_RX_UNDER_PosL                                     0
#define M_RX_UNDER_PosH                                     0

#define M_ALL_Msk                                           (M_RX_UNDER_Msk         |           \
                                                             M_RX_OVER_Msk          |           \
                                                             M_RX_FULL_Msk          |           \
                                                             M_TX_OVER_Msk          |           \
                                                             M_TX_EMPTY_Msk         |           \
                                                             M_RD_REQ_Msk           |           \
                                                             M_TX_ABRT_Msk          |           \
                                                             M_RX_DONE_Msk          |           \
                                                             M_ACTIVITY_Msk         |           \
                                                             M_STOP_DET_Msk         |           \
                                                             M_START_DET_Msk        |           \
                                                             M_GEN_CALL_Msk         |           \
                                                             M_RESTART_DET_Msk      |           \
                                                             SCL_STUCK_AT_LOW_Msk   |           \
                                                             SLV_TX_BCNT_ERR_Msk    |           \
                                                             M_SLV_RX_BCNT_ERR_Msk  |           \
                                                             M_SLV_TX_BCNT_TO_Msk   |           \
                                                             M_SLV_RX_BCNT_TO_Msk)
/*!< 0x002c ISR（raw）
*null
*/
#define SLV_RX_BCNT_TO_Msk                                  (0x01 << SLV_RX_BCNT_TO_Pos)
#define SLV_RX_BCNT_TO_Pos                                  17
#define SLV_RX_BCNT_TO_PosL                                 17
#define SLV_RX_BCNT_TO_PosH                                 17

/*!< 0x002c ISR（raw）
*null
*/
#define SLV_TX_BCNT_TO_Msk                                  (0x01 << SLV_TX_BCNT_TO_Pos)
#define SLV_TX_BCNT_TO_Pos                                  16
#define SLV_TX_BCNT_TO_PosL                                 16
#define SLV_TX_BCNT_TO_PosH                                 16

/*!< 0x002c ISR（raw）
*null
*/
#define SLV_RX_BCNT_ERR_Msk                                 (0x01 << SLV_RX_BCNT_ERR_Pos)
#define SLV_RX_BCNT_ERR_Pos                                 15
#define SLV_RX_BCNT_ERR_PosL                                15
#define SLV_RX_BCNT_ERR_PosH                                15

/*!< 0x002c ISR（raw）
*null
*/
#define SLV_TX_BCNT_ERR_Msk                                 (0x01 << SLV_TX_BCNT_ERR_Pos)
#define SLV_TX_BCNT_ERR_Pos                                 14
#define SLV_TX_BCNT_ERR_PosL                                14
#define SLV_TX_BCNT_ERR_PosH                                14

/*!< 0x002c ISR（raw）
*1、Indicates whether the SCL Line is stuck at low for the IC_SCL_STUCK_LOW_TIMEOUT number of
ic_clk periods. Enabled only when IC_BUS_CLEAR_FEATURE=1 and IC_ULTRA_FAST_MODE=0 (
IC_BUS_CLEAR_FEATURE=0 and  IC_ULTRA_FAST_MODE=0)2、Exists: IC_BUS_CLEAR_FEATURE==1
*/
#define SCL_STUCK_AT_LOW_Msk                                (0x01 << SCL_STUCK_AT_LOW_Pos)
#define SCL_STUCK_AT_LOW_Pos                                13
#define SCL_STUCK_AT_LOW_PosL                               13
#define SCL_STUCK_AT_LOW_PosH                               13

/*!< 0x002c ISR（raw）
*1、Indicates whether a RESTART condition has occurred on the I2C interface when DW_apb_i2c is
operating in Slave mode and the slave is being addressed. 2、Enabled only when
IC_SLV_RESTART_DET_EN=1. ( IC_SLV_RESTART_DET_EN = 0) 3、Note: However, in high-speed mode or
during a START BYTE transfer, the RESTART comes before the address field as per the I2C protocol.
In this case, the slave is not the addressed slave when the RESTART is issued, therefore
DW_apb_i2c does not generate the RESTART_DET interrupt.
*/
#define RESTART_DET_Msk                                     (0x01 << RESTART_DET_Pos)
#define RESTART_DET_Pos                                     12
#define RESTART_DET_PosL                                    12
#define RESTART_DET_PosH                                    12

/*!< 0x002c ISR（raw）
*Set only when a General Call address is received and it is acknowledged. It stays set until it
is cleared either by disabling DW_apb_i2c or when the CPU reads bit 0 of the IC_CLR_GEN_CALL
register. DW_apb_i2c stores the received data in the Rx buffer.
*/
#define GEN_CALL_Msk                                        (0x01 << GEN_CALL_Pos)
#define GEN_CALL_Pos                                        11
#define GEN_CALL_PosL                                       11
#define GEN_CALL_PosH                                       11

/*!< 0x002c ISR（raw）
*Indicates whether a START or RESTART condition has  occurred on the I2C interface regardless of
whether  DW_apb_i2c is operating in slave or master mode.
*/
#define START_DET_Msk                                       (0x01 << START_DET_Pos)
#define START_DET_Pos                                       10
#define START_DET_PosL                                      10
#define START_DET_PosH                                      10

/*!< 0x002c ISR（raw）
*1、Indicates whether a STOP condition has occurred on the I2C interface regardless of whether
DW_apb_i2c is operating in slave or master mode. 2、In Slave Mode: (1) If IC_CON[7]=1'b1 (
STOP_DET_IFADDRESSED), the STOP_DET interrupt will be issued only if slave is addressed.
Note: During a general call address, this slave does not issue a STOP_DET interrupt if
STOP_DET_IF_ADDRESSED=1'b1, even if the slave  responds to the general call address by generating
ACK. The STOP_DET interrupt is generated only when the transmitted address matches the slave
address (SAR). (2)  If IC_CON[7]=1'b0 (STOP_DET_IFADDRESSED), the STOP_DET interrupt is issued
irrespective of whether it is being addressed. 3、In Master Mode: (1) If IC_CON[10]=1'b1  (
STOP_DET_IF_MASTER_ACTIVE),the STOP_DET  interrupt will be issued only if Master is active. (2)
If IC_CON[10]=1'b0 (STOP_DET_IFADDRESSED),the  STOP_DET interrupt will be issued irrespective of
whether master is active or not.
*/
#define STOP_DET_Msk                                        (0x01 << STOP_DET_Pos)
#define STOP_DET_Pos                                        9
#define STOP_DET_PosL                                       9
#define STOP_DET_PosH                                       9

/*!< 0x002c ISR（raw）
*1、This bit captures DW_apb_i2c activity and stays set until it is cleared. There are four ways
to clear it: (1) Disabling the DW_apb_i2c; (2) Reading the IC_CLR_ACTIVITY register; (3) Reading
the IC_CLR_INTR register; (4) System reset. 2、Once this bit is set, it stays set unless one of
the four methods is used to clear it. Even if the DW_apb_i2c module is idle, this bit remains set
until cleared, indicating that there was activity on the bus.
*/
#define ISR_ACTIVITY_Msk                                        (0x01 << ISR_ACTIVITY_Pos)
#define ISR_ACTIVITY_Pos                                        8
#define ISR_ACTIVITY_PosL                                       8
#define ISR_ACTIVITY_PosH                                       8

/*!< 0x002c ISR（raw）
*When the DW_apb_i2c is acting as a slave-transmitter, this bit is set to 1 if the master does
not acknowledge a transmitted byte. This occurs on the last byte of the transmission, indicating
that the transmission is done.
*/
#define RX_DONE_Msk                                         (0x01 << RX_DONE_Pos)
#define RX_DONE_Pos                                         7
#define RX_DONE_PosL                                        7
#define RX_DONE_PosH                                        7

/*!< 0x002c ISR（raw）
*1、This bit indicates if DW_apb_i2c, as an I2C transmitter, is unable to complete the intended
actions on the contents of the transmit FIFO. This situation can occur both as an I2C master or
an I2C slave, and is referred to as a 'transmit abort'. When this bit is set to 1, the
IC_TX_ABRT_SOURCE register indicates the reason why the transmit abort takes places. 2、Note:
The DW_apb_i2c flushes/resets/empties only the TX_FIFO whenever there is a transmit abort caused
by any of the events tracked by the IC_TX_ABRT_SOURCE register. The Tx FIFO remains in this
flushed state until the register IC_CLR_TX_ABRT is read. Once this read is performed, the Tx FIFO
is then ready to accept more data bytes from the APB interface. RX FIFO flush because of TX_ABRT
is controlled by the coreConsultant parameter  IC_AVOID_RX_FIFO_FLUSH_ON_TX_ABRT.
*/
#define TX_ABRT_Msk                                         (0x01 << TX_ABRT_Pos)
#define TX_ABRT_Pos                                         6
#define TX_ABRT_PosL                                        6
#define TX_ABRT_PosH                                        6

/*!< 0x002c ISR（raw）
*This bit is set to 1 when DW_apb_i2c is acting as a slave and another I2C master is attempting
to read data from DW_apb_i2c. The DW_apb_i2c holds the I2C bus in a wait state (SCL=0) until this
interrupt is serviced, which means that the slave has been addressed by a remote master that is
asking for data to be transferred. The processor must respond to this interrupt and then write
the requested data to the IC_DATA_CMD register. This bit is set to 0 just after the processor
reads the IC_CLR_RD_REQ register.
*/
#define RD_REQ_Msk                                          (0x01 << RD_REQ_Pos)
#define RD_REQ_Pos                                          5
#define RD_REQ_PosL                                         5
#define RD_REQ_PosH                                         5

/*!< 0x002c ISR（raw）
*1、The behavior of the TX_EMPTY interrupt status differs based on the TX_EMPTY_CTRL selection
in the IC_CON register. (1) When TX_EMPTY_CTRL = 0: This bit is set to 1 when the transmit buffer
is at or below the threshold value set in the IC_TX_TL register. (2) When TX_EMPTY_CTRL = 1:
2、This bit is set to 1 when the transmit buffer is at or below the threshold value set in the
IC_TX_TL register and the transmission of the address/data from the internal shift register for
the most recently popped command is completed. 3、It is automatically cleared by hardware when
the buffer level goes above the threshold. When IC_ENABLE[0] is set to 0, the TX FIFO is flushed
and held in reset. There the TX FIFO looks like it has no data within it, so this bit is set to 1
, provided there is activity in the master or slave state machines. When there is no longer any
activity, then with ic_en=0, this bit is set to 0.
*/
#define TX_EMPTY_Msk                                        (0x01 << TX_EMPTY_Pos)
#define TX_EMPTY_Pos                                        4
#define TX_EMPTY_PosL                                       4
#define TX_EMPTY_PosH                                       4

/*!< 0x002c ISR（raw）
*Set during transmit if the transmit buffer is filled to  IC_TX_BUFFER_DEPTH and the processor
attempts to  issue another I2C command by writing to the IC_DATA_CMD register. When the module is
disabled, this bit keeps its level until the master or slave state machines go into idle, and
when ic_en goes to 0, this interrupt is cleared.
*/
#define TX_OVER_Msk                                         (0x01 << TX_OVER_Pos)
#define TX_OVER_Pos                                         3
#define TX_OVER_PosL                                        3
#define TX_OVER_PosH                                        3

/*!< 0x002c ISR（raw）
*Set when the receive buffer reaches or goes above the RX_TL threshold in the IC_RX_TL register.
It is automatically cleared by hardware when buffer level goes below the threshold. If the module
is disabled (IC_ENABLE[0]=0), the RX FIFO is flushed and held in reset; therefore the RX FIFO is
not full. So this bit is cleared once the IC_ENABLE bit 0 is programmed with a 0, regardless of
the activity that continues.
*/
#define RX_FULL_Msk                                         (0x01 << RX_FULL_Pos)
#define RX_FULL_Pos                                         2
#define RX_FULL_PosL                                        2
#define RX_FULL_PosH                                        2

/*!< 0x002c ISR（raw）
*1、Set if the receive buffer is completely filled to  IC_RX_BUFFER_DEPTH and an additional byte
is received from an external I2C device. The DW_apb_i2c acknowledges this, but any data bytes
received after the FIFO is full are lost. If the module is disabled (IC_ENABLE[0]=0), this bit
keeps its level until the master or slave state machines go into idle, and when ic_en goes to 0,
this interrupt is cleared. 2、Note: If the configuration parameter  IC_RX_FULL_HLD_BUS_EN is
enabled and bit 9 of the  IC_CON register (RX_FIFO_FULL_HLD_CTRL) is  programmed to HIGH, then
the RX_OVER interrupt never occurs, because the Rx FIFO never overflows. (IC_RX_FULL_HLD_BUS_EN =
0)
*/
#define RX_OVER_Msk                                         (0x01 << RX_OVER_Pos)
#define RX_OVER_Pos                                         1
#define RX_OVER_PosL                                        1
#define RX_OVER_PosH                                        1

/*!< 0x002c ISR（raw）
*Set if the processor attempts to read the receive buffer when it is empty by reading from the
IC_DATA_CMD register. If the module is disabled (IC_ENABLE[0]=0), this bit keeps its level until
the master or slave state machines go into idle, and when ic_en goes to 0, this interrupt is
cleared.
*/
#define RX_UNDER_Msk                                        (0x01 << RX_UNDER_Pos)
#define RX_UNDER_Pos                                        0
#define RX_UNDER_PosL                                       0
#define RX_UNDER_PosH                                       0


/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates the number of Tx FIFO Data Commands which are flushed due to TX_ABRT
interrupt. It is cleared whenever I2C is disabled. 2、Role of DW_apb_i2c: Master-Transmitter or
SlaveTransmitter
*/
#define TX_FLUSH_CNT_Msk                                    (0XFF << TX_FLUSH_CNT_Pos)
#define TX_FLUSH_CNT_Pos                                    24
#define TX_FLUSH_CNT_PosL                                   24
#define TX_FLUSH_CNT_PosH                                   31

/*!< 0x0030 TX_ABRT_SOURCE
*1、This is a master-mode-only bit. Master detects the SDA Stuck at low for the
IC_SDA_STUCK_AT_LOW_TIMEOUT value of ic_clks. 2、Role of DW_apb_i2c: Master 3、Exists:
IC_BUS_CLEAR_FEATURE == 1 (0)
*/
#define ABRT_SDA_STUCK_AT_LOW_Msk                           (0x01 << ABRT_SDA_STUCK_AT_LOW_Pos)
#define ABRT_SDA_STUCK_AT_LOW_Pos                           17
#define ABRT_SDA_STUCK_AT_LOW_PosL                          17
#define ABRT_SDA_STUCK_AT_LOW_PosH                          17

/*!< 0x0030 TX_ABRT_SOURCE
*1、This is a master-mode-only bit. Master has detected the transfer abort (IC_ENABLE[1]).
2、Role of DW_apb_i2c: Master-Transmitter.
*/
#define ABRT_USER_ABRT_Msk                                  (0x01 << ABRT_USER_ABRT_Pos)
#define ABRT_USER_ABRT_Pos                                  16
#define ABRT_USER_ABRT_PosL                                 16
#define ABRT_USER_ABRT_PosH                                 16

/*!< 0x0030 TX_ABRT_SOURCE
*1、When the processor side responds to a slave mode request for data to be transmitted to a
remote master and user writes a 1 in CMD (bit 8) of IC_DATA_CMD register. 2、Role of DW_apb_i2c:
Slave-Transmitter. 3、Exists: IC_ULTRA_FAST_MODE==0 (0)
*/
#define ABRT_SLVRD_INTX_Msk                                 (0x01 << ABRT_SLVRD_INTX_Pos)
#define ABRT_SLVRD_INTX_Pos                                 15
#define ABRT_SLVRD_INTX_PosL                                15
#define ABRT_SLVRD_INTX_PosH                                15

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that a Slave has lost the bus while transmitting data to a remote master
. IC_TX_ABRT_SOURCE[12] is set at the same time. 2、Even though the slave never 'owns' the bus,
something could go wrong on the bus. This is a fail safe check. For instance, during a data
transmission at the low-tohigh transition of SCL, if what is on the data bus is not what is
supposed to be transmitted, then DW_apb_i2c no longer own the bus. 3、Role of DW_apb_i2c: Slave-
Transmitter.  4、Exists: IC_ULTRA_FAST_MODE==0 (0).
*/
#define ABRT_SLV_ARBLOST_Msk                                (0x01 << ABRT_SLV_ARBLOST_Pos)
#define ABRT_SLV_ARBLOST_Pos                                14
#define ABRT_SLV_ARBLOST_PosL                               14
#define ABRT_SLV_ARBLOST_PosH                               14

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field specifies that the Slave has received a read command and some data exists in the
TX FIFO, so the slave issues a TX_ABRT interrupt to flush old data in TX FIFO. 2、Exists:
IC_ULTRA_FAST_MODE==0 (0).
*/
#define ABRT_SLVFLUSH_TXFIFO_Msk                            (0x01 << ABRT_SLVFLUSH_TXFIFO_Pos)
#define ABRT_SLVFLUSH_TXFIFO_Pos                            13
#define ABRT_SLVFLUSH_TXFIFO_PosL                           13
#define ABRT_SLVFLUSH_TXFIFO_PosH                           13

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field specifies that the Master has lost arbitration, or if IC_TX_ABRT_SOURCE[14] is
also set, then the slave transmitter has lost arbitration. 2、Role of DW_apb_i2c: Master-
Transmitter or SlaveTransmitter.  3、Exists: IC_ULTRA_FAST_MODE==0 (0).
*/
#define ARB_LOST_Msk                                        (0x01 << ARB_LOST_Pos)
#define ARB_LOST_Pos                                        12
#define ARB_LOST_PosL                                       12
#define ARB_LOST_PosH                                       12

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the User tries to initiate a Master operation with the Master mode
disabled. 2、Role of DW_apb_i2c: Master-Transmitter or MasterReceiver.
*/
#define ABRT_MASTER_DIS_Msk                                 (0x01 << ABRT_MASTER_DIS_Pos)
#define ABRT_MASTER_DIS_Pos                                 11
#define ABRT_MASTER_DIS_PosL                                11
#define ABRT_MASTER_DIS_PosH                                11

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the restart is disabled  (IC_RESTART_EN bit (IC_CON[5]) =0) and
the master  sends a read command in 10-bit addressing mode. 2、Role of DW_apb_i2c: Master-Receiver
*/
#define ABRT_10B_RD_NORSTRT_Msk                             (0x01 << ABRT_10B_RD_NORSTRT_Pos)
#define ABRT_10B_RD_NORSTRT_Pos                             10
#define ABRT_10B_RD_NORSTRT_PosL                            10
#define ABRT_10B_RD_NORSTRT_PosH                            10

/*!< 0x0030 TX_ABRT_SOURCE
*1、To clear Bit 9, the source of the ABRT_SBYTE_NORSTRT must be fixed first; restart must be
enabled (IC_CON[5]=1), the SPECIAL bit must be cleared (IC_TAR[11]), or the GC_OR_START bit must
be cleared (IC_TAR[10]).  Once the source of the ABRT_SBYTE_NORSTRT is fixed, then this bit can
be cleared in the same manner as other bits in this register. If the source of the
ABRT_SBYTE_NORSTRT is not fixed before attempting to clear this bit, bit 9 clears for one cycle
and then gets reasserted. When this field is set to 1, the restart is disabled (IC_RESTART_EN bit
(IC_CON[5]) =0) and the user is trying to send a START Byte. 2、Role of DW_apb_i2c: Master
*/
#define ABRT_SBYTE_NORSTRT_Msk                              (0x01 << ABRT_SBYTE_NORSTRT_Pos)
#define ABRT_SBYTE_NORSTRT_Pos                              9
#define ABRT_SBYTE_NORSTRT_PosL                             9
#define ABRT_SBYTE_NORSTRT_PosH                             9

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the restart is disabled  (IC_RESTART_EN bit (IC_CON[5]) =0) and
the user is trying to use the master to transfer data in High Speed mode. 2、Role of DW_apb_i2c:
Master-Transmitter or Master Receiver. 3、Exists: IC_ULTRA_FAST_MODE==0 (0).
*/
#define ABRT_HS_NORSTRT_Msk                                 (0x01 << ABRT_HS_NORSTRT_Pos)
#define ABRT_HS_NORSTRT_Pos                                 8
#define ABRT_HS_NORSTRT_PosL                                8
#define ABRT_HS_NORSTRT_PosH                                8

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the Master has sent a START Byte and the START Byte was
acknowledged (wrong behavior). 2、Role of DW_apb_i2c: Master. 3、Exists: IC_ULTRA_FAST_MODE==0(0
).
*/
#define ABRT_SBYTE_ACKDET_Msk                               (0x01 << ABRT_SBYTE_ACKDET_Pos)
#define ABRT_SBYTE_ACKDET_Pos                               7
#define ABRT_SBYTE_ACKDET_PosL                              7
#define ABRT_SBYTE_ACKDET_PosH                              7

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the Master is in High Speed mode and the High Speed Master code
was acknowledged (wrong behavior). 2、Role of DW_apb_i2c: Master. 3、Exists: IC_ULTRA_FAST_MODE
==0(0)
*/
#define ABRT_HS_ACKDET_Msk                                  (0x01 << ABRT_HS_ACKDET_Pos)
#define ABRT_HS_ACKDET_Pos                                  6
#define ABRT_HS_ACKDET_PosL                                 6
#define ABRT_HS_ACKDET_PosH                                 6

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that DW_apb_i2c in the master mode has sent a General Call but the user
programmed the byte following the General Call to be a read from the bus (IC_DATA_CMD[9] is set
to 1). 2、Role of DW_apb_i2c: Master-Transmitter.  3、Exists: IC_ULTRA_FAST_MODE==0(0).
*/
#define ABRT_GCALL_READ_Msk                                 (0x01 << ABRT_GCALL_READ_Pos)
#define ABRT_GCALL_READ_Pos                                 5
#define ABRT_GCALL_READ_PosL                                5
#define ABRT_GCALL_READ_PosH                                5

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that DW_apb_i2c in master mode has sent a General Call and no slave on
the bus acknowledged the General Call. 2、Role of DW_apb_i2c: Master-Transmitter. 3、Exists:
IC_ULTRA_FAST_MODE==0(0).
*/
#define ABRT_GCALL_NOACK_Msk                                (0x01 << ABRT_GCALL_NOACK_Pos)
#define ABRT_GCALL_NOACK_Pos                                4
#define ABRT_GCALL_NOACK_PosL                               4
#define ABRT_GCALL_NOACK_PosH                               4

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates the master-mode only bit. When the master receives an acknowledgement
for the address, but when it sends data byte(s) following the address, it did not receive an
acknowledge from the remote slave(s). 2、Role of DW_apb_i2c: Master-Transmitter. 3、Exists:
IC_ULTRA_FAST_MODE==0(0).
*/
#define ABRT_TXDATA_NOACK_Msk                               (0x01 << ABRT_TXDATA_NOACK_Pos)
#define ABRT_TXDATA_NOACK_Pos                               3
#define ABRT_TXDATA_NOACK_PosL                              3
#define ABRT_TXDATA_NOACK_PosH                              3

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the Master is in 10-bit address mode and that the second address
byte of the 10-bit address was not acknowledged by any slave. 2、Role of DW_apb_i2c: Master-
Transmitter or MasterReceiver. 3、Exists: IC_ULTRA_FAST_MODE==0(0).
*/
#define ABRT_10ADDR2_NOACK_Msk                              (0x01 << ABRT_10ADDR2_NOACK_Pos)
#define ABRT_10ADDR2_NOACK_Pos                              2
#define ABRT_10ADDR2_NOACK_PosL                             2
#define ABRT_10ADDR2_NOACK_PosH                             2

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the Master is in 10-bit address mode and the first 10-bit address
byte was not acknowledged by any slave. 2、Role of DW_apb_i2c: Master-Transmitter or
MasterReceiver. 3、Exists: IC_ULTRA_FAST_MODE==0(0).
*/
#define ABRT_10ADDR1_NOACK_Msk                              (0x01 << ABRT_10ADDR1_NOACK_Pos)
#define ABRT_10ADDR1_NOACK_Pos                              1
#define ABRT_10ADDR1_NOACK_PosL                             1
#define ABRT_10ADDR1_NOACK_PosH                             1

/*!< 0x0030 TX_ABRT_SOURCE
*1、This field indicates that the Master is in 7-bit addressing mode and the address sent was
not acknowledged by any slave. 2、Role of DW_apb_i2c: Master-Transmitter or MasterReceiver.
3、Exists: IC_ULTRA_FAST_MODE==0(0).
*/
#define ABRT_7B_ADDR_NOACK_Msk                              (0x01 << ABRT_7B_ADDR_NOACK_Pos)
#define ABRT_7B_ADDR_NOACK_Pos                              0
#define ABRT_7B_ADDR_NOACK_PosL                             0
#define ABRT_7B_ADDR_NOACK_PosH                             0


/*!< 0x0034 SR
*1、Slave Received Data Lost. This bit indicates if a SlaveReceiver operation has been aborted
with at least one data byte received from an I2C transfer due to the setting bit 0 of IC_ENABLE
from 1 to 0.  2、When read as 1, DW_apb_i2c is deemed to have been actively engaged in an
aborted I2C transfer (with matching address) and the data phase of the I2C transfer has been
entered, even though a data byte has been responded with a NACK. 3、If the remote I2C master
terminates the transfer with a STOP condition before the DW_apb_i2c has a chance to NACK a
transfer, and IC_ENABLE[0] has been set to 0, then this bit is also set to 1. 4、When read as 0,
DW_apb_i2c is deemed to have been disabled without being actively involved in the data phase of a
Slave-Receiver transfer. 5、The CPU can safely read this bit when IC_EN (bit 0) is read as 0.
*/
#define SLV_RX_DATA_LOST_Msk                                (0x01 << SLV_RX_DATA_LOST_Pos)
#define SLV_RX_DATA_LOST_Pos                                26
#define SLV_RX_DATA_LOST_PosL                               26
#define SLV_RX_DATA_LOST_PosH                               26

/*!< 0x0034 SR
*1、Slave Disabled While Busy (Transmit, Receive). This bit indicates if a potential or active
Slave operation has been aborted due to the setting bit 0 of the IC_ENABLE register from 1 to 0.
This bit is set when the CPU writes a 0 to the IC_ENABLE register while: (a) DW_apb_i2c is
receiving the address byte of the SlaveTransmitter operation from a remote master; OR, (b)
address and data bytes of the Slave-Receiver operation from a remote master. 2、When read as 1,
DW_apb_i2c is deemed to have forced a NACK during any part of an I2C transfer, irrespective of
whether the I2C address matches the slave address set in DW_apb_i2c (IC_SAR register) OR if the
transfer is completed before IC_ENABLE is set to 0 but has not taken effect 3、If the remote I2C
master terminates the transfer with a STOP condition before the DW_apb_i2c has a chance to NACK a
transfer, and IC_ENABLE[0] has been set to 0, then this bit will also be set to 1. 4、When read
as 0, DW_apb_i2c is deemed to have been disabled when there is master activity, or when the I2C
bus is idle. 5、The CPU can safely read this bit when IC_EN (bit 0) is read as 0.
*/
#define SLV_DISABLED_WHILE_BUSY_Msk                         (0x01 << SLV_DISABLED_WHILE_BUSY_Pos)
#define SLV_DISABLED_WHILE_BUSY_Pos                         25
#define SLV_DISABLED_WHILE_BUSY_PosL                        25
#define SLV_DISABLED_WHILE_BUSY_PosH                        25

/*!< 0x0034 SR
*1、ic_en Status. This bit always reflects the value driven on the output port ic_en. (1) When
read as 1, DW_apb_i2c is deemed to be in an enabled state. (2) When read as 0, DW_apb_i2c is
deemed completely inactive. 2、The CPU can safely read this bit anytime. When this bit is read
as 0, the CPU can safely read  SLV_RX_DATA_LOST (bit 2) and  SLV_DISABLED_WHILE_BUSY (bit 1).
*/
#define IC_EN_Msk                                           (0x01 << IC_EN_Pos)
#define IC_EN_Pos                                           24
#define IC_EN_PosL                                          24
#define IC_EN_PosH                                          24

/*!< 0x0034 SR
*1、Receive FIFO Level. Contains the number of valid data entries in the receive FIFO. 2、Range
Variable[x]: RX_ABW_P1 – 1 (RX_ABW_P1 = 4).
*/
#define RXFLR_Msk                                           (0x0F << RXFLR_Pos)
#define RXFLR_Pos                                           16
#define RXFLR_PosL                                          16
#define RXFLR_PosH                                          19

/*!< 0x0034 SR
*1、Transmit FIFO Level. Contains the number of valid data entries in the transmit FIFO.
2、Range Variable[x]: TX_ABW_P1 – 1, (TX_ABW_P1 = 4)
*/
#define TXFLR_Msk                                           (0x0F << TXFLR_Pos)
#define TXFLR_Pos                                           8
#define TXFLR_PosL                                          8
#define TXFLR_PosH                                          11

/*!< 0x0034 SR
*This bit indicates that SDA stuck at low is not recovered after the recovery mechanism. In Slave
mode, this register bit is not applicable.
*/
#define SDA_STUCK_NOT_RECOVERED_Msk                         (0x01 << SDA_STUCK_NOT_RECOVERED_Pos)
#define SDA_STUCK_NOT_RECOVERED_Pos                         7
#define SDA_STUCK_NOT_RECOVERED_PosL                        7
#define SDA_STUCK_NOT_RECOVERED_PosH                        7

/*!< 0x0034 SR
*Slave FSM Activity Status. When the Slave Finite State Machine (FSM) is not in the IDLE state,
this bit is set. (1) 0: Slave FSM is in IDLE state so the Slave part of DW_apb_i2c is not Active;
(2) 1: Slave FSM is not in IDLE state so the Slave part of DW_apb_i2c is Active.
*/
#define SLV_ACTIVITY_Msk                                    (0x01 << SLV_ACTIVITY_Pos)
#define SLV_ACTIVITY_Pos                                    6
#define SLV_ACTIVITY_PosL                                   6
#define SLV_ACTIVITY_PosH                                   6

/*!< 0x0034 SR
*Master FSM Activity Status. When the Master Finite State Machine (FSM) is not in the IDLE state,
this bit is set. (1) 0: Master FSM is in IDLE state so the Master part of DW_apb_i2c is not Active
; (2) 1: Master FSM is not in IDLE state so the Master part of DW_apb_i2c is Active.
*/
#define MST_ACTIVITY_Msk                                    (0x01 << MST_ACTIVITY_Pos)
#define MST_ACTIVITY_Pos                                    5
#define MST_ACTIVITY_PosL                                   5
#define MST_ACTIVITY_PosH                                   5

/*!< 0x0034 SR
*Receive FIFO Completely Full. When the receive FIFO is completely full, this bit is set. When
the receive FIFO contains one or more empty location, this bit is cleared.
*/
#define RFF_Msk                                             (0x01 << RFF_Pos)
#define RFF_Pos                                             4
#define RFF_PosL                                            4
#define RFF_PosH                                            4

/*!< 0x0034 SR
*Receive FIFO Not Empty. This bit is set when the receive FIFO contains one or more entries; it
is cleared when the receive FIFO is empty.
*/
#define RFNE_Msk                                            (0x01 << RFNE_Pos)
#define RFNE_Pos                                            3
#define RFNE_PosL                                           3
#define RFNE_PosH                                           3

/*!< 0x0034 SR
*Transmit FIFO Completely Empty. When the transmit FIFO is completely empty, this bit is set.
When it contains one or more valid entries, this bit is cleared. This bit field does not request
an interrupt.
*/
#define TFE_Msk                                             (0x01 << TFE_Pos)
#define TFE_Pos                                             2
#define TFE_PosL                                            2
#define TFE_PosH                                            2

/*!< 0x0034 SR
*Transmit FIFO Not Full. Set when the transmit FIFO contains one or more empty locations, and is
cleared when the FIFO is full.
*/
#define TFNF_Msk                                            (0x01 << TFNF_Pos)
#define TFNF_Pos                                            1
#define TFNF_PosL                                           1
#define TFNF_PosH                                           1

/*!< 0x0034 SR
*I2C Activity Status.
*/
#define SR_ACTIVITY_Msk                                        (0x01 << SR_ACTIVITY_Pos)
#define SR_ACTIVITY_Pos                                        0
#define SR_ACTIVITY_PosL                                       0
#define SR_ACTIVITY_PosH                                       0


/*!< 0x0038 TAR
*This bit indicates whether software performs a Device-ID or General Call or START BYTE command. (
1) 0: ignore bit 10 GC_OR_START and use IC_TAR normally. (2) 1: perform special I2C command as
specified in Device_ID or GC_OR_START bit.
*/
#define SPECIAL_Msk                                         (0x01 << SPECIAL_Pos)
#define SPECIAL_Pos                                         11
#define SPECIAL_PosL                                        11
#define SPECIAL_PosH                                        11

/*!< 0x0038 TAR
*If bit 11 (SPECIAL) is set to 1 and bit 13(Device-ID) is set to 0, then this bit indicates
whether a General Call or START byte command is to be performed by the DW_apb_i2c. (1) 0: General
Call Address - after issuing a General Call, only writes may be performed. Attempting to issue a
read command results in setting bit 6 (TX_ABRT) of the ISR register. The DW_apb_i2c remains in
General Call mode until the SPECIAL bit value (bit 11) is cleared. (2) 1: START BYTE
*/
#define GC_OR_START_Msk                                     (0x01 << GC_OR_START_Pos)
#define GC_OR_START_Pos                                     10
#define GC_OR_START_PosL                                    10
#define GC_OR_START_PosH                                    10

/*!< 0x0038 TAR
*1、This is the target address for any master transaction. When transmitting a General Call,
these bits are ignored. To generate a START BYTE, the CPU needs to write only once into these bits
. 2、If the IC_TAR and IC_SAR are the same, loopback exists but the FIFOs are shared between
master and slave, so full loopback is not feasible. Only one direction loopback mode is supported
(simplex), not duplex. A master cannot transmit to itself; it can transmit to only a slave.
*/
#define IC_TAR_Msk                                          (0x3FF << IC_TAR_Pos)
#define IC_TAR_Pos                                          0
#define IC_TAR_PosL                                         0
#define IC_TAR_PosH                                         9


/*!< 0x003c SAR
*1、The IC_SAR holds the slave address when the I2C is operating as a slave. For 7-bit addressing
, only IC_SAR[6:0] is used. 2、This register can be written only when the I2C interface is
disabled, which corresponds to the IC_ENABLE[0] register being set to 0. Writes at other times
have no effect. 3、Note: The default values cannot be any of the reserved address locations:
that is, 0x00 to 0x07, or 0x78 to 0x7f. The correct operation of the device is not guaranteed if
you program the IC_SAR or IC_TAR to a reserved value.
*/
#define IC_SAR_Msk                                          (0x3FF << IC_SAR_Pos)
#define IC_SAR_Pos                                          0
#define IC_SAR_PosL                                         0
#define IC_SAR_PosH                                         9


/*!< 0x0040 HS_MADDR
*1、This bit field holds the value of the I2C HS mode master code. HS-mode master codes are
reserved 8-bit codes (00001xxx) that are not used for slave addressing or other purposes. Each
master has its unique master code; up to eight high-speed mode masters can be present on the same
I2C bus system. Valid values are from 0 to 7.  2、This register goes away and becomes read-only
returning 0's if the IC_MAX_SPEED_MODE configuration parameter is set to either Standard (1) or
Fast (2). 3、This register can be written only when the I2C interface is disabled, which
corresponds to the IC_ENABLE[0] register being set to 0. Writes at other times have no effect.
4、Value After Reset: IC_DEFAULT_SLAVE_ADDR
*/
#define HS_MAR_Msk                                          (0x07 << HS_MAR_Pos)
#define HS_MAR_Pos                                          0
#define HS_MAR_PosL                                         0
#define HS_MAR_PosH                                         2


/*!< 0x0044 DATA_CMD
*1、Indicates the first data byte received after the address phase for receive transfer in
Master receiver or Slave receiver mode. 2、Exists: IC_FIRST_DATA_BYTE_STATUS == 1(0)
*/
#define FIRST_DATA_BYTE_Msk                                 (0x01 << FIRST_DATA_BYTE_Pos)
#define FIRST_DATA_BYTE_Pos                                 11
#define FIRST_DATA_BYTE_PosL                                11
#define FIRST_DATA_BYTE_PosH                                11

/*!< 0x0044 DATA_CMD
*Exists: IC_EMPTYFIFO_HOLD_MASTER_EN(0)
*/
#define RESTART_Msk                                         (0x01 << RESTART_Pos)
#define RESTART_Pos                                         10
#define RESTART_PosL                                        10
#define RESTART_PosH                                        10

/*!< 0x0044 DATA_CMD
*1、1 - STOP is issued after this byte, regardless of whether or not the Tx FIFO is empty. If
the Tx FIFO is not empty, the master immediately tries to start a new transfer by issuing a START
and arbitrating for the bus. 2、0 - STOP is not issued after this byte, regardless of  whether
or not the Tx FIFO is empty. If the Tx FIFO is not empty, the master continues the current
transfer by sending/receiving data bytes according to the value of the CMD bit. If the Tx FIFO is
empty, the master holds the SCL line low and stalls the bus until a new command is available in
the Tx FIFO. 3、Exists: IC_EMPTYFIFO_HOLD_MASTER_EN(0).
*/
#define STOP_Msk                                            (0x01 << STOP_Pos)
#define STOP_Pos                                            9
#define STOP_PosL                                           9
#define STOP_PosH                                           9

/*!< 0x0044 DATA_CMD
*1、This bit controls whether a read or a write is performed. This bit does not control the
direction when the DW_apb_i2c acts as a slave. It controls only the direction when it acts as a
master. 2、When a command is entered in the TX FIFO, this bit distinguishes the write and read
commands. In slave-receiver mode, this bit is a "don't care" because writes to this register are
not required. In slave-transmitter mode, a "0" indicates that the data in IC_DATA_CMD is to be
transmitted. 3、When programming this bit, you should remember the following: attempting to
perform a read operation after a General Call command has been sent results in a TX_ABRT
interrupt (bit 6 of the ISR register), unless bit 11 (SPECIAL) in the IC_TAR register has been
cleared. If a "1" is written to this bit after receiving a RD_REQ interrupt, then a TX_ABRT
interrupt occurs.
*/
#define CMD_Msk                                             (0x01 << CMD_Pos)
#define CMD_Pos                                             8
#define CMD_PosL                                            8
#define CMD_PosH                                            8

/*!< 0x0044 DATA_CMD
*1、This register contains the data to be transmitted or received on the I2C bus. If you are
writing to this register and want to perform a read, bits 7:0 (DAT) are ignored by the DW_apb_i2c
. However, when you read this register, these bits return the value of data received on the
DW_apb_i2c interface. 2、Memory Access: "(IC_HC_COUNT_VALUES==1) ? \"read-only\" : \"read-write
\""(0)
*/
#define DAT_Msk                                             (0xFF << DAT_Pos)
#define DAT_Pos                                             0
#define DAT_PosL                                            0
#define DAT_PosH                                            7


/*!< 0x0048 SCL_STUCK_AT_LOW_TIMEOUT
*DW_apb_i2c generate the interrupt to indicate SCL stuck at low if it detects the SCL stuck at
low for the in units of ic_clk period. This register can be written only when the I2C interface
is disabled which corresponds to the IC_ENABLE[0] register being set to 0. Writes at other times
have no effect.
*/
#define SCL_STUCK_AT_LOW_TIMEOUT_Msk                        (0XFFFFFFFF << SCL_STUCK_AT_LOW_TIMEOUT_Pos)
#define SCL_STUCK_AT_LOW_TIMEOUT_Pos                        0
#define SCL_STUCK_AT_LOW_TIMEOUT_PosL                       0
#define SCL_STUCK_AT_LOW_TIMEOUT_PosH                       31


/*!< 0x004c SDA_STUCK_AT_LOW_TIMEOUT
*DW_apb_i2c initiates the recovery of SDA line through enabling the SDA_STUCK_RECOVERY_EN
register bit, if it detects the SDA stuck at low for the IC_SDA_STUCK_LOW_TIMEOUT in units of
ic_clk period.
*/
#define SDA_STUCK_AT_LOW_TIMEOUT_Msk                        (0XFFFFFFFF << SDA_STUCK_AT_LOW_TIMEOUT_Pos)
#define SDA_STUCK_AT_LOW_TIMEOUT_Pos                        0
#define SDA_STUCK_AT_LOW_TIMEOUT_PosL                       0
#define SDA_STUCK_AT_LOW_TIMEOUT_PosH                       31


/*!< 0x0050 OE_RD
*null
*/
#define IC_CLK_IN_R_Msk                                     (0x01 << IC_CLK_IN_R_Pos)
#define IC_CLK_IN_R_Pos                                     3
#define IC_CLK_IN_R_PosL                                    3
#define IC_CLK_IN_R_PosH                                    3

/*!< 0x0050 OE_RD
*null
*/
#define IC_DATA_IN_R_Msk                                    (0x01 << IC_DATA_IN_R_Pos)
#define IC_DATA_IN_R_Pos                                    2
#define IC_DATA_IN_R_PosL                                   2
#define IC_DATA_IN_R_PosH                                   2

/*!< 0x0050 OE_RD
*null
*/
#define IC_CLK_OE_R_Msk                                     (0x01 << IC_CLK_OE_R_Pos)
#define IC_CLK_OE_R_Pos                                     1
#define IC_CLK_OE_R_PosL                                    1
#define IC_CLK_OE_R_PosH                                    1

/*!< 0x0050 OE_RD
*null
*/
#define IC_DATA_OE_R_Msk                                    (0x01 << IC_DATA_OE_R_Pos)
#define IC_DATA_OE_R_Pos                                    0
#define IC_DATA_OE_R_PosL                                   0
#define IC_DATA_OE_R_PosH                                   0


/*!< 0x0054 SLV_BCNT_CLR
*null
*/
#define SLV_RX_BCNT_TO_CLR_Msk                              (0x01 << SLV_RX_BCNT_TO_CLR_Pos)
#define SLV_RX_BCNT_TO_CLR_Pos                              3
#define SLV_RX_BCNT_TO_CLR_PosL                             3
#define SLV_RX_BCNT_TO_CLR_PosH                             3

/*!< 0x0054 SLV_BCNT_CLR
*null
*/
#define SLV_TX_BCNT_TO_CLR_Msk                              (0x01 << SLV_TX_BCNT_TO_CLR_Pos)
#define SLV_TX_BCNT_TO_CLR_Pos                              2
#define SLV_TX_BCNT_TO_CLR_PosL                             2
#define SLV_TX_BCNT_TO_CLR_PosH                             2

/*!< 0x0054 SLV_BCNT_CLR
*null
*/
#define SLV_RX_BCNT_ERR_CLR_Msk                             (0x01 << SLV_RX_BCNT_ERR_CLR_Pos)
#define SLV_RX_BCNT_ERR_CLR_Pos                             1
#define SLV_RX_BCNT_ERR_CLR_PosL                            1
#define SLV_RX_BCNT_ERR_CLR_PosH                            1

/*!< 0x0054 SLV_BCNT_CLR
*null
*/
#define SLV_TX_BCNT_ERR_CLR_Msk                             (0x01 << SLV_TX_BCNT_ERR_CLR_Pos)
#define SLV_TX_BCNT_ERR_CLR_Pos                             0
#define SLV_TX_BCNT_ERR_CLR_PosL                            0
#define SLV_TX_BCNT_ERR_CLR_PosH                            0


/*!< 0x0058 SLV_BCNT_TOUT
*IIC slave rx bit count timeout register.
*/
#define SLV_RX_BCNT_TOUT_Msk                                (0xFFFF << SLV_RX_BCNT_TOUT_Pos)
#define SLV_RX_BCNT_TOUT_Pos                                16
#define SLV_RX_BCNT_TOUT_PosL                               16
#define SLV_RX_BCNT_TOUT_PosH                               31

/*!< 0x0058 SLV_BCNT_TOUT
*IIC slave tx bit count timeout register.
*/
#define SLV_TX_BCNT_TOUT_Msk                                (0xFFFF << SLV_TX_BCNT_TOUT_Pos)
#define SLV_TX_BCNT_TOUT_Pos                                0
#define SLV_TX_BCNT_TOUT_PosL                               0
#define SLV_TX_BCNT_TOUT_PosH                               15


/**
* @}
* End of group HW_IIC
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_IIC_H_ */

