/**
 ******************************************************************************
 * @file    task_sw.S
 * @brief   task switch define.
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


#include "arch_riscv_config.h"
#include "riscv_csr.h"

#if CONFIG_CPU_KERNEL

/* Enable interrupts when returning from the handler */
#define MSTATUS_PRV1 0x1880


.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/


.global cpu_intrpt_save
.type cpu_intrpt_save, %function
cpu_intrpt_save:
    csrr    a0, mstatus
    csrc    mstatus, 8
    ret

.global cpu_intrpt_restore
.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    csrw    mstatus, a0
    ret

.global cpu_is_irq_enable
.type cpu_is_irq_enable, %function
cpu_is_irq_enable:
    csrr a0, mstatus
    andi a0, a0, RISCV_MSTATUS_MIE
    ret

#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
.macro FREERTOS_RESTORE_MSTATUS
    li       t1, 0

#if CONFIG_CPU_DSP_ENABLE
    addi     t1, t1, 4
#endif

#if CONFIG_CPU_FPU_ENABLE
    addi     t1, t1, 132
#endif
    addi     t1, t1, (128-4)

    add      sp, sp, t1
    lw       t3, (0)(sp)
    csrw     mstatus, t3
    sub      sp, sp, t1
.endm
#endif /* CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE */



.global vPortYield
.type   vPortYield, %function
vPortYield:
    li      t0, 0xE080100C
    lb      t1, (t0)
    li      t2, 0x01
    or      t1, t1, t2
    sb      t1, (t0)

    /* FIXME: trriger soft-irq quickly */
    fence
    nop
    nop
    nop
    nop
    nop

    ret



.global vPortStartTask
.type   vPortStartTask, %function
vPortStartTask:
    j       __vPortFistTask



.global tspend_handler
.type   tspend_handler, %function
tspend_handler:
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    addi    sp, sp, -128
#else
    addi    sp, sp, -124
#endif
    sw      x1, 0(sp)
    sw      x3, 4(sp)
    sw      x4, 8(sp)
    sw      x5, 12(sp)
    sw      x6, 16(sp)
    sw      x7, 20(sp)
    sw      x8, 24(sp)
    sw      x9, 28(sp)
    sw      x10, 32(sp)
    sw      x11, 36(sp)
    sw      x12, 40(sp)
    sw      x13, 44(sp)
    sw      x14, 48(sp)
    sw      x15, 52(sp)
    sw      x16, 56(sp)
    sw      x17, 60(sp)
    sw      x18, 64(sp)
    sw      x19, 68(sp)
    sw      x20, 72(sp)
    sw      x21, 76(sp)
    sw      x22, 80(sp)
    sw      x23, 84(sp)
    sw      x24, 88(sp)
    sw      x25, 92(sp)
    sw      x26, 96(sp)
    sw      x27, 100(sp)
    sw      x28, 104(sp)
    sw      x29, 108(sp)
    sw      x30, 112(sp)
    sw      x31, 116(sp)
    csrr    t0, mepc
    sw      t0, 120(sp)
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    csrr    t3, mstatus
    sw      t3, 124(sp)
#endif

#if CONFIG_CPU_FPU_ENABLE
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    li      t1, SR_FS_DIRTY
    and     t3, t3, t1
    bne     t3, t1, 1f
#endif
    addi     sp, sp, -4
    frcsr    t0
    sw       t0, 0(sp)

    addi    sp, sp, -128
    fsw     f31, 0(sp)
    fsw     f30, 4(sp)
    fsw     f29, 8(sp)
    fsw     f28, 12(sp)
    fsw     f27, 16(sp)
    fsw     f26, 20(sp)
    fsw     f25, 24(sp)
    fsw     f24, 28(sp)
    fsw     f23, 32(sp)
    fsw     f22, 36(sp)
    fsw     f21, 40(sp)
    fsw     f20, 44(sp)
    fsw     f19, 48(sp)
    fsw     f18, 52(sp)
    fsw     f17, 56(sp)
    fsw     f16, 60(sp)
    fsw     f15, 64(sp)
    fsw     f14, 68(sp)
    fsw     f13, 72(sp)
    fsw     f12, 76(sp)
    fsw     f11, 80(sp)
    fsw     f10, 84(sp)
    fsw     f9, 88(sp)
    fsw     f8, 92(sp)
    fsw     f7, 96(sp)
    fsw     f6, 100(sp)
    fsw     f5, 104(sp)
    fsw     f4, 108(sp)
    fsw     f3, 112(sp)
    fsw     f2, 116(sp)
    fsw     f1, 120(sp)
    fsw     f0, 124(sp)
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    j       2f
1:
    addi    sp, sp, -132
2:
#endif
#endif /* CONFIG_CPU_FPU_ENABLE */

#if CONFIG_CPU_DSP_ENABLE
    addi    sp, sp, -4
    csrr    t0, vxsat
    sw      t0, 0(sp)
#endif /*CONFIG_CPU_DSP_ENABLE */

    la      a1, pxCurrentTCB
    lw      a1, (a1)
    sw      sp, (a1)

    # jal     vTaskSwitchContext
    call    vTaskSwitchContext

__vPortFistTask:
    la      a1, pxCurrentTCB
    lw      a1, (a1)
    lw      sp, (a1)

#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    FREERTOS_RESTORE_MSTATUS
#endif

#if CONFIG_CPU_DSP_ENABLE
    lw      t0, 0(sp)
    csrw    vxsat, t0
    addi    sp, sp, 4
#endif /*CONFIG_CPU_DSP_ENABLE */

#if CONFIG_CPU_FPU_ENABLE
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    li      t1, SR_FS_DIRTY
    and     t3, t3, t1
    bne     t3, t1, 11f
#endif
    flw     f31, 0(sp)
    flw     f30, 4(sp)
    flw     f29, 8(sp)
    flw     f28, 12(sp)
    flw     f27, 16(sp)
    flw     f26, 20(sp)
    flw     f25, 24(sp)
    flw     f24, 28(sp)
    flw     f23, 32(sp)
    flw     f22, 36(sp)
    flw     f21, 40(sp)
    flw     f20, 44(sp)
    flw     f19, 48(sp)
    flw     f18, 52(sp)
    flw     f17, 56(sp)
    flw     f16, 60(sp)
    flw     f15, 64(sp)
    flw     f14, 68(sp)
    flw     f13, 72(sp)
    flw     f12, 76(sp)
    flw     f11, 80(sp)
    flw     f10, 84(sp)
    flw     f9, 88(sp)
    flw     f8, 92(sp)
    flw     f7, 96(sp)
    flw     f6, 100(sp)
    flw     f5, 104(sp)
    flw     f4, 108(sp)
    flw     f3, 112(sp)
    flw     f2, 116(sp)
    flw     f1, 120(sp)
    flw     f0, 124(sp)
    addi    sp, sp, 128

    lw       t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    j        12f
11:
    addi     sp, sp, 132
12:
#endif /* CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE */
#endif /* CONFIG_CPU_FPU_ENABLE */

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    lw      t0, 120(sp)
    csrw    mepc, t0

    lw      x1, 0(sp)
    lw      x3, 4(sp)
    lw      x4, 8(sp)
    lw      x5, 12(sp)
    lw      x6, 16(sp)
    lw      x7, 20(sp)
    lw      x8, 24(sp)
    lw      x9, 28(sp)
    lw      x10, 32(sp)
    lw      x11, 36(sp)
    lw      x12, 40(sp)
    lw      x13, 44(sp)
    lw      x14, 48(sp)
    lw      x15, 52(sp)
    lw      x16, 56(sp)
    lw      x17, 60(sp)
    lw      x18, 64(sp)
    lw      x19, 68(sp)
    lw      x20, 72(sp)
    lw      x21, 76(sp)
    lw      x22, 80(sp)
    lw      x23, 84(sp)
    lw      x24, 88(sp)
    lw      x25, 92(sp)
    lw      x26, 96(sp)
    lw      x27, 100(sp)
    lw      x28, 104(sp)
    lw      x29, 108(sp)
    lw      x30, 112(sp)
    lw      x31, 116(sp)
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    addi    sp, sp, 128
#else
    addi    sp, sp, 124
#endif
    mret

    .size   tspend_handler, . - tspend_handler

#endif /* CONFIG_CPU_KERNEL */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
