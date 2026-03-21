/**
 *   @file  mmw_math.h
 *
 *   @brief
 *      Header file of the Millimeter Wave Detection of 3D objects.
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2022 Possumic, Inc.
 *
 */
#ifndef MMW_MATH_H
#define MMW_MATH_H

/* Include LL Files */
#include "types.h"

/* Include Files */
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

float  fast_sin(float angle_deg);
float  fast_cos(float angle_deg);
double fast_atan2(double y, double x);
double fast_ceil(double x);
double fast_floor(double x);
double fast_exp(double x);
double fast_pow(double base, double exp);
uint32_t fast_cabs_int16(int16_t real, int16_t imag);
uint32_t fast_cabs_int32(int32_t real, int32_t imag);
/* for vector calculation */
void complex16_logdB(uint16_t *dst, uint32_t *src, uint32_t num, uint32_t src_step);
void complex16_abs16(uint16_t *dst, uint32_t *src, uint32_t num, uint32_t src_step);
void complex16_abs32(uint32_t *dst, uint32_t *src, uint32_t num, uint32_t src_step);

#ifdef __cplusplus
}
#endif

#endif /* MMW_MATH_H */
