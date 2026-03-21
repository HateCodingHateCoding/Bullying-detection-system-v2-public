/**
 ******************************************************************************
 * @file    cmd_flash.c
 * @brief   cmd flash define.
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

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_FLASH == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
char * strSpiLineWidth[3] = {
    "standard",
    "dual",
    "quad",
};

char * strClkSrc[4] = {
    "pll",
    "msi",
    "dcxo",
    "pll_dev1",
};

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define SHELL_FLASH_LIST_MAX                        32
/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */
int flash_read_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 3) {
            HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
            if (pflashDev == NULL) {
                shellPrint(shell, "hal flash not init\n");
                return HAL_STATUS_ERROR;
            }

            uint32_t addr = strtoul(argv[1], NULL, 0);
            uint32_t len = strtoul(argv[2], NULL, 0);

            uint8_t *pValue = OSI_Malloc(len);
            if (pValue == NULL) {
                shellPrint(shell, "malloc buff fail %d\n", status);
                return HAL_STATUS_NO_MEM;
            }
            shellPrint(shell, "malloc buff success %p %d\n", pValue, len);

            for (uint32_t idx = 0; idx < len; idx++) {
                pValue[idx] = 0;
            }

            status = HAL_FLASH_Read(pflashDev, addr, pValue, len);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "read flash fail %d\n", status);
                OSI_Free(pValue);
                return status;
            }

            for (uint32_t idx = 0; idx < len; idx++) {
                if ((idx % 16) == 0) {
                    shellPrint(shell, "\n\r%4d :", idx);
                }
                shellPrint(shell, " %02X", pValue[idx]);
            }
            OSI_Free(pValue);
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "flash read <addr> <len>");
    }


    return HAL_STATUS_OK;
}

int flash_write_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 4) {
            HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
            if (pflashDev == NULL) {
                shellPrint(shell, "hal flash not init\n");
                return HAL_STATUS_ERROR;
            }

            uint32_t addr = strtoul(argv[1], NULL, 0);
            uint32_t buff = strtoul(argv[2], NULL, 16);
            uint32_t len = strtoul(argv[3], NULL, 0);
            uint8_t *pValue = (uint8_t *)buff;

            status = HAL_FLASH_Write(pflashDev, addr, pValue, len);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "write flash fail %d\n", status);
                return status;
            }
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "flash write <addr> <buff> <len>");
    }

    return HAL_STATUS_OK;
}

int flash_erase_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 3) {
            HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
            if (pflashDev == NULL) {
                shellPrint(shell, "hal flash not init\n");
                return HAL_STATUS_ERROR;
            }

            uint32_t addr = strtoul(argv[1], NULL, 0);
            uint32_t len = strtoul(argv[2], NULL, 0);

            status = HAL_FLASH_Erase(pflashDev, addr, len);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "erase flash fail %d\n", status);
                return status;
            }
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "flash erase <addr> <len>");
    }


    return HAL_STATUS_OK;

}

__sram_text HAL_Status_t cmd_InitXIP(FLASH_InitParam_t flashInitParam, bool defaultList)
{
    HAL_Status_t status = HAL_STATUS_OK;
    HAL_Dev_t *pFlashDev = NULL;

    pFlashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, FLASH_DEFT_ID);
    if (pFlashDev == NULL) {
        status = HAL_STATUS_ERROR;
    }

    uint32_t listMode = FLASH_LIST_TYPE_LIST;
    if (defaultList) {
        listMode |= FLASH_LIST_TYPE_DEFAULT;
    }
    status = HAL_FLASH_ExtControl(pFlashDev, FLASH_EXT_SET_CHIP_INFO_MODE, &listMode, sizeof(listMode));
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = HAL_FLASH_Close(pFlashDev);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = HAL_FLASH_Open(pFlashDev, &flashInitParam);

    return status;
}

int flash_open_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 6) {
            FLASH_InitParam_t flashInitParam;
            uint8_t spiFormat = 0;

            if (!strcmp(argv[1], "sys")) {
                flashInitParam.clockSrc = FLASH_CLCOK_SRC_SYS;
            } else if (!strcmp(argv[1], "msi")) {
                flashInitParam.clockSrc = FLASH_CLCOK_SRC_MSI;
            } else if (!strcmp(argv[1], "dcxo")) {
                flashInitParam.clockSrc = FLASH_CLCOK_SRC_DCXO;
            } else if (!strcmp(argv[1], "pll_dev1")) {
                flashInitParam.clockSrc = FLASH_CLCOK_SRC_PLL_DEV1;
            } else {
                shellPrint(shell, "flash clock invalid\n");
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }

            flashInitParam.clockFreq = strtoul(argv[2], NULL, 10);
            spiFormat = strtoul(argv[3], NULL, 10);
            if (spiFormat == 1) {
                flashInitParam.spiFormat = FLASH_SPI_FORMAT_STANDARD;
            } else if (spiFormat == 2) {
                flashInitParam.spiFormat = FLASH_SPI_FORMAT_DUAL;
            } else if (spiFormat == 4) {
                flashInitParam.spiFormat = FLASH_SPI_FORMAT_QUAD;
            } else {
                shellPrint(shell, "flash line width invalid\n");
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }

            if (!strcmp(argv[4], "e")) {
                flashInitParam.psramEnable = true;
            } else if (!strcmp(argv[4], "d")) {
                flashInitParam.psramEnable = false;
            } else {
                shellPrint(shell, "psram enable invalid\n");
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }

            bool defaultList = false;
            if (!strcmp(argv[5], "e")) {
                defaultList = true;
            } else if (!strcmp(argv[5], "d")) {
                defaultList = false;
            } else {
                shellPrint(shell, "use default list invalid\n");
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }

            status = cmd_InitXIP(flashInitParam, defaultList);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "flash reinit fail\n");
                return status;
            }

            HAL_Dev_t * pFlashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
            if (pFlashDev == NULL) {
                shellPrint(shell, "flash reinit fail\n");
                return status;
            }

            shellPrint(shell, "flash reinit success\n");

            uint32_t flashFreq = 0;
            status = HAL_FLASH_ExtControl(pFlashDev, FLASH_EXT_GET_CLK_FREQ, &flashFreq, 4);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }
            shellPrint(shell, "flash run freq: %d Hz\n", flashFreq);

            status = HAL_FLASH_ExtControl(pFlashDev, FLASH_EXT_GET_SPI_FORMAT, &spiFormat, 1);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }
            shellPrint(shell, "flash run line width: %d\n", 1 << spiFormat);

        }else {
            shellPrint(shell, "param cnt invalid %d\n", argc);
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "<clk src> <freq> <line width> <psram enable> <use defalt list>\n");
        shellPrint(shell, "\t-clk src: sys / msi / dcxo / pll_dev1\n");
        shellPrint(shell, "\t-freq: less src freq\n");
        shellPrint(shell, "\t-line width: 1 / 2 / 4\n");
        shellPrint(shell, "\t-psram enable: e / d\n");
        shellPrint(shell, "\t-use defalt list: e / d\n");
    }

    return HAL_STATUS_OK;

}


int flash_close_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
    if (pflashDev == NULL) {
        return HAL_STATUS_OK;
    }

    return HAL_FLASH_Close(pflashDev);
}


int flash_size_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
        if (pflashDev == NULL) {
            shellPrint(shell, "hal flash not init\n");
            return HAL_STATUS_ERROR;
        }

        uint32_t val;
        status = HAL_FLASH_ExtControl(pflashDev, FLASH_GET_FLASH_SIZE, &val, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }
        shellPrint(shell, "flash size: %d KB\n", val / 1024);

        status = HAL_FLASH_ExtControl(pflashDev, FLASH_GET_BLOCK_SIZE, &val, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }
        shellPrint(shell, "block size: %d KB\n", val / 1024);

        status = HAL_FLASH_ExtControl(pflashDev, FLASH_GET_PAGE_SIZE, &val, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }
        shellPrint(shell, "page size: %d B\n", val);

        status = HAL_FLASH_ExtControl(pflashDev, FLASH_GET_PSRAM_SIZE, &val, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "psram size: 0\n");
            status = HAL_STATUS_OK;
            break;
        }
        shellPrint(shell, "psram size: %d KB\n", val / 1024);

    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "get flash size fail\n");
    }


    return HAL_STATUS_OK;
}

int flash_id_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
        if (pflashDev == NULL) {
            shellPrint(shell, "hal flash not init\n");
            return HAL_STATUS_ERROR;
        }

        uint8_t chipId[3];
        status = HAL_FLASH_ExtControl(pflashDev, FLASH_GET_CHIP_ID, chipId, 3);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }
        shellPrint(shell, "chip id: %02X %02X %02X\n", chipId[0], chipId[1], chipId[2]);
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "get chip id fail");
    }

    return status;
}


static void flash_list_show(Shell *shell, FLASH_ChipInfo_t *pchipInfo)
{
    shellPrint(shell, "chip id:        %02X %02X %02X\n",
            pchipInfo->chipId[0],
            pchipInfo->chipId[1],
            pchipInfo->chipId[2]);
    shellPrint(shell, "flash size:     %d KB\n", pchipInfo->flashSize / 1024);
    shellPrint(shell, "spi freq:       %d Hz (%d MHz)\n", pchipInfo->spiFreqMax, pchipInfo->spiFreqMax / 1000000);
    shellPrint(shell, "spi line width: %d %s\n", pchipInfo->spiFormatMax, strSpiLineWidth[pchipInfo->spiFormatMax]);
    shellPrint(shell, "qer type:       0x%02X\n", pchipInfo->qerType);
    shellPrint(shell, "sleep:          %s\n", pchipInfo->sleepSupport != 0 ? "ena": "dis");
    shellPrint(shell, "sleep latency:  %d us\n", pchipInfo->sleepLatency);
    shellPrint(shell, "resume latency: %d us\n", pchipInfo->resumeLatency);
}

int flash_list_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;


    do {
        HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
        if (pflashDev == NULL) {
            shellPrint(shell, "hal flash not init\n");
            return HAL_STATUS_ERROR;
        }

        uint32_t modeMask = 0;
        status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_CHIP_INFO_MODE, &modeMask, sizeof(modeMask));
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }

        uint32_t usedMode = 0;
        status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_USED_CHIP_INFO_MODE, &usedMode, sizeof(uint32_t));
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }

        if (usedMode == FLASH_LIST_TYPE_DEFAULT) {
            shellPrint(shell, "used default chip info\n");
        } else if (usedMode == FLASH_LIST_TYPE_LIST) {
            shellPrint(shell, "used flash list chip info\n");
        } else if (usedMode == FLASH_LIST_TYPE_DETECH) {
            shellPrint(shell, "used auto detech chip info\n");
        }

        if (modeMask & FLASH_LIST_TYPE_DEFAULT) {
            FLASH_ChipInfo_t flashChipInfo;
            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_CHIP_INFO_DEFAULT, &flashChipInfo, sizeof(FLASH_ChipInfo_t));
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }
            shellPrint(shell, "\n-----------------------\n");
            shellPrint(shell, "default chip info:\n");
            flash_list_show(shell, &flashChipInfo);
        }

        if (modeMask & FLASH_LIST_TYPE_LIST) {
            FLASH_ChipInfo_t *flashChipInfo = NULL;
            uint32_t chipSize = 0;
            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_LIST_SIZE, &chipSize, sizeof(chipSize));
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            if (chipSize == 0) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            flashChipInfo = (FLASH_ChipInfo_t *)OSI_Malloc(sizeof(FLASH_ChipInfo_t) * chipSize);
            if (flashChipInfo == NULL) {
                status = HAL_STATUS_ERROR;
                break;
            }

            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_CHIP_INFO_LIST, flashChipInfo, chipSize);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                OSI_Free(flashChipInfo);
                break;
            }
            shellPrint(shell, "\n-----------------------\n");
            shellPrint(shell, "flash list chip info total cnt: %d\n", chipSize);
            FLASH_ChipInfo_t *chipInfo = flashChipInfo;
            for (uint32_t idx = 0; idx < chipSize; idx++) {
                shellPrint(shell, "\n[%d]\n", idx);
                flash_list_show(shell, chipInfo);
                chipInfo++;
            }
            OSI_Free(flashChipInfo);
        }

        if (modeMask & FLASH_LIST_TYPE_DETECH) {
            FLASH_ChipInfo_t flashChipInfo;
            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_CHIP_INFO_DETECH, &flashChipInfo, sizeof(FLASH_ChipInfo_t));
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }
            shellPrint(shell, "\n-----------------------\n");
            shellPrint(shell, "auto detech chip info:\n");
            flash_list_show(shell, &flashChipInfo);
        }
    } while (0);

    return status;
}


int flash_speed_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
        if (pflashDev == NULL) {
            shellPrint(shell, "hal flash not init\n");
            return HAL_STATUS_ERROR;
        }

        uint32_t clkSrc = 0;
        uint32_t clkDiv = 0;
        uint32_t spiFreq = 0;
        uint32_t spiFormat = 0;
        status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_CLK_SRC, &clkSrc, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }

        status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_CLK_DIV, &clkDiv, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }

        status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_CLK_FREQ, &spiFreq, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }

        status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_SPI_FORMAT, &spiFormat, 4);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "fail %d\n", status);
            break;
        }

        shellPrint(shell, "flash ctrl speed:\n");
        shellPrint(shell, "clock src: %d %s\n", clkSrc, strClkSrc[clkSrc]);
        shellPrint(shell, "clock div: %d\n", clkDiv);
        shellPrint(shell, "clock freq: %d Hz (%d MHz)\n", spiFreq, spiFreq / 1000000);
        shellPrint(shell, "spi line width: %d %s\n", spiFormat, strSpiLineWidth[spiFormat]);

    } while (0);


    return status;
}


int flash_sleep_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
        if (pflashDev == NULL) {
            shellPrint(shell, "hal flash not init\n");
            return HAL_STATUS_ERROR;
        }

        uint32_t sleepState = 0;
        uint32_t sleepLatency = 0;
        uint32_t resumeLatency = 0;

        if (argc == 4) {
            sleepState = strtoul(argv[1], NULL, 0);
            sleepLatency = strtoul(argv[2], NULL, 0);
            resumeLatency = strtoul(argv[3], NULL, 0);

            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_SLEEP_STATE, &sleepState, 4);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_SET_SLEEP_LATENCY, &sleepLatency, 4);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_SET_RESUME_LATENCY, &resumeLatency, 4);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            shellPrint(shell, "set flash ctrl sleep:\n");
            shellPrint(shell, "sleep state: %d %s\n", sleepState, sleepState == 0 ? "dis": "ena");
            shellPrint(shell, "sleep latency: %d us\n", sleepLatency);
            shellPrint(shell, "resume latency: %d us\n", resumeLatency);

        } else {
            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_SET_SLEEP_STATE, &sleepState, 4);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_SLEEP_LATENCY, &sleepLatency, 4);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            status = HAL_FLASH_ExtControl(pflashDev, FLASH_EXT_GET_RESUME_LATENCY, &resumeLatency, 4);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "fail %d\n", status);
                break;
            }

            shellPrint(shell, "get flash ctrl sleep:\n");
            shellPrint(shell, "sleep state: %d %s\n", sleepState, sleepState == 0 ? "dis": "ena");
            shellPrint(shell, "sleep latency: %d us\n", sleepLatency);
            shellPrint(shell, "resume latency: %d us\n", resumeLatency);
        }
    } while (0);


    return status;
}




int flash_auto_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 4) {
            HAL_Dev_t * pflashDev = HAL_DEV_Find(HAL_DEV_TYPE_FLASH, 0);
            if (pflashDev == NULL) {
                shellPrint(shell, "hal flash not init\n");
                return HAL_STATUS_ERROR;
            }

            uint32_t addr = strtoul(argv[1], NULL, 0);
            uint32_t len = strtoul(argv[2], NULL, 0);
            uint32_t loop = strtoul(argv[3], NULL, 0);

            uint8_t *pWriteValue = NULL;
            uint8_t *pReadValue = NULL;
            pWriteValue = (uint8_t *)OSI_Malloc(len);
            if (pWriteValue == NULL) {
                shellPrint(shell, "malloc buff fail %d\n", status);
                return HAL_STATUS_NO_MEM;
            }
            pReadValue = (uint8_t *)OSI_Malloc(len);
            if (pReadValue == NULL) {
                shellPrint(shell, "malloc buff fail %d\n", status);
                OSI_Free(pWriteValue);
                return HAL_STATUS_NO_MEM;
            }


            while (loop) {
                shellPrint(shell, "\tauto test loop [%d]\n\r", loop);

                for (uint32_t idx = 0; idx < len; idx++) {
                    pWriteValue[idx] = loop;
                }

                uint32_t erase_len = 4096;
                uint32_t erase_addr = addr;
                for (uint32_t idx = 0; idx < len; idx += erase_len, erase_addr += erase_len) {
                    status = HAL_FLASH_Erase(pflashDev, erase_addr, erase_len);
                    if (status != HAL_STATUS_OK) {
                        shellPrint(shell, "erase flash fail %d\n", status);
                        break;
                    }
                }

                status = HAL_FLASH_Write(pflashDev, addr, pWriteValue, len);
                if (status != HAL_STATUS_OK) {
                    shellPrint(shell, "write flash fail %d\n", status);
                    break;
                }

                status = HAL_FLASH_Read(pflashDev, addr, pReadValue, len);
                if (status != HAL_STATUS_OK) {
                    shellPrint(shell, "read flash fail %d\n", status);
                    break;
                }

                for (uint32_t idx = 0; idx < len; idx++) {
                    if (pReadValue[idx] != pWriteValue[idx]) {
                        shellPrint(shell, "compare data fail: idx[%d] addr[0x%08X] read[%02X]-write[%02X]\n", 
                            idx, addr, pReadValue[idx], pWriteValue[idx]);
                        status = HAL_STATUS_ERROR;
                        break;
                    }
                }

                if (status != HAL_STATUS_OK) {
                    break;
                }

                loop--;
            }

            if (status == HAL_STATUS_OK) {
                shellPrint(shell, "auto test success\n\r");
            } else {
                shellPrint(shell, "auto test fail\n\r");
            }

            OSI_Free(pWriteValue);
            OSI_Free(pReadValue);
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "flash auto <addr> <size> <loop>\n\r");
        shellPrint(shell, "\t-addr: auto test start address\n\r");
        shellPrint(shell, "\t-size: auto test size\n\r");
        shellPrint(shell, "\t-loop: auto test loop counter\n\r");
    }


    return HAL_STATUS_OK;
}


ShellCommand flash_cmds[] = {
    SHELL_CMD_ITEM(read,        flash_read_cmd,         "<addr> <size>"),
    SHELL_CMD_ITEM(write,       flash_write_cmd,        "<addr> <data> <size>"),
    SHELL_CMD_ITEM(erase,       flash_erase_cmd,        "<addr> <size>"),
    SHELL_CMD_ITEM(open,        flash_open_cmd,         "<clk src> <freq> <line width> <psram enable> <use default list>"),
    SHELL_CMD_ITEM(close,       flash_close_cmd,        "close flash"),
    SHELL_CMD_ITEM(size,        flash_size_cmd,         "get flash size"),
    SHELL_CMD_ITEM(id,          flash_id_cmd,           "get flash chip id"),
    SHELL_CMD_ITEM(list,        flash_list_cmd,         "get flash support chip list"),
    SHELL_CMD_ITEM(speed,       flash_speed_cmd,        "get flash run speed"),
    SHELL_CMD_ITEM(sleep,       flash_sleep_cmd,        "<enable> <sleep latency> <resume latency>"),
    SHELL_CMD_ITEM(auto,        flash_auto_cmd,         "<addr> <size> <loop>"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(flash, flash_cmds, "flash command");


#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
