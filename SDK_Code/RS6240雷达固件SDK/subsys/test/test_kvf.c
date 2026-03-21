/**
 ******************************************************************************
 * @file    test_kvf.c
 * @brief   test kvf define.
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
#include "hal_board.h"
#include "kvf.h"
#include "test_config.h"
#include "mtest_test.h"
#include "mtest_assert.h"

#if ((CONFIG_TEST_KVF == 1) && (CONFIG_MTEST == 1))

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Private macros.
 * ----------------------------------------------------------------------------
 */
#define KVF_TEST_BUFF_SIZE                  256

#define KVF_TEST_ITEM_SIZE_MIN              1
#define KVF_TEST_ITEM_SIZE_MAX              256

#define KVF_TEST_ITEM_ID_MIN                1
#define KVF_TEST_ITEM_ID_MAX                256

/* Private variables.
 * ----------------------------------------------------------------------------
 */
/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/* Exported functions.
 * ----------------------------------------------------------------------------
 */


ZTEST(kvf, load_save)
{
    int ret = 0;

    uint8_t * wrBuff = (uint8_t *)OSI_Malloc(KVF_TEST_BUFF_SIZE);
    uint8_t * rdBuff = (uint8_t *)OSI_Malloc(KVF_TEST_BUFF_SIZE);

    zassert_not_null(wrBuff, " %p", wrBuff);
    zassert_not_null(rdBuff, " %p", wrBuff);

    uint16_t testSeq = (uint16_t)HAL_BOARD_GetRand();
    uint16_t testId = KVF_TEST_ITEM_ID_MIN;
    uint32_t testLen = KVF_TEST_ITEM_SIZE_MIN;

    do {
        TC_PRINT("---- id[%d] len[%d]\n", testId, testLen);

        for (uint32_t idx = 0; idx < testLen; idx++) {
            wrBuff[idx] = testSeq + idx;
            rdBuff[idx] = 0;
        }

        ret = KVF_Save(testId, wrBuff, testLen);
        zassert_equal(ret, testLen, "save: %d != %d", ret, testLen);

        ret = KVF_Load(testId, rdBuff, testLen);
        zassert_equal(ret, testLen, "load: %d != %d", ret, testLen);

        for (uint32_t idx = 0; idx < testLen; idx++) {
            if (wrBuff[idx] != rdBuff[idx]) {
                TC_PRINT("\ttest data error: idx(%d) save(%02X) != load(%02X)\n",
                    idx, wrBuff[idx], rdBuff[idx]);
                zassert_ok(TC_FAIL);
            }
        }

        testLen++;
        if (testLen > KVF_TEST_ITEM_SIZE_MAX) {
            testLen = KVF_TEST_ITEM_SIZE_MIN;
            testId++;
            if (testId > KVF_TEST_ITEM_ID_MAX) {
                zassert_ok(TC_PASS);
            }
        }

    } while (1);
}

ZTEST_SUITE(kvf, NULL, NULL, NULL, NULL, NULL);

#endif

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
