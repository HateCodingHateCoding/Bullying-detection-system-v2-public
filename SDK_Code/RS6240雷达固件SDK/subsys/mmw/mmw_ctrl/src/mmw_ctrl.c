/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "soc.h"
#include "ll_utils.h"
#include "ll_clock_calib.h"
#include "hal_clock_calib.h"
#include "kvf.h"
#include "osi_common.h"

#include "mmw.h"
#include "mmw_ctrl.h"
#include "mmw_low_power.h"
#include "hal_board.h"
#include "hal_dev.h"
#if (CONFIG_MMW_DRIVER_TEMP_MGMT)
#include "mmw_temp_mgmt_api.h"
#endif
#include "mmw_alg_ant_calibration.h"

#if (CONFIG_CONSOLE)
#define debug_log(...)
#define debug_warn(...)
#define debug_err(...)  printk("err:"__VA_ARGS__)
#define debug_err_fmt(fmt, str, code) printk("err: "fmt" %s (-0x%x)\n", str, (~code + 1))
#else
#define debug_log(...)
#define debug_err(...)
#define debug_warn(...)
#define debug_err_fmt(fmt, str, code)
#endif

#ifndef CONFIG_MMW_LOOP_TASK_MONITOR
#define CONFIG_MMW_LOOP_TASK_MONITOR    0
#endif

#ifndef CONFIG_HAS_PSIC_MPW
#define CONFIG_HAS_PSIC_MPW                                 0
#endif
#ifndef CONFIG_MMW_SRAM_OPTIMIZE
#define CONFIG_MMW_SRAM_OPTIMIZE                            0
#endif

#if CONFIG_MMW_LOOP_TASK_MONITOR || CONFIG_HAS_PSIC_MPW || CONFIG_MMW_SRAM_OPTIMIZE
#error "CONFIG_MMW_LOOP_TASK_MONITOR is not support in current version"
#endif

/**
 * @brief APP System configuration.
 * @details
 * The System config to be set into MMW system before it is set up.
 */
#define MMW_SYS_CLK_TYPE_DCXO		0
#define MMW_SYS_CLK_TYPE_MSI		1
#define MMW_PLL_CLK_TYPE_DCXO		0
#define MMW_PLL_CLK_TYPE_NONE		1

#define CONFIG_MMW_DEBUG_LOG		0
#define CONFIG_MMW_SPECAL_LOG		0

#define MMW_CHIRP_CALIB_ON_FLASH_ID (HWINFO_ID_CHIRP_CALIB | HWINFO_STORAGE_FLASH)


#define MMW_MAX_CALIB_NUM	5

typedef struct {
	uint8_t  sys_ref_clk_type;  /**< System reference clk: DCXO or MSI */
	uint8_t  mmw_pll_clk_type;  /**< DCXO or None */
	uint8_t  mpw_coexist;  		/**< coexist with mpw ctrl lib */
	uint8_t  mmw_clk_trim;
	uint8_t  mmw_trim_param[2];
	uint8_t  reserved[2];
	uint32_t sys_ref_clk_freq;  /**< 1LSB = 1Hz*/
    uint32_t sys_msi_clk_freq;  /**< 1LSB = 1Hz*/
	uint32_t sys_lsi_clk_freq;  /**< 1LSB = 1Hz*/
	uint8_t  mmw_uart_enable;
	uint8_t  mmw_wio_enable;
} SystemCfg;
int mmw_setup(void *sys_if, SystemCfg *sys_cfg);
int mmw_close(void);
int mmw_sensor_start(void);
int mmw_calibration_data(uint8_t *data, int data_len);
uint32_t mmw_misc2_get_dpll_afc_cfg(void);
#define MMW_CALIB_DATA_LEN		(4*16+8)
#define MMW_DPLL_TRIM_LEN		(3)
int mmw_data_callback(void *handle, uint8_t int_type);
int mmw_low_power_mode(uint8_t mode);

static int mmw_callback_init(void);
void mmw_callback_deinit(void);

extern void psic_sys_boot_partner(void);
extern void psic_sys_wakeup_partner(void);
extern void psic_sys_shutdown_partner(void);

int mmw_wait_active_timeout(uint32_t timeout_ms)
{
	while (mmw_state(NULL) != MMW_STATE_ACTIVE) {
		msleep(10);
		if (--timeout_ms <= 0) {
			return MMW_ERR_CODE_TIMEOUT;
		}
	}
	return 0;
}

#if (CONFIG_MMW_DRIVER)
static int mmw_callback_handle (uint32_t data_state);
#endif


bool         g_mmw_init;
bool         g_mmw_data_event;
bool         g_mmw_sleep_event;
uint8_t      cb_data_type;
uint32_t     g_mmw_frame_idx;
OSI_Semaphore_t cb_mmw_event_sema;
//OSI_Semaphore_t cb_sleep_event;
MMW_HANDLE   mmw_data_handle;
static void *cb_arg;
TaskHandle_t mmw_task_handle;
uint8_t g_uart_dbg_enable;
uint8_t g_uart_port;
uint8_t g_io_port;

bool mmw_event_process_completed(void)
{
	return !g_mmw_data_event;
}

#if (CONFIG_MMW_DEBUG_LOG)
static void hexdump(const char *str, const uint8_t *packet, size_t length)
{
	int n = 0;

	if (!length) {
		printk("%s zero-length signal packet\n", str);
		return;
	}

	while (length--) {
		if (n % 16 == 0) {
			printk("%s %08X ", str, n);
		}

		printk("%02X ", *packet++);
		//printk("length %d\n",length);
		n++;
		if (n % 8 == 0) {
			if (n % 16 == 0) {
				printk("\n");
			} else {
				printk(" ");
			}
		}
	}

	printk("\n");
}
#endif

static uint16_t cal_checksum16(uint8_t *buffer, uint32_t size)
{
	uint16_t checksum = 0;
	uint16_t *data = (uint16_t *)buffer;

	for (size_t i = 0; i < (size >> 1); ++i) {
		checksum += *data;
		data++;
	}

	if (size & 0x01) {
		checksum += buffer[size - 1];
	}
	return (uint16_t)(~checksum);
}

#if (CONFIG_MMW_CALIB_DATA_LOAD)
static void mmw_chirp_data_calibration(bool force_calib)
{
	int idx ;

	/* mmw chirp calib load from nvm fs and set to mmw subsys*/
	if (force_calib || mmwc_set_chirp_calib_data() < 0) {
		for (idx = 0; idx < MMW_MAX_CALIB_NUM; idx++) {
			if (mmw_set_calib_sw(MMW_CALIB_SIGMADELTA, 1) == 0) {
			#if (CONFIG_MMW_CALIB_DATA_LOAD)
				if (!force_calib)
					mmwc_save_chirp_calib_data();
			#endif
				break;
			}
		}

		if (idx == MMW_MAX_CALIB_NUM) {
			debug_err("calib reboot");
			//sys_reboot(SYS_REBOOT_COLD);
		}
	}
}

static uint16_t mmw_storage_checksum16_cal(uint8_t *data, uint32_t size)
{
	uint32_t checksum = cal_checksum16(data, size);
	if (size & 0x01) {
		checksum = ((checksum >> 8) & 0xFF) | (checksum & 0xFF) << 8;
	}
	return checksum;
}

static uint16_t mmw_storage_checksum16_check(uint8_t *data, uint32_t size)
{
	return cal_checksum16(data, size);
}

#if (CONFIG_MMW_CHRIP_CALIB_COMP_EN)
#define MMW_CHIPR_DATA_BW	 (13)
int mmw_chirp_calibdata_restore_raw(uint32_t data_num, uint8_t *compress_data, uint32_t compress_data_len,
										int16_t *raw_data, uint32_t raw_data_len)
{
	int ret = mmw_chirp_calibdata_restore(data_num, compress_data, compress_data_len, raw_data, raw_data_len);
	for (int i = 0; i < raw_data_len; i++) {
		raw_data[i] &= ((1 << MMW_CHIPR_DATA_BW) - 1);
	}
	return ret;
}

int mmw_chirp_calib_compress_raw(uint32_t data_num, int16_t *raw_data, uint32_t raw_data_len,
										 uint8_t *compress_data, uint32_t compress_data_len)
{
	for (int i = 0; i < raw_data_len; i++) {
		if (raw_data[i] & BIT(MMW_CHIPR_DATA_BW - 1)) {
			raw_data[i] |= ~((1 << MMW_CHIPR_DATA_BW) - 1);
		}
	}
	return mmw_chirp_calibdata_compress(data_num, raw_data, raw_data_len, compress_data, compress_data_len);
}
#endif

static int mmw_storage_load_data(uint32_t id, uint8_t *data, size_t len)
{
	uint16_t magic = 0xFFFF;
	int size;

	switch (id & HWINFO_STORAGE_ID_MSK) {
	case HWINFO_ID_CHIRP_CALIB:
		if (len > (MMW_STORAGE_CHIRP_CALIB_SIZE)) {
			len = MMW_STORAGE_CHIRP_CALIB_SIZE;
		}
		size = KVF_Load(id, (uint8_t *)&magic, MMW_STORAGE_MAGIC_SIZE);
		if ((size < MMW_STORAGE_MAGIC_SIZE) || (magic != MMW_STORAGE_CHIRP_CALIB_MAGIC)) {
			return -1;
		}
		break;

	default:
		return -1;
	}

	size = KVF_Load(id, data, len);

	if (size < len) {
		return -1;
	}

	return 0;
}

static int mmw_storage_save_data(uint32_t id, uint8_t *data, size_t len)
{
	int ret = 0;
	uint8_t *pStorageData = OSI_Malloc(MMW_STORAGE_CHIRP_CALIB_SIZE);
	uint16_t magic = 0xFFFF;

	if (pStorageData == NULL) {
		debug_err("code %d fsave size %d", -MMW_ERR_CODE_NO_MEMORY, MMW_STORAGE_CHIRP_CALIB_SIZE);
		return -MMW_ERR_CODE_NO_MEMORY;
	}

	switch (id & HWINFO_STORAGE_ID_MSK) {
	case HWINFO_ID_CHIRP_CALIB:
		if (len > (MMW_STORAGE_CHIRP_CALIB_SIZE)) {
			len = MMW_STORAGE_CHIRP_CALIB_SIZE;
		}
		*((uint16_t *) data) = MMW_STORAGE_CHIRP_CALIB_MAGIC;
		break;

	default:
		OSI_Free(pStorageData);
		return -1;
	}

	ret = KVF_Save(id, data, len);
	if (ret == 0) {
		/* once save hw data into flash should check
		** if save and load is matched, or reboot to
		** forbit saving wrong data
		*/
		KVF_Load(id, pStorageData, len);
		if (memcmp((void *)data, (void *)pStorageData, len)) {
			ret = -1;
		}
		if (ret < 0) {
			KVF_Save(id, (uint8_t *)&magic, MMW_STORAGE_MAGIC_SIZE);
			csi_core_reset();
		}
	}

	OSI_Free(pStorageData);
	return ret;
}

int mmwc_set_angle_calib_data(complex16_cube *calib_data, uint8_t rx_num)
{
	int ret;
	uint32_t chip_info[MMW_CALIB_DATA_LEN>>2] = { 0 };
	for (uint8_t rx_ant = 0; rx_ant < MMW_ANGLE_CALIB_RX_ANT; rx_ant++) {
		if (rx_ant < rx_num) {
			chip_info[rx_ant * (MMW_ANGLE_CALIB_RX_ANT + 1)] = \
				calib_data[rx_ant].real + (calib_data[rx_ant].imag << 16);
		} else {
			chip_info[rx_ant * (MMW_ANGLE_CALIB_RX_ANT + 1)] = 2048;
		}
	}
	chip_info[16] = (1 << 12);
	chip_info[17] = 15561;
	ret = mmw_calibration_data((uint8_t *)&chip_info[0], MMW_CALIB_DATA_LEN);
	return ret;
}

int mmwc_save_angle_calib_data(uint8_t *calib_data, uint8_t len)
{
	if (len != HWINFO_SIZE_NVM_MMW_CALIB) {
		return -1;
	}
	int ret = KVF_Save(HWINFO_ID_NVM_ANT_CALIB_1T3R, calib_data, len);
	if (ret != len) {
		return -1;
	}

	return 0;
}

#if (CONFIG_MMW_CHRIP_CALIB_COMP_EN)
int mmwc_set_chirp_calib_data(void)
{
	int ret = MMW_ERR_CODE_SUCCESS;
	uint8_t *pCalData = OSI_Malloc(MMW_STORAGE_CHIRP_CALIB_SIZE);
	uint8_t *pRawdata = OSI_Malloc(MMW_CHIRP_CALIB_SIZE);
	uint8_t *compress_data = pCalData;
	uint32_t compress_size = MMW_STORAGE_CHIRP_CALIB_SIZE;
	uint16_t version;

	if (pCalData == NULL || pRawdata == NULL) {
		ret = -MMW_ERR_CODE_NO_MEMORY;
		goto exit;
	}

	ret = mmw_storage_load_data(MMW_CHIRP_CALIB_ON_FLASH_ID, pCalData, MMW_STORAGE_CHIRP_CALIB_SIZE);
	if (ret < 0) {
		ret = -(MMW_ERR_CODE_EIO | MMW_ERR_SUBCODE_LOAD);
		goto exit;
	}

#if (CONFIG_MMW_DEBUG_LOG)
	hexdump("flash:", pCalData + MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE, MMW_CHIRP_CALIB_COMP_SIZE);
#endif

	compress_data += MMW_STORAGE_MAGIC_SIZE;
	compress_size -= MMW_STORAGE_MAGIC_SIZE;

	version = compress_data[0] | (compress_data[1] << 8);
	if (version != MMW_STORAGE_CHIRP_CALIB_VERSION) {
		ret = -(MMW_ERR_CODE_UNSUPPORT | MMW_ERR_SUBCODE_VERSION);
		goto exit;
	}

	ret = mmw_storage_checksum16_check(compress_data, compress_size);
	if (ret != 0) {
		ret = -(MMW_ERR_CODE_EIO | MMW_ERR_SUBCODE_CHECKSUM);
		goto exit;
	}

	compress_data += MMW_STORAGE_VERSION_SIZE;
	compress_size -= MMW_STORAGE_VERSION_SIZE;

	/* fixme : compress size should equal with MMW_STORAGE_CHECKSUM_SIZE or assert */
	ret = mmw_chirp_calibdata_restore_raw(MMW_CHIRP_CALIB_NUM, compress_data,
					MMW_CHIRP_CALIB_COMP_SIZE, (int16_t *) pRawdata, MMW_CHIRP_CALIB_SIZE >> 1);
	if (ret < 0) {
		ret = -(MMW_ERR_CODE_EIO | MMW_ERR_SUBCODE_RESTORE);
		goto exit;
	}

	#if (CONFIG_MMW_DEBUG_LOG)
	hexdump("register:", pRawdata, MMW_CHIRP_CALIB_SIZE);
	#endif
	mmw_chirp_calibdata_set((uint16_t *)pRawdata, MMW_CHIRP_CALIB_SIZE >> 1);

exit:
	if (pCalData) {
		OSI_Free(pCalData);
	}
	if (pRawdata) {
		OSI_Free(pRawdata);
	}

	if (ret < 0) {
		debug_err_fmt("chirp calib", "set", ret);
	}
	return ret;
}

int mmwc_save_chirp_calib_data(void)
{
	int ret;
	uint8_t *pCalData = OSI_Malloc(MMW_STORAGE_CHIRP_CALIB_SIZE);
	uint8_t *pRawdata = OSI_Malloc(MMW_CHIRP_CALIB_SIZE);
	uint8_t *compress_data = pCalData;
	uint32_t compress_size = MMW_STORAGE_CHIRP_CALIB_SIZE;
	int checksum;

	if (pCalData == NULL || pRawdata == NULL) {
		ret = -MMW_ERR_CODE_NO_MEMORY;
		goto exit;
	}

	/*step 1 : get calib chirp data from mmw*/
	mmw_chirp_calibdata_get((uint16_t *)(pRawdata), MMW_CHIRP_CALIB_SIZE >> 1);
#if (CONFIG_MMW_DEBUG_LOG)
	hexdump("register:", pRawdata, MMW_CHIRP_CALIB_SIZE);
#endif

#if (CONFIG_MMW_SPECAL_LOG)
	uint16_t *pData = (uint16_t *)(pRawdata);
	for (int i = 0; i < (MMW_CHIRP_CALIB_SIZE >> 1); i++) {
		printk("%x\n", pData[i]);
	}
#endif

	/*step 2 : fill with compress chirp calib data*/
	compress_data += MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE;
	compress_size -= MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE;
	ret = mmw_chirp_calib_compress_raw(MMW_CHIRP_CALIB_NUM, (int16_t *)(pRawdata),
									MMW_CHIRP_CALIB_SIZE >> 1, compress_data, MMW_CHIRP_CALIB_COMP_SIZE);
	if (ret < 0) {
		ret = -(MMW_ERR_CODE_EIO | MMW_ERR_SUBCODE_COMP);
		goto exit;
	}

	/*step 2: fill with version */
	compress_data -= MMW_STORAGE_VERSION_SIZE;
	compress_size += MMW_STORAGE_VERSION_SIZE;
	compress_data[0] = MMW_STORAGE_CHIRP_CALIB_VERSION & 0xFF;
	compress_data[1] = (MMW_STORAGE_CHIRP_CALIB_VERSION >> 8) & 0xFF;

	/*step 3: fill with checksume*/
	checksum = mmw_storage_checksum16_cal(compress_data, compress_size - MMW_STORAGE_CHECKSUM_SIZE);
	compress_data[compress_size - 2] = checksum & 0xFF;
	compress_data[compress_size - 1] = (checksum >> 8) & 0xFF;

	/*step 4: fill with magic and save into nvs with id*/
	compress_data -= MMW_STORAGE_MAGIC_SIZE;
	compress_size += MMW_STORAGE_MAGIC_SIZE;
#if (CONFIG_MMW_DEBUG_LOG)
	hexdump("flash:", compress_data + MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE, MMW_CHIRP_CALIB_COMP_SIZE);
#endif
	ret = mmw_storage_save_data(MMW_CHIRP_CALIB_ON_FLASH_ID, compress_data, compress_size);
	if (ret < 0) {
		ret = -(MMW_ERR_CODE_EIO | MMW_ERR_SUBCODE_SAVE);
	}

exit:
	if (pCalData) {
		OSI_Free(pCalData);
	}
	if (pRawdata) {
		OSI_Free(pRawdata);
	}
	if (ret < 0) {
		debug_err_fmt("chirp calib", "save", ret);
	}
	return ret;
}
#else
int mmwc_set_chirp_calib_data(void)
{
	int ret;
	uint8_t *pCalData = k_malloc(MMW_STORAGE_CHIRP_CALIB_SIZE);
	uint8_t *compress_data = pCalData;
	uint32_t compress_size = MMW_STORAGE_CHIRP_CALIB_SIZE;
	uint16_t version;

	if (pCalData == NULL) {
		ret = -MMW_ERR_CODE_NO_MEMORY;
		goto exit;
	}

	ret = mmw_storage_load_data(MMW_CHIRP_CALIB_ON_FLASH_ID, pCalData, MMW_STORAGE_CHIRP_CALIB_SIZE);
	if (ret < 0) {
		ret = -MMW_ERR_CODE_EIO;
		goto exit;
	}

#if (CONFIG_MMW_DEBUG_LOG)
	hexdump("flash:", pCalData + MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE, MMW_CHIRP_CALIB_COMP_SIZE);
#endif

	compress_data += MMW_STORAGE_MAGIC_SIZE;
	compress_size -= MMW_STORAGE_MAGIC_SIZE;

	version = compress_data[0] | (compress_data[1] << 8);
	if (version != MMW_STORAGE_CHIRP_CALIB_VERSION) {
		ret = -MMW_ERR_CODE_UNSUPPORT;
		goto exit;
	}

	ret = mmw_storage_checksum16_check(compress_data, compress_size);
	if (ret != 0) {
		ret = -(MMW_ERR_CODE_EIO | MMW_ERR_SUBCODE_CHECKSUM);
		goto exit;
	}

	compress_data += MMW_STORAGE_VERSION_SIZE;
	compress_size -= MMW_STORAGE_VERSION_SIZE;

	mmw_chirp_calibdata_set((uint16_t *)compress_data, MMW_CHIRP_CALIB_SIZE >> 1);
exit:
	if (pCalData) {
		k_free(pCalData);
	}

	if (ret < 0) {
		debug_err_fmt("chirp calib", "set", ret);
	}
	return ret;
}

int mmwc_save_chirp_calib_data(void)
{
	int ret;
	uint8_t *pCalData = k_malloc(MMW_STORAGE_CHIRP_CALIB_SIZE);
	uint8_t *compress_data = pCalData;
	uint32_t compress_size = MMW_STORAGE_CHIRP_CALIB_SIZE;
	int checksum;

	if (pCalData == NULL) {
		ret = -MMW_ERR_CODE_NO_MEMORY;
		goto exit;
	}

	/*step 1 : get calib chirp data from mmw*/
	compress_data += MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE;
	compress_size -= MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE;
	mmw_chirp_calibdata_get((uint16_t *)(compress_data), MMW_CHIRP_CALIB_SIZE >> 1);

#if (CONFIG_MMW_DEBUG_LOG)
	hexdump("register:", compress_data, MMW_CHIRP_CALIB_SIZE);
#endif

#if (CONFIG_MMW_SPECAL_LOG)
	uint16_t *pData = (uint16_t *)(pRawdata);
	for (int i = 0; i < (MMW_CHIRP_CALIB_SIZE >> 1); i++) {
		printk("%x\n", pData[i]);
	}
#endif

	/*step 2: fill with version */
	compress_data -= MMW_STORAGE_VERSION_SIZE;
	compress_size += MMW_STORAGE_VERSION_SIZE;
	compress_data[0] = MMW_STORAGE_CHIRP_CALIB_VERSION & 0xFF;
	compress_data[1] = (MMW_STORAGE_CHIRP_CALIB_VERSION >> 8) & 0xFF;

	/*step 3: fill with checksume*/
	checksum = mmw_storage_checksum16_cal(compress_data, compress_size - MMW_STORAGE_CHECKSUM_SIZE);
	compress_data[compress_size - 2] = checksum & 0xFF;
	compress_data[compress_size - 1] = (checksum >> 8) & 0xFF;

	/*step 4: fill with magic and save into nvs with id*/
	compress_data -= MMW_STORAGE_MAGIC_SIZE;
	compress_size += MMW_STORAGE_MAGIC_SIZE;
#if (CONFIG_MMW_DEBUG_LOG)
	hexdump("flash:", compress_data + MMW_STORAGE_MAGIC_SIZE + MMW_STORAGE_VERSION_SIZE, MMW_CHIRP_CALIB_COMP_SIZE);
#endif
	ret = mmw_storage_save_data(MMW_CHIRP_CALIB_ON_FLASH_ID, compress_data, compress_size);
	if (ret < 0) {
		ret = -MMW_ERR_CODE_EIO;
	}

exit:
	if (pCalData) {
		k_free(pCalData);
	}

	if (ret < 0) {
		debug_err_fmt("chirp calib", "save", ret);
	}
	return ret;
}
#endif

#else

static void mmw_chirp_data_calibration(bool force_calib)
{
	int idx ;

	/* mmw chirp calib load from nvm fs and set to mmw subsys*/
	for (idx = 0; idx < MMW_MAX_CALIB_NUM; idx++) {
		if (mmw_set_calib_sw(MMW_CALIB_SIGMADELTA, 1) == 0) {
			break;
		}
	}
	if (idx == MMW_MAX_CALIB_NUM) {
		debug_err("calib reboot");
		//sys_reboot(SYS_REBOOT_COLD);
	}
}

#endif //#if (CONFIG_MMW_CALIB_DATA_LOAD)


void dbg_uart_enable(uint8_t enable)
{
    g_uart_dbg_enable = enable;
}

void set_dbg_uart_port(uint8_t uart_port, uint8_t io_port)
{
    g_uart_port = uart_port;
    g_io_port = io_port;
}

int mmw_ctrl_open(bool ref_msi, bool pll_lc, bool force_calib)
{
	int ret = -1;
	SystemCfg cfg;
	if (g_mmw_init) {
#if !(CONFIG_MMW_DRIVER)
		printk("MMW open already!\n");
#endif
		return 0;
	}
	mmw_dev_os_api_init();
	mmw_enable_dbg_uart(g_uart_dbg_enable);
    mmw_set_dbg_uart_port(g_uart_port, g_io_port);

#if (CONFIG_PM && CONFIG_PM_DEVICE)
    extern int mmw_dev_init(const HAL_Dev_t *dev);
    HAL_Dev_t *temp_device = HAL_DEV_Find(HAL_DEV_TYPE_MMW, 0);
    if (temp_device == NULL) {
        temp_device = (HAL_Dev_t *)HAL_DEV_MemMalloc(sizeof(HAL_Dev_t));
    }
    mmw_dev_init(temp_device);
#endif

	/* Get system config */
    cfg.sys_msi_clk_freq = HAL_BOARD_GetFreq(CLOCK_MSI);//HAL_CLOCK_MsiGetFreq();
	cfg.sys_lsi_clk_freq = HAL_BOARD_GetFreq(CLOCK_LSI);
	if (ref_msi) {
		cfg.sys_ref_clk_type = MMW_SYS_CLK_TYPE_MSI;
		cfg.sys_ref_clk_freq = cfg.sys_msi_clk_freq;
	} else {
		cfg.sys_ref_clk_type = MMW_SYS_CLK_TYPE_DCXO;
		cfg.sys_ref_clk_freq = 32000000; //CONFIG_CLK_DCXO_FREQ;
	}

	if (pll_lc) {
		cfg.mmw_pll_clk_type = MMW_PLL_CLK_TYPE_NONE;
	#if 0
		len = (int)hwinfo_storage_load(MMW_DPLL_TRIM_ID, &cfg.mmw_clk_trim, MMW_DPLL_TRIM_LEN);
		if (len != MMW_DPLL_TRIM_LEN) {
			cfg.mmw_clk_trim = 0;
			printk("MMW No DPLL Trim! %d\n", len);
		}
	#endif
	} else {
		cfg.mmw_pll_clk_type = MMW_PLL_CLK_TYPE_DCXO;
		cfg.mmw_clk_trim = 0;
	}

#if (CONFIG_HAS_PSIC_MPW)
	cfg.mpw_coexist = 1;
#else
	cfg.mpw_coexist = 0;
#endif
	cfg.mmw_uart_enable = 1;
	cfg.mmw_wio_enable  = 0;

#if CONFIG_MMW_SRAM_OPTIMIZE
	mmw_memory_type_set(0xcccc, 0x1F);
#endif

	/* Setup system param to mmw controller */
	ret = mmw_setup(NULL, &cfg);
	if (ret)
		return ret;

	/* Wakeup MMW and set calibration_data */
#if !(CONFIG_MMW_DRIVER)
	psic_sys_boot_partner();
#endif
	ret = mmw_wait_active_timeout(200);
	if (ret) {
		mmw_ctrl_close();
		return ret;
	}

	/* Config mmw callback for app system */
	ret = mmw_callback_init();
	if (ret)
		return ret;

#if (CONFIG_MMW_CALIB_DATA_LOAD)
	mmw_ant_calib_data_load();
#endif

	/* mmw low-power initialization. */
#if (CONFIG_PM) && (CONFIG_MMW_DRIVER)
	mmw_low_power_init();
#endif

	/* mmw chirp calib load from nvm fs and set to mmw subsys*/
	mmw_chirp_data_calibration(force_calib);

	/* trigger noise estimation. */
	//mmw_set_calib_sw(MMW_CALIB_CLUTTER_EST, 1);

	g_mmw_init = true;

	return ret;
}
int mmw_ctrl_close(void)
{
#if (CONFIG_MMW_DRIVER)
	mmw_sensor_stop();
#else
	mmw_close();
	mmw_callback_deinit();
	psic_sys_shutdown_partner();
	g_mmw_init = false;
#endif
    return 0;
}

int mmw_ctrl_start(void)
{
	int ret = 0;
#if !(CONFIG_MMW_DRIVER)
	uint8_t lowpwr = mmw_low_power_get();

	if (lowpwr == MMW_LOW_POWER_MODE_AUTO) {
		ret = mmw_low_power_mode(MMW_LOW_POWER_MODE_DISABLE);
		psic_sys_wakeup_partner();
		if (ret)
			return ret;
		ret = mmw_wait_active_timeout(20);
		if (ret)
			return ret;
	}
#endif
    /* clutter halt must set to 0, due to start clutter must be run, or calibration data may have error */
    mmw_clutter_halt_set(MMW_HALT_CLUTTER_UPDATE_DISABLE);
	ret = mmw_sensor_start();
	if (ret)
		return ret;

#if !(CONFIG_MMW_DRIVER)
	if (lowpwr == MMW_LOW_POWER_MODE_AUTO) {
		ret = mmw_low_power_mode(lowpwr);
		if (ret)
			return ret;
	}
#endif
    g_mmw_frame_idx = 0;
    return ret;
}

int mmw_ctrl_low_power(void)
{
	return mmw_low_power_mode(MMW_LOW_POWER_MODE_AUTO);
}

int mmw_ctrl_wake_up(void)
{
	int ret = mmw_low_power_mode(MMW_LOW_POWER_MODE_DISABLE);
#if !(CONFIG_MMW_DRIVER)
	if (!ret) {
		psic_sys_wakeup_partner();
	}
#endif
	return ret;
}

MMW_HANDLE mmw_ctrl_callback_get (uint8_t *int_type, void **arg)
{
	if (arg != NULL) {
		*arg = cb_arg;
	}
	if (int_type != NULL) {
		*int_type = cb_data_type;
	}
	return mmw_data_handle;
}

int mmw_ctrl_callback_cfg (MMW_HANDLE handle, uint8_t int_type, void *arg)
{
	int ret;
	int_type = ((handle == NULL) ? MMW_DATA_TYPE_DISABLE : int_type);
#if !(CONFIG_MMW_DRIVER)
	ret = mmw_data_callback(NULL, int_type);
#else
	ret = mmw_data_callback((void *)&mmw_callback_handle, int_type);
#endif
	if (!ret) {
		cb_data_type    = int_type;
		mmw_data_handle = handle;
		cb_arg = arg;
	}
	return ret;
}

uint8_t mmw_ctrl_data_type_get(void)
{
	return cb_data_type;
}

#if !(CONFIG_MMW_DRIVER)
struct mbox_channel mmw_rx_channel;
static void mmw_irq_handle (const struct device *dev, uint32_t channel,
		     void *user_data, struct mbox_msg *msg)
{
	k_sem_give(&cb_mmw_event_sema);
}
#else
#ifndef BIT
#define BIT(n)    (1U<<(n))
#endif
static int mmw_callback_handle (uint32_t data_state)
{
	if (data_state & (BIT(cb_data_type) | BIT(MMW_NOISE_EST_COMPLETE_EVENT))) {
		if (can_mmw_configured() != 4) {
			/* alreay stop */
			return 0;
		}
		OSI_SemaphoreRelease(&cb_mmw_event_sema);
		g_mmw_data_event = true;
	}

#if (CONFIG_SOFT_PM_ARBITRATE)
	if (data_state & BIT(MMW_SLEEP_REQ_EVENT)) {
		if (pm_get_sys_pm_policy() == SYS_PM_FORCE) {
			OSI_SemaphoreCreate(&cb_mmw_event_sema);
			g_mmw_sleep_event = true;
		}
	}
#endif
	return 0;
}
#endif

uint32_t mmw_ctrl_frame_cnt_get(void)
{
	return g_mmw_frame_idx;
}

uint8_t g_pm_mmw_id;
static void mmw_ctrl_task (void *param)
{
	printk("mmw_ctrl_data_task init success\n");
	while (1) {
		OSI_SemaphoreWait(&cb_mmw_event_sema, OSI_WAIT_FOREVER);

		/* 1. process data */
		if (g_mmw_data_event) {
			if (mmw_data_handle != NULL) {
				mmw_data_handle(NULL, cb_arg);
			}
			g_mmw_frame_idx++;
			g_mmw_data_event = false;
		}

#if (CONFIG_MMW_DRIVER_TEMP_MGMT)
        if (temp_mgmt_get_triger_calib_event()) {
            temp_mgmt_set_triger_calib_event(false);
            temp_mgmt_app_process_stop(NULL);
#if (MMW_TEMP_MGMT_DEBUG)
            printk("trigger calib:cur_tmp:%d, calib_temp:%d\n", temp_mgmt_get_cur_temp(), temp_mgmt_get_calib_temp());
            printk("lo:0x%x, rxlo:0x%x\n", HW_READ_32BIT(0x40810548), HW_READ_32BIT(0x4081054c));
#endif
            temp_mgmt_calib_process(temp_mgmt_get_cur_temp());
            temp_mgmt_app_process_start(NULL);
        }
#endif

		/* 2. system try to sleep */
		if (g_mmw_sleep_event) {
			g_mmw_sleep_event = false;
#if (CONFIG_SOFT_PM_ARBITRATE)
			if (pm_get_sys_pm_policy() == SYS_PM_FORCE) {
				uint32_t irq_key = irq_lock();
				if (pm_constraint_get(PM_STATE_STANDBY) && !mmw_pm_lock_get()) {
					pm_set_module_power_state(g_pm_mmw_id, PSIC_PM_STATE_STANDBY, K_TICKS_FOREVER);
				}
				irq_unlock(irq_key);
			}
#endif
		}
	}
	printk("mmw_ctrl_data_task exit!\n");
	vTaskDelete(NULL);
}

static int mmw_callback_init(void)
{
	int ret = 0;
	g_mmw_data_event = false;
	g_mmw_sleep_event = false;
	ret = OSI_SemaphoreCreate(&cb_mmw_event_sema, 0, 50);
	if (ret) {
		printk("mmw SemaphoreCreate failed %d\n", ret);
		return ret;
	}
	if (pdPASS != xTaskCreate(&mmw_ctrl_task, "mmw_ctrl task", 2048/4, NULL, 11, &mmw_task_handle)) {
		printf("mmw_ctrl task create failed\n");
		return -1;
	}

#if !(CONFIG_MMW_DRIVER)
	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(mbox_0));
	mbox_init_channel(&mmw_rx_channel, dev, LL_MBOX_CHAN_ID_MMW);
	ret = mbox_register_callback(&mmw_rx_channel, mmw_irq_handle, NULL);
	if (ret) {
		printk("MMW mbox_register_callback failed %d\n", ret);
		return ret;
	}
	ret = mbox_set_enabled(&mmw_rx_channel, true);
	if (ret) {
		printk("MMW mbox_set_enabled failed %d\n", ret);
		return ret;
	}
#endif

#if (CONFIG_MMW_LOOP_TASK_MONITOR)
    mmw_loop_task_monitor_init();
#endif

	ret = mmw_data_callback(NULL, MMW_DATA_TYPE_DISABLE);
	cb_data_type = MMW_DATA_TYPE_DISABLE;
	if (ret) {
		printk("mmw_data_callback failed %d\n", ret);
	}

	return ret;
}

void mmw_callback_deinit(void)
{
#if !(CONFIG_MMW_DRIVER)
	if (g_mmw_init) {
		mbox_set_enabled(&mmw_rx_channel, false);
		mbox_register_callback(&mmw_rx_channel, NULL, NULL);
	}
#endif
	//k_thread_abort(&mmw_task_handle);
	vTaskDelete(mmw_task_handle);
	OSI_SemaphoreDelete(&cb_mmw_event_sema);
}

int mmw_sweep_chirp_info_get(SweepChirpInfo *Param)
{
	int ret = 0;
	/*get start;Freq; End freq*/
	ret = mmw_freq_get(&Param->start_freq, &Param->max_freq);
	if (ret) {
		return ret;
	}
	/*get slope;Sweep Bw;Valid Start Freq;Valid Bw;Sample Number;Chirp period*/
	ret = mmw_chirp_get(&Param->cfg);
	if (ret) {
		return ret;
	}
	/*get Ramp Time;Sample Time*/
	Param->ramp_time = Param->cfg.sweep_BW_MHz/Param->cfg.slope_MHz_us;
	Param->sample_time = Param->cfg.valid_BW_MHz/Param->cfg.slope_MHz_us;

	Param->max_freq = Param->start_freq + Param->cfg.sweep_BW_MHz;

	return ret;
}


