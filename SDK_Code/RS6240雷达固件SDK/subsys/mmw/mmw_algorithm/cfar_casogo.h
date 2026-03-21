/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef __CFAR_CASOGO_H__
#define __CFAR_CASOGO_H__

/* Include Files */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN
#define MIN(x, y)                       ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y)                       ((x) > (y) ? (x) : (y))
#endif

#if (CONFIG_MMW_LIB_CFAR_DATA_20LOG10) //data unit=log(dB).

#if (CONFIG_MMW_LIB_CFAR_DATA_32BIT)
typedef int32_t  CFAR_DATA_TYPE;
#else
typedef int16_t  CFAR_DATA_TYPE;
#endif
typedef int32_t  CFAR_SUM_TYPE;

#define CFAR_POWER(data)                 (data)
#define CFAR_PEAK_THRESH(noise, snr)     ((noise) + (snr))
#define CFAR_MIN_NOISE_LIMIT(ns)         (MAX(ns, (-90*256)))
#define PASS_LINE_NONE                   ((int32_t)0x7FFFFFFF) //power line

#else // amplitude.

#if (CONFIG_MMW_LIB_CFAR_DATA_32BIT)
typedef uint32_t  CFAR_DATA_TYPE;
#else
typedef uint16_t  CFAR_DATA_TYPE;
#endif
typedef uint32_t  CFAR_SUM_TYPE;

#define CFAR_POWER(data)                 (data)
#define CFAR_PEAK_THRESH(noise, snr)     (((noise)*(snr))>>2);
#define CFAR_MIN_NOISE_LIMIT(ns)         (MAX(ns, 1))
#define PASS_LINE_NONE                   ((uint32_t)0xFFFFFFFF)  //power line
#endif

/**************************************************************************
 ************************* Export Functions  ******************************
 **************************************************************************/
uint32_t cfar_ca(CFAR_DATA_TYPE *data_in, uint32_t data_num, uint16_t *out,
                uint32_t snr_q2, uint32_t noiseLen_log2, uint32_t guard_len, CFAR_SUM_TYPE pass_line);
uint32_t cfar_go(CFAR_DATA_TYPE *data_in, uint32_t data_num, uint16_t *out,
                uint32_t snr_q2, uint32_t noiseLen_log2, uint32_t guard_len, CFAR_SUM_TYPE pass_line);
uint32_t cfar_so(CFAR_DATA_TYPE *data_in, uint32_t data_num, uint16_t *out,
                uint32_t snr_q2, uint32_t noiseLen_log2, uint32_t guard_len, CFAR_SUM_TYPE pass_line);
void show_cfar_debug_info(void);
#ifdef __cplusplus
}
#endif

#endif /* __CFAR_CASOGO_H__ */

