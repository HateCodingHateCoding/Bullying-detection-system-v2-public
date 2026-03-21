/**
 ******************************************************************************
 * @file    cmd_kvf.c
 * @brief   cmd kvf define.
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
#include "kvf.h"

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_KVF == 1) && (CONFIG_SHELL == 1))

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
extern void * cmdGetMemBuff(uint8_t id);
extern uint32_t cmdGetMemSize(uint8_t id);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int kvf_save_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 4) {
        uint8_t kvfId = strtoul(argv[1], NULL, 10);
        uint32_t size = (uint32_t)strtoul(argv[2], NULL, 10);
        uint8_t memId = strtoul(argv[3], NULL, 10);

        uint8_t *pbuff = cmdGetMemBuff(memId);
        uint8_t buffSize = cmdGetMemSize(memId);
        if (pbuff == NULL) {
            shellPrint(shell, "buff error\n");
            return HAL_STATUS_INVALID_PARAM;
        }
        if (buffSize < size) {
            shellPrint(shell, "size error %d < %d\n", buffSize, size);
            return HAL_STATUS_INVALID_PARAM;
        }

        int ret = KVF_Save(kvfId, pbuff, size);
        if (ret == size) {
            shellPrint(shell, "KVF save success\n");
        } else {
            shellPrint(shell, "KVF save fail %d\n", ret);
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int kvf_load_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 4) {
        uint8_t kvfId = strtoul(argv[1], NULL, 10);
        uint32_t size = (uint32_t)strtoul(argv[2], NULL, 10);
        uint8_t memId = strtoul(argv[3], NULL, 10);

        uint8_t *pbuff = cmdGetMemBuff(memId);
        uint8_t buffSize = cmdGetMemSize(memId);
        if (pbuff == NULL) {
            shellPrint(shell, "buff error\n");
            return HAL_STATUS_INVALID_PARAM;
        }
        if (buffSize < size) {
            shellPrint(shell, "size error %d < %d\n", buffSize, size);
            return HAL_STATUS_INVALID_PARAM;
        }

        int ret = KVF_Load(kvfId, pbuff, size);
        if (ret == size) {
            shellPrint(shell, "KVF load success\n");
        } else {
            shellPrint(shell, "KVF load fail %d\n", ret);
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int kvf_delete_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 2) {
        uint8_t kvfId = strtoul(argv[1], NULL, 10);

        int ret = KVF_Delete(kvfId);
        if (ret == 0) {
            shellPrint(shell, "KVF save success\n");
        } else {
            shellPrint(shell, "KVF save fail %d\n", ret);
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int kvf_loop_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 6) {
        uint8_t kvfId = strtoul(argv[1], NULL, 10);
        uint32_t size = (uint32_t)strtoul(argv[2], NULL, 10);
        uint8_t memSaveId = strtoul(argv[3], NULL, 10);
        uint8_t memLoadId = strtoul(argv[4], NULL, 10);
        uint32_t loop = strtoul(argv[5], NULL, 10);

        uint8_t *pSavebuff = cmdGetMemBuff(memSaveId);
        uint8_t saveSize = cmdGetMemSize(memSaveId);
        if (pSavebuff == NULL) {
            shellPrint(shell, "buff error\n");
            return HAL_STATUS_INVALID_PARAM;
        }
        if (saveSize < size) {
            shellPrint(shell, "size error %d < %d\n", saveSize, size);
            return HAL_STATUS_INVALID_PARAM;
        }

        uint8_t *pLoadbuff = cmdGetMemBuff(memLoadId);
        uint8_t loadSize = cmdGetMemSize(memLoadId);
        if (pLoadbuff == NULL) {
            shellPrint(shell, "buff error\n");
            return HAL_STATUS_INVALID_PARAM;
        }
        if (loadSize < size) {
            shellPrint(shell, "size error %d < %d\n", loadSize, size);
            return HAL_STATUS_INVALID_PARAM;
        }

        int ret = 0;
        uint32_t idx = 0;
        for (idx = 0; idx < loop; idx++) {
            shellPrint(shell, "KVF loop [%d]\n", idx);
            ret = KVF_Save(kvfId, pSavebuff, size);
            if (ret == size) {
                shellPrint(shell, "KVF save success\n");
            } else {
                shellPrint(shell, "KVF save fail %d\n", ret);
                break;
            }

            ret = KVF_Load(kvfId, pLoadbuff, size);
            if (ret == size) {
                shellPrint(shell, "KVF load success\n");
            } else {
                shellPrint(shell, "KVF load fail %d\n", ret);
                break;
            }

            if (memcmp(pSavebuff, pLoadbuff, size) != 0) {
                shellPrint(shell, "save and load value cmp error\n");
                break;
            }
        }

    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

ShellCommand kvf_cmds[] = {
    SHELL_CMD_ITEM(save,      kvf_save_cmd,     "<id> <size> <mem-id>"),
    SHELL_CMD_ITEM(load,      kvf_load_cmd,     "<id> <size> <mem-id>"),
    SHELL_CMD_ITEM(delete,    kvf_delete_cmd,   "<id>"),
    SHELL_CMD_ITEM(loop,      kvf_loop_cmd,     "<id> <size> <save-mem-id> <load-mem-id> <loop>"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(kvf, kvf_cmds, "key-value flash command");

#endif /* CONFIG_SHELL_CMD_KVF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
