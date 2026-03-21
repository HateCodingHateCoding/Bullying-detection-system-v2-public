/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <csi_config.h>

extern void Default_Handler(void);
extern void CORET_IRQHandler(void);
extern void TIMA0_IRQHandler(void);
extern void TIMA1_IRQHandler(void);
extern void TIMB0_IRQHandler(void);
extern void TIMB1_IRQHandler(void);
extern void WDT_IRQHandler(void);
extern void USART0_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void I2C0_IRQHandler(void);
extern void I2C1_IRQHandler(void);
extern void SPI0_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void GPIO0_IRQHandler(void);
extern void GPIO1_IRQHandler(void);
extern void RTC_IRQHandler(void);
extern void RTC1_IRQHandler(void);
extern void AES_IRQHandler(void);
extern void SHA_IRQHandler(void);
extern void RSA_IRQHandler(void);
extern void ADC_IRQHandler(void);
extern void I2S_IRQHandler(void);
extern void DMAC0_IRQHandler(void);
extern void PWM_IRQHandler(void);

void (*g_irqvector[])(void) = {
    GPIO0_IRQHandler,  /* 0 */
    CORET_IRQHandler,  /* 1 */
    TIMA0_IRQHandler,  /* 2 */
    TIMA1_IRQHandler,  /* 3 */
    I2S_IRQHandler,    /* 4 */
    WDT_IRQHandler,    /* 5 */
    USART0_IRQHandler, /* 6 */
    USART1_IRQHandler, /* 7 */
    USART2_IRQHandler, /* 8 */
    I2C0_IRQHandler,   /* 9 */
    I2C1_IRQHandler,   /* 10 */
    SPI1_IRQHandler,   /* 11 */
    SPI0_IRQHandler,   /* 12 */
    RTC_IRQHandler,    /* 13 */
    Default_Handler,   /* 14 */
    ADC_IRQHandler,    /* 15 */
    Default_Handler,   /* 16 */
    DMAC0_IRQHandler,  /* 17 */
    Default_Handler,   /* 18 */
    PWM_IRQHandler,    /* 19 */
    Default_Handler,   /* 20 */
    Default_Handler,   /* 21 */
    Default_Handler,   /* 22 */
    TIMB0_IRQHandler,  /* 23 */
    TIMB1_IRQHandler,  /* 24 */
    Default_Handler,   /* 25 */
    AES_IRQHandler,    /* 26 */
    GPIO1_IRQHandler,  /* 27 */
    RSA_IRQHandler,    /* 28 */
    SHA_IRQHandler,    /* 29 */
};
