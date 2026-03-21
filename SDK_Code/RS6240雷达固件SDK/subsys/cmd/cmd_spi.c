/**
 ******************************************************************************
 * @file    cmd_spi.c
 * @brief   cmd spi define.
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
#include "hal_spi.h"
#include "hal_board.h"
#include "log.h"

#if ((CONFIG_SHELL_CMD_SPI == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
DEFINE_PINMUX(spi0, 6,
    { GPIO_PORT_A, GPIO_PIN_0,  {GPIOA_P0_F2_SPI_CS,   GPIO_DRIVING_LEVEL_3, GPIO_PUSH_PULL, GPIO_PULL_UP } },
    { GPIO_PORT_A, GPIO_PIN_1,  {GPIOA_P1_F2_SPI_CLK,   GPIO_DRIVING_LEVEL_2, GPIO_PUSH_PULL, GPIO_PULL_UP } },
    { GPIO_PORT_A, GPIO_PIN_2,  {GPIOA_P2_F2_SPI_MOSI,   GPIO_DRIVING_LEVEL_2, GPIO_PUSH_PULL, GPIO_PULL_UP } },
    { GPIO_PORT_A, GPIO_PIN_3,  {GPIOA_P3_F2_SPI_MISO,   GPIO_DRIVING_LEVEL_2, GPIO_PUSH_PULL, GPIO_PULL_UP } },
    { GPIO_PORT_A, GPIO_PIN_4,  {GPIOA_P4_F2_SPI_HOLD,   GPIO_DRIVING_LEVEL_2, GPIO_PUSH_PULL, GPIO_PULL_UP } },
    { GPIO_PORT_A, GPIO_PIN_5,  {GPIOA_P5_F2_SPI_WP,   GPIO_DRIVING_LEVEL_2, GPIO_PUSH_PULL, GPIO_PULL_UP } },
);

#define SPI_TEST_MASTER_WAIT_TIME                       3000
#define SPI_TEST_SLAVE_WAIT_TIME                        3000
#define SPI_TEST_CYCLE_TIME                             1000

#define SPI_TEST_CYCLE_NUM                             1

#define SPI_CMD_VER_MAJOR                              1
#define SPI_CMD_VER_MINOR                              0

#define SPI_CMD_MODE_TX                                0
#define SPI_CMD_MODE_RX                                1
#define SPI_CMD_MODE_FULL                              2

#define SPI_CMD_PRINTF_DATA                            1

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
extern Shell shell;
static TaskHandle_t task_handle = NULL;
static HAL_Dev_t *spi_handle;
const char *spi_name[1] = {"spi0"};
static uint16_t echo_size = 10;


#if 1
static __aligned(32) uint8_t rx_buf[4096];
static __aligned(32) uint8_t tx_buf[4096];
#else

static __aligned(32) uint8_t *rx_buf = (uint8_t *)(0x0041c800);
static __aligned(32) uint8_t *tx_buf = (uint8_t *)(0x0041c800 + 4096);

#endif


static uint8_t slave_index = 0;
static uint8_t echo_mode = 1;
static uint8_t tranf_mode = 0;  //norm:0 irq:1 dma:2
static uint8_t echo_data = 0;
static uint8_t mode = 1;
static uint8_t task_open = 0;
static uint8_t task_ctrl = 0;
static uint8_t task_ctrl2 = 0;
static uint8_t init_finish = 0;
static uint8_t mode_cfg = 0;


/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

void spi_task(void *param);
void spi_task_2(void *param);



static int shell_spi_config(Shell *shell, int argc, char *argv[])
{
    SPI_InitParam_t temp_param = {  .mode = SPI_MODE_MASTER,
                                    .sclkMode = SPI_SCLK_MODE0,
                                    .ioMode = SPI_IO_MODE_NORMAL,
                                    .csMode = SPI_CS_HARD,
                                    .dataWidth = SPI_DATAWIDTH_8BIT,
                                    .sclk = 4000000,
                                    };

    SPI_ID_t spi_index = SPI0_ID;
    HAL_Status_t status;
    uint8_t i;
    uint8_t j;
    static const char *mode_name[] = {"master", "slave"};
    static const uint8_t p_mode[] ={SPI_MODE_MASTER, SPI_MODE_SLAVE};
    static const char *sclk_name[] = {"mode0", "mode1", "mode2", "mode3"};
    static const uint8_t p_sclk[] = {SPI_SCLK_MODE0, SPI_SCLK_MODE1, SPI_SCLK_MODE2, SPI_SCLK_MODE3};
    static const char *cs_name[] = {"hard", "soft"};
    static const uint8_t p_cs[] = {SPI_CS_HARD, SPI_CS_SOFT};
    static const char *line_name[] = {"std", "dual", "quad"};
    static const uint8_t p_line[] = {SPI_IO_MODE_NORMAL, SPI_IO_MODE_DUAL, SPI_IO_MODE_QUAD};

    if (init_finish != 0) {
        shellPrint(shell, "spi already init finish \n");
        return HAL_STATUS_ERROR;
    }

    if ((argc > 1) && (argc < 10)) {
        for (i = 1; i < argc; i++) {
            switch (i) {
            case 1:
                /*SPI ID*/
                break;
            case 2:
                /*MODE*/
                for (j = 0; j < 2; j++) {
                    if (!memcmp(mode_name[j], argv[2], strlen(argv[2]))) {
                        temp_param.mode = p_mode[j];
                        mode = p_mode[j];
                    }
                }
                break;
            case 3:
                /*SCLK MODE*/
                for (j = 0; j < 4; j++) {
                    if (!memcmp(sclk_name[j], argv[3], strlen(argv[3]))) {
                        temp_param.sclkMode = p_sclk[j];
                    }
                }
                break;
            case 4:
                /*IO MODE*/
                for (j = 0; j < 3; j++) {
                    if (!memcmp(line_name[j], argv[4], strlen(argv[4]))) {
                        temp_param.ioMode = p_line[j];
                    }
                }
                break;
            case 5:
                /*CS MODE*/
                for (j = 0; j < 2; j++) {
                    if (!memcmp(cs_name[j], argv[5], strlen(argv[5]))) {
                        temp_param.csMode = p_cs[j];
                    }
                }
                break;
            case 6:
                /*DATA WIDTH*/
                temp_param.dataWidth = strtoul(argv[6], NULL, 10);
                break;
            case 7:
                /*SCLK*/
                temp_param.sclk = strtoul(argv[7], NULL, 10);
                break;
            default:
                break;
            }

        }
//        HAL_SPI_DeInit(spi_handle);
        spi_handle = HAL_SPI_Init(spi_index, &temp_param, 1);
        status = HAL_SPI_Open(spi_handle, slave_index);
        if (status != HAL_STATUS_OK) {
            return HAL_STATUS_ERROR;
        }
        init_finish = 1;

    shellPrint(shell, "SPI Init successful: %d,%d,%d,%d,%d,%d,%d",
                    spi_index, temp_param.mode, temp_param.sclkMode,
                    temp_param.ioMode, temp_param.csMode, temp_param.dataWidth,
                    temp_param.sclk);
    return HAL_STATUS_OK;

    } else {
        shellPrint(shell, "This is SPI Init command - (spiID, mode, sclkMode, ioMode, csMode, dataWidth, sclk)\r\n");
        shellPrint(shell, "spiID: spi0\r\n");
        shellPrint(shell, "mode: master/slave\r\n");
        shellPrint(shell, "sclkMode: mode0/mode1/mode2/mode3\r\n");
        shellPrint(shell, "ioMode: std/dual/quad\r\n");
        shellPrint(shell, "csMode: hard/soft\r\n");
        shellPrint(shell, "dataWidth: 8/16/32\r\n");
        shellPrint(shell, "sclk: 4000000/...\r\n");
        return 1;
    }
}

static int shell_spi_deconfig(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status;

    if (init_finish == 0) {
        shellPrint(shell, "spi no init \n");
        return HAL_STATUS_ERROR;
    }

    status = HAL_SPI_Close(spi_handle);
    if (status == HAL_STATUS_OK) {
        status = HAL_SPI_DeInit(spi_handle);
        if (status == HAL_STATUS_OK) {
            shellPrint(shell, "spi deconfig success\n");
            init_finish = 0;
            mode_cfg = 0;
            return status;
        }
    }
    shellPrint(shell, "spi deconfig fail\n");

    return HAL_STATUS_ERROR;
}


static int shell_spi_set_trans_mode(Shell *shell, int argc, char *argv[])
{
    static const char *dir_name[] = {"tx", "rx", "full"};
    static const char *mode_name[3] = {"normal", "interrupt", "dma"};
    uint8_t i;
    uint8_t j;
    SPI_TransDir_t dir = SPI_DIR_TX;
    SPI_TransMode_t tmode = SPI_TRANS_MODE_NOMA;
    HAL_Status_t status;

     if ((argc > 1) && (argc < 4) && (mode_cfg < 2)) {
        for (i = 1; i < argc; i++) {
            switch (i) {
            case 1:
                for (j = 0; j < 3; j++) {
                    if (!memcmp(dir_name[j], argv[i], strlen(argv[i]))) {
                        dir = j;
                        echo_mode = j;
                        break;
                    }
                }
                break;
            case 2:
                for (j = 0; j < 3; j++) {
                    if (!memcmp(mode_name[j], argv[i], strlen(argv[i]))) {
                        tmode = j;
                        tranf_mode = tmode;
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }
        status = HAL_SPI_SetTransferMode(spi_handle, dir, tmode);
        shellPrint(shell, "dir:%d mode:%d\r\n", dir, tmode);
        mode_cfg++;
        if (status == HAL_STATUS_OK) {
            return HAL_STATUS_OK;
        }
     } else {
        shellPrint(shell, "This is SPI set transmisson mode command - (dir, mode)\r\n");
        shellPrint(shell, "dir: tx/rx/full\r\n");
        shellPrint(shell, "mode: normal/interrupt/dma\r\n");
     }

     return HAL_STATUS_ERROR;
}

static int shell_spi_set_echo_size(Shell *shell, int argc, char *argv[])
{
    uint16_t tmp_size = 0;

    if (argc == 3) {
        tmp_size = strtoul(argv[1], NULL, 10);
        echo_size = tmp_size;

        tmp_size = strtoul(argv[2], NULL, 0);
        echo_data = tmp_size;

        for (int i = 0; i < echo_size; i++) {
            tx_buf[i] = i & 0xFF;
        }

        OSI_MSleep(10);
        if (tranf_mode == SPI_TRANS_MODE_DMA) {
            csi_dcache_clean();
            csi_dcache_invalid();
            printf("clean dcache\n");
        }

//        csi_dcache_clean_range((unsigned long *)tx_buf, echo_size);
//        csi_dcache_invalid_range((unsigned long *)tx_buf, echo_size);
        LOG_HEX(tx_buf, echo_size, "spi tx buff %d: ", tmp_size);
        return HAL_STATUS_OK;

    } else {
        shellPrint(shell, "This is SPI set echo size - (size, data)\r\n");
        shellPrint(shell, "size  : 0~4096\r\n");
        shellPrint(shell, "data  : 0x55/...\r\n");
    }
    return HAL_STATUS_ERROR;
}

static int shell_spi_task_ctrl(Shell *shell, int argc, char *argv[])
{
    const char *ctrl_name[2] = {"enable", "disable"};

    if (argc == 2) {
        if (!memcmp(ctrl_name[0], argv[1], strlen(argv[1]))) {
            if (task_open == 0) {
                if (pdPASS != xTaskCreate((TaskFunction_t)spi_task, "spi task", 1024, NULL, 3, &task_handle)) {
                    shellPrint(shell, "spi task create failed\n");
                    return HAL_STATUS_ERROR;
                }

                if (pdPASS != xTaskCreate((TaskFunction_t)spi_task_2, "spi task_2", 1024, NULL, 3, &task_handle)) {
                    shellPrint(shell, "spi task create failed\n");
                    return HAL_STATUS_ERROR;
                }

                task_open = 1;
            } else {
               // task_ctrl = 1;
            }
        } else if (!memcmp(ctrl_name[1], argv[1], strlen(argv[1]))) {
            vTaskDelete(task_handle);
            task_open = 0;
        }
    } else {
        shellPrint(shell, "This is SPI task control - (cmd)\r\n");
        shellPrint(shell, "cmd  : enable/disable\r\n");
    }

    return HAL_STATUS_OK;
}

static int spi_trans_slave_out(Shell *shell, int argc, char *argv[])
{
    const char *ctrl_name[2] = {"enable", "disable"};

    if (argc == 2) {
        if (!memcmp(ctrl_name[0], argv[1], strlen(argv[1]))) {
            task_ctrl = 1;
        } else if (!memcmp(ctrl_name[1], argv[1], strlen(argv[1]))) {
            task_ctrl = 0;
        }
    } else {
        shellPrint(shell, "This is SPI task control - (cmd)\r\n");
        shellPrint(shell, "cmd  : enable/disable\r\n");
    }

    return HAL_STATUS_OK;
}

static int spi_trans_master_out(Shell *shell, int argc, char *argv[])
{
    const char *ctrl_name[2] = {"enable", "disable"};

    if (argc == 2) {
        if (!memcmp(ctrl_name[0], argv[1], strlen(argv[1]))) {
            task_ctrl2 = 1;
        } else if (!memcmp(ctrl_name[1], argv[1], strlen(argv[1]))) {
            task_ctrl2 = 1;
        }
    } else {
        shellPrint(shell, "This is SPI task control - (cmd)\r\n");
        shellPrint(shell, "cmd  : enable/disable\r\n");
    }
    return HAL_STATUS_OK;
}


static int shell_spi_version_get(Shell *shell, int argc, char *argv[])
{
    shellPrint(shell, "spi version: %d.%d\r\n", SPI_CMD_VER_MAJOR, SPI_CMD_VER_MINOR);

    return HAL_STATUS_OK;
}


ShellCommand spi_group[] =
{
    SHELL_CMD_ITEM(config, shell_spi_config, "spi init command"),
    SHELL_CMD_ITEM(deconfig, shell_spi_deconfig, "spi deinit command"),
    SHELL_CMD_ITEM(setmode, shell_spi_set_trans_mode, "set trans mode"),
    SHELL_CMD_ITEM(echo, shell_spi_set_echo_size, "set echo length"),
    SHELL_CMD_ITEM(task, shell_spi_task_ctrl, "control the spi task"),
    SHELL_CMD_ITEM(slave_out, spi_trans_slave_out, "control the transmission"),
    SHELL_CMD_ITEM(master_out, spi_trans_master_out, "control the transmission"),
    SHELL_CMD_ITEM(ver, shell_spi_version_get, "get the spi cmd version"),
    SHELL_CMD_GROUP_END()
};

SHELL_CMD_GROUP(spi, spi_group, "spi test cmd group");

//slave out
void spi_task(void *param)
{
    int tmp_size = 0;
    int num_error = 0;
    while (1) {
        if (task_ctrl) {
            if (mode == SPI_MODE_MASTER) {
                if (echo_mode != SPI_CMD_MODE_FULL) {
                    tmp_size = HAL_SPI_Receive(spi_handle, rx_buf, echo_size, SPI_TEST_MASTER_WAIT_TIME);
                    if (tmp_size > 0 || tmp_size == HAL_STATUS_TIMEOUT) {
                        if (tranf_mode == SPI_TRANS_MODE_DMA) {
                            csi_dcache_invalid();
                        }
//                        csi_dcache_invalid_range((unsigned long *)rx_buf, echo_size);
                        LOG_HEX(rx_buf, echo_size, "spi master rec %d: ", tmp_size);
                        for (int i = 0; i < echo_size; i++) {
                            if (tx_buf[i] != rx_buf[i]) {
                                printf("txbuff[%d]:0x%02x rxbuff[%d]:0x%02x \n", i, tx_buf[i], i, rx_buf[i]);
                                num_error++;
                            }
                        }
                        if(num_error == 0) {
                            printf("Cmp buff success\n");
                        }
                        task_ctrl = 0;
                        tmp_size = 0;
                        num_error = 0;
                        memset(rx_buf, 0, sizeof(rx_buf));
                    }
                } else {
                    tmp_size = HAL_SPI_TransmitReceive(spi_handle, tx_buf, rx_buf, echo_size, SPI_TEST_MASTER_WAIT_TIME);
                    if (tmp_size > 0 || tmp_size == HAL_STATUS_TIMEOUT) {
                        LOG_HEX(rx_buf, echo_size, "spi master rec %d: ", tmp_size);
                        for (int i = 0; i < echo_size; i++) {
                            if (tx_buf[i] != rx_buf[i]) {
                                printf("txbuff[%d]:0x%02x rxbuff[%d]:0x%02x \n", i, tx_buf[i], i, rx_buf[i]);
                                num_error++;
                            }
                        }
                        if(num_error == 0) {
                            printf("Cmp buff success\n");
                        }
                        task_ctrl = 0;
                        tmp_size = 0;
                        num_error = 0;
                        memset(rx_buf, 0, sizeof(rx_buf));
                    }
                }
            } else if (mode == SPI_MODE_SLAVE) {
                if (echo_mode != SPI_CMD_MODE_FULL) {
                    HAL_SPI_Transmit(spi_handle, tx_buf, echo_size, SPI_TEST_SLAVE_WAIT_TIME);
                    printf("spi slave transmit finish \n");
                    task_ctrl = 0;
                    tmp_size = 0;
                    memset(rx_buf, 0, sizeof(rx_buf));
                } else {
                    tmp_size = HAL_SPI_TransmitReceive(spi_handle, tx_buf, rx_buf, echo_size, SPI_TEST_SLAVE_WAIT_TIME);
                    if (tmp_size > 0 || tmp_size == HAL_STATUS_TIMEOUT) {
                        LOG_HEX(rx_buf, echo_size, "spi slave rec %d: ", tmp_size);
                        for (int i = 0; i < echo_size; i++) {
                            if (tx_buf[i] != rx_buf[i]) {
                                printf("txbuff[%d]:0x%02x rxbuff[%d]:0x%02x \n", i, tx_buf[i], i, rx_buf[i]);
                                num_error++;
                            }
                        }
                        if(num_error == 0) {
                            printf("Cmp buff success\n");
                        }
                        task_ctrl = 0;
                        tmp_size = 0;
                        num_error = 0;
                        memset(rx_buf, 0, sizeof(rx_buf));
                    }
                }
            }
        }else {
            vTaskDelay(100);
        }
    }
}

//Master out
void spi_task_2(void *param)
{
    int tmp_size = 0;
    int num_error = 0;

    while (1) {
        if (task_ctrl2) {
            if (mode == SPI_MODE_MASTER) {
                if (echo_mode != SPI_CMD_MODE_FULL) {
                    HAL_SPI_Transmit(spi_handle, tx_buf, echo_size, SPI_TEST_MASTER_WAIT_TIME);
                    printf("spi master transmit finish \n");
                    task_ctrl2 = 0;
                    tmp_size = 0;
                    memset(rx_buf, 0, sizeof(rx_buf));
                } else {
                    tmp_size = HAL_SPI_TransmitReceive(spi_handle, tx_buf, rx_buf, echo_size, SPI_TEST_MASTER_WAIT_TIME);
                    if (tmp_size > 0 || tmp_size == HAL_STATUS_TIMEOUT) {
                        LOG_HEX(rx_buf, echo_size, "spi master rec %d: ", tmp_size);

                        for (int i = 0; i < echo_size; i++) {
                            if (tx_buf[i] != rx_buf[i]) {
                                printf("txbuff[%d]:0x%02x rxbuff[%d]:0x%02x \n", i, tx_buf[i], i, rx_buf[i]);
                                num_error++;
                            }
                        }
                        if(num_error == 0) {
                            printf("Cmp buff success\n");
                        }
                        task_ctrl2 = 0;
                        tmp_size = 0;
                        num_error = 0;
                        memset(rx_buf, 0, sizeof(rx_buf));
                    }
                }
            } else if (mode == SPI_MODE_SLAVE) {
                if (echo_mode != SPI_CMD_MODE_FULL) {
                    tmp_size = HAL_SPI_Receive(spi_handle, rx_buf, echo_size, SPI_TEST_SLAVE_WAIT_TIME);
                    if (tmp_size > 0 || tmp_size == HAL_STATUS_TIMEOUT) {
                        LOG_HEX(rx_buf, echo_size, "spi slave rec %d: ", tmp_size);
                    }
                    printf("\n");
                    if (tranf_mode == SPI_TRANS_MODE_DMA) {
                        csi_dcache_invalid();
                    }
                    for (int i = 0; i < echo_size; i++) {
                        if (tx_buf[i] != rx_buf[i]) {
                            printf("txbuff[%d]:0x%02x rxbuff[%d]:0x%02x \n", i, tx_buf[i], i, rx_buf[i]);
                            num_error++;
                        }
                    }
                    if(num_error == 0) {
                        printf("Cmp buff success\n");
                    }
                    task_ctrl2 = 0;
                    tmp_size = 0;
                    num_error = 0;
                    memset(rx_buf, 0, sizeof(rx_buf));
                } else {
                    tmp_size = HAL_SPI_TransmitReceive(spi_handle, tx_buf, rx_buf, echo_size, SPI_TEST_SLAVE_WAIT_TIME);
                    if (tmp_size > 0 || tmp_size == HAL_STATUS_TIMEOUT) {
                        LOG_HEX(rx_buf, echo_size, "spi slave rec %d: ", tmp_size);
                        for (int i = 0; i < echo_size; i++) {
                            if (tx_buf[i] != rx_buf[i]) {
                                printf("txbuff[%d]:0x%02x rxbuff[%d]:0x%02x \n", i, tx_buf[i], i, rx_buf[i]);
                                num_error++;
                            }
                        }
                        if(num_error == 0) {
                            printf("Cmp buff success\n");
                        }
                        task_ctrl2 = 0;
                        tmp_size = 0;
                        num_error = 0;
                        memset(rx_buf, 0, sizeof(rx_buf));
                    }
                }
            }
        }else {
            vTaskDelay(100);
        }
    }
}



#endif

