/**
 ******************************************************************************
 * @file        hal_i2c.c
 * @brief       i2c hardware abstract layer define source.
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
#include "common.h"
#include "ll_i2c.h"
#include "ll_utils.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_gpio.h"

#include "hal_board.h"
#include "hal_os.h"
#include "hal_gpio.h"
#include "hal_i2c.h"

#define LOG_MODULE                      "I2C"
#define LOG_LEVEL                       LEVEL_ERR
#include "log.h"


/* Config macro.
 * ----------------------------------------------------------------------------
 */
#ifndef CONFIG_I2C_MASTER
#define CONFIG_I2C_MASTER                               0
#endif

#ifndef CONFIG_I2C_SLAVE
#define CONFIG_I2C_SLAVE                                1
#endif

#ifndef CONFIG_I2C_1
#define CONFIG_I2C_1                                    0
#endif

#ifndef CONFIG_I2C_TRANS_IT_ENABLE
#define CONFIG_I2C_TRANS_IT_ENABLE                      0
#endif

#ifndef CONFIG_I2C_TRANS_DMA_ENABLE
#define CONFIG_I2C_TRANS_DMA_ENABLE                     0
#endif

#ifndef CONFIG_I2C_ASSERT
#define CONFIG_I2C_ASSERT                               1
#endif

#ifndef CONFIG_I2C_LOG_REG_EN
#define CONFIG_I2C_LOG_REG_EN                           0
#endif

#ifndef CONFIG_I2C_LOG_IO_EN
#define CONFIG_I2C_LOG_IO_EN                            0
#endif

#ifndef CONFIG_I2C_WAKEUP
#define CONFIG_I2C_WAKEUP                               0
#endif


#if ((CONFIG_I2C_WAKEUP >= 0) && (CONFIG_PM == 0))
#ifndef CONFIG_PM
#define CONFIG_PM                                       1
#endif
#endif

#if ((CONFIG_I2C_SLAVE == 1) || (CONFIG_I2C_MASTER == 1))
    #undef  CONFIG_DRIVER_I2C0
    #define CONFIG_DRIVER_I2C0                          1
#elif ((CONFIG_I2C_SLAVE == 0) && (CONFIG_I2C_MASTER == 0))
    #error "Must using CONFIG_I2C_SLAVE or CONFIG_I2C_MASTER enable!"
#endif

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_PM && CONFIG_I2C_WAKEUP && CONFIG_I2C_SLAVE
typedef enum {
    I2C_SDA_PIN_2                             =     GPIO_PIN_2,
    I2C_SDA_PIN_4                             =     GPIO_PIN_4,
    I2C_SDA_PIN_6                             =     GPIO_PIN_6,
    I2C_SDA_PIN_9                             =     GPIO_PIN_9,
    I2C_SDA_PIN_10                            =     GPIO_PIN_10,
    I2C_SDA_PIN_14                            =     GPIO_PIN_14,
    I2C_SDA_PIN_NUM                           =     6U,
} i2c_SdaPin_t;

typedef enum {
    I2C_SCL_PIN_3                             =     GPIO_PIN_3,
    I2C_SCL_PIN_5                             =     GPIO_PIN_5,
    I2C_SCL_PIN_7                             =     GPIO_PIN_7,
    I2C_SCL_PIN_8                             =     GPIO_PIN_8,
    I2C_SCL_PIN_11                            =     GPIO_PIN_11,
    I2C_SCL_PIN_15                            =     GPIO_PIN_15,
    I2C_SCL_PIN_NUM                           =     6U,
} i2c_SclPin_t;

typedef enum {
    I2C_WAKUP_PIN_IDX_0                       =     (0x00U),
    I2C_WAKUP_PIN_IDX_1                       =     (0x01U),
    I2C_WAKUP_PIN_IDX_2                       =     (0x02U),
    I2C_WAKUP_PIN_IDX_3                       =     (0x03U),
    I2C_WAKUP_PIN_IDX_4                       =     (0x04U),
    I2C_WAKUP_PIN_IDX_5                       =     (0x05U),
    I2C_WAKUP_PIN_IDX_ERR                     =     (0xFFU),
} i2c_WakeupPinIdx_t;
#endif

typedef enum {
    I2C_STATE_IDLE                            =     0U,
    I2C_STATE_RX_READY,
    I2C_STATE_TX_READY,
    I2C_STATE_RX_BUSY,
    I2C_STATE_TX_BUSY,
} i2c_State_t;

/* Hardware */
typedef enum {
    I2C_STATUS_ACTIVITY                       =    (HAL_BIT(0)),
    I2C_STATUS_TFNF                           =    (HAL_BIT(1)),
    I2C_STATUS_TFE                            =    (HAL_BIT(2)),
    I2C_STATUS_RFNE                           =    (HAL_BIT(3)),
    I2C_STATUS_RFF                            =    (HAL_BIT(4)),
    I2C_STATUS_MST_ACTIVITY                   =    (HAL_BIT(5)),
    I2C_STATUS_SLV_ACTIVITY                   =    (HAL_BIT(6)),
    I2C_STATUS_SDA_STUCK_NOT_RECOVERED        =    (HAL_BIT(7)),
} i2c_Status_t;

typedef enum {
    I2C_INTR_SCL_STUCK_AT_LOW                 =     (SCL_STUCK_AT_LOW_Msk),
    I2C_INTR_BCNT_TX_TIMEOUT                  =     (SLV_TX_BCNT_TO_Msk),
    I2C_INTR_BCNT_RX_TIMEOUT                  =     (SLV_RX_BCNT_TO_Msk),
    I2C_INTR_BCNT_TX_ERR                      =     (SLV_TX_BCNT_ERR_Msk),
    I2C_INTR_BCNT_RX_ERR                      =     (SLV_RX_BCNT_ERR_Msk),
    I2C_INTR_RESTART                          =     (RESTART_DET_Msk),
    I2C_INTR_GEN_CALL                         =     (GEN_CALL_Msk),
    I2C_INTR_START_DET                        =     (START_DET_Msk),
    I2C_INTR_STOP_DET                         =     (STOP_DET_Msk),
    I2C_INTR_ACTIVITY                         =     (ISR_ACTIVITY_Msk),
    I2C_INTR_RX_DONE                          =     (RX_DONE_Msk),
    I2C_INTR_TX_ABRT                          =     (TX_ABRT_Msk),
    I2C_INTR_RD_REQ                           =     (RD_REQ_Msk),
    I2C_INTR_TX_EMPTY                         =     (TX_EMPTY_Msk),
    I2C_INTR_TX_OVER                          =     (TX_OVER_Msk),
    I2C_INTR_RX_FULL                          =     (RX_FULL_Msk),
    I2C_INTR_RX_OVER                          =     (RX_OVER_Msk),
    I2C_INTR_RX_UNDER                         =     (RX_UNDER_Msk),
} i2c_IntrStatus_t;

typedef enum {
    I2C_ABORT_ALL                             =     (0x1FFFF),
    I2C_ABORT_TX_FLUSH_CNT                    =     (TX_FLUSH_CNT_Msk),
    I2C_ABORT_SDA_STUCK_AT_LOW                =     (ABRT_SDA_STUCK_AT_LOW_Msk),
    I2C_ABORT_USER_ABRT                       =     (ABRT_USER_ABRT_Msk),
    I2C_ABORT_SLV_RD_INTX                     =     (ABRT_SLVRD_INTX_Msk),
    I2C_ABORT_SLV_ARB_LOST                    =     (ABRT_SLV_ARBLOST_Msk),
    I2C_ABORT_SLV_FLUSH_TXFIFO                =     (ABRT_SLVFLUSH_TXFIFO_Msk),
    I2C_ABORT_LOST                            =     (ARB_LOST_Msk),
    I2C_ABORT_MASTER_DIS                      =     (ABRT_MASTER_DIS_Msk),
    I2C_ABORT_10B_RD_NORSTRT                  =     (ABRT_10B_RD_NORSTRT_Msk),
    I2C_ABORT_SBYTE_NORSTRT                   =     (ABRT_SBYTE_NORSTRT_Msk),
    I2C_ABORT_HS_NORSTRT                      =     (ABRT_HS_NORSTRT_Msk),
    I2C_ABORT_SBYTE_ACK_DET                   =     (ABRT_SBYTE_ACKDET_Msk),
    I2C_ABORT_HS_ACK_DET                      =     (ABRT_HS_ACKDET_Msk),
    I2C_ABORT_GCALL_READ                      =     (ABRT_GCALL_READ_Msk),
    I2C_ABORT_GCALL_NOACK                     =     (ABRT_GCALL_NOACK_Msk),
    I2C_ABORT_TXDATA_NOACK                    =     (ABRT_TXDATA_NOACK_Msk),
    I2C_ABORT_10ADDR2_NOACK                   =     (ABRT_10ADDR2_NOACK_Msk),
    I2C_ABORT_10ADDR1_NOACK                   =     (ABRT_10ADDR1_NOACK_Msk),
    I2C_ABORT_7ADDR_NOACK                     =     (ABRT_7B_ADDR_NOACK_Msk),
} i2c_TxAbortStatu_t;


#if (CONFIG_PM && CONFIG_I2C_WAKEUP)
typedef enum {
    I2C_WAKEUPSTATUS_DISABLE,
    I2C_WAKEUPSTATUS_ENABLE,
    I2C_WAKEUPSTATUS_ERROR,
} i2c_WakeupStatus_t;

typedef struct {
    bool                             wakeupEnable;
    bool                             thresholdReq;
    i2c_WakeupStatus_t               wakeupStatus;
    HAL_Callback_t                   wakeupCb;
    uint32_t                         wakeupThreshold;
    uint8_t                          sclPinIdx;
    uint8_t                          sdaPinIdx;
    uint8_t                          sclPin;
    uint8_t                          sdaPin;
} i2c_WakeupConf_t;
#endif


typedef struct {
    I2C_TransfMode_t                  tranfMode;

    uint8_t                          *buff;
    size_t                            size;
    uint32_t                          offset;
    bool                              stop;
} i2c_Msg_t;

typedef struct {
    uint32_t                          bitrate;
    uint32_t                          freq;
    I2C_Mode_t                        mode;
    I2C_Speed_t                       speed;
    I2C_AddrWidth_t                   addrWidth;
    uint16_t                          devAddr;
} i2c_Config_t;

typedef struct {
    uint32_t TL_SET;
    uint32_t IMR;
}i2c_reg_backup_t;

typedef struct {
    HAL_Dev_t                         device;

    int32_t                           clock;
    HAL_Pinmux_t                      pinMux;
    HAL_Pinmux_t                      dePinMux;
    i2c_Config_t                      config;
    I2C_Callback_t                    busErrCb;
    struct i2c_timing_t               timing;

    i2c_State_t                       State;
    i2c_Msg_t                         tx_msg;
    i2c_Msg_t                         rx_msg;


#if (CONFIG_I2C_TRANS_IT_ENABLE)
    #if CONFIG_I2C_SLAVE
    I2C_Callback_t                    slave_irqHandler;
    #endif
    #if CONFIG_I2C_MASTER
    I2C_Callback_t                    master_irqHandler;
    #endif
    HAL_Semaphore                     txSem;
    HAL_Semaphore                     rxSem;
#endif

#if CONFIG_I2C_TRANS_DMA_ENABLE
    I2C_Callback_t                    rx_dmaCb;
    I2C_Callback_t                    tx_dmaCb;
#endif

#if CONFIG_PM
    i2c_reg_backup_t                  reg_backup;
    i2c_WakeupMode_t                  wk_mode;
    #if CONFIG_I2C_WAKEUP
    i2c_WakeupConf_t                  wakeupConf;
    #endif
#endif
} I2C_Device_t;


/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define I2C_ID_GET(id)                                       (id >> 8)
#define I2C_TYPE_GET(id)                                     (id & 0XFF)


#define I2C_CLK_RISE_VALUE                                   (8000000UL)
#define I2C_CLK_DOWN_VALUE                                   (100000000UL)


#define MASTER_IO_NUM                                        (11)
#define SLAVE_IO_NUM                                         (12)

/* PA2, PA3 */
#define I2C_PIN_MODE0                                        (0)
/* PA4, PA5 */
#define I2C_PIN_MODE1                                        (1)
/* PA6, PA7 */
#define I2C_PIN_MODE2                                        (2)
/* PA8, PA9 */
#define I2C_PIN_MODE3                                        (3)
/* PA10, PA11 */
#define I2C_PIN_MODE4                                        (4)
/* PA14, PA15 */
#define I2C_PIN_MODE5                                        (5)

/* Private macros.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_I2C_LOG_IO_EN
#define I2C_MASTER_DBG_IO(mst_flip)                         \
        do {                                                \
            LOG_IO(0x123456, MASTER_IO_NUM, 0, mst_flip);   \
        } while (0)
#define I2C_SLAVE_DBG_IO(slv_flip)                          \
        do {                                                \
            LOG_IO(0x123456, SLAVE_IO_NUM,  0, slv_flip);   \
        } while (0)

#else
#define I2C_MASTER_DBG_IO(mst_flip)
#define I2C_SLAVE_DBG_IO(slv_flip)
#endif


#if CONFIG_I2C_LOG_REG_EN
#define I2C_LOG_REG(reg) LOG_DBG("["#reg"]""0x%08x.", (uint32_t)reg)

#define I2C_LOG_CLK_REG(i2c, msg)                       \
    do {                                                \
        LOG_DBG("%s", msg);                             \
        I2C_LOG_REG(i2c->SS_SCL_CNT_SET);               \
        I2C_LOG_REG(i2c->FS_SCL_CNT_SET);               \
        I2C_LOG_REG(i2c->HS_SCL_CNT_SET);               \
        I2C_LOG_REG(i2c->IC_FS_SPKLEN);                 \
        I2C_LOG_REG(i2c->SDA_HOLD_SETUP);               \
        LOG_DBG("\n");                                  \
    } while (0)

#define I2C_LOG_ALL_REG(i2c, msg)                       \
    do {                                                \
        LOG_DBG("%s", msg);                             \
        I2C_LOG_REG(i2c->CR);                           \
        I2C_LOG_REG(i2c->ENABLE);                       \
        I2C_LOG_REG(i2c->SR);                           \
        I2C_LOG_REG(i2c->SAR);                          \
        I2C_LOG_REG(i2c->TAR);                          \
        I2C_LOG_REG(i2c->ISR);                          \
        I2C_LOG_REG(i2c->IMR);                          \
        I2C_LOG_REG(i2c->ICR);                          \
        I2C_LOG_REG(i2c->TX_ABRT_SOURCE);               \
        I2C_LOG_REG(i2c->TL_SET);                       \
        I2C_LOG_REG(i2c->OE_RD);                        \
        I2C_LOG_REG(i2c->HS_MADDR);                     \
        I2C_LOG_REG(i2c->DMA_CR);                       \
        LOG_DBG("\n");                                  \
    } while (0)
#else
#define I2C_LOG_REG(reg)
#define I2C_LOG_CLK_REG(i2c, msg)
#define I2C_LOG_ALL_REG(i2c, msg)
#endif


/* Private variables.
 * ----------------------------------------------------------------------------
 */
#if CONFIG_PM && CONFIG_I2C_WAKEUP && CONFIG_I2C_SLAVE
static const uint8_t i2c_wakeup_sdapin_list[I2C_SDA_PIN_NUM][I2C_LINE_NUM] = {
        { I2C_SDA_PIN_2,    GPIOA_P2_F4_TWI0_SDA  },
        { I2C_SDA_PIN_4,    GPIOA_P4_F3_TWI0_SDA  },
        { I2C_SDA_PIN_6,    GPIOA_P6_F4_TWI1_SDA  },
        { I2C_SDA_PIN_9,    GPIOA_P9_F3_TWI1_SDA  },
        { I2C_SDA_PIN_10,   GPIOA_P10_F2_TWI1_SDA },
        { I2C_SDA_PIN_14,   GPIOA_P14_F2_TWI0_SDA },
    };

static const uint8_t i2c_wakeup_sclpin_list[I2C_SCL_PIN_NUM][I2C_LINE_NUM] = {
        { I2C_SCL_PIN_3,    GPIOA_P3_F4_TWI0_SCL  },
        { I2C_SCL_PIN_5,    GPIOA_P5_F3_TWI0_SCL  },
        { I2C_SCL_PIN_7,    GPIOA_P7_F4_TWI1_SCL  },
        { I2C_SCL_PIN_8,    GPIOA_P8_F3_TWI1_SCL  },
        { I2C_SCL_PIN_11,   GPIOA_P11_F2_TWI1_SCL },
        { I2C_SCL_PIN_15,   GPIOA_P15_F2_TWI0_SCL },
    };
#endif

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
/*
 * POLLING.
 */
#if CONFIG_I2C_MASTER
static int32_t i2c_Master_WritePoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec);
static int32_t i2c_Master_ReadPoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec);
#endif

#if CONFIG_I2C_SLAVE
static int32_t i2c_Slave_WritePoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec);
static int32_t i2c_Slave_ReadPoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec);
#endif

/*
 * INTRRUPT.
 */
#if (CONFIG_I2C_MASTER && CONFIG_I2C_TRANS_IT_ENABLE)
static int32_t i2c_Master_WriteIntr(I2C_Device_t *pi2cDev, uint32_t msec);
static int32_t i2c_Master_ReadIntr(I2C_Device_t *pi2cDev, uint32_t msec);
#endif

#if (CONFIG_I2C_SLAVE && CONFIG_I2C_TRANS_IT_ENABLE)
static int32_t i2c_Slave_WriteIntr(I2C_Device_t *pi2cDev, uint32_t msec);
static int32_t i2c_Slave_ReadIntr(I2C_Device_t *pi2cDev, uint32_t msec);
#endif

/*
 * DMA.
 */
#if (CONFIG_I2C_MASTER && CONFIG_I2C_TRANS_DMA_ENABLE)
static int32_t i2c_Master_WriteDMA(I2C_Device_t *pi2cDev, uint32_t msec);
static int32_t i2c_Master_ReadDMA(I2C_Device_t *pi2cDev, uint32_t msec);
#endif

#if (CONFIG_I2C_SLAVE && CONFIG_I2C_TRANS_DMA_ENABLE)
static int32_t i2c_Slave_WriteDMA(I2C_Device_t *pi2cDev, uint32_t msec);
static int32_t i2c_Slave_ReadDMA(I2C_Device_t *pi2cDev, uint32_t msec);
#endif


/* ISR Handler */
static int32_t i2c_TxAbortHandler(I2C_Device_t *pi2cDev, uint32_t abortMask);

#if (CONFIG_I2C_MASTER && CONFIG_I2C_TRANS_IT_ENABLE)
static uint32_t i2c_Master_WriteTxFifo(I2C_Device_t *pi2cDev);
static uint32_t i2c_Master_ReadRxFifoPre(I2C_Device_t *pi2cDev);
static void     i2c_Master_ReadRxFifo(I2C_Device_t *pi2cDev);
#endif

#if (CONFIG_I2C_SLAVE && CONFIG_I2C_TRANS_IT_ENABLE)
static void i2c_Slave_WriteTxFifo(I2C_Device_t *pi2cDev);
static void i2c_Slave_ReadRxFifo(I2C_Device_t *pi2cDev);
#endif


/*
 * PM  && Wakeup.
 */
#if (CONFIG_PM == 1) && (CONFIG_I2C_WAKEUP == 1) && (CONFIG_I2C_SLAVE == 1)
static HAL_Status_t i2c_GetWakeupPinIdx(HAL_Pinmux_t *pi2cPinMux, i2c_WakeupConf_t *pwkupCfg);
static HAL_Status_t i2c_WakeupConfig(I2C_Device_t *pi2cDev);
static HAL_Status_t i2c_WakeupEnable(I2C_Device_t *pi2cDev);
static HAL_Status_t i2c_WakeupDisable(I2C_Device_t *pi2cDev);
#endif


/*
 * utils.
 */
static int32_t i2c_CheckBusError(i2c_dev_t *i2cReg)
{
    uint32_t temp_msk = 0U;

    /* SDA Occurs error */
    temp_msk = LL_I2C_Get_Intr_RawStat(i2cReg);
    if (0 != (temp_msk & TX_ABRT_Msk)) {
        /* Check SDA stuck at low */
        temp_msk = LL_I2C_Get_Tx_Abort_Src(i2cReg);
        if (0 != (temp_msk & ABRT_SDA_STUCK_AT_LOW_Msk)) {
            LOG_ERR("SDA stuck at low");
            return HAL_STATUS_ERROR;
        }
    }

    /* SCL Occurs error */
    temp_msk = LL_I2C_Get_Intr_RawStat(i2cReg);
    if (0 != (temp_msk & SCL_STUCK_AT_LOW_Msk)) {
        LOG_ERR("SCL stuck at low");
        return HAL_STATUS_ERROR;
    }
    return 0;
}

#if CONFIG_I2C_MASTER
static int32_t i2c_CheckBusIdle(i2c_dev_t *i2cReg, uint32_t msec)
{
    uint32_t end_tick =  HAL_BOARD_GetTime(HAL_TIME_MS) + HAL_MSecsToTicks(msec);

    while (0 != LL_I2C_IsActivity(i2cReg)) {
        if (HAL_BOARD_GetTime(HAL_TIME_MS) >= end_tick) {
            return -1;
        }
    }
    return 0;
}
#endif

/* Waiting status timeout */
static int32_t i2c_WaitIntrStatus(i2c_dev_t *i2cReg, i2c_IntrStatus_t status, uint32_t msec)
{
    uint32_t end_tick =  HAL_BOARD_GetTime(HAL_TIME_MS) + HAL_MSecsToTicks(msec);

    while (!(LL_I2C_Get_Intr_RawStat(i2cReg) & (status))) {
        if (HAL_BOARD_GetTime(HAL_TIME_MS) >= end_tick) {
            return -1; /* return due to timeout */
        }
    }
    return 0;
}

static bool i2c_Check_Invalid_Addr(uint16_t addr, I2C_AddrWidth_t addrWidth)
{
    if (addrWidth == I2C_ADDR_WIDTH_10BIT) {
        return addr <= I2C_10BITADDR_MASK ? true : false;
    } else {
        return (addr >= 0x03 && addr <= I2C_7BITADDR_MASK) ? true : false;
    }
}

// static I2C_Speed_t i2c_BitrateMapSpeed(uint32_t bitrate)
// {
//     switch (bitrate) {
//     case I2C_BITRATE_STANDARD:
//         return I2C_SPEED_STANDARD;
//     case I2C_BITRATE_FAST:
//         return I2C_SPEED_FAST;
//     case I2C_BITRATE_FAST_PLUS:
//         return I2C_SPEED_FAST_PLUS;
//     default:
//         LOG_ERR("Invalid I2C bit rate value (%d)\n", bitrate);
//         return -1;
//     }
// }

static int i2c_SpeedMapBitrate(I2C_Speed_t speed)
{
    switch (speed) {
    case I2C_SPEED_STANDARD:
        return I2C_BITRATE_STANDARD;
    case I2C_SPEED_FAST:
        return I2C_BITRATE_FAST;
    case I2C_SPEED_FAST_PLUS:
        return I2C_BITRATE_FAST_PLUS;
    default:
        LOG_ERR("Invalid I2C speed value\n");
        return -1;
    }
}

__HAL_STATIC_INLINE void i2c_ClockDisable(void)
{
    LL_RCC_I2C0_Reset();
    LL_RCC_I2C0_DisableBusClock();
    LL_RCC_I2C0_DisableClock();
}

__HAL_STATIC_INLINE void i2c_ClockEnable(uint16_t clkSrc, uint16_t clkPers)
{
    LL_RCC_I2C0_SetClockSource(clkSrc);
    LL_RCC_I2C0_SetPrescaler(clkPers);
    LL_RCC_I2C0_DisableBusClock();
    LL_RCC_I2C0_DisableClock();
    LL_RCC_I2C0_Reset();
    LL_RCC_I2C0_EnableBusClock();
    LL_RCC_I2C0_EnableClock();
}

static int32_t i2c_SpeedConfig(i2c_dev_t *i2cReg, I2C_Speed_t speed, const struct i2c_timing_t *ptiming)
{
    /* Configure speed register */
    switch (speed) {
    case I2C_SPEED_STANDARD:
        LL_I2C_Set_Sspeed_Timing(i2cReg, ptiming);
        LL_I2C_Set_SpeedMode(i2cReg, LL_I2C_STD_SPEED_MODE);
        LOG_INF("speed std");
        break;

    case I2C_SPEED_FAST:
        LL_I2C_Set_Fspeed_Timing(i2cReg, ptiming);
        LL_I2C_Set_SpeedMode(i2cReg, LL_I2C_FAST_SPEED_MODE);
        LOG_INF("speed fast");
        break;

    case I2C_SPEED_FAST_PLUS:
        LL_I2C_Set_Fspeed_Timing(i2cReg, ptiming);
        LL_I2C_Set_SpeedMode(i2cReg, LL_I2C_FAST_SPEED_MODE);
        LOG_INF("speed fast plus");
        break;

    default:
        LOG_INF("Not Support Speed!");
        return -EINVAL;
        break;
    }
    return 0;
}

#if (CONFIG_I2C_MASTER == 1)
__HAL_STATIC_INLINE void i2c_MasterConfig(i2c_dev_t *i2cReg, uint16_t targetAddr, I2C_AddrWidth_t addrWidth)
{
    while (LL_I2C_IsAble(i2cReg)) {
    }
    LL_I2C_SetTxBlockDisable(i2cReg);
    LL_I2C_Set_MasterMode(i2cReg);
    if (addrWidth == I2C_ADDR_WIDTH_10BIT) {
        LL_I2C_SetMaster_10Bit_AddrMode(i2cReg);
        LL_I2C_SetMaster_Target_Addr(i2cReg, targetAddr);
    } else {
        LL_I2C_SetMaster_7Bit_AddrMode(i2cReg);
        LL_I2C_SetMaster_Target_Addr(i2cReg, targetAddr);
    }
    LL_I2C_EnableRxFifoHold(i2cReg);
    LL_I2C_EnableTxFifoEmpty(i2cReg);
    LL_I2C_Set_Timeout_SCL_StuckAtLow(i2cReg, I2C_TIMEOUT_DEFAULT_VALUE);
    LL_I2C_Set_Timeout_SDA_StuckAtLow(i2cReg, I2C_TIMEOUT_DEFAULT_VALUE);
    LL_I2C_Set_HighSpeedMsterCode(i2cReg, I2C_HIGH_SPEED_MASTER_CODE);
}
#endif

#if (CONFIG_I2C_SLAVE == 1)
__HAL_STATIC_INLINE void i2c_SlaveConfig(i2c_dev_t *i2cReg, uint16_t ownAddr, I2C_AddrWidth_t addrWidth)
{
    while (LL_I2C_IsAble(i2cReg)) {
    }
    LL_I2C_SetTxBlockEnable(i2cReg);
    LL_I2C_Set_SlaveMode(i2cReg);
    if (addrWidth == I2C_ADDR_WIDTH_10BIT) {
        LL_I2C_SetSlave_10Bit_AddrMode(i2cReg);
        LL_I2C_SetSlave_Slave_Addr(i2cReg, ownAddr);
    } else {
        LL_I2C_SetSlave_7Bit_AddrMode(i2cReg);
        LL_I2C_SetSlave_Slave_Addr(i2cReg, ownAddr);
    }
    LL_I2C_EnableTxFifoEmpty(i2cReg);
    LL_I2C_EnableRxFifoHold(i2cReg);
    LL_I2C_SetTxBlockDisable(i2cReg);
    LL_I2C_Set_Timeout_SCL_StuckAtLow(i2cReg, I2C_TIMEOUT_DEFAULT_VALUE);
    LL_I2C_Set_Timeout_SDA_StuckAtLow(i2cReg, I2C_TIMEOUT_DEFAULT_VALUE);
}
#endif


int32_t i2c_TimmingCalcu(I2C_Speed_t speed, uint32_t fclock, uint32_t bitrate, struct i2c_timing_t *timming)
{
    uint32_t lcnt, hcnt, spklen, sda_hold, sda_setup;

    if ((fclock == 0) || (speed < 0) || (bitrate == 0) || (timming == NULL)) {
        return -EINVAL;
    }

    // LOG_DBG("fclock %d speed %d bitrate %d \n", fclock, speed, bitrate);
    // lcnt = ((fclock / bitrate) >> 1) -1 - fclock/I2C_CLK_RISE_DOWN_VALUE;
    lcnt = ((fclock / bitrate) >> 1) - fclock / I2C_CLK_RISE_VALUE;

    if ((bitrate * 30) > fclock) {
        return -1;
    }

    switch (speed) {
    case I2C_SPEED_STANDARD:
        spklen = 1;
        sda_hold = fclock / 3000000;
        sda_setup = fclock / 4000000 + 2;
        if ((spklen == 0) | (sda_hold == 0) | ((sda_hold + sda_setup + 1) >= lcnt)) {
            return -2;
        }
        break;
    case I2C_SPEED_FAST:
        spklen = fclock / 20000000;
        sda_hold = fclock / 3000000;
        sda_setup = fclock / 10000000 + 2;
        if ((spklen == 0) | (sda_hold == 0) | ((sda_hold + sda_setup + 1) >= lcnt)) {
            return -3;
        }
        break;
    case I2C_SPEED_FAST_PLUS:
        spklen = fclock / 20000000;
        sda_hold = fclock / 100000000;
        sda_setup = fclock / 20000000 + 2;
        if ((spklen == 0) | ((sda_hold + sda_setup + 1) >= lcnt)) {
            return -4;
        }
        break;
    default:
        return -5;
        break;
    }

    if (((fclock / bitrate) >> 1) <= (7 + spklen + fclock / I2C_CLK_DOWN_VALUE)) {
        return -6;
    }

    hcnt = (((fclock / bitrate) >> 1) - 3- spklen + (fclock / I2C_CLK_DOWN_VALUE));

    timming->scl_hcnt   =  hcnt;
    timming->scl_lcnt   =  lcnt;
    timming->scl_spklen =  spklen;
    timming->sda_hold   =  sda_hold << 16 | (sda_hold + 1);
    timming->sda_setup  =  sda_setup;

    LOG_DBG("timming %04X %04X %02X %04X %02X", timming->scl_hcnt, timming->scl_lcnt,
                         timming->scl_spklen, timming->sda_hold, timming->sda_setup);
    return 0;
}

static int32_t i2c_Config(i2c_dev_t *i2cReg, const i2c_Config_t *cfg, struct i2c_timing_t *ptiming)
{
    int32_t ret = 0;

    /* Disable I2C */
    LL_I2C_Close(i2cReg);

    /* Configure i2c speed */
    ret = i2c_SpeedConfig(i2cReg, cfg->speed, ptiming);
    if (ret) {
        return ret;
    }

    /* Configure control register */
    switch (cfg->mode) {
    #if (CONFIG_I2C_MASTER == 1)
    case I2C_MODE_MASTER:
        i2c_MasterConfig(i2cReg, cfg->devAddr, cfg->addrWidth);
        break;
    #endif

    #if (CONFIG_I2C_SLAVE == 1)
    case I2C_MODE_SLAVE:
        i2c_SlaveConfig(i2cReg, cfg->devAddr, cfg->addrWidth);
        break;
    #endif

    default:
        ret = -1;
        break;
    }

    return ret;
}


#if CONFIG_PM
static HAL_Status_t i2c_pm_suspend(HAL_Dev_t *i2c)
{
    HAL_Status_t     status         =    HAL_STATUS_OK;
    I2C_Device_t     *pi2cDevice    =    (I2C_Device_t *)i2c;

    if (pi2cDevice->wk_mode == I2C_WK_MODE_AUTO) {
         status = HAL_i2c_PM_Suspend(i2c);
    }

#if CONFIG_I2C_SLAVE && CONFIG_I2C_WAKEUP
    if (pi2cDevice->wakeupConf.wakeupStatus == I2C_WAKEUPSTATUS_ENABLE) {
        if (pi2cDevice->wakeupConf.wakeupThreshold == 0) {
            pi2cDevice->wakeupConf.wakeupThreshold = pi2cDevice->config.devAddr;
        }

        status = i2c_WakeupEnable(pi2cDevice);
        if (status != HAL_STATUS_OK) {
            return HAL_STATUS_ERROR;
        }
    }
#endif

    return status;
}


static HAL_Status_t i2c_pm_resume(HAL_Dev_t *i2c)
{
    HAL_Status_t     status         =    HAL_STATUS_OK;
    I2C_Device_t     *pi2cDevice    =    (I2C_Device_t *)i2c;

#if CONFIG_I2C_SLAVE && CONFIG_I2C_WAKEUP
    if (pi2cDevice->wakeupConf.wakeupStatus == I2C_WAKEUPSTATUS_ENABLE) {
        status = i2c_WakeupDisable(pi2cDevice);
        if (status != HAL_STATUS_OK) {
            return HAL_STATUS_ERROR;
        }
    }
#endif

    if(pi2cDevice->wk_mode == I2C_WK_MODE_AUTO) {
         status = HAL_i2c_PM_Resume(i2c);
    }

    return status;
}

static int i2c_pm_action(void *dev, enum pm_device_action action)
{
    HAL_Status_t status = HAL_STATUS_ERROR;

    switch (action) {
    case PM_DEVICE_ACTION_RESUME:
        status = i2c_pm_resume((HAL_Dev_t *)dev);
        break;

    case PM_DEVICE_ACTION_SUSPEND:
        status = i2c_pm_suspend((HAL_Dev_t *)dev);
        break;

    default:
        break;
    }

    return (status == HAL_STATUS_OK) ? 0 : (-1);
}

HAL_Status_t HAL_i2c_PM_Suspend(HAL_Dev_t *i2c)
{
    HAL_Status_t     status         =    HAL_STATUS_OK;
    I2C_Device_t     *pi2cDevice    =    (I2C_Device_t *)i2c;
    i2c_dev_t        *i2cReg        =    (i2c_dev_t *)(pi2cDevice->device.reg);

    pi2cDevice->reg_backup.TL_SET = i2cReg->TL_SET;
    pi2cDevice->reg_backup.IMR = i2cReg->IMR;

    LL_I2C_Close(i2cReg);

    /* Disable I2C Bus(APB0) clock */
    i2c_ClockDisable();

    /* I2C GPIO Disable */
    status = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_I2C, I2C_ID_GET(i2c->id),
                             HAL_PINMUX_TYPE_DEINIT, &pi2cDevice->dePinMux);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    status = HAL_BOARD_SetPinMux(&pi2cDevice->dePinMux);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    return status;
}

HAL_Status_t HAL_i2c_PM_Resume(HAL_Dev_t *i2c)
{
    HAL_Status_t     status         =    HAL_STATUS_OK;
    I2C_Device_t     *pi2cDevice    =    (I2C_Device_t *)i2c;
    i2c_dev_t        *i2cReg        =    (i2c_dev_t *)(pi2cDevice->device.reg);

    /* Enable I2C Bus(APB0) clock */
    i2c_ClockEnable(LL_RCC_I2C0_APB0, LL_RCC_I2C0_DIV_1);

    /* Close to config */
    LL_I2C_Close(i2cReg);

    /* I2C GPIO AF */
    status = HAL_BOARD_SetPinMux(&pi2cDevice->pinMux);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    /* Config I2C Hardware */
    int ret = i2c_Config(i2cReg, &pi2cDevice->config, &pi2cDevice->timing);
    if (ret != 0) {
        return HAL_STATUS_ERROR;
    }

    i2cReg->TL_SET = pi2cDevice->reg_backup.TL_SET;
    i2cReg->IMR = pi2cDevice->reg_backup.IMR;

    /* Controller enable */
    LL_I2C_Enable(i2cReg);

    return status;
}


#endif

#if (CONFIG_I2C_TRANS_IT_ENABLE)
static HAL_Status_t i2c_SemaphoreCreate(HAL_Semaphore *psem)
{
    HAL_Status_t status = HAL_STATUS_OK;

    if (HAL_SemaphoreIsValid(psem) == 0) {
        status = HAL_SemaphoreInitBinary(psem);
    }

    return status;
}
#endif

static uint32_t    i2c_FlushRxFifo(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);
    uint32_t      rxNum               =    0U;
    uint8_t       flushData           =    0U;

    while ((!LL_I2C_isRx_Fifo_Empty(i2cReg))) {
        flushData = LL_I2C_GetCmdData(i2cReg);
        rxNum++;
    }

    UNUSED_PARAMETER(flushData);

    return rxNum;
}

uint32_t     i2c_GetEnableIRQ(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);
    uint32_t      intrMask            =    LL_I2C_Get_Intr_RawStat(i2cReg);
    uint32_t      enIrqMask           =    0;

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (intrMask & (LL_I2C_INTR_START_DET_MASK | LL_I2C_INTR_RESTART_DET_MASK)) {
        enIrqMask |= I2C_IRQSTATUS_START;
    }

    if (intrMask & LL_I2C_INTR_STOP_MASK) {
        enIrqMask |= I2C_IRQSTATUS_STOP;
    }

    if (intrMask & LL_I2C_INTR_RD_REQ_MASK) {
        enIrqMask |= I2C_IRQSTATUS_READ_REQ;
    }

    if (intrMask & LL_I2C_INTR_RX_DONE_MASK) {
        enIrqMask |= I2C_IRQSTATUS_READ_DONE;
    }

    if (intrMask & LL_I2C_INTR_TX_EMPTY_MASK) {
        enIrqMask |= I2C_IRQSTATUS_TX_FIFO_EMPTY;
    }

    if (intrMask & LL_I2C_INTR_RX_FULL_MASK) {
        enIrqMask |= I2C_IRQSTATUS_RX_FIFO_FULL;
    }

    if (intrMask & (LL_I2C_INTR_TX_OVRE_MASK | LL_I2C_INTR_TX_ABRT_MASK
                    | LL_I2C_INTR_RX_OVER_MASK | LL_I2C_INTR_RX_UNDER_MASK)) {
        enIrqMask |= I2C_IRQSTATUS_ERROR;
    }


    return enIrqMask;
}

/* Private functions prototypes(ISR).
 * ----------------------------------------------------------------------------
 */
/** Bug log:
 *  1. Master Read Slave more data lead to slave pull SCL stuck at low.
 */
#if (CONFIG_I2C_SLAVE && CONFIG_I2C_TRANS_IT_ENABLE)
__HAL_STATIC_INLINE void I2C_SlaveIRQHandler(void *dev, uint32_t occurIntr)
{
    I2C_Device_t *pi2cDevice      =     (I2C_Device_t *)dev;
    i2c_dev_t    *i2cReg          =     (i2c_dev_t *)(pi2cDevice->device.reg);
    i2c_Msg_t    *ptxMsg          =     &pi2cDevice->tx_msg;
    i2c_Msg_t    *prxMsg          =     &pi2cDevice->rx_msg;

//    if ((occurIntr & I2C_INTR_START_DET) || (occurIntr & I2C_INTR_RESTART)) {
//        LL_I2C_ClearStartDet_Intr(i2cReg);
//        LOG_DBG("start cond");
//    }

    /* Tx */
    if ((occurIntr & I2C_INTR_RD_REQ)) {
        /* Slave Put Data in Tx Empty IRQ */
        LL_I2C_Enable_Intr_TXE(i2cReg);

        LL_I2C_ClearReadReq_Intr(i2cReg);
    }

    if ((occurIntr & I2C_INTR_TX_EMPTY)) {
        if (!LL_I2C_IsActivity(i2cReg)) {
            LL_I2C_Disable_Intr_TXE(i2cReg);
            return;
        }

        if (ptxMsg->size > 0) {
            /* write data */
            i2c_Slave_WriteTxFifo(pi2cDevice);
        }

        /* Not Need clear */
    }

    /* RX */
    if ((occurIntr & I2C_INTR_RX_FULL)) {
        if (prxMsg->size > 0) {
            /* Read data */
            i2c_Slave_ReadRxFifo(pi2cDevice);
        }

        /* Not Need clear */
    }

//    if (occurIntr & I2C_INTR_RX_DONE) {
//        LL_I2C_ClearRxDone_Intr(i2cReg);
//    }

    if ((occurIntr & I2C_INTR_STOP_DET)) {
        if (ptxMsg->offset >= ptxMsg->size) {
            // LL_I2C_Disable_Intr_Mask(i2cReg, LL_I2C_INTR_STOP_MASK);
            HAL_SemaphoreRelease(&pi2cDevice->txSem);
        }
        LL_I2C_ClearStopDet_Intr(i2cReg);
    }

}
#endif


#if (CONFIG_I2C_MASTER && CONFIG_I2C_TRANS_IT_ENABLE)
__HAL_STATIC_INLINE void I2C_MasterIRQHandler(void *dev, uint32_t occurIntr)
{
    I2C_Device_t *pi2cDevice      =     (I2C_Device_t *)dev;
    i2c_dev_t    *i2cReg          =     (i2c_dev_t *)(pi2cDevice->device.reg);
    i2c_Msg_t    *ptxMsg          =     &pi2cDevice->tx_msg;
    i2c_Msg_t    *prxMsg          =     &pi2cDevice->rx_msg;

    if ((occurIntr & I2C_INTR_STOP_DET)) {
        HAL_SemaphoreRelease(&pi2cDevice->txSem);
        LL_I2C_ClearStopDet_Intr(i2cReg);
        return;
    }

    /* Tx */
   if ((occurIntr & I2C_INTR_TX_EMPTY)) {
        if (ptxMsg->size > 0U) {
            i2c_Master_WriteTxFifo(pi2cDevice);
        }

        /* Master Read Data */
        if (prxMsg->size > 0U) {
            /* Read cmd  */
            if (i2c_Master_ReadRxFifoPre(pi2cDevice)) {
                LL_I2C_Disable_Intr_Mask(i2cReg, I2C_INTR_TX_EMPTY);
            }
        }

        /* Not Need clear */
    }

    /* RX */
    if ((occurIntr & I2C_INTR_RX_FULL)) {
        if (prxMsg->size > 0) {
            /* Read data */
            i2c_Master_ReadRxFifo(pi2cDevice);
        }

        /* Not Need clear */
    }
}
#endif


static int32_t i2c_TxAbortHandler(I2C_Device_t *pi2cDev, uint32_t abort_mask)
{
    LOG_ERR("tx abort(msk %08X)", abort_mask);

    if (abort_mask & I2C_ABORT_7ADDR_NOACK) {
        LOG_DBG("7BitAddr(%02X) Not Ack", pi2cDev->config.devAddr);
    } else if (abort_mask & I2C_ABORT_10ADDR1_NOACK) {
        LOG_DBG("10BitAddr1 Not Ack");
    } else if (abort_mask & I2C_ABORT_10ADDR2_NOACK) {
        LOG_DBG("10BitAddr2 Not Ack");
    } else if (abort_mask & I2C_ABORT_10B_RD_NORSTRT) {

    } else if (abort_mask & I2C_ABORT_GCALL_NOACK) {

    } else if (abort_mask & I2C_ABORT_SDA_STUCK_AT_LOW) {

    } else if ((abort_mask & I2C_ABORT_TX_FLUSH_CNT) != 0) {
        LOG_DBG("FL Cnt%d", HAL_GET_MSK_VAL(abort_mask, TX_FLUSH_CNT_Msk, TX_FLUSH_CNT_Pos));
    } else if (abort_mask & I2C_ABORT_TXDATA_NOACK) {

    } else if (abort_mask & I2C_ABORT_SLV_FLUSH_TXFIFO) {

    }

    return 0;
}

__HAL_STATIC_INLINE void I2C_ErrorIRQHandler(void *dev, uint32_t occurIntr)
{
    I2C_Device_t *pi2cDevice      =     (I2C_Device_t *)dev;
    i2c_dev_t    *i2cReg          =     (i2c_dev_t *)(pi2cDevice->device.reg);

    /* Handler Error Intrrupt */
    if ((occurIntr & I2C_INTR_TX_ABRT)) {
        i2c_TxAbortHandler(dev, LL_I2C_Get_Tx_Abort_Src(i2cReg));
        LL_I2C_ClearTxAbort_Intr(i2cReg);
    } else if ((occurIntr & I2C_INTR_TX_OVER)) {
        LOG_ERR("tx over");
        LL_I2C_ClearTxOver_Intr(i2cReg);
    } else if ((occurIntr & I2C_INTR_RX_UNDER)) {
        LOG_ERR("rx under");
        LL_I2C_ClearRxUnder_Intr(i2cReg);
    } else if ((occurIntr & I2C_INTR_RX_OVER)) {
        LOG_ERR("rx over");
        LL_I2C_ClearRxOver_Intr(i2cReg);
    }

    if ((occurIntr & I2C_INTR_BCNT_RX_ERR)     ||
        (occurIntr & I2C_INTR_BCNT_TX_ERR)     ||
        (occurIntr & I2C_INTR_BCNT_RX_TIMEOUT) ||
        (occurIntr & I2C_INTR_BCNT_TX_TIMEOUT)) {
        if (pi2cDevice->busErrCb.cb) {
            pi2cDevice->busErrCb.cb((HAL_Dev_t *)dev, pi2cDevice->busErrCb.arg);
        }
        LL_I2C_ClearAll_Intr(i2cReg);
    }
}


__i2c_sram_text static void HAL_I2C_ISR(void *dev)
{
#if CONFIG_I2C_TRANS_IT_ENABLE
    I2C_Device_t *pi2cDevice      =    (I2C_Device_t *)dev;
    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    uint32_t      occurIntr       =    (LL_I2C_Get_Intr_Mask(i2cReg)
                                        & LL_I2C_Get_Intr_RawStat(i2cReg));

    HAL_Callback_t Handler;

    switch (pi2cDevice->config.mode) {
    #if CONFIG_I2C_SLAVE
    case I2C_MODE_SLAVE: {
        Handler.cb    =   pi2cDevice->slave_irqHandler.cb;
        Handler.arg   =   pi2cDevice->slave_irqHandler.arg;

        if (Handler.cb) {
            Handler.cb((HAL_Dev_t *)dev, Handler.arg);
        } else {
            I2C_ErrorIRQHandler(dev, occurIntr);
            I2C_SlaveIRQHandler(dev, occurIntr);
        }
        break;
    }
    #endif


    #if CONFIG_I2C_MASTER
    case I2C_MODE_MASTER: {
        Handler.cb    =   pi2cDevice->master_irqHandler.cb;
        Handler.arg   =   pi2cDevice->master_irqHandler.arg;

        if (Handler.cb) {
            Handler.cb((HAL_Dev_t *)dev, Handler.arg);
        } else {
            I2C_ErrorIRQHandler(dev, occurIntr);
            I2C_MasterIRQHandler(dev, occurIntr);
        }
        break;
    }
    #endif

    default:
        break;
    }
#endif
}


/* Exported functions.
 * ----------------------------------------------------------------------------
 */
HAL_Dev_t *HAL_I2C_Init(I2C_ID_t id, const I2C_InitParam_t *pinitParam)
{
    if (NULL == pinitParam || id > I2C_NUM) {
        return NULL;
    }

    if (i2c_SpeedMapBitrate(pinitParam->speed) <= 0) {
        return NULL;
    }

    I2C_Device_t *pi2cDevice  =  (I2C_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_I2C, id);
    if (NULL == pi2cDevice) {
        pi2cDevice = (I2C_Device_t *)HAL_DEV_MemMalloc(sizeof(I2C_Device_t));
        if (NULL == pi2cDevice) {
            return (HAL_Dev_t *)pi2cDevice;
        } else {
            /* Initialize device heap */
            HAL_Memset(pi2cDevice, 0, sizeof(I2C_Device_t));
        }

        if (id == I2C0_ID) {
            pi2cDevice->device.reg        =  (void *)I2C0_DEV;
            pi2cDevice->device.irqNum     =  I2C0_IRQn;
            pi2cDevice->device.irqHandler =  HAL_I2C_ISR;
            pi2cDevice->device.irqPrio    =  I2C0_IRQ_PRIORITY;
        } else if (id == I2C1_ID) {
            pi2cDevice->device.reg        =  (void *)I2C1_DEV;
            pi2cDevice->device.irqNum     =  I2C1_IRQn;
            pi2cDevice->device.irqHandler =  HAL_I2C_ISR;
            pi2cDevice->device.irqPrio    =  I2C1_IRQ_PRIORITY;
        } else {
            goto i2c_err_ret;
        }

        /* Configure information */
        #if (CONFIG_I2C_SLAVE && CONFIG_PM && CONFIG_I2C_WAKEUP)
        pi2cDevice->wakeupConf.wakeupEnable     = false;
        pi2cDevice->wakeupConf.wakeupStatus     = I2C_WAKEUPSTATUS_ERROR;
        #endif

        #if CONFIG_I2C_TRANS_DMA_ENABLE
            #error "unsupport"
        #endif

        if (pinitParam->busErrCb.cb) {
            pi2cDevice->busErrCb.cb      =   pinitParam->busErrCb.cb;
            pi2cDevice->busErrCb.arg     =   pinitParam->busErrCb.arg;
        }
        pi2cDevice->config.mode      =   pinitParam->mode;
        pi2cDevice->config.speed     =   pinitParam->speed;
        pi2cDevice->config.bitrate   =   i2c_SpeedMapBitrate(pinitParam->speed);

        /* Enable Intrrupt. */
        IRQ_Disable(pi2cDevice->device.irqNum);
        IRQ_Attach(pi2cDevice->device.irqNum, pi2cDevice->device.irqHandler);
        IRQ_AttachDevice(pi2cDevice->device.irqNum, (HAL_Dev_t *)pi2cDevice);
        IRQ_Priority(pi2cDevice->device.irqPrio, pi2cDevice->device.irqPrio);
        IRQ_Enable(pi2cDevice->device.irqNum);


    #if (CONFIG_I2C_TRANS_IT_ENABLE || CONFIG_I2C_TRANS_DMA_ENABLE)
        HAL_SemaphoreSetInvalid(&pi2cDevice->txSem);
        HAL_SemaphoreSetInvalid(&pi2cDevice->rxSem);
    #endif

        HAL_DEV_Register((HAL_Dev_t *)pi2cDevice, HAL_DEV_TYPE_I2C, id);

    #if CONFIG_PM
        struct pm_device *pi2cPmDev    =   &pi2cDevice->device.pmDev;

        int ret = pm_device_register(pi2cPmDev, i2c_pm_action);
        if (ret) {
            goto i2c_err_ret;
        }
    #endif /* PM */
    }

    return (HAL_Dev_t *)pi2cDevice;

i2c_err_ret:
    HAL_DEV_MemFree(pi2cDevice);
    return NULL;
}

HAL_Status_t HAL_I2C_DeInit(HAL_Dev_t *i2c)
{
    HAL_Status_t status           =    HAL_STATUS_OK;
    I2C_Device_t *pi2cDevice      =    (I2C_Device_t *)i2c;

    if (NULL == i2c)  {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    if (NULL == i2cReg) {
        return HAL_STATUS_INVALID_PARAM;
    }

    #if CONFIG_I2C_ASSERT
    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (HAL_DEV_GetState(i2c) == HAL_DEV_STATE_UNINIT ||
        (HAL_DEV_GetState(i2c) != HAL_DEV_STATE_CLOSE)) {
        return HAL_STATUS_INVALID_STATE;
    }

    if (LL_I2C_IsActivity(i2cReg)) {
        return HAL_STATUS_BUSY;
    }

    LL_I2C_Close(i2cReg);
    if (LL_I2C_IsAble(i2cReg)) {
        return HAL_STATUS_ERROR;
    }

    #if (CONFIG_PM)
    struct pm_device *p_i2cPmDev    =   &pi2cDevice->device.pmDev;

    int ret = pm_device_unregister(p_i2cPmDev);
    if (0 != ret) {
        return status;
    }
    #endif

    status = HAL_DEV_Unregister(i2c);
    if (HAL_STATUS_OK != status) {
        return status;
    }

    HAL_DEV_MemFree(i2c);

    #if (CONFIG_I2C_TRANS_IT_ENABLE || CONFIG_I2C_TRANS_DMA_ENABLE)
    HAL_SemaphoreDeinit(&pi2cDevice->txSem);
    HAL_SemaphoreDeinit(&pi2cDevice->rxSem);
    HAL_SemaphoreSetInvalid(&pi2cDevice->txSem);
    HAL_SemaphoreSetInvalid(&pi2cDevice->rxSem);

    #endif

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_I2C_Open(HAL_Dev_t *i2c, I2C_AddrWidth_t addrWidth, const uint16_t addr)
{
    int           ret              =    0;
    HAL_Status_t  status           =    HAL_STATUS_OK;
    I2C_Device_t  *pi2cDevice      =    (I2C_Device_t *)i2c;

    if (NULL == i2c)  {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    if (NULL == i2cReg) {
        return HAL_STATUS_INVALID_PARAM;
    }
    i2c_Config_t  *pconfig         =    &pi2cDevice->config;
    #if CONFIG_I2C_ASSERT
    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (false == i2c_Check_Invalid_Addr(addr, addrWidth)) {
        LOG_ERR("addr %08X-addrWidth %d invalid", addr, addrWidth);
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if ((HAL_DEV_GetState(i2c) == HAL_DEV_STATE_UNINIT)) {
        return HAL_STATUS_INVALID_STATE;
    }

    /* I2C GPIO AF */
    status = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_I2C, I2C_ID_GET(i2c->id),
                                 HAL_PINMUX_TYPE_INIT, &pi2cDevice->pinMux);
    if (status != HAL_STATUS_OK) {
        LOG_ERR("gpio get pinmux fail (%d)", status);
        return status;
    }

    status = HAL_BOARD_SetPinMux(&pi2cDevice->pinMux);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    /* Enable I2C Bus(APB0) clock */
    i2c_ClockEnable(LL_RCC_I2C0_APB0, LL_RCC_I2C0_DIV_1);

    /* Get Clock frequency (I2C Clock Source)*/
    uint32_t freq  =  HAL_BOARD_GetFreq(CLOCK_APB0);
    if (0 == freq) {
        LOG_ERR("Bus get frequency fail (%d)", freq);
        return HAL_STATUS_INVALID_PARAM;
    }

    /* Config mode */
    pconfig->freq          =   freq;
    pconfig->addrWidth     =   addrWidth;
    pconfig->devAddr       =   addr;

    ret = i2c_TimmingCalcu(pconfig->speed, pconfig->freq, pconfig->bitrate, &pi2cDevice->timing);
    if (ret != 0) {
        LOG_ERR("Timing calcu fail (%d)", ret);
        return ret;
    }

    /* Config I2C Hardware */
    ret = i2c_Config(i2cReg, &pi2cDevice->config, &pi2cDevice->timing);
    if (ret != 0) {
        LOG_DBG("Config low layer fail (%d)", ret);
        return HAL_STATUS_ERROR;
    }

#if CONFIG_I2C_SLAVE && CONFIG_PM && CONFIG_I2C_WAKEUP
    pi2cDevice->wakeupConf.wakeupEnable = true;

    pi2cDevice->wakeupConf.thresholdReq = false;
    pi2cDevice->wakeupConf.wakeupThreshold   =  addr;

    status = i2c_GetWakeupPinIdx(&pi2cDevice->pinMux, &pi2cDevice->wakeupConf);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    status = i2c_WakeupConfig(pi2cDevice);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
#endif

    /* Enable error check intrrupt */
    // LL_I2C_Enable_Intr_Mask(i2cReg, LL_I2C_INTR_ERR_MASK | LL_I2C_INTR_STOP_MASK);


    HAL_DEV_SetState(i2c, HAL_DEV_STATE_OPEN);

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_I2C_Close(HAL_Dev_t *i2c)
{
    HAL_Status_t  status          =    HAL_STATUS_ERROR;
    I2C_Device_t *pi2cDevice      =    (I2C_Device_t *)i2c;
    if (NULL == i2c)  {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    if (NULL == i2cReg) {
        return HAL_STATUS_INVALID_PARAM;
    }

    #if CONFIG_I2C_ASSERT
    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (HAL_DEV_GetState(i2c) != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    /* Waiting Bus idle */
    if (LL_I2C_IsActivity(i2cReg)) {
        return HAL_STATUS_BUSY;
    }

    LL_I2C_Close(i2cReg);

    if (LL_I2C_IsAble(i2cReg)) {
        return HAL_STATUS_ERROR;
    }

    /* Disable clock */
    i2c_ClockDisable();

    /* I2C GPIO Disable */
    status = HAL_BOARD_GetPinMux(HAL_DEV_TYPE_I2C, I2C_ID_GET(i2c->id),
                             HAL_PINMUX_TYPE_DEINIT, &pi2cDevice->dePinMux);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    status = HAL_BOARD_SetPinMux(&pi2cDevice->dePinMux);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    HAL_Memset(&pi2cDevice->tx_msg, 0, sizeof(i2c_Msg_t));
    HAL_Memset(&pi2cDevice->rx_msg, 0, sizeof(i2c_Msg_t));

#if CONFIG_I2C_SLAVE && CONFIG_PM && CONFIG_I2C_WAKEUP
    status = i2c_WakeupDisable(pi2cDevice);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    pi2cDevice->wakeupConf.wakeupEnable  =  false;
#endif

    HAL_DEV_SetState(i2c, HAL_DEV_STATE_CLOSE);

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_I2C_Enable(HAL_Dev_t *i2c)
{
    I2C_Device_t  *pi2cDevice      =    (I2C_Device_t *)i2c;

    if (NULL == i2c)  {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    if (NULL == i2cReg) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Enable(i2cReg);
    }

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_I2C_Disable(HAL_Dev_t *i2c)
{
    I2C_Device_t  *pi2cDevice      =    (I2C_Device_t *)i2c;

    if (NULL == i2c)  {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    if (NULL == i2cReg) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Disable(i2cReg);
    }

    return HAL_STATUS_OK;
}

__i2c_sram_text HAL_Status_t HAL_I2C_RecoverBus(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice      =    (I2C_Device_t *)i2c;
    if (NULL == i2c)  {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    if (NULL == i2cReg) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #if CONFIG_I2C_ASSERT
    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (HAL_DEV_GetState(i2c) != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_NOT_SUPPORTED;
    }

    switch (pi2cDevice->config.mode) {
    case I2C_MODE_MASTER:
    #if (CONFIG_I2C_MASTER == 1)
        /* Send Stop message. */
        LOG_DBG("Master send stop message to recover bus");
        LL_I2C_EnableRecoverySDAStuck(i2cReg);
        break;
    #endif

    case I2C_MODE_SLAVE:
    #if (CONFIG_I2C_SLAVE == 1)
        /* Send Stop message. */
        LOG_DBG("Slave close i2c device to recover bus");
        LL_I2C_Disable(i2cReg);
        LL_I2C_Enable(i2cReg);
        break;
    #endif

    default:
        return HAL_STATUS_ERROR;
        break;
    }

    return  HAL_STATUS_OK;
}

HAL_Status_t HAL_I2C_SetTransferMode(HAL_Dev_t *i2c, I2C_TransfDir_t tranfDir, I2C_TransfMode_t tranfMode)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_Msg_t    *ptxMsg              =     &pi2cDevice->tx_msg;
    i2c_Msg_t    *prxMsg              =     &pi2cDevice->rx_msg;

    #if CONFIG_I2C_ASSERT
    if (NULL == pi2cDevice || NULL == ptxMsg || NULL == prxMsg) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif


    switch (tranfDir) {
    case I2C_TRANSFDIR_RX:
        prxMsg->tranfMode = tranfMode;
        LOG_DBG("Config RX Mode %d", prxMsg->tranfMode);
        break;

    case I2C_TRANSFDIR_TX:
        ptxMsg->tranfMode = tranfMode;
        LOG_DBG("Config TX Mode %d", ptxMsg->tranfMode);
        break;

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}

int32_t HAL_I2C_Transmit(HAL_Dev_t *i2c, const uint8_t *buff, uint32_t size, uint32_t msec)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);
    i2c_Msg_t    *ptxMsg              =    &pi2cDevice->tx_msg;
    uint32_t      txNum               =    HAL_STATUS_ERROR;

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c) || (NULL == buff) || (0 == size)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif


    if (HAL_DEV_GetState(i2c) != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    HAL_Status_t status = i2c_Config(i2cReg, &pi2cDevice->config, &pi2cDevice->timing);
    if (status != 0) {
        printf("i2c config fail (%d)", status);
        return HAL_STATUS_ERROR;
    }

    if (buff && size != 0) {
        ptxMsg->buff   =   (uint8_t *)buff;
        ptxMsg->size   =   size;
        ptxMsg->offset =   0;
        ptxMsg->stop   =   true;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Enable(i2cReg);
    }

    /* Transmit Data */
    switch (pi2cDevice->config.mode) {
    #if (CONFIG_I2C_MASTER == 1)
    case I2C_MODE_MASTER: {
        if (ptxMsg->tranfMode == I2C_TRANSFMODE_INTERRUPT) {
            #if (CONFIG_I2C_TRANS_IT_ENABLE == 1)
            txNum = i2c_Master_WriteIntr(pi2cDevice, msec);
            if (txNum <= 0) {
                txNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (ptxMsg->tranfMode == I2C_TRANSFMODE_DMA) {
            #if (CONFIG_I2C_TRANS_DMA_ENABLE == 1)
            txNum = i2c_Master_WriteDMA(pi2cDevice, msec);
            if (txNum <= 0) {
                txNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (ptxMsg->tranfMode == I2C_TRANSFMODE_NORMAL) {
            txNum = i2c_Master_WritePoll(i2cReg, ptxMsg, msec);
            if (txNum <= 0) {
                if (txNum == HAL_STATUS_BUSY) {
                    txNum = i2c_CheckBusError(i2cReg);
                }
            }
        } else {
            txNum = HAL_STATUS_INVALID_PARAM;
        }
        break;
    }
    #endif

    #if (CONFIG_I2C_SLAVE == 1)
    case I2C_MODE_SLAVE: {
        if (ptxMsg->tranfMode == I2C_TRANSFMODE_INTERRUPT) {
            #if (CONFIG_I2C_TRANS_IT_ENABLE == 1)
            txNum = i2c_Slave_WriteIntr(pi2cDevice,  msec);
            if (txNum <= 0) {
                txNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (ptxMsg->tranfMode == I2C_TRANSFMODE_DMA) {
            #if (CONFIG_I2C_TRANS_DMA_ENABLE == 1)
            txNum = i2c_Slave_WriteDMA(pi2cDevice, msec);
            if (txNum <= 0) {
                txNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (ptxMsg->tranfMode == I2C_TRANSFMODE_NORMAL) {
            txNum = i2c_Slave_WritePoll(i2cReg, ptxMsg, msec);
            if (txNum < 0) {
                txNum = HAL_STATUS_ERROR;
            }
        } else {
            txNum = HAL_STATUS_INVALID_PARAM;
        }
        break;
    }
    #endif

    default:
        txNum = HAL_STATUS_INVALID_PARAM;
        break;
    }


    return txNum;
}

int32_t HAL_I2C_Receive(HAL_Dev_t *i2c, uint8_t *buff, uint32_t size, uint32_t msec)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_Msg_t    *prxMsg              =    &pi2cDevice->rx_msg;
    uint32_t      rxNum               =    HAL_STATUS_ERROR;

    if (NULL == i2c)  {
        return HAL_STATUS_INVALID_PARAM;
    }

    i2c_dev_t    *i2cReg          =    (i2c_dev_t *)(pi2cDevice->device.reg);
    if (NULL == i2cReg) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #if CONFIG_I2C_ASSERT
    if (0 == size) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif


    if (HAL_DEV_GetState(i2c) != HAL_DEV_STATE_OPEN) {
        return HAL_STATUS_INVALID_STATE;
    }

    HAL_Status_t status = i2c_Config(i2cReg, &pi2cDevice->config, &pi2cDevice->timing);
    if (status != 0) {
        printf("i2c config fail (%d)", status);
        return HAL_STATUS_ERROR;
    }


    if (buff && size != 0) {
        prxMsg->buff   =   (uint8_t *)buff;
        prxMsg->size   =   size;
        prxMsg->offset =   0;
        prxMsg->stop   =   true;
    } else {
        return HAL_STATUS_INVALID_PARAM;
    }

    if (!LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Enable(i2cReg);
    }

    /* Receive Data */
    switch (pi2cDevice->config.mode) {
    #if (CONFIG_I2C_MASTER == 1)
    case I2C_MODE_MASTER: {
        if (prxMsg->tranfMode == I2C_TRANSFMODE_INTERRUPT) {
           #if (CONFIG_I2C_TRANS_IT_ENABLE == 1)
            rxNum = i2c_Master_ReadIntr(pi2cDevice, msec);
            if (rxNum < 0) {
                rxNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (prxMsg->tranfMode == I2C_TRANSFMODE_DMA) {
            #if (CONFIG_I2C_TRANS_DMA_ENABLE == 1)
            rxNum = i2c_Master_ReadDMA(pi2cDevice, msec);
            if (rxNum < 0) {
                rxNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (prxMsg->tranfMode == I2C_TRANSFMODE_NORMAL) {
            rxNum = i2c_Master_ReadPoll(i2cReg, prxMsg, msec);
            if (rxNum < 0) {
                rxNum = i2c_CheckBusError(i2cReg);
            }
        } else {
            rxNum = HAL_STATUS_INVALID_PARAM;
        }
        break;
    }
    #endif

    #if (CONFIG_I2C_SLAVE == 1)
    case I2C_MODE_SLAVE: {
        if (prxMsg->tranfMode == I2C_TRANSFMODE_INTERRUPT) {
            #if (CONFIG_I2C_TRANS_IT_ENABLE == 1)
            rxNum = i2c_Slave_ReadIntr(pi2cDevice, msec);
            if (rxNum < 0) {
                rxNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (prxMsg->tranfMode == I2C_TRANSFMODE_DMA) {
            #if (CONFIG_I2C_TRANS_DMA_ENABLE == 1)
            rxNum = i2c_Slave_ReadDMA(pi2cDevice, msec);
            if (rxNum < 0) {
                rxNum = HAL_STATUS_ERROR;
            }
            #endif
        } else if (prxMsg->tranfMode == I2C_TRANSFMODE_NORMAL) {
            rxNum = i2c_Slave_ReadPoll(i2cReg, prxMsg, msec);
            if (rxNum < 0) {
                rxNum = i2c_CheckBusError(i2cReg);
            }
        } else {
            rxNum = HAL_STATUS_INVALID_PARAM;
        }
        break;
    }
    #endif

    default:
        rxNum = HAL_STATUS_INVALID_PARAM;
        break;
    }

    return rxNum;
}

__i2c_sram_text HAL_Status_t HAL_I2C_ExtendControl(HAL_Dev_t *i2c, I2C_ExtAttr_t attr, void *arg)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    switch (attr) {
    case I2C_EXTATTR_RESET:
        LL_I2C_Disable(i2cReg);
        while (!LL_I2C_IsAble(i2cReg)) {
        }
        LL_I2C_Enable(i2cReg);
        break;

    case I2C_EXTATTR_FLUSH_RXFIFO:
        (*(uint32_t *)arg) = i2c_FlushRxFifo(i2c);
        break;

    case I2C_EXTATTR_GET_TX_FLUSH_CNT:
        (*(uint32_t *)arg) = HAL_GET_MSK_VAL(LL_I2C_Get_Tx_Abort_Src(i2cReg),
                                            TX_FLUSH_CNT_Msk,
                                            TX_FLUSH_CNT_Pos);
        break;

    case I2C_EXTATTR_SET_RXFIFO_LEVEL:
        LL_I2C_Set_RxFifo_Threshold(i2cReg, (*(uint32_t *)arg));
        break;

    case I2C_EXTATTR_GET_ENABLE_INTR:
        (*(uint32_t *)arg) = i2c_GetEnableIRQ(i2c);
        break;

    case I2C_EXTATTR_SET_TXFIFO_LEVEL:
        LL_I2C_Set_TxFifo_Threshold(i2cReg, (*(uint32_t *)arg));
        break;

#if CONFIG_I2C_SLAVE && CONFIG_PM
#if CONFIG_I2C_WAKEUP
    case I2C_EXTATTR_WAKEUP_CTRL_ABLE:
        if ((*(bool *)arg) == true) {
            pi2cDevice->wakeupConf.wakeupStatus = I2C_WAKEUPSTATUS_ENABLE;
        } else if ((*(bool *)arg) == false) {
            pi2cDevice->wakeupConf.wakeupStatus = I2C_WAKEUPSTATUS_DISABLE;
        } else {
            pi2cDevice->wakeupConf.wakeupStatus = I2C_WAKEUPSTATUS_ERROR;
            return HAL_STATUS_INVALID_STATE;
        }
        break;

    case I2C_EXTATTR_WAKEUP_CALLBACK: {
        HAL_Callback_t wakeupCb  =  { 0 };

        HAL_Memcpy(&wakeupCb, (HAL_Callback_t *)arg, sizeof(HAL_Callback_t));

        if (wakeupCb.cb) {
            pi2cDevice->wakeupConf.wakeupCb.cb    =    wakeupCb.cb;
            pi2cDevice->wakeupConf.wakeupCb.arg   =    wakeupCb.arg;
        } else {
            return HAL_STATUS_INVALID_STATE;
        }
        break;
    }

    case I2C_EXTATTR_WAKEUP_THRESHOLD:
        if (pi2cDevice->wakeupConf.wakeupEnable == true) {
            pi2cDevice->wakeupConf.thresholdReq      =   true;
            pi2cDevice->wakeupConf.wakeupThreshold   =   *((uint32_t *)(arg));
         } else {
             return HAL_STATUS_INVALID_STATE;
         }
        break;
#endif
    case I2C_EXTATTR_WAKEUP_MODE:
        pi2cDevice->wk_mode = *((uint8_t *)(arg));
        break;
#endif

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}

__i2c_sram_text uint8_t HAL_I2C_GetRxData(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);

    return LL_I2C_GetCmdData(i2cReg);
}


__i2c_sram_text void HAL_I2C_PutTxData(HAL_Dev_t *i2c, uint8_t txData, I2C_Cmd_t cmd)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);

    return LL_I2C_PutCmdData(i2cReg, txData, cmd);
}


__i2c_sram_text bool HAL_I2C_IsTxReady(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);

    return (!LL_I2C_isTx_Fifo_Full(i2cReg));
}


__i2c_sram_text bool HAL_I2C_IsRxReady(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);

    return (!LL_I2C_isRx_Fifo_Empty(i2cReg));

}

bool HAL_I2C_IsTxEmpty(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);

    return (LL_I2C_isTx_Fifo_Empty(i2cReg));
}


#if (CONFIG_I2C_TRANS_IT_ENABLE == 1)
HAL_Status_t HAL_I2C_RegisterIRQ(HAL_Dev_t *i2c, I2C_Callback_t *irqCb)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c) || (NULL == irqCb)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    switch (pi2cDevice->config.mode) {
#if (CONFIG_I2C_MASTER == 1)
    case I2C_MODE_MASTER: {
        LOG_DBG("i2c master irq handler cb");
        pi2cDevice->master_irqHandler.cb     =   irqCb->cb;
        pi2cDevice->master_irqHandler.arg    =   irqCb->arg;
       break;
    }
#endif

#if (CONFIG_I2C_SLAVE == 1)
    case I2C_MODE_SLAVE: {
        LOG_DBG("i2c slave irq handler cb");
        pi2cDevice->slave_irqHandler.cb      =   irqCb->cb;
        pi2cDevice->slave_irqHandler.arg     =   irqCb->arg;
        break;
    }
#endif

    default:
       LOG_ERR("i2c mode err cb(%d)", pi2cDevice->config.mode);
       return HAL_STATUS_INVALID_PARAM;
       break;
    }

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_I2C_UnregisterIRQ(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    switch (pi2cDevice->config.mode) {
    #if (CONFIG_I2C_MASTER == 1)
    case I2C_MODE_MASTER: {
        LOG_DBG("i2c master irq handler unreg cb");
        pi2cDevice->master_irqHandler.cb     =   NULL;
        pi2cDevice->master_irqHandler.arg    =   NULL;
        break;
    }
    #endif

    #if (CONFIG_I2C_SLAVE == 1)
    case I2C_MODE_SLAVE: {
        LOG_DBG("i2c slave irq handler unreg cb");
        pi2cDevice->slave_irqHandler.cb      =   NULL;
        pi2cDevice->slave_irqHandler.arg     =   NULL;
        break;
    }
    #endif

    default:
        LOG_ERR("i2c mode err (%d) uncb", pi2cDevice->config.mode);
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}


__i2c_sram_text HAL_Status_t HAL_I2C_EnableIRQ(HAL_Dev_t *i2c, uint32_t irqStatus)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);
    uint32_t      intrMsk             =    0U;

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (irqStatus & (I2C_IRQSTATUS_START)) {
        intrMsk |= (LL_I2C_INTR_START_DET_MASK | LL_I2C_INTR_RESTART_DET_MASK);
    }

    if (irqStatus & (I2C_IRQSTATUS_STOP)) {
        intrMsk |= LL_I2C_INTR_STOP_MASK;
    }

    if (irqStatus & (I2C_IRQSTATUS_READ_REQ)) {
        intrMsk |= LL_I2C_INTR_RD_REQ_MASK;
    }

    if (irqStatus & (I2C_IRQSTATUS_READ_DONE)) {
        intrMsk |= LL_I2C_INTR_RX_DONE_MASK;
    }

    if (irqStatus & (I2C_IRQSTATUS_TX_FIFO_EMPTY)) {
        intrMsk |= LL_I2C_INTR_TX_EMPTY_MASK;
    }

    if (irqStatus & (I2C_IRQSTATUS_RX_FIFO_FULL)) {
        intrMsk |= LL_I2C_INTR_RX_FULL_MASK;
    }

    if (irqStatus & I2C_IRQSTATUS_ERROR) {
        intrMsk |= ((LL_I2C_INTR_TX_OVRE_MASK | LL_I2C_INTR_TX_ABRT_MASK
                    | LL_I2C_INTR_RX_OVER_MASK | LL_I2C_INTR_RX_UNDER_MASK));
    }

    if (!LL_I2C_IsActivity(i2cReg)) {
        LL_I2C_ClearAll_Intr(i2cReg);
    }
    LL_I2C_Enable_Intr_Mask(i2cReg, intrMsk);

    return HAL_STATUS_OK;
}


__i2c_sram_text HAL_Status_t HAL_I2C_DisableIRQ(HAL_Dev_t *i2c, uint32_t irqStatus)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);
    uint32_t      intrMsk             =    0U;

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (irqStatus & (I2C_IRQSTATUS_START)) {
        intrMsk |= (LL_I2C_INTR_START_DET_MASK | LL_I2C_INTR_RESTART_DET_MASK);

        LL_I2C_ClearStartDet_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_STOP)) {
        intrMsk |= LL_I2C_INTR_STOP_MASK;

        LL_I2C_ClearStartDet_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_READ_REQ)) {
        intrMsk |= LL_I2C_INTR_RD_REQ_MASK;

        LL_I2C_ClearReadReq_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_READ_DONE)) {
        intrMsk |= LL_I2C_INTR_RX_DONE_MASK;

        LL_I2C_ClearRxDone_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_TX_FIFO_EMPTY)) {
        intrMsk |= LL_I2C_INTR_TX_EMPTY_MASK;

        /* Not clear Tx Empty intrrupt */
    }

    if (irqStatus & (I2C_IRQSTATUS_RX_FIFO_FULL)) {
        intrMsk |= LL_I2C_INTR_RX_FULL_MASK;

        /* Not clear Rx Full intrrupt */
    }

    if (irqStatus & I2C_IRQSTATUS_ERROR) {
        intrMsk |= ((LL_I2C_INTR_TX_OVRE_MASK | LL_I2C_INTR_TX_ABRT_MASK
                    | LL_I2C_INTR_RX_OVER_MASK | LL_I2C_INTR_RX_UNDER_MASK));

        LL_I2C_ClearRxOver_Intr(i2cReg);
        LL_I2C_ClearRxOver_Intr(i2cReg);
        LL_I2C_ClearTxAbort_Intr(i2cReg);
        LL_I2C_ClearTxOver_Intr(i2cReg);
    }

    LL_I2C_Disable_Intr_Mask(i2cReg, intrMsk);

    return HAL_STATUS_OK;
}

__i2c_sram_text HAL_Status_t HAL_I2C_ClearIRQ(HAL_Dev_t *i2c, uint32_t irqStatus)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (irqStatus & (I2C_IRQSTATUS_START)) {
        LL_I2C_ClearStartDet_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_STOP)) {
        LL_I2C_ClearStopDet_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_READ_REQ)) {
        LL_I2C_ClearReadReq_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_READ_DONE)) {
        LL_I2C_ClearRxDone_Intr(i2cReg);
    }

    if (irqStatus & (I2C_IRQSTATUS_TX_FIFO_EMPTY)) {
        /* Not clear Tx Empty intrrupt */
    }

    if (irqStatus & (I2C_IRQSTATUS_RX_FIFO_FULL)) {
        /* Not clear Rx Full intrrupt */
    }

    if (irqStatus & I2C_IRQSTATUS_ERROR) {
        LL_I2C_ClearRxOver_Intr(i2cReg);
        LL_I2C_ClearRxOver_Intr(i2cReg);
        LL_I2C_ClearTxAbort_Intr(i2cReg);
        LL_I2C_ClearTxOver_Intr(i2cReg);
    }

    return HAL_STATUS_OK;
}


__i2c_sram_text uint32_t     HAL_I2C_GetIRQStatus(HAL_Dev_t *i2c)
{
    I2C_Device_t *pi2cDevice          =    (I2C_Device_t *)i2c;
    i2c_dev_t    *i2cReg              =    (i2c_dev_t *)(pi2cDevice->device.reg);
    uint32_t      intrMask            =    LL_I2C_Get_Intr_RawStat(i2cReg) &
                                           LL_I2C_Get_Intr_Mask(i2cReg);
    uint32_t      irqStatus           =    0;

    #if CONFIG_I2C_ASSERT
    if ((NULL == i2c)) {
        return HAL_STATUS_INVALID_PARAM;
    }

    if ((I2C_ID_GET(i2c->id) > I2C_NUM) || (I2C_TYPE_GET(i2c->id) != HAL_DEV_TYPE_I2C)) {
        return HAL_STATUS_INVALID_PARAM;
    }
    #endif

    if (intrMask & (LL_I2C_INTR_START_DET_MASK | LL_I2C_INTR_RESTART_DET_MASK)) {
        irqStatus |= I2C_IRQSTATUS_START;
    }

    if (intrMask & LL_I2C_INTR_STOP_MASK) {
        irqStatus |= I2C_IRQSTATUS_STOP;
    }

    if (intrMask & LL_I2C_INTR_RD_REQ_MASK) {
        irqStatus |= I2C_IRQSTATUS_READ_REQ;
    }

    if (intrMask & LL_I2C_INTR_RX_DONE_MASK) {
        irqStatus |= I2C_IRQSTATUS_READ_DONE;
    }

    if (intrMask & LL_I2C_INTR_TX_EMPTY_MASK) {
        irqStatus |= I2C_IRQSTATUS_TX_FIFO_EMPTY;
    }

    if (intrMask & LL_I2C_INTR_RX_FULL_MASK) {
        irqStatus |= I2C_IRQSTATUS_RX_FIFO_FULL;
    }

    if (intrMask & (LL_I2C_INTR_TX_OVRE_MASK | LL_I2C_INTR_TX_ABRT_MASK
                    | LL_I2C_INTR_RX_OVER_MASK | LL_I2C_INTR_RX_UNDER_MASK)) {
        irqStatus |= I2C_IRQSTATUS_ERROR;
    }

    LOG_DBG("IRQ flag Msk%08X Sta%08X", intrMask, irqStatus);

    return irqStatus;
}
#endif


#if (CONFIG_I2C_TRANS_DMA_ENABLE == 1)
HAL_Status_t HAL_I2C_RegisterDMA(HAL_Dev_t *i2c, I2C_TransfDir_t tranfDir, I2C_Callback_t *cb)
{
    return HAL_STATUS_OK;
}
HAL_Status_t HAL_I2C_UnregisterDMA(HAL_Dev_t *i2c)
{
    return HAL_STATUS_OK;
}
HAL_Status_t HAL_I2C_StartTxDMA(HAL_Dev_t *i2c)
{
    return HAL_STATUS_OK;
}
HAL_Status_t HAL_I2C_AbortTxDMA(HAL_Dev_t *i2c)
{
    return HAL_STATUS_OK;
}
HAL_Status_t HAL_I2C_StartRxDMA(HAL_Dev_t *i2c)
{
    return HAL_STATUS_OK;
}
HAL_Status_t HAL_I2C_AbortRxDMA(HAL_Dev_t *i2c)
{
    return HAL_STATUS_OK;
}
#endif



#if (CONFIG_PM == 1) && (CONFIG_I2C_WAKEUP == 1) && (CONFIG_I2C_SLAVE == 1)
static HAL_Status_t i2c_GetWakeupPinIdx(HAL_Pinmux_t *pi2cPinMux, i2c_WakeupConf_t *pwkupCfg)
{
    uint8_t   j      =   0U;
    bool sdaReady    =   false;
    bool sclReady    =   false;
    HAL_PinmuxConfig_t *ppinMux    =   pi2cPinMux->pinConfig;

    pwkupCfg->sdaPinIdx = I2C_WAKUP_PIN_IDX_ERR;
    pwkupCfg->sclPinIdx = I2C_WAKUP_PIN_IDX_ERR;

    for (j = 0; j < I2C_SDA_PIN_NUM; j++) {
        if (ppinMux->pin == i2c_wakeup_sdapin_list[j][0] &&
            ppinMux->config.mode == i2c_wakeup_sdapin_list[j][1]) {

            pwkupCfg->sdaPinIdx   =   j;
            pwkupCfg->sclPin      =   i2c_wakeup_sdapin_list[j][0];

            sdaReady   =   true;

            LOG_DBG("sda %d(M %d) idx %d",
                    pwkupCfg->sclPin,
                    ppinMux->config.mode,
                    pwkupCfg->sdaPinIdx);
            break;
        }
    }

    ppinMux++;

    for (j = 0; j < I2C_SCL_PIN_NUM; j++) {
        if (ppinMux->pin == i2c_wakeup_sclpin_list[j][0] &&
            ppinMux->config.mode == i2c_wakeup_sclpin_list[j][1]) {
            pwkupCfg->sclPinIdx   =   j;
            pwkupCfg->sclPin      =   i2c_wakeup_sclpin_list[j][0];

            sclReady = true;

            LOG_DBG("scl %d(M %d) idx %d",
                    pwkupCfg->sclPin,
                    ppinMux->config.mode,
                    pwkupCfg->sclPinIdx);

            break;
        }
    }


    if (pwkupCfg->sclPinIdx == I2C_WAKUP_PIN_IDX_ERR
        || pwkupCfg->sclPinIdx == I2C_WAKUP_PIN_IDX_ERR) {
        return HAL_STATUS_ERROR;
    }

    if (j >= I2C_SCL_PIN_NUM || sdaReady == false || sclReady == false) {
        LOG_ERR("pin mux error");
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

static HAL_Status_t i2c_WakeupConfig(I2C_Device_t *pi2cDev)
{
    i2c_WakeupConf_t  *pwkupCfg   =    &pi2cDev->wakeupConf;

    if (pwkupCfg->wakeupEnable == true) {
        LL_WKUP_EnableSrc(LL_WKUP_SRC_I2C, LL_SYS_GetCurrentSysId());

        LL_WKUP_I2C_ClrFlag();
        LL_WKUP_I2C_Reset();

        LL_WKUP_I2C_SetSCLPin(pwkupCfg->sdaPinIdx);
        LL_WKUP_I2C_SetSDAPin(pwkupCfg->sclPinIdx);
        LL_WKUP_I2C_SetSlaveAddr(pwkupCfg->wakeupThreshold);

        LOG_INF("WK CFG Addr %02X", LL_WKUP_I2C_GetSlaveAddr());
    } else {
        return HAL_STATUS_INVALID_STATE;
    }

    return HAL_STATUS_OK;
}

static HAL_Status_t i2c_WakeupEnable(I2C_Device_t *pi2cDev)
{
    i2c_WakeupConf_t  *pwkupCfg   =    &pi2cDev->wakeupConf;

    if (pwkupCfg->wakeupEnable == true) {
        LL_WKUP_I2C_ClrFlag();
        LL_WKUP_I2C_Reset();

        if (pwkupCfg->thresholdReq == true) {
            LL_WKUP_I2C_SetSlaveAddr(pwkupCfg->wakeupThreshold);
        }

        LL_WKUP_I2C_EnableSCL();
        LL_WKUP_I2C_Enable();

        LOG_INF("WK EN Addr %02X", LL_WKUP_I2C_GetSlaveAddr());
    } else {
        return HAL_STATUS_INVALID_STATE;
    }

    return HAL_STATUS_OK;
}

static HAL_Status_t i2c_WakeupDisable(I2C_Device_t *pi2cDev)
{
    i2c_WakeupConf_t  *pwkupCfg   =    &pi2cDev->wakeupConf;

    if (pwkupCfg->wakeupEnable == true) {
        if (LL_WKUP_I2C_GetFlag()) {
            LL_WKUP_I2C_ClrFlag();
            if (pwkupCfg->wakeupCb.cb) {
                pwkupCfg->wakeupCb.cb((HAL_Dev_t *)pi2cDev, pwkupCfg->wakeupCb.arg);
            }
            LOG_INF("WK DIS Addr %02X", LL_WKUP_I2C_GetSlaveAddr());
        }
        LL_WKUP_I2C_DisableSCL();
        LL_WKUP_I2C_Disable();
    } else {
        return HAL_STATUS_INVALID_STATE;
    }

    return HAL_STATUS_OK;
}
#endif


#if CONFIG_I2C_MASTER
static int32_t i2c_Master_WritePoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec)
{
    int32_t  status      =   0;
    uint32_t retTxNum    =   0;
    uint32_t end_tick    =   0U;
    uint8_t  *buff       =   i2cMsg->buff;
    uint32_t size        =   i2cMsg->size;
    uint32_t offset      =   i2cMsg->offset; /* Must be zero */

    LL_I2C_DisableAll_Intr(i2cReg);

    end_tick = HAL_BOARD_GetTime(HAL_TIME_MS) + HAL_MSecsToTicks(msec);

    /* Check bus IDLE(10ms) */
    status = i2c_CheckBusIdle(i2cReg, msec);
    if (status != 0) {
        return HAL_STATUS_BUSY;
    }

    /* size != 0 */
    do {
    if (i2cMsg->offset < size) {
        if (false == LL_I2C_isTx_Fifo_Full(i2cReg)) {
            if ((offset == size - 1U) && (i2cMsg->stop == true)) {
                /* stop msg */
                LL_I2C_PutCmdData(i2cReg, buff[offset], I2C_CMD_WRITE_STOP);
            } else if ((offset == 0U) && (i2cMsg->stop == false)) {
                /* restart msg */
                LL_I2C_PutCmdData(i2cReg, buff[offset], I2C_CMD_WRITE_RESTART);
            } else {
                /* normal write msg */
                LL_I2C_PutCmdData(i2cReg, buff[offset], I2C_CMD_WRITE);
            }
            //    LOG_DBG("0x%02X TFL %d", buff[offset], LL_I2C_Tx_Fifo_Level(i2cReg));

            offset++;
            i2cMsg->offset++;
            retTxNum++;
        }
    } else {
        if ((LL_I2C_Mst_IsActivity(i2cReg) == 0) && (LL_I2C_isTx_Fifo_Empty(i2cReg) == 1)) {
            if (LL_I2C_Get_Tx_Abort_Src(i2cReg)
                & (I2C_ABORT_TXDATA_NOACK | I2C_ABORT_7ADDR_NOACK)) {
                LOG_ERR("Empty Tx Data Abort (0x%08X)", LL_I2C_Get_Tx_Abort_Src(i2cReg));
                retTxNum = HAL_STATUS_ERROR;
                break;
            }
            break;
        }
    }

    /* Tx Abort Checkout */
    if (LL_I2C_Get_Tx_Abort_Src(i2cReg)
        & (I2C_ABORT_TXDATA_NOACK | I2C_ABORT_7ADDR_NOACK)) {
        LOG_ERR("Tx Data Abort (0x%08X)", LL_I2C_Get_Tx_Abort_Src(i2cReg));
        retTxNum = HAL_STATUS_ERROR;
        break;
    }

    /* Timeout */
    if (HAL_BOARD_GetTime(HAL_TIME_MS) >= end_tick) {
        return HAL_STATUS_TIMEOUT; /* return due to timeout */
    }
} while (1);

    /* Waiting Tranfer Done (Stop/Restart) */
    uint32_t doneStatus = I2C_INTR_STOP_DET | I2C_INTR_RESTART;
    if (0 != i2c_WaitIntrStatus(i2cReg, doneStatus, msec)) {
        LOG_ERR("Master Waiting Tranfer Done Timeout");
        retTxNum = HAL_STATUS_TIMEOUT;
    }

    /* Check bus IDLE(10ms) */
    status = i2c_CheckBusIdle(i2cReg, msec);
    if (status != 0) {
        return HAL_STATUS_BUSY;
    }

    return retTxNum;
}

static int32_t i2c_Master_ReadPoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec)
{
    int32_t  status      =   0;
    uint32_t retRxNum    =   0;
    uint32_t end_tick    =   0U;
    uint8_t  *buff       =   i2cMsg->buff;
    uint32_t size        =   i2cMsg->size;
    uint32_t cmd_offset  =   i2cMsg->offset;

    LL_I2C_DisableAll_Intr(i2cReg);

    end_tick = HAL_BOARD_GetTime(HAL_TIME_MS) + HAL_MSecsToTicks(msec);

    /* Check bus IDLE(10ms) */
    status = i2c_CheckBusIdle(i2cReg, msec);
    if (status != 0) {
        return HAL_STATUS_BUSY;
    }

    while (size > retRxNum) {
        /* Tx Abort Checkout */
        if (LL_I2C_Get_Tx_Abort_Src(i2cReg) & (I2C_ABORT_ALL)) {
            LOG_ERR("Receive Data Abort (0x%08X)", LL_I2C_Get_Tx_Abort_Src(i2cReg));
            retRxNum = HAL_STATUS_ERROR;
            break;
        }

        if (false == LL_I2C_isTx_Fifo_Full(i2cReg)) {
            if ((cmd_offset == size - 1U) && i2cMsg->stop == true) {
                LL_I2C_PutCmdData(i2cReg, 0, I2C_CMD_READ_STOP);
            } else if ((cmd_offset == 0U) && (i2cMsg->stop == false)) {
                LL_I2C_PutCmdData(i2cReg, 0, I2C_CMD_READ_RESTART_STOP);
            } else if (cmd_offset < size - 1U) {
                LL_I2C_PutCmdData(i2cReg, 0, I2C_CMD_READ);
            }

            cmd_offset++;
        }

        /* Wait Rx Not Empty */
        if (!LL_I2C_isRx_Fifo_Empty(i2cReg)) {
            /* Receive Data */
            buff[i2cMsg->offset]   =   LL_I2C_GetCmdData(i2cReg);

            //  LOG_DBG("0x%02x  RFL %d", buff[i2cMsg->offset], LL_I2C_Rx_Fifo_Level(i2cReg));

            i2cMsg->offset++;
            retRxNum++;
        }

        /* Timeout */
        if (HAL_BOARD_GetTime(HAL_TIME_MS) >= end_tick) {
            LL_I2C_EnableReStart(i2cReg);
            LOG_ERR("recoverbus \n");
            return HAL_STATUS_TIMEOUT;
        }
    }

    /* Waiting Reveive Done (Stop/Restart) */
    uint32_t doneStatus = (I2C_INTR_STOP_DET | I2C_INTR_RESTART);
    if (0 != i2c_WaitIntrStatus(i2cReg, doneStatus, msec)) {
        LOG_ERR("Master Waiting Receive Done Timeout");
        retRxNum = HAL_STATUS_TIMEOUT;
    }

    /* Check bus IDLE(10ms) */
    status = i2c_CheckBusIdle(i2cReg, msec);
    if (status != 0) {
        return HAL_STATUS_BUSY;
    }

    return retRxNum;
}
#endif


#if CONFIG_I2C_SLAVE
static int32_t i2c_Slave_WritePoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec)
{
    uint32_t  retTxNum    =  0;
    uint32_t  end_tick    =  0U;
    uint8_t  *buff        =  i2cMsg->buff;
    uint32_t  size        =  i2cMsg->size;
    uint32_t  offset      =  i2cMsg->offset;

    LL_I2C_DisableAll_Intr(i2cReg);

    end_tick = HAL_BOARD_GetTime(HAL_TIME_MS) + HAL_MSecsToTicks(msec);

    while (size > retTxNum) {
        if (LL_I2C_Get_Intr_RawStat(i2cReg) & I2C_INTR_RD_REQ) {
            LL_I2C_ClearReadReq_Intr(i2cReg);

            LL_I2C_PutCmdData(i2cReg, buff[offset], I2C_CMD_WRITE);

            offset++;
            retTxNum++;
            i2cMsg->offset++;

            // LOG_DBG("0x%02X  TXF %d", buff[offset], LL_I2C_Rx_Fifo_Level(i2cReg));
        }

        /* Timeout */
        if (HAL_BOARD_GetTime(HAL_TIME_MS) >= end_tick) {
            LL_I2C_EnableReStart(i2cReg);
            LOG_ERR("recoverbus \n");

            return HAL_STATUS_TIMEOUT;
        }
    }

    /* Waiting Tranfer Done (Stop/Restart) */
    uint32_t  doneStatus =  (I2C_INTR_STOP_DET | I2C_INTR_RESTART);
    if (0 != i2c_WaitIntrStatus(i2cReg, doneStatus, msec)) {
        LOG_ERR("slave waiting Tranfer Done Timeout");
        retTxNum = HAL_STATUS_TIMEOUT;
    }

    return retTxNum;
}

static int32_t i2c_Slave_ReadPoll(i2c_dev_t *i2cReg, i2c_Msg_t *i2cMsg, uint32_t msec)
{
    uint32_t  retRxNum   =  0U;
    uint32_t  end_tick   =  0U;
    uint8_t  *buff       =  i2cMsg->buff;
    uint32_t  size       =  i2cMsg->size;
    uint32_t  offset     =  i2cMsg->offset;

    LL_I2C_DisableAll_Intr(i2cReg);

    end_tick = HAL_BOARD_GetTime(HAL_TIME_MS) + HAL_MSecsToTicks(msec);

    while (size > retRxNum) {
        uint32_t fifoNum = LL_I2C_Rx_Fifo_Level(i2cReg);

        if (fifoNum > 0) {
            buff[offset]  =  LL_I2C_GetCmdData(i2cReg);

            // LOG_DBG("0x%02x RFL %d", buff[offset], LL_I2C_Rx_Fifo_Level(i2cReg));

            offset++;
            retRxNum++;
            i2cMsg->offset++;
        }

        /* Timeout */
        if (HAL_BOARD_GetTime(HAL_TIME_MS) >= end_tick) {
            LL_I2C_EnableReStart(i2cReg);
            LOG_ERR("recoverbus \n");

            return HAL_STATUS_TIMEOUT;
        }
    }

    /* Waiting Tranfer Done (Stop/Restart) */
    uint32_t  doneStatus =  (I2C_INTR_STOP_DET | I2C_INTR_RESTART);
    if (0 != i2c_WaitIntrStatus(i2cReg, doneStatus, msec)) {
        LOG_ERR("slave Waiting Receive Done Timeout");
        retRxNum = HAL_STATUS_TIMEOUT;
    }

    return retRxNum;
}
#endif


#if CONFIG_I2C_MASTER && CONFIG_I2C_TRANS_IT_ENABLE
static int32_t i2c_Master_WriteIntr(I2C_Device_t *pi2cDev, uint32_t msec)
{
    HAL_Status_t status     =  HAL_STATUS_ERROR;
    uint32_t     retTxNum   =  0U;
    i2c_Msg_t   *i2cMsg     =  &pi2cDev->tx_msg;
    i2c_dev_t   *i2cReg     =  (i2c_dev_t *)(pi2cDev->device.reg);


    status = i2c_SemaphoreCreate(&pi2cDev->txSem);
    if (HAL_STATUS_OK != status) {
        return HAL_STATUS_ERROR;
    }

    /* Set threshold */
    if (i2cMsg->size > 0U && (i2cMsg->size < LL_I2C_FIFO_DEPTH)) {
        LL_I2C_Set_TxFifo_Threshold(i2cReg, i2cMsg->size - 1U);
    } else {
        LL_I2C_Set_TxFifo_Threshold(i2cReg, I2C_TX_FIFO_LEVEL);
    }

    /* Unmask interrupts. */
    uint32_t irqStatus =  I2C_IRQSTATUS_ERROR |
                          I2C_IRQSTATUS_TX_FIFO_EMPTY |
                          I2C_IRQSTATUS_STOP;
    HAL_I2C_EnableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);

    /* Enable controller. */
    if (!LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Enable(i2cReg);
    }

    status = HAL_SemaphoreWait(&pi2cDev->txSem, msec);
    if (HAL_STATUS_OK != status) {
        LL_I2C_EnableReStart(i2cReg);
        LOG_ERR("TX Smeaphore Timeout");

        HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);
        retTxNum = i2cMsg->offset;
        pi2cDev->State    = I2C_STATE_IDLE;
        i2cMsg->buff   = 0;
        i2cMsg->size   = 0;
        i2cMsg->offset = 0;
        return HAL_STATUS_TIMEOUT;
    }

    HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);

    retTxNum = i2cMsg->offset;

    pi2cDev->State  = I2C_STATE_IDLE;
    i2cMsg->buff   = 0;
    i2cMsg->size   = 0;
    i2cMsg->offset = 0;

    return retTxNum;
}

static int32_t i2c_Master_ReadIntr(I2C_Device_t *pi2cDev, uint32_t msec)
{
    HAL_Status_t status     =  HAL_STATUS_ERROR;
    uint32_t     retRxNum   =  0U;
    i2c_Msg_t   *i2cMsg     =  &pi2cDev->rx_msg;
    i2c_dev_t   *i2cReg     =  (i2c_dev_t *)(pi2cDev->device.reg);


    status = i2c_SemaphoreCreate(&pi2cDev->rxSem);
    if (HAL_STATUS_OK != status) {
        return HAL_STATUS_ERROR;
    }

    /* Set threshold */
    LL_I2C_Set_TxFifo_Threshold(i2cReg, I2C_TX_FIFO_LEVEL);
    LL_I2C_Set_RxFifo_Threshold(i2cReg, I2C_RX_FIFO_LEVEL);

    /* Unmask interrupts. */
    uint32_t irqStatus =  I2C_IRQSTATUS_ERROR |
                          I2C_IRQSTATUS_RX_FIFO_FULL |
                          I2C_IRQSTATUS_TX_FIFO_EMPTY;
    HAL_I2C_EnableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);

    /* Enable controller. */
    if (!LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Enable(i2cReg);
    }

    status = HAL_SemaphoreWait(&pi2cDev->rxSem, msec);
    if (HAL_STATUS_OK != status) {
        LL_I2C_EnableReStart(i2cReg);
        LOG_ERR("RX Smeaphore Timeout");

        HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);
        retRxNum = i2cMsg->offset;
        pi2cDev->State    = I2C_STATE_IDLE;
        i2cMsg->buff   = 0;
        i2cMsg->size   = 0;
        i2cMsg->offset = 0;
        return HAL_STATUS_TIMEOUT;
    }

    HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);

    retRxNum = i2cMsg->offset;
    pi2cDev->State  = I2C_STATE_IDLE;
    i2cMsg->buff   = 0;
    i2cMsg->size   = 0;
    i2cMsg->offset = 0;

    return retRxNum;
}
#endif

#if (CONFIG_I2C_SLAVE && CONFIG_I2C_TRANS_IT_ENABLE)
static int32_t i2c_Slave_WriteIntr(I2C_Device_t *pi2cDev, uint32_t msec)
{
    HAL_Status_t status     =  HAL_STATUS_ERROR;
    uint32_t     retTxNum   =  0U;
    i2c_Msg_t   *i2cMsg     =  &pi2cDev->tx_msg;
    i2c_dev_t   *i2cReg     =  (i2c_dev_t *)(pi2cDev->device.reg);


    status = i2c_SemaphoreCreate(&pi2cDev->txSem);
    if (HAL_STATUS_OK != status) {
        return HAL_STATUS_ERROR;
    }

    /* Set threshold */
    if (i2cMsg->size > 0U && (i2cMsg->size < LL_I2C_FIFO_DEPTH)) {
        LL_I2C_Set_TxFifo_Threshold(i2cReg, i2cMsg->size - 1U);
    } else {
        LL_I2C_Set_TxFifo_Threshold(i2cReg, I2C_TX_FIFO_LEVEL);
    }

    /* Unmask interrupts. */
    uint32_t irqStatus =  I2C_IRQSTATUS_ERROR |
                          I2C_IRQSTATUS_READ_REQ |
                          I2C_IRQSTATUS_TX_FIFO_EMPTY |
                          I2C_IRQSTATUS_STOP;
    HAL_I2C_EnableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);

    /* Enable controller. */
    if (!LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Enable(i2cReg);
    }

    status = HAL_SemaphoreWait(&pi2cDev->txSem, msec);
    if (HAL_STATUS_OK != status) {
        LL_I2C_EnableReStart(i2cReg);
        LOG_ERR("TX Smeaphore Timeout");

        HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);
        retTxNum = i2cMsg->offset;
        pi2cDev->State  = I2C_STATE_IDLE;
        i2cMsg->buff   = 0;
        i2cMsg->size   = 0;
        i2cMsg->offset = 0;
        return HAL_STATUS_TIMEOUT;
    }

    HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);

    retTxNum = i2cMsg->offset;

    pi2cDev->State  = I2C_STATE_IDLE;
    i2cMsg->buff   = 0;
    i2cMsg->size   = 0;
    i2cMsg->offset = 0;

    return retTxNum;
}


static int32_t i2c_Slave_ReadIntr(I2C_Device_t *pi2cDev, uint32_t msec)
{
    HAL_Status_t status     =  HAL_STATUS_ERROR;
    uint32_t     retRxNum   =  0U;
    i2c_Msg_t   *i2cMsg     =  &pi2cDev->rx_msg;
    i2c_dev_t   *i2cReg     =  (i2c_dev_t *)(pi2cDev->device.reg);

    status = i2c_SemaphoreCreate(&pi2cDev->rxSem);
    if (HAL_STATUS_OK != status) {
        LOG_ERR("failed to create smeaphore");
        return HAL_STATUS_ERROR;
    }

    /* Set threshold */
    LL_I2C_Set_RxFifo_Threshold(i2cReg, I2C_RX_FIFO_LEVEL);

    /* Unmask interrupts. */
    uint32_t irqStatus =  I2C_IRQSTATUS_ERROR |
                          I2C_IRQSTATUS_READ_DONE |
                          I2C_IRQSTATUS_RX_FIFO_FULL;
    HAL_I2C_EnableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);


    /* Enable controller. */
    if (!LL_I2C_IsAble(i2cReg)) {
        LL_I2C_Enable(i2cReg);
    }

    status = HAL_SemaphoreWait(&pi2cDev->rxSem, msec);
    if (HAL_STATUS_OK != status) {
        LL_I2C_EnableReStart(i2cReg);
        LOG_ERR("RX Smeaphore Timeout");

        HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);
        retRxNum = i2cMsg->offset;
        pi2cDev->State  = I2C_STATE_IDLE;
        i2cMsg->buff   = 0;
        i2cMsg->size   = 0;
        i2cMsg->offset = 0;
        return HAL_STATUS_TIMEOUT;
    }

    HAL_I2C_DisableIRQ((HAL_Dev_t *)pi2cDev, irqStatus);

    LOG_DBG_HEX(pi2cDev->rx_msg.buff, i2cMsg->size, "rd:");

    retRxNum = i2cMsg->offset;

    pi2cDev->State  = I2C_STATE_IDLE;
    i2cMsg->buff   = 0;
    i2cMsg->size   = 0;
    i2cMsg->offset = 0;

    return retRxNum;
}
#endif


#if (CONFIG_I2C_MASTER)
static int32_t i2c_Master_WriteDMA(I2C_Device_t *pi2cDev, uint32_t msec)
{
    uint32_t retTxNum  =  0U;
    return retTxNum;
}

static int32_t i2c_Master_ReadDMA(I2C_Device_t *pi2cDev, uint32_t msec)
{
    uint32_t retRxNum  =  0U;
    return retRxNum;
}
#endif

#if ((CONFIG_I2C_SLAVE) && (CONFIG_I2C_TRANS_DMA_ENABLE == 1))
static int32_t i2c_Slave_ReadDMA(I2C_Device_t *pi2cDev, uint32_t msec)
{

    uint32_t retTxNum  =  0U;
    return retTxNum;
}

static int32_t i2c_Slave_WriteDMA(I2C_Device_t *pi2cDev, uint32_t msec)
{
    uint32_t retRxNum  =  0U;
    return retRxNum;
}
#endif


#if (CONFIG_I2C_MASTER && CONFIG_I2C_TRANS_IT_ENABLE)
static uint32_t i2c_Master_WriteTxFifo(I2C_Device_t *pi2cDev)
{
    i2c_dev_t    *i2cReg       =   (i2c_dev_t *)(pi2cDev->device.reg);
    i2c_Msg_t    *ptxMsg       =    &pi2cDev->tx_msg;
    uint32_t     txLeftNum     =    ptxMsg->size - ptxMsg->offset;
    uint32_t     fifoNum       =    LL_I2C_FIFO_DEPTH - LL_I2C_Tx_Fifo_Level(i2cReg);

    txLeftNum = (txLeftNum > fifoNum) ? fifoNum : txLeftNum;

    for (int i = 0; i < txLeftNum; i++) {
        if (ptxMsg->offset == (ptxMsg->size - 1U)) {
            LL_I2C_PutCmdData(i2cReg, ptxMsg->buff[ptxMsg->offset], I2C_CMD_WRITE_STOP);
        } else {
            LL_I2C_PutCmdData(i2cReg, ptxMsg->buff[ptxMsg->offset], I2C_CMD_WRITE);
        }
        ptxMsg->offset++;
    }

    if (ptxMsg->offset >= ptxMsg->size) {
        LL_I2C_Disable_Intr_Mask(i2cReg, LL_I2C_INTR_TX_EMPTY_MASK);
        //HAL_SemaphoreRelease(&pi2cDev->txSem);
    }

    return txLeftNum;
}


static uint32_t i2c_Master_ReadRxFifoPre(I2C_Device_t *pi2cDev)
{
    static uint32_t cmd_offset = 0U; /* master read cmd offset */

    i2c_dev_t    *i2cReg       =   (i2c_dev_t *)(pi2cDev->device.reg);
    i2c_Msg_t    *prxMsg       =    &pi2cDev->rx_msg;
    uint32_t      txLeftNum    =    prxMsg->size - cmd_offset;
    uint32_t      fifoNum      =    LL_I2C_FIFO_DEPTH - LL_I2C_Tx_Fifo_Level(i2cReg);

    txLeftNum = (txLeftNum > fifoNum) ? fifoNum : txLeftNum;

    for (int i = 0; i < txLeftNum; i++) {
        if (!LL_I2C_isTx_Fifo_Full(i2cReg)) {
            if (cmd_offset >= prxMsg->size - 1U) {
                LL_I2C_PutCmdData(i2cReg, 0, I2C_CMD_READ_STOP);
            } else {
                LL_I2C_PutCmdData(i2cReg, 0, I2C_CMD_READ);
            }
            cmd_offset++;
        }
    }

    if (cmd_offset >= prxMsg->size) {
        cmd_offset = 0U;
        return 1;
    }

    return 0;
}

static void i2c_Master_ReadRxFifo(I2C_Device_t *pi2cDev)
{
    i2c_dev_t    *i2cReg       =   (i2c_dev_t *)(pi2cDev->device.reg);
    i2c_Msg_t    *prxMsg       =    &pi2cDev->rx_msg;
    uint32_t     fifoNum       =    LL_I2C_Rx_Fifo_Level(i2cReg);

    for (int i = 0; i < fifoNum; i++) {
        prxMsg->buff[prxMsg->offset] = LL_I2C_GetCmdData(i2cReg);
        prxMsg->offset++;
    }

    if (!LL_I2C_isRx_Fifo_Empty(i2cReg)) {
        i2c_Master_ReadRxFifo(pi2cDev);
    }

    if (prxMsg->offset >= prxMsg->size) {
        LL_I2C_Disable_Intr_Mask(i2cReg, LL_I2C_INTR_RX_FULL_MASK |
                                         LL_I2C_INTR_TX_EMPTY_MASK);

        pi2cDev->State   =  I2C_STATE_IDLE;
        HAL_SemaphoreRelease(&pi2cDev->rxSem);
    }
}
#endif

#if (CONFIG_I2C_SLAVE && CONFIG_I2C_TRANS_IT_ENABLE)
static void i2c_Slave_WriteTxFifo(I2C_Device_t *pi2cDev)
{
    i2c_dev_t    *i2cReg       =   (i2c_dev_t *)(pi2cDev->device.reg);
    i2c_Msg_t    *ptxMsg       =    &pi2cDev->tx_msg;
    uint32_t     txLeftNum     =    ptxMsg->size - ptxMsg->offset;
    uint32_t     fifoNum       =    LL_I2C_FIFO_DEPTH - LL_I2C_Tx_Fifo_Level(i2cReg);

    txLeftNum = (txLeftNum > fifoNum) ? fifoNum : txLeftNum;

    for (int i = 0; i < txLeftNum; i++) {
        LL_I2C_PutCmdData(i2cReg, ptxMsg->buff[ptxMsg->offset], I2C_CMD_WRITE);
        ptxMsg->offset++;
    }

    if (ptxMsg->offset >= ptxMsg->size) {
        LL_I2C_Disable_Intr_Mask(i2cReg, LL_I2C_INTR_RD_REQ_MASK  |
                                         LL_I2C_INTR_TX_EMPTY_MASK);
        // HAL_SemaphoreRelease(&pi2cDev->txSem);
    }
}

static void i2c_Slave_ReadRxFifo(I2C_Device_t *pi2cDev)
{
    i2c_dev_t    *i2cReg       =   (i2c_dev_t *)(pi2cDev->device.reg);
    i2c_Msg_t    *prxMsg       =    &pi2cDev->rx_msg;
    uint32_t     fifoNum       =    LL_I2C_Rx_Fifo_Level(i2cReg);

    for (int i = 0; i < fifoNum; i++) {
        prxMsg->buff[prxMsg->offset] = LL_I2C_GetCmdData(i2cReg);
        prxMsg->offset++;
    }

    if (!LL_I2C_isRx_Fifo_Empty(i2cReg)) {
        i2c_Slave_ReadRxFifo(pi2cDev);
    }
    if (prxMsg->offset >= prxMsg->size) {
        LL_I2C_Disable_Intr_Mask(i2cReg, LL_I2C_INTR_RX_FULL_MASK |
                                         LL_I2C_INTR_RX_DONE_MASK);
        HAL_SemaphoreRelease(&pi2cDev->rxSem);
    }
}
#endif


/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
