/*
 *  Copyright (c) 2022 Possumic <www.possumic.com>
 */

#ifndef __HIF_CONFIG_H__
#define __HIF_CONFIG_H__

/* Include Files */
#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_HIF
#define CONFIG_HIF                          0
#endif


/* Enable Debug parameters and printk */
#define HIF_DEBUG_ENABLE                     0
#define HIF_DEBUG_API_ENABLE                 1


#ifndef CONFIG_HIF_DEVICE_COM_UART
#define CONFIG_HIF_DEVICE_COM_UART            0
#else

#ifndef CONFIG_HIF_UART_PORT
#define CONFIG_HIF_UART_PORT                0
#endif //CONFIG_HIF_UART_PORT

#undef CONFIG_DRIVER_UART
#define CONFIG_DRIVER_UART                    1

#endif //CONFIG_HIF_DEVICE_COM_UART

#ifndef CONFIG_HIF_DEVICE_WAKEUP_UART
#define CONFIG_HIF_DEVICE_WAKEUP_UART        0
#else
#if (CONFIG_PM == 1)
#define CONFIG_UART_WUP_ENABLED                1
#else
#define CONFIG_UART_WUP_ENABLED                0
#endif
#endif

#ifndef CONFIG_HIF_DEVICE_COM_SPI
#define CONFIG_HIF_DEVICE_COM_SPI            0
#else
#define CONFIG_DRIVER_SPI0                    1
#endif

#ifndef CONFIG_HIF_DEVICE_WAKEUP_SPI
#define CONFIG_HIF_DEVICE_WAKEUP_SPI        0
#else
#if (CONFIG_PM == 1)
#define CONFIG_SPI_WUP_ENABLED                1
#else
#define CONFIG_SPI_WUP_ENABLED                0
#endif
#endif

#ifndef CONFIG_HIF_DEVICE_COM_IIC
#define CONFIG_HIF_DEVICE_COM_IIC            0
#else
#define CONFIG_DRIVER_I2C0                    1
#define CONFIG_I2C_SLAVE                    1
#define CONFIG_I2C_TRANS_IT_ENABLE            1
#endif

#ifndef CONFIG_HIF_DEVICE_WAKEUP_IIC
#define CONFIG_HIF_DEVICE_WAKEUP_IIC        0
#else
#if (CONFIG_PM)
#define CONFIG_I2C_WAKEUP                    1
#else
#define CONFIG_I2C_WAKEUP                    0
#endif
#endif

#ifndef CONFIG_HIF_SEND_SPLIT
#define CONFIG_HIF_SEND_SPLIT                0
#endif

#ifndef CONFIG_HIF_SEND_DMA
#define CONFIG_HIF_SEND_DMA                    0
#endif

#ifndef CONFIG_HIF_DMA_PINGPONG
#define CONFIG_HIF_DMA_PINGPONG                0
#endif

#if (CONFIG_SECTION_ATTRIBUTE_SRAM && CONFIG_HIF)
#define __hif_sram_text             __sram_text
#define __hif_sram_rodata             __sram_rodata
#define __hif_sram_data             __sram_data
#define __hif_sram_bss                 __sram_bss
#else
#define __hif_sram_text
#define __hif_sram_rodata
#define __hif_sram_data
#define __hif_sram_bss
#endif


#if (CONFIG_HIF_CMD_PAYLOAD_MAX_LENGTH)
#define HOST_CMD_MAX_PAYLOAD                 CONFIG_HIF_CMD_PAYLOAD_MAX_LENGTH
#else
#define HOST_CMD_MAX_PAYLOAD                 1024
#endif

#if (CONFIG_HIF_CMD_PAYLOAD_MAX_LENGTH)
#define HIF_MSG_SEND_MAX_PAYLOAD             CONFIG_HIF_CMD_PAYLOAD_MAX_LENGTH
#else
#define HIF_MSG_SEND_MAX_PAYLOAD             4095
#endif


#ifndef CONFIG_HIF_THREAD_CHECK_PERIOD
#define HIF_THREAD_CHECK_PERIOD              10
#else
#define HIF_THREAD_CHECK_PERIOD              CONFIG_HIF_THREAD_CHECK_PERIOD
#endif

#if (CONFIG_HOST_INTERFACE_STACK_SIZE)
#define HOST_INTERFACE_STACK_SIZE            CONFIG_HOST_INTERFACE_STACK_SIZE
#else
#define HOST_INTERFACE_STACK_SIZE            2048
#endif

#if (CONFIG_HIF_WATCHDOG && !CONFIG_TASK_WDT_HW_FALLBACK)
#define HIF_WATCHDOG_ENABLE                  1
#define HIF_WATCHDOG_TIMEOUT_MS              4000
#else
#define HIF_WATCHDOG_ENABLE                  0
#endif

#ifdef __cplusplus
}
#endif

#endif //__HIF_CONFIG_H__
