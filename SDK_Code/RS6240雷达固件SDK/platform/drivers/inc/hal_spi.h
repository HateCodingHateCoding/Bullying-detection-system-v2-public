/**
 ******************************************************************************
 * @file    hal_spi.h
 * @brief   spi header define.
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

#ifndef _HAL_SPI_H
#define _HAL_SPI_H

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"
#include "ll_spi.h"
//#include "hal_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_SPI_TRANS_IT_ENABLE
#define CONFIG_SPI_TRANS_IT_ENABLE              1
#endif

#ifndef CONFIG_SPI_TRANS_DMA_ENABLE
#define CONFIG_SPI_TRANS_DMA_ENABLE             1
#endif

#ifndef CONFIG_SPI_PM
#define CONFIG_SPI_PM                           CONFIG_PM
#endif

#ifndef CONFIG_SPI_WUP_ENABLED
#define CONFIG_SPI_WUP_ENABLED                  0
#endif

#if (CONFIG_SECTION_ATTRIBUTE_SRAM && (CONFIG_HIF_DEVICE_COM_SPI || CONFIG_DRIVER_SPI0))
#define __spi_sram_text                 __sram_text
#define __spi_sram_rodata               __sram_rodata
#define __spi_sram_data                 __sram_data
#define __spi_sram_bss                  __sram_bss
#else
#define __spi_sram_text
#define __spi_sram_rodata
#define __spi_sram_data
#define __spi_sram_bss
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */
typedef enum {
    SPI0_ID   = 0U,
    SPI_NUM_MAX,
    SPI_INVALID_ID = 0xFFU
} SPI_ID_t;


typedef enum {
    SPI_MODE_SLAVE      = 0,
    SPI_MODE_MASTER     = 1,
} SPI_Mode_t;

typedef enum {
    SPI_SCLK_MODE0 = 0,             // CPOL=0 CPHA=0
    SPI_SCLK_MODE1 = 1,             // CPOL=0 CPHA=1
    SPI_SCLK_MODE2 = 2,             // CPOL=1 CPHA=0
    SPI_SCLK_MODE3 = 3              // CPOL=1 CPHA=1
} SPI_SclkMode_t;

typedef enum {
    SPI_DATAWIDTH_8BIT      = 8,
    SPI_DATAWIDTH_16BIT     = 16,
    SPI_DATAWIDTH_32BIT     = 32,
} SPI_DataWidth_t;

typedef enum {
    SPI_TRANS_MODE_NOMA      = 0,
    SPI_TRANS_MODE_INTERRUPT = 1,
    SPI_TRANS_MODE_DMA       = 2,
} SPI_TransMode_t;

typedef enum {
    SPI_IO_MODE_NORMAL  = LL_QSPI_LINES_STANDARD,           /*!< standar mode */
    SPI_IO_MODE_DUAL    = LL_QSPI_LINES_DUAL,                /*!<  */
    SPI_IO_MODE_QUAD    = LL_QSPI_LINES_QUAD,               /*!<  */
} SPI_IOMode_t;

typedef enum {
    SPI_CS_HARD = 0,
    SPI_CS_SOFT = 1,
} SPI_Cs_t;

typedef enum {
    SPI_DIR_TX               = 0,
    SPI_DIR_RX               = 1,
    SPI_DIR_FULL             = 2,
} SPI_TransDir_t;

typedef enum {
    SPI_TRANS_ALL_STANDARD        = 0,
    SPI_TRANS_INS_STD_ADDR_SEP    = 1,
    SPI_TRANS_ALL_SPECIFIED       = 2,
} SPI_FrameTransMode_t;

typedef enum {
    SPI_FIFO_TX             = 0,
    SPI_FIFO_RX             = 1,
} SPI_FifoType_t;

typedef enum {
    SPI_IRQ_TX              = 0,
    SPI_IRQ_RX              = 1,
    SPI_IRQ_CS              = 2,
} SPI_IRQ_t;

typedef enum {
    SPI_PARAM_TX_START_LEVEL    = 0,
    SPI_PARAM_TX_EMPTY_LEVEL    = 1,
    SPI_PARAM_RX_FULL_LEVEL     = 2,
    SPI_PARAM_TX_DMA_LEVEL      = 3,
    SPI_PARAM_RX_DMA_LEVEL      = 4,
    SPI_PARAM_FRAME_CTRL        = 5,
    SPI_PARAM_WAKEUP_THRESHOLD  = 6,
    SPI_PARAM_WAKEUP_CTRL       = 7,
    SPI_PARAM_WAKEUP_CALLBACK   = 8,
    SPI_PARAM_WAKEUP_MODE       = 9,
} SPI_ParamType_t;

typedef enum {
    SPI_WK_MODE_AUTO = 0,
    SPI_WK_MODE_NOTAUTO = 1,
} SPI_WakeupMode_t;

typedef struct {
    uint8_t                 instruction_len;                /* instruction length. must be 0,4,8,16 */
    uint8_t                 address_len;                    /* address length. must be multiple of 4, max num is 60*/
    SPI_FrameTransMode_t    trans_form;                     /* transmit type @brief SPI transmit type*/
} SPI_FrameCtrl_t;


typedef struct {
    SPI_Mode_t                  mode;       /*!< SPI master mode or slave mode, only master mode for now */
    SPI_SclkMode_t              sclkMode;    /*!< device sclk mode */
    SPI_IOMode_t                ioMode;
    SPI_Cs_t                    csMode;
    SPI_DataWidth_t             dataWidth;
    uint32_t                    sclk;       /*!< device sclk frequency */
} SPI_InitParam_t;



/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
HAL_Dev_t * HAL_SPI_Init(SPI_ID_t id, const SPI_InitParam_t *param, const uint8_t param_num);
HAL_Status_t HAL_SPI_DeInit(HAL_Dev_t *spi);

HAL_Status_t HAL_SPI_Open(HAL_Dev_t *spi, uint8_t slave_addrIndex);

HAL_Status_t HAL_SPI_Close(HAL_Dev_t *spi);

HAL_Status_t HAL_SPI_ExtControl(const HAL_Dev_t *spi, SPI_ParamType_t ptype, void *param);


HAL_Status_t HAL_SPI_SetTransferMode(const HAL_Dev_t *spi,
        SPI_TransDir_t transDir,
        SPI_TransMode_t Mode);

int32_t HAL_SPI_Receive(const HAL_Dev_t *spi,
            uint8_t *buff,
            int32_t size,
            uint32_t msec);

int32_t HAL_SPI_Transmit(const HAL_Dev_t *spi,
                    const uint8_t *buff,
                    int32_t size,
                    uint32_t msec);

int32_t HAL_SPI_TransmitReceive(const HAL_Dev_t *spi,
                    uint8_t *tx_data,
                    uint8_t *rx_data,
                    int32_t size,
                    uint32_t msec);



int32_t HAL_SPI_GetRxData(const HAL_Dev_t *spi);

void HAL_SPI_PutTxData(const HAL_Dev_t *spi, uint32_t data);

HAL_Status_t HAL_SPI_StartTrans(const HAL_Dev_t *spi , SPI_TransDir_t dir);

HAL_Status_t HAL_SPI_StopTrans(const HAL_Dev_t *spi);

int HAL_SPI_IsTxReady(HAL_Dev_t *spi);

int HAL_SPI_IsTxEmpty(HAL_Dev_t *spi);

int HAL_SPI_IsRxReady(HAL_Dev_t *spi);

int HAL_SPI_IsBusy(HAL_Dev_t *spi);

int HAL_SPI_GetFifoLevel(HAL_Dev_t *spi, SPI_FifoType_t type);

int HAL_SPI_IsCsInterrupt(HAL_Dev_t *spi);

HAL_Status_t HAL_SPI_EnableDMA(const HAL_Dev_t *spi , SPI_TransDir_t dir);
HAL_Status_t HAL_SPI_DisableDMA(const HAL_Dev_t *spi , SPI_TransDir_t dir);

HAL_Status_t HAL_SPI_EnableCS(const HAL_Dev_t *spi, uint8_t csId);
HAL_Status_t HAL_SPI_DisableCS(const HAL_Dev_t *spi, uint8_t csId);


#if CONFIG_SPI_TRANS_IT_ENABLE

HAL_Status_t HAL_SPI_RegisterIRQ(HAL_Dev_t *spi, SPI_IRQ_t irq_type, const HAL_Callback_t *irqCb);
HAL_Status_t HAL_SPI_UnregisterIRQ(const HAL_Dev_t *spi, SPI_IRQ_t irq_type);
HAL_Status_t HAL_SPI_EnableIRQ(const HAL_Dev_t *spi, SPI_IRQ_t irq_type);
HAL_Status_t HAL_SPI_DisableIRQ(const HAL_Dev_t *spi, SPI_IRQ_t irq_type);
#else
#define HAL_SPI_RegisterIRQ(spi, irq_type, irqCb)                    (__NOP())
#define HAL_SPI_UnregisterIRQ(spi, irq_type)                         (__NOP())
#define HAL_SPI_EnableIRQ(spi, irq_type)                             (__NOP())
#define HAL_SPI_DisableIRQ(spi, irq_type)                            (__NOP())
#endif


#if CONFIG_SPI_TRANS_DMA_ENABLE

HAL_Status_t HAL_SPI_RegisterDMA(HAL_Dev_t *spi, SPI_TransDir_t transDir, HAL_Callback_t *dmaCb);
HAL_Status_t HAL_SPI_UnregisterDMA(const HAL_Dev_t *spi, SPI_TransDir_t transDir);
HAL_Status_t HAL_SPI_StartDMA(const HAL_Dev_t *spi, SPI_TransDir_t transDir, const uint8_t *buf, int32_t size);
int32_t HAL_SPI_StopDMA(const HAL_Dev_t *spi, SPI_TransDir_t transDir);
#else
#define HAL_SPI_RegisterDMA(spi, transDir, dmaCb)                    (__NOP())
#define HAL_SPI_UnregisterDMA(spi, transDir)                         (__NOP())
#define HAL_SPI_StartDMA(spi, transDir, buf, size)                   (__NOP())
#define HAL_SPI_StopDMA(spi, transDir)                               (__NOP())

#endif /* CONFIG_SPI_TRANS_DMA_ENABLE */

#if CONFIG_SPI_PM
HAL_Status_t HAL_SPI_PM_Suspend(HAL_Dev_t *spi);
HAL_Status_t HAL_SPI_PM_Resume(HAL_Dev_t *spi);
#else
#define HAL_SPI_PM_Suspend(spi)                                 (__NOP())
#define HAL_SPI_PM_Resume(spi)                                  (__NOP())
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SPI_H_ */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */

