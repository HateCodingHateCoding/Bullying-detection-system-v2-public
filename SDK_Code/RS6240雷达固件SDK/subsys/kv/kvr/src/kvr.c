/**
 ******************************************************************************
 * @file    hal_kvr.c
 * @brief   hal key value ram define.
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

#include "hal_types.h"


#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

enum {
    KVR_SUCCESS                 = 0,
    KVR_ERR_STORE_ADDR          =-1,
    KVR_ERR_STORE_SIZE          =-2,
    KVR_ERR_MAGIC               =-3,
    KVR_ERR_HEAD_CS             =-4,
    KVR_ERR_PL_SIZE             =-5,
    KVR_ERR_LIST_CS             =-6,

    KVR_ERR_ITEM_NOT_FOUND      =-7,
    KVR_ERR_ITEMS_CS            =-8,
    KVR_ERR_ITEM_STATE          =-9,
    KVR_ERR_ITEM_PARAM          =-10,
    KVR_ERR_ITEM_PL_CS          =-11,

    KVR_ERR_PARAM               =-12,
    KVR_ERR_UNDEFINE            =-13,
};

typedef struct {
    uint32_t magic;
    uint32_t base_addr;
    uint16_t store_size;
    uint16_t free_offset;
    uint16_t list_size;
    uint8_t  list_cs;
    uint8_t  head_cs;
} KVR_Head_t;

typedef struct {
    uint16_t pl_offset;
    uint16_t pl_size;
    uint8_t  pl_cs;
    uint8_t  state;
    uint8_t  id;
    uint8_t  item_cs;
} KVR_Item_t;

/* Private defines.
 * ----------------------------------------------------------------------------
 */

#define CONFIG_KVR_MIN_BUFF_SIZE                        64
#define CONFIG_KVR_TYPE_MAX                             2

#ifndef CONFIG_KVR_AREA_ADDR
#define CONFIG_KVR_AREA_ADDR                            0x00400000
#endif

#ifndef CONFIG_KVR_AREA_SIZE
#define CONFIG_KVR_AREA_SIZE                            512
#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

#define KVR_MAGIC                               0x4B565253  /*KVRS*/

#define KVR_ITEM_STATE_ACTIVE                   0x5A

#define KVR_HEAD_SIZE                           sizeof(KVR_Head_t)
#define KVR_ITEM_SIZE                           sizeof(KVR_Item_t)

#define kvr_ListEreaStartOffset(pKvrHead)       (KVR_HEAD_SIZE)
#define kvr_ListEreaEndOffset(pKvrHead)         (KVR_HEAD_SIZE + pKvrHead->list_size)
#define kvr_ListEreaAddr(pKvrHead)              (pKvrHead->base_addr + KVR_HEAD_SIZE)

#define kvr_PlEreaStartOffset(pKvrHead)     (KVR_HEAD_SIZE + pKvrHead->list_size)
#define kvr_PlEreaEndOffset(pKvrHead)       (pKvrHead->store_size)


#define kvr_GetFreeSize(pKvrHead)           (pKvrHead->free_offset - KVR_HEAD_SIZE - pKvrHead->list_size)

#define kvr_GetItemMaxIdx(pKvrHead)         (pKvrHead->list_size / KVR_ITEM_SIZE)
#define kvr_GetItem(pKvrHead, idx)          \
    (KVR_Item_t *)(pKvrHead->base_addr + KVR_HEAD_SIZE + (KVR_ITEM_SIZE * (idx)))
#define kvr_IncItemIdx(pKvrHead)            (pKvrHead->list_size += KVR_ITEM_SIZE)
#define kvr_DecItemIdx(pKvrHead)            (pKvrHead->list_size -= KVR_ITEM_SIZE)
#define kvr_GetItemCS(pKvrItem)             (0 == kvr_CheckSum((uint8_t *)(pKvrItem), KVR_ITEM_SIZE))
#define kvr_SetItemCS(pKvrItem)                                                             \
            do {                                                                            \
                pKvrItem->item_cs = 0;                                                      \
                pKvrItem->item_cs = kvr_CheckSum((uint8_t *)(pKvrItem), KVR_ITEM_SIZE);     \
            } while (0)
#define kvr_CopyItem(pKvrHead, dst_idx, src_idx)                                                                \
            do {                                                                                                \
                memcpy((kvr_GetItem(pKvrHead, dst_idx)), (kvr_GetItem(pKvrHead, src_idx)), KVR_ITEM_SIZE);      \
            } while (0)


#define kvr_GetItemPl(pKvrHead, pKvrItem)       (uint8_t *)(pKvrHead->base_addr + pKvrItem->pl_offset)
#define kvr_SetItemPlCS(pKvrHead, pKvrItem)                                                         \
        do {                                                                                        \
            pKvrItem->pl_cs = kvr_CheckSum(kvr_GetItemPl(pKvrHead, pKvrItem), pKvrItem->pl_size);   \
        } while (0)
#define kvr_GetItemPlCS(pKvrHead, pKvrItem)       \
        (pKvrItem->pl_cs == kvr_CheckSum(kvr_GetItemPl(pKvrHead, pKvrItem), pKvrItem->pl_size))
#define kvr_GetItemPlValue(pKvrHead, pKvrItem, buff, len)                                   \
        do {                                                                                \
            memcpy(buff, kvr_GetItemPl(pKvrHead, pKvrItem), len);                           \
        } while (0)
#define kvr_SetItemPlValue(pKvrHead, pKvrItem, buff, len)                                   \
        do {                                                                                \
            memcpy(kvr_GetItemPl(pKvrHead, pKvrItem), buff, len);                           \
        } while (0)

#define kvr_UpdateHeadListCS(pKvrHead)    \
        do {                                                                                   \
            pKvrHead->list_cs = kvr_CheckSum((uint8_t *)(pKvrHead->base_addr + KVR_HEAD_SIZE), \
                                    pKvrHead->list_size);    \
            pKvrHead->head_cs = 0;                                                             \
            pKvrHead->head_cs = kvr_CheckSum((uint8_t *)(pKvrHead->base_addr), KVR_HEAD_SIZE); \
        } while (0)


#define KVR_GET_TYPE(id)                        (((id) >> 15) & 0x1)
#define KVR_GET_IDX(id)                         ((id) & 0x7FFF)


/* Private variables.
 * ----------------------------------------------------------------------------
 */
static KVR_Head_t *pKvrHeadCtrl[CONFIG_KVR_TYPE_MAX];

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static uint8_t kvr_CheckSum(void * pbuff, uint32_t size);
static int kvr_FindItem(KVR_Head_t *pKvrHead, uint8_t id, uint8_t * pItemIidx);
static int kvr_HeadCheck(KVR_Head_t *pKvrHead, uint32_t Addr);

int kvr_HwInit(uint8_t type, uint32_t addr, uint32_t size);
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int KVR_Init(void)
{
    int status = 0;

    /* user kvr */
    status = kvr_HwInit(0, CONFIG_KVR_AREA_ADDR, CONFIG_KVR_AREA_SIZE);
    if (status != 0) {
        return status;
    }

    /* sys kvr */
    status = kvr_HwInit(1, CONFIG_KVR_AREA_ADDR + CONFIG_KVR_AREA_SIZE,
        CONFIG_KVR_AREA_SIZE);
    if (status != 0) {
        return status;
    }

    return status;
}

int kvr_HwInit(uint8_t type, uint32_t addr, uint32_t size)
{
    int status = KVR_SUCCESS;
    KVR_Head_t *pKvrHead = NULL;


    if ((size == 0)
        || (size < CONFIG_KVR_MIN_BUFF_SIZE)
        || ((size & 0x3) != 0)) {
        LOG_ERR("store size err. (%d)\n", size);
        return KVR_ERR_STORE_SIZE;
    }

    if ((addr & 0x3) != 0) {
        LOG_ERR("store addr err. (0x%X)\n", addr);
        return KVR_ERR_STORE_ADDR;
    }


    if (type >= CONFIG_KVR_TYPE_MAX) {
        return KVR_ERR_PARAM;
    }

    if (pKvrHeadCtrl[type] == NULL) {
        pKvrHeadCtrl[type] = (KVR_Head_t *)addr;
    }


    do {
        status = kvr_HeadCheck(pKvrHeadCtrl[type], addr);
        if (status != 0) {
            break;
        }

        if (pKvrHeadCtrl[type]->store_size != size) {
            status = KVR_ERR_STORE_SIZE;
            LOG_ERR("head store size err. head_size(%d) regist_size(%d)\n", pKvrHeadCtrl[type]->store_size, size);
            break;
        }
    } while (0);


    if (status != KVR_SUCCESS) {
        pKvrHeadCtrl[type] = (KVR_Head_t *)addr;
        pKvrHead = pKvrHeadCtrl[type];

        pKvrHead->magic = KVR_MAGIC;
        pKvrHead->base_addr = addr;
        pKvrHead->store_size = size;
        pKvrHead->free_offset = size;
        pKvrHead->list_size = 0;
        pKvrHead->list_cs = 0;
        pKvrHead->head_cs = 0;
        pKvrHead->head_cs = kvr_CheckSum((uint8_t *)pKvrHead->base_addr, KVR_HEAD_SIZE);

        /* clear list and payload erea */
        uint32_t * pbuf = (uint32_t *)(pKvrHead->base_addr + KVR_HEAD_SIZE);
        uint16_t pl_size = (size - KVR_HEAD_SIZE) / 4;
        for (uint16_t idx = 0; idx < pl_size; idx++) {
            pbuf[idx] = 0;
        }

        status = KVR_SUCCESS;

        LOG_INF("head init success\n");
    } else {
        if ((uint32_t)pKvrHeadCtrl[type] != addr) {
            status = KVR_ERR_STORE_ADDR;
        }
    }


    return status;
}

int KVR_Save(uint16_t id, void *pdata, uint32_t size)
{
    int status = KVR_SUCCESS;
    uint8_t idx = 0;
    KVR_Item_t *pKvrItem = NULL;

    do {
        if ((pdata == NULL)
            || (size == 0)) {
            status = KVR_ERR_PARAM;
            LOG_ERR("input param err\n");
            break;
        }

        KVR_Head_t *pKvrHead = pKvrHeadCtrl[KVR_GET_TYPE(id)];
        id = KVR_GET_IDX(id);

        status = kvr_HeadCheck(pKvrHead, pKvrHead->base_addr);
        if (status != KVR_SUCCESS) {
            LOG_ERR("head check err(%d)\n", status);
            break;
        }

        status = kvr_FindItem(pKvrHead, id, &idx);
        if (status != KVR_SUCCESS) {
            if (status != KVR_ERR_ITEM_NOT_FOUND) {
                LOG_ERR("find item err(%d)\n", status);
                break;
            }

            /* create new item */
            if ((size + KVR_ITEM_SIZE) > kvr_GetFreeSize(pKvrHead)) {
                status = KVR_ERR_PL_SIZE;
                LOG_ERR("buff insufficient. free size(%d) req size(%d)\n", \
                    kvr_GetFreeSize(pKvrHead), size + KVR_ITEM_SIZE);
                break;
            }

            idx = kvr_GetItemMaxIdx(pKvrHead);
            pKvrItem = kvr_GetItem(pKvrHead, idx);
            pKvrItem->id = id;
            pKvrItem->pl_size = size;
            pKvrItem->pl_offset = pKvrHead->free_offset - pKvrItem->pl_size;
            pKvrItem->state = KVR_ITEM_STATE_ACTIVE;
            pKvrItem->pl_cs = 0;
            pKvrItem->item_cs = 0;

            pKvrHead->free_offset -= size;
            kvr_IncItemIdx(pKvrHead);
        } else {
            LOG_ERR("item found\n");
            pKvrItem = kvr_GetItem(pKvrHead, idx);

            /* remalloc */
            if (size > pKvrItem->pl_size) {
                LOG_DBG("remalloc buff. old size(%d), new size(%d)\n", pKvrItem->pl_size, size);
                uint16_t move_len = size - pKvrItem->pl_size;
                uint8_t lst_idx = kvr_GetItemMaxIdx(pKvrHead) - 1;
                if (lst_idx != idx) {
                    LOG_DBG("current item idx(%d), last item idx(%d)\n", idx, lst_idx);
                    KVR_Item_t *pKvrItemLst = kvr_GetItem(pKvrHead, lst_idx);
                    uint16_t move_size = pKvrItem->pl_offset - pKvrItemLst->pl_offset;
                    memcpy(kvr_GetItemPl(pKvrHead, pKvrItemLst) - move_len, \
                        kvr_GetItemPl(pKvrHead, pKvrItemLst), move_size);
                    LOG_DBG("payload move: from(%X) to(%X) size(%d)\n",
                        kvr_GetItemPl(pKvrHead, pKvrItemLst) - move_len,
                        kvr_GetItemPl(pKvrHead, pKvrItemLst),
                        move_size);

                    LOG_DBG("item move:\n");
                    KVR_Item_t *pKvrItemScan = NULL;
                    for (uint8_t scan_idx = idx + 1; scan_idx <= lst_idx; scan_idx++) {
                        pKvrItemScan = kvr_GetItem(pKvrHead, scan_idx);
                        pKvrItemScan->pl_offset -= move_len;
                        kvr_SetItemCS(pKvrItemScan);
                    }
                }

                pKvrItem->pl_offset -= move_len;
                pKvrItem->pl_size += move_len;

                pKvrHead->free_offset -= move_len;
            }
        }

        kvr_SetItemPlValue(pKvrHead, pKvrItem, pdata, size);
        kvr_SetItemPlCS(pKvrHead, pKvrItem);
        kvr_SetItemCS(pKvrItem);

        kvr_UpdateHeadListCS(pKvrHead);

        status = KVR_SUCCESS;
    } while (0);


    if (status == KVR_SUCCESS) {
        return size;
    } else {
        return status;
    }
}

int KVR_Load(uint16_t id, void *pdata, uint16_t size)
{
    int status = KVR_SUCCESS;
    uint8_t idx = 0;
    KVR_Item_t *pKvrItem = NULL;

    do {
        if ((pdata == NULL)
            || (size == 0)) {
            status = KVR_ERR_PARAM;
            LOG_ERR("input param err\n");
            break;
        }

        KVR_Head_t *pKvrHead = pKvrHeadCtrl[KVR_GET_TYPE(id)];
        id = KVR_GET_IDX(id);

        status = kvr_HeadCheck(pKvrHead, pKvrHead->base_addr);
        if (status != KVR_SUCCESS) {
            LOG_ERR("head check err(%d)\n", status);
            break;
        }

        status = kvr_FindItem(pKvrHead, id, &idx);
        if (status != KVR_SUCCESS) {
            LOG_ERR("find item err(%d)\n", status);
            break;
        }

        pKvrItem = kvr_GetItem(pKvrHead, idx);
        if (!kvr_GetItemPlCS(pKvrHead, pKvrItem)) {
            status = KVR_ERR_ITEM_PL_CS;
            LOG_ERR("find item payload checksum err(%d)\n", status);
            break;
        }

        if (size > pKvrItem->pl_size) {
            size = pKvrItem->pl_size;
        }

        LOG_DBG("\nidx(%d) ID(%d) item offset(%p):\n", idx, pKvrItem->id, pKvrItem);
        LOG_DBG("state: %X\n", pKvrItem->state);
        LOG_DBG("pl_offset: %04X\n", pKvrItem->pl_offset);
        LOG_DBG("pl_size: %X, req size: %d\n", pKvrItem->pl_size, size);
        LOG_DBG("pl_cs: %X\n", pKvrItem->pl_cs);
        LOG_DBG("item_cs: %X\n", pKvrItem->item_cs);

        kvr_GetItemPlValue(pKvrHead, pKvrItem, pdata, size);

        LOG_DBG_HEX(pdata, size, "get data:\n");
    } while (0);


    if (status == KVR_SUCCESS) {
        return size;
    } else {
        return status;
    }
}


int KVR_Delete(uint16_t id)
{
    int status = KVR_SUCCESS;
    uint8_t idx = 0;
    KVR_Item_t *pKvrItem = NULL;

    do {
        KVR_Head_t *pKvrHead = pKvrHeadCtrl[KVR_GET_TYPE(id)];
        id = KVR_GET_IDX(id);

        status = kvr_HeadCheck(pKvrHead, pKvrHead->base_addr);
        if (status != KVR_SUCCESS) {
            LOG_ERR("head check err(%d)\n", status);
            break;
        }

        status = kvr_FindItem(pKvrHead, id, &idx);
        if (status != KVR_SUCCESS) {
            LOG_ERR("find item err(%d)\n", status);
            break;
        }


        /* free payload */
        pKvrItem = kvr_GetItem(pKvrHead, idx);
        uint8_t lst_idx = kvr_GetItemMaxIdx(pKvrHead) - 1;
        uint16_t move_len = pKvrItem->pl_size;
        if (lst_idx != idx) {
            LOG_DBG("current item idx(%d), last item idx(%d)\n", idx, lst_idx);
            KVR_Item_t *pKvrItemLst = kvr_GetItem(pKvrHead, lst_idx);
            uint16_t move_size = pKvrItem->pl_offset - pKvrItemLst->pl_offset;
            uint8_t *pdst = kvr_GetItemPl(pKvrHead, pKvrItem) + move_len;
            uint8_t *psrc = kvr_GetItemPl(pKvrHead, pKvrItem);
            LOG_DBG("payload move: from(%p) to(%p) size(%d-%d)\n",
                psrc,
                pdst,
                move_size,
                move_len);
            for (uint32_t idx = 0; idx <= move_size; idx++) {
                *pdst-- = *psrc--;
            }

            /* clear last item move erea */
            /* nothing */
            KVR_Item_t *pKvrItemScan = NULL;
            for (uint8_t scan_idx = idx + 1; scan_idx <= lst_idx; scan_idx++) {
                pKvrItemScan = kvr_GetItem(pKvrHead, scan_idx);
                pKvrItemScan->pl_offset += move_len;
                kvr_SetItemCS(pKvrItemScan);
                kvr_CopyItem(pKvrHead, scan_idx - 1, scan_idx);
            }
        }

        kvr_DecItemIdx(pKvrHead);
        pKvrHead->free_offset += move_len;

        kvr_UpdateHeadListCS(pKvrHead);
    } while (0);

    return status;
}


static int kvr_FindItem(KVR_Head_t *pKvrHead, uint8_t id, uint8_t * pItemIidx)
{
    int status = KVR_ERR_ITEM_NOT_FOUND;
    KVR_Item_t * pKvrItemScan = NULL;

    for (uint8_t idx = 0; idx < kvr_GetItemMaxIdx(pKvrHead); idx++) {
        pKvrItemScan = kvr_GetItem(pKvrHead, idx);
        if (pKvrItemScan->id == id) {
            /* check item chekcsum */
            if (!kvr_GetItemCS(pKvrItemScan)) {
                status = KVR_ERR_ITEMS_CS;
                LOG_ERR("found id, but item checksum err\n");
                break;
            }

            if (pKvrItemScan->state != KVR_ITEM_STATE_ACTIVE) {
                status = KVR_ERR_ITEM_STATE;
                LOG_ERR("found id, but item state err\n");
                break;
            }

            if ((pKvrItemScan->pl_offset < kvr_PlEreaStartOffset(pKvrHead))
                || ((pKvrItemScan->pl_offset + pKvrItemScan->pl_size) > kvr_PlEreaEndOffset(pKvrHead))) {
                status = KVR_ERR_ITEM_PARAM;
                LOG_ERR("found id, but item payload param err. \pl_offset(%04X) pl_size(%04X)\n",
                    pKvrItemScan->pl_offset,
                    pKvrItemScan->pl_size,
                    kvr_PlEreaStartOffset(pKvrHead),
                    kvr_PlEreaEndOffset(pKvrHead));
                break;
            }

            *pItemIidx = idx;
            status = KVR_SUCCESS;
            LOG_DBG("found id %d\n", idx);
            break;
        }
    }


    return status;
}


static int kvr_HeadCheck(KVR_Head_t *pKvrHead, uint32_t Addr)
{
    int status = KVR_SUCCESS;

    do {
        if (pKvrHead == NULL) {
            status = KVR_ERR_STORE_ADDR;
            LOG_ERR("kvr_head is NULL\n");
            break;
        }

        if (pKvrHead->base_addr != (uint32_t)Addr) {
            status = KVR_ERR_STORE_ADDR;
            LOG_ERR("base addr(%08X) neque pramst_head(%08X)\n", pKvrHead->base_addr, (uint32_t)Addr);
            break;
        }

        if (pKvrHead->magic != KVR_MAGIC) {
            status = KVR_ERR_MAGIC;
            LOG_ERR("magic err(%08X)\n", pKvrHead->magic);
            break;
        }

        /* check head checksum */
        if (0 != kvr_CheckSum((uint8_t *)pKvrHead->base_addr, KVR_HEAD_SIZE)) {
            status = KVR_ERR_HEAD_CS;
            LOG_ERR("head checksum err\n");
            break;
        }

        if ((pKvrHead->free_offset > pKvrHead->store_size)
            || (pKvrHead->free_offset < kvr_PlEreaStartOffset(pKvrHead))) {
            status = KVR_ERR_PL_SIZE;
            LOG_ERR("free_offset err, free_offset(%04X) store_size(%04X) payload start(%04X)\n",
                pKvrHead->free_offset, pKvrHead->store_size, kvr_PlEreaStartOffset(pKvrHead));
            break;
        }

        /* check items list chcksum */
        if (pKvrHead->list_size) {
            if (pKvrHead->list_cs != kvr_CheckSum((uint8_t *)(kvr_ListEreaAddr(pKvrHead)), pKvrHead->list_size)) {
                status = KVR_ERR_LIST_CS;
                LOG_ERR("list checksum err\n");
                break;
            }
        }

        LOG_DBG("head checksum success\n");
    } while (0);


    return status;
}


static uint8_t kvr_CheckSum(void * pbuff, uint32_t size)
{
    uint8_t checksum = 0;
    uint8_t *pdata = (uint8_t *)pbuff;

    for (uint16_t idx = 0; idx < size; idx++) {
        checksum += pdata[idx];
    }

    return ~checksum;
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

