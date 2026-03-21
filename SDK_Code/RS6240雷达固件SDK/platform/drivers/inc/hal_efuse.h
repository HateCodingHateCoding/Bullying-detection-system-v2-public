/**
 ******************************************************************************
 * @file    hal_efuse.h
 * @brief   hal_efuse define.
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

#ifndef _HAL_EFUSE_H
#define _HAL_EFUSE_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "hal_types.h"
#include "hal_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */
/* Exported constants.
 * ----------------------------------------------------------------------------
 */

#define EFUSE_ADDR_MAX                          4096

#define EFUSE_CHIPID_SIZE                       132

#define EFUSE_LSI_250KHZ                        3
#define EFUSE_LSI_500KHZ                        2
#define EFUSE_LSI_1MHZ                          1
#define EFUSE_LSI_2MHZ                          0

#define EFUSE_PWR_BG                            0
#define EFUSE_PWR_EXT_LDO                       1
#define EFUSE_PWR_INT_LDO                       2
#define EFUSE_PWR_RTC_LDO                       3
#define EFUSE_PWR_SOC_LDO                       4
#define EFUSE_PWR_SPLL_LDO                      5

#define EFUSE_GPADC_VOLT_06                     0
#define EFUSE_GPADC_VOLT_24                     1

/* Exported macro.
 * ----------------------------------------------------------------------------
 */
/* Exported variables.
 * ----------------------------------------------------------------------------
 */
extern HAL_Dev_t * pEfuseDevice;

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
HAL_Dev_t * HAL_EFUSE_Init(void);
HAL_Status_t HAL_EFUSE_Deinit(HAL_Dev_t * efuse);

void * HAL_EFUSE_get_ctrl_handle(void);

HAL_Status_t HAL_EFUSE_GetChipId(HAL_Dev_t * efuse, uint8_t chipId[17], uint8_t len);
HAL_Status_t HAL_EFUSE_GetLsiTrim(HAL_Dev_t * efuse, uint8_t lsiId, uint8_t value[1]);
HAL_Status_t HAL_EFUSE_GetMsiTrim(HAL_Dev_t * efuse, uint32_t value[1]);
HAL_Status_t HAL_EFUSE_GetPwrTrim(HAL_Dev_t * efuse, uint8_t ldoId, uint8_t value[1]);
HAL_Status_t HAL_EFUSE_GetGpadcTrim(HAL_Dev_t * efuse, uint8_t voltId, uint16_t value[1]);
HAL_Status_t HAL_EFUSE_GetDCXOFreqOffset(HAL_Dev_t * efuse, uint8_t value[1]);

HAL_Status_t HAL_EFUSE_GetMAC(HAL_Dev_t * efuse, uint8_t MAC[6], uint8_t len);


HAL_Status_t HAL_EFUSE_EnableWrite(HAL_Dev_t * efuse);
HAL_Status_t HAL_EFUSE_DisableWrite(HAL_Dev_t * efuse);

HAL_Status_t HAL_EFUSE_WriteUserArea(HAL_Dev_t * efuse, uint32_t bitStart, uint32_t bitLen, uint8_t *pValue);
HAL_Status_t HAL_EFUSE_ReadUseArea(HAL_Dev_t * efuse, uint32_t bitStart, uint32_t bitLen, uint8_t *pValue);

HAL_Status_t HAL_EFUSE_AddCorrect(HAL_Dev_t * efuse, uint32_t loc);

HAL_Status_t HAL_EFUSE_SetMAC(HAL_Dev_t * efuse, uint8_t MAC[6], uint8_t len);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_EFUSE_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
