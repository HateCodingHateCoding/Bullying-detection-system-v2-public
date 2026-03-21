/**
 **************************************************************************************************
 * @brief   psic_ll_utils.h
 * @brief   utils hadware define.
 * @attention
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_UTILS_H
#define _PSIC_LL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */
#include "types.h"
#include "hw_ccmu1.h"
#include "hw_ccmu2.h"
#include "hw_pmu.h"
#include "hw_pmu2.h"
#include "hw_rcc.h"
#include "hw_syscfg.h"
#include "hw_swcfg.h"
#include "hw_misc.h"
#include "hw_misc2.h"
#include "soc.h"


/** @addtogroup PSIC_LL_Driver
  * @{
  */

/** @defgroup psic_ll_utils
  * @{
  */

/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
#define LL_INTR_SW1_POS							(16)
#define LL_INTR_AON_POS							(16 + 32)
#define LL_INTR_RTC_POS							(16 + 32 + 20)
#define LL_INTR_MAX                             (16 + 32 + 20 + 20)


#define LL_WKUP_SRC_RTT0						HW_BIT(0)
#define LL_WKUP_SRC_RTC							HW_BIT(1)
#define LL_WKUP_SRC_WPIO						HW_BIT(2)
#define LL_WKUP_SRC_RTT1						HW_BIT(12)
#define LL_WKUP_SRC_SPI							HW_BIT(13)
#define LL_WKUP_SRC_I2C							HW_BIT(14)
#define LL_WKUP_SRC_UART						HW_BIT(15)
#define LL_WKUP_SRC_RTICK0						HW_BIT(16)
#define LL_WKUP_SRC_RTICK1						HW_BIT(17)


#define LL_WKUP_UART_PA01						HW_BIT(0)
#define LL_WKUP_UART_PA03						HW_BIT(1)
#define LL_WKUP_UART_PA07						HW_BIT(2)
#define LL_WKUP_UART_PA09						HW_BIT(3)
#define LL_WKUP_UART_PA12						HW_BIT(4)
#define LL_WKUP_UART_PA14						HW_BIT(5)
#define LL_WKUP_UART_PB01						HW_BIT(6)

#define LL_WKUP_I2C_SCL_PA03					HW_BIT(0)
#define LL_WKUP_I2C_SCL_PA05					HW_BIT(1)
#define LL_WKUP_I2C_SCL_PA07					HW_BIT(2)
#define LL_WKUP_I2C_SCL_PA08					HW_BIT(3)
#define LL_WKUP_I2C_SCL_PA11					HW_BIT(4)
#define LL_WKUP_I2C_SCL_PA15					HW_BIT(5)

#define LL_WKUP_I2C_SDA_PA02					HW_BIT(0)
#define LL_WKUP_I2C_SDA_PA04					HW_BIT(1)
#define LL_WKUP_I2C_SDA_PA06					HW_BIT(2)
#define LL_WKUP_I2C_SDA_PA09					HW_BIT(3)
#define LL_WKUP_I2C_SDA_PA10					HW_BIT(4)
#define LL_WKUP_I2C_SDA_PA14					HW_BIT(5)

#define LL_WKUP_WKIO_SRC_MASK					HW_BIT(2)

#define LL_SRAMB_16K							HW_BIT(0)
#define LL_SRAMB_32K							HW_BIT(1)
#define LL_SRAMB_64K							HW_BIT(2)
#define LL_SRAMB_128K							HW_BIT(3)
#define LL_SRAMB_256K							HW_BIT(4)

#define LL_SHARE_MEM_256K   0xFFFF
#define LL_SHARE_MEM_128K   0xFF00
#define LL_SHARE_MEM_64K    0xF000
#define LL_SHARE_MEM_32K    0xC000
#define LL_SHARE_MEM_16K    0xE000

#define LL_WDG_RESET_CTRL_CPU					(0)
#define LL_WDG_RESET_CTRL_SYS					(1)

#define LL_BOOT_SYS_FLAG_RESET					(0x02)
#define LL_BOOT_SYS_FLAG_UPGRADE				(0x03)


#define LL_DELAY_CYCLE							0
#define LL_DELAY_US								1
#define LL_DELAY_MS								2

#define LL_DELAY_MIN_CYCLE						8

/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */



/** @defgroup utils-INTERRUPT MAP
  * @{
  */
__HW_STATIC_INLINE
void LL_INTR_EnableToCPUS(uint32_t intr_id)
{
	if (intr_id < LL_INTR_SW1_POS) {
		return ;
	} else if (intr_id < LL_INTR_AON_POS) {
		HW_CLR_BIT(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[0], (intr_id - LL_INTR_SW1_POS));
	} else if (intr_id < LL_INTR_RTC_POS) {
		HW_CLR_BIT(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[1], (intr_id - LL_INTR_AON_POS));
	} else if (intr_id < LL_INTR_MAX) {
		HW_CLR_BIT(RCC_DEV->RCC_CPUS_INTR_MASK, (intr_id - LL_INTR_RTC_POS));
	}
}

__HW_STATIC_INLINE
void LL_INTR_DisableToCPUS(uint32_t intr_id)
{
	if (intr_id < LL_INTR_SW1_POS) {
		return ;
	} else if (intr_id < LL_INTR_AON_POS) {
		HW_SET_BIT(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[0], (intr_id - LL_INTR_SW1_POS));
	} else if (intr_id < LL_INTR_RTC_POS) {
		HW_SET_BIT(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[1], (intr_id - LL_INTR_AON_POS));
	} else if (intr_id < LL_INTR_MAX) {
		HW_SET_BIT(RCC_DEV->RCC_CPUS_INTR_MASK, (intr_id - LL_INTR_RTC_POS));
	}
}

__HW_STATIC_INLINE
void LL_INTR_GetCPUSMsk(uint32_t *sw_msk, uint32_t *aon_msk, uint32_t *rtc_msk)
{
	*sw_msk = HW_GET_VAL(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[0]);
	*aon_msk = HW_GET_VAL(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[1]);
	*rtc_msk = HW_GET_VAL(RCC_DEV->RCC_CPUS_INTR_MASK);
}

__HW_STATIC_INLINE
void LL_INTR_SetCPUSMsk(uint32_t       sw_msk, uint32_t aon_msk, uint32_t rtc_msk)
{
	HW_SET_VAL(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[0], sw_msk);
	HW_SET_VAL(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[1], aon_msk);
	HW_SET_VAL(RCC_DEV->RCC_CPUS_INTR_MASK, rtc_msk);
}

__HW_STATIC_INLINE
void LL_INTR_EnableToCPUF(uint32_t intr_id)
{
	if (intr_id < LL_INTR_SW1_POS) {
		return ;
	} else if (intr_id < LL_INTR_AON_POS) {
		HW_CLR_BIT(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[0], (intr_id - LL_INTR_SW1_POS));
	} else if (intr_id < LL_INTR_RTC_POS) {
		HW_CLR_BIT(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[1], (intr_id - LL_INTR_AON_POS));
	} else if (intr_id < LL_INTR_MAX) {
		HW_CLR_BIT(RCC_DEV->RCC_CPUF_INTR_MASK, (intr_id - LL_INTR_RTC_POS));
	}
}

__HW_STATIC_INLINE
void LL_INTR_DisableToCPUF(uint32_t intr_id)
{
	if (intr_id < LL_INTR_SW1_POS) {
		return ;
	} else if (intr_id < LL_INTR_AON_POS) {
		HW_SET_BIT(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[0], (intr_id - LL_INTR_SW1_POS));
	} else if (intr_id < LL_INTR_RTC_POS) {
		HW_SET_BIT(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[1], (intr_id - LL_INTR_AON_POS));
	} else if (intr_id < LL_INTR_MAX) {
		HW_SET_BIT(RCC_DEV->RCC_CPUF_INTR_MASK, (intr_id - LL_INTR_RTC_POS));
	}
}

__HW_STATIC_INLINE
void LL_INTR_GetCPUFMsk(uint32_t *sw_msk, uint32_t *aon_msk, uint32_t *rtc_msk)
{
	*sw_msk = HW_GET_VAL(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[0]);
	*aon_msk = HW_GET_VAL(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[1]);
	*rtc_msk = HW_GET_VAL(RCC_DEV->RCC_CPUF_INTR_MASK);
}

__HW_STATIC_INLINE
void LL_INTR_SetCPUFMsk(uint32_t          sw_msk, uint32_t aon_msk, uint32_t rtc_msk)
{
	HW_SET_VAL(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[0], sw_msk);
	HW_SET_VAL(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[1], aon_msk);
	HW_SET_VAL(RCC_DEV->RCC_CPUF_INTR_MASK, rtc_msk);
}

__HW_STATIC_INLINE
uint32_t LL_INTR_IsEnabledToCPUS(uint32_t intr_id)
{
	if (intr_id < LL_INTR_SW1_POS) {
		return 0;
	} else if (intr_id < LL_INTR_AON_POS) {
		return !HW_TEST_BIT(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[0], (intr_id - LL_INTR_SW1_POS));
	} else if (intr_id < LL_INTR_RTC_POS) {
		return !HW_TEST_BIT(SYSCFG_DEV->SYSCFG_CPUS_INTR_MASK[1], (intr_id - LL_INTR_AON_POS));
	} else if (intr_id < LL_INTR_MAX) {
		return !HW_TEST_BIT(RCC_DEV->RCC_CPUS_INTR_MASK, (intr_id - LL_INTR_RTC_POS));
	} else {
		return 0;
	}
}

__HW_STATIC_INLINE
uint32_t LL_INTR_IsEnabledToCPUF(uint32_t intr_id)
{
	if (intr_id < LL_INTR_SW1_POS) {
		return 0;
	} else if (intr_id < LL_INTR_AON_POS) {
		return !HW_TEST_BIT(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[0], (intr_id - LL_INTR_SW1_POS));
	} else if (intr_id < LL_INTR_RTC_POS) {
		return !HW_TEST_BIT(SYSCFG_DEV->SYSCFG_CPUF_INTR_MASK[1], (intr_id - LL_INTR_AON_POS));
	} else if (intr_id < LL_INTR_MAX) {
		return !HW_TEST_BIT(RCC_DEV->RCC_CPUF_INTR_MASK, (intr_id - LL_INTR_RTC_POS));
	} else {
		return 0;
	}
}
/**
  * @}
  */



/** @defgroup utils-NVM Store
  * @{
  */
__HW_STATIC_INLINE
void LL_NVM_SetValue(uint32_t idx, uint32_t val)
{
	HW_SET_VAL(RCC_DEV->RCC_SPECIAL_REG[idx], val);
}

__HW_STATIC_INLINE
uint32_t LL_NVM_GetValue(uint32_t idx)
{
	return HW_GET_VAL(RCC_DEV->RCC_SPECIAL_REG[idx]);
}

__HW_STATIC_INLINE
void LL_NVM_SetMask(uint32_t idx, uint32_t bit)
{
	HW_SET_BIT(RCC_DEV->RCC_SPECIAL_REG[idx], bit);
}

__HW_STATIC_INLINE
uint32_t LL_NVM_GetMask(uint32_t idx, uint32_t bit)
{
	return HW_GET_BIT(RCC_DEV->RCC_SPECIAL_REG[idx], bit);
}
/**
  * @}
  */




/** @defgroup utils-BOOT
  * @{
  */
__HW_STATIC_INLINE
void LL_BOOT_SetCPUSAddr(uint32_t addr)
{
    HW_SET_VAL(RCC_DEV->RCC_SYS0_BOOTADDR, addr);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetCPUSAddr(void)
{
    return HW_GET_VAL(RCC_DEV->RCC_SYS0_BOOTADDR);
}

__HW_STATIC_INLINE
void LL_BOOT_SetCPUSParam(uint32_t param)
{
    HW_SET_VAL(RCC_DEV->RCC_SYS0_BOOTPARAM, param);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetCPUSParam(void)
{
    return HW_GET_VAL(RCC_DEV->RCC_SYS0_BOOTPARAM);
}

__HW_STATIC_INLINE
void LL_BOOT_SetCPUSFlag(uint32_t flag)
{
    HW_SET_VAL(RCC_DEV->RCC_SYS0_BOOTFLAG, flag);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetCPUSFlag(void)
{
    return HW_GET_VAL(RCC_DEV->RCC_SYS0_BOOTFLAG);
}

__HW_STATIC_INLINE
void LL_BOOT_SetCPUFAddr(uint32_t addr)
{
    HW_SET_VAL(RCC_DEV->RCC_SYS1_BOOTADDR, addr);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetCPUFAddr(void)
{
    return HW_GET_VAL(RCC_DEV->RCC_SYS1_BOOTADDR);
}

__HW_STATIC_INLINE
void LL_BOOT_SetCPUFParam(uint32_t param)
{
    HW_SET_VAL(RCC_DEV->RCC_SYS1_BOOTPARAM, param);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetCPUFParam(void)
{
    return HW_GET_VAL(RCC_DEV->RCC_SYS1_BOOTPARAM);
}

__HW_STATIC_INLINE
void LL_BOOT_SetCPUFFlag(uint32_t flag)
{
    HW_SET_VAL(RCC_DEV->RCC_SYS1_BOOTFLAG, flag);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetCPUFFlag(void)
{
    return HW_GET_VAL(RCC_DEV->RCC_SYS1_BOOTFLAG);
}

__HW_STATIC_INLINE
void LL_BOOT_SetFirstAddr(uint32_t addr)
{
    HW_SET_VAL(RCC_DEV->RCC_FIRST_BOOTADDR, addr);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetFirstAddr(void)
{
    return HW_GET_VAL(RCC_DEV->RCC_FIRST_BOOTADDR);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_GetFirstSysId(void)
{
  	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, BOOTSEL_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_BOOT_ResetEnable(void)
{
  	return HW_SET_BIT(CCMU2_DEV->CCMU2_RESETN_EN, SYS_RESET_EN_Pos);
}


/**
  * @}
  */



/** @defgroup utils-REMAP-RAM
  * @{
  */
__HW_STATIC_INLINE
void LL_REMAP_RAM_SetIBusMap(uint32_t idx, uint32_t addr, uint32_t len)
{
	HW_SET_VAL(SYSCFG_DEV->CPU_I_REMAP_BDADDR[idx], addr);
	if (idx == 0) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[0], CPU_I_REMAP0_SIZE_Msk, CPU_I_REMAP0_SIZE_Pos, (len >> 7));
	} else if (idx == 1) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[0], CPU_I_REMAP1_SIZE_Msk, CPU_I_REMAP1_SIZE_Pos, (len >> 7));
	} else if (idx == 2) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[1], CPU_I_REMAP2_SIZE_Msk, CPU_I_REMAP2_SIZE_Pos, (len >> 7));
	} else if (idx == 3) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[1], CPU_I_REMAP3_SIZE_Msk, CPU_I_REMAP3_SIZE_Pos, (len >> 7));
	}
}

__HW_STATIC_INLINE
uint32_t LL_REMAP_SRAM_SetIBus(uint32_t dst_addr, uint32_t src_addr, uint32_t addr_size)
{
	uint8_t remap_idx = 0;

	addr_size >>= 7;
	if (dst_addr == 0x00500000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[0], CPU_I_REMAP0_SIZE_Msk, CPU_I_REMAP0_SIZE_Pos, addr_size);
		remap_idx = 0;
	} else if (dst_addr == 0x00540000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[0], CPU_I_REMAP1_SIZE_Msk, CPU_I_REMAP1_SIZE_Pos, addr_size);
		remap_idx = 1;
	} else if (dst_addr == 0x00580000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[1], CPU_I_REMAP2_SIZE_Msk, CPU_I_REMAP2_SIZE_Pos, addr_size);
		remap_idx = 2;
	} else if (dst_addr == 0x005C0000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[1], CPU_I_REMAP3_SIZE_Msk, CPU_I_REMAP3_SIZE_Pos, addr_size);
		remap_idx = 3;
	} else {
		return 1;
	}

	HW_SET_VAL(SYSCFG_DEV->CPU_I_REMAP_BDADDR[remap_idx], src_addr);

	return 0;
}


__HW_STATIC_INLINE
void LL_REMAP_RAM_GetIBusMap(uint32_t idx, uint32_t *addr, uint32_t *len)
{
	*addr = HW_GET_VAL(SYSCFG_DEV->CPU_I_REMAP_BDADDR[idx]);
	if (idx == 0) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[0], CPU_I_REMAP0_SIZE_Msk, CPU_I_REMAP0_SIZE_Pos);
	} else if (idx == 1) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[0], CPU_I_REMAP1_SIZE_Msk, CPU_I_REMAP1_SIZE_Pos);
	} else if (idx == 2) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[1], CPU_I_REMAP2_SIZE_Msk, CPU_I_REMAP2_SIZE_Pos);
	} else if (idx == 3) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPU_I_REMAP_SIZE[1], CPU_I_REMAP3_SIZE_Msk, CPU_I_REMAP3_SIZE_Pos);
	} else {
		*addr = 0;
		*len = 0;
	}

	*len <<= 7;
}

__HW_STATIC_INLINE
void LL_REMAP_RAM_SetCPUSDBusMap(uint32_t idx, uint32_t addr, uint32_t len)
{
	HW_SET_VAL(SYSCFG_DEV->CPUS_D_REMAP_BDADDR[idx], addr);
	if (idx == 0) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[0], CPUS_D_REMAP0_SIZE_Msk, CPUS_D_REMAP0_SIZE_Pos, (len >> 7));
	} else if (idx == 1) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[0], CPUS_D_REMAP1_SIZE_Msk, CPUS_D_REMAP1_SIZE_Pos, (len >> 7));
	} else if (idx == 2) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[1], CPUS_D_REMAP2_SIZE_Msk, CPUS_D_REMAP2_SIZE_Pos, (len >> 7));
	}
}

__HW_STATIC_INLINE
uint32_t LL_REMAP_SRAM_SetCPUSDBus(uint32_t dst_addr, uint32_t src_addr, uint32_t addr_size)
{
	uint8_t remap_idx = 0;

	addr_size >>= 7;
	if (dst_addr == 0x10500000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[0], CPUS_D_REMAP0_SIZE_Msk, CPUS_D_REMAP0_SIZE_Pos, addr_size);
		remap_idx = 0;
	} else if (dst_addr == 0x10540000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[0], CPUS_D_REMAP1_SIZE_Msk, CPUS_D_REMAP1_SIZE_Pos, addr_size);
		remap_idx = 1;
	} else if (dst_addr == 0x10580000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[1], CPUS_D_REMAP2_SIZE_Msk, CPUS_D_REMAP2_SIZE_Pos, addr_size);
		remap_idx = 2;
	} else {
		return 1;
	}

	HW_SET_VAL(SYSCFG_DEV->CPUS_D_REMAP_BDADDR[remap_idx], src_addr);

	return 0;
}

__HW_STATIC_INLINE
void LL_REMAP_RAM_GetCPUSDBusMap(uint32_t idx, uint32_t *addr, uint32_t *len)
{
	*addr = HW_GET_VAL(SYSCFG_DEV->CPUS_D_REMAP_BDADDR[idx]);
	if (idx == 0) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[0], CPUS_D_REMAP0_SIZE_Msk, CPUS_D_REMAP0_SIZE_Pos);
	} else if (idx == 1) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[0], CPUS_D_REMAP1_SIZE_Msk, CPUS_D_REMAP1_SIZE_Pos);
	} else if (idx == 2) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPUS_D_REMAP_SIZE[1], CPUS_D_REMAP2_SIZE_Msk, CPUS_D_REMAP2_SIZE_Pos);
	} else {
		*addr = 0;
		*len = 0;
	}

	*len <<= 7;
}

__HW_STATIC_INLINE
void LL_REMAP_RAM_SetCPUFDBusMap(uint32_t idx, uint32_t addr, uint32_t len)
{
	HW_SET_VAL(SYSCFG_DEV->CPUF_D_REMAP_BDADDR[idx], addr);
	if (idx == 0) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[0], CPUF_D_REMAP0_SIZE_Msk, CPUF_D_REMAP0_SIZE_Pos, (len >> 7));
	} else if (idx == 1) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[0], CPUF_D_REMAP1_SIZE_Msk, CPUF_D_REMAP1_SIZE_Pos, (len >> 7));
	} else if (idx == 2) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[1], CPUF_D_REMAP2_SIZE_Msk, CPUF_D_REMAP2_SIZE_Pos, (len >> 7));
	}
}

__HW_STATIC_INLINE
uint32_t LL_REMAP_SRAM_SetCPUFDBus(uint32_t dst_addr, uint32_t src_addr, uint32_t addr_size)
{
	uint8_t remap_idx = 0;

	addr_size >>= 7;
	if (dst_addr == 0x10500000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[0], CPUF_D_REMAP0_SIZE_Msk, CPUF_D_REMAP0_SIZE_Pos, addr_size);
		remap_idx = 0;
	} else if (dst_addr == 0x10540000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[0], CPUF_D_REMAP1_SIZE_Msk, CPUF_D_REMAP1_SIZE_Pos, addr_size);
		remap_idx = 1;
	} else if (dst_addr == 0x10580000) {
		HW_SET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[1], CPUF_D_REMAP2_SIZE_Msk, CPUF_D_REMAP2_SIZE_Pos, addr_size);
		remap_idx = 2;
	} else {
		return 1;
	}

	HW_SET_VAL(SYSCFG_DEV->CPUF_D_REMAP_BDADDR[remap_idx], src_addr);

	return 0;
}

__HW_STATIC_INLINE
void LL_REMAP_RAM_GetCPUFDBusMap(uint32_t idx, uint32_t *addr, uint32_t *len)
{
	*addr = HW_GET_VAL(SYSCFG_DEV->CPUF_D_REMAP_BDADDR[idx]);
	if (idx == 0) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[0], CPUF_D_REMAP0_SIZE_Msk, CPUF_D_REMAP0_SIZE_Pos);
	} else if (idx == 1) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[0], CPUF_D_REMAP1_SIZE_Msk, CPUF_D_REMAP1_SIZE_Pos);
	} else if (idx == 2) {
		*len = HW_GET_MSK_VAL(SYSCFG_DEV->CPUF_D_REMAP_SIZE[1], CPUF_D_REMAP2_SIZE_Msk, CPUF_D_REMAP2_SIZE_Pos);
	} else {
		*addr = 0;
		*len = 0;
	}

	*len <<= 7;
}

__HW_STATIC_INLINE
void LL_REMAP_RAM_EnableDataCubeMapToSRAMA(void)
{
	HW_SET_BIT(SYSCFG_DEV->DATACUBE_REMAP_EN, DATACUBE_REMAP_EN_Msk);
}

__HW_STATIC_INLINE
void LL_REMAP_RAM_DisableDataCubeMapToSRAMA(void)
{
	HW_CLR_BIT(SYSCFG_DEV->DATACUBE_REMAP_EN, DATACUBE_REMAP_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_REMAP_RAMP_IsEnabledDataCubeMapToSRAMA(void)
{
	return HW_TEST_BIT(SYSCFG_DEV->DATACUBE_REMAP_EN, DATACUBE_REMAP_EN_Pos);
}

__HW_STATIC_INLINE
void LL_REMAP_RAM_SetDataCubeMapAddr(uint32_t addr)
{
	HW_SET_VAL(SYSCFG_DEV->DATACUBE_REMAP_BDADDR, addr);
}

__HW_STATIC_INLINE
uint32_t LL_REMAP_RAM_GetDataCubeMapAddr(void)
{
	return HW_GET_VAL(SYSCFG_DEV->DATACUBE_REMAP_BDADDR);
}
/**
  * @}
  */



/** @defgroup utils-REMAP-XIP
  * @{
  */
__HW_STATIC_INLINE
void LL_REMAP_XIP_EnableMap(void)
{
	HW_SET_BIT(SWCFG_DEV->SWCFG_XIP_REMAP_EN, XIP_HADDR_REMAP_EN_Pos);
}

__HW_STATIC_INLINE
void LL_REMAP_XIP_DisableMap(void)
{
	HW_CLR_BIT(SWCFG_DEV->SWCFG_XIP_REMAP_EN, XIP_HADDR_REMAP_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_REMAP_XIP_IsEnabledMap(void)
{
	return HW_TEST_BIT(SWCFG_DEV->SWCFG_XIP_REMAP_EN, XIP_HADDR_REMAP_EN_Pos);
}


#define PSIC_XIP_REMAP_GROUP_OFF 2
__HW_STATIC_INLINE
void LL_REMAP_XIP_Init(void)
{
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_START[0], 0x08000000);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP[0], 0x0);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_SIZE[0], (0xFFF000 >> 12));

	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_START[1], 0x0c000000);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP[1], 0x0);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_SIZE[1], (0xFFF000 >> 12));

	for (int i = PSIC_XIP_REMAP_GROUP_OFF; i < PSIC_XIP_REMAP_GROUP_MAX_NUM; i++) {
		HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_START[i], 0xFFFFFFFF);
		HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP[i], 0xFFFFFFFF);
		HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_SIZE[i], 0);
	}

}

__HW_STATIC_INLINE
void LL_REMAP_XIP_SetMap(uint32_t idx, uint32_t pre_addr, uint32_t lst_addr, uint32_t len)
{
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_START[idx + PSIC_XIP_REMAP_GROUP_OFF], pre_addr);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP[idx + PSIC_XIP_REMAP_GROUP_OFF], lst_addr);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_SIZE[idx + PSIC_XIP_REMAP_GROUP_OFF], (len >> 12));
}

__HW_STATIC_INLINE
void LL_REMAP_XIP_GetMap(uint32_t idx, uint32_t *pre_addr, uint32_t *lst_addr, uint32_t *len)
{
	*pre_addr = HW_GET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_START[idx + PSIC_XIP_REMAP_GROUP_OFF]);
	*lst_addr = HW_GET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP[idx + PSIC_XIP_REMAP_GROUP_OFF]);
	*len = HW_GET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_SIZE[idx + PSIC_XIP_REMAP_GROUP_OFF] << 12);
}

__HW_STATIC_INLINE
void LL_REMAP_XIP_SetReMap(uint32_t idx, uint32_t pre_addr, uint32_t lst_addr, uint32_t len)
{
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_START[idx], pre_addr);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP[idx], lst_addr);
	HW_SET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_SIZE[idx], len);
}

__HW_STATIC_INLINE
void LL_REMAP_XIP_GetReMap(uint32_t idx, uint32_t *pre_addr, uint32_t *lst_addr, uint32_t *len)
{
	*pre_addr = HW_GET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_START[idx]);
	*lst_addr = HW_GET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP[idx]);
	*len = HW_GET_VAL(SWCFG_DEV->SWCFG_XIP_REMAP_SIZE[idx]);
}


/**
  * @}
  */



/** @defgroup utils-DEENC
  * @{
  */
__HW_STATIC_INLINE
void LL_DEENC_SetAddrArea(uint32_t start_addr, uint32_t end_addr)
{
	HW_SET_VAL(SYSCFG_DEV->CPUF_DEENC_START_ADDR, start_addr);
	HW_SET_VAL(SYSCFG_DEV->CPUF_DEENC_END_ADDR, end_addr << 12);
}

__HW_STATIC_INLINE
void LL_DEENC_GetAddrArea(uint32_t *start_addr, uint32_t *end_addr)
{
	*start_addr = HW_GET_VAL(SYSCFG_DEV->CPUF_DEENC_START_ADDR);
	*end_addr = HW_GET_VAL(SYSCFG_DEV->CPUF_DEENC_END_ADDR);
}

__HW_STATIC_INLINE
void LL_DEENC_SetKey(uint32_t idx, uint32_t key)
{
	HW_SET_VAL(SYSCFG_DEV->CPUF_DEENC_KEY[idx], key);
}

__HW_STATIC_INLINE
uint32_t LL_DEENC_GetKey(uint32_t       idx)
{
	return HW_GET_VAL(SYSCFG_DEV->CPUF_DEENC_KEY[idx]);
}

__HW_STATIC_INLINE
void LL_DEENC_EnableCPUS(void)
{
	HW_SET_BIT(SYSCFG_DEV->CPUF_DEENC_EN, CPUS_DEENC_EN_Msk);
}

__HW_STATIC_INLINE
void LL_DEENC_DisableCPUS(void)
{
	HW_CLR_BIT(SYSCFG_DEV->CPUF_DEENC_EN, CPUS_DEENC_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_DEENC_IsEnabledCPUS(void)
{
	return HW_TEST_BIT(SYSCFG_DEV->CPUF_DEENC_EN, CPUS_DEENC_EN_Pos);
}

__HW_STATIC_INLINE
void LL_DEENC_EnableCPUF(void)
{
	HW_SET_BIT(SYSCFG_DEV->CPUF_DEENC_EN, CPUF_DEENC_EN_Msk);
}

__HW_STATIC_INLINE
void LL_DEENC_DisableCPUF(void)
{
	HW_CLR_BIT(SYSCFG_DEV->CPUF_DEENC_EN, CPUF_DEENC_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_DEENC_IsEnabledCPUF(void)
{
	return HW_TEST_BIT(SYSCFG_DEV->CPUF_DEENC_EN, CPUF_DEENC_EN_Pos);
}

/**
  * @}
  */




/** @defgroup utils-SYS
  * @{
  */
__HW_STATIC_INLINE
void LL_SYS_ForceUpSys0(void)
{
	HW_SET_MSK(PMU_DEV->PMU_FORCE_WUP_CTRL, FORCE_WUP_SYS0_Msk);
}

__HW_STATIC_INLINE
void LL_SYS_ForceUpSys1(void)
{
	HW_SET_MSK(PMU_DEV->PMU_FORCE_WUP_CTRL, FORCE_WUP_SYS1_Msk);
}

__HW_STATIC_INLINE
void LL_SYS_ForceDownSys0(void)
{
	HW_SET_MSK(PMU_DEV->PMU_FORCE_DOWN_CTRL, FORCE_DWN_SYS0_Msk);
}

__HW_STATIC_INLINE
void LL_SYS_ForceDownSys1(void)
{
	HW_SET_MSK(PMU_DEV->PMU_FORCE_DOWN_CTRL, FORCE_DWN_SYS1_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_Sys0IsPowerUpState(void)
{
  	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, SYS0_POWERED_UP_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_Sys1IsPowerUpState(void)
{
  	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, SYS1_POWERED_UP_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_Sys0IsPowerDownState(void)
{
  	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, SYS0_POWERED_DWN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_Sys1IsPowerDownState(void)
{
  	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, SYS1_POWERED_DWN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_SWIsPowerUpReady(void)
{
  	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, SW1_READY_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_IsSys0PowerUpSW(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, SYS0_SW_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_IsSys1PowerUpSW(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_STATUS, SYS1_SW_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_SYS_GetSys0ResetSource(void)
{
	return CCMU2_DEV->CCMU2_CPUS_RST_FLAG;
}

__HW_STATIC_INLINE
uint32_t LL_SYS_GetSys1ResetSource(void)
{
	return CCMU2_DEV->CCMU2_CPUF_RST_FLAG;
}

__HW_STATIC_INLINE
uint32_t LL_SYS_GetCurrentSysId(void)
{
	uint32_t tmp;
	uint32_t cpu_id;

	__ASM volatile("csrr %0, mcpuid\n\tcsrr %1, mcpuid\n\tcsrr %2, mcpuid\n\t" : "=r"(tmp), "=r"(cpu_id), "=r"(tmp));

	if (cpu_id == 0x120C2000) {
		return 0;
	} else {
		return 1;
	}
}
/**
  * @}
  */




/** @defgroup utils-SRAMA
  * @{
  */
__HW_STATIC_INLINE
void LL_SRAMA_Share(uint32_t share)
{
	HW_SET_MSK_VAL(PMU_DEV->PMU_SRAMA_SHARE, SRAMA_MMW_SHARE_EN_Msk, SRAMA_MMW_SHARE_EN_Pos, share);
}

__HW_STATIC_INLINE
void LL_SRAMA_EnableRetentionOnSys0(uint32_t sram_msk)
{
	HW_SET_MSK(PMU_DEV->SYS0_SRAM_SW_CTRL, sram_msk);
}

__HW_STATIC_INLINE
void LL_SRAMA_DisableRetentionOnSys0(uint32_t sram_msk)
{
	HW_CLR_MSK(PMU_DEV->SYS0_SRAM_SW_CTRL, sram_msk);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMA_GetRetentionOnSys0(void)
{
	return HW_GET_VAL(PMU_DEV->SYS0_SRAM_SW_CTRL);
}

__HW_STATIC_INLINE
void LL_SRAMA_EnableRetentionOnSys1(uint32_t sram_msk)
{
	HW_SET_MSK(PMU_DEV->SYS1_SRAM_SW_CTRL, sram_msk);
}

__HW_STATIC_INLINE
void LL_SRAMA_DisableRetentionOnSys1(uint32_t sram_msk)
{
	HW_CLR_MSK(PMU_DEV->SYS1_SRAM_SW_CTRL, sram_msk);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMA_GetRetentionOnSys1(void)
{
	return HW_GET_VAL(PMU_DEV->SYS1_SRAM_SW_CTRL);
}

__HW_STATIC_INLINE
void LL_SRAMB_EnablePwrUpWithSys0(void)
{
	HW_SET_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS0_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_DisablePwrUpWithSys0(void)
{
	HW_CLR_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS0_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_IsEnabledPwrUpWithSys0(void)
{
	return HW_TEST_BIT(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS0_Pos);
}

__HW_STATIC_INLINE
void LL_SRAMB_EnablePwrUpWithSys1(void)
{
	HW_SET_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS1_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_DisablePwrUpWithSys1(void)
{
	HW_CLR_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS1_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_IsEnabledPwrUpWithSys1(void)
{
	return HW_TEST_BIT(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS1_Pos);
}

__HW_STATIC_INLINE
void LL_SRAMB_EnablePwrUpForce(void)
{
	HW_SET_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, FORCE_SWSRAMTOP_SW_EN_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_DisablePwrUpForce(void)
{
	HW_CLR_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, FORCE_SWSRAMTOP_SW_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_IsEnabledPwrUpForce(void)
{
	return HW_TEST_BIT(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, FORCE_SWSRAMTOP_SW_EN_Pos);
}


__HW_STATIC_INLINE
void LL_SRAMB_SetPwrUpSize(uint32_t msk)
{
	/* all sramb must be powered on together  */
	HW_SET_MSK_VAL(MISCREG2_DEV->MISCREG2_SRAMB_ON, SRAMB_PU_Msk, SRAMB_PU_Pos, 0x1F);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_GetPwrUpSize(void)
{
	return HW_GET_MSK_VAL(MISCREG2_DEV->MISCREG2_SRAMB_ON, SRAMB_PU_Msk, SRAMB_PU_Pos);
}

__HW_STATIC_INLINE
void LL_SRAMB_SetRetSize(uint32_t msk)
{
	HW_SET_MSK_VAL(MISCREG2_DEV->MISCREG2_SRAMB_RET_ON, SRAMB_RET_Msk, SRAMB_RET_Pos, 0x1F);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_GetRetSize(void)
{
	return HW_GET_MSK_VAL(MISCREG2_DEV->MISCREG2_SRAMB_RET_ON, SRAMB_RET_Msk, 0x1F);
}

__HW_STATIC_INLINE
void LL_SRAMB_SetToCPUSize(uint32_t msk)
{
	HW_SET_MSK_VAL(SYSCFG_DEV->SYSCFG_CDMA_DATA_CUBE_SEL, CDMA_DATA_CUBE_SEL_Msk, CDMA_DATA_CUBE_SEL_Pos, msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_SetDatacubeRemap(uint32_t addr)
{
	HW_SET_VAL(SYSCFG_DEV->DATACUBE_REMAP_BDADDR, addr);
}

__HW_STATIC_INLINE
void LL_SRAMB_EnableDatacubeRemap(void)
{
	HW_SET_MSK_VAL(SYSCFG_DEV->DATACUBE_REMAP_EN, DATACUBE_REMAP_EN_Msk, DATACUBE_REMAP_EN_Pos, 1);
}

__HW_STATIC_INLINE
void LL_SRAMB_DisableDatacubeRemap(void)
{
	HW_SET_MSK_VAL(SYSCFG_DEV->DATACUBE_REMAP_EN, DATACUBE_REMAP_EN_Msk, DATACUBE_REMAP_EN_Pos, 0);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_GetToCPUSize(void)
{
	return HW_GET_MSK(SYSCFG_DEV->SYSCFG_CDMA_DATA_CUBE_SEL, CDMA_DATA_CUBE_SEL_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_FollowSys0PwrUp(void)
{
	HW_SET_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS0_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_Settxrf_ldo(uint32_t val)
{
    HW_SET_MSK_VAL(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, TXRF_LDO_TRIM_Msk, TXRF_LDO_TRIM_Pos, val);
}


__HW_STATIC_INLINE
void LL_SRAMB_DisableFollowSys0PwrUp(void)
{
	HW_CLR_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, SWSRAMTOP_ON_FLOW_SYS0_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_ForcePwrUp(void)
{
	HW_SET_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, FORCE_SWSRAMTOP_ACTIVE_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_DisableForcePwrUp(void)
{
	HW_CLR_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, FORCE_SWSRAMTOP_ACTIVE_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_IsReady(void)
{
	return HW_TEST_BIT(MISCREG2_DEV->MISCREG2_MMW_STATUS, SWSRAMTOP_STATUS_Pos);
}

__HW_STATIC_INLINE
void LL_SRAMB_EnableToCPU(void)
{
	HW_SET_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, MMW_SRAM_ON_Msk);
}

__HW_STATIC_INLINE
void LL_SRAMB_DisableToCPU(void)
{
	HW_SET_MSK(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, MMW_SRAM_ON_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_SRAMB_IsEnabledToCPU(void)
{
	return HW_TEST_BIT(MISCREG2_DEV->MISCREG2_MMW_POWERCTRL, MMW_SRAM_ON_Pos);
}

/**
  * @}
  */




/** @defgroup utils-WAKEUP
  * @{
  */
__HW_STATIC_INLINE
void LL_WKUP_EnableOnSys0(uint32_t msk)
{
	HW_CLR_MSK(PMU_DEV->PMU_SYS0_WAKEUP_C_MASK, msk);
}

__HW_STATIC_INLINE
void LL_WKUP_DisableOnSys0(uint32_t msk)
{
	HW_SET_MSK(PMU_DEV->PMU_SYS0_WAKEUP_C_MASK, msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_IsEnabledOnSys0(uint32_t msk)
{
	return HW_GET_MSK(PMU_DEV->PMU_SYS0_WAKEUP_C_MASK, msk);
}

__HW_STATIC_INLINE
void LL_WKUP_EnableOnSys1(uint32_t msk)
{
	HW_CLR_MSK(PMU_DEV->PMU_SYS1_WAKEUP_C_MASK, msk);
}

__HW_STATIC_INLINE
void LL_WKUP_DisableOnSys1(uint32_t msk)
{
	HW_SET_MSK(PMU_DEV->PMU_SYS1_WAKEUP_C_MASK, msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_IsEnabledOnSys1(uint32_t msk)
{
	return HW_GET_MSK(PMU_DEV->PMU_SYS1_WAKEUP_C_MASK, msk);
}

__HW_STATIC_INLINE
void LL_WKUP_EnableSrc(uint32_t src_msk, uint32_t cpu_id)
{
    if (cpu_id) {
        LL_WKUP_EnableOnSys1(src_msk);
    } else {
        LL_WKUP_EnableOnSys0(src_msk);
    }
}

__HW_STATIC_INLINE
void LL_WKUP_DisableSrc(uint32_t src_msk, uint32_t cpu_id)
{
    if (cpu_id) {
        LL_WKUP_DisableOnSys1(src_msk);
    } else {
        LL_WKUP_DisableOnSys0(src_msk);
    }
}

__HW_STATIC_INLINE
void LL_WKUP_UART_Reset(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_RESETN_EN_Msk);
	HW_SET_MSK(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_RESETN_EN_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_UART_Enable(void)
{
	HW_SET_MSK(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_PIN_EN_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_UART_Disable(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_PIN_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_UART_IsEnabled(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_PIN_EN_Pos);
}

__HW_STATIC_INLINE
void LL_WKUP_UART_SetTrget(uint32_t cnt)
{
	HW_SET_MSK_VAL(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_TARGET_Msk, UART_WUP_TARGET_Pos, cnt);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_UART_GetTrget(void)
{
	return HW_GET_MSK_VAL(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_TARGET_Msk, UART_WUP_TARGET_Pos);
}

__HW_STATIC_INLINE
void LL_WKUP_UART_SetPin(uint32_t pin)
{
	HW_SET_MSK_VAL(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_PIN_SEL_Msk, UART_WUP_PIN_SEL_Pos, pin);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_UART_GetPin(void)
{
	return HW_GET_MSK_VAL(PMU_DEV->PMU_UART_WUP_CTRL, UART_WUP_PIN_SEL_Msk, UART_WUP_PIN_SEL_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_UART_GetFlag(void)
{
	return HW_GET_MSK(PMU_DEV->PMU_WUP_FLAG, UART_WUP_FLAG_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_UART_ClrFlag(void)
{
	return HW_SET_MSK(PMU_DEV->PMU_WUP_FLAG, UART_WUP_FLAG_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_Reset(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_RESETN_EN_Msk);
	HW_SET_MSK(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_RESETN_EN_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_Enable(void)
{
	HW_SET_MSK(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_PIN_EN_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_Disable(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_PIN_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_I2C_IsEnabled(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_PIN_EN_Pos);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_EnableSCL(void)
{
	HW_SET_MSK(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_EN_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_DisableSCL(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_I2C_IsEnabledSCL(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_WUP_EN_Pos);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_SetSlaveAddr(uint32_t addr)
{
	HW_SET_MSK_VAL(PMU_DEV->PMU_IIC_WUP_CTRL, WUP_IIC_SLAVE_ADDR_Msk, WUP_IIC_SLAVE_ADDR_Pos, addr);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_I2C_GetSlaveAddr(void)
{
	return HW_GET_MSK_VAL(PMU_DEV->PMU_IIC_WUP_CTRL, WUP_IIC_SLAVE_ADDR_Msk, WUP_IIC_SLAVE_ADDR_Pos);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_SetSDAPin(uint32_t pin)
{
	HW_SET_MSK_VAL(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_SDA_WUP_PIN_SEL_Msk, IIC_SDA_WUP_PIN_SEL_Pos, pin);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_I2C_GetSDAPin(void)
{
	return HW_GET_MSK_VAL(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_SDA_WUP_PIN_SEL_Msk, IIC_SDA_WUP_PIN_SEL_Pos);
}

__HW_STATIC_INLINE
void LL_WKUP_I2C_SetSCLPin(uint32_t pin)
{
	HW_SET_MSK_VAL(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_SCL_WUP_PIN_SEL_Msk, IIC_SCL_WUP_PIN_SEL_Pos, pin);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_I2C_GetSCLPin(void)
{
	return HW_GET_MSK_VAL(PMU_DEV->PMU_IIC_WUP_CTRL, IIC_SCL_WUP_PIN_SEL_Msk, IIC_SCL_WUP_PIN_SEL_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_I2C_GetFlag(void)
{
	return HW_GET_MSK(PMU_DEV->PMU_WUP_FLAG, IIC_WUP_FLAG_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_I2C_ClrFlag(void)
{
	return HW_SET_MSK(PMU_DEV->PMU_WUP_FLAG, IIC_WUP_FLAG_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_SPI_Reset(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_SPI_WUP_CTRL, SPI_WUP_RESETN_EN_Msk);
	HW_SET_MSK(PMU_DEV->PMU_SPI_WUP_CTRL, SPI_WUP_RESETN_EN_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_SPI_Enable(void)
{
	HW_SET_MSK(PMU_DEV->PMU_SPI_WUP_CTRL, SPI_WUP_PIN_EN_Msk);
}

__HW_STATIC_INLINE
void LL_WKUP_SPI_Disable(void)
{
	HW_CLR_MSK(PMU_DEV->PMU_SPI_WUP_CTRL, SPI_WUP_PIN_EN_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_SPI_IsEnabled(void)
{
	return HW_TEST_BIT(PMU_DEV->PMU_SPI_WUP_CTRL, SPI_WUP_PIN_EN_Pos);
}

__HW_STATIC_INLINE
void LL_WKUP_SPI_SetTrget(uint32_t cnt)
{
	HW_SET_MSK_VAL(PMU_DEV->PMU_SPI_WUP_CTRL, SPI_WUP_TARGET_Msk, SPI_WUP_TARGET_Pos, cnt);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_SPI_GetTrget(void)
{
	return HW_GET_MSK_VAL(PMU_DEV->PMU_SPI_WUP_CTRL, SPI_WUP_TARGET_Msk, SPI_WUP_TARGET_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_SPI_GetFlag(void)
{
	return HW_GET_MSK(PMU_DEV->PMU_WUP_FLAG, SPI_WUP_FLAG_Msk);
}

__HW_STATIC_INLINE
uint32_t LL_WKUP_SPI_ClrFlag(void)
{
	return HW_SET_MSK(PMU_DEV->PMU_WUP_FLAG, SPI_WUP_FLAG_Msk);
}
/**
  * @}
  */

/** @defgroup utils-WATCHDOG RESET
  * @{
  */
__HW_STATIC_INLINE
void LL_WDG0_SetResetPatition(uint32_t patition_id)
{
    HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_WDG_RST_CTRL, WDG0_RST_CTRL_Msk, WDG0_RST_CTRL_PosL, patition_id);
}

__HW_STATIC_INLINE
uint32_t LL_WDG0_GetResetPatition(void)
{
    return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_WDG_RST_CTRL, WDG0_RST_CTRL_Msk, WDG0_RST_CTRL_PosL);
}

__HW_STATIC_INLINE
void LL_WDG1_SetResetPatition(uint32_t patition_id)
{
    HW_SET_MSK_VAL(CCMU2_DEV->CCMU2_WDG_RST_CTRL, WDG1_RST_CTRL_Msk, WDG1_RST_CTRL_PosL, patition_id);
}

__HW_STATIC_INLINE
uint32_t LL_WDG1_GetResetPatition(void)
{
    return HW_GET_MSK_VAL(CCMU2_DEV->CCMU2_WDG_RST_CTRL, WDG1_RST_CTRL_Msk, WDG1_RST_CTRL_PosL);
}
/**
  * @}
  */
__HW_STATIC_INLINE
void LL_H2H_Close(void)
{
    HW_SET_BIT(SYSCFG_DEV->SYSCFG_CPUF_H2H_BYPASS_CTRL, 0);
}



__HW_STATIC_INLINE
uint32_t LL_Delay(uint32_t value, uint32_t unit, uint32_t cpu_freq)
{
	uint32_t ms_cnt = 0;
	uint32_t us_cnt = 0;
	uint32_t ms_tick = 0;
	uint32_t us_tick = 0;
	uint32_t cycle_tick = 0;

	switch (unit) {
	case LL_DELAY_CYCLE:
		cycle_tick = value;
		break;

	case LL_DELAY_US:
		ms_cnt = value / 1000;
		if (ms_cnt > 0) {
			ms_tick = cpu_freq / 1000;
		}

		us_cnt = value % 1000;
		if (us_cnt) {
			us_tick = cpu_freq / 1000;
			us_tick /= 1000;
		}
		break;

	case LL_DELAY_MS:
		ms_tick = cpu_freq / 1000;
		ms_cnt = value;
		break;

	default:
		return 1;
	}


	uint32_t coret_ie = CLIC->CLICINT[7].IE;

	for (uint32_t idx = 0; idx < ms_cnt; idx++) {
		CLIC->CLICINT[7].IE = 0;
		CORET->MTIMECMP = CORET->MTIME + ms_tick;
		CLIC->CLICINT[7].IP = 0;
		while (CLIC->CLICINT[7].IP == 0) {
		}
	}

	for (uint32_t idx = 0; idx < us_cnt; idx++) {
		CLIC->CLICINT[7].IE = 0;
		CORET->MTIMECMP = CORET->MTIME + us_tick;
		CLIC->CLICINT[7].IP = 0;
		while (CLIC->CLICINT[7].IP == 0) {
		}
	}

	if (cycle_tick > 0) {
		CLIC->CLICINT[7].IE = 0;
		CORET->MTIMECMP = CORET->MTIME + us_tick;
		CLIC->CLICINT[7].IP = 0;
		while (CLIC->CLICINT[7].IP == 0) {
		}
	}

	CLIC->CLICINT[7].IE = coret_ie;


	return 0;
}


__HW_STATIC_INLINE
void LL_Delay_Start(uint32_t value, uint32_t unit, uint32_t cpu_freq)
{
	unsigned long long delta_tick = 0;

	switch (unit) {
	case LL_DELAY_CYCLE:
		delta_tick = value;
		break;

	case LL_DELAY_US:
		cpu_freq /= 1000;
		delta_tick = value * cpu_freq;
		break;

	case LL_DELAY_MS:
		delta_tick = value * cpu_freq;
		break;

	default:
		return ;
	}

	CLIC->CLICINT[7].IE = 0;
	CORET->MTIMECMP = CORET->MTIME + delta_tick;
	CLIC->CLICINT[7].IP = 0;
}


__HW_STATIC_INLINE
uint32_t LL_Delay_CheckDone(void)
{
	return CLIC->CLICINT[7].IP;
}


/** psic_ll_utils
  * @}
  */


/** PSIC_LL_Driver
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _PSIC_LL_UTILS_H */

/*
 **************************************************************************************************
 * (C) COPYRIGHT POSSUMIC Technology
 * END OF FILE
 */
