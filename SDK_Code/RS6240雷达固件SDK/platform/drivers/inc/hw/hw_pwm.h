/****************************************************************
* @file  PWM.h
* @brief PWM hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_PWM_H_
#define _HW_PWM_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup PWM
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __IOM uint32_t PWMCFG;                                  /*0x0000,           */
    __IOM uint32_t PWMINVERTTRIG;                           /*0x0004,           */
    __IOM uint32_t RSVD[3];                                 /*0x0008-0x0010,    */
    __IOM uint32_t PWMINTEN[2];                             /*0x0014-0x0018,    */
    __IO  uint32_t PWMRIS[2];                               /*0x001C-0x0020,    */
    __IOM uint32_t PWMIC[2];                                /*0x0024-0x0028,    */
    __IO  uint32_t PWMIS[2];                                /*0x002C-0x0030,    */
    __IOM uint32_t PWMCTL;                                  /*0x0034,           */
    __IOM uint32_t PWMLOADL[2];                             /*0x0038-0x003C,    */
          uint32_t RSVD1;                                   /*0x0040,           */
    __IO  uint32_t PWMCOUNTL[2];                            /*0x0044-0x0048,    */
          uint32_t RSVD2;                                   /*0x004C,           */
    __IOM uint32_t PWMCMPL[4];                              /*0x0050-0x005C,    */
          uint32_t RSVD3[2];                                /*0x0060-0x0064,    */
    __IOM uint32_t PWMDB[2];                                /*0x0068-0x006C,    */
          uint32_t RSVD4;                                   /*0x0070,           */
    __IOM uint32_t CAPCTL;                                  /*0x0074,           */
    __IOM uint32_t CAPINTEN;                                /*0x0078,           */
    __IO  uint32_t CAPRIS;                                  /*0x007C,           */
    __IOM uint32_t CAPIC;                                   /*0x0080,           */
    __IO  uint32_t CAPIS;                                   /*0x0084,           */
    __IOM uint32_t CAPTL[2];                                /*0x0088-0x008C,    */
          uint32_t RSVD5;                                   /*0x0090,           */
    __IOM uint32_t CAPMATCHL[2];                            /*0x0094-0x0098,    */
          uint32_t RSVD6[8];                                /*0x009C-0x00B8,    */
    __IO  uint32_t TIMCOUNTL[2];                            /*0x00BC-0x00C0,    */
          uint32_t RSVD7[7];                                /*0x00C4-0x00DC,    */
    __IOM uint32_t PWMCMPH[4];                              /*0x00E0-0x00EC,    */
          uint32_t RSVD8[5];                                /*0x00F0-0x0100,    */
    __IOM uint32_t PWMLOADH[2];                             /*0x0104-0x0108,    */
          uint32_t RSVD9;                                   /*0x010C,           */
    __IOM uint32_t CAPMATCHH[2];                            /*0x0110-0x0114,    */
          uint32_t RSVD10;                                  /*0x0118,           */
    __IO  uint32_t PWMCOUNTH[2];                            /*0x011C-0x0120,    */
          uint32_t RSVD11;                                  /*0x0124,           */
    __IO  uint32_t CAPTH[2];                                /*0x0128-0x012c,    */
          uint32_t RSVD12;                                  /*0x0130,           */
    __IO  uint32_t TIMCOUNTH[2];                            /*0x0134-0x0138,    */
          uint32_t RSVD13[4];                               /*0x013C-0x0148,    */
    __IOM uint32_t AUTOMODE;                                /*0x014C,           */
    __IOM uint32_t AUTOCNT[4];                              /*0x0150-0x015C,    */
          uint32_t RSVD14[2];                               /*0x0160-0x0164,    */
    __IOM uint32_t AUTOCMPA[4];                             /*0x0168-0x0174,    */
          uint32_t RSVD15[2];                               /*0x0178-0x017C,    */
    __IOM uint32_t AUTOCMPB[4];                             /*0x0180-0x018C,    */

} pwm_dev_t;


/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x0000 PWMCFG
 *
 */
#define CNTDIVEN_Msk                                          (0x01 << CNTDIV_Pos)
#define CNTDIVEN_Pos                                          27
#define CNTDIVEN_PosL                                         27
#define CNTDIVEN_PosH                                         27

#define CNTDIV_Msk                                          (0x07 << CNTDIV_Pos)
#define CNTDIV_Pos                                          24
#define CNTDIV_PosL                                         24
#define CNTDIV_PosH                                         26

/*!< 0x0000 PWMCFG
*
*/
#define CAP3EN_Msk                                          (0x01 << CAP3EN_Pos)
#define CAP3EN_Pos                                          15
#define CAP3EN_PosL                                         15
#define CAP3EN_PosH                                         15

/*!< 0x0000 PWMCFG
*
*/
#define CAP2EN_Msk                                          (0x01 << CAP2EN_Pos)
#define CAP2EN_Pos                                          14
#define CAP2EN_PosL                                         14
#define CAP2EN_PosH                                         14

/*!< 0x0000 PWMCFG
*
*/
#define CAP1EN_Msk                                          (0x01 << CAP1EN_Pos)
#define CAP1EN_Pos                                          13
#define CAP1EN_PosL                                         13
#define CAP1EN_PosH                                         13

/*!< 0x0000 PWMCFG
*
*/
#define CAP0EN_Msk                                          (0x01 << CAP0EN_Pos)
#define CAP0EN_Pos                                          12
#define CAP0EN_PosL                                         12
#define CAP0EN_PosH                                         12

/*!< 0x0000 PWMCFG
*
*/
#define PWM7EN_Msk                                          (0x01 << PWM7EN_Pos)
#define PWM7EN_Pos                                          7
#define PWM7EN_PosL                                         7
#define PWM7EN_PosH                                         7

/*!< 0x0000 PWMCFG
*
*/
#define PWM6EN_Msk                                          (0x01 << PWM6EN_Pos)
#define PWM6EN_Pos                                          6
#define PWM6EN_PosL                                         6
#define PWM6EN_PosH                                         6

/*!< 0x0000 PWMCFG
*
*/
#define PWM5EN_Msk                                          (0x01 << PWM5EN_Pos)
#define PWM5EN_Pos                                          5
#define PWM5EN_PosL                                         5
#define PWM5EN_PosH                                         5

/*!< 0x0000 PWMCFG
*
*/
#define PWM4EN_Msk                                          (0x01 << PWM4EN_Pos)
#define PWM4EN_Pos                                          4
#define PWM4EN_PosL                                         4
#define PWM4EN_PosH                                         4

/*!< 0x0000 PWMCFG
*
*/
#define PWM3EN_Msk                                          (0x01 << PWM3EN_Pos)
#define PWM3EN_Pos                                          3
#define PWM3EN_PosL                                         3
#define PWM3EN_PosH                                         3

/*!< 0x0000 PWMCFG
*
*/
#define PWM2EN_Msk                                          (0x01 << PWM2EN_Pos)
#define PWM2EN_Pos                                          2
#define PWM2EN_PosL                                         2
#define PWM2EN_PosH                                         2

/*!< 0x0000 PWMCFG
*
*/
#define PWM1EN_Msk                                          (0x01 << PWM1EN_Pos)
#define PWM1EN_Pos                                          1
#define PWM1EN_PosL                                         1
#define PWM1EN_PosH                                         1

/*!< 0x0000 PWMCFG
*
*/
#define PWM0EN_Msk                                          (0x01 << PWM0EN_Pos)
#define PWM0EN_Pos                                          0
#define PWM0EN_PosL                                         0
#define PWM0EN_PosH                                         0

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM7INV_Msk                                         (0x01 << PWM7INV_Pos)
#define PWM7INV_Pos                                         7
#define PWM7INV_PosL                                        7
#define PWM7INV_PosH                                        7

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM6INV_Msk                                         (0x01 << PWM6INV_Pos)
#define PWM6INV_Pos                                         6
#define PWM6INV_PosL                                        6
#define PWM6INV_PosH                                        6

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM5INV_Msk                                         (0x01 << PWM5INV_Pos)
#define PWM5INV_Pos                                         5
#define PWM5INV_PosL                                        5
#define PWM5INV_PosH                                        5

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM4INV_Msk                                         (0x01 << PWM4INV_Pos)
#define PWM4INV_Pos                                         4
#define PWM4INV_PosL                                        4
#define PWM4INV_PosH                                        4

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM3INV_Msk                                         (0x01 << PWM3INV_Pos)
#define PWM3INV_Pos                                         3
#define PWM3INV_PosL                                        3
#define PWM3INV_PosH                                        3

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM2INV_Msk                                         (0x01 << PWM2INV_Pos)
#define PWM2INV_Pos                                         2
#define PWM2INV_PosL                                        2
#define PWM2INV_PosH                                        2

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM1INV_Msk                                         (0x01 << PWM1INV_Pos)
#define PWM1INV_Pos                                         1
#define PWM1INV_PosL                                        1
#define PWM1INV_PosH                                        1

/*!< 0x0004 PWMINVERTTRIG
*
*/
#define PWM0INV_Msk                                         (0x01 << PWM0INV_Pos)
#define PWM0INV_Pos                                         0
#define PWM0INV_PosL                                        0
#define PWM0INV_PosH                                        0

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator B Down (channel5)
*/
#define INT2ENCMPBD_Msk                                     (0x01 << INT2ENCMPBD_Pos)
#define INT2ENCMPBD_Pos                                     29
#define INT2ENCMPBD_PosL                                    29
#define INT2ENCMPBD_PosH                                    29

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator A Down (channel4)
*/
#define INT2ENCMPAD_Msk                                     (0x01 << INT2ENCMPAD_Pos)
#define INT2ENCMPAD_Pos                                     28
#define INT2ENCMPAD_PosL                                    28
#define INT2ENCMPAD_PosH                                    28

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator B Up (channel5)
*/
#define INT2ENCMPBU_Msk                                     (0x01 << INT2ENCMPBU_Pos)
#define INT2ENCMPBU_Pos                                     27
#define INT2ENCMPBU_PosL                                    27
#define INT2ENCMPBU_PosH                                    27

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator A Up (channel4)
*/
#define INT2ENCMPAU_Msk                                     (0x01 << INT2ENCMPAU_Pos)
#define INT2ENCMPAU_Pos                                     26
#define INT2ENCMPAU_PosL                                    26
#define INT2ENCMPAU_PosH                                    26

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=Load (channel5/4)
*/
#define INT2ENCNTLOAD_Msk                                   (0x01 << INT2ENCNTLOAD_Pos)
#define INT2ENCNTLOAD_Pos                                   25
#define INT2ENCNTLOAD_PosL                                  25
#define INT2ENCNTLOAD_PosH                                  25

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=0 (channel5/4)
*/
#define INT2ENCNTZERO_Msk                                   (0x01 << INT2ENCNTZERO_Pos)
#define INT2ENCNTZERO_Pos                                   24
#define INT2ENCNTZERO_PosL                                  24
#define INT2ENCNTZERO_PosH                                  24

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator B Down (channel3)
*/
#define INT1ENCMPBD_Msk                                     (0x01 << INT1ENCMPBD_Pos)
#define INT1ENCMPBD_Pos                                     21
#define INT1ENCMPBD_PosL                                    21
#define INT1ENCMPBD_PosH                                    21

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator A Down (channel2)
*/
#define INT1ENCMPAD_Msk                                     (0x01 << INT1ENCMPAD_Pos)
#define INT1ENCMPAD_Pos                                     20
#define INT1ENCMPAD_PosL                                    20
#define INT1ENCMPAD_PosH                                    20

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator B Up (channel3)
*/
#define INT1ENCMPBU_Msk                                     (0x01 << INT1ENCMPBU_Pos)
#define INT1ENCMPBU_Pos                                     19
#define INT1ENCMPBU_PosL                                    19
#define INT1ENCMPBU_PosH                                    19

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator A Up (channel2)
*/
#define INT1ENCMPAU_Msk                                     (0x01 << INT1ENCMPAU_Pos)
#define INT1ENCMPAU_Pos                                     18
#define INT1ENCMPAU_PosL                                    18
#define INT1ENCMPAU_PosH                                    18

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=Load (channel3/2)
*/
#define INT1ENCNTLOAD_Msk                                   (0x01 << INT1ENCNTLOAD_Pos)
#define INT1ENCNTLOAD_Pos                                   17
#define INT1ENCNTLOAD_PosL                                  17
#define INT1ENCNTLOAD_PosH                                  17

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=0 (channel3/2)
*/
#define INT1ENCNTZERO_Msk                                   (0x01 << INT1ENCNTZERO_Pos)
#define INT1ENCNTZERO_Pos                                   16
#define INT1ENCNTZERO_PosL                                  16
#define INT1ENCNTZERO_PosH                                  16

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator B Down (channel1)
*/
#define INT0ENCMPBD_Msk                                     (0x01 << INT0ENCMPBD_Pos)
#define INT0ENCMPBD_Pos                                     13
#define INT0ENCMPBD_PosL                                    13
#define INT0ENCMPBD_PosH                                    13

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator A Down (channel0)
*/
#define INT0ENCMPAD_Msk                                     (0x01 << INT0ENCMPAD_Pos)
#define INT0ENCMPAD_Pos                                     12
#define INT0ENCMPAD_PosL                                    12
#define INT0ENCMPAD_PosH                                    12

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator B Up (channel1)
*/
#define INT0ENCMPBU_Msk                                     (0x01 << INT0ENCMPBU_Pos)
#define INT0ENCMPBU_Pos                                     11
#define INT0ENCMPBU_PosL                                    11
#define INT0ENCMPBU_PosH                                    11

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=comparator A Up (channel0)
*/
#define INT0ENCMPAU_Msk                                     (0x01 << INT0ENCMPAU_Pos)
#define INT0ENCMPAU_Pos                                     10
#define INT0ENCMPAU_PosL                                    10
#define INT0ENCMPAU_PosH                                    10

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=Load (channel1/0)
*/
#define INT0ENCNTLOAD_Msk                                   (0x01 << INT0ENCNTLOAD_Pos)
#define INT0ENCNTLOAD_Pos                                   9
#define INT0ENCNTLOAD_PosL                                  9
#define INT0ENCNTLOAD_PosH                                  9

/*!< 0x0014 PWMINTEN1
*Interrupt for counter=0 (channel1/0)
*/
#define INT0ENCNTZERO_Msk                                   (0x01 << INT0ENCNTZERO_Pos)
#define INT0ENCNTZERO_Pos                                   8
#define INT0ENCNTZERO_PosL                                  8
#define INT0ENCNTZERO_PosH                                  8

/*!< 0x0018 PWMINTEN2
*Interrupt for counter=comparator B Down (group3-channel7)
*/
#define INT3ENCMPBD_Msk                                     (0x01 << INT3ENCMPBD_Pos)
#define INT3ENCMPBD_Pos                                     5
#define INT3ENCMPBD_PosL                                    5
#define INT3ENCMPBD_PosH                                    5

/*!< 0x0018 PWMINTEN2
*Interrupt for counter=comparator A Down (group3-channel6)
*/
#define INT3ENCMPAD_Msk                                     (0x01 << INT3ENCMPAD_Pos)
#define INT3ENCMPAD_Pos                                     4
#define INT3ENCMPAD_PosL                                    4
#define INT3ENCMPAD_PosH                                    4

/*!< 0x0018 PWMINTEN2
*Interrupt for counter=comparator B Up (group3-channel7)
*/
#define INT3ENCMPBU_Msk                                     (0x01 << INT3ENCMPBU_Pos)
#define INT3ENCMPBU_Pos                                     3
#define INT3ENCMPBU_PosL                                    3
#define INT3ENCMPBU_PosH                                    3

/*!< 0x0018 PWMINTEN2
*Interrupt for counter=comparator A Up (group3-channel6)
*/
#define INT3ENCMPAU_Msk                                     (0x01 << INT3ENCMPAU_Pos)
#define INT3ENCMPAU_Pos                                     2
#define INT3ENCMPAU_PosL                                    2
#define INT3ENCMPAU_PosH                                    2

/*!< 0x0018 PWMINTEN2
*Interrupt for counter=Load (group3-channel7/6)
*/
#define INT3ENCNTLOAD_Msk                                   (0x01 << INT3ENCNTLOAD_Pos)
#define INT3ENCNTLOAD_Pos                                   1
#define INT3ENCNTLOAD_PosL                                  1
#define INT3ENCNTLOAD_PosH                                  1

/*!< 0x0018 PWMINTEN2
*Interrupt for counter=0 (group3-channel7/6)
*/
#define INT3ENCNTZERO_Msk                                   (0x01 << INT3ENCNTZERO_Pos)
#define INT3ENCNTZERO_Pos                                   0
#define INT3ENCNTZERO_PosL                                  0
#define INT3ENCNTZERO_PosH                                  0

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator B Down (group2-channel5)
*/
#define INTRIS2CMPBD_Msk                                    (0x01 << INTRIS2CMPBD_Pos)
#define INTRIS2CMPBD_Pos                                    29
#define INTRIS2CMPBD_PosL                                   29
#define INTRIS2CMPBD_PosH                                   29

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator A Down (group2-channel4)
*/
#define INTRIS2CMPAD_Msk                                    (0x01 << INTRIS2CMPAD_Pos)
#define INTRIS2CMPAD_Pos                                    28
#define INTRIS2CMPAD_PosL                                   28
#define INTRIS2CMPAD_PosH                                   28

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator B Up (group2-channel5)
*/
#define INTRIS2CMPBU_Msk                                    (0x01 << INTRIS2CMPBU_Pos)
#define INTRIS2CMPBU_Pos                                    27
#define INTRIS2CMPBU_PosL                                   27
#define INTRIS2CMPBU_PosH                                   27

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator A Up (group2-channel4)
*/
#define INTRIS2CMPAU_Msk                                    (0x01 << INTRIS2CMPAU_Pos)
#define INTRIS2CMPAU_Pos                                    26
#define INTRIS2CMPAU_PosL                                   26
#define INTRIS2CMPAU_PosH                                   26

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=Load (group2-channel5/4)
*/
#define INTRIS2CNTLOAD_Msk                                  (0x01 << INTRIS2CNTLOAD_Pos)
#define INTRIS2CNTLOAD_Pos                                  25
#define INTRIS2CNTLOAD_PosL                                 25
#define INTRIS2CNTLOAD_PosH                                 25

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=0 (group2-channel5/4)
*/
#define INTRIS2CNTZERO_Msk                                  (0x01 << INTRIS2CNTZERO_Pos)
#define INTRIS2CNTZERO_Pos                                  24
#define INTRIS2CNTZERO_PosL                                 24
#define INTRIS2CNTZERO_PosH                                 24

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator B Down (group1-channel3)
*/
#define INTRIS1CMPBD_Msk                                    (0x01 << INTRIS1CMPBD_Pos)
#define INTRIS1CMPBD_Pos                                    21
#define INTRIS1CMPBD_PosL                                   21
#define INTRIS1CMPBD_PosH                                   21

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator A Down (group1-channel2)
*/
#define INTRIS1CMPAD_Msk                                    (0x01 << INTRIS1CMPAD_Pos)
#define INTRIS1CMPAD_Pos                                    20
#define INTRIS1CMPAD_PosL                                   20
#define INTRIS1CMPAD_PosH                                   20

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator B Up (group1-channel3)
*/
#define INTRIS1CMPBU_Msk                                    (0x01 << INTRIS1CMPBU_Pos)
#define INTRIS1CMPBU_Pos                                    19
#define INTRIS1CMPBU_PosL                                   19
#define INTRIS1CMPBU_PosH                                   19

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator A Up (group1-channel2)
*/
#define INTRIS1CMPAU_Msk                                    (0x01 << INTRIS1CMPAU_Pos)
#define INTRIS1CMPAU_Pos                                    18
#define INTRIS1CMPAU_PosL                                   18
#define INTRIS1CMPAU_PosH                                   18

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=Load (group1-channel3/2)
*/
#define INTRIS1CNTLOAD_Msk                                  (0x01 << INTRIS1CNTLOAD_Pos)
#define INTRIS1CNTLOAD_Pos                                  17
#define INTRIS1CNTLOAD_PosL                                 17
#define INTRIS1CNTLOAD_PosH                                 17

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=0 (group1-channel3/2)
*/
#define INTRIS1CNTZERO_Msk                                  (0x01 << INTRIS1CNTZERO_Pos)
#define INTRIS1CNTZERO_Pos                                  16
#define INTRIS1CNTZERO_PosL                                 16
#define INTRIS1CNTZERO_PosH                                 16

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator B Down (group0-channel1)
*/
#define INTRIS0CMPBD_Msk                                    (0x01 << INTRIS0CMPBD_Pos)
#define INTRIS0CMPBD_Pos                                    13
#define INTRIS0CMPBD_PosL                                   13
#define INTRIS0CMPBD_PosH                                   13

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator A Down (group0-channel0)
*/
#define INTRIS0CMPAD_Msk                                    (0x01 << INTRIS0CMPAD_Pos)
#define INTRIS0CMPAD_Pos                                    12
#define INTRIS0CMPAD_PosL                                   12
#define INTRIS0CMPAD_PosH                                   12

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator B Up (group0-channel1)
*/
#define INTRIS0CMPBU_Msk                                    (0x01 << INTRIS0CMPBU_Pos)
#define INTRIS0CMPBU_Pos                                    11
#define INTRIS0CMPBU_PosL                                   11
#define INTRIS0CMPBU_PosH                                   11

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=comparator A Up (group0-channel0)
*/
#define INTRIS0CMPAU_Msk                                    (0x01 << INTRIS0CMPAU_Pos)
#define INTRIS0CMPAU_Pos                                    10
#define INTRIS0CMPAU_PosL                                   10
#define INTRIS0CMPAU_PosH                                   10

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=Load (group0-channel1/0)
*/
#define INTRIS0CNTLOAD_Msk                                  (0x01 << INTRIS0CNTLOAD_Pos)
#define INTRIS0CNTLOAD_Pos                                  9
#define INTRIS0CNTLOAD_PosL                                 9
#define INTRIS0CNTLOAD_PosH                                 9

/*!< 0x001c PWMRIS1
*Raw Interrupt for counter=0 (group0-channel1/0)
*/
#define INTRIS0CNTZERO_Msk                                  (0x01 << INTRIS0CNTZERO_Pos)
#define INTRIS0CNTZERO_Pos                                  8
#define INTRIS0CNTZERO_PosL                                 8
#define INTRIS0CNTZERO_PosH                                 8


/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator B Down (group5-channel11)
*/
#define INTRIS5CMPBD_Msk                                    (0x01 << INTRIS5CMPBD_Pos)
#define INTRIS5CMPBD_Pos                                    21
#define INTRIS5CMPBD_PosL                                   21
#define INTRIS5CMPBD_PosH                                   21

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator A Down (group5-channel10)
*/
#define INTRIS5CMPAD_Msk                                    (0x01 << INTRIS5CMPAD_Pos)
#define INTRIS5CMPAD_Pos                                    20
#define INTRIS5CMPAD_PosL                                   20
#define INTRIS5CMPAD_PosH                                   20

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator B Up (group5-channel11)
*/
#define INTRIS5CMPBU_Msk                                    (0x01 << INTRIS5CMPBU_Pos)
#define INTRIS5CMPBU_Pos                                    19
#define INTRIS5CMPBU_PosL                                   19
#define INTRIS5CMPBU_PosH                                   19

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator A Up (group5-channel10)
*/
#define INTRIS5CMPAU_Msk                                    (0x01 << INTRIS5CMPAU_Pos)
#define INTRIS5CMPAU_Pos                                    18
#define INTRIS5CMPAU_PosL                                   18
#define INTRIS5CMPAU_PosH                                   18

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=Load (group5-channel11/10)
*/
#define INTRIS5CNTLOAD_Msk                                  (0x01 << INTRIS5CNTLOAD_Pos)
#define INTRIS5CNTLOAD_Pos                                  17
#define INTRIS5CNTLOAD_PosL                                 17
#define INTRIS5CNTLOAD_PosH                                 17

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=0 (group5-channel11/10)
*/
#define INTRIS5CNTZERO_Msk                                  (0x01 << INTRIS5CNTZERO_Pos)
#define INTRIS5CNTZERO_Pos                                  16
#define INTRIS5CNTZERO_PosL                                 16
#define INTRIS5CNTZERO_PosH                                 16

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator B Down (group4-channel9)
*/
#define INTRIS4CMPBD_Msk                                    (0x01 << INTRIS4CMPBD_Pos)
#define INTRIS4CMPBD_Pos                                    13
#define INTRIS4CMPBD_PosL                                   13
#define INTRIS4CMPBD_PosH                                   13

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator A Down (group4-channel8)
*/
#define INTRIS4CMPAD_Msk                                    (0x01 << INTRIS4CMPAD_Pos)
#define INTRIS4CMPAD_Pos                                    12
#define INTRIS4CMPAD_PosL                                   12
#define INTRIS4CMPAD_PosH                                   12

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator B Up (group4-channel9)
*/
#define INTRIS4CMPBU_Msk                                    (0x01 << INTRIS4CMPBU_Pos)
#define INTRIS4CMPBU_Pos                                    11
#define INTRIS4CMPBU_PosL                                   11
#define INTRIS4CMPBU_PosH                                   11

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator A Up (group4-channel8)
*/
#define INTRIS4CMPAU_Msk                                    (0x01 << INTRIS4CMPAU_Pos)
#define INTRIS4CMPAU_Pos                                    10
#define INTRIS4CMPAU_PosL                                   10
#define INTRIS4CMPAU_PosH                                   10

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=Load (group4-channel9/8)
*/
#define INTRIS4CNTLOAD_Msk                                  (0x01 << INTRIS4CNTLOAD_Pos)
#define INTRIS4CNTLOAD_Pos                                  9
#define INTRIS4CNTLOAD_PosL                                 9
#define INTRIS4CNTLOAD_PosH                                 9

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=0 (group4-channel9/8)
*/
#define INTRIS4CNTZERO_Msk                                  (0x01 << INTRIS4CNTZERO_Pos)
#define INTRIS4CNTZERO_Pos                                  8
#define INTRIS4CNTZERO_PosL                                 8
#define INTRIS4CNTZERO_PosH                                 8

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator B Down (group3-channel7)
*/
#define INTRIS3CMPBD_Msk                                    (0x01 << INTRIS3CMPBD_Pos)
#define INTRIS3CMPBD_Pos                                    5
#define INTRIS3CMPBD_PosL                                   5
#define INTRIS3CMPBD_PosH                                   5

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator A Down (group3-channel6)
*/
#define INTRIS3CMPAD_Msk                                    (0x01 << INTRIS3CMPAD_Pos)
#define INTRIS3CMPAD_Pos                                    4
#define INTRIS3CMPAD_PosL                                   4
#define INTRIS3CMPAD_PosH                                   4

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator B Up (group3-channel7)
*/
#define INTRIS3CMPBU_Msk                                    (0x01 << INTRIS3CMPBU_Pos)
#define INTRIS3CMPBU_Pos                                    3
#define INTRIS3CMPBU_PosL                                   3
#define INTRIS3CMPBU_PosH                                   3

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=comparator A Up (group3-channel6)
*/
#define INTRIS3CMPAU_Msk                                    (0x01 << INTRIS3CMPAU_Pos)
#define INTRIS3CMPAU_Pos                                    2
#define INTRIS3CMPAU_PosL                                   2
#define INTRIS3CMPAU_PosH                                   2

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=Load (group3-channel7/6)
*/
#define INTRIS3CNTLOAD_Msk                                  (0x01 << INTRIS3CNTLOAD_Pos)
#define INTRIS3CNTLOAD_Pos                                  1
#define INTRIS3CNTLOAD_PosL                                 1
#define INTRIS3CNTLOAD_PosH                                 1

/*!< 0x0020 PWMRIS2
*Raw Interrupt for counter=0 (group3-channel7/6)
*/
#define INTRIS3CNTZERO_Msk                                  (0x01 << INTRIS3CNTZERO_Pos)
#define INTRIS3CNTZERO_Pos                                  0
#define INTRIS3CNTZERO_PosL                                 0
#define INTRIS3CNTZERO_PosH                                 0


/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator B Down (group2-channel5)
*/
#define INTIC2CMPBD_Msk                                     (0x01 << INTIC2CMPBD_Pos)
#define INTIC2CMPBD_Pos                                     29
#define INTIC2CMPBD_PosL                                    29
#define INTIC2CMPBD_PosH                                    29

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator A Down (group2-channel4)
*/
#define INTIC2CMPAD_Msk                                     (0x01 << INTIC2CMPAD_Pos)
#define INTIC2CMPAD_Pos                                     28
#define INTIC2CMPAD_PosL                                    28
#define INTIC2CMPAD_PosH                                    28

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator B Up (group2-channel5)
*/
#define INTIC2CMPBU_Msk                                     (0x01 << INTIC2CMPBU_Pos)
#define INTIC2CMPBU_Pos                                     27
#define INTIC2CMPBU_PosL                                    27
#define INTIC2CMPBU_PosH                                    27

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator A Up (group2-channel4)
*/
#define INTIC2CMPAU_Msk                                     (0x01 << INTIC2CMPAU_Pos)
#define INTIC2CMPAU_Pos                                     26
#define INTIC2CMPAU_PosL                                    26
#define INTIC2CMPAU_PosH                                    26

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=Load (group2-channel5/4)
*/
#define INTIC2CNTLOAD_Msk                                   (0x01 << INTIC2CNTLOAD_Pos)
#define INTIC2CNTLOAD_Pos                                   25
#define INTIC2CNTLOAD_PosL                                  25
#define INTIC2CNTLOAD_PosH                                  25

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=0 (group2-channel5/4)
*/
#define INTIC2CNTZERO_Msk                                   (0x01 << INTIC2CNTZERO_Pos)
#define INTIC2CNTZERO_Pos                                   24
#define INTIC2CNTZERO_PosL                                  24
#define INTIC2CNTZERO_PosH                                  24

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator B Down (group1-channel3)
*/
#define INTIC1CMPBD_Msk                                     (0x01 << INTIC1CMPBD_Pos)
#define INTIC1CMPBD_Pos                                     21
#define INTIC1CMPBD_PosL                                    21
#define INTIC1CMPBD_PosH                                    21

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator A Down (group1-channel2)
*/
#define INTIC1CMPAD_Msk                                     (0x01 << INTIC1CMPAD_Pos)
#define INTIC1CMPAD_Pos                                     20
#define INTIC1CMPAD_PosL                                    20
#define INTIC1CMPAD_PosH                                    20

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator B Up (group1-channel3)
*/
#define INTIC1CMPBU_Msk                                     (0x01 << INTIC1CMPBU_Pos)
#define INTIC1CMPBU_Pos                                     19
#define INTIC1CMPBU_PosL                                    19
#define INTIC1CMPBU_PosH                                    19

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator A Up (group1-channel2)
*/
#define INTIC1CMPAU_Msk                                     (0x01 << INTIC1CMPAU_Pos)
#define INTIC1CMPAU_Pos                                     18
#define INTIC1CMPAU_PosL                                    18
#define INTIC1CMPAU_PosH                                    18

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=Load (group1-channel3/2)
*/
#define INTIC1CNTLOAD_Msk                                   (0x01 << INTIC1CNTLOAD_Pos)
#define INTIC1CNTLOAD_Pos                                   17
#define INTIC1CNTLOAD_PosL                                  17
#define INTIC1CNTLOAD_PosH                                  17

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=0 (group1-channel3/2)
*/
#define INTIC1CNTZERO_Msk                                   (0x01 << INTIC1CNTZERO_Pos)
#define INTIC1CNTZERO_Pos                                   16
#define INTIC1CNTZERO_PosL                                  16
#define INTIC1CNTZERO_PosH                                  16

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator B Down (group0-channel1/0)
*/
#define INTIC0CMPBD_Msk                                     (0x01 << INTIC0CMPBD_Pos)
#define INTIC0CMPBD_Pos                                     13
#define INTIC0CMPBD_PosL                                    13
#define INTIC0CMPBD_PosH                                    13

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator A Down (group0-channel0)
*/
#define INTIC0CMPAD_Msk                                     (0x01 << INTIC0CMPAD_Pos)
#define INTIC0CMPAD_Pos                                     12
#define INTIC0CMPAD_PosL                                    12
#define INTIC0CMPAD_PosH                                    12

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator B Up (group0-channel1)
*/
#define INTIC0CMPBU_Msk                                     (0x01 << INTIC0CMPBU_Pos)
#define INTIC0CMPBU_Pos                                     11
#define INTIC0CMPBU_PosL                                    11
#define INTIC0CMPBU_PosH                                    11

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=comparator A Up (group0-channel0)
*/
#define INTIC0CMPAU_Msk                                     (0x01 << INTIC0CMPAU_Pos)
#define INTIC0CMPAU_Pos                                     10
#define INTIC0CMPAU_PosL                                    10
#define INTIC0CMPAU_PosH                                    10

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=Load (group0-channel1/0)
*/
#define INTIC0CNTLOAD_Msk                                   (0x01 << INTIC0CNTLOAD_Pos)
#define INTIC0CNTLOAD_Pos                                   9
#define INTIC0CNTLOAD_PosL                                  9
#define INTIC0CNTLOAD_PosH                                  9

/*!< 0x0024 PWMIC1
*Clear Interrupt for counter=0 (group0-channel1/0)
*/
#define INTIC0CNTZERO_Msk                                   (0x01 << INTIC0CNTZERO_Pos)
#define INTIC0CNTZERO_Pos                                   8
#define INTIC0CNTZERO_PosL                                  8
#define INTIC0CNTZERO_PosH                                  8


/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator B Down (group5-channel11)
*/
#define INTIC5CMPBD_Msk                                     (0x01 << INTIC5CMPBD_Pos)
#define INTIC5CMPBD_Pos                                     21
#define INTIC5CMPBD_PosL                                    21
#define INTIC5CMPBD_PosH                                    21

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator A Down (group5-channel10)
*/
#define INTIC5CMPAD_Msk                                     (0x01 << INTIC5CMPAD_Pos)
#define INTIC5CMPAD_Pos                                     20
#define INTIC5CMPAD_PosL                                    20
#define INTIC5CMPAD_PosH                                    20

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator B Up (group5-channel11)
*/
#define INTIC5CMPBU_Msk                                     (0x01 << INTIC5CMPBU_Pos)
#define INTIC5CMPBU_Pos                                     19
#define INTIC5CMPBU_PosL                                    19
#define INTIC5CMPBU_PosH                                    19

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator A Up (group5-channel10)
*/
#define INTIC5CMPAU_Msk                                     (0x01 << INTIC5CMPAU_Pos)
#define INTIC5CMPAU_Pos                                     18
#define INTIC5CMPAU_PosL                                    18
#define INTIC5CMPAU_PosH                                    18

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=Load (group5-channel11/10)
*/
#define INTIC5CNTLOAD_Msk                                   (0x01 << INTIC5CNTLOAD_Pos)
#define INTIC5CNTLOAD_Pos                                   17
#define INTIC5CNTLOAD_PosL                                  17
#define INTIC5CNTLOAD_PosH                                  17

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=0 (group5-channel11/10)
*/
#define INTIC5CNTZERO_Msk                                   (0x01 << INTIC5CNTZERO_Pos)
#define INTIC5CNTZERO_Pos                                   16
#define INTIC5CNTZERO_PosL                                  16
#define INTIC5CNTZERO_PosH                                  16

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator B Down (group4-channel9)
*/
#define INTIC4CMPBD_Msk                                     (0x01 << INTIC4CMPBD_Pos)
#define INTIC4CMPBD_Pos                                     13
#define INTIC4CMPBD_PosL                                    13
#define INTIC4CMPBD_PosH                                    13

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator A Down (group4-channel8)
*/
#define INTIC4CMPAD_Msk                                     (0x01 << INTIC4CMPAD_Pos)
#define INTIC4CMPAD_Pos                                     12
#define INTIC4CMPAD_PosL                                    12
#define INTIC4CMPAD_PosH                                    12

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator B Up (group4-channel9)
*/
#define INTIC4CMPBU_Msk                                     (0x01 << INTIC4CMPBU_Pos)
#define INTIC4CMPBU_Pos                                     11
#define INTIC4CMPBU_PosL                                    11
#define INTIC4CMPBU_PosH                                    11

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator A Up (group4-channel8)
*/
#define INTIC4CMPAU_Msk                                     (0x01 << INTIC4CMPAU_Pos)
#define INTIC4CMPAU_Pos                                     10
#define INTIC4CMPAU_PosL                                    10
#define INTIC4CMPAU_PosH                                    10

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=Load (group4-channel9/8)
*/
#define INTIC4CNTLOAD_Msk                                   (0x01 << INTIC4CNTLOAD_Pos)
#define INTIC4CNTLOAD_Pos                                   9
#define INTIC4CNTLOAD_PosL                                  9
#define INTIC4CNTLOAD_PosH                                  9

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=0 (group4-channel9/8)
*/
#define INTIC4CNTZERO_Msk                                   (0x01 << INTIC4CNTZERO_Pos)
#define INTIC4CNTZERO_Pos                                   8
#define INTIC4CNTZERO_PosL                                  8
#define INTIC4CNTZERO_PosH                                  8

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator B Down (group3-channel7)
*/
#define INTIC3CMPBD_Msk                                     (0x01 << INTIC3CMPBD_Pos)
#define INTIC3CMPBD_Pos                                     5
#define INTIC3CMPBD_PosL                                    5
#define INTIC3CMPBD_PosH                                    5

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator A Down (group3-channel6)
*/
#define INTIC3CMPAD_Msk                                     (0x01 << INTIC3CMPAD_Pos)
#define INTIC3CMPAD_Pos                                     4
#define INTIC3CMPAD_PosL                                    4
#define INTIC3CMPAD_PosH                                    4

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator B Up (group3-channel7)
*/
#define INTIC3CMPBU_Msk                                     (0x01 << INTIC3CMPBU_Pos)
#define INTIC3CMPBU_Pos                                     3
#define INTIC3CMPBU_PosL                                    3
#define INTIC3CMPBU_PosH                                    3

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=comparator A Up (group3-channel6)
*/
#define INTIC3CMPAU_Msk                                     (0x01 << INTIC3CMPAU_Pos)
#define INTIC3CMPAU_Pos                                     2
#define INTIC3CMPAU_PosL                                    2
#define INTIC3CMPAU_PosH                                    2

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=Load (group3-channel7/6)
*/
#define INTIC3CNTLOAD_Msk                                   (0x01 << INTIC3CNTLOAD_Pos)
#define INTIC3CNTLOAD_Pos                                   1
#define INTIC3CNTLOAD_PosL                                  1
#define INTIC3CNTLOAD_PosH                                  1

/*!< 0x0028 PWMIC2
*Clear Interrupt for counter=0 (group3-channel7/6)
*/
#define INTIC3CNTZERO_Msk                                   (0x01 << INTIC3CNTZERO_Pos)
#define INTIC3CNTZERO_Pos                                   0
#define INTIC3CNTZERO_PosL                                  0
#define INTIC3CNTZERO_PosH                                  0


/*!< 0x002c PWMIS1
*Interrupt for counter=comparator B Down (group2-channel5)
*/
#define INTIS2CMPBD_Msk                                     (0x01 << INTIS2CMPBD_Pos)
#define INTIS2CMPBD_Pos                                     29
#define INTIS2CMPBD_PosL                                    29
#define INTIS2CMPBD_PosH                                    29

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator A Down (group2-channel4)
*/
#define INTIS2CMPAD_Msk                                     (0x01 << INTIS2CMPAD_Pos)
#define INTIS2CMPAD_Pos                                     28
#define INTIS2CMPAD_PosL                                    28
#define INTIS2CMPAD_PosH                                    28

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator B Up (group2-channel5)
*/
#define INTIS2CMPBU_Msk                                     (0x01 << INTIS2CMPBU_Pos)
#define INTIS2CMPBU_Pos                                     27
#define INTIS2CMPBU_PosL                                    27
#define INTIS2CMPBU_PosH                                    27

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator A Up (group2-channel4)
*/
#define INTIS2CMPAU_Msk                                     (0x01 << INTIS2CMPAU_Pos)
#define INTIS2CMPAU_Pos                                     26
#define INTIS2CMPAU_PosL                                    26
#define INTIS2CMPAU_PosH                                    26

/*!< 0x002c PWMIS1
*Interrupt for counter=Load (group2-channel5/4)
*/
#define INTIS2CNTLOAD_Msk                                   (0x01 << INTIS2CNTLOAD_Pos)
#define INTIS2CNTLOAD_Pos                                   25
#define INTIS2CNTLOAD_PosL                                  25
#define INTIS2CNTLOAD_PosH                                  25

/*!< 0x002c PWMIS1
*Interrupt for counter=0 (group2-channel5/4)
*/
#define INTIS2CNTZERO_Msk                                   (0x01 << INTIS2CNTZERO_Pos)
#define INTIS2CNTZERO_Pos                                   24
#define INTIS2CNTZERO_PosL                                  24
#define INTIS2CNTZERO_PosH                                  24

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator B Down (group1-channel3)
*/
#define INTIS1CMPBD_Msk                                     (0x01 << INTIS1CMPBD_Pos)
#define INTIS1CMPBD_Pos                                     21
#define INTIS1CMPBD_PosL                                    21
#define INTIS1CMPBD_PosH                                    21

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator A Down (group1-channel2)
*/
#define INTIS1CMPAD_Msk                                     (0x01 << INTIS1CMPAD_Pos)
#define INTIS1CMPAD_Pos                                     20
#define INTIS1CMPAD_PosL                                    20
#define INTIS1CMPAD_PosH                                    20

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator B Up (group1-channel3)
*/
#define INTIS1CMPBU_Msk                                     (0x01 << INTIS1CMPBU_Pos)
#define INTIS1CMPBU_Pos                                     19
#define INTIS1CMPBU_PosL                                    19
#define INTIS1CMPBU_PosH                                    19

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator A Up (group1-channel2)
*/
#define INTIS1CMPAU_Msk                                     (0x01 << INTIS1CMPAU_Pos)
#define INTIS1CMPAU_Pos                                     18
#define INTIS1CMPAU_PosL                                    18
#define INTIS1CMPAU_PosH                                    18

/*!< 0x002c PWMIS1
*Interrupt for counter=Load (group1-channel3/2)
*/
#define INTIS1CNTLOAD_Msk                                   (0x01 << INTIS1CNTLOAD_Pos)
#define INTIS1CNTLOAD_Pos                                   17
#define INTIS1CNTLOAD_PosL                                  17
#define INTIS1CNTLOAD_PosH                                  17

/*!< 0x002c PWMIS1
*Interrupt for counter=0 (group1-channel3/2)
*/
#define INTIS1CNTZERO_Msk                                   (0x01 << INTIS1CNTZERO_Pos)
#define INTIS1CNTZERO_Pos                                   16
#define INTIS1CNTZERO_PosL                                  16
#define INTIS1CNTZERO_PosH                                  16

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator B Down (group0-channel1)
*/
#define INTIS0CMPBD_Msk                                     (0x01 << INTIS0CMPBD_Pos)
#define INTIS0CMPBD_Pos                                     13
#define INTIS0CMPBD_PosL                                    13
#define INTIS0CMPBD_PosH                                    13

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator A Down (group0-channel0)
*/
#define INTIS0CMPAD_Msk                                     (0x01 << INTIS0CMPAD_Pos)
#define INTIS0CMPAD_Pos                                     12
#define INTIS0CMPAD_PosL                                    12
#define INTIS0CMPAD_PosH                                    12

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator B Up (group0-channel1)
*/
#define INTIS0CMPBU_Msk                                     (0x01 << INTIS0CMPBU_Pos)
#define INTIS0CMPBU_Pos                                     11
#define INTIS0CMPBU_PosL                                    11
#define INTIS0CMPBU_PosH                                    11

/*!< 0x002c PWMIS1
*Interrupt for counter=comparator A Up (group0-channel0)
*/
#define INTIS0CMPAU_Msk                                     (0x01 << INTIS0CMPAU_Pos)
#define INTIS0CMPAU_Pos                                     10
#define INTIS0CMPAU_PosL                                    10
#define INTIS0CMPAU_PosH                                    10

/*!< 0x002c PWMIS1
*Interrupt for counter=Load (group0-channel1/0)
*/
#define INTIS0CNTLOAD_Msk                                   (0x01 << INTIS0CNTLOAD_Pos)
#define INTIS0CNTLOAD_Pos                                   9
#define INTIS0CNTLOAD_PosL                                  9
#define INTIS0CNTLOAD_PosH                                  9

/*!< 0x002c PWMIS1
*Interrupt for counter=0 (group0-channel1/0)
*/
#define INTIS0CNTZERO_Msk                                   (0x01 << INTIS0CNTZERO_Pos)
#define INTIS0CNTZERO_Pos                                   8
#define INTIS0CNTZERO_PosL                                  8
#define INTIS0CNTZERO_PosH                                  8


/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator B Down (group5-channel11)
*/
#define INTIS5CMPBD_Msk                                     (0x01 << INTIS5CMPBD_Pos)
#define INTIS5CMPBD_Pos                                     21
#define INTIS5CMPBD_PosL                                    21
#define INTIS5CMPBD_PosH                                    21

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator A Down (group5-channel10)
*/
#define INTIS5CMPAD_Msk                                     (0x01 << INTIS5CMPAD_Pos)
#define INTIS5CMPAD_Pos                                     20
#define INTIS5CMPAD_PosL                                    20
#define INTIS5CMPAD_PosH                                    20

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator B Up (group5-channel11)
*/
#define INTIS5CMPBU_Msk                                     (0x01 << INTIS5CMPBU_Pos)
#define INTIS5CMPBU_Pos                                     19
#define INTIS5CMPBU_PosL                                    19
#define INTIS5CMPBU_PosH                                    19

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator A Up (group5-channel10)
*/
#define INTIS5CMPAU_Msk                                     (0x01 << INTIS5CMPAU_Pos)
#define INTIS5CMPAU_Pos                                     18
#define INTIS5CMPAU_PosL                                    18
#define INTIS5CMPAU_PosH                                    18

/*!< 0x0030 PWMIS2
*Interrupt for counter=Load (group5-channel11/10)
*/
#define INTIS5CNTLOAD_Msk                                   (0x01 << INTIS5CNTLOAD_Pos)
#define INTIS5CNTLOAD_Pos                                   17
#define INTIS5CNTLOAD_PosL                                  17
#define INTIS5CNTLOAD_PosH                                  17

/*!< 0x0030 PWMIS2
*Interrupt for counter=0 (group5-channel11/10)
*/
#define INTIS5CNTZERO_Msk                                   (0x01 << INTIS5CNTZERO_Pos)
#define INTIS5CNTZERO_Pos                                   16
#define INTIS5CNTZERO_PosL                                  16
#define INTIS5CNTZERO_PosH                                  16

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator B Down (group4-channel9)
*/
#define INTIS4CMPBD_Msk                                     (0x01 << INTIS4CMPBD_Pos)
#define INTIS4CMPBD_Pos                                     13
#define INTIS4CMPBD_PosL                                    13
#define INTIS4CMPBD_PosH                                    13

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator A Down (group4-channel8)
*/
#define INTIS4CMPAD_Msk                                     (0x01 << INTIS4CMPAD_Pos)
#define INTIS4CMPAD_Pos                                     12
#define INTIS4CMPAD_PosL                                    12
#define INTIS4CMPAD_PosH                                    12

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator B Up (group4-channel9)
*/
#define INTIS4CMPBU_Msk                                     (0x01 << INTIS4CMPBU_Pos)
#define INTIS4CMPBU_Pos                                     11
#define INTIS4CMPBU_PosL                                    11
#define INTIS4CMPBU_PosH                                    11

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator A Up (group4-channel8)
*/
#define INTIS4CMPAU_Msk                                     (0x01 << INTIS4CMPAU_Pos)
#define INTIS4CMPAU_Pos                                     10
#define INTIS4CMPAU_PosL                                    10
#define INTIS4CMPAU_PosH                                    10

/*!< 0x0030 PWMIS2
*Interrupt for counter=Load (group4-channel9/8)
*/
#define INTIS4CNTLOAD_Msk                                   (0x01 << INTIS4CNTLOAD_Pos)
#define INTIS4CNTLOAD_Pos                                   9
#define INTIS4CNTLOAD_PosL                                  9
#define INTIS4CNTLOAD_PosH                                  9

/*!< 0x0030 PWMIS2
*Interrupt for counter=0 (group4-channel9/8)
*/
#define INTIS4CNTZERO_Msk                                   (0x01 << INTIS4CNTZERO_Pos)
#define INTIS4CNTZERO_Pos                                   8
#define INTIS4CNTZERO_PosL                                  8
#define INTIS4CNTZERO_PosH                                  8

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator B Down (group3-channel7)
*/
#define INTIS3CMPBD_Msk                                     (0x01 << INTIS3CMPBD_Pos)
#define INTIS3CMPBD_Pos                                     5
#define INTIS3CMPBD_PosL                                    5
#define INTIS3CMPBD_PosH                                    5

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator A Down (group3-channel6)
*/
#define INTIS3CMPAD_Msk                                     (0x01 << INTIS3CMPAD_Pos)
#define INTIS3CMPAD_Pos                                     4
#define INTIS3CMPAD_PosL                                    4
#define INTIS3CMPAD_PosH                                    4

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator B Up (group3-channel7)
*/
#define INTIS3CMPBU_Msk                                     (0x01 << INTIS3CMPBU_Pos)
#define INTIS3CMPBU_Pos                                     3
#define INTIS3CMPBU_PosL                                    3
#define INTIS3CMPBU_PosH                                    3

/*!< 0x0030 PWMIS2
*Interrupt for counter=comparator A Up (group3-channel6)
*/
#define INTIS3CMPAU_Msk                                     (0x01 << INTIS3CMPAU_Pos)
#define INTIS3CMPAU_Pos                                     2
#define INTIS3CMPAU_PosL                                    2
#define INTIS3CMPAU_PosH                                    2

/*!< 0x0030 PWMIS2
*Interrupt for counter=Load (group3-channel7/6)
*/
#define INTIS3CNTLOAD_Msk                                   (0x01 << INTIS3CNTLOAD_Pos)
#define INTIS3CNTLOAD_Pos                                   1
#define INTIS3CNTLOAD_PosL                                  1
#define INTIS3CNTLOAD_PosH                                  1

/*!< 0x0030 PWMIS2
*Interrupt for counter=0 (group3-channel7/6)
*/
#define INTIS3CNTZERO_Msk                                   (0x01 << INTIS3CNTZERO_Pos)
#define INTIS3CNTZERO_Pos                                   0
#define INTIS3CNTZERO_PosL                                  0
#define INTIS3CNTZERO_PosH                                  0

/*!< 0x0034 PWMCTL
*Synchronize mode (group3-channel7/6)
*/
#define SYNC3MODE_Msk                                       (0x03 << SYNC3MODE_Pos)
#define SYNC3MODE_Pos                                       12
#define SYNC3MODE_PosL                                      12
#define SYNC3MODE_PosH                                      13

/*!< 0x0034 PWMCTL
*Synchronize mode (group2-channel5/4)
*/
#define SYNC2MODE_Msk                                       (0x03 << SYNC2MODE_Pos)
#define SYNC2MODE_Pos                                       10
#define SYNC2MODE_PosL                                      10
#define SYNC2MODE_PosH                                      11

/*!< 0x0034 PWMCTL
*Synchronize mode (group1-channel3/2)
*/
#define SYNC1MODE_Msk                                       (0x03 << SYNC1MODE_Pos)
#define SYNC1MODE_Pos                                       8
#define SYNC1MODE_PosL                                      8
#define SYNC1MODE_PosH                                      9

/*!< 0x0034 PWMCTL
*Synchronize mode (group0-channel1/0)
*/
#define SYNC0MODE_Msk                                       (0x03 << SYNC0MODE_Pos)
#define SYNC0MODE_Pos                                       6
#define SYNC0MODE_PosL                                      6
#define SYNC0MODE_PosH                                      7

#define SYNCMODE_Msk                                        (0x03)
#define SYNCMODE_Pos                                        6

/*!< 0x0034 PWMCTL
*Counter mode (group3-channel7/6)
*/
#define PWM3MODE_Msk                                        (0x01 << PWM3MODE_Pos)
#define PWM3MODE_Pos                                        3
#define PWM3MODE_PosL                                       3
#define PWM3MODE_PosH                                       3

/*!< 0x0034 PWMCTL
*Counter mode (group2-channel5/4)
*/
#define PWM2MODE_Msk                                        (0x01 << PWM2MODE_Pos)
#define PWM2MODE_Pos                                        2
#define PWM2MODE_PosL                                       2
#define PWM2MODE_PosH                                       2

/*!< 0x0034 PWMCTL
*Counter mode (group1-channel3/2)
*/
#define PWM1MODE_Msk                                        (0x01 << PWM1MODE_Pos)
#define PWM1MODE_Pos                                        1
#define PWM1MODE_PosL                                       1
#define PWM1MODE_PosH                                       1

/*!< 0x0034 PWMCTL
*Counter mode (group0-channel1/0)
*/
#define PWM0MODE_Msk                                        (0x01 << PWM0MODE_Pos)
#define PWM0MODE_Pos                                        0
#define PWM0MODE_PosL                                       0
#define PWM0MODE_PosH                                       0

#define PWMMODE_Msk                                        (0x01)
#define PWMMODE_Pos                                        0

/*!< 0x0038 PWM01LOAD
*Counter load 1 value：group1-channel3/2
*/
#define LOAD1_Msk                                           (0xFFFF << LOAD1_Pos)
#define LOAD1_Pos                                           16
#define LOAD1_PosL                                          16
#define LOAD1_PosH                                          31

/*!< 0x0038 PWM01LOAD
*Counter load 0 value：group0-channel1/0
*/
#define LOAD0_Msk                                           (0xFFFF << LOAD0_Pos)
#define LOAD0_Pos                                           0
#define LOAD0_PosL                                          0
#define LOAD0_PosH                                          15


/*!< 0x003c PWM23LOAD
*Counter load 3 value：group3-channel7/6
*/
#define LOAD3_Msk                                           (0xFFFF << LOAD3_Pos)
#define LOAD3_Pos                                           16
#define LOAD3_PosL                                          16
#define LOAD3_PosH                                          31

/*!< 0x003c PWM23LOAD
*Counter load 2 value：group2-channel5/4
*/
#define LOAD2_Msk                                           (0xFFFF << LOAD2_Pos)
#define LOAD2_Pos                                           0
#define LOAD2_PosL                                          0
#define LOAD2_PosH                                          15


/*!< 0x0044 PWM01COUNT
*Counter 1 value：group1-channel3/2
*/
#define COUNT1_Msk                                          (0xFFFF << COUNT1_Pos)
#define COUNT1_Pos                                          16
#define COUNT1_PosL                                         16
#define COUNT1_PosH                                         31

/*!< 0x0044 PWM01COUNT
*Counter 0 value：group0-channel1/0
*/
#define COUNT0_Msk                                          (0xFFFF << COUNT0_Pos)
#define COUNT0_Pos                                          0
#define COUNT0_PosL                                         0
#define COUNT0_PosH                                         15


/*!< 0x0048 PWM23COUNT
*Counter 3 value：group3-channel7/6
*/
#define COUNT3_Msk                                          (0xFFFF << COUNT3_Pos)
#define COUNT3_Pos                                          16
#define COUNT3_PosL                                         16
#define COUNT3_PosH                                         31

/*!< 0x0048 PWM23COUNT
*Counter 2 value：group2-channel5/4
*/
#define COUNT2_Msk                                          (0xFFFF << COUNT2_Pos)
#define COUNT2_Pos                                          0
#define COUNT2_PosL                                         0
#define COUNT2_PosH                                         15


/*!< 0x0050 PWM0CMP
*Comparator B value：group0-channel1
*/
#define COMP0B_Msk                                          (0xFFFF << COMP0B_Pos)
#define COMP0B_Pos                                          16
#define COMP0B_PosL                                         16
#define COMP0B_PosH                                         31

/*!< 0x0050 PWM0CMP
*Comparator A value：group0-channel0
*/
#define COMP0A_Msk                                          (0xFFFF << COMP0A_Pos)
#define COMP0A_Pos                                          0
#define COMP0A_PosL                                         0
#define COMP0A_PosH                                         15


/*!< 0x0054 PWM1CMP
*Comparator B value：group1-channel3
*/
#define COMP1B_Msk                                          (0xFFFF << COMP1B_Pos)
#define COMP1B_Pos                                          16
#define COMP1B_PosL                                         16
#define COMP1B_PosH                                         31

/*!< 0x0054 PWM1CMP
*Comparator A value：group1-channel2
*/
#define COMP1A_Msk                                          (0xFFFF << COMP1A_Pos)
#define COMP1A_Pos                                          0
#define COMP1A_PosL                                         0
#define COMP1A_PosH                                         15


/*!< 0x0058 PWM2CMP
*Comparator B value：group2-channel5
*/
#define COMP2B_Msk                                          (0xFFFF << COMP2B_Pos)
#define COMP2B_Pos                                          16
#define COMP2B_PosL                                         16
#define COMP2B_PosH                                         31

/*!< 0x0058 PWM2CMP
*Comparator A value：group2-channel4
*/
#define COMP2A_Msk                                          (0xFFFF << COMP2A_Pos)
#define COMP2A_Pos                                          0
#define COMP2A_PosL                                         0
#define COMP2A_PosH                                         15


/*!< 0x005c PWM3CMP
*Comparator B value：group2-channel7
*/
#define COMP3B_Msk                                          (0xFFFF << COMP3B_Pos)
#define COMP3B_Pos                                          16
#define COMP3B_PosL                                         16
#define COMP3B_PosH                                         31

/*!< 0x005c PWM3CMP
*Comparator A value：group3-channel6
*/
#define COMP3A_Msk                                          (0xFFFF << COMP3A_Pos)
#define COMP3A_Pos                                          0
#define COMP3A_PosL                                         0
#define COMP3A_PosH                                         15


/*!< 0x0068 PWM01DB
*Dead-band m generator (group) enable：group1-channel3/2
*/
#define DB1EN_Msk                                           (0x01 << DB1EN_Pos)
#define DB1EN_Pos                                           25
#define DB1EN_PosL                                          25
#define DB1EN_PosH                                          25

/*!< 0x0068 PWM01DB
*Dead-band 0 generator (group) enable：group0-channel1/0
*/
#define DB0EN_Msk                                           (0x01 << DB0EN_Pos)
#define DB0EN_Pos                                           24
#define DB0EN_PosL                                          24
#define DB0EN_PosH                                          24

/*!< 0x0068 PWM01DB
*Dead-band delay：group1-channel3/2 The number of clock tick to delay
*/
#define DELAY1_Msk                                          (0xFF << DELAY1_Pos)
#define DELAY1_Pos                                          12
#define DELAY1_PosL                                         12
#define DELAY1_PosH                                         23

/*!< 0x0068 PWM01DB
*Dead-band delay：group0-channel1/0 The number of clock tick to delay
*/
#define DELAY0_Msk                                          (0xFF << DELAY0_Pos)
#define DELAY0_Pos                                          0
#define DELAY0_PosL                                         0
#define DELAY0_PosH                                         11


/*!< 0x006c PWM23DB
*Dead-band 3 generator (group) enable：group3-channel7/6
*/
#define DB3EN_Msk                                           (0x01 << DB3EN_Pos)
#define DB3EN_Pos                                           25
#define DB3EN_PosL                                          25
#define DB3EN_PosH                                          25

/*!< 0x006c PWM23DB
*Dead-band 2 generator (group) enable：group2-channel5/4
*/
#define DB2EN_Msk                                           (0x01 << DB2EN_Pos)
#define DB2EN_Pos                                           24
#define DB2EN_PosL                                          24
#define DB2EN_PosH                                          24

/*!< 0x006c PWM23DB
*Dead-band delay：group3-channel7/6 The number of clock tick to delay
*/
#define DELAY3_Msk                                          (0xFF << DELAY3_Pos)
#define DELAY3_Pos                                          12
#define DELAY3_PosL                                         12
#define DELAY3_PosH                                         23

/*!< 0x006c PWM23DB
*Dead-band delay：group2-channel5/4 The number of clock tick to delay
*/
#define DELAY2_Msk                                          (0xFF << DELAY2_Pos)
#define DELAY2_Pos                                          0
#define DELAY2_PosL                                         0
#define DELAY2_PosH                                         11

/*!< 0x0074 CAPCTL
*Capture3 edge event mode (group3-channel6)
*/
#define CAP3EVENT_Msk                                       (0x03 << CAP3EVENT_Pos)
#define CAP3EVENT_Pos                                       12
#define CAP3EVENT_PosL                                      12
#define CAP3EVENT_PosH                                      13

/*!< 0x0074 CAPCTL
*Capture2 edge event mode (group2-channel4)
*/
#define CAP2EVENT_Msk                                       (0x03 << CAP2EVENT_Pos)
#define CAP2EVENT_Pos                                       10
#define CAP2EVENT_PosL                                      10
#define CAP2EVENT_PosH                                      11

/*!< 0x0074 CAPCTL
*Capture1 edge event mode (group1-channel2)
*/
#define CAP1EVENT_Msk                                       (0x03 << CAP1EVENT_Pos)
#define CAP1EVENT_Pos                                       8
#define CAP1EVENT_PosL                                      8
#define CAP1EVENT_PosH                                      9

/*!< 0x0074 CAPCTL
*Capture0 edge event mode (group0-channel0)
*/
#define CAP0EVENT_Msk                                       (0x03 << CAP0EVENT_Pos)
#define CAP0EVENT_Pos                                       6
#define CAP0EVENT_PosL                                      6
#define CAP0EVENT_PosH                                      7

#define CAPEVENT_Msk                                        (0x03)
#define CAPEVENT_Pos                                        6

/*!< 0x0074 CAPCTL
*Capture3 mode (group3-channel6)
*/
#define CAP3MODE_Msk                                        (0x01 << CAP3MODE_Pos)
#define CAP3MODE_Pos                                        3
#define CAP3MODE_PosL                                       3
#define CAP3MODE_PosH                                       3

/*!< 0x0074 CAPCTL
*Capture2 mode (group2-channel4)
*/
#define CAP2MODE_Msk                                        (0x01 << CAP2MODE_Pos)
#define CAP2MODE_Pos                                        2
#define CAP2MODE_PosL                                       2
#define CAP2MODE_PosH                                       2

/*!< 0x0074 CAPCTL
*Capture1 mode (group1-channel2)
*/
#define CAP1MODE_Msk                                        (0x01 << CAP1MODE_Pos)
#define CAP1MODE_Pos                                        1
#define CAP1MODE_PosL                                       1
#define CAP1MODE_PosH                                       1

/*!< 0x0074 CAPCTL
*Capture0 mode (group0-channel0)
*/
#define CAP0MODE_Msk                                        (0x01 << CAP0MODE_Pos)
#define CAP0MODE_Pos                                        0
#define CAP0MODE_PosL                                       0
#define CAP0MODE_PosH                                       0

#define CAPMODE_Msk                                         (0x01)
#define CAPMODE_Pos                                         0

/*!< 0x0078 CAPINTEN
*Capture3 edge time interrupt enable
*/
#define CAP3TIMIE_Msk                                       (0x01 << CAP3TIMIE_Pos)
#define CAP3TIMIE_Pos                                       9
#define CAP3TIMIE_PosL                                      9
#define CAP3TIMIE_PosH                                      9

/*!< 0x0078 CAPINTEN
*Capture2 edge time interrupt enable
*/
#define CAP2TIMIE_Msk                                       (0x01 << CAP2TIMIE_Pos)
#define CAP2TIMIE_Pos                                       8
#define CAP2TIMIE_PosL                                      8
#define CAP2TIMIE_PosH                                      8

/*!< 0x0078 CAPINTEN
*Capture1 edge time interrupt enable
*/
#define CAP1TIMIE_Msk                                       (0x01 << CAP1TIMIE_Pos)
#define CAP1TIMIE_Pos                                       7
#define CAP1TIMIE_PosL                                      7
#define CAP1TIMIE_PosH                                      7

/*!< 0x0078 CAPINTEN
*Capture0 edge time interrupt enable
*/
#define CAP0TIMIE_Msk                                       (0x01 << CAP0TIMIE_Pos)
#define CAP0TIMIE_Pos                                       6
#define CAP0TIMIE_PosL                                      6
#define CAP0TIMIE_PosH                                      6

/*!< 0x0078 CAPINTEN
*Capture3 edge count interrupt enable
*/
#define CAP3CNTIE_Msk                                       (0x01 << CAP3CNTIE_Pos)
#define CAP3CNTIE_Pos                                       3
#define CAP3CNTIE_PosL                                      3
#define CAP3CNTIE_PosH                                      3

/*!< 0x0078 CAPINTEN
*Capture2 edge count interrupt enable
*/
#define CAP2CNTIE_Msk                                       (0x01 << CAP2CNTIE_Pos)
#define CAP2CNTIE_Pos                                       2
#define CAP2CNTIE_PosL                                      2
#define CAP2CNTIE_PosH                                      2

/*!< 0x0078 CAPINTEN
*Capture1 edge count interrupt enable
*/
#define CAP1CNTIE_Msk                                       (0x01 << CAP1CNTIE_Pos)
#define CAP1CNTIE_Pos                                       1
#define CAP1CNTIE_PosL                                      1
#define CAP1CNTIE_PosH                                      1

/*!< 0x0078 CAPINTEN
*Capture0 edge count interrupt enable
*/
#define CAP0CNTIE_Msk                                       (0x01 << CAP0CNTIE_Pos)
#define CAP0CNTIE_Pos                                       0
#define CAP0CNTIE_PosL                                      0
#define CAP0CNTIE_PosH                                      0


/*!< 0x007c CAPRIS
*Capture5 edge time raw interrupt occurs
*/
#define CAP5TIMRIS_Msk                                      (0x01 << CAP5TIMRIS_Pos)
#define CAP5TIMRIS_Pos                                      11
#define CAP5TIMRIS_PosL                                     11
#define CAP5TIMRIS_PosH                                     11

/*!< 0x007c CAPRIS
*Capture4 edge time raw interrupt occurs
*/
#define CAP4TIMRIS_Msk                                      (0x01 << CAP4TIMRIS_Pos)
#define CAP4TIMRIS_Pos                                      10
#define CAP4TIMRIS_PosL                                     10
#define CAP4TIMRIS_PosH                                     10

/*!< 0x007c CAPRIS
*Capture3 edge time raw interrupt occurs
*/
#define CAP3TIMRIS_Msk                                      (0x01 << CAP3TIMRIS_Pos)
#define CAP3TIMRIS_Pos                                      9
#define CAP3TIMRIS_PosL                                     9
#define CAP3TIMRIS_PosH                                     9

/*!< 0x007c CAPRIS
*Capture2 edge time raw interrupt occurs
*/
#define CAP2TIMRIS_Msk                                      (0x01 << CAP2TIMRIS_Pos)
#define CAP2TIMRIS_Pos                                      8
#define CAP2TIMRIS_PosL                                     8
#define CAP2TIMRIS_PosH                                     8

/*!< 0x007c CAPRIS
*Capture1 edge time raw interrupt occurs
*/
#define CAP1TIMRIS_Msk                                      (0x01 << CAP1TIMRIS_Pos)
#define CAP1TIMRIS_Pos                                      7
#define CAP1TIMRIS_PosL                                     7
#define CAP1TIMRIS_PosH                                     7

/*!< 0x007c CAPRIS
*Capture0 edge time raw interrupt occurs
*/
#define CAP0TIMRIS_Msk                                      (0x01 << CAP0TIMRIS_Pos)
#define CAP0TIMRIS_Pos                                      6
#define CAP0TIMRIS_PosL                                     6
#define CAP0TIMRIS_PosH                                     6

/*!< 0x007c CAPRIS
*Capture5 edge count raw interrupt occurs
*/
#define CAP5CNTRIS_Msk                                      (0x01 << CAP5CNTRIS_Pos)
#define CAP5CNTRIS_Pos                                      5
#define CAP5CNTRIS_PosL                                     5
#define CAP5CNTRIS_PosH                                     5

/*!< 0x007c CAPRIS
*Capture4 edge count raw interrupt occurs
*/
#define CAP4CNTRIS_Msk                                      (0x01 << CAP4CNTRIS_Pos)
#define CAP4CNTRIS_Pos                                      4
#define CAP4CNTRIS_PosL                                     4
#define CAP4CNTRIS_PosH                                     4

/*!< 0x007c CAPRIS
*Capture3 edge count raw interrupt occurs
*/
#define CAP3CNTRIS_Msk                                      (0x01 << CAP3CNTRIS_Pos)
#define CAP3CNTRIS_Pos                                      3
#define CAP3CNTRIS_PosL                                     3
#define CAP3CNTRIS_PosH                                     3

/*!< 0x007c CAPRIS
*Capture2 edge count raw interrupt occurs
*/
#define CAP2CNTRIS_Msk                                      (0x01 << CAP2CNTRIS_Pos)
#define CAP2CNTRIS_Pos                                      2
#define CAP2CNTRIS_PosL                                     2
#define CAP2CNTRIS_PosH                                     2

/*!< 0x007c CAPRIS
*Capture1 edge count raw interrupt occurs
*/
#define CAP1CNTRIS_Msk                                      (0x01 << CAP1CNTRIS_Pos)
#define CAP1CNTRIS_Pos                                      1
#define CAP1CNTRIS_PosL                                     1
#define CAP1CNTRIS_PosH                                     1

/*!< 0x007c CAPRIS
*Capture0 edge count raw interrupt occurs
*/
#define CAP0CNTRIS_Msk                                      (0x01 << CAP0CNTRIS_Pos)
#define CAP0CNTRIS_Pos                                      0
#define CAP0CNTRIS_PosL                                     0
#define CAP0CNTRIS_PosH                                     0


/*!< 0x0080 CAPIC
*Clear Capture5 edge time interrupt
*/
#define CAP5TIMIC_Msk                                       (0x01 << CAP5TIMIC_Pos)
#define CAP5TIMIC_Pos                                       11
#define CAP5TIMIC_PosL                                      11
#define CAP5TIMIC_PosH                                      11

/*!< 0x0080 CAPIC
*Clear Capture4 edge time interrupt
*/
#define CAP4TIMIC_Msk                                       (0x01 << CAP4TIMIC_Pos)
#define CAP4TIMIC_Pos                                       10
#define CAP4TIMIC_PosL                                      10
#define CAP4TIMIC_PosH                                      10

/*!< 0x0080 CAPIC
*Clear Capture3 edge time interrupt
*/
#define CAP3TIMIC_Msk                                       (0x01 << CAP3TIMIC_Pos)
#define CAP3TIMIC_Pos                                       9
#define CAP3TIMIC_PosL                                      9
#define CAP3TIMIC_PosH                                      9

/*!< 0x0080 CAPIC
*Clear Capture2 edge time interrupt
*/
#define CAP2TIMIC_Msk                                       (0x01 << CAP2TIMIC_Pos)
#define CAP2TIMIC_Pos                                       8
#define CAP2TIMIC_PosL                                      8
#define CAP2TIMIC_PosH                                      8

/*!< 0x0080 CAPIC
*Clear Capture1 edge time interrupt
*/
#define CAP1TIMIC_Msk                                       (0x01 << CAP1TIMIC_Pos)
#define CAP1TIMIC_Pos                                       7
#define CAP1TIMIC_PosL                                      7
#define CAP1TIMIC_PosH                                      7

/*!< 0x0080 CAPIC
*Clear Capture0 edge time interrupt
*/
#define CAP0TIMIC_Msk                                       (0x01 << CAP0TIMIC_Pos)
#define CAP0TIMIC_Pos                                       6
#define CAP0TIMIC_PosL                                      6
#define CAP0TIMIC_PosH                                      6

/*!< 0x0080 CAPIC
*Clear Capture5 edge count interrupt
*/
#define CAP5CNTIC_Msk                                       (0x01 << CAP5CNTIC_Pos)
#define CAP5CNTIC_Pos                                       5
#define CAP5CNTIC_PosL                                      5
#define CAP5CNTIC_PosH                                      5

/*!< 0x0080 CAPIC
*Clear Capture4 edge count interrupt
*/
#define CAP4CNTIC_Msk                                       (0x01 << CAP4CNTIC_Pos)
#define CAP4CNTIC_Pos                                       4
#define CAP4CNTIC_PosL                                      4
#define CAP4CNTIC_PosH                                      4

/*!< 0x0080 CAPIC
*Clear Capture3 edge count interrupt
*/
#define CAP3CNTIC_Msk                                       (0x01 << CAP3CNTIC_Pos)
#define CAP3CNTIC_Pos                                       3
#define CAP3CNTIC_PosL                                      3
#define CAP3CNTIC_PosH                                      3

/*!< 0x0080 CAPIC
*Clear Capture2 edge count interrupt
*/
#define CAP2CNTIC_Msk                                       (0x01 << CAP2CNTIC_Pos)
#define CAP2CNTIC_Pos                                       2
#define CAP2CNTIC_PosL                                      2
#define CAP2CNTIC_PosH                                      2

/*!< 0x0080 CAPIC
*Clear Capture1 edge count interrupt
*/
#define CAP1CNTIC_Msk                                       (0x01 << CAP1CNTIC_Pos)
#define CAP1CNTIC_Pos                                       1
#define CAP1CNTIC_PosL                                      1
#define CAP1CNTIC_PosH                                      1

/*!< 0x0080 CAPIC
*Clear Capture0 edge count interrupt
*/
#define CAP0CNTIC_Msk                                       (0x01 << CAP0CNTIC_Pos)
#define CAP0CNTIC_Pos                                       0
#define CAP0CNTIC_PosL                                      0
#define CAP0CNTIC_PosH                                      0


/*!< 0x0084 CAPIS
*Capture5 edge time interrupt occurs
*/
#define CAP5TIMIS_Msk                                       (0x01 << CAP5TIMIS_Pos)
#define CAP5TIMIS_Pos                                       11
#define CAP5TIMIS_PosL                                      11
#define CAP5TIMIS_PosH                                      11

/*!< 0x0084 CAPIS
*Capture4 edge time interrupt occurs
*/
#define CAP4TIMIS_Msk                                       (0x01 << CAP4TIMIS_Pos)
#define CAP4TIMIS_Pos                                       10
#define CAP4TIMIS_PosL                                      10
#define CAP4TIMIS_PosH                                      10

/*!< 0x0084 CAPIS
*Capture3 edge time interrupt occurs
*/
#define CAP3TIMIS_Msk                                       (0x01 << CAP3TIMIS_Pos)
#define CAP3TIMIS_Pos                                       9
#define CAP3TIMIS_PosL                                      9
#define CAP3TIMIS_PosH                                      9

/*!< 0x0084 CAPIS
*Capture2 edge time interrupt occurs
*/
#define CAP2TIMIS_Msk                                       (0x01 << CAP2TIMIS_Pos)
#define CAP2TIMIS_Pos                                       8
#define CAP2TIMIS_PosL                                      8
#define CAP2TIMIS_PosH                                      8

/*!< 0x0084 CAPIS
*Capture1 edge time interrupt occurs
*/
#define CAP1TIMIS_Msk                                       (0x01 << CAP1TIMIS_Pos)
#define CAP1TIMIS_Pos                                       7
#define CAP1TIMIS_PosL                                      7
#define CAP1TIMIS_PosH                                      7

/*!< 0x0084 CAPIS
*Capture0 edge time interrupt occurs
*/
#define CAP0TIMIS_Msk                                       (0x01 << CAP0TIMIS_Pos)
#define CAP0TIMIS_Pos                                       6
#define CAP0TIMIS_PosL                                      6
#define CAP0TIMIS_PosH                                      6

/*!< 0x0084 CAPIS
*Capture5 edge count interrupt occurs
*/
#define CAP5CNTIS_Msk                                       (0x01 << CAP5CNTIS_Pos)
#define CAP5CNTIS_Pos                                       5
#define CAP5CNTIS_PosL                                      5
#define CAP5CNTIS_PosH                                      5

/*!< 0x0084 CAPIS
*Capture4 edge count interrupt occurs
*/
#define CAP4CNTIS_Msk                                       (0x01 << CAP4CNTIS_Pos)
#define CAP4CNTIS_Pos                                       4
#define CAP4CNTIS_PosL                                      4
#define CAP4CNTIS_PosH                                      4

/*!< 0x0084 CAPIS
*Capture3 edge count interrupt occurs
*/
#define CAP3CNTIS_Msk                                       (0x01 << CAP3CNTIS_Pos)
#define CAP3CNTIS_Pos                                       3
#define CAP3CNTIS_PosL                                      3
#define CAP3CNTIS_PosH                                      3

/*!< 0x0084 CAPIS
*Capture2 edge count interrupt occurs
*/
#define CAP2CNTIS_Msk                                       (0x01 << CAP2CNTIS_Pos)
#define CAP2CNTIS_Pos                                       2
#define CAP2CNTIS_PosL                                      2
#define CAP2CNTIS_PosH                                      2

/*!< 0x0084 CAPIS
*Capture1 edge count interrupt occurs
*/
#define CAP1CNTIS_Msk                                       (0x01 << CAP1CNTIS_Pos)
#define CAP1CNTIS_Pos                                       1
#define CAP1CNTIS_PosL                                      1
#define CAP1CNTIS_PosH                                      1

/*!< 0x0084 CAPIS
*Capture0 edge count interrupt occurs
*/
#define CAP0CNTIS_Msk                                       (0x01 << CAP0CNTIS_Pos)
#define CAP0CNTIS_Pos                                       0
#define CAP0CNTIS_PosL                                      0
#define CAP0CNTIS_PosH                                      0


/*!< 0x0088 CAP01T
*Input capture 1 counter value：group1-channel2
*/
#define CAP01T1_Msk                                         (0xFFFF << CAP01T1_Pos)
#define CAP01T1_Pos                                         16
#define CAP01T1_PosL                                        16
#define CAP01T1_PosH                                        31

/*!< 0x0088 CAP01T
*Input capture 0 counter value：group1-channel0
*/
#define CAP01T0_Msk                                         (0xFFFF << CAP01T0_Pos)
#define CAP01T0_Pos                                         0
#define CAP01T0_PosL                                        0
#define CAP01T0_PosH                                        15


/*!< 0x008c CAP23T
*Input capture 3 counter value：group3-channel6
*/
#define CAP23T3_Msk                                         (0xFFFF << CAP23T3_Pos)
#define CAP23T3_Pos                                         16
#define CAP23T3_PosL                                        16
#define CAP23T3_PosH                                        31

/*!< 0x008c CAP23T
*Input capture 2 counter value：group3-channel4
*/
#define CAP23T2_Msk                                         (0xFFFF << CAP23T2_Pos)
#define CAP23T2_Pos                                         0
#define CAP23T2_PosL                                        0
#define CAP23T2_PosH                                        15


/*!< 0x0094 CAP01MATCH
*Input capture 1 match value：group1-channel2
*/
#define CAP01MATCH1_Msk                                     (0xFFFF << CAP01MATCH1_Pos)
#define CAP01MATCH1_Pos                                     16
#define CAP01MATCH1_PosL                                    16
#define CAP01MATCH1_PosH                                    31

/*!< 0x0094 CAP01MATCH
*Input capture 0 match value：group0-channel0
*/
#define CAP01MATCH0_Msk                                     (0xFFFF << CAP01MATCH0_Pos)
#define CAP01MATCH0_Pos                                     0
#define CAP01MATCH0_PosL                                    0
#define CAP01MATCH0_PosH                                    15


/*!< 0x0098 CAP23MATCH
*Input capture 3 match value：group3-channel6
*/
#define CAP23MATCH3_Msk                                     (0xFFFF << CAP23MATCH3_Pos)
#define CAP23MATCH3_Pos                                     16
#define CAP23MATCH3_PosL                                    16
#define CAP23MATCH3_PosH                                    31

/*!< 0x0098 CAP23MATCH
*Input capture 2 match value：group2-channel4
*/
#define CAP23MATCH2_Msk                                     (0xFFFF << CAP23MATCH2_Pos)
#define CAP23MATCH2_Pos                                     0
#define CAP23MATCH2_PosL                                    0
#define CAP23MATCH2_PosH                                    15


/*!< 0x00a0 TIMINTEN
*A interrupt enable when counter match the CMPnmMATCH value. (group3)
*/
#define TIM3IE_Msk                                          (0x01 << TIM3IE_Pos)
#define TIM3IE_Pos                                          3
#define TIM3IE_PosL                                         3
#define TIM3IE_PosH                                         3

/*!< 0x00a0 TIMINTEN
*A interrupt enable when counter match the CMPnmMATCH value. (group2)
*/
#define TIM2IE_Msk                                          (0x01 << TIM2IE_Pos)
#define TIM2IE_Pos                                          2
#define TIM2IE_PosL                                         2
#define TIM2IE_PosH                                         2

/*!< 0x00a0 TIMINTEN
*A interrupt enable when counter match the CMPnmMATCH value. (group1)
*/
#define TIM1IE_Msk                                          (0x01 << TIM1IE_Pos)
#define TIM1IE_Pos                                          1
#define TIM1IE_PosL                                         1
#define TIM1IE_PosH                                         1

/*!< 0x00a0 TIMINTEN
*A interrupt enable when counter match the CMPnmMATCH value. (group0)
*/
#define TIM0IE_Msk                                          (0x01 << TIM0IE_Pos)
#define TIM0IE_Pos                                          0
#define TIM0IE_PosL                                         0
#define TIM0IE_PosH                                         0


/*!< 0x00a4 TIMRIS
*Timer3 raw interrupt occurs. (group3)
*/
#define TIM3RIS_Msk                                         (0x01 << TIM3RIS_Pos)
#define TIM3RIS_Pos                                         3
#define TIM3RIS_PosL                                        3
#define TIM3RIS_PosH                                        3

/*!< 0x00a4 TIMRIS
*Timer2 raw interrupt occurs. (group2)
*/
#define TIM2RIS_Msk                                         (0x01 << TIM2RIS_Pos)
#define TIM2RIS_Pos                                         2
#define TIM2RIS_PosL                                        2
#define TIM2RIS_PosH                                        2

/*!< 0x00a4 TIMRIS
*Timer1 raw interrupt occurs. (group1)
*/
#define TIM1RIS_Msk                                         (0x01 << TIM1RIS_Pos)
#define TIM1RIS_Pos                                         1
#define TIM1RIS_PosL                                        1
#define TIM1RIS_PosH                                        1

/*!< 0x00a4 TIMRIS
*Timer0 raw interrupt occurs. (group0)
*/
#define TIM0RIS_Msk                                         (0x01 << TIM0RIS_Pos)
#define TIM0RIS_Pos                                         0
#define TIM0RIS_PosL                                        0
#define TIM0RIS_PosH                                        0


/*!< 0x00a8 TIMIC
*Timer3 interrupt. (group3)
*/
#define TIM3IC_Msk                                          (0x01 << TIM3IC_Pos)
#define TIM3IC_Pos                                          3
#define TIM3IC_PosL                                         3
#define TIM3IC_PosH                                         3

/*!< 0x00a8 TIMIC
*Timer2 interrupt. (group2)
*/
#define TIM2IC_Msk                                          (0x01 << TIM2IC_Pos)
#define TIM2IC_Pos                                          2
#define TIM2IC_PosL                                         2
#define TIM2IC_PosH                                         2

/*!< 0x00a8 TIMIC
*Timer1 interrupt. (group1)
*/
#define TIM1IC_Msk                                          (0x01 << TIM1IC_Pos)
#define TIM1IC_Pos                                          1
#define TIM1IC_PosL                                         1
#define TIM1IC_PosH                                         1

/*!< 0x00a8 TIMIC
*Timer0 interrupt. (group0)
*/
#define TIM0IC_Msk                                          (0x01 << TIM0IC_Pos)
#define TIM0IC_Pos                                          0
#define TIM0IC_PosL                                         0
#define TIM0IC_PosH                                         0


/*!< 0x00ac TIMIS
*Timer3 interrupt occurs. (group3)
*/
#define TIM3IS_Msk                                          (0x01 << TIM3IS_Pos)
#define TIM3IS_Pos                                          3
#define TIM3IS_PosL                                         3
#define TIM3IS_PosH                                         3

/*!< 0x00ac TIMIS
*Timer2 interrupt occurs. (group2)
*/
#define TIM2IS_Msk                                          (0x01 << TIM2IS_Pos)
#define TIM2IS_Pos                                          2
#define TIM2IS_PosL                                         2
#define TIM2IS_PosH                                         2

/*!< 0x00ac TIMIS
*Timer1 interrupt occurs. (group1)
*/
#define TIM1IS_Msk                                          (0x01 << TIM1IS_Pos)
#define TIM1IS_Pos                                          1
#define TIM1IS_PosL                                         1
#define TIM1IS_PosH                                         1

/*!< 0x00ac TIMIS
*Timer0 interrupt occurs. (group0)
*/
#define TIM0IS_Msk                                          (0x01 << TIM0IS_Pos)
#define TIM0IS_Pos                                          0
#define TIM0IS_PosL                                         0
#define TIM0IS_PosH                                         0


/*!< 0x00b0 TIM01LOAD
*Timer 1 load value：group1
*/
#define TIMLOAD1_Msk                                        (0xFFFF << TIMLOAD1_Pos)
#define TIMLOAD1_Pos                                        16
#define TIMLOAD1_PosL                                       16
#define TIMLOAD1_PosH                                       31

/*!< 0x00b0 TIM01LOAD
*Timer 0 load value： group0
*/
#define TIMLOAD0_Msk                                        (0xFFFF << TIMLOAD0_Pos)
#define TIMLOAD0_Pos                                        0
#define TIMLOAD0_PosL                                       0
#define TIMLOAD0_PosH                                       15


/*!< 0x00b4 TIM23LOAD
*Timer 3 load value
*/
#define TIMLOAD3_Msk                                        (0xFFFF << TIMLOAD3_Pos)
#define TIMLOAD3_Pos                                        16
#define TIMLOAD3_PosL                                       16
#define TIMLOAD3_PosH                                       31

/*!< 0x00b4 TIM23LOAD
*Timer 2 load value：group2
*/
#define TIMLOAD2_Msk                                        (0xFFFF << TIMLOAD2_Pos)
#define TIMLOAD2_Pos                                        0
#define TIMLOAD2_PosL                                       0
#define TIMLOAD2_PosH                                       15


/*!< 0x00bc TIM01COUNT
*Timer 1 current count value：group1
*/
#define TIMCNT1_Msk                                         (0xFFFF << TIMCNT1_Pos)
#define TIMCNT1_Pos                                         16
#define TIMCNT1_PosL                                        16
#define TIMCNT1_PosH                                        31

/*!< 0x00bc TIM01COUNT
*Timer 0 current count value：group0
*/
#define TIMCNT0_Msk                                         (0xFFFF << TIMCNT0_Pos)
#define TIMCNT0_Pos                                         0
#define TIMCNT0_PosL                                        0
#define TIMCNT0_PosH                                        15


/*!< 0x00c0 TIM23COUNT
*Timer 3 current count value：group3
*/
#define TIMCNT3_Msk                                         (0xFFFF << TIMCNT3_Pos)
#define TIMCNT3_Pos                                         16
#define TIMCNT3_PosL                                        16
#define TIMCNT3_PosH                                        31

/*!< 0x00c0 TIM23COUNT
*Timer 2 current count value：group2
*/
#define TIMCNT2_Msk                                         (0xFFFF << TIMCNT2_Pos)
#define TIMCNT2_Pos                                         0
#define TIMCNT2_PosL                                        0
#define TIMCNT2_PosH                                        15


/*!< 0x00c8 CNT01VAL
*Counter 1 captured input pulse number value：group1
*/
#define CNT1VAL_Msk                                         (0xFFFF << CNT1VAL_Pos)
#define CNT1VAL_Pos                                         16
#define CNT1VAL_PosL                                        16
#define CNT1VAL_PosH                                        31

/*!< 0x00c8 CNT01VAL
*Counter 0 captured input pulse number value：group0
*/
#define CNT0VAL_Msk                                         (0xFFFF << CNT0VAL_Pos)
#define CNT0VAL_Pos                                         0
#define CNT0VAL_PosL                                        0
#define CNT0VAL_PosH                                        15


/*!< 0x00cc CNT23VAL
*Counter 3 captured input pulse number value：group3
*/
#define CNT3VAL_Msk                                         (0xFFFF << CNT3VAL_Pos)
#define CNT3VAL_Pos                                         16
#define CNT3VAL_PosL                                        16
#define CNT3VAL_PosH                                        31

/*!< 0x00cc CNT23VAL
*Counter 2 captured input pulse number value：group2
*/
#define CNT2VAL_Msk                                         (0xFFFF << CNT2VAL_Pos)
#define CNT2VAL_Pos                                         0
#define CNT2VAL_PosL                                        0
#define CNT2VAL_PosH                                        15


/*!< 0x00d4 PWM01TRIG_H
*The value generating group 1 trigger signal high part
*/
#define PWM1TRIG_H_Msk                                      (0xFFFF << PWM1TRIG_H_Pos)
#define PWM1TRIG_H_Pos                                      16
#define PWM1TRIG_H_PosL                                     16
#define PWM1TRIG_H_PosH                                     31

/*!< 0x00d4 PWM01TRIG_H
*The value generating group 0 trigger signal high part
*/
#define PWM0TRIG_H_Msk                                      (0xFFFF << PWM0TRIG_H_Pos)
#define PWM0TRIG_H_Pos                                      0
#define PWM0TRIG_H_PosL                                     0
#define PWM0TRIG_H_PosH                                     15


/*!< 0x00d8 PWM23TRIG_H
*The value generating group 3 trigger signal high part
*/
#define PWM3TRIG_H_Msk                                      (0xFFFF << PWM3TRIG_H_Pos)
#define PWM3TRIG_H_Pos                                      16
#define PWM3TRIG_H_PosL                                     16
#define PWM3TRIG_H_PosH                                     31

/*!< 0x00d8 PWM23TRIG_H
*The value generating group 2 trigger signal high part
*/
#define PWM2TRIG_H_Msk                                      (0xFFFF << PWM2TRIG_H_Pos)
#define PWM2TRIG_H_Pos                                      0
#define PWM2TRIG_H_PosL                                     0
#define PWM2TRIG_H_PosH                                     15


/*!< 0x00e0 PWM0CMP_H
*Comparator B value high part：group0-channel1 The value to be compared against the counter
*/
#define COMP0B_H_Msk                                        (0xFFFF << COMP0B_H_Pos)
#define COMP0B_H_Pos                                        16
#define COMP0B_H_PosL                                       16
#define COMP0B_H_PosH                                       31

/*!< 0x00e0 PWM0CMP_H
*Comparator A value high part：group0-channel0 The value to be compared against the counter
*/
#define COMP0A_H_Msk                                        (0xFFFF << COMP0A_H_Pos)
#define COMP0A_H_Pos                                        0
#define COMP0A_H_PosL                                       0
#define COMP0A_H_PosH                                       15


/*!< 0x00e4 PWM1CMP_H
*Comparator B value high part：group1-channel3 The value to be compared against the counter
*/
#define COMP1B_H_Msk                                        (0xFFFF << COMP1B_H_Pos)
#define COMP1B_H_Pos                                        16
#define COMP1B_H_PosL                                       16
#define COMP1B_H_PosH                                       31

/*!< 0x00e4 PWM1CMP_H
*Comparator A value high part：group1-channel2 The value to be compared against the counter
*/
#define COMP1A_H_Msk                                        (0xFFFF << COMP1A_H_Pos)
#define COMP1A_H_Pos                                        0
#define COMP1A_H_PosL                                       0
#define COMP1A_H_PosH                                       15


/*!< 0x00e8 PWM2CMP_H
*Comparator B value high part：group1-channel3 The value to be compared against the counter
*/
#define COMP1B_H_Msk                                        (0xFFFF << COMP1B_H_Pos)
#define COMP1B_H_Pos                                        16
#define COMP1B_H_PosL                                       16
#define COMP1B_H_PosH                                       31

/*!< 0x00e8 PWM2CMP_H
*Comparator A value high part：group1-channel2 The value to be compared against the counter
*/
#define COMP1A_H_Msk                                        (0xFFFF << COMP1A_H_Pos)
#define COMP1A_H_Pos                                        0
#define COMP1A_H_PosL                                       0
#define COMP1A_H_PosH                                       15


/*!< 0x00ec PWM3CMP_H
*Comparator B value high part：group3-channel7 The value to be compared against the counter
*/
#define COMP3B_H_Msk                                        (0xFFFF << COMP3B_H_Pos)
#define COMP3B_H_Pos                                        16
#define COMP3B_H_PosL                                       16
#define COMP3B_H_PosH                                       31

/*!< 0x00ec PWM3CMP_H
*Comparator A value high part：group3-channel6 The value to be compared against the counter
*/
#define COMP3A_H_Msk                                        (0xFFFF << COMP3A_H_Pos)
#define COMP3A_H_Pos                                        0
#define COMP3A_H_PosL                                       0
#define COMP3A_H_PosH                                       15


/*!< 0x00f8 TIM01LOAD_H
*Timer 1 load value high part：group1
*/
#define TIMLOAD1_H_Msk                                      (0xFFFF << TIMLOAD1_H_Pos)
#define TIMLOAD1_H_Pos                                      16
#define TIMLOAD1_H_PosL                                     16
#define TIMLOAD1_H_PosH                                     31

/*!< 0x00f8 TIM01LOAD_H
*Timer 0 load value high part：group1
*/
#define TIMLOAD0_H_Msk                                      (0xFFFF << TIMLOAD0_H_Pos)
#define TIMLOAD0_H_Pos                                      0
#define TIMLOAD0_H_PosL                                     0
#define TIMLOAD0_H_PosH                                     15


/*!< 0x00fc TIM23LOAD_H
*Timer 3 load value high part：group3
*/
#define TIMLOAD3_H_Msk                                      (0xFFFF << TIMLOAD3_H_Pos)
#define TIMLOAD3_H_Pos                                      16
#define TIMLOAD3_H_PosL                                     16
#define TIMLOAD3_H_PosH                                     31

/*!< 0x00fc TIM23LOAD_H
*Timer 2 load value high part：group2
*/
#define TIMLOAD2_H_Msk                                      (0xFFFF << TIMLOAD2_H_Pos)
#define TIMLOAD2_H_Pos                                      0
#define TIMLOAD2_H_PosL                                     0
#define TIMLOAD2_H_PosH                                     15


/*!< 0x0104 PWM01LOAD_H
*Counter load 1 value high part：group1-channel3/2 The counter load value
*/
#define LOAD1_H_Msk                                         (0xFFFF << LOAD1_H_Pos)
#define LOAD1_H_Pos                                         16
#define LOAD1_H_PosL                                        16
#define LOAD1_H_PosH                                        31

/*!< 0x0104 PWM01LOAD_H
*Counter load 0 value high part：group0-channel1/0 The counter load value
*/
#define LOAD0_H_Msk                                         (0xFFFF << LOAD0_H_Pos)
#define LOAD0_H_Pos                                         0
#define LOAD0_H_PosL                                        0
#define LOAD0_H_PosH                                        15


/*!< 0x0108 PWM23LOAD_H
*Counter load 3 value high part：group3-channel7/6 The counter load value
*/
#define LOAD3_H_Msk                                         (0xFFFF << LOAD3_H_Pos)
#define LOAD3_H_Pos                                         16
#define LOAD3_H_PosL                                        16
#define LOAD3_H_PosH                                        31

/*!< 0x0108 PWM23LOAD_H
*Counter load 2 value high part：group2-channel5/4 The counter load value
*/
#define LOAD2_H_Msk                                         (0xFFFF << LOAD2_H_Pos)
#define LOAD2_H_Pos                                         0
#define LOAD2_H_PosL                                        0
#define LOAD2_H_PosH                                        15


/*!< 0x0110 CAP01MATCH_H
*Input capture 1 match value high part：group1-channel2
*/
#define CAP01MATCH1_H_Msk                                   (0xFFFF << CAP01MATCH1_H_Pos)
#define CAP01MATCH1_H_Pos                                   16
#define CAP01MATCH1_H_PosL                                  16
#define CAP01MATCH1_H_PosH                                  31

/*!< 0x0110 CAP01MATCH_H
*Input capture 0 match value high part：group0-channel0
*/
#define CAP01MATCH0_H_Msk                                   (0xFFFF << CAP01MATCH0_H_Pos)
#define CAP01MATCH0_H_Pos                                   0
#define CAP01MATCH0_H_PosL                                  0
#define CAP01MATCH0_H_PosH                                  15


/*!< 0x0114 CAP23MATCH_H
*Input capture 3 match value high part：group3-channel6
*/
#define CAP23MATCH3_H_Msk                                   (0xFFFF << CAP23MATCH3_H_Pos)
#define CAP23MATCH3_H_Pos                                   16
#define CAP23MATCH3_H_PosL                                  16
#define CAP23MATCH3_H_PosH                                  31

/*!< 0x0114 CAP23MATCH_H
*Input capture 2 match value high part：group2-channel4
*/
#define CAP23MATCH2_H_Msk                                   (0xFFFF << CAP23MATCH2_H_Pos)
#define CAP23MATCH2_H_Pos                                   0
#define CAP23MATCH2_H_PosL                                  0
#define CAP23MATCH2_H_PosH                                  15


/*!< 0x011c PWM01COUNT_H
*Counter 1 value high part：group1-channel3/2 The current value of the counter
*/
#define COUNT1_H_Msk                                        (0xFFFF << COUNT1_H_Pos)
#define COUNT1_H_Pos                                        16
#define COUNT1_H_PosL                                       16
#define COUNT1_H_PosH                                       31

/*!< 0x011c PWM01COUNT_H
*Counter 0 value high part：group0-channel1/0 The current value of the counter
*/
#define COUNT0_H_Msk                                        (0xFFFF << COUNT0_H_Pos)
#define COUNT0_H_Pos                                        0
#define COUNT0_H_PosL                                       0
#define COUNT0_H_PosH                                       15


/*!< 0x0120 PWM23COUNT_H
*Counter 3 value high part：group3-channel7/6 The current value of the counter
*/
#define COUNT3_H_Msk                                        (0xFFFF << COUNT3_H_Pos)
#define COUNT3_H_Pos                                        16
#define COUNT3_H_PosL                                       16
#define COUNT3_H_PosH                                       31

/*!< 0x0120 PWM23COUNT_H
*Counter 2 value high part：group2-channel5/4 The current value of the counter
*/
#define COUNT2_H_Msk                                        (0xFFFF << COUNT2_H_Pos)
#define COUNT2_H_Pos                                        0
#define COUNT2_H_PosL                                       0
#define COUNT2_H_PosH                                       15


/*!< 0x0128 CAP01T_H
*null
*/
#define CAP01T1_H_Msk                                       (0xFFFF << CAP01T1_H_Pos)
#define CAP01T1_H_Pos                                       16
#define CAP01T1_H_PosL                                      16
#define CAP01T1_H_PosH                                      31

/*!< 0x0128 CAP01T_H
*Input capture 0 counter value high part：group0-channel0
*/
#define CAP01T0_H_Msk                                       (0xFFFF << CAP01T0_H_Pos)
#define CAP01T0_H_Pos                                       0
#define CAP01T0_H_PosL                                      0
#define CAP01T0_H_PosH                                      15


/*!< 0x012c CAP23T_H
*Input capture 3 counter value high part：group3-channel6
*/
#define CAP23T3_H_Msk                                       (0xFFFF << CAP23T3_H_Pos)
#define CAP23T3_H_Pos                                       16
#define CAP23T3_H_PosL                                      16
#define CAP23T3_H_PosH                                      31

/*!< 0x012c CAP23T_H
*Input capture 0 counter value high part：group0-channel0
*/
#define CAP23T2_H_Msk                                       (0xFFFF << CAP01T0_H_Pos)
#define CAP23T2_H_Pos                                       0
#define CAP23T2_H_PosL                                      0
#define CAP23T2_H_PosH                                      15


/*!< 0x0134 TIM01COUNT_H
*Timer 1 current count value high part：group1
*/
#define TIMCNT1_H_Msk                                       (0xFFFF << TIMCNT1_H_Pos)
#define TIMCNT1_H_Pos                                       16
#define TIMCNT1_H_PosL                                      16
#define TIMCNT1_H_PosH                                      31

/*!< 0x0134 TIM01COUNT_H
*Timer 0 current count value high part：group0
*/
#define TIMCNT0_H_Msk                                       (0xFFFF << TIMCNT0_H_Pos)
#define TIMCNT0_H_Pos                                       0
#define TIMCNT0_H_PosL                                      0
#define TIMCNT0_H_PosH                                      15


/*!< 0x0138 TIM23COUNT_H
*Timer 3 current count value high part：group3
*/
#define TIMCNT3_H_Msk                                       (0xFFFF << TIMCNT3_H_Pos)
#define TIMCNT3_H_Pos                                       16
#define TIMCNT3_H_PosL                                      16
#define TIMCNT3_H_PosH                                      31

/*!< 0x0138 TIM23COUNT_H
*Timer 2 current count value high part：group2
*/
#define TIMCNT2_H_Msk                                       (0xFFFF << TIMCNT2_H_Pos)
#define TIMCNT2_H_Pos                                       0
#define TIMCNT2_H_PosL                                      0
#define TIMCNT2_H_PosH                                      15


/*!< 0x0140 CNT01VAL_H
*Counter 1 captured input pulse number value high part
*/
#define CNT1VAL_H_Msk                                       (0xFFFF << CNT1VAL_H_Pos)
#define CNT1VAL_H_Pos                                       16
#define CNT1VAL_H_PosL                                      16
#define CNT1VAL_H_PosH                                      31

/*!< 0x0140 CNT01VAL_H
*Counter n captured input pulse number value high part：group0
*/
#define CNT0VAL_H_Msk                                       (0xFFFF << CNT0VAL_H_Pos)
#define CNT0VAL_H_Pos                                       0
#define CNT0VAL_H_PosL                                      0
#define CNT0VAL_H_PosH                                      15


/*!< 0x0144 CNT23VAL_H
*Counter 3 captured input pulse number value high part：group3
*/
#define CNT3VAL_H_Msk                                       (0xFFFF << CNT3VAL_H_Pos)
#define CNT3VAL_H_Pos                                       16
#define CNT3VAL_H_PosL                                      16
#define CNT3VAL_H_PosH                                      31

/*!< 0x0144 CNT23VAL_H
*Counter 0 captured input pulse number value high part：group0
*/
#define CNT0VAL_H_Msk                                       (0xFFFF << CNT0VAL_H_Pos)
#define CNT0VAL_H_Pos                                       0
#define CNT0VAL_H_PosL                                      0
#define CNT0VAL_H_PosH                                      15


/*!< 0x014c AUTOMODE
*Group 3 auto mode configuration
*/
#define AUTO3EN_Msk                                         (0x01 << AUTO3EN_Pos)
#define AUTO3EN_Pos                                         3
#define AUTO3EN_PosL                                        3
#define AUTO3EN_PosH                                        3

/*!< 0x014c AUTOMODE
*Group 2 auto mode configuration
*/
#define AUTO2EN_Msk                                         (0x01 << AUTO2EN_Pos)
#define AUTO2EN_Pos                                         2
#define AUTO2EN_PosL                                        2
#define AUTO2EN_PosH                                        2

/*!< 0x014c AUTOMODE
*Group 1 auto mode configuration
*/
#define AUTO1EN_Msk                                         (0x01 << AUTO1EN_Pos)
#define AUTO1EN_Pos                                         1
#define AUTO1EN_PosL                                        1
#define AUTO1EN_PosH                                        1

/*!< 0x014c AUTOMODE
*Group 0 auto mode configuration
*/
#define AUTO0EN_Msk                                         (0x01 << AUTO0EN_Pos)
#define AUTO0EN_Pos                                         0
#define AUTO0EN_PosL                                        0
#define AUTO0EN_PosH                                        0


/*!< 0x0150 AUTOCNT0
*This register defines how many cycle will be taken before one step：group0-channel1
*/
#define STEP_CYCLE_Msk                                      (0xFF << STEP_CYCLE_Pos)
#define STEP_CYCLE_Pos                                      24
#define STEP_CYCLE_PosL                                     24
#define STEP_CYCLE_PosH                                     31

/*!< 0x0150 AUTOCNT0
*This register defines how many cycle will be taken for whole auto change process：group0-channel0
*/
#define TOTAL_CYCLE_Msk                                     (0xFFFFF << TOTAL_CYCLE_Pos)
#define TOTAL_CYCLE_Pos                                     0
#define TOTAL_CYCLE_PosL                                    0
#define TOTAL_CYCLE_PosH                                    23


/*!< 0x0154 AUTOCNT1
*This register defines how many cycle will be taken before one step：group1-channel3
*/
#define STEP_CYCLE_Msk                                      (0xFF << STEP_CYCLE_Pos)
#define STEP_CYCLE_Pos                                      24
#define STEP_CYCLE_PosL                                     24
#define STEP_CYCLE_PosH                                     31

/*!< 0x0154 AUTOCNT1
*This register defines how many cycle will be taken for whole auto change process：group1-channel2
*/
#define TOTAL_CYCLE_Msk                                     (0xFFFFF << TOTAL_CYCLE_Pos)
#define TOTAL_CYCLE_Pos                                     0
#define TOTAL_CYCLE_PosL                                    0
#define TOTAL_CYCLE_PosH                                    23


/*!< 0x0158 AUTOCNT2
*This register defines how many cycle will be taken before one step：group2-channel5
*/
#define STEP_CYCLE_Msk                                      (0xFF << STEP_CYCLE_Pos)
#define STEP_CYCLE_Pos                                      24
#define STEP_CYCLE_PosL                                     24
#define STEP_CYCLE_PosH                                     31

/*!< 0x0158 AUTOCNT2
*This register defines how many cycle will be taken for whole auto change process：group2-channel4
*/
#define TOTAL_CYCLE_Msk                                     (0xFFFFF << TOTAL_CYCLE_Pos)
#define TOTAL_CYCLE_Pos                                     0
#define TOTAL_CYCLE_PosL                                    0
#define TOTAL_CYCLE_PosH                                    23


/*!< 0x015c AUTOCNT3
*This register defines how many cycle will be taken before one step：group3-channel7
*/
#define STEP_CYCLE_Msk                                      (0xFF << STEP_CYCLE_Pos)
#define STEP_CYCLE_Pos                                      24
#define STEP_CYCLE_PosL                                     24
#define STEP_CYCLE_PosH                                     31

/*!< 0x015c AUTOCNT3
*This register defines how many cycle will be taken for whole auto change process：group3-channel6
*/
#define TOTAL_CYCLE_Msk                                     (0xFFFFF << TOTAL_CYCLE_Pos)
#define TOTAL_CYCLE_Pos                                     0
#define TOTAL_CYCLE_PosL                                    0
#define TOTAL_CYCLE_PosH                                    23


/*!< 0x0168 AUTO0CMPA
*This register defines the step length of PWM auto change mode：group0-channel0
*/
#define AUTO_COMPA_Msk                                      (0XFFFFFFFF << AUTO_COMPA_Pos)
#define AUTO_COMPA_Pos                                      0
#define AUTO_COMPA_PosL                                     0
#define AUTO_COMPA_PosH                                     31


/*!< 0x016c AUTO1CMPA
*This register defines the step length of PWM auto change mode：group0-channel0
*/
#define AUTO_COMPA_Msk                                      (0XFFFFFFFF << AUTO_COMPA_Pos)
#define AUTO_COMPA_Pos                                      0
#define AUTO_COMPA_PosL                                     0
#define AUTO_COMPA_PosH                                     31


/*!< 0x0170 AUTO2CMPA
*This register defines the step length of PWM auto change mode：group2-channel4
*/
#define AUTO_COMPA_Msk                                      (0XFFFFFFFF << AUTO_COMPA_Pos)
#define AUTO_COMPA_Pos                                      0
#define AUTO_COMPA_PosL                                     0
#define AUTO_COMPA_PosH                                     31


/*!< 0x0180 AUTO0CMPB
*This register defines the step length of PWM auto change mode：group0-channel1
*/
#define AUTO_COMPB_Msk                                      (0XFFFFFFFF << AUTO_COMPB_Pos)
#define AUTO_COMPB_Pos                                      0
#define AUTO_COMPB_PosL                                     0
#define AUTO_COMPB_PosH                                     31


/*!< 0x0184 AUTO1CMPB
*This register defines the step length of PWM auto change mode：group1-channel3
*/
#define AUTO_COMPB_Msk                                      (0XFFFFFFFF << AUTO_COMPB_Pos)
#define AUTO_COMPB_Pos                                      0
#define AUTO_COMPB_PosL                                     0
#define AUTO_COMPB_PosH                                     31


/*!< 0x0188 AUTO1CMPB
*This register defines the step length of PWM auto change mode：group2-channel5
*/
#define AUTO_COMPB_Msk                                      (0XFFFFFFFF << AUTO_COMPB_Pos)
#define AUTO_COMPB_Pos                                      0
#define AUTO_COMPB_PosL                                     0
#define AUTO_COMPB_PosH                                     31


/**
* @}
* End of group HW_PWM
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_PWM_H_ */