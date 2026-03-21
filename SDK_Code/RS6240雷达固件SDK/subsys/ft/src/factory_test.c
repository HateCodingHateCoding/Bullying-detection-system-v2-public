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

#include <stdio.h>
#include <string.h>
#include "osi.h"
#include "hif.h"

#include "mp_ctrl.h"
#include "factory_test.h"

#define FT_DEBUG_ON        (0)
#define FT_WRN_ON          (1)
#define FT_ERR_ON          (1)

#if FT_DEBUG_ON
#define FT_DEBUG(fmt, arg...)    printf("[FT-D] "fmt, ##arg)
#else
#define FT_DEBUG(fmt, arg...)
#endif
#if FT_WRN_ON
#define FT_WRN(fmt, arg...)      printf("[FT-W] "fmt, ##arg)
#else
#define FT_WRN(fmt, arg...)
#endif
#if FT_ERR_ON
#define FT_ERR(fmt, arg...)      printf("[FT-E] "fmt, ##arg)
#else
#define FT_ERR(fmt, arg...)
#endif

struct dcdc_vol_t {
	uint32_t min;
	uint32_t max;
};

struct io_ctrl_t {
	uint8_t type;
	uint8_t input;
	uint8_t output;
	uint8_t input_mode;
};

struct io_output_ctrl_t {
	uint8_t type;
	uint8_t output;
	uint8_t value;
};

struct io_input_ctrl_t {
	uint8_t type;
	uint8_t input;
	uint8_t value;
	uint8_t input_mode;
};


static uint8_t ft_mode_enable = 0;
static ft_cb_t ft_cb;

uint8_t g_factory_test_led_flag = 0;

extern void hif_pm_unlock(void);
extern void hif_pm_lock(void);

static int ft_mode_enable_handle(HIF_MsgHdr_t *msg)
{
	int ret = HIF_CMD_STATUS_SUCCESS;

	uint8_t enable = *((uint8_t *)(msg + 1));

	if (g_factory_test_led_flag && !enable) {
		FT_WRN("ft mode is already %s\n", enable ? "enabled" : "disabled");
		ret = HIF_CMD_STATUS_IO;
		goto exit;
	}
	if (enable == ft_mode_enable) {
		FT_WRN("ft mode is already %s\n", enable ? "enabled" : "disabled");
		ret = HIF_CMD_STATUS_IO;
		goto exit;
	}
	ft_mode_enable = enable;

	if (ft_cb)
		ft_cb(enable ? FT_EVT_ENTER : FT_EVT_EXIT);

exit:
	return HIF_MsgResp(msg, 0, ret);
}

static int ft_voltage_handle(HIF_MsgHdr_t *msg)
{
	FT_DEBUG("ft voltage test\n");

	struct dcdc_vol_t *vol = (struct dcdc_vol_t *)(msg + 1);

	int ret = -1;

	uint8_t *ack_data = (uint8_t *)HIF_Msg_AckDataPtr(msg);

	FT_DEBUG("voltage min:%d max:%d\n", vol->min, vol->max);

	extern uint32_t mp_ctrl_adc_read_mv(void);
	uint32_t adc_val = mp_ctrl_adc_read_mv();
	if(adc_val > vol->min && adc_val < vol->max) {
		ret = HIF_CMD_STATUS_SUCCESS;
	} else {
		ret = HIF_CMD_STATUS_IO;
	}

	memcpy(ack_data, &adc_val, sizeof(adc_val));

	return HIF_MsgResp(msg, sizeof(ack_data), ret);
}

static int ft_io_handle(HIF_MsgHdr_t *msg)
{
	FT_DEBUG("ft io test\n");

	struct io_ctrl_t *io_ctrl = (struct io_ctrl_t *)(msg + 1);
	struct io_output_ctrl_t *io_output_ctrl = (struct io_output_ctrl_t *)(msg + 1);
	struct io_input_ctrl_t *io_input_ctrl = (struct io_input_ctrl_t *)(msg + 1);

	int ret = -1;
	uint8_t status = HIF_CMD_STATUS_IO;

	FT_DEBUG("io ctrl type:%d\n", io_ctrl->type);

	/* input and output */
	if (io_ctrl->type == 0) {
		if(msg->length == 3) {
			io_ctrl->input_mode = 0;
		}
		ret = mp_gpio_ctrl(io_ctrl->input, io_ctrl->output, io_ctrl->input_mode);
		if (ret == -1) {
			status = HIF_CMD_STATUS_PARAM;
		} else if (ret == -2) {
			status = HIF_CMD_STATUS_SYSERR;
		} else if (ret == 0) {
			status = HIF_CMD_STATUS_SUCCESS;
		}
	/* output */
	} else if (io_ctrl->type == 1) {
		ret = mp_gpio_output_ctrl(io_output_ctrl->output, io_output_ctrl->value);
		if (ret == -1) {
			status = HIF_CMD_STATUS_PARAM;
		} else if (ret == -2) {
			status = HIF_CMD_STATUS_SYSERR;
		} else if (ret == 0) {
			status = HIF_CMD_STATUS_SUCCESS;
		}

	/* input */
	} else if (io_ctrl->type == 2) {
		if(msg->length == 3) {
			io_input_ctrl->input_mode = 0;
		}
		ret = mp_gpio_input_ctrl(io_input_ctrl->input, io_input_ctrl->value, io_input_ctrl->input_mode);
		if (ret == -1) {
			status = HIF_CMD_STATUS_PARAM;
		} else if (ret == -2) {
			status = HIF_CMD_STATUS_SYSERR;
		} else if (ret == 0) {
			status = HIF_CMD_STATUS_SUCCESS;
		}

	} else {
		status = HIF_CMD_STATUS_PARAM;
	}


	return HIF_MsgResp(msg, 0, status);
}


static int ft_mmw_perf_handle(HIF_MsgHdr_t *msg)
{
	FT_DEBUG("ft perf test\n");

	int ret = HIF_CMD_STATUS_SUCCESS;

	return HIF_MsgResp(msg, 0, ret);
}

static int ft_mmw_calib_handle(HIF_MsgHdr_t *msg)
{
	int ret = HIF_CMD_STATUS_SUCCESS;

	return HIF_MsgResp(msg, 0, ret);
}

static int ft_power_handle(HIF_MsgHdr_t *msg)
{
	FT_DEBUG("ft power test\n");
	int status = HIF_CMD_STATUS_SUCCESS;

	return HIF_MsgResp(msg, 0, status);
}


static int ft_save_calib_handle(HIF_MsgHdr_t *msg)
{
	FT_DEBUG("ft calib save test\n");

	int ret = HIF_CMD_STATUS_SUCCESS;
	return HIF_MsgResp(msg, 0, ret);
}

static int ft_get_calib_handle(HIF_MsgHdr_t *msg)
{
	int ret = HIF_CMD_STATUS_SUCCESS;

	return HIF_MsgResp(msg, 0, ret);
}


static int ft_test_trigger_func_handle(HIF_MsgHdr_t *msg)
{
    uint8_t ret = HIF_CMD_STATUS_SUCCESS;

    return HIF_MsgResp(msg, 0, ret);
}


int factory_test_init(void)
{
	HIF_MsgHdl_Regist(HIF_MSG_ID_FACTORY_MODE, ft_mode_enable_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_VOLTAGE_TEST, ft_voltage_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_IO_TEST, ft_io_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_MMW_PERF_TEST, ft_mmw_perf_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_MMW_CALIB_TEST, ft_mmw_calib_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_PWR_CONSUME_TEST, ft_power_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_PARAM_SAVE, ft_save_calib_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_PARAM_GET, ft_get_calib_handle);
	HIF_MsgHdl_Regist(HIF_MSG_ID_MMW_FUN_TEST, ft_test_trigger_func_handle);

	return 0;
}

void factory_test_callback_register(ft_cb_t cb)
{
	ft_cb = cb;
}

uint8_t factory_test_get_g_factory_test_led_flag(void)
{
	return g_factory_test_led_flag;
}

