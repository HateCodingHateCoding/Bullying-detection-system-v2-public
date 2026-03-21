/****************************************************************
* @file  watchdog.h
* @brief watchdog hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_WDG_H_
#define _HW_WDG_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup watchdog
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __IOM uint32_t CR;                                      /*0x0000,           */
    __IM  uint32_t CVR;                                     /*0x0004,           */
    __OM  uint32_t RST;                                     /*0x0008,           */
    __IM  uint32_t ISR;                                     /*0x000c,           */
    __IOM uint32_t ICR;                                     /*0x0010,           */
          uint32_t RSVD[2];                                 /*0x0014-0x0018,    */
          uint32_t TOP;                                     /*0x001c,           */
} wdg_dev_t;

/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x0000 CR
 *
 */
#define NO_NAME_Msk                                         (0x01 << NO_NAME_Pos)
#define NO_NAME_Pos                                         5
#define NO_NAME_PosL                                        5
#define NO_NAME_PosH                                        5

/*!< 0x0000 CR
 *
 */
#define RPL_Msk                                             (0x07 << RPL_Pos)
#define RPL_Pos                                             2
#define RPL_PosL                                            2
#define RPL_PosH                                            4

/*!< 0x0000 CR
 *
 */
#define RESPONSEMODE_Msk                                    (0x01 << RESPONSEMODE_Pos)
#define RESPONSEMODE_Pos                                    1
#define RESPONSEMODE_PosL                                   1
#define RESPONSEMODE_PosH                                   1

/*!< 0x0000 CR
 *
 */
#define WDTENABLE_Msk                                       (0x01 << WDTENABLE_Pos)
#define WDTENABLE_Pos                                       0
#define WDTENABLE_PosL                                      0
#define WDTENABLE_PosH                                      0


/*!< 0x0004 CVR
 *
 */
#define CCVR_Msk                                            (0XFFFFFFFF << CCVR_Pos)
#define CCVR_Pos                                            0
#define CCVR_PosL                                           0
#define CCVR_PosH                                           31


/*!< 0x0008 RST
 *
 */
#define CRR_Msk                                             (0xFFFF << CRR_Pos)
#define CRR_Pos                                             0
#define CRR_PosL                                            0
#define CRR_PosH                                            15


/*!< 0x000c ISR
 *
 */
#define STAT_Msk                                            (0x01 << STAT_Pos)
#define STAT_Pos                                            0
#define STAT_PosL                                           0
#define STAT_PosH                                           0


/*!< 0x0010 ICR
 *
 */
#define EOI_Msk                                             (0x01 << EOI_Pos)
#define EOI_Pos                                             0
#define EOI_PosL                                            0
#define EOI_PosH                                            0


/*!< 0x001c TOP
 *
 */
#define TOP_Msk                                             (0xFFFFFFFF << EOI_Pos)
#define TOP_Pos                                             0
#define TOP_PosL                                            0
#define TOP_PosH                                            0

/**
* @}
* End of group HW_WATCHDOG
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_WDG_H_ */