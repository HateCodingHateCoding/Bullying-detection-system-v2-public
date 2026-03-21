/**
 **************************************************************************************************
 * @brief   mailbox hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_MBX_H
#define _PSIC_LL_MBX_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

#include "psic_types.h"
#include "hw_mbx.h"
#include "soc.h"


#define MBX0_DEV                        ((mbox_dev_t *)    MBX0_BASE_ADDR)  /*!< 0x4000 6000 */
#define MBX1_DEV                        ((mbox_dev_t *)    MBX1_BASE_ADDR)  /*!< 0x4000 6400 */

/** @addtogroup PSIC_LL_Driver
  * @{
  */


/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */

/** @defgroup mbox map for channel mask
  * @{
  */
#define LL_MBOX_CHANNEL_Msk(n)                        HW_BIT(n)
#define LL_MBOX_CHANNEL_ALL_Msk                       0xFFFFFFFF

/**
  * @}
  */

/** @defgroup mbox map for channel number
  * @{
  */
#define LL_MBOX_CHANNEL_NUM(n)                        (n)
#define LL_MBOX_CHAN_ID_MMW                           31
/**
  * @}
  */

/** @defgroup Mailbox max channel num
  * @{
  */
#define LL_MBOX_CONF_NUM        32
/**
  * @}
  */


/**
  * @}
  */


/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/**
  * @brief  set the corresponding interrupt for channel.
  * @param  mbox_dev_t : Mailbox device
  * @param  channel This parameter can be a combination of the following values:
  *         @arg @ref (0 <= channel < PSIC_MBOX_CONF_NUM)
  */
__HW_STATIC_INLINE
void LL_MBOX_SetMsgSignal(mbox_dev_t *mbox_dev, uint32_t channel_msk)
{
    HW_SET_MSK(mbox_dev->ISCR, channel_msk);
}

/**
  * @brief  clear the corresponding interrupt for channel.
  * @param  mbox_dev_t : Mailbox device
  * @param  channel This parameter can be a combination of the following values:
  *         @arg @ref (0 <= channel < PSIC_MBOX_CONF_NUM)
  */

__HW_STATIC_INLINE
void LL_MBOX_ClearMsgSignal(mbox_dev_t *mbox_dev, uint32_t channel_msk)
{
    HW_SET_MSK(mbox_dev->ICCR, channel_msk);
}

__HW_STATIC_INLINE
uint32_t LL_MBOX_GetPending_MsgSignal(mbox_dev_t *mbox_dev)
{
    return HW_GET_MSK_VAL(mbox_dev->IPR, IPR_Msk, IPR_Pos);
}

__HW_STATIC_INLINE
void LL_MBOX_ITEnable(mbox_dev_t *mbox_dev, uint32_t channel_msk)
{
      HW_SET_MSK(mbox_dev->IECR, channel_msk);
}

__HW_STATIC_INLINE
void LL_MBOX_ITDisable(mbox_dev_t *mbox_dev, uint32_t channel_msk)
{
    HW_SET_MSK(mbox_dev->IDCR, channel_msk);
}

__HW_STATIC_INLINE
uint32_t LL_MBOX_ITIsEnabled(mbox_dev_t *mbox_dev)
{
//    return HW_GET_MSK_VAL(mbox_dev->IMR,
//              IMR_Msk,
//              channel_num);
    return HW_GET_VAL(mbox_dev->IMR);
}

__HW_STATIC_INLINE
uint32_t LL_MBOX_GetITEnabled(mbox_dev_t *mbox_dev)
{
    return HW_GET_VAL(mbox_dev->IMR);
}

__HW_STATIC_INLINE
uint32_t LL_MBOX_GetPendingIT_MsgSignal(mbox_dev_t *mbox_dev)
{
    return HW_GET_MSK_VAL(mbox_dev->IPR, IPR_Msk, IPR_Pos) & HW_GET_MSK_VAL(mbox_dev->IMR, IMR_Msk, IMR_Pos);
}


/**
  * @}
  */


/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_MBX_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
