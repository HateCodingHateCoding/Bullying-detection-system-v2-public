#include <stdint.h>
#include <string.h>

#include "securec.h"
#include "errcode.h"
#include "osal_debug.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "../inc/radar_sle_server_adv.h"

#define RADAR_SLE_NAME_MAX_LENGTH                 16
#define RADAR_SLE_CONN_INTV_MIN_DEFAULT           0x0A
#define RADAR_SLE_CONN_INTV_MAX_DEFAULT           0x0A
#define RADAR_SLE_ADV_INTERVAL_MIN_DEFAULT        0xC8
#define RADAR_SLE_ADV_INTERVAL_MAX_DEFAULT        0xC8
#define RADAR_SLE_CONN_SUPERVISION_TIMEOUT        0x1F4
#define RADAR_SLE_CONN_MAX_LATENCY                0x1F3
#define RADAR_SLE_ADV_TX_POWER                    20
#define RADAR_SLE_ADV_DATA_LEN_MAX                251

static uint8_t g_radar_sle_local_name[RADAR_SLE_NAME_MAX_LENGTH] = "radar_sle_srv";

static uint16_t radar_sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    uint8_t index = 0;
    uint8_t local_name_len = (uint8_t)strlen((char *)g_radar_sle_local_name);

    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = RADAR_SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    if (memcpy_s(&adv_data[index], max_len - index, g_radar_sle_local_name, local_name_len) != EOK) {
        osal_printk("[radar adv] local name memcpy fail\r\n");
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

static uint16_t radar_sle_set_adv_data(uint8_t *adv_data)
{
    uint16_t idx = 0;
    size_t len = sizeof(radar_sle_adv_common_value_t);

    radar_sle_adv_common_value_t disc_level = {
        .length = len - 1,
        .type = RADAR_SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    if (memcpy_s(&adv_data[idx], RADAR_SLE_ADV_DATA_LEN_MAX - idx, &disc_level, len) != EOK) {
        osal_printk("[radar adv] discovery level memcpy fail\r\n");
        return 0;
    }
    idx += len;

    radar_sle_adv_common_value_t access_mode = {
        .length = len - 1,
        .type = RADAR_SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    if (memcpy_s(&adv_data[idx], RADAR_SLE_ADV_DATA_LEN_MAX - idx, &access_mode, len) != EOK) {
        osal_printk("[radar adv] access mode memcpy fail\r\n");
        return 0;
    }
    idx += len;
    return idx;
}

static uint16_t radar_sle_set_scan_response_data(uint8_t *scan_rsp_data)
{
    uint16_t idx = 0;
    size_t len = sizeof(radar_sle_adv_common_value_t);
    radar_sle_adv_common_value_t tx_power = {
        .length = len - 1,
        .type = RADAR_SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = RADAR_SLE_ADV_TX_POWER,
    };

    if (memcpy_s(scan_rsp_data, RADAR_SLE_ADV_DATA_LEN_MAX, &tx_power, len) != EOK) {
        osal_printk("[radar adv] tx power memcpy fail\r\n");
        return 0;
    }
    idx += len;
    idx += radar_sle_set_adv_local_name(&scan_rsp_data[idx], RADAR_SLE_ADV_DATA_LEN_MAX - idx);
    return idx;
}

static errcode_t radar_sle_set_default_announce_param(void)
{
    sle_announce_param_t param = {0};
    uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x77};

    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = RADAR_SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = RADAR_SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = RADAR_SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = RADAR_SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = RADAR_SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = RADAR_SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = RADAR_SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = RADAR_SLE_CONN_SUPERVISION_TIMEOUT;
    param.announce_tx_power = RADAR_SLE_ADV_TX_POWER;
    param.own_addr.type = 0;
    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, mac, SLE_ADDR_LEN) != EOK) {
        return ERRCODE_FAIL;
    }
    return sle_set_announce_param(param.announce_handle, &param);
}

static errcode_t radar_sle_set_default_announce_data(void)
{
    sle_announce_data_t data = {0};
    uint8_t announce_data[RADAR_SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seek_rsp_data[RADAR_SLE_ADV_DATA_LEN_MAX] = {0};

    data.announce_data = announce_data;
    data.announce_data_len = radar_sle_set_adv_data(announce_data);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = radar_sle_set_scan_response_data(seek_rsp_data);
    return sle_set_announce_data(RADAR_SLE_ADV_HANDLE_DEFAULT, &data);
}

static void radar_sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("[radar adv] announce enable id:%u status:%x\r\n", announce_id, status);
}

static void radar_sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("[radar adv] announce disable id:%u status:%x\r\n", announce_id, status);
}

static void radar_sle_announce_terminal_cbk(uint32_t announce_id)
{
    osal_printk("[radar adv] announce terminal id:%u\r\n", announce_id);
}

static void radar_sle_enable_cbk(errcode_t status)
{
    osal_printk("[radar adv] sle enable status:%x\r\n", status);
}

static void radar_sle_announce_register_cbks(void)
{
    sle_announce_seek_callbacks_t cbks = {0};
    cbks.announce_enable_cb = radar_sle_announce_enable_cbk;
    cbks.announce_disable_cb = radar_sle_announce_disable_cbk;
    cbks.announce_terminal_cb = radar_sle_announce_terminal_cbk;
    cbks.sle_enable_cb = radar_sle_enable_cbk;
    sle_announce_seek_register_callbacks(&cbks);
}

errcode_t radar_sle_server_adv_init(void)
{
    errcode_t ret;

    radar_sle_announce_register_cbks();
    ret = radar_sle_set_default_announce_param();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[radar adv] set announce param fail:%x\r\n", ret);
        return ret;
    }
    ret = radar_sle_set_default_announce_data();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[radar adv] set announce data fail:%x\r\n", ret);
        return ret;
    }
    ret = sle_start_announce(RADAR_SLE_ADV_HANDLE_DEFAULT);
    osal_printk("[radar adv] start announce ret:%x\r\n", ret);
    return ret;
}
