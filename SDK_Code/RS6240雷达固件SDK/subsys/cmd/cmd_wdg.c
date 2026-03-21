/**
 ******************************************************************************
 * @file    cmd_wdg.c
 * @brief   cmd wdg define.
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
#include "hal_wdg.h"

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_WDG == 1) && (CONFIG_SHELL == 1))

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
int wdg_init_cmd(HAL_Dev_t * pDevice, void *arg)
{
    HAL_Dev_t *pWdgDevice = HAL_WDG_Init(WDG_DEFT_ID);
    if (pWdgDevice != NULL) {
        return HAL_STATUS_OK;
    } else {
        return HAL_STATUS_ERROR;
    }
}

int wdg_deinit_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;
    HAL_Dev_t *pWdgDevice = (HAL_Dev_t *)HAL_DEV_Find(HAL_DEV_TYPE_WDG, WDG_DEFT_ID);
    if (pWdgDevice == NULL) {
        return HAL_STATUS_INVALID_STATE;
    }

    status = HAL_WDG_DeInit(pWdgDevice);

    return status;
}

static void wdg_cmd_callback(HAL_Dev_t * pDevice, void *arg)
{
    HAL_WDG_Feed(pDevice);

    printf("wdg feed..\n");
}

int wdg_open_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 3) {
            WDG_Mode_t wdg_mode;
            uint32_t wdg_cnt;
            
            wdg_cnt = atoi(argv[1]);
            
            if (!strcmp(argv[2], "cpu") || !strcmp(argv[2], "CPU")) {
                wdg_mode = WDG_MODE_RESET_CPU;
            } else if (!strcmp(argv[2], "sys") || !strcmp(argv[2], "SYS")) {
                wdg_mode = WDG_MODE_RESET_SYS;
            } else if (!strcmp(argv[2], "irq") || !strcmp(argv[2], "IRQ")) {
                wdg_mode = WDG_MODE_INTERRUPT;
            } else {
                return HAL_STATUS_INVALID_PARAM;
            }
            
            HAL_Dev_t *pWdgDevice = HAL_DEV_Find(HAL_DEV_TYPE_WDG, WDG_DEFT_ID);
            if (pWdgDevice == NULL) {
                return HAL_STATUS_INVALID_STATE;
            }
            
            WDG_InitParam_t initParam = {
                .mode       = wdg_mode,
                .callback   = wdg_cmd_callback,
                .timeout    = wdg_cnt,
            };
            
            status = HAL_WDG_Open((HAL_Dev_t *)pWdgDevice, &initParam);
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "wdg open <timeout> <mode>\n");
        shellPrint(shell, "\t-timeout ms\n");
        shellPrint(shell, "\t-mode: sys/cpu/irq\n");
    }


    return status;
}

int wdg_close_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;
    HAL_Dev_t *pWdgDevice = HAL_DEV_Find(HAL_DEV_TYPE_WDG, WDG_DEFT_ID);
    if (pWdgDevice != NULL) {
        status = HAL_WDG_Close(pWdgDevice);
    } else {
        status = HAL_STATUS_INVALID_STATE;
    }

    return status;
}

int wdg_feed_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;
    HAL_Dev_t *pWdgDevice = HAL_DEV_Find(HAL_DEV_TYPE_WDG, WDG_DEFT_ID);
    if (pWdgDevice != NULL) {
        status = HAL_WDG_Feed(pWdgDevice);
    } else {
        status = HAL_STATUS_INVALID_STATE;
    }

    return status;
}

ShellCommand wdg_cmds[] = {
    SHELL_CMD_ITEM(init,        wdg_init_cmd,           "init watchdog"),
    SHELL_CMD_ITEM(deinit,      wdg_deinit_cmd,         "deinit watchdog"),
    SHELL_CMD_ITEM(open,        wdg_open_cmd,           "open watchdog <timeout> <mode>"),
    SHELL_CMD_ITEM(close,       wdg_close_cmd,          "close watchdog"),
    SHELL_CMD_ITEM(feed,        wdg_feed_cmd,           "feed watchdog"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(wdg, wdg_cmds, "wdg command");


#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

