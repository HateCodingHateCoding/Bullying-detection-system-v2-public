/**
 ************************************************************************
 * @file    hw_splk.h
 * @brief   splk hadware define.
 * @attention
 *          Copyright (c) 2022 possumic Technology. all rights reserved.
 ************************************************************************
 */

#ifndef _HW_SPLK_H_
#define _HW_SPLK_H_


#ifdef __cplusplus
 extern "C" {
#endif


/** @addtogroup hw_splk
 * @{
 */

/* Type definition for device register.
 * ----------------------------------------------------------------------
 */
#define SPLK_DEV_NUM 16
typedef struct {
  __IOM uint32_t  CTRL[SPLK_DEV_NUM];        /*!< 0x00,                          */
} splk_dev_t;



/* Bits definition for device register.
 * ----------------------------------------------------------------------
 */

/*!< 0x00, CTRL.
 * ------------------------------------------------------------
 */
#define CTRL_SPINLOCK_Msk                                      0x01
#define CTRL_SPINLOCK_Pos                                      0


/**
 * @}
 * End of group hw_splk
 */

#ifdef __cplusplus
}
#endif

#endif /* _HW_SPLK_H_ */

/*
 ************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */

