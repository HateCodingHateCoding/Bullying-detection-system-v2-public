/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */

/************** Include Files **************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "mmw_point_cloud_psic_lib.h"

#include "mmw_app_micro_pointcloud.h"
#include "mmw_alg_micro_cube.h"
#include "mmw_alg_debug.h"

/* micro cube config */
#define FAST_CUBE_FFT_AUTOGAIN          1
#if (FAST_CUBE_FFT_AUTOGAIN)
#define FAST_CUBE_FFT_GAIN_ADAPT_S16    (-10) /* -30dB for int16_t */
#endif
#define MICRO_CHIRP_DOP1_EN             0  /* using fast doppler +-1 */

/* debug config */
#define DEBUG_MICRO_CUBE_PRINT          0
#if (DEBUG_MICRO_CUBE_PRINT >= 3)
#define mcb_debug(fmt, arg...)         printk("[MCB]"fmt, ##arg)
#else
#define mcb_debug(fmt, arg...)
#endif
#if (DEBUG_MICRO_CUBE_PRINT >= 2)
#define mcb_info(fmt, arg...)          printk("[MCB]"fmt, ##arg)
#else
#define mcb_info(fmt, arg...)
#endif
#if (DEBUG_MICRO_CUBE_PRINT >= 1)
#define mcb_error(fmt, arg...)         printk("[MCB]"fmt, ##arg)
#else
#define mcb_error(fmt, arg...)
#endif

#define MICRO_CUBE_INITED             BIT(0)
#define MICRO_CUBE_CHIRP_READY        BIT(1)
#define MICRO_CUBE_NOISE_READY        BIT(2)
typedef struct _micro_cube_local_t {
	/*micro cube buffers */
	complex16_cube *chirp_cube;   /* [MICRO_RANGE_BIN_NUM] [MICRO_CHIRP_STORE_CNT] [MMW_ANT_CHANNEL_NUM]*/
	Noisetype      *noise_cube;   /* [Ping/Pang] [MICRO_RANGE_BIN_NUM] [MMW_ANT_CHANNEL_NUM] */
	void           *store_bufs;   /* [MICRO_CHIRP_NUM] [MMW_ANT_CHANNEL_NUM] */
	complex16_cube *chirp_bufs;   /* [MICRO_CHIRP_NUM] [MMW_ANT_CHANNEL_NUM] */

	/*micro cube control */
	uint16_t 	range_num_max;  //datacube max buffer
	uint16_t 	range_start;
	uint16_t 	range_end;     //runtime config
	uint16_t 	doppler_num;   /* sync with mmw ctrl */
	uint8_t     ant_tx_num;  /* sync with mmw ctrl */
	uint8_t     ant_rx_num;  /* sync with mmw ctrl */
	uint8_t     ant_mimo_num;  /* sync with mmw ctrl */
	uint8_t     reserved[2];
	uint8_t     chirp_store_num;
	uint8_t     chirp_read_num;
	/* state */
	uint16_t    chirp_read_idx;
	uint16_t    chirp_store_idx;
	uint32_t    cube_state;

    uint8_t     auto_gain_en;
} MICRO_CUBE_LOCAL;
MICRO_CUBE_LOCAL  g_micro_cube;

#if (FAST_CUBE_FFT_AUTOGAIN)
void mmw_fft_ants_auto_gain(complex16_cube *buffer, uint16_t interval_id, uint16_t range_idx)
{
	uint8_t tx_num  = g_micro_cube.ant_tx_num;
	uint8_t ant_num = g_micro_cube.ant_rx_num;
	__mmw_fft_ants(buffer, interval_id, range_idx);

    /* convert buffer to Q31 */
    for (uint32_t tx_idx = 0; tx_idx < tx_num; tx_idx++) {
        uint8_t q_tar = mmw_psic_auto_gain_base_get(tx_idx) + mmw_psic_auto_gain_rbin_get(range_idx, tx_idx);
		// shift_value is the value that will be left-shifted to get q31 data.
        int8_t shift_value = (16 + FAST_CUBE_FFT_GAIN_ADAPT_S16) - q_tar;
        if (shift_value > 0) {
			for (uint8_t ant_idx = 0; ant_idx < ant_num; ant_idx++) {
                buffer->real = buffer->real << shift_value;
                buffer->imag = buffer->imag << shift_value;
				buffer++;
            }
		} else {
			for (uint8_t ant_idx = 0; ant_idx < ant_num; ant_idx++) {
                buffer->real = buffer->real >> (-shift_value);
                buffer->imag = buffer->imag >> (-shift_value);
				buffer++;
			}
        }
    }
}

void mmw_fft_range_auto_gain(complex16_cube *buffer, uint16_t start, uint16_t read_num,
					   uint8_t tx_id, uint8_t rx_id, uint16_t interval_idx)
{
	__mmw_fft_range(buffer, start, read_num, tx_id, rx_id, interval_idx);
	uint8_t q_base = mmw_psic_auto_gain_base_get(tx_id);
    /* convert buffer to Q31 */
    for (uint32_t r_idx = start; r_idx < (start + read_num); r_idx++) {
        uint8_t q_tar = q_base + mmw_psic_auto_gain_rbin_get(r_idx, tx_id);
		// shift_value is the value that will be left-shifted to get q31 data.
        int8_t shift_value = (16 + FAST_CUBE_FFT_GAIN_ADAPT_S16) - q_tar;
        if (shift_value > 0) {
            buffer->real = buffer->real << shift_value;
            buffer->imag = buffer->imag << shift_value;
		} else {
            buffer->real = buffer->real >> (-shift_value);
            buffer->imag = buffer->imag >> (-shift_value);
        }
		buffer++;
    }
}

void mmw_fft_doppler_auto_gain(complex16_cube *buffer, uint16_t read_num,
	                uint8_t tx_id, uint8_t rx_id, uint16_t range_idx)
{
	__mmw_fft_doppler(buffer, 0, read_num, tx_id, rx_id, range_idx);
	/* convert buffer to Q31 */
	uint8_t q_tar = mmw_psic_auto_gain_base_get(tx_id) + mmw_psic_auto_gain_rbin_get(range_idx, tx_id);
	// shift_value is the value that will be left-shifted to get q31 data.
    int8_t shift_value = (16 + FAST_CUBE_FFT_GAIN_ADAPT_S16) - q_tar;
    if (shift_value > 0) {
		for (uint32_t dop_idx = 0; dop_idx < read_num; dop_idx++) {
            buffer->real = buffer->real << shift_value;
            buffer->imag = buffer->imag << shift_value;
			buffer++;
		}
	} else {
		shift_value = (-shift_value);
		for (uint32_t dop_idx = 0; dop_idx < read_num; dop_idx++) {
        	buffer->real = buffer->real >> shift_value;
        	buffer->imag = buffer->imag >> shift_value;
			buffer++;
		}
    }
}

#endif

void micro_noise_estimation(complex16_cube *range_data, uint32_t range_num, Noisetype *noiseth)
{
	int i = 0;
	Noisetype noise_sum = 0;
	/* calc first noise_sum of first section */
	do {
		noise_sum += COMPLEX16_POWER(range_data + i);
	} while (++i < NOISE_SEC_NUM);

	/* output left of first section */
	for (int j = 0; j < (NOISE_SEC_NUM - NOISE_SEC_RIGHT); ++j) {
		*noiseth += noise_sum;
		 noiseth += MMW_ANT_CHANNEL_NUM;
	}

	/* output moving section */
	while (i++ < range_num) {
		noise_sum -= COMPLEX16_POWER(range_data);
		noise_sum += COMPLEX16_POWER(range_data + NOISE_SEC_NUM);
		*noiseth  += noise_sum;
		 noiseth  += MMW_ANT_CHANNEL_NUM;
		range_data++;
	}

	/* output right of last section */
	for (int j = 0; j < NOISE_SEC_RIGHT; ++j) {
		*noiseth += noise_sum;
		 noiseth += MMW_ANT_CHANNEL_NUM;
	}
}

int micro_cube_noise_store(uint32_t chirps_idx, uint32_t doppler_num)
{
	register uint32_t ant_mimo_num  = g_micro_cube.ant_mimo_num;
	register Noisetype  *noise_cube = g_micro_cube.noise_cube;
	uint16_t range_start = g_micro_cube.range_start;
	uint16_t range_num = g_micro_cube.range_end - range_start;
	complex16_cube *store_data = g_micro_cube.store_bufs;

	/* reset noise cube at the first chirp */
	if (chirps_idx == 0) {
		mcb_debug("Noise Reset %p\n", noise_cube);
		memset(noise_cube, 0, sizeof(Noisetype) * ant_mimo_num * range_num);
	} else {
		mcb_debug("Noise Calc %d\n", chirps_idx);
	}
	for (uint32_t rx_num = 0; rx_num < ant_mimo_num; ++rx_num) {
		uint8_t tx_id = (rx_num>>2);
		uint8_t rx_id = (rx_num & 0x3);
		/* Get dop0,1,63 chirp data and do noise estimation */
#if (FAST_CUBE_FFT_AUTOGAIN)
        if (g_micro_cube.auto_gain_en) {
            mmw_fft_range_auto_gain((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, 0);        
            micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
            mmw_fft_range_auto_gain((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, 1);
            micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
            mmw_fft_range_auto_gain((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, (doppler_num-1));
            micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
        } else	{
            __mmw_fft_range((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, 0);
            micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
            __mmw_fft_range((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, 1);
            micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
            __mmw_fft_range((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, (doppler_num-1));
            micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
        }
#else
        __mmw_fft_range((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, 0);
        micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
        __mmw_fft_range((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, 1);
        micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
        __mmw_fft_range((complex16_cube *)store_data, range_start, range_num, tx_id, rx_id, (doppler_num-1));
        micro_noise_estimation((complex16_cube *)store_data, range_num, noise_cube);
#endif
		noise_cube++;
	}
	if (chirps_idx == (g_micro_cube.chirp_read_num - 1)) {
		g_micro_cube.cube_state |= MICRO_CUBE_NOISE_READY;
	}

	return 0;
}

int micro_cube_chirp_store(uint16_t chirps_idx, uint16_t doppler0_idx)
{
	uint32_t ant_mimo_num = g_micro_cube.ant_mimo_num;
	uint32_t chirp_range_step = ant_mimo_num * g_micro_cube.chirp_store_num;
	complex16_cube *chirp_cube = &g_micro_cube.chirp_cube[chirps_idx * ant_mimo_num];
	mcb_debug("chirp%d store cube %p\n", chirps_idx, chirp_cube);
	/* Get static doppler chirp data of ants, store in Micro Cube */
	for (uint32_t ridx = g_micro_cube.range_start; ridx < g_micro_cube.range_end; ++ridx) {
#if !(MICRO_CHIRP_DOP1_EN)
	#if (FAST_CUBE_FFT_AUTOGAIN)
	if (g_micro_cube.auto_gain_en) {
		mmw_fft_ants_auto_gain((complex16_cube *)chirp_cube, doppler0_idx, ridx);
	} else {
		__mmw_fft_ants((complex16_cube *)chirp_cube, doppler0_idx, ridx);
	}
	#else
		__mmw_fft_ants((complex16_cube *)chirp_cube, doppler0_idx, ridx);
	#endif
#else
		complex16_cube chirp_buff[MMW_ANT_CHANNEL_NUM];
	#if (FAST_CUBE_FFT_AUTOGAIN)
	if (g_micro_cube.auto_gain_en) {
		mmw_fft_ants_auto_gain((complex16_cube *)chirp_buff, doppler0_idx - 1, ridx);
		mmw_fft_ants_auto_gain((complex16_cube *)chirp_cube, doppler0_idx + 1, ridx);
	} else {
		__mmw_fft_ants((complex16_cube *)chirp_buff , doppler0_idx - 1, ridx);
		__mmw_fft_ants((complex16_cube *)chirp_cube, doppler0_idx + 1, ridx);
	}
	#else
		__mmw_fft_ants((complex16_cube *)chirp_buff , doppler0_idx - 1, ridx);
		__mmw_fft_ants((complex16_cube *)chirp_cube, doppler0_idx + 1, ridx);
	#endif
		vector_add_q15((int16_t *)chirp_cube, (int16_t *)chirp_cube, chirp_buff, ant_mimo_num*2);
#endif
		chirp_cube += chirp_range_step;
	}
	return 0;
}

int micro_cube_data_store(void)
{
	uint32_t time_start;
	uint16_t frame_idx = g_micro_cube.chirp_store_idx;
	uint16_t   doppler_num = g_micro_cube.doppler_num;
	uint16_t   chirps_idx  = (frame_idx % g_micro_cube.chirp_store_num);

	SET_TIME_START(time_start);
	if (!(g_micro_cube.cube_state & MICRO_CUBE_INITED)) {
		mcb_error("ERR: micro cube is not inited!=0x%08x\n", g_micro_cube.cube_state);
		return -1;
	}

	/* 1. sync MMW Config at first chirp */
	if (!(g_micro_cube.cube_state & MICRO_CUBE_CHIRP_READY) &&
		(chirps_idx == 0)) {
		MmwPsicMimoRxNum_t mimo_info;
		uint16_t range_num = 0;
		mmw_fft_num_get(&range_num, &doppler_num);
		g_micro_cube.doppler_num = doppler_num;
		g_micro_cube.range_start = MIN(g_micro_cube.range_start, range_num);
		g_micro_cube.range_end	 = MIN(g_micro_cube.range_end, range_num);

		mmw_psic_lib_sdk_get_tx_rx_num(&mimo_info);
		g_micro_cube.ant_tx_num = mimo_info.ant_tx_num;
		g_micro_cube.ant_rx_num = mimo_info.ant_rx_num;
		g_micro_cube.ant_mimo_num = mimo_info.mimo_rx_num;
		mcb_debug("range start=%d, end=%d!\n", g_micro_cube.range_start, g_micro_cube.range_end);
	}

	/* 2. Store chirp data to micro cube and calc noise data */
	mmw_motion_cube_access_open();
	micro_cube_chirp_store(chirps_idx, doppler_num>>1);
	if (!(g_micro_cube.cube_state & MICRO_CUBE_NOISE_READY)) {
		micro_cube_noise_store(chirps_idx, doppler_num);
	}
	mmw_motion_cube_access_close();

	/* 3. Update micro cube state */
	g_micro_cube.chirp_store_idx = ++frame_idx;
	if (!(g_micro_cube.cube_state & MICRO_CUBE_CHIRP_READY)) {
		if (frame_idx > g_micro_cube.chirp_read_num) {
			g_micro_cube.cube_state |= MICRO_CUBE_CHIRP_READY;
		}
	} else {
		if (++g_micro_cube.chirp_read_idx >= g_micro_cube.chirp_store_num) {
			g_micro_cube.chirp_read_idx = 0;
		}
	}

	SET_TIME_ESCAPE(time_start, time_start);
	UPDATE_TIME_DBG_INFO(time_start, "mstore", g_time_dbg_info.micro_cube_store);

	mcb_debug("cube state%x store %d read %d!\n", g_micro_cube.cube_state,
		      g_micro_cube.chirp_store_idx, g_micro_cube.chirp_read_idx);
	return 0;
}

static void micro_cube_dop_copy(complex16_cube *chirp_buf,
		complex16_cube *cube_range, uint32_t chirp_off, uint32_t range_step)
{
	uint32_t ant_mimo_num	= g_micro_cube.ant_mimo_num;
	uint32_t chirp_num		= g_micro_cube.chirp_read_num;
	/* chirps read */
	for (int i = 0; i < chirp_num; ++i) {
		for (int j = 0; j < ant_mimo_num; ++j) {
			chirp_buf[j*chirp_num] = cube_range[chirp_off++];
		}
		if (chirp_off >= range_step) {
			chirp_off = 0;
		}
		chirp_buf++;
	}
}

complex16_cube *micro_cube_chirps_get(uint32_t range_idx)
{
	uint32_t time_start;
	SET_TIME_START(time_start);
	if (g_micro_cube.cube_state & MICRO_CUBE_CHIRP_READY) {
		complex16_cube *cube_range;
		uint32_t ant_mimo_num = g_micro_cube.ant_mimo_num;
		uint32_t chirp_offs = ant_mimo_num * g_micro_cube.chirp_read_idx;
		uint32_t range_step = ant_mimo_num * g_micro_cube.chirp_store_num;
		uint32_t range_off = ((range_idx > g_micro_cube.range_start) ? 
				              (range_idx - g_micro_cube.range_start) : 0);
		range_off  = range_off * range_step;
		cube_range = &g_micro_cube.chirp_cube[range_off];
		micro_cube_dop_copy(g_micro_cube.chirp_bufs, cube_range, chirp_offs, range_step);
		SET_TIME_ESCAPE(time_start, time_start);
		UPDATE_TIME_DBG_INFO(time_start, "mread", g_time_dbg_info.micro_cube_read);
		return g_micro_cube.chirp_bufs;
	}
	return NULL;
}

Noisetype *micro_cube_noise_get(uint32_t range_idx)
{
	if (g_micro_cube.cube_state & MICRO_CUBE_NOISE_READY) {
		uint32_t ant_mimo_num = g_micro_cube.ant_mimo_num;
		uint32_t range_off = ((range_idx > g_micro_cube.range_start) ? 
				              (range_idx - g_micro_cube.range_start) : 0);
		mcb_debug("noise get r%d, off%d\n", range_idx, range_off);
		return &g_micro_cube.noise_cube[ant_mimo_num*range_off];
	}
	return NULL;
}

void micro_cube_reset(void)
{
	g_micro_cube.cube_state &= MICRO_CUBE_INITED;
	g_micro_cube.chirp_store_idx = 0;
	g_micro_cube.chirp_read_idx  = 0;
}

void micro_cube_auto_gain_set(uint8_t en)
{
    g_micro_cube.auto_gain_en = en;
	if ((FAST_CUBE_FFT_AUTOGAIN == 0) && (g_micro_cube.auto_gain_en == 1)) {
		printf("Auto Gain Function Is Not Enabled!\r\n");
	}
}

bool micro_cube_ready(void)
{
	return (g_micro_cube.cube_state & (MICRO_CUBE_CHIRP_READY|MICRO_CUBE_NOISE_READY)) == 
		   (MICRO_CUBE_CHIRP_READY|MICRO_CUBE_NOISE_READY);
}

int micro_cube_range_set(uint16_t range_start, uint16_t range_end)
{
	g_micro_cube.range_start = range_start;
	g_micro_cube.range_end   = MIN(range_end, range_start + g_micro_cube.range_num_max);
	micro_cube_reset();
	return MMW_ERR_CODE_SUCCESS;
}

int micro_cube_range_get(uint16_t *range_start, uint16_t *range_end)
{
	*range_start = g_micro_cube.range_start;
	*range_end   = g_micro_cube.range_end;
	return MMW_ERR_CODE_SUCCESS;
}

uint16_t micro_cube_chirp_num_get(void)
{
	return g_micro_cube.chirp_read_num;
}

/* 73.29K = 64K + 8K + 1K + 0.29K   @ 8chirps range256*/
/* 137.54K = 128K + 8K + 1K + 0.54K @ 16chirps range256 */
int micro_cube_init(uint16_t range_start, uint16_t range_end, uint16_t chirp_num, uint16_t ant_num)
{
	if (g_micro_cube.cube_state & MICRO_CUBE_INITED) {
		return 0;
	}
	if (range_end <= range_start || range_end < NOISE_SEC_NUM || range_end > 1024) {
		return -2;
	}
	uint16_t range_num = range_end - range_start;

	/* Initial micro cube control */
	g_micro_cube.range_start = range_start;
	g_micro_cube.range_end   = range_end;
	g_micro_cube.range_num_max   = range_num;
	g_micro_cube.ant_mimo_num    = ant_num;
	g_micro_cube.chirp_read_num  = chirp_num;
	g_micro_cube.chirp_store_num = chirp_num + MICRO_CUBE_STORE_EXT_NUM;

	/* Alloc micro cube,  64K @ 8chirps, 8MIMO, Range256 */
	uint32_t buff_size = sizeof(complex16_cube) * ant_num * g_micro_cube.chirp_store_num * range_num;
	g_micro_cube.chirp_cube = (complex16_cube *)os_malloc(buff_size);
	if (g_micro_cube.chirp_cube == NULL) {
		mcb_error("micro cube alloc %d failed!\n", buff_size);
		goto err;
	}

	/* Alloc micro noise buffer,  8K @ 8MIMO, Range256 */
	buff_size = sizeof(complex16_cube) * (ant_num * range_num);
	g_micro_cube.noise_cube = (Noisetype *)os_malloc(buff_size);
	if (g_micro_cube.noise_cube == NULL) {
		mcb_error("micro noise_cube alloc %d failed!\n", buff_size);
		goto err;
	}

	/* buffers */
	buff_size = sizeof(complex16_cube) * (chirp_num * ant_num + ant_num);  /* 8chirp=288, 16chirp=544*/
	g_micro_cube.chirp_bufs = os_malloc(buff_size);
	if (g_micro_cube.chirp_bufs == NULL) {
		mcb_error("read_bufs malloc %d failed!\n", buff_size);
		goto err;
	}

	buff_size = sizeof(complex16_cube)*range_num;  /* 1K = 4* 256 */
	g_micro_cube.store_bufs = os_malloc(buff_size);
	if (g_micro_cube.store_bufs == NULL) {
		mcb_error("store_bufs malloc %d failed!\n", buff_size);
		goto err;
	}
	g_micro_cube.cube_state = MICRO_CUBE_INITED;
	mcb_debug("micro cube=%p read_buf=%p range[%d, %d) chirps=%d ant=%d\n",
		g_micro_cube.chirp_cube, g_micro_cube.chirp_bufs,
		g_micro_cube.range_start, g_micro_cube.range_end, chirp_num, ant_num);
	return 0;

err:
	if (g_micro_cube.chirp_cube != NULL) {
		os_free(g_micro_cube.chirp_cube);
		g_micro_cube.chirp_cube = NULL;
	}
	if (g_micro_cube.noise_cube != NULL) {
		os_free(g_micro_cube.noise_cube);
		g_micro_cube.noise_cube = NULL;
	}

	if (g_micro_cube.chirp_bufs != NULL) {
		os_free(g_micro_cube.chirp_bufs);
		g_micro_cube.chirp_bufs = NULL;
	}
	if (g_micro_cube.store_bufs  != NULL) {
		os_free(g_micro_cube.store_bufs);
		g_micro_cube.store_bufs = NULL;
	}
	return -1;
}

void micro_cube_deinit(void)
{
	if (g_micro_cube.chirp_cube) {
		os_free(g_micro_cube.chirp_cube);
		g_micro_cube.chirp_cube = NULL;
	}
	if (g_micro_cube.noise_cube) {
		os_free(g_micro_cube.noise_cube);
		g_micro_cube.noise_cube = NULL;
	}
	if (g_micro_cube.chirp_bufs != NULL) {
		os_free(g_micro_cube.chirp_bufs);
		g_micro_cube.chirp_bufs = NULL;
	}
	if (g_micro_cube.store_bufs  != NULL) {
		os_free(g_micro_cube.store_bufs );
		g_micro_cube.store_bufs = NULL;
	}
	g_micro_cube.cube_state = 0;
}

