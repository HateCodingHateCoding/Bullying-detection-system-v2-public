/**
 ******************************************************************************
 * @file    hif_mem.c
 * @brief   hif mem define.
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
#include "hif_types.h"
#include "hif_config.h"
#include "hif_mem.h"

#if (CONFIG_HIF == 1)

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */


/* Private macros.
 * ----------------------------------------------------------------------------
 */
static payload_node_t hif_mem_list_free;

static uint8_t *hif_mem_heap = NULL;
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

static void payload_node_init(payload_node_t *payload_node)
{
    payload_node->next = NULL;
    payload_node->payload = NULL;
    payload_node->data_len = 0;
}

static void payload_node_add(payload_node_t *payload_node, payload_node_t *head)
{
    payload_node_t *node = head->next;
    head->next = payload_node;
    payload_node->next = node;
}


static int payload_node_add_tail(payload_node_t *payload_node, payload_node_t *head)
{
    payload_node_t *node = head->next;
    payload_node_t *preNode = head;
    while (node) {
        preNode = node;
        node = node->next;
    }
    preNode->next = payload_node;
    return 0;
}

static int payload_node_empty(const payload_node_t *head)
{
    return (head->next == NULL);
}

static payload_node_t *payload_node_get(payload_node_t *list)
{
	payload_node_t *node = NULL;

	if (!payload_node_empty(list)) {
		node = list->next;
		list->next = node->next;
        node->next = NULL;
	}

	return node;
}

int hif_mem_init(void)
{
    uint32_t malloc_size;

    malloc_size =  sizeof(payload_node_t) * HIF_MEM_BLK_CNT;

    hif_mem_heap = HIF_MemMalloc(malloc_size);
    if (hif_mem_heap == NULL) {
        return -1;
    }
    memset(hif_mem_heap, 0, malloc_size);
    payload_node_init(&hif_mem_list_free);
    uint8_t *pbuff = hif_mem_heap;
    payload_node_t *payload_node;

    for (uint32_t idx = 0; idx < HIF_MEM_BLK_CNT; idx++) {
        payload_node = (payload_node_t *)pbuff;
        pbuff += sizeof(payload_node_t);

        payload_node_add_tail(payload_node, &hif_mem_list_free);
    }

    return 0;
}

int hif_mem_deinit(void)
{
    payload_node_init(&hif_mem_list_free);

    HIF_MemFree(hif_mem_heap);

    return 0;
}

payload_node_t * hif_mem_malloc(void)
{
    return payload_node_get(&hif_mem_list_free);
}

int hif_mem_put(payload_node_t *payload_node, payload_node_t *payload_head)
{
    return payload_node_add_tail(payload_node, payload_head);
}

void hif_mem_free(payload_node_t *payload_head)
{
    payload_node_t *node = payload_head->next;
    while(node) {
        payload_head->next = node->next;
        payload_node_init(node);
        payload_node_add(node, &hif_mem_list_free);
        node = payload_head->next;
    }

    payload_node_init(payload_head);
    payload_node_add(payload_head, &hif_mem_list_free);
}


int hif_mem_put_item(payload_node_t *payload_node, hif_queue_item_t *hif_queue_item)
{
    payload_node_t *node = hif_queue_item->payload;
    if (payload_node == NULL) {
        return -1;
    }

    if(node == NULL) {
        hif_queue_item->payload = payload_node;
    } else {
        payload_node_add_tail(payload_node, node);
    }

    return 0;
}

void hif_mem_free_item(hif_queue_item_t *hif_queue_item)
{
    payload_node_t *node = hif_queue_item->payload;

    if(node) {
        hif_mem_free(node);
        hif_queue_item->payload = NULL;
    }
}

#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
