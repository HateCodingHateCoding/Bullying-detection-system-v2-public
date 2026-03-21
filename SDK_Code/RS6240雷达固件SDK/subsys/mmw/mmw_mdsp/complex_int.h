/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef __COMPLEX_INT_H__
#define __COMPLEX_INT_H__

#if (CONFIG_SOC_CPU_F)
#include "csi_const_structs.h"
#include "csi_math.h"
#endif
#include "mdsp_driver.h"

#if (CONFIG_SOC_CPUS)
#define  MDSP_COMPLEX_CALC_ENABLE             1
#else //CPUF
#define  MDSP_COMPLEX_CALC_ENABLE             0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define complex16_abs(real, imag)  fast_cabs_int16(real, imag)
__STATIC_INLINE uint32_t complex16_power(int16_t real, int16_t imag)
{
    return ((int32_t)real * real) + ((int32_t)imag * imag);
}
__STATIC_INLINE uint32_t complex16reim_abs(complex16_imre *in)
{
    return complex16_abs(in->real, in->imag);
}
__STATIC_INLINE uint32_t complex16reim_power(complex16_imre *in)
{
    return complex16_power(in->real, in->imag);
}
#define  COMPLEX16_ABS(vec)       complex16reim_abs((complex16_imre *)(vec))
#define  COMPLEX16_POWER(vec)     complex16reim_power((complex16_imre *)(vec))

__STATIC_INLINE void complex16imre_mul16(const complex16_imre *in0, const complex16_imre *in1, complex16_imre *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = (real>>16);
    out->imag = (imag>>16);
}

__STATIC_INLINE void complex16imre_conj_mul16(const complex16_imre *in0, const complex16_imre *in1, complex16_imre *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real>>16;
    out->imag = imag>>16;
}

__STATIC_INLINE void complex16imre_mul_unit(const complex16_imre *in0, const complex16_imre *unit, complex16_imre *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);

    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) - ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) + ((int32_t)in0->real * (int32_t)unit->imag);
        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16imre_conj_mul_unit(const complex16_imre *in0, const complex16_imre *unit, complex16_imre *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);

    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) + ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) - ((int32_t)in0->real * (int32_t)unit->imag);
        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16imre_mul32(const complex16_imre *in0, const complex16_imre *in1, complex32_imre *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
}

__STATIC_INLINE void complex16imre_conj_mul32(const complex16_imre *in0, const complex16_imre *in1, complex32_imre *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
}

__STATIC_INLINE void complex16reim_mul16(const complex16_reim *in0, const complex16_reim *in1, complex16_reim *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real>>16;
    out->imag = imag>>16;
}

__STATIC_INLINE void complex16reim_conj_mul16(const complex16_reim *in0, const complex16_reim *in1, complex16_reim *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real>>16;
    out->imag = imag>>16;
}

__STATIC_INLINE void complex16reim_mul_unit(const complex16_reim *in0, const complex16_reim *unit, complex16_reim *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);

    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) - ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) + ((int32_t)in0->real * (int32_t)unit->imag);
        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16reim_conj_mul_unit(const complex16_reim *in0, const complex16_reim *unit, complex16_reim *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);

    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) + ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) - ((int32_t)in0->real * (int32_t)unit->imag);
        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16reim_mul32(const complex16_reim *in0, const complex16_reim *in1, complex32_reim *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
}

__STATIC_INLINE void complex16reim_conj_mul32(const complex16_reim *in0, const complex16_reim *in1, complex32_reim *out)
{
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
}

__STATIC_INLINE void complex16mdsp_mul16(const complex16_mdsp *in0, const complex16_mdsp *in1, complex16_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_HALF_16BIT_RESULT(in0->value32, in1->value32, out->value32);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real>>16;
    out->imag = imag>>16;
#endif
}

__STATIC_INLINE void complex16mdsp_conj_mul16(const complex16_mdsp *in0, const complex16_mdsp *in1, complex16_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_HALF_16BIT_RESULT(in0->value32,
                    (in1->value32^0xFFFF0000) + 0x10000, out->value32);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real>>16;
    out->imag = imag>>16;
#endif
}


__STATIC_INLINE void complex16mdsp_mul_unit(const complex16_mdsp *in0, const complex16_mdsp *unit, complex16_mdsp *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);

    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) - ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) + ((int32_t)in0->real * (int32_t)unit->imag);

        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16mdsp_conj_mul_unit(const complex16_mdsp *in0, const complex16_mdsp *unit, complex16_mdsp *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);

    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) + ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) - ((int32_t)in0->real * (int32_t)unit->imag);

        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16mdsp_mul32(const complex16_mdsp *in0, const complex16_mdsp *in1, complex32_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(in0->value32, in1->value32, out->real, out->imag);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
#endif
}

__STATIC_INLINE void complex16mdsp_conj_mul32(const complex16_mdsp *in0, const complex16_mdsp *in1, complex32_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(in0->value32,
                    (in1->value32^0xFFFF0000) + 0x10000, out->real, out->imag);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
#endif
}

__STATIC_INLINE void complex16cube_mul_mdsp16(const complex16_cube *in0, const complex16_mdsp *in1, complex16_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    complex16_mdsp tmp;

    tmp.real = in0->real;
    tmp.imag = in0->imag;
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_HALF_16BIT_RESULT(tmp.value32, in1->value32, out->value32);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real>>16;
    out->imag = imag>>16;
#endif
}

__STATIC_INLINE void complex16cube_conj_mul_mdsp16(const complex16_cube *in0, const complex16_mdsp *in1, complex16_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    complex16_mdsp tmp;

    tmp.real = in0->real;
    tmp.imag = in0->imag;
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_HALF_16BIT_RESULT(tmp.value32,
                    (in1->value32^0xFFFF0000) + 0x10000, out->value32);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real>>16;
    out->imag = imag>>16;
#endif
}


__STATIC_INLINE void complex16cube_mul_mdsp_unit(const complex16_cube *in0, const complex16_mdsp *unit, complex16_mdsp *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);

    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) - ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) + ((int32_t)in0->real * (int32_t)unit->imag);

        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16cube_conj_mul_mdsp_unit(const complex16_cube *in0, const complex16_mdsp *unit, complex16_mdsp *out)
{
    uint32_t unit_abs = complex16_abs(unit->real, unit->imag);
    if (unit_abs) {
        int32_t real = ((int32_t)in0->real * (int32_t)unit->real) + ((int32_t)in0->imag * (int32_t)unit->imag);
        int32_t imag = ((int32_t)in0->imag * (int32_t)unit->real) - ((int32_t)in0->real * (int32_t)unit->imag);

        out->real = (real/unit_abs);
        out->imag = (imag/unit_abs);
    }
}

__STATIC_INLINE void complex16cube_mul_mdsp32(const complex16_cube *in0, const complex16_mdsp *in1, complex32_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    complex16_mdsp tmp;

    tmp.real = in0->real;
    tmp.imag = in0->imag;
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(tmp.value32, in1->value32, out->real, out->imag);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) - ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) + ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
#endif
}

__STATIC_INLINE void complex16cube_conj_mul_mdsp32(const complex16_cube *in0, const complex16_mdsp *in1, complex32_mdsp *out)
{
#if (MDSP_COMPLEX_CALC_ENABLE)
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(in0->value32,
                    (in1->value32^0xFFFF0000) + 0x10000, out->real, out->imag);
#else
    int32_t real = ((int32_t)in0->real * (int32_t)in1->real) + ((int32_t)in0->imag * (int32_t)in1->imag);
    int32_t imag = ((int32_t)in0->imag * (int32_t)in1->real) - ((int32_t)in0->real * (int32_t)in1->imag);

    out->real = real;
    out->imag = imag;
#endif
}

__STATIC_INLINE void vector_add_q7(int8_t *out, int8_t *in1, int8_t *in2, uint32_t block_num)
{
#if (CONFIG_SOC_CPU_F)
    csi_add_q7(in1, in2, out, block_num);
#else
    for (int i = 0; i < block_num; ++i) {
        int32_t tmp = (int32_t)in1[i] + in2[i];
        out[i] = S_SAT(tmp, 8);
    }
#endif
}

__STATIC_INLINE void vector_add_q15(int16_t *out, int16_t *in1, int16_t *in2, uint32_t block_num)
{
#if (CONFIG_SOC_CPU_F)
    csi_add_q15(in1, in2, out, block_num);
#else
    for (int i = 0; i < block_num; ++i) {
        int32_t tmp = (int32_t)in1[i] + in2[i];

        out[i] = S_SAT(tmp, 16);
    }
#endif
}

__STATIC_INLINE void vector_add_q31(int32_t *out, int32_t *in1, int32_t *in2, uint32_t block_num)
{
#if (CONFIG_SOC_CPU_F)
    csi_add_q31(in1, in2, out, block_num);
#else
    for (int i = 0; i < block_num; ++i) {
        int64_t tmp = (int64_t)in1[i] + in2[i];

        out[i] = S_SAT(tmp, 32);
    }
#endif
}

__STATIC_INLINE void vector_sub_q7(int8_t *out, int8_t *in1, int8_t *in2, uint32_t block_num)
{
#if (CONFIG_SOC_CPU_F)
    csi_sub_q7(in1, in2, out, block_num);
#else
    for (int i = 0; i < block_num; ++i) {
        int32_t tmp = (int32_t)in1[i] - in2[i];

        out[i] = S_SAT(tmp, 8);
    }
#endif
}

__STATIC_INLINE void vector_sub_q15(int16_t *out, int16_t *in1, int16_t *in2, uint32_t block_num)
{
#if (CONFIG_SOC_CPU_F)
    csi_sub_q15(in1, in2, out, block_num);
#else
    for (int i = 0; i < block_num; ++i) {
        int32_t tmp = (int32_t)in1[i] - in2[i];

        out[i] = S_SAT(tmp, 16);
    }
#endif
}

__STATIC_INLINE void vector_sub_q31(int32_t *out, int32_t *in1, int32_t *in2, uint32_t block_num)
{
#if (CONFIG_SOC_CPU_F)
    csi_sub_q31(in1, in2, out, block_num);
#else
    for (int i = 0; i < block_num; ++i) {
        int64_t tmp = (int64_t)in1[i] - in2[i];

        out[i] = S_SAT(tmp, 32);
    }
#endif
}

__STATIC_INLINE void vector_scatter_add_q7(int8_t *out, int8_t *in1, int8_t *in2, uint32_t block_num, uint32_t scat_inv)
{
    for (int i = 0; i < block_num; ++i) {
        int32_t tmp = (int32_t)in1[i] + in2[i];

        *out = S_SAT(tmp, 8);
        out += scat_inv;
    }
}

__STATIC_INLINE void vector_scatter_add_q15(int16_t *out, int16_t *in1, int16_t *in2, uint32_t block_num, uint32_t scat_inv)
{
    for (int i = 0; i < block_num; ++i) {
        int32_t tmp = (int32_t)in1[i] + in2[i];

        *out = S_SAT(tmp, 16);
        out += scat_inv;
    }
}

__STATIC_INLINE void vector_scatter_add_q31(int32_t *out, int32_t *in1, int32_t *in2, uint32_t block_num, uint32_t scat_inv)
{
    for (int i = 0; i < block_num; ++i) {
        int64_t tmp = (int64_t)in1[i] + in2[i];

        *out =  S_SAT(tmp, 32);
        out += scat_inv;
    }
}

#ifdef __cplusplus
}
#endif
#endif //__COMPLEX_INT_H__
