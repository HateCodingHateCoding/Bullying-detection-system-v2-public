/**
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

#if (CONFIG_MMW_DRIVER_TEMP_MGMT)
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "types.h"
#include "ll_utils.h"
#include "shell.h"
#include "shell_port.h"
#include "shell_cmd_group.h"

#include "mmw_ctrl.h"
#include "mmw_temp_mgmt_api.h"

static int cmd_mmw_dev_temp_mgmt_mode_cfg(const struct shell *sh, size_t argc, char *argv[])
{
	uint8_t temp_mgmt_mode;
	uint32_t frame_num_per_meas;

	if (argc < 3) {
		return -1;
	}

	temp_mgmt_mode = (uint8_t)strtoul(argv[1], NULL, 10);
	frame_num_per_meas = strtoul(argv[2], NULL, 10);

	if (mmw_temp_mgmt_mode_cfg(temp_mgmt_mode)) {
		return -1;
	}

	temp_mgmt_set_detect_period(frame_num_per_meas);

	return 0;
}

static int cmd_mmw_dev_temp_thr_cfg(const struct shell *sh, size_t argc, char *argv[])
{
	uint16_t temp_thr;

	if (argc < 2) {
		return -1;
	}

	temp_thr = (uint16_t)strtoul(argv[1], NULL, 10);

	temp_mgmt_triger_thr_cfg(temp_thr);

	return 0;
}

static int cmd_mmw_dev_itrim_temp_thr_cfg(const struct shell *sh, size_t argc, char *argv[])
{
	int16_t temp_low_thr, temp_high_thr;

	if (argc < 3) {
		return -1;
	}

	temp_low_thr = (int16_t)strtol(argv[1], NULL, 10);
	temp_high_thr = (int16_t)strtol(argv[2], NULL, 10);

	temp_mgmt_set_itrim_temp_thr(temp_low_thr, temp_high_thr);

	return 0;
}

static int cmd_mmw_dev_calib_mask_cfg(const struct shell *sh, size_t argc, char *argv[])
{
	uint32_t calib_mask;

	if (argc < 2) {
		return -1;
	}

	calib_mask = strtoul(argv[1], NULL, 16);

	g_temp_mgmt_info.calib_mask = calib_mask;

	return 0;
}

static int cmd_mmw_dev_read_temp(const struct shell *sh, size_t argc, char *argv[])
{
	int16_t temp = tsensor_read_temp();

	shell_fprintf(sh, SHELL_NORMAL, "cur_temp: %d, calib_temp:%d\n", temp, temp_mgmt_get_calib_temp());

	return 0;
}


SHELL_STATIC_SUBCMD_SET_CREATE(mmw_dev_cmds,
#if (CONFIG_MMW_DRIVER_TEMP_MGMT)
    SHELL_CMD(temp_mode,  		NULL, "<mode 0/1> <frame_num>", 		    cmd_mmw_dev_temp_mgmt_mode_cfg),
    SHELL_CMD(temp_thr, 		NULL, "<thr_0p1>", 		                    cmd_mmw_dev_temp_thr_cfg),
    SHELL_CMD(calib_mask, 		NULL, "<mask>", 		                    cmd_mmw_dev_calib_mask_cfg),
    SHELL_CMD(itrim_temp_thr, 	NULL, "<low_thr_0p1> <high_thr_0p1>", 		cmd_mmw_dev_itrim_temp_thr_cfg),
    SHELL_CMD(read_temp, 	    NULL, NULL, 		                        cmd_mmw_dev_read_temp),
#endif
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(mmw_dev, &mmw_dev_cmds, "MMW Dev shell commands", NULL);
#endif