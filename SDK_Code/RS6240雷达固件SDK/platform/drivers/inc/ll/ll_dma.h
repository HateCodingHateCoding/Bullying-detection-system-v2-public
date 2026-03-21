/**
 **************************************************************************************************
 * @file    ll_dma.h
 * @brief   dma hadware define.
 * @attention
 *          Copyright (c) 2025 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _LL_DMA_H
#define _LL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
// #include "soc.h"
#include "types.h"
#include "hw_dma.h"


/** @addtogroup LL_Driver
  * @{
  */

/** @defgroup ll_dma
  * @{
  */


/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
#define DMAC_DEV                                           ((dmac_dev_t *)DMAC_BASE_ADDR)  /*!< 0x4002 0C00 */


/** @defgroup XXX
  * @{
  */
#define DMAC_CH0                                            0U
#define DMAC_CH1                                            1U
#define DMAC_CH2                                            2U
#define DMAC_CH3                                            3U
#define DMAC_CH4                                            4U
#define DMAC_CH5                                            5U
#define DMAC_CH6                                            6U
#define DMAC_CH7                                            7U
/**
  * @}
  */


/** @defgroup XXX
  * @{
  */
#define DMAC_CH0_Msk                                       (HW_BIT(DMAC_CH0))
#define DMAC_CH1_Msk                                       (HW_BIT(DMAC_CH1))
#define DMAC_CH2_Msk                                       (HW_BIT(DMAC_CH2))
#define DMAC_CH3_Msk                                       (HW_BIT(DMAC_CH3))
#define DMAC_CH4_Msk                                       (HW_BIT(DMAC_CH4))
#define DMAC_CH5_Msk                                       (HW_BIT(DMAC_CH5))
#define DMAC_CH6_Msk                                       (HW_BIT(DMAC_CH6))
#define DMAC_CH7_Msk                                       (HW_BIT(DMAC_CH7))
#define DMAC_ALLCH_Msk                                     (HW_BIT_MSK(DMAC_MAX_CHANNEL_NUM))
/**
  * @}
  */

/** @defgroup CTRLHR
* @{
*/
#define DMAC_MAX_BLOCK_SIZE                                 BLOCK_TS_Msk
/**
  * @}
  */

/** @defgroup CTRLHR
* @{
*/
#define DMAC_WB_DISABLE                                     (0x00U)
#define DMAC_WB_ENABLE                                      (0x01U)
/**
  * @}
  */

/** @defgroup CTRLLR
* @{
*/
#define DMAC_MASTER_1                                       (0x00U)
#define DMAC_MASTER_2                                       (0x01U)
#define DMAC_MASTER_3                                       (0x02U)
#define DMAC_MASTER_4                                       (0x03U)
/**
  * @}
  */

/** @defgroup CTRLLR
* @{
*/
#define DMAC_TRAN_TYPE_MEM2MEM                              (0x00U)
#define DMAC_TRAN_TYPE_MEM2PER                              (0x01U)
#define DMAC_TRAN_TYPE_PER2MEM                              (0x02U)
#define DMAC_TRAN_TYPE_PER2PER                              (0x03U)
/**
  * @}
  */

/** @defgroup CTRLLR
* @{
*/
#define DMAC_DST_SCATTER_DISABLE                            (0x00U)
#define DMAC_DST_SCATTER_ENABLE                             (0x01U)

#define DMAC_SRC_GATHER_DISABLE                             (0x00U)
#define DMAC_SRC_GATHER_ENABLE                              (0x01U)
/**
  * @}
  */

/** @defgroup
* @{
*/
#define DMAC_MSIZE_1                                        (0x00U)
#define DMAC_MSIZE_4                                        (0x01U)
#define DMAC_MSIZE_8                                        (0x02U)
/**
  * @}
  */


/** @defgroup CTRLLR
* @{
*/
#define DMAC_ADDR_INCREMENT                                 (0x00U)
#define DMAC_ADDR_DECREMENT                                 (0x01U)
#define DMAC_ADDR_NO_CHANGE                                 (0x02U)
//#define DMAC_ADDR_RESERVED                                  (0x03U)
/**
  * @}
  */


/** @defgroup CTRLLR
* @{
*/
#define DMAC_BITWIDTH_8BIT                                  (0x00U)
#define DMAC_BITWIDTH_16BIT                                 (0x01U)
#define DMAC_BITWIDTH_32BIT                                 (0x02U)
/**
  * @}
  */


/** @defgroup CTRLLR
* @{
*/
#define DMAC_INTERRUPT_DISABLE                              (0x00U)
#define DMAC_INTERRUPT_ENABLE                               (0x01U)
/**
  * @}
  */


/** @defgroup CFLR DMAC polarity level
* @{
*/
#define DMAC_SRC_HS_POL_HIGH                                   (0x00U)
#define DMAC_SRC_HS_POL_LOW                                    (0x01U)

#define DMAC_DST_HS_POL_HIGH                                   (0x00U)
#define DMAC_DST_HS_POL_LOW                                    (0x01U)
/**
  * @}
  */

/** @defgroup CFLR DMAC
* @{
*/
#define DMAC_HS_HARDWARE                                       (0x00U)
#define DMAC_HS_SOFTWARE                                       (0x01U)
/**
  * @}
  */

/** @defgroup CFLR DMAC
* @{
*/
#define DMAC_FIFO_ISEMPTY                                      (0x01U)
#define DMAC_FIFO_NOTEMPTY                                     (0x00U)
/**
  * @}
  */

/** @defgroup CFLR DMAC
* @{
*/
#define DMAC_CH_ISSUSP                                         (0x01U)
#define DMAC_CH_NOTSUSP                                        (0x00U)
/**
  * @}
  */


/** @defgroup CFLR DMAC
* @{
*/
#define DMAC_CH_PRIOR_0                                        (0x00U)
#define DMAC_CH_PRIOR_1                                        (0x01U)
#define DMAC_CH_PRIOR_2                                        (0x02U)
#define DMAC_CH_PRIOR_3                                        (0x03U)
#define DMAC_CH_PRIOR_4                                        (0x04U)
#define DMAC_CH_PRIOR_5                                        (0x05U)
#define DMAC_CH_PRIOR_6                                        (0x06U)
#define DMAC_CH_PRIOR_7                                        (0x07U)
/**
  * @}
  */

/** @defgroup CFLR DMAC
* @{
*/
/* CFGHR */
#define DMAC_DST_TRIGGER_TXFIFO_UART0                          (0x00U)
#define DMAC_DST_TRIGGER_RXFIFO_UART0                          (0x01U)
#define DMAC_DST_TRIGGER_TXFIFO_UART1                          (0x02U)
#define DMAC_DST_TRIGGER_RXFIFO_UART1                          (0x03U)
#define DMAC_DST_TRIGGER_TXFIFO_SPI0                           (0x04U)
#define DMAC_DST_TRIGGER_RXFIFO_SPI0                           (0x05U)
#define DMAC_DST_TRIGGER_CDAA                                  (0x06U)
#define DMAC_DST_TRIGGER_GPADC                                 (0x07U)
#define DMAC_DST_TRIGGER_TXFIFO_IIC0                           (0x08U)
#define DMAC_DST_TRIGGER_RXFIFO_IIC0                           (0x09U)
#define DMAC_DST_TRIGGER_TXFIFO_IIC1                           (0x10U)
#define DMAC_DST_TRIGGER_RXFIFO_IIC1                           (0x11U)
#define DMAC_DST_TRIGGER_TXFLASHC                              (0x12U)
#define DMAC_DST_TRIGGER_RXFLASHC                              (0x13U)
#define DMAC_DST_TRIGGER_TXFIFO_UART2                          (0x14U)
#define DMAC_DST_TRIGGER_RXFIFO_UART2                          (0x15U)

#define DMAC_SRC_TRIGGER_TXFIFO_UART0                          (0x00U)
#define DMAC_SRC_TRIGGER_RXFIFO_UART0                          (0x01U)
#define DMAC_SRC_TRIGGER_TXFIFO_UART1                          (0x02U)
#define DMAC_SRC_TRIGGER_RXFIFO_UART1                          (0x03U)
#define DMAC_SRC_TRIGGER_TXFIFO_SPI0                           (0x04U)
#define DMAC_SRC_TRIGGER_RXFIFO_SPI0                           (0x05U)
#define DMAC_SRC_TRIGGER_CDAA                                  (0x06U)
#define DMAC_SRC_TRIGGER_GPADC                                 (0x07U)
#define DMAC_SRC_TRIGGER_TXFIFO_IIC0                           (0x08U)
#define DMAC_SRC_TRIGGER_RXFIFO_IIC0                           (0x09U)
#define DMAC_SRC_TRIGGER_TXFIFO_IIC1                           (0x10U)
#define DMAC_SRC_TRIGGER_RXFIFO_IIC1                           (0x11U)
#define DMAC_SRC_TRIGGER_TXFLASHC                              (0x12U)
#define DMAC_SRC_TRIGGER_RXFLASHC                              (0x13U)
#define DMAC_SRC_TRIGGER_TXFIFO_UART2                          (0x14U)
#define DMAC_SRC_TRIGGER_RXFIFO_UART2                          (0x15U)
/**
  * @}
  */

/** @defgroup CFLR DMAC
* @{
*/
#define DMAC_PROTCTL_TO_HPROT0
#define DMAC_PROTCTL_TO_HPROT1
#define DMAC_PROTCTL_TO_HPROT2
#define DMAC_PROTCTL_TO_HPROT3
/**
  * @}
  */

/** @defgroup CFLR Space/data available for single
*             AHB transfer of the specified transfer width
* @{
*/
#define DMAC_TRAN_FIFO_MODE0                                   (0x00U)
#define DMAC_TRAN_FIFO_MODE1                                   (0x01U)
/**
  * @}
  */

/** @defgroup CFLR Flow control mode on a DMA channel
* @{
*/
#define DMAC_FC_MODE_DISABLE                                   (0x00U)
#define DMAC_FC_MODE_ENABLE                                    (0x01U)
/**
  * @}
  */


/* ISPR_TFR */
#define DMAC_TFR_CH0_INT_PENDING                               (HW_BIT(0))
#define DMAC_TFR_CH1_INT_PENDING                               (HW_BIT(1))
#define DMAC_TFR_CH2_INT_PENDING                               (HW_BIT(2))
#define DMAC_TFR_CH3_INT_PENDING                               (HW_BIT(3))
#define DMAC_TFR_CH4_INT_PENDING                               (HW_BIT(4))
#define DMAC_TFR_CH5_INT_PENDING                               (HW_BIT(5))
#define DMAC_TFR_CH6_INT_PENDING                               (HW_BIT(6))
#define DMAC_TFR_CH7_INT_PENDING                               (HW_BIT(7))

/* ISPR_BLOCK */
#define DMAC_BLOCK_CH0_INT_PENDING                             (HW_BIT(0))
#define DMAC_BLOCK_CH1_INT_PENDING                             (HW_BIT(1))
#define DMAC_BLOCK_CH2_INT_PENDING                             (HW_BIT(2))
#define DMAC_BLOCK_CH3_INT_PENDING                             (HW_BIT(3))
#define DMAC_BLOCK_CH4_INT_PENDING                             (HW_BIT(4))
#define DMAC_BLOCK_CH5_INT_PENDING                             (HW_BIT(5))
#define DMAC_BLOCK_CH6_INT_PENDING                             (HW_BIT(6))
#define DMAC_BLOCK_CH7_INT_PENDING                             (HW_BIT(7))


/* ISPR_ERR */
#define DMAC_ERR_CH0_INT_PENDING                               (HW_BIT(0))
#define DMAC_ERR_CH1_INT_PENDING                               (HW_BIT(1))
#define DMAC_ERR_CH2_INT_PENDING                               (HW_BIT(2))
#define DMAC_ERR_CH3_INT_PENDING                               (HW_BIT(3))
#define DMAC_ERR_CH4_INT_PENDING                               (HW_BIT(4))
#define DMAC_ERR_CH5_INT_PENDING                               (HW_BIT(5))
#define DMAC_ERR_CH6_INT_PENDING                               (HW_BIT(6))
#define DMAC_ERR_CH7_INT_PENDING                               (HW_BIT(7))

/* ISR_TFR */
#define DMAC_TFR_CH0_INT_ACTIVE                                (HW_BIT(0))
#define DMAC_TFR_CH1_INT_ACTIVE                                (HW_BIT(1))
#define DMAC_TFR_CH2_INT_ACTIVE                                (HW_BIT(2))
#define DMAC_TFR_CH3_INT_ACTIVE                                (HW_BIT(3))
#define DMAC_TFR_CH4_INT_ACTIVE                                (HW_BIT(4))
#define DMAC_TFR_CH5_INT_ACTIVE                                (HW_BIT(5))
#define DMAC_TFR_CH6_INT_ACTIVE                                (HW_BIT(6))
#define DMAC_TFR_CH7_INT_ACTIVE                                (HW_BIT(7))

/* ISR_BLOCK */
#define DMAC_BLOCK_CH0_INT_ACTIVE                              (HW_BIT(0))
#define DMAC_BLOCK_CH1_INT_ACTIVE                              (HW_BIT(1))
#define DMAC_BLOCK_CH2_INT_ACTIVE                              (HW_BIT(2))
#define DMAC_BLOCK_CH3_INT_ACTIVE                              (HW_BIT(3))
#define DMAC_BLOCK_CH4_INT_ACTIVE                              (HW_BIT(4))
#define DMAC_BLOCK_CH5_INT_ACTIVE                              (HW_BIT(5))
#define DMAC_BLOCK_CH6_INT_ACTIVE                              (HW_BIT(6))
#define DMAC_BLOCK_CH7_INT_ACTIVE                              (HW_BIT(7))


/* ISR_EER */
#define DMAC_ERR_CH0_INT_ACTIVE                                (HW_BIT(0))
#define DMAC_ERR_CH1_INT_ACTIVE                                (HW_BIT(1))
#define DMAC_ERR_CH2_INT_ACTIVE                                (HW_BIT(2))
#define DMAC_ERR_CH3_INT_ACTIVE                                (HW_BIT(3))
#define DMAC_ERR_CH4_INT_ACTIVE                                (HW_BIT(4))
#define DMAC_ERR_CH5_INT_ACTIVE                                (HW_BIT(5))
#define DMAC_ERR_CH6_INT_ACTIVE                                (HW_BIT(6))
#define DMAC_ERR_CH7_INT_ACTIVE                                (HW_BIT(7))

/* IER_TFR */
#define DMAC_TFR_INT_CH0Msk_UnMsk                              (HW_BIT(0))
#define DMAC_TFR_INT_CH1Msk_UnMsk                              (HW_BIT(1))
#define DMAC_TFR_INT_CH2Msk_UnMsk                              (HW_BIT(2))
#define DMAC_TFR_INT_CH3Msk_UnMsk                              (HW_BIT(3))
#define DMAC_TFR_INT_CH4Msk_UnMsk                              (HW_BIT(4))
#define DMAC_TFR_INT_CH5Msk_UnMsk                              (HW_BIT(5))
#define DMAC_TFR_INT_CH6Msk_UnMsk                              (HW_BIT(6))
#define DMAC_TFR_INT_CH7Msk_UnMsk                              (HW_BIT(7))

#define DMAC_TFR_INT_CH0_ENABLE                                (HW_BIT(0))
#define DMAC_TFR_INT_CH1_ENABLE                                (HW_BIT(1))
#define DMAC_TFR_INT_CH2_ENABLE                                (HW_BIT(2))
#define DMAC_TFR_INT_CH3_ENABLE                                (HW_BIT(3))
#define DMAC_TFR_INT_CH4_ENABLE                                (HW_BIT(4))
#define DMAC_TFR_INT_CH5_ENABLE                                (HW_BIT(5))
#define DMAC_TFR_INT_CH6_ENABLE                                (HW_BIT(6))
#define DMAC_TFR_INT_CH7_ENABLE                                (HW_BIT(7))

/* IER_BLOCK */
#define DMAC_BLOCK_INT_CH0Msk_UnMsk                             (HW_BIT(0))
#define DMAC_BLOCK_INT_CH1Msk_UnMsk                             (HW_BIT(1))
#define DMAC_BLOCK_INT_CH2Msk_UnMsk                             (HW_BIT(2))
#define DMAC_BLOCK_INT_CH3Msk_UnMsk                             (HW_BIT(3))
#define DMAC_BLOCK_INT_CH4Msk_UnMsk                             (HW_BIT(4))
#define DMAC_BLOCK_INT_CH5Msk_UnMsk                             (HW_BIT(5))
#define DMAC_BLOCK_INT_CH6Msk_UnMsk                             (HW_BIT(6))
#define DMAC_BLOCK_INT_CH7Msk_UnMsk                             (HW_BIT(7))

#define DMAC_BLOCK_INT_CH0Msk_Msk                               (HW_BIT(0))
#define DMAC_BLOCK_INT_CH1Msk_Msk                               (HW_BIT(1))
#define DMAC_BLOCK_INT_CH2Msk_Msk                               (HW_BIT(2))
#define DMAC_BLOCK_INT_CH3Msk_Msk                               (HW_BIT(3))
#define DMAC_BLOCK_INT_CH4Msk_Msk                               (HW_BIT(4))
#define DMAC_BLOCK_INT_CH5Msk_Msk                               (HW_BIT(5))
#define DMAC_BLOCK_INT_CH6Msk_Msk                               (HW_BIT(6))
#define DMAC_BLOCK_INT_CH7Msk_Msk                               (HW_BIT(7))

#define DMAC_BLOCK_INT_CH0_ENABLE                               (HW_BIT(0))
#define DMAC_BLOCK_INT_CH1_ENABLE                               (HW_BIT(1))
#define DMAC_BLOCK_INT_CH2_ENABLE                               (HW_BIT(2))
#define DMAC_BLOCK_INT_CH3_ENABLE                               (HW_BIT(3))
#define DMAC_BLOCK_INT_CH4_ENABLE                               (HW_BIT(4))
#define DMAC_BLOCK_INT_CH5_ENABLE                               (HW_BIT(5))
#define DMAC_BLOCK_INT_CH6_ENABLE                               (HW_BIT(6))
#define DMAC_BLOCK_INT_CH7_ENABLE                               (HW_BIT(7))

#define DMAC_BLOCK_INT_CH0_DISABLE                              (HW_BIT(0))
#define DMAC_BLOCK_INT_CH1_DISABLE                              (HW_BIT(1))
#define DMAC_BLOCK_INT_CH2_DISABLE                              (HW_BIT(2))
#define DMAC_BLOCK_INT_CH3_DISABLE                              (HW_BIT(3))
#define DMAC_BLOCK_INT_CH4_DISABLE                              (HW_BIT(4))
#define DMAC_BLOCK_INT_CH5_DISABLE                              (HW_BIT(5))
#define DMAC_BLOCK_INT_CH6_DISABLE                              (HW_BIT(6))
#define DMAC_BLOCK_INT_CH7_DISABLE                              (HW_BIT(7))


/* IER_ERR */
#define DMAC_ERR_INT_CH0Msk_UnMsk                               (HW_BIT(0))
#define DMAC_ERR_INT_CH1Msk_UnMsk                               (HW_BIT(1))
#define DMAC_ERR_INT_CH2Msk_UnMsk                               (HW_BIT(2))
#define DMAC_ERR_INT_CH3Msk_UnMsk                               (HW_BIT(3))
#define DMAC_ERR_INT_CH4Msk_UnMsk                               (HW_BIT(4))
#define DMAC_ERR_INT_CH5Msk_UnMsk                               (HW_BIT(5))
#define DMAC_ERR_INT_CH6Msk_UnMsk                               (HW_BIT(6))
#define DMAC_ERR_INT_CH7Msk_UnMsk                               (HW_BIT(7))

#define DMAC_ERR_INT_CH0_ENABLE                                 (HW_BIT(0))
#define DMAC_ERR_INT_CH1_ENABLE                                 (HW_BIT(1))
#define DMAC_ERR_INT_CH2_ENABLE                                 (HW_BIT(2))
#define DMAC_ERR_INT_CH3_ENABLE                                 (HW_BIT(3))
#define DMAC_ERR_INT_CH4_ENABLE                                 (HW_BIT(4))
#define DMAC_ERR_INT_CH5_ENABLE                                 (HW_BIT(5))
#define DMAC_ERR_INT_CH6_ENABLE                                 (HW_BIT(6))
#define DMAC_ERR_INT_CH7_ENABLE                                 (HW_BIT(7))

/* ICR_TFR */
#define DMAC_TFR_INT_CH0_CLR                                    (HW_BIT(0))
#define DMAC_TFR_INT_CH1_CLR                                    (HW_BIT(1))
#define DMAC_TFR_INT_CH2_CLR                                    (HW_BIT(2))
#define DMAC_TFR_INT_CH3_CLR                                    (HW_BIT(3))
#define DMAC_TFR_INT_CH4_CLR                                    (HW_BIT(4))
#define DMAC_TFR_INT_CH5_CLR                                    (HW_BIT(5))
#define DMAC_TFR_INT_CH6_CLR                                    (HW_BIT(6))
#define DMAC_TFR_INT_CH7_CLR                                    (HW_BIT(7))

/* ICR_BLOCK */
#define DMAC_BLOCK_INT_CH0_CLR                                  (HW_BIT(0))
#define DMAC_BLOCK_INT_CH1_CLR                                  (HW_BIT(1))
#define DMAC_BLOCK_INT_CH2_CLR                                  (HW_BIT(2))
#define DMAC_BLOCK_INT_CH3_CLR                                  (HW_BIT(3))
#define DMAC_BLOCK_INT_CH4_CLR                                  (HW_BIT(4))
#define DMAC_BLOCK_INT_CH5_CLR                                  (HW_BIT(5))
#define DMAC_BLOCK_INT_CH6_CLR                                  (HW_BIT(6))
#define DMAC_BLOCK_INT_CH7_CLR                                  (HW_BIT(7))

/* ICR_EER */
#define DMAC_ERR_INT_CH0_CLR                                    (HW_BIT(0))
#define DMAC_ERR_INT_CH1_CLR                                    (HW_BIT(1))
#define DMAC_ERR_INT_CH2_CLR                                    (HW_BIT(2))
#define DMAC_ERR_INT_CH3_CLR                                    (HW_BIT(3))
#define DMAC_ERR_INT_CH4_CLR                                    (HW_BIT(4))
#define DMAC_ERR_INT_CH5_CLR                                    (HW_BIT(5))
#define DMAC_ERR_INT_CH6_CLR                                    (HW_BIT(6))
#define DMAC_ERR_INT_CH7_CLR                                    (HW_BIT(7))

/* SR */
#define DMAC_TFR_FLAG_ACTIVE                                    (HW_BIT(0))
#define DMAC_BLOCK_FLAG_ACTIVE                                  (HW_BIT(1))
#define DMAC_STRAN_FLAG_ACTIVE                                  (HW_BIT(2))
#define DMAC_DTRAN_FLAG_ACTIVE                                  (HW_BIT(3))
#define DMAC_EER_FLAG_ACTIVE                                    (HW_BIT(4))


/* DCR */
#define DMAC_DISABLE                                            (0x00U)
#define DMAC_ENABLE                                             (0x01U)

/* DCH_CR */
#define DMAC_CH0Msk_UnMsk                                       (HW_BIT(0))
#define DMAC_CH1Msk_UnMsk                                       (HW_BIT(1))
#define DMAC_CH2Msk_UnMsk                                       (HW_BIT(2))
#define DMAC_CH3Msk_UnMsk                                       (HW_BIT(3))
#define DMAC_CH4Msk_UnMsk                                       (HW_BIT(4))
#define DMAC_CH5Msk_UnMsk                                       (HW_BIT(5))
#define DMAC_CH6Msk_UnMsk                                       (HW_BIT(6))
#define DMAC_CH7Msk_UnMsk                                       (HW_BIT(7))

#define DMAC_CH0_ENABLE                                         (HW_BIT(0))
#define DMAC_CH1_ENABLE                                         (HW_BIT(1))
#define DMAC_CH2_ENABLE                                         (HW_BIT(2))
#define DMAC_CH3_ENABLE                                         (HW_BIT(3))
#define DMAC_CH4_ENABLE                                         (HW_BIT(4))
#define DMAC_CH5_ENABLE                                         (HW_BIT(5))
#define DMAC_CH6_ENABLE                                         (HW_BIT(6))
#define DMAC_CH7_ENABLE                                         (HW_BIT(7))


 #define DMAC_REMAP_CH                                          (0x00)
 #define DMAC_REMAP_MASTER                                      (0x01)
 #define DMAC_REMAP_BPS                                         (0x02)

/**
  * @brief dmac address remap cpu select.
  */
#define DMAC_REMAP_CPUS                          (0x0)
#define DMAC_REMAP_CPUF                          (0x1)


/* Exported functions --------------------------------------------------------*/
/*!< CONTROLLER EN CFGR */
/**
 * @brief  DMAC enable function
 * @param  DMA  DMAC Reg
 * @retval None
 */
__HW_STATIC_INLINE
void LL_DMAC_Enable(dmac_dev_t *dma)
{
    HW_SET_BIT(dma->DMAEN, DMA_EN_Pos);
}

/**
 * @brief  DMAC disable function
 * @param  DMA  DMAC Reg
 * @retval None
 */
__HW_STATIC_INLINE
void LL_DMAC_Disable(dmac_dev_t *dma)
{
    HW_CLR_BIT(dma->DMAEN, DMA_EN_Pos);
}

/**
 * @brief  DMAC isable function
 * @param  DMA  DMAC Reg
 * @retval 1(enable) / 0(disable)
 */
__HW_STATIC_INLINE
uint32_t LL_DMAC_Isable(dmac_dev_t *dma)
{
    return HW_GET_MSK_VAL(dma->DMAEN, DMA_EN_Msk, DMA_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnableCh(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_MSK(dma->CHEN,
               (HW_BIT(ch_num) << CH_EN_WE_Pos) |
               (HW_BIT(ch_num) << CH_EN_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_DisableCh(dmac_dev_t *dma, uint32_t ch_num)
{
#if 0
    // Invalid operation
    HW_CLR_MSK(dma->CHEN,
               (HW_BIT(ch_num) << CH_EN_WE_Pos) |
               (HW_BIT(ch_num) << CH_EN_Pos));
#endif
    /* ex: ch_0: 0x100 ch_1: 0x200 */
    HW_SET_VAL(dma->CHEN, (HW_BIT(ch_num) << CH_EN_WE_Pos));
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsableCh(dmac_dev_t *dma, uint32_t ch_num)
{
#if 0
    return HW_GET_MSK(dma->CHEN,
          (HW_BIT(ch_num) << CH_EN_WE_Pos) |
          (HW_BIT(ch_num) << CH_EN_Pos));
#endif
    return HW_GET_MSK(dma->CHEN, HW_BIT(ch_num) << CH_EN_Pos) >> ch_num;
}

__HW_STATIC_INLINE
void LL_DMAC_EnableALLCh(dmac_dev_t *dma)
{
    HW_SET_MSK(dma->CHEN, CH_EN_WE_Msk | CH_EN_Msk);
}

__HW_STATIC_INLINE
void LL_DMAC_DisableALLCh(dmac_dev_t *dma)
{
    HW_CLR_MSK(dma->CHEN, CH_EN_WE_Msk | CH_EN_Msk);
}

/**
 * @brief  DMAC Enable channel irq function
 * @param  DMA  DMAC Reg
 * @param  CH Number of channel, maximun value depends on the DMAC_MAX_CHANNEL_NUM
 * @retval None
 */
__HW_STATIC_INLINE
void LL_DMAC_EnableChINT(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CTLL, INT_EN_Pos);
}

/**
 * @brief  DMAC Disnable channel irq function
 * @param  DMA  DMAC Reg
 * @param  CH Number of channel, maximun value depends on the DMAC_MAX_CHANNEL_NUM
 * @retval None
 */
__HW_STATIC_INLINE
void LL_DMAC_DisableChINT(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CTLL, INT_EN_Pos);
}

/**
 * @brief  DMAC Disnable channel irq function
 * @param  DMA  DMAC Reg
 * @param  CH Number of channel, maximun value depends on the DMAC_MAX_CHANNEL_NUM
 * @retval
 */
__HW_STATIC_INLINE
uint32_t LL_DMAC_IsableChINT(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL, INT_EN_Msk, INT_EN_Pos);
}

/**
 * @brief  DMAC Enable channel irq function
 * @param  DMA  DMAC Reg
 * @param  CH Number of channel, maximun value depends on the DMAC_MAX_CHANNEL_NUM
 * @retval None
 */
__HW_STATIC_INLINE
void LL_DMAC_EnableAllCh_INT(dmac_dev_t *dma)
{
    for (int ch_num = 0; ch_num < DMAC_MAX_CHANNEL_NUM; ch_num++) {
        HW_SET_BIT(dma->CHANNEL[ch_num].CTLL, INT_EN_Pos);
    }
}

/**
 * @brief  DMAC Disable all channel irq function
 * @param  DMA  DMAC Reg
 * @param  CH Number of channel, maximun value depends on the DMAC_MAX_CHANNEL_NUM
 * @retval None
 */
__HW_STATIC_INLINE
void LL_DMAC_DisableAllCh_INT(dmac_dev_t *dma)
{
    for (int ch_num = 0; ch_num < DMAC_MAX_CHANNEL_NUM; ch_num++) {
        HW_CLR_BIT(dma->CHANNEL[ch_num].CTLL, INT_EN_Pos);
    }
}



/*!< INT STATUSR FUNCTION */
__HW_STATIC_INLINE
uint32_t LL_DMAC_IsActiveFlag_TFR(dmac_dev_t *dma)
{
    return HW_GET_MSK_VAL(dma->STATUSINT, TFR_CPLT_Msk, TFR_CPLT_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsActiveFlag_BLOCK(dmac_dev_t *dma)
{
    return HW_GET_MSK_VAL(dma->STATUSINT, BLOCK_Msk, BLOCK_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsActiveFlag_ERR(dmac_dev_t *dma)
{
    return HW_GET_MSK_VAL(dma->STATUSINT, ERR_Msk, ERR_Pos);
}


/*!< INTR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_MaskChINT_TFR(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_MSK(dma->MASKTFR,
               (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
               (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_UnMaskChINT_TFR(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_MSK(dma->MASKTFR,
               (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
               (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsMaskChINT_TFR(dmac_dev_t *dma, uint32_t ch_num)
{
#if 0
    return HW_GET_MSK(dma->MASKTFR,
                      (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
                      (HW_BIT(ch_num) << INT_MASK_Pos));
#endif
    return !!HW_GET_MSK(dma->MASKTFR, (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_MaskChINT_BLOCK(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_MSK(dma->MASKBLOCK,
               (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
               (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_UnMaskChINT_BLOCK(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_MSK(dma->MASKBLOCK,
               (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
               (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsMaskChINT_BLOCK(dmac_dev_t *dma, uint32_t ch_num)
{
#if 0
    return HW_GET_MSK(dma->MASKBLOCK,
                      (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
                      (HW_BIT(ch_num) << INT_MASK_Pos));
#endif
    return HW_GET_MSK(dma->MASKBLOCK, (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_MaskChINT_ERR(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_MSK(dma->MASKERR,
               (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
               (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_UnMaskChINT_ERR(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_MSK(dma->MASKERR,
               (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
               (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsMaskChINT_ERR(dmac_dev_t *dma, uint32_t ch_num)
{
#if 0
    return HW_GET_MSK(dma->MASKERR,
                      (HW_BIT(ch_num) << INT_MASK_WE_Pos) |
                      (HW_BIT(ch_num) << INT_MASK_Pos));
#endif
    return !!HW_GET_MSK(dma->MASKERR, (HW_BIT(ch_num) << INT_MASK_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_MaskCh_AllINT(dmac_dev_t *dma, uint32_t ch_id)
{
    LL_DMAC_MaskChINT_TFR(dma, ch_id);
    LL_DMAC_MaskChINT_BLOCK(dma, ch_id);
    LL_DMAC_MaskChINT_ERR(dma, ch_id);
}

__HW_STATIC_INLINE
void LL_DMAC_UnMaskCh_AllINT(dmac_dev_t *dma, uint32_t ch_id)
{
    LL_DMAC_UnMaskChINT_TFR(dma, ch_id);
    LL_DMAC_UnMaskChINT_BLOCK(dma, ch_id);
    LL_DMAC_UnMaskChINT_ERR(dma, ch_id);
}

__HW_STATIC_INLINE
void LL_DMAC_MaskAllCh_INT(dmac_dev_t *dma)
{
    HW_CLR_MSK(dma->MASKBLOCK, INT_MASK_WE_Msk | INT_MASK_Msk);
    HW_CLR_MSK(dma->MASKTFR, INT_MASK_WE_Msk | INT_MASK_Msk);
    HW_CLR_MSK(dma->MASKERR, INT_MASK_WE_Msk | INT_MASK_Msk);
}

__HW_STATIC_INLINE
void LL_DMAC_UnMaskAllCh_INT(dmac_dev_t *dma)
{
    HW_SET_MSK(dma->MASKBLOCK, INT_MASK_WE_Msk | INT_MASK_Msk);
    HW_SET_MSK(dma->MASKTFR, INT_MASK_WE_Msk | INT_MASK_Msk);
    HW_SET_MSK(dma->MASKERR, INT_MASK_WE_Msk | INT_MASK_Msk);
}


__HW_STATIC_INLINE
uint32_t LL_DMAC_IsPendingChINT_TFR(dmac_dev_t *dma, uint32_t ch_num)
{
    return !!HW_GET_MSK(dma->RAWTFR,
                      (HW_BIT(ch_num) << RAW_Pos));
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsPendingChINT_BLOCK(dmac_dev_t *dma, uint32_t ch_num)
{
    return !!HW_GET_MSK(dma->RAWBLOCK,
                      (HW_BIT(ch_num) << RAW_Pos));
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsPendingChINT_ERR(dmac_dev_t *dma, uint32_t ch_num)
{
    return !!HW_GET_MSK(dma->RAWERR,
                      (HW_BIT(ch_num) << RAW_Pos));
}

__HW_STATIC_INLINE
void LL_DMAC_ClearChINT_TFR(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CLEARTFR, ch_num);
}

__HW_STATIC_INLINE
void LL_DMAC_ClearChINT_BLOCK(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CLEARBLOCK, ch_num);

}

__HW_STATIC_INLINE
void LL_DMAC_ClearChINT_ERR(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CLEARERR, ch_num);
}

__HW_STATIC_INLINE
void LL_DMAC_ClearCh_AllINT(dmac_dev_t *dma, uint32_t ch_id)
{
    LL_DMAC_ClearChINT_TFR(dma, ch_id);
    LL_DMAC_ClearChINT_BLOCK(dma, ch_id);
    LL_DMAC_ClearChINT_ERR(dma, ch_id);
}

__HW_STATIC_INLINE
void LL_DMAC_ClearAllCh_INT(dmac_dev_t *dma)
{
    HW_SET_MSK(dma->CLEARTFR, CLEAR_Msk);
    HW_SET_MSK(dma->CLEARBLOCK, CLEAR_Msk);
    HW_SET_MSK(dma->CLEARERR, CLEAR_Msk);
}



/*!< SAR/DAR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_SetChSrcAddress(dmac_dev_t *dma, uint32_t ch_num, uint32_t src_addr)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].SAR, SAR_Msk, SAR_Pos, src_addr);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChSrcAddress(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].SAR, SAR_Msk, SAR_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChDstAddress(dmac_dev_t *dma, uint32_t ch_num, uint32_t dst_addr)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].DAR, DAR_Msk, DAR_Pos, dst_addr);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChDstAddress(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].DAR, DAR_Msk, DAR_Pos);
}


/* <! LLPR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_ResetLLP(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].LLP, LMS_Msk, LMS_Pos, 0);
}

__HW_STATIC_INLINE
void LL_DMAC_SelectListMaster(dmac_dev_t *dma, uint32_t ch_num, uint32_t master)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].LLP, LMS_Msk, LMS_Pos, master);
}


__HW_STATIC_INLINE
void LL_DMAC_SetLLItemAddr(dmac_dev_t *dma, uint32_t ch_num, uint32_t lp_addr)
{
    /* Tow LSBs of start address are not be stored */
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].LLP, LOC_Msk, LOC_Pos, (lp_addr >> 2));
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetLLItemAddr(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].LLP, LOC_Msk, LOC_Pos);
}


/*!< SGR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_SetSrcGatherCnt(dmac_dev_t *dma, uint32_t ch_num, uint32_t cnt)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].SGR,
                   SGC_Msk, SGC_Pos, cnt);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetSrcGatherCnt(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].SGR,
                          SGC_Msk, SGC_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetSrcGatherInterval(dmac_dev_t *dma, uint32_t ch_num, uint32_t interval)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].SGR,
                   SGI_Msk, SGI_Pos, interval);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetSrcGatherInterval(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].SGR,
                          SGI_Msk, SGI_Pos);
}


/*!< DSR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_SetDstScatterCnt(dmac_dev_t *dma, uint32_t ch_num, uint32_t cnt)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].DSR,
                   DSC_Msk, DSC_Pos, cnt);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetDstScatterCnt(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].DSR,
                          DSC_Msk, DSC_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetDstScatterInterval(dmac_dev_t *dma, uint32_t ch_num, uint32_t interval)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].DSR,
                   DSI_Msk, DSI_Pos, interval);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetDstScatterInterval(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].DSR,
                          DSI_Msk, DSI_Pos);
}



/*!< CTRLXLowR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_EnableSrcLLP(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CTLL,
               LLP_SRC_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisableSrcLLP(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CTLL,
               LLP_SRC_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnableDstLLP(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CTLL,
               LLP_DST_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisableDstLLP(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CTLL,
               LLP_DST_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetSrcMasterCh(dmac_dev_t *dma, uint32_t ch_num, uint32_t master_ch)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   SMS_Msk,
                   SMS_Pos,
                   master_ch);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetSrcMasterCh(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          SMS_Msk,
                          SMS_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetDstMasterCh(dmac_dev_t *dma, uint32_t ch_num, uint32_t master_ch)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   DMS_Msk,
                   DMS_Pos,
                   master_ch);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetDstMasterCh(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          DMS_Msk,
                          DMS_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChTransferDirection(dmac_dev_t *dma, uint32_t ch_num, uint32_t direction)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   TT_FC_Msk,
                   TT_FC_Pos,
                   direction);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChTransferDirection(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          TT_FC_Msk,
                          TT_FC_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnableSrcGather(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CTLL,
               SRC_GATHER_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisableSrcGather(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CTLL,
               SRC_GATHER_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsableSrcGather(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_TEST_BIT(dma->CHANNEL[ch_num].CTLL,
                        SRC_GATHER_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnableDstScatter(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CTLL,
               DST_SCATTER_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisableDstScatter(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CTLL,
               DST_SCATTER_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsableDstScatter(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_TEST_BIT(dma->CHANNEL[ch_num].CTLL,
                        DST_SCATTER_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChSrcBitWidth(dmac_dev_t *dma, uint32_t ch_num, uint32_t src_bitwidth)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   SRC_TR_WIDTH_Msk,
                   SRC_TR_WIDTH_Pos,
                   src_bitwidth);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChSrcBitWidth(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          SRC_TR_WIDTH_Msk,
                          SRC_TR_WIDTH_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChDstBitWidth(dmac_dev_t *dma, uint32_t ch_num, uint32_t dst_bitwidth)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   DST_TR_WIDTH_Msk,
                   DST_TR_WIDTH_Pos,
                   dst_bitwidth);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChDstBitWidth(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          DST_TR_WIDTH_Msk,
                          DST_TR_WIDTH_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChSrcIncMode(dmac_dev_t *dma, uint32_t ch_num, uint32_t mode)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   SINC_Msk,
                   SINC_Pos,
                   mode);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChSrcIncMode(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          SINC_Msk,
                          SINC_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChDstIncMode(dmac_dev_t *dma, uint32_t ch_num, uint32_t mode)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   DINC_Msk,
                   DINC_Pos,
                   mode);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChDstIncMode(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          DINC_Msk,
                          DINC_Pos);
}


/*!< CTRLHighR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_SetChBlockTransferSize(dmac_dev_t *dma, uint32_t ch_num, uint32_t block_size)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLH,
                   BLOCK_TS_Msk,
                   BLOCK_TS_Pos,
                   block_size);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChBlockTransferSize(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLH,
                          BLOCK_TS_Msk,
                          BLOCK_TS_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnWriteBackLLIStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CTLH, DONE_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisWriteBackLLIStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CTLH, DONE_Pos);
}


/*!< CFGXLowR FUNCTION */
/**
 * @brief  DMAC Set channel src burst size function
 * @param  DMA  DMAC Reg
 * @param  Ch Number of channel, maximun value depends on the DMAC_MAX_CHANNEL_NUM
 * @param  size burst size
 * @retval None
 *
 * @note   For configuration attributes set as peripheral,
 *         burst transfer must match the peripheral's trigger level.
 */
__HW_STATIC_INLINE
void LL_DMAC_SetChBurstSrcSize(dmac_dev_t *dma, uint32_t ch_num, uint32_t burst_size)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   SRC_MSIZE_Msk,
                   SRC_MSIZE_Pos,
                   burst_size);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChBurstSrcSize(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          SRC_MSIZE_Msk,
                          SRC_MSIZE_Pos);
}

/**
 * @brief  DMAC Set channel src burst size function
 * @param  DMA  DMAC Reg
 * @param  Ch Number of channel, maximun value depends on the DMAC_MAX_CHANNEL_NUM
 * @param  size burst size
 * @retval None
 *
 * @note   For configuration attributes set as peripheral,
 *         burst transfer must match the peripheral's trigger level.
 */
__HW_STATIC_INLINE
void LL_DMAC_SetChBurstDstSize(dmac_dev_t *dma, uint32_t ch_num, uint32_t burst_size)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                   DEST_MSIZE_Msk,
                   DEST_MSIZE_Pos,
                   burst_size);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChBurstDstSize(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CTLL,
                          DEST_MSIZE_Msk,
                          DEST_MSIZE_Pos);
}


/*!< CFGLowR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_StartSourceReload(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CFGL, RELOAD_SRC_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_StopSourceReload(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CFGL, RELOAD_SRC_Pos);
}
__HW_STATIC_INLINE
void LL_DMAC_StartDestinationReload(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CFGL, RELOAD_DST_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_StopDestinationReload(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CFGL, RELOAD_DST_Pos);
}

#if 1
__HW_STATIC_INLINE
void LL_DMAC_SetChSrcHandshakPol(dmac_dev_t *dma, uint32_t ch_num, uint32_t hs_pol)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                   SRC_HS_POL_Msk,
                   SRC_HS_POL_Pos,
                   hs_pol);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChSrcHandshakPol(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                          SRC_HS_POL_Msk,
                          SRC_HS_POL_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChDstHandshakPol(dmac_dev_t *dma, uint32_t ch_num, uint32_t hs_pol)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                   DST_HS_POL_Msk,
                   DST_HS_POL_Pos,
                   hs_pol);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChDstHandshakPol(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                          DST_HS_POL_Msk,
                          DST_HS_POL_Pos);
}


__HW_STATIC_INLINE
void LL_DMAC_SetChSrcHandshakSel(dmac_dev_t *dma, uint32_t ch_num, uint32_t hs_sel)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                   HS_SEL_SRC_Msk,
                   HS_SEL_SRC_Pos,
                   hs_sel);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChSrcHandshakSel(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                          HS_SEL_SRC_Msk,
                          HS_SEL_SRC_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChDstHandshakSel(dmac_dev_t *dma, uint32_t ch_num, uint32_t hs_sel)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                   HS_SEL_DST_Msk,
                   HS_SEL_DST_Pos,
                   hs_sel);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChDstHandshakSel(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                          HS_SEL_DST_Msk,
                          HS_SEL_DST_Pos);
}
#endif
__HW_STATIC_INLINE
uint32_t LL_DMAC_IsAbortCh(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                          CH_SUSP_Msk, CH_SUSP_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_AbortChannel(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CFGL, CH_SUSP_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_AbortCh_All(dmac_dev_t *dma)
{
    for (int ch_num = 0; ch_num < DMAC_MAX_CHANNEL_NUM; ch_num++) {
        HW_SET_BIT(dma->CHANNEL[ch_num].CFGL,
                   CH_SUSP_Pos);
    }
}

__HW_STATIC_INLINE
void LL_DMAC_ResumeChannel(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CFGL, CH_SUSP_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_ResumeCh_All(dmac_dev_t *dma)
{
    for (int ch_num = 0; ch_num < DMAC_MAX_CHANNEL_NUM; ch_num++) {
        HW_CLR_BIT(dma->CHANNEL[ch_num].CFGL,
                   CH_SUSP_Pos);
    }
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsChActiveFlag_FIFOEMPTY(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                          FIFO_EMPTY_Msk,
                          FIFO_EMPTY_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetChPriority(dmac_dev_t *dma, uint32_t ch_num, uint32_t priority)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                   CH_PRIOR_Msk,
                   CH_PRIOR_Pos,
                   priority);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetChPriority(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGL,
                          CH_PRIOR_Msk,
                          CH_PRIOR_Pos);
}



/*!< CFGXHighR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_SetSrcPerConnect(dmac_dev_t *dma, uint32_t ch_num, uint32_t per_trigger)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                   SRC_PER_Msk,
                   SRC_PER_Pos,
                   per_trigger);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetSrcPerConnect(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                          SRC_PER_Msk,
                          SRC_PER_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetDstPerConnect(dmac_dev_t *dma, uint32_t ch_num, uint32_t per_trigger)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                   DEST_PER_Msk,
                   DEST_PER_Pos,
                   per_trigger);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetDstPerConnect(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                          DEST_PER_Msk,
                          DEST_PER_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnSrcUpdStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CFGH, SS_UPD_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisSrcUpdStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CFGH, SS_UPD_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsableSrcUpdStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                          SS_UPD_EN_Msk,
                          SS_UPD_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnDstUpdStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CFGH, DS_UPD_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisDstUpdStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CFGH, DS_UPD_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsableDstUpdStatus(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                          DS_UPD_EN_Msk,
                          DS_UPD_EN_Pos);
}

#if 0
__HW_STATIC_INLINE
void LL_DMAC_SetAHBProtectAttribute(dmac_dev_t *dma, uint32_t ch_num,
                                        uint32_t hprot_signal)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                   PROTCTL_Msk,
                   PROTCTL_Pos,
                   hprot_signal);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetAHBProtectAttribute(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                                PROTCTL_Msk,
                                PROTCTL_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_EnableChFifoMode(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_BIT(dma->CHANNEL[ch_num].CFGH,
               FIFOMODE_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_DisableChFifoMode(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_BIT(dma->CHANNEL[ch_num].CFGH,
               FIFOMODE_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_IsableChFifoMode(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].CFGH,
                          FIFOMODE_Msk,
                          FIFOMODE_Pos);
}
#endif

/*!< ADDRSTATU FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_SetSrcStatuAddr(dmac_dev_t *dma, uint32_t ch_num,
                                        uint32_t statu_addr)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].SSTATAR,
                        SSTATAR_Msk,
                        SSTATAR_Pos,
                        statu_addr);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetSrcStatuAddr(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].SSTATAR,
                                        SSTATAR_Msk,
                                        SSTATAR_Pos);
}

__HW_STATIC_INLINE
void LL_DMAC_SetDstStatuAddr(dmac_dev_t *dma, uint32_t ch_num,
                                     uint32_t statu_addr)
{
    HW_SET_MSK_VAL(dma->CHANNEL[ch_num].DSTATAR,
                        DSTATAR_Msk,
                        DSTATAR_Pos,
                        statu_addr);
}

__HW_STATIC_INLINE
uint32_t LL_DMAC_GetDstStatuAddr(dmac_dev_t *dma, uint32_t ch_num)
{
    return HW_GET_MSK_VAL(dma->CHANNEL[ch_num].DSTATAR,
                                        DSTATAR_Msk,
                                        DSTATAR_Pos);
}


/*!< REMAPCTRLR FUNCTION */
__HW_STATIC_INLINE
void LL_DMAC_SetCh2CpuS(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_CLR_MSK(dma->DMAREMAPCTRL, HW_BIT(ch_num));
}

__HW_STATIC_INLINE
void LL_DMAC_SetCh2CpuF(dmac_dev_t *dma, uint32_t ch_num)
{
    HW_SET_MSK(dma->DMAREMAPCTRL, HW_BIT(ch_num));
}

__HW_STATIC_INLINE
void LL_DMAC_EnableCpu2RemapMaster1(dmac_dev_t *dma, uint32_t cpu_id)
{
    if (cpu_id) {
        HW_SET_BIT(dma->DMAREMAPCTRL, FORCE_DMA1_REMAP_F_Pos);
    } else {
        HW_CLR_BIT(dma->DMAREMAPCTRL, FORCE_DMA1_REMAP_F_Pos);
    }
}

__HW_STATIC_INLINE
void LL_DMAC_EnableCpu2RemapMaster2(dmac_dev_t *dma, uint32_t cpu_id)
{
    if (cpu_id) {
        HW_SET_BIT(dma->DMAREMAPCTRL, FORCE_DMA2_REMAP_F_Pos);
    } else {
        HW_CLR_BIT(dma->DMAREMAPCTRL, FORCE_DMA2_REMAP_F_Pos);
    }
}

__HW_STATIC_INLINE
void LL_DMAC_EnableRemapMaster(dmac_dev_t *dma, uint32_t cpu_id, uint32_t master_num)
{
    if (master_num == DMAC_MASTER_1) {
        HW_SET_BIT(dma->DMAREMAPCTRL, FORCE_DMA1_REMAP_EN_Pos);
        LL_DMAC_EnableCpu2RemapMaster1(dma, cpu_id);
    } else if (master_num == DMAC_MASTER_2) {
        HW_SET_BIT(dma->DMAREMAPCTRL, FORCE_DMA2_REMAP_EN_Pos);
        LL_DMAC_EnableCpu2RemapMaster2(dma, cpu_id);
    } else {
        HW_SET_BIT(dma->DMAREMAPCTRL, FORCE_DMA1_REMAP_EN_Pos);
        LL_DMAC_EnableCpu2RemapMaster1(dma, cpu_id);
    }
}

__HW_STATIC_INLINE
void LL_DMAC_EnBypassMaster(dmac_dev_t *dma, uint32_t master)
{
    if (master) {
        HW_SET_BIT(dma->DMAREMAPCTRL, BPS_DMA2_REMPA_EN_Pos);
    } else {
        HW_SET_BIT(dma->DMAREMAPCTRL, BPS_DMA1_REMPA_EN_Pos);
    }
}


/*!< REMAPLOCKR FUNCTION */
/* read clear */
__HW_STATIC_INLINE
uint32_t LL_DMAC_ChannelTrylock(dmac_dev_t *dma, uint32_t channel)
{
    return HW_GET_VAL(dma->CH_REMAP_LOCK[channel * 2]);
}

/* Set 0 -> 1 */
__HW_STATIC_INLINE
void LL_DMAC_ChannelTryUnlock(dmac_dev_t *dma, uint32_t channel)
{
    HW_SET_VAL(dma->CH_REMAP_LOCK[channel * 2], 0);
}




#ifdef __cplusplus
}
#endif


#endif /* _LL_DMA_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
