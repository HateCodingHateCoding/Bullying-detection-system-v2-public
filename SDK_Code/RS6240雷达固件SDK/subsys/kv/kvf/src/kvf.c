/**
 ******************************************************************************
 * @file    hal_kvf.c
 * @brief   hal key value flash define.
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
#include "hal_flash.h"
#include "nvs.h"

#define LOG_MODULE                      "KVF"
#define LOG_LEVEL                       LEVEL_ERR
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_KVF_BLOCK_CNT
#define CONFIG_KVF_BLOCK_CNT                            2
#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */

static struct nvs_fs nvsFs;

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int KVF_Init(void)
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        HAL_Dev_t * pFlashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
        if (pFlashDev == NULL) {
            status = HAL_STATUS_ERROR;
            LOG_ERR("flash not init");
            break;
        }

        uint32_t flashSize = 0;
        uint32_t blockSize = 0;

        status = HAL_FLASH_ExtControl(pFlashDev, FLASH_GET_FLASH_SIZE, &flashSize, 4);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("get flash size fail (%d)", status);
            break;
        }

        status = HAL_FLASH_ExtControl(pFlashDev, FLASH_GET_BLOCK_SIZE, &blockSize, 4);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("get block size fail (%d)", status);
            break;
        }

        LOG_DBG("flash size: 0x%08X (%dKB)", flashSize, flashSize / 1024);
        LOG_DBG("flash block size 0x%08X (%dKB)", blockSize, blockSize / 1024);

        nvsFs.flash_device = pFlashDev;
        nvsFs.sector_size = blockSize;
        nvsFs.sector_count = CONFIG_KVF_BLOCK_CNT;
        nvsFs.offset = flashSize - (blockSize * CONFIG_KVF_BLOCK_CNT);
        nvsFs.write_block_size = 1;
        nvsFs.erase_value = 0xFF;
        nvsFs.ready = false;
        nvsFs.ate_wra = 0;
        nvsFs.data_wra = 0;

        status = nvs_mount(&nvsFs);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("nvs_mount FAIL %d", status);
        }
    } while (0);

    return status;
}

int KVF_Save(uint16_t id, void *pdata, uint32_t size)
{
    int ret = 0;
    ret = nvs_write(&nvsFs, id, pdata, size);
    if (ret == 0) {
        ret = size;
    }

    return ret;
}

int KVF_Load(uint16_t id, void *pdata, uint16_t size)
{
    return nvs_read(&nvsFs, id, pdata, size);
}

int KVF_Delete(uint16_t id)
{
    return nvs_delete(&nvsFs, id);
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

