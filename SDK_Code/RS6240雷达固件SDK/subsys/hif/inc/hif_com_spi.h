/**
 **************************************************************************************************
 * @file    hif_com_spi.h
 * @brief   .
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _HIF_COM_SPI_H_
#define _HIF_COM_SPI_H_

#include "hif_com.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_RX_BUFFER_SIZE      (512)

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

int hif_com_spi_init(uint32_t rate);

int hif_com_spi_deinit(void);

int hif_com_spi_write(uint8_t *pbuff, uint16_t size);

int hif_com_spi_read(uint8_t *pbuff, uint16_t size);

int hif_com_dma_pingpong_spi_tx_start(void);

int hif_com_spi_tx_start(void);

int hif_com_spi_tx_end(void);

void hif_com_spi_tx_timeout(void *arg);

void hif_com_spi_rx_timeout(void *arg);

int hif_com_spi_rx_end(void);

void hif_com_spi_wakeup(void);




#if (CONFIG_HIF_DEVICE_WAKEUP_SPI)

int hif_com_spi_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg);

int hif_com_spi_pm_deinit(void);

#endif /* CONFIG_HIF_DEVICE_WAKEUP_SPI */



#ifdef __cplusplus
}
#endif

#endif /* _HIF_COM_SPI_H_ */

