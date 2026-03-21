/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of POSSUMIC Technology Co. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _FACTORY_TEST_H
#define _FACTORY_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum ft_event_t {
	FT_EVT_ENTER = 0,
	FT_EVT_EXIT,

} ft_event_t;

typedef enum ft_scene_event_t {
	FT_SCENE_EVT_START = 0,
	FT_SCENE_EVT_STOP,

} ft_scene_event_t;

typedef enum ft_trigger_event_t {
	FT_TRIGGER_EVT_START = 0,
	FT_TRIGGER_EVT_STOP,

} ft_trigger_event_t;


typedef enum ft_scene_id_t {
	FT_SCENE_ID_MOTION_SENSOR = 0,

} ft_scene_id_t;


typedef void (*ft_cb_t)(uint8_t event);

int factory_test_init(void);
void factory_test_callback_register(ft_cb_t cb);

#ifdef __cplusplus
}
#endif

extern uint8_t factory_test_get_g_factory_test_led_flag(void);
#endif /* _FACTORY_TEST_H */

