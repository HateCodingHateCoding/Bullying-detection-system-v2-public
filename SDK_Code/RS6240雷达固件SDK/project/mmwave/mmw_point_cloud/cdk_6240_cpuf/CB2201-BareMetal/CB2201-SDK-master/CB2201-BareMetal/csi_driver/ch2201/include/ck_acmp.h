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
 * @file     ck_acmp.h
 * @brief    header file for acmp driver
 * @version  V1.0
 * @date     02. June 2018
 ******************************************************************************/
#ifndef _CK_ACMP_H_
#define _CK_ACMP_H_

#include <stdint.h>
#include <soc.h>


#define ACMP_CR_FAST_MODE                   (1 << 3)        /* Fast Mode Control, Logic High Valid */
#define ACMP_CR_HYSTERESIS_ENBALE           (1 << 2)        /* Analog Comparator Hysteresis Enable Control */
#define ACMP_CR_IRQ_ENBALE                  (1 << 1)        /* Analog Comparator Interrupt Enable Control  */
#define ACMP_CR_ENBALE                      (1 << 0)        /* Analog Comparator Enable Control  */


#define ACMP_SR_CMPF                        (1 << 1)        /* Analog Comparator Flag */
#define ACMP_SR_CMPO                        (1 << 0)        /* Analog Comparator Output */

typedef struct {
    __IOM uint32_t ACMP_CR;                     /* Offset: 0x000 (W/R) Analog Comparator Control Register */
    __IOM uint32_t ACMP_SR;                     /* Offset: 0x004 (W/R) Analog Comparator Status Register*/
    __IOM uint32_t ACMP_RPTC;                   /* Offset: 0x008 (W/R) Analog Comparator Response Time Counter  */
    __IOM uint32_t ACMP_ISR;                    /* Offset: 0x00c (W/R) Analog Comparator Interrupt Status Register */
} ck_acmp_reg_t;

#endif /* _CK_ACMP_H_ */
