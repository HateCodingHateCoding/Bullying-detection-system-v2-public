/**
 ******************************************************************************
 * @file    rs724x.h
 * @brief   rs724x define.
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


#ifndef _RS624X_H
#define _RS624X_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "soc_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (CONFIG_SOC_SERIES_RS724X)

#include "csi_core.h"
#include "riscv_csr.h"

/* Exported types.
 * ----------------------------------------------------------------------------
 */
typedef enum {
    /* ------  RISC-V Specific Interrupt Numbers  ------ */
    User_Software_IRQn          =  0U,      /* User software interrupt */
    Supervisor_Software_IRQn    =  1U,      /* Supervisor software interrupt */
    Machine_Software_IRQn       =  3U,      /* Machine software interrupt */
    User_Timer_IRQn             =  4U,      /* User timer interrupt */
    Supervisor_Timer_IRQn       =  5U,      /* Supervisor timer interrupt */
    CORET_IRQn                  =  7U,      /* Machine timer interrupt */

    /* ------  RISC-V Peripheral Interrupt Numbers  ------ */
    UART0_IRQn                  =  16,    /* sw1 irq: total 32, 28 */
    UART1_IRQn                  =  17,
    WDG0_IRQn                   =  18,
    WDG1_IRQn                   =  19,
    TMR0_0_IRQn                 =  20,
    TMR1_0_IRQn                 =  21,
    I2C0_IRQn                   =  22,
    I2C1_IRQn                   =  23,
    DMAC_IRQn                   =  24,
    MBOX0_IRQn                  =  25,
    MBOX1_IRQn                  =  26,
    HICM_IRQn                   =  27,
    CDAA_IRQn                   =  28,
    AES_IRQn                    =  29,
    TRNG_IRQn                   =  30,
    QSPI0_IRQn                  =  31,
    QSPI1_IRQn                  =  32,
    FLASHC_IRQn                 =  33,
    MIPITX_IRQn                 =  35,
    PWM_IRQn                    =  36,
    UART2_IRQn                  =  37,
    TMR0_1_IRQn                 =  38,
    TMR1_1_IRQn                 =  39,
    PKA_IRQn                    =  40,
    DMAC_CPUS_IRQn              =  42,
    DMAC_CPUF_IRQn              =  43,
    CAN_IRQn                    =  48,    /* aon irq: total 20,  */
    CDMA_IRQn                   =  49,
    GPIOB_IRQn                  =  50,
    GPIOA_IRQn                  =  51,
    SPIBDG_IRQn                 =  52,
    MPW_BB_IRQn                 =  53,
    MMW_DBGEND_IRQn             =  54,
    TSENSOR_IRQn                =  55,
    DCXO_IRQn                   =  56,
    SPLL_IRQn                   =  57,
    PEAKOVF_IRQn                =  58,
    RFREG_RRR_IRQn              =  59,
    RTC_IRQn                    =  68,    /* rtc irq: total 20 */
    RTT_IRQn                    =  69,
    WIO_IRQn                    =  70,
    MDT_IRQn                    =  71,
    MTPNEG_IRQn                 =  72,
    MTP_IRQn                    =  73,
    FFT2D_IRQn                  =  74,
    FRCNT_IRQn                  =  75,
    INTER_END_IRQn              =  76,
    MMW_WORK_IND_IRQn           =  77,
    RTT1_IRQn                   =  78,
    GPADC_IRQn                  =  79,
    SPI_WUP_IRQn                =  80,
    UART_WUP_IRQn               =  81,
    IIC_WUP_IRQn                =  82,
    TMR_WUP0_IRQn               =  83,
    TMR_WUP1_IRQn               =  84,
    PWM_INTR_IRQn               =  85
} IRQn_Type;


/* Device Peripheral Address Definition.
 * ----------------------------------------------------------------------
 */
/*!< Boundary memory map */
#define I_BUS_BASE_ADDR                 (0x00000000UL)    /*!< I-bus base address */
#define D_BUS_BASE_ADDR                 (0x10000000UL)    /*!< D-bus base address */
#define S_BUS_BASE_ADDR                 (0x20000000UL)    /*!< S-bus base address */
#define PERIPH_BASE_ADDR                (0x40000000UL)    /*!< Peripheral base address */

#define FLASH_I_BUS_BASE                (0x08000000UL)    /*!< FLASH I-bus base address */
#define FLASH_D_BUS_BASE                (0x18000000UL)    /*!< FLASh I-bus base address */
#define FLASH_D_BUS_MASK                (0x0FFFFFFFUL)
#define PSRAM_I_BUS_BASE                (0x0C000000UL)    /*!< PSRAM I-bus base address */
#define PSRAM_D_BUS_BASE                (0x1C000000UL)    /*!< PSRAM D-bus base address */
#define PSRAM_D_BUS_MASK                (0x0FFFFFFFUL)
#define XIP_PSRAM_I_BUS_MASK            (0x00FFFFFFUL)

#define REMAP_I_BUS_BASE                (0x00500000UL)    /*!< REMAP I-bus base address */
#define REMAP_I_BUS_AREA                (0x00040000UL)    /*!< REMAP D-bus base address */
#define REMAP_I_BUS_AREA_MSK            (REMAP_I_BUS_AREA - 1)

/*!< SRAM memory map */
#define ROM_OFFSET                      (0x00000000UL)

#define SRAM_A_0_OFFSET                 (0x00400000UL)    /*!< 32KB */
#define SRAM_A_1_OFFSET                 (0x00408000UL)    /*!< 32KB */
#define SRAM_A_2_OFFSET                 (0x00410000UL)    /*!< 32KB */
#define SRAM_A_3_OFFSET                 (0x00418000UL)    /*!< 32KB */
#define SRAM_A_4_OFFSET                 (0x00420000UL)    /*!< 32KB */
#define SRAM_A_5_OFFSET                 (0x00428000UL)    /*!< 32KB */
#define SRAM_A_6_OFFSET                 (0x00430000UL)    /*!< 32KB */
#define SRAM_A_7_OFFSET                 (0x00438000UL)    /*!< 32KB */
#define SRAM_A_8_OFFSET                 (0x00440000UL)    /*!< 23KB */
#define SRAM_A_9_OFFSET                 (0x00445C00UL)    /*!< 16KB */

#define SRAM_B_0_OFFSET                 (0x01000000UL)    /*!< 16KB */
#define SRAM_B_1_OFFSET                 (0x01004000UL)    /*!< 16KB */
#define SRAM_B_2_OFFSET                 (0x01008000UL)    /*!< 32KB */
#define SRAM_B_3_OFFSET                 (0x01010000UL)    /*!< 64KB */
#define SRAM_B_4_OFFSET                 (0x01020000UL)    /*!< 128KB */
#define SRAM_B_AREA_MSK                 (0xFF000000UL)

#define I_REMAP_0_OFFSET                (0x00500000UL)    /*!< 256KB */
#define I_REMAP_1_OFFSET                (0x00540000UL)    /*!< 256KB */
#define I_REMAP_2_OFFSET                (0x00580000UL)    /*!< 256KB */
#define I_REMAP_3_OFFSET                (0x005C0000UL)    /*!< 256KB */

#define D_REMAP_0_OFFSET                (0x00500000UL)    /*!< 256KB */
#define D_REMAP_1_OFFSET                (0x00540000UL)    /*!< 256KB */
#define D_REMAP_2_OFFSET                (0x00580000UL)    /*!< 256KB */


/*!< Peripheral memory map */
#define APB0_PERIPH_BASE_ADDR           (PERIPH_BASE_ADDR + 0x00000000UL)    /*!< 0x4000 0000 */
#define APB1_PERIPH_BASE_ADDR           (PERIPH_BASE_ADDR + 0x00010000UL)    /*!< 0x4001 0000 */
#define AHB_PERIPH_BASE_ADDR            (PERIPH_BASE_ADDR + 0x00020000UL)    /*!< 0x4002 0000 */


/*!< APB0 peripherals, 4000 0000 */
#define CDAA_BASE_ADDR                  (APB0_PERIPH_BASE_ADDR + 0x00000000UL) /*!< 0x4000 0000 */
#define I2C0_BASE_ADDR                  (APB0_PERIPH_BASE_ADDR + 0x00004000UL) /*!< 0x4000 4000 */
#define I2C1_BASE_ADDR                  (APB0_PERIPH_BASE_ADDR + 0x00004400UL) /*!< 0x4000 4400 */
#define UART1_BASE_ADDR                 (APB0_PERIPH_BASE_ADDR + 0x00005000UL) /*!< 0x4000 5000 */
#define MBX0_BASE_ADDR                  (APB0_PERIPH_BASE_ADDR + 0x00006000UL) /*!< 0x4000 6000 */
#define MBX1_BASE_ADDR                  (APB0_PERIPH_BASE_ADDR + 0x00006400UL) /*!< 0x4000 6400 */
#define LA_BASE_ADDR                    (APB0_PERIPH_BASE_ADDR + 0x00006800UL) /*!< 0x4000 6800 */
#define PKA_BASE_ADDR                   (APB0_PERIPH_BASE_ADDR + 0x00008000UL) /*!< 0x4000 8000 */


/*!< APB1 peripherals, 4001 0000 */
#define TIMR0_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x00000000UL) /*!< 0x4001 0000 */
#define TIMR1_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x00000400UL) /*!< 0x4001 0400 */
#define WDG0_BASE_ADDR                  (APB1_PERIPH_BASE_ADDR + 0x00001000UL) /*!< 0x4001 1000 */
#define WDG1_BASE_ADDR                  (APB1_PERIPH_BASE_ADDR + 0x00001400UL) /*!< 0x4001 1400 */
#define UART0_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x00001C00UL) /*!< 0x4001 1C00 */
#define UART2_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x00002000UL) /*!< 0x4001 2000 */
#define SPLK_BASE_ADDR                  (APB1_PERIPH_BASE_ADDR + 0x00002C00UL) /*!< 0x4001 2C00 */
#define TRNG_BASE_ADDR                  (APB1_PERIPH_BASE_ADDR + 0x00006000UL) /*!< 0x4001 6000 */
#define AES_BASE_ADDR                   (APB1_PERIPH_BASE_ADDR + 0x00006400UL) /*!< 0x4001 6400 */
#define HICM_BASE_ADDR                  (APB1_PERIPH_BASE_ADDR + 0x00006C00UL) /*!< 0x4001 6C00 */
#define CCMU1_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x00007000UL) /*!< 0x4001 7000 */
#define SWCFG_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x00007400UL) /*!< 0x4001 7400 */
#define RTC_BASE_ADDR                   (APB1_PERIPH_BASE_ADDR + 0x00008000UL) /*!< 0x4001 8000 */
#define RTCSW_MISC_BASE_ADDR            (APB1_PERIPH_BASE_ADDR + 0x00008400UL) /*!< 0x4001 8400 */
#define GPADC_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x00008800UL) /*!< 0x4001 8800 */
#define RCC_BASE_ADDR                   (APB1_PERIPH_BASE_ADDR + 0x00009000UL) /*!< 0x4001 9000 */
#define PWR_BASE_ADDR                   (APB1_PERIPH_BASE_ADDR + 0x0000A000UL) /*!< 0x4001 A000 */
#define RTCSW2_MISC_BASE_ADDR           (APB1_PERIPH_BASE_ADDR + 0x0000B000UL) /*!< 0x4001 B000 */
#define SYSCFG_BASE_ADDR                (APB1_PERIPH_BASE_ADDR + 0x0000C000UL) /*!< 0x4001 C000 */
#define GPIO_BASE_ADDR                  (APB1_PERIPH_BASE_ADDR + 0x0000D000UL) /*!< 0x4001 D000 */
#define RCCAL_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x0000D400UL) /*!< 0x4001 D400 */
#define EFUSE_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x0000D800UL) /*!< 0x4001 D800 */
#define CAN_BASE_ADDR                   (APB1_PERIPH_BASE_ADDR + 0x0000DC00UL) /*!< 0x4001 DC00 */
#define CCMU2_BASE_ADDR                 (APB1_PERIPH_BASE_ADDR + 0x0000E000UL) /*!< 0x4001 E000 */
#define PWR2_BASE_ADDR                  (APB1_PERIPH_BASE_ADDR + 0x0000E400UL) /*!< 0x4001 E400 */
#define PWM_BASE_ADDR                   (APB1_PERIPH_BASE_ADDR + 0x0000E800UL) /*!< 0x4001 E800 */


/*!< AHB peripherals, 4002 0000 */
#define DMAC_BASE_ADDR                  (AHB_PERIPH_BASE_ADDR + 0x00000C00UL)  /*!< 0x4002 0C00 */
#define FCTRL_BASE_ADDR                 (AHB_PERIPH_BASE_ADDR + 0x00001000UL)  /*!< 0x4002 1000 */
#define QSPI_BASE_ADDR                  (AHB_PERIPH_BASE_ADDR + 0x00002000UL)  /*!< 0x4002 2000 */
#define MPW_BASE_ADDR                   (AHB_PERIPH_BASE_ADDR + 0x00600000UL)  /*!< 0x4060 0000 */
#define MMW_BASE_ADDR                   (AHB_PERIPH_BASE_ADDR + 0x00800000UL)  /*!< 0x4080 0000 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#define CONFIG_SOC_GPIOA                                1
#define CONFIG_SOC_GPIOA_PIN_NUM                        16

#define CONFIG_SOC_GPIOB                                1
#define CONFIG_SOC_GPIOB_PIN_NUM                        8


#define CONFIG_SOC_WKIOA                                1
#define CONFIG_SOC_WKIOA_PIN_NUM                        18

#define CONFIG_SOC_WKIOB                                0
#define CONFIG_SOC_WKIOB_PIN_NUM                        0


#define CONFIG_SOC_WDG0                                 1
#define CONFIG_SOC_WDG1                                 1



#endif /* CONFIG_SOC_SERIES_RS724X */

#ifdef __cplusplus
}
#endif

#endif /* _RS724X_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
