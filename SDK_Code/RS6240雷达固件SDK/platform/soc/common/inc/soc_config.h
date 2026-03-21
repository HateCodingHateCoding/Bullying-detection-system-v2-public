/**
 ******************************************************************************
 * @file    soc_config.h
 * @brief   soc config define.
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

#ifndef _SOC_CONFIG_H
#define _SOC_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "board_config.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_SOC_RS6130
#define CONFIG_SOC_RS6130                               0
#endif

#ifndef CONFIG_SOC_RS6240_CPUS
#define CONFIG_SOC_RS6240_CPUS                          0
#endif

#ifndef CONFIG_SOC_RS6240_CPUF
#define CONFIG_SOC_RS6240_CPUF                          0
#endif

#ifndef CONFIG_SOC_RS6241_CPUS
#define CONFIG_SOC_RS6241_CPUS                          0
#endif

#ifndef CONFIG_SOC_RS6241_CPUF
#define CONFIG_SOC_RS6241_CPUF                          0
#endif

#ifndef CONFIG_SOC_RS6242_CPUS
#define CONFIG_SOC_RS6242_CPUS                          0
#endif

#ifndef CONFIG_SOC_RS6242_CPUF
#define CONFIG_SOC_RS6242_CPUF                          0
#endif

#ifndef CONFIG_SOC_RS7241_CPUS
#define CONFIG_SOC_RS7241_CPUS                          0
#endif

#ifndef CONFIG_SOC_RS7241_CPUF
#define CONFIG_SOC_RS7241_CPUF                          0
#endif

#ifndef CONFIG_SOC_DUAL_CORE
#define CONFIG_SOC_DUAL_CORE                            0
#endif

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

#if (CONFIG_SOC_RS6240_CPUS || CONFIG_SOC_RS6240_CPUF)
#define CONFIG_SOC_RS6240                               1
#else
#define CONFIG_SOC_RS6240                               0
#endif

#if (CONFIG_SOC_RS6241_CPUS || CONFIG_SOC_RS6241_CPUF)
#define CONFIG_SOC_RS6241                               1
#else
#define CONFIG_SOC_RS6241                               0
#endif

#if (CONFIG_SOC_RS6242_CPUS || CONFIG_SOC_RS6242_CPUF)
#define CONFIG_SOC_RS6242                               1
#else
#define CONFIG_SOC_RS6242                               0
#endif

#if (CONFIG_SOC_RS7241_CPUS || CONFIG_SOC_RS7241_CPUF)
#define CONFIG_SOC_RS7241                               1
#else
#define CONFIG_SOC_RS7241                               0
#endif

#if (CONFIG_SOC_RS6130)
#define CONFIG_SOC_SERIES_RS613X                        1
#else
#define CONFIG_SOC_SERIES_RS613X                        0
#endif

#if (CONFIG_SOC_RS6240 || CONFIG_SOC_RS6241 || CONFIG_SOC_RS6242)
#define CONFIG_SOC_SERIES_RS624X                        1
#else
#define CONFIG_SOC_SERIES_RS624X                        0
#endif

#if (CONFIG_SOC_RS7241)
#define CONFIG_SOC_SERIES_RS724X                        1
#else
#define CONFIG_SOC_SERIES_RS724X                        0
#endif

#if (CONFIG_SOC_SERIES_RS613X || CONFIG_SOC_SERIES_RS624X)
#define CONFIG_SOC_SERIES_RS6XXX                        1
#else
#define CONFIG_SOC_SERIES_RS6XXX                        0
#endif

#if (CONFIG_SOC_SERIES_RS724X)
#define CONFIG_SOC_SERIES_RS7XXX                        1
#else
#define CONFIG_SOC_SERIES_RS7XXX                        0
#endif


#if (CONFIG_SOC_RS6130 || CONFIG_SOC_RS6240_CPUS || CONFIG_SOC_RS6241_CPUS ||\
    CONFIG_SOC_RS6242_CPUS || CONFIG_SOC_RS7241_CPUS)
#define CONFIG_SOC_CPU_S                                1
#else
#define CONFIG_SOC_CPU_S                                0
#endif

#if (CONFIG_SOC_RS6240_CPUF || CONFIG_SOC_RS6241_CPUF || CONFIG_SOC_RS6242_CPUF || CONFIG_SOC_RS7241_CPUF)
#define CONFIG_SOC_CPU_F                                1
#else
#define CONFIG_SOC_CPU_F                                0
#endif

#if (CONFIG_SOC_DUAL_CORE == 1) && (CONFIG_SOC_RS6130 == 1)
#error "The rs6130 does not support dual-core."
#endif

#if ((CONFIG_SOC_RS6130 + CONFIG_SOC_RS6240_CPUS + CONFIG_SOC_RS6240_CPUF +\
    CONFIG_SOC_RS6241_CPUS + CONFIG_SOC_RS6241_CPUF +\
    CONFIG_SOC_RS6242_CPUS + CONFIG_SOC_RS6242_CPUF +\
    CONFIG_SOC_RS7241_CPUS + CONFIG_SOC_RS7241_CPUF) > 1)
#error "Mutiple socs cannot be selected at the same time."
#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_SOC_RS6130
#include "rs6130_config.h"
#elif (CONFIG_SOC_RS6240)
#include "rs6240_config.h"
#elif (CONFIG_SOC_RS6241)
#include "rs6241_config.h"
#elif (CONFIG_SOC_RS6242)
#include "rs6242_config.h"
#elif (CONFIG_SOC_RS7241)
#include "rs7241_config.h"
#else
#error "No soc has been selected !!!"
#endif

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _SOC_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
