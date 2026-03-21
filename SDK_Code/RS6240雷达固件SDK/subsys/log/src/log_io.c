/**
 ******************************************************************************
 * @file    log_io.c
 * @brief   log io control define.
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

#include "hal_types.h"
#include "hal_dev.h"
#include "log.h"
#include "ll_gpio.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define LOG_IO_TOGGLE_MASK                      0xF0
#define LOG_IO_VALUE_MASK                       0x0F
/* Private macros.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_LOG_IO_NUM_MAX
#define CONFIG_LOG_IO_NUM_MAX                   16
#endif

#ifndef CONFIG_LOG_IO_DEV
#define CONFIG_LOG_IO_DEV                       GPIOA_DEV
#endif
/* Private variables.
 * ----------------------------------------------------------------------------
 */
#if (CONFIG_LOG_IO_TAG_MASK && CONFIG_LOG_IO_MASK)
static uint32_t logIoTag;
static uint32_t logIoValue;
static uint32_t logIoMask;
#endif

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
#if (CONFIG_LOG_IO_TAG_MASK && CONFIG_LOG_IO_MASK)
static void log_IO_Ctrl(uint8_t pinNum, uint8_t val);
#endif

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

#if (CONFIG_LOG_IO_TAG_MASK && CONFIG_LOG_IO_MASK)

__sram_text void LOG_IO_INIT(uint32_t tag, uint32_t mask)
{
    uint32_t pinNum;
    logIoTag = tag;
    logIoMask = mask;

    if (logIoMask != 0) {
        LL_GPIO_ResetOutputPin(CONFIG_LOG_IO_DEV, logIoMask);

        for (pinNum = 0 ; pinNum < CONFIG_LOG_IO_NUM_MAX; pinNum++) {
            if ((logIoMask & HAL_BIT(pinNum)) == 0)
                continue;

            LL_GPIO_SetPinFuncMode(CONFIG_LOG_IO_DEV, pinNum, LL_GPIOx_Pn_F1_OUTPUT);
            LL_GPIO_SetPinOutputType(CONFIG_LOG_IO_DEV, pinNum, LL_GPIO_OUTPUT_PUSH_PULL);

            LL_GPIO_SetOutputPin(CONFIG_LOG_IO_DEV, HAL_BIT(pinNum));
        }

        LL_GPIO_ResetOutputPin(CONFIG_LOG_IO_DEV, logIoMask);
    }
}

__sram_text void LOG_IO_RESUME(void)
{
    uint32_t pinNum = 0;

    if (logIoMask != 0) {
        for (pinNum = 0 ; pinNum < CONFIG_LOG_IO_NUM_MAX; pinNum++) {
            if ((logIoMask & HAL_BIT(pinNum)) == 0)
                continue;

            LL_GPIO_SetPinFuncMode(CONFIG_LOG_IO_DEV, pinNum, LL_GPIOx_Pn_F1_OUTPUT);
            LL_GPIO_SetPinOutputType(CONFIG_LOG_IO_DEV, pinNum, LL_GPIO_OUTPUT_PUSH_PULL);
            if ((logIoValue & HAL_BIT(pinNum)) != 0) {
                LL_GPIO_SetOutputPin(CONFIG_LOG_IO_DEV, HAL_BIT(pinNum));
            } else {
                LL_GPIO_ResetOutputPin(CONFIG_LOG_IO_DEV, HAL_BIT(pinNum));
            }
        }
    }
}

__sram_text void LOG_IO(uint32_t tag, uint8_t num, uint8_t val, uint8_t flip)
{
    if (logIoTag & tag) {
        if (flip) {
            if (flip == LOG_IO_FLIP_TOGGLE) {
                log_IO_Ctrl(num, LOG_IO_TOGGLE_MASK);
            } else {
                log_IO_Ctrl(num, 0);
                for (int i = 0 ; i < flip; i++) {
                    log_IO_Ctrl(num, 1);
                    log_IO_Ctrl(num, 0);
                }
            }
        } else {
            log_IO_Ctrl(num, val);
        }
    }
}

__sram_text static void log_IO_Ctrl(uint8_t pinNum, uint8_t val)
{
    if (pinNum < CONFIG_LOG_IO_NUM_MAX) {
        if ((logIoMask & HAL_BIT(pinNum)) != 0) {
            if (val & LOG_IO_TOGGLE_MASK) {
                logIoValue ^= HAL_BIT(pinNum);
                if (logIoValue & HAL_BIT(pinNum)) {
                    LL_GPIO_SetOutputPin(CONFIG_LOG_IO_DEV, HAL_BIT(pinNum));
                } else {
                    LL_GPIO_ResetOutputPin(CONFIG_LOG_IO_DEV, HAL_BIT(pinNum));
                }
            } else {
                if (val & LOG_IO_VALUE_MASK) {
                    HAL_SET_BIT(logIoValue, pinNum);
                    LL_GPIO_SetOutputPin(CONFIG_LOG_IO_DEV, HAL_BIT(pinNum));
                } else {
                    HAL_CLR_BIT(logIoValue, pinNum);
                    LL_GPIO_ResetOutputPin(CONFIG_LOG_IO_DEV, HAL_BIT(pinNum));
                }
            }
        }
    }
}

#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
