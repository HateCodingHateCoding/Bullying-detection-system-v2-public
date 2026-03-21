/**
 ******************************************************************************
 * @file    cmd_mem.c
 * @brief   cmd mem define.
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

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_MEM == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
typedef struct {
    void * pbuff;
    uint32_t size;
} cmd_mem_t;

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define CMD_MEM_CNT                         3

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
static cmd_mem_t cmdMem[CMD_MEM_CNT];

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

void * cmdGetMemBuff(uint8_t id)
{
    if (id >= CMD_MEM_CNT) {
        return NULL;
    }

    return cmdMem[id].pbuff;
}

uint32_t cmdGetMemSize(uint8_t id)
{
    if (id >= CMD_MEM_CNT) {
        return 0;
    }

    return cmdMem[id].size;
}

int mem_read_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t width = 0;
    uint32_t addr = 0;

    if (argc == 2) {
        width = 32;
        addr = strtoul(argv[1], NULL, 0);
    } else if (argc == 3) {
        width = strtoul(argv[1], NULL, 10);
        addr = strtoul(argv[2], NULL, 16);
    } else {
        return HAL_STATUS_INVALID_PARAM;;
    }

    if (addr == 0) {
        shellPrint(shell, "address is NULL\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    if (width == 8) {
        shellPrint(shell, "read 0x%08X : 0x%02X (%d)\n", addr, *(uint8_t *)addr, *(uint8_t *)addr);
    } else if (width == 16) {
        shellPrint(shell, "read 0x%08X : 0x%04X (%d)\n", addr, *(uint16_t *)addr, *(uint16_t *)addr);
    } else if (width == 32) {
        shellPrint(shell, "read 0x%08X : 0x%08X (%d)\n", addr, *(uint32_t *)addr, *(uint32_t *)addr);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }


    return HAL_STATUS_OK;
}

int mem_write_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t width = 0;
    uint32_t addr = 0;
    uint32_t value = 0;

    if (argc == 3) {
        width = 32;
        addr = strtoul(argv[1], NULL, 0);
        value = strtoul(argv[2], NULL, 0);
    } else if (argc == 4) {
        width = strtoul(argv[1], NULL, 10);
        addr = strtoul(argv[2], NULL, 16);
        value = strtoul(argv[3], NULL, 0);
    } else {
        return HAL_STATUS_INVALID_PARAM;;
    }

    if (addr == 0) {
        shellPrint(shell, "address is NULL\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    if (width == 8) {
        *(uint8_t *)addr = (uint8_t)value;
        shellPrint(shell, "write 0x%08 : 0x%02X (%d)\n", addr, value, value);
    } else if (width == 16) {
        *(uint16_t *)addr = (uint16_t)value;
        shellPrint(shell, "write 0x%08 : 0x%04X (%d)\n", addr, value, value);
    } else if (width == 32) {
        *(uint32_t *)addr = (uint32_t)value;
        shellPrint(shell, "write 0x%08 : 0x%08X (%d)\n", addr, value, value);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }


    return HAL_STATUS_OK;

}

int mem_set_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 4) {
        uint8_t id = strtoul(argv[1], NULL, 10);
        if (id  >= CMD_MEM_CNT) {
            shellPrint(shell, "buff id overflow\n");
            return HAL_STATUS_INVALID_STATE;
        }
        if (cmdMem[id].pbuff != NULL) {
            uint8_t * setAddr = (uint8_t *)strtoul(argv[1], NULL, 16);
            uint32_t size = (uint32_t)strtoul(argv[2], NULL, 10);

            if (cmdMem[id].size < size) {
                shellPrint(shell, "size error %d < %d\n", cmdMem[id].size, size);
                return HAL_STATUS_INVALID_PARAM;
            }
            uint8_t * ptmpBuff = cmdMem[id].pbuff;
            for (uint32_t idx = 0; idx < size; idx++) {
                setAddr[idx] = ptmpBuff[idx];
            }

        } else {
            shellPrint(shell, "buffer already free\n");
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;

}

int mem_get_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 4) {
        uint8_t id = strtoul(argv[1], NULL, 10);
        if (id  >= CMD_MEM_CNT) {
            shellPrint(shell, "buff id overflow\n");
            return HAL_STATUS_INVALID_STATE;
        }
        if (cmdMem[id].pbuff != NULL) {
            uint8_t * getAddr = (uint8_t *)strtoul(argv[2], NULL, 16);
            uint32_t size = (uint32_t)strtoul(argv[3], NULL, 10);

            if (cmdMem[id].size < size) {
                shellPrint(shell, "size error %d < %d\n", cmdMem[id].size, size);
                return HAL_STATUS_INVALID_PARAM;
            }

            uint8_t * ptmpBuff = cmdMem[id].pbuff;
            for (uint32_t idx = 0; idx < size; idx++) {
                ptmpBuff[idx] = getAddr[idx];
            }

        } else {
            shellPrint(shell, "buffer already free\n");
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int mem_dump_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 4) {
        uint8_t id = strtoul(argv[1], NULL, 10);
        if (id  >= CMD_MEM_CNT) {
            shellPrint(shell, "buff id overflow\n");
            return HAL_STATUS_INVALID_STATE;
        }
        if (cmdMem[id].pbuff != NULL) {
            uint8_t width = strtoul(argv[2], NULL, 10);
            uint32_t size = (uint32_t)strtoul(argv[3], NULL, 10);

            if (width == 8) {
                if (cmdMem[id].size < size) {
                    shellPrint(shell, "size error %d < %d\n", cmdMem[id].size, size);
                    return HAL_STATUS_INVALID_PARAM;
                }
                uint8_t *fillb = (uint8_t *)cmdMem[id].pbuff;
                for (uint32_t idx = 0; idx < size; idx++) {
                    if ((idx % 32) == 0) {
                        shellPrint(shell, "%08X : ", idx);
                    }
                    shellPrint(shell, "%02X ", fillb[idx]);
                    if ((idx % 32) == 31) {
                        shellPrint(shell, "\n\r");
                    }
                }
            } else if (width == 16) {
                if ((cmdMem[id].size / 2) < size) {
                    shellPrint(shell, "size error %d < %d\n", cmdMem[id].size / 2, size);
                    return HAL_STATUS_INVALID_PARAM;
                }
                uint16_t *fillh = (uint16_t *)cmdMem[id].pbuff;
                for (uint32_t idx = 0; idx < size; idx++) {
                    if ((idx % 16) == 0) {
                        shellPrint(shell, "%08X : ", idx);
                    }
                    shellPrint(shell, "%04X ", fillh[idx]);
                    if ((idx % 16) == 15) {
                        shellPrint(shell, "\n\r");
                    }
                }
            } else if (width == 32) {
                if ((cmdMem[id].size / 4) < size) {
                    shellPrint(shell, "size error %d < %d\n", cmdMem[id].size / 4, size);
                    return HAL_STATUS_INVALID_PARAM;
                }
                uint32_t *fillw = (uint32_t *)cmdMem[id].pbuff;
                for (uint32_t idx = 0; idx < size; idx++) {
                    if ((idx % 8) == 0) {
                        shellPrint(shell, "%08X : ", idx);
                    }
                    shellPrint(shell, "%08X ", fillw[idx]);
                    if ((idx % 8) == 7) {
                        shellPrint(shell, "\n\r");
                    }
                }
            } else {
                return HAL_STATUS_INVALID_PARAM;
            }

        } else {
            shellPrint(shell, "buffer already free\n");
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int mem_fill_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 6) {
        uint8_t id = strtoul(argv[1], NULL, 10);
        if (id  >= CMD_MEM_CNT) {
            shellPrint(shell, "buff id overflow\n");
            return HAL_STATUS_INVALID_STATE;
        }
        if (cmdMem[id].pbuff != NULL) {
            uint8_t width = strtoul(argv[2], NULL, 10);
            int step = (int)strtoul(argv[3], NULL, 10);
            uint32_t size = (uint32_t)strtoul(argv[4], NULL, 10);
            uint32_t initValue = (uint32_t)strtoul(argv[5], NULL, 10);

            if (width == 8) {
                if (cmdMem[id].size < size) {
                    shellPrint(shell, "size error %d < %d\n", cmdMem[id].size, size);
                    return HAL_STATUS_INVALID_PARAM;
                }
                uint8_t *fillb = (uint8_t *)cmdMem[id].pbuff;
                for (uint32_t idx = 0; idx < size; idx++) {
                    fillb[idx] = (uint8_t)(initValue + (step) * idx);
                }
            } else if (width == 16) {
                if ((cmdMem[id].size / 2) < size) {
                    shellPrint(shell, "size error %d < %d\n", cmdMem[id].size / 2, size);
                    return HAL_STATUS_INVALID_PARAM;
                }
                uint16_t *fillh = (uint16_t *)cmdMem[id].pbuff;
                for (uint32_t idx = 0; idx < size; idx++) {
                    fillh[idx] = (uint16_t)(initValue + (step) * idx);
                }
            } else if (width == 32) {
                if ((cmdMem[id].size / 4) < size) {
                    shellPrint(shell, "size error %d < %d\n", cmdMem[id].size / 4, size);
                    return HAL_STATUS_INVALID_PARAM;
                }
                uint32_t *fillw = (uint32_t *)cmdMem[id].pbuff;
                for (uint32_t idx = 0; idx < size; idx++) {
                    fillw[idx] = (uint32_t)(initValue + (step) * idx);
                }
            } else {
                return HAL_STATUS_INVALID_PARAM;
            }

        } else {
            shellPrint(shell, "buffer already free\n");
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int mem_alloc_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 3) {
        uint8_t id = strtoul(argv[1], NULL, 10);
        if (id  >= CMD_MEM_CNT) {
            shellPrint(shell, "buff id overflow\n");
            return HAL_STATUS_INVALID_STATE;
        }
        if (cmdMem[id].pbuff != NULL) {
            shellPrint(shell, "This is an unreleased buffer. %p\n", cmdMem[id].pbuff);
            return HAL_STATUS_INVALID_STATE;
        } else {
            uint32_t allocSize = strtoul(argv[2], NULL, 10);
            cmdMem[id].pbuff = OSI_Malloc(allocSize);
            if (cmdMem[id].pbuff != NULL) {
                cmdMem[id].size = allocSize;
                shellPrint(shell, "Malloc success: %p\n", cmdMem[id].pbuff);
            } else {
                shellPrint(shell, "Malloc fail: %p\n", cmdMem[id].pbuff);
                return HAL_STATUS_ERROR;
            }
        }
    } else if (argc == 2) {
        uint8_t id = strtoul(argv[1], NULL, 10);
        if (id  >= CMD_MEM_CNT) {
            shellPrint(shell, "buff id overflow\n");
            return HAL_STATUS_INVALID_STATE;
        }
        shellPrint(shell, "Current buffer: %p, size %d\n", cmdMem[id].pbuff, cmdMem[id].size);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int mem_free_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 2) {
        uint8_t id = strtoul(argv[1], NULL, 10);
        if (id  >= CMD_MEM_CNT) {
            shellPrint(shell, "buff id overflow\n");
            return HAL_STATUS_INVALID_STATE;
        }

        if (cmdMem[id].pbuff != NULL) {
            shellPrint(shell, "Free buffer success. %p\n", cmdMem[id].pbuff);
            OSI_Free(cmdMem[id].pbuff);
            cmdMem[id].pbuff = NULL;
            cmdMem[id].size = 0;
        } else {
            shellPrint(shell, "The allocted buffer has been released\n");
            return HAL_STATUS_INVALID_STATE;
        }
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

ShellCommand mem_cmds[] = {
    SHELL_CMD_ITEM(read,      mem_read_cmd,     "[width] <addr>; width-8/16/32 bits"),
    SHELL_CMD_ITEM(write,     mem_write_cmd,    "[width] <addr> <value>; width-8/16/32 bits"),
    SHELL_CMD_ITEM(set,       mem_set_cmd,      "<addr> <buff-id> <size>"),
    SHELL_CMD_ITEM(get,       mem_get_cmd,      "<addr> <buff-id> <size>"),
    SHELL_CMD_ITEM(dump,      mem_dump_cmd,     "<buff-id> <width> <size>; width-8/16/32 bits"),
    SHELL_CMD_ITEM(fill,      mem_fill_cmd,     "<buff-id> <width> <step> <size> <init-value>; width-8/16/32"),
    SHELL_CMD_ITEM(malloc,    mem_alloc_cmd,    "<buff-id> [size]"),
    SHELL_CMD_ITEM(free,      mem_free_cmd,     "<buff-id> free buffer"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(mem, mem_cmds, "memory command");

#endif /* CONFIG_SHELL_CMD_MEM */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
