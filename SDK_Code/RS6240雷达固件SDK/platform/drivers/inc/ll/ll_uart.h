/**
 **************************************************************************************************
 * @brief   psic_ll_uart.h
 * @brief   uart hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_UART_H
#define _PSIC_LL_UART_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

#include "types.h"
#include "hw_uart.h"
#include "soc.h"

/** @addtogroup PSIC_LL_Driver
  * @{
  */


/** @defgroup UART_LL UART
  * @{
  */

#define UART0_DEV                       ((uart_dev_t *)   UART0_BASE_ADDR) /*!< 0x4001 1C00 */
#define UART1_DEV                       ((uart_dev_t *)   UART1_BASE_ADDR) /*!< 0x4000 5000 */
#define UART2_DEV                       ((uart_dev_t *)   UART2_BASE_ADDR) /*!< 0x4001 2000 */
/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */

/**
  * @brief LL UART Init Structure definition
  */
typedef struct {
    uint32_t baudrate;                  /*!< This field defines expected UART communication baud rate.              */
    uint32_t data_width;                /*!< Specifies the number of data bits transmitted or received in a frame.  */
    uint32_t stop_bits;                 /*!< Specifies the number of stop bits transmitted.                         */
    uint32_t parity;                    /*!< Specifies the parity mode.                                             */
    uint32_t is_auto_flow_ctrl;         /*!< Specifies whether the hardware flow control mode is enabled or disabled.*/
} ll_uart_init_t;

/**
  * @}
  */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */

#define LL_UART_TX_FIFO_THRES_0_CHAR            0U  /*!< FIFO reaches 0 characters in FIFO                          */
#define LL_UART_TX_FIFO_THRES_2_CHAR            1U  /*!< FIFO reaches 2 characters in FIFO                          */
#define LL_UART_TX_FIFO_THRES_1_4               2U  /*!< FIFO reaches 16 characters in FIFO, 1/4 of its depth       */
#define LL_UART_TX_FIFO_THRES_1_2               3U  /*!< FIFO reaches 32 characters in FIFO, 1/2 of its depth       */

#define LL_UART_RX_FIFO_THRES_1_CHAR            0U  /*!< FIFO reaches 1 characters in FIFO                          */
#define LL_UART_RX_FIFO_THRES_1_4               1U  /*!< FIFO reaches 16 characters in FIFO, 1/4 of its depth       */
#define LL_UART_RX_FIFO_THRES_1_2               2U  /*!< FIFO reaches 32 characters in FIFO, 1/2 of its depth       */
#define LL_UART_RX_FIFO_THRES_2_LESS            3U  /*!< FIFO reaches 62 characters in FIFO, 2 less than full       */

#define LL_UART_DATA_WIDTH_5B                   0U  /*!< 7 bits word length : Start bit, 5 data bits, 1.5 stop bits */
#define LL_UART_DATA_WIDTH_6B                   1U  /*!< 8 bits word length : Start bit, 6 data bits, n stop bits   */
#define LL_UART_DATA_WIDTH_7B                   2U  /*!< 9 bits word length : Start bit, 7 data bits, n stop bits   */
#define LL_UART_DATA_WIDTH_8B                   3U  /*!< 10 bits word length : Start bit, 8 data bits, n stop bits  */

#define LL_UART_DATA_WIDTH_9B                   4U  /*!< 9 bits word length : Start bit, 9 data bits, n stop bits
                                                     *   must congfig 9 bit mode
                                                     */

#define LL_UART_STOP_BITS_1                     0U  /*!< 1 stop bit                                                 */
#define LL_UART_STOP_BITS_1_5                   1U  /*!< 1.5 stop bits                                              */
#define LL_UART_STOP_BITS_2                     1U  /*!< 2 stop bits                                                */
#define LL_UART_STOP_BITS_1_5_OR_2              1U  /*!< 1.5 stop bits                                              */

#define LL_UART_PARITY_NONE                     0U  /*!< Parity control disabled and None Parity is selected         */
#define LL_UART_PARITY_ODD                      1U  /*!< Parity control enabled  and Odd Parity is selected          */
#define LL_UART_PARITY_EVEN                     3U  /*!< Parity control enabled  and Even Parity is selected         */

#define LL_UART_FLW_DE_ASSERTED                 0U  /*!< RTS/CTS asserted, logic 1                                   */
#define LL_UART_FLW_ASSERTED                    1U  /*!< RTS/CTS asserted, logic 0                                   */

#define LL_UART_FLW_CTRL_NONE                   0U
#define LL_UART_FLW_CTRL_CTS_RTS                1U

#define LL_UART_LS_RX_READY                     HW_BIT(0)
#define LL_UART_LS_OVERRUN_ERR                  HW_BIT(1)
#define LL_UART_LS_PARITY_ERR                   HW_BIT(2)
#define LL_UART_LS_FRAME_ERR                    HW_BIT(3)
#define LL_UART_LS_BREAK                        HW_BIT(4)
#define LL_UART_LS_TX_DATA_EMPTY                HW_BIT(5)
#define LL_UART_LS_TX_EMPTY                     HW_BIT(6)
#define LL_UART_LS_FIFO_ERR                     HW_BIT(7)

#define LL_UART_ISR_MODEM_STAT                  HW_BIT(0)
#define LL_UART_ISR_NO_INTR_PEND                HW_BIT(1)
#define LL_UART_ISR_TDR_EMPTY                   HW_BIT(2)
#define LL_UART_ISR_RD_AVAILABLE                HW_BIT(3)
#define LL_UART_ISR_LINE_STAT                   HW_BIT(4)
#define LL_UART_ISR_BUSY_DETECT                 HW_BIT(5)
#define LL_UART_ISR_CHAR_TO                     HW_BIT(6)


#define LL_UART_BDRT_DIV_Pos                        4
#define LL_UART_BDRT_DEC_Pos                        6
#define LL_UART_BDRT_DIV_Msk                        (0xF)

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */


/**
  * @brief  Reset uart and fifo
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_Reset(uart_dev_t *uart)
{
    uart->SRCR = UR_Msk | RFR_Msk | XFR_Msk;
}


/**
  * @brief  Read Receiver Data register (Receive Data value, all bits)
  * @param  uart UART Instance
  * @retval Value between Min_Data=0x00 and Max_Data=0x1FF
  */
__HW_STATIC_INLINE
uint32_t LL_UART_ReceiveData(uart_dev_t *uart)
{
    return uart->RDR;
}

/**
  * @brief  Write in Transmitter Data Register (Transmit Data value, all bits)
  * @param  uart UART Instance
  * @param  Value between Min_Data=0x00 and Max_Data=0xFF
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_TransmitData(uart_dev_t *uart, uint32_t value)
{
    uart->TDR = value;
}

/**
  * @brief  Configure UART DIVR register for achieving expected Baud Rate value.
  * @note   baud rate = (serial clock freq) / (16 * divisor)
  *         divisor   = (serial clock freq) / (16 * baud rate)
  * @param  uart UART Instance
  * @param  PrescalerValue Value between Min_Data=0x0000 and Max_Data=0xFFFF
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetPrescaler(uart_dev_t *uart, uint32_t baud_rate, uint32_t bus_freq)
{
#if 0
    uint32_t int_div = 0;
    uint32_t dec_div = 0;
    uint32_t dec_diff = 0;
    int_div = (baud_rate << 4);
    /* rounding */
    int_div = (bus_freq + (int_div >> 1)) / int_div;

    dec_diff = ((int_div * baud_rate) << LL_UART_BDRT_DIV_Pos);
    dec_diff = (bus_freq >= dec_diff) ? (bus_freq - dec_diff) : (dec_diff - bus_freq);

    dec_div = (dec_diff << LL_UART_BDRT_DEC_Pos);
    /* rounding */
    dec_div = (dec_div + (dec_div >> 1)) / baud_rate;
    uart->DSR = (int_div & (DLL_Msk | DLHU_Msk)) | ((dec_div & 0x3f) << DLF_Pos);
#else
    uint32_t int_frac_div = 0;
    int_frac_div = (bus_freq << 2) / baud_rate;
    uart->DSR = ((int_frac_div >> LL_UART_BDRT_DEC_Pos) & (DLL_Msk | DLHU_Msk)) |
                ((int_frac_div & 0x3f) << 16);

#endif

}


/**
  * @brief  Return prescaler value
  * @note   baud rate = (serial clock freq) / (16 * divisor)
  * @param  uart UART Instance
  * @retval prescaler value (Value between Min_Data=0x0000 and Max_Data=0xFFFF)
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetPrescaler(uart_dev_t *uart)
{
    return ((uart->DSR & DLF_Msk) >> DLF_Pos) | ((uart->DSR & (DLL_Msk | DLHU_Msk)) << LL_UART_BDRT_DIV_Pos);
}


/**
  * @brief  FIFO Mode Enable
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableFIFO(uart_dev_t *uart)
{
    HW_SET_MSK(uart->FCR, FIFOE_Msk);
}

/**
  * @brief  FIFO Mode Disable
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableFIFO(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->FCR, FIFOE_Msk);
}

/**
  * @brief  Indicate if FIFO Mode is enabled
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsEnabledFIFO(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->FCR, FIFOE_Pos);
}


/**
  * @brief  Request a Receive Data and FIFO flush
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_RequestRxDataFlush(uart_dev_t *uart)
{
    HW_SET_MSK(uart->FCR, RFIFOR_Msk);
}


/**
  * @brief  Request a Transmit data and FIFO flush
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_RequestTxDataFlush(uart_dev_t *uart)
{
    HW_SET_MSK(uart->FCR, XFIFOR_Msk);
}


/**
  * @brief  Configure TX FIFO Threshold
  * @param  UARTx UART Instance
  * @param  Threshold This parameter can be one of the following values:
  *         @arg @ref LL_UART_TX_FIFO_THRES_0_CHAR
  *         @arg @ref LL_UART_TX_FIFO_THRES_2_CHAR
  *         @arg @ref LL_UART_TX_FIFO_THRES_1_4
  *         @arg @ref LL_UART_TX_FIFO_THRES_1_2
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetTXFIFOThreshold(uart_dev_t *uart, uint32_t threshold)
{
    HW_SET_MSK_VAL(uart->FCR, TET_Msk, TET_Pos, threshold);
}

/**
  * @brief  Return TX FIFO Threshold Configuration
  * @param  uart UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_TX_FIFO_THRES_0_CHAR
  *         @arg @ref LL_UART_TX_FIFO_THRES_2_CHAR
  *         @arg @ref LL_UART_TX_FIFO_THRES_1_4
  *         @arg @ref LL_UART_TX_FIFO_THRES_1_2
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetTXFIFOThreshold(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->FCR, TET_Msk, TET_Pos);
}


/**
  * @brief  Configure RX FIFO Threshold
  * @param  uart UART Instance
  * @param  Threshold This parameter can be one of the following values:
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_CHAR
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_4
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_2
  *         @arg @ref LL_UART_RX_FIFO_THRES_2_LESS
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetRXFIFOThreshold(uart_dev_t *uart, uint32_t threshold)
{
    HW_SET_MSK_VAL(uart->FCR, RT_Msk, RT_Pos, threshold);
}

/**
  * @brief  Return RX FIFO Threshold Configuration
  * @param  uart UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_CHAR
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_4
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_2
  *         @arg @ref LL_UART_RX_FIFO_THRES_2_LESS
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetRXFIFOThreshold(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->FCR, RT_Msk, RT_Pos);
}

/**
  * @brief  Configure TX and RX FIFOs Threshold
  * @param  uart UART Instance
  * @param  TXThreshold This parameter can be one of the following values:
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_CHAR
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_4
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_2
  *         @arg @ref LL_UART_RX_FIFO_THRES_2_LESS
  * @param  RXThreshold This parameter can be one of the following values:
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_CHAR
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_4
  *         @arg @ref LL_UART_RX_FIFO_THRES_1_2
  *         @arg @ref LL_UART_RX_FIFO_THRES_2_LESS
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_ConfigFIFOsThreshold(uart_dev_t *uart, uint32_t tx_threshold, uint32_t rx_threshold)
{
    HW_SET_MSK_VAL(uart->FCR, TET_Msk, TET_Pos, tx_threshold);
    HW_SET_MSK_VAL(uart->FCR, RT_Msk, RT_Pos, rx_threshold);
}


/**
  * @brief  Set Word length (i.e. nb of data bits, excluding start and stop bits)
  * @param  uart UART Instance
  * @param  DataWidth This parameter can be one of the following values:
  *         @arg @ref LL_UART_DATA_WIDTH_5B
  *         @arg @ref LL_UART_DATA_WIDTH_6B
  *         @arg @ref LL_UART_DATA_WIDTH_7B
  *         @arg @ref LL_UART_DATA_WIDTH_8B
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetDataWidth(uart_dev_t *uart, uint32_t data_width)
{
    HW_SET_MSK_VAL(uart->CR, DLS_Msk, DLS_Pos, data_width);
}


/**
  * @brief  Return Word length (i.e. nb of data bits, excluding start and stop bits)
  * @param  uart UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_DATA_WIDTH_5B
  *         @arg @ref LL_UART_DATA_WIDTH_6B
  *         @arg @ref LL_UART_DATA_WIDTH_7B
  *         @arg @ref LL_UART_DATA_WIDTH_8B
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetDataWidth(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->CR, DLS_Msk, DLS_Pos);
}


/**
  * @brief  Set the length of the stop bits
  * @param  uart UART Instance
  * @param  StopBits This parameter can be one of the following values:
  *         @arg @ref LL_UART_STOP_BITS_1
  *         @arg @ref LL_UART_STOP_BITS_1_5_OR_2 if data width is 5 bits, stop bit = 1.5
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetStopBitsLength(uart_dev_t *uart, uint32_t stop_bits)
{
    HW_SET_MSK_VAL(uart->CR, STOP_BIT_Msk, STOP_BIT_Pos, stop_bits);
}

/**
  * @brief  Retrieve the length of the stop bits
  * @param  uart UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_STOP_BITS_1
  *         @arg @ref LL_UART_STOP_BITS_1_5_OR_2 if data width is 5 bits, stop bit = 1.5
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetStopBitsLength(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->CR, STOP_BIT_Msk, STOP_BIT_Pos);
}



/**
  * @brief  Configure Parity (enabled/disabled and parity mode if enabled).
  * @param  uart UART Instance
  * @param  Parity This parameter can be one of the following values:
  *         @arg @ref LL_UART_PARITY_NONE
  *         @arg @ref LL_UART_PARITY_ODD
  *         @arg @ref LL_UART_PARITY_EVEN
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetParity(uart_dev_t *uart, uint32_t parity)
{
  HW_SET_MSK_VAL(uart->CR, PARITY_Msk, PARITY_Pos, parity);
}


/**
  * @brief  Return Parity configuration (enabled/disabled and parity mode if enabled)
  * @param  UARTx UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_PARITY_NONE
  *         @arg @ref LL_UART_PARITY_ODD
  *         @arg @ref LL_UART_PARITY_EVEN
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetParity(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->CR, PARITY_Msk, PARITY_Pos);
}

/**
  * @brief  Configure Character frame format (Datawidth, Parity control, Stop Bits)
  * @param  uart UART Instance
  * @param  DataWidth This parameter can be one of the following values:
  *         @arg @ref LL_UART_DATA_WIDTH_5B
  *         @arg @ref LL_UART_DATA_WIDTH_6B
  *         @arg @ref LL_UART_DATA_WIDTH_7B
  *         @arg @ref LL_UART_DATA_WIDTH_8B
  * @param  Parity This parameter can be one of the following values:
  *         @arg @ref LL_UART_PARITY_NONE
  *         @arg @ref LL_UART_PARITY_ODD
  *         @arg @ref LL_UART_PARITY_EVEN
  * @param  StopBits This parameter can be one of the following values:
  *         @arg @ref LL_UART_STOP_BITS_1
  *         @arg @ref LL_UART_STOP_BITS_1_5_OR_2 if data width is 5 bits, stop bit = 1.5
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_ConfigCharacter(uart_dev_t *uart, uint32_t data_width, uint32_t parity,
                                              uint32_t stop_bits)
{
    HW_SET_MSK_VAL(uart->CR, DLS_Msk, DLS_Pos, data_width);
    HW_SET_MSK_VAL(uart->CR, STOP_BIT_Msk, STOP_BIT_Pos, stop_bits);
    HW_SET_MSK_VAL(uart->CR, PARITY_Msk, PARITY_Pos, parity);
}


/**
  * @brief  Enable Break sending
  * @param  UARTx UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableBreakSending(uart_dev_t *uart)
{
    HW_SET_MSK(uart->CR, BC_Msk);
}


/**
  * @brief  Disable Break sending
  * @param  UARTx UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableBreakSending(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->CR, BC_Msk);
}


/**
  * @brief  Indicate if Break Mode is enabled
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsEnabledBreak(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->CR, BC_Pos);
}


/**
  * @brief  Enable HW auto flow Control
  * @param  UARTx UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableHWFlowCtrl(uart_dev_t *uart)
{
    HW_SET_MSK(uart->CR, AFCE_Msk);
}

/**
  * @brief  Disable HW auto flow Control
  * @param  UARTx UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableHWFlowCtrl(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->CR, AFCE_Msk);
    HW_CLR_MSK(uart->CR, RTS_Msk);
}


/**
  * @brief  Disable HW auto flow Control
  * @param  uart UART Instance
  * @param  DataWidth This parameter can be one of the following values:
  *         @arg @ref LL_UART_FLW_CTRL_NONE
  *         @arg @ref LL_UART_FLW_CTRL_CTS_RTS
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetHWFlowCtrl(uart_dev_t *uart, uint32_t flow)
{
    if (flow) {
        HW_SET_MSK(uart->CR, AFCE_Msk);
        HW_SET_MSK(uart->CR, RTS_Msk);
    } else {
        HW_CLR_MSK(uart->CR, AFCE_Msk);
        HW_CLR_MSK(uart->CR, RTS_Msk);
    }
}


/**
  * @brief  Return HW Flow Control configuration (both CTS and RTS)
  * @param  UARTx UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetHWFlowCtrl(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->CR, AFCE_Pos) | HW_TEST_BIT(uart->CR, RTS_Pos);
}


/**
  * @brief  Set RTS value
  * @param  uart UART Instance
  * @param  RTS This parameter can be one of the following values:
  *         @arg @ref LL_UART_FLW_DE_ASSERTED
  *         @arg @ref LL_UART_FLW_ASSERTED
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SetRTS(uart_dev_t *uart, uint32_t rts_value)
{
    HW_SET_MSK_VAL(uart->CR, RTS_Msk, RTS_Pos, rts_value);
}


/**
  * @brief  Get CTS value
  * @param  uart UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_FLW_DE_ASSERTED
  *         @arg @ref LL_UART_FLW_ASSERTED
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetCTS(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->SR, CTS_Pos);
}


/**
  * @brief  Return line status
  * @param  uart UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_LS_RX_READY
  *         @arg @ref LL_UART_LS_OVERRUN_ERR
  *         @arg @ref LL_UART_LS_PARITY_ERR
  *         @arg @ref LL_UART_LS_FRAME_ERR
  *         @arg @ref LL_UART_LS_BREAK
  *         @arg @ref LL_UART_LS_TX_DATA_EMPTY
  *         @arg @ref LL_UART_LS_TX_EMPTY
  *         @arg @ref LL_UART_LS_FIFO_ERR
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetLineStatus(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->SR, LS_Msk, LS_Pos);
}


/**
  * @brief  Return transmit complete status
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsActiveFlag_TC(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->SR, TMET_Pos);
}


/**
  * @brief  Return number of data in the transmit FIFO
  * @param  uart UART Instance
  * @retval Returned value data number 0-63:
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetTxFIFOLevel(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->FSR, TFL_Msk, TFL_Pos);
}


/**
  * @brief  Return number of data in the receive FIFO
  * @param  uart UART Instance
  * @retval Returned value data number 0-63:
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetRxFIFOLevel(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->FSR, RFL_Msk, RFL_Pos);
}


/**
  * @brief  Check if the UART TX FIFO Not Full Flag is set or not
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsActiveFlag_TXFNF(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->FSR, TFNF_Pos);
}


/**
  * @brief  Check if the UART TX FIFO Empty Flag is set or not
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsActiveFlag_TXFE(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->FSR, TFE_Pos);
}


/**
  * @brief  Check if the UART RX FIFO Not Empty Flag is set or not
  * @rmtoll ISR          RXFF          LL_UART_IsActiveFlag_RXFF
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsActiveFlag_RXFNE(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->FSR, RFNE_Pos);
}


/**
  * @brief  Check if the UART RX FIFO Full Flag is set or not
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsActiveFlag_RXFF(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->FSR, RFF_Pos);
}


/**
  * @brief  Check if the UART RX Ready Flag is set or not
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsActiveFlag_RXRD(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->SR, DR_Pos);
}

/**
  * @brief  Return interrupt id
  * @param  uart UART Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UART_ISR_MODEM_STAT
  *         @arg @ref LL_UART_ISR_NO_INTR_PEND
  *         @arg @ref LL_UART_ISR_TDR_EMPTY
  *         @arg @ref LL_UART_ISR_RD_AVAILABLE
  *         @arg @ref LL_UART_ISR_LINE_STAT
  *         @arg @ref LL_UART_ISR_BUSY_DETECT
  *         @arg @ref LL_UART_ISR_CHAR_TO
  */
__HW_STATIC_INLINE
uint32_t LL_UART_GetIrqFlag(uart_dev_t *uart)
{
    return HW_GET_MSK_VAL(uart->ISR, ISR_Msk, ISR_Pos);
}


/**
  * @brief  Enable Receiver data Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableIT_RD(uart_dev_t *uart)
{
    HW_SET_MSK(uart->IER, ERBFI_Msk);
}


/**
  * @brief  Disable Receiver data Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableIT_RD(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->IER, ERBFI_Msk);
}


/**
  * @brief  Check if the UART receive data Interrupt is enabled or disabled
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsEnabledIT_RD(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->IER, ERBFI_Pos);
}


/**
  * @brief  Enable transmit data Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableIT_TD(uart_dev_t *uart)
{
    HW_SET_MSK(uart->IER, ETBEI_Msk);
}


/**
  * @brief  Disable transmit data Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableIT_TD(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->IER, ETBEI_Msk);
}


/**
  * @brief  Check if the UART transmit data Interrupt is enabled or disabled
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsEnabledIT_TD(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->IER, ETBEI_Pos);
}


/**
  * @brief  Enable line status Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableIT_Line(uart_dev_t *uart)
{
    HW_SET_MSK(uart->IER, ELSI_Msk);
}


/**
  * @brief  Disable line status Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableIT_Line(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->IER, ELSI_Msk);
}


/**
  * @brief  Check if the UART line status Interrupt is enabled or disabled
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsEnabledIT_Line(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->IER, ELSI_Pos);
}


/**
  * @brief  Enable modem Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableIT_Modem(uart_dev_t *uart)
{
    HW_SET_MSK(uart->IER, EDSSI_Msk);
}


/**
  * @brief  Disable modem Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableIT_Modem(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->IER, EDSSI_Msk);
}


/**
  * @brief  Check if the UART modem status Interrupt is enabled or disabled
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsEnabledIT_Modem(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->IER, EDSSI_Pos);
}


/**
  * @brief  Enable FIFO threshold Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_EnableIT_FIFOThreshold(uart_dev_t *uart)
{
    HW_SET_MSK(uart->IER, PTIME_Msk);
}

/**
  * @brief  Disable FIFO threshold Interrupt
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_DisableIT_FIFOThreshold(uart_dev_t *uart)
{
    HW_CLR_MSK(uart->IER, PTIME_Msk);
}

/**
  * @brief  Check if the UART program fifo threshold Interrupt is enabled or disabled
  * @param  uart UART Instance
  * @retval State of bit (1 or 0).
  */
__HW_STATIC_INLINE
uint32_t LL_UART_IsEnabledIT_FIFOThreshold(uart_dev_t *uart)
{
    return HW_TEST_BIT(uart->IER, PTIME_Pos);
}


/**
  * @brief  Send char
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SendChar(uart_dev_t *uart, uint8_t val)
{
    while ((uart->FSR & TFNF_Msk) == 0) {
    }
    uart->TDR = val;
    while (((uart->FSR & TFE_Msk) == 0) || ((uart->SR & TMET_Msk) == 0)) {
    }
}


/**
  * @brief  Send String
  * @param  uart UART Instance
  * @retval None
  */
__HW_STATIC_INLINE
void LL_UART_SendData(uart_dev_t *uart, uint8_t *data, uint32_t len)
{
    uint32_t idx;

    for (idx = 0; idx < len; idx++) {
        while ((uart->FSR & TFNF_Msk) == 0) {
        }
        uart->TDR = data[idx];
    }

    while (((uart->FSR & TFE_Msk) == 0) || ((uart->SR & TMET_Msk) == 0)) {
    }
}



/**
  * @}
  */


/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_UART_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
