/**
 ******************************************************************************
 * @file    cmd_efuse.c
 * @brief   cmd efuse define.
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

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_EFUSE == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
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

int efuse_read_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 3) {
            HAL_Dev_t * pefuseDev = HAL_DEV_Find(HAL_DEV_TYPE_EFUSE, 0);
            if (pefuseDev == NULL) {
                shellPrint(shell, "hal efuse not init\n");
                return HAL_STATUS_ERROR;
            }

            uint32_t bitStart = strtoul(argv[1], NULL, 0);
            uint32_t bitLen = strtoul(argv[2], NULL, 0);
            if ((bitStart + bitLen) >= 2048) {
                shellPrint(shell, "efuse size is 2048 bits\n");
                return HAL_STATUS_INVALID_PARAM;
            }

            uint32_t byteLen = bitLen / 8 + 4;
            uint8_t *pValue = NULL;
            if (byteLen > 64) {
                uint8_t cmdBuff[64];
                pValue = cmdBuff;
            } else {
                pValue = (uint8_t *)OSI_Malloc(byteLen);
                if (pValue == NULL) {
                    shellPrint(shell, "malloc buff fail %d\n", status);
                    return HAL_STATUS_NO_MEM;
                }
            }

            for (uint32_t idx = 0; idx < byteLen; idx++) {
                pValue[idx] = 0;
            }

            status = HAL_EFUSE_Read(pefuseDev, bitStart, bitLen, pValue);
            if (status != HAL_STATUS_OK) {
                shellPrint(shell, "read efuse fail %d\n", status);
                return status;
            }

            uint32_t showLen = bitLen / 8;
            if ((bitLen % 8) > 0) {
                showLen += 1;
            }
            for (uint32_t idx = 0; idx < showLen; idx++) {
                if ((idx % 16) == 0) {
                    shellPrint(shell, "\n%4d :", idx);
                }
                shellPrint(shell, " %02X", pValue[idx]);
            }
        }else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "efuse read <start-bit> <len-bit>");
        shellPrint(shell, "\tstart-bit/len-bit < 2048");
    }


    return HAL_STATUS_OK;
}

int efuse_write_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 4) {
            HAL_Dev_t * pefuseDev = HAL_DEV_Find(HAL_DEV_TYPE_EFUSE, 0);
            if (pefuseDev == NULL) {
                shellPrint(shell, "hal efuse not init\n");
                return HAL_STATUS_ERROR;
            }

            uint32_t bitStart = strtoul(argv[1], NULL, 0);
            uint32_t bitLen = strtoul(argv[2], NULL, 0);
            if ((bitStart >= 2048) || (bitLen > 32)) {
                status = HAL_STATUS_INVALID_PARAM;
            }

            uint32_t writeValue = strtoul(argv[3], NULL, 0);

            status = HAL_EnableWrite(pefuseDev);
            if (status != HAL_STATUS_OK) {
                status = HAL_STATUS_ERROR;
                break;
            }

            status = HAL_EFUSE_Write(pefuseDev, bitStart, bitLen, (uint8_t *)&writeValue);
            if (status != HAL_STATUS_OK) {
                HAL_DisableWrite(pefuseDev);
                shellPrint(shell, "write efuse fail %d\n", status);
                return status;
            }

            status = HAL_DisableWrite(pefuseDev);
            if (status != HAL_STATUS_OK) {
                status = HAL_STATUS_ERROR;
                break;
            }

        }else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "efuse write <start-bit> <len-bit> <value>");
        shellPrint(shell, "\tstart-bit: 0 ~ 2048");
        shellPrint(shell, "\tlen-bit: 0 ~ 32");
    }


    return status;
}


int efuse_chipid_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;
    HAL_Dev_t * pefuseDev = HAL_DEV_Find(HAL_DEV_TYPE_EFUSE, 0);
    if (pefuseDev == NULL) {
        shellPrint(shell, "hal efuse not init\n");
        return HAL_STATUS_ERROR;
    }

    uint8_t chipId[18];
    status = HAL_EFUSE_GetChipId(pefuseDev, chipId, 17);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "get chipid fail %d\n", status);
        return status;
    }

    for (uint32_t idx = 0; idx < 17; idx++) {
        shellPrint(shell, "%02X ", chipId[idx]);
    }
    shellPrint(shell, "\n\r");

    return status;
}

ShellCommand efuse_cmds[] = {
    SHELL_CMD_ITEM(read,      efuse_read_cmd,     "<start-bit> <len-bit>"),
    SHELL_CMD_ITEM(write,     efuse_write_cmd,    "<start-bit> <len-bit> <value>"),
    SHELL_CMD_ITEM(chipid,    efuse_chipid_cmd,   "get chip id"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(efuse, efuse_cmds, "efuse command");

#endif /* CONFIG_SHELL_CMD_EFUSE */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
