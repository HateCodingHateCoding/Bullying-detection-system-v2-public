/**
 ******************************************************************************
 * @file    cmd_uart.c
 * @brief   cmd uart define.
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

#include "stdio.h"
#include "shell.h"
#include "hal_uart.h"

#if ((CONFIG_SHELL_CMD_UART == 1) && (CONFIG_SHELL == 1))
/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */

#define UART_CMD_RX_TIMEOUT                HAL_WAIT_FOREVER
#define UART_CMD_TX_TIMEOUT                60000
/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */

extern Shell shell;
static HAL_Dev_t *uart_handle;
TaskHandle_t task_handle = NULL;
const char *uart_name[3] = {"uart0", "uart1", "uart2"};
static uint16_t echo_size = 0;
static uint8_t rx_buf[4096];
static uint8_t task_enable = 0;
static uint8_t task_open = 0;


/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

void uart_task(void *param);

int shell_uart_config(Shell *shell, int argc, char *argv[])
{
    UART_InitParam_t temp_param = { .baudRate = 115200,
                                    .parity = UART_PARITY_NONE,
                                    .dataBits = UART_STOP_BIT_1,
                                    .dataBits = UART_DATA_WIDTH_8,
                                   .autoFlowCtrl = UART_PARITY_NONE};
    UART_ID_t uart_index = UART0_ID;
    HAL_Status_t status;
    uint8_t i;
    uint8_t j;
    static const char *parity_name[3] = {"none", "even", "odd"};
    static const uint8_t p_parity[] ={UART_PARITY_NONE, UART_PARITY_EVEN, UART_PARITY_ODD};
    static const char *stop_name[3] = {"1", "1.5", "2"};
    static const uint8_t p_stop[] = {UART_STOP_BIT_1, UART_STOP_BIT_2, UART_STOP_BIT_2};
    static const char *data_name[4] = {"5", "6", "7", "8"};
    static const uint8_t p_data[] = {UART_DATA_WIDTH_5,UART_DATA_WIDTH_6,UART_DATA_WIDTH_7,UART_DATA_WIDTH_8};
    static const char *flow_name[2] = {"enable", "disable"};

    if ((argc > 1) && (argc < 9)) {
        for (i = 1; i < argc; i++) {
            switch (i) {
                case 1:
                    /*UART ID*/
                    shellPrint(shell, "argv1: %s\r\n", argv[1]);
                    for (j = 0; j < 3; j++) {
                        if (!memcmp(uart_name[j], argv[1], 5)) {
                            uart_index = j;
                            break;
                        }
                    }
                    break;
                case 2:
                    /*UART Baudrate*/
                    shellPrint(shell, "argv2: %s\r\n", argv[2]);
                    temp_param.baudRate = strtol(argv[2], NULL, 10);
                    break;
                case 3:
                    /*UART Parity*/
                    shellPrint(shell, "argv3: %s\r\n", argv[3]);
                    for (j = 0; j < 3; j++) {
                        if (!memcmp(parity_name[j], argv[3], strlen(argv[3]))) {
                            temp_param.parity = p_parity[j];
                            break;
                        }
                    }
                    break;
                case 4:
                    /*UART Stop bit*/
                    shellPrint(shell, "argv4: %s\r\n", argv[4]);
                    for (j = 0; j < 3; j++) {
                        if (!memcmp(stop_name[j], argv[4], strlen(argv[4]))) {
                            temp_param.stopBits = p_stop[j];
                            break;
                        }
                    }
                    break;
                case 5:
                    /*UART Data Length*/
                    shellPrint(shell, "argv5: %s\r\n", argv[5]);
                    for (j = 0; j < 4; j++) {
                        if (!memcmp(data_name[j], argv[5], strlen(argv[5]))) {
                            temp_param.dataBits = p_data[j];
                            break;
                        }
                    }
                    break;
                case 6:
                    /*UART Flow Ctrl*/
                    shellPrint(shell, "argv6: %s\r\n", argv[6]);
                    if (!memcmp(flow_name[0], argv[6], strlen(argv[6]))) {
                        temp_param.autoFlowCtrl = UART_FLOW_CTRL_AUTO;
                    } else {
                        temp_param.autoFlowCtrl = UART_FLOW_CTRL_NONE;
                    }
                    break;
                default:
                    break;
            }

        }
        uart_handle = HAL_UART_Init(uart_index, &temp_param);
        status = HAL_UART_Open(uart_handle);
        if (status != HAL_STATUS_OK) {
            return HAL_STATUS_ERROR;
        }
#if CONFIG_UART_PM
        return HAL_STATUS_OK;
#else
        shellPrint(shell, "UART Init successful: %d,%d,%d,%d,%d,%d",
                    uart_index, temp_param.baudRate, temp_param.parity,
                    temp_param.stopBits, temp_param.dataBits, temp_param.autoFlowCtrl);
        return HAL_STATUS_OK;
#endif
    } else {
        shellPrint(shell, "This is UART Init command - (uarID, baudrate, parity, stopbit, datalength, flow)\r\n");
        shellPrint(shell, "uartID: uart0/uart1/uart2\r\n");
        shellPrint(shell, "baudrate: 9600/115200...\r\n");
        shellPrint(shell, "parity: none/odd/even\r\n");
        shellPrint(shell, "stopbit: 1 \r\n");
        shellPrint(shell, "data_length: 6/7/8\r\n");
        shellPrint(shell, "flow: enable/disable\r\n");
#if CONFIG_UART_PM
#endif
        return HAL_STATUS_INVALID_PARAM;
    }
}

int shell_uart_deconfig(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status;

    status = HAL_UART_Close(uart_handle);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell,"uart close fail:%d \n",status);
        return status;
    }

    status = HAL_UART_DeInit(uart_handle);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell,"uart deinit fail:%d \n",status);
        return status;
    }

    shellPrint(shell,"uart deconfig success \n");
    return HAL_STATUS_OK;
}



int shell_uart_set_trans_mode(Shell *shell, int argc, char *argv[])
{
    static const char *dir_name[2] = {"tx", "rx"};
    static const char *mode_name[3] = {"normal", "interrupt", "dma"};
    uint8_t i;
    uint8_t j;
    UART_TransDir_t dir = UART_DIR_TX;
    UART_TransMode_t mode = UART_TRANS_MODE_NOMA;
    HAL_Status_t status;

     if ((argc > 1) && (argc < 4)) {
        for (i = 1; i < argc; i++) {
            switch (i) {
            case 1:
                for (j = 0; j < 2; j++) {
                    if (!memcmp(dir_name[j], argv[i], strlen(argv[i]))) {
                        dir = j;
                        break;
                    }
                }
                break;
            case 2:
                for (j = 0; j < 2; j++) {
                    if (!memcmp(mode_name[j], argv[i], strlen(argv[i]))) {
                        mode = j;
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }
        status = HAL_UART_SetTransferMode(uart_handle, dir, mode);
        if (status == HAL_STATUS_OK) {
            return HAL_STATUS_OK;
        }
     } else {
        shellPrint(shell, "This is UART set transmisson mode command - (dir, mode)\r\n");
        shellPrint(shell, "dir: tx/rx\r\n");
        shellPrint(shell, "mode: normal/interrupt/dma\r\n");
     }

     return HAL_STATUS_ERROR;
}

int shell_uart_set_echo_size(Shell *shell, int argc, char *argv[])
{
    uint16_t tmp_size = 0;

    tmp_size = strtoul(argv[2], NULL, 10);
    echo_size = tmp_size;
    return HAL_STATUS_OK;
}

int shell_uart_task_ctrl(Shell *shell, int argc, char *argv[])
{
    if (!strcmp(argv[1], "enable") || !strcmp(argv[1], "ENABLE")) {
        if (task_enable == 0) {
            if (pdPASS != xTaskCreate((TaskFunction_t)uart_task, "uart task", 1024, NULL, 3, &task_handle)) {
                shellPrint(shell, "uart task create failed\n");
                return HAL_STATUS_ERROR;
            }
            task_enable = 1;
            shellPrint(shell, "uart task create success\n");
        }
    } else if (!strcmp(argv[1], "disable") || !strcmp(argv[1], "DISABLE")) {
        vTaskDelete(task_handle);
        task_enable = 0;
    }

    return HAL_STATUS_OK;
}

int shell_uart_buffer_flush(Shell *shell, int argc, char *argv[])
{
    memset(rx_buf, 0, sizeof(rx_buf));
    task_open = 1;
    shellPrint(shell, "uart buffer size: %d\r\n", sizeof(rx_buf));

    return HAL_STATUS_OK;
}

int shell_uart_showbuff_cmd(Shell *shell, int argc, char *argv[])
{
    for (uint32_t idx = 0; idx < 4096; idx++) {
        if (idx % 16 == 0 && idx != 0) {
            shellPrint(shell, "\n");
        }
        shellPrint(shell, "%c ", rx_buf[idx]);
    }
    shellPrint(shell, "\n");

    return HAL_STATUS_OK;
}


ShellCommand uart_group[] =
{
    SHELL_CMD_ITEM(config,      shell_uart_config,           "uart init command"),
    SHELL_CMD_ITEM(deconfig,    shell_uart_deconfig,         "uart deinit command"),
    SHELL_CMD_ITEM(show,        shell_uart_showbuff_cmd,     "uart show"),
    SHELL_CMD_ITEM(setmode,     shell_uart_set_trans_mode,   "set trans mode"),
    SHELL_CMD_ITEM(echo,        shell_uart_set_echo_size,    "set echo length"),
    SHELL_CMD_ITEM(task,        shell_uart_task_ctrl,        "control the uart task"),
    SHELL_CMD_ITEM(flush,       shell_uart_buffer_flush,     "flush the uart receiver buffer"),
    SHELL_CMD_GROUP_END()
};

SHELL_CMD_GROUP(uart, uart_group, "uart test cmd group");

void uart_task(void *param)
{
    int tmp_size;

    #if CONFIG_UART_PM
    #endif

    while (1) {
        if (task_open == 1) {
            printf("uart rx start\n");
            tmp_size = HAL_UART_Receive(uart_handle, rx_buf, echo_size, UART_CMD_RX_TIMEOUT);
            printf("uart rx end\n");

            if (tmp_size > 0) {
                printf("uart tx start\n");
                HAL_UART_Transmit(uart_handle, rx_buf, tmp_size, UART_CMD_TX_TIMEOUT); //transmit data
            }
            printf("uart get: %d\n", tmp_size);
            tmp_size = 0;
            task_open = 0;
//            for (uint32_t idx = 0; idx < 32; idx++) {
//                if (idx % 16 == 0 && idx != 0) {
//                    printf("\n");
//                }
//                printf("%c ", rx_buf[idx]);
//            }
//            printf("\n");
        } else {
            vTaskDelay(100);
        }
    }
}

#endif

