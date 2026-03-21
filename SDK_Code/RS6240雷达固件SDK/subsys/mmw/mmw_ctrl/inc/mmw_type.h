/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef __MMW_TYPE_H__
#define __MMW_TYPE_H__

/* Include Files */
#include <stdbool.h>
#include "types.h"
#include "hal_types.h"
#include "mmw_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN
#define MIN(x, y)                   ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y)                   ((x) > (y) ? (x) : (y))
#endif

#ifndef BIT
#define BIT(n)                      (1U<<(n))
#endif

#ifndef ABS
#define ABS(v)                      ((v)<0 ? -(v) : (v))
#endif

#ifndef contain_off
#define contain_off(t, m)           ((uint32_t)(&((t *)0)->m))
#endif

#ifndef contain_mb
#define contain_mb(t, m)           ((uint32_t)((&((t *)0)->m) + 1))
#endif

#ifndef wrap32_before
#define wrap32_before(a, b)           ((int32_t)((a) - (b)) < 0)
#define wrap32_after(a, b)            wrap32_before(b, a)
#define wrap32_diff(a, b)             ((int32_t)((a) - (b)))
#endif

#ifndef wrap16_before
#define wrap16_before(a, b)           ((int16_t)((a) - (b)) < 0)
#define wrap16_after(a, b)            wrap16_before(b, a)
#define wrap16_diff(a, b)             ((int16_t)((a) - (b)))
#endif

#ifndef time_diff
#define time_diff(a, b)                wrap32_diff(a, b)
#define time_before(a, b)              wrap32_before(a, b)
#define time_after(a, b)               wrap32_after(a, b)
#endif

#ifndef ROUNDUP4
#define ROUNDUP4(v)         (((v) + 3) & ~0x3)
#endif

#ifndef IS_POW2
#define IS_POW2(n)          (!((n) & ((n) - 1)))
#endif

#ifndef RDQN
#define RDQN(v, mbit)       (((v) + (1<<(mbit-1)))>>(mbit))
#endif

#ifndef FRACQN
#define FRACQN(v, mbit)     (RDQN(v, mbit)<<(mbit))
#endif

#ifndef S_SAT
#define S_SAT(v, nbit)      ((v) > ((1U<<(nbit-1))-1) ? ((1U<<(nbit-1))-1) : \
                            ((v) < (-1<<(nbit-1)) ? (-1<<(nbit-1)) : (v)))
#define U_SAT(v, nbit)      ((v)> ((1<<(nbit))-1) ? ((1<<(nbit))-1) : (v))
#endif

#define CLR_MSK(word, msk)                       ((word) &= (~((uint32_t)(msk))))
#define CLR_BIT(word, num)                       ((word) &= (~(uint32_t)(1U << (num))))
#define TEST_BIT(word, bit)                      ((word) & (1 << (bit)))
#define SET_BIT(word, num)                       ((word) |= (uint32_t)(1U << (num)))

typedef union _complex16_imre_u {
    struct {
        int16_t imag;
        int16_t real;
    };
    uint32_t value32;
} complex16_imre;

typedef struct _complex32_imre_t {
    int32_t imag;
    int32_t real;
} complex32_imre;

typedef union _complex16_reim_u {
    struct {
        int16_t real;
        int16_t imag;
    };
    uint32_t value32;
} complex16_reim;

typedef struct _complex32_reim_t {
    int32_t real;
    int32_t imag;
} complex32_reim;

/**
 * @brief A struct type of 16bit complex data.
 */
typedef union _complex16_cube_u {
    struct {
        int16_t imag;
        int16_t real;
    };
    uint32_t value32;
} complex16_cube;
typedef complex16_cube Complex16_RealImag;

/**
 * @brief Polar coordinate system point cloud structure.
 */
typedef struct {
    int16_t range;	   /**< 1LSB = 1cm */
    int16_t azim;	   /**< azimuth 1LSB = 0.01 degree */
    int16_t elev;	   /**< elevation 1LSB = 0.01 degree */
    int16_t vel;	   /**< velocity 1LSB = 1cm/s */
    int16_t snr;	   /**< 1LSB = 0.01dB */
    int16_t track_uid;	/**< 1LSB = 0.01dB */
} PointCloud_Polar;

/**
 * @brief Cartesian coordinate system point cloud structure.
 */
typedef struct {
    int16_t x;		   /**< 1LSB = 1cm */
    int16_t y;		   /**< 1LSB = 1cm */
    int16_t z;		   /**< 1LSB = 1cm */
    int16_t vel;	   /**< velocity 1LSB = 1cm/s */
    int16_t snr;	   /**< 1LSB = 0.01dB */
    int16_t track_uid;	/**< 1LSB = 0.01dB */
} PointCloud_Cart;

/**
 * @brief Union type of coordinate system point cloud.
 */
typedef union {
    PointCloud_Polar polar;
    PointCloud_Cart  cart;
} PointCloud3D;

/**
 * @brief Cartesian coordinate system tracking object structure.
 */
typedef struct {
    uint16_t track_uid;
    uint8_t  track_state;
    uint8_t  points_num;
    /* 1LSB=1cm of range, or 1LSB=0.01 degree of angle.*/
    int16_t  x;
    int16_t  y;
    int16_t  z;
    /* vx, vy, vz: 1LSB=1cm/s */
    int16_t  vx;
    int16_t  vy;
    int16_t  vz;
} TrackObjectElm;

#ifdef __cplusplus
}
#endif
#endif /* __MMW_TYPE_H__ */
