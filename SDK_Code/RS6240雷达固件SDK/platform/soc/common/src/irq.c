/**
 ******************************************************************************
 * @file    irq.c
 * @brief   irq define.
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
#include "hal_dev.h"
#include "ll_utils.h"
#include "irq.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define CONFIG_IRQ_HANDLER_MASK                         0x7FFFFFFF
#define CONFIG_IRQ_HANDLER_DEVICE                       0x80000000

/* Private variables.
 * ----------------------------------------------------------------------------
 */
static void * Irq_Table[CONFIG_CPU_IRQ_MAX];
static uint8_t Irq_SysId;
volatile uint32_t  Irq_NestedLevel;

static IRQ_Handler_t IrqPrevHandler;
static IRQ_Handler_t IrqPostHandler;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
extern void Default_Handler(void);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

void IRQ_Init(void)
{
    __disable_irq();

    Irq_SysId = csi_get_cpu_id();

    /* disable and clear software, core time and external interrupt */
    __set_MIE(0);
    __set_MIP(0);

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    for (int i = 0; i < CONFIG_CPU_IRQ_MAX; i++) {
        CLIC->CLICINT[i].IP = 0;
        CLIC->CLICINT[i].IE = 0;
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
    }

    /* tspend use positive interrupt */
    CLIC->CLICINT[Machine_Software_IRQn].ATTR = 0x3;
    csi_vic_enable_irq(Machine_Software_IRQn);


    for (int i = 0; i < CONFIG_CPU_IRQ_MAX; i++) {
        Irq_Table[i] = NULL;
    }

    __set_MIE(0xFFFFFFFF);

    __enable_irq();

}

HAL_Status_t IRQ_Attach(uint32_t irq_num, IRQ_Handler_t handler)
{
    if (handler == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Irq_Table[irq_num] = handler;

    return HAL_STATUS_OK;
}

HAL_Status_t IRQ_AttachDevice(uint32_t irq_num, HAL_Dev_t *pdevice)
{
    if (pdevice == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Irq_Table[irq_num] = (void *)(((uint32_t)pdevice) | CONFIG_IRQ_HANDLER_DEVICE);

    return HAL_STATUS_OK;
}

void IRQ_Detach(uint32_t irq_num)
{
    Irq_Table[irq_num] = NULL;
}

void IRQ_Enable(uint32_t irq_num)
{
    if (irq_num < CONFIG_CPU_IRQ_MAX) {
        if (Irq_SysId) {
            LL_INTR_EnableToCPUF(irq_num);
        }
        else {
            LL_INTR_EnableToCPUS(irq_num);
        }

        csi_vic_enable_irq((int32_t)irq_num);
    }
}

void IRQ_Disable(uint32_t irq_num)
{
    if (irq_num < CONFIG_CPU_IRQ_MAX) {
        if (Irq_SysId) {
            LL_INTR_DisableToCPUF(irq_num);
        }
        else {
            LL_INTR_DisableToCPUS(irq_num);
        }

        csi_vic_disable_irq((int32_t)irq_num);
    }
}

int IRQ_IsEnabled(uint32_t irq_num)
{
    if (irq_num < CONFIG_CPU_IRQ_MAX) {
        return csi_vic_get_enabled_irq(irq_num);
    } else {
        return 0;
    }
}

void IRQ_Priority(uint32_t irq_num, uint32_t priority)
{
    csi_vic_set_prio((int32_t)irq_num, priority);
}

int IRQ_Context(void)
{
    return ((Irq_NestedLevel > 0U) ? 1 : 0);
}

void IRQ_RegisterExtHandler(IRQ_Handler_t prevHandler, IRQ_Handler_t postHandler)
{
    IrqPrevHandler = prevHandler;
    IrqPostHandler = postHandler;
}


__keep_sram_text void do_irq(void)
{
    uint32_t irqn;

    Irq_NestedLevel++;

    if (IrqPrevHandler != NULL) {
        IrqPrevHandler(NULL);
    }


    irqn = __get_MCAUSE() & 0x3FFU;
    if (irqn < CONFIG_CPU_IRQ_MAX) {
        uint32_t table = (uint32_t)Irq_Table[irqn];
        uint32_t point = table & CONFIG_IRQ_HANDLER_MASK;
        IRQ_Handler_t Irq_Handler = (IRQ_Handler_t)point;

        if (table & CONFIG_IRQ_HANDLER_DEVICE) {
            HAL_Dev_t *pdevice = (HAL_Dev_t *)point;
            Irq_Handler = (IRQ_Handler_t)(pdevice->irqHandler);
        }

        if (Irq_Handler != NULL) {
            Irq_Handler((void *)point);
        }
    }


    if (IrqPostHandler != NULL) {
        IrqPostHandler(NULL);
    }

    Irq_NestedLevel--;
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

