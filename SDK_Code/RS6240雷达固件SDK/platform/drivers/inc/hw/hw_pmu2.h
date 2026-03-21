/****************************************************************
* @file  hw_pmu2.h
* @brief hw_pmu2 hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_PMU2_H_
#define _HW_PMU2_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup HW_PMU2
 * @{
 * */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */

typedef struct {
  __IOM uint32_t SYSCFG_AON_SW_STATUS;                    /*0x0000,             */
        uint32_t RSVD[3];                                 /*0x0004-0x000c,      */
  __IOM uint32_t SYSCFG_MMWAVE_STATUS;                    /*0x0010,             */
        uint32_t RSVD1[7];                                /*0x0014-0x002c,      */
  __IOM uint32_t PMU2_M50_CTRL;                           /*0x0030,             */
} pmu2_dev_t;


#define PMU2_DEV                        ((pmu2_dev_t *)   PWR2_BASE_ADDR)  /*!< 0x4001 E400 */
/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */
 
/*!< 0x0000 SYSCFG_AON_SW_STATUS
*null
*/
#define SWP32F_STATUS_Msk                                   (0x01 << SWP32F_STATUS_Pos)
#define SWP32F_STATUS_Pos                                   1
#define SWP32F_STATUS_PosL                                  1
#define SWP32F_STATUS_PosH                                  1

/*!< 0x0000 SYSCFG_AON_SW_STATUS
*null
*/
#define SWP32S_STATUS_Msk                                   (0x01 << SWP32S_STATUS_Pos)
#define SWP32S_STATUS_Pos                                   0
#define SWP32S_STATUS_PosL                                  0
#define SWP32S_STATUS_PosH                                  0


/*!< 0x0010 SYSCFG_MMWAVE_STATUS
*null
*/
#define CUBE_WORK_EN_Msk                                    (0x01 << CUBE_WORK_EN_Pos)
#define CUBE_WORK_EN_Pos                                    1
#define CUBE_WORK_EN_PosL                                   1
#define CUBE_WORK_EN_PosH                                   1

/*!< 0x0010 SYSCFG_MMWAVE_STATUS
*null
*/
#define BB_WORK_EN_Msk                                      (0x01 << BB_WORK_EN_Pos)
#define BB_WORK_EN_Pos                                      0
#define BB_WORK_EN_PosL                                     0
#define BB_WORK_EN_PosH                                     0


/*!< 0x0030 PMU2_M50_CTRL
*null
*/
#define M50_DCXO_READY_Msk                                  (0x01 << M50_DCXO_READY_Pos)
#define M50_DCXO_READY_Pos                                  20
#define M50_DCXO_READY_PosL                                 20
#define M50_DCXO_READY_PosH                                 20

/*!< 0x0030 PMU2_M50_CTRL
*null
*/
#define M50_DCXO_EN_Msk                                     (0x01 << M50_DCXO_EN_Pos)
#define M50_DCXO_EN_Pos                                     19
#define M50_DCXO_EN_PosL                                    19
#define M50_DCXO_EN_PosH                                    19

/*!< 0x0030 PMU2_M50_CTRL
*null
*/
#define M50_LDOBYPASS_Msk                                   (0x01 << M50_LDOBYPASS_Pos)
#define M50_LDOBYPASS_Pos                                   18
#define M50_LDOBYPASS_PosL                                  18
#define M50_LDOBYPASS_PosH                                  18

/*!< 0x0030 PMU2_M50_CTRL
*null
*/
#define M50_DIGCLKEN_Msk                                    (0x01 << M50_DIGCLKEN_Pos)
#define M50_DIGCLKEN_Pos                                    16
#define M50_DIGCLKEN_PosL                                   16
#define M50_DIGCLKEN_PosH                                   16

/*!< 0x0030 PMU2_M50_CTRL
*null
*/
#define M50_HRESETN_EN_Msk                                  (0x01 << M50_HRESETN_EN_Pos)
#define M50_HRESETN_EN_Pos                                  1
#define M50_HRESETN_EN_PosL                                 1
#define M50_HRESETN_EN_PosH                                 1

/*!< 0x0030 PMU2_M50_CTRL
*null
*/
#define M50_HCLK_EN_Msk                                     (0x01 << M50_HCLK_EN_Pos)
#define M50_HCLK_EN_Pos                                     0
#define M50_HCLK_EN_PosL                                    0
#define M50_HCLK_EN_PosH                                    0


/**
* @}
* End of group HW_PMU2
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_PMU2_H_ */