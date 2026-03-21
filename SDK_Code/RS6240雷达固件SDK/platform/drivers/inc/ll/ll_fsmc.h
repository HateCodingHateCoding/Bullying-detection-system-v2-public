/**
 **************************************************************************************************
 * @file    psic_ll_fsmc.h
 * @brief   Flexible static memory controoler define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_FSMC_H
#define _PSIC_LL_FSMC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "types.h"
#include "soc.h"
#include "hw_fsmc.h"

/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */

 
#define FSMC_DEV                       ((fsmc_dev_t *)  FCTRL_BASE_ADDR) /*!< 0x4002 1000 */


/** @defgroup FSMC master or slave
  * @{
  */
#define LL_FSMC_SLV                         (0 << SSI_TYPE_PosL)
#define LL_FSMC_MST                         (1 << SSI_TYPE_PosL)
/**
  * @}
  */


/** @defgroup FSMC frame format
  * @{
  */
#define LL_FSMC_FRAME_FORMAT_STD            0
#define LL_FSMC_FRAME_FORMAT_DUAL           1
#define LL_FSMC_FRAME_FORMAT_QUAD           2
#define LL_FSMC_FRAME_FORMAT_OCTAL          3
/**
  * @}
  */

/** @defgroup FSMC conrtol fram size
  * @{
  */
#define LL_FSMC_CFS_BIT(n)                  ((n) - 1)
/**
  * @}
  */

/** @defgroup FSMC transfer mode
  * @{
  */
#define LL_FSMC_TRANS_MODE_TX_AND_RX        0
#define LL_FSMC_TRANS_MODE_TX_ONLY          1
#define LL_FSMC_TRANS_MODE_RX_ONLY          2
#define LL_FSMC_TRANS_MODE_EEPROM_READ      3
#define LL_FSMC_RX_NUME_MAX                 32768
/**
  * @}
  */

/** @defgroup FSMC clock polarity
  * @{
  */
#define LL_FSMC_CLK_POLARITY_H              (0 << SCPOL_PosL)
#define LL_FSMC_CLK_POLARITY_L              (1 << SCPOL_PosL)
/**
  * @}
  */

/** @defgroup FSMC clock phase
  * @{
  */
#define LL_FSMC_CLK_PHASE_START             (0 << SCPH_PosL)
#define LL_FSMC_CLK_PHASE_MIDDLE            (1 << SCPH_PosL)
/**
  * @}
  */

/** @defgroup FSMC frame type
  * @{
  */
#define LL_FSMC_FRAME_TYPE_SPI              0
#define LL_FSMC_FRAME_TYPE_TI               1
#define LL_FSMC_FRAME_TYPE_MW               2
/**
  * @}
  */

/** @defgroup FSMC data frame size
  * @{
  */
#define LL_FSMC_DFS_BIT(n)                  ((n) - 1)
/**
  * @}
  */

/** @defgroup FSMC microwire control direct
  * @{
  */
#define LL_FSMC_MWC_DIRECT_TX               0
#define LL_FSMC_MWC_DIRECT_RX               1
/**
  * @}
  */

/** @defgroup FSMC data frame size
  * @{
  */
#define LL_FSMC_FIFO_LEVEL(n)                  ((n) - 1)
/**
  * @}
  */


/** @defgroup FSMC status flags
  * @{
  */
#define LL_FSMC_STATUS_BUSY                 HW_BIT(0)
#define LL_FSMC_STATUS_TXNF                 HW_BIT(1)
#define LL_FSMC_STATUS_TXE                  HW_BIT(2)
#define LL_FSMC_STATUS_RXNE                 HW_BIT(3)
#define LL_FSMC_STATUS_TXF                  HW_BIT(4)
#define LL_FSMC_STATUS_TXER                 HW_BIT(5)
#define LL_FSMC_STATUS_DCOL                 HW_BIT(6)
/**
  * @}
  */

/** @defgroup FSMC IT flags
  * @{
  */
#define LL_FSMC_IT_TXE                      HW_BIT(0)
#define LL_FSMC_IT_TXO                      HW_BIT(1)
#define LL_FSMC_IT_RXU                      HW_BIT(2)
#define LL_FSMC_IT_RXO                      HW_BIT(3)
#define LL_FSMC_IT_RXF                      HW_BIT(4)
#define LL_FSMC_IT_MST                      HW_BIT(5)
#define LL_FSMC_IT_XRXO                     HW_BIT(6)

#define LL_FSMC_ITE_TXE                     HW_BIT(16)
#define LL_FSMC_ITE_TXO                     HW_BIT(17)
#define LL_FSMC_ITE_RXU                     HW_BIT(18)
#define LL_FSMC_ITE_RXO                     HW_BIT(19)
#define LL_FSMC_ITE_RXF                     HW_BIT(20)
#define LL_FSMC_ITE_MST                     HW_BIT(21)
#define LL_FSMC_ITE_XRXO                    HW_BIT(22)

/**
  * @}
  */

/** @defgroup FSMC address and instruction transfer format
  * @{
  */
#define LL_FSMC_INST_STD_ADDR_STD          0
#define LL_FSMC_INST_STD_ADDR_CFG          1
#define LL_FSMC_INST_CFG_ADDR_CFG          2
/**
  * @}
  */

/** @defgroup FSMC address length
  * @{
  */
#define LL_FSMC_ADDR_LEN(n)                 ((n) / 4)
#define LL_FSMC_ADDR_LEN_0                  0
#define LL_FSMC_ADDR_LEN_4                  1
#define LL_FSMC_ADDR_LEN_8                  2
#define LL_FSMC_ADDR_LEN_12                 3
#define LL_FSMC_ADDR_LEN_16                 4
#define LL_FSMC_ADDR_LEN_20                 5
#define LL_FSMC_ADDR_LEN_24                 6
#define LL_FSMC_ADDR_LEN_28                 7
#define LL_FSMC_ADDR_LEN_32                 8
#define LL_FSMC_ADDR_LEN_36                 9
#define LL_FSMC_ADDR_LEN_40                 10
#define LL_FSMC_ADDR_LEN_44                 11
#define LL_FSMC_ADDR_LEN_48                 12
#define LL_FSMC_ADDR_LEN_52                 13
#define LL_FSMC_ADDR_LEN_56                 14
#define LL_FSMC_ADDR_LEN_60                 15
/**
  * @}
  */

/** @defgroup FSMC instruction length
  * @{
  */
#define LL_FSMC_INST_LEN(n)                 ((n) / 4)
#define LL_FSMC_INST_LEN_0                  0
#define LL_FSMC_INST_LEN_4                  1
#define LL_FSMC_INST_LEN_8                  2
#define LL_FSMC_INST_LEN_16                 3
/**
  * @}
  */

/** @defgroup FSMC mode bits length
  * @{
  */
#define LL_FSMC_MDB_LEN(n)                 ((n) / 4)
/**
  * @}
  */

/** @defgroup FSMC wait cycles length
  * @{
  */
#define LL_FSMC_WAIT_CYCLES(n)                 ((n))
/**
  * @}
  */


/** @defgroup FSMC data mask
  * @{
  */
#define LL_FSMC_BITS_MASK(n)                ((uint32_t)((1 << (n)) - 1))
#define LL_FSMC_BITS_MSK_BYTE                      (0xFF)
/**
  * @}
  */

/** @defgroup FSMC rx sample delay
  * @{
  */
#define LL_FSMC_SAMPLE_DELAY(n)             ((n))
#define LL_FSMC_SAMPLE_DELAY_0              0
#define LL_FSMC_SAMPLE_DELAY_1              1
#define LL_FSMC_SAMPLE_DELAY_2              2
#define LL_FSMC_SAMPLE_DELAY_3              3
#define LL_FSMC_SAMPLE_DELAY_4              4
/**
  * @}
  */

#define LL_FSMC_SELECT_CS0                 0x01
#define LL_FSMC_SELECT_CS1                 0x02

#define LL_FSMC_XIP_ENC_KEY_SIZE           16
#define LL_FSMC_XIP_ENC_KEY_SEL_EFUSE      0
#define LL_FSMC_XIP_ENC_KEY_SEL_CONFG      1

#define LL_FSMC_XIP_ENC_SECTION_CNT         3
/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup FSMC_ll_exported_functions FSMC exported functions
  * @{
  */

/** @defgroup common functions
  * @{
  */
__HW_STATIC_INLINE
void LL_FSMC_Enable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->CTRL, SSI_START_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_Disable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->CTRL, SSI_START_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_IsEnabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->CTRL, SSI_START_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetPrescaler(fsmc_dev_t *fsmc, uint32_t prescaler)
{
    HW_SET_MSK_VAL(fsmc->CLOCK_CTRL, CDR_Msk, CDR_PosL, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetPrescaler(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->CLOCK_CTRL, CDR_Msk, CDR_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetClockPhase(fsmc_dev_t *fsmc, uint32_t phase_msk)
{
    HW_SET_MSK(fsmc->CONFIG, phase_msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetClockPhase(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->CONFIG, SCPH_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetClockPolarity(fsmc_dev_t *fsmc, uint32_t polarity_msk)
{
    HW_SET_MSK(fsmc->CONFIG, polarity_msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetClockPolarity(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->CONFIG, SCPOL_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetFrameFormat(fsmc_dev_t *fsmc, uint32_t format)
{
    HW_SET_MSK_VAL(fsmc->CONFIG, SPIMODE_Msk, SPIMODE_PosL, format);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetFrameFormat(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->CONFIG, SPIMODE_Msk, SPIMODE_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetMasterSlave(fsmc_dev_t *fsmc, uint32_t master_msk)
{
    HW_SET_MSK(fsmc->CTRL, master_msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetMasterSlave(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->CTRL, SSI_TYPE_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetFrameType(fsmc_dev_t *fsmc, uint32_t type)
{
    HW_SET_MSK_VAL(fsmc->CONFIG, COM_FRF_Msk, COM_FRF_PosL, type);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetFrameType(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->CONFIG, COM_FRF_Msk, COM_FRF_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetDataWidth(fsmc_dev_t *fsmc, uint32_t width)
{
  HW_SET_MSK_VAL(fsmc->CONFIG, DFW_Msk, DFW_PosL, width);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetDataWidth(fsmc_dev_t *fsmc)
{
  return HW_GET_MSK_VAL(fsmc->CONFIG, DFW_Msk, DFW_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_EnableBigendian(fsmc_dev_t *fsmc)
{
  HW_SET_MSK_VAL(fsmc->CONFIG, BIGENDIAN_Msk, BIGENDIAN_PosL, 1);
}

__HW_STATIC_INLINE
void LL_FSMC_DisableBigendian(fsmc_dev_t *fsmc)
{
  HW_SET_MSK_VAL(fsmc->CONFIG, BIGENDIAN_Msk, BIGENDIAN_PosL, 0);
}

/** common functions
  * @}
  */


/** @defgroup nomal functions
  * @{
  */
__HW_STATIC_INLINE
void LL_FSMC_WriteData(fsmc_dev_t *fsmc, uint32_t data, uint32_t mask)
{
    HW_SET_VAL(fsmc->DB, (data & mask));
}

__HW_STATIC_INLINE
void LL_FSMC_WriteDataByte(fsmc_dev_t *fsmc, uint8_t data)
{
    HW_SET_VAL(fsmc->DB, data);
}

__HW_STATIC_INLINE
void LL_FSMC_WriteDataWord(fsmc_dev_t *fsmc, uint32_t data)
{
    HW_SET_VAL(fsmc->DB, data);
}


__HW_STATIC_INLINE
uint32_t LL_FSMC_ReadData(fsmc_dev_t *fsmc, uint32_t mask)
{
    return (HW_GET_VAL(fsmc->DB) & mask);
}

__HW_STATIC_INLINE
uint8_t LL_FSMC_ReadDataByte(fsmc_dev_t *fsmc)
{
    return (uint8_t)HW_GET_VAL(fsmc->DB);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_ReadDataWord(fsmc_dev_t *fsmc)
{
    return (uint32_t)HW_GET_VAL(fsmc->DB);
}


__HW_STATIC_INLINE
void LL_FSMC_SetRxFIFOThreshold(fsmc_dev_t *fsmc, uint32_t threshold)
{
    HW_SET_MSK_VAL(fsmc->RXF_CTL, RXFITL_Msk, RXFITL_PosL, threshold);
}

__HW_STATIC_INLINE
void LL_FSMC_SetTxFIFOThreshold(fsmc_dev_t *fsmc, uint32_t threshold)
{
    HW_SET_MSK_VAL(fsmc->TXF_CTL, TXFITL_Msk, TXFITL_PosL, threshold);
}

__HW_STATIC_INLINE
void LL_FSMC_SetTxStartFIFOLevel(fsmc_dev_t *fsmc, uint32_t level)
{
    HW_SET_MSK_VAL(fsmc->TXF_CTL, TXFSL_Msk, TXFSL_PosL, level);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetRxFIFOThreshold(fsmc_dev_t *fsmc)
{

    return HW_GET_MSK_VAL(fsmc->RXF_CTL, RXFITL_Msk, RXFITL_PosL);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetTxFIFOThreshold(fsmc_dev_t *fsmc)
{

    return HW_GET_MSK_VAL(fsmc->TXF_CTL, TXFITL_Msk, TXFITL_PosL);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetTxStartFIFOLevel(fsmc_dev_t *fsmc)
{

    return HW_GET_MSK_VAL(fsmc->TXF_CTL, TXFSL_Msk, TXFITL_PosL);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetRxFIFOLevel(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->FIFO_STATUS, RXFS_Msk, RXFS_PosL);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetTxFIFOLevel(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->FIFO_STATUS, TXFS_Msk, TXFS_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_DMAEnableTx(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->DMA_EN, DMATE_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_DMADisableTx(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->DMA_EN, DMATE_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_DMAIsEnabledTx(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->DMA_EN, DMATE_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_DMAEnableRx(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->DMA_EN, DMARE_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_DMADisableRx(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->DMA_EN, DMARE_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_DMAIsEnabledRx(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->DMA_EN, DMARE_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetDMATxLevel(fsmc_dev_t *fsmc, uint32_t inst)
{
    HW_SET_MSK_VAL(fsmc->DMATR, DMATRL_Msk, DMATRL_PosL, inst);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetDMATxLevel(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->DMATR, DMATRL_Msk, DMATRL_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetDMARxLevel(fsmc_dev_t *fsmc, uint32_t inst)
{
    HW_SET_MSK_VAL(fsmc->DMARR, DMARRL_Msk, DMARRL_PosL, inst);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetDMARxLevel(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->DMARR, DMARRL_Msk, DMARRL_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetTransferMode(fsmc_dev_t *fsmc, uint32_t mode)
{
    HW_SET_MSK_VAL(fsmc->CTRL, TTYP_Msk, TTYP_PosL, mode);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetTransferMode(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->CTRL, TTYP_Msk, TTYP_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetRxDataNumber(fsmc_dev_t *fsmc, uint32_t number)
{
    HW_SET_MSK_VAL(fsmc->RECEIVE_DATA_NUM, REC_DATA_NUM_Msk, REC_DATA_NUM_PosL, number - 1);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetRxDataNumber(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->RECEIVE_DATA_NUM, REC_DATA_NUM_Msk, REC_DATA_NUM_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_EnableSlaveSelect(fsmc_dev_t *fsmc, uint8_t dev_num)
{
    HW_SET_MSK_VAL(fsmc->CTRL, CSE_Msk, CSE_PosL, dev_num);
}

__HW_STATIC_INLINE
void LL_FSMC_DisableSlaveSelect(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->CTRL, CSE_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_StatusIsSet(fsmc_dev_t *fsmc, uint32_t mask)
{
    return HW_GET_MSK(fsmc->SSR, mask);
}

__HW_STATIC_INLINE
void LL_FSMC_ITEnable(fsmc_dev_t *fsmc, uint32_t mask)
{
    HW_SET_MSK(fsmc->INT_CS, mask);
}

__HW_STATIC_INLINE
void LL_FSMC_ITDisable(fsmc_dev_t *fsmc, uint32_t mask)
{
    HW_CLR_MSK(fsmc->INT_CS, mask);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_ITIsEnabled(fsmc_dev_t *fsmc, uint32_t mask)
{
    return HW_GET_MSK(fsmc->INT_CS, mask);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_ITIsActiveFlag(fsmc_dev_t *fsmc, uint32_t mask)
{
    return HW_GET_MSK(fsmc->INT_CS, mask);
}

__HW_STATIC_INLINE
void LL_FSMC_ITFlagClearALL(fsmc_dev_t *fsmc)
{
    (void)HW_GET_MSK(fsmc->INT_CLEAR, CI_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_ITFlagClearMST(fsmc_dev_t *fsmc)
{
    (void)HW_GET_MSK_VAL(fsmc->INT_CLEAR, MSTICR_Msk, MSTICR_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_ITFlagClearRXU(fsmc_dev_t *fsmc)
{
    (void)HW_GET_MSK_VAL(fsmc->INT_CLEAR, RXUICR_Msk, RXUICR_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_ITFlagClearRXO(fsmc_dev_t *fsmc)
{
    (void)HW_GET_MSK_VAL(fsmc->INT_CLEAR, RXOICR_Msk, RXOICR_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_ITFlagClearTXE(fsmc_dev_t *fsmc)
{
    (void)HW_GET_MSK_VAL(fsmc->INT_CLEAR, TXOICR_Msk, TXOICR_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetAddrLen(fsmc_dev_t *fsmc, uint32_t len)
{
    HW_SET_MSK_VAL(fsmc->QSPI_ENHANCE, ADDR_B_Msk, ADDR_B_PosL, len);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetAddrLen(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->QSPI_ENHANCE, ADDR_B_Msk, ADDR_B_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetInstLen(fsmc_dev_t *fsmc, uint32_t len)
{
    HW_SET_MSK_VAL(fsmc->QSPI_ENHANCE, INST_B_Msk, INST_B_PosL, len);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetInstLen(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->QSPI_ENHANCE, INST_B_Msk, INST_B_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetDummyCycles(fsmc_dev_t *fsmc, uint32_t cycle)
{
    HW_SET_MSK_VAL(fsmc->QSPI_ENHANCE, DUMMY_Msk, DUMMY_PosL, cycle);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetDummyCycles(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->QSPI_ENHANCE, DUMMY_Msk, DUMMY_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SetTransType(fsmc_dev_t *fsmc, uint32_t trans)
{
    HW_SET_MSK_VAL(fsmc->QSPI_ENHANCE, TRANS_FORM_Msk, TRANS_FORM_PosL, trans);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetTransType(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->QSPI_ENHANCE, TRANS_FORM_Msk, TRANS_FORM_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_AddHalfRxSampleDelay(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->SAMPLE_CTL, RSE_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_RemoveHalfRxSampleDelay(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->SAMPLE_CTL, RSE_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_SetRxSampleDelay(fsmc_dev_t *fsmc, uint32_t delay)
{
    HW_SET_MSK_VAL(fsmc->SAMPLE_CTL, RSD_Msk, RSD_PosL, delay);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_GetRxSampleDelay(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->SAMPLE_CTL, RSD_Msk, RSD_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_StretchEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->QSPI_ENHANCE, CLK_STRETCH_EN_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_StretchDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->QSPI_ENHANCE, CLK_STRETCH_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_StretchIsabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->QSPI_ENHANCE, CLK_STRETCH_EN_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_SlaveSelectToggleEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK_VAL(fsmc->CTRL, SCSTE_Msk, SCSTE_PosL, 0x01);
}

__HW_STATIC_INLINE
void LL_FSMC_SlaveSelectToggleDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->CTRL, SCSTE_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_SlaveSelectToggleIsabled(fsmc_dev_t *fsmc)
{
    return (HW_GET_MSK_VAL(fsmc->CTRL, SCSTE_Msk, SCSTE_PosL) & 0x01);
}

__HW_STATIC_INLINE
void LL_FSMC_FullDuplexEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->CTRL, FULL_DUPLEX_EN_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_FullDuplexDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->CTRL, FULL_DUPLEX_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_FullDuplexIsabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->CTRL, FULL_DUPLEX_EN_PosL);
}


/** nomal functions
  * @}
  */



/** @defgroup XIP functions
  * @{
  */

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetIncrInst(fsmc_dev_t *fsmc, uint32_t inst)
{
    HW_SET_MSK_VAL(fsmc->XIP_INCR_INST, INCR_INST_Msk, INCR_INST_PosL, inst);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetIncrInst(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_INCR_INST, INCR_INST_Msk, INCR_INST_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetWrapInst(fsmc_dev_t *fsmc, uint32_t inst)
{
    HW_SET_MSK_VAL(fsmc->XIP_WRAP_INST, WRAP_INST_Msk, WRAP_INST_PosL, inst);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetWrapInst(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_WRAP_INST, WRAP_INST_Msk, WRAP_INST_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetContModeSSTimeout(fsmc_dev_t *fsmc, uint32_t timeout)
{
    HW_SET_MSK_VAL(fsmc->XIP_TIME_OUT, XIP_TIME_OUT_Msk, XIP_TIME_OUT_PosL, timeout);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetContModeSSTimeout(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_TIME_OUT, XIP_TIME_OUT_Msk, XIP_TIME_OUT_PosL);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetAndClearFlagRXO(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XRFOIC, XRFOICR_Msk, XRFOICR_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_EnableSlaveSelect(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->XIP_SER, SLAVE_SELECT_ENABLE_FLAG_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_DisableSlaveSelect(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->XIP_SER, SLAVE_SELECT_ENABLE_FLAG_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetFrameFormat(fsmc_dev_t *fsmc, uint32_t format)
{
    HW_SET_MSK_VAL(fsmc->XIP_CONFIG, XIP_MODE_Msk, XIP_MODE_PosL, format);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetFrameFormat(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_CONFIG, XIP_MODE_Msk, XIP_MODE_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetTransType(fsmc_dev_t *fsmc, uint32_t trans)
{
    HW_SET_MSK_VAL(fsmc->XIP_CONFIG, XIP_TRANS_FORM_Msk, XIP_TRANS_FORM_PosL, trans);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetTransType(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_CONFIG, XIP_TRANS_FORM_Msk, XIP_TRANS_FORM_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetAddrLen(fsmc_dev_t *fsmc, uint32_t len)
{
    HW_SET_MSK_VAL(fsmc->XIP_CONFIG, XIP_ADDR_B_Msk, XIP_ADDR_B_PosL, len);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetAddrLen(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_CONFIG, XIP_ADDR_B_Msk, XIP_ADDR_B_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetInstLen(fsmc_dev_t *fsmc, uint32_t len)
{
    HW_SET_MSK_VAL(fsmc->XIP_CONFIG, XIP_INST_B_Msk, XIP_INST_B_PosL, len);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetInstLen(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_CONFIG, XIP_INST_B_Msk, XIP_INST_B_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_ModeBitsEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->XIP_CONFIG, XIP_MODE_BIT_EN_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_ModeBitsDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->XIP_CONFIG, XIP_MODE_BIT_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_ModeBitsIsEnabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->XIP_CONFIG, XIP_MODE_BIT_EN_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetModeBitsLen(fsmc_dev_t *fsmc, uint32_t len)
{
    HW_SET_MSK_VAL(fsmc->XIP_CONFIG, XIP_MBL_Msk, XIP_MBL_PosL, len);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetModeBitsLen(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_CONFIG, XIP_MBL_Msk, XIP_MBL_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetModeBits(fsmc_dev_t *fsmc, uint32_t val)
{
    HW_SET_MSK_VAL(fsmc->XIP_MODE_BITS, XIP_MODE_BITS_Msk, XIP_MODE_BITS_PosL, val);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetModeBits(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_MODE_BITS, XIP_MODE_BITS_Msk, XIP_MODE_BITS_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_SetDummyCycles(fsmc_dev_t *fsmc, uint32_t cycles)
{
    HW_SET_MSK_VAL(fsmc->XIP_CONFIG, XIP_DUMMY_Msk, XIP_DUMMY_PosL, cycles);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_GetDummyCycles(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_CONFIG, XIP_DUMMY_Msk, XIP_DUMMY_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_FixFrameSizeEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->XIP_CONFIG, XIP_DFS_HC_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_FixFrameSizeDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->XIP_CONFIG, XIP_DFS_HC_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_FixFrameSizeIsEnabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->XIP_CONFIG, XIP_DFS_HC_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_InstEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->XIP_CONFIG, XIP_INST_EN_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_InstDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->XIP_CONFIG, XIP_INST_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_InstIsEnabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->XIP_CONFIG, XIP_INST_EN_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_ContXferEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->XIP_CONFIG, XIP_CONT_XFER_EN_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_ContXferDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->XIP_CONFIG, XIP_CONT_XFER_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_ContXferIsEnabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->XIP_CONFIG, XIP_CONT_XFER_EN_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_PreFetchEnable(fsmc_dev_t *fsmc)
{
    HW_SET_MSK(fsmc->XIP_CONFIG, XIP_PREFETCH_EN_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_PreFetchDisable(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->XIP_CONFIG, XIP_PREFETCH_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_PreFetchIsEnabled(fsmc_dev_t *fsmc)
{
    return HW_TEST_BIT(fsmc->XIP_CONFIG, XIP_PREFETCH_EN_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_Set_EncKey(fsmc_dev_t *fsmc, uint32_t *enc_key)
{
    for (int idx = 0; idx < (LL_FSMC_XIP_ENC_KEY_SIZE / 4); idx++) {
        HW_SET_MSK_VAL(fsmc->XIP_ENC_KEY[idx],
                   ENC_KEY_Msk,
                   ENC_KEY_PosL,
                   enc_key[idx]);
    }
}
__HW_STATIC_INLINE
int LL_FSMC_XIP_Get_EncKey(fsmc_dev_t *fsmc, uint32_t *enc_key, uint8_t enc_key_cnt)
{
    if (enc_key_cnt != LL_FSMC_XIP_ENC_KEY_SIZE / 4)
        return -1;
    for (int idx = 0; idx < enc_key_cnt; idx++) {
        enc_key[idx] = HW_GET_MSK_VAL(fsmc->XIP_ENC_KEY[idx],
                   ENC_KEY_Msk,
                   ENC_KEY_PosL);
    }
    return 0;
}
__HW_STATIC_INLINE
void LL_FSMC_XIP_Set_EncInfo(fsmc_dev_t *fsmc, uint8_t idx, uint32_t start_addr, uint32_t end_addr)
{
    HW_SET_MSK_VAL(fsmc->XIP_ENC_INFO[idx].XIP_ENC_START_ADDR,
                   ENC_START_ADDR_Msk,
                   ENC_START_ADDR_PosL,
                   start_addr);
    HW_SET_MSK_VAL(fsmc->XIP_ENC_INFO[idx].XIP_ENC_END_ADDR,
                   ENC_END_ADDR_Msk,
                   ENC_END_ADDR_PosL,
                   end_addr);
    HW_SET_BIT(fsmc->XIP_ENC_INFO[idx].XIP_ENC_ENABLE, 0);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_EncAddrSet(fsmc_dev_t *fsmc, uint8_t idx, uint32_t start_addr, uint32_t end_addr)
{
    HW_SET_MSK_VAL(fsmc->XIP_ENC_INFO[idx].XIP_ENC_START_ADDR,
                   ENC_START_ADDR_Msk,
                   ENC_START_ADDR_PosL,
                   start_addr);
    HW_SET_MSK_VAL(fsmc->XIP_ENC_INFO[idx].XIP_ENC_END_ADDR,
                   ENC_END_ADDR_Msk,
                   ENC_END_ADDR_PosL,
                   end_addr);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_EncEnable(fsmc_dev_t *fsmc, uint8_t idx)
{
    HW_SET_BIT(fsmc->XIP_ENC_INFO[idx].XIP_ENC_ENABLE, 0);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_EncDisable(fsmc_dev_t *fsmc, uint8_t idx)
{
    HW_CLR_BIT(fsmc->XIP_ENC_INFO[idx].XIP_ENC_ENABLE, 0);
}

__HW_STATIC_INLINE
void LL_FSMC_XIP_Get_EncInfo(fsmc_dev_t *fsmc, uint8_t idx, fsmc_xip_enc_t *enc_info)
{
    enc_info->XIP_ENC_START_ADDR = HW_GET_MSK_VAL(fsmc->XIP_ENC_INFO[idx].XIP_ENC_START_ADDR,
                   ENC_START_ADDR_Msk,
                   ENC_START_ADDR_PosL);
    enc_info->XIP_ENC_END_ADDR = HW_GET_MSK_VAL(fsmc->XIP_ENC_INFO[idx].XIP_ENC_END_ADDR,
                   ENC_END_ADDR_Msk,
                   ENC_END_ADDR_PosL);
    enc_info->XIP_ENC_ENABLE = HW_GET_BIT(fsmc->XIP_ENC_INFO[idx].XIP_ENC_ENABLE, 0);
}
__HW_STATIC_INLINE
void LL_FSMC_XIP_Set_EncKeySel(fsmc_dev_t *fsmc, uint32_t sel)
{
    HW_SET_MSK_VAL(fsmc->XIP_ENC_CTRL,
                       ENC_KEY_SEL_Msk,
                       ENC_KEY_SEL_PosL,
                       sel);
}
__HW_STATIC_INLINE
uint32_t LL_FSMC_XIP_Get_EncKeySel(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->XIP_ENC_CTRL,
                       ENC_KEY_SEL_Msk,
                       ENC_KEY_SEL_PosL);
}
/** XIP functions
  * @}
  */

/** @defgroup PSRAM functions
  * @{
  */
__HW_STATIC_INLINE
void LL_FSMC_PSRAM_SetTxInst(fsmc_dev_t *fsmc, uint8_t inst)
{
    HW_SET_MSK_VAL(fsmc->PSRAM_INST, PSRAM_TX_INST_Msk, PSRAM_TX_INST_PosL, inst);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_PSRAM_GetTxInst(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->PSRAM_INST, PSRAM_TX_INST_Msk, PSRAM_TX_INST_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_PSRAM_SetRxInst(fsmc_dev_t *fsmc, uint8_t inst)
{
    HW_SET_MSK_VAL(fsmc->PSRAM_INST, PSRAM_RX_INST_Msk, PSRAM_RX_INST_PosL, inst);
}

__HW_STATIC_INLINE
uint32_t LL_FSMC_PSRAM_GetRxInst(fsmc_dev_t *fsmc)
{
    return HW_GET_MSK_VAL(fsmc->PSRAM_INST, PSRAM_RX_INST_Msk, PSRAM_RX_INST_PosL);
}

__HW_STATIC_INLINE
void LL_FSMC_PSRAM_EnableSlaveSelect(fsmc_dev_t *fsmc)
{
    HW_SET_MSK_VAL(fsmc->CTRL, CSE_Msk, CSE_PosL, 0x02);
}

__HW_STATIC_INLINE
void LL_FSMC_PSRAM_DisableSlaveSelect(fsmc_dev_t *fsmc)
{
    HW_CLR_MSK(fsmc->CTRL, CSE_Msk);
}

__HW_STATIC_INLINE
void LL_FSMC_PSRAM_EnableSelectCS1(fsmc_dev_t *fsmc)
{
    HW_SET_MSK_VAL(fsmc->PSRAM_CS_SEL, PSRAM_CS_SEL_Msk, PSRAM_CS_SEL_PosL, 0x01);
}


/** PSRAM functions
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}

#endif

#endif /* _PSIC_LL_FSMC_H */

/*
 **********************************************************************************************************************
 * (C) Copyright POSSUMIC Technology
 * END OF FILE
 */

