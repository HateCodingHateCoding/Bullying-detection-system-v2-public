/**
 ******************************************************************************
 * @file    cmd_wkio.c
 * @brief   cmd wkio define.
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
#include "hal_wkio.h"

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_WKIO == 1) && (CONFIG_SHELL == 1))

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
static HAL_Dev_t * wkioDev = NULL;

#if (CONFIG_PM == 1)

struct wakelock pm_lock_wkio;

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int pm_unlock_cmd(Shell *shell, int argc, char *argv[])
{
    pm_policy_wake_unlock(&pm_lock_wkio);
    return HAL_STATUS_OK;
}
#endif


int set_modetype_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_PinMode_t mode;

    if(argc < 4) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "gpio")) {
        mode = HAL_WKIO_MODE_GPIO;
    } else if (!strcmp(argv[3], "hold")) {
        mode = HAL_WKIO_MODE_HOLD;
    } else if (!strcmp(argv[3], "wake")) {
        mode = HAL_WKIO_MODE_WAKE;
    } else if (!strcmp(argv[3], "disa")) {
        mode = HAL_WKIO_MODE_DISA;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_WKIO_SetPinMode(wkioDev, pin, mode);
    return status;
}

int get_modetype_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_PinMode_t mode;

    if(argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_WKIO_GetPinMode(wkioDev, pin, &mode);

    shellPrint(shell, "wkio %s %s mode: %d\r",
                argv[1],
                argv[2],
                mode);

    return HAL_STATUS_OK;
}

int hold_write_pin_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_PinState_t out;

    if(argc < 4) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    out = atoi(argv[3]);
    if (out == 1){
        pin = atoi(argv[2]);
        if (pin > 17 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_WKIO_WritePin(wkioDev, pin, WKIO_PIN_SET);
        return status;
    } else if (out == 0){
        pin = atoi(argv[2]);
        if (pin > 17 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_WKIO_WritePin(wkioDev, pin, WKIO_PIN_RESET);
        return status;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }
}

int hold_write_port_cmd(Shell *shell, int argc, char *argv[])
{
    uint32_t portMask;

    if(argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    portMask = strtol(argv[2], NULL, 2);

    HAL_Status_t status = HAL_WKIO_WritePort(wkioDev, portMask);
    return status;
}

void CMD_WKIO_EXTI_Callback(void *arg)
{
    printf("\nwkio_irq_trigger \n");
    HAL_WKIO_DisableWake(wkioDev);
#if (CONFIG_PM == 1)
    pm_policy_wake_lock(&pm_lock_wkio);
#endif
}

int init_wake_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_WakeParam_t wkioIrq;

    if(argc < 4) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "rising")) {
        wkioIrq.event = WKIO_WAKE_EVT_RISING_EDGE;
    } else if (!strcmp(argv[3], "falling")) {
        wkioIrq.event = WKIO_WAKE_EVT_FALLING_EDGE;
    } else if (!strcmp(argv[3], "both")) {
        wkioIrq.event = WKIO_WAKE_EVT_BOTH_EDGE;
    } else if (!strcmp(argv[3], "high")) {
        wkioIrq.event = WKIO_WAKE_EVT_HIGH_LEVEL;
    } else if (!strcmp(argv[3], "low")) {
        wkioIrq.event = WKIO_WAKE_EVT_LOW_LEVEL;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    wkioIrq.pull = HAL_WKIO_PULL_FLOATING;
    wkioIrq.callback = CMD_WKIO_EXTI_Callback;
    wkioIrq.arg =  NULL;

    HAL_WKIO_EnableWake(wkioDev);
    HAL_Status_t status = HAL_WKIO_SetWakeParam(wkioDev, pin, &wkioIrq);

    return status;
}

int set_pull_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_PinPull_t pull;

    if(argc < 4) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "no")) {
        pull = HAL_WKIO_PULL_FLOATING;
    } else if (!strcmp(argv[3], "up")) {
        pull = HAL_WKIO_PULL_UP;
    } else if (!strcmp(argv[3], "down")) {
        pull = HAL_WKIO_PULL_DOWN;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_WKIO_SetPinPull(wkioDev, pin, pull);
    return status;
}

int get_wkio_pull_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_PinPull_t pull;

    if(argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_WKIO_GetPinPull(wkioDev, pin, &pull);

    shellPrint(shell, "gpio %s %s pull: %d\r",
                argv[1],
                argv[2],
                pull);

    return HAL_STATUS_OK;
}

int enable_wake_cmd(Shell *shell, int argc, char *argv[])
{
    if(argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_WKIO_EnableWake(wkioDev);
    return status;
}

int disable_wake_cmd(Shell *shell, int argc, char *argv[])
{
    if(argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_WKIO_DisableWake(wkioDev);
    return status;
}

int set_waketype_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_WakeEvent_t waketype;

    if(argc < 4) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "rising")) {
        waketype = WKIO_WAKE_EVT_RISING_EDGE;
    } else if (!strcmp(argv[3], "falling")) {
        waketype = WKIO_WAKE_EVT_FALLING_EDGE;
    } else if (!strcmp(argv[3], "both")) {
        waketype = WKIO_WAKE_EVT_BOTH_EDGE;
    } else if (!strcmp(argv[3], "high")) {
        waketype = WKIO_WAKE_EVT_HIGH_LEVEL;
    } else if (!strcmp(argv[3], "low")) {
        waketype = WKIO_WAKE_EVT_LOW_LEVEL;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_WKIO_SetWakeEvent(wkioDev, pin, waketype);
    return status;
}

int get_waketype_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_WakeEvent_t waketype;

    if(argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_WKIO_GetWakeEvent(wkioDev, pin, &waketype);

    shellPrint(shell, "gpio %s %s waketype: %d\r",
                argv[1],
                argv[2],
                waketype);

    return HAL_STATUS_OK;
}

int read_pin_value_cmd(Shell *shell, int argc, char *argv[])
{
    WKIO_Pin_t pin;
    WKIO_PinState_t input;

    if(argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 17 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    input = HAL_WKIO_ReadWakePin(wkioDev, pin);

    shellPrint(shell, "gpio %s %s input: %d\r",
                argv[1],
                argv[2],
                input);

    return HAL_STATUS_OK;
}

int set_deb_cmd(Shell *shell, int argc, char *argv[])
{
    uint32_t debounce;

    if(argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        wkioDev = HAL_WKIO_Init(WKIO_PORT_A);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    debounce = atoi(argv[2]);

    HAL_Status_t status = HAL_WKIO_SetWakeDebounce(wkioDev, debounce);
    return status;
}

int wkio_version_cmd(Shell *shell, int argc, char *argv[])
{
    shellPrint(shell, "wkio version:1.0 \n");
    return HAL_STATUS_OK;
}

ShellCommand wkio_cmds[] = {
    SHELL_CMD_ITEM(set_modetype,      set_modetype_cmd,     "<port> <pin> <mode> (gpio/hold/wake/disa)"),
    SHELL_CMD_ITEM(get_modetype,      get_modetype_cmd,     "<port> <pin>"),
    SHELL_CMD_ITEM(hold_write_pin,    hold_write_pin_cmd,   "<port> <pin> <pin-value>"),
    SHELL_CMD_ITEM(hold_write_port,   hold_write_port_cmd,  "<port> <value-mask>"),
    SHELL_CMD_ITEM(init_wake,         init_wake_cmd,        "<port> <pin> <event> (rising/falling/both/high/low)"),
    SHELL_CMD_ITEM(set_pull,          set_pull_cmd,         "<port> <pin> <pull> (no/up/down)"),
    SHELL_CMD_ITEM(get_wkio_pull,     get_wkio_pull_cmd,    "<port> <pin>"),
    SHELL_CMD_ITEM(enable_wake,       enable_wake_cmd,      "<port>"),
    SHELL_CMD_ITEM(disable_wake,      disable_wake_cmd,     "<port>"),
    SHELL_CMD_ITEM(set_waketype,      set_waketype_cmd,     "<port> <pin> <event> (rising/falling/both/high/low)"),
    SHELL_CMD_ITEM(get_waketype,      get_waketype_cmd,     "<port> <pin>"),
    SHELL_CMD_ITEM(read_pin_value,    read_pin_value_cmd,   "<port> <pin>"),
    SHELL_CMD_ITEM(set_deb,           set_deb_cmd,          "<port> <debounce>"),
    SHELL_CMD_ITEM(wkio_version,      wkio_version_cmd,     ""),

#if (CONFIG_PM == 1)
    SHELL_CMD_ITEM(pm_unlock,         pm_unlock_cmd,        ""),
#endif
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(wkio, wkio_cmds, "wkio command");

#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
