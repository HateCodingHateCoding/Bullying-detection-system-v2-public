/**
 **************************************************************************************************
 * @file    psic_ll_gpio.h
 * @brief   general purpose I/O hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_GPIO_H
#define _PSIC_LL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "soc.h"
#include "types.h"
#include "hw_gpio.h"


#define GPIOA_DEV                       ((gpio_dev_t *)   (GPIO_BASE_ADDR + 0X00))  /*!< 0x4001 D000 */
#define GPIOB_DEV                       ((gpio_dev_t *)   (GPIO_BASE_ADDR + 0x80))  /*!< 0x4001 D000 */

/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup GPIO_LL GPIO
  * @{
  */

#define PSIC_GPIO_DBUG_REG    1
#if PSIC_GPIO_DBUG_REG
#define PSIC_GPIO_LOG_REG(reg) printk("["#reg"]""0x%08x.\n", (uint32_t)reg)
#define PSIC_GPIO_LOG_ALL(gpio, msg) {                          \
        printk("%s\n", msg);                                     \
        printk("base[0x%08x]\n", (uint32_t)gpio);                \
        PSIC_GPIO_LOG_REG(gpio->FUNC[0]);                       \
        PSIC_GPIO_LOG_REG(gpio->FUNC[1]);                       \
        PSIC_GPIO_LOG_REG(gpio->FUNC[2]);                       \
        PSIC_GPIO_LOG_REG(gpio->FUNC[3]);                       \
        PSIC_GPIO_LOG_REG(gpio->PULLUPDWN_SET[0]);              \
        PSIC_GPIO_LOG_REG(gpio->PULLUPDWN_SET[1]);              \
        PSIC_GPIO_LOG_REG(gpio->OUTPUT_TYPE);                   \
        PSIC_GPIO_LOG_REG(gpio->DRIVING[0]);                    \
        PSIC_GPIO_LOG_REG(gpio->DRIVING[1]);                    \
        PSIC_GPIO_LOG_REG(gpio->OUTPUT);                        \
        PSIC_GPIO_LOG_REG(gpio->INPUT);                         \
        PSIC_GPIO_LOG_REG(gpio->INT_EN);                        \
        PSIC_GPIO_LOG_REG(gpio->INT_TYPE[0]);                   \
        PSIC_GPIO_LOG_REG(gpio->INT_TYPE[1]);                   \
        PSIC_GPIO_LOG_REG(gpio->INT_TYPE[2]);                   \
        PSIC_GPIO_LOG_REG(gpio->INT_TYPE[3]);                   \
        PSIC_GPIO_LOG_REG(gpio->INT_CLR);                       \
        PSIC_GPIO_LOG_REG(gpio->INT_FLAG);                      \
        PSIC_GPIO_LOG_REG(gpio->DBC);                           \
        PSIC_GPIO_LOG_REG(gpio->I2C_MODE);                      \
}
#else
#define PSIC_GPIO_LOG_REG(reg)
#define PSIC_GPIO_LOG_ALL(qspi, msg)
#endif



/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup GPIO_LL_ES_INIT GPIO Exported Init structures
  * @{
  */

typedef void (*gpio_irq_cb_t) (uint32_t pin_msk, void *arg);


/**
  * @brief LL GPIO Init Structure definition
  */
typedef struct {
	uint32_t pin;          /*!< Specifies the GPIO pins to be configured.
                              This parameter can be any value of @ref GPIO_LL_PIN */

	uint32_t func_mode;   /*!< Specifies the operating mode for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_MODE.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinMode().*/

	uint32_t driv_level;  /*!< Specifies the speed for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_SPEED.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinSpeed().*/

	uint32_t output_type; /*!< Specifies the operating output type for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_OUTPUT.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinOutputType().*/

	uint32_t pull;         /*!< Specifies the operating Pull-up/Pull down for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_PULL.
                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinPull().*/

	uint32_t irq_type;

	uint32_t param;

	gpio_irq_cb_t irq_cb;
} ll_gpio_conf_t;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */


/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup GPIO_LL_Exported_Constants GPIO Exported Constants
  * @{
  */

/** @defgroup GPIO_LL_PIN PIN
  * @{
  */
#define LL_GPIO_PIN_0                               (0)
#define LL_GPIO_PIN_1                               (1)
#define LL_GPIO_PIN_2                               (2)
#define LL_GPIO_PIN_3                               (3)
#define LL_GPIO_PIN_4                               (4)
#define LL_GPIO_PIN_5                               (5)
#define LL_GPIO_PIN_6                               (6)
#define LL_GPIO_PIN_7                               (7)
#define LL_GPIO_PIN_8                               (8)
#define LL_GPIO_PIN_9                               (9)
#define LL_GPIO_PIN_10                              (10)
#define LL_GPIO_PIN_11                              (11)
#define LL_GPIO_PIN_12                              (12)
#define LL_GPIO_PIN_13                              (13)
#define LL_GPIO_PIN_14                              (14)
#define LL_GPIO_PIN_15                              (15)

#define LL_GPIO_PIN_MAX                             (16)

#define LL_GPIO_PORT_MAX                            (1)


#define LL_GPIO_PIN_MSK_0                           HW_BIT(0)
#define LL_GPIO_PIN_MSK_1                           HW_BIT(1)
#define LL_GPIO_PIN_MSK_2                           HW_BIT(2)
#define LL_GPIO_PIN_MSK_3                           HW_BIT(3)
#define LL_GPIO_PIN_MSK_4                           HW_BIT(4)
#define LL_GPIO_PIN_MSK_5                           HW_BIT(5)
#define LL_GPIO_PIN_MSK_6                           HW_BIT(6)
#define LL_GPIO_PIN_MSK_7                           HW_BIT(7)
#define LL_GPIO_PIN_MSK_8                           HW_BIT(8)
#define LL_GPIO_PIN_MSK_9                           HW_BIT(9)
#define LL_GPIO_PIN_MSK_10                          HW_BIT(10)
#define LL_GPIO_PIN_MSK_11                          HW_BIT(11)
#define LL_GPIO_PIN_MSK_12                          HW_BIT(12)
#define LL_GPIO_PIN_MSK_13                          HW_BIT(13)
#define LL_GPIO_PIN_MSK_14                          HW_BIT(14)
#define LL_GPIO_PIN_MSK_15                          HW_BIT(15)

#define LL_GPIO_PIN_MSK                             0x0000FFFF

#define LL_GPIO_PIN_TO_MSK(pin)                     HW_BIT(pin)
/**
  * @}
  */

/** @defgroup GPIO_LL_FUNCTION_MODE Mode
  * @{
  */
#define LL_GPIOx_Pn_F0_INPUT                0   /* for all pins */
#define LL_GPIOx_Pn_F1_OUTPUT               1   /* for all pins */
#define LL_GPIOx_Pn_F8_OUTPUT               8   /* for all pins */
#define LL_GPIOx_Pn_F14_EINT                14  /* for all pins */
#define LL_GPIOx_Pn_F15_DIS                 15  /* for all pins */

#define LL_GPIOA_P0_F2_SPI_CS               2
#define LL_GPIOA_P0_F3_UART1_TX             3
#define LL_GPIOA_P0_F4_CANFD_TX             4
#define LL_GPIOA_P0_F5_F_SWD_TMS            5
#define LL_GPIOA_P0_F6_PWM0                 6
#define LL_GPIOA_P0_F8_DBG0                 8
#define LL_GPIOA_P0_F9_MCO_CLK              9

#define LL_GPIOA_P1_F2_SPI_CLK              2
#define LL_GPIOA_P1_F3_UART1_RX             3
#define LL_GPIOA_P1_F4_CANFD_RX             4
#define LL_GPIOA_P1_F5_F_SWD_TCK            5
#define LL_GPIOA_P1_F6_PWM1                 6
#define LL_GPIOA_P1_F8_DBG1                 8

#define LL_GPIOA_P2_F2_SPI_MOSI             2
#define LL_GPIOA_P2_F3_UART1_RTS            3
#define LL_GPIOA_P2_F4_TWI0_SDA             4
#define LL_GPIOA_P2_F5_UART2_TX             5
#define LL_GPIOA_P2_F6_PWM2                 6
#define LL_GPIOA_P2_F8_DBG2                 8
#define LL_GPIOA_P2_F9_ADC_CLK              9

#define LL_GPIOA_P3_F2_SPI_MISO             2
#define LL_GPIOA_P3_F3_UART1_CTS            3
#define LL_GPIOA_P3_F4_TWI0_SCL             4
#define LL_GPIOA_P3_F5_UART2_RX             5
#define LL_GPIOA_P3_F6_PWM3                 6
#define LL_GPIOA_P3_F8_DBG3                 8
#define LL_GPIOA_P3_F9_ADC_VALID            9

#define LL_GPIOA_P4_F2_SPI_HOLD             2
#define LL_GPIOA_P4_F3_TWI0_SDA             3
#define LL_GPIOA_P4_F4_REQ_IRQ              4
#define LL_GPIOA_P4_F5_CANFD_TX             5
#define LL_GPIOA_P4_F6_PWM4                 6
#define LL_GPIOA_P4_F7_PSRAM_CS             7
#define LL_GPIOA_P4_F8_DBG4                 8
#define LL_GPIOA_P4_F9_ADC11                9

#define LL_GPIOA_P5_F2_SPI_WP               2
#define LL_GPIOA_P5_F3_TWI0_SCL             3
#define LL_GPIOA_P5_F4_PGOOD                4
#define LL_GPIOA_P5_F5_CANFD_RX             5
#define LL_GPIOA_P5_F6_PWM5                 6
#define LL_GPIOA_P5_F8_DBG5                 8
#define LL_GPIOA_P5_F9_ADC10                9

#define LL_GPIOA_P6_F2_UART1_RTS            2
#define LL_GPIOA_P6_F3_UART2_TX             3
#define LL_GPIOA_P6_F4_TWI1_SDA             4
#define LL_GPIOA_P6_F6_PWM6                 6
#define LL_GPIOA_P6_F6_GPADC6               7
#define LL_GPIOA_P6_F8_DBG6                 8
#define LL_GPIOA_P6_F9_ADC9                 9

#define LL_GPIOA_P7_F2_UART1_CTS            2
#define LL_GPIOA_P7_F3_UART2_RX             3
#define LL_GPIOA_P7_F4_TWI1_SCL             4
#define LL_GPIOA_P7_F5_PSRAM_CS             5
#define LL_GPIOA_P7_F6_PWM7                 6
#define LL_GPIOA_P7_F6_GPADC7               7
#define LL_GPIOA_P7_F8_DBG7                 8
#define LL_GPIOA_P7_F9_ADC8                 9

#define LL_GPIOA_P8_F2_UART1_TX             2
#define LL_GPIOA_P8_F3_TWI1_SCL             3
#define LL_GPIOA_P8_F5_REQ_IRQ              5
#define LL_GPIOA_P8_F6_PWM0                 6
#define LL_GPIOA_P8_F6_GPADC0               7
#define LL_GPIOA_P8_F8_DBG8                 8
#define LL_GPIOA_P8_F9_ADC7                 9

#define LL_GPIOA_P9_F2_UART1_RX             2
#define LL_GPIOA_P9_F3_TWI1_SDA             3
#define LL_GPIOA_P9_F5_PGOOD                5
#define LL_GPIOA_P9_F6_PWM1                 6
#define LL_GPIOA_P9_F6_GPADC1               7
#define LL_GPIOA_P9_F8_DBG9                 8
#define LL_GPIOA_P9_F9_ADC6                 9

#define LL_GPIOA_P10_F2_TWI1_SDA             2
#define LL_GPIOA_P10_F3_CANFD_TX             3
#define LL_GPIOA_P10_F4_S_SWD_TMS            4
#define LL_GPIOA_P10_F5_PGOOD                5
#define LL_GPIOA_P10_F6_PWM2                 6
#define LL_GPIOA_P10_F6_GPADC2               7
#define LL_GPIOA_P10_F8_DBG10                8
#define LL_GPIOA_P10_F9_ADC5                 9

#define LL_GPIOA_P11_F2_TWI1_SCL             2
#define LL_GPIOA_P11_F3_CANFD_RX             3
#define LL_GPIOA_P11_F4_S_SWD_TCK            4
#define LL_GPIOA_P11_F5_REQ_IRQ              5
#define LL_GPIOA_P11_F6_PWM3                 6
#define LL_GPIOA_P11_F6_GPADC3               7
#define LL_GPIOA_P11_F8_DBG11                8
#define LL_GPIOA_P11_F9_ADC4                 9

#define LL_GPIOA_P12_F2_UART2_RX             2
#define LL_GPIOA_P12_F3_UART1_RTS            3
#define LL_GPIOA_P12_F4_CANFD_TX             4
#define LL_GPIOA_P12_F5_PGOOD                5
#define LL_GPIOA_P12_F6_PWM4                 6
#define LL_GPIOA_P12_F6_GPADC4               7
#define LL_GPIOA_P12_F8_DBG12                8
#define LL_GPIOA_P12_F9_ADC3                 9

#define LL_GPIOA_P13_F2_UART2_TX             2
#define LL_GPIOA_P13_F3_MCO_CLK              3//????
#define LL_GPIOA_P13_F4_CANFD_RX             4
#define LL_GPIOA_P13_F5_REQ_IRQ              5
#define LL_GPIOA_P13_F6_PWM5                 6
#define LL_GPIOA_P13_F6_GPADC5               7
#define LL_GPIOA_P13_F8_DBG13                8
#define LL_GPIOA_P13_F9_ADC2                 9

#define LL_GPIOA_P14_F2_TWI0_SDA             2
#define LL_GPIOA_P14_F3_UART2_RX             3
#define LL_GPIOA_P14_F4_S_SWD_TMS            4
#define LL_GPIOA_P14_F5_CANFD_TX             5
#define LL_GPIOA_P14_F6_PWM6                 6
#define LL_GPIOA_P14_F6_GPADC6               7
#define LL_GPIOA_P14_F8_DBG14                8
#define LL_GPIOA_P14_F9_ADC1                 9

#define LL_GPIOA_P15_F2_TWI0_SCL             2
#define LL_GPIOA_P15_F3_UART2_TX             3
#define LL_GPIOA_P15_F4_S_SWD_TCK            4
#define LL_GPIOA_P15_F5_CANFD_RX             5
#define LL_GPIOA_P15_F6_PWM7                 6
#define LL_GPIOA_P15_F6_GPADC7               7
#define LL_GPIOA_P15_F8_DBG15                8
#define LL_GPIOA_P15_F9_ADC0                 9

#define LL_GPIOB_P0_F2_UART0_TX              2
#define LL_GPIOB_P0_F3_S_SWD_TMS             3
#define LL_GPIOB_P0_F5_F_SWD_TMS             5
#define LL_GPIOB_P0_F6_PWM0                  6
#define LL_GPIOB_P0_F8_S_TDI                 8
#define LL_GPIOB_P0_F9_F_TDI                 9

#define LL_GPIOB_P1_F2_UART0_RX              2
#define LL_GPIOB_P1_F3_S_SWD_TCK             3
#define LL_GPIOB_P1_F5_F_SWD_TCK             5
#define LL_GPIOB_P1_F6_PWM1                  6
#define LL_GPIOB_P1_F8_S_TDO                 8
#define LL_GPIOB_P1_F9_F_TDO                 9

#define LL_GPIOB_P2_F2_UART0_RTS             2
#define LL_GPIOB_P2_F3_FC_WP                 3
#define LL_GPIOB_P2_F4_SPI_WP                4
#define LL_GPIOB_P2_F5_ADC0                  5
#define LL_GPIOB_P2_F6_PWM2                  6
#define LL_GPIOB_P2_F8_S_TMS                 8
#define LL_GPIOB_P2_F9_F_TMS                 9

#define LL_GPIOB_P3_F2_UART0_CTS             2
#define LL_GPIOB_P3_F3_FC_HOLD               3
#define LL_GPIOB_P3_F4_SPI_HOLD              4
#define LL_GPIOB_P3_F5_ADC1                  5
#define LL_GPIOB_P3_F6_PWM3                  6
#define LL_GPIOB_P3_F8_S_TCK                 8
#define LL_GPIOB_P3_F9_F_TCK                 9

#define LL_GPIOB_P4_F2_FC_CS                 2
#define LL_GPIOB_P4_F3_UART2_RX              3
#define LL_GPIOB_P4_F4_SPI_CS                4
#define LL_GPIOB_P4_F5_CANFD_TX              5
#define LL_GPIOB_P4_F6_PWM4                  6
#define LL_GPIOB_P4_F7_PSRAM_CS              7
#define LL_GPIOB_P4_F8_UART1_RTS             8
#define LL_GPIOB_P4_F9_TWI1_SCL              9

#define LL_GPIOB_P5_F2_FC_CLK                2
#define LL_GPIOB_P5_F3_UART2_TX              3
#define LL_GPIOB_P5_F4_SPI_CLK               4
#define LL_GPIOB_P5_F5_CANFD_RX              5
#define LL_GPIOB_P5_F6_PWM5                  6
#define LL_GPIOB_P5_F8_UART1_CTS             8
#define LL_GPIOB_P5_F9_TWI1_SDA              9

#define LL_GPIOB_P6_F2_FC_MOSI               2
#define LL_GPIOB_P6_F3_TWI0_SDA              3
#define LL_GPIOB_P6_F4_SPI_MOSI              4
#define LL_GPIOB_P6_F5_DBG14                 5
#define LL_GPIOB_P6_F6_PWM6                  6
#define LL_GPIOB_P6_F8_UART1_TX              8

#define LL_GPIOB_P7_F2_FC_MIS0               2
#define LL_GPIOB_P7_F3_TWI0_SCL              3
#define LL_GPIOB_P7_F4_SPI_MISO              4
#define LL_GPIOB_P7_F5_DBG15                 5
#define LL_GPIOB_P7_F6_PWM7                  6
#define LL_GPIOB_P7_F8_UART1_RX              8

/**
  * @}
  */

/** @defgroup GPIO_LL_OUTPUT Output Type
  * @{
  */
#define LL_GPIO_OUTPUT_PUSH_PULL           (0) /*!< Select push-pull as output type */
#define LL_GPIO_OUTPUT_OPEN_DRAIN          (1) /*!< Select open-drain as output type */
/**
  * @}
  */

/** @defgroup GPIO_LL_DRIVEING_LEVEL Output Driving Level
  * @{
  */
#define LL_GPIO_DRIVING_LVL_0              (0) /*!< Select I/O low output speed    */
#define LL_GPIO_DRIVING_LVL_1              (1) /*!< Select I/O medium output speed */
#define LL_GPIO_DRIVING_LVL_2              (2) /*!< Select I/O fast output speed   */
#define LL_GPIO_DRIVING_LVL_3              (3) /*!< Select I/O high output speed   */
/**
  * @}
  */

/** @defgroup GPIO_LL_PULL Pull Up Pull Down
  * @{
  */
#define LL_GPIO_PULL_NO                    (0) /*!< Select I/O no pull */
#define LL_GPIO_PULL_UP                    (1) /*!< Select I/O pull up */
#define LL_GPIO_PULL_DOWN                  (2) /*!< Select I/O pull down */
/**
  * @}
  */

/** @defgroup GPIO_LL_INTRR_TYPE
  * @{
  */
#define LL_GPIO_INTRR_RISING                (0)
#define LL_GPIO_INTRR_FAILLING              (1)
#define LL_GPIO_INTRR_RISING_FALLING        (2)
#define LL_GPIO_INTRR_HIGH_LEVEL            (3)
#define LL_GPIO_INTRR_LOW_LEVEL             (4)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/** @defgroup GPIO_LL_Exported_Functions GPIO Exported Functions
  * @{
  */

/** @defgroup GPIO_LL_EF_Port_Configuration Port Configuration
  * @{
  */

/**
  * @brief  Configure gpio mode for a dedicated pin on dedicated port.
  * @note   I/O mode can be Input mode, General purpose output, Alternate function mode or Analog.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  gpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_GPIOx_Pn_F0_INPUT
  *         @arg @ref LL_GPIOx_Pn_F1_OUTPUT
  *         @arg @ref LL_GPIOx_Pn_F14_EINT
  *         @arg @ref LL_GPIOx_Pn_F15_DIS
  *         @arg @ref LL_GPIOx_Pn_Fxx
  * @retval None
  */
__HW_FORCE_INLIDE
void LL_GPIO_SetPinFuncMode(gpio_dev_t *gpio, uint32_t pin, uint32_t mode)
{
	HW_SET_MSK_VAL(gpio->FUNC[HW_IDX_4(pin)], HW_MSK_4(pin), HW_POS_4(pin), mode);
}

/**
  * @brief  Return gpio mode for a dedicated pin on dedicated port.
  * @note   I/O mode can be Input mode, General purpose output, Alternate function mode or Analog.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  gpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_GPIOx_Pn_F0_INPUT
  *         @arg @ref LL_GPIOx_Pn_F1_OUTPUT
  *         @arg @ref LL_GPIOx_Pn_F14_EINT
  *         @arg @ref LL_GPIOx_Pn_F15_DIS
  *         @arg @ref LL_GPIOx_Pn_Fxx
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_GetPinFuncMode(gpio_dev_t *gpio, uint32_t pin)
{
	return HW_GET_MSK_VAL(gpio->FUNC[HW_IDX_4(pin)], HW_MSK_4(pin), HW_POS_4(pin));
}

/**
  * @brief  Configure gpio output type for several pins on dedicated port.
  * @note   Output type as to be set when gpio pin is in output or
  *         alternate modes. Possible type are Push-pull or Open-drain.
  * @param  gpio GPIO Port
  * @param  pin This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @param  OutputType This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_OUTPUT_PUSH_PULL
  *         @arg @ref LL_GPIO_OUTPUT_OPEN_DRAIN
  * @retval None
  */
__HW_FORCE_INLIDE
void LL_GPIO_SetPinOutputType(gpio_dev_t *gpio, uint32_t pin, uint32_t output_type)
{
	//HW_SET_MSK_VAL(gpio->OUTPUT_TYPE, OUTPUT_TYPE_Msk, pin, output_type);
	output_type ? HW_SET_BIT(gpio->OUTPUT_TYPE, pin) : HW_CLR_BIT(gpio->OUTPUT_TYPE, pin);
}

/**
  * @brief  Return gpio output type for several pins on dedicated port.
  * @note   Output type as to be set when gpio pin is in output or
  *         alternate modes. Possible type are Push-pull or Open-drain.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  gpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_GPIO_OUTPUT_PUSHPULL
  *         @arg @ref LL_GPIO_OUTPUT_OPENDRAIN
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_GetPinOutputType(gpio_dev_t *gpio, uint32_t pin)
{
	return HW_GET_MSK_VAL(gpio->OUTPUT_TYPE, HW_BIT(pin), pin);
}

/**
  * @brief  Configure gpio driving for a dedicated pin on dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  gpio GPIO Port
  * @param  Pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @param  level This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_DRIVING_LVL_0
  *         @arg @ref LL_GPIO_DRIVING_LVL_1
  *         @arg @ref LL_GPIO_DRIVING_LVL_2
  *         @arg @ref LL_GPIO_DRIVING_LVL_3
  * @retval None
  */
__HW_FORCE_INLIDE
void LL_GPIO_SetPinStreng(gpio_dev_t *gpio, uint32_t pin, uint32_t level)
{
	HW_SET_MSK_VAL(gpio->DRIVING[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin), level);
}

/**
  * @brief  Return gpio speed for a dedicated pin on dedicated port.
  * @note   I/O speed can be Low, Medium, Fast or High speed.
  * @note   Warning: only one pin can be passed as parameter.
  * @note   Refer to datasheet for frequency specifications and the power
  *         supply and load conditions for each speed.
  * @param  gpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_GPIO_DRIVING_LVL_0
  *         @arg @ref LL_GPIO_DRIVING_LVL_1
  *         @arg @ref LL_GPIO_DRIVING_LVL_2
  *         @arg @ref LL_GPIO_DRIVING_LVL_3
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_GetPinStreng(gpio_dev_t *gpio, uint32_t pin)
{
	return HW_GET_MSK_VAL(gpio->DRIVING[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin));

}

/**
  * @brief  Configure gpio pull-up or pull-down for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  gpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @param  pull This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_PULL_NO
  *         @arg @ref LL_GPIO_PULL_UP
  *         @arg @ref LL_GPIO_PULL_DOWN
  * @retval None
  */
__HW_FORCE_INLIDE
void LL_GPIO_SetPinPull(gpio_dev_t *gpio, uint32_t pin, uint32_t pull)
{
#if 0
	extern uint32_t idx ;
	extern uint32_t msk ;
	extern uint32_t pos ;

	idx = HW_IDX_2(pin);
	msk = HW_MSK_2(pin);
	pos = HW_POS_2(pin);

//	((word) = ((word) & ~(vmask)) | ((vmask) & ((val) << (shift))))

	gpio->PULLUPDWN_SET[idx] &= ~msk;
	pin <<= pos;
	pin = msk & pin;
	gpio->PULLUPDWN_SET[idx] |= pin;
#endif
//    HW_SET_MSK_VAL(gpio->PULLUPDWN_SET[idx], msk, pos, pull);
	HW_SET_MSK_VAL(gpio->PULLUPDWN_SET[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin), pull);
//    HW_SET_MSK_VAL(gpio->PULLUPDWN_SET[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin), pull);
}

/**
  * @brief  Return gpio pull-up or pull-down for a dedicated pin on a dedicated port
  * @note   Warning: only one pin can be passed as parameter.
  * @param  gpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_GPIO_PULL_NO
  *         @arg @ref LL_GPIO_PULL_UP
  *         @arg @ref LL_GPIO_PULL_DOWN
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_GetPinPull(gpio_dev_t *gpio, uint32_t pin)
{
	return HW_GET_MSK_VAL(gpio->PULLUPDWN_SET[HW_IDX_2(pin)], HW_MSK_2(pin), HW_POS_2(pin));
}

/**
  * @brief  Configure gpio interrupt trigger type for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @param  wake_type This parameter can be one of the following values:
  *         @arg @ref LL_GPIO_INTRR_RISING
  *         @arg @ref LL_GPIO_INTRR_FAILLING
  *         @arg @ref LL_GPIO_INTRR_RISING_FALLING
  *         @arg @ref LL_GPIO_INTRR_HIGH_LEVEL
  *         @arg @ref LL_GPIO_INTRR_LOW_LEVEL
  * @retval None
  */
__HW_FORCE_INLIDE
void LL_GPIO_SetPinInterruptType(gpio_dev_t *gpio, uint32_t pin, uint32_t intrr_type)
{
	HW_SET_MSK_VAL(gpio->INT_TYPE[HW_IDX_4(pin)], HW_MSK_4(pin), HW_POS_4(pin), intrr_type);
}

/**
  * @brief  Return gpio interrupt trigger for a dedicated pin on a dedicated port
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_GPIO_INTRR_RISING
  *         @arg @ref LL_GPIO_INTRR_FAILLING
  *         @arg @ref LL_GPIO_INTRR_RISING_FALLING
  *         @arg @ref LL_GPIO_INTRR_HIGH_LEVEL
  *         @arg @ref LL_GPIO_INTRR_LOW_LEVEL
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_GetPinInterruptType(gpio_dev_t *gpio, uint32_t pin)
{
	return HW_GET_MSK_VAL(gpio->INT_TYPE[HW_IDX_4(pin)], HW_MSK_4(pin), HW_POS_4(pin));
}

/**
  * @brief  Configure gpio delay for a dedicated port.
  * @param  gpio GPIO Port
  * @param  delay This parameter is 0 - 31:
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_SetPinDelay(gpio_dev_t *gpio, uint32_t delay)
{
	HW_SET_VAL(gpio->DBC, delay);
}

/**
  * @brief  Get gpio delay for a dedicated port.
  * @param  gpio GPIO Port
  * @param  delay This parameter is 0 - 31:
  * @retval None
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_GetPinDelay(gpio_dev_t *gpio)
{
	return HW_GET_VAL(gpio->DBC);
}

/**
  * @brief  Enable gpio alternate enhanced(I2C mode) for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_EnableI2CMode(gpio_dev_t *gpio, uint32_t pin)
{
	HW_SET_BIT(gpio->I2C_MODE, pin);
}

/**
  * @brief  Disable gpio alternate enhanced(I2C mode) for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_DisableI2CMode(gpio_dev_t *gpio, uint32_t pin)
{
	HW_CLR_BIT(gpio->I2C_MODE, pin);
}

/**
  * @brief  Enable gpio interrupt for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_EnablePinInterrupt(gpio_dev_t *gpio, uint32_t pin)
{
	HW_SET_BIT(gpio->INT_EN, pin);
}

/**
  * @brief  Disable gpio interrupt for a dedicated pin on a dedicated port.
  * @note   Warning: only one pin can be passed as parameter.
  * @param  lpio GPIO Port
  * @param  pin This parameter can be one of the following values:
  *         if port A LL_GPIO_PIN_0 to LL_GPIO_PIN_19
  *         if port B LL_GPIO_PIN_0 to LL_GPIO_PIN_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_DisablePinInterrupt(gpio_dev_t *gpio, uint32_t pin)
{
	HW_CLR_BIT(gpio->INT_EN, pin);
}
/**
  * @}
  */

__HW_STATIC_INLINE
uint32_t LL_GPIO_GetPinInterrupt(gpio_dev_t *gpio)
{
	return HW_GET_VAL(gpio->INT_EN);
}

__HW_STATIC_INLINE
uint32_t LL_GPIO_SetPinInterrupt(gpio_dev_t *gpio, uint32_t inter_en)
{
	return HW_SET_VAL(gpio->INT_EN, inter_en);
}

/**
  * @}
  */




/** @defgroup GPIO_LL_EF_Data_Access Data Access
  * @{
  */

/**
  * @brief  Return full input data register value for a dedicated port.
  * @param  gpio GPIO Port
  * @retval Input data register value of port
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_ReadInputPort(gpio_dev_t *gpio)
{
	return gpio->INPUT;
}

/**
  * @brief  Return if input data level for several pins of dedicated port is high or low.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_IsInputPinSet(gpio_dev_t *gpio, uint32_t pin_mask)
{
	return (((gpio->INPUT & pin_mask) == (pin_mask)) ? 1UL : 0UL);
}

/**
  * @brief  Write output data register for the port.
  * @param  gpio GPIO Port
  * @param  port_value Level value for each pin of the port
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_WriteOutputPort(gpio_dev_t *gpio, uint32_t port_value)
{
	gpio->OUTPUT = port_value;
}

/**
  * @brief  Return full output data register value for a dedicated port.
  * @param  gpio GPIO Port
  * @retval Output data register value of port
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_ReadOutputPort(gpio_dev_t *gpio)
{
	return gpio->OUTPUT;
}

/**
  * @brief  Return if input data level for several pins of dedicated port is high or low.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_IsOutputPinSet(gpio_dev_t *gpio, uint32_t pin_mask)
{
	return (((gpio->OUTPUT & pin_mask) == (pin_mask)) ? 1UL : 0UL);
}

/**
  * @brief  Set several pins to high level on dedicated gpio port.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_SetOutputPin(gpio_dev_t *gpio, uint32_t pin_mask)
{
	gpio->OUTPUT |= pin_mask;
}

/**
  * @brief  Set several pins to low level on dedicated gpio port.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_ResetOutputPin(gpio_dev_t *gpio, uint32_t pin_mask)
{
	gpio->OUTPUT &= ~pin_mask;
}

/**
  * @brief  Toggle data value for several pin of dedicated port.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_TogglePin(gpio_dev_t *gpio, uint32_t pin_mask)
{
	gpio->OUTPUT ^= pin_mask;
}

/**
  * @brief  Check interrupt pending for a dedicated port.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_IsPinInterruptPending(gpio_dev_t *gpio, uint32_t pin_mask)
{
	return (gpio->INT_FLAG & pin_mask);
}

/**
  * @brief  Clear pin interrupt pending of dedicated port.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_GPIO_ClearPinInterruptPending(gpio_dev_t *gpio, uint32_t pin_mask)
{
	gpio->INT_CLR |= pin_mask;
}

/**
  * @brief  Check interrupt enable for a dedicated port.
  * @param  gpio GPIO Port
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_GPIO_IsPinInterruptEnabled(gpio_dev_t *gpio, uint32_t pin_mask)
{
	return (gpio->INT_EN & pin_mask);
}

/**
  * @}
  */


/** @defgroup GPIO_LL_FUNC_DELAY Func Delay
  * @{
  */

/**
  * @brief  Set spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_SPI_SetSclkOutDelay(delay_chain_ctrl_t *chain_ctrl, uint32_t trim)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKOUT_DELAY_TRIM_Msk, QSPI0_SCLKOUT_DELAY_TRIM_Pos, trim);
}

/**
  * @brief  Set spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_GetSclkOutDelay(delay_chain_ctrl_t *chain_ctrl)
{
	return HW_GET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKOUT_DELAY_TRIM_Msk, QSPI0_SCLKOUT_DELAY_TRIM_Pos);
}

/**
  * @brief  Set spi func clock in delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_SPI_SetSclkInDelay(delay_chain_ctrl_t *chain_ctrl, uint32_t trim)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKIN_DELAY_TRIM_Msk, QSPI0_SCLKIN_DELAY_TRIM_Pos, trim);
}

/**
  * @brief  Set spi func clock in delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_GetSclkInDelay(delay_chain_ctrl_t *chain_ctrl)
{
	return HW_GET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKIN_DELAY_TRIM_Msk, QSPI0_SCLKIN_DELAY_TRIM_Pos);
}


/**
  * @brief  Enable spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_SPI_EnableSclkOutDelay(delay_chain_ctrl_t *chain_ctrl, uint32_t trim)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKOUT_DELAY_EN_Msk, QSPI0_SCLKOUT_DELAY_EN_Pos, 1);
}

/**
  * @brief  Disable spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
void LL_SPI_DisableSclkOutDelay(delay_chain_ctrl_t *chain_ctrl)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKOUT_DELAY_EN_Msk, QSPI0_SCLKOUT_DELAY_EN_Pos, 0);
}

/**
  * @brief  Check spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_IsableSclkOutDelay(delay_chain_ctrl_t *chain_ctrl)
{
	return HW_GET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKOUT_DELAY_EN_Msk, QSPI0_SCLKOUT_DELAY_EN_Pos);
}


/**
  * @brief  Enable spi func clock in delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
void LL_SPI_EnableSclkInDelay(delay_chain_ctrl_t *chain_ctrl, uint32_t trim)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKIN_DELAY_EN_Msk, QSPI0_SCLKIN_DELAY_EN_Pos, 1);
}

/**
  * @brief  Disable spi func clock in delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
void LL_SPI_DisableSclkInDelay(delay_chain_ctrl_t *chain_ctrl)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKIN_DELAY_EN_Msk, QSPI0_SCLKIN_DELAY_EN_Pos, 0);
}

/**
  * @brief  Check spi func clock in delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_SPI_IsableSclkInDelay(delay_chain_ctrl_t *chain_ctrl)
{
	return HW_GET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, QSPI0_SCLKIN_DELAY_EN_Msk, QSPI0_SCLKIN_DELAY_EN_Pos);
}


/**
  * @brief  Set spi func clock in delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_FSMC_SetSclkDelay(delay_chain_ctrl_t *chain_ctrl, uint32_t trim)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, FLASHC_DELAY_EN_Msk, FLASHC_DELAY_EN_Pos, trim);
}

/**
  * @brief  Set spi func clock in delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  */
__HW_STATIC_INLINE
uint32_t LL_FSMC_GetSclkDelay(delay_chain_ctrl_t *chain_ctrl)
{
	return HW_GET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, FLASHC_DELAY_EN_Msk, FLASHC_DELAY_EN_Pos);
}


/**
  * @brief  Enable spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval None
  */
__HW_STATIC_INLINE
void LL_FSMC_EnableSclkDelay(delay_chain_ctrl_t *chain_ctrl, uint32_t trim)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, FLASHC_DELAY_EN_Msk, FLASHC_DELAY_EN_Pos, 1);
}

/**
  * @brief  Disable spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
void LL_FSMC_DisableSclkDelay(delay_chain_ctrl_t *chain_ctrl)
{
	HW_SET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, FLASHC_DELAY_EN_Msk, FLASHC_DELAY_EN_Pos, 0);
}

/**
  * @brief  Check spi func clock out delay
  * @param  chain_ctrl CHAIN_CTRL
  * @param  pin_mask This parameter can be a combination of the following values:
  *         if port A LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_19
  *         if port B LL_GPIO_PIN_MSK_0 to LL_GPIO_PIN_MSK_9
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_FSMC_IsableSclkDelay(delay_chain_ctrl_t *chain_ctrl)
{
	return HW_GET_MSK_VAL(chain_ctrl->DELAY_CHAIN_CTRL, FLASHC_DELAY_EN_Msk, FLASHC_DELAY_EN_Pos);
}


/**
  * @}
  */



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_GPIO_H */

