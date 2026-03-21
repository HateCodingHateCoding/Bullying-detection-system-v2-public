/**
 ******************************************************************************
 * @file    packet_cfg.c
 * @brief   packet cfg define.
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
#include "soc_config.h"

#include "mmw_config.h"
#include "hal_types.h"
#include "version.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define IMAGE_ENABLE                                        1
#define IMAGE_DISABLE                                       0

/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* define in application project
 */
#ifndef CONFIG_PROJECT_VERSION_MAJOR
#define CONFIG_PROJECT_VERSION_MAJOR                        1
#endif

#ifndef CONFIG_PROJECT_VERSION_MINOR
#define CONFIG_PROJECT_VERSION_MINOR                        0
#endif

#ifndef CONFIG_PROJECT_VERSION_REVISION
#define CONFIG_PROJECT_VERSION_REVISION                     0
#endif

#ifndef CONFIG_PROJECT_VERSION_BUILD
#define CONFIG_PROJECT_VERSION_BUILD                        0
#endif


#ifndef CONFIG_BOOT_LOG
#define CONFIG_BOOT_LOG                                     0
#endif

#ifndef CONFIG_BOOT_NAME
#if (CONFIG_BOOT_LOG == 1)
#define CONFIG_BOOT_NAME                                    "psdfboot_log"
#else
#define CONFIG_BOOT_NAME                                    "psdfboot"
#endif
#endif

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */

/* Package Information
 *
 * This file is used for image packaging and will not occupy code space.
 * The following variable links are in the 'info' section, 
 * which only contains elf files and not imagefiles.
 * The following variable names must be consistent whith the packaging script, so do not modify them.
 */

const __attribute__((section(".cfg.info"))) char config_board_name[]                = CONFIG_BOARD_STRING;


/* bootloader image
 */
const __attribute__((section(".cfg.info"))) char config_img_boot                    = IMAGE_ENABLE;
const __attribute__((section(".cfg.info"))) char config_img_boot_path_str[]         = "bootloader";
const __attribute__((section(".cfg.info"))) char config_boot_name[]                 = CONFIG_BOOT_NAME;


/* merge image
 * version define in platform/utils/version.h
 */
const __attribute__((section(".cfg.info"))) uint8_t config_merge_ver_v_major        = VERSION_V_MAJOR;
const __attribute__((section(".cfg.info"))) uint8_t config_merge_ver_v_minor        = VERSION_V_MINOR;
const __attribute__((section(".cfg.info"))) uint16_t config_merge_ver_v_revision    = VERSION_V_REVISION;
const __attribute__((section(".cfg.info"))) uint8_t config_merge_ver_r_type         = VERSION_R_TYPE;
const __attribute__((section(".cfg.info"))) uint16_t config_merge_ver_r_minor       = VERSION_R_MINOR;
const __attribute__((section(".cfg.info"))) uint8_t config_merge_ver_m_project      = VERSION_M_PROJECT;
const __attribute__((section(".cfg.info"))) uint16_t config_merge_ver_m_minor       = VERSION_M_MINOR;
const __attribute__((section(".cfg.info"))) uint16_t config_merge_ver_m_number      = VERSION_M_NUMBER;

/* application image 
 */
const __attribute__((section(".cfg.info"))) char config_img_sys0                    = CONFIG_SOC_CPU_S;
const __attribute__((section(".cfg.info"))) char config_img_sys1                    = CONFIG_SOC_CPU_F;
const __attribute__((section(".cfg.info"))) char config_sys0_name[]                 = "cpus";
const __attribute__((section(".cfg.info"))) char config_sys1_name[]                 = "cpuf";
const __attribute__((section(".cfg.info"))) char config_img_sys_path_str[]          = "sys";

const __attribute__((section(".cfg.info"))) uint8_t config_sys_ver_v_major          = CONFIG_PROJECT_VERSION_MAJOR;
const __attribute__((section(".cfg.info"))) uint8_t config_sys_ver_v_minor          = CONFIG_PROJECT_VERSION_MINOR;
const __attribute__((section(".cfg.info"))) uint16_t config_sys_ver_v_revision      = CONFIG_PROJECT_VERSION_REVISION;
const __attribute__((section(".cfg.info"))) uint32_t config_sys_ver_v_build         = CONFIG_PROJECT_VERSION_BUILD;


/*
 * mmw ctrl image
 */
#if (CONFIG_MMW_CTRL == 1)
const __attribute__((section(".cfg.info"))) char config_img_mmw                     = CONFIG_MMW_CTRL;
const __attribute__((section(".cfg.info"))) char config_mmw_name[]                  = CONFIG_MMW_IMG_NAME;
const __attribute__((section(".cfg.info"))) char config_img_mmw_path_str[]          = CONFIG_MMW_IMG_PATH;
#endif

/* mmw alg image 
 */
#if (CONFIG_MMW_ALG == 1)
const __attribute__((section(".cfg.info"))) char config_img_mmw_alg                 = CONFIG_MMW_ALG;
const __attribute__((section(".cfg.info"))) char config_mmw_alg_name[]              = CONFIG_MMW_ALG_IMG_NAME;
const __attribute__((section(".cfg.info"))) char config_img_mmw_alg_path_str[]      = CONFIG_MMW_ALG_IMG_PATH;
#endif

/* flash list image
 */
const __attribute__((section(".cfg.info"))) char config_img_flash_list              = IMAGE_ENABLE;
const __attribute__((section(".cfg.info"))) char config_img_flash_list_path_str[]   = "spiflash";
const __attribute__((section(".cfg.info"))) char config_flash_list_name[]           = "spiflash_list";

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
