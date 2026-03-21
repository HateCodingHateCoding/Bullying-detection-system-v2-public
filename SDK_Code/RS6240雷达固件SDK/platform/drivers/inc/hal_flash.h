/**
 ******************************************************************************
 * @file    hal_flash.h
 * @brief   hal flash define.
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

#ifndef _HAL_FLASH_H
#define _HAL_FLASH_H

/* Includes.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
extern "C" {
#endif


#include "hal_types.h"
#include "hal_dev.h"
#include "driver_config.h"


/* Exported config.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_FLASH_CHIP_INFO_LIST
#define CONFIG_FLASH_CHIP_INFO_LIST                     1
#endif

#ifndef CONFIG_FLASH_CHIP_INFO_DETECH
#define CONFIG_FLASH_CHIP_INFO_DETECH                   0
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */
typedef enum {
    FLASH_DEFT_ID       = 0U,
    FLASH_NUM,
    FLASH_INVALID_ID    = 0xFFU
} FLASH_ID_t;

typedef enum {
    FLASH_CLCOK_SRC_SYS         = 0,
    FLASH_CLCOK_SRC_MSI         = 1,
    FLASH_CLCOK_SRC_DCXO        = 2,
    FLASH_CLCOK_SRC_PLL_DEV1    = 3,
    FLASH_CLCOK_SRC_MAX         = 4
} FLASH_ClockSrc_t;

typedef enum {
    FLASH_SPI_FORMAT_STANDARD   = 0,
    FLASH_SPI_FORMAT_DUAL       = 1,
    FLASH_SPI_FORMAT_QUAD       = 2,
    FLASH_SPI_FORMAT_MAX        = 3
} FLASH_SpiFormat_t;

typedef struct {
    uint32_t            clockFreq;
    FLASH_ClockSrc_t    clockSrc;
    FLASH_SpiFormat_t   spiFormat;
    bool                psramEnable;
} FLASH_InitParam_t;

typedef struct {
    uint32_t    flashSize;      /* Bytes */
    uint32_t    spiFreqMax;     /* Hz */
    /* S2Bx: Status REgister 2, bit x
     * None : 0
     * S2B1 : 1
     * S2B6 : 2
     * S2B7 : 3
     */
    uint8_t     spiFormatMax;   /* see FLASH_SpiFormat_t */
    uint8_t     qerType;
    uint8_t     sleepSupport;   /* sleep support */
    uint8_t     sleepLatency;   /* us */
    uint8_t     resumeLatency;  /* us */

    #define FLASH_CHIP_ID_SIZE              3
    uint8_t     chipId[FLASH_CHIP_ID_SIZE];
} FLASH_ChipInfo_t;

typedef enum {
    FLASH_LIST_TYPE_DEFAULT         = 1,
    FLASH_LIST_TYPE_LIST            = 2,
    FLASH_LIST_TYPE_DETECH          = 4,
} FLASH_ListType_t;

typedef enum {
    FLASH_GET_FLASH_SIZE            = 0,
    FLASH_GET_PAGE_SIZE             = 1,
    FLASH_GET_BLOCK_SIZE            = 2,
    FLASH_GET_PSRAM_SIZE            = 3,
    FLASH_GET_CHIP_ID               = 4,

    FLASH_EXT_GET_CLK_SRC           = 5,
    FLASH_EXT_GET_CLK_DIV           = 6,
    FLASH_EXT_GET_CLK_FREQ          = 7,
    FLASH_EXT_GET_SPI_FORMAT        = 8,
    FLASH_EXT_GET_SLEEP_STATE       = 9,
    FLASH_EXT_SET_SLEEP_STATE       = 10,
    FLASH_EXT_GET_SLEEP_LATENCY     = 11,
    FLASH_EXT_SET_SLEEP_LATENCY     = 12,
    FLASH_EXT_GET_RESUME_LATENCY    = 13,
    FLASH_EXT_SET_RESUME_LATENCY    = 14,
    FLASH_EXT_GET_AUTO_SLEEP_STATE  = 15,
    FLASH_EXT_SET_AUTO_SLEEP_STATE  = 16,

    FLASH_EXT_GET_LIST_SIZE         = 17,
    FLASH_EXT_GET_CHIP_INFO_DEFAULT = 18,
    FLASH_EXT_GET_CHIP_INFO_LIST    = 19,
    FLASH_EXT_GET_CHIP_INFO_DETECH  = 20,
    FLASH_EXT_GET_CHIP_INFO_MODE    = 21,
    FLASH_EXT_SET_CHIP_INFO_MODE    = 22,
    FLASH_EXT_GET_USED_CHIP_INFO_MODE   = 23,

} FLASH_Attr_t;


/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
/**
 * @brief      pre-initialise the FLASH and PSRAM device.
 * @param      None
 * @return     None
 */
void HAL_FLASH_PreInit(void);

/**
 * @brief      Initialise the FLASH and PSRAM device.
 * @param[in]  pinitParam
 * @return     Hardware status (HAL_STATUS_OK if Successful)
 */
HAL_Dev_t * HAL_FLASH_Init(FLASH_ID_t id);

HAL_Status_t HAL_FLASH_Open(HAL_Dev_t * flash, FLASH_InitParam_t * pInitParam);

HAL_Status_t HAL_FLASH_Close(HAL_Dev_t * flash);

HAL_Status_t HAL_FLASH_DeInit(HAL_Dev_t * flash);

HAL_Status_t HAL_FLASH_Read(const HAL_Dev_t *flash, uint32_t addr, uint8_t *pdata, uint32_t size);

HAL_Status_t HAL_FLASH_Write(const HAL_Dev_t *flash, uint32_t addr, const uint8_t *pdata, uint32_t size);

HAL_Status_t HAL_FLASH_Erase(const HAL_Dev_t *flash, uint32_t addr, uint32_t size);

HAL_Status_t HAL_FLASH_ExtControl(HAL_Dev_t *flash, FLASH_Attr_t attr, void * arg, uint32_t argSize);

HAL_Status_t HAL_FLASH_Suspend(HAL_Dev_t * flash);

HAL_Status_t HAL_FLASH_Resume(HAL_Dev_t * flash);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_FLASH_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

