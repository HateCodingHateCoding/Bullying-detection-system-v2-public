/**
 ******************************************************************************
 * @file      hal_dma.c
 * @brief     dma hadware abstract layer
 *            DMA HAL module driver function.
 * @verbatim  null
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes.
* ------------------------------------------------------------------------------------------------
*/
#include "irq.h"
#include "hal_dma.h"

#include "hal_board.h"
#include "hw_ccmu1.h"
#include "ll_utils.h"
#include "ll_splk.h"
#include "ll_rcc_bus.h"
#include "ll_rcc_dev.h"
#include "ll_dma.h"

#define LOG_MODULE                      "DMA"
#define LOG_LEVEL                       LEVEL_ERR
#include "log.h"

#define DMA_LOG_REG(reg) LOG_DBG("["#reg"]""0x%08x.\n", (uint32_t)reg)
#define DMA_LOG_ALL(dma, ch, msg) {                                      \
    LOG_DBG("%s\n", msg);                                                \
    LOG_DBG("reg base_address[0x%08x]  ", (uint32_t)dma);                \
    LOG_DBG("ch%d base_addr[0x%08x]\n", ch, (uint32_t)&dma->CHANNEL[ch]);\
    DMA_LOG_REG(dma->CHANNEL[ch].CTLL);                            \
    DMA_LOG_REG(dma->CHANNEL[ch].CTLH);                            \
    DMA_LOG_REG(dma->CHANNEL[ch].CFGL);                            \
    DMA_LOG_REG(dma->CHANNEL[ch].CFGH);                            \
    DMA_LOG_REG(dma->CHANNEL[ch].SAR);                             \
    DMA_LOG_REG(dma->CHANNEL[ch].DAR);                             \
    DMA_LOG_REG(dma->CHANNEL[ch].LLP);                             \
    DMA_LOG_REG(dma->CHANNEL[ch].SSTAT);                           \
    DMA_LOG_REG(dma->CHANNEL[ch].DSTAT);                           \
    DMA_LOG_REG(dma->CHANNEL[ch].SSTATAR);                         \
    DMA_LOG_REG(dma->CHANNEL[ch].DSTATAR);                         \
    DMA_LOG_REG(dma->CHANNEL[ch].SGR);                             \
    DMA_LOG_REG(dma->CHANNEL[ch].DSR);                             \
    DMA_LOG_REG(dma->RAWBLOCK);                                    \
    DMA_LOG_REG(dma->RAWTFR);                                      \
    DMA_LOG_REG(dma->RAWERR);                                      \
    DMA_LOG_REG(dma->MASKBLOCK);                                   \
    DMA_LOG_REG(dma->MASKTFR);                                     \
    DMA_LOG_REG(dma->MASKERR);                                     \
    DMA_LOG_REG(dma->CLEARBLOCK);                                  \
    DMA_LOG_REG(dma->CLEARTFR);                                    \
    DMA_LOG_REG(dma->CLEARERR);                                    \
    DMA_LOG_REG(dma->STATUSINT);                                   \
    DMA_LOG_REG(dma->DMAEN);                                       \
    DMA_LOG_REG(dma->CHEN);                                        \
    DMA_LOG_REG(dma->DMAREMAPCTRL);                                \
    DMA_LOG_REG(dma->LPTIME);                                      \
    LOG_DBG("\n");                                                 \
}

/* Private typedef.
 * ----------------------------------------------------------------------------
 */
typedef HAL_Callback_t DMA_EventCb_t;

/**
 * @brief
 */
typedef enum {
    DMA_TRANFMODE_BLOCK    =   (0x00U),
    DMA_TRANFMODE_LIST     =   (0x01U),
    DMA_TRANFMODE_SGDC     =   (0x02U),
    DMA_TRANFMODE_CYCLIC   =   (0x03U),
    DMA_TRANFMODE_NUM      =   (0x04U),
} dma_TranferMode_t;


/**
 * @brief
 */
typedef enum {
    DMA_STATE_IDLE         =   (0x0U),
    DMA_STATE_READY        =   (0x1U),
    DMA_STATE_BUSY         =   (0x2U),
} dma_ChannelState_t;


typedef enum {
    DMA_UNLOCK,
    DMA_LOCK,
} dma_Lock_t;


/**
 * @brief peripheral slot.
 */
typedef enum {
    PERIPH_UART0_RXDR      =   (0x00U),
    PERIPH_UART0_TXDR,
    PERIPH_UART1_RXDR,
    PERIPH_UART1_TXDR,
    PERIPH_UART2_RXDR,
    PERIPH_UART2_TXDR,
    PERIPH_IIC_DR,
    PERIPH_QSPI_DR,

    PERIPH_ADDR_TYPE_MAX,
    PERIPH_ERR_SLOT,
} PeriphSlot_t;

/**
 * @brief peripheral slot.
 */
typedef enum {
    DMA_MASTER_1           =   (0x00U),
    DMA_MASTER_2,
    DMA_MASTER_3,
    DMA_MASTER_4,

    DMA_MASTER_NUM,
    DMA_MASTER_NONE,
} dma_Master_t;



/**
 * @brief  DMA Bottom half Ctrl register.
 */
union ctrl_low_reg {
    uint32_t raw;
    struct {
        uint32_t int_en            :1;  /*!< Specifies channel interrupt Enable.*/
        uint32_t dst_width         :3;  /*!< Specifies the Destination Transfer Width.*/
        uint32_t src_width         :3;  /*!< Specifies the Source Transfer Width.*/
        uint32_t dst_inc           :2;  /*!< Specifies the Destination Address Increment.*/
        uint32_t src_inc           :2;  /*!< Specifies the Source Address Increment.*/
        uint32_t dst_msize         :3;  /*!< Specifies the Destination Burst Transaction Length.*/
        uint32_t src_msize         :3;  /*!< Specifies the Source Burst Transaction Length.*/
        uint32_t src_gather_en     :1;  /*!< Specifies Source gather enable.*/
        uint32_t dst_scatter_en    :1;  /*!< Specifies Destination scatter enable.*/
        uint32_t reverse1          :1;  /*!< reversed */
        uint32_t direction         :3;  /*!< Specifies Transfer Type and Flow Control.*/
        uint32_t dst_mst_sel       :2;  /*!< Specifies the Destination Master Select.*/
        uint32_t src_mst_sel       :2;  /*!< Specifies the Source Master Select.*/
        uint32_t llp_dst_en        :1;  /*!< Specifies Starting Destination Linked List Transfers*/
        uint32_t llp_src_en        :1;  /*!< Specifies Starting Source Linked List Transfer.*/
        uint32_t reverse2          :3;  /*!< reversed */
    } bits;
};

/**
 * @brief  DMA Top half Ctrl register.
 */
union ctrl_high_reg {
    uint32_t raw;
    struct {
        uint32_t block_size        :12;  /*!< Specifies Block Transfer Size, Max:4096(12 bits).*/
        uint32_t done_state        :1;   /*!< Specifies Done status bit. */
        uint32_t reverse1          :19;  /*!< reversed */
    } bits;
};

/**
 * @brief  DMA linker list item.
 *
 */
typedef struct dma_llItem {
    uint32_t src_addr;                       /**< Block source address. */
    uint32_t dst_addr;                       /**< Block destination address. */
    struct dma_llItem *pnextItem;           /*!< Specifies Pointer to next LLI. */
    union ctrl_low_reg  ctrl_low;
    union ctrl_high_reg ctrl_high;
    /* write backs for D/S STAT*/
    uint32_t src_statu;
    uint32_t dst_statu;
} DMA_LLItem_t;

typedef struct {
    uint32_t MASKBLOCK;     /*0x01cc,1、The contents of the Raw St*/
    uint32_t MASKTFR;       /*0x01d0,1、The contents of the Raw St*/
    uint32_t MASKERR;       /*0x01d4,1、The contents of the Raw St*/
    uint32_t DMAEN;
    uint32_t CHEN;          /* 0x01ec,1、This is the DW_ahb_dmac Ch*/
    uint32_t DMAREMAPCTRL;  /* 0x01f0,This register is used to con*/
}dma_reg_backup_t;

/**
 * @brief DMA Device structure.
 */
typedef struct {
    HAL_Dev_t                  device;

    uint32_t                   listItemNums;
    uint32_t                   doneBlocksNum;
    uint32_t                   reqChanNum;

    DMA_LLItem_t               *headllItem[DMA_CHANNEL_MAX_NUM];

    dma_Master_t               srcMaster[DMA_CHANNEL_MAX_NUM];
    dma_Master_t               dstMaster[DMA_CHANNEL_MAX_NUM];

    // HAL_Mutex                  cfgMutex[DMA_CHANNEL_MAX_NUM];

    dma_ChannelState_t         chanState[DMA_CHANNEL_MAX_NUM];
    DMA_ChannelConf_t          chanCfg[DMA_CHANNEL_MAX_NUM];
    DMA_EventCb_t              errCb[DMA_CHANNEL_MAX_NUM];
    DMA_EventCb_t              doneCb[DMA_CHANNEL_MAX_NUM];
    DMA_EventCb_t              blockCb[DMA_CHANNEL_MAX_NUM];
#if CONFIG_DMA_PM
    dma_reg_backup_t           reg_backup;
    dma_WakeupMode_t           wk_mode;
    HAL_Callback_t             suspend_cb[DMA_CHANNEL_MAX_NUM];
    HAL_Callback_t             resume_cb[DMA_CHANNEL_MAX_NUM];
#endif
} DMA_Device_t;



/* Private defines.
 * ----------------------------------------------------------------------------
 */
#define ISMEMORY                             (0x01U)
#define ISPERIPHERAL                         (0x02U)


#define FLASH_ADDR_REMAP_MSK                 (0x10000000UL)    /*!< FLASH MSK */
#define FLASH_ADDR_REMAP_MAX                 (0x01000000UL)    /*!< FLASH (16M) */

#define PSRAM_ADDR_REMAP_MSK                 (0x10000000UL)    /*!< PSRAM MSK */
#define PSRAM_ADDR_REMAP_MAX                 (0x01000000UL)    /*!< PSRAM (16M) */

/* Private macros.
 * ----------------------------------------------------------------------------
 */

/* Private variables.
 * ----------------------------------------------------------------------------
 */
DMA_Device_t *g_pdmaDevice = NULL;

/* Private function prototypes.
 * ----------------------------------------------------------------------------
 */
__dma_sram_text static uint8_t dma_Width2Byte(uint8_t width)
{
    switch (width) {
    case DMAC_BITWIDTH_8BIT:
        return 1;
    case DMAC_BITWIDTH_16BIT:
        return 2;
    case DMAC_BITWIDTH_32BIT:
        return 4;
    default:
        return 0;
    }
}

__dma_sram_text static bool dma_Is_SRAMb_Addr(uint32_t addr)
{
    return ((addr & SRAM_B_AREA_MSK) == SRAM_B_0_OFFSET) ? true : false;
}

__dma_sram_text static bool dma_Is_XIPOrPSRAM_Addr(uint32_t addr)
{
    uint32_t addr_mask = addr & 0xFFFFFFF;
    return (addr_mask >= (uint32_t)0x08000000) ? true : false;
}

__dma_sram_text static PeriphSlot_t dma_Is_PeripDataReg_Addr(uint32_t addr)
{
    PeriphSlot_t type;
    switch (addr) {
    case DMA_UART0_RX_ADDR:
        type = PERIPH_UART0_RXDR; break;
    case DMA_UART0_TX_ADDR:
        type = PERIPH_UART0_TXDR; break;
    case DMA_UART1_RX_ADDR:
        type = PERIPH_UART1_RXDR; break;
    case DMA_UART1_TX_ADDR:
        type = PERIPH_UART1_TXDR; break;
    case DMA_UART2_RX_ADDR:
        type = PERIPH_UART2_RXDR; break;
    case DMA_UART2_TX_ADDR:
        type = PERIPH_UART2_TXDR; break;
    case DMA_QSPI_TXRX_ADDR:
        type = PERIPH_QSPI_DR;    break;
    default:
        type = PERIPH_ERR_SLOT;   break;
    }
    return type;
}

__dma_sram_text static uint32_t dma_DeCodeAddr(uint32_t addr)
{
    uint32_t decode_addr = 0;

    if ((addr >= 0x00500000) && (addr <= (0x005C0000 + 0x00040000))) {
        if (addr < 0x00540000) {
            decode_addr = (addr - 0x00500000) + HW_GET_VAL(SYSCFG_DEV->CPU_I_REMAP_BDADDR[0]);
        } else if (addr < 0x00580000) {
            decode_addr = (addr - 0x00540000) + HW_GET_VAL(SYSCFG_DEV->CPU_I_REMAP_BDADDR[1]);
        } else if (addr < 0x005C0000) {
            decode_addr = (addr - 0x00580000) + HW_GET_VAL(SYSCFG_DEV->CPU_I_REMAP_BDADDR[2]);
        } else {
            decode_addr = (addr - 0x005C0000) + HW_GET_VAL(SYSCFG_DEV->CPU_I_REMAP_BDADDR[3]);
        }
        decode_addr |= 0x10000000;
    } else if ((addr >= 0x10500000) && (addr <= (0x10580000 + 0x00040000))) {
        if (LL_SYS_GetCurrentSysId()) {
            if (addr < 0x10540000) {
                decode_addr = (addr - 0x10500000) + HW_GET_VAL(SYSCFG_DEV->CPUF_D_REMAP_BDADDR[0]);
            } else if (addr < 0x10580000) {
                decode_addr = (addr - 0x10540000) + HW_GET_VAL(SYSCFG_DEV->CPUF_D_REMAP_BDADDR[1]);
            } else {
                decode_addr = (addr - 0x10580000) + HW_GET_VAL(SYSCFG_DEV->CPUF_D_REMAP_BDADDR[2]);
            }
        } else {
            if (addr < 0x10540000) {
                decode_addr = (addr - 0x10500000) + HW_GET_VAL(SYSCFG_DEV->CPUS_D_REMAP_BDADDR[0]);
            } else if (addr < 0x10580000) {
                decode_addr = (addr - 0x10540000) + HW_GET_VAL(SYSCFG_DEV->CPUS_D_REMAP_BDADDR[1]);
            } else {
                decode_addr = (addr - 0x10580000) + HW_GET_VAL(SYSCFG_DEV->CPUS_D_REMAP_BDADDR[2]);
            }
        }
    } else if (addr < 0x20000000) {
        decode_addr = addr | 0x10000000;
    } else {
        decode_addr = addr;
    }

    return decode_addr;
}

__dma_sram_text static dma_Master_t dma_Select_Mst(uint8_t mem_periph, uint32_t addr)
{
    uint32_t master;
    switch (mem_periph) {
    case ISMEMORY:
        if (dma_Is_SRAMb_Addr(addr)) {
            master = DMA_MASTER_4;
        } else {
            master = DMA_MASTER_1;
        }
        break;
    case ISPERIPHERAL:
        if (PERIPH_ERR_SLOT == dma_Is_PeripDataReg_Addr(addr)) {
            master = DMA_MASTER_NONE;
            break;
        }

        if (PERIPH_QSPI_DR == dma_Is_PeripDataReg_Addr(addr)) {
            master = DMA_MASTER_3;
        } else {
            master = DMA_MASTER_2;
        }
        break;

    default:
        master = DMA_MASTER_NONE;
        break;
    }
    return master;
}


__HAL_STATIC_INLINE
void DMA_DisableClock(void)
{
    LL_RCC_DMA_Reset();
    LL_RCC_DMA_DisableBusClock();
    LL_RCC_DMA_DisableClock();
}

__HAL_STATIC_INLINE
void DMA_EnableClock(void)
{
    LL_RCC_DMA_SetClockSource(LL_RCC_DMA_AHB);
    LL_RCC_DMA_SetPrescaler(LL_RCC_DMA_DIV_1);
    LL_RCC_DMA_DisableBusClock();
    LL_RCC_DMA_DisableClock();
    LL_RCC_DMA_Reset();
    LL_RCC_DMA_EnableBusClock();
    LL_RCC_DMA_EnableClock();
}

__HAL_STATIC_INLINE
void DMA_Disable_Channel(dmac_dev_t *dmaReg, uint32_t ch_id)
{
    /* Abort the channel, Ensure transfer completion */
    LL_DMAC_AbortChannel(dmaReg, ch_id);
    /* Ensure that the channel has been suspended */
    // while (!LL_DMAC_IsAbortCh(dmaReg, ch_id));
    /* Polling ande until it indicates that the channel FIFO is empty */
    // while (!LL_DMAC_IsChActiveFlag_FIFOEMPTY(dmaReg, ch_id));
    /* CH_EN bit can then be cleared by software once the channel FIFO is empty */
    /* Disable the channel and wait to confirm that it has been disabled. */
    LL_DMAC_DisableCh(dmaReg, ch_id);
}

__HAL_STATIC_INLINE
void DMA_Enable_Channel(dmac_dev_t *dmaReg, uint32_t ch_id)
{
    /* channel cannot be disabled right away, so first need to suspend it. */
    LL_DMAC_ResumeChannel(dmaReg, ch_id);
    /* Enbale write and enable channel */
    LL_DMAC_EnableCh(dmaReg, ch_id);
    /* Ensure channel was enable */
}

__HAL_STATIC_INLINE
void DMA_Disable_ChannelIRQ(dmac_dev_t *dmaReg, uint32_t ch_id)
{
    /* Clear IRQ */
    LL_DMAC_ClearCh_AllINT(dmaReg, ch_id);
    /* Mask IRQ */
    LL_DMAC_MaskCh_AllINT(dmaReg, ch_id);
    /* Disable IRQ*/
    LL_DMAC_DisableChINT(dmaReg, ch_id);
}

__HAL_STATIC_INLINE
void DMA_Enable_ChannelIRQ(dmac_dev_t *dmaReg, uint32_t ch_id)
{
    /* Clear IRQ flag */
    LL_DMAC_ClearCh_AllINT(dmaReg, ch_id);
    /* Always want to unmask channel's
    * the tfr, block and err interrupts IRQ flag */
    LL_DMAC_UnMaskCh_AllINT(dmaReg, ch_id);
    /* Enbale IRQ enable switch */
    LL_DMAC_EnableChINT(dmaReg, ch_id);
}

/**
 * @brief  UnLock the DMA channel module.
 * @param DMA controller peripheral instance.
 * @param The channel number.
 *         @arg [0 ~ 7] MAX Quantity: DMA_CHANNEL_MAX_NUM
 * @note
 *
 * @retval Hardware status.
 * @retval 0 on success, other status is err.
 */
__dma_sram_text static int32_t DMA_UnLock_Channel(dmac_dev_t *dmaReg, uint32_t ch_id)
{
    /* Set 1 unlock (write 0 set 1) */
    LL_DMAC_ChannelTryUnlock(dmaReg, ch_id);
    return HW_SUCCESS;
}

/**
 * @brief  Adjust lock and acquire a remap lock for the DMA channel module.
 * @param The channel number.
 *         @arg [0 ~ 7] MAX Quantity: DMA_CHANNEL_MAX_NUM
 * @note
 *
 * @retval Hardware status.
 * @retval @ref HW_BUSY: It was locked areadly.
 * @retval @ref HW_SUCCESS: lock success.
 */
__dma_sram_text static int32_t DMA_TryLock_Channel(dmac_dev_t *dmaReg, uint32_t ch_id)
{
    /* read clear lock */
    return LL_DMAC_ChannelTrylock(dmaReg, ch_id) ? HW_SUCCESS : HW_BUSY;
}

__HAL_STATIC_INLINE
void DMA_Close_AllChannel(dmac_dev_t *dmaReg)
{
    /* Disable all channel */
    LL_DMAC_DisableALLCh(dmaReg);
    /* Disable all channel interrrupt */
    LL_DMAC_DisableAllCh_INT(dmaReg);
    /* Mask all channel interrupt */
    LL_DMAC_MaskAllCh_INT(dmaReg);
    /* Clear all channel interrput */
    LL_DMAC_ClearAllCh_INT(dmaReg);
    /* Allocate lock resource and initialize it */
    for (uint32_t ch_id = 0; ch_id < DMA_CHANNEL_MAX_NUM; ch_id++) {
        /* Unlock all channels. */
        LL_DMAC_ChannelTryUnlock(dmaReg, ch_id);
        /* Set the handshake type. This is hardcoded to hardware */
        LL_DMAC_SetChSrcHandshakSel(dmaReg, ch_id, DMAC_HS_HARDWARE);
        LL_DMAC_SetChDstHandshakSel(dmaReg, ch_id, DMAC_HS_HARDWARE);
        /* Set the handshake polarity, always HIGH */
        LL_DMAC_SetChSrcHandshakPol(dmaReg, ch_id, DMAC_SRC_HS_POL_HIGH);
        LL_DMAC_SetChDstHandshakPol(dmaReg, ch_id, DMAC_DST_HS_POL_HIGH);
        /* Set enabel or not fifo mode */
        // LL_DMAC_DisableChFifoMode(dmaReg, ch_id);
    }
    /* Clear DMA suspend all channels. */
    LL_DMAC_ResumeCh_All(dmaReg);
}

static int32_t dma_ChannelConf(dmac_dev_t *dmaReg,
                    uint32_t chId, const DMA_ChannelConf_t *pchanCfg)
{
    /* Lock configure channel. */
    if (HW_BUSY != DMA_TryLock_Channel(dmaReg, chId)) {
        LOG_DBG("not request channel, lock channel hw lock in config!\n");
    }

    /* 1.Disable channel before configure */
    DMA_Disable_Channel(dmaReg, chId);

    /* Decode address */
    /* 2.1 Set the source and destination address */
    /* 3.1.Set Block Transfer Size */

    /* 4.1 Set the source and destination transfer width. */
    LL_DMAC_SetChSrcBitWidth(dmaReg, chId, pchanCfg->srcWidth);
    LL_DMAC_SetChDstBitWidth(dmaReg, chId, pchanCfg->dstWidth);
    /* 4.2 Set the source and destination burst transfer length */
    LL_DMAC_SetChBurstSrcSize(dmaReg, chId, pchanCfg->srcBrustNum);
    LL_DMAC_SetChBurstDstSize(dmaReg, chId, pchanCfg->dstBrustnum);

    /* 5.1 Set the increment type depending on direction */
    LL_DMAC_SetChTransferDirection(dmaReg, chId, pchanCfg->direction);

    /* Specific which peripheral */
    switch (pchanCfg->direction) {
    case DMAC_TRAN_TYPE_MEM2MEM:
        /* Select master */
        // g_pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISMEMORY, src_deaddr);
        // g_pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISMEMORY, dst_deaddr);
        break;

    case DMAC_TRAN_TYPE_MEM2PER:
        /* Select master */
        // g_pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISMEMORY,     src_deaddr);
        // g_pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISPERIPHERAL, dst_deaddr);
        /* Set the handshake interface. */
        LL_DMAC_SetDstPerConnect(dmaReg, chId, pchanCfg->dstHandShake);
        break;

    case DMAC_TRAN_TYPE_PER2MEM:
        /* Select master */
        // g_pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISPERIPHERAL, src_deaddr);
        // g_pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISMEMORY,     dst_deaddr);
        /* Set the handshake interface. */
        LL_DMAC_SetSrcPerConnect(dmaReg, chId, pchanCfg->srcHandShake);
        break;

    case DMAC_TRAN_TYPE_PER2PER:
        /* Select master */
        // g_pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISPERIPHERAL, src_deaddr);
        // g_pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISPERIPHERAL, dst_deaddr);
        /* Set the handshake interface. */
        LL_DMAC_SetSrcPerConnect(dmaReg, chId, pchanCfg->srcHandShake);
        LL_DMAC_SetDstPerConnect(dmaReg, chId, pchanCfg->dstHandShake);
        break;

    default:
        return -EINVAL;
        break;
    }

    LL_DMAC_SetChSrcHandshakSel(dmaReg, chId, DMAC_HS_HARDWARE);
    LL_DMAC_SetChDstHandshakSel(dmaReg, chId, DMAC_HS_HARDWARE);
    /* Set the handshake polarity, always HIGH */
    LL_DMAC_SetChSrcHandshakPol(dmaReg, chId, DMAC_SRC_HS_POL_HIGH);
    LL_DMAC_SetChDstHandshakPol(dmaReg, chId, DMAC_DST_HS_POL_HIGH);

    /* 9.Set Source and Destination Address Increment mode. */
    LL_DMAC_SetChSrcIncMode(dmaReg, chId, pchanCfg->srcIncreMode);
    LL_DMAC_SetChDstIncMode(dmaReg, chId, pchanCfg->dstIncreMode);

    /* 10.Set the channel priority */
    LL_DMAC_SetChPriority(dmaReg, chId, pchanCfg->priority);

    /* 11.1 Enable source reload address. */
    if (pchanCfg->srcEnReload) {
        LL_DMAC_StartSourceReload(dmaReg, chId);
    }
    /* 11.2 Enable destination reload address. */
    if (pchanCfg->dstEnReload) {
        LL_DMAC_StartDestinationReload(dmaReg, chId);
    }

    /* 12 Enable status updata */
    if (pchanCfg->srcEnStatus) {
        LL_DMAC_EnSrcUpdStatus(dmaReg, chId);
    }
    if (pchanCfg->dstEnStatus) {
        LL_DMAC_EnDstUpdStatus(dmaReg, chId);
    }

    /* 13.Set channel used to whitch cpu remap */
    if (LL_SYS_GetCurrentSysId()) {
        LL_DMAC_SetCh2CpuF(dmaReg, chId);
        // LL_DMAC_EnableRemapMaster(dmaReg, 0, 0);
    } else {
        LL_DMAC_SetCh2CpuS(dmaReg, chId);
        // LL_DMAC_EnableRemapMaster(dmaReg, 1, 0);
    }

    LL_DMAC_SetLLItemAddr(dmaReg, chId, (uint32_t)0x0000);

    /* Save param */
    HAL_Memcpy(&g_pdmaDevice->chanCfg[chId], pchanCfg, sizeof(DMA_ChannelConf_t));

    return 0;
}

#if CONFIG_DMA_PM

HAL_Status_t HAL_dma_PM_Suspend(HAL_Dev_t *dma)
{
    HAL_Status_t status = HAL_STATUS_OK;
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    for (uint32_t ch_id = DMA_MIN_CHID; ch_id <= DMA_MAX_CHID; ch_id++) {
        if (1 == LL_DMAC_IsableCh(dmaReg, ch_id)) {
            if (pdmaDevice->suspend_cb[ch_id].cb) {
                pdmaDevice->suspend_cb[ch_id].cb(dma, pdmaDevice->suspend_cb[ch_id].arg);
            }
            HAL_DMA_AbortTransfer(dma, ch_id);
        }
    }

    pdmaDevice->reg_backup.MASKBLOCK = dmaReg->MASKBLOCK;
    pdmaDevice->reg_backup.MASKTFR = dmaReg->MASKTFR;
    pdmaDevice->reg_backup.MASKERR = dmaReg->MASKERR;
    pdmaDevice->reg_backup.DMAEN = dmaReg->DMAEN;
    pdmaDevice->reg_backup.DMAREMAPCTRL = dmaReg->DMAREMAPCTRL;

    /* Close All channel */
    DMA_Close_AllChannel(dmaReg);

    /* Disable controller */
    LL_DMAC_Disable(dmaReg);

    /* Close Clock */
    DMA_DisableClock();

    return status;
}

HAL_Status_t HAL_dma_PM_Resume(HAL_Dev_t *dma)
{
    DMA_ChannelConf_t channecCfg = {0x00};
    HAL_Status_t status = HAL_STATUS_OK;
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    DMA_EnableClock();

    /* Disable all channel */
    LL_DMAC_DisableALLCh(dmaReg);
    /* Disable all channel interrrupt */
    LL_DMAC_DisableAllCh_INT(dmaReg);

    for (uint32_t ch_id = DMA_MIN_CHID; ch_id <= DMA_MAX_CHID; ch_id++) {
        if (pdmaDevice->chanState[ch_id] > DMA_STATE_IDLE) {
            memcpy(&channecCfg, &pdmaDevice->chanCfg[ch_id], sizeof(DMA_ChannelConf_t));
            status = dma_ChannelConf(dmaReg, ch_id, &channecCfg);
            LL_DMAC_SetSrcPerConnect(dmaReg, ch_id, channecCfg.srcHandShake);
            if (pdmaDevice->reg_backup.MASKBLOCK & (0x01 << ch_id)) {
                HAL_DMA_EnableIRQ(dma, ch_id, DMA_EVENT_BLOCK_DONE);
            }
            if (pdmaDevice->reg_backup.MASKTFR & (0x01 << ch_id)) {
                HAL_DMA_EnableIRQ(dma, ch_id, DMA_EVENT_DONE);
            }
            if (pdmaDevice->reg_backup.MASKERR & (0x01 << ch_id)) {
                HAL_DMA_EnableIRQ(dma, ch_id, DMA_EVENT_ERR);
            }
        }
    }

    dmaReg->DMAEN = pdmaDevice->reg_backup.DMAEN;
    dmaReg->DMAREMAPCTRL = pdmaDevice->reg_backup.DMAREMAPCTRL;

    for (uint32_t ch_id = DMA_MIN_CHID; ch_id <= DMA_MAX_CHID; ch_id++) {
        if (pdmaDevice->chanState[ch_id] > DMA_STATE_IDLE) {
            if(pdmaDevice->resume_cb[ch_id].cb) {
                pdmaDevice->resume_cb[ch_id].cb(dma, pdmaDevice->resume_cb[ch_id].arg);
            }
        }
    }

    return status;
}


HAL_Status_t Dma_Suspend(HAL_Dev_t *dma)
{
    DMA_Device_t *temp_dev = (DMA_Device_t *)dma;
    HAL_Status_t status = HAL_STATUS_OK;

    if (temp_dev->wk_mode == DMA_WK_MODE_AUTO) {
        status = HAL_dma_PM_Suspend(dma);
    }

    return status;
}

HAL_Status_t Dma_Resume(HAL_Dev_t *dma)
{
    DMA_Device_t *temp_dev = (DMA_Device_t *)dma;
    HAL_Status_t status = HAL_STATUS_OK;

    if (temp_dev->wk_mode == DMA_WK_MODE_AUTO) {
        status = HAL_dma_PM_Resume(dma);
    }

    return status;
}


static int Dma_pm_action(void *dev, enum pm_device_action action)
{
    int ret = -1;

    switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
        ret = Dma_Suspend((HAL_Dev_t *)dev);
        break;
    case PM_DEVICE_ACTION_RESUME:
        ret = Dma_Resume((HAL_Dev_t *)dev);
        break;
    default:
        break;
    }
    return (ret == HAL_STATUS_OK) ? 0 : -1;
}


static void Dma_pm_register(HAL_Dev_t *dev)
{
    struct pm_device *pm_dev = &dev->pmDev;

    pm_dev->action = Dma_pm_action;
    pm_dev->early_action = NULL;
    pm_dev->flags = 0;
    pm_device_register(pm_dev, Dma_pm_action);
}

static void Dma_pm_unregister(HAL_Dev_t *dev)
{
    struct pm_device *pm_dev = &dev->pmDev;

    pm_device_unregister(pm_dev);
}

#endif


/* Exported functions.
 * ----------------------------------------------------------------------------
 */
__dma_sram_text static void DMA_Channel_IRQHandler(dmac_dev_t *dmaReg, uint32_t ch_id)
{
    /* if channel is interrupt enable */
    if (DMAC_INTERRUPT_ENABLE != LL_DMAC_IsableChINT(dmaReg, ch_id)) {
        return;
    }

    /* Flag channel is be free */
    if (g_pdmaDevice->chanState[ch_id] != DMA_STATE_BUSY) {
        // printf("[%d]%d \n", ch_id, g_pdmaDevice->chanState[ch_id]);
        return;
    }

    if (HW_BUSY != DMA_TryLock_Channel(dmaReg, ch_id)) {
        DMA_UnLock_Channel(dmaReg, ch_id);
        return;
    }

    if (LL_DMAC_IsPendingChINT_ERR(dmaReg, ch_id)
        && LL_DMAC_IsMaskChINT_ERR(dmaReg, ch_id)) {
        // LL_DMAC_ClearChINT_ERR(dmaReg, ch_id);
        LL_DMAC_ClearCh_AllINT(dmaReg, ch_id);

        if (g_pdmaDevice->errCb[ch_id].cb) {
            g_pdmaDevice->errCb[ch_id].cb((HAL_Dev_t *)g_pdmaDevice,
                                    g_pdmaDevice->errCb[ch_id].arg);
        }

        return;
    }

    g_pdmaDevice->doneBlocksNum++;

    if (LL_DMAC_IsPendingChINT_BLOCK(dmaReg, ch_id)
        && LL_DMAC_IsMaskChINT_BLOCK(dmaReg, ch_id)) {
        LL_DMAC_ClearChINT_BLOCK(dmaReg, ch_id);

        g_pdmaDevice->listItemNums--;

        if (g_pdmaDevice->blockCb[ch_id].cb) {
            g_pdmaDevice->blockCb[ch_id].cb((HAL_Dev_t *)g_pdmaDevice,
                                    g_pdmaDevice->blockCb[ch_id].arg);
        }
    }


    /* Read the interrupt status register adjust intrrupt source */
    if (LL_DMAC_IsPendingChINT_TFR(dmaReg, ch_id)
        && LL_DMAC_IsMaskChINT_TFR(dmaReg, ch_id)) {
        LL_DMAC_ClearChINT_TFR(dmaReg, ch_id);
        g_pdmaDevice->chanState[ch_id]   =   DMA_STATE_READY;
        if (g_pdmaDevice->doneCb[ch_id].cb) {
            g_pdmaDevice->doneCb[ch_id].cb((HAL_Dev_t *)g_pdmaDevice,
                                    g_pdmaDevice->doneCb[ch_id].arg);
        }
    }

    #if FLASH_CACHE_FLUSH
    uint32_t  dst_deaddr = LL_DMAC_GetChDstAddress(dmaReg, ch_id);
    uint8_t   direction  = LL_DMAC_GetChTransferDirection(dmaReg, ch_id);
    uint8_t   srcByte    = dma_Width2Byte(LL_DMAC_GetChSrcBitWidth(dmaReg, ch_id));
    uint32_t  block_size = LL_DMAC_GetChBlockTransferSize(dmaReg, ch_id) * srcByte;

    /* clean dcache */
    if ((DMA_DIRECTION_MEM2MEM == direction) ||
        (DMA_DIRECTION_PER2MEM == direction)) {
        /* flush dcache if destination is memory */
        csi_dcache_invalid_range((uint32_t *)dst_deaddr, block_size);
    }
    #endif

    /* Flash Unlock */
    if (dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChSrcAddress(dmaReg, ch_id)) ||
        dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChDstAddress(dmaReg, ch_id))) {
        // LL_SPLK_RawUnlock(SPLK_ID_FLASH_PSRAM);
    }

    /* The channel should disabled
     * when the transfer complete callback is triggered
     * but Hardware will clear after done.
     */
    // LL_DMAC_DisableCh(dmaReg, ch_id);
    // LL_DMAC_DisableChINT(dmaReg, ch_id);

}

__dma_sram_text void HAL_DMA_ISR(void *dev)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dev;
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    for (int ch_id = 0; ch_id < DMA_CHANNEL_MAX_NUM; ch_id++) {
        DMA_Channel_IRQHandler(dmaReg, ch_id);
    }

}


HAL_Dev_t *HAL_DMA_Init(DMA_ID_t id)
{
    DMA_Device_t *pdmaDevice = (DMA_Device_t *)HAL_DEV_Find(HAL_DEV_TYPE_DMA, id);
    if (NULL == pdmaDevice) {
        pdmaDevice = (DMA_Device_t *)HAL_DEV_MemMalloc(sizeof(DMA_Device_t));
        if (NULL == pdmaDevice) {
            return (HAL_Dev_t *)pdmaDevice;
        } else {
            if (id == DMA0_ID) {
                pdmaDevice->device.reg        =   (void *)DMAC_DEV;
                pdmaDevice->device.irqNum     =   DMAC_IRQn;
                pdmaDevice->device.irqPrio    =   DMA_IRQ_PRIORITY;
                pdmaDevice->device.irqHandler =   HAL_DMA_ISR;
            } else {
                HAL_DEV_MemFree((void *)pdmaDevice);
                return NULL;
            }

            /* Enable */
            DMA_EnableClock();

            /* Disable controller */
            LL_DMAC_Disable(pdmaDevice->device.reg);
            /* Close All channel */
            DMA_Close_AllChannel(pdmaDevice->device.reg);
            /* Enable DMA contorller */
            LL_DMAC_Enable(pdmaDevice->device.reg);

            IRQ_Disable(pdmaDevice->device.irqNum);
            IRQ_Attach(pdmaDevice->device.irqNum, pdmaDevice->device.irqHandler);
            IRQ_AttachDevice(pdmaDevice->device.irqNum, (HAL_Dev_t *)pdmaDevice);
            IRQ_Priority(pdmaDevice->device.irqPrio, pdmaDevice->device.irqPrio);
            IRQ_Enable(pdmaDevice->device.irqNum);

            pdmaDevice->reqChanNum      =  DMA_REQUEST_CHNUM;
            pdmaDevice->doneBlocksNum   =  0;
            pdmaDevice->listItemNums    =  0;

            HAL_Memset(&pdmaDevice->chanCfg, 0,
                sizeof(DMA_ChannelConf_t) * DMA_REQUEST_CHNUM);
            HAL_Memset(&pdmaDevice->errCb, 0,
                sizeof(DMA_EventCb_t) * DMA_REQUEST_CHNUM);
            HAL_Memset(&pdmaDevice->blockCb, 0,
                sizeof(DMA_EventCb_t) * DMA_REQUEST_CHNUM);
            HAL_Memset(&pdmaDevice->doneCb, 0,
                sizeof(DMA_EventCb_t) * DMA_REQUEST_CHNUM);
            HAL_Memset(&pdmaDevice->chanState, DMA_STATE_IDLE,
                sizeof(dma_ChannelState_t) * DMA_REQUEST_CHNUM);
            HAL_Memset(&pdmaDevice->headllItem, 0,
                sizeof(uint32_t) * DMA_REQUEST_CHNUM);
            #if 0
            for (uint32_t ch_id = 0; ch_id < DMA_REQUEST_CHNUM; ch_id++) {
                if (HAL_STATUS_OK != HAL_MutexInit(&pdmaDevice->cfgMutex[ch_id])) {
                    printf("Init Mutex error \n");
                }
            }
            #endif
#if CONFIG_DMA_PM
            pdmaDevice->wk_mode = DMA_WK_MODE_AUTO;
            HAL_Memset(&pdmaDevice->suspend_cb, 0,
                sizeof(HAL_Callback_t) * DMA_REQUEST_CHNUM);
            HAL_Memset(&pdmaDevice->resume_cb, 0,
                sizeof(HAL_Callback_t) * DMA_REQUEST_CHNUM);
            Dma_pm_register((HAL_Dev_t *)pdmaDevice);
#endif
            HAL_DEV_Register((HAL_Dev_t *)pdmaDevice, HAL_DEV_TYPE_DMA, id);
        }
    }

    g_pdmaDevice = pdmaDevice;

    return (HAL_Dev_t *)pdmaDevice;
}


HAL_Status_t HAL_DMA_DeInit(HAL_Dev_t *dma)
{
    HAL_Status_t  status           =    HAL_STATUS_OK;
    DMA_Device_t *pdmaDevice       =    (DMA_Device_t *)dma;
    if (NULL == dma) {
        return HAL_STATUS_INVALID_PARAM;
    }
    dmac_dev_t   *dmaReg           =    (dmac_dev_t *)(pdmaDevice->device.reg);
    if (HAL_DEV_GetState(dma) == 0) {
        return HAL_STATUS_INVALID_STATE;
    }


    for (uint32_t ch_id = 0; ch_id < DMA_CHANNEL_MAX_NUM; ch_id++) {
        if ((pdmaDevice->chanState[ch_id] == DMA_STATE_READY) ||
            ((pdmaDevice->chanState[ch_id] == DMA_STATE_BUSY)))  {
            return HAL_STATUS_ERROR;
        }
    }


    /* Close All channel */
    DMA_Close_AllChannel(dmaReg);

    /* Disable controller */
    LL_DMAC_Disable(dmaReg);

    /* Close Clock */
    DMA_DisableClock();

#if CONFIG_DMA_PM
    Dma_pm_unregister(dma);
#endif

    status = HAL_DEV_Unregister(dma);
    if (HAL_STATUS_OK != status) {
        return HAL_STATUS_ERROR;
    }

    HAL_DEV_MemFree((void *)dma);

    return HAL_STATUS_OK;
}

int32_t HAL_DMA_Open(HAL_Dev_t *dma, const DMA_ChannelConf_t *pchanCfg)
{
    uint32_t ch_mask              =    0U;
    uint32_t reqChId              =    DMA_CHANNEL_MAX_NUM;
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma || NULL == pchanCfg) {
        // HAL_ASSERT_PARAM(dma || pchanCfg);
        return HAL_STATUS_INVALID_PARAM;
    }

    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    for (uint32_t ch_id = DMA_MIN_CHID; ch_id <= DMA_MAX_CHID; ch_id++) {
        ch_mask |= HAL_BIT(ch_id);
    }

    // LL_SPLK_RawLock(SPLK_ID_DMA, 1);

    /* Select unused channel */
    for (uint32_t ch_id = DMA_MIN_CHID; ch_id <= DMA_MAX_CHID; ch_id++) {
        if (0 == HW_TEST_BIT(ch_mask, ch_id)) {
            continue;
        }

        if (HW_BUSY != DMA_TryLock_Channel(dmaReg, ch_id)) {
            if (DMA_STATE_BUSY != pdmaDevice->chanState[ch_id]) {
                reqChId = ch_id;
                break;
            }
        }
    }

    if (DMA_CHANNEL_MAX_NUM == reqChId)
        return HAL_STATUS_ERROR;

    // HAL_MutexLock(&pdmaDevice->cfgMutex[reqChId], OSI_WAIT_FOREVER);

    int32_t ret = dma_ChannelConf(dmaReg, reqChId, pchanCfg);
    if  (ret) {
        LOG_ERR("dma channel configure fail (%d)\n", ret);
        DMA_UnLock_Channel(dmaReg, reqChId);
        return HAL_STATUS_ERROR;
    }

    // LL_SPLK_RawUnlock(SPLK_ID_DMA);

    pdmaDevice->chanState[reqChId] = DMA_STATE_READY;

    // HAL_MutexUnlock(&pdmaDevice->cfgMutex[reqChId]);

    /* Only flag DMA is open, Not channel open */
    HAL_DEV_SetState((HAL_Dev_t *)pdmaDevice, HAL_DEV_STATE_OPEN);

    return reqChId;
}

HAL_Status_t HAL_DMA_Close(HAL_Dev_t *dma, uint32_t chId)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma) {
        HAL_ASSERT_PARAM(dma);
        return HAL_STATUS_INVALID_PARAM;
    }
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    /* This release channel must not enable */
    if (DMAC_ENABLE == LL_DMAC_IsableCh(dmaReg, chId)) {
        DMA_Disable_Channel(dmaReg, chId);
    }

    // LL_SPLK_RawLock(SPLK_ID_DMA, 1);

    /* Unlock HW lock */
    DMA_UnLock_Channel(dmaReg, chId);

    /* Flash Unlock */
    if (dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChSrcAddress(dmaReg, chId)) ||
        dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChDstAddress(dmaReg, chId))) {
        // LL_SPLK_RawUnlock(SPLK_ID_FLASH_PSRAM);
    }

    // LL_SPLK_RawUnlock(SPLK_ID_DMA);

    /* Flag channel as free */
    pdmaDevice->chanState[chId] = DMA_STATE_IDLE;

    HAL_DEV_SetState((HAL_Dev_t *)pdmaDevice, HAL_DEV_STATE_CLOSE);

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_DMA_ExtendControl(HAL_Dev_t *dma, uint32_t chId, DMA_ExtAttr_t attr, void *arg)
{
#if CONFIG_DMA_PM
    DMA_Device_t *pdmaDevice          =    (DMA_Device_t *)dma;

    switch (attr) {
    case DMA_EXTATTR_WAKEUP_MODE:
        pdmaDevice->wk_mode = *((uint8_t *)(arg));
        break;
    case DMA_EXTATTR_SUSPEND_CALLBACK:
        pdmaDevice->suspend_cb[chId].cb = ((HAL_Callback_t *)arg)->cb;
        pdmaDevice->suspend_cb[chId].arg = ((HAL_Callback_t *)arg)->arg;
        break;
    case DMA_EXTATTR_RESUME_CALLBACK:
        pdmaDevice->resume_cb[chId].cb = ((HAL_Callback_t *)arg)->cb;
        pdmaDevice->resume_cb[chId].arg = ((HAL_Callback_t *)arg)->arg;
        break;
    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }
#endif
    return HAL_STATUS_OK;
}

__dma_sram_text uint32_t HAL_DMA_BlockSizeGet(HAL_Dev_t *dma, uint32_t chId)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    return LL_DMAC_GetChBlockTransferSize(dmaReg, chId);
}

HAL_Status_t HAL_DMA_ConfigBlockTranfer(HAL_Dev_t *dma, uint32_t chId, const DMA_BlockConf_t *pblockCfg)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    if (NULL == dma || NULL == pblockCfg) {
        HAL_ASSERT_PARAM(dma);
        return HAL_STATUS_INVALID_PARAM;
    }


    if ((pdmaDevice->chanState[chId] != DMA_STATE_READY)) {
        return HAL_STATUS_INVALID_STATE;
    }

    uint32_t srcByte    = dma_Width2Byte(pdmaDevice->chanCfg[chId].srcWidth);
    uint32_t block_size = pblockCfg->blockSize / srcByte;
    if (block_size > DMAC_MAX_BLOCK_SIZE) {
        return HAL_STATUS_INVALID_PARAM;
    }

    /* Decode address */
    uint32_t  srcDeAddr = dma_DeCodeAddr((uint32_t)pblockCfg->srcAddr);
    uint32_t  dstDeAddr = dma_DeCodeAddr((uint32_t)pblockCfg->dstAddr);

    // HAL_MutexLock(&pdmaDevice->cfgMutex[chId], OSI_WAIT_FOREVER);

    /* 1. Disable channel before configure */
    DMA_Disable_Channel(dmaReg, chId);

    switch (pdmaDevice->chanCfg[chId].direction) {
    case DMA_DIRECTION_MEM2MEM:
        /* Select master */
        pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISMEMORY,     srcDeAddr);
        pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISMEMORY,     dstDeAddr);
        break;

    case DMA_DIRECTION_MEM2PER:
        /* Select master */
        pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISMEMORY,     srcDeAddr);
        pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISPERIPHERAL, dstDeAddr);
        break;

    case DMA_DIRECTION_PER2MEM:
        /* Select master */
        pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISPERIPHERAL, srcDeAddr);
        pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISMEMORY,     dstDeAddr);
        break;

    case DMA_DIRECTION_PER2PER:
        /* Select master */
        pdmaDevice->srcMaster[chId] = dma_Select_Mst(ISPERIPHERAL, srcDeAddr);
        pdmaDevice->dstMaster[chId] = dma_Select_Mst(ISPERIPHERAL, dstDeAddr);
        break;

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }
    if (pdmaDevice->srcMaster[chId] < DMA_MASTER_NONE) {
        /* 6.1 Set select Source master */
        LL_DMAC_SetSrcMasterCh(dmaReg, chId, pdmaDevice->srcMaster[chId]);
    }
    if (g_pdmaDevice->dstMaster[chId] < DMA_MASTER_NONE) {
        /* 6.2 Set select Destination master */
        LL_DMAC_SetDstMasterCh(dmaReg, chId, pdmaDevice->dstMaster[chId]);
    }

    /* 2. Set the source and destination address */
    LL_DMAC_SetChSrcAddress(dmaReg, chId, srcDeAddr);
    LL_DMAC_SetChDstAddress(dmaReg, chId, dstDeAddr);

    /* 3. Set Block Transfer Size */
    LL_DMAC_SetChBlockTransferSize(dmaReg, chId, block_size);

    // HAL_MutexUnlock(&pdmaDevice->cfgMutex[chId]);

    // DMA_LOG_ALL(DMAC_DEV, chId, "CfgBlock");

    return HAL_STATUS_OK;
}

__dma_sram_text HAL_Status_t HAL_DMA_InitNodeCtrl(HAL_Dev_t *dma, uint32_t chId, DMA_NodeConf_t *pcurrentNode, uint8_t int_en)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;
    // dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);
    DMA_LLItem_t *pnode           =    (DMA_LLItem_t *)pcurrentNode;

    if (NULL == dma || NULL == pcurrentNode) {
        HAL_ASSERT_PARAM(dma);
        return HAL_STATUS_INVALID_PARAM;
    }

    uint32_t srcByte    = dma_Width2Byte(pdmaDevice->chanCfg[chId].srcWidth);
    uint32_t block_size = pcurrentNode->blockSize / srcByte;
    if (block_size > DMAC_MAX_BLOCK_SIZE) {
        return HAL_STATUS_INVALID_PARAM;
    }

    /* Decode address */
    uint32_t  srcDeAddr = dma_DeCodeAddr((uint32_t)pcurrentNode->srcAddr);
    uint32_t  dstDeAddr = dma_DeCodeAddr((uint32_t)pcurrentNode->dstAddr);

    pnode->src_addr                     =   srcDeAddr;
    pnode->dst_addr                     =   dstDeAddr;

    /* must clear done */
    pnode->ctrl_high.bits.done_state    =   0;  /* Reset */
    pnode->ctrl_high.bits.block_size    =   block_size;

    pnode->ctrl_low.bits.int_en         =   int_en;  /* Enable */
    pnode->ctrl_low.bits.src_width      =   pdmaDevice->chanCfg[chId].srcWidth;
    pnode->ctrl_low.bits.dst_width      =   pdmaDevice->chanCfg[chId].dstWidth;
    pnode->ctrl_low.bits.src_inc        =   pdmaDevice->chanCfg[chId].srcIncreMode;
    pnode->ctrl_low.bits.dst_inc        =   pdmaDevice->chanCfg[chId].dstIncreMode;
    pnode->ctrl_low.bits.src_msize      =   pdmaDevice->chanCfg[chId].srcBrustNum;
    pnode->ctrl_low.bits.dst_msize      =   pdmaDevice->chanCfg[chId].dstBrustnum;
    pnode->ctrl_low.bits.src_gather_en  =   0;  /* Disable */
    pnode->ctrl_low.bits.dst_scatter_en =   0;  /* Disable */
    pnode->src_statu                    =   0;  /* Extend */
    pnode->dst_statu                    =   0;  /* Extend */

    switch (pdmaDevice->chanCfg[chId].direction) {
    case DMA_DIRECTION_MEM2MEM:
        /* Select master */
        pnode->ctrl_low.bits.src_mst_sel    =   dma_Select_Mst(ISMEMORY,     srcDeAddr);
        pnode->ctrl_low.bits.dst_mst_sel    =   dma_Select_Mst(ISMEMORY,     dstDeAddr);

        pnode->ctrl_low.bits.direction      =   DMA_DIRECTION_MEM2MEM;

        pnode->ctrl_low.bits.llp_src_en     =   1;
        pnode->ctrl_low.bits.llp_dst_en     =   1;
        break;

    case DMA_DIRECTION_MEM2PER:
        /* Select master */
        pnode->ctrl_low.bits.src_mst_sel    =   dma_Select_Mst(ISMEMORY,     srcDeAddr);
        pnode->ctrl_low.bits.dst_mst_sel    =   dma_Select_Mst(ISPERIPHERAL, dstDeAddr);

        pnode->ctrl_low.bits.direction      =   DMA_DIRECTION_MEM2PER;

        pnode->ctrl_low.bits.llp_src_en     =   1;
        pnode->ctrl_low.bits.llp_dst_en     =   0;
        break;

    case DMA_DIRECTION_PER2MEM:
        /* Select master */
        pnode->ctrl_low.bits.src_mst_sel    =   dma_Select_Mst(ISPERIPHERAL, srcDeAddr);
        pnode->ctrl_low.bits.dst_mst_sel    =   dma_Select_Mst(ISMEMORY,     dstDeAddr);

        pnode->ctrl_low.bits.direction      =   DMA_DIRECTION_PER2MEM;

        pnode->ctrl_low.bits.llp_src_en     =   0;
        pnode->ctrl_low.bits.llp_dst_en     =   1;
        break;

    case DMA_DIRECTION_PER2PER:
        /* Select master */
        pnode->ctrl_low.bits.src_mst_sel    =   dma_Select_Mst(ISPERIPHERAL, srcDeAddr);
        pnode->ctrl_low.bits.dst_mst_sel    =   dma_Select_Mst(ISPERIPHERAL, dstDeAddr);

        pnode->ctrl_low.bits.direction      =   DMA_DIRECTION_PER2PER;

        pnode->ctrl_low.bits.llp_src_en     =   0;  /* Reset */
        pnode->ctrl_low.bits.llp_dst_en     =   0;  /* Reset */
        break;

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    pdmaDevice->listItemNums++;

    return HAL_STATUS_OK;
}

__dma_sram_text HAL_Status_t HAL_DMA_ConfigListTranfer(HAL_Dev_t *dma, uint32_t chId, const DMA_NodeConf_t *pheadList)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;
    DMA_LLItem_t *phead_lli       =    (DMA_LLItem_t *)pheadList;

    if (NULL == dma || NULL == pheadList) {
        HAL_ASSERT_PARAM(dma);
        return HAL_STATUS_INVALID_PARAM;
    }
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);
    if ((pdmaDevice->chanState[chId] != DMA_STATE_READY)) {
        return HAL_STATUS_INVALID_STATE;
    }

    // HAL_MutexLock(&pdmaDevice->cfgMutex[chId], OSI_WAIT_FOREVER);

    /* 1.Disable channel before configure */
    DMA_Disable_Channel(dmaReg, chId);

    /* 2. Set the source and destination address */
    LL_DMAC_SetChSrcAddress(dmaReg, chId, (uint32_t)(pheadList->srcAddr));
    LL_DMAC_SetChDstAddress(dmaReg, chId, (uint32_t)(pheadList->dstAddr));

    /* 3. Set Block Transfer Size */
    LL_DMAC_SetChBlockTransferSize(dmaReg, chId, pheadList->blockSize);


    if (phead_lli->ctrl_low.bits.src_mst_sel < DMA_MASTER_NONE) {
        /* Set select Source master */
        LL_DMAC_SetSrcMasterCh(dmaReg, chId, phead_lli->ctrl_low.bits.src_mst_sel);
    }
    if (phead_lli->ctrl_low.bits.dst_mst_sel < DMA_MASTER_NONE) {
        /* Set select Destination master */
        LL_DMAC_SetDstMasterCh(dmaReg, chId, phead_lli->ctrl_low.bits.dst_mst_sel);
    }


    if (LL_DMAC_IsableDstUpdStatus(dmaReg, chId)) {
        LL_DMAC_SetSrcStatuAddr(dmaReg, chId, phead_lli->src_statu);
    }

    if (LL_DMAC_IsableSrcUpdStatus(dmaReg, chId)) {
        LL_DMAC_SetDstStatuAddr(dmaReg, chId, phead_lli->dst_statu);
    }

    /* Set linked-list node configuration */
    if (phead_lli->ctrl_low.bits.llp_src_en) {
        LL_DMAC_EnableSrcLLP(dmaReg, chId);
    } else {
        LL_DMAC_DisableSrcLLP(dmaReg, chId);
    }

    if (phead_lli->ctrl_low.bits.llp_dst_en) {
        LL_DMAC_EnableDstLLP(dmaReg, chId);
    } else {
        LL_DMAC_DisableDstLLP(dmaReg, chId);
    }

    LL_DMAC_SetLLItemAddr(dmaReg, chId, (uint32_t)phead_lli);

    LL_DMAC_SelectListMaster(dmaReg, chId, DMA_MASTER_1);  // DMA_MASTER_2

    // HAL_MutexUnlock(&pdmaDevice->cfgMutex[chId]);

    pdmaDevice->headllItem[chId]   =   phead_lli;

    // DMA_LOG_ALL(DMAC_DEV, chId, "Cfglist");
    // LOG_DBG("Node %08X  %08X\n", (uint32_t)pcurrentNode, (uint32_t)pnextlistNode);

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_DMA_StartTransfer(HAL_Dev_t *dma, uint32_t chId)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma) {
        return HAL_STATUS_INVALID_PARAM;
    }
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);
    /* Check channel software busy */
    if (pdmaDevice->chanState[chId] != DMA_STATE_READY &&
        pdmaDevice->chanState[chId] == DMA_STATE_BUSY) {
        LOG_ERR("channel cfg fail %d\n", pdmaDevice->chanState[chId]);
        return HAL_STATUS_INVALID_STATE;
    }

    /* Check channel hardware busy */
    if (HW_BUSY != DMA_TryLock_Channel(dmaReg, chId)) {
        LOG_ERR("channel hw lock not locked. ");
        DMA_UnLock_Channel(dmaReg, chId);
        return HAL_STATUS_ERROR;
    }

    if (DMAC_ENABLE == LL_DMAC_IsableCh(dmaReg, chId)) {
        return HAL_STATUS_BUSY;
    }

#if 0
    /* Flash lock */
    if (dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChSrcAddress(dmaReg, chId)) ||
        dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChDstAddress(dmaReg, chId))) {
        // LL_SPLK_RawLock(SPLK_ID_FLASH_PSRAM, 1);
    }
#endif

    pdmaDevice->chanState[chId] = DMA_STATE_BUSY;

    // DMA_LOG_ALL(DMAC_DEV, chId, "start");

    /* Enable the channel interrupts */
    // DMA_Enable_ChannelIRQ(dmaReg, chId);

    /* Enable the DMA channel */
    DMA_Enable_Channel(dmaReg, chId);
#if 0
    end_tick = HAL_BOARD_GetTime(HAL_TIME_MS) + DMA_TIMEOUT;
    if (!LL_DMAC_IsableChINT(dmaReg, chId)) {
        while (LL_DMAC_IsableCh(dmaReg, chId)) {
            if ((HAL_BOARD_GetTime(HAL_TIME_MS) > end_tick))
                return HAL_STATUS_TIMEOUT;
        }
        pdmaDevice->doneBlocksNum++;
    }
#endif
    return HAL_STATUS_OK;
}

__dma_sram_text HAL_Status_t HAL_DMA_AbortTransfer(HAL_Dev_t *dma, uint32_t chId)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma) {
        return HAL_STATUS_INVALID_PARAM;
    }
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    /* Disable channel */
    DMA_Disable_Channel(dmaReg, chId);

    /* Disable channel interrupt */
    // DMA_Disable_ChannelIRQ(dmaReg, chId);

    /* Flash Unlock */
    if (dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChSrcAddress(dmaReg, chId)) ||
        dma_Is_XIPOrPSRAM_Addr(LL_DMAC_GetChDstAddress(dmaReg, chId))) {
        // LL_SPLK_RawUnlock(SPLK_ID_FLASH_PSRAM);
    }

    pdmaDevice->chanState[chId] = DMA_STATE_READY;

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_DMA_RegisterIRQ(HAL_Dev_t *dma, uint32_t chId,
                             DMA_Event_t event, HAL_Callback_t *eventCb)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma || NULL == eventCb) {
        return HAL_STATUS_INVALID_PARAM;
    }

    switch (event) {
    case DMA_EVENT_DONE:
        if (eventCb->cb) {
            pdmaDevice->doneCb[chId].cb    =   eventCb->cb;
            pdmaDevice->doneCb[chId].arg   =   eventCb->arg;
        }
        break;

    case DMA_EVENT_ERR:
        if (eventCb->cb) {
            pdmaDevice->errCb[chId].cb     =   eventCb->cb;
            pdmaDevice->errCb[chId].arg    =   eventCb->arg;
        }
        break;

    case DMA_EVENT_BLOCK_DONE:
        if (eventCb->cb) {
            pdmaDevice->blockCb[chId].cb    =   eventCb->cb;
            pdmaDevice->blockCb[chId].arg   =   eventCb->arg;
        }
        break;

    case DMA_EVENT_ALL:
    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_DMA_UnRegisterIRQ(HAL_Dev_t *dma, uint32_t chId, DMA_Event_t event)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma) {
        return HAL_STATUS_INVALID_PARAM;
    }

    switch (event) {
    case DMA_EVENT_DONE:
        pdmaDevice->doneCb[chId].cb    =   NULL;
        pdmaDevice->doneCb[chId].arg   =   NULL;
        break;

    case DMA_EVENT_ERR:
        pdmaDevice->errCb[chId].cb     =   NULL;
        pdmaDevice->errCb[chId].arg    =   NULL;
        break;

    case DMA_EVENT_BLOCK_DONE:
        pdmaDevice->blockCb[chId].cb   =   NULL;
        pdmaDevice->blockCb[chId].arg  =   NULL;
        break;

    case DMA_EVENT_ALL:
        pdmaDevice->blockCb[chId].cb   =   NULL;
        pdmaDevice->blockCb[chId].arg  =   NULL;

        pdmaDevice->errCb[chId].cb     =   NULL;
        pdmaDevice->errCb[chId].arg    =   NULL;

        pdmaDevice->doneCb[chId].cb    =   NULL;
        pdmaDevice->doneCb[chId].arg   =   NULL;
        break;

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}


HAL_Status_t HAL_DMA_EnableIRQ(HAL_Dev_t *dma, uint32_t chId, DMA_Event_t event)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma) {
        return HAL_STATUS_INVALID_PARAM;
    }
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    switch (event) {
    case DMA_EVENT_DONE:
        LL_DMAC_ClearChINT_TFR(dmaReg, chId);
        LL_DMAC_UnMaskChINT_TFR(dmaReg, chId);
        /* Enbale IRQ enable switch */
        LL_DMAC_EnableChINT(dmaReg, chId);
        break;

    case DMA_EVENT_ERR:
        /* Clear IRQ flag */
        LL_DMAC_ClearChINT_ERR(dmaReg, chId);
        LL_DMAC_UnMaskChINT_ERR(dmaReg, chId);
        /* Enbale IRQ enable switch */
        LL_DMAC_EnableChINT(dmaReg, chId);
        break;

    case DMA_EVENT_BLOCK_DONE:
        /* Clear IRQ flag */
        LL_DMAC_ClearChINT_BLOCK(dmaReg, chId);
        LL_DMAC_UnMaskChINT_BLOCK(dmaReg, chId);
        /* Enbale IRQ enable switch */
        LL_DMAC_EnableChINT(dmaReg, chId);
        break;

    case DMA_EVENT_ALL:
        DMA_Enable_ChannelIRQ(dmaReg, chId);
        break;

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}

HAL_Status_t HAL_DMA_DisableIRQ(HAL_Dev_t *dma, uint32_t chId, DMA_Event_t event)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;


    if (NULL == dma) {
        return HAL_STATUS_INVALID_PARAM;
    }
    dmac_dev_t   *dmaReg          =    (dmac_dev_t *)(pdmaDevice->device.reg);

    switch (event) {
    case DMA_EVENT_DONE:
        LL_DMAC_ClearChINT_TFR(dmaReg, chId);
        LL_DMAC_MaskChINT_TFR(dmaReg, chId);
        /* Enbale IRQ enable switch */
        LL_DMAC_DisableChINT(dmaReg, chId);
        break;

    case DMA_EVENT_ERR:
        /* Clear IRQ flag */
        LL_DMAC_ClearChINT_ERR(dmaReg, chId);
        LL_DMAC_MaskChINT_ERR(dmaReg, chId);
        /* Enbale IRQ enable switch */
        LL_DMAC_DisableChINT(dmaReg, chId);
        break;

    case DMA_EVENT_BLOCK_DONE:
        /* Clear IRQ flag */
        LL_DMAC_ClearChINT_BLOCK(dmaReg, chId);
        LL_DMAC_MaskChINT_BLOCK(dmaReg, chId);
        /* Enbale IRQ enable switch */
        LL_DMAC_DisableChINT(dmaReg, chId);
        break;

    case DMA_EVENT_ALL:
        DMA_Disable_ChannelIRQ(dmaReg, chId);
        break;

    default:
        return HAL_STATUS_INVALID_PARAM;
        break;
    }

    return HAL_STATUS_OK;
}

int32_t HAL_DMA_GetChanStatus(HAL_Dev_t *dma, uint32_t chId, uint32_t *doneBlockNum)
{
    DMA_Device_t *pdmaDevice      =    (DMA_Device_t *)dma;

    if (NULL == dma || !doneBlockNum) {
        return HAL_STATUS_INVALID_PARAM;
    }

    *doneBlockNum = pdmaDevice->doneBlocksNum;

    return ((pdmaDevice->chanState[chId] >= DMA_STATE_BUSY) ? 1 : 0);
}


/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
