/**
 **************************************************************************************************
 * @brief   factory test config define.
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
 **************************************************************************************************
 */
 
#ifndef __FACTORY_TEST_CONFIG_H__
#define __FACTORY_TEST_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include "mmw_ctrl.h"

#if CONFIG_SOC_SERIES_RS624X
#define CONFIG_MAX_RX_ANT 		(8)
#elif CONFIG_SOC_RS6130
#define CONFIG_MAX_RX_ANT 		(3)
#endif

#define CONFIG_MMW_LOG_DEBUG		0

#if CONFIG_MMW_LOG_DEBUG
#define MMW_LOG_DBG(...)	printk(__VA_ARGS__)
#else
#define MMW_LOG_DBG(...)	
#endif

/*mmw radio noise hardware speical config */
#define CONFIG_MMW_VENDOR_FILTER_FRAME_NUM		(1)

#define FT_DEFAULT_RANGE				(10240)
#define FT_DEFAULT_RANGE_RES			(80)
#define FT_DEFAULT_START_FREQ 			(59000)

/* noise est default param set */
#define NOISE_FT_DEFAULT_VELOC			(10240)
#define NOISE_FT_DEFAULT_VEL_RES		(640)	/* max=10.24m/s, resol=0.64m/s, 32dfft */
#define NOISE_FT_DEFAULT_FRAME_NUM		(20)
#define NOISE_FT_DEFAULT_FRAME_PERIOD	(200)
#define NOISE_FT_DEFAULT_RANGE_MAX_MM	(1400)
#define NOISE_FT_DEFAULT_RANGE_MIN_MM	(900)

/* angle est default param set */
#define ANGLE_CALIB_DEFAULT_FRAME_NUM	(10)
#define ANGLE_EST_DEFAULT_FRAME_PERIOD	(100)
#define ANGLE_EST_DEFAULT_RANGE_MAX_MM	(1400)
#define ANGLE_EST_DEFAULT_RANGE_MIN_MM	(900)

/* FT trigger function param set */
#define FT_MIN_FORCE_TRIGGER_MICRO_PC_NUM	(3)
#define FT_MIN_FORCE_TRIGGER_MOVING_PC_NUM	(2)

#define FT_TRIGGER_RANGE_MM   		(1500)

#define FT_LED_WKIO_PORT                 WKIO_PORT_A
#define FT_LED_WKIO_PIN                  WKIO_PIN_11
#define FT_LED_WKIO_MODE                 HAL_WKIO_MODE_HOLD




#ifdef __cplusplus
}
#endif

#endif /* FACTORY_TEST_CONFIG_H */