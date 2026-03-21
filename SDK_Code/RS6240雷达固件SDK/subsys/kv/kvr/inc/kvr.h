/**
 ******************************************************************************
 * @file    hal_kvr.h
 * @brief   hal key value ram define.
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

#ifndef _KVR_H
#define _KVR_H

/* Includes.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "hal_types.h"


/* Exported types.
 * ----------------------------------------------------------------------------
 */
 /* Exported macro.
 * ----------------------------------------------------------------------------
 */
#define KVR_ID_TYPE_SYS                     (1 << 15)
#define KVR_ID_MSK(id)                      ((id) & 0x7FFF)

#define KVR_SYS_ID(id)                      (KVR_ID_MSK(id) | KVR_ID_TYPE_SYS)

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
#define KVR_SYS_ID_BOOT_IMG_INFO                        (1 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_MERGE_IMG_INFO                       (2 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_SYS0_IMG_INFO                        (3 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_SYS1_IMG_INFO                        (4 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_MMW_IMG_INFO                         (5 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_MMW_ALG_IMG_INFO                     (6 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_MPW_IMG_INFO                         (7 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_MMW_CUSTOM_CFG_IMG_INFO              (8 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_FLASH_LIST_IMG_INFO                  (9 | KVR_ID_TYPE_SYS)

#define KVR_SYS_ID_PWR_CLK_DUAL_CORE                    (11 | KVR_ID_TYPE_SYS)
#define KVR_SYS_ID_FLASH_DUAL_CORE                      (12 | KVR_ID_TYPE_SYS)

/* Private macro.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int KVR_Init(void);
int KVR_Save(uint16_t id, void *pdata, uint32_t size);
int KVR_Load(uint16_t id, void *pdata, uint32_t size);
int KVR_Delete(uint16_t id);

#ifdef __cplusplus
}
#endif

#endif /* _KVR_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

