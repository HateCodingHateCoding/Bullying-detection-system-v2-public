/**
 **************************************************************************************************
 * @brief   can hadware define.
 * @attention
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_CAN_H
#define _PSIC_LL_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "soc.h"
#include "types.h"
#include "hw_can.h"

#define CAN_DEV                         ((can_dev_t *)    CAN_BASE_ADDR) /*!< 0x4001 DC00 */
/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */

#define LL_CAN_AFC_MODE_STANARD_FRAMES				2
#define LL_CAN_AFC_MODE_EXTENDED_FRAMES				3
#define LL_CAN_AFC_MODE_BOTH_FRAMES					0

#define LL_CAN_TX_BUFF_PRIMARY					    0
#define LL_CAN_TX_BUFF_SECONDARY				    1
#define LL_CAN_TX_BUFF_SECONDARY_ONE				1
#define LL_CAN_TX_BUFF_SECONDARY_ALL				2

#define LL_CAN_ERROR_NO								0
#define LL_CAN_ERROR_BIT							1
#define LL_CAN_ERROR_FORM							2
#define LL_CAN_ERROR_STUFF							3
#define LL_CAN_ERROR_ACK							4
#define LL_CAN_ERROR_CRC							5
#define LL_CAN_ERROR_OTHER							6

#define LL_CAN_STB_EMPTY							0
#define LL_CAN_STB_LESS_EQUAL_HALF_FULL				1
#define LL_CAN_STB_MORE_HALF_FULL					2
#define LL_CAN_STB_FULL								3

#define LL_CAN_ITR_MSK_ERROR						(1 << 0)
#define LL_CAN_ITR_MSK_TSB							(1 << 1)
#define LL_CAN_ITR_MSK_TPB							(1 << 2)
#define LL_CAN_ITR_MSK_RAF							(1 << 3)
#define LL_CAN_ITR_MSK_RF							(1 << 4)
#define LL_CAN_ITR_MSK_RO							(1 << 5)
#define LL_CAN_ITR_MSK_RX							(1 << 6)
#define LL_CAN_ITR_MSK_BE							(1 << 7)
#define LL_CAN_ITR_MSK_AL							(1 << 8)
#define LL_CAN_ITR_MSK_EP							(1 << 9)

/* Exported functions
 * ------------------------------------------------------------------------------------------------
 */

__HW_STATIC_INLINE
void LL_CAN_PSSModeEnable(void)
{
	HW_SET_MSK(CAN_DEV->CFG, PSS_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_PSSModeDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CFG, PSS_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsPSSModeEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CFG, PSS_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_SSSModeEnable(void)
{
	HW_SET_MSK(CAN_DEV->CFG, SSS_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_SSSModeDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CFG, SSS_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsSSSModeEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CFG, SSS_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_LBEModeEnable(void)
{
	HW_SET_MSK(CAN_DEV->CFG, LBME_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_LBEModeDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CFG, LBME_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsLBEModeEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CFG, LBME_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_LBIModeEnable(void)
{
	HW_SET_MSK(CAN_DEV->CFG, LBMI_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_LBIModeDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CFG, LBMI_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsLBIModeEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CFG, LBMI_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_ListenModeEnable(void)
{
	HW_SET_MSK(CAN_DEV->CFG, LOM_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_ListenModeDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CFG, LOM_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsListenModeEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CFG, LOM_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_FDISOModeEnable(void)
{
	HW_SET_MSK(CAN_DEV->CFG, FD_ISO_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_FDISOModeDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CFG, FD_ISO_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsFDModeEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CFG, FD_ISO_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_ResetHold(void)
{
	HW_SET_MSK(CAN_DEV->CTRL, RESET_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_ResetRelease(void)
{
	HW_CLR_MSK(CAN_DEV->CTRL, RESET_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_RxSampleDelaySet(uint32_t delay)
{
	HW_SET_MSK_VAL(CAN_DEV->CTRL, SSPOFF_Msk, SSPOFF_Pos, delay);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_RxSampleDelayGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->CTRL, SSPOFF_Msk, SSPOFF_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_TxSampleDelaySet(uint32_t delay)
{
	HW_SET_MSK_VAL(CAN_DEV->CTRL, TDCEN_Msk, TDCEN_Pos, delay);
}

__HW_STATIC_INLINE
void LL_CAN_SlowPrescSet(uint32_t prescale)
{
	HW_SET_MSK_VAL(CAN_DEV->S_BAUDR, S_PRESC_Msk, S_PRESC_Pos, prescale);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_SlowPrescGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->S_BAUDR, S_PRESC_Msk, S_PRESC_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_SlowSJWSet(uint32_t tq)
{
	HW_SET_MSK_VAL(CAN_DEV->S_BAUDR, S_SJW_Msk, S_SJW_Pos, tq);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_SlowSJWGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->S_BAUDR, S_SJW_Msk, S_SJW_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_SlowSeg1Set(uint32_t tq)
{
	HW_SET_MSK_VAL(CAN_DEV->S_BAUDR, S_SEG_1_Msk, S_SEG_1_Pos, tq);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_SlowSeg1Get(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->S_BAUDR, S_SEG_1_Msk, S_SEG_1_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_SlowSeg2Set(uint32_t tq)
{
	HW_SET_MSK_VAL(CAN_DEV->S_BAUDR, S_SEG_2_Msk, S_SEG_2_Pos, tq);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_SlowSeg2Get(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->S_BAUDR, S_SEG_2_Msk, S_SEG_2_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_FastPrescSet(uint32_t prescale)
{
	HW_SET_MSK_VAL(CAN_DEV->F_BAUDR, F_PRESC_Msk, F_PRESC_Pos, prescale);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_FastPrescGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->F_BAUDR, F_PRESC_Msk, F_PRESC_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_FastSJWSet(uint32_t tq)
{
	HW_SET_MSK_VAL(CAN_DEV->F_BAUDR, F_SJW_Msk, F_SJW_Pos, tq);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_FastSJWGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->F_BAUDR, F_SJW_Msk, F_SJW_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_FastSeg1Set(uint32_t tq)
{
	HW_SET_MSK_VAL(CAN_DEV->F_BAUDR, F_SEG_1_Msk, F_SEG_1_Pos, tq);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_FastSeg1Get(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->F_BAUDR, F_SEG_1_Msk, F_SEG_1_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_FastSeg2Set(uint32_t tq)
{
	HW_SET_MSK_VAL(CAN_DEV->F_BAUDR, F_SEG_2_Msk, F_SEG_2_Pos, tq);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_FastSeg2Get(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->F_BAUDR, F_SEG_2_Msk, F_SEG_2_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_ACFSetState(uint32_t statemsk)
{
	HW_SET_MSK_VAL(CAN_DEV->CFG, AE_Msk, AE_Pos, statemsk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_ACFGetState(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->CFG, AE_Msk, AE_Pos);
}


__HW_STATIC_INLINE
void LL_CAN_ACFSetIndex(uint32_t idx)
{
	HW_SET_MSK_VAL(CAN_DEV->CTRL, ACFADR_Msk, ACFADR_Pos, idx);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_ACFGetIndex(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->CTRL, ACFADR_Msk, ACFADR_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_SelACFMask(void)
{
	HW_SET_MSK(CAN_DEV->CTRL, SELMASK_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_SelACFCode(void)
{
	HW_CLR_MSK(CAN_DEV->CTRL, SELMASK_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_ACFSet(uint32_t value)
{
	CAN_DEV->ACF = value;
}

__HW_STATIC_INLINE
void LL_CAN_ACFValueSet(uint32_t value)
{
	HW_SET_MSK_VAL(CAN_DEV->ACF, ACFADR_Msk, ACFADR_Pos, value);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_ACFValueGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->ACF, ACFADR_Msk, ACFADR_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_ACFModeSet(uint32_t mode)
{
	HW_SET_MSK_VAL(CAN_DEV->ACF, ACF_MODE_Msk, ACF_MODE_Pos, mode);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_ACFModeGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->ACF, ACF_MODE_Msk, ACF_MODE_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_TxBuffNext(void)
{
	HW_SET_MSK(CAN_DEV->CTRL, TSNEXT_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_RxBuffNext(void)
{
	HW_SET_MSK(CAN_DEV->CTRL, RREL_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_RxDataGet(uint8_t offset)
{
	return CAN_DEV->RBUF[offset];
}

__HW_STATIC_INLINE
void LL_CAN_TxDataSet(uint8_t offset, uint32_t value)
{
	CAN_DEV->TBUF[offset] = value;
}

__HW_STATIC_INLINE
uint32_t LL_CAN_TxDataGet(uint8_t offset)
{
	return CAN_DEV->TBUF[offset];
}

__HW_STATIC_INLINE
void * LL_CAN_GetTxBuff(void)
{
	return (void *)(CAN_DEV->TBUF);
}

__HW_STATIC_INLINE
void * LL_CAN_GetRxBuff(void)
{
	return (void *)(CAN_DEV->RBUF);
}

#define LL_CAN_TX_BUFF_TYPE_MASK					1
#define LL_CAN_TX_BUFF_SECONDARY_ALL_MASK			2

__HW_STATIC_INLINE
void LL_CAN_TxDataEnable(uint32_t buff_type)
{
	if (buff_type == LL_CAN_TX_BUFF_PRIMARY) {
		HW_CLR_MSK(CAN_DEV->CTRL, TSALL_Msk);
		HW_CLR_MSK(CAN_DEV->CTRL, TSONE_Msk);
		HW_SET_MSK(CAN_DEV->CTRL, TPE_Msk);
	} else if (buff_type == LL_CAN_TX_BUFF_SECONDARY_ALL) {
		HW_CLR_MSK(CAN_DEV->CTRL, TPE_Msk);
		HW_CLR_MSK(CAN_DEV->CTRL, TSONE_Msk);
		HW_SET_MSK(CAN_DEV->CTRL, TSALL_Msk);
	} else {
		HW_CLR_MSK(CAN_DEV->CTRL, TPE_Msk);
		HW_CLR_MSK(CAN_DEV->CTRL, TSALL_Msk);
		HW_SET_MSK(CAN_DEV->CTRL, TSONE_Msk);
	}
}


__HW_STATIC_INLINE
void LL_CAN_TxDataDisable(uint32_t buff_type)
{
	if ((buff_type & LL_CAN_TX_BUFF_TYPE_MASK) == LL_CAN_TX_BUFF_PRIMARY) {
		HW_SET_MSK(CAN_DEV->CTRL, TPA_Msk);
	} else {
		HW_SET_MSK(CAN_DEV->CTRL, TSA_Msk);
	}
}

__HW_STATIC_INLINE
void LL_CAN_TxBuffTypeSet(uint32_t buff_type)
{
	HW_SET_MSK_VAL(CAN_DEV->CTRL, TBSEL_Msk, TBSEL_Pos, buff_type);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_TxBuffTypeGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->CTRL, TBSEL_Msk, TBSEL_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_RxBuffThresholdSet(uint32_t level)
{
	HW_SET_MSK_VAL(CAN_DEV->CTRL, RFTL_Msk, RFTL_Pos, level);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_RxBuffThresholdGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->CTRL, RFTL_Msk, RFTL_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_AutoACKEnable(void)
{
	HW_SET_MSK(CAN_DEV->CTRL, SACK_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_AutoACKDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CTRL, SACK_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsAutoACKEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CTRL, SACK_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_RxErrorFramesEnable(void)
{
	HW_SET_MSK(CAN_DEV->CTRL, RALL_Msk);
}

__HW_STATIC_INLINE
void LL_CAN_RxErrorFramesDisable(void)
{
	HW_CLR_MSK(CAN_DEV->CTRL, RALL_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsRxErrorFramesEnabled(void)
{
	return HW_TEST_BIT(CAN_DEV->CTRL, RALL_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_RxOverflowModeSet(uint32_t mode)
{
	HW_SET_MSK_VAL(CAN_DEV->CTRL, ROM_Msk, ROM_Pos, mode);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_RxOverflowModeGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->CTRL, ROM_Msk, ROM_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_ArbitLostCaptGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->STATUS, ALC_Msk, ALC_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_TxBuffStatusGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->STATUS, TSSTAT_Msk, TSSTAT_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_RxBuffStatusGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->STATUS, RSTAT_Msk, RSTAT_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_ErrorCodeGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->STATUS, KOER_Msk, KOER_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsRxBuffOverflow(void)
{
	return HW_TEST_BIT(CAN_DEV->STATUS, ROV_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsRxBusy(void)
{
	return HW_TEST_BIT(CAN_DEV->STATUS, RACTIVE_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsTxBusy(void)
{
	return HW_TEST_BIT(CAN_DEV->STATUS, TACTIVE_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsOverErrorLimit(void)
{
	return HW_TEST_BIT(CAN_DEV->STATUS, EWARN_Pos);
}

__HW_STATIC_INLINE
void LL_CAN_ErrorLimitSet(uint32_t limit)
{
	HW_SET_MSK_VAL(CAN_DEV->ERR_COUNT, EWL_Msk, EWL_Pos, limit);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_ErrorLimitGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->ERR_COUNT, EWL_Msk, EWL_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_TxErrorCountGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->ERR_COUNT, TECNT_Msk, TECNT_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_RxErrorCountGet(void)
{
	return HW_GET_MSK_VAL(CAN_DEV->ERR_COUNT, RECNT_Msk, RECNT_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_InterruptFlagGet(uint32_t msk)
{
	return HW_GET_MSK(CAN_DEV->ISR, msk);
}

__HW_STATIC_INLINE
void LL_CAN_InterruptClear(uint32_t msk)
{
	HW_SET_MSK(CAN_DEV->ISR, msk);
}

__HW_STATIC_INLINE
void LL_CAN_InterruptEnable(uint32_t msk)
{
	HW_SET_MSK(CAN_DEV->IER, msk);
}

__HW_STATIC_INLINE
void LL_CAN_InterruptDisable(uint32_t msk)
{
	HW_CLR_MSK(CAN_DEV->IER, msk);
}

__HW_STATIC_INLINE
uint32_t LL_CAN_IsInterruptEnabled(uint32_t msk)
{
	return HW_GET_MSK(CAN_DEV->IER, msk);
}


#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_DMA_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
