/**
 ******************************************************************************
 * @file    soc.h
 * @brief   soc define.
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

#ifndef _SOC_H
#define _SOC_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "soc_config.h"

#if CONFIG_SOC_SERIES_RS613X
#include "rs613x.h"
#elif (CONFIG_SOC_SERIES_RS624X)
#include "rs624x.h"
#elif (CONFIG_SOC_SERIES_RS724X)
#include "rs724x.h"
#else
#error "Please select the SOC model in the board_config.h file!!!"
#endif


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

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */



int SOC_ArchInit(void);

#define HWINFO_ID_CLK_PWR					0
#define HWINFO_ID_FLASH_PSRAM				1
#define HWINFO_ID_CPUF_ADDR					3
#define HWINFO_ID_FLASH_SIZE				4
#define HWINFO_ID_CHIRP_CALIB				9 /* same HWINFO_ID_CHIRP_CALIB */

#define HWINFO_ID_NVM_CHIP_ID				(0)
#define HWINFO_ID_NVM_CHIP_VER				(1)
#define HWINFO_ID_NVM_CHIP_DATE				(2)
#define HWINFO_ID_NVM_MODULE_ID				(3)
#define HWINFO_ID_NVM_MODULE_DATE			(4)
#define HWINFO_ID_NVM_MODULE_NAME			(5)
#define HWINFO_ID_NVM_LSI					(6)
#define HWINFO_ID_NVM_MSI					(7)
#define HWINFO_ID_NVM_DPLL					(8)
//#define HWINFO_ID_NVM_MMW_SIGMADELTA		(9)
#define HWINFO_ID_NVM_ANT_CALIB_1T3R		(10)
#define HWINFO_ID_NVM_ANT_CALIB_2T4R		(11)

#define HWINFO_SIZE_NVM_CHIP_ID				(4)
#define HWINFO_SIZE_NVM_CHIP_VER			(8)
#define HWINFO_SIZE_NVM_CHIP_DATE			(8)
#define HWINFO_SIZE_NVM_MODULE_ID			(4)
#define HWINFO_SIZE_NVM_MODULE_DATE			(8)
#define HWINFO_SIZE_NVM_MODULE_NAME			(32)
#define HWINFO_SIZE_NVM_LSI					(4)
#define HWINFO_SIZE_NVM_MSI					(4)
#define HWINFO_SIZE_NVM_DPLL				(4)
//#define HWINFO_SIZE_NVM_MMW_SIGMADELTA		(8)
#define HWINFO_SIZE_NVM_MMW_CALIB			(20)

#ifdef __cplusplus
}
#endif

#endif /* _SOC_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
