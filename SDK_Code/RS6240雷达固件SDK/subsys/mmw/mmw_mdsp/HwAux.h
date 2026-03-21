/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef HW_AUX_H
#define HW_AUX_H

#include <stdint.h>
#include "mdsp_driver.h"

#define MINI_DSP_EN                         (1)

#define REG_MDSP_CMULT16_MODE               MDSP_COMPLEX16_MULTI_MODE_IN
#define REG_MDSP_CMULT16_IN_X               MDSP_COMPLEX16_MULTI_X_IN
#define REG_MDSP_CMULT16_IN_Y               MDSP_COMPLEX16_MULTI_Y_IN
#define REG_MDSP_CMULT16_IN_Z               MDSP_COMPLEX16_MULTI_Z_IN
#define REG_MDSP_CMULT16_OUT0               ((volatile uint32_t*)(MDSP_BASE_ADDR + 16))
#define REG_MDSP_CMULT16_OUT1               ((volatile uint32_t*)(MDSP_BASE_ADDR + 20))
#define REG_MDSP_CMULT16_OUT2               ((volatile uint32_t*)(MDSP_BASE_ADDR + 24))
#define REG_MDSP_CMULT16_OUT3               MDSP_COMPLEX16_MULTI_BUTTERFLY_1_OUT
#define REG_MDSP_CMULT16_OUT4               MDSP_COMPLEX16_MULTI_BUTTERFLY_2_OUT
#define REG_MDSP_CMULT16_OUT5               MDSP_COMPLEX16_MULTI_BUTTERFLY_3_OUT

#define REG_MDSP_RCUM4_IN1                  ((volatile uint32_t*)(MDSP_BASE_ADDR + 80))
#define REG_MDSP_RCUM4_IN2                  ((volatile uint32_t*)(MDSP_BASE_ADDR + 84))
#define REG_MDSP_RCUM4_IN3                  ((volatile uint32_t*)(MDSP_BASE_ADDR + 88))
#define REG_MDSP_RCUM4_IN4                  ((volatile uint32_t*)(MDSP_BASE_ADDR + 92))
#define REG_MDSP_RCUM4_OUT                  ((volatile uint32_t*)(MDSP_BASE_ADDR + 96))

#define REG_MDSP_BITINV_LEN                 MDSP_BIT_INV_B_IN
#define REG_MDSP_BITINV_DATA                MDSP_BIT_INV_A_IN
#define REG_MDSP_BITINV_OUT                 MDSP_BIT_INV_OUT

#define REG_MDSP_SIMDIV2ROUND_MODE          ((volatile uint32_t*)(MDSP_BASE_ADDR + 136))
#define REG_MDSP_SIMDIV2ROUND_IN1           MDSP_COMPLEX_ADDER_COMPLEX32_X_IN
#define REG_MDSP_SIMDIV2ROUND_IN2           MDSP_COMPLEX_ADDER_COMPLEX32_Y_IN
#define REG_MDSP_SIMDIV2ROUND_SUM           MDSP_COMPLEX_ADDER_COMPLEX32_ADD_HALF_OUT
#define REG_MDSP_SIMDIV2ROUND_SUB           MDSP_COMPLEX_ADDER_COMPLEX32_MINUS_HALF_OUT

#define REG_MDSP_MAP_FFT_BUFFER_CTRL        ((volatile uint32_t*)(MDSP_BASE_ADDR + 164))
#define REG_MDSP_MAP_FFT_BUFFER_IN          ((volatile uint32_t*)(MDSP_BASE_ADDR + 168))
#define REG_MDSP_MAP_FFT_BUFFER_OUT0        ((volatile uint32_t*)(MDSP_BASE_ADDR + 172))
#define REG_MDSP_MAP_FFT_BUFFER_OUT1        ((volatile uint32_t*)(MDSP_BASE_ADDR + 176))
#define REG_MDSP_MAP_FFT_BUFFER_OUT2        ((volatile uint32_t*)(MDSP_BASE_ADDR + 180))
#define REG_MDSP_MAP_FFT_BUFFER_OUT3        ((volatile uint32_t*)(MDSP_BASE_ADDR + 184))

#define REG_MDSP_ABS_IN                     ((volatile uint32_t*)(MDSP_BASE_ADDR + 124))
#define REG_MDSP_ABS_OUT                    ((volatile uint32_t*)(MDSP_BASE_ADDR + 128))
#define REG_MDSP_ABS_DB_OUT                 ((volatile uint32_t*)(MDSP_BASE_ADDR + 132))

#ifndef WRITE_REG
#define WRITE_REG(reg, data)                *(volatile uint32_t*)(reg) = (data)
#endif
#ifndef READ_REG
#define READ_REG(reg)                       (*(volatile uint32_t*)(reg))
#endif

//inline void Complex16Mul(const S16Q15 x[2], const S16Q15 y[2], const S16Q15 z[2], uint8_t mode, S16Q15* addr7B16, S16Q15* addr8B16, S16Q15* addr9B16)
#define Complex16MulPath1(x, y, z, addr7B16, addr8B16)  \
{           \
    WRITE_REG(REG_MDSP_CMULT16_IN_X, *(const uint32_t*)(x));    \
    WRITE_REG(REG_MDSP_CMULT16_IN_Y, *(const uint32_t*)(y));    \
    WRITE_REG(REG_MDSP_CMULT16_IN_Z, *(const uint32_t*)(z));    \
    *(uint32_t*)(addr7B16) = READ_REG(REG_MDSP_CMULT16_OUT3);   \
    *(uint32_t*)(addr8B16) = READ_REG(REG_MDSP_CMULT16_OUT4);   \
}

#define Complex16MulPath2(x, y, z, addr9B16)	\
{           \
    WRITE_REG(REG_MDSP_CMULT16_IN_X, *(const uint32_t*)(x));\
    WRITE_REG(REG_MDSP_CMULT16_IN_Y, *(const uint32_t*)(y));\
    WRITE_REG(REG_MDSP_CMULT16_IN_Z, *(const uint32_t*)(z));\
    *(uint32_t*)(addr9B16) = READ_REG(REG_MDSP_CMULT16_OUT5);   \
}

#define SumDiv2Hw(x, y, sum, sub)                                   \
{                                                                   \
    WRITE_REG(REG_MDSP_SIMDIV2ROUND_IN1, *(const uint32_t*)(x));        \
    WRITE_REG(REG_MDSP_SIMDIV2ROUND_IN2, *(const uint32_t*)(y));        \
    *(uint32_t*)(sum) = READ_REG(REG_MDSP_SIMDIV2ROUND_SUM);                    \
    *(uint32_t*)(sub) = READ_REG(REG_MDSP_SIMDIV2ROUND_SUB);                    \
}

#define MapFFTBuffer(idx, mapped_idx_0, mapped_idx_1, mapped_idx_2, mapped_idx_3)   \
{                                                                               \
    WRITE_REG(REG_MDSP_MAP_FFT_BUFFER_IN, (idx));                               \
    (mapped_idx_0) = READ_REG(REG_MDSP_MAP_FFT_BUFFER_OUT0);                    \
    (mapped_idx_1) = READ_REG(REG_MDSP_MAP_FFT_BUFFER_OUT1);                    \
    (mapped_idx_2) = READ_REG(REG_MDSP_MAP_FFT_BUFFER_OUT2);                    \
    (mapped_idx_3) = READ_REG(REG_MDSP_MAP_FFT_BUFFER_OUT3);                    \
}

#define RCUM4(a,b,c,d)  \
{                           \
    WRITE_REG(REG_MDSP_RCUM4_IN1, a);       \
    WRITE_REG(REG_MDSP_RCUM4_IN2, b);       \
    WRITE_REG(REG_MDSP_RCUM4_IN3, c);       \
    WRITE_REG(REG_MDSP_RCUM4_IN4, d);       \
}

#define MDSP_ABS(complex_in, abs_out)               \
{                                                   \
    WRITE_REG(REG_MDSP_ABS_IN, complex_in);         \
    abs_out = READ_REG(REG_MDSP_ABS_OUT);           \
}

#define MDSP_DB(complex_in, log_out)                \
{                                                   \
    WRITE_REG(REG_MDSP_ABS_IN, complex_in);         \
    asm("nop");\
    asm("nop");\
    log_out = READ_REG(REG_MDSP_ABS_DB_OUT);        \
}

#if !MINI_DSP_EN
void Complex16Mul(const S16Q15 x[2], const S16Q15 y[2], const S16Q15 z[2],
                  uint8_t mode, S16Q15* addr7B16, S16Q15* addr8B16, S16Q15* addr9B16);
void SumDiv2RoundS16Q15Complex(const S16Q15* adder_1, const S16Q15* adder_2, uint8_t add_mode, S16Q15* result);
#endif
uint32_t BitRev(uint16_t data, uint8_t valid_len);

#endif
