/**
 ******************************************************************************
 * @file    sleep.S
 * @brief   sleep define.
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
#include "arch_riscv_config.h"
#include "riscv_csr.h"

/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#define CPUS_BOOT_FLAG          0x40019070
#define CPUS_BOOT_ADDR          0x40019074
#define CPUS_BOOT_ARG           0x40019078
#define CPUF_BOOT_FLAG          0x40019090
#define CPUF_BOOT_ADDR          0x40019094
#define CPUF_BOOT_ARG           0x40019098
#define CPU_FBOOT_ADDR          0x40019040

#define CPU_ID_CPUS             0x120C2000
#define BOOT_FLAG_MASK          0x3
#define BOOT_FLAG_WUP           0x1



.section .text
    .globl   sys_sleep
    .globl   sys_sleep_notify
    .weak   sys_sleep_notify
    .text
    .align   2

sys_sleep_notify:
    ret

sys_sleep:
    bnez a0, deep_sleep

light_sleep:
    csrr a2, mexstatus
    ori a3, a2, 4
    csrw mexstatus, a3
    wfi
    nop
    nop
    ret

/* deep sleep
 * a0: sleep mode: 1
 * a1: save address
 */
deep_sleep:

store_gpr:
    sw x1,  0(a1)
    sw x2,  4(a1)
    sw x3,  8(a1)
    sw x4,  12(a1)
    sw x5,  16(a1)
    sw x6,  20(a1)
    sw x7,  24(a1)
    sw x8,  28(a1)
    sw x9,  32(a1)
    sw x10, 36(a1)
    sw x11, 40(a1)
    sw x12, 44(a1)
    sw x13, 48(a1)
    sw x14, 52(a1)
    sw x15, 56(a1)
    sw x16, 60(a1)
    sw x17, 64(a1)
    sw x18, 68(a1)
    sw x19, 72(a1)
    sw x20, 76(a1)
    sw x21, 80(a1)
    sw x22, 84(a1)
    sw x23, 88(a1)
    sw x24, 92(a1)
    sw x25, 96(a1)
    sw x26, 100(a1)
    sw x27, 104(a1)
    sw x28, 108(a1)
    sw x29, 112(a1)
    sw x30, 116(a1)
    sw x31, 120(a1)


store_csr:
    csrr a0, mstatus
    sw a0, 260(a1)

    csrr a0, mtvec
    sw a0, 264(a1)

    csrr a0, mxstatus
    sw a0, 268(a1)

    csrr a0, mhcr
    sw a0, 272(a1)

    csrr a0, mexstatus
    sw a0, 276(a1)

    csrr a0, mtvt
    sw a0, 280(a1)

    /* other, mie, mscratch, mnxti, mscratchcsw, mscratchcsl, mip */

    /* save contex */
suspend:
    /* select cpu id */
    csrr t0, mcpuid
    csrr t1, mcpuid
    csrr t0, mcpuid

    li t2, CPU_ID_CPUS
    bne t2, t1, cpuf_suspend

cpus_suspend:
    /* set bootflag */
    li t0, CPUS_BOOT_FLAG
    li t1, BOOT_FLAG_WUP
    sw t1, (t0)

    /* set resume address */
    li t0, CPUS_BOOT_ADDR
    la t1, resume_start
    sw t1, (t0)

    /* store cpu contex */
    li t0, CPUS_BOOT_ARG
    sw a1, (t0)

    j suspend_end

cpuf_suspend:
    /* set bootflag */
    li t0, CPUF_BOOT_FLAG
    li t1, BOOT_FLAG_WUP
    sw t1, (t0)

    /* set resume address */
    li t0, CPUF_BOOT_ADDR
    la t1, resume_start
    sw t1, (t0)

    /* store cpu contex */
    li t0, CPUF_BOOT_ARG
    sw a1, (t0)

suspend_end:
    /* clear and invalid all d cache */
    dcache.ciall

    csrr t0, mexstatus
    li t1, 0x301F3
    and t0, t0, t1
    csrw mexstatus, t0
    
    /* check interrupt status to save time */
    csrr a0, mnxti
    bnez a0, resume_start

    li a0, 1
    jal ra, sys_sleep_notify

    /* ensure to goto wfi */
    li t0, 0xe0800008
    li t1, 0xFF000000
    sw t1, (t0)
    fence
    fence
    
    wfi
    nop
    nop

resume_start:
    /*
    jal ra, __system_map
    */

    /* select cpu */
    csrr t0, mcpuid
    csrr t1, mcpuid
    csrr t0, mcpuid

    li t2, CPU_ID_CPUS
    bne t2, t1, cpuf_resume

cpus_resume:
    /* remove boot flag */
    li t0, CPUS_BOOT_FLAG
    sw x0, (t0)

    /* restore cpu contex */
    li t0, CPUS_BOOT_ARG
    lw a1, (t0)

    j resume

cpuf_resume:
    /* remove boot flag */
    li t0, CPUF_BOOT_FLAG
    sw x0, (t0)

    /* restore cpu contex */
    li t0, CPUF_BOOT_ARG
    lw a1, (t0)

resume:

resume_csr:
    /* mstatus */
    lw a0, 260(a1)
    csrw mstatus, a0

    /* mtvec */
    lw a0, 264(a1)
    csrw mtvec, a0

    /* mxstatus */
    lw a0, 268(a1)
    csrw mxstatus, a0

    /* mhcr */
    lw a0, 272(a1)
    csrw mhcr, a0

    /* mexstatus */
    lw a0, 276(a1)
    ori t0, a0, 4
    csrw mexstatus, t0

    lw a0, 280(a1)
    csrw mtvt, a0
    /* other, mie, mscratch, mnxti, mscratchcsw, mscratchcsl, mip */

resume_gpr:
    lw x1,  0(a1)
    lw x2,  4(a1)
    lw x3,  8(a1)
    lw x4,  12(a1)
    lw x5,  16(a1)
    lw x6,  20(a1)
    lw x7,  24(a1)
    lw x8,  28(a1)
    lw x9,  32(a1)
    lw x10, 36(a1)
    lw x11, 40(a1)
    lw x12, 44(a1)
    lw x13, 48(a1)
    lw x14, 52(a1)
    lw x15, 56(a1)
    lw x16, 60(a1)
    lw x17, 64(a1)
    lw x18, 68(a1)
    lw x19, 72(a1)
    lw x20, 76(a1)
    lw x21, 80(a1)
    lw x22, 84(a1)
    lw x23, 88(a1)
    lw x24, 92(a1)
    lw x25, 96(a1)
    lw x26, 100(a1)
    lw x27, 104(a1)
    lw x28, 108(a1)
    lw x29, 112(a1)
    lw x30, 116(a1)
    lw x31, 120(a1)

    ret


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
