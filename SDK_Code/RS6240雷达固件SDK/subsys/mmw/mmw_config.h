/**
 ******************************************************************************
 * @file    mmw_config.h
 * @brief   mmw config define.
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

#ifndef _MMW_CONFIG_H
#define _MMW_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include "soc_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* mmw ctrl
 */
#ifndef CONFIG_MMW_CTRL
#define CONFIG_MMW_CTRL                                 0
#endif

#ifndef CONFIG_MMW_CTRL_DEVELOP
#define CONFIG_MMW_CTRL_DEVELOP                         0
#endif

/* 1 : mmw ctrl image run at sram
 * 0 : mmw ctrl image run at sram and xip
 */
#ifndef CONFIG_MMW_CTRL_SRAM
#define CONFIG_MMW_CTRL_SRAM                            0
#endif

#ifndef CONFIG_MMW_IMG_NAME
#if (CONFIG_MMW_MOTION_SENSOR == 1)
#if (CONFIG_MMW_CTRL_SRAM == 1)
#define CONFIG_MMW_IMG_NAME                             "motion_sensor_sram"
#else
#define CONFIG_MMW_IMG_NAME                             "motion_sensor"
#endif
#else
#if (CONFIG_MMW_CTRL_SRAM == 1)
#define CONFIG_MMW_IMG_NAME                             "mmw_ctrl_sram"
#else
#define CONFIG_MMW_IMG_NAME                             "mmw_ctrl"
#endif
#endif
#endif

#ifndef CONFIG_MMW_IMG_PATH

#if (CONFIG_SOC_RS6130 == 1)
#define CONFIG_MMW_IMG_PATH                             "mmw/mmw_ctrl/rs6130"
#elif (CONFIG_SOC_RS6240_CPUS || CONFIG_SOC_RS6240_CPUF)
#define CONFIG_MMW_IMG_PATH                             "mmw/mmw_ctrl/rs6240"
#elif (CONFIG_SOC_RS7241_CPUS || CONFIG_SOC_RS7241_CPUF)
#define CONFIG_MMW_IMG_PATH                             "mmw/mmw_ctrl/rs7241"
#else
#error "Not define CONFIG_MMW_IMG_PATH"
#endif

#endif



/* mmw alg 
 */
#ifndef CONFIG_MMW_ALG
#define CONFIG_MMW_ALG                                  0
#endif

#ifndef CONFIG_MMW_MICRO_POINT_CLOUD
#define CONFIG_MMW_MICRO_POINT_CLOUD                    0
#endif

#if (CONFIG_MMW_ALG == 1)
#ifndef CONFIG_MMW_ALG_DEVELOP
#define CONFIG_MMW_ALG_DEVELOP                          0
#endif

#ifndef CONFIG_MMW_ALG_POINT_CLOUD
#define CONFIG_MMW_ALG_POINT_CLOUD                      0
#endif

#ifndef CONFIG_MMW_ALG_IMG_NAME
#if (CONFIG_MMW_ALG_POINT_CLOUD == 1)
#define CONFIG_MMW_ALG_IMG_NAME                         "point_cloud"
#else
#error "Not define CONFIG_MMW_ALG_IMG_NAME"
#endif
#endif

#ifndef CONFIG_MMW_ALG_IMG_PATH
#if (CONFIG_MMW_ALG_POINT_CLOUD == 1)

#if (CONFIG_SOC_RS6130 == 1)
#define CONFIG_MMW_ALG_IMG_PATH                         "mmw/mmw_alg/point_cloud/rs6130"
#elif (CONFIG_SOC_RS6240_CPUS || CONFIG_SOC_RS6240_CPUF)
#define CONFIG_MMW_ALG_IMG_PATH                         "mmw/mmw_alg/point_cloud/rs6240"
#elif (CONFIG_SOC_RS7241_CPUS || CONFIG_SOC_RS7241_CPUF)
#define CONFIG_MMW_ALG_IMG_PATH                         "mmw/mmw_alg/point_cloud/rs7241"
#else
#error "Not define CONFIG_MMW_ALG_IMG_PATH"
#endif

#else
#error "Not define CONFIG_MMW_ALG_IMG_PATH"
#endif
#endif

#endif


/* Exported functions.
 * ----------------------------------------------------------------------------
 */


#ifdef __cplusplus
}
#endif

#endif /* _MMW_CONFIG_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
