/**
 ******************************************************************************
 * @file    cmd_gpio.c
 * @brief   cmd gpio define.
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
#include "hal_gpio.h"
#include "irq.h"
#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_GPIO == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define GPIO_SHELL_TEST_PIN                              11

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static HAL_Dev_t *gpioDev = NULL;
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int set_qcfg_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_PinParam_t gpioInitParam;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "out")) {
        gpioInitParam.mode = GPIOx_Pn_F1_OUTPUT;
        gpioInitParam.pull = GPIO_PULL_FLOATING;
        gpioInitParam.push = GPIO_PUSH_PULL;
        gpioInitParam.driving = GPIO_DRIVING_LEVEL_1;

        pin = atoi(argv[2]);
        if (pin > 15 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_GPIO_SetPinParam(gpioDev, pin, &gpioInitParam);
        return status;
    } else if (!strcmp(argv[3], "in")) {
        gpioInitParam.mode = GPIOx_Pn_F0_INPUT;
        gpioInitParam.pull = GPIO_PULL_FLOATING;
        gpioInitParam.driving = GPIO_DRIVING_LEVEL_1;

        pin = atoi(argv[2]);
        if (pin > 15 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_GPIO_SetPinParam(gpioDev, pin, &gpioInitParam);
        return status;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }
}

int set_config_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t pin;
    GPIO_PinParam_t gpioInitParam;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "out")) {
        if (!strcmp(argv[4], "push")) {
            gpioInitParam.push = GPIO_PUSH_PULL;
        } else if (!strcmp(argv[4], "open")) {
            gpioInitParam.push = GPIO_OPEN_DRAIN;
        } else {
            return HAL_STATUS_INVALID_PARAM;
        }

        gpioInitParam.mode = GPIOx_Pn_F1_OUTPUT;
        gpioInitParam.pull = GPIO_PULL_FLOATING;
        gpioInitParam.driving = atoi(argv[6]);

        pin = atoi(argv[2]);
        if (pin > 15 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_GPIO_SetPinParam(gpioDev, pin, &gpioInitParam);
        return status;
    } else if (!strcmp(argv[3], "in")) {
        if (!strcmp(argv[5], "up")) {
            gpioInitParam.pull = GPIO_PULL_UP;
        } else if (!strcmp(argv[5], "no")) {
            gpioInitParam.pull = GPIO_PULL_FLOATING;
        } else if (!strcmp(argv[5], "down")) {
            gpioInitParam.pull = GPIO_PULL_DOWN;
        } else {
            return HAL_STATUS_INVALID_PARAM;
        }

        gpioInitParam.mode = GPIOx_Pn_F0_INPUT;
        gpioInitParam.driving = atoi(argv[6]);

        pin = atoi(argv[2]);
        if (pin > 15 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_GPIO_SetPinParam(gpioDev, pin, &gpioInitParam);
        return status;
    }else {
        return HAL_STATUS_INVALID_PARAM;
    }
}

int get_config_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_PinParam_t gpioParam;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_GetPinParam(gpioDev, pin, &gpioParam);
    shellPrint(shell, "gpio %s %s mode: %d push: %d pull: %d driving: %d\r",
                argv[1],
                argv[2],
                gpioParam.mode,
                gpioParam.pull,
                gpioParam.push,
                gpioParam.driving);

    return status;
}

int get_mode_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_PinParam_t gpioParam;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_GetPinParam(gpioDev, pin, &gpioParam);
    shellPrint(shell, "gpio %s %s mode: %d\r",
                argv[1],
                argv[2],
                gpioParam.mode);

    return status;
}

int get_pull_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_PinParam_t gpioParam;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_GetPinParam(gpioDev, pin, &gpioParam);
    shellPrint(shell, "gpio %s %s pull: %d\r",
                argv[1],
                argv[2],
                gpioParam.pull);

    return status;
}

int get_push_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_PinParam_t gpioParam;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_GetPinParam(gpioDev, pin, &gpioParam);
    shellPrint(shell, "gpio %s %s push: %d\r",
                argv[1],
                argv[2],
                gpioParam.push);

    return status;
}

int get_driving_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_PinParam_t gpioParam;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_GetPinParam(gpioDev, pin, &gpioParam);
    shellPrint(shell, "gpio %s %s driving: %d\r",
                argv[1],
                argv[2],
                gpioParam.driving);

    return status;
}

int read_port_cmd(Shell *shell, int argc, char *argv[])
{
    uint32_t in;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    in = HAL_GPIO_ReadPort(gpioDev);

    shellPrint(shell, "gpio %s get:", argv[1]);
    for (int i = 15; i >= 0; i--) {
        if (in & (1 << i)){
            shellPrint(shell,"1");
        } else {
            shellPrint(shell,"0");
        }
    }

    return HAL_STATUS_OK;
}


int read_pin_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t pin, in;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    in = HAL_GPIO_ReadPin(gpioDev, pin);

    shellPrint(shell, "gpio %s %s get: %d\r",
                argv[1],
                argv[2],
                in);

    return HAL_STATUS_OK;
}

int write_port_cmd(Shell *shell, int argc, char *argv[])
{
    uint32_t portMask;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    portMask = strtol(argv[2], NULL, 2);

    HAL_Status_t status = HAL_GPIO_WritePort(gpioDev, portMask);
    return status;
}

int write_pin_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t pin, out;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    out = atoi(argv[3]);
    if (out == 1){
        pin = atoi(argv[2]);
        if (pin > 15 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_GPIO_WritePin(gpioDev, pin, GPIO_PIN_SET);
        return status;
    } else if (out == 0){
        pin = atoi(argv[2]);
        if (pin > 15 || pin < 0) {
            return HAL_STATUS_INVALID_PARAM;
        }

        HAL_Status_t status = HAL_GPIO_WritePin(gpioDev, pin, GPIO_PIN_RESET);
        return status;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }
}

int toggle_pin_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_TogglePin(gpioDev, pin);
    return status;
}

int enable_irq_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_EnableIRQ(gpioDev, pin);

    return status;
}

int disable_irq_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_DisableIRQ(gpioDev, pin, 1);
    return status;
}

void CMD_GPIO_EXTI_Callback(void *arg)
{
    HAL_GPIO_DisableIRQ(gpioDev, GPIO_SHELL_TEST_PIN, 1);
    printf("\ngpio_irq_trigger \n");
}

int init_irq_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_PinParam_t gpioParam;
    GPIO_IrqParam_t gpioirq;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "rising")) {
        gpioirq.event = GPIO_IRQ_EVT_RISING_EDGE;
    } else if (!strcmp(argv[3], "falling")) {
        gpioirq.event = GPIO_IRQ_EVT_FALLING_EDGE;
    } else if (!strcmp(argv[3], "both")) {
        gpioirq.event = GPIO_IRQ_EVT_BOTH_EDGE;
    } else if (!strcmp(argv[3], "high")) {
        gpioirq.event = GPIO_IRQ_EVT_HIGH_LEVEL;
    } else if (!strcmp(argv[3], "low")) {
        gpioirq.event = GPIO_IRQ_EVT_LOW_LEVEL;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    gpioParam.mode = GPIOx_Pn_F14_EINT;
    gpioParam.pull = GPIO_PULL_FLOATING;
    gpioParam.driving = GPIO_DRIVING_LEVEL_1;

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_GPIO_SetPinParam(gpioDev, pin, &gpioParam);

    gpioirq.callback = CMD_GPIO_EXTI_Callback;
    gpioirq.arg      = NULL;

    HAL_Status_t status = HAL_GPIO_SetIRQParam(gpioDev, pin, &gpioirq);
    return status;
}

int set_event_cmd(Shell *shell, int argc, char *argv[])
{
    GPIO_Pin_t pin;
    GPIO_IrqParam_t gpioirq;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[3], "rising")) {
        gpioirq.event = GPIO_IRQ_EVT_RISING_EDGE;
    } else if (!strcmp(argv[3], "falling")) {
        gpioirq.event = GPIO_IRQ_EVT_FALLING_EDGE;
    } else if (!strcmp(argv[3], "both")) {
        gpioirq.event = GPIO_IRQ_EVT_BOTH_EDGE;
    } else if (!strcmp(argv[3], "high")) {
        gpioirq.event = GPIO_IRQ_EVT_HIGH_LEVEL;
    } else if (!strcmp(argv[3], "low")) {
        gpioirq.event = GPIO_IRQ_EVT_LOW_LEVEL;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    pin = atoi(argv[2]);
    if (pin > 15 || pin < 0) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_GPIO_SetIRQEvent(gpioDev, pin, gpioirq.event);
    return status;
}

int set_debounce_cmd(Shell *shell, int argc, char *argv[])
{
    uint32_t debounce;

    if (!strcmp(argv[1], "a") || !strcmp(argv[1], "A")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_A);
    } else if (!strcmp(argv[1], "b") || !strcmp(argv[1], "B")) {
        gpioDev = HAL_GPIO_Init(GPIO_PORT_B);
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    debounce = atoi(argv[2]);

    HAL_Status_t status = HAL_GPIO_SetIRQDebounce(gpioDev, debounce);
    return status;
}

int gpio_version_cmd(Shell *shell, int argc, char *argv[])
{
    shellPrint(shell, "gpio version:1.0 \n");
    return HAL_STATUS_OK;
}

int default_cmd(Shell *shell, int argc, char *argv[])
{
    shellPrint(shell, "argc: %d\n", argc);
    return 0;
}

ShellCommand gpio_cmds[] = {
    SHELL_CMD_ITEM(set_qcfg,        set_qcfg_cmd,         "<port> <pin> <mode> (out/in)"),
    SHELL_CMD_ITEM(set_config,      set_config_cmd,       "<port> <pin> <mode> (out/in) <push> (push/open) <pull> (no/up/down) <driving>"),
    SHELL_CMD_ITEM(get_config,      get_config_cmd,       "<port> <pin>"),
    SHELL_CMD_ITEM(get_mode,        get_mode_cmd,         "<port> <pin>"),
    SHELL_CMD_ITEM(get_pull,        get_pull_cmd,         "<port> <pin>"),
    SHELL_CMD_ITEM(get_push,        get_push_cmd,         "<port> <pin>"),
    SHELL_CMD_ITEM(get_driving,     get_driving_cmd,      "<port> <pin>"),
    SHELL_CMD_ITEM(read_port,       read_port_cmd,        "<port>"),
    SHELL_CMD_ITEM(read_pin,        read_pin_cmd,         "<port> <pin>"),
    SHELL_CMD_ITEM(write_port,      write_port_cmd,       "<port> <value-mask>"),
    SHELL_CMD_ITEM(write_pin,       write_pin_cmd,        "<port> <pin> <pin-value>"),
    SHELL_CMD_ITEM(toggle_pin,      toggle_pin_cmd,       "<port> <pin>"),
    SHELL_CMD_ITEM(enable_irq,      enable_irq_cmd,       "<port> <pin>"),
    SHELL_CMD_ITEM(disable_irq,     disable_irq_cmd,      "<port> <pin>"),
    SHELL_CMD_ITEM(init_irq,        init_irq_cmd,         "<port> <pin> <event> (rising/falling/both/high/low)"),
    SHELL_CMD_ITEM(set_event,       set_event_cmd,        "<port> <pin> <event> (rising/falling/both/high/low)"),
    SHELL_CMD_ITEM(set_debounce,    set_debounce_cmd,     "<port> <debounce>"),
    SHELL_CMD_ITEM(gpio_version,    gpio_version_cmd,     ""),

    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(gpio, gpio_cmds, "gpio command");


#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
