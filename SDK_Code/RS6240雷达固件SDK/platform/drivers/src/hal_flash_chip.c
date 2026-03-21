/**
 ******************************************************************************
 * @file    hal_flash_chip.c
 * @brief   hal flash chip define.
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
#include "hal_flash_jesd216.h"
#include "hal_flash.h"
#include "kvr.h"


#if CONFIG_FLASH_CHIP_INFO_LIST
#include "kvr.h"
#endif

#ifndef CONFIG_FLASH_LOG_LEVEL
#define CONFIG_FLASH_LOG_LEVEL          LEVEL_ERR
#endif

#define LOG_MODULE                      "FLASH"
#define LOG_LEVEL                       CONFIG_FLASH_LOG_LEVEL
#include "log.h"


/* Private config.
 * ----------------------------------------------------------------------------
 */

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

#if CONFIG_FLASH_CHIP_INFO_LIST
typedef struct {
    uint32_t magic;
    uint8_t vMajor;
    uint8_t vMinor;
    uint8_t itemSize;
    uint8_t itemCnt;
} Flash_ListHead_t;

typedef struct {
    uint32_t ivBuildNum;
    uint16_t ivRevision;
    uint8_t ivMinor;
    uint8_t ivMajor;
    uint32_t addr;
    uint16_t size;
    uint8_t id;
    uint8_t pad;
} Flash_ListInfo_t;

#endif


#ifndef CONFIG_FLASH_DEFAULT_FREQ
#define CONFIG_FLASH_DEFAULT_FREQ                       80000000
#endif

#ifndef CONFIG_FLASH_DEFAULT_SLEEP_ENABLE
#define CONFIG_FLASH_DEFAULT_SLEEP_ENABLE               1
#endif

#ifndef CONFIG_FLASH_DEFAULT_SLEEP_LATENCY
#define CONFIG_FLASH_DEFAULT_SLEEP_LATENCY              3
#endif

#ifndef CONFIG_FLASH_DEFAULT_RESUME_LATENCY
#define CONFIG_FLASH_DEFAULT_RESUME_LATENCY             20
#endif

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define FLASH_LIST_BASE_ADDR                            0x08000000
#define FLASH_LIST_HEAD_MAGIC                           0x6C706673

/* Private variables.
 * ----------------------------------------------------------------------------
 */
static FLASH_ChipInfo_t FixedflashChipInfo ={
    /* default config */
    .chipId             = {0, 0, 0},
    .flashSize          = 0,
    .spiFreqMax         = CONFIG_FLASH_DEFAULT_FREQ,
    .spiFormatMax       = FLASH_SPI_FORMAT_DUAL,
    .qerType            = 4,
    .sleepSupport       = CONFIG_FLASH_DEFAULT_SLEEP_ENABLE,
    .sleepLatency       = CONFIG_FLASH_DEFAULT_SLEEP_LATENCY,
    .resumeLatency      = CONFIG_FLASH_DEFAULT_RESUME_LATENCY,
};

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_FLASH_CHIP_INFO_LIST
static HAL_Status_t flash_Chip_GetListAddrAndSize(uint32_t *pAddr, uint32_t *pSize);
#endif

#if CONFIG_FLASH_CHIP_INFO_DETECH
extern HAL_Status_t flash_ReadSfdp(const HAL_Dev_t * flash, uint32_t addr, uint8_t *pdata, uint32_t size);
extern HAL_Status_t flash_ReadChipId(const HAL_Dev_t * flash, uint8_t *chipId);

static HAL_Status_t flash_Chip_GetInfoFromSfdp(HAL_Dev_t * flash, FLASH_ChipInfo_t * pChip);

#endif
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int32_t flash_GetChipListSize(HAL_Dev_t *flash, FLASH_ListType_t listType)
{
    HAL_Status_t status = HAL_STATUS_ERROR;
    uint32_t listSize = 0;

    do {
        if (flash == NULL) {
            status = HAL_STATUS_INVALID_PARAM;
            break;
        }

        if (listType == FLASH_LIST_TYPE_DEFAULT) {
            listSize = 1;
            status = HAL_STATUS_OK;
            break;
        }

#if CONFIG_FLASH_CHIP_INFO_LIST
    if (listType == FLASH_LIST_TYPE_LIST) {
        uint32_t addr = 0;
        status = flash_Chip_GetListAddrAndSize(&addr, &listSize);
        break;
    }
#endif

#if CONFIG_FLASH_CHIP_INFO_DETECH
    if (listType == FLASH_LIST_TYPE_DETECH) {
        FLASH_ChipInfo_t flashChipInfo;
        status = flash_Chip_GetInfoFromSfdp(flash, &flashChipInfo);
        if (status == HAL_STATUS_OK) {
            listSize = 1;
        }
        break;
    }
#endif
    } while (0);


    if (status == HAL_STATUS_OK) {
        return listSize;
    } else {
        return status;
    }
}

HAL_Status_t flash_GetChipListInfo(HAL_Dev_t *flash,
            FLASH_ListType_t listType,
            FLASH_ChipInfo_t * pChip,
            uint32_t chipSize)
{
    HAL_Status_t status = HAL_STATUS_ERROR;

    do {
        if ((flash == NULL) || (pChip == NULL)) {
            status = HAL_STATUS_INVALID_PARAM;
            break;
        }

        if ((listType == FLASH_LIST_TYPE_DEFAULT) && (chipSize == 1)) {
            memcpy(pChip, &FixedflashChipInfo, sizeof(FLASH_ChipInfo_t));
            status = HAL_STATUS_OK;
            break;
        }

#if CONFIG_FLASH_CHIP_INFO_LIST
    if (listType == FLASH_LIST_TYPE_LIST) {
        uint32_t startAddr = 0;
        uint32_t endAddr = 0;
        uint32_t listSize = 0;
        status = flash_Chip_GetListAddrAndSize(&startAddr, &listSize);
        if (status == HAL_STATUS_OK) {
            if (chipSize > listSize) {
                chipSize = listSize;
            }
            startAddr += FLASH_LIST_BASE_ADDR;
            endAddr = startAddr + chipSize * sizeof(FLASH_ChipInfo_t);
            FLASH_ChipInfo_t *scanItem = NULL;
            for (uint32_t addr = startAddr; addr < endAddr; addr += sizeof(FLASH_ChipInfo_t)) {
                scanItem = (FLASH_ChipInfo_t *)(addr);
                if ((scanItem == NULL) || (scanItem == (FLASH_ChipInfo_t *)(0xFFFFFFFF))) {
                    break;
                }
                memcpy(pChip, scanItem, sizeof(FLASH_ChipInfo_t));
                pChip++;
            }
        }
    }
#endif

#if CONFIG_FLASH_CHIP_INFO_DETECH
    if (listType == FLASH_LIST_TYPE_DETECH) {
        status = flash_Chip_GetInfoFromSfdp(flash, pChip);
        break;
    }
#endif
    } while (0);


    return status;
}


int32_t flash_FindMatchChip(HAL_Dev_t * flash,
            uint8_t chipId[FLASH_CHIP_ID_SIZE],
            uint8_t listType,
            FLASH_ChipInfo_t * pChip)
{
    HAL_Status_t status = HAL_STATUS_ERROR;

    if ((chipId == NULL) || (pChip == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    do {
#if CONFIG_FLASH_CHIP_INFO_LIST
        if (listType & FLASH_LIST_TYPE_LIST) {
            LOG_DBG("match chip : LIST");
            uint32_t startAddr = 0;
            uint32_t endAddr = 0;
            uint32_t listSize = 0;
            status = flash_Chip_GetListAddrAndSize(&startAddr, &listSize);
            if (status == HAL_STATUS_OK) {
                startAddr += FLASH_LIST_BASE_ADDR;
                endAddr = startAddr + listSize * sizeof(FLASH_ChipInfo_t);
                FLASH_ChipInfo_t *scanItem = NULL;
                status = HAL_STATUS_ERROR;
                for (uint32_t addr = startAddr; addr < endAddr; addr += sizeof(FLASH_ChipInfo_t)) {
                    scanItem = (FLASH_ChipInfo_t *)(addr);
                    if ((scanItem == NULL) || (scanItem == (FLASH_ChipInfo_t *)(0xFFFFFFFF))) {
                        break;
                    }
                    if (memcmp(scanItem->chipId, chipId, FLASH_CHIP_ID_SIZE) == 0) {
                        memcpy(pChip, scanItem, sizeof(FLASH_ChipInfo_t));
                        status = HAL_STATUS_OK;
                        break;
                    }
                }
            }

            if (status == HAL_STATUS_OK) {
                return FLASH_LIST_TYPE_LIST;
            }
        }
#endif

#if CONFIG_FLASH_CHIP_INFO_DETECH
        if (listType & FLASH_LIST_TYPE_DETECH) {
            LOG_DBG("match chip : DETECH");
            status = flash_Chip_GetInfoFromSfdp(flash, pChip);
            if (status == HAL_STATUS_OK) {
                return FLASH_LIST_TYPE_DETECH;
            }
        }
#endif

        if (listType & FLASH_LIST_TYPE_DEFAULT) {
            LOG_DBG("match chip : DEFAULT");
            memcpy(pChip, &FixedflashChipInfo, sizeof(FLASH_ChipInfo_t));
            return FLASH_LIST_TYPE_DEFAULT;
        }
    } while (0);


    return status;
}



#if CONFIG_FLASH_CHIP_INFO_LIST
static HAL_Status_t flash_Chip_GetListAddrAndSize(uint32_t *pAddr, uint32_t *pSize)
{
    int ret = 0;
    uint32_t flashListStoreAddr = 0;
    Flash_ListHead_t *pflashListHead = NULL;
    Flash_ListInfo_t flashListInfo;

    if ((pAddr == NULL) || (pSize == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LOG_DBG("load flash list info from KVR");
    ret = KVR_Load(KVR_SYS_ID_FLASH_LIST_IMG_INFO, &flashListInfo, sizeof(Flash_ListInfo_t));
    if (ret != sizeof(Flash_ListInfo_t)) {
        LOG_ERR("load kvr fail (%d)", ret);
        return HAL_STATUS_ERROR;
    }
    LOG_DBG("V %d.%d.%d+%08X\n",
            flashListInfo.ivMajor,
            flashListInfo.ivMinor,
            flashListInfo.ivRevision,
            flashListInfo.ivBuildNum);
    LOG_DBG("addr: %08X\nsize: %08X id: %d", flashListInfo.addr, flashListInfo.size, flashListInfo.id);

    flashListStoreAddr = flashListInfo.addr + FLASH_LIST_BASE_ADDR;

    /* Obtain information using the XIP read mode. */
    pflashListHead = (Flash_ListHead_t *)flashListStoreAddr;
    if ((pflashListHead->magic != FLASH_LIST_HEAD_MAGIC)
        || (pflashListHead->vMajor == 0)
        || (pflashListHead->itemSize != sizeof(FLASH_ChipInfo_t))
        || (pflashListHead->itemCnt == 0)) {
        
        LOG_ERR("magic %08X", pflashListHead->magic);
        LOG_ERR("vMajor %d", pflashListHead->vMajor);
        LOG_ERR("itemSize %d", pflashListHead->itemSize);
        LOG_ERR("itemCnt %d", pflashListHead->itemCnt);
        LOG_ERR("Flash list head err");
        return HAL_STATUS_INVALID_STATE;
    }

    /* get flash list store param */
    flashListStoreAddr += sizeof(Flash_ListHead_t);

    *pAddr = flashListStoreAddr - FLASH_LIST_BASE_ADDR;
    *pSize = pflashListHead->itemCnt;


    return HAL_STATUS_OK;
}
#endif /* CONFIG_FLASH_CHIP_INFO_LIST */


#if CONFIG_FLASH_CHIP_INFO_DETECH
static enum jesd216_mode_type flash_Opcode2Jesd216Mode(uint8_t opcode)
{
    enum jesd216_mode_type mode;

    switch (opcode) {
    case SPI_NOR_CMD_READ:
    case SPI_NOR_CMD_READ_FAST:
    case SPI_NOR_CMD_READ_FAST_4B:
    case SPI_NOR_CMD_PP:
        mode = JESD216_MODE_111;
        break;

    case SPI_NOR_CMD_DREAD:
    case SPI_NOR_CMD_DREAD_4B:
    case SPI_NOR_CMD_PP_1_1_2:
        mode = JESD216_MODE_112;
        break;
    case SPI_NOR_CMD_QREAD:
    case SPI_NOR_CMD_QREAD_4B:
    case SPI_NOR_CMD_PP_1_1_4:
        mode = JESD216_MODE_114;
        break;

    case SPI_NOR_CMD_2READ:
    case SPI_NOR_CMD_2READ_4B:
        mode = JESD216_MODE_122;
        break;

    case SPI_NOR_CMD_4READ:
    case SPI_NOR_CMD_4READ_4B:
    case SPI_NOR_CMD_PP_1_4_4:
        mode = JESD216_MODE_144;
        break;

    default:
        mode = JESD216_MODE_LIMIT;
        break;
    }

    return mode;
}

static HAL_Status_t flash_GetSfdpParam(const struct jesd216_param_header *php,
                            const struct jesd216_bfp *bfp, FLASH_ChipInfo_t * pChip)
{
    int rc = 0;

    pChip->flashSize = jesd216_bfp_density(bfp) / 8U;
    LOG_DBG("flash size [%dKB] [%dB]\n", pChip->flashSize >> 10, pChip->flashSize);


    struct jesd216_bfp_dw15 dw15;
    rc = jesd216_bfp_decode_dw15(php, bfp, &dw15);

    pChip->spiFormatMax = FLASH_SPI_FORMAT_STANDARD;
    if ((rc == 0) && (dw15.qer)) {
        pChip->qerType = dw15.qer;
        pChip->spiFormatMax = FLASH_SPI_FORMAT_DUAL;

        LOG_DBG("QE requirement mode[%X]\n", pChip->qerType);
    } else {
        pChip->qerType = 0;
        pChip->spiFormatMax = FLASH_SPI_FORMAT_QUAD;

        LOG_DBG("not found qe bit, chaneg quad spi to dual spi\n");
        LOG_ERR("get qe bit error\n");
    }

    pChip->spiFreqMax = CONFIG_FLASH_DEFAULT_FREQ;
    pChip->sleepSupport = CONFIG_FLASH_DEFAULT_SLEEP_ENABLE;
    pChip->sleepLatency = CONFIG_FLASH_DEFAULT_SLEEP_LATENCY;
    pChip->resumeLatency = CONFIG_FLASH_DEFAULT_RESUME_LATENCY;

    return HAL_STATUS_OK;
}


static HAL_Status_t flash_Chip_GetInfoFromSfdp(HAL_Dev_t * flash, FLASH_ChipInfo_t * pChip)
{
    HAL_Status_t status = -EINVAL;

    LOG_DBG("read jesd");
    /* read sfdp */
    const uint8_t decl_nph = 2;
    union {
        /* We only process BFP so use one parameter block */
        uint8_t raw[JESD216_SFDP_SIZE(decl_nph)];
        struct jesd216_sfdp_header sfdp;
    } u;
    const struct jesd216_sfdp_header *hp = &u.sfdp;

    status = flash_ReadSfdp(flash, 0, u.raw, sizeof(u.raw));
    if (status != HAL_STATUS_OK) {
        LOG_ERR("read jesd error[%d]", status);
        return HAL_STATUS_ERROR;
    }

    if (jesd216_sfdp_magic(&u.sfdp) != JESD216_SFDP_MAGIC) {
        LOG_ERR("magic error");
        return HAL_STATUS_ERROR;
    }

    LOG_DBG("read bfp\n");
    const struct jesd216_param_header *php = hp->phdr;
    const struct jesd216_param_header *phpe = php + MIN(decl_nph, 1 + hp->nph);
    status = HAL_STATUS_ERROR;
    while (php != phpe) {
        uint16_t id = jesd216_param_id(php);
        if (id == JESD216_SFDP_PARAM_ID_BFP) {
            union {
                /* uint32_t dw[MIN(php->len_dw, 20)]; */
                uint32_t dw[20];
                struct jesd216_bfp bfp;
            } u;

            status = flash_ReadSfdp(flash, jesd216_param_addr(php), (uint8_t *)u.dw, sizeof(u.dw));
            LOG_DBG("Dword info start");
            // for (int i = 0; i < sizeof(u.dw); i++) {
            //     LOG_DBG("dw[%02d]-0x%08x", i + 1, u.dw[i]);
            // }
            LOG_DBG("Dword info done\n");

            if (status != HAL_STATUS_OK) {
                LOG_ERR("read bfp error[%d]", status);
                return HAL_STATUS_ERROR;
            }
            status = flash_GetSfdpParam(php, &u.bfp, pChip);
            if (status == HAL_STATUS_OK) {
                uint8_t chipId[FLASH_CHIP_ID_SIZE];
                status = flash_ReadChipId(flash, chipId);
                if (status == HAL_STATUS_OK) {
                    memcpy(pChip->chipId, chipId, FLASH_CHIP_ID_SIZE);
                }
            }
            break;
        }

        ++php;
    }

    LOG_DBG("auto detect done[%d]\n\n", status);

    return status;
}

#endif /* CONFIG_FLASH_CHIP_INFO_DETECH */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

