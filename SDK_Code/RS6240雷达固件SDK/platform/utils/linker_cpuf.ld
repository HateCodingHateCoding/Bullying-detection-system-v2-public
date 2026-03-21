/**
 ******************************************************************************
 * @file    linker_cpuf.ld
 * @brief   linker cpuf define.
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

/*
 * sys 0: 
 *          isram: 0050 0000
 *          dsram: 1050 0000
 *          ixip : 0090 0000
 * sys 1: 
 *          isram: 0050 0000
 *          dsram: 1050 0000
 *          ixip : 0094 0000
 *
 * mmw ctr:
 *          isram: 0058 0000
 *          dsram: 1058 0000
 *          ixip : 009C 0000
 *
 * mmw alg: 
 *          isram: 0054 0000
 *          dsram: 1054 0000
 *          ixip : 0098 0000
 *
 */

MEMORY
{
    ISRAM  : ORIGIN = 0x00500000 , LENGTH = 0x40000   /* 256 KB */
    DSRAM  : ORIGIN = 0x10500000 , LENGTH = 0x40000   /* 256 KB */
    IXIP   : ORIGIN = 0x09400000 , LENGTH = 0x40000   /* IXIP   256 KB */
    IPSRAM : ORIGIN = 0x0D200000 , LENGTH = 0x40000   /* IPSRAM 256 KB */
    DPSRAM : ORIGIN = 0x1D200000 , LENGTH = 0x40000   /* DPSRAM 256 KB */
}



ENTRY(Reset_Handler)
SECTIONS
{
    head_pad : {
        . = ALIGN(32);
        __s_head_pad = . ;
        LONG(0xFFFFFFFF)
        LONG(0xFFFFFFFF)
        LONG(0xFFFFFFFF)
        LONG(0xFFFFFFFF)
        LONG(0xFFFFFFFF)
        LONG(0xFFFFFFFF)
        LONG(0xFFFFFFFF)
        LONG(0xFFFFFFFF)
        __e_head_pad = . ;
    } > IXIP

    isram_text : {
        . = ALIGN(0x4) ;
        __s_isram_text = . ;

        /* startup (eraly) */
        __s_startup_text = . ;
        KEEP(*startup.o(*.text))
        KEEP(*vectors.o(*.vectors))
        KEEP(*trap.o(*.text))
        *startup_c.o(*.text*)

        /* startup (post) */
        *soc.o(*.text*)
        *irq.o(*.text*)
        __e_startup_text = . ;

        /* kernel (freeRTOS) */
        __s_os_text = . ;
        *portable_port.o(*.text*)
        *portable_port_riscv.o(*.text*)
        *heap_4.o(*.text*)
        *list.o(*.text*)
        *queue.o(*.text*)
        *tasks.o(*.text*)
        *timers.o(*.text*)
        *stream_buffer.o(*.text*)
        *event_group.o(*.text*)
        *croutune.o(*.text*)
        
        /* osi (memory, mutex, port, queue, semphore, thread, timer) */
        *osi_*.o(*.text*)
        __e_os_text = . ;

        /* libc and libgcc */
        __s_libs_text = . ;
        *libc.a:(.text.atoi .text.__errno .text.malloc .text.free .text.memcmp)
        *libc.a:lib_a-memcpy-asm.o(*.text*)
        *libc.a:lib_a-memset.o(*.text*)
        *libgcc.a:_ashrdi3.o(*.text*)
        *libgcc.a:_floatundisf.o(*.text*)
        *libgcc.a:_udivdi3.o(*.text*)
        *libgcc.a:adddf3.o(*.text*)
        *libgcc.a:muldf3.o(*.text*)
        *libgcc.a:floatunsidf.o(*.text*)
        *libgcc.a:truncdfsf2.o(*.text*)
        *libgcc.a:unaligned-libcall.o(*.text*)
        *libgcc.a:_clzsi2.o(*.text*)
        *libdsp.a:(*.text*)
        *libm.a:(*.text*)

        *libc_newlib_stub*.o(*.text*)
        __e_libs_text = . ;
        
        /* mmw */
        __s_mmw_algo_text = . ;

        *mmw_algorithm*.o(*.text*)
        *mmw_application*.o(*.text*)
        *test_mmw*.o(*.text*)
        /* mmw_cmd */
        *mmw_hif.o(*.text*)
        *mmw_report.o(*.text*)
        /* mmw_ctrl */
        *mmw_ctrl.o(*.text*)
        *mmw_dev.o(*.text*)
        *mmw_low_power.o(*.text*)
        *mmw_pal.o(*.text*)
        *mmw_temp_mgmt.o(*.text*)
        /* mmw_mdsp */
        *mmw_mdsp*.o(*.text*)
        /* mmw_psic_lib */
        *mmw_psic_lib*.o(*.text*)
        
        __e_mmw_algo_text = . ;

        /* pm port */
        __s_pm_text = . ;
        *sleep.o(*.text*)
        *pmu_tmr.o(*.text*)
        *pm_device.o(*.text*)
        *pm_device_runtime.o(*.text*)
        *pm_pm.o(*.text*)
        *pm_policy.o(*.text*)
        *port_port.o(*.text*)
        __e_pm_text = . ;

        /* other divers */
        __s_driver_text = . ;
        *hal_board.o(*.text*)
        *hal_flash.o(*.text*)
        __e_driver_text = . ;

        /* hif pack */
        __s_hif_text = . ;
        *hif*.o(*.text*)
		__e_hif_text = . ;

		__s_sram_text = .;
        KEEP(*(.keep_sram_text));
        *(.sram_text);
        __e_sram_text = .;

        . = ALIGN(0x4) ;
        __e_isram_text = . ;
    } > ISRAM

    isram_rodata : {
        . = ALIGN(0x4) ;
        __s_isram_rodata = . ;

        /* startup (eraly) */
        __s_startup_rodata = .;
        KEEP(*trap.o(.rodata* .srodata*))
        *trap*.o(.rodata* .srodata*)
        *startup_c.o(.rodata* .srodata*)

        /* startup (post) */
        *soc.o(.rodata* .srodata*)
        *irq.o(.rodata* .srodata*)
        __e_startup_rodata = . ;

        /* kernel (freeRTOS) */
        __s_os_rodata = .;
        *portable_port.o(.rodata* .srodata*)
        *portable_port_riscv.o(.rodata* .srodata*)
        *heap_4.o(.rodata* .srodata*)
        *list.o(.rodata* .srodata*)
        *queue.o(.rodata* .srodata*)
        *tasks.o(.rodata* .srodata*)
        *timers.o(.rodata* .srodata*)
        *stream_buffer.o(.rodata* .srodata*)
        *event_group.o(.rodata* .srodata*)
        *croutune.o(.rodata* .srodata*)

        /* osi (memory, mutex, port, queue, semphore, thread, timer) */
        *osi_*.o(.rodata* .srodata*)
        __e_os_rodata = . ;

        /* libc and libgcc */
        __s_libs_rodata = .;
        *libc.a:(.rodata* .srodata*)
        *libgcc.a:_ashrdi3.o(.rodata* .srodata*)
        *libgcc.a:_floatundisf.o(.rodata* .srodata*)
        *libgcc.a:_udivdi3.o(.rodata* .srodata*)
        *libgcc.a:adddf3.o(.rodata* .srodata*)
        *libgcc.a:muldf3.o(.rodata* .srodata*)
        *libgcc.a:floatunsidf.o(.rodata* .srodata*)
        *libgcc.a:truncdfsf2.o(.rodata* .srodata*)
        *libgcc.a:unaligned-libcall.o(.rodata* .srodata*)
        *libgcc.a:_clzsi2.o(.rodata* .srodata*)
        *libdsp.a:(.rodata* .srodata*)
        *libm.a:(.rodata* .srodata*)
        __e_libs_rodata = . ;

        /* mmw */
        __s_mmw_algo_rodata = . ;

        *mmw_algorithm*.o(.rodata* .srodata*)
        *mmw_application*.o(.rodata* .srodata*)
        *test_mmw*.o(.rodata* .srodata*)
        /* mmw_cmd */
        *mmw_hif.o(.rodata* .srodata*)
        *mmw_report.o(.rodata* .srodata*)
        /* mmw_ctrl */
        *mmw_ctrl.o(.rodata* .srodata*)
        *mmw_dev.o(.rodata* .srodata*)
        *mmw_low_power.o(.rodata* .srodata*)
        *mmw_pal.o(.rodata* .srodata*)
        *mmw_temp_mgmt.o(.rodata* .srodata*)
        /* mmw_mdsp */
        *mmw_mdsp*.o(.rodata* .srodata*)
        /* mmw_psic_lib */
        *mmw_psic_lib*.o(.rodata* .srodata*)
        __e_mmw_algo_rodata = . ;

        /* pm port */
        __s_pm_rodata = . ;
        *sleep.o(.rodata* .srodata*)
        *pmu_tmr.o(.rodata* .srodata*)
        *pm_device.o(.rodata* .srodata*)
        *pm_device_runtime.o(.rodata* .srodata*)
        *pm_pm.o(.rodata* .srodata*)
        *pm_policy.o(.rodata* .srodata*)
        *port_port.o(.rodata* .srodata*)
        __e_pm_rodata = . ;

        /* other divers */
        __s_driver_rodata = . ; 
        *hal_board.o(.rodata* .srodata*)
        *hal_flash.o(.rodata* .srodata*)
        __e_driver_rodata = .;

		__s_sram_rodata = .;
        KEEP(*(.keep_sram_rodata));
        *(.sram_rodata);
        __e_sram_rodata = .;

        . = ALIGN(0x4) ;
        __e_isram_rodata = . ;
    } > ISRAM

    ipsram_text : {
        . = ALIGN(0x4) ;
        __s_ipsram_text = .;

        . = ALIGN(0x4) ;
        __e_ipsram_text = .;
    } > IPSRAM

    ixip_text : {
        . = ALIGN(0x4) ;
        __s_ixip_text = . ;

        *(.text*)

        /* If the shell not used, the relevant code cannot be compiled.
         * Otherwise, it will take up storage space.
         */
        . = ALIGN(0x4) ;
		_shell_command_start = .;
        KEEP(*(shellCommand));
        _shell_command_end = .;
		. = ALIGN(0x4) ;
        __e_ixip_text = .;
    } > IXIP

    ixip_rodata : {
        . = ALIGN(0x4);
        __s_ixip_rodata = . ;

        *(.rodata* .srodata*)

        /* If the mtest not used, the relevant code cannot be compiled.
         * Otherwise, it will take up storage space.
         */
        . = ALIGN(0x4) ;
        _mtest_expected_result_entry_list_start = .;
        KEEP(*(SORT_BY_NAME(._mtest_expected_result_entry.static.*)));
        _mtest_expected_result_entry_list_end = .;

        _mtest_suite_node_list_start = .;
        KEEP(*(._iterables_mtest_suite_node.static));
        _mtest_suite_node_list_end = .;
        
        _mtest_unit_test_list_start = .;
        KEEP(*(._iterables_mtest_unit_test.static));
        _mtest_unit_test_list_end = .;

        _mtest_test_rule_list_start = .;
        KEEP(*(SORT_BY_NAME(._mtest_test_rule.static.*)));
        _mtest_test_rule_list_end = .;

        . = ALIGN(0x4) ;
        __e_ixip_rodata = . ;
    } > IXIP

    dsram_data : {
        . = ALIGN(0x4) ;
        __s_dsram_data = . ;
        __sdata = . ;
        __data_start__ = . ;
        *(.data*)
        __global_pointer$ = .;
        *(.sdata)

        . = ALIGN(0x4) ;
        __e_dsram_data = . ;
        __edata = .;
        __data_end__ = .;
    } > DSRAM

    dsram_bss : {
        . = ALIGN(0x4) ;
        __s_dsram_bss = . ;
        __sbss = ALIGN(0x4) ;
        __bss_start__ = . ;

        KEEP(*trap.o(*.bss))
        *(.sbss)
        *(.bss*)

        . = ALIGN(0x4) ;
        __e_dsram_bss = . ;
        __ebss = . ;
        __bss_end__ = .;
        __end = . ;
        end = . ;
    } > DSRAM


    dsram_stack (NOLOAD): {
        . = ALIGN(0x4) ;
        __s_dsram_stack = .;
        __stack_start = .;
        *(.stack*)
        __stack_end = .;
        . = ALIGN(0x4) ;
        __heap_start = .;
        KEEP (*(.heap));
        /*__heap_end = .;*/
        . = ALIGN(0x4) ;
        __e_dsram_stack = .;
    } > DSRAM

    ipsram_rodata : {
        . = ALIGN(0x4) ;
        __s_ipsram_rodata = . ;

        . = ALIGN(0x4) ;
        __e_ipsram_rodata = . ;
    } > IPSRAM

    dpsram_data : {
        . = ALIGN(0x4) ;
        __s_ipsram_data = . ;

        . = ALIGN(0x4) ;
        __e_ipsram_data = . ;
    } > DPSRAM

    dpsram_bss : {
        . = ALIGN(0x4) ;
        __s_ipsram_bss = . ;

        . = ALIGN(0x4) ;
        __e_ipsram_bss = . ;
    } > DPSRAM

    /* for image packet */
    .info : {
        . = ALIGN(0x1) ;
        KEEP (*(.cfg.info));
    }

}

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
