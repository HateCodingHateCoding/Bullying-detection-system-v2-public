/*
 * **Copyright (c) 2021 - 2024, POSSUMIC Technology Co., Ltd**
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *	 this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *	 notice, this list of conditions and the following disclaimer in the
 *	 documentation and/or other materials provided with the distribution.
 *  3. Neither the name of POSSUMIC Technology Co. nor the names of its
 *	 contributors may be used to endorse or promote products derived from
 *	 this software without specific prior written permission.
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

#ifndef _MP_CTRL_H_
#define _MP_CTRL_H_

#include "stdint.h"
 /**
 * @brief Check the connectivity of input/output pins
 *
 * The interface sets a value of 0 or 1 through an output pin,
 * verifies the setting using an input pin
 *
 * @param port GPIO device instance.
 * @param input_pin input pin.
 * @param output_pin output pin.
 * @retval 0 pins connectivity.
 * @retval non-zero incorrect
 */
int mp_gpio_ctrl(uint8_t input_pin, uint8_t output_pin, uint8_t intput_mode);

int mp_gpio_output_ctrl(uint8_t output_pin, uint8_t output_value);

int mp_gpio_input_ctrl(uint8_t input_pin, uint8_t input_value, uint8_t intput_mode);

/**
* @brief System immediately enters sleep mode and is woken up by a falling edge.
*
* After the interface is called, the system immediately enters sleep mode and
* is woken up by a falling edge on a configured pin.
*
* Note: Before calling the interface, keep the configured pin at a high level.
*
* @param dev WKIO device instance.
* @param pin wkio pin.
*/
void mp_pm_wkio(uint8_t pin);

/**
* @brief System immediately enters sleep mode and wakes up after time out.
*
* After the interface is called, the system immediately enters sleep mode and
* wakes up after a configured duration.
*
* Note: the time unit for the configuration is in milliseconds (ms).
*
* @param time_out_ms time out.
*/
void mp_pm_timer(uint32_t time_out_ms);

uint32_t mp_ctrl_adc_read_mv(void);


/**
 * @brief Check if the DCDC voltage(mv) is within the specified range.
 *
 * @param min minimum voltage specified.
 * @param max max voltage specified.
 *
 * @note this api no set intldo voltage
 */
int mp_ctrl_dcdc_no_voltage(uint32_t min, uint32_t max);

#endif /* _MP_CTRL_H_ */
