/**
 ******************************************************************************
 * @file    hal_efuse.c
 * @brief   hal_efuse define.
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
#include "hal_efuse.h"
#include "hal_board.h"
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

 typedef struct {
    HAL_Dev_t           device;

    void *              efuseCtrl;
} Efuse_Device_t;

HAL_Dev_t * pEfuseDevice;

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_EFUSE_MSI_FREQ
#define CONFIG_EFUSE_MSI_FREQ                           32000000
#endif

/* Private variables.
 * ----------------------------------------------------------------------------
 */


/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

extern int EFUSE_HwInit(void * efuseDev, uint32_t clock);
extern uint32_t EFUSE_GetDeviceSize(void);
extern int EFUSE_EnableWrite(void * efuseDev);
extern int EFUSE_DisableWrite(void * efuseDev);
extern int EFUSE_GetChipId(void * efuseDev, uint8_t chipId[17], uint8_t len);
extern int EFUSE_GetLsiTrim(void * efuseDev, uint8_t lsiId, uint8_t value[1]);
extern int EFUSE_GetMsiTrim(void * efuseDev, uint32_t value[1]);
extern int EFUSE_GetPwrTrim(void * efuseDev, uint8_t ldoId, uint8_t value[1]);
extern int EFUSE_GetGpadcTrim(void * efuseDev, uint8_t voltId, uint16_t value[1]);
extern int EFUSE_GetDCXOFreqOffset(void * efuseDev, uint8_t value[1]);
extern int EFUSE_GetMAC(void * efuseDev, uint8_t MAC[6], uint8_t len);
extern int EFUSE_WriteUserArea(void * efuseDev, uint32_t bitStart, uint32_t bitLen, uint8_t *pValue);
extern int EFUSE_ReadUseArea(void * efuseDev, uint32_t bitStart, uint32_t bitLen, uint8_t *pValue);
extern int EFUSE_AddCorrect(void * efuseDev, uint32_t loc);
extern int EFUSE_SetMAC(void * efuseDev, uint8_t MAC[6], uint8_t len);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

void * HAL_EFUSE_get_ctrl_handle(void)
{
    return (pEfuseDevice + 1);
}

HAL_Dev_t * HAL_EFUSE_Init(void)
{
    HAL_Status_t status = HAL_STATUS_OK;

    HAL_Dev_t *pEfuseDeviceTmp = HAL_DEV_Find(HAL_DEV_TYPE_EFUSE, 0);

    if (pEfuseDeviceTmp == NULL) {
        pEfuseDeviceTmp = (HAL_Dev_t *)HAL_DEV_MemMalloc(EFUSE_GetDeviceSize() + sizeof(HAL_Dev_t));
        if (pEfuseDeviceTmp == NULL) {
            return NULL;
        }

        status = EFUSE_HwInit((pEfuseDeviceTmp + 1), CONFIG_EFUSE_MSI_FREQ);
        if (status == HAL_STATUS_OK) {
            HAL_DEV_Register(pEfuseDeviceTmp, HAL_DEV_TYPE_EFUSE, 0);
            pEfuseDevice = pEfuseDeviceTmp;
        } else {
            HAL_DEV_MemFree(pEfuseDeviceTmp);
            pEfuseDevice = NULL;
            HAL_ABORT();
        }
    }

    return pEfuseDeviceTmp;
}

HAL_Status_t HAL_EFUSE_Deinit(HAL_Dev_t * efuse)
{
    if (efuse == NULL) {
        return HAL_STATUS_OK;
    }

    HAL_DEV_Unregister(efuse);
    HAL_DEV_MemFree(efuse);

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_EFUSE_GetChipId(HAL_Dev_t * efuse, uint8_t chipId[17], uint8_t len)
{
    return (HAL_Status_t)EFUSE_GetChipId(efuse++, chipId, len);
}

HAL_Status_t HAL_EFUSE_GetLsiTrim(HAL_Dev_t * efuse, uint8_t lsiId, uint8_t value[1])
{
    return (HAL_Status_t)EFUSE_GetLsiTrim(efuse++, lsiId, value);
}

HAL_Status_t HAL_EFUSE_GetMsiTrim(HAL_Dev_t * efuse, uint32_t value[1])
{
    return (HAL_Status_t)EFUSE_GetMsiTrim(efuse++, value);
}

HAL_Status_t HAL_EFUSE_GetPwrTrim(HAL_Dev_t * efuse, uint8_t ldoId, uint8_t value[1])
{
    return (HAL_Status_t)EFUSE_GetPwrTrim(efuse++, ldoId, value);
}

HAL_Status_t HAL_EFUSE_GetGpadcTrim(HAL_Dev_t * efuse, uint8_t voltId, uint16_t value[1])
{
    return (HAL_Status_t)EFUSE_GetGpadcTrim(efuse++, voltId, value);
}

HAL_Status_t HAL_EFUSE_GetDCXOFreqOffset(HAL_Dev_t * efuse, uint8_t value[1])
{
    return (HAL_Status_t)EFUSE_GetDCXOFreqOffset(efuse++, value);
}

HAL_Status_t HAL_EFUSE_GetMAC(HAL_Dev_t * efuse, uint8_t MAC[6], uint8_t len)
{
    return (HAL_Status_t)EFUSE_GetMAC(efuse++, MAC, len);
}

HAL_Status_t HAL_EFUSE_EnableWrite(HAL_Dev_t * efuse)
{
    return (HAL_Status_t)EFUSE_EnableWrite(efuse++);
}

HAL_Status_t HAL_EFUSE_DisableWrite(HAL_Dev_t * efuse)
{
    return (HAL_Status_t)EFUSE_DisableWrite(efuse++);
}

HAL_Status_t HAL_EFUSE_WriteUserArea(HAL_Dev_t * efuse, uint32_t bitStart, uint32_t bitLen, uint8_t *pValue)
{
    return (HAL_Status_t)EFUSE_WriteUserArea(efuse++, bitStart, bitLen, pValue);
}

HAL_Status_t HAL_EFUSE_ReadUseArea(HAL_Dev_t * efuse, uint32_t bitStart, uint32_t bitLen, uint8_t *pValue)
{
    return (HAL_Status_t)EFUSE_ReadUseArea(efuse++, bitStart, bitLen, pValue);
}

HAL_Status_t HAL_EFUSE_AddCorrect(HAL_Dev_t * efuse, uint32_t loc)
{
    return (HAL_Status_t)EFUSE_AddCorrect(efuse++, loc);
}

HAL_Status_t HAL_EFUSE_SetMAC(HAL_Dev_t * efuse, uint8_t MAC[6], uint8_t len)
{
    return (HAL_Status_t)EFUSE_SetMAC(efuse++, MAC, len);
}





/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
