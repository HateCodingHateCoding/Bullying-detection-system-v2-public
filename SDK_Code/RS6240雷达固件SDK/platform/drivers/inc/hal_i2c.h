/**
 ******************************************************************************
 * @file    hal_i2c.h
 * @brief   I2C hardware abstract layer define header.
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

#ifndef __HAL_I2C_H_
#define __HAL_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ----------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "hal_dev.h"

#include "ll_i2c.h"
//#include "hal_dma.h"

/* Exported types.
 * ----------------------------------------------------------------------------
 */
/**
 * @brief I2C Controller ID
 */
typedef enum {
    I2C0_ID                        =    0U,
    I2C1_ID                        =    1U,
    I2C_NUM,
} I2C_ID_t;

/**
 * @brief I2C Role
 */
typedef enum {
    I2C_MODE_MASTER                =    0U,
    I2C_MODE_SLAVE                 =    1U,
    I2C_MODE_NUM
} I2C_Mode_t;

/**
 * @brief I2C Addressing Mode
 */
typedef enum {
    I2C_ADDR_WIDTH_7BIT             =    0U,  /* 7-bit addressing mode */
    I2C_ADDR_WIDTH_10BIT            =    1U,  /* 10-bit addressing mode */
} I2C_AddrWidth_t;

/**
 * @brief I2C Speed Mode
 */
typedef enum {
    I2C_SPEED_STANDARD              =    0U, /* Standard mode (100 Kbps). */
    I2C_SPEED_FAST                  =    1U, /* Fast mode (400 Kbps). */
    I2C_SPEED_FAST_PLUS             =    2U, /* Fast plus mode (1 Mbps). */
    I2C_SPEED_MODE_NUM,
} I2C_Speed_t;

/**
 * @brief I2C IRQ Status
 */
typedef enum {
    I2C_IRQSTATUS_START             =     HAL_BIT(0),
    I2C_IRQSTATUS_STOP              =     HAL_BIT(1),
    I2C_IRQSTATUS_READ_REQ          =     HAL_BIT(2),
    I2C_IRQSTATUS_READ_DONE         =     HAL_BIT(3),
    I2C_IRQSTATUS_ERROR             =     HAL_BIT(4),
    I2C_IRQSTATUS_TX_FIFO_EMPTY     =     HAL_BIT(5),
    I2C_IRQSTATUS_RX_FIFO_FULL      =     HAL_BIT(6),
    I2C_IRQSTATUS_ALL               =     HAL_BIT(0) | HAL_BIT(1) | HAL_BIT(2) |
                                          HAL_BIT(3) | HAL_BIT(4) | HAL_BIT(5) |
                                          HAL_BIT(6),
} I2C_IRQStatus_t;

/**
 * @brief I2C Transfer Direction
 */
typedef enum {
    I2C_TRANSFDIR_TX                 =    0U,
    I2C_TRANSFDIR_RX                 =    1U,
} I2C_TransfDir_t;

/**
 * @brief I2C Transfer type.
 */
typedef enum {
    I2C_TRANSFMODE_NORMAL            =    0U,
    I2C_TRANSFMODE_INTERRUPT         =    1U,
    I2C_TRANSFMODE_DMA               =    2U,
} I2C_TransfMode_t;


/**
 * @brief I2C Extended Attribute Control.
 */
typedef enum {
    I2C_EXTATTR_RESET                =    0U,
    I2C_EXTATTR_SET_TXFIFO_LEVEL     =    1U,
    I2C_EXTATTR_SET_RXFIFO_LEVEL     =    2U,
    I2C_EXTATTR_GET_TX_FLUSH_CNT     =    3U,
    I2C_EXTATTR_GET_ENABLE_INTR      =    4U,
    I2C_EXTATTR_FLUSH_RXFIFO         =    5U,
    I2C_EXTATTR_WAKEUP_CTRL_ABLE     =    6U,
    I2C_EXTATTR_WAKEUP_THRESHOLD     =    7U,
    I2C_EXTATTR_WAKEUP_CALLBACK      =    8U,
    I2C_EXTATTR_WAKEUP_MODE          =    9U,
} I2C_ExtAttr_t;

typedef enum {
    I2C_WK_MODE_AUTO,
    I2C_WK_MODE_MANUAL,
}i2c_WakeupMode_t;

/**
 * @brief I2C Callback Typedef
 */
typedef HAL_Callback_t I2C_Callback_t;


/**
 * @brief I2C Initialize Parameters
 */
typedef struct {
    I2C_Mode_t          mode;
    I2C_Speed_t         speed;
    I2C_Callback_t      busErrCb;
} I2C_InitParam_t;


/* Private macro.
 * ----------------------------------------------------------------------------
 */
#define I2C0_IRQ_PRIORITY                               (9U)
#define I2C1_IRQ_PRIORITY                               (9U)

#define I2C_HIGH_SPEED_MASTER_CODE                      (0x1U)

#define I2C_TIMEOUT_DEFAULT_VALUE                       (10000U)

#define I2C_RESERVED_ADDR_00                            (0x00U)
#define I2C_RESERVED_ADDR_01                            (0x01U)
#define I2C_RESERVED_ADDR_02                            (0x02U)
#define I2C_RESERVED_ADDR_03                            (0x03U)
#define I2C_RESERVED_ADDR_04                            (0x04U)
#define I2C_RESERVED_ADDR_05                            (0x05U)
#define I2C_RESERVED_ADDR_06                            (0x06U)
#define I2C_RESERVED_ADDR_07                            (0x07U)

#define I2C_RESERVED_ADDR_78                            (0x78U)
#define I2C_RESERVED_ADDR_79                            (0x79U)
#define I2C_RESERVED_ADDR_7A                            (0x7AU)
#define I2C_RESERVED_ADDR_7B                            (0x7BU)
#define I2C_RESERVED_ADDR_7C                            (0x7CU)
#define I2C_RESERVED_ADDR_7D                            (0x7DU)
#define I2C_RESERVED_ADDR_7F                            (0x7FU)

#define I2C_TX_FIFO_LEVEL                               (LL_I2C_FIFO_DEPTH / 2)
#define I2C_RX_FIFO_LEVEL                               (LL_I2C_FIFO_THRESH_RX_FULL)


/* Exported constants.
 * ----------------------------------------------------------------------------
 */
#define I2C_LINE_NUM                                    (2U)

#define I2C_7BITADDR_MASK                               (0x7FU)
#define I2C_10BITADDR_MASK                              (0x3FFU)

/* 100 Kbit/s */
#define I2C_BITRATE_STANDARD                            (100000UL)
/* 400 Kbit/s */
#define I2C_BITRATE_FAST                                (400000UL)
/* 1 Mbit/s */
#define I2C_BITRATE_FAST_PLUS                           (1000000UL)
/* 3.4 Mbit/s */
#define I2C_BITRATE_HIGH                                (3400000UL)

/* Exported macro.
 * ----------------------------------------------------------------------------
 */
#if (CONFIG_SECTION_ATTRIBUTE_SRAM && CONFIG_HIF_DEVICE_COM_IIC)
#define __i2c_sram_text                                 __sram_text
#define __i2c_sram_rodata                               __sram_rodata
#define __i2c_sram_data                                 __sram_data
#define __i2c_sram_bss                                  __sram_bss
#else
#define __i2c_sram_text
#define __i2c_sram_rodata
#define __i2c_sram_data
#define __i2c_sram_bss
#endif


/* Exported functions.
 * ----------------------------------------------------------------------------
 */
/**
 * @brief  Initialize the I2C peripheral according to the specified parameters.
 * @param id  I2C device id. @ref I2C_ID_t
 * @param pinitParam I2C initial configuration. This must not be NULL.
 * @return   I2C device handler pointer
 */
HAL_Dev_t   *HAL_I2C_Init(I2C_ID_t id, const I2C_InitParam_t *pinitParam);

/**
 * @brief  DeInitialize the I2C peripheral.
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_DeInit(HAL_Dev_t *i2c);

/**
 * @brief  Open specified address sub-device.
 * @param i2c  I2C device handler pointer
 * @param addrWidth Address width @ref I2C_AddrWidth_t
 * @param addr      Target device address for master.
 *                  or register address for slave.
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_Open(HAL_Dev_t *i2c, I2C_AddrWidth_t addrWidth, const uint16_t addr);

/**
 * @brief  Close specified address sub-device.
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_Close(HAL_Dev_t *i2c);

/**
 * @brief  Enable I2C peripheral.
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_Enable(HAL_Dev_t *i2c);

/**
 * @brief  Disable I2C peripheral.
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_Disable(HAL_Dev_t *i2c);

/**
 * @brief Recoverd bus when bus stuck at low.
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_RecoverBus(HAL_Dev_t *i2c);

/**
 * @brief Set rx or tx transfer type.
 * @param i2c  I2C device handler pointer
 * @param tranfDir  Transfer direction. @ref I2C_TransfDir_t
 * @param tranfMode Transfer type. @ref I2C_TransfMode_t
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_SetTransferMode(HAL_Dev_t *i2c, I2C_TransfDir_t tranfDir, I2C_TransfMode_t tranfMode);

/**
 * @brief  1.Transmit in master mode an amount of data in blocking mode.
 *           or Transmit in slave mode an amount of data in blocking mode.
 *         2.Transmit in master mode an amount of data in non-blocking mode with Interrupt
 *           or Transmit in slave mode an amount of data in non-blocking mode with Interrupt
 *         3.Transmit in master mode an amount of data in non-blocking mode with DMA
 *           or Transmit in slave mode an amount of data in non-blocking mode with DMA
 * @param i2c  I2C device handler pointer
 * @param buff  Pointer to data buffer
 * @param size  Amount of data to be sent
 * @param msec  Timeout duration (ms)
 * @return    Negative HAL_STATUS_* errno for possible error codes.
 *            or Amount of data to be send.
 */
int32_t      HAL_I2C_Transmit(HAL_Dev_t *i2c, const uint8_t *buff, uint32_t size, uint32_t msec);

/**
 * @brief  1.Receive in master mode an amount of data in blocking mode.
 *           or Receives in slave mode an amount of data in blocking mode.
 *         2.Receives in master mode an amount of data in non-blocking mode with Interrupt
 *           or Receives in slave mode an amount of data in non-blocking mode with Interrupt
 *         3.Receives in master mode an amount of data in non-blocking mode with DMA
 *           or Receives in slave mode an amount of data in non-blocking mode with DMA
 * @param i2c  I2C device handler pointer
 * @param buff  Pointer to data buffer
 * @param size  Amount of data to be get
 * @param msec  Timeout duration (ms)
 * @return    Negative HAL_STATUS_* errno for possible error codes.
 *            or Amount of data to be get.
 */
int32_t      HAL_I2C_Receive(HAL_Dev_t *i2c, uint8_t *buff, uint32_t size, uint32_t msec);


/**
 * @brief  Extended control attribute for custom I2C device.
 * @param i2c  I2C device handler pointer
 * @param attr Need control the attribute
 * @param arg  custom parameters pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_ExtendControl(HAL_Dev_t *i2c, I2C_ExtAttr_t attr, void *arg);

/**
 * @brief  Get data from rx fifo
 * @param i2c  I2C device handler pointer
 * @return data (byte)
 */
uint8_t      HAL_I2C_GetRxData(HAL_Dev_t *i2c);

/**
 * @brief  Put data in Tx fifo
 * @param i2c  I2C device handler pointer
 * @param rxData  Put data (byte)
 * @param cmd @ref I2C_Cmd_t
 * @note  If in slave role, cmd only I2C_CMD_WRITE.
 *
 * @Note: Slave mode need uesd I2C_CMD_WRITE.@ref I2C_Cmd_t
 */
void         HAL_I2C_PutTxData(HAL_Dev_t *i2c, uint8_t txData, I2C_Cmd_t cmd);

/**
 * @brief  Adjust Tx fifo full or not.
 * @param i2c  I2C device handler pointer
 * @return Ture or false
 */
bool         HAL_I2C_IsTxReady(HAL_Dev_t *i2c);

/**
 * @brief  Adjust Rx fifo empty or not.
 * @param i2c  I2C device handler pointer
 * @return  Ture or false
 */
bool         HAL_I2C_IsRxReady(HAL_Dev_t *i2c);

/**
 * @brief  Adjust Tx fifo empty or not.
 * @param i2c  I2C device handler pointer
 * @return  Ture or false
 */
bool         HAL_I2C_IsTxEmpty(HAL_Dev_t *i2c);

#if (CONFIG_PM == 1)

HAL_Status_t HAL_i2c_PM_Suspend(HAL_Dev_t *i2c);

HAL_Status_t HAL_i2c_PM_Resume(HAL_Dev_t *i2c);


#endif

#if (CONFIG_I2C_TRANS_IT_ENABLE == 1)
/**
 * @brief   Register I2C master or slave ISR callback function.
 * @param i2c  I2C device handler pointer
 * @param irqCb     Transfer callbcak. @ref I2C_Callback_t
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_RegisterIRQ(HAL_Dev_t *i2c, I2C_Callback_t *irqCb);

/**
 * @brief Unregister I2C tx or rx  ISR callback function.
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_UnregisterIRQ(HAL_Dev_t *i2c);

/**
 * @brief  Enable intrrupt for intrrupt mode tranfser
 * @param i2c  I2C device handler pointer
 * @param irqStatus  ISR status. Value of @ref I2C_IRQSTATUS_*.
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_EnableIRQ(HAL_Dev_t *i2c, uint32_t irqStatus);

/**
 * @brief  Disable intrrupt for intrrupt mode tranfser
 * @param i2c  I2C device handler pointer
 * @param irqStatus  ISR status. Value of @ref I2C_IRQSTATUS_*.
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_DisableIRQ(HAL_Dev_t *i2c, uint32_t irqStatus);

/**
 * @brief  Clear intrrupt Flag for intrrupt mode tranfser
 * @param i2c  I2C device handler pointer
 * @param irqStatus  ISR status. Value of @ref I2C_IRQSTATUS_*.
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_ClearIRQ(HAL_Dev_t *i2c, uint32_t irqStatus);

/**
 * @brief Get intrrupt status occured.
 * @param i2c  I2C device handler pointer
 * @return ISR status. Value of @ref I2C_IRQSTATUS_*.
 */
uint32_t     HAL_I2C_GetIRQStatus(HAL_Dev_t *i2c);
#endif


/* TODO: Not support now */
#if (CONFIG_I2C_TRANS_DMA_ENABLE == 1)
/**
 * @brief
 * @param i2c  I2C device handler pointer
 * @param tranfDir
 * @param dmaCb
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_RegisterDMA(HAL_Dev_t *i2c, I2C_TransfDir_t tranfDir, I2C_Callback_t *dmaCb);

/**
 * @brief
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_UnregisterDMA(HAL_Dev_t *i2c);

/**
 * @brief
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_StartTxDMA(HAL_Dev_t *i2c);

/**
 * @brief  Abort a master I2C IT or DMA process communication with Interrupt.
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_AbortTxDMA(HAL_Dev_t *i2c);

/**
 * @brief
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_StartRxDMA(HAL_Dev_t *i2c);

/**
 * @brief
 * @param i2c  I2C device handler pointer
 * @return  HAL_STATUS_OK if successful,
 *          Negative HAL_STATUS_* errno for possible error codes.
 */
HAL_Status_t HAL_I2C_AbortRxDMA(HAL_Dev_t *i2c);
#endif


#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_H_ */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */

