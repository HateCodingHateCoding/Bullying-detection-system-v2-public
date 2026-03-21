/**
 ******************************************************************************
 * @file    cmd_config.h
 * @brief   cmd config define.
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

#ifndef _CMD_CONFIG_H
#define _CMD_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */


#include "shell_config.h"
#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif



/* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_SHELL_CMD_MEM
#define CONFIG_SHELL_CMD_MEM                                0
#endif

#ifndef CONFIG_SHELL_CMD_KERNEL
#define CONFIG_SHELL_CMD_KERNEL                             0
#endif

#ifndef CONFIG_SHELL_CMD_HEAP
#define CONFIG_SHELL_CMD_HEAP                               0
#endif

#ifndef CONFIG_SHELL_CMD_GPIO
#define CONFIG_SHELL_CMD_GPIO                               0
#endif

#ifndef CONFIG_SHELL_CMD_WKIO
#define CONFIG_SHELL_CMD_WKIO                               0
#endif

#ifndef CONFIG_SHELL_CMD_I2C
#define CONFIG_SHELL_CMD_I2C                                0
#endif

#ifndef CONFIG_SHELL_CMD_SPI
#define CONFIG_SHELL_CMD_SPI                                0
#endif

#ifndef CONFIG_SHELL_CMD_PM
#define CONFIG_SHELL_CMD_PM                                0
#endif

#ifndef CONFIG_SHELL_CMD_DMA
#define CONFIG_SHELL_CMD_DMA                                0
#endif

#ifndef CONFIG_SHELL_CMD_WDG
#define CONFIG_SHELL_CMD_WDG                                0
#endif

#ifndef CONFIG_SHELL_CMD_KVF
#define CONFIG_SHELL_CMD_KVF                                0
#endif

#ifndef CONFIG_SHELL_CMD_CLK
#define CONFIG_SHELL_CMD_CLK                                0
#endif

#ifndef CONFIG_SHELL_CMD_EFUSE
#define CONFIG_SHELL_CMD_EFUSE                              0
#endif

#ifndef CONFIG_SHELL_CMD_FLASH
#define CONFIG_SHELL_CMD_FLASH                              0
#endif

#ifndef CONFIG_SHELL_CMD_VERSION
#define CONFIG_SHELL_CMD_VERSION                            0
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

#endif /* _CMD_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
