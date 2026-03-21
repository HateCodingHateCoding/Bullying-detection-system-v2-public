/**
 ******************************************************************************
 * @file    hif checksume.c
 * @brief   hif checksume define.
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
#include "hif_log.h"

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

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

__hif_sram_text uint8_t HIF_CheckSum8(uint8_t initValue, uint8_t* data, int dataLen)
{
    register uint8_t checksum8 = initValue;
    while (dataLen--) {
        checksum8 += *data++;
    }

    return checksum8;
}

uint16_t HIF_CheckSum16(uint16_t initValue, uint16_t* data, int dataLen)
{
    register uint16_t checksum16 = initValue;
    register int word_len = (dataLen >> 1);
    while (word_len--) {
        checksum16 += *data++;
    }
    if (dataLen & 0x1) {
        checksum16 += *(uint8_t*)data;
    }

    return checksum16;
}

__hif_sram_text uint32_t HIF_CheckSum32(uint32_t initValue, uint32_t* data, int dataLen)
{
    register uint32_t checksum32 = initValue;
    register int dword_len = (dataLen >> 2);
    while (dword_len--) {
        checksum32 += *data++;
    }
    dataLen = (dataLen & 0x3);
    if (dataLen) {
        uint32_t mask = (0x00FFFFFF >> ((3 - dataLen) << 3));
        checksum32 += *data & mask;
    }

    return checksum32;
}


#define CPU_ADDR_UNALIGN_SUPPORT    1
__hif_sram_text uint32_t checksum32_calc(uint8_t *data, uint32_t len, uint32_t *offset)
{
    register uint32_t result = 0;
    register uint32_t off = *offset & 0x1F;
    register uint32_t dword_len;

#if (CPU_ADDR_UNALIGN_SUPPORT)
    /* offset is not aligned4 */
    while (off && len) {
        result += ((uint32_t)(*data++)) << off;
        off = (off + 8) & 0x1F;
        len--;
    }

#else
    /* address or offset is not aligned4 */
    while (((data & 0x3) | off) && len) {
        result += (*data++) << off;
        off = (off + 8) & 0x1F;
        len--;
    }
#endif

    /* address and len is aligned4 */
    dword_len = (len >> 2);
    while (dword_len--) {
        result += *(uint32_t *)data;
        data += 4;
    }

    /* len not aligned4 */
    len = (len & 0x3);
    if (len) {
        result += *(uint32_t *)data & ((0x1U<<(len<<3)) - 1);
        off += (len<<3);
    }

    *offset = off;
    return result;
}


#endif /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
