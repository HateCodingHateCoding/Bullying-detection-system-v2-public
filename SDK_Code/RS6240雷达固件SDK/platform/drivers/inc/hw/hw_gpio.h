/****************************************************************
* @file  hw_gpio.h
* @brief gpio hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_GPIO_H_
#define _HW_GPIO_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup gpio
 * @{
 * */

/***
 * @brief General Purpose I/O
 */



typedef struct {
    __IOM uint32_t FUNC[4];                                 /*0x0000,null                                               */
    __IOM uint32_t PULLUPDWN_SET[2];                        /*0x0010,null                                               */
    __IOM uint32_t OUTPUT_TYPE;                             /*0x0018,null                                               */
    __IOM uint32_t DRIVING[2];                              /*0x001c,null                                               */
    __IOM uint32_t OUTPUT;                                  /*0x0024,null                                               */
    __IM  uint32_t INPUT;                                   /*0x0028,null                                               */
    __IOM uint32_t INT_EN;                                  /*0x002c,null                                               */
    __IOM uint32_t INT_TYPE[4];                             /*0x0030,null                                               */
    __IOM uint32_t INT_CLR;                                 /*0x0040,null                                               */
    __IM  uint32_t INT_FLAG;                                /*0x0044,null                                               */
    __IOM uint32_t DBC;                                     /*0x0048,null                                               */
    __IOM uint32_t I2C_MODE;                                /*0x004c,null                                               */
} gpio_dev_t;

typedef struct {
    __IOM uint32_t DELAY_CHAIN_CTRL;                        /*0x0000,                                               */
} delay_chain_ctrl_t;

/*!< 0x0000 FUNC
*null
*/
#define FUNC_Msk                                            (0x0F << FUNC_Pos)
#define FUNC_Pos                                            0
#define FUNC_PosL                                           0
#define FUNC_PosH                                           3
#define FUNC_Idx_Sft                                        3
#define FUNC_Pos_Msk                                        0x7
#define FUNC_Pos_Sft                                        2

/*!< 0x0010 PULLUPDWN_SE
*null
*/
#define PULLUPDWN_SET_Msk                                    (0x03 << PULLUPDWN_SET_Pos)
#define PULLUPDWN_SET_Pos                                    0
#define PULLUPDWN_SET_PosL                                   0
#define PULLUPDWN_SET_PosH                                   1
#define PULLUPDWN_SET_Idx_Sft                                4
#define PULLUPDWN_SET_Pos_Msk                                0xF
#define PULLUPDWN_SET_Pos_Sft                                1

/*!< 0x0018 OUTPUT_TYPE
*null
*/
#define OUTPUT_TYPE_Msk                                     (0xFFFF << OUTPUT_TYPE_Pos)
#define OUTPUT_TYPE_Pos                                     0
#define OUTPUT_TYPE_PosL                                    0
#define OUTPUT_TYPE_PosH                                    15


/*!< 0x001c DRIVING
*null
*/
#define DRIVING_Msk                                         (0x03 << DRIVING_Pos)
#define DRIVING_Pos                                         0
#define DRIVING_PosL                                        0
#define DRIVING_PosH                                        1
#define DRIVING_Idx_Sft                                     4
#define DRIVING_Pos_Msk                                     0xF
#define DRIVING_Pos_Sft                                     1

/*!< 0x0024 OUTPUT
*null
*/
#define OUTPUT_Msk                                          (0x01 << OUTPUT_Pos)
#define OUTPUT_Pos                                          0
#define OUTPUT_PosL                                         0
#define OUTPUT_PosH                                         0


/*!< 0x0028 INPUT
*null
*/
#define INPUT_Msk                                           (0x01 << INPUT_Pos)
#define INPUT_Pos                                           0
#define INPUT_PosL                                          0
#define INPUT_PosH                                          0


/*!< 0x002c INT_EN
*null
*/
#define INT_EN_Msk                                          (0x01 << INT_EN_Pos)
#define INT_EN_Pos                                          0
#define INT_EN_PosL                                         0
#define INT_EN_PosH                                         0


/*!< 0x0030 INT_TYPE
*null
*/
#define INT_TYPE_Msk                                        (0x07 << INT_TYPE_Pos)
#define INT_TYPE_Pos                                        0
#define INT_TYPE_PosL                                       0
#define INT_TYPE_PosH                                       4
#define INT_TYPE_Idx_Sft                                    3
#define INT_TYPE_Pos_Msk                                    0x7
#define INT_TYPE_Pos_Sft                                    2

/*!< 0x0040 INT_CLR
*W1C
*/
#define INT_CLR_Msk                                         (0x01 << INT_CLR_Pos)
#define INT_CLR_Pos                                         0
#define INT_CLR_PosL                                        0
#define INT_CLR_PosH                                        0


/*!< 0x0044 INT_FLAG
*null
*/
#define INT_FLAG_Msk                                        (0x01 << INT_FLAG_Pos)
#define INT_FLAG_Pos                                        0
#define INT_FLAG_PosL                                       0
#define INT_FLAG_PosH                                       0


/*!< 0x0048 DBC
*null
*/
#define DBC_Msk                                             (0x1F << DBC_Pos)
#define DBC_Pos                                             0
#define DBC_PosL                                            0
#define DBC_PosH                                            4


/*!< 0x004c I2C_MODE
*null
*/
#define I2C_MODE_Msk                                        (0x01 << I2C_MODE_Pos)
#define I2C_MODE_Pos                                        0
#define I2C_MODE_PosL                                       0
#define I2C_MODE_PosH                                       0

/*!< 0x0000 DELAY_CHAIN_CTRL
*null
*/
#define QSPI0_SCLKOUT_DELAY_TRIM_Msk                        (0x0F << QSPI0_SCLKOUT_DELAY_TRIM_Pos)
#define QSPI0_SCLKOUT_DELAY_TRIM_Pos                        20
#define QSPI0_SCLKOUT_DELAY_TRIM_PosL                       20
#define QSPI0_SCLKOUT_DELAY_TRIM_PosH                       23

/*!< 0x0000 DELAY_CHAIN_CTRL
*null
*/
#define QSPI0_SCLKOUT_DELAY_EN_Msk                          (0x01 << QSPI0_SCLKOUT_DELAY_EN_Pos)
#define QSPI0_SCLKOUT_DELAY_EN_Pos                          16
#define QSPI0_SCLKOUT_DELAY_EN_PosL                         16
#define QSPI0_SCLKOUT_DELAY_EN_PosH                         16

/*!< 0x0000 DELAY_CHAIN_CTRL
*null
*/
#define QSPI0_SCLKIN_DELAY_TRIM_Msk                         (0x0F << QSPI0_SCLKIN_DELAY_TRIM_Pos)
#define QSPI0_SCLKIN_DELAY_TRIM_Pos                         12
#define QSPI0_SCLKIN_DELAY_TRIM_PosL                        12
#define QSPI0_SCLKIN_DELAY_TRIM_PosH                        15

/*!< 0x0000 DELAY_CHAIN_CTRL
*null
*/
#define QSPI0_SCLKIN_DELAY_EN_Msk                           (0x01 << QSPI0_SCLKIN_DELAY_EN_Pos)
#define QSPI0_SCLKIN_DELAY_EN_Pos                           8
#define QSPI0_SCLKIN_DELAY_EN_PosL                          8
#define QSPI0_SCLKIN_DELAY_EN_PosH                          8

/*!< 0x0000 DELAY_CHAIN_CTRL
*null
*/
#define FLASHC_DELAY_TRIM_Msk                               (0x0F << FLASHC_DELAY_TRIM_Pos)
#define FLASHC_DELAY_TRIM_Pos                               4
#define FLASHC_DELAY_TRIM_PosL                              4
#define FLASHC_DELAY_TRIM_PosH                              7

/*!< 0x0000 DELAY_CHAIN_CTRL
*null
*/
#define FLASHC_DELAY_EN_Msk                                 (0x01 << FLASHC_DELAY_EN_Pos)
#define FLASHC_DELAY_EN_Pos                                 0
#define FLASHC_DELAY_EN_PosL                                0
#define FLASHC_DELAY_EN_PosH                                0

/**
* @}
* End of group HW_GPIO
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_GPIO_H_ */
