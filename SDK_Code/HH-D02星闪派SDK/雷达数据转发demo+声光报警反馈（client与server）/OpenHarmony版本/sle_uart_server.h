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
#include "sle_ssap_server.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0x2222

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x2323

/* Property Property */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Operation indication */
#define SLE_UUID_TEST_OPERATION_INDICATION  (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)

/* Descriptor Property */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

errcode_t sle_uart_server_init(void);

errcode_t sle_uart_server_send_report_by_uuid(const uint8_t *data, uint8_t len);

errcode_t sle_uart_server_send_report_by_handle(const uint8_t *data, uint16_t len);

uint16_t SleUartClientIsConnected(void);

typedef void (*SleUartServerMsgQueue)(uint8_t *bufferAddr, uint16_t bufferSize);

void SleUartServerRegisterMsg(SleUartServerMsgQueue sleUartServerMsg);

errcode_t sle_enable_server_cbk(void);

uint32_t UartSleSendData(uint8_t *data, uint8_t length);

bool sle_uart_server_is_ready(void);

uint16_t sle_uart_server_get_mtu(void);

errcode_t sle_uart_server_send_bytes(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif