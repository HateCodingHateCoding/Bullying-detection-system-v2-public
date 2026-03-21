/**
 ******************************************************************************
 * @file    startup.S
 * @brief   startup define.
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
#include <arch_riscv_config.h>

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

.section .text
    .globl   Reset_Handler
    .text
    .align   2

    /* disable global interrupt */
    csrc    mstatus, 8

    j        Reset_Handler
    .align   2

Reset_Handler:
.option push
.option norelax
    la      gp, __global_pointer$

.option pop

    /* config csr - mxstatus
     * switch to machine mode - bit 30-31
     * enable private extension instruction - bit 22
     * enable unaligned access - bit 15
     */
    li t0, 0xC0408000
    csrw mxstatus,t0

    /* config csr - mexstatus
     * disable deep sleep mode - bit 2-3
     * disable wait for event mode - bit 4
     * disable interrupt auto push stack - bit 16
     * disable interrupt stack and thread stack auto switch - bit 17
     */
    li t0, 4
    csrw mexstatus,t0

    /* config csr - mhcr
     * enable BTB
     * enable BTE
     * disable RS(return stack)
     * disable WA
     * disable WB
     * disable DE
     * disable IE
     */
    li t0, 0x102C
    csrw mhcr, t0

    /*
     * disable MIE - bit 3
     * disable MPIS - bit 7
     * disable MPP - bit 11 - 12
     * enable FS - bit 13 - 14 (01)
     * enable XS - bit 15 - 16 (11)
     * disable MPRV - bit 17
     */
#if CONFIG_CPU_FPU_ENABLE
    li t0, 0x1A000
    csrw mstatus, t0
#else
    li t0, 0
    csrw mstatus, t0
#endif


    la      a0, Default_Handler
    ori     a0, a0, 3
    csrw    mtvec, a0

    la      a0, __Vectors
    csrw    mtvt, a0

    la      sp, g_top_irqstack
    csrw    mscratch, sp
#if CONFIG_CPU_BAREMATE
    la      sp, g_top_mainstack
#endif

    call    System_Init

    .size   Reset_Handler, . - Reset_Handler

__exit:
    j      __exit


.section .stack
    .align  4
    .global g_base_irqstack
    .global g_top_irqstack
g_base_irqstack:
    .space CONFIG_CPU_IRQ_STACK_SIZE
g_top_irqstack:
#if CONFIG_CPU_BAREMATE
    .align  4
    .global g_base_mainstack
    .global g_top_mainstack
g_base_mainstack:
    .space CONFIG_CPU_BAREMATE_MAIN_STACK_SIZE
g_top_mainstack:
#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
