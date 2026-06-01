#ifndef RADAR_SLE_SERVER_ADV_H
#define RADAR_SLE_SERVER_ADV_H

#include <stdint.h>

#include "errcode.h"

#define RADAR_SLE_ADV_HANDLE_DEFAULT 1

typedef struct {
    uint8_t length;
    uint8_t type;
    uint8_t value;
} radar_sle_adv_common_value_t;

typedef enum {
    RADAR_SLE_ADV_CHANNEL_MAP_77 = 0x01,
    RADAR_SLE_ADV_CHANNEL_MAP_78 = 0x02,
    RADAR_SLE_ADV_CHANNEL_MAP_79 = 0x04,
    RADAR_SLE_ADV_CHANNEL_MAP_DEFAULT = 0x07,
} radar_sle_adv_channel_map_t;

typedef enum {
    RADAR_SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL = 0x01,
    RADAR_SLE_ADV_DATA_TYPE_ACCESS_MODE = 0x02,
    RADAR_SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME = 0x0B,
    RADAR_SLE_ADV_DATA_TYPE_TX_POWER_LEVEL = 0x0C,
} radar_sle_adv_data_type_t;

errcode_t radar_sle_server_adv_init(void);

#endif
