/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <stdint.h>
#include "fft_func.h"

#if MINI_DSP_EN
uint32_t BitRev(uint16_t data, uint8_t valid_len)
{
    WRITE_REG(REG_MDSP_BITINV_DATA, data);
    WRITE_REG(REG_MDSP_BITINV_LEN, valid_len);
    return READ_REG(REG_MDSP_BITINV_OUT);
}
#else
uint32_t BitRev(uint16_t data, uint8_t valid_len)
{
    uint32_t output = 0;
    uint8_t search_idx = 0;

    for (search_idx = 0; search_idx < valid_len; search_idx++) {
        output |= (data & 1) << (valid_len - search_idx - 1);
        data >>= 1;
    }
    return output;
}

void ModuleComplexMul(S16Q15 x[2], S16Q15 y[2], S32Q30 mul[2])
{
    S32Q30 k1, k2, k3;
    k1 = x[0] * (y[0] + y[1]);
    k2 = y[1] * (x[0] + x[1]);
    k3 = y[0] * (x[1] - x[0]);
    mul[0] = k1 - k2;
    mul[1] = k1 + k3;
}

void Complex16Mul(const S16Q15 x[2], const S16Q15 y[2], const S16Q15 z[2],
                     uint8_t mode, S16Q15* addr7B16, S16Q15* addr8B16, S16Q15* addr9B16)
{
    S16Q15 complex_val_1[2], complex_val_2[2], addr6B16[2];
    S32Q30 mul_val_1_val_2[2];
    S32Q30 tmp_sum[2] = { 0, 0 };

    if (mode == 0) {
        complex_val_1[0] = x[0];
        complex_val_1[1] = x[1];
        complex_val_2[0] = y[0];
        complex_val_2[1] = y[1];
    } else {
        complex_val_1[0] = x[1];
        complex_val_1[1] = y[0];
        complex_val_2[0] = z[0];
        complex_val_2[1] = z[1];
    }
    ModuleComplexMul(complex_val_1, complex_val_2, mul_val_1_val_2);

    addr6B16[0] = (S16Q15)(mul_val_1_val_2[0] >> 16) + ((mul_val_1_val_2[0] >> 15) & 0x01);
    addr6B16[1] = (S16Q15)(mul_val_1_val_2[1] >> 16) + ((mul_val_1_val_2[1] >> 15) & 0x01);

    if (addr7B16) {
        addr7B16[0] = ((S32Q30)z[0] >> 1) + (z[0] & 0x01) + addr6B16[0];
        addr7B16[1] = ((S32Q30)z[1] >> 1) + (z[1] & 0x01) + addr6B16[1];
    }

    if (addr8B16) {
        addr8B16[0] = ((S32Q30)z[0] >> 1) + (z[0] & 0x01) - addr6B16[0];
        addr8B16[1] = ((S32Q30)z[1] >> 1) + (z[1] & 0x01) - addr6B16[1];
    }

    if (addr9B16) {
        addr9B16[0] = ((S32Q30)x[0] >> 1) + (x[0] & 0x01) + addr6B16[0];
        addr9B16[1] = ((S32Q30)y[1] >> 1) + (y[1] & 0x01) - addr6B16[1];
    }
}
void SumDiv2RoundS16Q15Complex(const S16Q15* adder_1, const S16Q15* adder_2, uint8_t add_mode, S16Q15* result)
{
    uint8_t idx = 0;
    int32_t sum = 0;

    for (idx = 0; idx < 2; idx++) {
        if (add_mode) {
            sum = adder_1[idx] + adder_2[idx];
        } else {
            sum = adder_1[idx] - adder_2[idx];
        }
        sum = (sum >> 1) + (sum & 0x01);
        result[idx] = (S16Q15)sum;
    }
}
#endif
