/**
 **************************************************************************************************
 * @file    hif_io.h
 * @brief   .
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _HIF_IO_H_
#define _HIF_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hif_com.h"
/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */
#define HIF_IO_TRIG_DIS					0
#define HIF_IO_TRIG_EDGE_RISING			0x20
#define HIF_IO_TRIG_EDGE_FALLING		0x21
#define HIF_IO_TRIG_EDGE_BOTH			0x22
#define HIF_IO_TRIG_LEVEL_LOW			0x10
#define HIF_IO_TRIG_LEVEL_HIGH			0x11

#define HIF_IO_PULL_NONE				0
#define HIF_IO_PULL_UP					1
#define HIF_IO_PULL_DW					2

typedef struct {
	uint8_t io_num;
	uint8_t pull_mode;
	uint8_t trig_type;
} hif_io_pm_cfg_t;

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */
int hif_wkio_hold_init(uint32_t irq_pins);

void hif_io_out_set(uint8_t io_num, uint8_t io_val);

int hif_io_in_value_get(void);

int hif_io_in_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg);

int hif_io_in_pm_deinit(void);


#ifdef __cplusplus
}
#endif

#endif /* _HIF_IO_H_ */

