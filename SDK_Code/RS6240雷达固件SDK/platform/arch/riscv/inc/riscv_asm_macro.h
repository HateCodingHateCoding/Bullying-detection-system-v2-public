 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * attention: don't modify this file as a suggest
 * you should copy from chip_riscv_dummy/include/asm/riscv_asm_macro.h and keep it newer
 * please contact xuantie-rtos os team if have question 
 */

#ifndef _RISCV_ASM_MACRO_H
#define _RISCV_ASM_MACRO_H


#include "arch_riscv_config.h"
#include "riscv_csr.h"



#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
.macro RESTORE_MSTATUS
    /* t0 and t1 are not restored before using */
    /* now, sp is at the top of the stack (the lowest address)*/
    li       t1, 0

#if CONFIG_CPU_FPU_ENABLE
    addi     t1, t1, 84
#endif



#if CONFIG_CPU_DSP_ENABLE  /* vxsat register, 32-bit cpu only */
    addi     t1, t1, 4
#endif

    addi     t1, t1, 72

    add      sp, sp, t1

    /* now, sp is the position of mstatus */
    load_x   t3, (0)(sp)
    csrw     mstatus, t3
    sub      sp, sp, t1
.endm

.macro RESTORE_SSTATUS
    /* t0 and t1 are not restored before using */
    /* now, sp is at the top of the stack (the lowest address)*/
    li       t1, 0

#if CONFIG_CPU_FPU_ENABLE
    addi     t1, t1, 84
#endif

    addi     t1, t1, 72
    add      sp, sp, t1

    /* now, sp is the position of mstatus */
    load_x   t3, (0)(sp)
    csrw     sstatus, t3
    sub      sp, sp, t1
.endm

#endif /* CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE || CONFIG_CHECK_VECTOR_DIRTY */

.macro SAVE_VECTOR_REGISTERS
    /* t0,t1 saved before using */
    /* mstatus->t3 */
.endm

.macro RESTORE_VECTOR_REGISTERS
    /* t0,t1,t2 not restored before using, mstatus has been restored before using */
.endm


.macro SAVE_FLOAT_REGISTERS
    /* t0, t1 saved before using */
#if CONFIG_CPU_FPU_ENABLE
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    /* check if FS filed of MSTATUS is 'dirty' */
    li       t1, SR_FS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 1f
#endif  /* CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE */

    addi     sp, sp, -4
    frcsr    t0
    store_x  t0, 0(sp)

    addi     sp,  sp, -80
    fstore_x ft0, 0(sp)
    fstore_x ft1, 4(sp)
    fstore_x ft2, 8(sp)
    fstore_x ft3, 12(sp)
    fstore_x ft4, 16(sp)
    fstore_x ft5, 20(sp)
    fstore_x ft6, 24(sp)
    fstore_x ft7, 28(sp)
    fstore_x fa0, 32(sp)
    fstore_x fa1, 36(sp)
    fstore_x fa2, 40(sp)
    fstore_x fa3, 44(sp)
    fstore_x fa4, 48(sp)
    fstore_x fa5, 52(sp)
    fstore_x fa6, 56(sp)
    fstore_x fa7, 60(sp)
    fstore_x ft8, 64(sp)
    fstore_x ft9, 68(sp)
    fstore_x ft10,72(sp)
    fstore_x ft11,76(sp)
#endif
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    j        2f
1:
    /* don't store, move sp only */
#if CONFIG_CPU_FPU_ENABLE
    addi     sp, sp, -84
#endif
2:
#endif
.endm

.macro RESTORE_FLOAT_REGISTERS
    /* t0 and t1 are not restored before using, mstatus has been restored before using */
#if CONFIG_CPU_FPU_ENABLE
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    /* check if FS filed of MSTATUS is 'dirty' */
    li       t1, SR_FS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 1f
#endif /* CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE */

    /* restore */
    fload_x  ft0, 0(sp)
    fload_x  ft1, 4(sp)
    fload_x  ft2, 8(sp)
    fload_x  ft3, 12(sp)
    fload_x  ft4, 16(sp)
    fload_x  ft5, 20(sp)
    fload_x  ft6, 24(sp)
    fload_x  ft7, 28(sp)
    fload_x  fa0, 32(sp)
    fload_x  fa1, 36(sp)
    fload_x  fa2, 40(sp)
    fload_x  fa3, 44(sp)
    fload_x  fa4, 48(sp)
    fload_x  fa5, 52(sp)
    fload_x  fa6, 56(sp)
    fload_x  fa7, 60(sp)
    fload_x  ft8, 64(sp)
    fload_x  ft9, 68(sp)
    fload_x  ft10,72(sp)
    fload_x  ft11,76(sp)
    addi     sp, sp, 80

    load_x   t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#endif
#if CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE
    j        2f
1:
    /* don't restore, move sp only */
#if CONFIG_CPU_FPU_ENABLE
    addi     sp, sp, 84
#endif
2:
#endif /* CONFIG_CPU_CHECK_FPU_DIRTY_ENABLE */
.endm


#endif /* _RISCV_ASM_MACRO_H */

