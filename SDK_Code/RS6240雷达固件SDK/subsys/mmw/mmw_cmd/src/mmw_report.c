/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <hal_types.h>
#include "mmw_ctrl.h"
#include "mmw_report.h"
#include "mmw_alg_debug.h"

#include "hif_com.h"
#include "hif_checksum.h"
#include "log.h"
#include "util.h"

#include "mmw_app_pointcloud_config.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_app_micro_pointcloud.h"
#include "mmw_point_cloud_psic_lib.h"
#include <math.h>


#ifndef MIN
#define MIN(x, y)					((x) < (y) ? (x) : (y))
#endif

/* 
 * The fisrt and second frame after call mmw_start() contains invalid data, so we should bypass 2st frame
 * This flag is set in cmd_mmw_report() function, and count down in mmw_ctrl_data_cube_spi/uart_report_cb/mmw_ctrl_data_point_report_cb()
 */
static uint8_t s_mmw_radar_analysis_skip_frame_status;

#if (CONFIG_MMW_MICRO_POINT_CLOUD)
extern uint8_t g_micro_point_en;
#endif
/* cfar method select: 0/1 */
extern uint8_t g_cfar_method;

int get_tx_rx_num(uint8_t *tx, uint8_t *rx)
{
	int ret = 0;
	uint8_t work = 0;
	uint8_t txrx = 0;

	ret = mmw_mode_get(&txrx, &work);
	if(ret) {
		printk("mmw mode get fail ! ret = %d \r\n", ret);
		return ret;
	}

	switch(txrx) {
		case MMW_MIMO_1T1R:
			*tx = 1;
			*rx = 1;
			break;
		case MMW_MIMO_1T3R:
			*tx = 1;
			*rx = 3;
			break;
		case MMW_MIMO_2T3R:
			*tx = 2;
			*rx = 3;
			break;
		case MMW_MIMO_1T4R:
			*tx = 1;
			*rx = 4;
			break;
		case MMW_MIMO_2T4R:
			*tx = 2;
			*rx = 4;
			break;
		case MMW_MIMO_1T2R:
			*tx = 1;
			*rx = 2;
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

struct cube_report_info_t{
	MMW_FRAME_UPLOAD report_hdr;
	uint32_t report_time_ms;
	uint32_t next_frame_time;
	uint32_t frame_cb_cnt;
	uint16_t range_fft_num;
	uint16_t doppler_fft_num;
	uint16_t range_idx;
	uint16_t dop_idx;
	uint16_t block_cnt;
    uint16_t block_idx;
    uint16_t finish_idx;
	uint8_t  aborted;
	uint8_t  reserve;
	uint8_t tx_cnt;
	uint8_t rx_cnt;
	uint8_t tx_idx;
	uint8_t rx_idx;
} cube_report_info;

struct cube_retry_info_t{
    HOST_POLL_APP_CUBE *retry_cfg;
    uint16_t retry_block_idx;
    uint16_t retry_offset;
    uint16_t retry_arr_idx;
} cube_retry_info;

#define CUBE_CACHE_BLOCK    (4)
OSI_Semaphore_t cube_report_sem;
uint8_t *cube_cache[CUBE_CACHE_BLOCK];
bool cube_access = false;

Detection3D_Data D_data_test = {0x00};
Detection3D_State D_State_test = {0x00};

void cube_report_frame_reset(void)
{
	cube_report_info.report_hdr.offset = 0;
	cube_report_info.range_idx = 0;
	cube_report_info.dop_idx   = 0;
	cube_report_info.tx_idx = 0;
	cube_report_info.rx_idx = 0;

	cube_report_info.block_idx = 0;
	cube_report_info.finish_idx = 0;
	cube_report_info.aborted   = 0;
}

static bool is_cube_report_frame_finish(void)
{
	return (cube_report_info.block_idx && cube_report_info.finish_idx >= cube_report_info.block_idx);
}


static void cube_access_ctrl(bool enable)
{
    if(enable == cube_access) {
        return ;
    } else {
        cube_access = enable;
        if(enable) {
            mmw_motion_cube_access_open();
        } else {
            mmw_motion_cube_access_close();
        }
    }
}

void cube_report_callback(hif_queue_item_t *hif_queue_item, int8_t status);
static int hif_dma_cube_report_handler(uint8_t idx)
{
	HIF_MsgHdr_t hifHdr = {0x00};
	uint8_t *cube_data = NULL;
	uint16_t read_num = 0;
	uint16_t offset = CUBE_HEAD_LEN;
	uint16_t read_cnt = 0;
    int ret = 0;

    hifHdr.flag = (1<<2);
	hifHdr.msg_id = 0xC1;
	hifHdr.type = 2;

    LOG_IO(0x1000, 11, 1, 0);
	//memset(cube_cache[idx], 0, CUBE_SIZE_MAX);
	cube_data = cube_cache[idx];

    for(uint32_t i=0; i<cube_report_info.tx_cnt*cube_report_info.rx_cnt*cube_report_info.doppler_fft_num;i++ ) {
        if((CUBE_SIZE_MAX - offset) >= 4 * (cube_report_info.range_fft_num - cube_report_info.range_idx)) {
            read_num = cube_report_info.range_fft_num - cube_report_info.range_idx;
        } else {
            read_num = (CUBE_SIZE_MAX - offset) / 4;
        }
        __mmw_fft_range((complex16_cube *)&cube_data[offset], cube_report_info.range_idx, read_num, cube_report_info.tx_idx, cube_report_info.rx_idx, cube_report_info.dop_idx);
        offset = offset + (4*read_num);
        cube_report_info.range_idx += read_num;
        if(cube_report_info.range_idx >= cube_report_info.range_fft_num) {
            cube_report_info.range_idx = 0;
            cube_report_info.dop_idx ++;
            if(cube_report_info.dop_idx == cube_report_info.doppler_fft_num) {
                cube_report_info.dop_idx = 0;
                cube_report_info.rx_idx ++;
                if(cube_report_info.rx_idx == cube_report_info.rx_cnt) {
                    cube_report_info.rx_idx = 0;
                    cube_report_info.tx_idx ++;
                    if(cube_report_info.tx_idx == cube_report_info.tx_cnt) {
                        cube_report_info.tx_idx = 0;
                    }
                }
            }
        }
        read_cnt = (offset - CUBE_HEAD_LEN) / 4;
        if(offset == (CUBE_SIZE_MAX) || (cube_report_info.report_hdr.frame_len - cube_report_info.report_hdr.offset - read_cnt) == 0) {
            memcpy(cube_data, &cube_report_info.report_hdr, sizeof(MMW_FRAME_UPLOAD));
            hifHdr.length = offset;
            ret = HIF_MsgReport(&hifHdr, cube_data, cube_report_callback);
            cube_report_info.report_hdr.offset += read_cnt;
            LOG_IO(0x1000, 11, 0, 0);
            return ret;
        }
    }
	return -1;
}

void cube_report_retry_callback(hif_queue_item_t *hif_queue_item, int8_t status);
static int hif_dma_cube_report_retry_send(uint32_t retry_offset, uint8_t retry_idx)
{
    uint16_t retry_range_idx = retry_offset % cube_report_info.range_fft_num;
    uint16_t retry_dop = (retry_offset - retry_range_idx) / cube_report_info.range_fft_num;
    uint16_t retry_dop_idx = retry_dop % cube_report_info.doppler_fft_num;
    uint16_t retry_rx = (retry_dop - retry_dop_idx) / cube_report_info.doppler_fft_num;
    uint8_t retry_rx_idx = retry_rx % cube_report_info.rx_cnt;
    uint16_t retry_tx = (retry_rx - retry_rx_idx) / cube_report_info.rx_cnt;
    uint8_t retry_tx_idx = retry_tx % cube_report_info.tx_cnt;

    MMW_FRAME_UPLOAD retry_report_hdr;
	HIF_MsgHdr_t hifHdr = {0x00};
	uint8_t *cube_data = NULL;
	uint16_t read_num = 0;
	uint16_t offset = CUBE_HEAD_LEN;
	uint16_t read_cnt = 0;

    hifHdr.flag = (1<<2);
	hifHdr.msg_id = 0xC1;
	hifHdr.type = 2;

    LOG_IO(0x1000, 11, 1, 0);
	//memset(cube_cache[retry_idx], 0, CUBE_SIZE_MAX);
	cube_data = cube_cache[retry_idx];

    for(uint32_t i = 0; i < cube_report_info.tx_cnt*cube_report_info.rx_cnt*cube_report_info.doppler_fft_num; i++) {
        if((CUBE_SIZE_MAX - offset) >= 4 * (cube_report_info.range_fft_num - retry_range_idx)) {
            read_num = cube_report_info.range_fft_num - retry_range_idx;
        } else {
            read_num = (CUBE_SIZE_MAX - offset) / 4;
        }
        __mmw_fft_range((complex16_cube *)&cube_data[offset], retry_range_idx, read_num, retry_tx_idx, retry_rx_idx, retry_dop_idx);
        offset = offset + (4*read_num);
        retry_range_idx += read_num;
        if(retry_range_idx >= cube_report_info.range_fft_num) {
            retry_range_idx = 0;
            retry_dop_idx ++;
            if(retry_dop_idx == cube_report_info.doppler_fft_num) {
                retry_dop_idx = 0;
                retry_rx_idx ++;
                if(retry_rx_idx == cube_report_info.rx_cnt) {
                    retry_rx_idx = 0;
                    retry_tx_idx ++;
                    if(retry_tx_idx == cube_report_info.tx_cnt) {
                        retry_tx_idx = 0;
                    }
                }
            }
        }
        read_cnt = (offset - CUBE_HEAD_LEN) / 4;
        retry_report_hdr.frame_len = cube_report_info.report_hdr.frame_len;
        retry_report_hdr.offset = retry_offset;
        retry_report_hdr.frame_idx = cube_report_info.report_hdr.frame_idx;
        if(offset == (CUBE_SIZE_MAX) || (retry_report_hdr.frame_len - retry_report_hdr.offset - read_cnt) == 0) {
            memcpy(cube_data, &retry_report_hdr, sizeof(MMW_FRAME_UPLOAD));
            hifHdr.length = offset;
		 	LOG_IO(0x1000, 11, 0, 0);
            return HIF_MsgReport(&hifHdr, cube_data, cube_report_retry_callback);
        }
    }
	return -1;
}

void cube_report_retry_callback(hif_queue_item_t *hif_queue_item, int8_t status)
{
	/* 1. frame is aborted.
     * 2. time is not enough */
    if (cube_report_info.aborted || time_after(csi_coret_get_value(), cube_report_info.next_frame_time)) {
		if (cube_retry_info.retry_cfg) {
			OSI_Free(cube_retry_info.retry_cfg);
			cube_retry_info.retry_cfg = NULL;
			cube_access_ctrl(false);
		}
        return ;
    }

    if(cube_retry_info.retry_cfg && cube_retry_info.retry_cfg->retry_num) {
        LOG_IO(0x1000, 11, 0, 6);
		uint32_t frame_len = cube_report_info.report_hdr.frame_len;
		uint32_t retry_arr_idx = cube_retry_info.retry_arr_idx;
		uint32_t offset = cube_retry_info.retry_cfg->retry[retry_arr_idx].offset + cube_retry_info.retry_offset;
		uint32_t read_num = MIN(CUBE_DATA_SIZE / 4, cube_retry_info.retry_cfg->retry[retry_arr_idx].len);
		if (read_num && (offset + read_num <= frame_len)) {
			hif_dma_cube_report_retry_send(offset, (cube_retry_info.retry_block_idx++) % CUBE_CACHE_BLOCK);
			cube_retry_info.retry_offset += read_num;
			LOG_IO(0x1000, 11, 0, 2);
			if (cube_retry_info.retry_offset == cube_retry_info.retry_cfg->retry[retry_arr_idx].len) {
				cube_retry_info.retry_cfg->retry_num--;
				cube_retry_info.retry_arr_idx += 1;
				cube_retry_info.retry_offset = 0;
				LOG_IO(0x1000, 11, 0, 4);
			}
		} else {
			cube_retry_info.retry_cfg->retry_num = 0;
		}
		if (cube_retry_info.retry_cfg->retry_num == 0) {
			cube_access_ctrl(false);
			OSI_Free(cube_retry_info.retry_cfg);
			cube_retry_info.retry_cfg = NULL;
		}
    }
}

void cube_report_retry_handler(HOST_POLL_APP_CUBE *data)
{
	uint32_t retry_num = data->retry_num;
    if (retry_num) {
		/* 1. frame report is not finish
		 * 2. last retry is in progress
		 * 3. new frame is coming */
		if (cube_report_info.aborted || !is_cube_report_frame_finish() || cube_retry_info.retry_cfg ||
			time_after(csi_coret_get_value(), cube_report_info.next_frame_time)) {
			return ;
		}
	    cube_retry_info.retry_block_idx = 0;
	    cube_retry_info.retry_offset = 0;
	    cube_retry_info.retry_arr_idx = 0;

		uint32_t size = sizeof(HOST_POLL_APP_CUBE) + retry_num * sizeof(HOST_CUBE_RETRY);
        cube_retry_info.retry_cfg = (HOST_POLL_APP_CUBE *)OSI_Malloc(size);
        if(cube_retry_info.retry_cfg == NULL) {
            return ;
        }
        memcpy(cube_retry_info.retry_cfg, (uint8_t *)data, sizeof(HOST_POLL_APP_CUBE) + retry_num * sizeof(HOST_CUBE_RETRY));

		uint32_t retry_arr_idx = cube_retry_info.retry_arr_idx;
		uint32_t frame_len = cube_report_info.report_hdr.frame_len;
        cube_access_ctrl(true);
        for(int i = 0; i < CUBE_CACHE_BLOCK; i++) {
            if (retry_num) {
				uint32_t offset = cube_retry_info.retry_cfg->retry[retry_arr_idx].offset + cube_retry_info.retry_offset;
				uint32_t read_num = MIN(CUBE_DATA_SIZE / 4, cube_retry_info.retry_cfg->retry[retry_arr_idx].len);
				if (read_num && (offset + read_num <= frame_len)) {
                	hif_dma_cube_report_retry_send(offset, (cube_retry_info.retry_block_idx++) % CUBE_CACHE_BLOCK);
                	cube_retry_info.retry_offset += read_num;
	                LOG_IO(0x1000, 11, 0, 2);
	                if (cube_retry_info.retry_offset == cube_retry_info.retry_cfg->retry[retry_arr_idx].len) {
	                    retry_num--;
	                    retry_arr_idx += 1;
	                    cube_retry_info.retry_offset = 0;
	                    LOG_IO(0x1000, 11, 0, 4);
	                }
				} else {
					retry_num = 0;
				}
                if (retry_num == 0) {
                    cube_access_ctrl(false);
                    OSI_Free(cube_retry_info.retry_cfg);
                    cube_retry_info.retry_cfg = NULL;
                    break;
                }
            }
        }
		cube_retry_info.retry_cfg->retry_num = retry_num;
		cube_retry_info.retry_arr_idx = retry_arr_idx;
    }
}

void cube_report_callback(hif_queue_item_t *hif_queue_item, int8_t status)
{
	uint32_t time_start;
	SET_TIME_START(time_start);
	cube_report_info.finish_idx++;
	if (!cube_report_info.aborted) {
		/* there are blocks to fill. */
		if (status == 0 && cube_report_info.block_idx < cube_report_info.block_cnt) {
			hif_dma_cube_report_handler((cube_report_info.block_idx++) % CUBE_CACHE_BLOCK);
		} else if (cube_report_info.finish_idx >= cube_report_info.block_idx) {
			/* All cube read done, release waitting of frame callback. */
			OSI_SemaphoreRelease(&cube_report_sem);
		}
	}
	SET_TIME_ESCAPE(time_start, time_start);
	UPDATE_TIME_DBG_INFO(time_start, "cube-bk", g_time_dbg_info.report_block);
}

void mmw_data_cube_spi_report_deinit(void)
{
	cube_report_info.aborted = true;
	mmw_data_report_hif_deinit();
    for (int i = 0; i < CUBE_CACHE_BLOCK; i++) {
        if (cube_cache[i]) {
            OSI_Free(cube_cache[0]);
		    cube_cache[0] = NULL;
        }
    }
	OSI_SemaphoreDelete(&cube_report_sem);
	//memset(&cube_report_info, 0, sizeof(struct cube_report_info_t));
	//HIF_MsgReport_BurstDeinit();
}

extern uint8_t g_dc_suppression_flag;
int mmw_ctrl_data_cube_spi_report_cb(void *mmw_data, void *arg)
{
	int ret = 0;
	/* Skip default 2 frames to wait for clutter coverage */
	if (!s_mmw_radar_analysis_skip_frame_status) { /* s_mmw_radar_analysis_skip_frame_status is 0 indicates that the invalid data frames has skipped */
		uint32_t time_now = csi_coret_get_value();
		cube_report_info.report_hdr.frame_idx = cube_report_info.frame_cb_cnt++;

		/* last frame is not finish */
		if (cube_report_info.finish_idx < cube_report_info.block_idx) {
			/* printf("spi report drop\n") */;
			return ret;
		}
		if (cube_report_info.report_hdr.frame_idx == 0) {
			uint32_t period_ms = 0;
			uint32_t frame_num = 0;
			mmw_frame_get(&period_ms,  &frame_num);
			uint32_t interval_period = 0;
			uint32_t interval_num = 0;;
			mmw_interval_get(&interval_period,  &interval_num);
			interval_period = (interval_period * interval_num) / 1000;
			if(period_ms > interval_period) {
				cube_report_info.report_time_ms = period_ms - interval_period;
			} else {
				return ret; /* no time to report */
			}    
		}

		if (g_dc_suppression_flag) {
			if (MMW_ERR_CODE_SUCCESS != mmw_psic_dc_suppression_update()) {
				printf("condition error!\r\n");
			}
		}

		/* reset report status */
		cube_report_frame_reset();
		cube_access_ctrl(true);
		for(int i = 0; i < CUBE_CACHE_BLOCK; i++) {
			if(cube_report_info.block_idx < cube_report_info.block_cnt) {
				hif_dma_cube_report_handler((cube_report_info.block_idx++) % CUBE_CACHE_BLOCK);
			}
		}
		/* 500us is margin */
		cube_report_info.next_frame_time = time_now + US_TO_CORET(cube_report_info.report_time_ms*1000 - 500);

		ret = OSI_SemaphoreWait(&cube_report_sem, cube_report_info.report_time_ms);
		if(ret != OSI_STATUS_OK) {
			cube_report_info.aborted = true;
		}
		cube_access_ctrl(false);

		SET_TIME_ESCAPE(time_now, time_now);
		UPDATE_TIME_DBG_INFO(time_now, "report", g_time_dbg_info.report);
	} else {
		s_mmw_radar_analysis_skip_frame_status--; /* count skipped frames */
		/* when clutter_rm_method is DC,skip the last frame init DC */
		if (g_dc_suppression_flag && !s_mmw_radar_analysis_skip_frame_status) {
			mmw_clutter_halt_set(MMW_HALT_CLUTTER_UPDATE_ENABLE);
			mmw_psic_dc_suppression_init();
			/* mmw_psic_dc_suppression_init will clear cluuter and needs to be update once */
			mmw_psic_dc_suppression_update();
		}
	}

	return ret;
}


void mmw_report_param_get(void)
{
	int ret = 0;

	D_State_test.proc_frame_num = 0;

	ret = get_tx_rx_num(&cube_report_info.tx_cnt, &cube_report_info.rx_cnt);
	if(ret) {
		printf("mmw mode get fail ! ret = %d \r\n", ret);
		return;
	}
	ret = mmw_fft_num_get(&cube_report_info.range_fft_num, &cube_report_info.doppler_fft_num);
	if(ret) {
		printk("fft num get fail ! ret = %d \r\n", ret);
		return;
	}

	uint32_t frame_len = cube_report_info.tx_cnt * cube_report_info.rx_cnt *
	                     cube_report_info.range_fft_num * cube_report_info.doppler_fft_num;
	cube_report_info.block_cnt = (frame_len * 4 + CUBE_DATA_SIZE - 1) / CUBE_DATA_SIZE;
	cube_report_info.frame_cb_cnt = 0;
	cube_report_info.report_hdr.frame_len = frame_len;
	cube_report_info.report_hdr.frame_idx = 0;
	cube_report_frame_reset();
}


void mmw_data_report_hif_init(uint8_t mode, uint32_t rate, uint8_t type)
{
	int ret = 0;

	HIF_InitParam_t hifInitParam;
	if (mode == HIF_COM_TYPE_SPI) {
#if CONFIG_R3_DATA_BOX_UPLOAD_DATA_CUBE
        if(type) {
            for(uint8_t i = 0; i<CUBE_CACHE_BLOCK; i++) {
                if(cube_cache[i] == NULL) {
                    cube_cache[i] = (uint8_t *)OSI_Malloc(CUBE_SIZE_MAX);
                }
            }

            OSI_SemaphoreCreate(&cube_report_sem, 0, 1);
        }
#endif
		HIF_DefaultInitParam(&hifInitParam, HIF_COM_TYPE_SPI, 8000000);
	} else if (mode == HIF_COM_TYPE_UART) {
#if CONFIG_R3_DATA_BOX_UPLOAD_DATA_CUBE
        if(cube_cache[0] == NULL) {
		    cube_cache[0] = (uint8_t *)OSI_Malloc(CUBE_SIZE_MAX);
	    }
		if (rate == 0) {
			rate = 10000000;
		}
#endif
		HIF_DefaultInitParam(&hifInitParam, HIF_COM_TYPE_UART, rate);
	} else if (mode == HIF_COM_TYPE_IIC) {
		HIF_DefaultInitParam(&hifInitParam, HIF_COM_TYPE_IIC, 100000);
	} else {
		return ;
	}

	ret = HIF_Init(&hifInitParam);

	if(ret) {
		printf("host interface init fail\r\n");
	}

    //HIF_ExtControl(HIF_SET_SIMPLEX_MODE, (void *)&singleMode, sizeof(uint32_t));
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
	if ((D_data_test.micro_points == NULL) && g_micro_point_en) {
		D_data_test.micro_points = OSI_Malloc(sizeof(PointCloud3D)*MICRO_POINT_MAX);
	}
#endif
}

void mmw_data_report_hif_deinit(void)
{
	mmw_ctrl_callback_cfg(NULL, MMW_DATA_TYPE_DISABLE, NULL);
	HIF_DeInit();
#if (CONFIG_MMW_MICRO_POINT_CLOUD)
	if ((D_data_test.micro_points != NULL) && g_micro_point_en) {
		OSI_Free(D_data_test.micro_points);
		D_data_test.micro_points = NULL;
	}
#endif
	if(cube_cache[0]) {
		OSI_Free(cube_cache[0]);
		cube_cache[0] = NULL;
	}

	memset(&cube_report_info, 0, sizeof(struct cube_report_info_t));
}

int mmw_ctrl_data_cube_uart_report_cb(void *mmw_data, void *arg)
{
	int ret = 0;
	
	if (!s_mmw_radar_analysis_skip_frame_status) {
		HIF_MsgHdr_t msg = {0x00};
		uint8_t *cube_data = NULL;
		uint16_t read_num = 0;
		uint16_t offset = CUBE_HEAD_LEN;
		uint16_t read_cnt = 0;
		uint16_t i=0;
		
		uint32_t time_start;
		SET_TIME_START(time_start);

		msg.flag = 1<<2;
		msg.msg_id = 0xC1;
		msg.type = 2;

		cube_data = cube_cache[0];
		memset(cube_data, 0, CUBE_SIZE_MAX);
		if (g_dc_suppression_flag) {
			if (MMW_ERR_CODE_SUCCESS != mmw_psic_dc_suppression_update()) {
				printf("condition error!\r\n");
			}
		}
			
		ret = mmw_motion_cube_access_open();
		if(ret) {
			printk("cube access open fail ! ret = %d \r\n", ret);
			return 0;
		}

		for (i = 0; i < cube_report_info.tx_cnt * cube_report_info.rx_cnt * cube_report_info.doppler_fft_num;) {
			if((CUBE_HEAD_LEN + CUBE_DATA_SIZE - offset) >= 4 * (cube_report_info.range_fft_num - cube_report_info.range_idx)) {
				read_num = cube_report_info.range_fft_num - cube_report_info.range_idx;
			} else {
				read_num = (CUBE_HEAD_LEN + CUBE_DATA_SIZE - offset) / 4;
			}
			__mmw_fft_range((complex16_cube *)&cube_data[offset], cube_report_info.range_idx, read_num, cube_report_info.tx_idx, cube_report_info.rx_idx, cube_report_info.dop_idx);

			offset = offset + (4*read_num);
			cube_report_info.range_idx += read_num;
			if(cube_report_info.range_idx >= cube_report_info.range_fft_num) {
				cube_report_info.range_idx = 0;
				cube_report_info.dop_idx++;
				i++;
				if(cube_report_info.dop_idx == cube_report_info.doppler_fft_num) {
					cube_report_info.dop_idx = 0;
					cube_report_info.rx_idx ++;
					if(cube_report_info.rx_idx == cube_report_info.rx_cnt) {
						cube_report_info.rx_idx = 0;
						cube_report_info.tx_idx ++;
						if(cube_report_info.tx_idx == cube_report_info.tx_cnt) {
							cube_report_info.tx_idx = 0;
						}
					}
				}
			}
			read_cnt = (offset - CUBE_HEAD_LEN) / 4;
			if(offset >= (CUBE_HEAD_LEN + CUBE_DATA_SIZE) || (cube_report_info.report_hdr.frame_len - cube_report_info.report_hdr.offset - read_cnt) == 0) {
				msg.length = offset;
				memcpy(cube_data, &cube_report_info.report_hdr, sizeof(MMW_FRAME_UPLOAD));
				HIF_MsgReport(&msg, cube_data, NULL);
				if((cube_report_info.report_hdr.frame_len - cube_report_info.report_hdr.offset - read_cnt) == 0) {
					break;
				}
				cube_report_info.report_hdr.offset += read_cnt;
				offset = CUBE_HEAD_LEN;
			}
		}


		cube_report_info.report_hdr.frame_idx ++;
		cube_report_info.report_hdr.offset = 0;
		cube_report_info.dop_idx = 0;
		cube_report_info.range_idx = 0;
		cube_report_info.tx_idx = 0;
		cube_report_info.rx_idx = 0;

		ret = mmw_motion_cube_access_close();
		if(ret) {
			printk("cube access open fail ! ret = %d \r\n", ret);
		}
		SET_TIME_ESCAPE(time_start, time_start);
		UPDATE_TIME_DBG_INFO(time_start, "report", g_time_dbg_info.report);
	} else {
		s_mmw_radar_analysis_skip_frame_status--; /* count skipped frames */
		/* when clutter_rm_method is DC,skip the last frame init DC */
		if (g_dc_suppression_flag && !s_mmw_radar_analysis_skip_frame_status) {
			mmw_clutter_halt_set(MMW_HALT_CLUTTER_UPDATE_ENABLE);
			mmw_psic_dc_suppression_init();
			/* mmw_psic_dc_suppression_init will clear cluuter and needs to be update once */
			mmw_psic_dc_suppression_update();
		}
	}
	
	return 0;
}

void send_obj_data_Callback(hif_queue_item_t *hif_queue_item, int8_t status)
{
}
static void mmw_msg_send_obj_data(MMW_FRAME_UPLOAD *report_hdr, TL_HEADER *tl, uint8_t *payload, uint32_t len)
{
	HIF_MsgHdr_t msg = {0x00};
	msg.flag = 1<<2;
	msg.msg_id = 0xC3;
	msg.type = 2;
	msg.length = 0;

    payload_node_t *payload_head = NULL;

    payload_head = HIF_MsgReport_ListAddData(payload_head, report_hdr, sizeof(MMW_FRAME_UPLOAD));
	if (tl != NULL) {
		payload_head = HIF_MsgReport_ListAddData(payload_head, tl, sizeof(TL_HEADER));
	}
	if (len) {
		payload_head = HIF_MsgReport_ListAddData(payload_head, payload, len);
	}

    HIF_MsgReport_ListStart(&msg, payload_head, NULL);

    if (tl != NULL) {
        report_hdr->offset += (sizeof(TL_HEADER)>>2);
    }
    if(len) {
        report_hdr->offset += (len>>2);
    }
}

void mmw_pointcloud_upload(MMW_FRAME_UPLOAD *frame_hdr, TL_HEADER *tl, uint8_t *payload, uint32_t payload_len)
{
	do {
	 	uint32_t packet_len = MIN(payload_len, POINT_DATA_SIZE);
		payload_len -= packet_len;
		mmw_msg_send_obj_data(frame_hdr, tl, payload, packet_len);
		payload   += packet_len;
		tl = NULL;
	} while (payload_len > 0);
}

void tracker_targets_upload(MMW_FRAME_UPLOAD *frame_hdr, ClusterTrackElm *buff, uint32_t total_num)
{
	TL_HEADER tl;
	TL_HEADER *tl_ptr = &tl;
	tl.type   = 5;
	tl.flag   = 1;
	tl.length = total_num*sizeof(ClusterTrackElm);
	do {
		uint32_t packet_num = MIN(total_num, POINT_DATA_SIZE/sizeof(ClusterTrackElm));
		total_num -= packet_num;
		mmw_msg_send_obj_data(frame_hdr, tl_ptr, (uint8_t *)buff, packet_num*sizeof(ClusterTrackElm));
		buff += packet_num;
		tl_ptr = NULL;
		OSI_MSleep(1);
	} while (total_num > 0);
}

extern uint8_t  gui_data_report;
int mmw_detection3d_data_report(Detection3D_Data *data, Detection3D_State *state, void *arg)
{
	MMW_FRAME_UPLOAD frame_hdr;
	bool upload_point   = false;
	#if (CONFIG_MMW_APP_TRACK_ENABLE)
	bool upload_targets = false;
	#endif
	uint32_t frame_num  = state->proc_frame_num;

	switch (gui_data_report) {
	case GUI_DATA_TYPE_POINTS:
		upload_point = true;
		break;
	case GUI_DATA_TYPE_TRACKING:
	#if (CONFIG_MMW_APP_TRACK_ENABLE)
		upload_targets = true;
	#endif
		break;
	case GUI_DATA_TYPE_POINTS_TRACKING:
		upload_point   = true;
	#if (CONFIG_MMW_APP_TRACK_ENABLE)
		upload_targets = true;
	#endif
		break;
	default:
		return 0;
	}

	/* Setup frame header */
	frame_hdr.frame_idx = frame_num;
	frame_hdr.frame_len = 0;
	frame_hdr.offset	 = 0;
#if 1 /* (CONFIG_MMW_APP_POINT_CLOUD_ENABLE) */
	if (upload_point) {
		frame_hdr.frame_len += ((sizeof(PointCloud_Polar)*data->motion_points_num  + sizeof(TL_HEADER))>>2);
	}
#endif

#if (CONFIG_MMW_MICRO_POINT_CLOUD)
	if (upload_point && g_micro_point_en) {
		frame_hdr.frame_len += ((sizeof(PointCloud_Polar)*data->micro_points_num  + sizeof(TL_HEADER))>>2);
	}
#endif
#if (CONFIG_MMW_APP_TRACK_ENABLE)
	if (upload_targets) {
		frame_hdr.frame_len += ((sizeof(ClusterTrackElm)*data->tracker_objs_num  + sizeof(TL_HEADER))>>2);
	}
#endif

	/* Output data packets */
#if 1 /*CONFIG_MMW_APP_POINT_CLOUD_ENABLE) */
	if (upload_point) { /* motion points packets */
		TL_HEADER tl;
		uint32_t total_len = sizeof(PointCloud_Polar) * data->motion_points_num;
		tl.type    = MMW_TL_TYPE_POINTS;
		tl.flag    = MMW_TL_FLAG_CART;
		tl.length = total_len;
		mmw_pointcloud_upload(&frame_hdr, &tl, (uint8_t *)data->motion_points, tl.length);
	}
#endif

#if (CONFIG_MMW_MICRO_POINT_CLOUD)
	if (upload_point && g_micro_point_en) {
		TL_HEADER tl;
		tl.type    = MMW_TL_TYPE_POINTS;
		tl.flag    = mmw_coordinate_get()|MMW_TL_FLAG_MICRO;
		tl.length  = sizeof(PointCloud_Polar) * data->micro_points_num;
		/* output micro points packets */
		mmw_pointcloud_upload(&frame_hdr, &tl, (uint8_t *)data->micro_points, tl.length);
	}
#endif

#if (CONFIG_MMW_APP_TRACK_ENABLE)
	if (upload_targets) {
		/* tracker targes packets */
		tracker_targets_upload(&frame_hdr, data->tracker_objs, data->tracker_objs_num);
	}
#endif
	(void)frame_hdr;
	return 0;
}

#if (CONFIG_MMW_MICRO_POINT_CLOUD)

typedef struct micro_report_arg {
	PointCloud3D *buffer;
	uint32_t  point_num;
	int32_t   micro_range_unit;  //cm-q4
	int32_t   micro_veloc_unit;  //cm/s_q7
	uint8_t   coordinate_cart;
} MICRO_REPORT_ARG;
MICRO_REPORT_ARG g_micro_arg;

static uint16_t mpc_snr_trans_db(uint32_t snr_lin) //10*log10
{
	uint16_t snr_db = 0;
	while (snr_lin >= 4) {
		snr_db += 3;
		snr_lin = snr_lin>>1;
	}
	return snr_db + (snr_lin == 3 ? 5 : (snr_lin == 2 ? 3 : 0));
}

int mmw_micro_point_handle(uint32_t range_idx, int veloc_idx,
					uint32_t snr_linear, MmwAngleInfo_t *angle, void *arg)
{
	MICRO_REPORT_ARG *report = (MICRO_REPORT_ARG *)arg;
	int32_t range_cm_q4 = range_idx * report->micro_range_unit;
	if (!report->coordinate_cart) {
		PointCloud_Polar *polar = (PointCloud_Polar *)report->buffer;
		polar[report->point_num].range = (range_cm_q4)>>4;
		polar[report->point_num].azim	= (angle->azimuth*100)>>7;
		polar[report->point_num].elev	= (angle->elevation*100)>>7;
	} else {
		PointCloud_Cart *cart = (PointCloud_Cart *)report->buffer;
		int32_t range_cmq1 = (range_cm_q4>>3);
        /* Adjust mount position in dev-board, change radar coordinate to user coordinate */
		mmw_micro_frame_trans_radar_coord_to_user_coord(
							(range_cmq1 * angle->sinValue_X) >> 16,
							(range_cmq1 * angle->sinValue_Y) >> 16,
							(range_cmq1 * angle->sinValue_Z) >> 16,
							&cart[report->point_num]
							);
	}
	report->buffer[report->point_num].polar.vel = (veloc_idx * report->micro_veloc_unit)>>7;
	report->buffer[report->point_num].polar.snr = mpc_snr_trans_db(snr_linear) * 100;
	return (++report->point_num >= MICRO_POINT_MAX);
}
#endif

/* The callback function when 2D-FFT finished in point cloud application */
int mmw_ctrl_data_point_report_cb(void *mmw_data, void *arg)
{
	/* Skip default 2 frames to wait for clutter coverage */
	if (!s_mmw_radar_analysis_skip_frame_status) { /* s_mmw_point_cloud_skip_frame_status is 0 indicates that the invalid data frames has skipped */
		uint32_t time_start, time_motion = 0, time_micro = 0, time_report = 0;

		float sin_y, sin_z, sin_x; /* x, y, z, vel, */

		uint32_t range_mm, range_reol_mm, velocity_mm, veloc_resol_mm;
		uint16_t range_fft_num, doppler_fft_num;
		float range_bin_size_cm, dop_bin_size_cm;
		int16_t doppler_idx_half;
		float user_coordinate[3];
		mmw_range_get(&range_mm, &range_reol_mm);
		mmw_fft_num_get(&range_fft_num, &doppler_fft_num);

		SET_TIME_START(time_start);

		/*--------------------- MOTION POINT PROCESS --------------------- */
		PointCloudBuffer_t *ptr_point_cloud_buffer;
		if (g_cfar_method == 0) {
			ptr_point_cloud_buffer = mmw_point_cloud_process();
		} else if (g_cfar_method == 1) {
			ptr_point_cloud_buffer = mmw_point_cloud_process_sw_cfar();
		} else {
			ptr_point_cloud_buffer = 0;
		}

		SET_TIME_ESCAPE(time_motion, time_start);
		UPDATE_TIME_DBG_INFO(time_motion, "motion", g_time_dbg_info.motion_point);
		SET_TIME_START(time_start);

		/*---------------------  MICRO MOTION POINT PROCESS --------------------- */
		#if (CONFIG_MMW_MICRO_POINT_CLOUD)
		if (g_micro_point_en) {
			if (D_State_test.proc_frame_num == 0) {
				/* init parameters on first frame */
				uint32_t frame_period, dummy;
				uint32_t micro_dop_num = mmw_micro_doppler_num_get();
				mmw_frame_get(&frame_period, &dummy);
				frame_period = frame_period * mmw_micro_frame_rate_get();
				g_micro_arg.coordinate_cart = mmw_coordinate_get();
				g_micro_arg.micro_range_unit = (range_mm<<4)/10/range_fft_num;
				g_micro_arg.micro_veloc_unit = (5*100<<7)/(frame_period * micro_dop_num);
				g_micro_arg.buffer = D_data_test.micro_points;
			} else {
				bool micro_update = mmw_micro_point_frame();
				/* auto gain function must be cleared manually as soon as gain factor is not used,
				   to make auto gain in next frame works well.
				   Hardware reuired gain factor cleared before chirp transmit period
				*/
				mmw_psic_auto_gain_clear();
				if (micro_update) {
					g_micro_arg.point_num = 0;
					mmw_micro_point_process(&mmw_micro_point_handle, &g_micro_arg);
					D_data_test.micro_points_num = g_micro_arg.point_num;
				}
			}
		} else {
			D_data_test.micro_points_num = 0;
			/* auto gain function must be cleared manually as soon as gain factor is not used,
			   to make auto gain in next frame works well.
			   Hardware reuired gain factor cleared before chirp transmit period
			*/
			mmw_psic_auto_gain_clear();
		}
		ADD_TIME_ESCAPE(time_micro, time_start);
		UPDATE_TIME_DBG_INFO(time_micro, "micro", g_time_dbg_info.micro_point);
		SET_TIME_START(time_start);
	#else
		D_data_test.micro_points_num = 0;
		(void)time_micro;
		/* auto gain function must be cleared manually as soon as gain factor is not used,
		   to make auto gain in next frame works well.
		   Hardware reuired gain factor cleared before chirp transmit period
		*/
		mmw_psic_auto_gain_clear();
	#endif

		/*---------------------  DATA CONVERT PROCESS --------------------- */
		mmw_velocity_get(&velocity_mm, &veloc_resol_mm);
		range_bin_size_cm = (float)range_mm / range_fft_num * 0.1f;
		dop_bin_size_cm = (float)velocity_mm / doppler_fft_num * 0.2f; /* velocity_mm is single size */
		doppler_idx_half = doppler_fft_num >> 1;

		/* Convert to cartisian coordinated system, an extra memory copy will be applied */
		if (D_data_test.motion_points == 0){
			//memset(&D_data_test, 0, sizeof(D_data_test));
			mmw_process_mem_alloc((void**)&D_data_test.motion_points, sizeof(*D_data_test.motion_points) * ptr_point_cloud_buffer->point_cloud_num);
		}
		for (int point_cloud_idx = 0; point_cloud_idx < ptr_point_cloud_buffer->point_cloud_num; point_cloud_idx++) {
			/*
			 * unit cm in D_data_test structure,
			 * x = range * cos(phi) * sin(theta)
			 * y = range * cos(phi) * cos(theta)
			 * z = range * sin(phi)
			 * or
			 * x = range * sin(x)
			 * y = range * sqrt(1-sin(x)^2-sin(z)^2)
			 * z = range * sin(z)
			*/
			sin_x = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_cloud_idx].azi_phase;
			sin_z = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_cloud_idx].ele_phase;
			sin_y = 1.f - sin_x * sin_x - sin_z * sin_z;
			sin_y = sqrtf(sin_y);
				mmw_point_cloud_trans_radar_coord_to_user_coord(ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_cloud_idx].range_idx * sin_x * range_bin_size_cm,
									   ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_cloud_idx].range_idx * sin_y * range_bin_size_cm,
									   ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_cloud_idx].range_idx * sin_z * range_bin_size_cm,
									   user_coordinate
									);

			D_data_test.motion_points[point_cloud_idx].cart.x = user_coordinate[0];
			D_data_test.motion_points[point_cloud_idx].cart.y = user_coordinate[1];
			D_data_test.motion_points[point_cloud_idx].cart.z = user_coordinate[2];
			D_data_test.motion_points[point_cloud_idx].cart.vel = (ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_cloud_idx].doppler_idx - doppler_idx_half) * dop_bin_size_cm;
			D_data_test.motion_points[point_cloud_idx].cart.snr = ptr_point_cloud_buffer->ptr_motion_point_cloud_data[point_cloud_idx].sig_snr * 100;
		}

		D_data_test.motion_points_num = ptr_point_cloud_buffer->point_cloud_num;
		D_data_test.tracker_objs_num  = 0;

		mmw_process_mem_free((void**) &ptr_point_cloud_buffer->ptr_motion_point_cloud_data);
		mmw_process_mem_free((void**) &ptr_point_cloud_buffer);
		SET_TIME_ESCAPE(time_start, time_start);
		UPDATE_TIME_DBG_INFO(time_start, "convert", g_time_dbg_info.motion_convert);
		SET_TIME_START(time_start);

		/*---------------------  DATA REPORT PROCESS --------------------- */
		/* This is a block function, free memory after this functon returns */
		mmw_detection3d_data_report(&D_data_test, &D_State_test, NULL);
		ADD_TIME_ESCAPE(time_report, time_start);
		UPDATE_TIME_DBG_INFO(time_report, "report", g_time_dbg_info.report);

		mmw_process_mem_free((void**)&D_data_test.motion_points);
		D_State_test.proc_frame_num++;
		
		if (g_dc_suppression_flag) {
			if (MMW_ERR_CODE_SUCCESS != mmw_psic_dc_suppression_update()) {
				printf("condition error!\r\n");
			}
		}
	} else {
		s_mmw_radar_analysis_skip_frame_status--; /* count skipped frames */
		/* when clutter_rm_method is DC,skip the last frame init DC */
		if (g_dc_suppression_flag && !s_mmw_radar_analysis_skip_frame_status) {
			mmw_clutter_halt_set(MMW_HALT_CLUTTER_UPDATE_ENABLE);
			mmw_psic_dc_suppression_init();
			/* mmw_psic_dc_suppression_init will clear cluuter and needs to be update once */
			mmw_psic_dc_suppression_update();
		}
	}

	return 0;
}

void radar_analysis_reset_skip_frame_status(void) {
	s_mmw_radar_analysis_skip_frame_status = RADAR_ANALYSIS_SKIP_FRAME_NUM;
}
