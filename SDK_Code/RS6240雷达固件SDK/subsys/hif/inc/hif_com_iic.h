/**
 **************************************************************************************************
 * @file    hif_com_iic.h
 * @brief   .
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _HIF_COM_IIC_H_
#define _HIF_COM_IIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */
/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

int hif_com_iic_init(uint32_t rate);

int hif_com_iic_deinit(void);

void hif_com_iic_wakeup(void);

int hif_com_iic_tx_start(void);

int hif_com_iic_write_end(void);

int hif_com_iic_read_end(void);

int hif_com_iic_write(uint8_t *pbuff, uint16_t size);

int hif_com_iic_read(uint8_t *pbuff, uint16_t size);

void hif_com_iic_param_set(uint8_t param);

uint8_t hif_com_iic_param_get(void);

#if (CONFIG_HIF_DEVICE_WAKEUP_IIC)

int hif_com_iic_pm_init(pm_wakeup_callback_t pm_cb, void *pcfg);

int hif_com_iic_pm_deinit(void);

#endif /* CONFIG_HIF_DEVICE_WAKEUP_IIC */



#ifdef __cplusplus
}
#endif

#endif /* _HIF_COM_IIC_H_ */

