/**
 **************************************************************************************************
 * @file    hif_mem.h
 * @brief   hif mem.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _HIF_MEM_H_
#define _HIF_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hif_msg.h"
/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */

#define HIF_QUEUE_SIZE          (20)
#define HIF_MEM_BLK_CNT         (HIF_QUEUE_SIZE*3)
#define CONFIG_DMA_ITEM_COUNT   (128)
#define CONFIG_DMA_ITEM_MASK	(CONFIG_DMA_ITEM_COUNT - 1)

typedef struct payload_node_s{
    struct payload_node_s *next;
    uint8_t *payload;
    uint16_t data_len;
}payload_node_t;

typedef struct __queue_item_s{
	struct __queue_item_s *next;
    uint8_t         hdr_byte[7];
    uint8_t         hdr_len;
    uint8_t         checksum[7];
    uint8_t         check_len;
    payload_node_t  *payload;
    void            *node_end;
    void            *send_cb;
	OSI_Semaphore_t  block_sem;
} hif_queue_item_t;

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

#define HIF_MemMalloc(size)             OSI_Malloc(size)

#define HIF_MemFree(mem)                OSI_Free(mem)


int hif_mem_init(void);

int hif_mem_deinit(void);

payload_node_t * hif_mem_malloc(void);

void hif_mem_free(payload_node_t *payload_head);

int hif_mem_put(payload_node_t *payload_node, payload_node_t *payload_head);

int hif_mem_put_item(payload_node_t *payload_node, hif_queue_item_t *hif_queue_item);

void hif_mem_free_item(hif_queue_item_t *hif_queue_item);


#ifdef __cplusplus
}
#endif

#endif /* _HIF_MEM_H_ */


