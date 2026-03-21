/**
 ******************************************************************************
 * @file    hal_can.c
 * @brief   can define.
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
#include "hal_os.h"
#include "ll_can.h"
#include "ll_rcc_dev.h"
#include "hal_can.h"
#include "irq.h"


#ifndef CONFIG_CAN_LOG_LEVEL
#define CONFIG_CAN_LOG_LEVEL            LEVEL_DBG
#endif

#define LOG_MODULE                      "CAN"
#define LOG_LEVEL                       CONFIG_CAN_LOG_LEVEL
#include "log.h"

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
typedef struct {
    HAL_Dev_t           device;
    CAN_Acf_t           acf[CAN_ACF_SIZE];
    HAL_Callback_t      CallBack;
} Can_Device_t;


/* Private defines.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_CAN_IRQ_PRIO
#define CONFIG_CAN_IRQ_PRIO                                26
#endif

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
static void CAN_IRQHandler(void * param);

static HAL_Status_t can_HwInit(const CAN_InitParam_t * initParam);

static HAL_Status_t can_WriteFrame(CAN_FrameHeader_t *phdr, uint8_t * pdata);

static HAL_Status_t can_ReadFrame(CAN_FrameHeader_t *phdr, uint8_t * pdata);

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

HAL_Dev_t * HAL_CAN_Init(CAN_ID_t id)
{
    Can_Device_t *pCanDevice = (Can_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_CAN, id);

    if (pCanDevice == NULL) {
        pCanDevice = (Can_Device_t *)HAL_DEV_MemMalloc(sizeof(Can_Device_t));
        if (pCanDevice == NULL) {
            return NULL;
        }

        pCanDevice->device.reg         = CAN_DEV;
        pCanDevice->device.irqNum      = CAN_IRQn;
        pCanDevice->device.irqPrio     = CONFIG_CAN_IRQ_PRIO;
        pCanDevice->device.irqHandler  = CAN_IRQHandler;

        HAL_DEV_SetState((HAL_Dev_t *)pCanDevice, HAL_DEV_STATE_INIT);
        HAL_DEV_Register((HAL_Dev_t *)pCanDevice, HAL_DEV_TYPE_CAN, id);
    }

    return (HAL_Dev_t *)pCanDevice;
}


HAL_Status_t HAL_CAN_DeInit(HAL_Dev_t *can)
{
    HAL_Status_t status = HAL_STATUS_OK;
    
    Can_Device_t *pCanDevice = (Can_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_CAN, 0);

    if (pCanDevice != NULL) {
        HAL_DevState_t state = HAL_DEV_GetState(can);
        if (state != HAL_DEV_STATE_UNINIT) {

            status = HAL_DEV_Unregister((HAL_Dev_t *)pCanDevice);
            if (status == HAL_STATUS_OK) {
                HAL_DEV_MemFree(pCanDevice);
            }
        }
    }

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_CAN_Open(HAL_Dev_t *can, const CAN_InitParam_t *param)
{
    HAL_Status_t status = HAL_STATUS_OK;

    Can_Device_t *pCanDevice = (Can_Device_t *)can;
    if (pCanDevice == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if ((state == HAL_DEV_STATE_INIT) || (state == HAL_DEV_STATE_CLOSE)) {
        status = can_HwInit(param);
        if (status == HAL_STATUS_OK) {
            HAL_DEV_SetState(can, HAL_DEV_STATE_ACTIVE);
        }

        LL_CAN_InterruptDisable(CAN_INTR_ALL_MSK);
        IRQ_AttachDevice(pCanDevice->device.irqNum, can);
        IRQ_Priority(pCanDevice->device.irqNum, pCanDevice->device.irqPrio);
        IRQ_Enable(pCanDevice->device.irqNum);
    }

    return status;
}


HAL_Status_t HAL_CAN_Close(HAL_Dev_t *can)
{
    Can_Device_t *pCanDevice = (Can_Device_t *)can;

    if (pCanDevice != NULL) {
        HAL_DevState_t state = HAL_DEV_GetState(can);
        if ((state != HAL_DEV_STATE_CLOSE) || (state != HAL_DEV_STATE_UNINIT)) {
            HAL_DEV_SetState(can, HAL_DEV_STATE_CLOSE);
        }
    }


    return HAL_STATUS_OK;
}


HAL_Status_t HAL_CAN_WriteFrame(HAL_Dev_t *can,
                                    CAN_FrameHeader_t *phdr,
                                    uint8_t * pdata)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if ((can == NULL) || (phdr == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((pdata == NULL) && (phdr->LEN != 0)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if (state != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }


    LL_CAN_TxBuffTypeSet(LL_CAN_TX_BUFF_SECONDARY);
    status = can_WriteFrame(phdr, pdata);
    if (status == HAL_STATUS_OK) {
        LL_CAN_TxBuffNext();
    }


    return status;
}


HAL_Status_t HAL_CAN_ReadFrame(HAL_Dev_t *can,
                                    CAN_FrameHeader_t *phdr,
                                    uint8_t * pdata)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if ((can == NULL) || (phdr == NULL) || (pdata == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if (state != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }


    status = can_ReadFrame(phdr, pdata);
    if (status == HAL_STATUS_OK) {
        LL_CAN_RxBuffNext();
    }


    return status;
}


HAL_Status_t HAL_CAN_StartTransmit(HAL_Dev_t *can, uint32_t trans)
{
    if (can == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if (state != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    if ((trans & CAN_TRANS_ARTX_MSK) == CAN_TRANS_AUTO_RTX) {
        LL_CAN_SSSModeDisable();
    } else {
        LL_CAN_SSSModeEnable();
    }

    if ((trans & CAN_TRANS_TX_MODE) == CAN_TRANS_TX_ALL) {
        LL_CAN_TxDataEnable(LL_CAN_TX_BUFF_SECONDARY_ALL);
    } else {
        LL_CAN_TxDataEnable(LL_CAN_TX_BUFF_SECONDARY_ONE);
    }
    
    return HAL_STATUS_OK;
}


HAL_Status_t HAL_CAN_StopTransmit(HAL_Dev_t *can)
{
    if (can == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if (state != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    LL_CAN_TxDataDisable(LL_CAN_TX_BUFF_SECONDARY_ALL);

    return HAL_STATUS_OK;
}


CAN_FrameFifoState_t HAL_CAN_GetFrameFIFOState(HAL_Dev_t *can, CAN_TransDir_t dir)
{
    if (dir == CAN_DIR_TX) {
        return LL_CAN_TxBuffStatusGet();
    } else {
        return LL_CAN_RxBuffStatusGet();
    }
}


HAL_Status_t HAL_CAN_SetErrWarnLimit(HAL_Dev_t *can, uint8_t cnt)
{
    if (can == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if (state != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    LL_CAN_ErrorLimitSet(cnt);

    return HAL_STATUS_OK;
}


int32_t HAL_CAN_GetErrCnt(HAL_Dev_t *can, CAN_TransDir_t dir)
{
    if (can == NULL) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if (state != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (dir == CAN_DIR_TX) {
        return LL_CAN_TxErrorCountGet();
    } else {
        return LL_CAN_RxErrorCountGet();
    }
}


CAN_Error_t HAL_CAN_GetError(HAL_Dev_t *can)
{
    return LL_CAN_ErrorCodeGet();
}


uint32_t HAL_CAN_GetArbitLost(HAL_Dev_t *can)
{
    return LL_CAN_ArbitLostCaptGet();
}


HAL_Status_t HAL_CAN_SetACF(HAL_Dev_t *can, uint8_t acfIdx, CAN_Acf_t * pacfCfg)
{
    if ((can == NULL) || (pacfCfg == NULL) || (acfIdx >= CAN_ACF_SIZE)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    HAL_DevState_t state = HAL_DEV_GetState(can);
    if (state != HAL_DEV_STATE_ACTIVE) {
        return HAL_STATUS_INVALID_STATE;
    }

    Can_Device_t *pCanDevice = (Can_Device_t *)can;

    memcpy(&pCanDevice->acf[acfIdx], pacfCfg, sizeof(CAN_Acf_t));

    uint32_t stateMsk = LL_CAN_ACFGetState();
    LL_CAN_ResetHold();

    uint32_t acf = 0;
    uint32_t mode = 0;
    if (pCanDevice->acf[acfIdx].enable) {
        HW_SET_BIT(stateMsk, acfIdx);
        LL_CAN_ACFSetState(stateMsk);

        if (pacfCfg->mode == CAN_ACF_MODE_STD_ONLY) {
            mode = 0x40000000;
        } else if (pacfCfg->mode == CAN_ACF_MODE_EXT_ONLY){
            mode = 0x60000000;
        } else {
            mode = 0x00000000;
        }

        LL_CAN_ACFSetIndex(acfIdx);
        LL_CAN_SelACFMask();
        acf = (pCanDevice->acf[acfIdx].idMsk & 0x1FFFFFFF) | mode;
        LL_CAN_ACFSet(acf);
        LL_CAN_SelACFCode();
        acf = (pCanDevice->acf[acfIdx].idVal & 0x1FFFFFFF) | mode;
        LL_CAN_ACFSet(acf);
    } else {
        HW_CLR_BIT(stateMsk, acfIdx);
        LL_CAN_ACFSetState(stateMsk);
    }

    LL_CAN_ResetRelease();
    
    return HAL_STATUS_OK;
}


HAL_Status_t HAL_CAN_GetACF(HAL_Dev_t *can, uint8_t acfIdx, CAN_Acf_t * pacfCfg)
{
    if ((can == NULL) || (pacfCfg == NULL) || (acfIdx >= CAN_ACF_SIZE)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    Can_Device_t *pCanDevice = (Can_Device_t *)can;

    memcpy(pacfCfg, &pCanDevice->acf[acfIdx], sizeof(CAN_Acf_t));

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_CAN_RegisterIRQ(HAL_Dev_t *can, const HAL_Callback_t *irqCb)
{
    Can_Device_t *pCanDevice = (Can_Device_t *)can;

    if ((pCanDevice == NULL) || (irqCb == NULL)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    pCanDevice->CallBack.cb = irqCb->cb;
    pCanDevice->CallBack.arg = irqCb->arg;


    return HAL_STATUS_OK;
}


uint32_t HAL_CAN_GetIRQFlag(HAL_Dev_t *can, uint32_t irqMsk)
{
    return LL_CAN_InterruptFlagGet(irqMsk);
}


void HAL_CAN_ClearIRQFlag(HAL_Dev_t *can, uint32_t irqMsk)
{
    LL_CAN_InterruptClear(irqMsk);
}


void HAL_CAN_EnableIRQ(HAL_Dev_t *can, uint32_t irqMsk)
{
    LL_CAN_InterruptEnable(irqMsk);
}


void HAL_CAN_DisableIRQ(HAL_Dev_t *can, uint32_t irqMsk)
{
    LL_CAN_InterruptDisable(irqMsk);
}


uint32_t HAL_CAN_IsEnabledIRQ(HAL_Dev_t *can, uint32_t irqMsk)
{
    return LL_CAN_IsInterruptEnabled(irqMsk);
}


#if (CONFIG_CAN_TRF_IT_ENABLE == 1)
static void CAN_IRQHandler(void * param)
{
    Can_Device_t *pCanDevice = (Can_Device_t *)param;
    uint32_t flag = LL_CAN_InterruptFlagGet(CAN_INTR_ALL_MSK);

    if (pCanDevice == NULL) {
        LL_CAN_InterruptClear(flag);
        return ;
    }

    if (pCanDevice->CallBack.cb != NULL) {
        pCanDevice->CallBack.cb(param, pCanDevice->CallBack.arg);
    }
}
#endif


static HAL_Status_t can_WriteFrame(CAN_FrameHeader_t *phdr, uint8_t * pdata)
{
    uint32_t tmpVal = 0;
    uint32_t sizeMsk = 0;


#if CONFIG_CAN_FD_ENABEL
    if (phdr->FDF == 1) {
        switch (phdr->LEN) {
        case 64:
            sizeMsk = 0xF;
            break;

        case 48:
            sizeMsk = 0xE;
            break;

        case 32:
            sizeMsk = 0xD;
            break;

        case 24:
            sizeMsk = 0xC;
            break;

        case 20:
            sizeMsk = 0xB;
            break;

        case 16:
            sizeMsk = 0xA;
            break;

        case 12:
            sizeMsk = 0x9;
            break;

        default:
            if (phdr->LEN <= 8) {
                sizeMsk = phdr->LEN;
            } else {
                return HAL_STATUS_INVALID_PARAM;
            }
        }
    }else {    
        if (phdr->LEN > 8) {
            return HAL_STATUS_INVALID_PARAM;
        }
        sizeMsk = phdr->LEN;
    }
#else
    sizeMsk = phdr->LEN;
#endif


    if (phdr->IDE == 1) {
        tmpVal = phdr->ID & 0x1FFFFFFF;
    } else {
        tmpVal = phdr->ID & 0x7FF;
    }
    LL_CAN_TxDataSet(0, tmpVal);


    tmpVal = 0;
    if (phdr->IDE) {
        tmpVal = 0x80;
    }
    tmpVal |= ((phdr->RTR & 0x1) << 6);
#if CONFIG_CAN_FD_ENABEL
    tmpVal |= ((phdr->FDF & 0x1) << 5) | ((phdr->BRS & 0x1) << 4);
#endif
    tmpVal |= sizeMsk & 0xF;
    LL_CAN_TxDataSet(1, tmpVal);


    for (uint32_t idx = 0; idx < phdr->LEN; idx += 4) {
        tmpVal = *(uint32_t *)&pdata[idx];
        LL_CAN_TxDataSet(2 + (idx >> 2), tmpVal);
    }


    return HAL_STATUS_OK;
}


static HAL_Status_t can_ReadFrame(CAN_FrameHeader_t *phdr, uint8_t * pdata)
{
    uint32_t tmpVal = 0;
    int32_t rcvSize = 0;


    phdr->ID = LL_CAN_RxDataGet(0);
    tmpVal = LL_CAN_RxDataGet(1);

#if CONFIG_CAN_FD_ENABEL
    if (phdr->ID & 0x80000000) {
        phdr->ESI = 1;
    } else {
        phdr->ESI = 0;
    }

    if (tmpVal & (1 << 6)) {
        phdr->FDF = 1;
    } else {
        phdr->FDF = 0;
    }

    if (tmpVal & (1 << 5)) {
        phdr->BRS = 1;
    } else {
        phdr->BRS = 0;
    }

    switch (tmpVal & 0xF) {
    case 0x9:
        rcvSize = 12;
        break;
    case 0xA:
        rcvSize = 16;
        break;
    case 0xB:
        rcvSize = 20;
        break;
    case 0xC:
        rcvSize = 24;
        break;
    case 0xD:
        rcvSize = 32;
        break;
    case 0xE:
        rcvSize = 48;
        break;
    case 0xF:
        rcvSize = 64;
        break;
    default:
        if ((tmpVal & 0xF) <= 8) {
            rcvSize = tmpVal & 0xF;
        } else {
            return HAL_STATUS_INVALID_PARAM;
        }
    }
#else
    rcvSize = tmpVal & 0xF;
#endif

    if (tmpVal & 0x80) {
        phdr->IDE = 1;
        phdr->ID &= 0x1FFFFFFF;
    } else {
        phdr->IDE = 0;
        phdr->IDE &= 0x7FF;
    }

    if (tmpVal & 0x40) {
        phdr->RTR = 1;
    } else {
        phdr->RTR = 0;
    }

    phdr->LEN = rcvSize & 0xFF;
    uint32_t tmpCnt = rcvSize / 4;
    for (uint32_t idx = 0; idx < tmpCnt; idx++) {
        *(uint32_t *)&pdata[idx << 2] = LL_CAN_RxDataGet(2 + idx);
    }

    uint32_t remCnt = rcvSize % 4;
    if (remCnt) {
        tmpVal = LL_CAN_RxDataGet(2 + tmpCnt);    
        for (uint32_t idx = 0; idx < remCnt; idx++) {
            pdata[(tmpCnt << 2) + idx] = tmpVal & 0xFF;
            tmpVal >>= 8;
        }
    }


    return HAL_STATUS_OK;
}


static HAL_Status_t can_HwInit(const CAN_InitParam_t * initParam)
{
    HAL_Status_t status = HAL_STATUS_OK;
    HAL_Pinmux_t canPinmux;

    if ((initParam->SyncJumpWidth > 0x7F) ||
        (initParam->TimeSeg1 > 0xFF) ||
        (initParam->TimeSeg2 > 0x7F) ||
        (initParam->SyncJumpWidth > initParam->TimeSeg2)) {
        return HAL_STATUS_INVALID_PARAM;
    }

#if CONFIG_CAN_FD_ENABEL
    if ((initParam->FDSyncJumpWidth > 0x7F) ||
        (initParam->FDTimeSeg1 > 0xFF) ||
        (initParam->FDTimeSeg2 > 0x7F) ||
        (initParam->FDSyncJumpWidth > initParam->FDTimeSeg2)) {
        return HAL_STATUS_INVALID_PARAM;
    }
#endif


    LOG_DBG("pimux config");
    status = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_CAN, CAN_DEFT_ID, HAL_PINMUX_TYPE_INIT, &canPinmux);
    if (status != HAL_STATUS_OK) {
        LOG_ERR("get pinmux error: %d", status);
        return HAL_STATUS_ERROR;
    }
    HAL_BOARD_SetPinMux(&canPinmux);

    LOG_DBG("clock config");
    LL_RCC_CAN_DisableClock();

    LL_RCC_CAN_IPSet(0);
    LL_RCC_CAN_ModeSet(LL_RCC_CAN_MODE_FD);
    LL_RCC_CAN_Reset();
    
    LL_RCC_CAN_SetClockSource(CONFIG_CAN_CLK_SRC);
    LL_RCC_CAN_SetPrescaler(CONFIG_CAN_CLK_DIV);

    LL_RCC_CAN_EnableClock();


    LOG_DBG("baudrate config");
    LL_CAN_ResetHold();

    LL_CAN_SlowPrescSet(initParam->ClkPresc);
    LL_CAN_SlowSJWSet(initParam->SyncJumpWidth);
    LL_CAN_SlowSeg1Set(initParam->TimeSeg1);
    LL_CAN_SlowSeg2Set(initParam->TimeSeg2);

#if CONFIG_CAN_FD_ENABEL
    LL_CAN_FastPrescSet(initParam->FDClkPresc);
    LL_CAN_FastSJWSet(initParam->FDSyncJumpWidth);
    LL_CAN_FastSeg1Set(initParam->FDTimeSeg1);
    LL_CAN_FastSeg2Set(initParam->FDTimeSeg2);

    if (initParam->FDMode == CAN_FD_MODE_NON_ISO) {
        LL_CAN_FDISOModeEnable();
    } else if (initParam->FDMode == CAN_FD_MODE_NON_ISO) {
        LL_CAN_FDISOModeDisable();
    }

    LL_CAN_TxSampleDelaySet(initParam->TxDelayComp);
    LL_CAN_RxSampleDelaySet(initParam->RxDelayComp);
#endif

    LL_CAN_RxOverflowModeSet(initParam->RecvOverMode);
    LL_CAN_RxBuffThresholdSet(initParam->RecvFFOThresh);

    LL_CAN_ResetRelease();


    return HAL_STATUS_OK;
}



/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

