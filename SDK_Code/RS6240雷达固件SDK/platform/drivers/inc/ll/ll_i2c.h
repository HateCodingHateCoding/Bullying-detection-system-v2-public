/**
 **************************************************************************************************
 * @file    ll_i2c.h
 * @brief   I2C hadware define.
 * @attention
 *          Copyright (c) 2025 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _LL_I2C_H_
#define _LL_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "soc.h"
#include "types.h"
#include "hw_iic.h"

/** @addtogroup LL_Driver
 * @{
 */

/** @addtogroup ll_i2c
 * @{
 */

/**
 * @{
 */

/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */
struct i2c_timing_t {
    uint32_t sda_hold;
    uint32_t sda_setup;
    uint32_t scl_lcnt;
    uint32_t scl_hcnt;
    uint32_t scl_spklen;
};

typedef enum {
  I2C_CMD_WRITE              = 0x00U,
  I2C_CMD_READ               = CMD_Msk,
  I2C_CMD_WRITE_STOP         = STOP_Msk,
  I2C_CMD_WRITE_RESTART      = RESTART_Msk,
  I2C_CMD_WRITE_RESTART_STOP = STOP_Msk | RESTART_Msk,
  I2C_CMD_READ_STOP          = CMD_Msk | STOP_Msk,
  I2C_CMD_READ_RESTART       = CMD_Msk | RESTART_Msk,
  I2C_CMD_READ_RESTART_STOP  = CMD_Msk | STOP_Msk | RESTART_Msk,
} I2C_Cmd_t;

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
#define LL_I2C_FIFO_DEPTH                           8
#define LL_I2C_FIFO_THRESH_RX_FULL                  0
#define LL_I2C_FIFO_THRESH_TX_EMPTY                 2

#define LL_I2C_READ_DATA_MASK                       CMD_Msk
#define LL_I2C_STOP_DATA_MASK                       STOP_Msk
#define LL_I2C_RESTART_DATA_MASK                    RESTART_Msk

#define LL_I2C_STD_SPEED_MODE                       (0x1U)
#define LL_I2C_FAST_SPEED_MODE                      (0x2U)
#define LL_I2C_HIGH_SPEED_MODE                      (0x3U)

#define LL_I2C_SLV_RX_BCNT_TO_BIT                   (SLV_RX_BCNT_TO_Pos)
#define LL_I2C_SLV_TX_BCNT_TO_BIT                   (SLV_TX_BCNT_TO_Pos)
#define LL_I2C_SLV_RX_BCNT_ERR_BIT                  (SLV_RX_BCNT_ERR_Pos)
#define LL_I2C_SLV_TX_BCNT_ERR_BIT                  (SLV_TX_BCNT_ERR_Pos)
#define LL_I2C_SCL_STUCK_AT_LOW_BIT                 (SCL_STUCK_AT_LOW_Pos)
#define LL_I2C_RESTART_BIT                          (RESTART_DET_Pos)
#define LL_I2C_GEN_CALL_BIT                         (GEN_CALL_Pos)
#define LL_I2C_START_DET_BIT                        (START_DET_Pos)
#define LL_I2C_STOP_DET_BIT                         (STOP_DET_Pos)
#define LL_I2C_ACTIVITY_BIT                         (ISR_ACTIVITY_Pos)
#define LL_I2C_RX_DONE_BIT                          (RX_DONE_Pos)
#define LL_I2C_TX_ABRT_BIT                          (TX_ABRT_Pos)
#define LL_I2C_RD_REQ_BIT                           (RD_REQ_Pos)  /* slv*/
#define LL_I2C_TX_EMPTY_BIT                         (TX_EMPTY_Pos)
#define LL_I2C_TX_OVER_BIT                          (TX_OVER_Pos)
#define LL_I2C_RX_FUL_BIT                           (RX_FULL_Pos)
#define LL_I2C_RX_OVER_BIT                          (RX_OVER_Pos)
#define LL_I2C_RX_UNDER_BIT                         (RX_UNDER_Pos)

#define LL_I2C_INTR_TX_EMPTY_MASK                   (1 << M_TX_EMPTY_Pos)
#define LL_I2C_INTR_TX_OVRE_MASK                    (1 << M_TX_OVER_Pos)
#define LL_I2C_INTR_TX_ABRT_MASK                    (1 << M_TX_ABRT_Pos)

#define LL_I2C_INTR_STOP_MASK                       (1 << M_STOP_DET_Pos)

#define LL_I2C_INTR_RX_DONE_MASK                    (1 << M_RX_DONE_Pos)
#define LL_I2C_INTR_RX_FULL_MASK                    (1 << M_RX_FULL_Pos)
#define LL_I2C_INTR_RX_OVER_MASK                    (1 << M_RX_OVER_Pos)
#define LL_I2C_INTR_RX_UNDER_MASK                   (1 << M_RX_UNDER_Pos)
#define LL_I2C_INTR_RD_REQ_MASK                     (1 << M_RD_REQ_Pos)
#define LL_I2C_INTR_START_DET_MASK                  (1 << M_START_DET_Pos)
#define LL_I2C_INTR_RESTART_DET_MASK                (1 << M_RESTART_DET_Pos)

#define LL_I2C_STOP_END_MASK                        ((1 << LL_I2C_SLV_RX_BCNT_TO_BIT)  | \
                                                     (1 << LL_I2C_SLV_TX_BCNT_TO_BIT) | \
                                                     (1 << LL_I2C_SLV_RX_BCNT_ERR_BIT) | \
                                                     (1 << LL_I2C_SLV_TX_BCNT_ERR_BIT) | \
                                                     (1 << LL_I2C_SCL_STUCK_AT_LOW_BIT) | \
                                                     (1 << LL_I2C_STOP_DET_BIT) | \
                                                     (1 << LL_I2C_TX_ABRT_BIT) | \
                                                     (1 << LL_I2C_TX_OVER_BIT) | \
                                                     (1 << LL_I2C_RX_OVER_BIT) | \
                                                     (1 << LL_I2C_RX_UNDER_BIT))

#define LL_I2C_INTR_ERR_MASK                        (LL_I2C_INTR_TX_ABRT_MASK | \
                                                     LL_I2C_INTR_TX_OVRE_MASK | \
                                                     LL_I2C_INTR_RX_OVER_MASK | \
                                                     LL_I2C_INTR_RX_UNDER_MASK)

#define LL_I2C_INTR_MASTER_TX_MASK                  (LL_I2C_INTR_TX_EMPTY_MASK)
#define LL_I2C_INTR_MASTER_RX_MASK                  (LL_I2C_INTR_RX_FULL_MASK)
#define LL_I2C_INTR_SLAVE_TX_MASK                   (LL_I2C_INTR_TX_EMPTY_MASK)
#define LL_I2C_INTR_SLAVE_RX_MASK                   (LL_I2C_INTR_RX_DONE_MASK | \
                                                     LL_I2C_INTR_RD_REQ_MASK | \
                                                     LL_I2C_INTR_RESTART_DET_MASK| \
                                                     LL_I2C_INTR_START_DET_MASK | \
                                                     LL_I2C_INTR_RX_FULL_MASK)

#define LL_I2C_TX_ABORT_TX_DATA_NAK_BIT             ABRT_TXDATA_NOACK_Pos
#define LL_I2C_TX_ABORT_10BIT_ADDR2_NAK_BIT         ABRT_10ADDR2_NOACK_Pos
#define LL_I2C_TX_ABORT_10BIT_ADDR_NAK_BIT          ABRT_10ADDR1_NOACK_Pos
#define LL_I2C_TX_ABORT_7BIT_ADDR_NAK_BIT           ABRT_7B_ADDR_NOACK_Pos


#define LL_I2C_FCLCK_32M                            (0x00)
#define LL_I2C_FCLCK_160M                           (0x01)
#define LL_I2C_FCLCK_128M                           (0x02)
#define LL_I2C_FCLCK_CNT                            (0x03)

#define LL_I2C_FCLCK_32M_VALUE                      (32000000)
#define LL_I2C_FCLCK_128M_VALUE                     (128000000)
#define LL_I2C_FCLCK_160M_VALUE                     (160000000)

/** @defgroup I2C
  * @{
  */

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */
/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_Set_SpeedMode(i2c_dev_t *i2c, uint8_t mode)
{
    HW_SET_MSK_VAL(i2c->CR, SPEED_Msk, SPEED_Pos, mode);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_Set_MasterMode(i2c_dev_t *i2c)
{
    HW_SET_MSK(i2c->CR, MASTER_ENABLE_MASK);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_Set_SlaveMode(i2c_dev_t *i2c)
{
    HW_CLR_MSK(i2c->CR, MASTER_ENABLE_MASK);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_GetMode(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->CR, MASTER_MODE_Msk, MASTER_MODE_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_MasterIs10BitAddr(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->CR, IC_10BITADDR_MASTER_Msk, IC_10BITADDR_MASTER_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_SlaveIs10BitAddr(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->CR, IC_10BITADDR_SLAVE_Msk, IC_10BITADDR_SLAVE_Pos);
}


/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_EnableReStart(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->CR, IC_RESTART_EN_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_DisableReStart(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->CR, IC_RESTART_EN_Pos);
}


/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_EnableTxFifoEmpty(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->CR, TX_EMPTY_CTRL_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_DisableTxFifoEmpty(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->CR, TX_EMPTY_CTRL_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_EnableRxFifoHold(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->CR, RX_FIFO_FULL_HLD_CTRL_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_DisableRxFifoHold(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->CR, RX_FIFO_FULL_HLD_CTRL_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_SetMaster_10Bit_AddrMode(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->CR, IC_10BITADDR_MASTER_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_SetMaster_7Bit_AddrMode(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->CR, IC_10BITADDR_MASTER_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_SetSlave_10Bit_AddrMode(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->CR, IC_10BITADDR_SLAVE_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_SetSlave_7Bit_AddrMode(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->CR, IC_10BITADDR_SLAVE_Pos);
}


/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_ClearBusFeature(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->CR, BUS_CLEAR_FEATURE_CTRL_Pos);
}


/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_EnableRecoverySDAStuck(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->CR, SDA_STUCK_RECOVERY_ENABLE_Pos);
}


/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_SetMaster_Target_Addr(i2c_dev_t *i2c, uint16_t target)
{
    HW_SET_MSK_VAL(i2c->TAR, IC_TAR_Msk, IC_TAR_Pos, target);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_SetSlave_Slave_Addr(i2c_dev_t *i2c, uint16_t slave)
{
    HW_SET_MSK_VAL(i2c->SAR, IC_SAR_Msk, IC_SAR_Pos, slave);
}


/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_PutData(i2c_dev_t *i2c, uint16_t tx_data)
{
    // i2c->DATA_CMD = tx_data;
    HW_SET_MSK_VAL(i2c->DATA_CMD, CMD_Msk, CMD_Pos, tx_data);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_PutCmdData(i2c_dev_t *i2c, uint8_t tx_data, I2C_Cmd_t cmd)
{
    HW_SET_VAL(i2c->DATA_CMD, (uint32_t)tx_data | (uint32_t)cmd);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
uint8_t LL_I2C_GetCmdData(i2c_dev_t *i2c)
{
    return (uint8_t)(HW_GET_VAL(i2c->DATA_CMD) & DAT_Msk);
}


/**
 * @brief
 */
__HW_STATIC_INLINE
uint8_t LL_I2C_GetData(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->DATA_CMD, DAT_Msk, DAT_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
bool LL_I2C_IsFirstByteData(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->DATA_CMD, FIRST_DATA_BYTE_Msk, FIRST_DATA_BYTE_Pos);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_Set_Sspeed_Timing(i2c_dev_t *i2c, const struct i2c_timing_t *timing)
{
    HW_SET_MSK_VAL(i2c->SS_SCL_CNT_SET, IC_SS_SCL_LCNT_Msk, IC_SS_SCL_LCNT_Pos, timing->scl_lcnt);
    HW_SET_MSK_VAL(i2c->SS_SCL_CNT_SET, IC_SS_SCL_HCNT_Msk, IC_SS_SCL_HCNT_Pos, timing->scl_hcnt);
    HW_SET_MSK_VAL(i2c->IC_FS_SPKLEN,   IC_FS_SPKLEN_Msk,   IC_FS_SPKLEN_Pos,   timing->scl_spklen);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, IC_SDA_RX_HOLD_Msk, IC_SDA_RX_HOLD_Pos, timing->sda_hold);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, IC_SDA_TX_HOLD_Msk, IC_SDA_TX_HOLD_Pos, timing->sda_hold);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, SDA_SETUP_Msk,      SDA_SETUP_Pos,      timing->sda_setup);
}

/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_Set_Fspeed_Timing(i2c_dev_t *i2c, const struct i2c_timing_t *timing)
{
    HW_SET_MSK_VAL(i2c->FS_SCL_CNT_SET, IC_FS_SCL_LCNT_Msk, IC_FS_SCL_LCNT_Pos, timing->scl_lcnt);
    HW_SET_MSK_VAL(i2c->FS_SCL_CNT_SET, IC_FS_SCL_HCNT_Msk, IC_FS_SCL_HCNT_Pos, timing->scl_hcnt);
    HW_SET_MSK_VAL(i2c->IC_FS_SPKLEN,   IC_FS_SPKLEN_Msk,   IC_FS_SPKLEN_Pos,   timing->scl_spklen);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, IC_SDA_RX_HOLD_Msk, IC_SDA_RX_HOLD_Pos, timing->sda_hold);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, IC_SDA_TX_HOLD_Msk, IC_SDA_TX_HOLD_Pos, timing->sda_hold);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, SDA_SETUP_Msk,      SDA_SETUP_Pos,      timing->sda_setup);
}

#if 0
/**
 * @brief
 */
__HW_STATIC_INLINE
void LL_I2C_Set_Hspeed_Timing(i2c_dev_t *i2c, const struct i2c_timing_t *timing)
{
    HW_SET_MSK_VAL(i2c->HS_SCL_CNT_SET, IC_HS_SCL_LCNT_Msk, IC_HS_SCL_LCNT_Pos, timing->scl_lcnt);
    HW_SET_MSK_VAL(i2c->HS_SCL_CNT_SET, IC_HS_SCL_HCNT_Msk, IC_HS_SCL_HCNT_Pos, timing->scl_hcnt);
    HW_SET_MSK_VAL(i2c->IC_FS_SPKLEN,   IC_HS_SPKLEN_Msk,   IC_HS_SPKLEN_Pos,   timing->scl_spklen);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, IC_SDA_RX_HOLD_Msk, IC_SDA_RX_HOLD_Pos, timing->sda_hold);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, IC_SDA_TX_HOLD_Msk, IC_SDA_TX_HOLD_Pos, timing->sda_hold);
    HW_SET_MSK_VAL(i2c->SDA_HOLD_SETUP, SDA_SETUP_Msk,      SDA_SETUP_Pos,      timing->sda_setup);
}
#endif


/**
 * @brief i2c get abort source.
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_Get_Tx_Abort_Src(i2c_dev_t *i2c)
{
    return HW_GET_VAL(i2c->TX_ABRT_SOURCE);
}

/**
 * @brief i2c get all interrupt status.
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_Get_Intr_RawStat(i2c_dev_t *i2c)
{
    return HW_GET_VAL(i2c->ISR);
}

/**
 * @brief i2c get interrupt mask.
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_Get_Intr_Mask(i2c_dev_t *i2c)
{
    return HW_GET_VAL(i2c->IMR);
}

/**
 * @brief i2c enable interrupt mask.
 */
__HW_STATIC_INLINE
void LL_I2C_Enable_Intr_Mask(i2c_dev_t *i2c, uint32_t mask)
{
    HW_SET_MSK(i2c->IMR, mask);
}

/**
 * @brief i2c disable interrupt mask.
 */
__HW_STATIC_INLINE
void LL_I2C_Disable_Intr_Mask(i2c_dev_t *i2c, uint32_t mask)
{
    HW_CLR_MSK(i2c->IMR, mask);
}

/**
 * @brief i2c disable all interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_DisableAll_Intr(i2c_dev_t *i2c)
{
    HW_CLR_MSK(i2c->IMR, M_ALL_Msk);
}

/**
 * @brief i2c disable all interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_EnableAll_Intr(i2c_dev_t *i2c)
{
    HW_SET_MSK(i2c->IMR, M_ALL_Msk);
}

/**
 * @brief i2c set interrupt mask.
 */
__HW_STATIC_INLINE
void LL_I2C_Disable_Intr_TXE(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->IMR, M_TX_EMPTY_Pos);
}

/**
 * @brief i2c set interrupt mask.
 */
__HW_STATIC_INLINE
void LL_I2C_Enable_Intr_TXE(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->IMR, M_TX_EMPTY_Pos);
}

/**
 * @brief i2c set interrupt mask.
 */
__HW_STATIC_INLINE
void LL_I2C_Disable_Intr_RXF(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->IMR, M_RX_FULL_Pos);
}

/**
 * @brief i2c set interrupt mask.
 */
__HW_STATIC_INLINE
void LL_I2C_Enable_Intr_RXF(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->IMR, M_RX_FULL_Pos);
}

/**
 * @brief i2c get start interrupt mask.
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_isIntrStat_StartDet(uint32_t intr_stat, uint32_t intr_mask)
{
    return (!!(HW_TEST_BIT(intr_stat, M_START_DET_Pos) & HW_TEST_BIT(intr_mask, M_START_DET_Pos)));
}


/**
 * @brief i2c clear all interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearAll_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_INTR_Msk);
}

/**
 * @brief i2c clear interrupt mask.
 */
//__HW_STATIC_INLINE
//void LL_I2C_Clear_Intr(i2c_dev_t *i2c, uint32_t intr_mask)
//{
//    HW_SET_MSK(i2c->ICR, intr_mask);
//}

/**
 * @brief i2c clear under flow interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearRxUnder_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_RX_UNDER_Msk);
}

/**
 * @brief i2c clear rx flow interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearRxOver_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_RX_OVER_Msk);
}

/**
 * @brief i2c clear tx flow interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearTxOver_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_TX_OVER_Msk);
}

/**
 * @brief i2c clear read request interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearReadReq_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_RD_REQ_Msk);
}

/**
 * @brief i2c clear tx abort interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearTxAbort_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_TX_ABRT_Msk);
}

/**
 * @brief i2c clear rx done interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearRxDone_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_RX_DONE_Msk);
}

/**
 * @brief i2c clear activity interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearAct_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_ACTIVITY_Msk);
}

/**
 * @brief i2c clear stop det interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearStopDet_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_STOP_DET_Msk);
}

/**
 * @brief i2c clear start det interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearStartDet_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_START_DET_Msk);
}

/**
 * @brief i2c clear generic call interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearGenCall_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_GEN_CALL_Msk);
}

/**
 * @brief i2c clear Timeout interrupt.
 */
__HW_STATIC_INLINE
void LL_I2C_ClearSCLStuckDet_Intr(i2c_dev_t *i2c)
{
    HW_SET_VAL(i2c->ICR, CLR_SCL_STUCK_DET_Msk);
}

/**
 * @brief i2c set tx fifo level
 */
__HW_STATIC_INLINE
void LL_I2C_Set_RxFifo_Threshold(i2c_dev_t *i2c, uint8_t threshold)
{
    HW_SET_MSK_VAL(i2c->TL_SET, RX_TL_Msk, RX_TL_Pos, threshold);
}

/**
 * @brief i2c get tx fifo level.
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_Get_RxFifo_Threshold(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->TL_SET, RX_TL_Msk, RX_TL_Pos);
}

/**
 * @brief i2c set tx fifo level.
 */
__HW_STATIC_INLINE
void LL_I2C_Set_TxFifo_Threshold(i2c_dev_t *i2c, uint8_t threshold)
{
    HW_SET_MSK_VAL(i2c->TL_SET, TX_TL_Msk, TX_TL_Pos, threshold);
}

/**
 * @brief i2c get tx fifo level.
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_Get_TxFifo_Threshold(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->TL_SET, TX_TL_Msk, TX_TL_Pos);
}


/**
 * @brief i2c Enable
 */
__HW_STATIC_INLINE
void LL_I2C_Enable(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->ENABLE, ENABLE_Pos);
}

/**
 * @brief i2c Disable
 */
__HW_STATIC_INLINE
void LL_I2C_Disable(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->ENABLE, ENABLE_Pos);
}

/**
 * @brief i2c Close
 */
__HW_STATIC_INLINE
void LL_I2C_SetTxBlockDisable(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->ENABLE, TX_CMD_BLOCK_Pos);
}

/**
 * @brief i2c Close
 */
__HW_STATIC_INLINE
void LL_I2C_SetTxBlockEnable(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->ENABLE, TX_CMD_BLOCK_Pos);
}



/**
 * @brief i2c Close
 */
__HW_STATIC_INLINE
void LL_I2C_Close(i2c_dev_t *i2c)
{
    HW_SET_MSK(i2c->ICR,    CLR_INTR_Pos);
    HW_CLR_MSK(i2c->IMR,    M_ALL_Msk);
    HW_CLR_BIT(i2c->ENABLE, ENABLE_Pos);
}

/**
 * @brief I2C Get is Able
 */
__HW_STATIC_INLINE
bool LL_I2C_IsAble(i2c_dev_t *i2c)
{
    return ((0 != HW_TEST_BIT(i2c->ENABLE, ENABLE_Pos))
            && (0 != HW_TEST_BIT(i2c->SR, IC_EN_Pos)));
}

/**
 * @brief I2C Get Tx Fifo empty (0: not empyt 1: empty)
 */
__HW_STATIC_INLINE
bool LL_I2C_isTx_Fifo_Empty(i2c_dev_t *i2c)
{
    return (!!HW_TEST_BIT(i2c->SR, TFE_Pos));
}

/**
 * @brief I2C Get Tx Fifo full(0: full 1: not full)
 */
__HW_STATIC_INLINE
bool LL_I2C_isTx_Fifo_Full(i2c_dev_t *i2c)
{
    return (!HW_TEST_BIT(i2c->SR, TFNF_Pos));
}

/**
 * @brief I2C Get Rx Fifo full (0: not full 1: full)
 */
__HW_STATIC_INLINE
bool LL_I2C_isRx_Fifo_Full(i2c_dev_t *i2c)
{
    return (!!HW_TEST_BIT(i2c->SR, RFF_Pos));
}

/**
 * @brief I2C Get Rx Fifo empty(1: rx fifo not empty)
 */
__HW_STATIC_INLINE
bool LL_I2C_isRx_Fifo_Empty(i2c_dev_t *i2c)
{
    return (!HW_TEST_BIT(i2c->SR, RFNE_Pos));
}

/**
 * @brief Get I2C tx water level
 */
__HW_STATIC_INLINE volatile
uint32_t LL_I2C_Tx_Fifo_Level(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->SR, TXFLR_Msk, TXFLR_Pos);
}

/**
 * @brief Get I2C rx water level
 */
__HW_STATIC_INLINE volatile
uint32_t LL_I2C_Rx_Fifo_Level(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->SR, RXFLR_Msk, RXFLR_Pos);
}

/**
 * @brief Get i2c Transmit ending
 */
__HW_STATIC_INLINE
volatile bool LL_I2C_IsActivity(i2c_dev_t *i2c)
{
    return !!HW_TEST_BIT(i2c->SR, SR_ACTIVITY_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_I2C_Get_Status(i2c_dev_t *i2c)
{
    return HW_GET_VAL(i2c->SR);
}

__HW_STATIC_INLINE
uint32_t LL_I2C_Mst_IsActivity(i2c_dev_t *i2c)
{
    return HW_TEST_BIT(i2c->SR, MST_ACTIVITY_Pos);
}

__HW_STATIC_INLINE
void LL_I2C_Set_Timeout_SCL_StuckAtLow(i2c_dev_t *i2c, uint32_t timeout)
{
    HW_SET_VAL(i2c->SCL_STUCK_AT_LOW_TIMEOUT, timeout);
}

__HW_STATIC_INLINE
uint32_t LL_I2C_Get_Timeout_SCL_StuckAtLow(i2c_dev_t *i2c)
{
    return HW_GET_VAL(i2c->SCL_STUCK_AT_LOW_TIMEOUT);
}

__HW_STATIC_INLINE
void LL_I2C_Set_Timeout_SDA_StuckAtLow(i2c_dev_t *i2c, uint32_t timeout)
{
    HW_SET_VAL(i2c->SDA_STUCK_AT_LOW_TIMEOUT, timeout);
}

__HW_STATIC_INLINE
uint32_t LL_I2C_Get_Timeout_SDA_StuckAtLow(i2c_dev_t *i2c)
{
    return HW_GET_VAL(i2c->SDA_STUCK_AT_LOW_TIMEOUT);
}

/* This bit field holds the value of the I2C HS mode master code  */
__HW_STATIC_INLINE
void LL_I2C_Set_HighSpeedMsterCode(i2c_dev_t *i2c, uint32_t masterCode)
{
    HW_SET_MSK_VAL(i2c->HS_MADDR, HS_MAR_Msk, HS_MAR_Pos, masterCode);
}

__HW_STATIC_INLINE
uint32_t LL_I2C_Get_HighSpeedMsterCode(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->HS_MADDR, HS_MAR_Msk, HS_MAR_Pos);
}



#if (CONFIG_I2C_TRANS_DMA_ENABLE == 1)
/**
 * @brief Set i2c DMA Tx Fifo Level
 */
__HW_STATIC_INLINE
void LL_I2C_Set_TxFifoLevel_DMA(i2c_dev_t *i2c, uint32_t tx_level)
{
    HW_SET_MSK_VAL(i2c->TL_SET, TX_TL_Msk, TX_TL_Pos, tx_level);
}

/**
 * @brief Set i2c DMA Rx Fifo Level
 */
__HW_STATIC_INLINE
void LL_I2C_Set_RxFifoLevel_DMA(i2c_dev_t *i2c, uint32_t rx_level)
{
    HW_SET_MSK_VAL(i2c->TL_SET, RX_TL_Msk, RX_TL_Pos, rx_level);
}

/**
 * @brief Get i2c DMA Tx Fifo Level
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_Get_TxFifoLevel_DMA(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->TL_SET, TX_TL_Msk, TX_TL_Pos);
}

/**
 * @brief Get i2c DMA Rx Fifo Level
 */
__HW_STATIC_INLINE
uint32_t LL_I2C_Get_RxFifoLevel_DMA(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->TL_SET, RX_TL_Msk, RX_TL_Pos);
}


/**
 * @brief i2c DMA Tx Enable
 */
__HW_STATIC_INLINE
void LL_I2C_EnableTxDMA(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->DMA_CR, TDMAE_Pos);
}

/**
 * @brief i2c DMA Tx Disable
 */
__HW_STATIC_INLINE
void LL_I2C_DisableTxDMA(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->DMA_CR, TDMAE_Pos);
}

/**
 * @brief i2c DMA Rx Enable
 */
__HW_STATIC_INLINE
void LL_I2C_EnableRxDMA(i2c_dev_t *i2c)
{
    HW_SET_BIT(i2c->DMA_CR, RDMAE_Pos);
}

/**
 * @brief i2c DMA Rx Disable
 */
__HW_STATIC_INLINE
void LL_I2C_DisableRxDMA(i2c_dev_t *i2c)
{
    HW_CLR_BIT(i2c->DMA_CR, RDMAE_Pos);
}

/**
 * @brief Get I2C tx water level
 */
__HW_STATIC_INLINE volatile
uint32_t LL_I2C_Tx_Fifo_Level_DMA(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->DMA_CR, DMATDL_Msk, DMATDL_Pos);
}

/**
 * @brief Get I2C rx water level
 */
__HW_STATIC_INLINE volatile
uint32_t LL_I2C_Rx_Fifo_Level_DMA(i2c_dev_t *i2c)
{
    return HW_GET_MSK_VAL(i2c->DMA_CR, DMARDL_Msk, DMARDL_Pos);
}
#endif // CONFIG_I2C_DMA_ENABLE
/**
 * @}
 */




/**
 * @} ll_i2c
 */


/**
 * @} LL_Driver
 */

#ifdef __cplusplus
}
#endif

#endif /* _LL_I2C_H_ */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */

