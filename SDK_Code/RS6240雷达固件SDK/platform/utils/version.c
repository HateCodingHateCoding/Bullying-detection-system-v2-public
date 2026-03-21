/**
 ******************************************************************************
 * @file    version.c
 * @brief   version define.
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
#include "ll_utils.h"
#include "kvr.h"
#include "version.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

typedef struct {
    uint32_t iv_build_num;
    uint16_t iv_revision;
    uint8_t iv_minor;
    uint8_t iv_major;

    uint32_t addr;
    uint16_t size;

    uint8_t id;
    uint8_t pad;
} cfg_img_info_t;

typedef struct __packed {
    uint16_t ir_minor;
    uint8_t  ir_type;

    uint8_t  im_project;
    uint16_t im_number;
    uint16_t im_minor;

    uint8_t  iv_major;
    uint8_t  iv_minor;
    uint16_t iv_revision;

    uint32_t addr;
    uint32_t size;
}  merge_img_info_t;

typedef struct {
    uint8_t  iv_major;
    uint8_t  iv_minor;
    uint16_t iv_revision;
    uint32_t ih_boot_size;
    uint32_t ih_data_section_size;
    uint32_t ih_flash_size;
} boot_img_info_t;

/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int VER_GetSubimgVersion(VER_SubType_t verType, VER_SubVersion_t * pver)
{
    int status = HAL_STATUS_OK;
    uint8_t sysId;
    uint16_t verKvrId = 0;

    if ((pver == NULL) || (verType >= VER_TYPE_MAX)) {
        return HAL_STATUS_INVALID_STATE;
    }

    switch (verType) {
    case VER_TYPE_SYS:
        sysId = LL_SYS_GetCurrentSysId();
        if (sysId == 1) {
            verKvrId = KVR_SYS_ID_SYS1_IMG_INFO;
        } else {
            verKvrId = KVR_SYS_ID_SYS0_IMG_INFO;
        }
        break;

    case VER_TYPE_SYS0:
        verKvrId = KVR_SYS_ID_SYS0_IMG_INFO;
        break;

    case VER_TYPE_SYS1:
        verKvrId = KVR_SYS_ID_SYS1_IMG_INFO;
        break;

    case VER_TYPE_MMW:
        verKvrId = KVR_SYS_ID_MMW_IMG_INFO;
        break;

    case VER_TYPE_MMW_ALG:
        verKvrId = KVR_SYS_ID_MMW_ALG_IMG_INFO;
        break;

    case VER_TYPE_MPW:
        verKvrId = KVR_SYS_ID_MPW_IMG_INFO;
        break;

    case VER_TYPE_CUSTOM_CFG:
        verKvrId = KVR_SYS_ID_MMW_CUSTOM_CFG_IMG_INFO;
        break;

    case VER_TYPE_FLASH_LIST:
        verKvrId = KVR_SYS_ID_FLASH_LIST_IMG_INFO;
        break;

    default:
        break;
    }

    if (verKvrId != 0) {
        int ret = 0;
        cfg_img_info_t cfg_img_info;
        ret = KVR_Load(verKvrId, &cfg_img_info, sizeof(cfg_img_info_t));
        if (ret != sizeof(cfg_img_info_t)) {
            return HAL_STATUS_ERROR;
        }

        pver->iv_major = cfg_img_info.iv_major;
        pver->iv_minor = cfg_img_info.iv_minor;
        pver->iv_revision = cfg_img_info.iv_revision;
        pver->iv_build_num = cfg_img_info.iv_build_num;
    }

    return status;
}

int VER_GetSdkVersion(VER_SdkVersion_t * pver)
{
    if (pver == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    merge_img_info_t merge_img_info;
    int ret = 0;
    ret = KVR_Load(KVR_SYS_ID_MERGE_IMG_INFO, &merge_img_info, sizeof(merge_img_info_t));
    if (ret != sizeof(merge_img_info_t)) {
        return HAL_STATUS_ERROR;
    }

    pver->iv_major = merge_img_info.iv_major;
    pver->iv_minor = merge_img_info.iv_minor;
    pver->iv_revision = merge_img_info.iv_revision;

    pver->ir_minor = merge_img_info.ir_minor;
    pver->ir_type = merge_img_info.ir_type;

    pver->im_project = merge_img_info.im_project;
    pver->im_number = merge_img_info.im_number;
    pver->im_minor = merge_img_info.im_minor;


    return HAL_STATUS_OK;
}

int VER_GetBootVersion(VER_BootVersion_t * pver)
{
    if (pver == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    boot_img_info_t boot_img_info;
    int ret = 0;
    ret = KVR_Load(KVR_SYS_ID_BOOT_IMG_INFO, &boot_img_info, sizeof(boot_img_info_t));
    if (ret != sizeof(boot_img_info_t)) {
        return HAL_STATUS_ERROR;
    }

    pver->iv_major = boot_img_info.iv_major;
    pver->iv_minor = boot_img_info.iv_minor;
    pver->iv_revision = boot_img_info.iv_revision;

    return HAL_STATUS_OK;
}

void VER_GetBuildTime(char *pdate, char *ptime)
{
    if (pdate != NULL) {
        memcpy(pdate, __DATE__, sizeof(__DATE__));
    }

    if (ptime != NULL) {
        memcpy(ptime, __TIME__, sizeof(__TIME__));
    }
}

int VER_GetCustomCfgStoreInfo(VER_ImgInfo_t * pver)
{
    int ret = 0;
    cfg_img_info_t cfg_img_info;

    ret = KVR_Load(KVR_SYS_ID_MMW_CUSTOM_CFG_IMG_INFO, &cfg_img_info, sizeof(cfg_img_info_t));
    if (ret != sizeof(cfg_img_info_t)) {
        return HAL_STATUS_ERROR;
    }

    pver->addr = cfg_img_info.addr;
    pver->size = cfg_img_info.size;

    return HAL_STATUS_OK;
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

