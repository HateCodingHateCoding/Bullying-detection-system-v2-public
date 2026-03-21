/**
 ******************************************************************************
 * @file    cmd_version.c
 * @brief   cmd version define.
 * @verbatim    null
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


/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"
#include "version.h"

#include "cmd_config.h"


#if ((CONFIG_SHELL_CMD_VERSION == 1) && (CONFIG_SHELL == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Private macros.
 * ----------------------------------------------------------------------------
 */
/* Private variables.
 * ----------------------------------------------------------------------------
 */
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int verion_cmd(Shell *shell, int argc, char *argv[])
{
    HAL_Status_t status = HAL_STATUS_OK;

    do {
        if (argc == 2) {
            if (!strcmp(argv[1], "sdk")) {
                VER_SdkVersion_t VER_SdkVersion;
                status = VER_GetSdkVersion(&VER_SdkVersion);
                if (status != 0) {
                    shellPrint(shell, "get sdk version fail\n\r");
                    break;
                }

                shellPrint(shell, "sdk version: R%d.%d-M%d.%d.%d-V%d.%d.%d\n",
                    VER_SdkVersion.ir_type, VER_SdkVersion.ir_minor,
                    VER_SdkVersion.im_number, VER_SdkVersion.im_project, VER_SdkVersion.im_minor,
                    VER_SdkVersion.iv_major, VER_SdkVersion.iv_minor, VER_SdkVersion.iv_revision);
            } else if (!strcmp(argv[1], "boot")) {
                VER_BootVersion_t VER_BootVersion;
                status = VER_GetBootVersion(&VER_BootVersion);
                if (status != 0) {
                    shellPrint(shell, "get boot version fail\n\r");
                    break;
                }

                shellPrint(shell, "boot version: V%d.%d.%d\n",
                    VER_BootVersion.iv_major, VER_BootVersion.iv_minor, VER_BootVersion.iv_revision);
            } else if (!strcmp(argv[1], "sys0")) {
                VER_SubVersion_t VER_SubVersion;
                status = VER_GetSubimgVersion(VER_TYPE_SYS0, &VER_SubVersion);
                if (status != 0) {
                    shellPrint(shell, "get sys0 version fail\n");
                    break;
                }

                shellPrint(shell, "sys0 version: V%d.%d.%d.%08X\n",
                    VER_SubVersion.iv_major,
                    VER_SubVersion.iv_minor,
                    VER_SubVersion.iv_revision,
                    VER_SubVersion.iv_build_num);
            } else if (!strcmp(argv[1], "sys1")) {
                VER_SubVersion_t VER_SubVersion;
                status = VER_GetSubimgVersion(VER_TYPE_SYS1, &VER_SubVersion);
                if (status != 0) {
                    shellPrint(shell, "get sys1 version fail\n");
                    break;
                }

                shellPrint(shell, "sys1 version: V%d.%d.%d.%08X\n",
                    VER_SubVersion.iv_major,
                    VER_SubVersion.iv_minor,
                    VER_SubVersion.iv_revision,
                    VER_SubVersion.iv_build_num);
            } else if (!strcmp(argv[1], "sys")) {
                VER_SubVersion_t VER_SubVersion;
                status = VER_GetSubimgVersion(VER_TYPE_SYS, &VER_SubVersion);
                if (status != 0) {
                    shellPrint(shell, "get sys version fail\n");
                    break;
                }

                shellPrint(shell, "sys version: V%d.%d.%d.%08X\n",
                    VER_SubVersion.iv_major,
                    VER_SubVersion.iv_minor,
                    VER_SubVersion.iv_revision,
                    VER_SubVersion.iv_build_num);
            } else if (!strcmp(argv[1], "mmw")) {
                VER_SubVersion_t VER_SubVersion;
                status = VER_GetSubimgVersion(VER_TYPE_MMW, &VER_SubVersion);
                if (status != 0) {
                    shellPrint(shell, "get mmw version fail\n");
                    break;
                }

                shellPrint(shell, "mmw version: V%d.%d.%d.%08X\n",
                    VER_SubVersion.iv_major,
                    VER_SubVersion.iv_minor,
                    VER_SubVersion.iv_revision,
                    VER_SubVersion.iv_build_num);
            } else if (!strcmp(argv[1], "mmw_alg")) {
                VER_SubVersion_t VER_SubVersion;
                status = VER_GetSubimgVersion(VER_TYPE_MMW_ALG, &VER_SubVersion);
                if (status != 0) {
                    shellPrint(shell, "get mmw alg version fail\n");
                    break;
                }

                shellPrint(shell, "mmw alg version: V%d.%d.%d.%08X\n",
                    VER_SubVersion.iv_major,
                    VER_SubVersion.iv_minor,
                    VER_SubVersion.iv_revision,
                    VER_SubVersion.iv_build_num);
            } else if (!strcmp(argv[1], "cust_cfg")) {
                VER_SubVersion_t VER_SubVersion;
                status = VER_GetSubimgVersion(VER_TYPE_CUSTOM_CFG, &VER_SubVersion);
                if (status != 0) {
                    shellPrint(shell, "get mmw custom config version fail\n");
                    break;
                }

                shellPrint(shell, "custom config version: V%d.%d.%d.%08X\n",
                    VER_SubVersion.iv_major,
                    VER_SubVersion.iv_minor,
                    VER_SubVersion.iv_revision,
                    VER_SubVersion.iv_build_num);
            } else if (!strcmp(argv[1], "flash_list")) {
                VER_SubVersion_t VER_SubVersion;
                status = VER_GetSubimgVersion(VER_TYPE_FLASH_LIST, &VER_SubVersion);
                if (status != 0) {
                    shellPrint(shell, "get flash list version fail\n");
                    break;
                }

                shellPrint(shell, "flash list version: V%d.%d.%d.%08X\n",
                    VER_SubVersion.iv_major,
                    VER_SubVersion.iv_minor,
                    VER_SubVersion.iv_revision,
                    VER_SubVersion.iv_build_num);
            } else if (!strcmp(argv[1], "build_time")) {
                char buildDate[16];
                char buildTime[16];
                VER_GetBuildTime(buildDate, buildTime);
                shellPrint(shell, "time: %s %s\n", buildDate, buildTime);
            } else {
                status = HAL_STATUS_INVALID_PARAM;
            }
        } else {
            status = HAL_STATUS_INVALID_PARAM;
        }
    } while (0);


    if (status != HAL_STATUS_OK) {
        shellPrint(shell, "version <sub_type>\n");
        shellPrint(shell, "\t-sdk\n");
        shellPrint(shell, "\t-boot\n");
        shellPrint(shell, "\t-sys0\n");
        shellPrint(shell, "\t-sys1\n");
        shellPrint(shell, "\t-sys\n");
        shellPrint(shell, "\t-mmw\n");
        shellPrint(shell, "\t-mmw_alg\n");
        shellPrint(shell, "\t-mpw\n");
        shellPrint(shell, "\t-cust_cfg\n");
        shellPrint(shell, "\t-flash_list\n");
        shellPrint(shell, "\t-build_time\n");
    }


    return HAL_STATUS_OK;
}


SHELL_CMD(version, verion_cmd, "version command");

#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

