/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

#include "mmw_type.h"
#include "mmw_math.h"

/* offset = 0.01, factor = 0.01, need to be optimized */
const static float g_log2_table[200] = {
    -6.6439, -5.6439, -5.0589, -4.6439, -4.3219, -4.0589, -3.8365, -3.6439, -3.4739, -3.3219, -3.1844, -3.0589, -2.9434, -2.8365, -2.7370, -2.6439, -2.5564, -2.4739, -2.3959, -2.3219, -2.2515, -2.1844, -2.1203, -2.0589, -2.0000, -1.9434, -1.8890, -1.8365, -1.7859, -1.7370, -1.6897, -1.6439, -1.5995, -1.5564, -1.5146, -1.4739, -1.4344, -1.3959, -1.3585, -1.3219,
    -1.2863, -1.2515, -1.2176, -1.1844, -1.1520, -1.1203, -1.0893, -1.0589, -1.0291, -1.0000, -0.9714, -0.9434, -0.9159, -0.8890, -0.8625, -0.8365, -0.8110, -0.7859, -0.7612, -0.7370, -0.7131, -0.6897, -0.6666, -0.6439, -0.6215, -0.5995, -0.5778, -0.5564, -0.5353, -0.5146, -0.4941, -0.4739, -0.4540, -0.4344, -0.4150, -0.3959, -0.3771, -0.3585, -0.3401, -0.3219,
    -0.3040, -0.2863, -0.2688, -0.2515, -0.2345, -0.2176, -0.2009, -0.1844, -0.1681, -0.1520, -0.1361, -0.1203, -0.1047, -0.0893, -0.0740, -0.0589, -0.0439, -0.0291, -0.0145, 0.0000, 0.0144, 0.0286, 0.0426, 0.0566, 0.0704, 0.0841, 0.0976, 0.1110, 0.1243, 0.1375, 0.1506, 0.1635, 0.1763, 0.1890, 0.2016, 0.2141, 0.2265, 0.2388, 0.2510, 0.2630,
    0.2750, 0.2869, 0.2987, 0.3103, 0.3219, 0.3334, 0.3448, 0.3561, 0.3674, 0.3785, 0.3896, 0.4005, 0.4114, 0.4222, 0.4330, 0.4436, 0.4542, 0.4647, 0.4751, 0.4854, 0.4957, 0.5059, 0.5160, 0.5261, 0.5361, 0.5460, 0.5558, 0.5656, 0.5753, 0.5850, 0.5945, 0.6041, 0.6135, 0.6229, 0.6323, 0.6415, 0.6508, 0.6599, 0.6690, 0.6781,
    0.6871, 0.6960, 0.7049, 0.7137, 0.7225, 0.7312, 0.7398, 0.7485, 0.7570, 0.7655, 0.7740, 0.7824, 0.7908, 0.7991, 0.8074, 0.8156, 0.8237, 0.8319, 0.8400, 0.8480, 0.8560, 0.8639, 0.8718, 0.8797, 0.8875, 0.8953, 0.9030, 0.9107, 0.9184, 0.9260, 0.9336, 0.9411, 0.9486, 0.9561, 0.9635, 0.9709, 0.9782, 0.9855, 0.9928, 1
};

static float fast_log(float val)
{
    uint8_t cnt = 0;
    int16_t idx = 0;

    while (val >= 2){
        cnt++;
        val = val / 2.0f;
    }
    idx = (int)(val / 0.01f) - 1;
    if (idx < 0) {
        idx = 0;
    }
    return (g_log2_table[idx] + cnt);
}

static double fast_sqrt(double data)
{
    double new_x, x;
    x = data;

    new_x = data + 1;
    while (ABS(new_x - x) > 1e-5) {
        new_x = x;
        x = (x + data / x) / 2.0;
    }
    return x;
}

#define LN2           0.693147180559945309417232121458176568
#define INV_LN2       1.442695040888963407359924681001892137
#define TAYLOR_TERMS  16
#define MAX_EXP       709.782712893384
double fast_exp(double x)
{
    if (x == 0.0) return 1.0;
    if (x > MAX_EXP) return DBL_MAX;
    if (x < -MAX_EXP) return 0.0;

    double n_dbl = x * INV_LN2;
    n_dbl = (x > 0.0) ? fast_floor(n_dbl + 0.5) : fast_ceil(n_dbl - 0.5);

    if (n_dbl > (double)(INT32_MAX/2)) {
        return (x > 0) ? DBL_MAX : 0.0;
    }
    if (n_dbl < (double)(INT32_MIN/2)) {
        return (x > 0) ? 0.0 : DBL_MAX;
    }

    int32_t n = (int32_t)n_dbl;
    double r = x - n_dbl * LN2;
    double taylor_sum = 1.0 + r;
    double term = r;
    for (int k = 2; k < TAYLOR_TERMS; ++k) {
        term *= r / k;
        taylor_sum += term;
        if (term < DBL_EPSILON && term > -DBL_EPSILON) break;
    }

    union {
        double d;
        int64_t i;
    } u;
    int64_t exponent = ((int64_t)n + 1023) << 52;
    u.i = exponent;
    double pow2n = u.d;
    double result = pow2n * taylor_sum;
    return result;
}

double fast_pow(double base, double exponent)
{
    if (base == 0.0) {
        return 0.0;
    }
    long   int_part = (long)(exponent);
    double frac_part = exponent - int_part;

    if (frac_part != 0.0) {
        if (base < 0.0) {
            return 0.0;
        }
        return fast_exp(exponent * fast_log(base));
    }

    if (int_part < 0) {
        int_part = -int_part;
        base = 1.0 / base;
    }
    double result = 1.0;
    while (int_part > 0) {
        if (int_part % 2 == 1) {
            result *= base;
        }
        base *= base;
        int_part /= 2;
    }
    return result;
}

uint32_t fast_cabs_int16(int16_t real, int16_t imag)
{
    uint32_t max, min, result;
    uint32_t real_abs = ABS(real);
    uint32_t imag_abs = ABS(imag);

    if (real_abs > imag_abs) {
        max = real_abs;
        min = imag_abs;
    } else {
        max = imag_abs;
        min = real_abs;
    }
    result = (31 * max + 9 * min);
    min    = (26 * max + 19 * min + 16);
    result = MAX(result, min)>>5;
    return MAX(max, result);
}

uint32_t fast_cabs_int32(int32_t real, int32_t imag)
{
    uint32_t max, min, result;
    uint32_t real_abs = ABS(real);
    uint32_t imag_abs = ABS(imag);

    if (real_abs > imag_abs) {
        max = real_abs;
        min = imag_abs;
    } else {
        max = imag_abs;
        min = real_abs;
    }
    result = (31 * (uint64_t)max + 9 * (uint64_t)min)>>5;
    min    = (26 * (uint64_t)max + 19 * (uint64_t)min + 16)>>5;
    result = MAX(result, min);
    return MAX(max, result);
}
