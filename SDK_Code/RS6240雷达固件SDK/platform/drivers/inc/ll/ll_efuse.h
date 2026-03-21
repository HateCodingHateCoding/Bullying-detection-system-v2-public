/**
 **************************************************************************************************
 * @file    psic_ll_efuse.h
 * @brief   efuse hadware define.
 * @attention
 *          Copyright (c) 2024 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */


/* Define to prevent recursive inclusion.
 * ------------------------------------------------------------------------------------------------
 */
#ifndef _PSIC_LL_EFUSE_H
#define _PSIC_LL_EFUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes.
 * ------------------------------------------------------------------------------------------------
 */

#include "hw_efuse.h"
#include "soc.h"
#include "types.h"

#define EFUSE_DEV                       ((efuse_dev_t *)  EFUSE_BASE_ADDR) /*!< 0x4001 D800 */
/** @addtogroup PSIC_LL_Driver
  * @{
  */


/** @defgroup EFUSE_LL EFUSE
  * @{
  */


/* Exported types.
 * ------------------------------------------------------------------------------------------------
 */


/**
  * @}
  */


/* Exported constants.
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup EFUSE_LL_Exported_Constants EFUSE Exported Constants
  * @{
  */

/** @defgroup EFUSE Mager EFUSE_PGMEN
  * @{
  */
#define LL_EFUSE_PGMEN_MAGIC_OPCODE					(0x50474D4D)
/**
  * @}
  */

/** @defgroup EFUSE Read Mode
  * @{
  */
#define LL_EFUSE_ONE_READ_MODE                           (0)
#define LL_EFUSE_ALL_READ_MODE                           (1)
/**
  * @}
  */


/* Exported functions.
 * ------------------------------------------------------------------------------------------------
 */

/** @defgroup EFUSE_LL_Exported_Functions EFUSE Exported Functions
  * @{
  */

/**
  * @brief  Enable efuse programming (0x0000 EFUSE_PGMEN)
  * @note   for efuse programming should set to 32’h50474D4D(“PGMM”，PGM Mode).
  * @note   programming done efuse will auto read and write into EFUSE_DATA.
  * @note   software should read the lowest bit when the value is zero means efuse program done .
  * @note   for efuse program key , software should check the scramble init code if programed or not,
  *         if not , key program should not program key , read KEY_STATUS to check key porogramed succeed or not.
  * @retval None
  */
__HW_STATIC_INLINE
void LL_EFUSE_EnableWrite(void)
{
	EFUSE_DEV->EFUSE_PGMEN = LL_EFUSE_PGMEN_MAGIC_OPCODE;
}

/**
* @brief  check if programming complete. (0x0000 EFUSE_PGMEN)
* @note   the lowest bit set to zero mean program data or key done
* @param  None
* @retval regiseter value masked with the lowest bit 0x01.
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_IsWriteDone(void)
{
	return (HW_TEST_BIT(EFUSE_DEV->EFUSE_PGMEN, EFUSE_PGMEN_Pos) == 0);
}

/**
* @brief  Eanble efuse read operation. (0x0004 EFUSE_RD_CTRL_REG)
* @note   config efuse read mode and read efuse addr before trigger read operation.
* @note   once efuse complete reading operation , read enable bit will be cleared.
* @param  None
* @retval regiseter value masked with the lowest bit 0x01.
*/
__HW_STATIC_INLINE
void LL_EFUSE_EnableRead(void)
{
	HW_SET_BIT(EFUSE_DEV->EFUSE_RD_CTRL_REG, EFUSE_RD_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_EFUSE_IsReadDone(void)
{
	return (HW_TEST_BIT(EFUSE_DEV->EFUSE_RD_CTRL_REG, EFUSE_RD_EN_Pos) == 0);
}

/**
* @brief  select efuse read operate mode .(0x0004 EFUSE_RD_CTRL_REG)
* @note   LL_EFUSE_ONE_READ_MODE use to read one value of EFUSE_ADDR efuse value from EFUSE_DATA.
* @note   LL_EFUSE_ONE_READ_MODE use to read ALL value of efuse values from EFUSE_REGBANK_DATA.
* @note   key efuse domain couldn't be read which will return all zero.
* @param  LL_EFUSE_ONE_READ_MODE or LL_EFUSE_ALL_READ_MODE.
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SelReadMode(uint32_t mode)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_RD_CTRL_REG, EFUSE_RD_MODE_SEL_Msk, EFUSE_RD_MODE_SEL_Pos, mode);
}

/**
* @brief  enable efuse key zone programming .(0x0008 EFUSE_ADDR)
* @note   key zone permit to read only so different with LL_EFUSE_EnablePGM function.
* @note   key programmed done will auto clear EFUSE_KEY_PGM_EN Bit.
* @param  None
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_EnableKeyWrite(void)
{
    HW_SET_BIT(EFUSE_DEV->EFUSE_ADDR, EFUSE_KEY_PGM_EN_Pos);
}

__HW_STATIC_INLINE
uint32_t LL_EFUSE_IsKeyWriteDone(void)
{
	return (HW_TEST_BIT(EFUSE_DEV->EFUSE_ADDR, EFUSE_KEY_PGM_EN_Pos) == 0);
}

/**
* @brief  set efuse programming addr or set efuse read addr .(0x0008 EFUSE_ADDR)
* @note   for programming , this is the first efuse programming addr.
* @note...for reading, this addr is the first efuse read addr.
* @note   addr unit is bit and read operation should be align with 8bit.
* @note   for key burning is different , see LL_EFUSE_EnableKeyPGM.
* @param  None
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetAddr(uint32_t addr)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_ADDR, EFUSE_ADDR_Msk, EFUSE_ADDR_Pos, addr);
}

/**
* @brief  set efuse programming wit 32bit value .(0x000c EFUSE_WDATA)
* @note   write with 32bit value into efuse EFUSE_ADDR (bit unit addr).
* @note...software couldn't set value into this register when programming not done.
* @param  32bit value to programming
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_WriteData(uint32_t value)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_WDATA, EFUSE_DATA_Msk, EFUSE_DATA_Pos, value);
}

/**
* @brief  efuse read data for 32bit(1 word) .(0x0018 EFUSE_RDATA)
* @note   efuse IP will read 4 times to generate 1 word.
* @note...efuse will clear EFUSE_RD_EN after read done.
* @param  None
* @retval None
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_ReadData(void)
{
    return HW_GET_MSK_VAL(EFUSE_DEV->EFUSE_RDATA, EFUSE_RDATA_Msk, EFUSE_RDATA_Pos);
}

/**
* @brief  time for TPGM .(0x0010 EFUSE_SETTINGS1)
* @note   time > 1.9 us .
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetTpgmTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS1, TAEN_TPGM_Msk, TAEN_TPGM_Pos, time);
}

/**
* @brief  time for THP .(0x0010 EFUSE_SETTINGS1)
* @note   time > 50 ns.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetThpTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS1, THP_A_Msk, THP_A_Pos, time);
}

/**
* @brief  time for TSPRD_THPRD .(0x0010 EFUSE_SETTINGS1)
* @note   time > 150 ns.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetTshprdTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS1, TSPRD_THPRD_Msk, TSPRD_THPRD_Pos, time);
}

/**
* @brief  time for TSRRD_THRRD .(0x0010 EFUSE_SETTINGS1)
* @note   time > 100 ns.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetTshrrdTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS1, TSRRD_THRRD_Msk, TSRRD_THRRD_Pos, time);
}

/**
* @brief  time for TRD .(0x0010 EFUSE_SETTINGS1)
* @note   time > 40 ns.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetTrdTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS1, TRD_Msk, TRD_Pos, time);
}

/**
* @brief  time for TAEN_TRD .(0x0010 EFUSE_SETTINGS1)
* @note   time > 35 ns.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetTaenTrdTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS1, TAEN_TRD_Msk, TAEN_TRD_Pos, time);
}

/**
* @brief  time for THR_A .(0x0010 EFUSE_SETTINGS1)
* @note   time > 10 ns.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetThrATiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS1, THR_A_Msk, THR_A_Pos, time);
}

/**
* @brief  time for T_PGM .(0x0014 EFUSE_SETTINGS2)
* @note   time rang 9us~11us.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetPgmTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS2, T_PGM_Msk, T_PGM_Pos, time);
}

/**
* @brief  time for TSP_PGM .(0x0014 EFUSE_SETTINGS2)
* @note   time > 100ns.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetTspPgmTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS2, TSP_PGM_Msk, TSP_PGM_Pos, time);
}

/**
* @brief  time for THP_PG_AVDD .(0x0014 EFUSE_SETTINGS2)
* @note   time > 1us.
* @note...fclock cycles depends on clock source such as 30MHz or 50Mhz Msi clock.
* @param  fclock cycles number
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_SetTspThpPgTiming(uint32_t time)
{
    HW_SET_MSK_VAL(EFUSE_DEV->EFUSE_SETTINGS2, TSP_THP_PG_AVDD_Msk, TSP_THP_PG_AVDD_Pos, time);
}

/**
* @brief  ecc err bit location .(0x0024 KEY_ECC_STATUS)
* @note   None.
* @note...None.
* @param  None
* @retval None
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_GetKeyEccErrLoc(void)
{
    return HW_GET_MSK_VAL(EFUSE_DEV->KEY_ECC_STATUS, KEY_ECC_ERR_LOC_Msk, KEY_ECC_ERR_LOC_Pos);
}

/**
* @brief  ecc verify and correct status .(0x0024 KEY_ECC_STATUS)
* @note   efuse IP will correct one bit only.
* @note...efuse status error when more than one bit error.
* @param  None
* @retval ecc status : zero means error
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_GetKeyEccStatus(void)
{
    return HW_GET_MSK_VAL(EFUSE_DEV->KEY_ECC_STATUS, KEY_ECC_ERR_FLAG_Msk, KEY_ECC_ERR_FLAG_Pos);
}

/**
* @brief  efuse burned status .(0x0030 EFUSE_BURNED_STATUS)
* @note   zero means efuse not burned .
* @note...not zero means efuse invaild for FT bad chip selection.
* @note...efuse burned nor not is unknowed when it's bad.
* @param  None
* @retval efuse burned status : not zero means error
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_GetBounedStatus(void)
{
    return HW_TEST_BIT(EFUSE_DEV->EFUSE_BURNED_STATUS, EFUSE_BURNED_Pos);
}


/**
* @brief  key burn err bit location .(0x0034 KEY_BURN_STATUS)
* @note   KEY_BURN_ERR_NUM > 2 .
* @note...None.
* @param  None
* @retval None
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_GetKeyErrLoc1(void)
{
    return HW_GET_MSK_VAL(EFUSE_DEV->KEY_BURN_STATUS, KEY_BURN_ERR_LOC1_Msk, KEY_BURN_ERR_LOC1_Pos);
}

/**
* @brief  key burn err bit location .(0x0034 KEY_BURN_STATUS)
* @note   KEY_BURN_ERR_NUM > 1 .
* @note...None.
* @param  None
* @retval None
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_GetKeyErrLoc0(void)
{
    return HW_GET_MSK_VAL(EFUSE_DEV->KEY_BURN_STATUS, KEY_BURN_ERR_LOC0_Msk, KEY_BURN_ERR_LOC0_Pos);
}

/**
* @brief  key burn err bit number .(0x0034 KEY_BURN_STATUS)
* @note   support less than 7 bit errors, more then 7 bit will return 7 bit error only.
* @param  None
* @retval None
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_GetKeyErrNum(void)
{
    return HW_GET_MSK_VAL(EFUSE_DEV->KEY_BURN_STATUS, KEY_BURN_ERR_NUM_Msk, KEY_BURN_ERR_NUM_Pos);
}


/**
* @brief  efuse key burned status .(0x0034 KEY_BURN_STATUS)
* @note   zero means efuse key had burned done but error bit unknowed .
* @note...not zero means efuse burned fail for scrambling code initial.
* @note...this bit will vail when EFUSE_PGMEN is done.
* @param  None
* @retval efuse key burned status : not zero means error
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_GetKeyBounedStatus(void)
{
    return HW_GET_MSK_VAL(EFUSE_DEV->KEY_BURN_STATUS, KEY_BURN_STATUS_Msk, KEY_BURN_STATUS_Pos);
}

/**
* @brief  wirte key value into efuse part 2 .(0x000c KEY_WDATA)
* @note   IP support less than 512bit Key but instant only 128bit keys.
* @note...only burn data prepare ready key burn will be begin to set EFUSE_PGMEN and EFUSE_KEY_EN.
* @note...when EFUSE_PGMEN and EFUSE_KEY_EN is 1, config will not success.
* @note   enable key burn (LL_EFUSE_EnableKeyPGM) then enable programming(LL_EFUSE_EnablePGM).
* @param  128bit value to programming
* @retval None
*/
__HW_STATIC_INLINE
void LL_EFUSE_WriteKeyValue(uint32_t *val, uint32_t len)
{
	for (int index = 0; index < len; index++) {
	    HW_SET_MSK_VAL(EFUSE_DEV->KEY_WDATA[index], KEY_VALUE_Msk, KEY_VALUE_Pos, val[index]);
	}
}

/**
* @brief  efuse read data for bank(128 words) .(0x0100h EFUSE_REGBANK_DATA)
* @note   read 128 words form efuse.
* @param  buf porinter
* @retval None
*/
__HW_STATIC_INLINE
uint32_t LL_EFUSE_ReadBankData(uint32_t idx)
{
	return HW_GET_MSK_VAL(EFUSE_DEV->EFUSE_REGBANK_DATA[idx], EFUSE_Msk, EFUSE_Pos);
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

#endif /* _PSIC_LL_EFUSE_H */

