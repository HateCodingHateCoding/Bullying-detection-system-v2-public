/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef FFT_FUNC_H
#define FFT_FUNC_H
#include <stdint.h>

#if (CONFIG_SOC_CPU_F)
#include "csi_const_structs.h"
#include "csi_math.h"
#endif
#include "HwAux.h"

#define MAX_FFT_LEN         (256u)

#ifndef NULL_PTR
#define NULL_PTR            0//((void*)0)
#endif
#ifndef TRUE
#define TRUE                (1)
#endif
#ifndef FALSE
#define FALSE               (0)
#endif

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


#ifndef DIVIDE_POW_2
#define DIVIDE_POW_2(x, y) ((x) >> (y)) + (((x) >> ((y)-1)) & 0x01)
#endif
void RunComplexFFT(uint32_t* fft_src, uint8_t fft_factor, uint32_t *fft_out);

#endif
