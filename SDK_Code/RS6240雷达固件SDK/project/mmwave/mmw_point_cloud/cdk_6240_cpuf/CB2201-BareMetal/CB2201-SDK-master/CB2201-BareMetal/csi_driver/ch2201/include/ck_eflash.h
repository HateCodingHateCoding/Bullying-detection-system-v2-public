/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     ck_eflash.h
 * @brief    head file for ck eflash
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_EFLASH_H_
#define _CK_EFLASH_H_

#include "drv_eflash.h"
#include "soc.h"

#define EFLASH_SECTOR_SIZE  0x200
#define EFLASH_ERASED_VALUE 0xff
#define EFLASH_PROGRAM_UINT 0x4
#define EFLASH_PAGE_SIZE    0

typedef struct {
    uint32_t RESERVED1[1];
    __IOM uint32_t ADDR;                 /* Offset: 0x004 (R/W) operate addr register */
    uint32_t RESERVED2[2];
    __IOM uint32_t PE;                   /* Offset: 0x010 (R/W) page erase enable register */
    uint32_t RESERVED3[1];
    __IOM uint32_t WE;                   /* Offset: 0x018 (R/W) writer enable register */
    __IOM uint32_t WDATA;                /* Offset: 0x01c (R/W) writer data register */
} ck_eflash_reg_t;

#endif
