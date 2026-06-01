#ifndef RADAR_SLE_SERVER_H
#define RADAR_SLE_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include "errcode.h"
#include "sle_ssap_server.h"

#define RADAR_SLE_UUID_SERVER_SERVICE     0x2222
#define RADAR_SLE_UUID_SERVER_NTF_REPORT  0x2323

#define RADAR_SLE_TEST_PROPERTIES (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)
#define RADAR_SLE_TEST_DESCRIPTOR (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

errcode_t radar_sle_server_init(void);
bool radar_sle_server_is_ready(void);
uint16_t radar_sle_server_get_mtu(void);
uint8_t radar_sle_flow_ctrl_flag(void);
errcode_t radar_sle_server_send_bytes(uint8_t *data, uint16_t len);

#endif
