/**
 ******************************************************************************
 * @file    cmd_dma.c
 * @brief   cmd dma define.
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
#include "board_config.h"

#include <errno.h>

#include "hal_types.h"
#include "hal_dev.h"
#include "hal_dma.h"

#include "shell.h"
#include "shell_cmd_group.h"

#include "osi.h"

#if ((CONFIG_SHELL_CMD_DMA == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
struct dam_loop_obj {
    int                     test_num;
    int                     ch_id;
    Shell                   *shell;
    OSI_Semaphore_t         *dma_transfer_sem;
    OSI_Thread_t            thread_id;
};

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define DMA_BUFF_DEFAULT_SIZE                   (2048U)

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define DMA_BUFF_SIZE                           (DMA_BUFF_DEFAULT_SIZE)

#define DMA_ERR_LOG_SIZE                        (DMA_BUFF_DEFAULT_SIZE / 4)

#define DMA_CMD_LIST_CNT                        (8U)

#define DMA_LIST_SIZE                           (512U)

#define ALIGNMENT_DMA                           (64)

#define DMA_BLOCK                               (0U)

#define DMA_LIST                                (1U)


/* Private variables.
 * ----------------------------------------------------------------------------
 */
static uint8_t          *dma_src_buff    =    NULL;

static uint8_t          *dma_dst_buff    =    NULL;

static uint8_t          *dma_src_buff_big    =    NULL;

static uint8_t          *dma_dst_buff_big    =    NULL;

static uint8_t src_buff[DMA_CMD_LIST_CNT][DMA_LIST_SIZE];

static uint8_t dst_buff[DMA_LIST_SIZE * DMA_CMD_LIST_CNT];

static OSI_Semaphore_t  doneSem;

static uint32_t         g_chid     =       DMA_CHANNEL_MAX_NUM;

static HAL_Dev_t        *pdmaDev   =       NULL;

static DMA_ChannelConf_t g_chancfg = { 0 };

static DMA_BlockConf_t g_blockcfg  = { 0 };

static DMA_NodeConf_t Node[DMA_CMD_LIST_CNT];

static OSI_Thread_t g_dma_loop_thread[DMA_CHANNEL_MAX_NUM];

static struct dam_loop_obj g_dma_loop_obj[DMA_CHANNEL_MAX_NUM];

static OSI_Semaphore_t g_dma_transfer_sem[DMA_CHANNEL_MAX_NUM];

static uint8_t dma_mode = DMA_BLOCK;


static bool init_aready = false;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static void log_data_err(Shell *shell, uint8_t *src_data, uint8_t *dst_data)
{
    int err_idx = 0;

    /* Compare */
    for (int i = 0; i < DMA_BUFF_SIZE; i++) {
        if (src_data[i] != dst_data[i]) {
            err_idx = i;
            //break;
            shellPrint(shell, "error idx %d\n\r", err_idx);
        }
    }

    shellPrint(shell, "\n\r--src data:\n\r");
    shellHexPrint(shell, src_data, DMA_ERR_LOG_SIZE);
    #if 0
    for (int i = 0; i < 32; i++) {
        if ((i % 16) == 0) {
            shellPrint(shell, SHELL_NORMAL, "\n[%d] 0x%02x ", i + err_idx, src_data[i + err_idx]);
        } else {
            shellPrint(shell, SHELL_NORMAL, "0x%02x ", src_data[i + err_idx]);
        }
    }
    #endif

    shellPrint(shell, "\n\r");
    shellPrint(shell, "\n\r--dst data:\n\r");
    shellHexPrint(shell, dst_data, DMA_ERR_LOG_SIZE);
    #if 0
    for (int i = 0; i < 32; i++) {
        if ((i % 16) == 0) {
            shellPrint(shell, SHELL_NORMAL, "\n[%d] 0x%02x ", i + err_idx, dst_data[i + err_idx]);
        } else {
            shellPrint(shell, SHELL_NORMAL, "0x%02x ", dst_data[i + err_idx]);
        }
    }
    #endif
    shellPrint(shell, "\n\r");
}

static void test_done(HAL_Dev_t *dma, void *arg)
{
    OSI_Semaphore_t *doneSem = (OSI_Semaphore_t *)arg;

   printf("\n\tDMA Tranfer Done!\n\n\r");

    OSI_SemaphoreRelease(doneSem);
}


static void dma_channel_loop_do(void *p_obj)
{
    uint32_t doneNums   =  0;
    struct dam_loop_obj *dma_loop_obj = (struct dam_loop_obj *)p_obj;
    uint32_t ch_id = dma_loop_obj->ch_id;
    int test_num = dma_loop_obj->test_num;
    Shell *shell = (Shell *)dma_loop_obj->shell;
    OSI_Semaphore_t *dma_transfer_sem = dma_loop_obj->dma_transfer_sem;
    static DMA_BlockConf_t blockcfg  = { 0 };

    shellPrint(shell, "test_num %d ch_id %d \n\r", test_num, ch_id);

    OSI_MSleep(100);

    if (HAL_DMA_StartTransfer(pdmaDev, ch_id)) {
        shellPrint(shell, "ERROR: start\n\r");
        goto _ret;
    }

    /* wait 5s */
    if (OSI_SemaphoreWait(dma_transfer_sem, 5000)) {
        shellPrint(shell, "ERROR:ch_%d wait sem\n\r", ch_id);
        goto _ret;
    };


    if (memcmp(dma_src_buff, dma_dst_buff, g_blockcfg.blockSize)) {
        log_data_err(shell, dma_src_buff, dma_dst_buff);
        HAL_DMA_GetChanStatus(pdmaDev, ch_id, &doneNums);
        shellPrint(shell, "ch_%d number [%d] busy[%d]\n\r",
                    ch_id, doneNums,
                    HAL_DMA_GetChanStatus(pdmaDev, ch_id, &doneNums));
        shellPrint(shell, "fail...\n\r");
        goto _ret;
    }

    test_num--;

    while (test_num > 0) {
        blockcfg.srcAddr = (uint32_t)dma_src_buff;
        blockcfg.dstAddr = (uint32_t)dma_dst_buff;
        blockcfg.blockSize = g_blockcfg.blockSize;
        if (HAL_DMA_ConfigBlockTranfer(pdmaDev, ch_id, &blockcfg) != 0) {
            shellPrint(shell, "ERROR:ch_%d Block Cfg\n\r", ch_id);
            HAL_DMA_AbortTransfer(pdmaDev, ch_id);
            goto _ret;
        }

        /* wait 5s */
        if (OSI_SemaphoreWait(dma_transfer_sem, 5000)) {
            shellPrint(shell, "ERROR:ch_%d take sem\n\r", ch_id);
            HAL_DMA_AbortTransfer(pdmaDev, ch_id);
            goto _ret;
        };


        if (memcmp(dma_src_buff, dma_dst_buff, g_blockcfg.blockSize)) {
            log_data_err(shell, dma_src_buff, dma_dst_buff);
            HAL_DMA_GetChanStatus(pdmaDev, ch_id, &doneNums);
            shellPrint(shell, "ch_%d number [%d] busy[%d]\n\r",
                        ch_id, doneNums,
                        HAL_DMA_GetChanStatus(pdmaDev, ch_id, &doneNums));
            shellPrint(shell, "fail...\n\r");
            goto _ret;
        }

        test_num--;
    }

_ret:
    /* abort thread */
    shellPrint(shell, "pass... less %d ch_%d\n\r", test_num, ch_id);
    //k_sem_reset(dma_transfer_sem);
    //k_free(dma_loop_obj->p_thread_stack);
    //OSI_ThreadSuspendScheduler();
    return;
}

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
int dma_set_map_cmd(Shell *shell, int argc, char *argv[])
{
    int      i;
    uint32_t idx;
    uint32_t data;
    uint32_t cnt = 1;
    int      inc = 0;

    if (dma_mode == DMA_BLOCK) {
        if (argc < 3) {
            shellPrint(shell, "[idx][data][cnt][inc]\n\r");
            return HAL_STATUS_INVALID_PARAM;
        }

        if ((dma_dst_buff == NULL) || (dma_src_buff == NULL)) {
            shellPrint(shell, "ERR: buff is null!\n\r");
            return HAL_STATUS_INVALID_PARAM;
        }

        idx  = strtoul(argv[1], NULL, 10);
        if (idx >= g_blockcfg.blockSize) {
            shellPrint(shell, "ERR: idx bigger than %d!\n\r", g_blockcfg.blockSize);
            return HAL_STATUS_INVALID_PARAM;
        }

        data = strtoul(argv[2], NULL, 16);
        if (argc > 3) {
            cnt = strtoul(argv[3], NULL, 10);
            cnt = (cnt > (g_blockcfg.blockSize - idx) ? (g_blockcfg.blockSize - idx) : cnt);
            if (argc > 4) {
                inc = strtoul(argv[4], NULL, 10);
            }
        }

        if (inc == 0) {
            memset(&dma_src_buff[idx], data & 0xFF, cnt);
        } else {
            cnt = idx + cnt;
            for (i = idx; i < cnt ; ++i, data += inc) {
                dma_src_buff[i] = data & 0xFF;
            }
        }
        shellPrint(shell, "idx=%d, data=0x%02x, cnt=%d, inc=%d\n\r", idx, data, cnt, inc);
    }else if (dma_mode == DMA_LIST){

        for (int i = 0; i < DMA_CMD_LIST_CNT; i++) {
            shellPrint(shell, "init src addr 0x%08x\n", (uint32_t)&src_buff[i][0]);
            if ((i % 2) == 0) {
                for (int j = 0; j < DMA_LIST_SIZE; j++) {
                    src_buff[i][j] = j & 0xFF;
                }
            } else {
                for (int j = 0; j < DMA_LIST_SIZE; j++) {
                    src_buff[i][j] = 0xAA;
                }
            }
        }
    } else {
        return HAL_STATUS_ERROR;
    }
    csi_dcache_clean_invalid();
    return HAL_STATUS_OK;
}

int32_t dma_reset_map_cmd(Shell *shell, int argc, char *argv[])
{
    uint32_t reset_num = 0;

    if ((dma_dst_buff == NULL) || (dma_src_buff == NULL)) {
        shellPrint(shell, "ERR: buff is null!\n\r");
        return HAL_STATUS_INVALID_PARAM;
    }

    if (argc != 2) {
        reset_num = g_blockcfg.blockSize;
    } else {
        reset_num = strtoul(argv[1], NULL, 10);
    }

    if (reset_num > g_blockcfg.blockSize) {
        shellPrint(shell, "ERR: idx bigger than %d!\n\r", g_blockcfg.blockSize);
        return HAL_STATUS_INVALID_PARAM;
    } else {
        shellPrint(shell, "reset num [%d]\n\r", reset_num);
    }

    memset(dma_dst_buff, 0x00, reset_num);
    memset(dma_src_buff, 0x00, reset_num);

    return HAL_STATUS_OK;
}

int32_t dma_view_map_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc < 2) {
        shellPrint(shell, "argc size [%d] too few param...\n\r", argc);
        return HAL_STATUS_INVALID_PARAM;
    }

    uint32_t view_num = 0;
    uint32_t view_sum = 0;

    if (dma_mode == DMA_BLOCK) {
        view_num = strtoul(argv[1], NULL, 10);
        if (view_num > g_blockcfg.blockSize) {
            shellPrint(shell, "err: idx bigger than %d!\n\r", g_blockcfg.blockSize);
            return HAL_STATUS_INVALID_PARAM;
        } else {
            shellPrint(shell, "view num [%d]\n\r", view_num);
        }

        if (view_num > 0) {
            shellPrint(shell, "src data: \n\r");
            for (uint32_t idx = 0; idx < view_num; idx++) {
                if (idx % 16 == 0 && idx != 0) {
                    shellPrint(shell, "\n\r");
                }
                shellPrint(shell, "0x%02x ", dma_src_buff[idx]);
            }
            shellPrint(shell, "\n\n\r");
        }

        if (view_num > 0) {
            shellPrint(shell, "dst data: \n\r");
            for (uint32_t idx = 0; idx < view_num; idx++) {
                if (idx % 16 == 0 && idx != 0) {
                    shellPrint(shell, "\n\r");
                }
                shellPrint(shell, "0x%02x ", dma_dst_buff[idx]);
            }
            shellPrint(shell, "\n\r");
        }
    }else if(dma_mode == DMA_LIST){
        view_num = strtoul(argv[2], NULL, 10);
        view_sum = strtoul(argv[1], NULL, 10);

        if (view_num > 0 && view_sum < 8) {
            shellPrint(shell, "src_buff[%d][0] data: \n", view_sum);
            for (uint32_t idx = 0; idx < view_num; idx++) {
            if (idx % 16 == 0 && idx != 0) {
                shellPrint(shell, "\n");
                }
                shellPrint(shell, "0x%02x ", src_buff[view_sum][idx]);
            }
            shellPrint(shell, "\n\r");

            shellPrint(shell, "dst_buff[%d] data: \n", view_sum * DMA_LIST_SIZE);
            for (uint32_t idx = 0; idx < view_num; idx++) {
                if (idx % 16 == 0 && idx != 0) {
                shellPrint(shell, "\n");
                }
                shellPrint(shell, "0x%02x ", dst_buff[view_sum * DMA_LIST_SIZE + idx]);
            }
            shellPrint(shell, "\n\r");


        } else {
            return HAL_STATUS_INVALID_PARAM;
        }
    }

    return HAL_STATUS_OK;
}

int dma_mode_cmd(Shell *shell, int argc, char *argv[])
{
    uint8_t mode = 0;

    if (argc < 2) {
        return HAL_STATUS_INVALID_PARAM;
    }

    mode = atoi(argv[1]);
    if (mode == 0) {
        dma_mode = DMA_BLOCK;
    } else if (mode == 1) {
        dma_mode = DMA_LIST;
         OSI_Memset(dst_buff, 0, DMA_LIST_SIZE * DMA_CMD_LIST_CNT);
    } else {
    return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}



int dma_init_cmd(Shell *shell, int argc, char *argv[])
{
    if (init_aready) {
        shellPrint(shell, "dma init aready\n\r");
        return HAL_STATUS_ERROR;
    }

    if (dma_mode == DMA_BLOCK) {

        if (argc < 2) {
            return HAL_STATUS_INVALID_PARAM;
        }
        g_blockcfg.blockSize = strtoul(argv[1], NULL, 10);

        /* Init semaphore */
        OSI_SemaphoreCreateBinary(&doneSem);

        pdmaDev = HAL_DMA_Init(DMA0_ID);
        if (NULL == pdmaDev) {
            shellPrint(shell, "dma init fail...\n\r");
            return HAL_STATUS_ERROR;
        }

        dma_src_buff_big = (uint8_t *)OSI_Malloc(g_blockcfg.blockSize + ALIGNMENT_DMA - 1);
        dma_dst_buff_big = (uint8_t *)OSI_Malloc(g_blockcfg.blockSize + ALIGNMENT_DMA - 1);

        dma_src_buff = (uint8_t *)((uintptr_t)(dma_src_buff_big + ALIGNMENT_DMA - 1) & ~(uintptr_t)(ALIGNMENT_DMA - 1));
        shellPrint(shell, "dma_src_buff--0x%08X\n\r", (uint32_t)dma_src_buff);
        dma_dst_buff = (uint8_t *)((uintptr_t)(dma_dst_buff_big + ALIGNMENT_DMA - 1) & ~(uintptr_t)(ALIGNMENT_DMA - 1));
        shellPrint(shell, "dma_dst_buff--0x%08X\n\r", (uint32_t)dma_dst_buff);

        init_aready = true;
    } else if (dma_mode == DMA_LIST) {
        /* Init semaphore */
        OSI_SemaphoreCreateBinary(&doneSem);

        pdmaDev = HAL_DMA_Init(DMA0_ID);
        if (NULL == pdmaDev) {
            shellPrint(shell, "dma init fail...\n\r");
            return HAL_STATUS_ERROR;
        }

        init_aready = true;
    }

    return HAL_STATUS_OK;
}


int dma_deinit_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status;

    status  =  HAL_DMA_DeInit(pdmaDev);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma deinit fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    }  else {
        shellPrint(shell, "dma deinit success\n\r");
    }

    OSI_SemaphoreDelete(&doneSem);

    if (dma_mode == DMA_BLOCK) {
        memset(dma_src_buff_big, 0x01, g_blockcfg.blockSize + ALIGNMENT_DMA - 1);
        memset(dma_dst_buff_big, 0x00, g_blockcfg.blockSize + ALIGNMENT_DMA - 1);

        OSI_Free((void *)dma_src_buff_big);
        OSI_Free((void *)dma_dst_buff_big);
        dma_src_buff_big = NULL;
        dma_dst_buff_big = NULL;
    } else if(dma_mode == DMA_LIST) {
        memset(dst_buff, 0x01, DMA_LIST_SIZE * DMA_CMD_LIST_CNT * sizeof(uint8_t));
        memset(src_buff, 0x00, DMA_LIST_SIZE * DMA_CMD_LIST_CNT * sizeof(uint8_t));
    }

    init_aready = false;

    return HAL_STATUS_OK;
}

int dma_init_list_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status       =  HAL_STATUS_OK;
    OSI_Memset(Node, 0, sizeof(DMA_NodeConf_t) * DMA_CMD_LIST_CNT);

    for (int i = 0; i < DMA_CMD_LIST_CNT; i++) {
        Node[i].srcAddr     =   (uint32_t)(&src_buff[i][0]);
        Node[i].dstAddr     =   (uint32_t)(&dst_buff[DMA_LIST_SIZE * i]);
        Node[i].blockSize   =   DMA_LIST_SIZE;

        status = HAL_DMA_InitNodeCtrl(pdmaDev, g_chid, &Node[i], true);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "init list node_%d fail (%d)\n", i, status);
            return -1;
        }

        shellPrint(shell, "  node_%d(0x%08X) src_addr:0x%08X\n",
                i, (uint32_t)&Node[i], (uint32_t)Node[i].srcAddr);
        shellPrint(shell, "  node_%d(0x%08X) dst_addr:0x%08X\n\n",
                i, (uint32_t)&Node[i], (uint32_t)Node[i].dstAddr);
        //shellPrint(shell, "Add node(0x%04X) into node(0x%04X)\n\n",
                //(uint32_t)&Node[i + 1], (uint32_t)&Node[i]);
    }

    for (int i = 0; i < DMA_CMD_LIST_CNT - 1U; i++) {
        HAL_DMA_AddNextNode(&Node[i], &Node[i + 1]);
    }

    status = HAL_DMA_ConfigListTranfer(pdmaDev, g_chid, Node);
    if ( status != HAL_STATUS_OK ) {
        shellPrint(shell, "config list tranfer(%d)\n", status);
        return -2;
    }

    return HAL_STATUS_OK;
}

int dma_open_chan_cmd(Shell *shell, int argc, char *argv[])
{
    int32_t ret_val = 0;

    ret_val = HAL_DMA_Open(pdmaDev, &g_chancfg);
    if (ret_val < 0) {
        shellPrint(shell, "dma open channel fail (%d)\n\r", ret_val);
        return HAL_STATUS_ERROR;
    } else {
        g_chid = ret_val;
        shellPrint(shell, "dma open ch_(%d) success\n\r", g_chid);
    }

    return HAL_STATUS_OK;
}

int dma_close_chan_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_ERROR;

    status = HAL_DMA_Close(pdmaDev, g_chid);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma close chanenl fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    } else {
        shellPrint(shell, "dma close chanenl success\n\r");
    }

    status = HAL_DMA_UnRegisterIRQ(pdmaDev, g_chid, DMA_EVENT_ALL);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma unregister channel irq fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    } else {
        shellPrint(shell, "dma unregister channel irq success\n\r");
    }

    status = HAL_DMA_DisableIRQ(pdmaDev, g_chid, DMA_EVENT_ALL);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma disable channel irq fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    } else {
        shellPrint(shell, "dma disable channel irq success\n\r");
    }

    return HAL_STATUS_OK;
}

int dma_conf_trans_cmd(Shell *shell, int argc, char *argv[])
{
    if (!init_aready) {
        shellPrint(shell, "dma no init \n");
        return HAL_STATUS_ERROR;
    }

    if (argc < 5) {
        shellPrint(shell, "Config Params\n\r"
                          "<s_width><d_width>\n\r"
                          "<s_burst><d_burst>\n\r");
        return HAL_STATUS_INVALID_PARAM;
    }

    if (dma_mode == DMA_BLOCK) {
        g_blockcfg.srcAddr           =   (uint32_t)dma_src_buff;
        g_blockcfg.dstAddr           =   (uint32_t)dma_dst_buff;
        //g_blockcfg.blockSize         =   strtoul(argv[1], NULL, 10);  //4096
    }

    g_chancfg.direction          =   DMA_DIRECTION_MEM2MEM;
    g_chancfg.priority           =   DMA_PRIORITY_0;
    g_chancfg.srcWidth           =   strtoul(argv[1], NULL, 10);  //2
    g_chancfg.dstWidth           =   strtoul(argv[2], NULL, 10);  //2
    g_chancfg.srcBrustNum        =   strtoul(argv[3], NULL, 10);  //2
    g_chancfg.dstBrustnum        =   strtoul(argv[4], NULL, 10);  //2
    g_chancfg.srcIncreMode       =   0;//strtoul(argv[6], NULL, 10);
    g_chancfg.dstIncreMode       =   0;//strtoul(argv[7], NULL, 10);
    g_chancfg.srcEnReload        =   0;
    g_chancfg.dstEnReload        =   0;
    g_chancfg.srcEnStatus        =   0;
    g_chancfg.dstEnStatus        =   0;

    shellPrint(shell, "dma config success          \n\r");
    shellPrint(shell, "dma_tran_size         0x%08x\n\r", g_blockcfg.blockSize);
    shellPrint(shell, "dma_src_buff          0x%08x\n\r", (uint32_t)dma_src_buff);
    shellPrint(shell, "dma_dst_buff          0x%08x\n\r", (uint32_t)dma_dst_buff);
    shellPrint(shell, "dma_src_wdith         0x%08x\n\r", g_chancfg.srcWidth);
    shellPrint(shell, "dma_dst_wdith         0x%08x\n\r", g_chancfg.dstWidth);
    shellPrint(shell, "dma_src_burst         0x%08x\n\r", g_chancfg.srcBrustNum);
    shellPrint(shell, "dma_dst_burst         0x%08x\n\r", g_chancfg.dstBrustnum);
    shellPrint(shell, "dma_src_inc_addr          %d\n\r", g_chancfg.srcIncreMode);
    shellPrint(shell, "dma_dst_inc_addr          %d\n\r", g_chancfg.dstIncreMode);

    return HAL_STATUS_OK;
}

int dma_start_trans_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t   status  =   HAL_STATUS_ERROR;
    HAL_Callback_t dma_done_cb  = { .cb = NULL, .arg = NULL };

    if (dma_mode == DMA_BLOCK) {
        status = HAL_DMA_ConfigBlockTranfer(pdmaDev, g_chid, &g_blockcfg);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "dma config block fail (%d)\n", status);
            return HAL_STATUS_ERROR;
        }
    }

    dma_done_cb.cb  = test_done;
    dma_done_cb.arg = &doneSem;
    status = HAL_DMA_RegisterIRQ(pdmaDev, g_chid, DMA_EVENT_DONE, &dma_done_cb);

    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma register channel irq fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    }

    status = HAL_DMA_EnableIRQ(pdmaDev, g_chid, DMA_EVENT_DONE);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma enable channel irq fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    }

    status = HAL_DMA_StartTransfer(pdmaDev, g_chid);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma start channel fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    }

    OSI_Status_t osiStatus = OSI_SemaphoreWait(&doneSem, 2000);
    if (osiStatus != OSI_STATUS_OK) {
        shellPrint(shell, "semaphore waiting timeout\n\r");
        return HAL_STATUS_ERROR;
    }

    if (dma_mode == DMA_BLOCK) {
        csi_dcache_invalid_range((unsigned long *)dma_src_buff, g_blockcfg.blockSize);
        csi_dcache_invalid_range((unsigned long *)dma_dst_buff, g_blockcfg.blockSize);

        if (0 != HAL_Memcmp(dma_src_buff, dma_dst_buff, g_blockcfg.blockSize)) {
            shellPrint(shell, "dma cmp memory fail\n");
            return HAL_STATUS_ERROR;
        } else {
            shellPrint(shell, "dma cmp memory success\n");
            return HAL_STATUS_OK;
        }
    }

    if (dma_mode == DMA_LIST) {
        csi_dcache_invalid_range((unsigned long *)src_buff, DMA_LIST_SIZE * DMA_CMD_LIST_CNT);
        csi_dcache_invalid_range((unsigned long *)dst_buff, DMA_LIST_SIZE * DMA_CMD_LIST_CNT);
    #if 0
        for (uint32_t j = 0; j < DMA_CMD_LIST_CNT; j++) {
            for (uint32_t idx = 0; idx < DMA_LIST_SIZE; idx++) {
                if(src_buff[j][idx] != dst_buff[j * 512 + idx]) {
                    shellPrint(shell, "src_buff[%d][%d]:0x%02x  dst_buff[%d]:0x%02x \n",
                        j, idx, src_buff[j][idx],
                        j * 512 + idx , dst_buff[j * 512 + idx]);
                        shellPrint(shell, "dma cmp memory fail\n");
                        return HAL_STATUS_ERROR;
                }
            }
        }
        shellPrint(shell, "dma cmp memory success\n");
        return HAL_STATUS_OK;
    #endif
        if (0 != HAL_Memcmp(src_buff, dst_buff, DMA_LIST_SIZE * DMA_CMD_LIST_CNT)) {
            shellPrint(shell, "dma cmp memory fail\n");
            return HAL_STATUS_ERROR;
        } else {
            shellPrint(shell, "dma cmp memory success\n");
            return HAL_STATUS_OK;
        }
    }

    return HAL_STATUS_OK;
}

int dma_abort_trans_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t  status  =   HAL_STATUS_ERROR;

    status = HAL_DMA_AbortTransfer(pdmaDev, g_chid);
    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "dma abort channel fail (%d)\n\r", status);
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

int32_t dma_auto_test_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc != 5) {
        shellPrint(shell, "param cnt(%d) err\n\r", argc);
        return HAL_STATUS_INVALID_PARAM;
    }

    uint32_t test_num    = strtoul(argv[1], NULL, 10);
    uint32_t base_ch_id  = strtoul(argv[2], NULL, 10);
    uint32_t base_ch_inc = strtoul(argv[3], NULL, 10);
    uint32_t test_ch_num = strtoul(argv[4], NULL, 10);

    uint32_t  curr_ch_id = 0;
    OSI_Status_t osistatus;
    HAL_Status_t status;
    HAL_Callback_t dma_done_cb = { .cb = NULL, .arg = NULL };

    shellPrint(shell, "idx[%d] inc[%d] total[%d]", base_ch_id, base_ch_inc, test_ch_num);

    if ((base_ch_id + (test_ch_num * base_ch_inc)) > 8) {
        shellPrint(shell, "ERROR: ch over");
        return HAL_STATUS_ERROR;
    }

    for (int i = base_ch_id; i < test_ch_num; i++, curr_ch_id += base_ch_inc) {
        OSI_SemaphoreCreate(&g_dma_transfer_sem[curr_ch_id], 0, 1);

        g_blockcfg.srcAddr = (uint32_t)(dma_src_buff + (curr_ch_id * g_blockcfg.blockSize));
        g_blockcfg.dstAddr = (uint32_t)(dma_dst_buff + (curr_ch_id * g_blockcfg.blockSize));

        int32_t ret = HAL_DMA_Open(pdmaDev, &g_chancfg);
        if (ret < 0) {
            shellPrint(shell, "dma open channel fail (%d)\n\r", ret);
            return HAL_STATUS_ERROR;
        } else {
            curr_ch_id = ret;
            shellPrint(shell, "dma open channel%d success(%d)\n\r", curr_ch_id);
        }

        status = HAL_DMA_ConfigBlockTranfer(pdmaDev, curr_ch_id, &g_blockcfg);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "dma config block mode fail (%d)\n\r",  status);
            return HAL_STATUS_ERROR;
        } else {
            shellPrint(shell, "dma config block mode success\n\r");
        }

        dma_done_cb.cb  = test_done;
        dma_done_cb.arg = &doneSem;
        status = HAL_DMA_RegisterIRQ(pdmaDev, curr_ch_id, DMA_EVENT_DONE, &dma_done_cb);
        if (status != HAL_STATUS_OK) {
           shellPrint(shell, "dma register channel irq fail (%d)\n\r", status);
           return HAL_STATUS_ERROR;
        }

        status = HAL_DMA_EnableIRQ(pdmaDev, curr_ch_id, DMA_EVENT_DONE);
        if (status != HAL_STATUS_OK) {
            shellPrint(shell, "dma enable channel irq fail (%d)\n\r", status);
            return HAL_STATUS_ERROR;
        }

        g_dma_loop_obj[curr_ch_id].ch_id = curr_ch_id;
        g_dma_loop_obj[curr_ch_id].test_num = test_num;
        g_dma_loop_obj[curr_ch_id].dma_transfer_sem = &g_dma_transfer_sem[curr_ch_id];
        osistatus = OSI_ThreadCreate(&g_dma_loop_thread[curr_ch_id], "dma_test_thread",
                                        dma_channel_loop_do, (void *)&g_dma_loop_obj[curr_ch_id],
                                        OSI_PRIORITY_NORMAL, 64);
        if (osistatus != OSI_STATUS_OK) {
            shellPrint(shell, "thread created fail\n\r", osistatus);
            return HAL_STATUS_ERROR;
        }

    }

    return HAL_STATUS_OK;
}

int dma_version_cmd(Shell *shell, int argc, char *argv[])
{
    shellPrint(shell, "dma version:1.2 \n");
    return HAL_STATUS_OK;
}



ShellCommand dma_cmds[] = {
    SHELL_CMD_ITEM(mode_choose,  dma_mode_cmd,           "<mode> 0:block 1:list "),
    SHELL_CMD_ITEM(init,         dma_init_cmd,           "[NULL]: Init dma device"),
    SHELL_CMD_ITEM(deinit,       dma_deinit_cmd,         "[NULL]: Deinit dma device"),
    SHELL_CMD_ITEM(init_list,    dma_init_list_cmd,      ""),
    SHELL_CMD_ITEM(config,       dma_conf_trans_cmd,     "[size][s_width][d_width][s_burst][d_brust]"),
    SHELL_CMD_ITEM(open,         dma_open_chan_cmd,      "[NULL]: Open channel"),
    SHELL_CMD_ITEM(close,        dma_close_chan_cmd,     "[NULL]: Close channel"),
    SHELL_CMD_ITEM(start,        dma_start_trans_cmd,    "[NULL]: Start transfer of open channel"),
    SHELL_CMD_ITEM(stop,         dma_abort_trans_cmd,    "[NULL]: Abort transfer of open channel"),
    SHELL_CMD_ITEM(setmap,       dma_set_map_cmd,        "[idx][fir_data][cnt][inc_val]: Set map memory"),
    SHELL_CMD_ITEM(reset,        dma_reset_map_cmd,      "[num]: Reset num of map memory(0x00)"),
    SHELL_CMD_ITEM(view,         dma_view_map_cmd,       "[num]: View num of map memory"),
    SHELL_CMD_ITEM(auto,         dma_auto_test_cmd,      "{test_cnt][base_id][ch_inc][test_ch_num]: Auto test channel"),
    SHELL_CMD_ITEM(version,      dma_version_cmd,        ""),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(dma, dma_cmds, "dma peripheral test command");

#endif
/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
