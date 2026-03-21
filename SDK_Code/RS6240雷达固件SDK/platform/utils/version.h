/**
 ******************************************************************************
 * @file    version.h
 * @brief   version define.
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

#ifndef _VERSION_H
#define _VERSION_H

/* Includes.
 * ----------------------------------------------------------------------------
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types.
 * ----------------------------------------------------------------------------
 */
typedef enum {
    VER_TYPE_SYS                    = 0,
    VER_TYPE_SYS0                   = 1,
    VER_TYPE_SYS1                   = 2,
    VER_TYPE_MMW                    = 3,
    VER_TYPE_MMW_ALG                = 4,
    VER_TYPE_MPW                    = 5,
    VER_TYPE_CUSTOM_CFG             = 6,
    VER_TYPE_FLASH_LIST             = 7,
    VER_TYPE_MAX
} VER_SubType_t;

typedef struct {
    uint8_t iv_major;
    uint8_t iv_minor;
    uint16_t iv_revision;
    uint32_t iv_build_num;
} VER_SubVersion_t;

typedef struct {
    uint32_t addr;
    uint32_t size;
} VER_ImgInfo_t;

typedef struct {
    uint16_t ir_minor;
    uint8_t  ir_type;

    uint8_t  im_number;
    uint16_t im_project;
    uint16_t im_minor;

    uint8_t  iv_major;
    uint8_t  iv_minor;
    uint16_t iv_revision;
} VER_SdkVersion_t;

typedef struct {
    uint8_t  iv_major;
    uint8_t  iv_minor;
    uint16_t iv_revision;
} VER_BootVersion_t;

/* Exported constants.
 * ----------------------------------------------------------------------------
 */
/* R type.minor M number.project.minor V major.minor.revision */

#ifndef VERSION_V_MAJOR
#define VERSION_V_MAJOR                   2
#endif

#ifndef VERSION_V_MINOR
#define VERSION_V_MINOR                   0
#endif

#ifndef VERSION_V_REVISION
#define VERSION_V_REVISION                6
#endif

#ifndef VERSION_R_TYPE
#define VERSION_R_TYPE                    0
#endif

#ifndef VERSION_R_MINOR
#define VERSION_R_MINOR                   0
#endif

#ifndef VERSION_M_PROJECT
#define VERSION_M_PROJECT                 0
#endif

#ifndef VERSION_M_MINOR
#define VERSION_M_MINOR                   0
#endif

#ifndef VERSION_M_NUMBER
#define VERSION_M_NUMBER                  0
#endif

/* Exported macro.
 * ----------------------------------------------------------------------------
 */

/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int VER_GetSubimgVersion(VER_SubType_t verType, VER_SubVersion_t * pver);

int VER_GetSdkVersion(VER_SdkVersion_t * pver);

int VER_GetBootVersion(VER_BootVersion_t * pver);

void VER_GetBuildTime(char *pdate, char *ptime);

int VER_GetCustomCfgStoreInfo(VER_ImgInfo_t * pver);

#ifdef __cplusplus
}
#endif

#endif /* _VERSION_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
