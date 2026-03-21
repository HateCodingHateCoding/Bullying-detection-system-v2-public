/**
 ******************************************************************************
 * @file    hal_flash.c
 * @brief   hal flash define.
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
#include "hal_board.h"

#include "ll_rcc_dev.h"
#include "ll_gpio.h"
#include "ll_fsmc.h"
#include "ll_splk.h"
#include "ll_utils.h"

#include "hal_flash.h"
#include "hal_flash_jesd216.h"

#include "hal_clock.h"


#ifndef CONFIG_FLASH_LOG_LEVEL
#define CONFIG_FLASH_LOG_LEVEL          LEVEL_ERR
#endif

#define LOG_MODULE                      "FLASH"
#define LOG_LEVEL                       CONFIG_FLASH_LOG_LEVEL
#include "log.h"

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define CONFIG_FLASH_PM                                         CONFIG_PM

#ifndef CONFIG_FLASH_PM_EARLY
#define CONFIG_FLASH_PM_EARLY                                   1
#endif

/*
 * preinit
 */
#ifndef CONFIG_XIP_PREINIT_CLK_SRC
#define CONFIG_XIP_PREINIT_CLK_SRC                              1
#endif

#ifndef CONFIG_XIP_PREINIT_CLK_PRESCALER
#define CONFIG_XIP_PREINIT_CLK_PRESCALER                        0
#endif

#ifndef CONFIG_XIP_PREINIT_INTER_CLK_DIV
#define CONFIG_XIP_PREINIT_INTER_CLK_DIV                        2
#endif

#ifndef CONFIG_XIP_PREINIT_SPI_FORMAT
#define CONFIG_XIP_PREINIT_SPI_FORMAT                           1
#endif

#ifndef CONFIG_XIP_PREINIT_READ_DUMMY
#define CONFIG_XIP_PREINIT_READ_DUMMY                           8
#endif

#ifndef CONFIG_PSRAM_PREINIT_WRITE_OPCODE
#define CONFIG_PSRAM_PREINIT_WRITE_OPCODE                       0x38
#endif

#ifndef CONFIG_PSRAM_PREINIT_READ_OPCODE
#define CONFIG_PSRAM_PREINIT_READ_OPCODE                        0xEB
#endif

#ifndef CONFIG_PSRAM_PREINIT_READ_DUMMY
#define CONFIG_PSRAM_PREINIT_READ_DUMMY                         6
#endif

/*
 * multi core
 */
#ifndef CONFIG_FLASH_MULTI_CORE_ENABLE
#define CONFIG_FLASH_MULTI_CORE_ENABLE                           0
#endif


#ifndef CONFIG_FLASH_LISTS_SIZE
#define CONFIG_FLASH_LISTS_SIZE                                 3
#endif

/*
 * feature select
 */

#ifndef CONFIG_FLASH_ENC_EN
/* not support */
#define CONFIG_FLASH_ENC_EN                                     0
#endif

#ifndef CONFIG_FLASH_PRAM_AUTO_DETECT
#define CONFIG_FLASH_PRAM_AUTO_DETECT                           0
#endif

#ifndef CONFIG_FLASH_DBG_LOG
#define CONFIG_FLASH_DBG_LOG                                    0
#endif

#ifndef CONFIG_PSRAM_PREINIT_EN
#define CONFIG_PSRAM_PREINIT_EN                                 0
#endif

#ifndef CONFIG_MERGE_IMG_ENCRYPT_EN
#define CONFIG_MERGE_IMG_ENCRYPT_EN                             0
#endif

#ifndef CONFIG_API_READ_ID_EN
#define CONFIG_API_READ_ID_EN                                   0
#endif

#ifndef CONFIG_API_READ_SFDP_EN
#define CONFIG_API_READ_SFDP_EN                                 0
#endif

#ifndef CONFIG_API_QUARD_MODE_EN
#define CONFIG_API_QUARD_MODE_EN                                0
#endif

#ifndef CONFIG_API_WRITE_PROTECT_EN
#define CONFIG_API_WRITE_PROTECT_EN                             0
#endif

#ifndef CONFIG_API_PM_EN
#define CONFIG_API_PM_EN                                        0
#endif

#ifndef CONFIG_API_CLOCK_IO_DEINIT_EN
#define CONFIG_API_CLOCK_IO_DEINIT_EN                           0
#endif


#ifndef CONFIG_PSRAM_DCACHE_FLUSH_LATENCY
#define CONFIG_PSRAM_DCACHE_FLUSH_LATENCY                       1000
#endif

#ifndef CONFIG_FLASH_REMAP_SIZE
#define CONFIG_FLASH_REMAP_SIZE                                 10
#endif


#ifndef CONFIG_FLASH_SIZE_AUTO_DETECH
#define CONFIG_FLASH_SIZE_AUTO_DETECH                           1
#endif

#define CONFIG_FLASH_MIN_SIZE                                   0x0040000   /* 256KB */
#define CONFIG_FLASH_MAX_SIZE                                   0x1000000   /* 16MB  */

#define CONFIG_BOOT_HEAD_SIZE                                   64

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

typedef struct {
    #define FLASH_STATE_UNINIT              0
    #define FLASH_STATE_INIT                1
    #define FLASH_STATE_ACTIVE              2
    #define FLASH_STATE_SLEEP               4
    // uint8_t     state;
    uint8_t     supportListMode;
    uint8_t     usedListMode;
    uint8_t     clockSrc;
    uint8_t     spiFormat;
    uint8_t     psramEnable;
    uint8_t     flashId[FLASH_CHIP_ID_SIZE];
    uint8_t     sampleDelay;
    uint8_t     qerType;
    uint8_t     readDummy;
    uint8_t     sleepEnable;
    uint8_t     sleepLatency;
    uint8_t     resumeLatency;
    uint32_t    clockFreq;
    uint32_t    clockPsc;
    uint32_t    flashSize;
    uint32_t    flashPageSize;
    uint32_t    flashBlockSize;
    uint32_t    psramSize;

    uint32_t    remapAddrPre[CONFIG_FLASH_REMAP_SIZE];
    uint32_t    remapAddrLst[CONFIG_FLASH_REMAP_SIZE];
    uint32_t    remapLen[CONFIG_FLASH_REMAP_SIZE];

    uint32_t    psramFlushLatency;
    uint8_t     psramRemapCnt;
    uint8_t     remapEnable;

} FLASH_Info_t;


typedef struct {
    HAL_Dev_t           device;
    FLASH_Info_t        info;
    uint8_t             autoSleep;
    uint8_t             sysId;
} Flash_Device_t;


/* Private macros.
 * ----------------------------------------------------------------------------
 */

#define FLASH_CMD_READ                                          0x03
#define FLASH_CMD_WRITE                                         0x02
#define FLASH_CMD_ERASE                                         0x20
#define FLASH_CMD_DREAD                                         0x3B
#define FLASH_CMD_QREAD                                         0x6B

#define FLASH_CMD_WREN                                          0x06
#define FLASH_CMD_RDSR                                          0x05
#define FLASH_CMD_RDSR2                                         0x35
#define FLASH_CMD_RDSR3                                         0x15
#define FLASH_CMD_WRSR                                          0x01
#define FLASH_CMD_WRSR2                                         0x31
#define FLASH_CMD_WRSR3                                         0x11


#define FLASH_CMD_SLEEP                                         0xB9
#define FLASH_CMD_RESUME                                        0xAB

#define FLASH_CMD_VSR                                           0x50
#define FLASH_CMD_RDSFDP                                        0x5A

#define FLASH_CMD_RDID                                          0x9F

#define PSRAM_CMD_READ                                          0xEB
#define PSRAM_CMD_WRITE                                         0x38
#define PSRAM_CMD_DUMMY                                         6


#define FSMC_SR_WIP_BIT                                         (0x1)
#define FSMC_SR_WEL_BIT                                         (0x2)

#define FSMC_PAGE_SIZE                                          256
#define FSMC_BLOCK_SIZE                                         4096

#define FSMC_ERASE_SIZE                                         0x1000
#define FSMC_ERASE_ADDR_MSK                                     0xFFF
#define FSMC_ERASE_SIZE_MSK                                     0xFFF

#define FSMC_WRITE_SIZE                                         0x100
#define FSMC_WRITE_ADDR_MSK                                     0xFF
#define FSMC_WRITE_SIZE_MSK                                     0xFF


#define FSMC_DATA_MODE_RX                                       0
#define FSMC_DATA_MODE_TX                                       1
#define FSMC_DATA_MODE_NULL                                     3

#define FSMC_SR_MAX                                             3


#define FLASH_CMD_TO                                            0xFFFF


#define FLASH_PRESCALER_MIN                                     2
#define FLASH_PRESCALER_MAX                                     200
#define FLASH_PRESCALER_STEP                                    2


#define FLASH_DELAY_2_THRESHOLD_FREQ                            200000000
#define FLASH_DELAY_1_THRESHOLD_FREQ                            64000000
#define FLASH_DELAY_0_THRESHOLD_FREQ                            16000000

#define FLASH_DUMMY_DEFAULT                                     8

#define FLASH_MULTI_CORE_GET                                    0
#define FLASH_MULTI_CORE_SET                                    0

#define REMAP_ADDR_AREA_MASK                                    0x07000000
#define REMAP_ADDR_TYPE_PSRAM                                   0x05000000


static uint32_t flashLockKey = 0;

#define flashLock()         do {                                            \
                                flashLockKey = __lock_irq();                \
                                LL_SPLK_RawLock(SPLK_ID_FLASH_PSRAM, 1)

#define flashUnlock()           LL_SPLK_RawUnlock(SPLK_ID_FLASH_PSRAM);     \
                                __unlock_irq(flashLockKey);                 \
                            } while (0)

/* Private variables.
 * ----------------------------------------------------------------------------
 */

static uint8_t flash_preinit_state;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

static HAL_Status_t flash_MultiCoreInfoSync(uint8_t ctrl, FLASH_Info_t * flashInfo);


static void flash_IoAndClkInit(uint8_t clk_src, uint8_t clk_div, uint8_t psram);
static void flash_IoAndClkDeInit(uint8_t psram);
static HAL_Status_t flash_Init(uint16_t prescaler,
                        uint8_t sample_delay,
                        uint8_t xip_spi_format,
                        uint8_t xip_dummy,
                        uint8_t psram_write_inst,
                        uint8_t psram_read_inst,
                        uint8_t psram_dummy);
static HAL_Status_t flash_Transfer(const Flash_Device_t *pDevice,
                                const uint8_t *pInstBuff,
                                uint32_t InstSize,
                                uint8_t *pDataBuff,
                                uint32_t DataSize,
                                uint8_t Mode);
static HAL_Status_t flash_Cmd(const Flash_Device_t *pDevice,
                    uint8_t Opcode,
                    uint8_t *pData,
                    uint32_t Size,
                    uint8_t Mode);
static HAL_Status_t flash_Data(const Flash_Device_t *pDevice,
                        uint8_t Opcode,
                        uint32_t Addr,
                        uint8_t *pData,
                        uint32_t Size,
                        uint8_t Mode);
static HAL_Status_t flash_CmdWriteEnable(const Flash_Device_t *pDevice);
static HAL_Status_t flash_CmdWaitDone(const Flash_Device_t *pDevice);

static HAL_Status_t flash_CmdSleep(const Flash_Device_t *pDevice);
static HAL_Status_t flash_CmdResume(const Flash_Device_t *pDevice);
// static HAL_Status_t flash_SetWriteProtect(const Flash_Device_t *pDevice, uint8_t wp, uint8_t wp_msk);
// static HAL_Status_t flash_GetWriteProtect(const Flash_Device_t *pDevice, uint8_t *wp, uint8_t wp_msk);
static HAL_Status_t flash_ReadId(const Flash_Device_t *pDevice, uint8_t *pid);
static HAL_Status_t flash_ConfigQuardMode(const Flash_Device_t *pDevice,
                    uint8_t reg_idx, uint8_t bit_idx, bool seq_read);
static HAL_Status_t flash_EnableQuardMode(const Flash_Device_t *pDevice, uint8_t qerType);
static uint8_t flash_StatusReg2Opcpde(uint8_t reg_idx);
static HAL_Status_t flash_HwInit(HAL_Dev_t *flash);
extern int32_t flash_GetChipListSize(HAL_Dev_t *flash,
            FLASH_ListType_t listType);
extern HAL_Status_t flash_GetChipListInfo(HAL_Dev_t *flash,
            FLASH_ListType_t listType,
            FLASH_ChipInfo_t * pChip,
            uint32_t chipSize);
extern int32_t flash_FindMatchChip(HAL_Dev_t * flash,
            uint8_t chipId[FLASH_CHIP_ID_SIZE],
            uint8_t listType,
            FLASH_ChipInfo_t * pChip);

#if CONFIG_FLASH_PM
static int flash_pm_action(void *dev, enum pm_device_action action);
#endif

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t * HAL_FLASH_Init(FLASH_ID_t id)
{
    Flash_Device_t *pFlashDevice = (Flash_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_FLASH, id);

    if (pFlashDevice == NULL) {
        pFlashDevice = (Flash_Device_t *)HAL_DEV_MemMalloc(sizeof(Flash_Device_t));
        if (pFlashDevice == NULL) {
            return NULL;
        }

        pFlashDevice->device.reg            = FSMC_DEV;
        pFlashDevice->autoSleep             = 1;
        pFlashDevice->sysId                 = csi_get_cpu_id();
        HAL_DEV_SetName((HAL_Dev_t *)pFlashDevice, "flash");

        HAL_DEV_SetState((HAL_Dev_t *)pFlashDevice, HAL_DEV_STATE_INIT);
#if CONFIG_FLASH_PM
#if CONFIG_FLASH_PM_EARLY
        pm_device_mult_register(&(pFlashDevice->device.pmDev), NULL, flash_pm_action);
#else
        pm_device_mult_register(&(pFlashDevice->device.pmDev), flash_pm_action, NULL);
    #endif
#endif
        HAL_DEV_Register((HAL_Dev_t *)pFlashDevice, HAL_DEV_TYPE_FLASH, id);
    }


    return (HAL_Dev_t *)pFlashDevice;
}


HAL_Status_t HAL_FLASH_DeInit(HAL_Dev_t * flash)
{
    HAL_Status_t status = HAL_STATUS_OK;

    Flash_Device_t *pFlashDevice = (Flash_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);

    if (pFlashDevice != NULL) {
        HAL_DevState_t state = HAL_DEV_GetState(flash);
        if (HAL_DEV_GetState(flash) != HAL_DEV_STATE_UNINIT) {
            FLASH_Info_t * pFlashInfo = &(pFlashDevice->info);
            if (state != HAL_DEV_STATE_CLOSE) {
                flash_IoAndClkDeInit(pFlashInfo->psramEnable);
            }
#if CONFIG_FLASH_PM
            pm_device_unregister(&(pFlashDevice->device.pmDev));
#endif
            status = HAL_DEV_Unregister((HAL_Dev_t *)pFlashDevice);
            if (status == HAL_STATUS_OK) {
                HAL_DEV_MemFree(pFlashDevice);
            }
        }
    }

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_FLASH_Open(HAL_Dev_t * flash, FLASH_InitParam_t * pInitParam)
{
    HAL_Status_t status = HAL_STATUS_OK;

    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;
    if (pFlashDevice == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(flash);
    if ((state == HAL_DEV_STATE_INIT) || (state == HAL_DEV_STATE_CLOSE)) {
#if CONFIG_FLASH_MULTI_CORE_ENABLE
        /* get info from kvr */
        
#else
        pFlashDevice->info.clockFreq        = pInitParam->clockFreq;
        pFlashDevice->info.clockSrc         = (uint8_t)pInitParam->clockSrc;
        pFlashDevice->info.spiFormat        = (uint8_t)pInitParam->spiFormat;
        pFlashDevice->info.psramEnable      = (uint8_t)pInitParam->psramEnable;
        pFlashDevice->info.flashSize        = 0;
        pFlashDevice->info.psramSize        = 0;
        pFlashDevice->info.flashPageSize    = FSMC_PAGE_SIZE;
        pFlashDevice->info.flashBlockSize   = FSMC_BLOCK_SIZE;
#endif

        flashLock();
        status = flash_HwInit((HAL_Dev_t *)pFlashDevice);
        flashUnlock();

        if (status == HAL_STATUS_OK) {
            HAL_DEV_SetState(flash, HAL_DEV_STATE_ACTIVE);
        }
    }
//    else {
//        status = HAL_STATUS_INVALID_STATE;
//    }

    return status;
}


HAL_Status_t HAL_FLASH_Close(HAL_Dev_t * flash)
{
    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    if (pFlashDevice != NULL) {
        HAL_DevState_t state = HAL_DEV_GetState(flash);
        if ((state != HAL_DEV_STATE_CLOSE) || (state != HAL_DEV_STATE_UNINIT)) {
            FLASH_Info_t * pFlashInfo = &(pFlashDevice->info);
            flash_IoAndClkDeInit(pFlashInfo->psramEnable);
            HAL_DEV_SetState(flash, HAL_DEV_STATE_CLOSE);
        }
    }

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_FLASH_Read(const HAL_Dev_t *flash, uint32_t addr, uint8_t *pdata, uint32_t size)
{
    HAL_Status_t status = HAL_STATUS_OK;
    if ((flash == NULL) || (pdata == NULL)) {
        LOG_ERR("read flash param invalid");
        return HAL_STATUS_INVALID_PARAM;
    }

    if (size == 0) {
        LOG_ERR("read flash size 0");
        return HAL_STATUS_OK;
    }

    if (HAL_DEV_GetState(flash) != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    flashLock();
    status = flash_Data(pFlashDevice, FLASH_CMD_READ, addr, pdata, size, FSMC_DATA_MODE_RX);
    flashUnlock();

    return status;
}

HAL_Status_t HAL_FLASH_Write(const HAL_Dev_t *flash, uint32_t addr, const uint8_t *pdata, uint32_t size)
{
    HAL_Status_t status = HAL_STATUS_OK;
    uint32_t write_size = 0;

    if ((flash == NULL) || (pdata == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (size == 0) {
        return HAL_STATUS_OK;
    }

    if (HAL_DEV_GetState(flash) != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    flashLock();

    for (uint32_t scan_size = 0; scan_size < size; scan_size += write_size) {
        status = flash_CmdWriteEnable(pFlashDevice);
        if (status != 0) {
            break;
        }

        write_size = FSMC_WRITE_SIZE - (addr & FSMC_WRITE_ADDR_MSK);
        if (write_size > (size - scan_size)) {
            write_size = (size - scan_size);
        }

        status = flash_Data(pFlashDevice, FLASH_CMD_WRITE, addr, (uint8_t *)pdata, write_size, FSMC_DATA_MODE_TX);
        if (status != 0) {
            break;
        }

        status = flash_CmdWaitDone(pFlashDevice);
        if (status != 0) {
            break;
        }

        pdata += write_size;
        addr += write_size;
    }

    flashUnlock();


    if (status != HAL_STATUS_OK) {
        status = HAL_STATUS_ERROR;
    }

    return status;
}

HAL_Status_t HAL_FLASH_Erase(const HAL_Dev_t *flash, uint32_t addr, uint32_t size)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if (((addr & FSMC_ERASE_ADDR_MSK) != 0)
        || ((addr & FSMC_ERASE_SIZE_MSK) != 0)
        || (flash == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (size == 0) {
        return HAL_STATUS_OK;
    }

    if (HAL_DEV_GetState(flash) != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    flashLock();

    for (uint32_t scan_size = 0; scan_size < size; scan_size += FSMC_ERASE_SIZE) {
        status = flash_CmdWriteEnable(pFlashDevice);
        if (status != 0) {
            break;
        }

        status = flash_Data(pFlashDevice, FLASH_CMD_ERASE, addr, NULL, 0, FSMC_DATA_MODE_NULL);
        if (status != 0) {
            break;
        }

        status = flash_CmdWaitDone(pFlashDevice);
        if (status != 0) {
            break;
        }

        addr += FSMC_ERASE_SIZE;
    }

    flashUnlock();

    if (status != HAL_STATUS_OK) {
        status = HAL_STATUS_ERROR;
    }

    return status;
}


HAL_Status_t HAL_FLASH_ExtControl(HAL_Dev_t *flash, FLASH_Attr_t attr, void * arg, uint32_t argSize)
{
    HAL_Status_t status = HAL_STATUS_INVALID_PARAM;
    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    if ((flash == NULL) || (arg == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    flashLock();

    switch (attr) {
    case FLASH_GET_FLASH_SIZE:
        if (argSize == 4) {
            uint32_t *flashSize = (uint32_t *)arg;
            *flashSize = pFlashDevice->info.flashSize;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_GET_PAGE_SIZE:
        if (argSize == 4) {
            uint32_t *pageSize = (uint32_t *)arg;
            *pageSize = pFlashDevice->info.flashPageSize;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_GET_BLOCK_SIZE:
        if (argSize == 4) {
            uint32_t *blockSize = (uint32_t *)arg;
            *blockSize = pFlashDevice->info.flashBlockSize;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_GET_PSRAM_SIZE:
        if (argSize == 4) {
            uint32_t *psramSize = (uint32_t *)arg;
            *psramSize = pFlashDevice->info.psramSize;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_GET_CHIP_ID:
        if (argSize == 3) {
            uint8_t *pchipId = (uint8_t *)arg;
            status = flash_ReadId((Flash_Device_t *)flash, pchipId);
        }
        break;

    case FLASH_EXT_GET_CLK_SRC:
        if (argSize >= 1) {
            uint8_t *clockSrc = (uint8_t *)arg;
            *clockSrc = pFlashDevice->info.clockSrc;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_CLK_DIV:
        if (argSize >= 4) {
            uint32_t *clockPsc = (uint32_t *)arg;
            *clockPsc = pFlashDevice->info.clockPsc;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_CLK_FREQ:
        if (argSize == 4) {
            uint32_t *clockFreq = (uint32_t *)arg;
            *clockFreq = pFlashDevice->info.clockFreq;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_SPI_FORMAT:
        if (argSize >= 1) {
            uint8_t *spiFormat = (uint8_t *)arg;
            *spiFormat = pFlashDevice->info.spiFormat;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_SLEEP_STATE:
        if (argSize >= 1) {
            uint8_t *sleepEnable = (uint8_t *)arg;
            *sleepEnable = pFlashDevice->info.sleepEnable;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_SET_SLEEP_STATE:
        if (argSize >= 1) {
            uint8_t *sleepEnable = (uint8_t *)arg;
            pFlashDevice->info.sleepEnable = *sleepEnable;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_SLEEP_LATENCY:
        if (argSize >= 1) {
            uint8_t *sleepLatency = (uint8_t *)arg;
            *sleepLatency = pFlashDevice->info.sleepLatency;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_SET_SLEEP_LATENCY:
        if (argSize >= 1) {
            uint8_t *sleepLatency = (uint8_t *)arg;
            pFlashDevice->info.sleepLatency = *sleepLatency;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_RESUME_LATENCY:
        if (argSize >= 1) {
            uint8_t *resumeLatency = (uint8_t *)arg;
            *resumeLatency = pFlashDevice->info.resumeLatency;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_SET_RESUME_LATENCY:
        if (argSize >= 1) {
            uint8_t *resumeLatency = (uint8_t *)arg;
            pFlashDevice->info.resumeLatency = *resumeLatency;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_AUTO_SLEEP_STATE:
        if (argSize >= 1) {
            uint8_t *autoSleep = (uint8_t *)arg;
            *autoSleep = pFlashDevice->autoSleep;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_SET_AUTO_SLEEP_STATE:
        if (argSize >= 1) {
            uint8_t *autoSleep = (uint8_t *)arg;
            pFlashDevice->autoSleep = *autoSleep;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_LIST_SIZE:
        if (argSize == 4) {
            int32_t *listSize = (int32_t *)arg;
            *listSize = flash_GetChipListSize(flash, FLASH_LIST_TYPE_LIST);
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_CHIP_INFO_DEFAULT:
    {
        uint32_t infosize = sizeof(FLASH_ChipInfo_t);
        if (argSize >= infosize) {
            FLASH_ChipInfo_t *chipInfo = (FLASH_ChipInfo_t *)arg;
            status = flash_GetChipListInfo(flash, FLASH_LIST_TYPE_DEFAULT, chipInfo, 1);
        }
        break;
    }

    case FLASH_EXT_GET_CHIP_INFO_LIST:
    {
        uint32_t listSize = flash_GetChipListSize(flash, FLASH_LIST_TYPE_LIST);
        if (listSize > argSize) {
            listSize = argSize;
        }
        FLASH_ChipInfo_t *chipInfo = (FLASH_ChipInfo_t *)arg;
        status = flash_GetChipListInfo(flash, FLASH_LIST_TYPE_LIST, chipInfo, listSize);
        break;
    }

    case FLASH_EXT_GET_CHIP_INFO_DETECH:
    {
        uint32_t infosize = sizeof(FLASH_ChipInfo_t);
        if (argSize >= infosize) {
            FLASH_ChipInfo_t *chipInfo = (FLASH_ChipInfo_t *)arg;
            status = flash_GetChipListInfo(flash, FLASH_LIST_TYPE_DETECH, chipInfo, 1);
        }
        break;
    }

    case FLASH_EXT_GET_CHIP_INFO_MODE:
        if (argSize >= sizeof(uint8_t)) {
            uint8_t *listMode = (uint8_t *)arg;
            *listMode = pFlashDevice->info.supportListMode;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_SET_CHIP_INFO_MODE:
        if (argSize >= sizeof(uint8_t)) {
            pFlashDevice->info.supportListMode = *(uint8_t *)arg & 0x07;
            status = HAL_STATUS_OK;
        }
        break;

    case FLASH_EXT_GET_USED_CHIP_INFO_MODE:
        if (argSize >= sizeof(uint8_t)) {
            uint8_t *usedListMode = (uint8_t *)arg;
            *usedListMode = pFlashDevice->info.usedListMode;
            LOG_DBG("used %d %d", pFlashDevice->info.usedListMode, *usedListMode);
            status = HAL_STATUS_OK;
        }
        break;

    default:
        status = HAL_STATUS_INVALID_PARAM;
    }

    flashUnlock();

    return status;
}


HAL_Status_t HAL_FLASH_Suspend(HAL_Dev_t * flash)
{
    HAL_Status_t status = HAL_STATUS_OK;
    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    if (flash == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    flashLock();

    do {
        FLASH_Info_t * pFlashInfo = &(pFlashDevice->info);

        status = flash_MultiCoreInfoSync(FLASH_MULTI_CORE_GET, pFlashInfo);
        if (status != HAL_STATUS_OK) {
            status = HAL_STATUS_INVALID_STATE;
            break;
        }

        HAL_DevState_t state = HAL_DEV_GetState(flash);
        if (state < HAL_DEV_STATE_OPEN) {
            status = HAL_STATUS_INVALID_STATE;
            break;
        }

        if ((state == HAL_DEV_STATE_DEEP_SLEEP) ||
            (state == HAL_DEV_STATE_LIGHT_SLEEP)) {
            break;
        }

        HAL_DEV_SetState(flash, HAL_DEV_STATE_DEEP_SLEEP);
        status = flash_MultiCoreInfoSync(FLASH_MULTI_CORE_SET, pFlashInfo);
        if (status != HAL_STATUS_OK) {
            status = HAL_STATUS_INVALID_STATE;
            break;
        }

        if (pFlashInfo->sleepEnable) {
            status = flash_CmdSleep(pFlashDevice);
            if (status != HAL_STATUS_OK) {
                break;
            }

            HAL_BOARD_BlkDelay(pFlashInfo->sleepLatency, HAL_TIME_US);
        }

        if (pFlashInfo->psramRemapCnt > 0) {
            csi_dcache_clean();
            HAL_BOARD_BlkDelay(pFlashInfo->psramFlushLatency, HAL_TIME_US);
        }

        flash_IoAndClkDeInit(pFlashInfo->psramEnable);
    } while (0);

    flashUnlock();

    if (status != HAL_STATUS_OK) {
        HAL_ABORT();
    }

    return status;
}


HAL_Status_t HAL_FLASH_Resume(HAL_Dev_t * flash)
{
    HAL_Status_t status = HAL_STATUS_OK;
    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    if (flash == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    flashLock();

    do {
        FLASH_Info_t * pFlashInfo = &(pFlashDevice->info);

        status = flash_MultiCoreInfoSync(FLASH_MULTI_CORE_GET, pFlashInfo);
        if (status != HAL_STATUS_OK) {
            status = HAL_STATUS_INVALID_STATE;
            break;
        }

        HAL_DevState_t state = HAL_DEV_GetState(flash);
        if (state < HAL_DEV_STATE_OPEN) {
            status = HAL_STATUS_INVALID_STATE;
            break;
        }

        if (state == FLASH_STATE_ACTIVE) {
            break;
        }

        flash_IoAndClkInit(pFlashInfo->clockSrc, 0, pFlashInfo->psramEnable);
        flash_Init(pFlashInfo->clockPsc,
                    pFlashInfo->sampleDelay,
                    pFlashInfo->spiFormat,
                    pFlashInfo->readDummy,
                    PSRAM_CMD_WRITE,
                    PSRAM_CMD_READ,
                    PSRAM_CMD_DUMMY);

        if (pFlashInfo->sleepEnable) {
            status = flash_CmdResume(pFlashDevice);
            if (status != HAL_STATUS_OK) {
                break;
            }

            HAL_BOARD_BlkDelay(pFlashInfo->resumeLatency, HAL_TIME_US);
        }

        if (pFlashInfo->spiFormat == FLASH_SPI_FORMAT_QUAD) {
            if (pFlashInfo->qerType != 0) {
                status = flash_EnableQuardMode(pFlashDevice, pFlashInfo->qerType);
                if (status != HAL_STATUS_OK) {
                    break;
                }
            } else {
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }
        }

        if (pFlashInfo->remapEnable != 0) {
            LL_REMAP_XIP_EnableMap();
            for (uint8_t idx = 0; idx < CONFIG_FLASH_REMAP_SIZE; idx++) {
                LL_REMAP_XIP_SetReMap(idx,
                    pFlashInfo->remapAddrPre[idx],
                    pFlashInfo->remapAddrLst[idx],
                    pFlashInfo->remapLen[idx]);
            }
        }

        HAL_DEV_SetState(flash, HAL_DEV_STATE_ACTIVE);
        status = flash_MultiCoreInfoSync(FLASH_MULTI_CORE_SET, pFlashInfo);
        if (status != HAL_STATUS_OK) {
            status = HAL_STATUS_INVALID_STATE;
            break;
        }
    } while (0);

    flashUnlock();

    if (status != HAL_STATUS_OK) {
        HAL_ABORT();
    }


    return status;
}

#if (CONFIG_FLASH_CHIP_INFO_DETECH == 1)
HAL_Status_t flash_ReadSfdp(const HAL_Dev_t * flash, uint32_t addr, uint8_t *pdata, uint32_t size)
{
    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    if (flash == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    uint8_t InstAddrBuff[5];

    if ((size == 0)
        || (pdata == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    InstAddrBuff[0] = FLASH_CMD_RDSFDP;
    InstAddrBuff[1] = (addr >> 16) & 0xFF;
    InstAddrBuff[2] = (addr >> 8) & 0xFF;
    InstAddrBuff[3] = addr & 0xFF;
    InstAddrBuff[4] = 0xFF;

    return flash_Transfer(pFlashDevice, InstAddrBuff, 5, pdata, size, FSMC_DATA_MODE_RX);
}
#endif

HAL_Status_t flash_ReadChipId(const HAL_Dev_t * flash, uint8_t *chipId)
{
    Flash_Device_t *pFlashDevice = (Flash_Device_t *)flash;

    if (flash == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    return flash_ReadId(pFlashDevice, chipId);
}

void HAL_FLASH_PreInit(void)
{
    if (flash_preinit_state == 0) {
//        LL_RCC_GPIO_EnableBusClock();
//        LL_RCC_GPIO_EnableClock();
//        LL_RCC_GPIO_Reset();

        flash_IoAndClkInit(CONFIG_XIP_PREINIT_CLK_SRC,
                    CONFIG_XIP_PREINIT_CLK_PRESCALER,
                    CONFIG_PSRAM_PREINIT_EN);

        flash_Init(CONFIG_XIP_PREINIT_INTER_CLK_DIV,
                    0,
                    CONFIG_XIP_PREINIT_SPI_FORMAT,
                    CONFIG_XIP_PREINIT_READ_DUMMY,
                    CONFIG_PSRAM_PREINIT_WRITE_OPCODE,
                    CONFIG_PSRAM_PREINIT_READ_OPCODE,
                    CONFIG_PSRAM_PREINIT_READ_DUMMY);

//        flash_preinit_state = 1;
    }
}

#if CONFIG_FLASH_PM
static int flash_pm_action(void *dev, enum pm_device_action action)
{
    HAL_Status_t status = HAL_STATUS_OK;
    if (dev == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Flash_Device_t *pFlashDevice = (Flash_Device_t *)dev;
    if (action == PM_DEVICE_ACTION_SUSPEND) {
        if (pFlashDevice->autoSleep) {
            status = HAL_FLASH_Suspend(dev);
        }
    } else if (action == PM_DEVICE_ACTION_RESUME) {
        if (pFlashDevice->autoSleep) {
            status = HAL_FLASH_Resume(dev);
        }
    } else {
        status = HAL_STATUS_INVALID_PARAM;
    }

    return status;
}

#endif

static HAL_Status_t flash_HwInit(HAL_Dev_t *flash)
{
    HAL_Status_t status = HAL_STATUS_OK;
    Flash_Device_t * pFlashDev = (Flash_Device_t *)flash;
    FLASH_Info_t * pFlashInfo = &(pFlashDev->info);

    if (pFlashDev == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    status = flash_MultiCoreInfoSync(FLASH_MULTI_CORE_GET, pFlashInfo);
    if (status != HAL_STATUS_OK) {
        LOG_ERR("get multi core info error");
        HAL_DEV_SetState(flash, HAL_DEV_STATE_CLOSE);
    }

    HAL_DevState_t state = HAL_DEV_GetState(flash);
    LOG_DBG("get state %d", state);

    do {
        if ((state == HAL_DEV_STATE_INIT) || (state == HAL_DEV_STATE_CLOSE)) {
            LOG_DBG("HAL_FLASH_PreInit");

            HAL_FLASH_PreInit();

            /* 1>. get flash id */
            status = flash_ReadId(pFlashDev, pFlashInfo->flashId);
            if (status != HAL_STATUS_OK) {
                LOG_ERR("get flash id error %d", status);
                break;
            }
            LOG_INF("flash id %02X %02X %02X",
                    pFlashInfo->flashId[0], pFlashInfo->flashId[1], pFlashInfo->flashId[2]);

            /* 2>. get flash info.
             * from flash-list img or  auto detect
             */
            FLASH_ChipInfo_t flashChip;
            int32_t ret = flash_FindMatchChip(flash, pFlashInfo->flashId, pFlashInfo->supportListMode, &flashChip);
            if (ret <= 0) {
                LOG_ERR("get flash chip info error %d", status);
                status = HAL_STATUS_ERROR;
                break;
            }
            pFlashInfo->usedListMode = ret;

            /* 3.1>. generate flash freq
             * spi freq, prescaler and sample delay
             */
            uint32_t req_spi_freq = pFlashInfo->clockFreq;
            uint32_t bus_freq = 0;
            switch (pFlashInfo->clockSrc) {
            case FLASH_CLCOK_SRC_SYS:
                bus_freq = HAL_BOARD_GetFreq(CLOCK_SYS);
                break;
            case FLASH_CLCOK_SRC_MSI:
                bus_freq = HAL_BOARD_GetFreq(CLOCK_MSI);
                break;
            case FLASH_CLCOK_SRC_DCXO:
                bus_freq = HAL_BOARD_GetFreq(CLOCK_DCXO);
                break;
            case FLASH_CLCOK_SRC_PLL_DEV1:
                bus_freq = HAL_BOARD_GetFreq(CLOCK_PLL_DEV1);
                break;
            default:
                break;
            }
            if (bus_freq == 0) {
                status = HAL_STATUS_INVALID_PARAM;
                LOG_ERR("flash bus freq %d", pFlashInfo->clockSrc);
                break;
            }

            if (req_spi_freq > flashChip.spiFreqMax) {
                req_spi_freq = flashChip.spiFreqMax;
            }


            uint32_t curr_freq = 0;
            uint32_t curr_diff = 0;
            uint32_t cap_diff = bus_freq;
            /*
             * soc ahb bus max frequency is 200MHz, it is considered that
             * the flash min frequency is 1MHz, then the max prescaler is
             * 200MHz / 1MHz = 200 = PSIC_FSMC_MAX_PRESCALER.
             *
             * note: the soc prescaler of the flash conctrller must be even
             */
            for (uint32_t prescaler = FLASH_PRESCALER_MIN;
                prescaler < FLASH_PRESCALER_MAX;
                prescaler += FLASH_PRESCALER_STEP) {

                curr_freq = (bus_freq / prescaler);
                curr_diff = (req_spi_freq >= curr_freq) ?
                            (req_spi_freq - curr_freq) :
                            (curr_freq - req_spi_freq);
                if (curr_diff <= cap_diff) {
                    cap_diff = curr_diff;
                    pFlashInfo->clockPsc = prescaler;
                    pFlashInfo->clockFreq = curr_freq;
                } else {
                    break;
                }
            }


            /* get sample delay
             */
            if (bus_freq >= FLASH_DELAY_2_THRESHOLD_FREQ) {
                pFlashInfo->sampleDelay = LL_FSMC_SAMPLE_DELAY_2;
            } else if (bus_freq >= FLASH_DELAY_1_THRESHOLD_FREQ) {
                pFlashInfo->sampleDelay = LL_FSMC_SAMPLE_DELAY_1;
            } else {
                pFlashInfo->sampleDelay = LL_FSMC_SAMPLE_DELAY_0;
            }

            /* 3.2>. generate spi format
             * read opcode, read dummy, qe bit
             */
            if (pFlashInfo->spiFormat > flashChip.spiFormatMax) {
                pFlashInfo->spiFormat = flashChip.spiFormatMax;
            }
            pFlashInfo->qerType = flashChip.qerType;

            /* quad-spi */
            if (pFlashInfo->spiFormat == FLASH_SPI_FORMAT_QUAD) {
                if (pFlashInfo->qerType != 0) {
                    status = flash_EnableQuardMode(pFlashDev, pFlashInfo->qerType);
                    if (status != HAL_STATUS_OK) {
                        LOG_ERR("enable QE bit err(%d)", status);
                        break;
                    }
                } else {
                    /* chaneg quad-spi to dual-spi */
                    pFlashInfo->spiFormat = FLASH_SPI_FORMAT_DUAL;
                }
            }

            pFlashInfo->readDummy = FLASH_DUMMY_DEFAULT;

            /* 3.3>. get sleep param
             * sleep enable, sleep tatency, resum latency
             */
            pFlashInfo->sleepEnable = 0;
            if (flashChip.sleepSupport) {
                pFlashInfo->sleepEnable = 1;
                pFlashInfo->sleepLatency = flashChip.sleepLatency;
                pFlashInfo->resumeLatency = flashChip.resumeLatency;
            }

            /* 3.4>. get flash size
             * flash size, page size, block size
             */
            pFlashInfo->flashSize = flashChip.flashSize;

            LOG_DBG("flash init");
            LOG_DBG("clock src: %d", pFlashInfo->clockSrc);
            LOG_DBG("clock div: %d", 1);
            LOG_DBG("psram enable: %d", pFlashInfo->psramEnable);

            LOG_DBG("psc: %d", pFlashInfo->clockPsc);
            LOG_DBG("sample delay: %d", pFlashInfo->sampleDelay);
            LOG_DBG("line width: %d", pFlashInfo->spiFormat);
            LOG_DBG("dummy: %d", pFlashInfo->readDummy);

            flash_IoAndClkInit(pFlashInfo->clockSrc, 0, pFlashInfo->psramEnable);
            flash_Init(pFlashInfo->clockPsc,
                        pFlashInfo->sampleDelay,
                        pFlashInfo->spiFormat,
                        pFlashInfo->readDummy,
                        PSRAM_CMD_WRITE,
                        PSRAM_CMD_READ,
                        PSRAM_CMD_DUMMY);
            LOG_DBG("flash ctrl init done");

            pFlashInfo->remapEnable = LL_REMAP_XIP_IsEnabledMap();
            if (pFlashInfo->remapEnable) {
                pFlashInfo->psramRemapCnt = 0;
                for (uint8_t idx = 0; idx < CONFIG_FLASH_REMAP_SIZE; idx++) {
                    LL_REMAP_XIP_GetReMap(idx,
                            &pFlashInfo->remapAddrPre[idx],
                            &pFlashInfo->remapAddrLst[idx],
                            &pFlashInfo->remapLen[idx]);

                    if (pFlashInfo->remapLen[idx] > 0) {
                        if ((pFlashInfo->remapAddrPre[idx] & REMAP_ADDR_AREA_MASK) == REMAP_ADDR_TYPE_PSRAM) {
                            pFlashInfo->psramRemapCnt++;
                        }
                    }
                }
            }

            if (pFlashInfo->psramRemapCnt > 0) {
                pFlashInfo->psramFlushLatency = CONFIG_PSRAM_DCACHE_FLUSH_LATENCY;
            }

#if CONFIG_FLASH_SIZE_AUTO_DETECH
            uint8_t bootData[CONFIG_BOOT_HEAD_SIZE];
            uint8_t scanData[CONFIG_BOOT_HEAD_SIZE];
            status = flash_Data(pFlashDev, FLASH_CMD_READ, 0, bootData, CONFIG_BOOT_HEAD_SIZE, FSMC_DATA_MODE_RX);
            if (status != HAL_STATUS_OK) {
                break;
            }
            for (uint32_t offset = CONFIG_FLASH_MIN_SIZE; offset < CONFIG_FLASH_MAX_SIZE; ) {
                status = flash_Data(pFlashDev, FLASH_CMD_READ, offset, scanData, CONFIG_BOOT_HEAD_SIZE, FSMC_DATA_MODE_RX);
                if (status != HAL_STATUS_OK) {
                    break;
                }

                if (memcmp(scanData, bootData, CONFIG_BOOT_HEAD_SIZE) == 0) {
                    pFlashInfo->flashSize = offset;
                    LOG_DBG("auto flash size: %d KB(%d)", pFlashInfo->flashSize / 1024, pFlashInfo->flashSize);
                    break;
                }

                offset <<= 1;
            }
#endif
            HAL_DEV_SetState(flash, HAL_DEV_STATE_CLOSE);
        } else {
            if ((state == HAL_DEV_STATE_LIGHT_SLEEP) ||
                (state == HAL_DEV_STATE_DEEP_SLEEP)) {
                /* resume */
                HAL_FLASH_Resume(flash);
            }
        }
    } while (0);

    if (status == HAL_STATUS_OK) {
        status = flash_MultiCoreInfoSync(FLASH_MULTI_CORE_SET, &(pFlashDev->info));
    }

    __DSB();
    __ISB();


    return status;
}


static HAL_Status_t flash_MultiCoreInfoSync(uint8_t ctrl, FLASH_Info_t * flashInfo)
{
    HAL_Status_t status = HAL_STATUS_OK;

#if CONFIG_FLASH_MULTI_CORE_ENABLE
    int ret = 0;
    if (ctrl == FLASH_MULTI_CORE_GET) {
        FLASH_Info_t tmpFlashInfo;
        ret = KVR_Load(KVR_SYS_ID_FLASH_DUAL_CORE, tmpFlashInfo, sizeof(FLASH_Info_t));
        if (ret == sizeof(FLASH_Info_t)) {
            memcpy(flashInfo, tmpFlashInfo, sizeof(FLASH_Info_t));
        } else {
            status = HAL_STATUS_ERROR;
        }
    } else if (ctrl == FLASH_MULTI_CORE_SET) {
        ret = KVR_Save(KVR_SYS_ID_FLASH_DUAL_CORE, flashInfo, sizeof(FLASH_Info_t));
        if (ret != sizeof(FLASH_Info_t)) {
            status = HAL_STATUS_ERROR;
        }
    } else {
        status = HAL_STATUS_INVALID_PARAM;
    }
#else
    return status;
#endif
}

static HAL_Status_t flash_EnableQuardMode(const Flash_Device_t *pDevice, uint8_t qerType)
{
    HAL_Status_t status = HAL_STATUS_OK;
    uint8_t qe_reg_num;
    uint8_t qe_bit;

    switch (qerType) {
    case JESD216_DW15_QER_NONE:
        /* no QE bit, device detects reads based on opcode */
        status = HAL_STATUS_NOT_SUPPORTED;
        break;
    case JESD216_DW15_QER_S1B6:
        qe_reg_num = 0U;
        qe_bit = 6U;
        break;
    case JESD216_DW15_QER_S2B7:
        qe_reg_num = 1U;
        qe_bit = 7U;
        break;
    case JESD216_DW15_QER_S2B1v1:
    case JESD216_DW15_QER_S2B1v4:
    case JESD216_DW15_QER_S2B1v5:
    case JESD216_DW15_QER_S2B1v6:
        qe_reg_num = 1U;
        qe_bit = 1U;
        break;
    default:
        status = HAL_STATUS_NOT_SUPPORTED;
        break;
    }

    if (status == HAL_STATUS_OK) {
        status = flash_ConfigQuardMode(pDevice, qe_reg_num, qe_bit, false);
    }

    return status;
}

static uint8_t flash_StatusReg2Opcpde(uint8_t reg_idx)
{
    uint8_t opcode = 0;
    switch (reg_idx + 1) {
    case 1U:
        opcode = FLASH_CMD_RDSR;
        break;
    case 2U:
        opcode = FLASH_CMD_RDSR2;
        break;
    case 3U:
        opcode = FLASH_CMD_RDSR3;
        break;
    default:
        opcode = FLASH_CMD_RDSR;
    }
    return opcode;
}

static HAL_Status_t flash_ConfigQuardMode(const Flash_Device_t *pDevice,
                                uint8_t reg_idx, uint8_t bit_idx, bool seq_read)
{
    HAL_Status_t status = HAL_STATUS_OK;
    uint8_t status_reg[FSMC_SR_MAX];
    uint8_t *tmp_sr;
    uint8_t inst = 0;
    uint8_t read_size = 0;

    if (((reg_idx + 1) > FSMC_SR_MAX)
        || (bit_idx > 8)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (seq_read) {
        tmp_sr = status_reg;
        inst = FLASH_CMD_RDSR;
        read_size = reg_idx + 1;
    } else {
        tmp_sr = &status_reg[reg_idx];
        inst = flash_StatusReg2Opcpde(reg_idx);
        read_size = 1;
    }

    status = flash_Cmd(pDevice, inst, tmp_sr, read_size, FSMC_DATA_MODE_RX);
    if (status != 0) {
        return status;
    }

    if ((status_reg[reg_idx] & (1 << bit_idx)) != 0U) {
        return 0;
    }

    /* pertect other sr */
    for (int i = 0; i < reg_idx; i++) {
        flash_Cmd(pDevice, flash_StatusReg2Opcpde(i),
                     &status_reg[i], read_size, FSMC_DATA_MODE_RX);

        if (status != 0) {
            return status;
        }
    }

    status_reg[reg_idx] |= (1 << bit_idx);

    status = flash_CmdWriteEnable(pDevice);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = flash_Cmd(pDevice, FLASH_CMD_WRSR, status_reg, (reg_idx + 1), FSMC_DATA_MODE_TX);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = flash_CmdWaitDone(pDevice);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = flash_Cmd(pDevice, inst, tmp_sr, read_size, FSMC_DATA_MODE_RX);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    if ((status_reg[reg_idx] & (1 << bit_idx)) == 0U) {
        return HAL_STATUS_NOT_SUPPORTED;
    }


    return status;
}

static HAL_Status_t flash_ReadId(const Flash_Device_t *pDevice, uint8_t *pid)
{
    return flash_Cmd(pDevice, FLASH_CMD_RDID, pid, 3, FSMC_DATA_MODE_RX);
}

static HAL_Status_t flash_CmdSleep(const Flash_Device_t *pDevice)
{
    return flash_Cmd(pDevice, FLASH_CMD_SLEEP, NULL, 0, FSMC_DATA_MODE_NULL);
}

static HAL_Status_t flash_CmdResume(const Flash_Device_t *pDevice)
{
    return flash_Cmd(pDevice, FLASH_CMD_RESUME, NULL, 0, FSMC_DATA_MODE_NULL);
}


// static HAL_Status_t flash_SetWriteProtect(const Flash_Device_t *pDevice, uint8_t wp, uint8_t wp_msk)
// {
//     HAL_Status_t status = HAL_STATUS_OK;
//     uint8_t status_reg[FSMC_SR_MAX];

//     status = flash_Cmd(pDevice, FLASH_CMD_VSR, NULL, 1, FSMC_DATA_MODE_NULL);
//     if (status != HAL_STATUS_OK) {
//         return status;
//     }

//     status = flash_CmdWriteEnable(pDevice);
//     if (status != HAL_STATUS_OK) {
//         return status;
//     }

//     status = flash_Cmd(pDevice, FLASH_CMD_RDSR, status_reg, 1, FSMC_DATA_MODE_RX);
//     if (status != HAL_STATUS_OK) {
//         return status;
//     }

//     status_reg[0] &= ~wp_msk;
//     status_reg[0] |= wp;

//     status = flash_Cmd(pDevice, FLASH_CMD_WRSR, status_reg, 1, FSMC_DATA_MODE_TX);
//     if (status != HAL_STATUS_OK) {
//         return status;
//     }

//     status = flash_CmdWaitDone(pDevice);

//     return status;
// }


// static HAL_Status_t flash_GetWriteProtect(const Flash_Device_t *pDevice, uint8_t *wp, uint8_t wp_msk)
// {
//     HAL_Status_t status = HAL_STATUS_OK;
//     uint8_t status_reg[FSMC_SR_MAX];

//     status = flash_Cmd(pDevice, FLASH_CMD_RDSR, status_reg, 1, FSMC_DATA_MODE_RX);
//     if (status != HAL_STATUS_OK) {
//         return status;
//     }

//     *wp = status_reg[0] & wp_msk;

//     return status;
// }


static HAL_Status_t flash_Cmd(const Flash_Device_t *pDevice,
                    uint8_t Opcode,
                    uint8_t *pData,
                    uint32_t Size,
                    uint8_t Mode)
{
    return flash_Transfer(pDevice, &Opcode, 1, pData, Size, Mode);
}

static HAL_Status_t flash_Data(const Flash_Device_t *pDevice,
                        uint8_t Opcode,
                        uint32_t Addr,
                        uint8_t *pData,
                        uint32_t Size,
                        uint8_t Mode)
{
    uint8_t InstAddrBuff[4];

    InstAddrBuff[0] = Opcode;
    InstAddrBuff[1] = (Addr >> 16) & 0xFF;
    InstAddrBuff[2] = (Addr >> 8) & 0xFF;
    InstAddrBuff[3] = Addr & 0xFF;

    return flash_Transfer(pDevice, InstAddrBuff, 4, pData, Size, Mode);
}

static HAL_Status_t flash_CmdWriteEnable(const Flash_Device_t *pDevice)
{
    HAL_Status_t status = HAL_STATUS_OK;
    uint8_t cmd = 0;
    uint8_t reg = 0;
    uint32_t cmd_to = 0;

    cmd = FLASH_CMD_WREN;
    status = flash_Transfer(pDevice, &cmd, 1, NULL, 0, FSMC_DATA_MODE_NULL);
    if (status != 0) {
        return HAL_STATUS_ERROR;
    }


    status = HAL_STATUS_ERROR;
    for (cmd_to = 0; cmd_to < FLASH_CMD_TO; cmd_to++) {
        cmd = FLASH_CMD_RDSR;
        status = flash_Transfer(pDevice, &cmd, 1, &reg, 1, FSMC_DATA_MODE_RX);
        if (status != 0) {
            status = HAL_STATUS_ERROR;
            break;
        }

        if (reg & FSMC_SR_WEL_BIT) {
            status = HAL_STATUS_OK;
            break;
        }
    }

    return status;
}

static HAL_Status_t flash_CmdWaitDone(const Flash_Device_t *pDevice)
{
    HAL_Status_t status = HAL_STATUS_OK;
    uint8_t cmd = 0;
    uint8_t reg = 0;
    uint32_t cmd_to = 0;

    status = HAL_STATUS_NOT_SUPPORTED;
    for (cmd_to = 0; cmd_to < FLASH_CMD_TO; cmd_to++) {
        cmd = FLASH_CMD_RDSR;
        status = flash_Transfer(pDevice, &cmd, 1, &reg, 1, FSMC_DATA_MODE_RX);
        if (status != HAL_STATUS_OK) {
            status = HAL_STATUS_ERROR;
            break;
        }

        if (!(reg & FSMC_SR_WIP_BIT)) {
            status = HAL_STATUS_OK;
            break;
        }
    }

    return status;
}


static HAL_Status_t flash_Transfer(const Flash_Device_t *pDevice,
                                const uint8_t *pInstBuff,
                                uint32_t InstSize,
                                uint8_t *pDataBuff,
                                uint32_t DataSize,
                                uint8_t Mode)
{
    uint32_t tmp_reg = 0;
    fsmc_dev_t *FsmcReg = (fsmc_dev_t *)(pDevice->device.reg);

    while (LL_FSMC_StatusIsSet(FsmcReg, LL_FSMC_STATUS_RXNE)) {
        (void)LL_FSMC_ReadDataByte(FsmcReg);
    }

    tmp_reg = FsmcReg->QSPI_ENHANCE;

    LL_FSMC_Disable(FsmcReg);

    if (pDevice->info.psramEnable) {
        LL_FSMC_SetFrameFormat(FsmcReg, LL_FSMC_FRAME_FORMAT_STD);
        LL_FSMC_SetTransferMode(FsmcReg, LL_FSMC_TRANS_MODE_TX_AND_RX);
        LL_FSMC_SetDataWidth(FsmcReg, LL_FSMC_DFS_BIT(8));
        LL_FSMC_SetTxStartFIFOLevel(FsmcReg, LL_FSMC_FIFO_LEVEL(1));
    }

    LL_FSMC_EnableSlaveSelect(FsmcReg, LL_FSMC_SELECT_CS0);
    LL_FSMC_StretchEnable(FsmcReg);
    LL_FSMC_Enable(FsmcReg);


    /* cs */
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, LL_GPIO_PIN_4, LL_GPIOx_Pn_F1_OUTPUT);
    LL_GPIO_ResetOutputPin(GPIOB_DEV, LL_GPIO_PIN_MSK_4);    /*cs = 0*/


    for (uint32_t idx = 0; idx < InstSize; idx++) {
        LL_FSMC_WriteDataWord(FsmcReg, pInstBuff[idx]);

        while (1) {
            if (LL_FSMC_GetRxFIFOLevel(FsmcReg) > 0) {
                LL_FSMC_ReadDataWord(FsmcReg);
                break;
            }
        }
    }

    for (uint32_t idx = 0; idx < DataSize;) {
        /* write */
        if (Mode) {
            if (LL_FSMC_GetTxFIFOLevel(FsmcReg) < 24) {
                LL_FSMC_WriteDataWord(FsmcReg, pDataBuff[idx]);
                idx++;
            }

            if (LL_FSMC_GetRxFIFOLevel(FsmcReg)) {
                LL_FSMC_ReadDataWord(FsmcReg);
            }

        /* read */
        } else {
            if (LL_FSMC_GetTxFIFOLevel(FsmcReg) < 24) {
                LL_FSMC_WriteDataWord(FsmcReg, 0xFF);
            }

            if (LL_FSMC_GetRxFIFOLevel(FsmcReg)) {
                pDataBuff[idx] = LL_FSMC_ReadDataWord(FsmcReg);
                idx++;
            }
        }
    }


    while (!LL_FSMC_StatusIsSet(FsmcReg, LL_FSMC_STATUS_TXE)) {
    }

    while (LL_FSMC_StatusIsSet(FsmcReg, LL_FSMC_STATUS_BUSY)) {
    }

    /* cs */
    LL_GPIO_SetOutputPin(GPIOB_DEV, LL_GPIO_PIN_MSK_4);        /*cs = 1*/
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, LL_GPIO_PIN_4, LL_GPIOB_P4_F2_FC_CS);

    /* resume config */
    LL_FSMC_Disable(FsmcReg);
    FsmcReg->QSPI_ENHANCE = tmp_reg;
    LL_FSMC_EnableSlaveSelect(FsmcReg, LL_FSMC_SELECT_CS1);
    LL_FSMC_StretchDisable(FsmcReg);
    LL_FSMC_Enable(FsmcReg);

    return 0;
}

static HAL_Status_t flash_Init(uint16_t prescaler,
                        uint8_t sample_delay,
                        uint8_t xip_spi_format,
                        uint8_t xip_dummy,
                        uint8_t psram_write_inst,
                        uint8_t psram_read_inst,
                        uint8_t psram_dummy)
{
    uint8_t xip_read_inst = 0;

    LL_FSMC_Disable(FSMC_DEV);

    LL_FSMC_ITFlagClearALL(FSMC_DEV);
    LL_FSMC_ITDisable(FSMC_DEV, 0x3F);

    LL_FSMC_SetMasterSlave(FSMC_DEV, LL_FSMC_MST);
    LL_FSMC_SetFrameType(FSMC_DEV, LL_FSMC_FRAME_TYPE_SPI);
    LL_FSMC_SetPrescaler(FSMC_DEV, prescaler);

    LL_FSMC_SetRxSampleDelay(FSMC_DEV, sample_delay);
    LL_FSMC_RemoveHalfRxSampleDelay(FSMC_DEV);

    /* common qspi config ------------------------------------------------- */
    LL_FSMC_SetFrameFormat(FSMC_DEV, LL_FSMC_FRAME_FORMAT_STD);
    LL_FSMC_SetClockPolarity(FSMC_DEV, LL_FSMC_CLK_POLARITY_H);
    LL_FSMC_SetClockPhase(FSMC_DEV, LL_FSMC_CLK_PHASE_START);
    LL_FSMC_SetTransferMode(FSMC_DEV, LL_FSMC_TRANS_MODE_TX_AND_RX);
    LL_FSMC_SetDataWidth(FSMC_DEV, LL_FSMC_DFS_BIT(8));
    LL_FSMC_SlaveSelectToggleDisable(FSMC_DEV);

    LL_FSMC_SetTransType(FSMC_DEV, LL_FSMC_INST_CFG_ADDR_CFG);

    LL_FSMC_SetTxStartFIFOLevel(FSMC_DEV, LL_FSMC_FIFO_LEVEL(1));
    LL_FSMC_SetRxFIFOThreshold(FSMC_DEV, LL_FSMC_FIFO_LEVEL(1));

    LL_FSMC_SetTransType(FSMC_DEV, LL_FSMC_INST_STD_ADDR_STD);
    LL_FSMC_SetInstLen(FSMC_DEV, LL_FSMC_INST_LEN_8);

    LL_FSMC_EnableSlaveSelect(FSMC_DEV, LL_FSMC_SELECT_CS1);

    LL_FSMC_DisableBigendian(FSMC_DEV);


    /* XIP config --------------------------------------------------------- */
    if (xip_spi_format == FLASH_SPI_FORMAT_DUAL) {
        xip_read_inst = FLASH_CMD_DREAD;
    } else if (xip_spi_format == FLASH_SPI_FORMAT_QUAD) {
        xip_read_inst = FLASH_CMD_QREAD;
    } else {
        xip_read_inst = FLASH_CMD_READ;
    }

    LL_FSMC_XIP_SetIncrInst(FSMC_DEV, xip_read_inst);
    LL_FSMC_XIP_SetWrapInst(FSMC_DEV, xip_read_inst);

    LL_FSMC_XIP_SetModeBitsLen(FSMC_DEV, LL_FSMC_MDB_LEN(2));
    LL_FSMC_XIP_SetDummyCycles(FSMC_DEV, LL_FSMC_WAIT_CYCLES(xip_dummy));
    LL_FSMC_XIP_ModeBitsDisable(FSMC_DEV);
    LL_FSMC_XIP_SetTransType(FSMC_DEV, LL_FSMC_INST_STD_ADDR_STD);
    LL_FSMC_XIP_SetFrameFormat(FSMC_DEV, xip_spi_format);
    /* not support 16bit cmd in 2023-08-12 */
    LL_FSMC_XIP_SetInstLen(FSMC_DEV, LL_FSMC_INST_LEN_8);
    /* not support 32 bit addr in 2023-08-12 */
    LL_FSMC_XIP_SetAddrLen(FSMC_DEV, LL_FSMC_ADDR_LEN_24);
    LL_FSMC_XIP_EnableSlaveSelect(FSMC_DEV);
    LL_FSMC_XIP_PreFetchDisable(FSMC_DEV);
    LL_FSMC_XIP_ContXferDisable(FSMC_DEV);

    /* PSRAM config --------------------------------------------------------- */
    LL_FSMC_FullDuplexDisable(FSMC_DEV);
    LL_FSMC_SetFrameFormat(FSMC_DEV, LL_FSMC_FRAME_FORMAT_QUAD);
    LL_FSMC_SetTransType(FSMC_DEV, LL_FSMC_INST_STD_ADDR_CFG);
    LL_FSMC_SetDummyCycles(FSMC_DEV, psram_dummy);

    LL_FSMC_PSRAM_EnableSelectCS1(FSMC_DEV);

    LL_FSMC_PSRAM_SetTxInst(FSMC_DEV, psram_write_inst);
    LL_FSMC_PSRAM_SetRxInst(FSMC_DEV, psram_read_inst);

    LL_FSMC_SetInstLen(FSMC_DEV, LL_FSMC_INST_LEN_8);
    LL_FSMC_SetAddrLen(FSMC_DEV, LL_FSMC_ADDR_LEN_24);


    LL_FSMC_SetFrameFormat(FSMC_DEV, LL_FSMC_FRAME_FORMAT_STD);
    LL_FSMC_SetTransferMode(FSMC_DEV, LL_FSMC_TRANS_MODE_TX_AND_RX);
    LL_FSMC_FullDuplexEnable(FSMC_DEV);
    LL_FSMC_SetRxDataNumber(FSMC_DEV, 1);

    /* enable fsmc */
    LL_FSMC_Enable(FSMC_DEV);

    return 0;
}

static void flash_IoAndClkInit(uint8_t clk_src, uint8_t clk_div, uint8_t psram)
{
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 2, LL_GPIOB_P2_F3_FC_WP);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 3, LL_GPIOB_P3_F3_FC_HOLD);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 4, LL_GPIOB_P4_F2_FC_CS);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 5, LL_GPIOB_P5_F2_FC_CLK);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 6, LL_GPIOB_P6_F2_FC_MOSI);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 7, LL_GPIOB_P7_F2_FC_MIS0);

    if (psram) {
        LL_GPIO_SetPinFuncMode(GPIOA_DEV, 7, LL_GPIOA_P7_F5_PSRAM_CS);
    }

    LL_RCC_FSMC_Reset();
    LL_RCC_FSMC_SetClockSource(clk_src);
    LL_RCC_FSMC_SetPrescaler(clk_div);
    LL_RCC_FSMC_EnableBusClock();
    LL_RCC_FSMC_EnableClock();

    flash_preinit_state = 1;
}


static void flash_IoAndClkDeInit(uint8_t psram)
{
    flash_preinit_state = 0;

    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 2, LL_GPIOx_Pn_F15_DIS);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 3, LL_GPIOx_Pn_F15_DIS);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 4, LL_GPIOx_Pn_F15_DIS);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 5, LL_GPIOx_Pn_F15_DIS);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 6, LL_GPIOx_Pn_F15_DIS);
    LL_GPIO_SetPinFuncMode(GPIOB_DEV, 7, LL_GPIOx_Pn_F15_DIS);

    if (psram) {
        LL_GPIO_SetPinFuncMode(GPIOA_DEV, 7, LL_GPIOx_Pn_F15_DIS);
    }

    LL_RCC_FSMC_DisableBusClock();
    LL_RCC_FSMC_DisableClock();
    LL_RCC_FSMC_Reset();
    LL_RCC_FSMC_DisableClock();
    LL_RCC_FSMC_DisableBusClock();
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
