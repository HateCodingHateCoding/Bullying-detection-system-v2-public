/**
 ******************************************************************************
 * @file    hal_can.h
 * @brief   can define.
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

#ifndef _HAL_CAN_H
#define _HAL_CAN_H

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Exported types.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_CAN_FD_ENABEL
#define CONFIG_CAN_FD_ENABEL                        0
#endif

/* 0: PLL SOC
 * 1: MSI
 * 2: DCXO
 * 3: PLL DEV
 */
#ifndef CONFIG_CAN_CLK_SRC
#define CONFIG_CAN_CLK_SRC                          2
#endif

/* 0: div 1
 * 1: div 2
 * 2: div 4
 * 3: div 8
 */
#ifndef CONFIG_CAN_CLK_DIV
#define CONFIG_CAN_CLK_DIV                          0
#endif


#ifndef CONFIG_CAN_TRF_IT_ENABLE
#define CONFIG_CAN_TRF_IT_ENABLE                    1
#endif

#ifndef CONFIG_CAN_TRF_POLL_ENABLE
#define CONFIG_CAN_TRF_POLL_ENABLE                  1
#endif



typedef enum {
    CAN_DEFT_ID         = 0U,
    CAN_NUM,
    CAN_INVALID_ID      = 0xFFU
} CAN_ID_t;


typedef struct {

#if CONFIG_CAN_FD_ENABEL
    #define CAN_FD_MODE_DISABLE                     0
    #define CAN_FD_MODE_NON_ISO                     1
    #define CAN_FD_MODE_ISO                         2
    uint32_t    FDMode;

    /* clock freq refs CONFIG_CAN_CLK_SRC and CONFIG_CAN_CLK_DIV
     * default clock src select DCXO
     * default clock freq is 32MHz (fclock = 32MHz)
     * FDNprescaler = FDClkPresc + 1
     * FDTQ = FDNprescaler / fclock
     */
    uint8_t     FDClkPresc;
    /* FDTsjw = FDTQ * FDNsjw
     * FDNsjw = FDSyncJumpWidth + 1
     * FDTimeSeg2 must be greater than FDSyncJumpWidth
     */
    uint8_t     FDSyncJumpWidth;
    /* FDBT (FD BitTime) = FDTQ * FDNbt
     * FDNbt = FDNseg1 + FDNseg2
     * FDNseg1 = FDTimeSeg1 + 2
     * FDNseg2 = FDTimeSeg2 + 1
     */
    uint8_t     FDTimeSeg1;
    uint8_t     FDTimeSeg2;

    uint8_t     TxDelayComp;
    uint8_t     RxDelayComp;

#endif /* CONFIG_CAN_FD_ENABEL */

    /* clock freq refs CONFIG_CAN_CLK_SRC and CONFIG_CAN_CLK_DIV
     * default clock src select DCXO
     * default clock freq is 32MHz (fclock = 32MHz)
     * Nprescaler = ClkPresc + 1
     * TQ = Nprescaler / fclock
     */
    uint8_t     ClkPresc;
    /* Tsjw = TQ * Nsjw
     * Nsjw = SyncJumpWidth + 1
     * TimeSeg2 must be greater than SyncJumpWidth
     */
    uint8_t     SyncJumpWidth;
    /* BT (BitTime) = TQ * Nbt
     * Nbt = Nseg1 + Nseg2
     * Nseg1 = TimeSeg1 + 2
     * Nseg2 = TimeSeg2 + 1
     */
    uint8_t     TimeSeg1;
    uint8_t     TimeSeg2;

    /* The oldest frame will be overwritten 
     * if a new frame is received when RX buffer is full.
     */
    #define CAN_RECV_OVER_MODE_OVERWRITEN           0
    /* The new frame will not be stored 
     * if a new frame is received when RX buffer is full.
     */
    #define CAN_RECV_OVER_MODE_DISCARD              1
    uint8_t     RecvOverMode;

    /* Receive frame FIFO threshold level. from 0 to 2.
     */
    uint8_t     RecvFFOThresh;
} CAN_InitParam_t;

typedef struct {
    uint32_t    ID;             /* if Extended Format bit[0:28], if Standard Format bit[0:10] */
    uint8_t     LEN;            /* data len, if stand mode <= 8, if FD mode <= 64 */
    uint8_t     IDE;            /* 1: Extended Format, 0: Standard Format*/
    uint8_t     RTR;            /* 1: Remote Frame, 0 : Data Frame*/
#if CONFIG_CAN_FD_ENABEL
    uint8_t     FDF;            /* FD Format */
    uint8_t     BRS;            /* Bit Rate Switch */
    uint8_t     ESI;            /* Error State Indicator*/
#endif
} CAN_FrameHeader_t;


typedef enum {
    FRAME_FIFO_STATE_EMPTY          = 0,
    FRAME_FIFO_STATE_LESS_THRESH    = 1,
    FRAME_FIFO_STATE_MORE_THRESH    = 2,
    FRAME_FIFO_STATE_FULL           = 3,
} CAN_FrameFifoState_t;


typedef enum {
    CAN_ERR_OK                      = 0,
    CAN_ERR_BIT                     = 1,
    CAN_ERR_FORM                    = 2,
    CAN_ERR_STUFF                   = 3,
    CAN_ERR_ACK                     = 4,
    CAN_ERR_CRC                     = 5,
    CAN_ERR_OTHER                   = 6,
} CAN_Error_t;


typedef struct {
    uint32_t idVal;
    uint32_t idMsk;
    #define CAN_ACF_MODE_STD_AND_EXT            0
    #define CAN_ACF_MODE_STD_ONLY               1
    #define CAN_ACF_MODE_EXT_ONLY               2
    uint8_t  mode;
    uint8_t  enable;
} CAN_Acf_t;


typedef enum {
    CAN_SET_AFC                     = 0,
    CAN_GET_AFC                     = 1,
    CAN_GET_AFC_SIZE                = 2,
    CAN_SET_SLOW_BAUDRATE           = 3,
    CAN_GET_SLOW_BAUDRATE           = 4,
    CAN_SET_FAST_BAUDRATE           = 5,
    CAN_GET_FAST_BAUDRATE           = 6,
    CAN_SET_FD_MODE                 = 7,
    CAN_GET_FD_MODE                 = 8,
} CAN_Attr_t;


typedef enum {
    CAN_TRANS_MODE_POLL             = 0,
    CAN_TRANS_MODE_INTERRUPT        = 1,
} CAN_TransMode_t;

typedef enum {
    CAN_DIR_TX                      = 0,
    CAN_DIR_RX                      = 1,
} CAN_TransDir_t;

/* Exported constants.
 * ----------------------------------------------------------------------------
 */

/* transfer param, wuto retransfer */
#define CAN_TRANS_AUTO_RTX          0x10
#define CAN_TRANS_ARTX_MSK          0xF0

/* transfer param, transmit one frame or all frame */
#define CAN_TRANS_TX_ONE            0x00
#define CAN_TRANS_TX_ALL            0x01
#define CAN_TRANS_TX_MODE           0x0F

#define CAN_ACF_SIZE                3


#define CAN_INTR_ERR                (1 << 0)
#define CAN_INTR_TX                 (1 << 1)
#define CAN_INTR_RX_THRESH          (1 << 3)
#define CAN_INTR_RX_FULL            (1 << 4)
#define CAN_INTR_RX_OVER            (1 << 5)
#define CAN_INTR_RX                 (1 << 6)
#define CAN_INTR_BUS_ERR            (1 << 7)
#define CAN_INTR_ARBIT_LOST         (1 << 8)
#define CAN_INTR_ERR_PASS           (1 << 9)

#define CAN_INTR_ALL_MSK            (CAN_INTR_ERR | CAN_INTR_TX | CAN_INTR_RX_THRESH | \
                                    CAN_INTR_RX_FULL | CAN_INTR_RX_OVER | CAN_INTR_RX | \
                                    CAN_INTR_ARBIT_LOST | CAN_INTR_ERR_PASS)

/* Exported macro.
 * ----------------------------------------------------------------------------
 */
/* Private macro.
 * ----------------------------------------------------------------------------
 */

/* Exported functions.
 * ----------------------------------------------------------------------------
 */
HAL_Dev_t * HAL_CAN_Init(CAN_ID_t id);

HAL_Status_t HAL_CAN_DeInit(HAL_Dev_t *can);

HAL_Status_t HAL_CAN_Open(HAL_Dev_t *can, const CAN_InitParam_t *param);

HAL_Status_t HAL_CAN_Close(HAL_Dev_t *can);

HAL_Status_t HAL_CAN_WriteFrame(HAL_Dev_t *can,
                                    CAN_FrameHeader_t *phdr,
                                    uint8_t * pdata);

HAL_Status_t HAL_CAN_ReadFrame(HAL_Dev_t *can,
                                CAN_FrameHeader_t *phdr,
                                uint8_t * pdata);

/* transfer param
 * CAN_TRANS_AUTO_RTX
 * CAN_TRANS_TX_ONE or CAN_TRANS_TX_ALL
 */
HAL_Status_t HAL_CAN_StartTransmit(HAL_Dev_t *can, uint32_t trans);

HAL_Status_t HAL_CAN_StopTransmit(HAL_Dev_t *can);

CAN_FrameFifoState_t HAL_CAN_GetFrameFIFOState(HAL_Dev_t *can, CAN_TransDir_t dir);


HAL_Status_t HAL_CAN_SetErrWarnLimit(HAL_Dev_t *can, uint8_t cnt);

int32_t HAL_CAN_GetErrCnt(HAL_Dev_t *can, CAN_TransDir_t dir);

CAN_Error_t HAL_CAN_GetError(HAL_Dev_t *can);

uint32_t HAL_CAN_GetArbitLost(HAL_Dev_t *can);

HAL_Status_t HAL_CAN_SetACF(HAL_Dev_t *can, uint8_t idx, CAN_Acf_t *pacf);

HAL_Status_t HAL_CAN_GetACF(HAL_Dev_t *can, uint8_t idx, CAN_Acf_t *pacf);


HAL_Status_t HAL_CAN_RegisterIRQ(HAL_Dev_t *can, const HAL_Callback_t *irqCb);

uint32_t HAL_CAN_GetIRQFlag(HAL_Dev_t *can, uint32_t irqMsk);

void HAL_CAN_ClearIRQFlag(HAL_Dev_t *can, uint32_t irqMsk);

void HAL_CAN_EnableIRQ(HAL_Dev_t *can, uint32_t irqMsk);

void HAL_CAN_DisableIRQ(HAL_Dev_t *can, uint32_t irqMsk);

uint32_t HAL_CAN_IsEnabledIRQ(HAL_Dev_t *can, uint32_t irqMsk);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_CAN_H */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
