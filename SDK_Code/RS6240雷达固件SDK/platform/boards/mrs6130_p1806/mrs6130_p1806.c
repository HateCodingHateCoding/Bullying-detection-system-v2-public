/**
 ******************************************************************************
 * @file    mrs6130_p1806.c
 * @brief   vmrs6130 p1806 define.
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

#include "soc_config.h"

#if (CONFIG_BOARD_MRS6130_P1806)

#include "hal_types.h"
#include "hal_dev.h"
#include "hal_board.h"

#include "hal_power.h"
#include "hal_clock.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "hal_spi.h"
#include "hal_i2c.h"
#include "hal_flash.h"
#include "ll_rcc_dev.h"
#include "ll_utils.h"
#include "log_port.h"

#if CONFIG_SHELL
#include "shell.h"
#endif

#ifndef CONFIG_BOARD_LOG_LEVEL
#define CONFIG_BOARD_LOG_LEVEL          LEVEL_ERR
#endif

#define LOG_MODULE                      "1806"
#define LOG_LEVEL                       CONFIG_BOARD_LOG_LEVEL
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */


/* Private constants.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_DRIVER_UART
DEFINE_PINMUX_WEAK(uart0, 2,
    { GPIO_PORT_B, GPIO_PIN_0,  {GPIOB_P0_F2_UART0_TX,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* TX */
    { GPIO_PORT_B, GPIO_PIN_1,  {GPIOB_P1_F2_UART0_RX,   GPIO_DRIVING_LEVEL_1, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* RX */
);

DEFINE_PINMUX_WEAK(uart0_deinit, 2,
    { GPIO_PORT_B, GPIO_PIN_0,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* TX */
    { GPIO_PORT_B, GPIO_PIN_1,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_1, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* RX */
);


DEFINE_PINMUX_WEAK(uart1, 2,
    { GPIO_PORT_A, GPIO_PIN_0,  {GPIOA_P0_F3_UART1_TX,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* TX */
    { GPIO_PORT_A, GPIO_PIN_1,  {GPIOA_P1_F3_UART1_RX,   GPIO_DRIVING_LEVEL_1, GPIO_OPEN_DRAIN,GPIO_PULL_UP } }, /* RX */
);

DEFINE_PINMUX_WEAK(uart1_deinit, 2,
    { GPIO_PORT_A, GPIO_PIN_0,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* TX */
    { GPIO_PORT_A, GPIO_PIN_1,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_1, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* RX */
);


DEFINE_PINMUX_WEAK(uart2, 2,
    { GPIO_PORT_A, GPIO_PIN_2,  {GPIOA_P2_F5_UART2_TX,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* TX */
    { GPIO_PORT_A, GPIO_PIN_3,  {GPIOA_P3_F5_UART2_RX,   GPIO_DRIVING_LEVEL_1, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* RX */
);

DEFINE_PINMUX_WEAK(uart2_deinit, 2,
    { GPIO_PORT_A, GPIO_PIN_2,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* TX */
    { GPIO_PORT_A, GPIO_PIN_3,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_1, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* RX */
);

#endif

#if CONFIG_DRIVER_SPI
DEFINE_PINMUX_WEAK(spi0, 4,
	{ GPIO_PORT_A, GPIO_PIN_0,  {GPIOA_P0_F2_SPI_CS,   GPIO_DRIVING_LEVEL_0, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* CS */
    { GPIO_PORT_A, GPIO_PIN_1,  {GPIOA_P1_F2_SPI_CLK,   GPIO_DRIVING_LEVEL_3, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* CLK */
    { GPIO_PORT_A, GPIO_PIN_2,  {GPIOA_P2_F2_SPI_MOSI,   GPIO_DRIVING_LEVEL_3, GPIO_PUSH_PULL, GPIO_PULL_UP } },/* MOSI */
    { GPIO_PORT_A, GPIO_PIN_3,  {GPIOA_P3_F2_SPI_MISO,   GPIO_DRIVING_LEVEL_3, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* MISO */
);

DEFINE_PINMUX_WEAK(spi0_deinit, 4,
	{ GPIO_PORT_A, GPIO_PIN_0,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* CS */
    { GPIO_PORT_A, GPIO_PIN_1,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* CLK */
    { GPIO_PORT_A, GPIO_PIN_2,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_PUSH_PULL, GPIO_PULL_UP } },/* MOSI */
    { GPIO_PORT_A, GPIO_PIN_3,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* MISO */
);


DEFINE_PINMUX_WEAK(spi0_cs, 1,
    { GPIO_PORT_A, GPIO_PIN_0,  {GPIOx_Pn_F1_OUTPUT,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* CS */
);

DEFINE_PINMUX_WEAK(spi0_cs_deinit, 1,
    { GPIO_PORT_A, GPIO_PIN_0,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_1, GPIO_PUSH_PULL, GPIO_PULL_UP } }, /* CS */
);

#endif

#if CONFIG_DRIVER_I2C
DEFINE_PINMUX_WEAK(i2c0, 2,
    { GPIO_PORT_A, GPIO_PIN_4,  {GPIOA_P4_F3_TWI0_SDA,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SDA */
    { GPIO_PORT_A, GPIO_PIN_5,  {GPIOA_P5_F3_TWI0_SCL,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SCL */
);

DEFINE_PINMUX_WEAK(i2c0_deinit, 2,
    { GPIO_PORT_A, GPIO_PIN_4,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SDA */
    { GPIO_PORT_A, GPIO_PIN_5,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SCL */
);


DEFINE_PINMUX_WEAK(i2c1, 2,
    { GPIO_PORT_A, GPIO_PIN_10,  {GPIOA_P10_F2_TWI1_SDA,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SDA */
    { GPIO_PORT_A, GPIO_PIN_11,  {GPIOA_P11_F2_TWI1_SCL,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SCL */
);

DEFINE_PINMUX_WEAK(i2c1_deinit, 2,
    { GPIO_PORT_A, GPIO_PIN_10,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SDA */
    { GPIO_PORT_A, GPIO_PIN_11,  {GPIOx_Pn_F15_DIS,   GPIO_DRIVING_LEVEL_0, GPIO_OPEN_DRAIN, GPIO_PULL_UP } }, /* SCL */
);

#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */


/* Private variables.
 * ----------------------------------------------------------------------------
 */
static HAL_Dev_t *pBoardDev_Clock;
static HAL_Dev_t *pBoardDev_Gpio[GPIO_PORT_NUM];
static uint32_t boardTickFreq;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

__keep_sram_text int32_t board_GetFreq(CLOCK_Type_t clkType);
__keep_sram_text uint32_t board_GetTime(HAL_TimeType_t timeType);
static HAL_Status_t board_SetPinMux(HAL_Pinmux_t * pConfig);
static HAL_Status_t board_GetPinMux(
                            HAL_DevType_t type,
                            uint8_t idx,
                            HAL_PinMuxType_t cfgType,
                            HAL_Pinmux_t * pPinMux);
__keep_sram_text void board_BlkDelay(uint32_t delayVal, HAL_TimeType_t timeType);
__keep_sram_text void board_Reset(HAL_ResetType_t resetType);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Status_t BOARD_InitPowerClock(void)
{
    HAL_Status_t status = HAL_STATUS_OK;

    status = HAL_POWER_Init();
    if (status != HAL_STATUS_OK) {
        return status;
    }

    pBoardDev_Clock = HAL_CLOCK_Init();

    LOG_DBG("board clock init");

    do {
        LOG_DBG("dcxo init");
        CLOCK_DcxoCfg_t DcxoCfg;
        DcxoCfg.enable = CONFIG_CLK_DCXO_EN;
        DcxoCfg.freq = CONFIG_CLK_DCXO_FREQ;
        DcxoCfg.out_dev = CONFIG_CLK_DCXO_OUT_DEV;
        DcxoCfg.out_pll = CONFIG_CLK_DCXO_OUT_PLL;

        status = HAL_CLOCK_ConfigDcxo(pBoardDev_Clock, &DcxoCfg);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("DCXO config fail");
            break;
        }


        LOG_DBG("msi lsi init");
        CLOCK_LsiMsiCfg_t LsiMsiCfg;
        LsiMsiCfg.lsiFreq = CONFIG_CLK_LSI_FREQ;
#if CONFIG_CLK_LSI_CALIB
        LsiMsiCfg.lsiCfgMode = CLOCK_LSIMSI_CFG_TRIM_CALIB | CLOCK_LSIMSI_CFG_FREQ_REFR;
#else
        LsiMsiCfg.lsiCfgMode = CLOCK_LSIMSI_CFG_TRIM_EFUSE | CLOCK_LSIMSI_CFG_FREQ_FIXED;
#endif
        LsiMsiCfg.msiFreq = CONFIG_CLK_MSI_FREQ;
#if CONFIG_CLK_MSI_CALIB
        LsiMsiCfg.msiCfgMode = CLOCK_LSIMSI_CFG_TRIM_CALIB | CLOCK_LSIMSI_CFG_FREQ_REFR;
#else
        LsiMsiCfg.msiCfgMode = CLOCK_LSIMSI_CFG_TRIM_EFUSE | CLOCK_LSIMSI_CFG_FREQ_FIXED;
#endif

        status = HAL_CLOCK_ConfigLsiMsi(pBoardDev_Clock, &LsiMsiCfg);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("lsimsi config fail %d", status);
            break;
        }

        LOG_DBG("pll init\n");
        CLOCK_PllCfg_t PllCfg;
        PllCfg.enable = CONFIG_CLK_PLL_EN;
        PllCfg.clkSrc = CONFIG_CLK_PLL_SRC;
        PllCfg.pllDiv = CONFIG_CLK_PLL_M_DIVISOR;
        PllCfg.pllMul = CONFIG_CLK_PLL_N_MULTIPLIER;
        PllCfg.socEnable = CONFIG_CLK_PLL_OUT_SOC;
        PllCfg.socDiv = CONFIG_CLK_PLL_OUT_SOC_DIVISOR;
        PllCfg.devEnable = CONFIG_CLK_PLL_OUT_DEV;
        PllCfg.devDiv = CONFIG_CLK_PLL_OUT_DEV_DIVISOR;
        PllCfg.dev1Enable = CONFIG_CLK_PLL_OUT_DEV1;
        PllCfg.dev1Div = CONFIG_CLK_PLL_OUT_DEV1_DIVISOR;
        PllCfg.cdaaEnable = CONFIG_CLK_PLL_OUT_CDAA;
        PllCfg.cdaaDiv = CONFIG_CLK_PLL_OUT_CDAA_DIVISOR;

        status = HAL_CLOCK_ConfigPll(pBoardDev_Clock, &PllCfg);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("pll config fail %d", status);
            break;
        }


        LOG_DBG("system clock init\n");
        CLOCK_SysCfg_t SysCfg;
        SysCfg.clkSrc = CONFIG_CLK_SYS_SRC;
        SysCfg.sysDiv = CONFIG_CLK_SYS_DIVISOR;
        SysCfg.ahbDiv = CONFIG_CLK_AHB_DIVISOR;
        SysCfg.apb0Div = CONFIG_CLK_APB0_DIVISOR;
        SysCfg.apb1Div = CONFIG_CLK_APB1_DIVISOR;

        status = HAL_CLOCK_ConfigSys(pBoardDev_Clock, &SysCfg);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("sys config fail %d", status);
            break;
        }


        LOG_DBG("lse init\n");
        CLOCK_LseCfg_t LseCfg;
        LseCfg.enable = CONFIG_CLK_LSE_EN;
        LseCfg.freq = CONFIG_CLK_LSE_FREQ;

        status = HAL_CLOCK_ConfigLse(pBoardDev_Clock, &LseCfg);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("lse config fail");
            break;
        }


        LOG_DBG("lp clock init\n");
        CLOCK_LpclkCfg_t LpclkCfg;
        LpclkCfg.clkSrc = CONFIG_CLK_LPCLK_SRC;
        LpclkCfg.inteEnable = CONFIG_CLK_LPCLK_INTR_EN;
        LpclkCfg.inteDiv = CONFIG_CLK_LPCLK_INTR_DIVISOR;
        LpclkCfg.fracEnable = CONFIG_CLK_LPCLK_FRAC_EN;
        LpclkCfg.fracQuotient = CONFIG_CLK_LPCLK_FRAC_QUOTIENT;
        LpclkCfg.fracRemainder = CONFIG_CLK_LPCLK_FRAC_REMAINDER;
        LpclkCfg.fracDenominator = CONFIG_CLK_LPCLK_FRAC_DENOMINATOR;

        status = HAL_CLOCK_ConfigLpclk(pBoardDev_Clock, &LpclkCfg);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("lpclk config fail");
            break;
        }

        status = HAL_CLOCK_SetupTimeInit(pBoardDev_Clock);
        if (status != HAL_STATUS_OK) {
            LOG_ERR("lpclk config fail");
            break;
        }

#if CONFIG_H2C_DISABLE
        LL_H2H_Close();
#endif

        boardTickFreq = HAL_CLOCK_GetFreq(pBoardDev_Clock, CLOCK_AHB);
        LL_RCC_TICK_SetPrescaler(0);
        LL_RCC_TICK_Reset();
        csi_vic_disable_irq(CORET_IRQn);


        status = HAL_BOARD_RegisterHandle(HAL_BOARD_HANDLE_GET_FREQ, board_GetFreq);
        if (status != HAL_STATUS_OK) {
            break;
        }

        status = HAL_BOARD_RegisterHandle(HAL_BOARD_HANDLE_GET_TIME, board_GetTime);
        if (status != HAL_STATUS_OK) {
            break;
        }

        status = HAL_BOARD_RegisterHandle(HAL_BOARD_HANDLE_BLK_DELAY, board_BlkDelay);
        if (status != HAL_STATUS_OK) {
            break;
        }

        status = HAL_BOARD_RegisterHandle(HAL_BOARD_HANDLE_RESET, board_Reset);
        if (status != HAL_STATUS_OK) {
            break;
        }
    } while (0);

    return status;
}

HAL_Status_t BOARD_InitXIP(void)
{
    HAL_Status_t status = HAL_STATUS_OK;
    HAL_Dev_t *pFlashDev = NULL;

    pFlashDev = HAL_FLASH_Init(FLASH_DEFT_ID);
    if (pFlashDev == NULL) {
        LOG_DBG("init fail");
        status = HAL_STATUS_ERROR;
    }

    uint32_t listMode = FLASH_LIST_TYPE_DEFAULT | FLASH_LIST_TYPE_LIST;
    status = HAL_FLASH_ExtControl(pFlashDev, FLASH_EXT_SET_CHIP_INFO_MODE, &listMode, sizeof(listMode));
    if (status != HAL_STATUS_OK) {
        LOG_DBG("set list mode fail");
        return status;
    }

    uint32_t freq = HAL_CLOCK_GetFreq(pBoardDev_Clock, CLOCK_SYS);
    FLASH_InitParam_t flashInitParam = {
        .clockSrc      = CONFIG_XIP_CLK_SRC,
        .clockFreq     = freq / CONFIG_XIP_CLK_DIV,
        .spiFormat     = FLASH_SPI_FORMAT_DUAL,
        .psramEnable   = false,
    };

    status = HAL_FLASH_Open(pFlashDev, &flashInitParam);
    if (status == HAL_STATUS_OK) {
        listMode = 0;
        status = HAL_FLASH_ExtControl(pFlashDev, FLASH_EXT_GET_USED_CHIP_INFO_MODE, &listMode, sizeof(listMode));
        if (status != HAL_STATUS_OK) {
            LOG_DBG("get used list mode fail");
            return status;
        }

        if (listMode == FLASH_LIST_TYPE_DEFAULT) {
            status = HAL_STATUS_BUSY;
        }
    }

    return status;
}

HAL_Status_t BOARD_InitPinMux(void)
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        for (uint8_t idx = 0; idx < GPIO_PORT_NUM; idx++) {
            pBoardDev_Gpio[idx] = HAL_GPIO_Init(idx);
            if (pBoardDev_Gpio[idx] == NULL) {
                status = HAL_STATUS_ERROR;
                break;
            }
        }

        status = HAL_BOARD_RegisterHandle(HAL_BOARD_HANDLE_GET_PINMUX, board_GetPinMux);
        if (status != HAL_STATUS_OK) {
            break;
        }

        status = HAL_BOARD_RegisterHandle(HAL_BOARD_HANDLE_SET_PINMUX, board_SetPinMux);
        if (status != HAL_STATUS_OK) {
            break;
        }
    } while (0);

    return HAL_STATUS_OK;
}

__keep_sram_text int32_t board_GetFreq(CLOCK_Type_t clkType)
{
    return HAL_CLOCK_GetFreq(pBoardDev_Clock, clkType);
}

__keep_sram_text uint32_t board_GetTime(HAL_TimeType_t timeType)
{
    uint32_t timeRet = 0;
    uint32_t divTime = 0;
    uint64_t timeValue = csi_coret_get_value2();

    switch (timeType) {
    case HAL_TIME_CYCLE:
        divTime = 1;
        break;

    case HAL_TIME_US:
        divTime = boardTickFreq / 1000000;
        break;

    case HAL_TIME_MS:
        divTime = boardTickFreq / 1000;
        break;

    case HAL_TIME_S:
        divTime = boardTickFreq;
        break;

    default:
        divTime = 1;
        return 0;
    }

    timeRet = (timeValue / divTime) & 0xFFFFFFFF;

    return timeRet;
}


__keep_sram_text void board_BlkDelay(uint32_t delayVal, HAL_TimeType_t timeType)
{
    uint64_t delayCycle = 0;

    switch (timeType) {
    case HAL_TIME_CYCLE:
        delayCycle = delayVal;
        break;

    case HAL_TIME_US:
        delayCycle = (boardTickFreq / 1000000) * delayVal;
        break;

    case HAL_TIME_MS:
        delayCycle = (boardTickFreq / 1000) * delayVal;
        break;

    case HAL_TIME_S:
        delayCycle = boardTickFreq * delayVal;
        break;

    default:
        break;
    }

    CLIC->CLICINT[7].IE = 0;
    CORET->MTIMECMP = CORET->MTIME + delayCycle;
    CLIC->CLICINT[7].IP = 0;
    while (CLIC->CLICINT[7].IP == 0) {
    }

}

static HAL_Status_t board_SetPinMux(HAL_Pinmux_t * pConfig)
{
    HAL_Status_t status = HAL_STATUS_OK;

    HAL_PinmuxConfig_t *pinConfig = pConfig->pinConfig;
    for (uint8_t i = 0; i < pConfig->pinCnt; i++) {
        status = HAL_GPIO_SetPinParam(pBoardDev_Gpio[pinConfig[i].port], pinConfig[i].pin, &pinConfig[i].config);
        if (status != 0) {
            status = HAL_STATUS_ERROR;
            break;
        }
    }

    return status;
}

__keep_sram_text void board_Reset(HAL_ResetType_t resetType)
{
    LL_BOOT_SetCPUSFlag(LL_BOOT_SYS_FLAG_RESET);
    LL_WDG0_SetResetPatition(resetType);
    LL_BOOT_ResetEnable();
}

HAL_Status_t board_GetPinMux(
                            HAL_DevType_t type,
                            uint8_t idx,
                            HAL_PinMuxType_t cfgType,
                            HAL_Pinmux_t * pPinMux)
{
    HAL_Status_t status = HAL_STATUS_INVALID_STATE;

    switch (type) {
    case HAL_DEV_TYPE_UART:
#if CONFIG_DRIVER_UART
        if (idx == UART0_ID) {
            if (cfgType == HAL_PINMUX_TYPE_INIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_uart0.pinConfig;
                pPinMux->pinCnt = pinmux_uart0.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_DEINIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_uart0_deinit.pinConfig;
                pPinMux->pinCnt = pinmux_uart0_deinit.pinCnt;
                status = HAL_STATUS_OK;
            }
        } else if (idx == UART1_ID) {
            if (cfgType == HAL_PINMUX_TYPE_INIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_uart1.pinConfig;
                pPinMux->pinCnt = pinmux_uart1.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_DEINIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_uart1_deinit.pinConfig;
                pPinMux->pinCnt = pinmux_uart1_deinit.pinCnt;
                status = HAL_STATUS_OK;
            }
        } else if (idx == UART2_ID) {
            if (cfgType == HAL_PINMUX_TYPE_INIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_uart2.pinConfig;
                pPinMux->pinCnt = pinmux_uart2.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_DEINIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_uart2_deinit.pinConfig;
                pPinMux->pinCnt = pinmux_uart2_deinit.pinCnt;
                status = HAL_STATUS_OK;
            }
        } else {
            break;
        }
#endif
        break;

    case HAL_DEV_TYPE_SPI:
#if CONFIG_DRIVER_SPI
        if (idx == SPI0_ID) {
            if (cfgType == HAL_PINMUX_TYPE_INIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_spi0.pinConfig;
                pPinMux->pinCnt = pinmux_spi0.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_SPI_CS) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_spi0_cs.pinConfig;
                pPinMux->pinCnt = pinmux_spi0_cs.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_DEINIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_spi0_deinit.pinConfig;
                pPinMux->pinCnt = pinmux_spi0_deinit.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_SPI_CS_DEINIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_spi0_cs_deinit.pinConfig;
                pPinMux->pinCnt = pinmux_spi0_cs_deinit.pinCnt;
                status = HAL_STATUS_OK;
            }
        }
#endif
        break;

    case HAL_DEV_TYPE_I2C:
#if CONFIG_DRIVER_I2C
        if (idx == I2C0_ID) {
            /* Init I2C Gpio AF */
            if (cfgType == HAL_PINMUX_TYPE_INIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_i2c0.pinConfig;
                pPinMux->pinCnt = pinmux_i2c0.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_DEINIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_i2c0_deinit.pinConfig;
                pPinMux->pinCnt = pinmux_i2c0_deinit.pinCnt;
                status = HAL_STATUS_OK;
            }
        #if CONFIG_DRIVER_I2C1
        } else if (idx == I2C1_ID) {
            if (cfgType == HAL_PINMUX_TYPE_INIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_i2c1.pinConfig;
                pPinMux->pinCnt = pinmux_i2c1.pinCnt;
                status = HAL_STATUS_OK;
            } else if (cfgType == HAL_PINMUX_TYPE_DEINIT) {
                pPinMux->pinConfig = (HAL_PinmuxConfig_t *)pinmux_i2c1_deinit.pinConfig;
                pPinMux->pinCnt = pinmux_i2c1_deinit.pinCnt;
                status = HAL_STATUS_OK;
            }
        #endif
        } else {
            status = HAL_STATUS_NOT_SUPPORTED;
        }
#endif

    default:
        break;
    }

    return status;
}

int BOARD_InitEarlyLog(void)
{
    return PrintfEarly_Init(NULL);
}

int BOARD_InitLogIO(void)
{
    LOG_IO_INIT(CONFIG_LOG_IO_TAG_MASK, CONFIG_LOG_IO_MASK);
    return 0;
}

int BOARD_InitLog(void)
{
    return Printf_Init();
}

int BOARD_InitShell(void)
{
    int status = 0;
#if CONFIG_SHELL
    status = Shell_Init();
#endif
    return status;
}


#endif /* CONFIG_BOARD_MRS6130_P1806 */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

