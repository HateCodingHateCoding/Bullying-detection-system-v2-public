/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE UART Server module.
 */

/**
 * @defgroup SLE UART SERVER API
 * @ingroup
 * @{
 */
#ifndef SLE_UUID_SERVER_H
#define SLE_UUID_SERVER_H

#include "sle_ssap_server.h"

/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0xABCD

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x1122

/* Property Property */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Descriptor Property */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

errcode_t sle_uuid_server_init(void);
errcode_t sle_uuid_server_send_report_by_uuid(uint8_t *data, uint16_t len);
errcode_t sle_uuid_server_send_report_by_handle_id(uint8_t *data, uint16_t len, uint16_t connect_id);
#endif
