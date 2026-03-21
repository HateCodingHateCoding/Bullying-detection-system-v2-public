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

#include "ll_pmu.h"
#include "ll_utils.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_gpio.h"
#include "ll_uart.h"
#include "ll_clock_calib.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Private macros.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_PRINTF_CLK_MSI_FREQ
#define CONFIG_PRINTF_CLK_MSI_FREQ                  32000000
#endif

#ifndef CONFIG_PRINTF_EARLY_CLK_MSI
#define CONFIG_PRINTF_EARLY_CLK_MSI                 1
#endif

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_PRINTF_EARLY
extern void printf_hook_install(int (*fn)(char c));
__sram_text static int printfEarly_char_out(char val);
__sram_text static void printfEarly_HwInit(void * param);
__sram_text static void printfEarly_HwDeinit(void);
#endif

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

#if CONFIG_PRINTF_EARLY
int PrintfEarly_Init(void * param)
{
    printfEarly_HwInit(param);

    printf_hook_install(printfEarly_char_out);

    return HAL_STATUS_OK;
}


__sram_text void PrintfEarly_Suspend(void)
{
    printfEarly_HwDeinit();
}

__sram_text void PrinftEarly_Resume(void)
{
    printfEarly_HwInit(NULL);
}


__sram_text static int printfEarly_char_out(char val)
{
#if (CONFIG_PRINTF_EARLY_UART_NUM == 0)
    LL_UART_SendChar(UART0_DEV, val);
#elif (CONFIG_PRINTF_EARLY_UART_NUM == 1)
    LL_UART_SendChar(UART1_DEV, val);
#elif (CONFIG_PRINTF_EARLY_UART_NUM == 2)
    LL_UART_SendChar(UART2_DEV, val);
#endif
    return 0;
}

__sram_text static void printfEarly_HwInit(void * param)
{
//    LL_RCC_GPIO_EnableBusClock();
//    LL_RCC_GPIO_EnableClock();
//    LL_RCC_GPIO_Reset();

    // for 32MHz
#if (CONFIG_PRINTF_EARLY_CLK_MSI == 1)
    if (param != NULL) {
        LL_CALIB_MSICfgWrite(*(uint32_t *)param);
    }
#else
    LL_RCC_LPCLK_EnableToPMU();
    LL_PMU_DCXOLDO_SoftEnable();
    LL_PMU_DCXO_SoftEnable();

    while (LL_PMU_DCXOLDO_IsReady() == 0) {
    }

    while (LL_PMU_DCXO_IsReady() == 0) {
    }
    LL_PMU_DCXO_EnableToSoc();
#endif


#if (CONFIG_PRINTF_EARLY_UART_NUM == 0)

    LL_GPIO_SetPinOutputType(GPIOB_DEV, 0, LL_GPIO_OUTPUT_PUSH_PULL);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 0, LL_GPIOB_P0_F2_UART0_TX);

#if (CONFIG_PRINTF_EARLY_CLK_MSI == 1)
    LL_RCC_UART0_SetClockSource(LL_RCC_UART0_MSI);
#else
    LL_RCC_UART0_SetClockSource(LL_RCC_UART0_HSE);
#endif
    LL_RCC_UART0_SetPrescaler(LL_RCC_UART0_DIV_1);
    LL_RCC_UART0_EnableBusClock();
    LL_RCC_UART0_EnableClock();
    LL_RCC_UART0_Reset();

    LL_UART_Reset(UART0_DEV);
    LL_UART_DisableHWFlowCtrl(UART0_DEV);
    LL_UART_SetDataWidth(UART0_DEV, LL_UART_DATA_WIDTH_8B);
    LL_UART_SetParity(UART0_DEV, LL_UART_PARITY_NONE);
    LL_UART_SetStopBitsLength(UART0_DEV, LL_UART_STOP_BITS_1);

    LL_UART_RequestRxDataFlush(UART0_DEV);
    LL_UART_RequestTxDataFlush(UART0_DEV);
    LL_UART_EnableFIFO(UART0_DEV);

    LL_UART_SetPrescaler(UART0_DEV, CONFIG_PRINTF_EARLY_UART_BAUDRATE, CONFIG_PRINTF_CLK_MSI_FREQ);

#elif (CONFIG_PRINTF_EARLY_UART_NUM == 1)

    LL_GPIO_SetPinOutputType(GPIOA_DEV, 0, LL_GPIO_OUTPUT_PUSH_PULL);
    LL_GPIO_SetPinFuncMode(GPIOA_DEV, 0, LL_GPIOA_P0_F3_UART1_TX);

#if (CONFIG_PRINTF_EARLY_CLK_MSI == 1)
    LL_RCC_UART1_SetClockSource(LL_RCC_UART1_MSI);
#else
    LL_RCC_UART1_SetClockSource(LL_RCC_UART1_HSE);
#endif
    LL_RCC_UART1_SetPrescaler(LL_RCC_UART1_DIV_1);
    LL_RCC_UART1_EnableBusClock();
    LL_RCC_UART1_EnableClock();
    LL_RCC_UART1_Reset();

    LL_UART_Reset(UART1_DEV);
    LL_UART_DisableHWFlowCtrl(UART1_DEV);
    LL_UART_SetDataWidth(UART1_DEV, LL_UART_DATA_WIDTH_8B);
    LL_UART_SetParity(UART1_DEV, LL_UART_PARITY_NONE);
    LL_UART_SetStopBitsLength(UART1_DEV, LL_UART_STOP_BITS_1);

    LL_UART_RequestRxDataFlush(UART1_DEV);
    LL_UART_RequestTxDataFlush(UART1_DEV);
    LL_UART_EnableFIFO(UART1_DEV);

    LL_UART_SetPrescaler(UART1_DEV, CONFIG_PRINTF_EARLY_UART_BAUDRATE, CONFIG_PRINTF_CLK_MSI_FREQ);

#elif (CONFIG_PRINTF_EARLY_UART_NUM == 2)

    LL_GPIO_SetPinOutputType(GPIOA_DEV, 15, LL_GPIO_OUTPUT_PUSH_PULL);
    LL_GPIO_SetPinFuncMode(GPIOA_DEV, 15, LL_GPIOA_P15_F3_UART2_TX);

#if (CONFIG_PRINTF_EARLY_CLK_MSI == 1)
    LL_RCC_UART2_SetClockSource(LL_RCC_UART2_MSI);
#else
    LL_RCC_UART2_SetClockSource(LL_RCC_UART2_HSE);
#endif
    LL_RCC_UART2_SetPrescaler(LL_RCC_UART2_DIV_1);
    LL_RCC_UART2_EnableBusClock();
    LL_RCC_UART2_EnableClock();
    LL_RCC_UART2_Reset();

    LL_UART_Reset(UART2_DEV);
    LL_UART_DisableHWFlowCtrl(UART2_DEV);
    LL_UART_SetDataWidth(UART2_DEV, LL_UART_DATA_WIDTH_8B);
    LL_UART_SetParity(UART2_DEV, LL_UART_PARITY_NONE);
    LL_UART_SetStopBitsLength(UART2_DEV, LL_UART_STOP_BITS_1);

    LL_UART_RequestRxDataFlush(UART2_DEV);
    LL_UART_RequestTxDataFlush(UART2_DEV);
    LL_UART_EnableFIFO(UART2_DEV);

    LL_UART_SetPrescaler(UART2_DEV, CONFIG_PRINTF_EARLY_UART_BAUDRATE, CONFIG_PRINTF_CLK_MSI_FREQ);
#else
#error "min debug print uart number error"
#endif

}

__sram_text static void printfEarly_HwDeinit(void)
{

#if (CONFIG_PRINTF_EARLY_UART_NUM == 0)
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 0, LL_GPIOx_Pn_F15_DIS);
#elif (CONFIG_PRINTF_EARLY_UART_NUM == 1)
    LL_GPIO_SetPinFuncMode(GPIOA_DEV, 0, LL_GPIOx_Pn_F15_DIS);
#elif (CONFIG_PRINTF_EARLY_UART_NUM == 2)
    LL_GPIO_SetPinFuncMode(GPIOA_DEV, 15, LL_GPIOx_Pn_F15_DIS);
#else
#error "min debug print uart number error"
#endif

}

#endif


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
