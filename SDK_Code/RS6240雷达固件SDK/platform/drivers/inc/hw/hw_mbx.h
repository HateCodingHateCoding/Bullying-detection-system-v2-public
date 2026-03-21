/****************************************************************
* @file  mailbox.h
* @brief mailbox hadware define
* @attention
*          Copyright (c) 2024 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_MBX_H_
#define _HW_MBX_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup mailbox
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
    __OM  uint32_t ISCR;                                    /*0x0000,Interrupt Set Command Regist*/
    __OM  uint32_t ICCR;                                    /*0x0004,Interrupt Clear Command Regi*/
    __OM  uint32_t IPR;                                     /*0x0008,Interrupt Pending Register                         */
    __OM  uint32_t IECR;                                    /*0x000c,                                               */
    __OM  uint32_t IDCR;                                    /*0x0010,Interrupt Disable Command Re*/
    __OM  uint32_t IMR;                                     /*0x0014,Interrupt Mask Register                            */
} mbox_dev_t;

/*!< 0x0000 ISCR
 * Interrupt Set.
 */
#define ISCR_Msk                           (0XFFFFFFFF << ISCR_Pos)
#define ISCR_Pos                           0
#define ISCR_PosL                          0
#define ISCR_PosH                          31


/*!< 0x0004 ICCR
 * Interrupt Clear.
 */
#define ICCR_Msk                           (0XFFFFFFFF << ICCR_Pos)
#define ICCR_Pos                           0
#define ICCR_PosL                          0
#define ICCR_PosH                          31


/*!< 0x0008 IPR
 * Interrupt Pending.
 */
#define IPR_Msk                           (0XFFFFFFFF << IPR_Pos)
#define IPR_Pos                           0
#define IPR_PosL                          0
#define IPR_PosH                          31


/*!< 0x000c IECR
 * Interrupt Enable.
 */
#define IECR_Msk                           (0XFFFFFFFF << IECR_Pos)
#define IECR_Pos                           0
#define IECR_PosL                          0
#define IECR_PosH                          31


/*!< 0x0010 IDCR
 * Interrupt Disable.
 */
#define IDCR_Msk                           (0XFFFFFFFF << IDCR_Pos_Pos)
#define IDCR_Pos                           0
#define IDCR_PosL                          0
#define IDCR_PosH                          31


/*!< 0x0014 IMR
 * Interrupt Mask.
 */
#define IMR_Msk                           (0XFFFFFFFF << IMR_Pos)
#define IMR_Pos                           0
#define IMR_PosL                          0
#define IMR_PosH                          31


/**
* @}
* End of group HW_MAILBOX
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_MBX_H_ */