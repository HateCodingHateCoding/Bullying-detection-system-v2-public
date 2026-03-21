/**
 ******************************************************************************
 * @file    hif_msg.c
 * @brief   hif msg define.
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
#include "hif_types.h"
#include "hif.h"
#include "ll_rtctick.h"
#include "hif_log.h"
#include "hif_config.h"
#include "hal_board.h"
#if (CONFIG_HIF == 1)

/* Private typedef.
 * ----------------------------------------------------------------------------
 */

/* Private defines.
 * ----------------------------------------------------------------------------
 */
/* Version of HIF handler implementation */
#define HIF_HANDLE_VERSION                      0

#define HIF_MSG_CMD_GROUP_NUM                   10 /* 0x00~0x9F, cmds */
#define HIF_MSG_CMD_INDEX_CNT                   16

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
int msghdl_get_version(HIF_MsgHdr_t *msg);
int msghdl_chipinfo(HIF_MsgHdr_t *msg);
int msghdl_phy_get(HIF_MsgHdr_t *msg);
int msghdl_phy_set(HIF_MsgHdr_t *msg);
int msghdl_hif_param_handler(HIF_MsgHdr_t *msg);
int msghdl_device_pwrctrl(HIF_MsgHdr_t *msg);
int msghdl_host_poll(HIF_MsgHdr_t *msg);
int msghdl_host_state(HIF_MsgHdr_t *msg);
int msghdl_host_reboot(HIF_MsgHdr_t *msg);

bool hif_operate_lock(void);
void hif_operate_unlock(void);
void hif_device_wakeup_req(void);
void hif_device_suspend_req(void);
void hif_msg_report_set(uint8_t new_state);

/* Private variables.
 * ----------------------------------------------------------------------------
 */

HIF_MsgHdl_t *g_hif_msghdl[HIF_MSG_CMD_GROUP_NUM];
HIF_MsgHdl_t g_common_msghdl[HIF_MSG_CMD_INDEX_CNT] = {
/* 0x00 */    &msghdl_get_version,
/* 0x01 */    &msghdl_chipinfo,
/* 0x02 */    &msghdl_phy_get,
/* 0x03 */    NULL,
/* 0x04 */    &msghdl_host_reboot,
/* 0x05 */    &msghdl_device_pwrctrl,
/* 0x06 */    NULL,
/* 0x07 */    NULL,
/* 0x08 */    NULL,
/* 0x09 */    &msghdl_phy_set,
/* 0x0A */    &msghdl_hif_param_handler,
/* 0x0B */    &msghdl_host_state,
/* 0x0C */    &msghdl_host_poll,
/* 0x0D */    NULL,
/* 0x0E */    NULL,
/* 0x0F */    NULL,
};

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

int HIF_MsgHdl_Regist(uint8_t msgId, HIF_MsgHdl_t msgHdl)
{
    uint8_t msg_gp  = msgId>>4;
    uint8_t msg_idx = (msgId & 0xF);
    if (msg_gp >= HIF_MSG_CMD_GROUP_NUM || msg_idx >= HIF_MSG_CMD_INDEX_CNT) {
        HIF_LOG_INF("Invalid msghdl regist %d %d\n", msg_gp, msg_idx);
        return HIF_ERRCODE_INVALID_PARAM;
    }

    if (!hif_operate_lock()) {
        return HIF_ERRCODE_NOT_READY;
    }

    if (g_hif_msghdl[msg_gp] == NULL) {
        int group_size = sizeof(HIF_MsgHdl_t)*HIF_MSG_CMD_INDEX_CNT;
        g_hif_msghdl[msg_gp] = (HIF_MsgHdl_t *)OSI_Malloc(group_size);
        if (g_hif_msghdl[msg_gp] == NULL) {
            return -2;
        }
        HIF_LOG_INF("Msghdl alloc %d 0x%0x\n", msg_gp, (uint32_t)g_hif_msghdl[msg_gp]);
        memset(g_hif_msghdl[msg_gp], 0, group_size);
    }
    g_hif_msghdl[msg_gp][msg_idx] = msgHdl;
    hif_operate_unlock();

    HIF_LOG_INF("Msghdl 0x%02X regist %d %d 0x%0x\n", msgId, msg_gp, msg_idx, (uint32_t)msgHdl);
    return 0;
}

void HIF_MsgHdl_Process(HIF_MsgHdr_t *cmd)
{
    uint8_t msg_gp  = cmd->msg_id>>4;
    uint8_t msg_idx = cmd->msg_id & 0xF;

    if (msg_gp >= HIF_MSG_CMD_GROUP_NUM  ||
        msg_idx >= HIF_MSG_CMD_INDEX_CNT ||
        g_hif_msghdl[msg_gp] == NULL ||
        g_hif_msghdl[msg_gp][msg_idx] == NULL) {
        HIF_MsgResp(cmd, 0, HIF_CMD_STATUS_UNSUPPORT);
    } else {
        g_hif_msghdl[msg_gp][msg_idx](cmd);
    }
}

int HIF_MsgHdl_Init(void)
{
    g_hif_msghdl[0] = &g_common_msghdl[0];
    return 0;
}

void HIF_MsgHdl_Deinit(void)
{
    for (int i = 1; i < HIF_MSG_CMD_GROUP_NUM; ++i) {
        if (g_hif_msghdl[i] != NULL) {
            HIF_LOG_INF("Msghdl free %d 0x%0x\n", i, (uint32_t)g_hif_msghdl[i]);
            OSI_Free(g_hif_msghdl[i]);
            g_hif_msghdl[i] = NULL;
        }
    }
}



#define VALID_VERSION_V            HAL_BIT(0)
#define VALID_VERSION_R            HAL_BIT(1)
#define VALID_VERSION_M            HAL_BIT(2)
#define VALID_VERSION_OS_BD        HAL_BIT(3)
#define VALID_VERSION_MMW_BD       HAL_BIT(4)
#define VALID_VERSION_MPW_BD       HAL_BIT(5)
struct hif_get_version_t {
    //uint8_t ack_status;
    uint8_t  hif_proto_ver;
    uint8_t  hif_handle_ver;
    uint8_t  ver_valid;
    uint8_t  ver_V_x;
    uint8_t  ver_V_y;
    uint16_t ver_V_z;
    uint16_t ver_M_x;
    uint16_t ver_M_y;
    uint8_t  ver_M_p;
    uint8_t  ver_R_x;
    uint16_t ver_R_y;
    uint32_t os_build;
    uint32_t mmw_build;
    uint32_t mpw_build;
} __packed;
typedef struct hif_get_version_t HIF_VERSION_GET;

int msghdl_get_version(HIF_MsgHdr_t *msg)
{
#if 1
    int status = 0;
    uint8_t version_msk = 0;

    if (msg->length != 0) {
        version_msk = *(uint8_t *)(msg + 1);
    }

    if (version_msk == 0) {
        HIF_VERSION_GET *resp = HIF_Msg_AckDataPtr(msg);
        uint8_t ver_valid = 0;

        resp->hif_proto_ver  = HIF_PROTO_VERSION;
        resp->hif_handle_ver = HIF_HANDLE_VERSION;

#ifdef VERSION_V_MAJOR
        resp->ver_V_x = VERSION_V_MAJOR;
        resp->ver_V_y = VERSION_V_MINOR;
        resp->ver_V_z = VERSION_V_REVISION;
        ver_valid |= VALID_VERSION_V;
#endif
#ifdef VERSION_M_NUMBER
        resp->ver_M_x = VERSION_M_NUMBER;
        resp->ver_M_y = VERSION_M_MINOR;
        resp->ver_M_p = VERSION_M_PROJECT;
        ver_valid |= VALID_VERSION_M;
#endif
#ifdef VERSION_R_TYPE
        resp->ver_R_x = VERSION_R_TYPE;
        resp->ver_R_y = VERSION_R_MINOR;
        ver_valid |= VALID_VERSION_R;
#endif
#ifdef BUILD_VERSION_HEX
        resp->os_build    = BUILD_VERSION_HEX;
        ver_valid |= VALID_VERSION_OS_BD;
#endif
#ifdef MMW_BUILD_HEX_VERSION
        resp->mmw_build = 0x01;
        ver_valid |= VALID_VERSION_MMW_BD;
#endif
#ifdef MPW_BUILD_VERSION_HEX
        resp->mpw_build = MPW_BUILD_VERSION_HEX;
        ver_valid |= VALID_VERSION_MPW_BD;
#endif
        resp->ver_valid = ver_valid;

        status = HIF_MsgResp(msg, sizeof(HIF_VERSION_GET), HIF_CMD_STATUS_SUCCESS);
    }else {
        uint8_t *rsp_version = HIF_Msg_AckDataPtr(msg);
        uint8_t rsp_len = 0;
        uint8_t ret_status = 0;
        VER_SubVersion_t cfg_img;

        if ((version_msk & 0x02) != 0) {
            VER_SdkVersion_t VER_SdkVersion;
            status = VER_GetSdkVersion(&VER_SdkVersion);
            if (status == HAL_STATUS_OK) {
                rsp_version[rsp_len] = VER_SdkVersion.ir_type;
                rsp_len += 1;
                *(uint16_t *)&rsp_version[rsp_len] = VER_SdkVersion.ir_minor;
                rsp_len += 2;
                *(uint16_t *)&rsp_version[rsp_len] = VER_SdkVersion.im_minor;
                rsp_len += 2;
                rsp_version[rsp_len] = VER_SdkVersion.im_project;
                rsp_len += 1;
                *(uint16_t *)&rsp_version[rsp_len] = VER_SdkVersion.im_number;
                rsp_len += 2;
                rsp_version[rsp_len] = VER_SdkVersion.iv_major;
                rsp_len += 1;
                rsp_version[rsp_len] = VER_SdkVersion.iv_minor;
                rsp_len += 1;
                *(uint16_t *)&rsp_version[rsp_len] = VER_SdkVersion.iv_revision;
                rsp_len += 2;
            } else {
                ret_status |= 0x02;
            }
        }

        if ((version_msk & 0x10) != 0) {
            status = VER_GetSubimgVersion(VER_TYPE_FLASH_LIST, &cfg_img);
            if (status == HAL_STATUS_OK) {
                rsp_version[rsp_len] = cfg_img.iv_major;
                rsp_len += 1;
                rsp_version[rsp_len] = cfg_img.iv_minor;
                rsp_len += 1;
                *(uint16_t *)&rsp_version[rsp_len] = cfg_img.iv_revision;
                rsp_len += 2;
                *(uint32_t *)&rsp_version[rsp_len] = cfg_img.iv_build_num;
                rsp_len += 4;
            } else {
                ret_status |= 0x10;
            }
        }

        if ((version_msk & 0x40) != 0) {
            status = VER_GetSubimgVersion(VER_TYPE_CUSTOM_CFG, &cfg_img);
            if (status == HAL_STATUS_OK) {
                rsp_version[rsp_len] = cfg_img.iv_major;
                rsp_len += 1;
                rsp_version[rsp_len] = cfg_img.iv_minor;
                rsp_len += 1;
                *(uint16_t *)&rsp_version[rsp_len] = cfg_img.iv_revision;
                rsp_len += 2;
                *(uint32_t *)&rsp_version[rsp_len] = cfg_img.iv_build_num;
                rsp_len += 4;
            } else {
                ret_status |= 0x40;
            }
        }

        if (ret_status == 0) {
            status = HIF_MsgResp(msg, rsp_len, HIF_CMD_STATUS_SUCCESS);
        } else {
            status = HIF_MsgResp(msg, 0, HIF_CMD_STATUS_SYSERR);
        }
    }
#endif
    return 0;
}

#define HIF_CHIPINFO_TICK_GET            0x40
int msghdl_chipinfo(HIF_MsgHdr_t *msg)
{
    uint8_t req_type = *(uint8_t *)(msg + 1);

    if (req_type == HIF_CHIPINFO_TICK_GET) {
        uint32_t *resp = HIF_Msg_AckDataPtr(msg);

        resp[0] = LL_RTCTICK_GetCurrentValue32();
        return HIF_MsgResp(msg, sizeof(uint32_t), HIF_CMD_STATUS_SUCCESS);
    } else {
        return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_UNSUPPORT);
    }
    return 0;
}

#define HIF_PHY_INFO_I2C                    0x02
int msghdl_phy_get(HIF_MsgHdr_t *msg)
{
#if 0
    uint8_t req_type = *(uint8_t *)(msg + 1);

    if (req_type == HIF_PHY_INFO_I2C) {
#if (CONFIG_I2C == 1)
        uint8_t *resp = HIF_Msg_AckDataPtr(msg);

        extern uint8_t hif_com_iic_param_get(void);
        resp[0] = hif_com_iic_param_get();
#endif
        return HIF_MsgResp(msg, 1, HIF_CMD_STATUS_SUCCESS);
    } else {
        return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_UNSUPPORT);
    }
#endif
    return 0;
}

int msghdl_phy_set(HIF_MsgHdr_t *msg)
{
#if 0
    uint8_t *preq = (uint8_t *)(msg + 1);

    if (preq[0] == HIF_PHY_INFO_I2C) {
#if (CONFIG_I2C == 1)
        extern void hif_com_iic_param_set(uint8_t param);
        hif_com_iic_param_set(preq[1]);
#endif
        return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_SUCCESS);
    } else {
        return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_UNSUPPORT);
    }
#endif
    return 0;
}

#define HIF_SUBID_SPLIT_SET             (0x01)
#define HIF_SUBID_SPLIT_GET             (0x02)
#define HIF_SUBID_UART_IO_DELAY_SET     (0x03)
#define HIF_SUBID_UART_IO_DELAY_GET     (0x04)
#define HIF_SUBID_SPI_IO_DELAY_GET      (0x05)
#define HIF_SUBID_SPI_POLL_DELAY_GET    (0x06)
#define HIF_SUBID_RETRY_SET             (0x07)

#define SPI_IO_RX2TX_MIN_DELAY     500
#define SPI_IO_RX2TX_TIMEOUT       10000
#define SPI_IO_TX2RX_MIN_DELAY     500
#define SPI_IO_TX2RX_TIMEOUT       10000
#define SPI_POLL_DELAY             10000
#define SPI_RX2TX_DELAY            1000
#define SPI_TX2RX_DELAY            1000

int msghdl_hif_param_handler(HIF_MsgHdr_t *msg)
{
    uint8_t *data = (uint8_t *)(msg + 1);    
    uint8_t *rsp_data = HIF_Msg_AckDataPtr(msg);
    int ret = 0;
    int ret_status = 0;
    uint8_t rsp_len = 0;
    switch(data[0]) {
        case HIF_SUBID_SPI_IO_DELAY_GET:
        {
            ret_status = 1;
            uint32_t rx2tx_min_delay = SPI_IO_RX2TX_MIN_DELAY;
            uint32_t rx2tx_timeout = SPI_IO_RX2TX_TIMEOUT;
            uint32_t tx2rx_min_delay = SPI_IO_TX2RX_MIN_DELAY;
            uint32_t tx2rx_timeout = SPI_IO_TX2RX_TIMEOUT;
            
            rsp_data[0] = HIF_SUBID_SPI_IO_DELAY_GET;
            rsp_data[1] = rx2tx_min_delay & 0xFF;
            rsp_data[2] = (rx2tx_min_delay >> 8) & 0xFF;
            rsp_data[3] = (rx2tx_min_delay >> 16) & 0xFF;
            rsp_data[4] = (rx2tx_min_delay >> 24) & 0xFF;
            rsp_data[5] = rx2tx_timeout & 0xFF;
            rsp_data[6] = (rx2tx_timeout >> 8) & 0xFF;
            rsp_data[7] = (rx2tx_timeout >> 16) & 0xFF;
            rsp_data[8] = (rx2tx_timeout >> 24) & 0xFF;
            rsp_data[9] = tx2rx_min_delay & 0xFF;
            rsp_data[10] = (tx2rx_min_delay >> 8) & 0xFF;
            rsp_data[11] = (tx2rx_min_delay >> 16) & 0xFF;
            rsp_data[12] = (tx2rx_min_delay >> 24) & 0xFF;
            rsp_data[13] = tx2rx_timeout & 0xFF;
            rsp_data[14] = (tx2rx_timeout >> 8) & 0xFF;
            rsp_data[15] = (tx2rx_timeout >> 16) & 0xFF;
            rsp_data[16] = (tx2rx_timeout >> 24) & 0xFF; 
            rsp_len = 17;
            break;
        }

        case HIF_SUBID_SPI_POLL_DELAY_GET:
        {
            ret_status = 1;
            uint32_t poll_delay = SPI_POLL_DELAY;
            uint32_t rx2tx_delay = SPI_RX2TX_DELAY;
            uint32_t tx2rx_delay = SPI_TX2RX_DELAY;

            rsp_data[0] = HIF_SUBID_SPI_POLL_DELAY_GET;
            rsp_data[1] = poll_delay & 0xFF;
            rsp_data[2] = (poll_delay >> 8) & 0xFF;
            rsp_data[3] = (poll_delay >> 16) & 0xFF;
            rsp_data[4] = (poll_delay >> 24) & 0xFF;
            rsp_data[5] = rx2tx_delay & 0xFF;
            rsp_data[6] = (rx2tx_delay >> 8) & 0xFF;
            rsp_data[7] = (rx2tx_delay >> 16) & 0xFF;
            rsp_data[8] = (rx2tx_delay >> 24) & 0xFF;
            rsp_data[9] = tx2rx_delay & 0xFF;
            rsp_data[10] = (tx2rx_delay >> 8) & 0xFF;
            rsp_data[11] = (tx2rx_delay >> 16) & 0xFF;
            rsp_data[12] = (tx2rx_delay >> 24) & 0xFF;  
            rsp_len = 13;
            break;
        }

        default:
            ret = HIF_CMD_STATUS_UNSUPPORT;
        break;
    }
    
    if(ret_status == 0) {
        return HIF_MsgResp(msg, 0, ret);
    } else {
        return HIF_MsgResp(msg, rsp_len, ret);
    }
}


#define HIF_DEVICE_PWRCTRL_ACTIVE        0
#define HIF_DEVICE_PWRCTRL_SLEEP         1
int msghdl_device_pwrctrl(HIF_MsgHdr_t *msg)
{
    uint8_t req_type = *(uint8_t *)(msg + 1);

    if (req_type == HIF_DEVICE_PWRCTRL_ACTIVE) {
        hif_device_wakeup_req();
    } else {
        hif_device_suspend_req();
    }
    return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_SUCCESS);
    return 0;
}

#define HIF_HOST_STATE_ACTIVE            0
#define HIF_HOST_STATE_SLEEP             1
extern void hif_host_state_change(uint8_t ready);
int msghdl_host_state(HIF_MsgHdr_t *msg)
{
    if (msg->length == 1) {
        uint8_t state = *(uint8_t *)(msg + 1);
        hif_host_state_change((state == HIF_HOST_STATE_ACTIVE));
        return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_SUCCESS);
    } else {
        return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_PARAM);
    }
    return 0;
}

#define HIF_HOST_RESET_WARM              (0)
#define HIF_HOST_STATE_COLD              (1)
#define HIF_HOST_STATE_UPGRADE           (2)
#define CONFIG_SYS_REBOOT_DELAY          (100)

int msghdl_host_reboot(HIF_MsgHdr_t *msg)
{
    uint8_t mode = *((uint8_t *)(msg + 1));

    if (mode == HIF_HOST_STATE_COLD) { /* soft reboot */
        HIF_MsgResp(msg, 0, HIF_CMD_STATUS_SUCCESS);

#if (CONFIG_SYS_REBOOT_DELAY > 0)
        vTaskDelay(pdMS_TO_TICKS(CONFIG_SYS_REBOOT_DELAY));
#endif
        HAL_BOARD_Reset(HAL_RESET_SYS);
    } else {
        return HIF_MsgResp(msg, 0, HIF_CMD_STATUS_UNSUPPORT);
    }

    return 0;
}

#endif  /* CONFIG_HIF */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
