/**
 ******************************************************************************
 * @file    board.c
 * @brief   board define.
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

#include <config.h>
#include <soc.h>
#include "board.h"
#include "hal_types.h"
#include "hal_dev.h"
#include "hal_clock.h"
#include "ll_splk.h"
#include "version.h"

#include "hal_common.h"
#include "hal_board.h"


#ifndef CONFIG_BOARD_LOG_LEVEL
#define CONFIG_BOARD_LOG_LEVEL          LEVEL_ERR
#endif

#define LOG_MODULE                      "BOARD"
#define LOG_LEVEL                       CONFIG_BOARD_LOG_LEVEL
#include "log.h"

#if (CONFIG_PM)
#include "pm.h"
#endif

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_BANNER_ENABLE
#define CONFIG_BANNER_ENABLE                                1
#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
static uint8_t efuseState;
static uint8_t xipState;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
extern HAL_Status_t BOARD_InitPowerClock(void);
extern HAL_Status_t BOARD_InitXIP(void);
extern HAL_Status_t BOARD_InitPinMux(void);
extern HAL_Status_t OSI_Init(void);
extern void HAL_FLASH_PreInit(void);
extern int KVR_Init(void);
extern int KVF_Init(void);

extern int BOARD_InitEarlyLog(void);
extern int BOARD_InitLogIO(void);
extern int BOARD_InitLog(void);
extern int BOARD_InitShell(void);

static HAL_Status_t SwitchToMainThread(void * mainThread);


#if CONFIG_BANNER_ENABLE
static void board_Banner(void);
#endif

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
__attribute__((weak)) int main(void)
{
     return 0;
}

__attribute__((weak)) void BOARD_SetupDone(void)
{

}

static void mainThread(void *arg)
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        LOG_DBG("init pinmux");
        status = BOARD_InitPinMux();
        if (status != HAL_STATUS_OK) {
            LOG_ERR("init pinmux fail %d", status);
            break;
        }

        /* xip and flash init */
#if CONFIG_DRIVER_FLASH
        LOG_DBG("init xip");
        status = BOARD_InitXIP();
        if (status != HAL_STATUS_OK) {
            if (status == HAL_STATUS_BUSY) {
                xipState = 1;
                status = HAL_STATUS_OK;
            } else {
                LOG_ERR("init xip fail %d", status);
                break;
            }
        }
#endif

#if (CONFIG_PM)
       LOG_DBG("init pm");
       pm_policy_wake_lock_init();
#endif

        /* kvf init */
#if CONFIG_DRIVER_FLASH
        LOG_DBG("init KVF");
        status = KVF_Init();
        if (status != HAL_STATUS_OK) {
            LOG_ERR("init KVF fail %d", status);
            break;
        }
#endif

        /* console and shell init */
        LOG_DBG("init log");
        status = BOARD_InitLog();
        if (status != HAL_STATUS_OK) {
            LOG_ERR("init log fail %d", status);
            break;
        }

        LOG_DBG("init shell");
        status = BOARD_InitShell();
        if (status != HAL_STATUS_OK) {
            LOG_ERR("init shell fail %d", status);
            break;
        }

#if CONFIG_BANNER_ENABLE
        board_Banner();
#endif

        BOARD_SetupDone();

        LOG_DBG("goto main");
        status = main();

    } while (0);

    if (status != HAL_STATUS_OK) {
        LOG_ERR("board after init fail %d", status);
        HAL_BOARD_Reset(HAL_RESET_SYS);
        while (1);
    } else {
        OSI_ThreadDelete(NULL);
    }
}


void BOARD_Init(void)
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        /* Debug initialization during the system startup phse */
        BOARD_InitEarlyLog();
        BOARD_InitLogIO();

        LOG_DBG("board init start");
        /* Driver initialization during the system pre-startup phse
         * include splk and xip pre-init
         */
        LL_SPLK_Init();

#if CONFIG_DRIVER_FLASH
        LOG_DBG("preinit flash");
        HAL_FLASH_PreInit();
        /*  */
        __DSB();
        __ISB();
#endif
        LOG_DBG("init dev");
        HAL_DEV_Init();

        /* key-value ram init */
        LOG_DBG("init kvr");
        status = KVR_Init();
        if (status != HAL_STATUS_OK) {
            LOG_ERR("init KVR fail %d", status);
            break;
        }

#if CONFIG_DRIVER_EFUSE
        LOG_DBG("init EFUSE");
        HAL_Dev_t * efuseDev = HAL_EFUSE_Init();
        if (efuseDev == NULL) {
            status = HAL_STATUS_ERROR;
            LOG_ERR("init EFUSE fail %d", status);
            efuseState = 1;
//            break;
        }
#endif

        LOG_DBG("init power and clock");
        status = BOARD_InitPowerClock();
        if (status != HAL_STATUS_OK) {
            LOG_ERR("Init clock fail(%d)", status);
            break;
        }

        LOG_DBG("start kernel");
        status = SwitchToMainThread(mainThread);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("Switch to main thread fail(%d)", status);
            break;
        }

    } while (0);

    if (status != HAL_STATUS_OK) {
        LOG_ERR("board prev init fail %d", status);
        HAL_BOARD_Reset(HAL_RESET_SYS);
    }

    while (1) {
        /* not run here */
    }
}

static HAL_Status_t SwitchToMainThread(void * mainThread)
{
    HAL_Status_t status = HAL_STATUS_OK;
    int prio = configMAX_PRIORITIES;

    status = OSI_Init();
    if (status != HAL_STATUS_OK) {
        return status;
    }

    OSI_Thread_t xHandle;
    OSI_ThreadSetInvalid(&xHandle);
    OSI_Status_t ret = OSI_ThreadCreate(&xHandle,
                        "main thread",
                        mainThread,
                        NULL,
                        prio,
                        CONFIG_INIT_TASK_STACK_SIZE);
    if (ret != OSI_STATUS_OK) {
        LOG_ERR("create main thread fail %d\n", ret);
        return HAL_STATUS_ERROR;
    }

    OSI_ThreadStartScheduler();

    return HAL_STATUS_OK;
}

#if CONFIG_BANNER_ENABLE
static void board_Banner(void)
{
    uint32_t cpu_freq = 0;
    HAL_Dev_t *pClockDevice = HAL_DEV_Find(HAL_DEV_TYPE_PWR_CLK, 0);
    if (pClockDevice != NULL) {
        cpu_freq = HAL_CLOCK_GetFreq(pClockDevice, CLOCK_CURR_CPU);
    }

    LOG_PRINT("\n\n\n");
    LOG_PRINT("Welcome to use POSSUMIC Technology products\n");
    LOG_PRINT("-------------------------------------------\n");
    LOG_PRINT("board: %s\n", CONFIG_BOARD_STRING);
    LOG_PRINT("cpu freq: %d\n", cpu_freq);
    LOG_PRINT("version: R%d.%d M%d.%d.%d V%d.%d.%d\n", 
        VERSION_R_TYPE, VERSION_R_MINOR,
        VERSION_M_PROJECT, VERSION_M_NUMBER, VERSION_M_MINOR,
        VERSION_V_MAJOR, VERSION_V_MINOR, VERSION_V_REVISION);
    char buildDate[16];
    char buildTime[16];
    VER_GetBuildTime(buildDate, buildTime);
    LOG_PRINT("time: %s %s\n", buildDate, buildTime);
    if (efuseState) {
        LOG_PRINT("\nEFUSE Not init\n");
    }
    if (xipState) {
        LOG_PRINT("\nFLASH is not in the flash list\n");
    }
    LOG_PRINT("-------------------------------------------\n\n");
}
#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
