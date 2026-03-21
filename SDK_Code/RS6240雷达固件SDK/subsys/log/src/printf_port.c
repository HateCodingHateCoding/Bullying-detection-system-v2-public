/**
 ******************************************************************************
 * @file    printf_port.c
 * @brief   printf port define.
 * @verbatim    null
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


/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "common.h"
#include "log_config.h"

#include "hal_uart.h"
#include "ll_uart.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_PRINTF
static HAL_Dev_t *printfDev;
#endif

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

extern void printf_hook_install(int (*fn)(char c));

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_PRINTF

__keep_sram_text int printf_char_out(char c)
{
    if (printfDev != NULL) {
        LL_UART_SendChar(printfDev->reg, c);
        //LL_UART_SendChar(UART0_DEV, c);
    }

    // HAL_DevState_t PmState =HAL_DEV_GetState(printfDev);

    // if (PmState == HAL_DEV_STATE_ACTIVE) {
    //     LL_UART_SendChar(printfDev->reg, c);
    // } else if ((PmState == HAL_DEV_STATE_DEEP_SLEEP)
    //     || (PmState == HAL_DEV_STATE_LIGHT_SLEEP)) {
    // }

    return HAL_STATUS_OK;
}


int Printf_Init(void)
{
#if CONFIG_PRINTF_UART
    HAL_Status_t status = HAL_STATUS_OK;

    UART_InitParam_t UartParam = {
        .baudRate = CONFIG_PRINTF_UART_BAUDRATE,
        .parity = UART_PARITY_NONE,
        .stopBits = UART_STOP_BIT_1,
        .dataBits = UART_DATA_WIDTH_8,
        .autoFlowCtrl = UART_FLOW_CTRL_NONE
    };

    UART_ID_t uart_index = CONFIG_PRINTF_UART_NUM;
    printfDev = HAL_UART_Init(uart_index, &UartParam);
    if (printfDev == NULL) {
        return HAL_STATUS_ERROR;
    }
    status = HAL_UART_Open(printfDev);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    status = HAL_UART_SetTransferMode(printfDev, UART_DIR_TX, UART_TRANS_MODE_NOMA);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    printf_hook_install(printf_char_out);

#endif /* CONFIG_PRINTF_UART */


    return HAL_STATUS_OK;
}

#endif /* CONFIG_PRINTF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
