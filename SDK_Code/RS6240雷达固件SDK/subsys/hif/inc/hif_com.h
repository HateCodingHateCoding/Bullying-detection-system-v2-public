/**
 **************************************************************************************************
 * @file    hif_com.h
 * @brief   .
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _HIF_COM_H_
#define _HIF_COM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "hif_types.h"
#include  "hif.h"
//#include "hif_pm.h"

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */

// typedef enum {
// 	HIF_COM_TYPE_DISABLE	= 0,
// 	HIF_COM_TYPE_UART,
// 	HIF_COM_TYPE_IIC,
// 	HIF_COM_TYPE_SPI,

// } hif_com_type_t;

typedef enum {
	TRF_STATE_IDLE			= 0,
	TRF_STATE_RX_WAIT,
	TRF_STATE_RX_BUSY,
	TRF_STATE_RX_DONE,
	TRF_STATE_TX_WAIT,
	TRF_STATE_TX_BUSY,
	TRF_STATE_TX_DONE,
}trf_state_t;

typedef struct {
	uint8_t *pbuff;
	uint16_t size;
	uint16_t offset;
	uint8_t status;
	trf_state_t state;
} hif_trf_t;

	typedef int (* com_init_t)(uint32_t rate);
	typedef int (* com_deinit_t)(void);

	typedef int (* com_tx_start_t)(void);
	typedef int (* com_tx_t)(uint8_t *pbuff, uint16_t size);
	typedef int (* com_tx_end_t)(void);
	typedef void (* com_tx_timeout_t)(void *arg);

	typedef int (* com_rx_start_t)(void);
	typedef int (* com_rx_t)(uint8_t *pbuff, uint16_t size);
	typedef int (* com_rx_end_t)(void);
	typedef void (* com_rx_timeout_t)(void *arg);

	typedef void (* com_wakeup_t)(void);

	typedef void (* pm_wakeup_callback_t)(uint32_t param);
	typedef int (* com_pm_init_t)(pm_wakeup_callback_t pm_cb, void *pcfg);
	typedef int (* com_pm_deinit_t)(void);


typedef struct {
	HIF_ComType_t				com_type;

	com_init_t					com_init;
	com_deinit_t				com_deinit;

	com_wakeup_t				com_wakeup;

	com_tx_start_t				com_tx_start;
	com_tx_t					com_tx;
	com_tx_end_t				com_tx_end;
	com_tx_timeout_t			com_tx_timeout;

	com_rx_start_t				com_rx_start;
	com_rx_t					com_rx;
	com_rx_end_t				com_rx_end;
	com_rx_timeout_t			com_rx_timeout;

	com_pm_init_t				com_pm_init;
	com_pm_deinit_t				com_pm_deinit;

} hif_com_t;

typedef enum{
	com_uart0,
	com_uart1,
	com_uart2,
	com_spi,
	com_i2c,
}com_type_e;

typedef struct{
	com_type_e com_type;
	uint32_t dstAddr;
}com_port_t;

extern com_port_t com_port;
void com_port_set(com_type_e com_type);

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

int hif_com_register(HIF_ComType_t com_type);

void hif_com_rx_buff_reset(void);

int hif_com_init(uint32_t rate);

int hif_com_deinit(void);

int hif_com_msg_send(void *param, bool start);

void hif_com_send_start(void);

void hif_com_send_finish(void);

int hif_com_pm_init(pm_wakeup_callback_t pm_cb,  void *pcfg);

int hif_com_pm_deinit(void);

	#define HIF_COM_RX_SUCCESS						0
	#define HIF_COM_RX_CONTINUE 					1
int hif_com_data_recv(void);

void hif_com_data_recv_done(void);

	#define HIF_COM_TX_NULL 						0
	#define HIF_COM_TX_SUCCESS						1
	#define HIF_COM_TX_CONTINUE 					2
	#define HIF_COM_TX_DONE 						3
int hif_com_data_send(uint8_t start);
void hif_com_data_send_done(void);

void hif_com_data_output_done(void);

void hif_com_block_send_done(void);

#ifdef __cplusplus
}
#endif

#endif /* _HIF_COM_H_ */

