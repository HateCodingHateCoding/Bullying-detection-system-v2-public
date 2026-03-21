/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <stdint.h>
#include <string.h>
#include "fft_func.h"
#include "Luts.h"

uint32_t WriteBufferNoFlip(uint32_t *ptr_fft_source, uint32_t *ptr_fft_buffer, uint16_t fft_factor)
{
    uint32_t fft_len = 1 << fft_factor;

    for (uint16_t im = 0; im < fft_len; im++) {
        uint32_t reverse_idx = BitRev(im, fft_factor);
        ptr_fft_buffer[reverse_idx] = ptr_fft_source[im];
    }
    return fft_len;
}

void RunFFTCore1(uint32_t *ptr_fft_buffer, uint32_t fft_len)
{
    uint16_t buf_idx = 0;
    uint32_t cmplx_adder_1, cmplx_adder_2;

    for (buf_idx = 0; buf_idx < fft_len; buf_idx += 2) {
        cmplx_adder_1 = ptr_fft_buffer[buf_idx];
        cmplx_adder_2 = ptr_fft_buffer[buf_idx + 1];
#if MINI_DSP_EN
        SumDiv2Hw(&cmplx_adder_1, &cmplx_adder_2, &ptr_fft_buffer[buf_idx], &ptr_fft_buffer[buf_idx + 1]);
#else
        SumDiv2RoundS16Q15Complex((const S16Q15*)&cmplx_adder_1, (const S16Q15*)&cmplx_adder_2, 1,
                                  (S16Q15 *)&ptr_fft_buffer[buf_idx]);
        SumDiv2RoundS16Q15Complex((const S16Q15*)&cmplx_adder_1, (const S16Q15*)&cmplx_adder_2, 0,
        	                      (S16Q15 *)&ptr_fft_buffer[buf_idx + 1]);
#endif
    }
}

void RunFFTCore2(S16Q15_A2 ptr_fft_buffer, uint32_t fft_len, uint32_t h, uint32_t gap, S16Q15_A2 ptr_lut_cos)
{
    uint32_t idx, k, cos_lut_idx;
    uint16_t step = h << 1;
    S16Q15   weight[2];
#if !(MINI_DSP_EN)
    S16Q15 arr7B16[2], arr8B16[2];
#endif

    cos_lut_idx = gap - 1;
    for (idx = 0; idx < fft_len; idx += step) {
        weight[0] = 32767;
        weight[1] = 0;
        for (k = idx; k < idx + h;) {
#if MINI_DSP_EN
            Complex16MulPath1(ptr_fft_buffer[k + h], weight, ptr_fft_buffer[k], ptr_fft_buffer[k], ptr_fft_buffer[k + h]);
#else
            Complex16Mul(ptr_fft_buffer[k + h], weight, ptr_fft_buffer[k], 0, arr7B16, arr8B16, NULL_PTR);
            *(volatile uint32_t*)ptr_fft_buffer[k] = *(volatile uint32_t*)arr7B16;
            *(volatile uint32_t*)ptr_fft_buffer[k + h] = *(volatile uint32_t*)arr8B16;
#endif
            *(uint32_t*)weight = *(const uint32_t *)ptr_lut_cos[cos_lut_idx];
            cos_lut_idx = (cos_lut_idx + gap) & 127;
            k++;

#if MINI_DSP_EN
            Complex16MulPath1(ptr_fft_buffer[k + h], weight, ptr_fft_buffer[k], ptr_fft_buffer[k], ptr_fft_buffer[k + h]);
#else
            Complex16Mul(ptr_fft_buffer[k + h], weight, ptr_fft_buffer[k], 0, arr7B16, arr8B16, NULL_PTR);
            *(volatile uint32_t*)ptr_fft_buffer[k] = *(volatile uint32_t*)arr7B16;
            *(volatile uint32_t*)ptr_fft_buffer[k + h] = *(volatile uint32_t*)arr8B16;
#endif
            * (uint32_t*)weight = *(const uint32_t *)ptr_lut_cos[cos_lut_idx];
            cos_lut_idx = (cos_lut_idx + gap) & 127;
            k++;
        }
    }
}

void RunComplexFFT(uint32_t* fft_src, uint8_t fft_factor, uint32_t *fft_out)
{
    uint32_t fft_len = 0;
    uint32_t h   = 2;
    uint32_t gap = 64;	/* initial_val = 128/h */

#if (MINI_DSP_EN)
    WRITE_REG(REG_MDSP_CMULT16_MODE, 0);
#endif
    fft_len = WriteBufferNoFlip(fft_src, fft_out, fft_factor);
    RunFFTCore1(fft_out, fft_len);
    while (h < fft_len) {
        RunFFTCore2((S16Q15_A2)fft_out, fft_len, h, gap, g_LutCos);
        gap >>= 1;
        h <<= 1;
    }
}
