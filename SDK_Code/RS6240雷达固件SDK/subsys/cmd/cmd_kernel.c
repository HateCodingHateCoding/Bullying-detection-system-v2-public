/**
 ******************************************************************************
 * @file    cmd_kernel.c
 * @brief   cmd kernel define.
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


#if ((CONFIG_SHELL_CMD_KERNEL == 1) && (CONFIG_SHELL == 1))

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
#define CMD_KERNEL_TASK_MAX                         12

/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */
char * strTaskState[] = {
    "Running",
    "Ready",
    "Blocked",
    "Suspended",
    "Deleted",
    "Invalid",
};

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */


int kernel_all_cmd(Shell *shell, int argc, char *argv[])
{
#if (configUSE_TRACE_FACILITY == 1)
    UBaseType_t uxArraySize = uxTaskGetNumberOfTasks();

    if (uxArraySize > CMD_KERNEL_TASK_MAX) {
        shellPrint(shell, "current stask number : %d, Get %d Task\n", uxArraySize, CMD_KERNEL_TASK_MAX);
        uxArraySize = CMD_KERNEL_TASK_MAX;
    }

    TaskStatus_t cmdKernel[CMD_KERNEL_TASK_MAX];
    uint32_t pulTotalRunTime = 0;
    TaskStatus_t * pxTaskStatusArray = cmdKernel;
    uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &pulTotalRunTime);
    shellPrint(shell, "time(us): %d\n\r", pulTotalRunTime);
    shellPrint(shell, "%8s %16s %12s %12s %12s %12s %12s %12s\n\r",
        "id",
        "name",
        "state",
        "curr-prio",
        "base-prio",
        "time(us)",
        "use-stack",
        "stack");

    for (uint32_t i = 0; i < uxArraySize; i++) {
        shellPrint(shell, "%8d %16s %12s %12d %12d %12d %12d     %p\n\r",
            pxTaskStatusArray->xTaskNumber,
            pxTaskStatusArray->pcTaskName,
            strTaskState[pxTaskStatusArray->eCurrentState],
            pxTaskStatusArray->uxCurrentPriority,
            pxTaskStatusArray->uxBasePriority,
            pxTaskStatusArray->ulRunTimeCounter,
            pxTaskStatusArray->usStackHighWaterMark,
            pxTaskStatusArray->pxStackBase);

       pxTaskStatusArray++;
    }
#else
    shellPrint(shell, "must enable configUSE_TRACE_FACILITY");
#endif
    return HAL_STATUS_OK;
}

ShellCommand kernel_cmds[] = {
    SHELL_CMD_ITEM(all,       kernel_all_cmd,     "Get kernel all task status"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(kernel, kernel_cmds, "kernel command");

#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
