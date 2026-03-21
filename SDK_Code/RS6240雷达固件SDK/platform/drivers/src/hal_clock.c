/**
 ******************************************************************************
 * @file    hal_clock.c
 * @brief   hal clock define.
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
#include "hal_common.h"
#include "hal_clock.h"
#include "ll_utils.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_pmu.h"
#include "ll_timer.h"
#include "ll_gpio.h"
#include "ll_clock_calib.h"
#include "hal_clock_calib.h"


#ifndef CONFIG_CLOCK_LOG_LEVEL
#define CONFIG_CLOCK_LOG_LEVEL          LEVEL_DIS
#endif

#define LOG_MODULE                      "CLK"
#define LOG_LEVEL                       CONFIG_CLOCK_LOG_LEVEL
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

typedef struct {
    HAL_Dev_t           device;

    uint8_t             sysId;
    uint8_t             dcxoAmcState;
    uint8_t             dcxoAmcTrim;
    uint8_t             sysClkSrc;
    uint8_t             apb0Div;
    uint32_t            freq[CLOCK_MAX];
} Clock_Device_t;


/* Private defines.
 * ----------------------------------------------------------------------------
 */

#define CONFIG_CLOCK_PM                                     CONFIG_PM


#ifndef CONFIG_CLK_DCXO_AMC_TRIM
#define CONFIG_CLK_DCXO_AMC_TRIM                            8
#endif

#ifndef CONFIG_CLK_DCXO_AMC_TIME
#define CONFIG_CLK_DCXO_AMC_TIME                            2
#endif

#ifndef CONFIG_CLK_DCXO_FIRST_POWERUP_GUARD_TIME
#define CONFIG_CLK_DCXO_FIRST_POWERUP_GUARD_TIME            65000
#endif

#ifndef CONFIG_CLK_DCXO_POWERUP_GUARD_TIME
#define CONFIG_CLK_DCXO_POWERUP_GUARD_TIME                  1000000
#endif


#ifndef CONFIG_CLK_MSI_FREQ
#define CONFIG_CLK_MSI_FREQ                                 32000000
#endif

#ifndef CONFIG_CLK_MCO_PIN
#define CONFIG_CLK_MCO_PIN                                  0
#endif


#define NS_2_CYCLE(time, clk) ((time / 100) * ((clk) / 1000 / 10) / 1000)

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define CLOCK_FREQ_JETTER                       2000000

#define CLOCK_PLL_DIV_MAX                       8
#define CLOCK_PLL_MUL_MAX                       64
#define CLOCK_PLL_FREQ_MIN                      120000000
#define CLOCK_PLL_FREQ_MAX                      (448000000 + CLOCK_FREQ_JETTER)

#define CLOCK_PLL_SOC_DIV_MAX                   7
#define CLOCK_PLL_SOC_FREQ_MAX                  (256000000 + CLOCK_FREQ_JETTER)

#define CLOCK_PLL_DEV_DIV_MAX                   7
#define CLOCK_PLL_DEV_FREQ_MAX                  (80000000 + CLOCK_FREQ_JETTER)

#define CLOCK_PLL_DEV1_DIV_MAX                  7
#define CLOCK_PLL_DEV1_FREQ_MAX                 (200000000 + CLOCK_FREQ_JETTER)

#define CLOCK_PLL_CDAA_DIV_MAX                  7
#define CLOCK_PLL_CDAA_FREQ_MAX                 (120000000 + CLOCK_FREQ_JETTER)



#define CLOCK_SYS_DIV_MAX                       8
#define CLOCK_SYS_FREQ_MAX                      (256000000 + CLOCK_FREQ_JETTER)

#define CLOCK_AHB_DIV_MAX                       8
#define CLOCK_AHB_FREQ_MAX                      (128000000 + CLOCK_FREQ_JETTER)

#define CLOCK_APB0_DIV_MAX                      16
#define CLOCK_APB0_FREQ_MAX                     (70000000 + CLOCK_FREQ_JETTER)

#define CLOCK_APB1_DIV_MAX                      16
#define CLOCK_APB1_FREQ_MAX                     (70000000 + CLOCK_FREQ_JETTER)

#define CLOCK_MMW_FREQ_80MHZ                    (80000000 + CLOCK_FREQ_JETTER)

/* power time config
 */
#ifndef CONFIG_PMU_SOCLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_SOCLDO_POWERUP_GUARD_TIME    8000
#endif

#ifndef CONFIG_PMU_SOCLDO_SETTING_GUARD_TIME
#define CONFIG_PMU_SOCLDO_SETTING_GUARD_TIME    10000
#endif

#ifndef CONFIG_PMU_BG_POWERUP_GUARD_TIME
#define CONFIG_PMU_BG_POWERUP_GUARD_TIME        20000
#endif

/* clock power slow smps time pram */
#ifndef CONFIG_PMU_SMPS_POWERUP_GUARD_TIME
#define CONFIG_PMU_SMPS_POWERUP_GUARD_TIME      0
#endif

#ifndef CONFIG_PMU_SMPS_POWERDW_GUARD_TIME
#define CONFIG_PMU_SMPS_POWERDW_GUARD_TIME      0
#endif

#ifndef CONFIG_PMU_EFUSE_POWERUP_GUARD_TIME
#define CONFIG_PMU_EFUSE_POWERUP_GUARD_TIME     0
#endif

#ifndef CONFIG_PMU_SW_POWERUP_GUARD_TIME
#define CONFIG_PMU_SW_POWERUP_GUARD_TIME        0
#endif

#ifndef CONFIG_PMU_SW_SETTING_GUARD_TIME
#define CONFIG_PMU_SW_SETTING_GUARD_TIME        0
#endif

#ifndef CONFIG_CLK_SPLL_POWERDWN_GUARD_TIME
#define CONFIG_CLK_SPLL_POWERDWN_GUARD_TIME     0
#endif

#ifndef CONFIG_PMU_EXTINTLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_EXTINTLDO_POWERUP_GUARD_TIME 0
#endif

#ifndef CONFIG_CLK_DCXO_POWERDWN_GUARD_TIME
#define CONFIG_CLK_DCXO_POWERDWN_GUARD_TIME     0
#endif


/* clock power dcxo fast fsm time pram */
#ifndef CONFIG_PMU_DCXOLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_DCXOLDO_POWERUP_GUARD_TIME   1000
#endif

#ifndef CONFIG_CLK_DCXO_POWERUP_GUARD_TIME
#define CONFIG_CLK_DCXO_POWERUP_GUARD_TIME      1000000
#endif

#ifndef CONFIG_CLK_DCXO_STABLE_TIME
#define CONFIG_CLK_DCXO_STABLE_TIME             100
#endif

#ifndef CONFIG_CLK_DCXO_DETECT_TIME
#define CONFIG_CLK_DCXO_DETECT_TIME             10000
#endif

#ifndef CONFIG_CLK_DCXO_STARTUP_TIME
#define CONFIG_CLK_DCXO_STARTUP_TIME            1
#endif


/* clock power spll fast fsm time pram */
#ifndef CONFIG_PMU_SPLLLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_SPLLLDO_POWERUP_GUARD_TIME   1000
#endif

#ifndef CONFIG_CLK_SPLL_POWERUP_GUARD_TIME
#define CONFIG_CLK_SPLL_POWERUP_GUARD_TIME      5000
#endif


/* clock power other time pram */
#ifndef CONFIG_PMU_RTCSW1_POWERUP_GUARD_TIME
#define CONFIG_PMU_RTCSW1_POWERUP_GUARD_TIME    50
#endif

#ifndef CONFIG_PMU_RTCSW2_POWERUP_GUARD_TIME
#define CONFIG_PMU_RTCSW2_POWERUP_GUARD_TIME    50
#endif


#define CLOCK_LSI_IDX_250KHZ                    3
#define CLOCK_LSI_IDX_500KHZ                    2
#define CLOCK_LSI_IDX_1000KHZ                   1
#define CLOCK_LSI_IDX_2000KHZ                   0

#define CLOCK_MSI_IDX_32MHZ                     2
#define CLOCK_MSI_IDX_40MHZ                     1
#define CLOCK_MSI_IDX_64MHZ                     0


/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static uint8_t clock_DivToIndex(uint8_t div);
static void clock_DcxoOpen(Clock_Device_t *pClockDevice);
static void clock_SlowFsmInit(uint32_t lsiClkFreq);
static void clock_DcxoFastFsmInit(uint32_t msiClkFreq);
static void clock_SpllFastFsmInit(uint32_t msiClkFreq);
static void clock_OtherTimeInit(uint32_t lsiClkFreq);
#if CONFIG_CLOCK_PM
static int clock_pm_action(void *dev, enum pm_device_action action);
#endif


/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t * HAL_CLOCK_Init(void)
{
    Clock_Device_t *pClockDevice = (Clock_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_PWR_CLK, 0);

    if (pClockDevice == NULL) {
        pClockDevice = (Clock_Device_t *)HAL_DEV_MemMalloc(sizeof(Clock_Device_t));
        if (pClockDevice == NULL) {
            return (HAL_Dev_t *)pClockDevice;
        }

        pClockDevice->sysId = csi_get_cpu_id();
        HAL_DEV_SetName((HAL_Dev_t *)pClockDevice, "clock");

#if CONFIG_CLOCK_PM
        pm_device_mult_register(&(pClockDevice->device.pmDev), NULL, clock_pm_action);
#endif

        HAL_DEV_Register((HAL_Dev_t *)pClockDevice, HAL_DEV_TYPE_PWR_CLK, 0);
    }

    return (HAL_Dev_t *)pClockDevice;
}


__keep_sram_text int32_t HAL_CLOCK_GetFreq(HAL_Dev_t *clock, CLOCK_Type_t clkType)
{
    if ((clock == NULL) || (clkType >= CLOCK_MAX)) {
        return (int32_t)0;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;

    return pClockDevice->freq[clkType];
}


HAL_Status_t HAL_CLOCK_ConfigLsiMsi(HAL_Dev_t *clock, CLOCK_LsiMsiCfg_t *pLsiMsi)
{
    if ((clock == NULL) || (pLsiMsi == NULL)) {
        LOG_ERR("input param invalid, clock: %p, pLsiMsi : %p", clock, pLsiMsi);
        return HAL_STATUS_INVALID_PARAM;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;
    HAL_Status_t status = HAL_STATUS_OK;
    uint8_t lsiIdx;
    uint8_t lsiEfuseToCalib = 0;
    uint8_t msiEfuseToCalib = 0;
    uint32_t sysClkSrc = 0;
    uint32_t dcxoToDev = 0;

    do {
        if (((pLsiMsi->lsiCfgMode & 0x0F) >= CLOCK_LSIMSI_CFG_TRIM_MAX)
            || ((pLsiMsi->msiCfgMode & 0x0F) >= CLOCK_LSIMSI_CFG_TRIM_MAX)) {
            LOG_ERR("mode error. lsiCfgMode: %d, msiCfgMode : %d", pLsiMsi->lsiCfgMode, pLsiMsi->msiCfgMode);
            status = HAL_STATUS_INVALID_PARAM;
            break;
        }

        switch (pLsiMsi->lsiFreq) {
        case CLOCK_LSI_2000KHZ:
            lsiIdx = CLOCK_LSI_IDX_2000KHZ;
            break;

        case CLOCK_LSI_1000KHZ:
            lsiIdx = CLOCK_LSI_IDX_1000KHZ;
            break;

        case CLOCK_LSI_500KHZ:
            lsiIdx = CLOCK_LSI_IDX_500KHZ;
            break;

        case CLOCK_LSI_250KHZ:
            lsiIdx = CLOCK_LSI_IDX_250KHZ;
            break;

        default:
            status = HAL_STATUS_INVALID_PARAM;
            break;
        }

        if (status != HAL_STATUS_OK) {
            LOG_ERR("lsiFreq error : %d", pLsiMsi->lsiFreq);
            break;
        }

        if ((pLsiMsi->lsiCfgMode & 0xF) == CLOCK_LSIMSI_CFG_TRIM_EFUSE) {
            uint8_t lsiTrim = 0;
            #if CONFIG_DRIVER_EFUSE
            status = HAL_EFUSE_GetLsiTrim(pEfuseDevice, lsiIdx, &lsiTrim);
            #else
            status = HAL_STATUS_INVALID_STATE;
            #endif
            LOG_INF("get lsi trim from efuse");

            if (status == HAL_STATUS_OK) {
                LOG_INF("success: lsiIdx: %d, lsiTrim: %d", lsiIdx, lsiTrim);
                LL_CALIB_LSICfgWrite((lsiIdx << 8) | lsiTrim);
            } else {
                if ((pLsiMsi->lsiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR) {
                    LOG_INF("error but refr trim");
                    lsiEfuseToCalib = 1;
                    status = HAL_STATUS_OK;
                } else {
                    LOG_INF("nothing");
                    break;
                }
            }
        } else if ((pLsiMsi->lsiCfgMode & 0xF) == CLOCK_LSIMSI_CFG_TRIM_INPUT) {
            LOG_INF("get lsi trim from input: lsiTrim: %d", pLsiMsi->lsiTrim);
            LL_CALIB_LSICfgWrite(pLsiMsi->lsiTrim);
        }

        if ((pLsiMsi->msiFreq != CLOCK_MSI_64MHZ)
            && (pLsiMsi->msiFreq != CLOCK_MSI_40MHZ)
            && (pLsiMsi->msiFreq != CLOCK_MSI_32MHZ)) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("msiFreq error : %d", pLsiMsi->msiFreq);
            break;
        }

        if ((pLsiMsi->msiCfgMode & 0xF) == CLOCK_LSIMSI_CFG_TRIM_EFUSE) {
            uint32_t msiTrim = 0;
            #if CONFIG_DRIVER_EFUSE
            status = HAL_EFUSE_GetMsiTrim(pEfuseDevice, &msiTrim);
            if ((status == HAL_STATUS_OK) && (msiTrim == 0)) {
                status = HAL_STATUS_INVALID_STATE;
            }
            #else
            status = HAL_STATUS_INVALID_STATE;
            #endif
            LOG_INF("get msi trim from efuse");
            if (status == HAL_STATUS_OK) {
                LL_CALIB_MSICfgWrite(msiTrim);
                LOG_INF("success: msiTrim: %d", msiTrim);
            } else {
                if ((pLsiMsi->msiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR) {
                    msiEfuseToCalib = 1;
                    status = HAL_STATUS_OK;
                    LOG_INF("error but refr trim");
                } else {
                    LOG_INF("nothing");
                    break;
                }
            }
        } else if ((pLsiMsi->msiCfgMode & 0xF) == CLOCK_LSIMSI_CFG_TRIM_INPUT) {
            LOG_INF("get msi trim from input: msiTrim: %d", pLsiMsi->msiTrim);
            LL_CALIB_MSICfgWrite(pLsiMsi->msiTrim);
        }

        LL_RCC_LSI_DisableClkIn();
        LL_RCC_LSI_DisableClkOut();
        LL_RCC_LSI_SetClockSource(LL_RCC_LSI_SRC_RC);
        LL_RCC_LSI_Enable();

        LL_RCC_MSI_DisableClkIn();
        LL_RCC_MSI_SetClockSource(LL_RCC_MSI_SRC_RC);
        LL_PMU_MSI_Enable();

        if (((pLsiMsi->lsiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR)
            || ((pLsiMsi->msiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR)) {
            clock_DcxoOpen(pClockDevice);

            /* system clock switch to dcxo */
            sysClkSrc = LL_RCC_SYS_GetClockSource(pClockDevice->sysId);
            dcxoToDev = LL_PMU_DCXO_IsEnabledToSoc();
            LL_PMU_DCXO_EnableToSoc();
            LL_RCC_SYS_SetClockSource(pClockDevice->sysId, LL_RCC_SYS_SRC_HSE);
            LOG_DBG("sysclock switch to dcxo");
        }

        if ((pLsiMsi->lsiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR) {
            if (((pLsiMsi->lsiCfgMode & 0xF) == CLOCK_LSIMSI_CFG_TRIM_CALIB)
                || (lsiEfuseToCalib != 0)) {
                pClockDevice->freq[CLOCK_LSI] = HAL_CLOCK_LsiCalib(pLsiMsi->lsiFreq, NULL);
                LOG_INF("calib lsi freq: %d, %d", pClockDevice->freq[CLOCK_LSI], pLsiMsi->lsiFreq);
            } else {
                pClockDevice->freq[CLOCK_LSI] = HAL_CLOCK_LsiGetFreq();
                LOG_INF("get lsi freq: %d", pClockDevice->freq[CLOCK_LSI]);
            }
        } else {
            pClockDevice->freq[CLOCK_LSI] = pLsiMsi->lsiFreq;
            LOG_INF("input lsi freq: %d", pClockDevice->freq[CLOCK_LSI]);
        }

        if ((pLsiMsi->msiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR) {
            if (((pLsiMsi->msiCfgMode & 0xF) == CLOCK_LSIMSI_CFG_TRIM_CALIB)
                || (msiEfuseToCalib != 0)) {
                pClockDevice->freq[CLOCK_MSI] = HAL_CLOCK_MsiCalib(pLsiMsi->msiFreq, NULL);
                LOG_INF("calib msi freq: %d, %d", pClockDevice->freq[CLOCK_MSI], pLsiMsi->msiFreq);
            } else {
                pClockDevice->freq[CLOCK_MSI] = HAL_CLOCK_MsiGetFreq();
                LOG_INF("get msi freq: %d", pClockDevice->freq[CLOCK_MSI]);
            }
        } else {
            pClockDevice->freq[CLOCK_MSI] = pLsiMsi->msiFreq;
            LOG_INF("input msi freq: %d", pClockDevice->freq[CLOCK_MSI]);
        }

        if (((pLsiMsi->lsiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR)
            || ((pLsiMsi->msiCfgMode & 0x10) == CLOCK_LSIMSI_CFG_FREQ_REFR)) {
            /* restore system clock */
            LL_RCC_SYS_SetClockSource(pClockDevice->sysId, sysClkSrc);
            LOG_DBG("sysclock switch to %d", sysClkSrc);
            if (dcxoToDev == 0) {
                LL_PMU_DCXO_DisableToSoc();
            }

            if (pClockDevice->freq[CLOCK_DCXO] == 0) {
                LOG_DBG("close dcxo output");
                LL_PMU_DCXO_DisableToSoc();
                LL_PMU_DCXO_DisableToPll();
                LL_PMU_DCXO_SoftDisable();
                LL_PMU_DCXOLDO_SoftDisable();
            }
        }
    } while (0);

    return status;
}


HAL_Status_t HAL_CLOCK_ConfigLpclk(HAL_Dev_t *clock, CLOCK_LpclkCfg_t *pLpclk)
{
    if ((clock == NULL) || (pLpclk == NULL)) {
        LOG_ERR("input param invalid, clock: %p, pLpclk : %p", clock, pLpclk);
        return HAL_STATUS_INVALID_PARAM;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;
    HAL_Status_t status = HAL_STATUS_OK;
    uint32_t tmpLpclkFreq = 0;

    do {
        switch (pLpclk->clkSrc) {
        case LL_RCC_LPCLK_SRC_LSE:
            tmpLpclkFreq = pClockDevice->freq[CLOCK_LSE];
            break;

        case LL_RCC_LPCLK_SRC_LSI:
            tmpLpclkFreq = pClockDevice->freq[CLOCK_LSI];
            break;

        default:
            break;
        }

        /* the clock is not ready */
        if (tmpLpclkFreq == 0) {
            LOG_ERR("LpclkFreq error, src: %d", pLpclk->clkSrc);
            status = HAL_STATUS_INVALID_STATE;
            break;
        }

        if ((pLpclk->inteEnable) && ((pLpclk->inteDiv > 128) || (pLpclk->inteDiv == 0))) {
            LOG_ERR("Lpclk param error, inteEnable: %d, inteDiv: %d", pLpclk->inteEnable, pLpclk->inteDiv);
            status = HAL_STATUS_INVALID_STATE;
            break;
        }

        LL_RCC_LPCLK_SetClockSource(pLpclk->clkSrc);
        pClockDevice->freq[CLOCK_LPCLK] = tmpLpclkFreq;
        LOG_DBG("Lpclk src: %d, freq: %d", pLpclk->clkSrc, pClockDevice->freq[CLOCK_LPCLK]);

        if (pLpclk->inteEnable) {
            LL_RCC_LPCLK_SetIntegerDivision(clock_DivToIndex(pLpclk->inteDiv));
            LL_RCC_LPCLK_EnableIntegerDivision();
            LL_RCC_LPCLK_Ckin32kClockSrc(0);
            pClockDevice->freq[CLOCK_LPCLK_INTE_DIV] = tmpLpclkFreq / pLpclk->inteDiv;
            LOG_INF("Lpclk inter out: freq: %d, div: %d", pClockDevice->freq[CLOCK_LPCLK_INTE_DIV], pLpclk->inteDiv);
        } else {
            LL_RCC_LPCLK_DisableIntegerDivision();
            LOG_INF("Lpclk inter out: close");
        }

        if (pLpclk->fracEnable) {
            LL_RCC_LPCLK_SetFracDivision(pLpclk->fracDenominator,
                                    pLpclk->fracRemainder,
                                    pLpclk->fracQuotient);
            LL_RCC_LPCLK_EnableFracDivision();
            LL_RCC_LPCLK_Ckin32kClockSrc(1);
            pClockDevice->freq[CLOCK_LPCLK_FRAC_DIV] = pLpclk->fracDenominator;
            LOG_INF("Lpclk frac out: freq: %d", pClockDevice->freq[CLOCK_LPCLK_FRAC_DIV]);
        } else {
            LL_RCC_LPCLK_DisableFracDivision();
            LOG_INF("Lpclk frac out: close");
        }
    } while (0);

    return status;
}


HAL_Status_t HAL_CLOCK_ConfigLse(HAL_Dev_t *clock, CLOCK_LseCfg_t *pLse)
{
    if ((clock == NULL) || (pLse == NULL)) {
        LOG_ERR("input param invalid, clock: %p, pLse : %p", clock, pLse);
        return HAL_STATUS_INVALID_PARAM;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;

    if (pLse->enable == 0) {
        LL_RCC_LSE_DisableClkIn();
        LL_RCC_LSE_Disable();
        LOG_INF("lse: close");
    } else {
        LL_RCC_LSE_DisableClkIn();
        LL_RCC_LSE_SetClockSource(LL_RCC_LSE_SRC_XO);
        LL_RCC_LSE_Enable();
        pClockDevice->freq[CLOCK_LSE] = pLse->freq;
        LOG_INF("lse: freq: %d", pClockDevice->freq[CLOCK_LSE]);
    }

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_CLOCK_ConfigDcxo(HAL_Dev_t *clock, CLOCK_DcxoCfg_t *pDcxo)
{
    if ((clock == NULL) || (pDcxo == NULL)) {
        LOG_ERR("input param invalid, clock: %p, pDcxo : %p", clock, pDcxo);
        return HAL_STATUS_INVALID_PARAM;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (pDcxo->enable == 0) {
            LL_PMU_DCXO_DisableToSoc();
            LL_PMU_DCXO_DisableToPll();
            LL_PMU_DCXO_SoftDisable();
            LL_PMU_DCXOLDO_SoftDisable();

            pClockDevice->freq[CLOCK_DCXO] = 0;
            LOG_INF("dcxo: close");
        } else {
            clock_DcxoOpen(pClockDevice);

            pClockDevice->freq[CLOCK_DCXO] = pDcxo->freq;

            if (pDcxo->out_dev) {
                LL_PMU_DCXO_EnableToSoc();
                LOG_INF("dcxo to dev: open");
            } else {
                LL_PMU_DCXO_DisableToSoc();
                LOG_INF("dcxo to dev: close");
            }

            if (pDcxo->out_pll) {
                LL_PMU_DCXO_EnableToPll();
                LOG_INF("dcxo to pll: open");
            } else {
                LL_PMU_DCXO_DisableToPll();
                LOG_INF("dcxo to pll: close");
            }
            LOG_INF("dcxo: freq: %d", pClockDevice->freq[CLOCK_DCXO]);
        }
    } while (0);

    return status;
}


HAL_Status_t HAL_CLOCK_ConfigPll(HAL_Dev_t *clock, CLOCK_PllCfg_t *pPll)
{
    if ((clock == NULL) || (pPll == NULL)) {
        LOG_ERR("input param invalid, clock: %p, pPll : %p", clock, pPll);
        return HAL_STATUS_INVALID_PARAM;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;
    HAL_Status_t status = HAL_STATUS_OK;
    uint32_t src_freq = 0;
    uint32_t sysClkSrc = 0;
    uint32_t tmp_pll_freq = 0;
    uint32_t tmp_pll_soc_freq = 0;
    uint32_t tmp_pll_dev_freq = 0;
    uint32_t tmp_pll_dev1_freq = 0;
    uint32_t tmp_pll_cdaa_freq = 0;


    do {
        if (pPll->enable == 0) {
            if ((pPll->socEnable != 0)
                || (pPll->devEnable != 0)
                || (pPll->dev1Enable != 0)
                || (pPll->cdaaEnable != 0)) {
                LOG_ERR("pll enable but subout disable");
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }

            LL_PMU_SPLL_SoftDisable();
            LL_PMU_SPLLLDO_SoftDisable();

            pClockDevice->freq[CLOCK_PLL] = 0;
            pClockDevice->freq[CLOCK_PLL_SOC] = 0;
            pClockDevice->freq[CLOCK_PLL_DEV] = 0;
            pClockDevice->freq[CLOCK_PLL_DEV1] = 0;
            pClockDevice->freq[CLOCK_PLL_CDAA] = 0;
            LOG_INF("pll disable");
        } else {
            LOG_INF("pll enable");

            switch (pPll->clkSrc) {
            case LL_RCC_PLL_SRC_MSI:
                src_freq = pClockDevice->freq[CLOCK_MSI];
                break;

            case LL_RCC_PLL_SRC_HSE:
                src_freq = pClockDevice->freq[CLOCK_DCXO];
                break;

            default:
                break;
            }

            /* the clock is not ready */
            if (src_freq == 0) {
                LOG_ERR("pll src freq error: src: %d", pPll->clkSrc);
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }

            /* check param */
            if ((pPll->pllDiv > CLOCK_PLL_DIV_MAX)
                || (pPll->pllMul > CLOCK_PLL_MUL_MAX)) {
                LOG_ERR("pll div or mul error: pllDiv: %d, pllMul: %d", pPll->pllDiv, pPll->pllMul);
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }
            tmp_pll_freq = (src_freq / pPll->pllDiv) * pPll->pllMul;
            LOG_INF("pll freq: %d, src freq: %d, pllDiv: %d, pllMul: %d",
                tmp_pll_freq, src_freq, pPll->pllDiv, pPll->pllMul);
            if ((tmp_pll_freq > CLOCK_PLL_FREQ_MAX)
                || (tmp_pll_freq < CLOCK_PLL_FREQ_MIN)) {
                LOG_ERR("pll freq error");
                status = HAL_STATUS_INVALID_PARAM;
                break;
            }

            if (pPll->socEnable) {
                if (pPll->socDiv > CLOCK_PLL_SOC_DIV_MAX) {
                    status = HAL_STATUS_INVALID_PARAM;
                    LOG_ERR("pll socDiv error: %d", pPll->socDiv);
                    break;
                }
                tmp_pll_soc_freq = tmp_pll_freq / pPll->socDiv;
                LOG_INF("pll soc freq: %d, pllDiv: %d", tmp_pll_soc_freq, pPll->socDiv);
                if (tmp_pll_soc_freq > CLOCK_PLL_SOC_FREQ_MAX) {
                    status = HAL_STATUS_INVALID_PARAM;
                    LOG_ERR("pll soc freq error");
                    break;
                }
            }

            if (pPll->devEnable) {
                if (pPll->devDiv > CLOCK_PLL_DEV_DIV_MAX) {
                    LOG_ERR("pll devDiv error: %d", pPll->devDiv);
                    status = HAL_STATUS_INVALID_PARAM;
                    break;
                }
                tmp_pll_dev_freq = tmp_pll_freq / pPll->devDiv;
                LOG_INF("pll dev freq: %d, pllDiv: %d", tmp_pll_dev_freq, pPll->devDiv);
                if (tmp_pll_dev_freq > CLOCK_PLL_DEV_FREQ_MAX) {
                    status = HAL_STATUS_INVALID_PARAM;
                    LOG_ERR("pll dev freq error");
                    break;
                }
            }

            if (pPll->dev1Enable) {
                if (pPll->dev1Div > CLOCK_PLL_DEV1_DIV_MAX) {
                    status = HAL_STATUS_INVALID_PARAM;
                    LOG_ERR("pll dev1Div error: %d", pPll->dev1Div);
                    break;
                }
                tmp_pll_dev1_freq = tmp_pll_freq / pPll->dev1Div;
                LOG_INF("pll dev1 freq: %d, pllDiv: %d", tmp_pll_dev1_freq, pPll->dev1Div);
                if (tmp_pll_dev1_freq > CLOCK_PLL_DEV1_FREQ_MAX) {
                    status = HAL_STATUS_INVALID_PARAM;
                    LOG_ERR("pll dev1 freq error");
                    break;
                }
            }

            if (pPll->cdaaEnable) {
                if (pPll->cdaaDiv > CLOCK_PLL_CDAA_DIV_MAX) {
                    status = HAL_STATUS_INVALID_PARAM;
                    LOG_ERR("pll cdaaDiv error: %d", pPll->cdaaDiv);
                    break;
                }
                tmp_pll_cdaa_freq = tmp_pll_freq / pPll->cdaaDiv;
                LOG_INF("pll cdaa freq: %d, pllDiv: %d", tmp_pll_cdaa_freq, pPll->cdaaDiv);
                if (tmp_pll_cdaa_freq > CLOCK_PLL_CDAA_FREQ_MAX) {
                    status = HAL_STATUS_INVALID_PARAM;
                    LOG_ERR("pll cdaa freq error");
                    break;
                }
            }

            pClockDevice->freq[CLOCK_PLL] = tmp_pll_freq;
            pClockDevice->freq[CLOCK_PLL_SOC] = tmp_pll_soc_freq;
            pClockDevice->freq[CLOCK_PLL_DEV] = tmp_pll_dev_freq;
            pClockDevice->freq[CLOCK_PLL_DEV1] = tmp_pll_dev1_freq;
            pClockDevice->freq[CLOCK_PLL_CDAA] = tmp_pll_cdaa_freq;

            LOG_INF("disable pll out");
            sysClkSrc = LL_RCC_SYS_GetClockSource(pClockDevice->sysId);
            if (sysClkSrc == CLOCK_SYS_SRC_PLL) {
                LL_RCC_SYS_SetClockSource(pClockDevice->sysId, CLOCK_SYS_SRC_MSI);
            }

            LL_RCC_PLL_DisableCh0SOC();
            LL_RCC_PLL_DisableCh1DEV0();
            LL_RCC_PLL_DisableCh2DEV1();
            LL_RCC_PLL_DisableCh3CDAA();

            LOG_INF("config pll");
            LL_RCC_PLL_SetClockDivision(pPll->pllDiv - 1);
            LL_RCC_PLL_SetClockMultiple(pPll->pllMul - 1);

            if (pPll->clkSrc == LL_RCC_PLL_SRC_HSE) {
                clock_DcxoOpen(pClockDevice);

                if (LL_PMU_DCXO_IsEnabledToPll() == 0) {
                    LL_PMU_DCXO_EnableToPll();
                }
            }
            LL_RCC_PLL_SetClockSource(pPll->clkSrc);
            LOG_INF("set pll src: %d", pPll->clkSrc);

            if (LL_PMU_SPLL_IsReady() == 0) {
                LOG_INF("enable pll");
                LL_RCC_LPCLK_EnableToPMU();
                if (LL_PMU_SPLLLDO_IsReady() == 0) {
                    LL_PMU_SPLLLDO_SoftEnable();
                    while (LL_PMU_SPLLLDO_IsReady() == 0) {
                    }
                }

                LL_PMU_SPLL_SoftEnable();
                while (LL_PMU_SPLL_IsReady() == 0) {
                }
            }

            if (pClockDevice->freq[CLOCK_PLL_SOC] != 0) {
                LL_RCC_PLL_SetCh0SOCDivision(pPll->socDiv);
                LL_RCC_PLL_EnableCh0SOC();
                LOG_INF("enable pll soc");
            }

            if (pClockDevice->freq[CLOCK_PLL_DEV] != 0) {
                LL_RCC_PLL_SetCh1DEV0Division(pPll->devDiv);
                LL_RCC_PLL_EnableCh1DEV0();
                LOG_INF("enable pll dev");
            }

            if (pClockDevice->freq[CLOCK_PLL_DEV1] != 0) {
                LL_RCC_PLL_SetCh2DEV1Division(pPll->dev1Div);
                LL_RCC_PLL_EnableCh2DEV1();
                LOG_INF("enable pll dev1");
            }

            if (pClockDevice->freq[CLOCK_PLL_CDAA] != 0) {
                LL_RCC_PLL_SetCh3CDAADivision(pPll->cdaaDiv);
                LL_RCC_PLL_EnableCh3CDAA();
                LOG_INF("enable pll cdaa");
            }

            LL_RCC_SYS_SetClockSource(pClockDevice->sysId, sysClkSrc);
        }
    } while (0);

    return status;
}


HAL_Status_t HAL_CLOCK_ConfigSys(HAL_Dev_t *clock, CLOCK_SysCfg_t *pSys)
{
    if ((clock == NULL) || (pSys == NULL)) {
        LOG_ERR("input param invalid, clock: %p, pSys : %p", clock, pSys);
        return HAL_STATUS_INVALID_PARAM;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;
    HAL_Status_t status = HAL_STATUS_OK;
    uint32_t src_freq = 0;
    uint32_t tmp_sys_freq = 0;
    uint32_t tmp_ahb_freq = 0;
    uint32_t tmp_apb0_freq = 0;
    uint32_t tmp_apb1_freq = 0;

    do {
        switch (pSys->clkSrc) {
        case CLOCK_SYS_SRC_MSI:
            src_freq = pClockDevice->freq[CLOCK_MSI];
            break;

        case CLOCK_SYS_SRC_DCXO:
            src_freq = pClockDevice->freq[CLOCK_DCXO];
            break;

        case CLOCK_SYS_SRC_PLL:
            src_freq = pClockDevice->freq[CLOCK_PLL_SOC];
            break;

        default:
            break;
        }

        /* the clock is not ready */
        if (src_freq == 0) {
            status = HAL_STATUS_INVALID_STATE;
            LOG_ERR("clock(%d) freq is 0", pSys->clkSrc);
            break;
        }

        /* check param */
        if (pSys->sysDiv > CLOCK_SYS_DIV_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("sys div err %d", pSys->sysDiv);
            break;
        }
        tmp_sys_freq = src_freq / pSys->sysDiv;
        if (tmp_sys_freq > CLOCK_SYS_FREQ_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("sys freq err %d %d", src_freq, tmp_sys_freq);
            break;
        }

        if (pSys->ahbDiv > CLOCK_AHB_DIV_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("ahb div err %d", pSys->ahbDiv);
            break;
        }
        tmp_ahb_freq = tmp_sys_freq / pSys->ahbDiv;
        if (tmp_ahb_freq > CLOCK_AHB_FREQ_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("ahb freq err %d", tmp_ahb_freq);
            break;
        }

        if (pSys->apb0Div > CLOCK_APB0_DIV_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("apb0 div err %d", pSys->apb0Div);
            break;
        }
        tmp_apb0_freq = tmp_ahb_freq / pSys->apb0Div;
        if (tmp_apb0_freq > CLOCK_APB0_FREQ_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("apb0 freq err %d", tmp_apb0_freq);
            break;
        }

        if (pSys->apb1Div > CLOCK_APB1_DIV_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("apb1 div err %d", pSys->apb1Div);
            break;
        }
        tmp_apb1_freq = tmp_ahb_freq / pSys->apb1Div;
        if (tmp_apb1_freq > CLOCK_APB1_FREQ_MAX) {
            status = HAL_STATUS_INVALID_PARAM;
            LOG_ERR("apb1 freq err %d", tmp_apb1_freq);
            break;
        }

        /* first, switch to the low-speed clock to avoid clock three overflow */
        LL_RCC_SYS_SetClockSource(pClockDevice->sysId, LL_RCC_SYS_SRC_MSI);


        LL_RCC_APB0_SetClockDivision(clock_DivToIndex(pSys->apb0Div));
        LL_RCC_APB1_SetClockDivision(clock_DivToIndex(pSys->apb1Div));
        LL_RCC_AHB_SetClockDivision(clock_DivToIndex(pSys->ahbDiv));
        LL_RCC_SYS_SetClockDivision(clock_DivToIndex(pSys->sysDiv));

        if (tmp_ahb_freq > CLOCK_MMW_FREQ_80MHZ) {
            LL_RCC_AHB_SetMMWHClockDivision(1);
        } else {
            LL_RCC_AHB_SetMMWHClockDivision(0);
        }

        LL_RCC_SYS_SetClockSource(pClockDevice->sysId, pSys->clkSrc);
        pClockDevice->sysClkSrc = pSys->clkSrc;
        pClockDevice->apb0Div   = pSys->apb0Div;

        pClockDevice->freq[CLOCK_SYS] = tmp_sys_freq;
        pClockDevice->freq[CLOCK_AHB] = tmp_ahb_freq;
        pClockDevice->freq[CLOCK_APB0] = tmp_apb0_freq;
        pClockDevice->freq[CLOCK_APB1] = tmp_apb1_freq;
        pClockDevice->freq[CLOCK_CPUF] = tmp_sys_freq;
        pClockDevice->freq[CLOCK_CPUS] = tmp_ahb_freq;

        if (pClockDevice->sysId == 0) {
            pClockDevice->freq[CLOCK_CURR_CPU] = pClockDevice->freq[CLOCK_CPUS];
        } else {
            pClockDevice->freq[CLOCK_CURR_CPU] = pClockDevice->freq[CLOCK_CPUF];
        }

        LOG_INF("sys freq: %d", tmp_sys_freq);
        LOG_INF("ahb freq: %d", tmp_ahb_freq);
        LOG_INF("apb0 freq: %d", tmp_apb0_freq);
        LOG_INF("apb1 freq: %d", tmp_apb1_freq);
        LOG_INF("cpu freq: %d", pClockDevice->freq[CLOCK_CURR_CPU]);
    } while (0);


    return status;
}

HAL_Status_t HAL_CLOCK_SetupTimeInit(HAL_Dev_t *clock)
{
    if (clock == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Clock_Device_t *pClockDevice = (Clock_Device_t *)clock;
    if ((pClockDevice->freq[CLOCK_LSI] == 0) || (pClockDevice->freq[CLOCK_MSI] == 0)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    clock_SlowFsmInit(pClockDevice->freq[CLOCK_LSI]);

    clock_DcxoFastFsmInit(pClockDevice->freq[CLOCK_MSI]);

    clock_SpllFastFsmInit(pClockDevice->freq[CLOCK_MSI]);

    clock_OtherTimeInit(pClockDevice->freq[CLOCK_LSI]);

    return HAL_STATUS_OK;
}

#if CONFIG_CLOCK_MCO_ENABEL
HAL_Status_t HAL_CLOCK_EnableMCO(CLOCK_McoSrc_t clk, CLOCK_McoDiv_t div)
{
    uint8_t pin_id;
    uint8_t func;

#if (CONFIG_CLK_MCO_PIN == 0)
    pin_id = LL_GPIO_PIN_0;
    func = LL_GPIOA_P0_F9_MCO_CLK;
#elif (CONFIG_CLK_MCO_PIN == 13)
    pin_id = LL_GPIO_PIN_13;
    func = LL_GPIOA_P13_F3_MCO_CLK;
#else
    #error "not define out pin\n"
#endif


    if ((clk >= CLOCK_MCO_SRC_MAX) || (div >= CLOCK_MCO_SRC_DIVMAX)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    LL_GPIO_SetPinPull(GPIOA_DEV, pin_id, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinFuncMode(GPIOA_DEV, pin_id, func);

    LL_RCC_MCO_SetClockSource(clk);
    LL_RCC_MCO_SetPrescaler(div);
    LL_RCC_MCO_EnableClock();
    if (!LL_RCC_MCO_IsEnabledClock()) {
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_CLOCK_DisableMCO(void)
{
    uint8_t pin_id;

#if (CONFIG_CLK_MCO_PIN == 0)
    pin_id = LL_GPIO_PIN_0;
#elif (CONFIG_CLK_MCO_PIN == 13)
    pin_id = LL_GPIO_PIN_13;
#else
    #error "not define out pin\n"
#endif


    LL_GPIO_SetPinPull(GPIOA_DEV, pin_id, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinFuncMode(GPIOA_DEV, pin_id, LL_GPIOx_Pn_F15_DIS);
    LL_RCC_MCO_SetClockSource(0);
    LL_RCC_MCO_SetPrescaler(0);
    LL_RCC_MCO_DisableClock();

    if (LL_RCC_MCO_IsEnabledClock()) {
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

#endif

#if CONFIG_CLOCK_PM
__keep_sram_text static int clock_pm_action(void *dev, enum pm_device_action action)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if (action == PM_DEVICE_ACTION_SUSPEND) {
//        LL_PMU_BGLDO_ForceEnable();
    } else if (action == PM_DEVICE_ACTION_RESUME) {
        Clock_Device_t *pClockDevice = (Clock_Device_t *)dev;
        if (pClockDevice != NULL) {
            if (pClockDevice->freq[CLOCK_DCXO] != 0) {
                while (LL_PMU_DCXOLDO_IsReady() == 0) {
                }
                while (LL_PMU_DCXO_IsReady() == 0) {
                }
            }

            if (pClockDevice->freq[CLOCK_PLL] != 0) {
                while (LL_PMU_SPLLLDO_IsReady() == 0) {
                }
                while (LL_PMU_SPLL_IsReady() == 0) {
                }
            }

            LL_RCC_APB0_SetClockDivision(clock_DivToIndex(CONFIG_CLK_APB0_DIVISOR));
            LL_RCC_SYS_SetClockSource(pClockDevice->sysId, pClockDevice->sysClkSrc);
//            LL_PMU_BGLDO_ForceDisable();
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    }

    return status;
}
#endif

static void clock_DcxoOpen(Clock_Device_t *pClockDevice)
{
    if (LL_PMU_DCXO_IsReady() == 0) {
        if (pClockDevice->dcxoAmcState == 0) {
            LL_PMU_DCXO_SetAMCITrim(CONFIG_CLK_DCXO_AMC_TRIM);
            LL_PMU_DCXO_SetAMCTim(CONFIG_CLK_DCXO_AMC_TIME);
            LL_PMU_DCXO_DisableITrimBypass();
            LL_PMU_DCXO_EnableAMC();

        } else {
            LL_PMU_DCXO_SetAITrim(pClockDevice->dcxoAmcTrim);
            LL_PMU_DCXO_DisableAMC();
            LL_PMU_DCXO_EnableITrimBypass();
        }

        LL_RCC_LPCLK_EnableToPMU();
        LL_PMU_DCXO_SetPwrUpGuardTime(CONFIG_CLK_DCXO_FIRST_POWERUP_GUARD_TIME);
        LL_PMU_DCXOLDO_SoftEnable();
        LL_PMU_DCXO_SoftEnable();

        while (LL_PMU_DCXOLDO_IsReady() == 0) {
        }

        while (LL_PMU_DCXO_IsReady() == 0) {
        }

        LL_PMU_DCXO_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_CLK_DCXO_POWERUP_GUARD_TIME, CONFIG_CLK_MSI_FREQ));

        if (pClockDevice->dcxoAmcState == 0) {
            while (LL_PMU_DCXO_IsReadyAMC() == 0) {
            }
            pClockDevice->dcxoAmcTrim = LL_PMU_DCXO_GetAMCCalcITrim();
            LL_PMU_DCXO_SetAITrim(pClockDevice->dcxoAmcTrim);
            LL_PMU_DCXO_DisableAMC();
            LL_PMU_DCXO_EnableITrimBypass();
            pClockDevice->dcxoAmcState = 1;
        }
    }
}


static uint8_t clock_DivToIndex(uint8_t div)
{
    uint8_t idx = 0;

    switch (div) {
    case 128:
        idx = LL_RCC_CLK_DIV_128;
        break;
    case 64:
        idx = LL_RCC_CLK_DIV_64;
        break;
    case 32:
        idx = LL_RCC_CLK_DIV_32;
        break;
    case 16:
        idx = LL_RCC_CLK_DIV_16;
        break;
    case 8:
        idx = LL_RCC_CLK_DIV_8;
        break;
    case 4:
        idx = LL_RCC_CLK_DIV_4;
        break;
    case 2:
        idx = LL_RCC_CLK_DIV_2;
        break;
    case 1:
        idx = LL_RCC_CLK_DIV_1;
        break;
    default:
        idx = LL_RCC_CLK_DIV_1;
        break;
    }

    return idx;
}

static void clock_SlowFsmInit(uint32_t lsiClkFreq)
{
    /* 0 */
    LL_PMU_EXTINTLDO_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_PMU_EXTINTLDO_POWERUP_GUARD_TIME, lsiClkFreq));

    /* 2 */
    LL_PMU_SMPS_SetPwrUpGuardTime(CONFIG_PMU_SMPS_POWERUP_GUARD_TIME);

    /* 8000 */
    LL_PMU_SOCLDO_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_PMU_SOCLDO_POWERUP_GUARD_TIME, lsiClkFreq));
    LL_PMU_EFUSE_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_PMU_EFUSE_POWERUP_GUARD_TIME, lsiClkFreq));

    LL_PMU_SW_UpProcTime(0);
    /* 0 */
    LL_PMU_SW_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_PMU_SW_POWERUP_GUARD_TIME, lsiClkFreq));

    /* 0 */
    LL_PMU_SW_SetTimingGuardTime(NS_2_CYCLE(CONFIG_PMU_SW_SETTING_GUARD_TIME, lsiClkFreq));

    /* 200 */
    LL_PMU_SPLL_SetPwrDwnGuardTime(NS_2_CYCLE(CONFIG_CLK_SPLL_POWERDWN_GUARD_TIME, lsiClkFreq));

    /* 0 */
    LL_PMU_DCXO_SetPwrDwnGuardTime(NS_2_CYCLE(CONFIG_CLK_DCXO_POWERDWN_GUARD_TIME, lsiClkFreq));

    /* 100 */
    LL_PMU_SOCLDO_SetTimingGuardTime(NS_2_CYCLE(CONFIG_PMU_SOCLDO_SETTING_GUARD_TIME, lsiClkFreq));

    /* 100 */
    LL_PMU_SMPS_SetPwrDwnGuardTime(NS_2_CYCLE(CONFIG_PMU_SMPS_POWERDW_GUARD_TIME, lsiClkFreq));

    LL_PMU_SW_FastUpEnable();
    LL_PMU_DCXO_FastDwnEnable();
    LL_PMU_PLL_FastDwnEnable();
}

static void clock_DcxoFastFsmInit(uint32_t msiClkFreq)
{
    /* 1000 */
    LL_PMU_DCXOLDO_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_PMU_DCXOLDO_POWERUP_GUARD_TIME, msiClkFreq));

    /* 10000 */
    LL_PMU_DCXO_SetDetectTime(NS_2_CYCLE(CONFIG_CLK_DCXO_DETECT_TIME, msiClkFreq));

    /* 1000000 */
    LL_PMU_DCXO_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_CLK_DCXO_POWERUP_GUARD_TIME, msiClkFreq));

    /* 100 */
    LL_PMU_DCXO_SetStableTime(NS_2_CYCLE(CONFIG_CLK_DCXO_STABLE_TIME, msiClkFreq));

    /* 1 cycles */
    LL_PMU_DCXO_SetStartupTime(CONFIG_CLK_DCXO_STARTUP_TIME);
}

static void clock_SpllFastFsmInit(uint32_t msiClkFreq)
{
    /* 1000 */
    LL_PMU_SPLLLDO_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_PMU_SPLLLDO_POWERUP_GUARD_TIME, msiClkFreq));

    /* 5000 */
    LL_PMU_SPLL_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_CLK_SPLL_POWERUP_GUARD_TIME, msiClkFreq));
}

static void clock_OtherTimeInit(uint32_t lsiClkFreq)
{
    /* 20us */
    LL_PMU_BGLDO_SetPwrUpGuardTime(NS_2_CYCLE(CONFIG_PMU_BG_POWERUP_GUARD_TIME, lsiClkFreq));

    LL_PMU_SW_RTC1SetPwrUpGuardTime(CONFIG_PMU_RTCSW1_POWERUP_GUARD_TIME);
    LL_PMU_SW_RTC2SetPwrUpGuardTime(CONFIG_PMU_RTCSW2_POWERUP_GUARD_TIME);
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

