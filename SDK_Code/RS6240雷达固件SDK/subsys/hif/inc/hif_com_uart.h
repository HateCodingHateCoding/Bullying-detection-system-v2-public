/**
 **************************************************************************************************
 * @file    hif_com_uart.h
 * @brief   .
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _HIF_COM_UART_H_
#define _HIF_COM_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */
/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

int hif_com_uart_init(uint32_t rate);

int hif_com_uart_deinit(void);

void hif_com_uart_wakeup(void);

int hif_com_uart_tx_start(void);

int hif_com_uart_tx_done(void);

void hif_com_uart_tx_timeout(void *arg);

int hif_com_uart_rx_done(void);

void hif_com_uart_rx_timeout(void *arg);

int hif_com_uart_write(uint8_t *pbuff, uint16_t size);

int hif_com_uart_read(uint8_t *pbuff, uint16_t size);


#if (CONFIG_HIF_DEVICE_WAKEUP_UART)

int hif_com_uart_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg);

int hif_com_uart_pm_deinit(void);

#endif /* CONFIG_HIF_DEVICE_WAKEUP_UART */



#ifdef __cplusplus
}
#endif

#endif /* _HIF_COM_UART_H_ */

