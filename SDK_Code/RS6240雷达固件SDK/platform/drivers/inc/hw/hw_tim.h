/****************************************************************
* @file  timer.h
* @brief timer hadware define
* @attention
*          Copyright (c) 2023 Possumic Technology. all rights reserved.
****************************************************************
*/
#ifndef _HW_TIM_H_
#define _HW_TIM_H_


#ifdef __cplusplus
  extern "C" {
#endif


/** @addtopgroup timer
 * @{
 * */

/** Type definition for device register
 * ----------------------------------------------------------------------------
 * */

typedef struct {
	__IOM uint32_t CTRL;                                  	/*0x0000,Control Register for Timer N*/
      uint32_t RSVD;
    __IM  uint32_t CLR;                                   	/*0x0008,Clears the interrupt from Ti*/
    __IM  uint32_t ISR;                                   	/*0x000c,Contains the interrupt statu*/
    __IOM uint32_t LVR;                                   	/*0x0010,Value to be loaded into Time*/
    __IM  uint32_t CVR;                                   	/*0x0014,Current value of Timer      */

} timer_dev_t;

/*!< 0x0000 CTRL
*Timer interrupt mask for Timer N.
*/
#define TIMER_INTERRUPT_MASK_Msk                            (0x01 << TIMER_INTERRUPT_MASK_Pos)
#define TIMER_INTERRUPT_MASK_Pos                            2
#define TIMER_INTERRUPT_MASK_PosL                           2
#define TIMER_INTERRUPT_MASK_PosH                           2

/*!< 0x0000 CTRL
*Timer mode for Timer N, must set the Timer1LoadCount register to all 1s before enabling the timer in free-running mode.
*/
#define TIMER_MODE_Msk                                      (0x01 << TIMER_MODE_Pos)
#define TIMER_MODE_Pos                                      1
#define TIMER_MODE_PosL                                     1
#define TIMER_MODE_PosH                                     1

/*!< 0x0000 CTRL
*Timer enable bit for Timer N.
*/
#define TIMER_ENABLE_Msk                                    (0x01 << TIMER_ENABLE_Pos)
#define TIMER_ENABLE_Pos                                    0
#define TIMER_ENABLE_PosL                                   0
#define TIMER_ENABLE_PosH                                   0

/*!< 0x0008 CLR
*Reading from this register returns all zeroes (0) and clears the interrupt from Timer N.
*/
#define TIMERNEOI_Msk                                       (0x01 << TIMERNEOI_Pos)
#define TIMERNEOI_Pos                                       0
#define TIMERNEOI_PosL                                      0
#define TIMERNEOI_PosH                                      0


/*!< 0x000c ISR
*The register contains the unmasked interrupt status of all timers in the component.
*/
#define TIMERSRAWINTSTAT_Msk                                (0x01 << TIMERSRAWINTSTAT_Pos)
#define TIMERSRAWINTSTAT_Pos                                0
#define TIMERSRAWINTSTAT_PosL                               0
#define TIMERSRAWINTSTAT_PosH                               0


/*!< 0x0010 LVR
*Value to be loaded into Timer N. This is the value from which counting commences. Any value written to this register is loaded into the associated timer.
*/
#define TIMERNLOADCOUNT_Msk                                 (0XFFFFFFFF << TIMERNLOADCOUNT_Pos)
#define TIMERNLOADCOUNT_Pos                                 0
#define TIMERNLOADCOUNT_PosL                                0
#define TIMERNLOADCOUNT_PosH                                31


/*!< 0x0014 CVR
*Current Value of Timer N.
*/
#define TIMERNCURRENTVALUE_Msk                              (0XFFFFFFFF << TIMERNCURRENTVALUE_Pos)
#define TIMERNCURRENTVALUE_Pos                              0
#define TIMERNCURRENTVALUE_PosL                             0
#define TIMERNCURRENTVALUE_PosH                             31


/**
* @}
* End of group HW_TIMER
*/

#ifdef __cplusplus
}
#endif

#endif /* _HW_TIM_H_ */