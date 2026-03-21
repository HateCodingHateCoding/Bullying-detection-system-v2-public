/**
 **************************************************************************************************
 * @brief   project config define.
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
 
#ifndef __FACTORY_TEST_H__
#define __FACTORY_TEST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include "hif_msg.h"
#include "ft_noise_est.h"
#include "ft_angle_est.h"
#include "factory_test_config.h"
#include "ft_type.h"

#define HIF_MSG_ID_MMW_CALIB_IQ_DATA_GET        0x39  /**< calib iq data Get in factory test */

#define FT_POWER_MODE_STANDBY			(0x00)
#define FT_POWER_MODE_MMW_2HZ 			(0x01)

#define FT_POWER_WAKEUP_MODE_IO			(0x00)
#define FT_POWER_WAKEUP_MODE_TIMEROUT	(0x01)

#define FT_POWER_MMW_2HZ_QUIT			(0x00)
#define FT_POWER_MMW_2HZ_ENTER			(0x01)


struct ft_proc_status_t {
	uint8_t ft_status;
	uint8_t ft_running;
};

enum ft_trigger_event_t {
	FT_TRIGGER_MODE_QUIT = 0,
	FT_TRIGGER_MODE_ENTER,
};



int factory_test_msg_init(void);


#ifdef __cplusplus
}
#endif
#endif  