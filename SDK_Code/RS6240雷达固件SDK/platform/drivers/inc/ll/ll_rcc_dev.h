/**
 **************************************************************************************************
 * @file   psic_ll_rcc_dev.h
 * @brief   reset clock hadware define for bus.
 * @attention
 *
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/*
 * 1.1 common Peripheral
 *  GPIO
 *  UART 0/1/2
 *  I2C 0/1
 *  QSPI
 *  Flash CTRL
 *  PWM
 *  GPADC
 *  MCAN
 *  CAN
 *  CDAA
 *
 *  1.2 common Timer
 *  TIMER 0/1
 *  WDG 0/1
 *  RTC/RTT
 *
 * 1.3 Auxiliary
 *  DMAC
 *  MIN DSP
 *  SPIN LOCK
 *  MAIL BOX 0/1
 *  EFUSE CTRL
 *  RCCAL
 *
 * 1.4 Security
 *  PKA
 *  HICM
 *  AES
 *  TRNG
 *
 * 1.5 Lage module
 *  MMW
 *  MPW
 * 1.6 Debug
 *  LA
 *
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_RCC_DEV_H
#define _PSIC_LL_RCC_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "types.h"
#include "hw_ccmu1.h"
#include "hw_ccmu2.h"
#include "hw_pmu.h"
#include "hw_pmu2.h"
#include "hw_rcc.h"
#include "hw_syscfg.h"
#include "hw_swcfg.h"
#include "hw_misc.h"
#include "hw_misc2.h"
#include "soc.h"
#include "hw_gpadc.h"

/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup psic_ll_rcc
  * @{
  */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup reset clock define
  * @{
  */

/* must be consistent with hardware */
/* function clock source */
#define LL_RCC_GPIO_MSI                    0
#define LL_RCC_GPIO_AHB                    1
#define LL_RCC_GPIO_HSE                    2
#define LL_RCC_GPIO_LSI                    3
#define LL_RCC_GPIO_BUS_MAX                3
/* function clock div*/
#define LL_RCC_GPIO_DIV_1                  0
#define LL_RCC_GPIO_DIV_MAX                0


/* function clock source */
#define LL_RCC_WPIO_LSE                    0
#define LL_RCC_WPIO_LPCLK_IDIV             1
#define LL_RCC_WPIO_LPCLK_FDIV             2
#define LL_RCC_WPIO_BUS_MAX                2
/* function clock div*/
#define LL_RCC_WPIO_DIV_1                  0
#define LL_RCC_WPIO_DIV_MAX                0


/* function clock source */
#define LL_RCC_UART0_APB1                  0
#define LL_RCC_UART0_MSI                   1
#define LL_RCC_UART0_HSE                   2
#define LL_RCC_UART0_PLL_DEV               3
#define LL_RCC_UART0_PLL_DEV1              4
#define LL_RCC_UART0_BUS_MAX               4
/* function clock div*/
#define LL_RCC_UART0_DIV_1                 0
#define LL_RCC_UART0_DIV_2                 1
#define LL_RCC_UART0_DIV_4                 2
#define LL_RCC_UART0_DIV_8                 3
#define LL_RCC_UART0_DIV_MAX               3


/* function clock source */
#define LL_RCC_UART1_APB0                  0
#define LL_RCC_UART1_MSI                   1
#define LL_RCC_UART1_HSE                   2
#define LL_RCC_UART1_PLL_DEV               3
#define LL_RCC_UART1_PLL_DEV1              4
#define LL_RCC_UART1_BUS_MAX               4
/* function clock div*/
#define LL_RCC_UART1_DIV_1                 0
#define LL_RCC_UART1_DIV_2                 1
#define LL_RCC_UART1_DIV_4                 2
#define LL_RCC_UART1_DIV_8                 3
#define LL_RCC_UART1_DIV_MAX               3


/* function clock source */
#define LL_RCC_UART2_APB1                  0
#define LL_RCC_UART2_MSI                   1
#define LL_RCC_UART2_HSE                   2
#define LL_RCC_UART2_PLL_DEV               3
#define LL_RCC_UART2_PLL_DEV1              4
#define LL_RCC_UART2_BUS_MAX               4
/* function clock div*/
#define LL_RCC_UART2_DIV_1                 0
#define LL_RCC_UART2_DIV_2                 1
#define LL_RCC_UART2_DIV_4                 2
#define LL_RCC_UART2_DIV_8                 3
#define LL_RCC_UART2_DIV_MAX               3


#define LL_RCC_UART_APB                    0
#define LL_RCC_UART_MSI                    1
#define LL_RCC_UART_HSE                    2
#define LL_RCC_UART_PLL_DEV                3
#define LL_RCC_UART_PLL_DEV1               4
#define LL_RCC_UART_BUS_MAX                4

#define LL_RCC_UART_DIV_1                  0
#define LL_RCC_UART_DIV_2                  1
#define LL_RCC_UART_DIV_4                  2
#define LL_RCC_UART_DIV_8                  3
#define LL_RCC_UART_DIV_MAX                3


/* function clock source */
#define LL_RCC_I2C0_APB0                   0
#define LL_RCC_I2C0_MSI                    1
#define LL_RCC_I2C0_HSE                    2
#define LL_RCC_I2C0_PLL_DEV                3
#define LL_RCC_I2C0_BUS_MAX                3
/* function clock div*/
#define LL_RCC_I2C0_DIV_1                  0
#define LL_RCC_I2C0_DIV_2                  1
#define LL_RCC_I2C0_DIV_4                  2
#define LL_RCC_I2C0_DIV_8                  3
#define LL_RCC_I2C0_DIV_MAX                3


/* function clock source */
#define LL_RCC_I2C1_APB0                   0
#define LL_RCC_I2C1_MSI                    1
#define LL_RCC_I2C1_HSE                    2
#define LL_RCC_I2C1_PLL_DEV                3
#define LL_RCC_I2C1_BUS_MAX                3
/* function clock div*/
#define LL_RCC_I2C1_DIV_1                  0
#define LL_RCC_I2C1_DIV_2                  1
#define LL_RCC_I2C1_DIV_4                  2
#define LL_RCC_I2C1_DIV_8                  3
#define LL_RCC_I2C1_DIV_MAX                3


/* function clock source */
#define LL_RCC_SPI_AHB                     0
#define LL_RCC_SPI_MSI                     1
#define LL_RCC_SPI_HSE                     2
#define LL_RCC_SPI_PLL_DEV1                3
#define LL_RCC_SPI_BUS_MAX                 3
/* function clock div*/
#define LL_RCC_SPI_DIV_1                   0
#define LL_RCC_SPI_DIV_2                   1
#define LL_RCC_SPI_DIV_4                   2
#define LL_RCC_SPI_DIV_8                   3
#define LL_RCC_SPI_DIV_MAX                 3


/* function clock source */
#define LL_RCC_FSMC_AHB                    0
#define LL_RCC_FSMC_MSI                    1
#define LL_RCC_FSMC_HSE                    2
#define LL_RCC_FSMC_PLL_DEV1               3
#define LL_RCC_FSMC_BUS_MAX                3
/* function clock div*/
#define LL_RCC_FSMC_DIV_1                  0
#define LL_RCC_FSMC_DIV_2                  1
#define LL_RCC_FSMC_DIV_4                  2
#define LL_RCC_FSMC_DIV_8                  3
#define LL_RCC_FSMC_DIV_MAX                3


/* function clock source */
#define LL_RCC_PWM_LPCLK                   0
#define LL_RCC_PWM_HSE                     1
#define LL_RCC_PWM_MSI                     2
#define LL_RCC_PWM_PLL_DEV                 3
#define LL_RCC_PWM_BUS_MAX                 3
/* function clock div*/
#define LL_RCC_PWM_DIV_1                   0
#define LL_RCC_PWM_DIV_2                   1
#define LL_RCC_PWM_DIV_4                   2
#define LL_RCC_PWM_DIV_8                   3
#define LL_RCC_PWM_DIV_MAX                 3


/* function clock source */
#define LL_RCC_GPADC_LPCLK                 0
#define LL_RCC_GPADC_HSE                   1
#define LL_RCC_GPADC_MSI                   2
#define LL_RCC_GPADC_PLL_DEV               3
#define LL_RCC_GPADC_BUS_MAX               3
/* function clock div*/
#define LL_RCC_GPADC_DIV_1                 0
#define LL_RCC_GPADC_DIV_2                 1
#define LL_RCC_GPADC_DIV_4                 2
#define LL_RCC_GPADC_DIV_8                 3
#define LL_RCC_GPADC_DIV_16                4
#define LL_RCC_GPADC_DIV_32                5
#define LL_RCC_GPADC_DIV_64                6
#define LL_RCC_GPADC_DIV_128               7
#define LL_RCC_GPADC_DIV_MAX               7


/* function clock source */
#define LL_RCC_GPADC_TMR_LSI               0
#define LL_RCC_GPADC_TMR_LPCLK_IDIV        1
#define LL_RCC_GPADC_TMR_LPCLK_FDIV        2
#define LL_RCC_GPADC_TMR_LPCLK             3
#define LL_RCC_GPADC_TMR_BUS_MAX           3
/* function clock div*/
#define LL_RCC_GPADC_TMR_DIV_1             0
#define LL_RCC_GPADC_TMR_DIV_MAX           0


/* function clock source */
#define LL_RCC_MCAN_PLL_SOC                0
#define LL_RCC_MCAN_MSI                    1
#define LL_RCC_MCAN_HSE                    2
#define LL_RCC_MCAN_PLL_DEV                3
#define LL_RCC_MCAN_BUS_MAX                3
/* function clock div*/
#define LL_RCC_MCAN_DIV_1                  0
#define LL_RCC_MCAN_DIV_2                  1
#define LL_RCC_MCAN_DIV_4                  2
#define LL_RCC_MCAN_DIV_8                  3
#define LL_RCC_MCAN_DIV_MAX                3


/* function clock source */
#define LL_RCC_CAN_PLL_SOC                 0
#define LL_RCC_CAN_MSI                     1
#define LL_RCC_CAN_HSE                     2
#define LL_RCC_CAN_PLL_DEV                 3
#define LL_RCC_CAN_BUS_MAX                 3
/* function clock div*/
#define LL_RCC_CAN_DIV_1                   0
#define LL_RCC_CAN_DIV_2                   1
#define LL_RCC_CAN_DIV_4                   2
#define LL_RCC_CAN_DIV_8                   3
#define LL_RCC_CAN_DIV_MAX                 3


/* function clock source */
#define LL_RCC_CDAA_PLL_CDAA               0
#define LL_RCC_CDAA_BUS_MAX                0
/* function clock div*/
#define LL_RCC_CDAA_DIV_1                  0
#define LL_RCC_CDAA_DIV_2                  1
#define LL_RCC_CDAA_DIV_4                  2
#define LL_RCC_CDAA_DIV_8                  3
#define LL_RCC_CDAA_DIV_MAX                3


/* function clock source */
#define LL_RCC_TMR0_APB1                  0
#define LL_RCC_TMR0_MSI                   1
#define LL_RCC_TMR0_HSE                   2
#define LL_RCC_TMR0_LSI                   3
#define LL_RCC_TMR0_BUS_MAX               3
/* function clock div*/
#define LL_RCC_TMR0_DIV_1                 0
#define LL_RCC_TMR0_DIV_2                 1
#define LL_RCC_TMR0_DIV_4                 2
#define LL_RCC_TMR0_DIV_8                 3
#define LL_RCC_TMR0_DIV_16                4
#define LL_RCC_TMR0_DIV_32                5
#define LL_RCC_TMR0_DIV_64                6
#define LL_RCC_TMR0_DIV_128               7
#define LL_RCC_TMR0_DIV_MAX               7


/* function clock source */
#define LL_RCC_TMR1_APB1                  0
#define LL_RCC_TMR1_MSI                   1
#define LL_RCC_TMR1_HSE                   2
#define LL_RCC_TMR1_LSI                   3
#define LL_RCC_TMR1_BUS_MAX               3
/* function clock div*/
#define LL_RCC_TMR1_DIV_1                 0
#define LL_RCC_TMR1_DIV_2                 1
#define LL_RCC_TMR1_DIV_4                 2
#define LL_RCC_TMR1_DIV_8                 3
#define LL_RCC_TMR1_DIV_16                4
#define LL_RCC_TMR1_DIV_32                5
#define LL_RCC_TMR1_DIV_64                6
#define LL_RCC_TMR1_DIV_128               7
#define LL_RCC_TMR1_DIV_MAX               7


/* function clock source */
#define LL_RCC_TMR2_APB1                  0
#define LL_RCC_TMR2_MSI                   1
#define LL_RCC_TMR2_HSE                   2
#define LL_RCC_TMR2_LSI                   3
#define LL_RCC_TMR2_BUS_MAX               3
/* function clock div*/
#define LL_RCC_TMR2_DIV_1                 0
#define LL_RCC_TMR2_DIV_2                 1
#define LL_RCC_TMR2_DIV_4                 2
#define LL_RCC_TMR2_DIV_8                 3
#define LL_RCC_TMR2_DIV_16                4
#define LL_RCC_TMR2_DIV_32                5
#define LL_RCC_TMR2_DIV_64                6
#define LL_RCC_TMR2_DIV_128               7
#define LL_RCC_TMR2_DIV_MAX               7


/* function clock source */
#define LL_RCC_TMR3_APB1                  0
#define LL_RCC_TMR3_MSI                   1
#define LL_RCC_TMR3_HSE                   2
#define LL_RCC_TMR3_LSI                   3
#define LL_RCC_TMR3_BUS_MAX               3
/* function clock div*/
#define LL_RCC_TMR3_DIV_1                 0
#define LL_RCC_TMR3_DIV_2                 1
#define LL_RCC_TMR3_DIV_4                 2
#define LL_RCC_TMR3_DIV_8                 3
#define LL_RCC_TMR3_DIV_16                4
#define LL_RCC_TMR3_DIV_32                5
#define LL_RCC_TMR3_DIV_64                6
#define LL_RCC_TMR3_DIV_128               7
#define LL_RCC_TMR3_DIV_MAX               7


/* function clock source */
#define LL_RCC_WDG0_APB1                   0
#define LL_RCC_WDG0_MSI                    1
#define LL_RCC_WDG0_HSE                    2
#define LL_RCC_WDG0_LSI                    3
#define LL_RCC_WDG0_BUS_MAX                3
/* function clock div*/
#define LL_RCC_WDG0_DIV_1                  0
#define LL_RCC_WDG0_DIV_2                  1
#define LL_RCC_WDG0_DIV_4                  2
#define LL_RCC_WDG0_DIV_8                  3
#define LL_RCC_WDG0_DIV_16                 4
#define LL_RCC_WDG0_DIV_32                 5
#define LL_RCC_WDG0_DIV_64                 6
#define LL_RCC_WDG0_DIV_128                7
#define LL_RCC_WDG0_DIV_MAX                7


/* function clock source */
#define LL_RCC_WDG1_APB1                   0
#define LL_RCC_WDG1_MSI                    1
#define LL_RCC_WDG1_HSE                    2
#define LL_RCC_WDG1_LSI                    3
#define LL_RCC_WDG1_BUS_MAX                3
/* function clock div*/
#define LL_RCC_WDG1_DIV_1                  0
#define LL_RCC_WDG1_DIV_2                  1
#define LL_RCC_WDG1_DIV_4                  2
#define LL_RCC_WDG1_DIV_8                  3
#define LL_RCC_WDG1_DIV_16                 4
#define LL_RCC_WDG1_DIV_32                 5
#define LL_RCC_WDG1_DIV_64                 6
#define LL_RCC_WDG1_DIV_128                7
#define LL_RCC_WDG1_DIV_MAX                7


/* function clock source */
#define LL_RCC_RTC_LSE                     0
#define LL_RCC_RTC_LPCLK_IDIV              1
#define LL_RCC_RTC_LPCLK_FDIV              2
#define LL_RCC_RTC_BUS_MAX                 2
/* function clock div*/
#define LL_RCC_RTC_DIV_1                   0
#define LL_RCC_RTC_DIV_MAX                 0


/* function clock source */
#define LL_RCC_RTT_LSE                     0
#define LL_RCC_RTT_LPCLK_IDIV              1
#define LL_RCC_RTT_LPCLK_FDIV              2
#define LL_RCC_RTT_BUS_MAX                 2
/* function clock div*/
#define LL_RCC_RTT_DIV_1                   0
#define LL_RCC_RTT_DIV_MAX                 0


/* function clock source */
#define LL_RCC_RTICK_LSE                   0
#define LL_RCC_RTICK_LPCLK_IDIV            1
#define LL_RCC_RTICK_LPCLK_FDIV            2
#define LL_RCC_RTICK_BUS_MAX               2
/* function clock div*/
#define LL_RCC_RTICK_DIV_1                 0
#define LL_RCC_RTICK_DIV_MAX               0


/* function clock source */
#define LL_RCC_MDSP_AHB                    0
#define LL_RCC_MDSP_BUS_MAX                0
/* function clock div*/
#define LL_RCC_MDSP_DIV_1                  0
#define LL_RCC_MDSP_DIV_MAX                0


/* function clock source */
#define LL_RCC_DMA_AHB                     0
#define LL_RCC_DMA_BUS_MAX                 0
/* function clock div*/
#define LL_RCC_DMA_DIV_1                   0
#define LL_RCC_DMA_DIV_MAX                 0


/* function clock source */
#define LL_RCC_SPLK_AHB                    0
#define LL_RCC_SPLK_BUS_MAX                0
/* function clock div*/
#define LL_RCC_SPLK_DIV_1                  0
#define LL_RCC_SPLK_DIV_MAX                0


/* function clock source */
#define LL_RCC_MBX0_AHB                    0
#define LL_RCC_MBX0_BUS_MAX                0
/* function clock div*/
#define LL_RCC_MBX0_DIV_1                  0
#define LL_RCC_MBX0_DIV_MAX                0


/* function clock source */
#define LL_RCC_MBX1_AHB                    0
#define LL_RCC_MBX1_BUS_MAX                0
/* function clock div*/
#define LL_RCC_MBX1_DIV_1                  0
#define LL_RCC_MBX1_DIV_MAX                0


/* function clock source */
#define LL_RCC_EFUSE_MSI                   0
#define LL_RCC_EFUSE_LSI                   1
#define LL_RCC_EFUSE_HSE                   3
#define LL_RCC_EFUSE_BUS_MAX               3
/* function clock div*/
#define LL_RCC_EFUSE_DIV_1                 0
#define LL_RCC_EFUSE_DIV_2                 1
#define LL_RCC_EFUSE_DIV_4                 2
#define LL_RCC_EFUSE_DIV_8                 3
#define LL_RCC_EFUSE_DIV_16                4
#define LL_RCC_EFUSE_DIV_MAX               4


/* function clock source */
#define LL_RCC_RCCAL_MSI                   0
#define LL_RCC_RCCAL_BUS_MAX               3
/* function clock div*/
#define LL_RCC_RCCAL_DIV_1                 0
#define LL_RCC_RCCAL_DIV_MAX               0


/* function clock source */
#define LL_RCC_PKA_APB0                    0
#define LL_RCC_PKA_BUS_MAX                 0
/* function clock div*/
#define LL_RCC_PKA_DIV_1                   0
#define LL_RCC_PKA_DIV_MAX                 0


/* function clock source */
#define LL_RCC_HICM_APB1                   0
#define LL_RCC_HICM_BUS_MAX                0
/* function clock div*/
#define LL_RCC_HICM_DIV_1                  0
#define LL_RCC_HICM_DIV_MAX                0


/* function clock source */
#define LL_RCC_AES_APB1                    0
#define LL_RCC_AES_BUS_MAX                 0
/* function clock div*/
#define LL_RCC_AES_DIV_1                   0
#define LL_RCC_AES_DIV_MAX                 0


/* function clock source */
#define LL_RCC_TRNG_APB1                   0
#define LL_RCC_TRNG_BUS_MAX                0
/* function clock div*/
#define LL_RCC_TRNG_DIV_1                  0
#define LL_RCC_TRNG_DIV_MAX                0


/* function clock source */
#define LL_RCC_MCO_MSI                     0
#define LL_RCC_MCO_HSE                     1
#define LL_RCC_MCO_HSE_M50                 2
#define LL_RCC_MCO_LSI                     3
#define LL_RCC_MCO_PLL_DEV                 4
#define LL_RCC_MCO_PLL_DEV1                5
#define LL_RCC_MCO_PLL_SOC                 6
#define LL_RCC_MCO_AHB                     7
#define LL_RCC_MCO_LSE                     8
#define LL_RCC_MCO_LSE_M50                 9
#define LL_RCC_MCO_PLL_CDAA                10
#define LL_RCC_MCO_MMW                     11
#define LL_RCC_MCO_BUS_MAX                 11
/* function clock div*/
#define LL_RCC_MCO_DIV_1                   0
#define LL_RCC_MCO_DIV_2                   1
#define LL_RCC_MCO_DIV_4                   2
#define LL_RCC_MCO_DIV_8                   3
#define LL_RCC_MCO_DIV_16                  4
#define LL_RCC_MCO_DIV_MAX                 4

/**
  * @}
  */

#define LL_RCC_CAN_MODE_20B					0
#define LL_RCC_CAN_MODE_FD					1


/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */


/** @defgroup GPIO
  * @{
  */
/*
 * clock source:
 * msi, sysclk, dcxo, lsi
 */
__HW_STATIC_INLINE
void LL_RCC_GPIO_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_GPIO_FCLK_SEL, SYSCFG_GPIO_FCLK_SRC_SEL_Msk, SYSCFG_GPIO_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_GPIO_GetClockSource(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_GPIO_FCLK_SEL, SYSCFG_GPIO_FCLK_SRC_SEL_Msk, SYSCFG_GPIO_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_GPIO_SetPrescaler(uint32_t prescaler)
{
/* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_GPIO_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_GPIO_EnableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->CCMU2_CLK_EN, SYSCFG_GPIO_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_GPIO_DisableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->CCMU2_CLK_EN, SYSCFG_GPIO_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_GPIO_IsEnabledClock(void)
{
  return HW_TEST_BIT(CCMU2_DEV->CCMU2_CLK_EN, SYSCFG_GPIO_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_GPIO_EnableBusClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_GPIO_DisableBusClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_GPIO_Reset(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_RESETN_EN, GPIO_RESETN_EN_Msk);
	HW_SET_MSK(CCMU2_DEV->CCMU2_RESETN_EN, GPIO_RESETN_EN_Msk);
}
/**
  * @}
  */



/** @defgroup WPIO
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_WPIO_SetClockSource(uint32_t source)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_WPIO_GetClockSource(void)
{
  return 0;
}

__HW_STATIC_INLINE
void LL_RCC_WPIO_SetPrescaler(uint32_t prescaler)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_WPIO_GetPrescaler(void)
{
   return 0;
}

__HW_STATIC_INLINE
void LL_RCC_WPIO_EnableClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_WPIO_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_WPIO_IsEnabledClock(void)
{
  return 0;
}

__HW_STATIC_INLINE
void LL_RCC_WPIO_EnableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_WPIO_DisableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_WPIO_Reset(void)
{

}
/**
  * @}
  */



/** @defgroup UART0
  * @{
  */
/*
 * clock source:
 * apb1, msi, dcxo, pll_clkdev, pll_clkdev1
 */
__HW_STATIC_INLINE
void LL_RCC_UART0_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_UART0_FCLK_SRC_SEL, CCMU_UART0_FCLK_SRC_SEL_Msk, CCMU_UART0_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART0_GetClockSource(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_UART0_FCLK_SRC_SEL, CCMU_UART0_FCLK_SRC_SEL_Msk, CCMU_UART0_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART0_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_UART0_FCLK_SRC_SEL, CCMU_UART0_FCLK_POSTDIV_Msk, CCMU_UART0_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART0_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_UART0_FCLK_SRC_SEL, CCMU_UART0_FCLK_POSTDIV_Msk, CCMU_UART0_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART0_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, UART0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART0_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, UART0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART0_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, UART0_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART0_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, UART0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, UART0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART0_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, UART0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, UART0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART0_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, UART0_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, UART0_RESETN_Msk);
}
/**
  * @}
  */


/** @defgroup UART1
  * @{
  */
/*
 * clock source:
 * apb0, msi, dcxo, pll_clkdev, pll_clkdev1
 */
__HW_STATIC_INLINE
void LL_RCC_UART1_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_UART1_FCLK_SRC_SEL, CCMU_UART1_FCLK_SRC_SEL_Msk, CCMU_UART1_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART1_GetClockSource(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_UART1_FCLK_SRC_SEL, CCMU_UART1_FCLK_SRC_SEL_Msk, CCMU_UART1_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART1_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_UART1_FCLK_SRC_SEL, CCMU_UART1_FCLK_POSTDIV_Msk, CCMU_UART1_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART1_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_UART1_FCLK_SRC_SEL, CCMU_UART1_FCLK_POSTDIV_Msk, CCMU_UART1_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART1_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, UART1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART1_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, UART1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART1_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB0_FCLK_EN, UART1_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART1_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, UART1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, UART1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART1_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, UART1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, UART1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART1_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, UART1_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, UART1_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup UART2
  * @{
  */
/*
 * clock source:
 * apb1, msi, dcxo, pll_clkdev, pll_clkdev1
 */
__HW_STATIC_INLINE
void LL_RCC_UART2_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_UART2_FCLK_SRC_SEL, CCMU_UART2_FCLK_SRC_SEL_Msk, CCMU_UART2_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART2_GetClockSource(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_UART2_FCLK_SRC_SEL, CCMU_UART2_FCLK_SRC_SEL_Msk, CCMU_UART2_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART2_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_UART2_FCLK_SRC_SEL, CCMU_UART2_FCLK_POSTDIV_Msk, CCMU_UART2_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART2_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_UART2_FCLK_SRC_SEL, CCMU_UART2_FCLK_POSTDIV_Msk, CCMU_UART2_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART2_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, UART2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART2_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, UART2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_UART2_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, UART2_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_UART2_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, UART2_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, UART2_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART2_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, UART2_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, UART2_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_UART2_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, UART2_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, UART2_RESETN_Msk);
}
/**
  * @}
  */


/** @defgroup I2C0
  * @{
  */
/*
 * clock source:
 * apb0, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_I2C0_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_I2C0_FCLK_SRC_SEL, CCMU_I2C0_FCLK_SRC_SEL_Msk, CCMU_I2C0_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_I2C0_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU1_DEV->CCMU_I2C0_FCLK_SRC_SEL, CCMU_I2C0_FCLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C0_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_I2C0_FCLK_SRC_SEL, CCMU_I2C0_FCLK_POSTDIV_Msk, CCMU_I2C0_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_I2C0_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_I2C0_FCLK_SRC_SEL, CCMU_I2C0_FCLK_POSTDIV_Msk, CCMU_I2C0_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_I2C0_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, I2C0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C0_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, I2C0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_I2C0_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB0_FCLK_EN, I2C0_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_I2C0_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, I2C0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, I2C0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C0_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, I2C0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, I2C0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C0_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, I2C0_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, I2C0_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup I2C0
  * @{
  */
/*
 * clock source:
 * apb0, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_I2C1_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_I2C1_FCLK_SRC_SEL, CCMU_I2C1_FCLK_SRC_SEL_Msk, CCMU_I2C1_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_I2C1_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU1_DEV->CCMU_I2C1_FCLK_SRC_SEL, CCMU_I2C1_FCLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C1_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_I2C1_FCLK_SRC_SEL, CCMU_I2C1_FCLK_POSTDIV_Msk, CCMU_I2C1_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_I2C1_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_I2C1_FCLK_SRC_SEL, CCMU_I2C1_FCLK_POSTDIV_Msk, CCMU_I2C1_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_I2C1_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, I2C1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C1_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, I2C1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_I2C1_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB0_FCLK_EN, I2C1_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_I2C1_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, I2C1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, I2C1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C1_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, I2C1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, I2C1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_I2C1_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, I2C1_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, I2C1_RESETN_Msk);
}
/**
  * @}
  */


/** @defgroup SPI
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev1
 */
__HW_STATIC_INLINE
void LL_RCC_SPI_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_QSPI0_FCLK_SRC_SEL, CCMU_QSPI0_FCLK_SRC_SEL_Msk, CCMU_QSPI0_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SPI_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU1_DEV->CCMU_QSPI0_FCLK_SRC_SEL, CCMU_QSPI0_FCLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SPI_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_QSPI0_FCLK_SRC_SEL, CCMU_QSPI0_FCLK_POSTDIV_Msk, CCMU_QSPI0_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SPI_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_QSPI0_FCLK_SRC_SEL, CCMU_QSPI0_FCLK_POSTDIV_Msk, CCMU_QSPI0_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_SPI_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_AHB_FCLK_EN, QSPI0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SPI_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_AHB_FCLK_EN, QSPI0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SPI_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_AHB_FCLK_EN, QSPI0_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_SPI_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLKEN_SEL, QSPI0_HCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, QSPI0_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SPI_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_HCLKEN_SEL, QSPI0_HCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, QSPI0_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SPI_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_RESETN_EN, QSPI0_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_RESETN_EN, QSPI0_RESETN_Msk);
}
/**
  * @}
  */


/** @defgroup FSMC
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_FSMC_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_FCTRL_FCLK_SRC_SEL, CCMU_FCTRL_FCLK_SRC_SEL_Msk, CCMU_FCTRL_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_FSMC_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU1_DEV->CCMU_FCTRL_FCLK_SRC_SEL, CCMU_FCTRL_FCLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_FSMC_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_FCTRL_FCLK_SRC_SEL, CCMU_FCTRL_FCLK_POSTDIV_Msk, CCMU_FCTRL_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_FSMC_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_FCTRL_FCLK_SRC_SEL, CCMU_FCTRL_FCLK_POSTDIV_Msk, CCMU_FCTRL_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_FSMC_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_AHB_FCLK_EN, FLASHCTRL_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_FSMC_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_AHB_FCLK_EN, FLASHCTRL_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_FSMC_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_AHB_FCLK_EN, FLASHCTRL_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_FSMC_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLKEN_SEL, FLASHCTRL_HCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, FLASHCTRL_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_FSMC_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_HCLKEN_SEL, FLASHCTRL_HCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, FLASHCTRL_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_FSMC_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_RESETN_EN, FLASHCTRL_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_RESETN_EN, FLASHCTRL_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup PWM
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_PWM_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU2_DEV->PWM_CTRL, PWM_CLK_SRC_SEL_Msk, PWM_CLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PWM_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU2_DEV->PWM_CTRL, PWM_CLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PWM_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU2_DEV->PWM_CTRL, PWM_CLK_POSTDIV_Msk, PWM_CLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PWM_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->PWM_CTRL, PWM_CLK_POSTDIV_Msk, PWM_CLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_PWM_EnableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->PWM_CTRL, PWM_CLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PWM_DisableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->PWM_CTRL, PWM_CLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PWM_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU2_DEV->PWM_CTRL, PWM_CLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_PWM_EnableBusClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_PWM_DisableBusClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_PWM_Reset(void)
{
	HW_CLR_MSK(CCMU2_DEV->PWM_CTRL, PWM_RESETN_EN_Msk);
	HW_SET_MSK(CCMU2_DEV->PWM_CTRL, PWM_RESETN_EN_Msk);
}
/**
  * @}
  */


/** @defgroup GPADC
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_GPADC_SetClockSource(uint32_t source)
{
  /* define in gpadc module register */
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_FCLK_SRC_SEL_Msk, GPADC_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_GPADC_GetClockSource(void)
{
  /* define in gpadc module register */
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_FCLK_SRC_SEL_Msk, GPADC_FCLK_SRC_SEL_Pos);
    return  0;
}

__HW_STATIC_INLINE
void LL_RCC_GPADC_SetPrescaler(uint32_t prescaler)
{
  /* define in gpadc module register */
    HW_SET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_FCLK_POSTDIV_Msk, GPADC_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_GPADC_GetPrescaler(void)
{
  /* define in gpadc module register */
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_FCLK_POSTDIV_Msk, GPADC_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_GPADC_EnableClock(void)
{
  /* define in gpadc module register */
    HW_SET_BIT(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_GPADC_DisableClock(void)
{
  /* define in gpadc module register */
    HW_CLR_BIT(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_GPADC_IsEnabledClock(void)
{
  /* define in gpadc module register */
    return HW_GET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_FCLK_EN_Msk, GPADC_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_GPADC_EnableBusClock(void)
{
    /* all rtc pclk share */
    /* gpadc, rtc, rtt */
    HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, RTC_SW_PCLKEN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, RTC_PCLKEN_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_GPADC_DisableBusClock(void)
{
    /* all rtc pclk share */
    /* gpadc, rtc, rtt */
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, RTC_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, RTC_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_GPADC_Reset(void)
{
  /* define in gpadc module register */
	HW_SET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_RESETN_Msk, GPADC_RESETN_Pos, 0);
	HW_SET_MSK_VAL(GPADC_DEV->GPADC_CCMU_CTRL, GPADC_RESETN_Msk, GPADC_RESETN_Pos, 1);
}
/**
  * @}
  */


/** @defgroup MCAN
  * @{
  */
/*
 * clock source:
 * pllsoc, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_MCAN_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_MCAN_FCLK_SRC_SEL, CCMU_MCAN2_FCLK_SRC_SEL_Msk, CCMU_MCAN2_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MCAN_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU1_DEV->CCMU_MCAN_FCLK_SRC_SEL, CCMU_MCAN2_FCLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MCAN_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_MCAN_FCLK_SRC_SEL, CCMU_MCAN2_FCLK_POSTDIV_Msk, CCMU_MCAN2_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MCAN_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_MCAN_FCLK_SRC_SEL, CCMU_MCAN2_FCLK_POSTDIV_Msk, CCMU_MCAN2_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_MCAN_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, MCAN2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MCAN_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_FCLK_EN, MCAN2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MCAN_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB0_FCLK_EN, MCAN2_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_MCAN_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, MCAN2_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MCAN_DisableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, MCAN2_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MCAN_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_RESETN_EN, MCAN2_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_RESETN_EN, MCAN2_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup CAN
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_CAN_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_FCLK_SRC_SEL_Msk, CAN_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CAN_GetClockSource(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_FCLK_SRC_SEL_Msk, CAN_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_CAN_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_FCLK_POSTDIV_Msk, CAN_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CAN_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_FCLK_POSTDIV_Msk, CAN_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_CAN_EnableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->CCMU2_CAN_CTRL, (CAN_FCLK_EN_Msk | CAN_TMR_FCLK_EN_Msk));
}

__HW_STATIC_INLINE
void LL_RCC_CAN_DisableClock(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_CAN_CTRL, (CAN_FCLK_EN_Msk | CAN_TMR_FCLK_EN_Msk));
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CAN_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_CAN_EnableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_CAN_DisableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_CAN_Reset(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_RESETN_Msk);
	HW_SET_MSK(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_RESETN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_CAN_ModeSet(uint32_t mode)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_CAN_CTRL, SYSCFG_CAN_FD_EN_Msk, SYSCFG_CAN_FD_EN_Pos, mode);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CAN_ModeGet(void)
{
    return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_CAN_CTRL, SYSCFG_CAN_FD_EN_Msk, SYSCFG_CAN_FD_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_CAN_IPSet(uint32_t ip)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_CAN_CTRL, CAN_IP_SEL_Msk, CAN_IP_SEL_Pos, ip);
}

/**
  * @}
  */


/** @defgroup CDAA
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_CDAA_SetClockSource(uint32_t source)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CDAA_GetClockSource(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_CDAA_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_CDAA_FCLK_DIV, CCMU_CDAA_FCLK_POSTDIV_Msk, CCMU_CDAA_FCLK_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CDAA_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_CDAA_FCLK_DIV, CCMU_CDAA_FCLK_POSTDIV_Msk, CCMU_CDAA_FCLK_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_CDAA_EnableClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_CDAA_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_CDAA_IsEnabledClock(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_CDAA_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, CDAA_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, CDAA_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_CDAA_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, CDAA_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, CDAA_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_CDAA_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, CDAA_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, CDAA_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup TMR01
  * @{
  */
/*
 * clock source:
 * apb1, msi, dcxo, lsi
 */
__HW_STATIC_INLINE
void LL_RCC_TMR0_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_1_FCLK_SRC_SEL_Msk, CCMU_TMR0_1_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR0_GetClockSource(void)
{
	return	HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_1_FCLK_SRC_SEL_Msk, CCMU_TMR0_1_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR0_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_1_POSTDIV_Msk, CCMU_TMR0_1_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR0_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_1_POSTDIV_Msk, CCMU_TMR0_1_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR0_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR0_1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR0_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR0_1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR0_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR0_1_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR0_EnableBusClock(void)
{
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR0_PCLKEN_SEL_Msk);
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR0_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR0_Reset(void)
{
  /* include tmr01, tmr02 */
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR0_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR0_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup TMR02
  * @{
  */
/*
 * clock source:
 * apb1, msi, dcxo, lsi
 */
__HW_STATIC_INLINE
void LL_RCC_TMR1_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_2_FCLK_SRC_SEL_Msk, CCMU_TMR0_1_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR1_GetClockSource(void)
{
	return	HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_2_FCLK_SRC_SEL_Msk, CCMU_TMR0_1_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR1_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_2_POSTDIV_Msk, CCMU_TMR0_2_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR1_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR0_FCLK_SRC_SEL, CCMU_TMR0_2_POSTDIV_Msk, CCMU_TMR0_2_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR1_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR0_2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR1_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR0_2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR1_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR0_2_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR1_EnableBusClock(void)
{
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR0_PCLKEN_SEL_Msk);
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR1_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR1_Reset(void)
{
  /* include tmr01, tmr02 */
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR0_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR0_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup TMR11
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_TMR2_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_1_FCLK_SRC_SEL_Msk, CCMU_TMR1_1_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR2_GetClockSource(void)
{
	return	HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_1_FCLK_SRC_SEL_Msk, CCMU_TMR1_1_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR2_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_1_POSTDIV_Msk, CCMU_TMR1_1_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR2_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_1_POSTDIV_Msk, CCMU_TMR1_1_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR2_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR1_1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR2_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR1_1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR2_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR1_1_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR2_EnableBusClock(void)
{
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR1_PCLKEN_SEL_Msk);
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR2_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR2_Reset(void)
{
  /* include tmr11, tmr12 */
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR1_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR1_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup TMR12
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_TMR3_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_2_FCLK_SRC_SEL_Msk, CCMU_TMR1_2_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR3_GetClockSource(void)
{
	return	HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_2_FCLK_SRC_SEL_Msk, CCMU_TMR1_2_FCLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR3_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_2_POSTDIV_Msk, CCMU_TMR1_2_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR3_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_TMR1_FCLK_SRC_SEL, CCMU_TMR1_2_POSTDIV_Msk, CCMU_TMR1_2_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR3_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR1_2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR3_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR1_2_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_TMR3_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, TMR1_2_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_TMR3_EnableBusClock(void)
{
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR1_PCLKEN_SEL_Msk);
  HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR3_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, TMR1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, TMR1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TMR3_Reset(void)
{
  /* include tmr11, tmr12 */
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR1_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, TMR1_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup WDG0
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_WDG0_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_WDG0_FCLK_SRC_SEL, CCMU_WDG0_FCLK_SRC_SEL_Msk, CCMU_WDG0_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_WDG0_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU1_DEV->CCMU_WDG0_FCLK_SRC_SEL, CCMU_WDG0_FCLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG0_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_WDG0_FCLK_SRC_SEL, CCMU_WDG0_POSTDIV_Msk, CCMU_WDG0_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_WDG0_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_WDG0_FCLK_SRC_SEL, CCMU_WDG0_POSTDIV_Msk, CCMU_WDG0_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_WDG0_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, WDG0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG0_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, WDG0_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_WDG0_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, WDG0_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_WDG0_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, WDG0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, WDG0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG0_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, WDG0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, WDG0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG0_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, WDG0_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, WDG0_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup WDG1
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_WDG1_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_WDG1_FCLK_SRC_SEL, CCMU_WDG1_FCLK_SRC_SEL_Msk, CCMU_WDG1_FCLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_WDG1_GetClockSource(void)
{
	return	HW_GET_MSK(CCMU1_DEV->CCMU_WDG1_FCLK_SRC_SEL, CCMU_WDG1_FCLK_SRC_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG1_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU1_DEV->CCMU_WDG1_FCLK_SRC_SEL, CCMU_WDG1_POSTDIV_Msk, CCMU_WDG1_POSTDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_WDG1_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU1_DEV->CCMU_WDG1_FCLK_SRC_SEL, CCMU_WDG1_POSTDIV_Msk, CCMU_WDG1_POSTDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_WDG1_EnableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, WDG1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG1_DisableClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_FCLK_EN, WDG1_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_WDG1_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU1_DEV->CCMU_APB1_FCLK_EN, WDG1_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_WDG1_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, WDG1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, WDG1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG1_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, WDG1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, WDG1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_WDG1_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, WDG1_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, WDG1_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup RTTRTC
  * @{
  */
/*
 * clock source:
 * xo32k, lp_div, lp_frac
 */
__HW_STATIC_INLINE
void LL_RCC_RTC_SetClockCnt(uint32_t clk_cnt)
{
	HW_SET_MSK_VAL(MISCREG1_DEV->MISCREG_RTC_CLK_CNT, RTC_CLK_CNT_Msk, RTC_CLK_CNT_Pos, clk_cnt);
}
__HW_STATIC_INLINE
uint32_t LL_RCC_RTC_GetClockCnt(void)
{
	return HW_GET_MSK_VAL(MISCREG1_DEV->MISCREG_RTC_CLK_CNT, RTC_CLK_CNT_Msk, RTC_CLK_CNT_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTC_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_CLK_SRC_SEL_Msk, RTTRTC_CLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTC_GetClockSource(void)
{
	return	HW_GET_MSK_VAL(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_CLK_SRC_SEL_Msk, RTTRTC_CLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTC_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */
	HW_SET_MSK_VAL(RTTRTC_DEV->RTT_MR, RTT_MR_PREDIV_Msk, RTT_MR_PREDIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTC_GetPrescaler(void)
{
  /* No processing required */
	return HW_GET_MSK_VAL(RTTRTC_DEV->RTT_MR, RTT_MR_PREDIV_Msk, RTT_MR_PREDIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTC_EnableClock(void)
{
	HW_SET_MSK_VAL(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_CLK_EN_Msk, RTTRTC_CLK_EN_Pos, 1);
}

__HW_STATIC_INLINE
void LL_RCC_RTC_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTC_IsEnabledClock(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_RTC_EnableBusClock(void)
{
    /* all rtc pclk share */
    /* gpadc, rtc, rtt */
    HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, RTC_SW_PCLKEN_Msk);
    HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, RTC_PCLKEN_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_RTC_DisableBusClock(void)
{
    /* all rtc pclk share */
    /* gpadc, rtc, rtt */
    HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, RTC_PCLKEN_SEL_Msk);
    HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, RTC_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_RTC_Reset(void)
{
	HW_CLR_BIT(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_RESETN_EN_Pos);
	HW_SET_BIT(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_RESETN_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTT_RTC_Resetn_Release(void)
{
	HW_SET_BIT(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_RESETN_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTT_RTC_Resetn_Enable(void)
{
	HW_CLR_BIT(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_RESETN_EN_Pos);
}

/**
  * @}
  */



/** @defgroup RTT
  * @{
  */
/*
 * clock source:
 * xo32k, lp_div, lp_frac
 */
__HW_STATIC_INLINE
void LL_RCC_RTT_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_CLK_SRC_SEL_Msk, RTTRTC_CLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTT_GetClockSource(void)
{
	return	HW_GET_MSK_VAL(MISCREG1_DEV->RTTRTC_CTRL, RTTRTC_CLK_SRC_SEL_Msk, RTTRTC_CLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTT_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */

}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTT_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_RTT_EnableClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_RTT_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTT_IsEnabledClock(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_RTT_EnableBusClock(void)
{
    /* all rtc pclk share */
    /* gpadc, rtc, rtt */
    HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, RTC_SW_PCLKEN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, RTC_PCLKEN_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_RTT_DisableBusClock(void)
{
    /* all rtc pclk share */
    /* gpadc, rtc, rtt */
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, RTC_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, RTC_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_RTT_Reset(void)
{


}
/**
  * @}
  */


/** @defgroup RTICK
  * @{
  */
/*
 * clock source:
 * xo32k, lp_div, lp_frac
 */
__HW_STATIC_INLINE
void LL_RCC_RTICK_SetClockSource(uint32_t source)
{
  	HW_SET_MSK_VAL(PMU_DEV->PMU_TMR_WUP_CTRL0, TMR_WUP_CLK_SRC_SEL_Msk, TMR_WUP_CLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTICK_GetClockSource(void)
{
	return	HW_GET_MSK_VAL(PMU_DEV->PMU_TMR_WUP_CTRL0, TMR_WUP_CLK_SRC_SEL_Msk, TMR_WUP_CLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTICK_SetPrescaler(uint32_t prescaler)
{
 	/* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTICK_GetPrescaler(void)
{
  	/* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_RTICK_EnableClock(void)
{
  	HW_SET_MSK(PMU_DEV->PMU_TMR_WUP_CTRL0, TMR_WUP_CLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_RTICK_DisableClock(void)
{
  	HW_CLR_MSK(PMU_DEV->PMU_TMR_WUP_CTRL0, TMR_WUP_CLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_RTICK_IsEnabledClock(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_TMR_WUP_CTRL0, TMR_WUP_CLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_RTICK_EnableBusClock(void)
{
  	/* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_RTICK_DisableBusClock(void)
{
  	/* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_RTICK_Reset(void)
{
  	HW_CLR_MSK(PMU_DEV->PMU_TMR_WUP_CTRL0, TMR_WUP_RESETN_EN_Msk);
  	HW_SET_MSK(PMU_DEV->PMU_TMR_WUP_CTRL0, TMR_WUP_RESETN_EN_Msk);
}
/**
  * @}
  */



/** @defgroup DMA
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_DMA_SetClockSource(uint32_t source)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_DMA_GetClockSource(void)
{
  /* No processing required */
	return  0;
}

__HW_STATIC_INLINE
void LL_RCC_DMA_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_DMA_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_DMA_EnableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_DMA_DisableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_DMA_IsEnabledClock(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_DMA_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, DMAC_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_DMA_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, DMAC_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_DMA_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_RESETN_EN, DMAC_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_RESETN_EN, DMAC_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup MBX0
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_MBX0_SetClockSource(uint32_t source)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MBX0_GetClockSource(void)
{
  /* No processing required */
	return  0;
}

__HW_STATIC_INLINE
void LL_RCC_MBX0_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MBX0_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_MBX0_EnableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_MBX0_DisableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MBX0_IsEnabledClock(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_MBX0_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, MAILBOX0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, MAILBOX0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MBX0_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, MAILBOX0_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, MAILBOX0_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MBX0_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, MAILBOX0_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, MAILBOX0_RESETN_Msk);
}
/**
  * @}
  */



/** @defgroup MBX1
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_MBX1_SetClockSource(uint32_t source)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MBX1_GetClockSource(void)
{
  /* No processing required */
	return  0;
}

__HW_STATIC_INLINE
void LL_RCC_MBX1_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MBX1_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_MBX1_EnableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_MBX1_DisableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MBX1_IsEnabledClock(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_MBX1_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, MAILBOX1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, MAILBOX1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MBX1_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_PCLKEN_SEL, MAILBOX1_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, MAILBOX1_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MBX1_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, MAILBOX1_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, MAILBOX1_RESETN_Msk);
}
/**
  * @}
  */


/** @defgroup SPLK
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_SPLK_SetClockSource(uint32_t source)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SPLK_GetClockSource(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_SPLK_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SPLK_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_SPLK_EnableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_SPLK_DisableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SPLK_IsEnabledClock(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_SPLK_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, SPINLOCK_PCLKEN_SEL_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, SPINLOCK_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SPLK_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_PCLKEN_SEL, SPINLOCK_PCLKEN_SEL_Msk);
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_SW_PCLKEN, SPINLOCK_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SPLK_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, SPINLOCK_PRESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, SPINLOCK_PRESETN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SPLK_Release(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, SPINLOCK_PRESETN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SPLK_IsRelease(void)
{
	return HW_GET_MSK(CCMU1_DEV->CCMU_APB1_RESETN_EN, SPINLOCK_PRESETN_Msk);
}

/**
  * @}
  */


/** @defgroup EFUSE
  * @{
  */
/*
 * clock source:
 * 00(MSI)\01(ATE_EFUSE_CLK)\10(LSI)\11(HSE)
 */
__HW_STATIC_INLINE
void LL_RCC_EFUSE_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_EFUSEC_CTRL, EFUSE_CLK_SRC_SEL_Msk, EFUSE_CLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_EFUSE_GetClockSource(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_EFUSEC_CTRL, EFUSE_CLK_SRC_SEL_Msk, EFUSE_CLK_SRC_SEL_Pos);
}

/*
 * clock div:
 * 00(1)\01(2)\2(4)\3(8)\...\7(16)
 */
__HW_STATIC_INLINE
void LL_RCC_EFUSE_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_EFUSEC_CTRL, EFUSE_CLK_DIV_Msk, EFUSE_CLK_DIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_EFUSE_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_EFUSEC_CTRL, EFUSE_CLK_DIV_Msk, EFUSE_CLK_DIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_EFUSE_EnableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->CCMU2_CLK_EN, SYSCFG_EFUSECTRL_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_EFUSE_DisableClock(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_CLK_EN, SYSCFG_EFUSECTRL_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_EFUSE_IsEnabledClock(void)
{
	return HW_GET_MSK(CCMU2_DEV->CCMU2_CLK_EN, SYSCFG_EFUSECTRL_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_EFUSE_EnableBusClock(void)
{
	/* no need , default enable pclock*/
}

__HW_STATIC_INLINE
void LL_RCC_EFUSE_DisableBusClock(void)
{
	/* no need , default enable pclock*/
}

__HW_STATIC_INLINE
void LL_RCC_EFUSE_Reset(void)
{
	/* no need , without reset*/
}
/**
  * @}
  */


/** @defgroup RCCAL
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_RCCAL_SetClockSource(uint32_t source)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_RCCAL_GetClockSource(void)
{
	return  0;
}

__HW_STATIC_INLINE
void LL_RCC_RCCAL_SetPrescaler(uint32_t prescaler)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_RCCAL_GetPrescaler(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_RCCAL_EnableClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_RCCAL_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_RCCAL_IsEnabledClock(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_RCCAL_EnableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_RCCAL_DisableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_RCCAL_Reset(void)
{

}
/**
  * @}
  */



/** @defgroup PKA
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_PKA_SetClockSource(uint32_t source)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PKA_GetClockSource(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_PKA_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PKA_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_PKA_EnableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_PKA_DisableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_PKA_IsEnabledClock(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_PKA_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, PKA_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PKA_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_SW_PCLKEN, PKA_SW_PCLKEN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_PKA_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, PKA_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_APB0_RESETN_EN, PKA_RESETN_Msk);
}
/**
  * @}
  */




/** @defgroup CRYPT
  * @{
  */
__HW_STATIC_INLINE
void LL_RCC_CRYPT_SetClockSource(uint32_t source)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CRYPT_GetClockSource(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_CRYPT_SetPrescaler(uint32_t prescaler)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CRYPT_GetPrescaler(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_CRYPT_EnableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_CRYPT_DisableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_CRYPT_IsEnabledClock(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_CRYPT_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, ENCRYPT_SS_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_CRYPT_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, ENCRYPT_SS_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_CRYPT_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_RESETN_EN, SS_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_RESETN_EN, SS_RESETN_Msk);
}

/**
  * @}
  */


/** @defgroup HICM
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_HICM_SetClockSource(uint32_t source)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_HICM_GetClockSource(void)
{
	return  0;
}

__HW_STATIC_INLINE
void LL_RCC_HICM_SetPrescaler(uint32_t prescaler)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_HICM_GetPrescaler(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_HICM_EnableClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_HICM_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_HICM_IsEnabledClock(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_HICM_EnableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_HICM_DisableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_HICM_Reset(void)
{

}
/**
  * @}
  */



/** @defgroup AES
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_AES_SetClockSource(uint32_t source)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_AES_GetClockSource(void)
{
	return  0;
}

__HW_STATIC_INLINE
void LL_RCC_AES_SetPrescaler(uint32_t prescaler)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_AES_GetPrescaler(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_AES_EnableClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_AES_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_AES_IsEnabledClock(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_AES_EnableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_AES_DisableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_AES_Reset(void)
{

}
/**
  * @}
  */



/** @defgroup AES
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_TRNG_SetClockSource(uint32_t source)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_TRNG_GetClockSource(void)
{
	return  0;
}

__HW_STATIC_INLINE
void LL_RCC_TRNG_SetPrescaler(uint32_t prescaler)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_TRNG_GetPrescaler(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_TRNG_EnableClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_TRNG_DisableClock(void)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_TRNG_IsEnabledClock(void)
{
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_TRNG_EnableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_TRNG_DisableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_TRNG_Reset(void)
{

}
/**
  * @}
  */


/** @defgroup MDSP
  * @{
  */
/*
 * clock source:
 * sysclk, msi, dcxo, pll_clkdev
 */
__HW_STATIC_INLINE
void LL_RCC_MDSP_SetClockSource(uint32_t source)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_MDSP_GetClockSource(void)
{

	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_MDSP_SetPrescaler(uint32_t prescaler)
{

}

__HW_STATIC_INLINE
uint32_t LL_RCC_MDSP_GetPrescaler(void)
{

	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_MDSP_EnableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
void LL_RCC_MDSP_DisableClock(void)
{
  /* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MDSP_IsEnabledClock(void)
{
  /* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_MDSP_EnableBusClock(void)
{
	HW_SET_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, MINIDSP_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MDSP_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_HCLK_SW_EN, MINIDSP_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MDSP_Reset(void)
{
	HW_CLR_MSK(CCMU1_DEV->CCMU_RESETN_EN, MINIDSP_RESETN_Msk);
	HW_SET_MSK(CCMU1_DEV->CCMU_RESETN_EN, MINIDSP_RESETN_Msk);
}
/**
  * @}
  */


/** @defgroup SRAMB
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_SRAMB_SetClockSource(uint32_t source)
{
	/* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SRAMB_GetClockSource(void)
{
	/* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_SRAMB_SetPrescaler(uint32_t prescaler)
{
	/* No processing required */
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SRAMB_GetPrescaler(void)
{
	/* No processing required */
	return 0;
}

__HW_STATIC_INLINE
void LL_RCC_SRAMB_EnableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->CCMU2_RESETN_EN, SYSCFG_DATACUBE_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SRAMB_DisableClock(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_RESETN_EN, SYSCFG_DATACUBE_FCLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_SRAMB_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU2_DEV->CCMU2_RESETN_EN, SYSCFG_DATACUBE_FCLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_SRAMB_EnableBusClock(void)
{
	HW_SET_MSK(CCMU2_DEV->CCMU2_RESETN_EN, SYSCFG_DATACUBE_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SRAMB_DisableBusClock(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_RESETN_EN, SYSCFG_DATACUBE_HCLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_SRAMB_Reset(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_RESETN_EN, SYSCFG_DATACUBE_RESETN_EN_Msk | DATACUBE_HRESETN_EN_Msk);
	HW_SET_MSK(CCMU2_DEV->CCMU2_RESETN_EN, SYSCFG_DATACUBE_RESETN_EN_Msk | DATACUBE_HRESETN_EN_Msk);
}
/**
  * @}
  */


/** @defgroup MCO
  * @{
  */
/*
 * clock source:
 *
 */
__HW_STATIC_INLINE
void LL_RCC_MCO_SetClockSource(uint32_t source)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_MCO_CLK_CTRL, MCO_CLK_SRC_SEL_Msk, MCO_CLK_SRC_SEL_Pos, source);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MCO_GetClockSource(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_MCO_CLK_CTRL, MCO_CLK_SRC_SEL_Msk, MCO_CLK_SRC_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_MCO_SetPrescaler(uint32_t prescaler)
{
	HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_MCO_CLK_CTRL, MCO_CLK_DIV_Msk, MCO_CLK_DIV_Pos, prescaler);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MCO_GetPrescaler(void)
{
	return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_MCO_CLK_CTRL, MCO_CLK_DIV_Msk, MCO_CLK_DIV_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_MCO_EnableClock(void)
{
	HW_SET_MSK(CCMU2_DEV->CCMU2_MCO_CLK_CTRL, MCO_CLK_EN_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_MCO_DisableClock(void)
{
	HW_CLR_MSK(CCMU2_DEV->CCMU2_MCO_CLK_CTRL, MCO_CLK_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_RCC_MCO_IsEnabledClock(void)
{
	return HW_TEST_BIT(CCMU2_DEV->CCMU2_MCO_CLK_CTRL, MCO_CLK_EN_Pos);
}

__HW_STATIC_INLINE
void LL_RCC_MCO_EnableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_MCO_DisableBusClock(void)
{

}

__HW_STATIC_INLINE
void LL_RCC_MCO_Reset(void)
{

}
/**
  * @}
  */


__HW_STATIC_INLINE
void LL_RCC_TICK_Reset(void)
{
    HW_SET_MSK(CCMU2_DEV->CCMU2_FREECNT_CTRL, FREECNT_CLR_Msk);
    HW_CLR_MSK(CCMU2_DEV->CCMU2_FREECNT_CTRL, FREECNT_CLR_Msk);
}

__HW_STATIC_INLINE
void LL_RCC_TICK_SetPrescaler(uint32_t prescaler)
{
    HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_FREECNT_CTRL, FREECNT_CLK_DIV_Msk, FREECNT_CLK_DIV_Pos, prescaler);
}


/** PSIC_LL_Driver
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_RCC_DEV_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
