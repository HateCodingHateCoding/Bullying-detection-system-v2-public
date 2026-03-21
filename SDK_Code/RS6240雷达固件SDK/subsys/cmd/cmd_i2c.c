/**
 ******************************************************************************
 * @file    cmd_i2c.c
 * @brief   cmd i2c define.
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
#include "prj_config.h"

#include <errno.h>

#include "hal_types.h"
#include "hal_dev.h"
#include "hal_i2c.h"

#include "shell.h"
#include "shell_cmd_group.h"

#if ((CONFIG_SHELL_CMD_I2C == 1) && (CONFIG_SHELL == 1))


/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define I2C_TEST_CYCLE_TIME            3000

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
bool                   initAready     = false;
static HAL_Dev_t       *i2cDev        = NULL;
static TaskHandle_t    i2c_task_handle    = NULL;
static uint8_t         task_mode      = 2;
static uint8_t         i2c_task_open  = 0;
static uint8_t         i2c_task_create  = 1;
static uint8_t         addr_width  = 0;
static uint16_t        addr  = 0x3A;


/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
void i2c_task(void *param);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int init_cmd(Shell *shell, int argc, char *argv[])
{
    I2C_InitParam_t i2cParam;

    if(argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "master")) {
        i2cParam.mode = I2C_MODE_MASTER;
        task_mode = I2C_MODE_MASTER;
    } else if (!strcmp(argv[1], "slave")) {
        i2cParam.mode = I2C_MODE_SLAVE;
        task_mode = I2C_MODE_SLAVE;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[2], "std")) {
        i2cParam.speed = I2C_SPEED_STANDARD;
    } else if (!strcmp(argv[2], "fast")) {
        i2cParam.speed = I2C_SPEED_FAST;
    } else if (!strcmp(argv[2], "fast_p")) {
        i2cParam.speed = I2C_SPEED_FAST_PLUS;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2cParam.busErrCb.arg = NULL;
    i2cParam.busErrCb.cb = NULL;

    i2cDev  = HAL_I2C_Init(I2C0_ID, &i2cParam);
    initAready = true;

    return HAL_STATUS_OK;
}

int open_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t i2c_addr = 0;

    if(argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_addr = strtoul(argv[1], NULL, 10);

    if(i2c_addr == 7) {
        addr_width = 0;
        addr = 0x3A;
        HAL_Status_t status = HAL_I2C_Open(i2cDev, addr_width, addr);
        if(status != HAL_STATUS_OK) {
            return status;
        }
        shellPrint(shell, "i2c open success\n");
    } else if(i2c_addr == 10) {
        addr_width = 1;
        addr = 0x78;
        HAL_Status_t status = HAL_I2C_Open(i2cDev, addr_width, addr);
        if(status != HAL_STATUS_OK) {
            return status;
        }
        shellPrint(shell, "i2c open success\n");
    } else {
        shellPrint(shell, "i2c open fail\n");
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int set_rx_transfer_cmd(Shell *shell, int argc, char *argv[])
{
    I2C_TransfMode_t mode;

    if(argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if(initAready != true) {
        shellPrint(shell, "no init i2c\n");
        return HAL_STATUS_ERROR;
    }

    if (!strcmp(argv[1], "norm") || !strcmp(argv[1], "NORM")) {
        mode = I2C_TRANSFMODE_NORMAL;
    } else if (!strcmp(argv[1], "irq") || !strcmp(argv[1], "IRQ")) {
        mode = I2C_TRANSFMODE_INTERRUPT;
    } else if (!strcmp(argv[1], "dma") || !strcmp(argv[1], "DMA")) {
        mode = I2C_TRANSFMODE_DMA;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_I2C_SetTransferMode(i2cDev, I2C_TRANSFDIR_RX, mode);
    return status;
}

int set_tx_transfer_cmd(Shell *shell, int argc, char *argv[])
{
    I2C_TransfMode_t mode;

    if(argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if(initAready != true) {
        shellPrint(shell, "no init i2c\n");
        return HAL_STATUS_ERROR;
    }

    if (!strcmp(argv[1], "norm") || !strcmp(argv[1], "NORM")) {
        mode = I2C_TRANSFMODE_NORMAL;
    } else if (!strcmp(argv[1], "irq") || !strcmp(argv[1], "IRQ")) {
        mode = I2C_TRANSFMODE_INTERRUPT;
    } else if (!strcmp(argv[1], "dma") || !strcmp(argv[1], "DMA")) {
        mode = I2C_TRANSFMODE_DMA;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_Status_t status = HAL_I2C_SetTransferMode(i2cDev, I2C_TRANSFDIR_TX, mode);
    return status;
}

int transmit_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t txBuff[30];
    uint32_t txbuff_num = 0;
    uint32_t timeout = 0;
    HAL_Status_t status_close = 0;

    if (argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (initAready != true) {
        shellPrint(shell, "no init i2c\n");
        return HAL_STATUS_ERROR;
    }

    HAL_Status_t status_open = HAL_I2C_Open(i2cDev, addr_width, addr);
    if (!status_open == HAL_STATUS_OK) {
        shellPrint(shell, "i2c open fail:%d\n",status_open);
        return HAL_STATUS_ERROR;
    }

    txbuff_num = argc - 2;

    for (uint8_t i = 0; i < txbuff_num; i++) {
        txBuff[i] = strtoul(argv[i+2], NULL, 16);
    }

    timeout = atoi(argv[1]);

    HAL_Status_t status = HAL_I2C_Transmit(i2cDev, txBuff, txbuff_num, timeout);
    if (status != txbuff_num) {
        if (status == HAL_STATUS_TIMEOUT) {
            shellPrint(shell, "transmit timeout \n");
            status_close = HAL_I2C_Close(i2cDev);
            if (!status_close == HAL_STATUS_OK) {
                shellPrint(shell, "i2c close fail:%d\n", status_close);
            }
            return status;
        } else {
            shellPrint(shell, "transmit fail:%d \n",status);
            status_close = HAL_I2C_Close(i2cDev);
            if (!status_close == HAL_STATUS_OK) {
                shellPrint(shell, "i2c close fail:%d\n", status_close);
            }
            return HAL_STATUS_ERROR;
        }
    }

    status_close = HAL_I2C_Close(i2cDev);
    if (!status_close == HAL_STATUS_OK) {
        shellPrint(shell, "i2c close fail:%d\n", status_close);
    }

    shellPrint(shell, "i2c transmit: ");

    for (uint8_t i = 0; i < txbuff_num; i++) {
        shellPrint(shell, "0x%02X ", txBuff[i]);
    }
    shellPrint(shell, "\n");

    shellPrint(shell, "transmit success \n");

    return HAL_STATUS_OK;
}


int receive_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t rxBuff[30];
    uint32_t rxbuff_num = 0;
    uint32_t timeout = 0;
    HAL_Status_t status_close = 0;

    if (argc < 3) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (initAready != true) {
        shellPrint(shell, "no init i2c\n");
        return HAL_STATUS_ERROR;
    }

    HAL_Status_t status_open = HAL_I2C_Open(i2cDev, addr_width, addr);
    if (!status_open == HAL_STATUS_OK) {
        shellPrint(shell, "i2c open fail:%d\n",status_open);
        return HAL_STATUS_ERROR;
    }

    timeout = atoi(argv[1]);
    rxbuff_num = atoi(argv[2]);

    HAL_Status_t status = HAL_I2C_Receive(i2cDev, rxBuff, rxbuff_num, timeout);
    if (status != rxbuff_num) {
        if (status == HAL_STATUS_TIMEOUT) {
            shellPrint(shell, "transmit timeout \n");
            status_close = HAL_I2C_Close(i2cDev);
            if (!status_close == HAL_STATUS_OK) {
                shellPrint(shell, "i2c close fail:%d\n",status_close);
            }
            return status;
        } else {
            shellPrint(shell, "receive fail:%d \n",status);
            status_close = HAL_I2C_Close(i2cDev);
            if(!status_close == HAL_STATUS_OK) {
                shellPrint(shell, "i2c close fail:%d\n",status_close);
            }
            return HAL_STATUS_ERROR;
        }
    }

    status_close = HAL_I2C_Close(i2cDev);
    if (!status_close == HAL_STATUS_OK) {
        shellPrint(shell, "i2c close fail:%d\n",status_close);
    }

    shellPrint(shell, "i2c receive: ");

    for(uint8_t i = 0; i < rxbuff_num; i++) {
        shellPrint(shell, "0x%02X ", rxBuff[i]);
    }
    shellPrint(shell, "\n");

    return HAL_STATUS_OK;
}


int deinit_cmd(Shell *shell, int argc, char *argv[])
{
    if (initAready != true) {
        shellPrint(shell, "no init i2c\n");
        return HAL_STATUS_ERROR;
    }

//    HAL_Status_t status1 = HAL_I2C_Close(i2cDev);
//    if(!status1 == HAL_STATUS_OK) {
//        shellPrint(shell, "de close fail:%d\n",status1);
//        return HAL_STATUS_ERROR;
//    }

    HAL_Status_t status2 = HAL_I2C_DeInit(i2cDev);
    if (!status2 == HAL_STATUS_OK) {
        shellPrint(shell, "de init fail:%d\n",status2);
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

int i2c_task_ctrl_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "enable") || !strcmp(argv[1], "ENABLE")) {
        if (i2c_task_create == 1) {
            if (pdPASS != xTaskCreate(
                    (TaskFunction_t)i2c_task,
                    "i2c task",
                    1024,
                    NULL,
                    4,
                    &i2c_task_handle)) {
                shellPrint(shell, "i2c task create failed\n");
                return HAL_STATUS_ERROR;
            }
            i2c_task_create = 0;
			shellPrint(shell, "i2c_task_open:%d \n",i2c_task_open);
            shellPrint(shell, "i2c task is created success \n");
        }
    } else if (!strcmp(argv[1], "disable") || !strcmp(argv[1], "DISABLE")) {
        vTaskDelete(i2c_task_handle);
        i2c_task_create = 1;
        shellPrint(shell, "i2c task is delete success \n");

    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int i2c_task_start_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!strcmp(argv[1], "open") || !strcmp(argv[1], "OPEN")) {
        i2c_task_open = 1;
    } else if (!strcmp(argv[1], "close") || !strcmp(argv[1], "CLOSE")) {
        i2c_task_open = 0;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }
    shellPrint(shell, "task open \n");
    return HAL_STATUS_OK;
}

int i2c_version_cmd(Shell *shell, int argc, char *argv[])
{
    shellPrint(shell, "i2c version:2.6 \n");
    return HAL_STATUS_OK;
}

ShellCommand i2c_cmds[] = {
    SHELL_CMD_ITEM(init,                     init_cmd,              "<mode> (master/slave) <speed> (std/fast/fast_p)"),
    SHELL_CMD_ITEM(open,                     open_cmd,              "<addr> (7/10)"),
    SHELL_CMD_ITEM(set_rx_transfer,          set_rx_transfer_cmd,   "<mode> (norm/irq)"),
    SHELL_CMD_ITEM(set_tx_transfer,          set_tx_transfer_cmd,   "<mode> (norm/irq)"),
    SHELL_CMD_ITEM(transmit,                 transmit_cmd,          "<timeout> (>0) <buff[0]> <buff[1]> ...<buff[29]>"),
    SHELL_CMD_ITEM(receive,                  receive_cmd,           "<timeout> (>0) <num>"),
    SHELL_CMD_ITEM(deinit,                   deinit_cmd,            ""),
    SHELL_CMD_ITEM(i2c_task_ctrl,            i2c_task_ctrl_cmd,     "<ctrl> (enable/disable)"),
    SHELL_CMD_ITEM(i2c_task_start,           i2c_task_start_cmd,    "<ctrl> (open/close)"),
    SHELL_CMD_ITEM(i2c_version,              i2c_version_cmd,       ""),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(i2c, i2c_cmds, "i2c command");


void i2c_task(void *param)
{
    uint32_t     ret         =   0U;
    int32_t      rx_num      =   0;
    int32_t      tx_num      =   0;
    uint32_t     fail_flag    =   0U;
    uint8_t txBuff[20]  =  { 0x00, 0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08, 0x09,
                             0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
                             0x0F, 0x10, 0x11, 0x12, 0x13};

    uint8_t rxBuff[20];

    while(1) {
        if (task_mode == I2C_MODE_MASTER && i2c_task_open == 1) {

            HAL_Status_t status_open = HAL_I2C_Open(i2cDev, I2C_ADDR_WIDTH_7BIT, 0x3A);
            if (!status_open == HAL_STATUS_OK) {
                printf("i2c open fail:%d\n",status_open);
            }

            tx_num = HAL_I2C_Transmit(i2cDev, txBuff, sizeof(txBuff), I2C_TEST_CYCLE_TIME);
            if (tx_num <= 0) {
                i2c_task_open = 0;
                printf("i2c Transmit Fail (%d)\n", tx_num);
                fail_flag = 1;
            }

            if (fail_flag == 0) {
                printf("tx_ok\n");
                rx_num = HAL_I2C_Receive(i2cDev, rxBuff, sizeof(rxBuff), I2C_TEST_CYCLE_TIME);
                if (rx_num <= 0) {
                    printf("i2c Receive Fail (%d)\n", rx_num);
                    ret |= HAL_BIT(1);
                }

                printf("i2c master receive : ");
                for (uint8_t i = 0; i < rx_num; i++) {
                    printf("0x%02X ", rxBuff[i]);
                }
                printf("\n");

                if (OSI_Memcmp(txBuff, rxBuff, sizeof(rxBuff)) != 0) {
                    printf("Cmp buff fail\n");
                    ret |= HAL_BIT(2);
                }

                if (ret != 0) {
                    printf("test run fail (%08X)\n", ret);
                    ret = 0;
                } else {
                    printf("test run success\n");
                }
                memset(rxBuff, 0, sizeof(rxBuff));
             }

            HAL_Status_t status_close = HAL_I2C_Close(i2cDev);
            if(!status_close == HAL_STATUS_OK) {
                printf("i2c close fail:%d\n",status_close);
            }

            fail_flag = 0;
            i2c_task_open = 0;

            vTaskDelay(200);

        } else if (task_mode == I2C_MODE_SLAVE && i2c_task_open == 1) {

            HAL_Status_t status_open = HAL_I2C_Open(i2cDev, I2C_ADDR_WIDTH_7BIT, 0x3A);
            if (!status_open == HAL_STATUS_OK) {
                printf("i2c open fail:%d\n",status_open);
            }

            rx_num = HAL_I2C_Receive(i2cDev, rxBuff, sizeof(rxBuff), I2C_TEST_CYCLE_TIME);
            if (rx_num <= 0) {
                printf("i2c Receive Fail (%d)\n", rx_num);
                i2c_task_open = 0;
                fail_flag = 1;
            }

            if (fail_flag == 0) {
                uint8_t *slave_txBuff = rxBuff;

                tx_num = HAL_I2C_Transmit(i2cDev, slave_txBuff, sizeof(rxBuff), I2C_TEST_CYCLE_TIME);
                if (tx_num <= 0) {
                    printf("i2c Transmit Fail (%d)\n", tx_num);
                    ret |= HAL_BIT(1);
                }

                printf("i2c slave receive : ");
                for (uint8_t i = 0; i < rx_num; i++) {
                    printf("0x%02X ", rxBuff[i]);
                }
                printf("\n");

                if (OSI_Memcmp(txBuff, rxBuff, sizeof(rxBuff)) != 0) {
                    printf("Cmp buff fail\n");
                    ret |= HAL_BIT(2);
                }

                if (ret != 0) {
                    printf("test run fail (%08X)\n", ret);
                    ret = 0;
                } else {
                    printf("test run success\n");
                }
            }

            HAL_Status_t status_close = HAL_I2C_Close(i2cDev);
            if(!status_close == HAL_STATUS_OK) {
                printf("i2c close fail:%d\n",status_close);
            }

            memset(rxBuff, 0, sizeof(rxBuff));
            fail_flag = 0;
            i2c_task_open = 0;

            vTaskDelay(200);
        }else {
            vTaskDelay(100);
        }

    }
}


#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
