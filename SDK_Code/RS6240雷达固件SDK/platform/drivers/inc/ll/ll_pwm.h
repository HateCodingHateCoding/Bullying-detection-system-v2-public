/**
 **************************************************************************************************
 * @file    psic_ll_pwm.h
 * @brief   PWM hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_PWM_H
#define _PSIC_LL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "soc.h"
#include "psic_types.h"
#include "hw_pwm.h"

#define PWM_DEV                         ((pwm_dev_t *)    PWM_BASE_ADDR) /*!< 0x4001 E800 */

/** @addtogroup PSIC_LL_Driver
  * @{
  */


/** @defgroup PWM_LL
  * @{
  */


/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
#define PWM_CH_MASK                     0xF
#define PWM_MAX_CH                      0x7
#define PWM_CHANNEL_NUM                 0x8
#define PWM_CHANNEL_GRP_NUM             0x4

#define PMW_FEAT_NORMAL                 0x1
#define PWM_FEAT_CAPTURE                0x2
#define PWM_FEAT_TIMER                  0x3

#define PWM_CH_CAPT_MASK                0xE
#define PMW_CAPT_TMR_MODE				0x0
#define PMW_CAPT_CTR_MODE				0x1

#define PWM_CAPT_POS_EVT_MODE			0x0
#define PWM_CAPT_NEG_EVT_MODE			0x1
#define PWM_CAPT_BOTH_EVT_MODE			0x3

/** @defgroup PWM_DIV define
  * @{
  */
#define PWM_INT_DIV_1                   0x0
#define PWM_INT_DIV_2                   0x8
#define PWM_INT_DIV_4                   0x9
#define PWM_INT_DIV_8                   0xA
#define PWM_INT_DIV_16                  0xB
#define PWM_INT_DIV_32                  0xC
#define PWM_INT_DIV_64                  0xD
#define PWM_INT_DIV_128                 0xE
/** @defgroup PWM_DIV
  * @{
  */

/** @defgroup PWM Polarity
  * @{
  */
#define PWM_POLARITY_LOW                 (0x0U)
#define PWM_POLARITY_HIGH                (0x1U)

/**
  * @}
  */

/** @defgroup PWM_interrupt Mode
  * @{
  */
#define PWM_INTR_MODE_ZERO            	0x1
#define PWM_INTR_MODE_LOAD              0x2
#define PWM_INTR_MODE_CMP_UP            0x4
#define PWM_INTR_MODE_CMP_DW            0x8

#define PWM_INTR_MODE_MSK				0xF

#define PWM_INTR_MODE_ZERO_Pos        	0x0
#define PWM_INTR_MODE_LOAD_Pos          0x1
#define PWM_INTR_MODE_CMP_UP_Pos        0x2
#define PWM_INTR_MODE_CMP_DW_Pos        0x3

/**
  * @}
  */

/** @defgroup PWM_UPDATE_MODE
  * @{
  */
#define PWM_UPDATE_MODE_ZERO			0x0
#define PWM_UPDATE_MODE_LOAD			0x1
#define PWM_UPDATE_MODE_ZERO_LOAD		0x2
#define PWM_UPDATE_MODE_NONE			0x3
/**
  * @}
  */

/** @defgroup PWM_CUNT_MODE
  * @{
  */
#define PWM_CUNT_MODE_UP				0x0
#define PWM_CUNT_MODE_UP_DM				0x1
/**
  * @}
  */

/** @defgroup CAPTURE_EVENT_MODE
  * @{
  */
#define CAPT_EVT_EDGE_POS				0x0
#define CAPT_EVT_EDGE_NEG				0x1
#define CAPT_EVT_EDGE_BOTH				0x3
/**
  * @}
  */

/** @defgroup CAPTURE_INTR_MODE
  * @{
  */
#define CAPT_INTR_MODE_TIME				0x1
#define CAPT_INTR_MODE_CUNT				0x2

#define CAPT_INTR_MODE_TIME_POS 		0x0
#define CAPT_INTR_MODE_CUNT_POS			0x1

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/** @defgroup PWM and cpature function
  * @{
  */

__HW_STATIC_INLINE
void LL_PWM_Prescaler_Enable(void)
{
	HW_SET_BIT(PWM_DEV->PWMCFG, CNTDIVEN_Pos);
}

__HW_STATIC_INLINE
void LL_PWM_Prescaler_Disable(void)
{
	HW_CLR_BIT(PWM_DEV->PWMCFG, CNTDIVEN_Pos);
}

__HW_STATIC_INLINE
void LL_PWM_Set_Prescaler(uint32_t div)
{
	HW_SET_MSK_VAL(PWM_DEV->PWMCFG, CNTDIV_Msk, CNTDIV_Pos, div);
}

__HW_STATIC_INLINE
uint32_t LL_PWM_Get_Prescaler(void)
{
	return HW_GET_MSK_VAL(PWM_DEV->PWMCFG, CNTDIV_Msk, CNTDIV_Pos);
}

/** @defgroup PWM
  * @{
  */
__HW_STATIC_INLINE
void LL_PWM_Enable(uint32_t channel)
{
	HW_SET_BIT(PWM_DEV->PWMCFG, channel);
}

__HW_STATIC_INLINE
void LL_PWM_Disable(uint32_t channel)
{
	HW_CLR_BIT(PWM_DEV->PWMCFG, channel);
}

__HW_STATIC_INLINE
uint32_t LL_PWM_IsEnabled(uint32_t channel)
{
    return HW_TEST_BIT(PWM_DEV->PWMCFG, channel);
}

__HW_STATIC_INLINE
void LL_PWM_Reset(void)
{
	PWM_DEV->PWMINTEN[0] = 0;
	PWM_DEV->PWMINTEN[1] = 0;
	PWM_DEV->PWMIC[0] = 0x3F3F3F00;
	PWM_DEV->PWMIC[1] = 0x3F3F3F;
	PWM_DEV->PWMCTL = 0;
	PWM_DEV->PWMLOADL[0] = 0;
	PWM_DEV->PWMLOADL[1] = 0;
	PWM_DEV->PWMCMPL[0] = 0;
	PWM_DEV->PWMCMPL[1] = 0;
	PWM_DEV->PWMCMPL[2] = 0;
	PWM_DEV->PWMCMPL[3] = 0;
}

__HW_STATIC_INLINE
void LL_PWM_Intr_clearAll(void)
{
	PWM_DEV->PWMIC[0] = 0xFFFFFFFF;
	PWM_DEV->PWMIC[1] = 0x3F3F3F;
}

__HW_STATIC_INLINE
void LL_PWM_EnableInverted(uint32_t channel)
{
	HW_SET_BIT(PWM_DEV->PWMINVERTTRIG, channel);
}

__HW_STATIC_INLINE
void LL_PWM_DisableInverted(uint32_t channel)
{
	HW_CLR_BIT(PWM_DEV->PWMINVERTTRIG, channel);
}

__HW_STATIC_INLINE
uint32_t LL_PWM_IsEnabledInverted(uint32_t channel)
{
    return HW_TEST_BIT(PWM_DEV->PWMINVERTTRIG, channel);
}

__HW_STATIC_INLINE
void LL_PWM_EnableIntr(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);
	uint32_t group_mak = (group + 1) << 3;
	uint32_t channel_mak = channel & 0x1;

	if (intr_mode & PWM_INTR_MODE_ZERO) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMINTEN[0],  group_mak);
		} else {
			HW_SET_BIT(PWM_DEV->PWMINTEN[1],  8);
		}
	}

	if (intr_mode & PWM_INTR_MODE_LOAD) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMINTEN[0],  group_mak + 1);
		} else {
			HW_SET_BIT(PWM_DEV->PWMINTEN[1],  9);
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_UP) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMINTEN[0], group_mak + 2 + channel_mak);
		} else {
			HW_SET_BIT(PWM_DEV->PWMINTEN[1], 10 + channel_mak);
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_DW) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMINTEN[0], group_mak + 4 + channel_mak);
		} else {
			HW_SET_BIT(PWM_DEV->PWMINTEN[1], 12);
		}
	}
}

__HW_STATIC_INLINE
void LL_PWM_DisableIntr(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);
	uint32_t group_mak = (group + 1) << 3;
	uint32_t channel_mak = channel & 0x1;

	if (intr_mode & PWM_INTR_MODE_ZERO) {
		if (group < 3) {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[0],  group_mak);
		} else {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[1],  8);
		}
	}

	if (intr_mode & PWM_INTR_MODE_LOAD) {
		if (group < 3) {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[0],  group_mak + 1);
		} else {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[1],  9);
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_UP) {
		if (group < 3) {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[0], group_mak + 2 + channel_mak);
		} else {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[1], 10 + channel_mak);
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_DW) {
		if (group < 3) {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[0], group_mak + 4 + channel_mak);
		} else {
			HW_CLR_BIT(PWM_DEV->PWMINTEN[1], 12);
		}
	}
}

__HW_STATIC_INLINE
uint32_t LL_PWM_IsEnabledIntr(uint32_t channel, uint32_t intr_mode)
{
	uint32_t intr_msk = 0;
	uint32_t group = (channel >> 1);
	uint32_t group_mak = (group + 1) << 3;
	uint32_t channel_mak = channel & 0x1;

	if (intr_mode & PWM_INTR_MODE_ZERO) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[0],  group_mak) << PWM_INTR_MODE_ZERO_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[1],  8) << PWM_INTR_MODE_ZERO_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_LOAD) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[0],  group_mak + 1) << PWM_INTR_MODE_LOAD_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[1],  9) << PWM_INTR_MODE_LOAD_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_UP) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[0], group_mak + 2 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[1], 10 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_DW) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[0], group_mak + 4 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMINTEN[1], 12) << PWM_INTR_MODE_CMP_UP_Pos;
		}
	}

	return intr_msk;
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetIntrStatus(uint32_t channel, uint32_t intr_mode)
{
	uint32_t intr_msk = 0;
	uint32_t group = (channel >> 1);
	uint32_t group_mak = (group + 1) << 3;
	uint32_t channel_mak = channel & 0x1;

	if (intr_mode & PWM_INTR_MODE_ZERO) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[0],  group_mak) << PWM_INTR_MODE_ZERO_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[1],  8) << PWM_INTR_MODE_ZERO_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_LOAD) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[0],  group_mak + 1) << PWM_INTR_MODE_LOAD_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[1],  9) << PWM_INTR_MODE_LOAD_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_UP) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[0], group_mak + 2 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[1], 10 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_DW) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[0], group_mak + 4 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMRIS[1], 12) << PWM_INTR_MODE_CMP_UP_Pos;
		}
	}

	return intr_msk;
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetIntrFlag(uint32_t channel, uint32_t intr_mode)
{
	uint32_t intr_msk = 0;
	uint32_t group = (channel >> 1);
	uint32_t group_mak = (group + 1) << 3;
	uint32_t channel_mak = channel & 0x1;

	if (intr_mode & PWM_INTR_MODE_ZERO) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[0],  group_mak) << PWM_INTR_MODE_ZERO_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[1],  8) << PWM_INTR_MODE_ZERO_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_LOAD) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[0],  group_mak + 1) << PWM_INTR_MODE_LOAD_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[1],  9) << PWM_INTR_MODE_LOAD_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_UP) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[0], group_mak + 2 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[1], 10 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_DW) {
		if (group < 3) {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[0], group_mak + 4 + channel_mak) << PWM_INTR_MODE_CMP_UP_Pos;
		} else {
			intr_msk |= HW_TEST_BIT(PWM_DEV->PWMIS[1], 12) << PWM_INTR_MODE_CMP_UP_Pos;
		}
	}

	return intr_msk;

}

__HW_STATIC_INLINE
void LL_PWM_ClearIntrFlag(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);
	uint32_t group_mak = (group + 1) << 3;
	uint32_t channel_mak = channel & 0x1;

	if (intr_mode & PWM_INTR_MODE_ZERO) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMIC[0],  group_mak);
		} else {
			HW_SET_BIT(PWM_DEV->PWMIC[1],  8);
		}
	}

	if (intr_mode & PWM_INTR_MODE_LOAD) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMIC[0],  group_mak + 1);
		} else {
			HW_SET_BIT(PWM_DEV->PWMIC[1],  9);
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_UP) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMIC[0], group_mak + 2 + channel_mak);
		} else {
			HW_SET_BIT(PWM_DEV->PWMIC[1], 10 + channel_mak);
		}
	}

	if (intr_mode & PWM_INTR_MODE_CMP_DW) {
		if (group < 3) {
			HW_SET_BIT(PWM_DEV->PWMIC[0], group_mak + 4 + channel_mak);
		} else {
			HW_SET_BIT(PWM_DEV->PWMIC[1], 12);
		}
	}
}

__HW_STATIC_INLINE
void LL_PWM_SetUpdateMode(uint32_t channel, uint32_t update_mode)
{
	uint32_t group = channel >> 1;

	HW_SET_MSK_VAL(PWM_DEV->PWMCTL, SYNCMODE_Msk << (group + SYNCMODE_Pos), (group + SYNCMODE_Pos), update_mode);
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetUpdateMode(pwm_dev_t *pwm, uint32_t channel)
{
	uint32_t group = channel >> 1;

	return HW_GET_MSK_VAL(PWM_DEV->PWMCTL, SYNCMODE_Msk << (group + SYNCMODE_Pos), (group + SYNCMODE_Pos));
}

__HW_STATIC_INLINE
void LL_PWM_SetCuntModeValue(uint32_t channel, uint32_t cunt_mode)
{
	uint32_t group = channel >> 1;

	HW_SET_MSK_VAL(PWM_DEV->PWMCTL, PWMMODE_Msk << (group + PWMMODE_Pos), (group + PWMMODE_Pos), cunt_mode);
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetCuntMode(uint32_t channel)
{
	uint32_t group = channel >> 1;

	return HW_GET_MSK_VAL(PWM_DEV->PWMCTL, PWMMODE_Msk << (group + PWMMODE_Pos), (group + PWMMODE_Pos));
}

__HW_STATIC_INLINE
void LL_PWM_SetReloadValue(uint32_t channel, uint32_t value)
{
	uint32_t group = channel >> 1;

	if (group & 0x01) {
		HW_SET_MSK_VAL(PWM_DEV->PWMLOADL[channel >> 2], LOAD1_Msk, LOAD1_Pos, value);
		HW_SET_MSK_VAL(PWM_DEV->PWMLOADH[channel >> 2], LOAD1_H_Msk, LOAD1_H_Pos, (value >> 16));
	} else {
		HW_SET_MSK_VAL(PWM_DEV->PWMLOADL[channel >> 2], LOAD0_Msk, LOAD0_Pos, value);
		HW_SET_MSK_VAL(PWM_DEV->PWMLOADH[channel >> 2], LOAD0_H_Msk, LOAD0_H_Pos, (value >> 16));
	}

}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetReloadValue(uint32_t channel)
{
	uint32_t group = channel >> 1;
	uint32_t value = 0;

	if (group & 0x01) {
		value = HW_GET_MSK_VAL(PWM_DEV->PWMLOADH[channel >> 2], LOAD1_H_Msk, LOAD1_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->PWMLOADL[channel >> 2], LOAD1_Msk, LOAD1_Pos);
	} else {
		value = HW_GET_MSK_VAL(PWM_DEV->PWMLOADH[channel >> 2], LOAD0_H_Msk, LOAD0_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->PWMLOADL[channel >> 2], LOAD0_Msk, LOAD0_Pos);
	}

	return value;
}

__HW_STATIC_INLINE
void LL_PWM_SetCompareValue(uint32_t channel, uint32_t value)
{
	uint32_t group = channel >> 1;

	if (channel  & 0x01) {
		HW_SET_MSK_VAL(PWM_DEV->PWMCMPL[group], COMP0B_Msk, COMP0B_Pos, value);
		HW_SET_MSK_VAL(PWM_DEV->PWMCMPH[group], COMP0B_H_Msk, COMP0B_H_Pos, (value >> 16));
	} else {
		HW_SET_MSK_VAL(PWM_DEV->PWMCMPL[group], COMP0A_Msk, COMP0A_Pos, value);
		HW_SET_MSK_VAL(PWM_DEV->PWMCMPH[group], COMP0A_H_Msk, COMP0A_H_Pos, (value >> 16));
	}
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetCompareValue(uint32_t channel)
{
	uint32_t group = channel >> 1;
	uint32_t value = 0;

	if (channel  & 0x01) {
		value = HW_GET_MSK_VAL(PWM_DEV->PWMCMPH[group], COMP0B_Msk, COMP0B_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->PWMCMPL[group], COMP0B_Msk, COMP0B_Pos);
	} else {
		value = HW_GET_MSK_VAL(PWM_DEV->PWMCMPH[group], COMP0A_Msk, COMP0A_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->PWMCMPL[group], COMP0A_Msk, COMP0A_Pos);
	}

	return value;
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetCurrentValue(uint32_t channel)
{
	uint32_t group = channel >> 1;
	uint32_t value = 0;

	if (group & 0x01) {
		value = HW_GET_MSK_VAL(PWM_DEV->PWMCOUNTH[channel >> 2], COUNT1_H_Msk, COUNT1_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->PWMCOUNTL[channel >> 2], COUNT1_Msk, COUNT1_Pos);
	} else {
		value = HW_GET_MSK_VAL(PWM_DEV->PWMCOUNTH[channel >> 2], COUNT0_H_Msk, COUNT0_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->PWMCOUNTL[channel >> 2], COUNT0_Msk, COUNT0_Pos);
	}

	return value;
}


__HW_STATIC_INLINE
void LL_PWM_EnableDeadband(uint32_t channel)
{
	uint32_t group = (channel >> 1);
	uint32_t channel_mak = channel & 0x1;

	if (group < 2) {
		HW_SET_BIT(PWM_DEV->PWMDB[0], DB0EN_Pos + channel_mak);
	} else {
		HW_SET_BIT(PWM_DEV->PWMDB[1], DB0EN_Pos + channel_mak);
	}
}

__HW_STATIC_INLINE
void LL_PWM_DisableDeadband(uint32_t channel)
{
	uint32_t group = (channel >> 1);
	uint32_t channel_mak = channel & 0x1;

	if (group < 2) {
		HW_CLR_BIT(PWM_DEV->PWMDB[0], DB0EN_Pos + channel_mak);
	} else {
		HW_CLR_BIT(PWM_DEV->PWMDB[1], DB0EN_Pos + channel_mak);
	}
}

__HW_STATIC_INLINE
uint32_t LL_PWM_IsEnabledDeadband(uint32_t channel)
{
	uint32_t group = (channel >> 1);
	uint32_t channel_mak = channel & 0x1;
	uint32_t status = 0;

	if (group < 2) {
		status = HW_TEST_BIT(PWM_DEV->PWMDB[0], DB0EN_Pos + channel_mak);
	} else {
		status = HW_TEST_BIT(PWM_DEV->PWMDB[1], DB0EN_Pos + channel_mak);
	}

	return status;
}

__HW_STATIC_INLINE
void LL_PWM_SetDeadband(uint32_t channel, uint32_t value)
{
	uint32_t group = (channel >> 1);

	switch (group) {
	case 0:
		HW_SET_MSK_VAL(PWM_DEV->PWMDB[0], DELAY0_Msk, DELAY0_Pos, value);
		break;
	case 1:
		HW_SET_MSK_VAL(PWM_DEV->PWMDB[0], DELAY1_Msk, DELAY1_Pos, value);
		break;
	case 2:
		HW_SET_MSK_VAL(PWM_DEV->PWMDB[1], DELAY2_Msk, DELAY2_Pos, value);
		break;
	case 3:
		HW_SET_MSK_VAL(PWM_DEV->PWMDB[1], DELAY3_Msk, DELAY3_Pos, value);
		break;
	default:
		break;
	}
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetDeadband(uint32_t channel)
{
	uint32_t group = (channel >> 1);
	uint32_t value = 0;

	switch (group) {
	case 0:
		value = HW_GET_MSK_VAL(PWM_DEV->PWMDB[0], DELAY0_Msk, DELAY0_Pos);
		break;
	case 1:
		value = HW_GET_MSK_VAL(PWM_DEV->PWMDB[0], DELAY1_Msk, DELAY1_Pos);
		break;
	case 2:
		value = HW_GET_MSK_VAL(PWM_DEV->PWMDB[1], DELAY2_Msk, DELAY2_Pos);
		break;
	case 3:
		value = HW_GET_MSK_VAL(PWM_DEV->PWMDB[1], DELAY3_Msk, DELAY3_Pos);
		break;
	default:
		break;
	}

	return value;
}

__HW_STATIC_INLINE
void LL_PWM_EnableAutoMode(uint32_t channel)
{
	HW_SET_BIT(PWM_DEV->AUTOMODE, (channel >> 1));
}

__HW_STATIC_INLINE
void LL_PWM_DisableAutoMode(uint32_t channel)
{
	HW_CLR_BIT(PWM_DEV->AUTOMODE, (channel >> 1));
}

__HW_STATIC_INLINE
uint32_t LL_PWM_IsEnabledAutoMode(uint32_t channel)
{
    return HW_TEST_BIT(PWM_DEV->AUTOMODE, (channel >> 1));
}


__HW_STATIC_INLINE
void LL_PWM_SetAutoCompareValue(uint32_t channel, uint32_t value)
{
	uint32_t group = (channel >> 1);

	if (group & 0x1) {
		HW_SET_VAL(PWM_DEV->AUTOCMPB[group], value);
	} else {
		HW_SET_VAL(PWM_DEV->AUTOCMPA[group], value);
	}
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetAutoCompareValue(uint32_t channel)
{
	uint32_t group = (channel >> 1);
	uint32_t value = 0;

	if (group & 0x1) {
		value = HW_GET_VAL(PWM_DEV->AUTOCMPB[group]);
	} else {
		value = HW_GET_VAL(PWM_DEV->AUTOCMPA[group]);
	}

	return value;
}

__HW_STATIC_INLINE
void LL_PWM_SetAutoInterval(uint32_t channel, uint32_t value)
{
	uint32_t group = (channel >> 1);

	HW_SET_MSK_VAL(PWM_DEV->AUTOCNT[group], TOTAL_CYCLE_Msk, TOTAL_CYCLE_Pos, value);
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetAutoInterval(uint32_t channel)
{
	uint32_t group = (channel >> 1);

    return HW_GET_MSK_VAL(PWM_DEV->AUTOCNT[group], TOTAL_CYCLE_Msk, TOTAL_CYCLE_Pos);
}

__HW_STATIC_INLINE
void LL_PWM_SetAutoStep(uint32_t channel, uint32_t value)
{
	uint32_t group = (channel >> 1);

	HW_SET_MSK_VAL(PWM_DEV->AUTOCNT[group], STEP_CYCLE_Msk, STEP_CYCLE_Pos, value);
}

__HW_STATIC_INLINE
uint32_t LL_PWM_GetAutoStep(uint32_t channel)
{
	uint32_t group = (channel >> 1);

    return HW_GET_MSK_VAL(PWM_DEV->AUTOCNT[group], STEP_CYCLE_Msk, STEP_CYCLE_Pos);
}
/**
  * @}
  */




/** @defgroup CAPT
  * @{
  */
__HW_STATIC_INLINE
void LL_CAPT_Enable(uint32_t channel)
{
	uint32_t group = (channel >> 1);

	HW_SET_BIT(PWM_DEV->PWMCFG, CAP0EN_Pos + group);
}

__HW_STATIC_INLINE
void LL_CAPT_Disable(uint32_t channel)
{
	uint32_t group = (channel >> 1);

	HW_CLR_BIT(PWM_DEV->PWMCFG, CAP0EN_Pos + group);
}

__HW_STATIC_INLINE
void LL_CAPT_Reset(void)
{
	PWM_DEV->CAPCTL = 0;
	PWM_DEV->CAPINTEN = 0;
	PWM_DEV->CAPIC |= 0x3CF;
}

__HW_STATIC_INLINE
void LL_CAPT_Intr_clearAll(void)
{
	PWM_DEV->CAPIC |= 0x3CF;
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_IsEnabled(uint32_t channel)
{
	uint32_t group = (channel >> 1);

    return HW_TEST_BIT(PWM_DEV->PWMCFG, CAP0EN_Pos + group);
}

__HW_STATIC_INLINE
void LL_CAPT_SetEventMode(uint32_t channel, uint32_t mode)
{
	uint32_t group = ((channel >> 1) << 1);

    HW_SET_MSK_VAL(PWM_DEV->CAPCTL, CAPEVENT_Msk << (CAPEVENT_Pos + group), (CAPEVENT_Pos + group), mode);
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_GetEventMode(uint32_t channel)
{
	uint32_t group = ((channel >> 1) << 1);

    return HW_GET_MSK_VAL(PWM_DEV->CAPCTL, CAPEVENT_Msk << (CAPEVENT_Pos + group), (CAPEVENT_Pos + group));
}

__HW_STATIC_INLINE
void LL_CAPT_SetMode(uint32_t channel, uint32_t mode)
{
	uint32_t group = (channel >> 1);

    HW_SET_MSK_VAL(PWM_DEV->CAPCTL, CAPMODE_Msk << (CAPMODE_Pos + group), (CAPMODE_Pos + group), mode);
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_GetMode(uint32_t channel)
{
	uint32_t group = (channel >> 1);

    return HW_GET_MSK_VAL(PWM_DEV->CAPCTL, CAPMODE_Msk << (CAPMODE_Pos + group), (CAPMODE_Pos + group));
}


__HW_STATIC_INLINE
void LL_CAPT_EnableIntr(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);

	if (intr_mode & CAPT_INTR_MODE_TIME) {
	    HW_SET_BIT(PWM_DEV->CAPINTEN, (CAP0TIMIE_Pos + group));
	}

	if (intr_mode & CAPT_INTR_MODE_CUNT) {
		HW_SET_BIT(PWM_DEV->CAPINTEN, (CAP0CNTIE_Pos + group));
	}
}

__HW_STATIC_INLINE
void LL_CAPT_DisableIntr(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);

	if (intr_mode & CAPT_INTR_MODE_TIME) {
	    HW_CLR_BIT(PWM_DEV->CAPINTEN, CAP0TIMIE_Pos + group);
	}

	if (intr_mode & CAPT_INTR_MODE_CUNT) {
		HW_CLR_BIT(PWM_DEV->CAPINTEN, CAP0CNTIE_Pos + group);
	}
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_IsEnabledIntr(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);
	uint32_t status = 0;

	if (intr_mode & CAPT_INTR_MODE_TIME) {
	    status = HW_TEST_BIT(PWM_DEV->CAPINTEN, CAP0TIMIE_Pos + group) << CAPT_INTR_MODE_TIME_POS;
	}

	if (intr_mode & CAPT_INTR_MODE_CUNT) {
		status |= HW_TEST_BIT(PWM_DEV->CAPINTEN, CAP0CNTIE_Pos + group) << CAPT_INTR_MODE_CUNT_POS;
	}

	return status;
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_GetStatus(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);
	uint32_t status = 0;

	if (intr_mode & CAPT_INTR_MODE_TIME) {
	    status = HW_TEST_BIT(PWM_DEV->CAPRIS, CAP0TIMIE_Pos + group) << CAPT_INTR_MODE_TIME_POS;
	}

	if (intr_mode & CAPT_INTR_MODE_CUNT) {
		status |= HW_TEST_BIT(PWM_DEV->CAPRIS, CAP0CNTIE_Pos + group) << CAPT_INTR_MODE_CUNT_POS;
	}

	return status;
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_GetIntrFlag(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);
	uint32_t status = 0;

	if (intr_mode & CAPT_INTR_MODE_TIME) {
	    status = HW_TEST_BIT(PWM_DEV->CAPIS, CAP0TIMIE_Pos + group) << CAPT_INTR_MODE_TIME_POS;
	}

	if (intr_mode & CAPT_INTR_MODE_CUNT) {
		status |= HW_TEST_BIT(PWM_DEV->CAPIS, CAP0CNTIE_Pos + group) << CAPT_INTR_MODE_CUNT_POS;
	}

	return status;
}

__HW_STATIC_INLINE
void LL_CAPT_ClearIntrFlag(uint32_t channel, uint32_t intr_mode)
{
	uint32_t group = (channel >> 1);

	if (intr_mode & CAPT_INTR_MODE_TIME) {
	    HW_SET_BIT(PWM_DEV->CAPIC, CAP0TIMIE_Pos + group);
	}

	if (intr_mode & CAPT_INTR_MODE_CUNT) {
		HW_SET_BIT(PWM_DEV->CAPIC, CAP0CNTIE_Pos + group);
	}
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_GetValue(uint32_t channel)
{
	uint32_t group = (channel >> 1);
	uint32_t value = 0;

	switch (group) {
	case 0:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPTH[0], CAP01T0_H_Msk, CAP01T0_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPTL[0], CAP01T0_Msk, CAP01T0_Pos);
		break;
	case 1:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPTH[0], CAP01T1_H_Msk, CAP01T1_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPTL[0], CAP01T1_Msk, CAP01T1_Pos);
		break;
	case 2:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPTH[1], CAP23T2_H_Msk, CAP23T2_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPTL[1], CAP23T2_Msk, CAP23T2_Pos);
		break;
	case 3:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPTH[1], CAP23T3_H_Msk, CAP23T3_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPTL[1], CAP23T3_Msk, CAP23T3_Pos);
		break;
	default:
		break;
	}

	return value;
}

__HW_STATIC_INLINE
void LL_CAPT_SetCountMathValue(uint32_t channel, uint32_t value)
{
	uint32_t group = (channel >> 1);

	switch (group) {
	case 0:
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHH[0], CAP01MATCH0_H_Msk, CAP01MATCH0_H_Pos, value >> 16);
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHL[0], CAP01MATCH0_Msk, CAP01MATCH0_Pos, value);
		break;
	case 1:
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHH[0], CAP01MATCH1_H_Msk, CAP01MATCH1_H_Pos, value >> 16);
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHL[0], CAP01MATCH1_Msk, CAP01MATCH1_Pos, value);
		break;
	case 2:
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHH[1], CAP23MATCH2_H_Msk, CAP23MATCH2_H_Pos, value >> 16);
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHL[1], CAP23MATCH2_Msk, CAP23MATCH2_Pos, value);
		break;
	case 3:
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHH[1], CAP23MATCH3_H_Msk, CAP23MATCH3_H_Pos, value >> 16);
		HW_SET_MSK_VAL(PWM_DEV->CAPMATCHL[1], CAP23MATCH3_Msk, CAP23MATCH3_Pos, value);
		break;
	default:
		break;
	}
}

__HW_STATIC_INLINE
uint32_t LL_CAPT_GetCountMathValue(uint32_t channel)
{
	uint32_t group = (channel >> 1);
	uint32_t value = 0;

	switch (group) {
	case 0:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPMATCHH[0], CAP01MATCH0_H_Msk, CAP01MATCH0_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPMATCHL[0], CAP01MATCH0_Msk, CAP01MATCH0_Pos);
		break;
	case 1:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPMATCHH[0], CAP01MATCH1_H_Msk, CAP01MATCH1_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPMATCHL[0], CAP01MATCH1_Msk, CAP01MATCH1_Pos);
		break;
	case 2:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPMATCHH[1], CAP23MATCH2_H_Msk, CAP23MATCH2_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPMATCHL[1], CAP23MATCH2_Msk, CAP23MATCH2_Pos);
		break;
	case 3:
		value = HW_GET_MSK_VAL(PWM_DEV->CAPMATCHH[1], CAP23MATCH3_H_Msk, CAP23MATCH3_H_Pos);
		value <<= 16;
		value |= HW_GET_MSK_VAL(PWM_DEV->CAPMATCHL[1], CAP23MATCH3_Msk, CAP23MATCH3_Pos);
		break;
	default:
		break;
	}

	return value;
}

/**
  * @}
  */



/**
  * @}
  */

/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_PWM_H */

