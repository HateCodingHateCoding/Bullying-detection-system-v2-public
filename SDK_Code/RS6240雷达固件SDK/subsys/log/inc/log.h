/**
 ******************************************************************************
 * @file    log.h
 * @brief   log define.
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

#ifndef _LOG_H
#define _LOG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "log_config.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_LOG
#define CONFIG_LOG                                  0
#endif

#ifndef CONFIG_LOG_HEAD
#define CONFIG_LOG_HEAD                             1
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL                                   0
#endif


#ifndef CONFIG_LOG_IO_TAG_MASK
#define CONFIG_LOG_IO_TAG_MASK                      0
#endif

#ifndef CONFIG_LOG_IO_MASK
#define CONFIG_LOG_IO_MASK                          0
#endif

/* Exported constants.
 * ----------------------------------------------------------------------------
 */


#if (CONFIG_LOG != 0)

#define LOG_PRINT(fmt, arg...)      printf(fmt, ##arg)

#define LOG_HEX(data, len, fmt, arg...)                         \
        do {                                                    \
            printf(fmt, ##arg);                                 \
            uint8_t *pbuf = (uint8_t *)data;                    \
            printf("(%d)\n", len);                              \
            for (uint32_t idx = 0; idx < len; idx++) {          \
                printf("%02X ", pbuf[idx]);                     \
                if ((idx % 16) == 15) {                         \
                    printf("\n");                               \
                }                                               \
            }                                                   \
            printf("\n");                                       \
        } while (0)

#define LOG_WATCH_POINT()               LOG_PRINT("%s %d\n", __func__, __LINE__)

#else

#define LOG_PRINT(...)

#define LOG_HEX(data, len, str)

#define LOG_WATCH_POINT()

#endif



#if (LOG_LEVEL > 0)

#define _LOG_PRINT(fmt, arg...)                                 \
        do {                                                    \
            printf(fmt, ##arg);                                 \
            printf("\n");                                       \
        } while (0)

#define _LOG_HEX(data, len, fmt, arg...)                        \
        do {                                                    \
            printf(fmt, ##arg);                                 \
            uint8_t *pbuf = (uint8_t *)data;                    \
            printf("(%d)\n", len);                              \
            for (uint32_t idx = 0; idx < len; idx++) {          \
                printf("%02X ", pbuf[idx]);                     \
                if ((idx % 16) == 15) {                         \
                    printf("\n");                               \
                }                                               \
            }                                                   \
            printf("\n");                                       \
        } while (0)

#else

#define _LOG_PRINT(...)
#define _LOG_HEX(data, len, str)

#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */



#if CONFIG_LOG_HEAD

#ifdef LOG_MODULE
#define ERR_STR                     "[ERR "LOG_MODULE"] "
#define WRN_STR                     "[WRN "LOG_MODULE"] "
#define INF_STR                     "[INF "LOG_MODULE"] "
#define DBG_STR                     "[DBG "LOG_MODULE"] "
#else
#define ERR_STR                     "[ERR] "
#define WRN_STR                     "[WRN] "
#define INF_STR                     "[INF] "
#define DBG_STR                     "[DBG] "
#endif

#else /* CONFIG_LOG_HEAD */

#define ERR_STR
#define WRN_STR
#define INF_STR
#define DBG_STR

#endif /* CONFIG_LOG_HEAD */



#if (LOG_LEVEL >= LEVEL_ERR)
#define _LOG_ERR(fmt, arg...)                    _LOG_PRINT(ERR_STR fmt, ##arg)
#define _LOG_ERR_HEX(data, len, fmt, arg...)     _LOG_HEX(data, len, ERR_STR fmt, ##arg)
#else
#define _LOG_ERR(fmt, arg...)
#define _LOG_ERR_HEX(data, len, fmt, arg...)
#endif

#if (LOG_LEVEL >= LEVEL_WRN)
#define _LOG_WRN(fmt, arg...)                    _LOG_PRINT(WRN_STR fmt, ##arg)
#define _LOG_WRN_HEX(data, len, fmt, arg...)     _LOG_HEX(data, len, WRN_STR fmt, ##arg)
#else
#define _LOG_WRN(fmt, arg...)
#define _LOG_WRN_HEX(data, len, fmt, arg...)
#endif

#if (LOG_LEVEL >= LEVEL_INF)
#define _LOG_INF(fmt, arg...)                    _LOG_PRINT(INF_STR fmt, ##arg)
#define _LOG_INF_HEX(data, len, fmt, arg...)     _LOG_HEX(data, len, INF_STR fmt, ##arg)
#else
#define _LOG_INF(fmt, arg...)
#define _LOG_INF_HEX(data, len, fmt, arg...)
#endif

#if (LOG_LEVEL >= LEVEL_DBG)
#define _LOG_DBG(fmt, arg...)                    _LOG_PRINT(DBG_STR fmt, ##arg)
#define _LOG_DBG_HEX(data, len, fmt, arg...)     _LOG_HEX(data, len, DBG_STR fmt, ##arg)
#else
#define _LOG_DBG(fmt, arg...)
#define _LOG_DBG_HEX(data, len, fmt, arg...)
#endif



#define LOG_ERR(fmt, arg...)                    _LOG_ERR(fmt, ##arg)
#define LOG_ERR_HEX(data, len, fmt, arg...)     _LOG_ERR_HEX(data, len, fmt, ##arg)
#define LOG_WRN(fmt, arg...)                    _LOG_WRN(fmt, ##arg)
#define LOG_WRN_HEX(data, len, fmt, arg...)     _LOG_WRN_HEX(data, len, fmt, ##arg)
#define LOG_INF(fmt, arg...)                    _LOG_INF(fmt, ##arg)
#define LOG_INF_HEX(data, len, fmt, arg...)     _LOG_INF_HEX(data, len, fmt, ##arg)
#define LOG_DBG(fmt, arg...)                    _LOG_DBG(fmt, ##arg)
#define LOG_DBG_HEX(data, len, fmt, arg...)     _LOG_DBG_HEX(data, len, fmt, ##arg)


/* Private macro.
 * ----------------------------------------------------------------------------
 */
#define LOG_IO_FLIP_TOGGLE                          0xFF


#define LOG_IO_TAG_PM                               (1 << 0)


#define LOG_IO_TAG_SCH                              (1 << 31)

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

#if (CONFIG_LOG_IO_TAG_MASK && CONFIG_LOG_IO_MASK)
extern void LOG_IO_INIT(uint32_t tag, uint32_t mask);
extern void LOG_IO_RESUME(void);
extern void LOG_IO(uint32_t tag, uint8_t num, uint8_t val, uint8_t flip);
#else
#define LOG_IO_INIT(tag, mask)
#define LOG_IO_RESUME()
#define LOG_IO(tag, num, val, flip)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LOG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
