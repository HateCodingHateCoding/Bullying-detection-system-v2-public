/**
 ******************************************************************************
 * @file    soc_power.c
 * @brief   soc_power define.
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

#include "hal_types.h"
#include "hal_dev.h"

#include "ll_utils.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_pmu.h"
#include "ll_gpadc.h"
#include "ll_timer.h"

#if CONFIG_DRIVER_EFUSE
#include "hal_efuse.h"
#endif

#include "irq.h"

#include "log.h"
/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* ext ldo
 */
#ifndef CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_SYS0
#define CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_SYS0                       0
#endif

#ifndef CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_SYS1
#define CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_SYS1                       0
#endif

#ifndef CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_MMW
#define CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_MMW                        0
#endif

#ifndef CONFIG_PMU_EXTLDO_ACTIVE_VOLTAGE
#define CONFIG_PMU_EXTLDO_ACTIVE_VOLTAGE                            15
#endif

#ifndef CONFIG_PMU_EXTLDO_SLEEP_VOLTAGE
#define CONFIG_PMU_EXTLDO_SLEEP_VOLTAGE                             15
#endif

#ifndef CONFIG_PMU_EXTLDO_SLEEP_LQ_MODE
#define CONFIG_PMU_EXTLDO_SLEEP_LQ_MODE                             1
#endif

#ifndef CONFIG_PMU_EXTLDO_SLEEP_SW_SUBMODE
#define CONFIG_PMU_EXTLDO_SLEEP_SW_SUBMODE                          0
#endif

#ifndef CONFIG_PMU_EXTLDO_BYPASS
#define CONFIG_PMU_EXTLDO_BYPASS                                    0
#endif

#ifndef CONFIG_PMU_EXTLDO_SHUTDOWN
#define CONFIG_PMU_EXTLDO_SHUTDOWN                                  0
#endif


/* int ldo
 */
#ifndef CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS0
#define CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS0                       0
#endif

#ifndef CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS1
#define CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS1                       0
#endif

#ifndef CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_MMW
#define CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_MMW                        0
#endif

#ifndef CONFIG_PMU_INTLDO_ACTIVE_VOLTAGE
#define CONFIG_PMU_INTLDO_ACTIVE_VOLTAGE                            3
#endif

#ifndef CONFIG_PMU_INTLDO_SLEEP_VOLTAGE
#define CONFIG_PMU_INTLDO_SLEEP_VOLTAGE                             3
#endif

#ifndef CONFIG_PMU_INTLDO_SLEEP_LQ_MODE
#define CONFIG_PMU_INTLDO_SLEEP_LQ_MODE                             1
#endif

#ifndef CONFIG_PMU_INTLDO_SLEEP_SW_SUBMODE
#define CONFIG_PMU_INTLDO_SLEEP_SW_SUBMODE                          0
#endif

#ifndef CONFIG_PMU_INTLDO_BYPASS
#define CONFIG_PMU_INTLDO_BYPASS                                    0
#endif

#ifndef CONFIG_PMU_INTLDO_SHUTDOWN
#define CONFIG_PMU_INTLDO_SHUTDOWN                                  0
#endif

#ifndef CONFIG_POWER_DCDC_LDO_SW_ENABLE
#define CONFIG_POWER_DCDC_LDO_SW_ENABLE                             1
#endif

#ifndef CONFIG_POWER_SWITCH_DELAY_ENABLE
#define CONFIG_POWER_SWITCH_DELAY_ENABLE                            1
#endif


/* soc ldo
 */
#ifndef CONFIG_PMU_SOCLDO_ACTIVE_LQ_MODE_SYS0
#define CONFIG_PMU_SOCLDO_ACTIVE_LQ_MODE_SYS0                       0
#endif

#ifndef CONFIG_PMU_SOCLDO_ACTIVE_LQ_MODE_SYS1
#define CONFIG_PMU_SOCLDO_ACTIVE_LQ_MODE_SYS1                       0
#endif

#ifndef CONFIG_PMU_SOCLDO_ACTIVE_LQ_MODE_MMW
#define CONFIG_PMU_SOCLDO_ACTIVE_LQ_MODE_MMW                        0
#endif

#ifndef CONFIG_PMU_SOCLDO_ACTIVE_VOLTAGE
#define CONFIG_PMU_SOCLDO_ACTIVE_VOLTAGE                            0
#endif

#ifndef CONFIG_PMU_SOCLDO_SLEEP_VOLTAGE
#define CONFIG_PMU_SOCLDO_SLEEP_VOLTAGE                             0
#endif

#ifndef CONFIG_PMU_SOCLDO_SLEEP_LQ_MODE
#define CONFIG_PMU_SOCLDO_SLEEP_LQ_MODE                             1
#endif

#ifndef CONFIG_PMU_SOCLDO_SHUTDOWN
#define CONFIG_PMU_SOCLDO_SHUTDOWN                                  0
#endif


/* rtc ldo
 */
#ifndef CONFIG_PMU_RTCLDO_ACTIVE_VOLTAGE
#define CONFIG_PMU_RTCLDO_ACTIVE_VOLTAGE                            0
#endif

#ifndef CONFIG_PMU_RTCLDO_SLEEP_VOLTAGE
#define CONFIG_PMU_RTCLDO_SLEEP_VOLTAGE                             7
#endif


/* bg
 */
#ifndef CONFIG_PMU_BG_FAST_ON
#define CONFIG_PMU_BG_FAST_ON                                       1
#endif

#ifndef CONFIG_PMU_BG_TRIM
#define CONFIG_PMU_BG_TRIM                                          0
#endif

#ifndef CONFIG_PMU_BG_REF_I_TRIM
#define CONFIG_PMU_BG_REF_I_TRIM                                    0x23
#endif

#ifndef CONFIG_PMU_BG_FS_PD
#define CONFIG_PMU_BG_FS_PD                                         0
#endif

#ifndef CONFIG_PMU_BG_V2I_IREF_FS_PD
#define CONFIG_PMU_BG_V2I_IREF_FS_PD                                0
#endif

#ifndef CONFIG_PMU_BG_FORCE_ON
#define CONFIG_PMU_BG_FORCE_ON                                      0
#endif


/* smps
 */
#ifndef CONFIG_PMU_SMPS_ACTIVE_VOLTAGE
#define CONFIG_PMU_SMPS_ACTIVE_VOLTAGE                              0
#endif

#ifndef CONFIG_PMU_SMPS_SLEEP_VOLTAGE
#define CONFIG_PMU_SMPS_SLEEP_VOLTAGE                               0
#endif

#ifndef CONFIG_PMU_SMPS_FPM_MODE
#define CONFIG_PMU_SMPS_FPM_MODE                                    0
#endif

#ifndef CONFIG_PMU_SMPS_SHUTDOWN
#define CONFIG_PMU_SMPS_SHUTDOWN                                    0
#endif


/* dcxo ldo
 */
#ifndef CONFIG_PMU_DCXOLDO_VOLTAGE
#define CONFIG_PMU_DCXOLDO_VOLTAGE                                  8
#endif

#ifndef CONFIG_PMU_DCXOLDO_ACTIVE_LQ_MODE
#define CONFIG_PMU_DCXOLDO_ACTIVE_LQ_MODE                           0
#endif

#ifndef CONFIG_PMU_DCXOLDO_BYPASS
#define CONFIG_PMU_DCXOLDO_BYPASS                                   0
#endif


/* spll ldo
 */
#ifndef CONFIG_PMU_SPLLLDO_VOLTAGE
#define CONFIG_PMU_SPLLLDO_VOLTAGE                                  8
#endif

#ifndef CONFIG_PMU_SPLLLDO_LQ_MODE
#define CONFIG_PMU_SPLLLDO_LQ_MODE                                  0
#endif


/* efuse
 */
#ifndef CONFIG_PMU_EFUSE_UP_RD_SYS0
#define CONFIG_PMU_EFUSE_UP_RD_SYS0                                 1
#endif

#ifndef CONFIG_PMU_EFUSE_UP_RD_SYS1
#define CONFIG_PMU_EFUSE_UP_RD_SYS1                                 1
#endif

#ifndef CONFIG_PMU_EFUSE_UP_RD_MMW
#define CONFIG_PMU_EFUSE_UP_RD_MMW                                  0
#endif


/* clock power slow smps time pram */
#ifndef CONFIG_PMU_SMPS_POWERUP_GUARD_TIME
#define CONFIG_PMU_SMPS_POWERUP_GUARD_TIME                          0
#endif

#ifndef CONFIG_PMU_SMPS_POWERDW_GUARD_TIME
#define CONFIG_PMU_SMPS_POWERDW_GUARD_TIME                          0
#endif

#ifndef CONFIG_PMU_EFUSE_POWERUP_GUARD_TIME
#define CONFIG_PMU_EFUSE_POWERUP_GUARD_TIME                         0
#endif

#ifndef CONFIG_PMU_SW_POWERUP_GUARD_TIME
#define CONFIG_PMU_SW_POWERUP_GUARD_TIME                            0
#endif

#ifndef CONFIG_PMU_SW_SETTING_GUARD_TIME
#define CONFIG_PMU_SW_SETTING_GUARD_TIME                            0
#endif

#ifndef CONFIG_CLK_SPLL_POWERDWN_GUARD_TIME
#define CONFIG_CLK_SPLL_POWERDWN_GUARD_TIME                         0
#endif

#ifndef CONFIG_PMU_EXTINTLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_EXTINTLDO_POWERUP_GUARD_TIME                     0
#endif

#ifndef CONFIG_PMU_SOCLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_SOCLDO_POWERUP_GUARD_TIME                        8000
#endif

#ifndef CONFIG_PMU_SOCLDO_SETTING_GUARD_TIME
#define CONFIG_PMU_SOCLDO_SETTING_GUARD_TIME                        10000
#endif

#ifndef CONFIG_CLK_DCXO_POWERDWN_GUARD_TIME
#define CONFIG_CLK_DCXO_POWERDWN_GUARD_TIME                         0
#endif


/* clock power dcxo fast fsm time pram */
#ifndef CONFIG_PMU_DCXOLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_DCXOLDO_POWERUP_GUARD_TIME                       1000
#endif

#ifndef CONFIG_CLK_DCXO_POWERUP_GUARD_TIME
#define CONFIG_CLK_DCXO_POWERUP_GUARD_TIME                          1000000
#endif

#ifndef CONFIG_CLK_DCXO_STABLE_TIME
#define CONFIG_CLK_DCXO_STABLE_TIME                                 100
#endif

#ifndef CONFIG_CLK_DCXO_DETECT_TIME
#define CONFIG_CLK_DCXO_DETECT_TIME                                 10000
#endif

#ifndef CONFIG_CLK_DCXO_STARTUP_TIME
#define CONFIG_CLK_DCXO_STARTUP_TIME                                1
#endif


/* clock power spll fast fsm time pram */
#ifndef CONFIG_PMU_SPLLLDO_POWERUP_GUARD_TIME
#define CONFIG_PMU_SPLLLDO_POWERUP_GUARD_TIME                       1000
#endif

#ifndef CONFIG_CLK_SPLL_POWERUP_GUARD_TIME
#define CONFIG_CLK_SPLL_POWERUP_GUARD_TIME                          5000
#endif


/* clock power other time pram */
#ifndef CONFIG_PMU_RTCSW1_POWERUP_GUARD_TIME
#define CONFIG_PMU_RTCSW1_POWERUP_GUARD_TIME                        50
#endif

#ifndef CONFIG_PMU_RTCSW2_POWERUP_GUARD_TIME
#define CONFIG_PMU_RTCSW2_POWERUP_GUARD_TIME                        50
#endif

#ifndef CONFIG_PMU_BG_POWERUP_GUARD_TIME
#define CONFIG_PMU_BG_POWERUP_GUARD_TIME                            20000
#endif


/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

static void power_BgInit(void);
static void power_ExtLdoInit(void);
static void power_IntLdoInit(void);
static void power_SocLdoInit(void);
static void power_RtcLdoInit(void);
static void power_SmpsInit(void);
static void power_DcxoLdoInit(void);
static void power_SpllLdoInit(void);
static void power_EfuseInit(void);
static void power_OtherInit(void);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Status_t HAL_POWER_Init(void)
{
    /* power config */
    power_BgInit();
    power_ExtLdoInit();
    power_IntLdoInit();
    power_SocLdoInit();
    power_RtcLdoInit();
    power_SmpsInit();
    power_DcxoLdoInit();
    power_SpllLdoInit();
    power_EfuseInit();
    power_OtherInit();

    return HAL_STATUS_OK;
}

static void power_BgInit(void)
{
    /* default 0x00 */
    uint8_t bg_trim = CONFIG_PMU_BG_TRIM;

#if CONFIG_DRIVER_EFUSE
    if (HAL_EFUSE_GetPwrTrim(pEfuseDevice, EFUSE_PWR_BG, &bg_trim) != 0) {
        bg_trim = CONFIG_PMU_BG_TRIM;
    }
#endif

    /* 0x23 */
    LL_PMU_BGLDO_SetTrim(bg_trim);
#if CONFIG_PMU_BG_FAST_ON /* default 0x01 */
    LL_PMU_BGLDO_EnableFastOn();
#else
    LL_PMU_BGLDO_DisableFastOn();
#endif
    /* default 0x23 */
    LL_PMU_BGLDO_SetRefITrim(CONFIG_PMU_BG_REF_I_TRIM);
    /* default 1 */
    LL_PMU_BGLDO_SetFsPd(CONFIG_PMU_BG_FS_PD);
    /* default 0 */
    LL_PMU_BGLDO_SetIRefFsPd(CONFIG_PMU_BG_V2I_IREF_FS_PD);

#if CONFIG_PMU_BG_FORCE_ON
    LL_PMU_BGLDO_ForceEnable();
#else
    LL_PMU_BGLDO_ForceDisable();
#endif
}

static void power_ExtLdoInit(void)
{
    /* active state config */
    LL_PMU_EXTLDO_SetActStateVoltage(CONFIG_PMU_EXTLDO_ACTIVE_VOLTAGE);

#if CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_SYS0
    LL_PMU_EXTLDO_Sys0EnableActStateLQMode();
#else
    LL_PMU_EXTLDO_Sys0DisableActStateLQMode();
#endif
#if CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_SYS1
    LL_PMU_EXTLDO_Sys1EnableActStateLQMode();
#else
    LL_PMU_EXTLDO_Sys1DisableActStateLQMode();
#endif
#if CONFIG_PMU_EXTLDO_ACTIVE_LQ_MODE_MMW
    LL_PMU_EXTLDO_MmwEnableActStateLQMode();
#else
    LL_PMU_EXTLDO_MmwDisableActStateLQMode();
#endif


    /* sleep state config */
    LL_PMU_EXTLDO_SetSlpStateVoltage(CONFIG_PMU_EXTLDO_SLEEP_VOLTAGE);

#if CONFIG_PMU_EXTLDO_SLEEP_LQ_MODE
    LL_PMU_EXTLDO_EnableSlpStateLQMode();
#else
    LL_PMU_EXTLDO_DisableSlpStateLQMode();
#endif

#if CONFIG_PMU_EXTLDO_SLEEP_SW_SUBMODE
    LL_PMU_EXTLDO_EnableSlpStateSWSubMode();
#else
    LL_PMU_EXTLDO_DisableSlpStateSWSubMode();
#endif

#if CONFIG_PMU_EXTLDO_BYPASS
    LL_PMU_EXTLDO_EnableBypass();
#else
    LL_PMU_EXTLDO_DisableBypass();
#endif

#if CONFIG_PMU_EXTLDO_SHUTDOWN
    LL_PMU_EXTLDO_EnableShutdownState();
#else
    LL_PMU_EXTLDO_DisableShutdownState();
#endif

}

#if (CONFIG_POWER_DCDC_LDO_SW_ENABLE == 1)

#if CONFIG_POWER_SWITCH_DELAY_ENABLE
void power_usDelay(uint32_t wait_us)
{
    LL_TIM_ClockCtrlOn(1, 0, true);
    /* use 40Mhz as MSI clock */
    LL_TIM_BusyWait(wait_us * 40);
    LL_TIM_ClockCtrlOn(1, 0, false);
}
#endif

uint32_t power_ReadV13(void)
{
    #define GPADC_CH_S8         (8)
    static bool b_adc_config = false;
    uint32_t ch_val, sum_val = 0;
    bool need_restore = true;
    uint32_t adc_trim06 = 2202;
    uint32_t adc_trim24 = 8985;

#if CONFIG_DRIVER_EFUSE
    int status = 0;
    status = HAL_EFUSE_GetGpadcTrim(pEfuseDevice, EFUSE_GPADC_VOLT_06, (uint16_t *)&adc_trim06);
    if (status == 0 && adc_trim06 != 0) {
        adc_trim06 *= 3;
    } else {
        adc_trim06 = 2202;
    }

    status = HAL_EFUSE_GetGpadcTrim(pEfuseDevice, EFUSE_GPADC_VOLT_24, (uint16_t *)&adc_trim24);
    if (status == 0 && adc_trim24 != 0) {
        adc_trim24 *= 3;
    } else {
        adc_trim24 = 8985;
    }

    if (adc_trim24 <= adc_trim06) {
        adc_trim06 = 2202;
        adc_trim24 = 8985;
    }
#endif

    if (b_adc_config == false) {
        /*power on sw 1 and config clock */
        LL_GPADC_SetInitConfig();
        /*setting configure only one time*/
        b_adc_config = true;
    }

    /*disable irq for polling mode*/
    if (IRQ_IsEnabled(GPADC_IRQn)) {
       IRQ_Disable(GPADC_IRQn);
       need_restore = true;
    }

    for (int i = 0; i < 16; i++) {
        LL_GPADC_SetChConfig(GPADC_CH_S8);
        ch_val = LL_GPADC_Polling_GetChVal(GPADC_CH_S8);
        ch_val = LL_GPADC_RawVal2mv(ch_val * 3, adc_trim06, 600, adc_trim24, 2400);
        sum_val += ch_val;
    }
    ch_val = sum_val >> 4;

    if (need_restore == true) {
       /* restore gpadc irq*/
       IRQ_Enable(GPADC_IRQn);
    }

    return ch_val;
}

void power_SwitchLdoToDcdc(void)
{
    /* left for debug
    ** LL_GPIO_ClockOn(PSIC_CLK_GPIO_MSI, PSIC_CLK_GPIO_DIV_1);
    ** LL_GPIO_SetPinFuncMode(GPIOA_DEV, 0, LL_GPIOx_Pn_F1_OUTPUT);
    */
    LL_PMU_INTLDO_SetActStateVoltage(7);
    #if (CONFIG_POWER_SWITCH_DELAY_ENABLE == 1)
    /* LL_GPIO_SetOutputPin(GPIOA_DEV, HW_BIT(1)); */
    /* need more than 580 us to wait voltage to be stable for 10 uf*/
    /* need more than 35ms * 1.3 = 50ms to wait voltage to be stable for 200 uf*/
    power_usDelay(50000);
    #endif

    uint32_t read_vint = power_ReadV13();
    if ((read_vint > 1200) && (read_vint < 1500)) {
        LOG_DBG("disable ldo\n");
        HW_SET_BIT(PMU_DEV->PMU_HBACK, 9);
    }

    LL_PMU_INTLDO_SetActStateVoltage(CONFIG_PMU_INTLDO_ACTIVE_VOLTAGE);
    #if (CONFIG_POWER_SWITCH_DELAY_ENABLE == 1)
    /* need more than 60 us to wait voltage to be stable */
    /* LL_GPIO_SetOutputPin(GPIOA_DEV, HW_BIT(1)); */
    /* need more than 60 us to wait voltage to be stable for 10 uf*/
    /* need more than 2ms to wait voltage to be stable for 200 uf*/
    power_usDelay(2000);
    #endif
}
#endif /* CONFIG_POWER_DCDC_LDO_SW_ENABLE */

static void power_IntLdoInit(void)
{
    /* active state config */
    LL_PMU_INTLDO_SetActStateVoltage(CONFIG_PMU_INTLDO_ACTIVE_VOLTAGE);

    /* default: disable, 0 */
#if CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS0
    LL_PMU_INTLDO_Sys0EnableActStateLQMode();
#else
    LL_PMU_INTLDO_Sys0DisableActStateLQMode();
#endif
#if CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS1
    LL_PMU_INTLDO_Sys1EnableActStateLQMode();
#else
    LL_PMU_INTLDO_Sys1DisableActStateLQMode();
#endif
#if CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_MMW
    LL_PMU_INTLDO_MmwEnableActStateLQMode();
#else
    LL_PMU_INTLDO_MmwDisableActStateLQMode();
#endif

    /* sleep state config */
    LL_PMU_INTLDO_SetSlpStateVoltage(CONFIG_PMU_INTLDO_SLEEP_VOLTAGE);

#if CONFIG_PMU_INTLDO_SLEEP_LQ_MODE
    LL_PMU_INTLDO_EnableSlpStateLQMode();
#else
    LL_PMU_INTLDO_DisableSlpStateLQMode();
#endif

#if CONFIG_PMU_INTLDO_SLEEP_SW_SUBMODE
    LL_PMU_INTLDO_EnableSlpStateSWSubMode();
#else
    LL_PMU_INTLDO_DisableSlpStateSWSubMode();
#endif

    /* other config */
#if CONFIG_PMU_INTLDO_BYPASS
    LL_PMU_INTLDO_EnableBypass();
#else
    LL_PMU_INTLDO_DisableBypass();
#endif

#if CONFIG_PMU_INTLDO_SHUTDOWN
    LL_PMU_INTLDO_EnableShutdownState();
#else
    LL_PMU_INTLDO_DisableShutdownState();
#endif

#if (CONFIG_POWER_DCDC_LDO_SW_ENABLE == 1)
    power_SwitchLdoToDcdc();
#endif /* end of PSIC_DCDC_LDO_SW_EN */

}

static void power_SocLdoInit(void)
{
    LL_PMU_SOCLDO_SetActStateVoltage(CONFIG_PMU_SOCLDO_ACTIVE_VOLTAGE);

#if CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS0
    LL_PMU_SOCLDO_Sys0EnableActStateLQMode();
#else
    LL_PMU_SOCLDO_Sys0DisableActStateLQMode();
#endif
#if CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_SYS1
    LL_PMU_SOCLDO_Sys1EnableActStateLQMode();
#else
    LL_PMU_SOCLDO_Sys1DisableActStateLQMode();
#endif
#if CONFIG_PMU_INTLDO_ACTIVE_LQ_MODE_MMW
    LL_PMU_SOCLDO_MmwEnableActStateLQMode();
#else
    LL_PMU_SOCLDO_MmwDisableActStateLQMode();
#endif

    /* sleep state config */
    LL_PMU_SOCLDO_SetSlpStateVoltage(CONFIG_PMU_SOCLDO_SLEEP_VOLTAGE);

#if CONFIG_PMU_SOCLDO_SLEEP_LQ_MODE
    LL_PMU_SOCLDO_EnableSlpStateLQMode();
#else
    LL_PMU_SOCLDO_DisableSlpStateLQMode();
#endif

#if CONFIG_PMU_SOCLDO_SHUTDOWN
    LL_PMU_SOCLDO_EnableShutdownState();
#else
    LL_PMU_SOCLDO_DisableShutdownState();
#endif

    /* ?? */
    LL_PMU_SOCLDO_EnableFastUp();
    LL_PMU_SOCLDO_EnableBypassUpTiming();
}

static void power_RtcLdoInit(void)
{
    LL_PMU_RTCLDO_SetActStateVoltage(CONFIG_PMU_RTCLDO_ACTIVE_VOLTAGE);
    LL_PMU_RTCLDO_SetSlpStateVoltage(CONFIG_PMU_RTCLDO_SLEEP_VOLTAGE);
}

static void power_SmpsInit(void)
{
    LL_PMU_SMPS_SetActStateVoltage(CONFIG_PMU_SMPS_ACTIVE_VOLTAGE);
    LL_PMU_SMPS_SetSlpStateVoltage(CONFIG_PMU_SMPS_SLEEP_VOLTAGE);

#if CONFIG_PMU_SMPS_FPM_MODE
    LL_PMU_SMPS_EnablePFMMode();
#else
    LL_PMU_SMPS_DisablePFMMode();
#endif

#if CONFIG_PMU_SMPS_SHUTDOWN
    LL_PMU_SMPS_EnableShutdownState();
#else
    LL_PMU_SMPS_DisableShutdownState();
#endif
}

static void power_DcxoLdoInit(void)
{
    LL_PMU_DCXOLDO_SetVoltage(CONFIG_PMU_DCXOLDO_VOLTAGE);

#if CONFIG_PMU_DCXOLDO_ACTIVE_LQ_MODE
    LL_PMU_DCXOLDO_EnableLQMode();
#else
    LL_PMU_DCXOLDO_DisableLQMode();
#endif

#if CONFIG_PMU_DCXOLDO_BYPASS
    LL_PMU_DCXOLDO_EnableBypass();
#else
    LL_PMU_DCXOLDO_DisableBypass();
#endif

    LL_PMU_DCXO_EnableShutdownState();
}

static void power_SpllLdoInit(void)
{
    uint8_t spll_ldo_active_voltage = CONFIG_PMU_SPLLLDO_VOLTAGE;
    #if CONFIG_DRIVER_EFUSE
    if (HAL_EFUSE_GetPwrTrim(pEfuseDevice, EFUSE_PWR_SPLL_LDO, &spll_ldo_active_voltage) != 0) {
        spll_ldo_active_voltage = CONFIG_PMU_SPLLLDO_VOLTAGE;
    }
    #endif

    LL_PMU_SPLLLDO_SetVoltage(spll_ldo_active_voltage);

#if CONFIG_PMU_SPLLLDO_LQ_MODE
    LL_PMU_SPLLLDO_EnableLQMode();
#else
    LL_PMU_SPLLLDO_DisableLQMode();
#endif

    LL_PMU_SPLL_EnableShutdownState();
}

static void power_EfuseInit(void)
{
#if CONFIG_PMU_EFUSE_UP_RD_SYS0
    LL_PMU_EFUSE_EnableSys0AutoRead();
#else
    LL_PMU_EFUSE_DisableSys0AutoRead();
#endif
#if CONFIG_PMU_EFUSE_UP_RD_SYS1
    LL_PMU_EFUSE_EnableSys1AutoRead();
#else
    LL_PMU_EFUSE_DisableSys1AutoRead();
#endif
#if CONFIG_PMU_EFUSE_UP_RD_MMW
    LL_PMU_EFUSE_EnableMmwAutoRead();
#else
    LL_PMU_EFUSE_DisableMmwAutoRead();
#endif
}


static void power_OtherInit(void)
{
    LL_PMU_DPLL_Disable();
#if CONFIG_HW_TRACE_NVM

#else
    #ifndef CONFIG_RTC
    LL_PMU_SW_RTC1SoftDisable();
    #endif
#endif

#ifndef CONFIG_SRAM_B_REMAP_EN
    LL_PMU_SW_RTC2SoftDisable();
#endif
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
