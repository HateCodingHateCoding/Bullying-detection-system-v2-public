/**
 ******************************************************************************
 * @file    trap_c.c
 * @brief   tracp c define.
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <arch_riscv_config.h>

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_CPU_TRAP_LOG_ENABLE
#include <soc.h>
#define TRAP_LOG    printf
#else
#define TRAP_LOG(...)
#endif


/* Exported functions.
 * ----------------------------------------------------------------------------
 */

 __attribute__((weak)) void ExceptionCallback(void)  // NOLINT
{
    /* default add reset system */

}

#if CONFIG_CPU_TRAP_LOG_ENABLE

static char *cause_str(unsigned long cause)
{
    switch (cause) {
    case 0:
        return "Instruction address misaligned";
    case 1:
        return "Instruction Access fault";
    case 2:
        return "Illegal instruction";
    case 3:
        return "Breakpoint";
    case 4:
        return "Load address misaligned";
    case 5:
        return "Load access fault";
    case 6:
        return "Store/AMO address misaligned";
    case 7:
        return "Store/AMO access fault";
    case 8:
        return "Environment call from U-mode";
    case 9:
        return "Environment call from S-mode";
    case 11:
        return "Environment call from M-mode";
    default:
        return "unknown";
    }
}

void Trap_Log(uintptr_t *regs)
{
    int i;
    unsigned int vec = 0;

    vec = __get_MCAUSE();

    TRAP_LOG("\n");
    TRAP_LOG("\n");
    TRAP_LOG("CPU Exception; %d %s", (vec & 0xFFFF), cause_str(vec & 0xFFFF));
    TRAP_LOG("\n");

    TRAP_LOG("x1  ra: %p\t", (void *)regs[0]);
    TRAP_LOG("x2  sp: %p\t", (void *)regs[1]);
    TRAP_LOG("x3  gp: %p\t", (void *)regs[2]);
    TRAP_LOG("x4  tp: %p\t", (void *)regs[3]);
    TRAP_LOG("\n");
    TRAP_LOG("x5  t0: %p\t", (void *)regs[4]);
    TRAP_LOG("x6  t1: %p\t", (void *)regs[5]);
    TRAP_LOG("x7  t2: %p\t", (void *)regs[6]);
    TRAP_LOG("x8  s0: %p\t", (void *)regs[7]);
    TRAP_LOG("\n");
    TRAP_LOG("x9  s1: %p\t", (void *)regs[8]);
    TRAP_LOG("x10 a0: %p\t", (void *)regs[9]);
    TRAP_LOG("x11 a1: %p\t", (void *)regs[10]);
    TRAP_LOG("x12 a2: %p\t", (void *)regs[11]);
    TRAP_LOG("\n");
    TRAP_LOG("x13 a3: %p\t", (void *)regs[12]);
    TRAP_LOG("x14 a4: %p\t", (void *)regs[13]);
    TRAP_LOG("x15 a5: %p\t", (void *)regs[14]);
    TRAP_LOG("x16 a6: %p\t", (void *)regs[15]);
    TRAP_LOG("\n");
    TRAP_LOG("x17 a7: %p\t", (void *)regs[16]);
    TRAP_LOG("x18 s2: %p\t", (void *)regs[17]);
    TRAP_LOG("x19 s3: %p\t", (void *)regs[18]);
    TRAP_LOG("x20 s4: %p\t", (void *)regs[19]);
    TRAP_LOG("\n");
    TRAP_LOG("x21 s5: %p\t", (void *)regs[20]);
    TRAP_LOG("x22 s6: %p\t", (void *)regs[21]);
    TRAP_LOG("x23 s7: %p\t", (void *)regs[22]);
    TRAP_LOG("x24 s8: %p\t", (void *)regs[23]);
    TRAP_LOG("\n");
    TRAP_LOG("x25 s9: %p\t", (void *)regs[24]);
    TRAP_LOG("x26 s10: %p\t", (void *)regs[25]);
    TRAP_LOG("x27 s11: %p\t", (void *)regs[26]);
    TRAP_LOG("x28 t3: %p\t", (void *)regs[27]);
    TRAP_LOG("\n");
    TRAP_LOG("x29 t4: %p\t", (void *)regs[28]);
    TRAP_LOG("x30 t5: %p\t", (void *)regs[29]);
    TRAP_LOG("x31 t6: %p\t", (void *)regs[30]);
    TRAP_LOG("\n");


    TRAP_LOG("\n\n");
    TRAP_LOG("mepc   : %p\n", (void *)regs[31]);
    TRAP_LOG("mstatus: %p\n", (void *)regs[32]);
    TRAP_LOG("mcause : %p\n", (void *)regs[33]);
    TRAP_LOG("mtval  : %p\n", (void *)regs[34]);
    TRAP_LOG("\n");

    unsigned int stack_addr = regs[1];
    unsigned int *stack_value = (void *)regs[1];  // NOLINT
    for (i = 0; i < CONFIG_CPU_TRAP_LOG_STACK_SIZE;) {
        TRAP_LOG("%08X : ", stack_addr);
        TRAP_LOG("%08X %08X %08X %08X %08X %08X %08X %08X\n",
            stack_value[i], stack_value[i + 1],
            stack_value[i + 2], stack_value[i + 3],
            stack_value[i + 4], stack_value[i + 5],
            stack_value[i + 6], stack_value[i + 7]);

        i += 8;
        stack_addr += 8;
    }
}
#endif /* CONFIG_CPU_TRAP_LOG_ENABLE */


void ExceptionHandler(void *context)
{
#if CONFIG_CPU_TRAP_LOG_ENABLE
    Trap_Log((uintptr_t *)context);  // NOLINT
#endif

    ExceptionCallback();

    while (1) {
    }
}

void NmiExceptionHandler(void)
{
    ExceptionCallback();

    while (1) {
    }
}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
