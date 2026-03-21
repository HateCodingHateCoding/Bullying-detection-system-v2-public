/*
 *  Copyright (c) 2022 Possumic <www.possumic.com>
 */

#ifndef __HIF_PM_H__
#define __HIF_PM_H__

/* Include Files */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* hif_pm_wup_callback_t)(uint32_t param);

void hif_pm_lock(void);

void hif_pm_unlock(void);


#ifdef __cplusplus
}
#endif

#endif //__HIF_PM_H__

