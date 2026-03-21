/**
 ******************************************************************************
 * @brief   board_config define.
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


#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "prj_config.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

#ifndef CONFIG_BOARD_MRS6130_P1806
#define CONFIG_BOARD_MRS6130_P1806                      0
#endif

#ifndef CONFIG_BOARD_MRS6130_P1812
#define CONFIG_BOARD_MRS6130_P1812                      0
#endif

#ifndef CONFIG_BOARD_MRS6240_P2512_CPUS
#define CONFIG_BOARD_MRS6240_P2512_CPUS                 0
#endif

#ifndef CONFIG_BOARD_MRS6240_P2512_CPUF
#define CONFIG_BOARD_MRS6240_P2512_CPUF                 0
#endif

#ifndef CONFIG_BOARD_MRS7241_P2828_CPUS
#define CONFIG_BOARD_MRS7241_P2828_CPUS                 0
#endif

#ifndef CONFIG_BOARD_MRS7241_P2828_CPUF
#define CONFIG_BOARD_MRS7241_P2828_CPUF                 0
#endif

#ifndef CONFIG_BOARD_MRS7241_P2840_CPUS
#define CONFIG_BOARD_MRS7241_P2840_CPUS                 0
#endif

#ifndef CONFIG_BOARD_MRS7241_P2840_CPUF
#define CONFIG_BOARD_MRS7241_P2840_CPUF                 0
#endif


/* Private includes.
 * ----------------------------------------------------------------------------
 */

#if CONFIG_BOARD_MRS6130_P1806
#include "mrs6130_p1806/mrs6130_p1806_config.h"
#elif CONFIG_BOARD_MRS6130_P1812
#include "mrs6130_p1812/mrs6130_p1812_config.h"
#elif (CONFIG_BOARD_MRS6240_P2512_CPUS || CONFIG_BOARD_MRS6240_P2512_CPUF)
#include "mrs6240_p2512/mrs6240_p2512_config.h"
#elif (CONFIG_BOARD_MRS7241_P2828_CPUS || CONFIG_BOARD_MRS7241_P2828_CPUF)
#include "mrs7241_p2828/mrs7241_p2828_config.h"
#elif (CONFIG_BOARD_MRS7241_P2840_CPUS || CONFIG_BOARD_MRS7241_P2840_CPUF)
#include "mrs7241_p2840/mrs7241_p2840_config.h"
#else
#error "No board has been selected !!!"
#endif


#ifdef __cplusplus
}
#endif


#endif /* _BOARD_CONFIG_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
