/**
 ******************************************************************************
 * @file    rs7241_config.h
 * @brief   rs7241 config define.
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

#ifndef _RS7241_CONFIG_H
#define _RS7241_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
extern "C" {
#endif


#if (CONFIG_SOC_RS7241_CPUS || CONFIG_SOC_RS7241_CPUF)


 /* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */


#define CONFIG_CPU_RISCV_E906F                          1
#define CONFIG_CPU_IRQ_MAX                              88

/* drivers
 */
#ifndef CONFIG_DRIVER_CLOCK
#define CONFIG_DRIVER_CLLOCK                            1
#define CONFIG_DRIVER_CLOCK_CALIB                       1
#endif

#ifndef CONFIG_DRIVER_EFUSE
#define CONFIG_DRIVER_EFUSE                             0
#endif

#ifndef CONFIG_DRIVER_INIT_CLK
#define CONFIG_DRIVER_INIT_CLK                          1
#endif


#ifndef CONFIG_DRIVER_SPIMEM
#define CONFIG_DRIVER_SPIMEM                            0
#endif

#ifndef CONFIG_DRIVER_INIT_SPIMEM
#define CONFIG_DRIVER_INIT_SPIMEM                      1
#endif


#define CONFIG_DRIVER_WDG_CNT                          2

#ifndef CONFIG_DRIVER_WDG0
#define CONFIG_DRIVER_WDG0                             0
#endif

#ifndef CONFIG_DRIVER_WDG1
#define CONFIG_DRIVER_WDG1                             0
#endif

#define CONFIG_DRIVER_WDG  (CONFIG_DRIVER_WDG0 || CONFIG_DRIVER_WDG1)

#ifndef CONFIG_DRIVER_INIT_WDG0
#define CONFIG_DRIVER_INIT_WDG0                         1
#endif

#ifndef CONFIG_DRIVER_INIT_WDG1
#define CONFIG_DRIVER_INIT_WDG1                         1
#endif

#define CONFIG_DRIVER_INIT_WDG  (CONFIG_DRIVER_INIT_WDG0 || CONFIG_DRIVER_INIT_WDG1)


#ifndef CONFIG_DRIVER_DMA
#define CONFIG_DRIVER_DMA                               0
#endif

#ifndef CONFIG_DRIVER_INIT_DMA
#define CONFIG_DRIVER_INIT_DMA                          1
#endif


#define CONFIG_DRIVER_GPIO_CNT                          2

#ifndef CONFIG_DRIVER_GPIOA
#define CONFIG_DRIVER_GPIOA                             0
#endif

#ifndef CONFIG_DRIVER_GPIOB
#define CONFIG_DRIVER_GPIOB                             0
#endif

#define CONFIG_DRIVER_GPIO  (CONFIG_DRIVER_GPIOA || CONFIG_DRIVER_GPIOB)

#ifndef CONFIG_DRIVER_INIT_GPIOA
#define CONFIG_DRIVER_INIT_GPIOA                        1
#endif

#ifndef CONFIG_DRIVER_INIT_GPIOB
#define CONFIG_DRIVER_INIT_GPIOB                        1
#endif

#define CONFIG_DRIVER_INIT_GPIO  (CONFIG_DRIVER_INIT_GPIOA || CONFIG_DRIVER_INIT_GPIOB)


#define CONFIG_DRIVER_UART_CNT                          3

#ifndef CONFIG_DRIVER_UART
#define CONFIG_DRIVER_UART                              0
#endif

#ifndef CONFIG_DRIVER_INIT_UART0
#define CONFIG_DRIVER_INIT_UART0                        1
#endif

#ifndef CONFIG_DRIVER_INIT_UART1
#define CONFIG_DRIVER_INIT_UART1                        1
#endif

#ifndef CONFIG_DRIVER_INIT_UART2
#define CONFIG_DRIVER_INIT_UART2                        1
#endif

#define CONFIG_DRIVER_INIT_UART  (CONFIG_DRIVER_INIT_UART0 || CONFIG_DRIVER_INIT_UART1 || CONFIG_DRIVER_INIT_UART2)


#define CONFIG_DRIVER_SPI_CNT                           1

#ifndef CONFIG_DRIVER_SPI0
#define CONFIG_DRIVER_SPI0                              0
#endif

#define CONFIG_DRIVER_SPI  (CONFIG_DRIVER_SPI0)

#ifndef CONFIG_DRIVER_INIT_SPI0
#define CONFIG_DRIVER_INIT_SPI0                         1
#endif

#define CONFIG_DRIVER_INIT_SPI  (CONFIG_DRIVER_INIT_SPI0)


#define CONFIG_DRIVER_I2C_CNT                           1

#ifndef CONFIG_DRIVER_I2C0
#define CONFIG_DRIVER_I2C0                              0
#endif

#define CONFIG_DRIVER_I2C  (CONFIG_DRIVER_I2C0)

#ifndef CONFIG_DRIVER_INIT_I2C0
#define CONFIG_DRIVER_INIT_I2C0                         1
#endif

#define CONFIG_DRIVER_INIT_I2C  (CONFIG_DRIVER_INIT_I2C0)

/* Exported macro.
 * ----------------------------------------------------------------------------
 */
#if (CONFIG_SOC_CLOCK_CFG == 32)
#include "rs724x_config_cpuf_32mhz.h"
#elif (CONFIG_SOC_CLOCK_CFG == 96)
#include "rs724x_config_cpuf_96mhz.h"
#elif (CONFIG_SOC_CLOCK_CFG == 112)
#include "rs724x_config_cpuf_112mhz.h"
#elif (CONFIG_SOC_CLOCK_CFG == 160)
#include "rs724x_config_cpuf_160mhz.h"
#elif (CONFIG_SOC_CLOCK_CFG == 192)
#include "rs724x_config_cpuf_192mhz.h"
#elif (CONFIG_SOC_CLOCK_CFG == 224)
#include "rs724x_config_cpuf_224mhz.h"
#elif (CONFIG_SOC_CLOCK_CFG == 240)
#include "rs724x_config_cpuf_240mhz.h"
#elif (CONFIG_SOC_CLOCK_CFG == 256)
#include "rs724x_config_cpuf_256mhz.h"
#endif

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RS7241_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
