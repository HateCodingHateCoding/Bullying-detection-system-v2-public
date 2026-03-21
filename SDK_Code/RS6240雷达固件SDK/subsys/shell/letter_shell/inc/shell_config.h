
/**
 ******************************************************************************
 * @file    shell_config.h
 * @brief   shell config define.
 ******************************************************************************
 * @author Letter (nevermindzzt@gmail.com)
 * @version 3.0.0
 * @date 2019-12-31
 * 
 * @copyright (c) 2019 Letter
 *
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

#ifndef _SHELL_CONFIG_H
#define _SHELL_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */
/* Exported constants.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_SHELL
#define CONFIG_SHELL                            0
#endif

#ifndef CONFIG_SHELL_TASK_STACK_SIZE
#define CONFIG_SHELL_TASK_STACK_SIZE            (1024 * 2)
#endif

#ifndef CONFIG_SHELL_TASK_PRIORITY
#define CONFIG_SHELL_TASK_PRIORITY              4
#endif

#ifndef CONFIG_SHELL_UART
#define CONFIG_SHELL_UART                       1
#endif

#ifndef CONFIG_SHELL_UART_NUM
#define CONFIG_SHELL_UART_NUM                   1
#endif

#ifndef CONFIG_SHELL_UART_BAUDRATE
#define CONFIG_SHELL_UART_BAUDRATE              115200
#endif

#ifndef CONFIG_SHELL_RECV_TIMEOUT
#define CONFIG_SHELL_RECV_TIMEOUT               500
#endif

#ifndef CONFIG_SHELL_BUFF_SIZE
#define CONFIG_SHELL_BUFF_SIZE                  512
#endif

#ifndef CONFIG_SHELL_PM
#define CONFIG_SHELL_PM                         0
#endif

#ifndef CONFIG_SHELL_PM_LOCK_TO
#define CONFIG_SHELL_PM_LOCK_TO                10000  //  ms as unit
#endif

#ifndef SHELL_TASK_WHILE
#define SHELL_TASK_WHILE                        0
#endif

#ifndef SHELL_USING_CMD_EXPORT
#define SHELL_USING_CMD_EXPORT                  1
#endif

#ifndef SHELL_USING_COMPANION
#define SHELL_USING_COMPANION                   0
#endif

#ifndef SHELL_SUPPORT_END_LINE
#define SHELL_SUPPORT_END_LINE                  0
#endif

#ifndef SHELL_HELP_LIST_USER
#define SHELL_HELP_LIST_USER                    0
#endif

#ifndef SHELL_HELP_LIST_VAR
#define SHELL_HELP_LIST_VAR                     0
#endif

#ifndef SHELL_HELP_LIST_KEY
#define SHELL_HELP_LIST_KEY                     0
#endif

#ifndef SHELL_HELP_SHOW_PERMISSION
#define SHELL_HELP_SHOW_PERMISSION              0
#endif

#ifndef SHELL_ENTER_LF
#define SHELL_ENTER_LF                          1
#endif

#ifndef SHELL_ENTER_CR
#define SHELL_ENTER_CR                          1
#endif

#ifndef SHELL_ENTER_CRLF
#define SHELL_ENTER_CRLF                        0
#endif

#ifndef SHELL_EXEC_UNDEF_FUNC
#define SHELL_EXEC_UNDEF_FUNC                   0
#endif

#ifndef SHELL_PARAMETER_MAX_NUMBER
#define SHELL_PARAMETER_MAX_NUMBER              12
#endif

#ifndef SHELL_HISTORY_MAX_NUMBER
#define SHELL_HISTORY_MAX_NUMBER                5
#endif

#ifndef SHELL_DOUBLE_CLICK_TIME
#define SHELL_DOUBLE_CLICK_TIME                 200
#endif

#ifndef SHELL_QUICK_HELP
#define SHELL_QUICK_HELP                        1
#endif

#ifndef SHELL_KEEP_RETURN_VALUE
#define SHELL_KEEP_RETURN_VALUE                 0
#endif

#ifndef SHELL_MAX_NUMBER
#define SHELL_MAX_NUMBER                        1
#endif

#ifndef SHELL_PRINT_BUFFER
#define SHELL_PRINT_BUFFER                      128
#endif

#ifndef SHELL_SCAN_BUFFER
#define SHELL_SCAN_BUFFER                       128
#endif

#ifndef SHELL_GET_TICK
#define SHELL_GET_TICK()                        0
#endif

#ifndef SHELL_USING_LOCK
#define SHELL_USING_LOCK                        1
#endif

#ifndef SHELL_MALLOC
#define SHELL_MALLOC(size)                      0
#endif

#ifndef SHELL_FREE
#define SHELL_FREE(obj)                         0
#endif

#ifndef SHELL_SHOW_INFO
#define SHELL_SHOW_INFO                         0
#endif

#ifndef SHELL_CLS_WHEN_LOGIN
#define SHELL_CLS_WHEN_LOGIN                    0
#endif

#ifndef SHELL_DEFAULT_USER
#define SHELL_DEFAULT_USER                      "psic"
#endif

#ifndef SHELL_DEFAULT_USER_PASSWORD
#define SHELL_DEFAULT_USER_PASSWORD             ""
#endif

#ifndef SHELL_LOCK_TIMEOUT
#define SHELL_LOCK_TIMEOUT                      0
#endif

#ifndef SHELL_USING_FUNC_SIGNATURE
#define SHELL_USING_FUNC_SIGNATURE              0
#endif

#ifndef SHELL_SUPPORT_ARRAY_PARAM
#define SHELL_SUPPORT_ARRAY_PARAM               0
#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _SHELL_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
