/**
 ******************************************************************************
 * @file    uart.h
 * @brief   uart define.
 * @note    If using a baud rate of 1M or higher, it is recommended to
 *          enable CONFIG_CLK_DCXO_EN and CONFIG_CLK_DCXO_OUT_PLL and
 *          set CONFIG_CLK_PLL_SRC to 1, in prj_config.h
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

#ifndef _HAL_UART_H
#define _HAL_UART_H

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_board.h"
#include "hal_os.h"
#include "ll_uart.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_UART_TRF_IT_ENABLE
#define CONFIG_UART_TRF_IT_ENABLE               1
#endif

#ifndef CONFIG_UART_TRF_DMA_ENABLE
#define CONFIG_UART_TRF_DMA_ENABLE              1
#endif

#ifndef CONFIG_UART_PM
#define CONFIG_UART_PM                          CONFIG_PM
#endif

#ifndef CONFIG_UART_WUP_ENABLED
#define CONFIG_UART_WUP_ENABLED                 0
#endif

#ifndef CONFIG_UART_MAX_BAUDRATE
#define CONFIG_UART_MAX_BAUDRATE                4000000
#endif

#if (CONFIG_SECTION_ATTRIBUTE_SRAM && CONFIG_HIF_DEVICE_COM_UART)
#define __uart_sram_text              __sram_text
#define __uart_sram_rodata            __sram_rodata
#define __uart_sram_data              __sram_data
#define __uart_sram_bss               __sram_bss
#else
#define __uart_sram_text
#define __uart_sram_rodata
#define __uart_sram_data
#define __uart_sram_bss
#endif


/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/** @defgroup UART_Data_Width UART Data Width
  * @{
  */
typedef enum {
    UART_DATA_WIDTH_5               = LL_UART_DATA_WIDTH_5B,
    UART_DATA_WIDTH_6               = LL_UART_DATA_WIDTH_6B,
    UART_DATA_WIDTH_7               = LL_UART_DATA_WIDTH_7B,
    UART_DATA_WIDTH_8               = LL_UART_DATA_WIDTH_8B,
} UART_DataWidth_t;


/** @defgroup UART_Stop_Bits   UART Number of Stop Bits
  * @{
  */
typedef enum {
    UART_STOP_BIT_1                 = LL_UART_STOP_BITS_1,
    UART_STOP_BIT_2                 = LL_UART_STOP_BITS_2,
} UART_StopBit_t;

/** @defgroup UART_Parity  UART Parity
  * @{
  */
typedef enum {
    UART_PARITY_NONE                = LL_UART_PARITY_NONE,
    UART_PARITY_ODD                 = LL_UART_PARITY_ODD,
    UART_PARITY_EVEN                = LL_UART_PARITY_EVEN,
} UART_Parity_t;

/**
  * @}
  */

/** @defgroup UART_Hardware_Flow_Control UART Hardware Flow Control
  * @{
  */
typedef enum {
    UART_FLOW_CTRL_NONE             = 0,
    UART_FLOW_CTRL_AUTO             = 1,
} UART_FlowCtrl_t;

typedef enum {
    UART_PARAM_WAKEUP_THRESHOLD  = 0,
    UART_PARAM_WAKEUP_CTRL       = 1,
    UART_PARAM_WAKEUP_CALLBACK   = 2,
    UART_PARAM_WAKEUP_MODE       = 3,
} UART_ParamType_t;


/* Exported types.
 * ----------------------------------------------------------------------------
 */

typedef enum {
    UART0_ID = 0U,
    UART1_ID,
    UART2_ID,
    UART_NUM,
    UART_INVALID_ID = 0xFFU
} UART_ID_t;

/**
  * @brief HAL UART Init Structure definition
  */
typedef struct {
    uint32_t            baudRate;
    UART_DataWidth_t    dataBits;
    UART_StopBit_t      stopBits;
    UART_Parity_t       parity;
    UART_FlowCtrl_t     autoFlowCtrl;
} UART_InitParam_t;

typedef enum {
    UART_TRANS_MODE_NOMA      = 0,
    UART_TRANS_MODE_INTERRUPT = 1,
    UART_TRANS_MODE_DMA       = 2,
} UART_TransMode_t;

typedef enum {
    UART_DIR_TX               = 0,
    UART_DIR_RX               = 1,
} UART_TransDir_t;

typedef enum {
    UART_WK_MODE_AUTO = 0,
    UART_WK_MODE_NOTAUTO = 1,
} UART_WakeupMode_t;

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
HAL_Status_t HAL_UART_ExtControl(const HAL_Dev_t *uart, UART_ParamType_t ptype, void *param);

HAL_Dev_t * HAL_UART_Init(UART_ID_t id, const UART_InitParam_t *param);
HAL_Status_t HAL_UART_DeInit(HAL_Dev_t *uart);

HAL_Status_t HAL_UART_Open(HAL_Dev_t *uart);
HAL_Status_t HAL_UART_Close(HAL_Dev_t *uart);

HAL_Status_t HAL_UART_SetTransferMode(const HAL_Dev_t *uart,
        UART_TransDir_t transDir,
        UART_TransMode_t Mode);

int32_t HAL_UART_Receive(const HAL_Dev_t *uart,
                    uint8_t *buff,
                    int32_t size,
                    uint32_t msec);

int32_t HAL_UART_Transmit(const HAL_Dev_t *uart,
                    const uint8_t *buff,
                    int32_t size,
                    uint32_t msec);


uint8_t HAL_UART_GetRxData(const HAL_Dev_t *uart);
void HAL_UART_PutTxData(const HAL_Dev_t *uart, uint8_t data);
bool HAL_UART_IsTxReady(const HAL_Dev_t *uart);
bool HAL_UART_IsTxEmpty(const HAL_Dev_t *uart);
bool HAL_UART_IsRxReady(const HAL_Dev_t *uart);


#if CONFIG_UART_TRF_IT_ENABLE
HAL_Status_t HAL_UART_RegisterIRQ(HAL_Dev_t *uart, UART_TransDir_t transDir, const HAL_Callback_t *irqCb);
HAL_Status_t HAL_UART_UnregisterIRQ(const HAL_Dev_t *uart, UART_TransDir_t transDir);
HAL_Status_t HAL_UART_EnableTxReadyIRQ(const HAL_Dev_t *uart);
HAL_Status_t HAL_UART_EnableRxReadyIRQ(const HAL_Dev_t *uart);
HAL_Status_t HAL_UART_DisableTxReadyIRQ(const HAL_Dev_t *uart);
HAL_Status_t HAL_UART_DisableRxReadyIRQ(const HAL_Dev_t *uart);
#else
#define HAL_UART_RegisterIRQ(uart, transDir, irqCb)             (__NOP())
#define HAL_UART_UnregisterIRQ(uart, transDir)                  (__NOP())
#define HAL_UART_EnableTxReadyIRQ(uart)                         (__NOP())
#define HAL_UART_EnableRxReadyIRQ(uart)                         (__NOP())
#define HAL_UART_DisableTxReadyIRQ(uart)                        (__NOP())
#define HAL_UART_DisableRxReadyIRQ(uart)                        (__NOP())

#endif


#if CONFIG_UART_TRF_DMA_ENABLE
HAL_Status_t HAL_UART_RegisterDMA(HAL_Dev_t *uart, UART_TransDir_t transDir, HAL_Callback_t *dmaCb);
HAL_Status_t HAL_UART_UnregisterDMA(const HAL_Dev_t *uart, UART_TransDir_t transDir);
HAL_Status_t HAL_UART_StartTransmitDMA(const HAL_Dev_t *uart, const uint8_t *buf, int32_t size);
int32_t HAL_UART_StopTransmitDMA(const HAL_Dev_t *uart);
HAL_Status_t HAL_UART_StartReceiveDMA(const HAL_Dev_t *uart, uint8_t *buf, int32_t size);
int32_t HAL_UART_StopReceiveDMA(const HAL_Dev_t *uart);
#else
#define HAL_UART_RegisterDMA(uart, transDir, dmaCb)             (__NOP())
#define HAL_UART_UnregisterDMA(uart, transDir)                  (__NOP())
#define HAL_UART_StartTransmitDMA(uart, buf, size)              (__NOP())
#define HAL_UART_StopTransmitDMA(uart)                          (__NOP())
#define HAL_UART_StartReceiveDMA(uart, buf, size)               (__NOP())
#define HAL_UART_StopReceiveDMA(uart)                           (__NOP())
#endif /* CONFIG_UART_TRF_DMA_ENABLE */

#if CONFIG_UART_PM
HAL_Status_t HAL_UART_PM_Suspend(HAL_Dev_t *uart);
HAL_Status_t HAL_UART_PM_Resume(HAL_Dev_t *uart);
#else
#define HAL_UART_PM_Suspend(uart)                                 (__NOP())
#define HAL_UART_PM_Resume(uart)                                  (__NOP())
#endif


#ifdef __cplusplus
}
#endif

#endif /* _HAL_UART_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */

