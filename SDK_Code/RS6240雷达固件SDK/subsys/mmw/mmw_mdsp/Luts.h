/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef LUTS_H
#define LUTS_H
#include <stdint.h>

#ifndef _TYPE_S16Q15_
#define _TYPE_S16Q15_
typedef int16_t S16Q15;
typedef S16Q15 (* S16Q15_A2)[2];
#endif //_TYPE_S16Q15_

#ifndef _TYPE_S32Q30_
#define _TYPE_S32Q30_
typedef int32_t S32Q30;
typedef S32Q30 (* S32Q30_A2)[2];
#endif //_TYPE_S32Q30_

/* 在FFT中使用的标定数据 */
extern S16Q15 g_LutCos[128][2];

#endif
