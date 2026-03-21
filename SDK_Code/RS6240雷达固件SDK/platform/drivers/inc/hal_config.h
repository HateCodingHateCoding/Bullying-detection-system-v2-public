/**
 ******************************************************************************
 * @file    driver_config.h
 * @brief   drive config define.
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

#ifndef _DRIVER_CONFIG_H
#define _DRIVER_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */


 #ifdef __cplusplus
extern "C" {
#endif


#include "soc_config.h"


/* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */


#ifndef CONFIG_DRIVER_CLOCK
#define CONFIG_DRIVER_CLOCK                             0
#endif

#ifndef CONFIG_DRIVER_CLOCK_CALIB
#define CONFIG_DRIVER_CLOCK_CALIB                       0
#endif

#ifndef CONFIG_DRIVER_GPIO
#define CONFIG_DRIVER_GPIO                              0
#endif

#ifndef CONFIG_DRIVER_UART
#define CONFIG_DRIVER_UART                            0
#endif

#ifndef CONFIG_DRIVER_SPI
#define CONFIG_DRIVER_SPI                               0
#endif

#ifndef CONFIG_DRIVER_I2C
#define CONFIG_DRIVER_I2C                               0
#endif

#ifndef CONFIG_DRIVER_CAN
#define CONFIG_DRIVER_CAN                               0
#endif

#ifndef CONFIG_DRIVER_DMA
#define CONFIG_DRIVER_DMA                               0
#endif

#ifndef CONFIG_DRIVER_IPC
#define CONFIG_DRIVER_IPC                               0
#endif

#ifndef CONFIG_DRIVER_TIME
#define CONFIG_DRIVER_TIME                              0
#endif

#ifndef CONFIG_DRIVER_PWM
#define CONFIG_DRIVER_PWM                               0
#endif

#ifndef CONFIG_DRIVER_WKIO
#define CONFIG_DRIVER_WKIO                              0
#endif

#ifndef CONFIG_DRIVER_GPADC
#define CONFIG_DRIVER_GPADC                             0
#endif

#ifndef CONFIG_DRIVER_FLASH
#define CONFIG_DRIVER_FLASH                             0
#endif


/* Private macro.
 * ----------------------------------------------------------------------------
 */




/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

