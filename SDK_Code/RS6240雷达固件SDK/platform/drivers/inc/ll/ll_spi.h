/**
 **************************************************************************************************
 * @file   psic_ll_spi.h
 * @brief   SPI hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PSIC_LL_SPI_H
#define _PSIC_LL_SPI_H


#include "types.h"
#include "soc.h"
#include "hw_qspi.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * --------------------------------------------------------------------------------------------------------------------
 */
#define QSPI_DEV                        ((qspi_dev_t *)   QSPI_BASE_ADDR)  /*!< 0x4002 2000 */
/* Exported constants.
 * --------------------------------------------------------------------------------------------------------------------
 */
/* Spi control register1 */


/** @defgroup qspi data frame size
  * @{
  */
#define LL_QSPI_DFS_BIT(n)                          ((n) - 1)
/**
  * @}
  */


/** @defgroup QSPI address length
  * @{
  */
#define LL_QSPI_ADDR_LEN(n)                          ((n) / 4)
#define LL_QSPI_ADDR_LEN_0                           0
#define LL_QSPI_ADDR_LEN_8                           2
#define LL_QSPI_ADDR_LEN_16                          4
#define LL_QSPI_ADDR_LEN_24                          6
#define LL_QSPI_ADDR_LEN_32                          8
/**
  * @}
  */


/** @defgroup QSPI data frame size
  * @{
  */
#define LL_QSPI_FIFO_LEVEL(n)                       ((n) - 1)
/**
  * @}
  */

/** @defgroup QSPI data frame size
  * @{
  */
#define LL_QSPI_RX_NUM(n)                           ((n) - 1)
/**
  * @}
  */


/** @defgroup FSMC data mask
  * @{
  */
#define LL_QSPI_BITS_MASK(n)                            ((1 << (n)) - 1)
#define LL_QSPI_BITS_8                                  (0xFF)
#define LL_QSPI_BITS_32                                 (0xFFFFFFFF)

/**
  * @}
  */

/** @defgroup FSMC rx sample delay
  * @{
  */
#define LL_QSPI_SAMPLE_DELAY(n)             ((n))
#define LL_QSPI_SAMPLE_DELAY_1              1
#define LL_QSPI_SAMPLE_DELAY_2              2
#define LL_QSPI_SAMPLE_DELAY_3              3
#define LL_QSPI_SAMPLE_DELAY_4              4
/**
  * @}
  */


#define LL_QSPI_SLV_MODE                                 (0x00U)
#define LL_QSPI_MST_MODE                                 (0x01U)

#define LL_QSPI_DUPLE_ENABLE                             (0x01U)
#define LL_QSPI_DUPLE_DISABLE                            (0x00U)

#define LL_QSPI_SLVCLK_SSICLK                            (0x00U)
#define LL_QSPI_SLVCLK_FUNCLK                            (0x01U)

#define LL_QSPI_CR1_UNCLEAR_Pos2                         (28U)
#define LL_QSPI_CR1_UNCLEAR_Msk2                         (0x01U)

#define LL_QSPI_LINES_STANDARD                           (0x00U)
#define LL_QSPI_LINES_DUAL                               (0x01U)
#define LL_QSPI_LINES_QUAD                               (0x02U)
#define LL_QSPI_LINES_OCTAL                              (0x03U)

/* not support in 2323-02-20*/
#define LL_QSPI_CFS_MAXBIT                               (0x0FU)

#define LL_QSPI_SSTE_ENABLE                              (0x00U)
#define LL_QSPI_SSTE_DISABLE                             (0x01U)

/* not support in 2323-02-20*/
#define LL_QSPI_SRL_ENABLE                               (0x01U)
#define LL_QSPI_SRL_DISABLE                              (0x00U)

#define LL_QSPI_SLVOE_ENABLE                             (0x01U)
#define LL_QSPI_SLVOE_DISANLE                            (0x01U)

#define LL_QSPI_TMODE_TXRX                               (0x00U)
#define LL_QSPI_TMODE_TXONLY                             (0x01U)
#define LL_QSPI_TMODE_RXONLY                             (0x02U)
#define LL_QSPI_TMODE_RXEEROM                            (0x03U)

#define LL_QSPI_CPOL_1                                   (0x01U)
#define LL_QSPI_CPOL_0                                   (0x00U)

#define LL_QSPI_CPHA_1                                   (0x01U)
#define LL_QSPI_CPHA_0                                   (0x00U)

#define LL_QSPI_FMODE_SPI                                (0x00U)
#define LL_QSPI_FMODE_SSP                                (0x01U)
#define LL_QSPI_FMODE_NSM                                (0x02U)

/** @defgroup FSMC data frame size
  * @{
  */
#define LL_QSPI_DFS_BIT(n)                               ((n) - 1)
#define LL_QSPI_DFS_4BIT                                 (0x03U)
#define LL_QSPI_DFS_8BIT                                 (0x07U)
#define LL_QSPI_DFS_16BIT                                (0x0FU)
#define LL_QSPI_DFS_32BIT                                (0x1FU)
#define LL_QSPI_DFS_MAXNUM                               (0x1FU)
/**
  * @}
  */


/* Spi control register2 */
#define LL_QSPI_NDF_MAXNUMBER                            (0xFFFFU)

/* Spi enable register */
#define LL_QSPI_ENABLE                                   (0x01U)
#define LL_QSPI_DISABLE                                  (0x00U)

/* Spi microwire control register */
/* not support in 2323-02-20*/
#define LL_QSPI_HANDSHAKING_ENABLE                       (0x01U)
#define LL_QSPI_HANDSHAKING_DISABLE                      (0x00U)
/* not support in 2323-02-20*/
#define LL_QSPI_MRXDATA                                  (0x01U)
#define LL_QSPI_MTXDATA                                  (0x00U)
/* not support in 2323-02-20*/
#define LL_QSPI_MSEQ_ENABLE                              (0x01U)
#define LL_QSPI_MSEQ_DISABLE                             (0x00U)

/* Spi slave enable register */
#define LL_QSPI_SS_ENABLE                                (0x01U)
#define LL_QSPI_SS_DISABLE                               (0x00U)

/* Spi bard rate select(prescaler) register */
#define LL_QSPI_CLK_DIV2                                 (0x01U)
#define LL_QSPI_CLK_DIV4                                 (0x02U)
#define LL_QSPI_CLK_DIV8                                 (0x04U)
#define LL_QSPI_CLK_DIV16                                (0x08U)
#define LL_QSPI_CLK_DIV32                                (0x10U)
#define LL_QSPI_CLK_DIV64                                (0x20U)
#define LL_QSPI_CLK_DIV128                               (0x40U)
#define LL_QSPI_CLK_DIV256                               (0x80U)
#define LL_QSPI_CLK_DIV512                               (0x100U)
#define LL_QSPI_DIVI_MAXNUM                              (0xFFFEU)

/* Spi transmit fifo threshold level register */
#define LL_QSPI_TSTART_LEVEL1                            (0x00U)
#define LL_QSPI_TSTART_LEVEL2                            (0x01U)
#define LL_QSPI_TSTART_LEVEL3                            (0x02U)
#define LL_QSPI_TSTART_LEVEL4                            (0x03U)
#define LL_QSPI_TSTART_LEVEL5                            (0x04U)
#define LL_QSPI_TSTART_LEVEL6                            (0x05U)
#define LL_QSPI_TSTART_LEVEL7                            (0x06U)
#define LL_QSPI_TSTART_LEVEL8                            (0x07U) /* 1/4fifo */
#define LL_QSPI_TSTART_LEVEL16                           (0x0FU) /* 1/2fifo */
#define LL_QSPI_TSTART_LEVEL24                           (0x17U) /* 3/4fifo */
#define LL_QSPI_TSTART_LEVEL32                           (0x1FU)

#define LL_QSPI_TXFIFO_THRESHOLD1                        (0x01U)
#define LL_QSPI_TXFIFO_THRESHOLD2                        (0x02U)
#define LL_QSPI_TXFIFO_THRESHOLD4                        (0x04U)
#define LL_QSPI_TXFIFO_THRESHOLD8                        (0x08U)
#define LL_QSPI_TXFIFO_THRESHOLD16                       (0x10U)
#define LL_QSPI_TXFIFO_THRESHOLD32                       (0x20U)

/* Spi receive fifo threshold level register */
#define LL_QSPI_RXFIFO_THRESHOLD1                        (0x00U)
#define LL_QSPI_RXFIFO_THRESHOLD2                        (0x01U)
#define LL_QSPI_RXFIFO_THRESHOLD4                        (0x03U)
#define LL_QSPI_RXFIFO_THRESHOLD8                        (0x07U)
#define LL_QSPI_RXFIFO_THRESHOLD16                       (0x0FU)
#define LL_QSPI_RXFIFO_THRESHOLD24                       (0x17U)
#define LL_QSPI_RXFIFO_THRESHOLD32                       (0x1FU)


/** @defgroup SPI status flags
  * @{
  */
#define LL_QSPI_STATUS_BUSY                 HW_BIT(0)
#define LL_QSPI_STATUS_TXNF                 HW_BIT(1)
#define LL_QSPI_STATUS_TXE                  HW_BIT(2)
#define LL_QSPI_STATUS_RXNE                 HW_BIT(3)
#define LL_QSPI_STATUS_TXF                  HW_BIT(4)
#define LL_QSPI_STATUS_TXER                 HW_BIT(5)
#define LL_QSPI_STATUS_DCOL                 HW_BIT(6)
/**
  * @}
  */

/** @defgroup SPI IT flags
  * @{
  */
#define LL_QSPI_IT_TXE                      HW_BIT(0)
#define LL_QSPI_IT_TXO                      HW_BIT(1)
#define LL_QSPI_IT_RXU                      HW_BIT(2)
#define LL_QSPI_IT_RXO                      HW_BIT(3)
#define LL_QSPI_IT_RXF                      HW_BIT(4)
#define LL_QSPI_IT_MST                      HW_BIT(5)
#define LL_QSPI_IT_XRXO                     HW_BIT(6)
#define LL_QSPI_IT_HIGH_EDG                 HW_BIT(7)


#define LL_QSPI_ITE_TXE                     HW_BIT(16)
#define LL_QSPI_ITE_TXO                     HW_BIT(17)
#define LL_QSPI_ITE_RXU                     HW_BIT(18)
#define LL_QSPI_ITE_RXO                     HW_BIT(19)
#define LL_QSPI_ITE_RXF                     HW_BIT(20)
#define LL_QSPI_ITE_MST                     HW_BIT(21)
#define LL_QSPI_ITE_XRXO                    HW_BIT(22)
#define LL_QSPI_ITE_HIGH_EDG                HW_BIT(23)
#define LL_QSPI_ITE_ALL                     (LL_QSPI_ITE_TXE |        \
                                             LL_QSPI_ITE_TXO |        \
                                             LL_QSPI_ITE_RXU |        \
                                             LL_QSPI_ITE_RXO |        \
                                             LL_QSPI_ITE_RXF |        \
                                             LL_QSPI_ITE_MST |        \
                                             LL_QSPI_ITE_XRXO|        \
                                             LL_QSPI_ITE_HIGH_EDG)

/**
  * @}
  */


/* Spi dma control register */
#define LL_QSPI_TX_DMA_ENABLE                            (0x01U)
#define LL_QSPI_TX_DMA_DISABLE                           (0x00U)
#define LL_QSPI_RX_DMA_ENABLE                            (0x01U)
#define LL_QSPI_RX_DMA_DISABLE                           (0x00U)

/* Spi receive sample delay register */
#define LL_QSPI_SAMPLE_DELAY_ENABLE                      (0x01U)
#define LL_QSPI_SAMPLE_DELAY_DISABLE                     (0x00U)

#define LL_QSPI_RECEIVE_SAMPLE_DELAY_MAXNUM              (0xFFU)

/* Spi control register */
/* not support in 2323-02-20*/
#define LL_QSPI_WAIT_CYC_ENABLE                          (0x01U)
#define LL_QSPI_WAIT_CYC_DISABLE                         (0x00U)
#define LL_QSPI_WAIT_CYC_MAXNUM                          (0x1FU)

#define LL_QSPI_INS_LENGHT_4BIT                          (0x01U)
#define LL_QSPI_INS_LENGHT_8BIT                          (0x02U)
#define LL_QSPI_INS_LENGHT_16BIT                         (0x03U)

#define LL_QSPI_XIP_MODE_ENABLE                          (0x01)
#define LL_QSPI_XIP_MODE_DISABLE                         (0x00)

#define LL_QSPI_ADDR_LENGHT_MAX                          (0x0F)

#define LL_QSPI_TRAN_INS_AND_ADDR_STANDARD               (0x00U)
#define LL_QSPI_TRAN_INS_STANDARD_ADDR_TYPE              (0x01U)
#define LL_QSPI_TRAN_INS_AND_ADDR_TYPE                   (0x02U)


/* Spi slave transmit wait cycles configure register */
#define LL_QSPI_TWC_MAX                                  (0xFFFF)


/* Exported functions --------------------------------------------------------*/
/** @defgroup spi_ll_exported_functions spi exported functions
  * @{
  */

/** @defgroup spi_ll_configuration configuration
  * @{
  */

/**
  * @brief    enable spi peripheral
  * @api    SPIENR            SPE           LL_SPI_Enable
  * @param    spi spi register
  * @retval None
  */
__HW_STATIC_INLINE
void LL_SPI_Enable(qspi_dev_t *qspi)
{
    HW_SET_BIT(qspi->CTRL_REG, SSI_START_Pos);
}

/**
  * @brief    disable spi peripheral
  * @api    SPIENR            SPE           LL_SPI_Disable
  * @param    spi spi register
  * @note    when disabling the spi, follow the procedure described in the reference manual.
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_Disable(qspi_dev_t *qspi)
{
    HW_CLR_BIT(qspi->CTRL_REG, SSI_START_Pos);
}

/**
  * @brief    check if spi peripheral is enabled
  * @api    SPIENR            SPE           LL_SPI_Isable
  * @param    spi spi register
  * @retval state of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_Isable(qspi_dev_t *qspi)
{
    return HW_TEST_BIT(qspi->CTRL_REG,
                        SSI_START_Pos);
}

/**
  * @brief    set spi operation mode to master or slave
  * @api    CR1          MST          LL_SPI_SetMode
  * @param    spi spi register
  * @param    mode this parameter can be one of the following values:
  *         @arg @ref SPI_MST_MODE
  *         @arg @ref SPI_SLV_MODE
  * @note    this bit should not be changed when communication is ongoing.
  * @retval none
  */
__STATIC_INLINE
void LL_SPI_SetMode(qspi_dev_t *qspi, uint32_t mode)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                   SSI_TYPE_Msk,
                   SSI_TYPE_Pos,
                   mode);
}

/**
  * @brief    get spi operation mode (master or slave)
  * @api    CR1          MST          LL_SPI_GetMode
  * @param    spi spi register
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_MST_MODE
  *         @arg @ref SPI_SLV_MODE
  */
__STATIC_INLINE
uint32_t LL_SPI_GetMode(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CTRL_REG,
                          SSI_TYPE_Msk,
                          SSI_TYPE_Pos);
}

/**
  * @brief    set clock phase
  * @api    CR1          CPHA           LL_SPI_SetClockPhase
  * @param    spi spi register
  * @param    clockphase this parameter can be one of the following values:
  *         @arg @ref SPI_CPHA_1
  *         @arg @ref SPI_CPHA_0
  * @note    this bit should not be changed when communication is ongoing.
  *         this bit is not changed in spi ti mode.
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_SetClockPhase(qspi_dev_t *qspi, uint32_t clockphase)
{
    HW_SET_MSK_VAL(qspi->CONFIG_REG,
                    SCPH_Msk,
                    SCPH_Pos,
                    clockphase);
}

/**
  * @brief    get clock phase
  * @api    CR1          CPHA           LL_SPI_GetClockPhase
  * @param    spi spi register
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_CPHA_1
  *         @arg @ref SPI_CPHA_0
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_GetClockPhase(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CONFIG_REG,
                            SCPH_Msk,
                            SCPH_Pos);
}

/**
  * @brief    set clock polarity
  * @api    CR1          CPOL           LL_SPI_SetClockPolarity
  * @param    spi spi register
  * @param    clockpolarity this parameter can be one of the following values:
  *         @arg @ref SPI_CPOL_1
  *         @arg @ref SPI_CPOL_0
  * @note    this bit should not be changed when communication is ongoing.
  *         this bit is not changed in spi ti mode.
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_SetClockPolarity(qspi_dev_t *qspi, uint32_t clockpolarity)
{
    HW_SET_MSK_VAL(qspi->CONFIG_REG,
                    SCPOL_Msk,
                    SCPOL_Pos,
                    clockpolarity);
}

/**
  * @brief    get clock polarity
  * @api    CR1          CPOL           LL_SPI_GetClockPolarity
  * @param    spi spi register
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_CPOL_1
  *         @arg @ref SPI_CPOL_0
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_GetClockPolarity(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CONFIG_REG,
                            SCPOL_Msk,
                            SCPOL_Pos);
}

/**
  * @brief    set clock divid
  * @api    CR1          BAUDR            LL_SPI_SetBaudRatePrescaler
  * @param    spi spi register
  * @param    divid this parameter can be one of the following values:
  *         @arg @ref SPI_CLK_DIV2
  *         @arg @ref SPI_CLK_DIV4
  *         @arg @ref SPI_CLK_DIV8
  *         @arg @ref SPI_CLK_DIV16
  *         @arg @ref SPI_CLK_DIV32
  *         @arg @ref SPI_CLK_DIV64
  *         @arg @ref SPI_CLK_DIV128
  *         @arg @ref SPI_CLK_DIV256
  *         @arg @ref SPI_CLK_DIV512
  *         @arg @ref ((divid & (divid - 1))  <= SPI_DIVI_MAXNUM)
  * @note    divid must be half word alignment
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_SetPrescaler(qspi_dev_t *qspi, uint32_t divid)
{
    HW_SET_MSK_VAL(qspi->CLOCK_CTRL_REG,
                    CDR_Msk,
                    CDR_Pos,
                    divid);
}

/**
  * @brief    get clock divid
  * @api    CR1          BAUDR            LL_SPI_GetBaudRatePrescaler
  * @param    spi spi register
  * @note    divid must be half word alignment
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_CLK_DIV2
  *         @arg @ref SPI_CLK_DIV4
  *         @arg @ref SPI_CLK_DIV8
  *         @arg @ref SPI_CLK_DIV16
  *         @arg @ref SPI_CLK_DIV32
  *         @arg @ref SPI_CLK_DIV64
  *         @arg @ref SPI_CLK_DIV128
  *         @arg @ref SPI_CLK_DIV256
  *         @arg @ref SPI_CLK_DIV512
  *         @arg @ref ((divid & (divid - 1))  <= SPI_DIVI_MAXNUM)
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_GetPrescaler(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CLOCK_CTRL_REG,
                            CDR_Msk,
                            CDR_Pos);
}

/**
  * @brief    set duple mode
  * @api    CR1          DUPLE            LL_SPI_SetDuplex
  * @param    spi spi register
  *         duple this parameter can be one of the following values:
  *         @arg @ref SPI_DUPLE_ENABLE
  *         @arg @ref SPI_DUPLE_DISABLE
  * @note    if want to get the transfer direction, please cooperate with the following api:
  *         @ LL_SPI_SetClockPolarity
  *         @ LL_SPI_SetTransferMode
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_EnableStdDuplex(qspi_dev_t *qspi)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                   FULL_DUPLEX_EN_Msk,
                   FULL_DUPLEX_EN_Pos,
                   1);
}

/**
  * @brief    get duple mode
  * @api    CR1          DUPLE            LL_SPI_GetDuple
  * @param    spi spi register
  * @note    if want to get the transfer direction, please cooperate with the following api:
  *         @ LL_SPI_GetClockPolarity
  *         @ LL_SPI_GetTransferMode
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_DUPLE_ENABLE
  *         @arg @ref SPI_DUPLE_DISABLE
  */
__HW_STATIC_INLINE
void LL_SPI_DisableStdDuplex(qspi_dev_t *qspi)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                   FULL_DUPLEX_EN_Msk,
                   FULL_DUPLEX_EN_Pos,
                   0);
}

/**
  * @brief    set transfer mode
  * @api    CR1          TMODE            LL_SPI_SetTransferMode
  * @param    spi spi register
  *         transfer_mode this parameter can be one of the following values:
  *         @arg @ref SPI_TMODE_TXRX
  *         @arg @ref SPI_TMODE_TXONLY
  *         @arg @ref SPI_TMODE_RXONLY
  *         @arg @ref SPI_TMODE_RXEEROM
  * @note    if want to get the transfer direction, please cooperate with the following api:
  *         @ LL_SPI_SetClockPolarity
  *         @ LL_SPI_SetTransferMode
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_SetTransferMode(qspi_dev_t *qspi, uint32_t transfer_mode)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                    TTYP_Msk,
                    TTYP_Pos,
                    transfer_mode);
}

/**
  * @brief    get transfer mode
  * @api    CR1          TMODE            LL_SPI_GetTransferMode
  * @param    spi spi register
  * @note    if want to get the transfer direction, please cooperate with the following api:
  *         @ LL_SPI_GetClockPolarity
  *         @ LL_SPI_GetTransferMode
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_TMODE_TXRX
  *         @arg @ref SPI_TMODE_TXONLY
  *         @arg @ref SPI_TMODE_RXONLY
  *         @arg @ref SPI_TMODE_RXEEROM
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_GetTransferMode(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CTRL_REG,
                            TTYP_Msk,
                            TTYP_Pos);
}

/**
  * @brief    set datawidth mode
  * @api    CR1          DFS          LL_SPI_SetDataWidth
  * @param    spi spi register
  *         datawidth this parameter can be one of the following values:
  *         @arg @ref SPI_DFS_4BIT
  *         @arg @ref SPI_DFS_8BIT
  *         @arg @ref SPI_DFS_16BIT
  *         @arg @ref SPI_DFS_32BIT
  *         @arg @ref (datawidth - 1) <= SPI_DFS_MAXNU
  * @note    datawidth must be word alignment, when not used standard spi
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_SetDataWidth(qspi_dev_t *qspi, uint32_t datawidth)
{
    HW_SET_MSK_VAL(qspi->CONFIG_REG,
                    DFW_Msk,
                    DFW_Pos,
                    datawidth);
}

/**
  * @brief    get datawidth mode
  * @api    CR1          DFS          LL_SPI_GetDataWidth
  * @param    spi spi register
  * @note    datawidth must be word alignment, when not used standard spi
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_DFS_4BIT
  *         @arg @ref SPI_DFS_8BIT
  *         @arg @ref SPI_DFS_16BIT
  *         @arg @ref SPI_DFS_32BIT
  *         @arg @ref (datawidth - 1) <= SPI_DFS_MAXNU
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_GetDataWidth(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CONFIG_REG,
                            DFW_Msk,
                            DFW_Pos);
}

/**
  * @brief    hw slave select enable (enable or disable)
  * @api    CR1          SSE          LL_SPI_EnableSS
  * @param    spi spi register
  * @note    if set of SPI_SS_ENABLE, the hardware will automatically perform the slave select
  * @retval none
  */
__HW_STATIC_INLINE
void LL_SPI_EnableSlaveSelect(qspi_dev_t *qspi)
{
    HW_SET_BIT(qspi->CTRL_REG, CSE_Pos);
}

/**
  * @brief    hw slave select disable (enable or disable)
  * @api    CR1          SSE          LL_SPI_DisableSS
  * @param    spi spi register
  * @note    if set of SPI_SS_ENABLE, the hardware will automatically perform the slave select
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_SS_ENABLE
  *         @arg @ref SPI_SS_DISABLE
  */
__HW_STATIC_INLINE
void LL_SPI_DisableSlaveSelect(qspi_dev_t *qspi)
{
    HW_SET_BIT(qspi->CTRL_REG, CSE_Pos);
}


/**
  * @brief    get slave select 0 (enable or disable)
  * @api    CR1          SSE          LL_SPI_GetSS0
  * @param    spi spi register
  * @retval returned value can be one of the following values:
  *         @arg @ref SPI_SS_ENABLE
  *         @arg @ref SPI_SS_DISABLE
  */
__HW_STATIC_INLINE uint32_t LL_SPI_IsableSlaveSelect(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CTRL_REG,
                            CSE_Msk,
                            CSE_Pos);
}

/**
  * @}
  */




/** @defgroup spi_ll_fifo_configuration configuration
  * @{
  */

__HW_STATIC_INLINE void LL_SPI_SetTxFifoStartLevel(qspi_dev_t *qspi, uint32_t start_level)
{
    HW_SET_MSK_VAL(qspi->TXF_CTL_REG,
                   TXFSL_Msk,
                   TXFSL_Pos,
                   start_level);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetTxFifoStartLevel(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->TXF_CTL_REG,
                          TXFSL_Msk,
                          TXFSL_Pos);
}

//@note this bit is used in spi ti mode
__HW_STATIC_INLINE void LL_SPI_SetTxFifoThreshold(qspi_dev_t *qspi, uint32_t threshold)
{
    HW_SET_MSK_VAL(qspi->TXF_CTL_REG,
                   TXFITL_Msk,
                   TXFITL_Pos,
                   threshold);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetTxFifoThreshold(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->TXF_CTL_REG,
                          TXFITL_Msk,
                          TXFITL_Pos);
}


__HW_STATIC_INLINE void LL_SPI_SetRxFifoThreshold(qspi_dev_t *qspi, uint32_t threshold)
{
    HW_SET_MSK_VAL(qspi->RXF_CTL_REG,
                   RXFITL_Msk,
                   RXFITL_Pos,
                   threshold);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetRxFifoThreshold(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->RXF_CTL_REG,
                          RXFITL_Msk,
                          RXFITL_Pos);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetTxFifoLevel(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->FIFO_STATUS_REG,
                            TXFS_Msk,
                            TXFS_Pos);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetRxFifoLevel(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->FIFO_STATUS_REG,
                            RXFS_Msk,
                            RXFS_Pos);
}



/**
  * @}
  */


/** @defgroup spi_ll_int_control control
  * @{
  */

__HW_STATIC_INLINE
void LL_SPI_ITEnable(qspi_dev_t *qspi, uint32_t mask)
{
    HW_SET_MSK(qspi->INT_CS_REG, mask);
}

__HW_STATIC_INLINE
void LL_SPI_ITDisable(qspi_dev_t *qspi, uint32_t mask)
{
    HW_CLR_MSK(qspi->INT_CS_REG, mask);
}

__HW_STATIC_INLINE
void LL_SPI_ITDisableALL(qspi_dev_t *qspi)
{
    HW_CLR_MSK(qspi->INT_CS_REG, LL_QSPI_ITE_ALL);
}


__HW_STATIC_INLINE
uint32_t LL_SPI_ITIsEnabled(qspi_dev_t *qspi, uint32_t mask)
{
    return HW_GET_MSK(qspi->INT_CS_REG, mask);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_ITIsActiveFlag(qspi_dev_t *qspi, uint32_t mask)
{
    return HW_GET_MSK(qspi->INT_CS_REG, mask);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_BusIsActiveFlag(qspi_dev_t *qspi, uint32_t mask)
{
    return HW_GET_MSK(qspi->SSR, mask);
}


__HW_STATIC_INLINE
uint32_t LL_SPI_ITFlagGetALL(qspi_dev_t *qspi)
{
    return qspi->INT_CS_REG;
}

__HW_STATIC_INLINE
void LL_SPI_ITFlagClearALL(qspi_dev_t *qspi)
{
    HW_GET_VAL(qspi->INT_CLEAR_REG);
}

__HW_STATIC_INLINE
void LL_SPI_ITFlagClearMST(qspi_dev_t *qspi)
{
    (void)HW_GET_MSK_VAL(qspi->INT_CLEAR_REG, MSTICR_Msk, MSTICR_PosL);
}

__HW_STATIC_INLINE
void LL_SPI_ITFlagClearRXU(qspi_dev_t *qspi)
{
    (void)HW_GET_MSK_VAL(qspi->INT_CLEAR_REG, RXUICR_Msk, RXUICR_PosL);
}

__HW_STATIC_INLINE
void LL_SPI_ITFlagClearRXO(qspi_dev_t *qspi)
{
    (void)HW_GET_MSK_VAL(qspi->INT_CLEAR_REG, RXOICR_Msk, RXOICR_PosL);
}

__HW_STATIC_INLINE
void LL_SPI_ITFlagClearTXE(qspi_dev_t *qspi)
{
    (void)HW_GET_MSK_VAL(qspi->INT_CLEAR_REG, TXOICR_Msk, TXOICR_PosL);
}

/**
  * @}
  */


/** @defgroup spi_ll_dma_control control
  * @{
  */
__HW_STATIC_INLINE void LL_SPI_EnableDMAReq_RX(qspi_dev_t *qspi)
{
    HW_SET_BIT(qspi->DMA_EN_REG, DMARE_Pos);
}

__HW_STATIC_INLINE void LL_SPI_DisableDMAReq_RX(qspi_dev_t *qspi)
{
    HW_CLR_BIT(qspi->DMA_EN_REG, DMARE_Pos);
}

__HW_STATIC_INLINE uint32_t LL_SPI_IsableDMAReq_RX(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->DMA_EN_REG,
                            DMARE_Msk,
                            DMARE_Pos);
}

__HW_STATIC_INLINE void LL_SPI_EnableDMAReq_TX(qspi_dev_t *qspi)
{
    HW_SET_BIT(qspi->DMA_EN_REG, DMATE_Pos);
}

__HW_STATIC_INLINE void LL_SPI_DisableDMAReq_TX(qspi_dev_t *qspi)
{
    HW_CLR_BIT(qspi->DMA_EN_REG, DMATE_Pos);
}

__HW_STATIC_INLINE uint32_t LL_SPI_IsableDMAReq_TX(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->DMA_EN_REG,
                          DMATE_Msk,
                          DMATE_Pos);
}

__HW_STATIC_INLINE uint32_t LL_SPI_DMA_GetRegAddr(qspi_dev_t *qspi)
{
    return (uint32_t)(qspi->DR);
}

__HW_STATIC_INLINE void LL_SPI_SetDMALevel_RX(qspi_dev_t *qspi, uint32_t level)
{
    HW_SET_MSK_VAL(qspi->DMARR_REG,
                   DMARRL_Msk,
                   DMARRL_Pos,
                   level);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetDMALevel_RX(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->DMARR_REG,
                          DMARRL_Msk,
                          DMARRL_Pos);
}

__HW_STATIC_INLINE void LL_SPI_SetDMALevel_TX(qspi_dev_t *qspi, uint32_t level)
{
    HW_SET_MSK_VAL(qspi->DMATR_REG,
                   DMATRL_Msk,
                   DMATRL_Pos,
                   level);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetDMALevel_TX(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->DMATR_REG,
                          DMATRL_Msk,
                          DMATRL_Pos);
}

/**
  * @}
  */


/** @defgroup spi_ll_transfer  transfer
  * @{
  */

__HW_STATIC_INLINE void LL_SPI_ReceiveData4(qspi_dev_t *qspi, uint8_t *data)
{
    *(uint8_t *)data = (uint8_t)qspi->DR & 0x0f;
}

__HW_STATIC_INLINE void LL_SPI_ReceiveData8(qspi_dev_t *qspi, uint8_t *data)
{
    *(uint8_t *)data = (uint8_t)qspi->DR;
}

__HW_STATIC_INLINE void LL_SPI_ReceiveData16(qspi_dev_t *qspi, uint8_t *data)
{
    *(uint16_t *)data = (uint16_t)qspi->DR;
}

__HW_STATIC_INLINE void LL_SPI_ReceiveData32(qspi_dev_t *qspi, uint8_t *data)
{
    *(uint32_t *)data = (uint32_t)qspi->DR;
}

__HW_STATIC_INLINE uint32_t LL_SPI_ReceiveMaskData(qspi_dev_t *qspi, uint32_t mask)
{
    return (qspi->DR & mask);
}

__HW_STATIC_INLINE uint32_t LL_SPI_ReceiveData(qspi_dev_t *qspi)
{
    return (uint32_t)qspi->DR;
}

__HW_STATIC_INLINE void LL_SPI_TransmitData4(qspi_dev_t *qspi, const uint8_t *data)
{
    qspi->DR = *(uint8_t *)data & 0x0f;
}

__HW_STATIC_INLINE void LL_SPI_TransmitData8(qspi_dev_t *qspi, const uint8_t *data)
{
    qspi->DR = *(uint8_t *)data;
}

__HW_STATIC_INLINE void LL_SPI_TransmitData16(qspi_dev_t *qspi, const uint8_t *data)
{
    qspi->DR = *(uint16_t *)data;
}

__HW_STATIC_INLINE void LL_SPI_TransmitData32(qspi_dev_t *qspi, const uint8_t *data)
{
    qspi->DR = *(uint32_t *)data;
}

__HW_STATIC_INLINE void LL_SPI_TransmitMaskData(qspi_dev_t *qspi, uint32_t data, uint32_t mask)
{
    qspi->DR = (data & mask);
}

__HW_STATIC_INLINE void LL_SPI_TransmitData(qspi_dev_t *qspi, uint32_t data)
{
    qspi->DR = (uint32_t)data;
}
//__HW_STATIC_INLINE void LL_SPI_TransmitDataupppp(qspi_dev_t *qspi, uint8_t *data)
//{
//    #qspi->DR = *(uint16_t *)data;
//}



/**
  * @}
  */


/** @defgroup spi_ll_expend_configuration_control configuration control
  * @{
  */

/* only slave use */
/* set sampling clk */
/*
 * SPI_SLVCLK_SSICLK  ->  BUS CLK
 * SPI_SLVCLK_FUNCLK  ->  FUN CLK
 * reset is SPI_SLVCLK_SSICLK
 */
__HW_STATIC_INLINE
void LL_SPI_SlvClkSelect(qspi_dev_t *qspi, uint32_t slvclk)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                    SLV_BLOCK_SEL_Msk,
                    SLV_BLOCK_SEL_Pos,
                    slvclk);
}

__HW_STATIC_INLINE
void LL_SPI_EnableSlvClkIn(qspi_dev_t *qspi)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                    FORCE_SLV_CLK_IN_Msk,
                    FORCE_SLV_CLK_IN_Pos,
                    1);
}

__HW_STATIC_INLINE
void LL_SPI_DisableSlvClkIn(qspi_dev_t *qspi)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                    FORCE_SLV_CLK_IN_Msk,
                    FORCE_SLV_CLK_IN_Pos,
                    0);
}

__HW_STATIC_INLINE
void LL_SPI_EnableForceSlvFCLKSampling(qspi_dev_t *qspi)
{
    LL_SPI_SetMode(qspi, LL_QSPI_MST_MODE);
    LL_SPI_EnableSlvClkIn(qspi);
    LL_SPI_SetMode(qspi, LL_QSPI_SLV_MODE);
    LL_SPI_SlvClkSelect(qspi, 1);
}

/* set of standard/dual/quad/octal spi work type */
/* octal spi work type not support */
/* dual/quad/octal spi work type not work in full_duplex mode */
/*
 * SPI_LINES_STANDARD
 * SPI_LINES_DUAL
 * SPI_LINES_QUAD
 * SPI_LINES_OCTAL
 */
__HW_STATIC_INLINE
void LL_SPI_SetWorkLines(qspi_dev_t *qspi, uint32_t worktype)
{
    HW_SET_MSK_VAL(qspi->CONFIG_REG,
                    SPIMODE_Msk,
                    SPIMODE_Pos,
                    worktype);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_GetWorkLines(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CONFIG_REG,
                            SPIMODE_Msk,
                            SPIMODE_Pos);
}

/* set slave select(hw_cs) toggle when tx/rx */
/* if used the gpio(software)cs this bit not work */
/*
 *SPI_SSTE_ENABLE
 *SPI_SSTE_DISABLE
 */
__HW_STATIC_INLINE
void LL_SPI_EnableSStoggle(qspi_dev_t *qspi)
{
    HW_SET_BIT(qspi->CTRL_REG, SCSTE_Pos);
}

__HW_STATIC_INLINE
void LL_SPI_DisableSStoggle(qspi_dev_t *qspi)
{
    HW_CLR_BIT(qspi->CTRL_REG, SCSTE_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_IsableSStoggle(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CTRL_REG,
                            SCSTE_Msk,
                            SCSTE_Pos);
}

/* slave output enable */
/* not work in master */
/*
 *SPI_SLVOE_ENABLE
 *SPI_SLVOE_DISANLE
 */
__HW_STATIC_INLINE void LL_SPI_EnableSlvOupt(qspi_dev_t *spi)
{
    HW_CLR_BIT(spi->CTRL_REG, SLV_OE_Pos);
}

__HW_STATIC_INLINE void LL_SPI_DisableSlvOupt(qspi_dev_t *spi)
{
    HW_SET_BIT(spi->CTRL_REG, SLV_OE_Pos);
}

__HW_STATIC_INLINE uint32_t LL_SPI_IsableSlvOupt(qspi_dev_t *spi)
{
    return HW_GET_MSK_VAL(spi->CTRL_REG,
                            SLV_OE_Msk,
                            SLV_OE_Pos);
}


/* set spi work format */
/* only support spi */
/*
 *SPI_FMODE_SPI
 *SPI_FMODE_SSP
 *SPI_FMODE_NSM
 */
__HW_STATIC_INLINE
void LL_SPI_SetWorkFormat(qspi_dev_t *qspi, uint32_t workformat)
{
    HW_SET_MSK_VAL(qspi->CONFIG_REG,
                    COM_FRF_Msk,
                    COM_FRF_Pos,
                    workformat);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_GetWorkFormat(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CONFIG_REG,
                            COM_FRF_Msk,
                            COM_FRF_Pos);
}

/* when TMOD = 10/11 set of this field indicate  number of rx*/
/* num < SPI_NDF_MAXNUMBER */
__HW_STATIC_INLINE void LL_SPI_SetReciveNum(qspi_dev_t *qspi, uint32_t num)
{
    HW_SET_MSK_VAL(qspi->RECEIVE_DATA_NUM_REG,
                    REC_DATA_NUM_Msk,
                    REC_DATA_NUM_Pos,
                    num - 1);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetReciveNum(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->RECEIVE_DATA_NUM_REG,
                            REC_DATA_NUM_Msk,
                            REC_DATA_NUM_Pos);
}

/* current data num of fifo */
__HW_STATIC_INLINE uint32_t LL_SPI_GetCurrTxDataNum(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->FIFO_STATUS_REG,
                            TXFS_Msk,
                            TXFS_Pos);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetCurrRxDataNum(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->FIFO_STATUS_REG,
                            RXFS_Msk,
                            RXFS_Pos);
}

///* get spi iden id */
//__HW_STATIC_INLINE uint32_t LL_SPI_GetIdenID(qspi_dev_t *qspi)
//{
//    return HW_GET_MSK_VAL(qspi->RXFLR,
//                            SPI_IDR_Msk,
//                            SPI_IDR_Pos);
//}
//
///* get spi ver id */
//__HW_STATIC_INLINE uint32_t LL_SPI_GetVerID(qspi_dev_t *qspi)
//{
//    return HW_GET_MSK_VAL(qspi->RXFLR,
//                            SPI_VER_Msk,
//                            SPI_VER_Pos);
//}

/* set rx sample delay */
/* delay = wait number of sample clk*/
/*
 * SPI_SAMPLE_DELAY_CLK_ENABLE
 * SPI_SAMPLE_DELAY_CLK_DISABLE
 */

__HW_STATIC_INLINE
void LL_SPI_EnableRxHalfSampleDelayClk(qspi_dev_t *qspi)
{
    HW_SET_BIT(qspi->SAMPLE_CTL_REG, RSE_Pos);
}

__HW_STATIC_INLINE
void LL_SPI_DisableRxHalfSampleDelayClk(qspi_dev_t *qspi)
{
    HW_CLR_BIT(qspi->SAMPLE_CTL_REG, RSE_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_IsableRxHalfSampleDelayClk(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->SAMPLE_CTL_REG,
                          RSE_Msk,
                          RSE_Pos);
}

__HW_STATIC_INLINE
void LL_SPI_SetRxSampleDelay(qspi_dev_t *qspi, uint32_t delay_num)
{
    HW_SET_MSK_VAL(qspi->SAMPLE_CTL_REG,
                   RSD_Msk,
                   RSD_Pos,
                   delay_num);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_GetRxSampleDelay(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->SAMPLE_CTL_REG,
                          RSD_Msk,
                          RSD_Pos);
}

__HW_STATIC_INLINE
void LL_SPI_SetSlvAbdDelay(qspi_dev_t *qspi, uint32_t abd)
{
    HW_SET_MSK_VAL(qspi->CTRL_REG,
                   SS_N_IN_DEGLITCH_SEL_Msk,
                   SS_N_IN_DEGLITCH_SEL_Pos,
                   abd);
}

__HW_STATIC_INLINE
uint32_t LL_SPI_GetSlvAbdDelay(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->CTRL_REG,
                          SS_N_IN_DEGLITCH_SEL_Msk,
                          SS_N_IN_DEGLITCH_SEL_Pos);
}


/* spi control register */

/* only work for dual/quad/octal spi mode */
__HW_STATIC_INLINE void LL_SPI_SetDummyCycles(qspi_dev_t *qspi, uint32_t clk_num)
{
    HW_SET_MSK_VAL(qspi->QSPI_ENHANCE,
                    DUMMY_Msk,
                    DUMMY_Pos,
                    clk_num);
}
/* only work for dual/quad/octal spi mode */
__HW_STATIC_INLINE uint32_t LL_SPI_GetDummyCycles(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->QSPI_ENHANCE,
                            DUMMY_Msk,
                            DUMMY_Pos);
}

__HW_STATIC_INLINE void LL_SPI_SetInstructLenght(qspi_dev_t *qspi, uint32_t ins_len)
{
    HW_SET_MSK_VAL(qspi->QSPI_ENHANCE,
                    INST_B_Msk,
                    INST_B_Pos,
                    ins_len);
}

__HW_STATIC_INLINE uint32_t  LL_SPI_GetInstructLenght  (qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->QSPI_ENHANCE,
                            INST_B_Msk,
                            INST_B_Pos);
}

__HW_STATIC_INLINE void LL_SPI_StretchEnable(qspi_dev_t *qspi)
{
    HW_SET_MSK_VAL(qspi->QSPI_ENHANCE,
                    CLK_STRETCH_EN_Msk,
                    CLK_STRETCH_EN_Pos,
                    1);
}

__HW_STATIC_INLINE void LL_SPI_StretchDisable(qspi_dev_t *qspi)
{
    HW_SET_MSK_VAL(qspi->QSPI_ENHANCE,
                    CLK_STRETCH_EN_Msk,
                    CLK_STRETCH_EN_Pos,
                    0);
}

__HW_STATIC_INLINE uint32_t LL_SPI_StretchIsable(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->QSPI_ENHANCE,
                          CLK_STRETCH_EN_Msk,
                          CLK_STRETCH_EN_Pos);
}


/* only work for dual/quad/octal spi mode */
__HW_STATIC_INLINE void LL_SPI_SetAddrLenght(qspi_dev_t *qspi, uint32_t addr_len)
{
    HW_SET_MSK_VAL(qspi->QSPI_ENHANCE,
                    ADDR_B_Msk,
                    ADDR_B_Pos,
                    addr_len);
}

__HW_STATIC_INLINE uint32_t LL_SPI_GetAddrLenght(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->QSPI_ENHANCE,
                            ADDR_B_Msk,
                            ADDR_B_Pos);
}


__HW_STATIC_INLINE void LL_SPI_SetTransType(qspi_dev_t *qspi, uint32_t tran_type)
{
    HW_SET_MSK_VAL(qspi->QSPI_ENHANCE,
                    TRANS_FORM_Msk,
                    TRANS_FORM_Pos,
                    tran_type);
}

/*
 * SPI_TRAN_INS_AND_ADDR_STANDARD
 * SPI_TRAN_INS_STANDARD_ADDR_TYPE
 * SPI_TRAN_INS_AND_ADDR_TYPE
 */
__HW_STATIC_INLINE uint32_t LL_SPI_GetTransType(qspi_dev_t *qspi)
{
    return HW_GET_MSK_VAL(qspi->QSPI_ENHANCE,
                            TRANS_FORM_Msk,
                            TRANS_FORM_Pos);
}



/**
  * @}
  */
/** @defgroup spi_ll_api  ll api
  * @{
  */
//void psic_hal_spi_read(qspi_dev_t *qspi_dev, void *rx_buff, uint32_t len, uint8_t bitwide);
//void psic_hal_spi_write(qspi_dev_t *qspi_dev, void *tx_buff, uint32_t len, uint8_t bitwide);
//void psic_hal_spi_transfer(qspi_dev_t *qspi_dev,
//                                void *tx_buff,
//                                void *rx_buff,
//                                uint32_t tx_len,
//                                uint32_t rx_len,
//                                uint8_t bitwide);
//
//uint16_t psic_ll_spi_read(qspi_dev_t *qspi_dev, uint8_t bitwide);
//void psic_ll_spi_write(qspi_dev_t *qspi_dev, uint16_t tx_data, uint8_t bitwide);
//void psic_ll_spi_cs_enable(qspi_dev_t *qspi_dev);
//void psic_ll_spi_cs_disable(qspi_dev_t *qspi_dev);





/**
  * @}
  */



/**
  * @}
  */


#ifdef __cplusplus
}

#endif

#endif /* _PSIC_LL_SPI_H */


/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
