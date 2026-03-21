/**
 ******************************************************************************
 * @file    cmd_mco.c
 * @brief   cmd mco define.
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
#include "hal_clock.h"

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_CLK == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Private macros.
 * ----------------------------------------------------------------------------
 */
const char * clk_str[CLOCK_MAX] = {
    "lsi",
    "msi",
    "lse",
    "dcxo",
    "lpclk",
    "lpclk_inte",
    "lpclk_frac",
    "pll",
    "pll_soc",
    "pll_dev",
    "pll_dev1",
    "pll_cdaa",
    "sys",
    "ahb",
    "apb0",
    "apb1",
    "cpu_curr",
    "cpus",
    "cpuf",
};

const uint8_t mco_to_clk_tab[CLOCK_MCO_SRC_MAX] = {
    CLOCK_MSI,          /* msi */
    CLOCK_DCXO,         /* dcxo */
    CLOCK_MAX,          /* m50 */
    CLOCK_LSI,          /* lsi */
    CLOCK_PLL_DEV,      /* pll_dev */
    CLOCK_PLL_DEV1,     /* pll_dev1 */
    CLOCK_PLL_SOC,      /* pll_soc */
    CLOCK_AHB,          /* ahb */
    CLOCK_LSE,          /* lse */
    CLOCK_MAX,          /* lse_m50 */
    CLOCK_PLL_CDAA,     /* pll_cdaa */
};

const char * mco_str[CLOCK_MCO_SRC_MAX] = {
    "msi",
    "dcxo",
    "dcxo_m50",
    "lsi",
    "pll_dev",
    "pll_dev1",
    "pll_soc",
    "ahb",
    "lse",
    "lse_m50",
    "pll_cdaa",
};


/* Private variables.
 * ----------------------------------------------------------------------------
 */
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int clk_get_cmd(Shell *shell, int argc, char *argv[])
{
    if (argc == 2) {
        HAL_Dev_t * pclkDev = HAL_DEV_Find(HAL_DEV_TYPE_PWR_CLK, 0);
        if (pclkDev == NULL) {
            shellPrint(shell, "hal clock not init\n");
            return HAL_STATUS_ERROR;
        }

        for (uint8_t clkid = 0; clkid < CLOCK_MAX; clkid++) {
            if (!strcmp(argv[1], clk_str[clkid])) {
                uint32_t freq = HAL_CLOCK_GetFreq(pclkDev, (CLOCK_Type_t)clkid);
                shellPrint(shell, "clock %s freq: %d Hz (%d MHz)\n", clk_str[clkid], freq, freq / 1000000);
                break;
            }
        }
    } else {
        shellPrint(shell, "get clk. clk value:\n\r");

        for (uint8_t idx = 0; idx < CLOCK_MAX; idx++) {
            shellPrint(shell, "\t%s\n\r", clk_str[idx]);
        }

        return HAL_STATUS_INVALID_PARAM;
    }

    return HAL_STATUS_OK;
}

int clk_mco_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 3) {
            uint8_t div_tab[CLOCK_MCO_SRC_DIVMAX] = {1, 2, 4, 8, 16};
            uint8_t div = strtoul(argv[2], NULL, 10);
            uint8_t div_id = 0;
            for (div_id = 0; div_id < CLOCK_MCO_SRC_DIVMAX; div_id++) {
                if (div_tab[div_id] == div) {
                    break;
                }
            }

            if (div_id == CLOCK_MCO_SRC_DIVMAX) {
                status = HAL_STATUS_ERROR;
                break;
            }

            for (uint8_t mcoid = 0; mcoid < CLOCK_MCO_SRC_MAX; mcoid++) {
                if (!strcmp(argv[1], mco_str[mcoid])) {

                    status = HAL_CLOCK_EnableMCO(mcoid, div_id);
                    if (status != HAL_STATUS_OK) {
                        break;
                    }
                    uint8_t clk_id = mco_to_clk_tab[mcoid];
                    if (clk_id < CLOCK_MAX) {
                        HAL_Dev_t * pclkDev = HAL_DEV_Find(HAL_DEV_TYPE_PWR_CLK, 0);
                        if (pclkDev == NULL) {
                            shellPrint(shell, "hal clock not init\n");
                            return HAL_STATUS_ERROR;
                        }
                        uint32_t freq = HAL_CLOCK_GetFreq(pclkDev, (CLOCK_Type_t)clk_id);
                        shellPrint(shell, "clock %s freq: %d Hz (%d MHz), mco: %d Hz\n", 
                            clk_str[clk_id], freq, freq / 1000000,
                            freq / div_tab[div_id]);
                    }
                    break;
                }
            }

        } else if (argc == 2) {
            if (!strcmp(argv[1], "dis")) {
                status = HAL_CLOCK_DisableMCO();
            } else {
                status = HAL_STATUS_INVALID_PARAM;
            }
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);


    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "clk mco dis: disable clock output\n\r");
        shellPrint(shell, "clk mco <clk> <div>. output clk:\n\r");
        for (uint8_t mcoid = 0; mcoid < CLOCK_MCO_SRC_MAX; mcoid++) {
            shellPrint(shell, "\t%s\n\r", mco_str[mcoid]);
        }

        shellPrint(shell, "mco. div value: 1/2/4/8/16\n\r");

    }

    return status;
}

ShellCommand clk_cmds[] = {
    SHELL_CMD_ITEM(get,      clk_get_cmd,     "<clk> get clock freq"),
    SHELL_CMD_ITEM(mco,      clk_mco_cmd,     "[clk/dis] [div] output clock"),
    SHELL_CMD_ITEM_END()
};

SHELL_CMD_GROUP(clk, clk_cmds, "clock command");

#endif /* CONFIG_SHELL_CMD_CLK */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
