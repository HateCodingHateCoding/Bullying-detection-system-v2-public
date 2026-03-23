/**
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 */

#ifndef SLE_UART_SERVER_H
#define SLE_UART_SERVER_H

#include <stdint.h>
#include "ohos_sle_ssap_server.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0x2222
/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x2323

/* Property permissions */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)
/* Operation indication */
#define SLE_UUID_TEST_OPERATION_INDICATION  (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)
/* Descriptor permissions */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Audio frame protocol constants */
#define AUDIO_FRAME_HEADER       0x55AA
#define AUDIO_FRAME_TAIL         0xAA55
#define AUDIO_FRAME_OVERHEAD     8
#define AUDIO_CHUNK_SAMPLES      512
#define AUDIO_FRAME_MAX_SIZE     (AUDIO_CHUNK_SAMPLES * 2 + AUDIO_FRAME_OVERHEAD)

errcode_t sle_uart_server_init(void);
errcode_t sle_uart_server_send_report_by_handle(const uint8_t *data, uint16_t len);
errcode_t sle_enable_server_cbk(void);
uint32_t UartSleSendData(uint8_t *data, uint16_t length);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
