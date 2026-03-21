#ifndef _HIF_COM_DMA_H_
#define _HIF_COM_DMA_H_

#include "hif_types.h"
#include "hal_dma.h"
#include "hif_mem.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
	uart0_tx = DMA_HANDSHAKE_TXFIFO_UART0,
	uart1_tx = DMA_HANDSHAKE_TXFIFO_UART1,
	uart2_tx = DMA_HANDSHAKE_TXFIFO_UART2,
	spi0_tx = DMA_HANDSHAKE_TXFIFO_SPI0,
	spi0_rx = DMA_HANDSHAKE_RXFIFO_SPI0,
}dma_trigger_e;

typedef enum{
    tx_channel_idx = 0,
    rx_channel_idx = 1,
}dma_channel_id_e;

typedef struct{
    uint32_t src_addr;
    uint32_t dest_addr;
    uint32_t block_size;
    uint8_t int_en;
    uint8_t head_idx;
    uint8_t null_tail;
}dma_node_param_t;

int hif_com_tx_dma_init(dma_trigger_e dmaTrigger);

int hif_com_rx_dma_init(dma_trigger_e dmaTrigger);

int hif_com_dma_block_config(dma_channel_id_e dma_channel, uint32_t dest_addr, uint32_t size);

uint32_t hif_com_dma_block_size_get(dma_channel_id_e dma_channel);

DMA_NodeConf_t *hif_com_dma_list_node_add(dma_node_param_t dma_node_param);

bool hif_com_dma_list_node_add_allow(void);

void hif_com_dma_reset_racing(void);

bool hif_com_is_burst_end_race(void);

bool hif_com_dma_free_allow(hif_queue_item_t *hif_queue_item);

int hif_com_dma_list_free(hif_queue_item_t *hif_queue_item);

void hif_com_data_dma_list_config(void);

int hif_com_dma_transfer_start(dma_channel_id_e dma_channel);

int hif_com_dma_transfer_stop(dma_channel_id_e dma_channel);

int hif_com_dma_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
