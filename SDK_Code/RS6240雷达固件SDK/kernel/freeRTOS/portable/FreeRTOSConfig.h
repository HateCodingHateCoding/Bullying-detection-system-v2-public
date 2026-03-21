/**
 ******************************************************************************
 * @file    FreeRTOSConfig.h
 * @brief   FreeRTOS config define.
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

#ifndef _FREERTOS_CONFIG_H
#define _FREERTOS_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "kernel_config.h"
#include "stdio.h"

 #ifdef __cplusplus
extern "C" {
#endif

extern uint32_t os_system_freq;


#define configUSE_PREEMPTION                                        1//
#define configUSE_PORT_OPTIMISED_TASK_SELECTION                     0//
#define configUSE_TICKLESS_IDLE                                     1//0->1
#define configCPU_CLOCK_HZ                                          os_system_freq//60000000->system_core_clock
// #define configSYSTICK_CLOCK_HZ                                      1000000
#define configTICK_RATE_HZ                                          CONFIG_SYSTICK_HZ//250->1000
#define configMAX_PRIORITIES                                        16//5->
#define configMINIMAL_STACK_SIZE                                    256//
#define configMAX_TASK_NAME_LEN                                     16//
#define configUSE_16_BIT_TICKS                                      0//
#define configIDLE_SHOULD_YIELD                                     0//1->0
#define configUSE_TASK_NOTIFICATIONS                                1//
#define configTASK_NOTIFICATION_ARRAY_ENTRIES                       1//3->0
#define configUSE_MUTEXES                                           1//0->1
#define configUSE_RECURSIVE_MUTEXES                                 1//0->1
#define configUSE_COUNTING_SEMAPHORES                               1//0->1
// #define configUSE_ALTERNATIVE_API                                   0 /* Deprecated! */
#define configQUEUE_REGISTRY_SIZE                                   0//10->0
#define configUSE_QUEUE_SETS                                        0//xradio 1
#define configUSE_TIME_SLICING                                      1//0->1
#define configUSE_NEWLIB_REENTRANT                                  0//
#define configENABLE_BACKWARD_COMPATIBILITY                         0//
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS                     1//5->1
// #define configUSE_MINI_LIST_ITEM                                    1//no
#define configSTACK_DEPTH_TYPE                                      uint16_t//uint16_t --> uint32_t
#define configMESSAGE_BUFFER_LENGTH_TYPE                            size_t
// #define configHEAP_CLEAR_MEMORY_ON_FREE                             1//x

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION                             0//1->0
#define configSUPPORT_DYNAMIC_ALLOCATION                            1//
#define configTOTAL_HEAP_SIZE                                       10240 //?
#define configAPPLICATION_ALLOCATED_HEAP                            1//?
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP                   1//?

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                                 1//0->1
#define configUSE_TICK_HOOK                                 0//
#define configCHECK_FOR_STACK_OVERFLOW                      1//0->1
#define configUSE_MALLOC_FAILED_HOOK                        1//0->1
#define configUSE_DAEMON_TASK_STARTUP_HOOK                  0//
#define configUSE_SB_COMPLETED_CALLBACK                     0//

/* Run time and task stats gathering related definitions. */
#if CONFIG_KERNEL_DEBUG
#define configGENERATE_RUN_TIME_STATS                       1//
#define configUSE_TRACE_FACILITY                            1//
#define configUSE_STATS_FORMATTING_FUNCTIONS                1//
#else
#define configGENERATE_RUN_TIME_STATS                       0//
#define configUSE_TRACE_FACILITY                            0//
#define configUSE_STATS_FORMATTING_FUNCTIONS                0//
#endif

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                               0//
#define configMAX_CO_ROUTINE_PRIORITIES                     1//

/* Software timer related definitions. */
#define configUSE_TIMERS                                    1//
#define configTIMER_TASK_PRIORITY                           1//3->1
#define configTIMER_QUEUE_LENGTH                            10//
#define configTIMER_TASK_STACK_DEPTH                        configMINIMAL_STACK_SIZE//

/* Interrupt nesting behaviour configuration. */
// #define configKERNEL_INTERRUPT_PRIORITY         [dependent of processor]
// #define configMAX_SYSCALL_INTERRUPT_PRIORITY    [dependent on processor and application]
// #define configMAX_API_CALL_INTERRUPT_PRIORITY   [dependent on processor and application]

/* Define to trap errors during development. */
// #define configASSERT( ( x ) ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

/* FreeRTOS MPU specific definitions. */
// #define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0
// #define configTOTAL_MPU_REGIONS                                8 /* Default value */
// #define configTEX_S_C_B_FLASH                                  0x07UL /* Default value */
// #define configTEX_S_C_B_SRAM                                   0x07UL /* Default value */
// #define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY            1
// #define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS             1
// #define configENABLE_ERRATA_837070_WORKAROUND                  1

/* ARMv8-M secure side port related definitions. */
// #define secureconfigMAX_SECURE_CONTEXTS         5

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_uxTaskGetStackHighWaterMark2    0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   0//1?
#define INCLUDE_xTimerPendFunctionCall          0//1?
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskGetHandle                  0
#define INCLUDE_xTaskResumeFromISR              1


#define portHAS_STACK_OVERFLOW_CHECKING         1


#ifdef __cplusplus
}
#endif

#endif /* _FREERTOS_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
