/**
 ******************************************************************************
 * @file       hal_dma.h
 * @brief      DMA hadware abstract layer define
 * @verbatim   null
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


/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HAL_DMA_H__
#define __HAL_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup HAL_DRIVER
  * @{
  */


/** @defgroup HAL_DMA_Driver
  * @{
  */


/* Includes ------------------------------------------------------------------*/

#include "ll_i2c.h"
#include "ll_uart.h"
#include "ll_spi.h"

#include "hal_types.h"
#include "hal_dev.h"
#include "hal_os.h"

//#if (CONFIG_PM && CONFIG_HIF_SEND_DMA)
#if (CONFIG_PM)
#define CONFIG_DMA_PM                      1
#else
#define CONFIG_DMA_PM                      0
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */
/**
 * @brief DMA Device ID.
 */
typedef enum {
    DMA0_ID                                 =   (0x0U),
    DMA_NUM,
} DMA_ID_t;

/**
 * @brief DMA channel direction.
 */
typedef enum {
    DMA_DIRECTION_MEM2MEM                  =   (0x00U),
    DMA_DIRECTION_MEM2PER                  =   (0x01U),
    DMA_DIRECTION_PER2MEM                  =   (0x02U),
    DMA_DIRECTION_PER2PER                  =   (0x03U),
} DMA_Direction_t;

/**
 * @brief DMA channel priority. (DMA_PRIORITY_7 most hight)
 */
typedef enum {
    DMA_PRIORITY_0                         =   (0x00U),
    DMA_PRIORITY_1                         =   (0x01U),
    DMA_PRIORITY_2                         =   (0x02U),
    DMA_PRIORITY_3                         =   (0x03U),
    DMA_PRIORITY_4                         =   (0x04U),
    DMA_PRIORITY_5                         =   (0x05U),
    DMA_PRIORITY_6                         =   (0x06U),
    DMA_PRIORITY_7                         =   (0x07U),
} DMA_Priority_t;


/**
 * @brief  DMA Burst Transfer Length
 */
typedef enum {
    DMA_BURSTNUM_1                         =   (0x00U),  /**< Burst length 1 data item. */
    DMA_BURSTNUM_4                         =   (0x01U),  /**< Burst length 4 data items. */
    DMA_BURSTNUM_8                         =   (0x02U),  /**< Burst length 8 data items. */
} DMA_BurstNum_t;

/**
 * @brief  DMA Transfer Width
 */
typedef enum {
    DMA_BITWIDTH_8                          =   (0x00U),   /**< Transfer width of 8 bits. */
    DMA_BITWIDTH_16                         =   (0x01U),   /**< Transfer width of 16 bits. */
    DMA_BITWIDTH_32                         =   (0x02U),   /**< Transfer width of 32 bits. */
} DMA_BitWidth_t;

/**
 * @brief DMA Address whether to increment or decrement.
 */
typedef enum {
    DMA_ADDR_INCREMENT                      =   (0x00U),
    DMA_ADDR_DECREMENT                      =   (0x01U),
    DMA_ADDR_FIXED                          =   (0x02U),
} DMA_AddrIncreMode_t;

/**
 * @brief DMA Handshake types
 */
typedef enum {
    DMA_HANDSHAKE_TXFIFO_UART0              =   (0x00U),
    DMA_HANDSHAKE_RXFIFO_UART0              =   (0x01U),
    DMA_HANDSHAKE_TXFIFO_UART1              =   (0x02U),
    DMA_HANDSHAKE_RXFIFO_UART1              =   (0x03U),
    DMA_HANDSHAKE_TXFIFO_SPI0               =   (0x04U),
    DMA_HANDSHAKE_RXFIFO_SPI0               =   (0x05U),
    DMA_HANDSHAKE_CDAA                      =   (0x06U),
    DMA_HANDSHAKE_GPADC                     =   (0x07U),
    DMA_HANDSHAKE_TXFIFO_IIC0               =   (0x08U),
    DMA_HANDSHAKE_RXFIFO_IIC0               =   (0x09U),
    DMA_HANDSHAKE_TXFIFO_IIC1               =   (0x10U),
    DMA_HANDSHAKE_RXFIFO_IIC1               =   (0x11U),
    DMA_HANDSHAKE_TXFLASHC                  =   (0x12U),
    DMA_HANDSHAKE_RXFLASHC                  =   (0x13U),
    DMA_HANDSHAKE_TXFIFO_UART2              =   (0x14U),
    DMA_HANDSHAKE_RXFIFO_UART2              =   (0x15U),
    DMA_HANDSHAKE_MAX_NUM,
} DMA_HandShake_t;

/**
 * @brief DMA Event
 */
typedef enum {
    DMA_EVENT_DONE                          =   (HAL_BIT(0)),
    DMA_EVENT_BLOCK_DONE                    =   (HAL_BIT(1)),
    DMA_EVENT_ERR                           =   (HAL_BIT(2)),
    DMA_EVENT_ALL = (DMA_EVENT_DONE | DMA_EVENT_BLOCK_DONE | DMA_EVENT_ERR),
} DMA_Event_t;

typedef struct {
    uint32_t  srcAddr;                  /*!< Specifies the Current Source Address.*/
    uint32_t  dstAddr;                  /*!< Specifies the Current Destination Address. */
    uint32_t  blockSize;                /*!< Specifies Block Transfer Size, Max:4096(12 bits).*/
} DMA_BlockConf_t;


typedef struct DMA_Node {
    uint32_t srcAddr;                  /*!< Specifies the Current Source Address.*/
    uint32_t dstAddr;                  /*!< Specifies the Current Destination Address. */
    struct   DMA_Node *pnextNode;      /*!< Specifies the Next Node Address. */
    uint32_t nodeCtrl0;                /* User Not Care */
    uint32_t blockSize     :12;        /*!< Specifies Block Transfer Size, Max:4096(12 bits).*/
    uint32_t nodeCtrl1     :20;        /* User Not Care */
    uint32_t nodeCtrl2;                /* User Not Care */
    uint32_t nodeCtrl3;                /* User Not Care */
} DMA_NodeConf_t;


/**
  *  @brief This structure is used to set configuration param[in] eters
  *         for a channel in the DMA Controller.
  *  @Note1 All of these configuration param[in] eters must be programmed
  *         into the DMA controller before enabling the channel.
  *  @Note2 To initialize the structure, the user should call the
  *         DMA_Channel_InitCFG() after reset of the DMA controller.
  *  @Note3 Call the HAL_DMA_Channel_SetCFG() to set up the DMA channel transfer.
  */
typedef struct {
    uint32_t  direction      : 4;        /*!< Specifies the types of DMA transfer.*/
    uint32_t  priority       : 4;        /*!< Specifies the DMA Channel Priority.*/
    uint32_t  srcIncreMode   : 4;        /*!< Specifies whether to increment or decrement.*/
    uint32_t  dstIncreMode   : 4;        /*!< Specifies whether to increment or decrement.*/
    uint32_t  srcBrustNum    : 4;        /*!< Specifies source burst length.*/
    uint32_t  dstBrustnum    : 4;        /*!< Specifies destination burst length.*/
    uint32_t  srcWidth       : 4;        /*!< Specifies source transfer width.*/
    uint32_t  dstWidth       : 4;        /*!< Specifies destination transfer width.*/
    uint16_t  srcHandShake   : 4;        /*!< Specifies channel handshake interface ID.*/
    uint16_t  dstHandShake   : 4;        /*!< Specifies channel handshake interface ID.*/
    uint16_t  srcEnReload    : 1;        /*!< Specifies Automatically reloaded source at the end of transfers.*/
    uint16_t  dstEnReload    : 1;        /*!< Specifies Automatically reloaded destination at the end of transfers.*/
    uint16_t  srcEnStatus    : 1;        /*!< Specifies Destination Status Update Enable*/
    uint16_t  dstEnStatus    : 1;        /*!< Specifies Source Status Update Enable*/
    uint16_t  pad            : 4;
} DMA_ChannelConf_t;

typedef enum {
    DMA_EXTATTR_WAKEUP_MODE          =    0U,
    DMA_EXTATTR_SUSPEND_CALLBACK     =    1U,
    DMA_EXTATTR_RESUME_CALLBACK      =    2U,
} DMA_ExtAttr_t;

typedef enum {
    DMA_WK_MODE_AUTO,
    DMA_WK_MODE_MANUAL,
}dma_WakeupMode_t;


/* Exported constants.
 * ----------------------------------------------------------------------------
 */
#define DMA_CHANNEL_MAX_NUM                    (0x08U)

/* Exported macro.
 * ----------------------------------------------------------------------------
 */
#define DMA_IRQ_PRIORITY                       (10U)

#define DMA_MIN_CHID                           (0x00U)
#define DMA_MAX_CHID                           (0x07U)

#define DMA_TIMEOUT                            (100U)

#define DMA_REQUEST_CHNUM                      (DMA_CHANNEL_MAX_NUM)

#define DMA_UART0_RX_ADDR          ((uint32_t)(&((uart_dev_t *)UART0_DEV)->RDR))
#define DMA_UART0_TX_ADDR          ((uint32_t)(&((uart_dev_t *)UART0_DEV)->TDR))
#define DMA_UART1_RX_ADDR          ((uint32_t)(&((uart_dev_t *)UART1_DEV)->RDR))
#define DMA_UART1_TX_ADDR          ((uint32_t)(&((uart_dev_t *)UART1_DEV)->TDR))
#define DMA_UART2_RX_ADDR          ((uint32_t)(&((uart_dev_t *)UART2_DEV)->RDR))
#define DMA_UART2_TX_ADDR          ((uint32_t)(&((uart_dev_t *)UART2_DEV)->TDR))
#define DMA_QSPI_TXRX_ADDR         ((uint32_t)(&((qspi_dev_t *)QSPI_DEV)->DR))

#if (CONFIG_SECTION_ATTRIBUTE_SRAM && CONFIG_HIF_SEND_DMA)
#define __dma_sram_text                                 __sram_text
#define __dma_sram_rodata                               __sram_rodata
#define __dma_sram_data                                 __sram_data
#define __dma_sram_bss                                  __sram_bss
#else
#define __dma_sram_text
#define __dma_sram_rodata
#define __dma_sram_data
#define __dma_sram_bss
#endif

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief      Initialise the DMA device.
 * @param[in]  id  DMA Device ID
 * @param[in]  pinitParam
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Dev_t    *HAL_DMA_Init(DMA_ID_t id);

/**
 * @brief      Deinitialise the DMA device.
 * @param[in]  dma  DMA instance
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_DeInit(HAL_Dev_t *dma);

/**
 * @brief      Open the channel of configure.
 * @param[in]  dma  DMA instance
 * @param[in]  pchanCfg
 * @return     Request usful channel id
 */
int32_t      HAL_DMA_Open(HAL_Dev_t *dma, const DMA_ChannelConf_t *pchanCfg);

/**
 * @brief      Close the channel
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_Close(HAL_Dev_t *dma, uint32_t chId);

/**
 * @brief      Close the channel
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_ExtendControl(HAL_Dev_t *dma, uint32_t chId, DMA_ExtAttr_t attr, void *arg);

uint32_t HAL_DMA_BlockSizeGet(HAL_Dev_t *dma, uint32_t chId);
/**
 * @brief      Setup a DMA single block transfer
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[in]  pblockCfg
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_ConfigBlockTranfer(HAL_Dev_t *dma, uint32_t chId,
                                const DMA_BlockConf_t *pblockCfg);

/**
 * @brief      Initialise Linked List Item control register parameters.
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[in]  pcurrentNode
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_InitNodeCtrl(HAL_Dev_t *dma, uint32_t chId,
                            DMA_NodeConf_t *pcurrentNode, uint8_t int_en);

/**
 * @brief      Invert @pnextlistNode after the current location
 * @param[in]  pcurrentNode    Current node address
 * @param[in]  pnextlistNode   Next node address
 */
__HAL_STATIC_INLINE
void HAL_DMA_AddNextNode(DMA_NodeConf_t *pcurrentNode, DMA_NodeConf_t *pnextlistNode)
{
    if (pcurrentNode)
        pcurrentNode->pnextNode = pnextlistNode;
}

/**
 * @brief      Configure user-defined Linked List Pointer (LLP) items.
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[in]  pheadlist
 *
 * @Note  In SRAM to SRAM, Block IRQ and Done IRQ,
 *        Will occurs in the same time maybe.
 *
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_ConfigListTranfer(HAL_Dev_t *dma, uint32_t chId,
                                   const DMA_NodeConf_t *pheadlist);


/**
 * @brief      Start DMA channel transfer.
 * @param[in]  dma  DMA instance
 * @param[in]  chId dma channel id
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_StartTransfer(HAL_Dev_t *dma, uint32_t chId);

/**
 * @brief      Abort DMA channel transfer in process.
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_AbortTransfer(HAL_Dev_t *dma, uint32_t chId);

/**
 * @brief      Register the callback function for each DMA channel.
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[in]  event
 * @param[in]  eventCb
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_RegisterIRQ(HAL_Dev_t *dma, uint32_t chId,
                             DMA_Event_t event, HAL_Callback_t *eventCb);

/**
 * @brief      Unregister the callback function for each DMA channel.
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[in]  event
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_UnRegisterIRQ(HAL_Dev_t *dma, uint32_t chId, DMA_Event_t event);

/**
 * @brief      Enables interrupts for the selected channel
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[in]  event
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_EnableIRQ(HAL_Dev_t *dma, uint32_t chId, DMA_Event_t event);

/**
 * @brief      Disables interrupts for the selected channel
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[in]  event
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Status_t HAL_DMA_DisableIRQ(HAL_Dev_t *dma, uint32_t chId, DMA_Event_t event);

/**
 * @brief      Get channel's status
 * @param[in]  dma   DMA instance
 * @param[in]  chId  dma channel id
 * @param[out] doneBlockNum
 * @return     Channel Busy State.
 */
int32_t      HAL_DMA_GetChanStatus(HAL_Dev_t *dma, uint32_t chId, uint32_t *doneBlockNum);




/**
 * @}  HAL_DMA_Driver
 */

/**
 * @}  HAL_DRIVER
 */

#ifdef __cplusplus
}
#endif

#endif /* __HAL_DMA_H__ */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
