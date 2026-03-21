/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef __MMW_MICRO_CUBE_H__
#define __MMW_MICRO_CUBE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* Include Files */
#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_DBUS_ADDR          0x1C000000
#define MICRO_CUBE_ADDR          PSRAM_DBUS_ADDR

#if (CONFIG_SOC_MULTI_CORE)
#define MICRO_CUBE_STORE_EXT_NUM     2   /* ping/pong */
#else
#define MICRO_CUBE_STORE_EXT_NUM     0
#endif

typedef uint32_t  Noisetype;

int  micro_cube_data_store(void);
bool micro_cube_ready(void);
complex16_cube *micro_cube_chirps_get(uint32_t range_idx);
Noisetype *micro_cube_noise_get(uint32_t range_idx);

void micro_cube_reset(void);
int  micro_cube_init(uint16_t range_start, uint16_t range_end, uint16_t chirp_num, uint16_t ant_num);
void micro_cube_deinit(void);

int micro_cube_range_set(uint16_t range_start, uint16_t range_end);
int micro_cube_range_get(uint16_t *range_start, uint16_t *range_end);
uint16_t micro_cube_chirp_num_get(void);

void micro_cube_auto_gain_set(uint8_t en);

#ifdef __cplusplus
}
#endif

#endif /* __MMW_MICRO_CUBE_H__ */
