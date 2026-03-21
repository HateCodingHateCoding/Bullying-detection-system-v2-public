/**
 **************************************************************************************************
 * *@brief   factory test function define.
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
 **************************************************************************************************
 */

#include "ft_noise_est.h"
#include "mmw_alg_pointcloud.h"
#include "mmw_point_cloud_psic_lib.h"
#include "csi_math.h"
#include "mmw_alg_debug.h"
#include "hal_board.h"
#include "log.h"

/* mmw radio filter DC index */
#define CONFIG_FILTER_DC_RANGE_MM				(400)
/* config peak range bin min range scope*/
#define CONFIG_MIN_SCOPE_RANGE_MM				(3000)

/*mmw software param config*/
#define CONFIG_NOISE_SEM_EN						(0)
#define CONFIG_NOISE_BUF_MAX_CHIRP_NUM 			(128)

#if CONFIG_SOC_SERIES_RS624X
#define CONFIG_NOISE_EST_MIMO_MODE 	(MMW_MIMO_2T4R) 	/* MIMO mode config*/
#define CONFIG_MMW_RX_ANT_IO_MASK 	0xF					/* ant msk config*/
#elif CONFIG_SOC_RS6130	
#define CONFIG_NOISE_EST_MIMO_MODE 	(MMW_MIMO_1T3R) 
#define CONFIG_MMW_RX_ANT_IO_MASK 	0x7
#endif

#define RX_ANT_VAILD(rx_id) (CONFIG_MMW_RX_ANT_IO_MASK & BIT(rx_id))
#define RBIN_ANT_PWR(PTR_BUF, RBIN_NUM, RX_ANT, BIN) (PTR_BUF[(RBIN_NUM) * (RX_ANT) + (BIN)])

/*mmw radar param config */
#define CONFIG_NOISE_CHIRP_NUM					(128)

#define RBIN_SCOPE_OF_START_ID	(0)
#define RBIN_SCOPE_OF_END_ID	(1)


static struct mmw_noise_gap_param_t massess_gap = {
	.peak_pwr 	= 400.f,		// unit 0.01db
	.rbin_gap_num 	= 2,	// rbin gap num
	.snr 		= 600.f,		// unit 0.01db
	.min 		= 500.f,		// unit 0.01db
	.avge 		= 500.f,		// unit 0.01db
	.max 		= 700.f		// unit 0.01db
};

static struct mmw_noise_est_param {
	uint32_t hw_init_id;
	uint16_t fft_len;
	uint16_t rbin_scop[2];
	uint16_t rbin_scop_snr[2];
} g_rbin_noise_est_param = {
	.hw_init_id = 0,
	.fft_len = 128,
	.rbin_scop = {
		[0] = 0,
		[1] = 0
	},
	.rbin_scop_snr = {
		[0] = 0,
		[1] = 0
	}
};

struct radar_noise_est_t mradar_est_data;
struct mmw_noise_report_t report = {0};
struct mmw_noise_est_final_report_t noise_est_report = {0};
extern struct mmw_noise_est_frame_t mradar_frame;

struct mmw_noise_est_task_status_t {
	bool running;
	int16_t curr_frame_index;
	OSI_Semaphore_t mmw_wave_sem;
} ft_noise_est_task_status_obj;

extern uint8_t est_rx_msk;

extern int16_t fast_db_trans_f32(float lin_data);

/** @brief set noise estimation param */
void ft_noise_est_param_set(struct mmw_nosie_est_recive_param_t *param)
{
	/* check validity of param */ 
	if (param == NULL)  {
		return ;
	}
	if (param->est_rx_msk == 0 || param->est_rx_msk > CONFIG_MMW_RX_ANT_IO_MASK) {
		return ;
	}
	est_rx_msk = param->est_rx_msk;
	/* Set typical value for each enabled ants */
	for (uint8_t rx_ant_id = 0; rx_ant_id < CONFIG_MAX_RX_ANT; rx_ant_id++) {
		mmw_noise_factor_set(rx_ant_id, &param->factor[rx_ant_id], &mradar_est_data);
	}
	
	mmw_noise_est_range_set(NOISE_FT_DEFAULT_RANGE_MIN_MM, NOISE_FT_DEFAULT_RANGE_MAX_MM, &mradar_est_data.usr_param);			/* min 0.9m, max 1.4m */
	mmw_noise_hw_init_set(param->hw_init_id);
	mmw_noise_meas_scope_set(param->est_rbin_start_id, param->est_rbin_end_id);
	
	mmw_noise_assess_gap_param_config(NOISE_PEAK_PWR_GAP_ID, param->gap.peak_pwr);
	mmw_noise_assess_gap_param_config(NOISE_SNR_GAP_ID, param->gap.snr);
	mmw_noise_assess_gap_param_config(NOISE_MIN_GAP_ID, param->gap.min);
	mmw_noise_assess_gap_param_config(NOISE_AVGE_GAP_ID, param->gap.avge);
	mmw_noise_assess_gap_param_config(NOISE_MAX_GAP_ID, param->gap.max);
	
	/* config noise estimation param  */
	/* 
		If user don't config chirp & frame params, use default param:
		59GHz, 128fft, 8cm, +-10m/s, 16 dfft
	*/
	if (param->range_mm == 0) {
		mradar_frame.range_mm = FT_DEFAULT_RANGE;
	} else {
		mradar_frame.range_mm = param->range_mm;
	}

	if (param->range_resolut == 0) {
		mradar_frame.resol_mm = FT_DEFAULT_RANGE_RES;
	} else {
		mradar_frame.resol_mm = param->range_resolut;
	}

	if (param->start_freq == 0) {
		mradar_frame.startFreq_MHz = FT_DEFAULT_START_FREQ;
	} else {
		mradar_frame.startFreq_MHz = param->start_freq;
	}

	if (param->veloc_mm == 0) {
		mradar_frame.velocity_mm = NOISE_FT_DEFAULT_VELOC;
	} else {
		mradar_frame.velocity_mm = param->veloc_mm;
	}

	if (param->vel_resol == 0) {
		mradar_frame.veloc_resol = NOISE_FT_DEFAULT_VEL_RES;
	} else {
		mradar_frame.veloc_resol = param->vel_resol;
	}

	if (param->frame_num == 0) {
		mradar_frame.frame_num = NOISE_FT_DEFAULT_FRAME_NUM + CONFIG_MMW_VENDOR_FILTER_FRAME_NUM;
	} else {
		mradar_frame.frame_num = param->frame_num + CONFIG_MMW_VENDOR_FILTER_FRAME_NUM;
	}
}


/** @brief set noise est result to struct "est_report" */
void mmw_noise_est_report_data(int event, struct mmw_noise_report_t *report, struct mmw_noise_est_final_report_t *est_report){

	est_report->rx_msk = est_rx_msk;
	switch (event) {
		case MMW_NOISE_RX_ANT_REPORT_EVT:{
			uint8_t rx_ant_idx = report->rx_id;
			est_report->factor[rx_ant_idx].peak_pwr_avge = report->peak_pwr_db;
			est_report->factor[rx_ant_idx].pwr_high_avge = report->noise_high_db;
			est_report->factor[rx_ant_idx].pwr_low_avge = report->noise_low_db;
			est_report->factor[rx_ant_idx].pwr_avge = report->noise_avge_db;
			est_report->factor[rx_ant_idx].peak_snr_avge = report->snr_db;
			est_report->peak_distance = report->peak_distance;
			break;
		}
		case MMW_NOISE_FINAL_REPORT_EVT:{
			est_report->noise_db = report->noise_avge_db;
			est_report->fail_msk = report->test_fail_mask;
			est_report->invalid_msk = report->test_invalid_mask;
			break;
		}
		default:
			break;
	}
}

/** @brief set radar perfermance typical value */
int mmw_noise_factor_set(uint16_t mimo_rx_ant_id, struct mmw_noise_factor_t *factor, struct radar_noise_est_t *mradar_est_data)
{
	if (mimo_rx_ant_id >= CONFIG_MAX_RX_ANT || factor == NULL ) {
		return -1;
	}
	mradar_est_data->factor_param[mimo_rx_ant_id].pwr_high_avge = factor->pwr_high_avge;
	mradar_est_data->factor_param[mimo_rx_ant_id].pwr_low_avge = factor->pwr_low_avge;
	mradar_est_data->factor_param[mimo_rx_ant_id].pwr_avge = factor->pwr_avge;
	mradar_est_data->factor_param[mimo_rx_ant_id].peak_pwr_avge = factor->peak_pwr_avge;
	mradar_est_data->factor_param[mimo_rx_ant_id].peak_snr_avge = factor->peak_snr_avge;
	return 0;
}

/** @brief set noise est range  */
int mmw_noise_est_range_set(uint16_t range_min, uint16_t range_max, struct mmw_noise_usr_t *usr_param)
{
	usr_param->range_min_mm 	= range_min;
	usr_param->range_max_mm 	= range_max;
	MMW_LOG_DBG("noise config range %dmm - %dmm\n", range_min, range_max);
	return 0;
}

void mmw_noise_hw_init_set(uint32_t id)
{
	g_rbin_noise_est_param.hw_init_id = id;
}

/** @brief set scope of noise est range index */
int mmw_noise_meas_scope_set(uint16_t start_index, uint16_t end_index)
{
	if ((start_index > end_index) || (end_index >= g_rbin_noise_est_param.fft_len)) {
	}
	g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID] 	= start_index;
	g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID] 	= end_index;
	return 0;
}

/** @brief config gap value of radar perfermance value  */
int mmw_noise_assess_gap_param_config(uint8_t gap_id, int16_t gap_val)
{
	int ret = 0;
	struct mmw_noise_gap_param_t	*assess = &massess_gap;

	if (gap_val < 0) {
		return -1;
	}

	switch (gap_id) {
	case NOISE_PEAK_PWR_GAP_ID:
		assess->peak_pwr = gap_val;
		break;
	case NOISE_SNR_GAP_ID:
		assess->snr = gap_val;
		break;
	case NOISE_MIN_GAP_ID:
		assess->min = gap_val;
		break;
	case NOISE_AVGE_GAP_ID:
		assess->avge = gap_val;
		break;
	case NOISE_MAX_GAP_ID:
		assess->max = gap_val;
		break;
	default:
		ret = -1;
		break;
	}
	return ret;
}

uint16_t round_2power(uint16_t fft_len)
{
	uint16_t power = 1;
	while (fft_len > power) {
		power <<= 1;
	}
	return power;
}

/** @brief init param before noise eatimation process
 * 1. init noise estimation param 
 * 2. memlloc buffer use for storage data which in noise est processing
 * */
int ft_noise_est_init(struct mmw_noise_est_frame_t *pFrame)
{
	int ret = 0;
	float *noise_est_buf = 0;

	if (ft_noise_est_task_status_obj.running == true) {
		ret = -1;
		return ret;
	}
	
	/* init g_rbin_noise_est_param struct */
	uint16_t fft_len = (pFrame->range_mm + pFrame->resol_mm - 1) / pFrame->resol_mm;
	fft_len = round_2power(fft_len);
	g_rbin_noise_est_param.fft_len = fft_len;
	
	/* if user not set scope of range index, use default param as below:
	 * 		start range index: 	fft_len*3/10
	 * 		end range index:	(fft_len*9 + 9)/10 - 1
	 * else 
	 * 		check the scope is valid.
	*/
	if (g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID] == 0
		&& g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID] == 0) {
		g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID] = fft_len*3/10;
		g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID] = (fft_len*9 + 9)/10 - 1;
	} else {
		if (g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID] >= fft_len) {
			g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID] = fft_len -1;
		}
		if (g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID] >= fft_len) {
			g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID] = fft_len -1;
		}
	}

	/* set scope of range index use for calculate snr */
	if (g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_START_ID] == 0
		&& g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_END_ID] == 0) {
		g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_START_ID] = fft_len*7/10;
		g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_END_ID] = (fft_len*9 + 9)/10 - 1;
	} else {
		if (g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_START_ID] >= fft_len) {
			g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_START_ID] = fft_len -1;
		}
		if (g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_END_ID] >= fft_len) {
			g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_END_ID] = fft_len -1;
		}
	}

	MMW_LOG_DBG("start %d end %d frame period %d\n", g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID],
									g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID], pFrame->frame_period_ms);

	mmw_process_mem_alloc((void**)&noise_est_buf, CONFIG_MAX_RX_ANT * g_rbin_noise_est_param.fft_len * sizeof(float));
	if (noise_est_buf == NULL) {
		ret = -1;
		ft_noise_est_deinit(&mradar_est_data);
		return ret;
	}

	for (uint8_t rx_idx = 0; rx_idx < CONFIG_MAX_RX_ANT; rx_idx++) {
		mradar_est_data.noise_param.rbin_max_pwr_idx[rx_idx] = 0;
		mradar_est_data.noise_param.rbin_max_pwr[rx_idx] = 0;
	}
	
	mradar_est_data.noise_param.frame_num = pFrame->frame_num;
	ft_noise_est_task_status_obj.curr_frame_index = 0;
	ft_noise_est_task_status_obj.running = true;	
	
	memset(noise_est_buf, 0, CONFIG_MAX_RX_ANT * g_rbin_noise_est_param.fft_len * sizeof(float));
	mradar_est_data.noise_est_buf = noise_est_buf;
	
	memset(&report, 0, sizeof(report));
	memset(&noise_est_report, 0, sizeof(noise_est_report));
	return ret;
}


void ft_noise_est_deinit(struct radar_noise_est_t *mradar_est_data) {

	if (mradar_est_data->noise_est_buf) {
		mmw_process_mem_free((void**)&mradar_est_data->noise_est_buf);
		mradar_est_data->noise_est_buf = NULL;
	}

	ft_noise_est_task_status_obj.running = false;
	ft_noise_est_task_status_obj.curr_frame_index = 0;
}

/** @brief calculate noise and set result in struct mmw_noise_report_t  */
void mmw_noise_process_finish(struct radar_noise_est_t *mradar_est_data, struct mmw_noise_report_t *report){
	
	uint32_t frame_num = ft_noise_est_task_status_obj.curr_frame_index - CONFIG_MMW_VENDOR_FILTER_FRAME_NUM;
	float *rbin_max_pwr = &mradar_est_data->noise_param.rbin_max_pwr[0];
	float *rbin_power; 
	float noise_db = 0.f, all_noise_db = 0.f;
	bool pass = false;
	uint8_t tx_num = mradar_est_data->noise_param.tx_num;
	uint8_t rx_num = mradar_est_data->noise_param.rx_num;
	
	mmw_ctrl_stop();
	ft_noise_est_task_status_obj.running = false;
	ft_noise_est_task_status_obj.curr_frame_index = 0;
	
	if (mradar_est_data->noise_est_buf == NULL) {
		printf("mradar_est_data->noise_est_buf == NULL\n");
		return;
	}

	if (rx_num > CONFIG_MAX_RX_ANT ||
		mradar_est_data->noise_param.range_len > g_rbin_noise_est_param.fft_len) {
		return;
	}
	
	rbin_power = mradar_est_data->noise_est_buf;

	for (int rx_ant_idx = 0; rx_ant_idx < tx_num * rx_num; rx_ant_idx++) {
		for (int range_idx = 0; range_idx < mradar_est_data->noise_param.range_len; range_idx++) {
			rbin_power[range_idx] = rbin_power[range_idx] / ((mradar_est_data->noise_param.doppler_len >> 1) * frame_num);
		}
		rbin_power += g_rbin_noise_est_param.fft_len;
		rbin_max_pwr[rx_ant_idx] = rbin_max_pwr[rx_ant_idx] / frame_num;
		
		pass = mmw_radio_noise_est_pass(rx_ant_idx, &noise_db, mradar_est_data, report);
		if (pass == true) {
			MMW_LOG_DBG("mmw noise est on rx ant %d test pass\n", rx_ant_idx);
		} else {
			report->test_fail_mask |= BIT(rx_ant_idx);
			MMW_LOG_DBG("mmw noise est on rx ant %d test fail %x\n", rx_ant_idx, report->test_fail_mask);
		}
		all_noise_db += noise_db;
	}
	report->noise_avge_db = all_noise_db / (tx_num * rx_num);
	mmw_noise_est_report_data(MMW_NOISE_FINAL_REPORT_EVT, report, &noise_est_report);
	
	int status = OSI_SemaphoreRelease(&ft_noise_est_task_status_obj.mmw_wave_sem);
	if (status == OSI_STATUS_OK) {
		MMW_LOG_DBG("mmw wave task released\n");
	}
	
}

static int noise_est_frame_check(struct radar_noise_est_t *mradar_est_data)
{
	uint8_t txrx, work;
	uint16_t range_len, doppler_len;
	uint32_t range_mm, resol_mm;

	mmw_fft_num_get(&range_len, &doppler_len);
	if (range_len != g_rbin_noise_est_param.fft_len){
		printf("Err: mmw_fft_num_get for range_len %d doppler_len %d\n", range_len, doppler_len);
		return -1;
	}

	mmw_mode_get (&txrx, &work);
	if ((txrx != CONFIG_NOISE_EST_MIMO_MODE) || (work != MMW_WORK_MODE_2DFFT)) {
		printf("Err: mmw_mode_get for txrx %d work %d\n", txrx, work);
		return -1;
	}

	if (CONFIG_NOISE_EST_MIMO_MODE == MMW_MIMO_2T4R) {
			mradar_est_data->noise_param.tx_num = 2;
			mradar_est_data->noise_param.rx_num = 4;
	} else if (CONFIG_NOISE_EST_MIMO_MODE == MMW_MIMO_1T3R) {
			mradar_est_data->noise_param.tx_num = 1;
			mradar_est_data->noise_param.rx_num = 3;
	} else {
			return -1;
	}

	mmw_range_get (&range_mm, &resol_mm);
	if (resol_mm) {
		mradar_est_data->noise_param.range_index_min = mradar_est_data->usr_param.range_min_mm / resol_mm;
		mradar_est_data->noise_param.range_index_max = mradar_est_data->usr_param.range_max_mm / resol_mm;
		mradar_est_data->noise_param.range_index_max = MAX(mradar_est_data->noise_param.range_index_max, (CONFIG_MIN_SCOPE_RANGE_MM/resol_mm));
		mradar_est_data->noise_param.range_index_dc = MAX(1, CONFIG_FILTER_DC_RANGE_MM / resol_mm);
	} else {
		mradar_est_data->noise_param.range_index_min = 0;
		mradar_est_data->noise_param.range_index_max = 0;
		mradar_est_data->noise_param.range_index_dc = 1;
	}

	if (range_len) {
		mradar_est_data->noise_param.resol_mm = range_mm / range_len;
	} else {
		mradar_est_data->noise_param.resol_mm = resol_mm;
	}
	mradar_est_data->noise_param.range_len = range_len;
	mradar_est_data->noise_param.doppler_len = doppler_len;
	return 0;
}

void fft_autogain_restore_f32(Complex16_RealImag *buffer, Complexf32_RealImag *buffer_f32, uint8_t tx_idx, uint16_t range_idx, uint16_t buff_size) 
{
	
	uint8_t q_tar;  // q_tar is the total q value that shoud be recovered.
	int8_t shift_value;
	Complex32_RealImag *buffer_32 = 0;
	mmw_process_mem_alloc((void **)&buffer_32, buff_size * sizeof(Complex32_RealImag));
	
	/* get auto gain factor */
	q_tar = mmw_psic_auto_gain_base_get(tx_idx) + mmw_psic_auto_gain_rbin_get(range_idx, tx_idx);
	
	/* convert buffer to Q31 */
	shift_value = 16 - q_tar;  // shift_value is the value that will be left-shifted to get q31 data.
	for (uint16_t idx = 0; idx < buff_size; idx++) {
		buffer_32[idx].real = (int32_t)buffer[idx].real << shift_value;
		buffer_32[idx].imag = (int32_t)buffer[idx].imag << shift_value;
		
		/* Q31 to float */
		buffer_f32[idx].real = (float)buffer_32[idx].real / 2147483648;
		buffer_f32[idx].imag = (float)buffer_32[idx].imag / 2147483648;
		
	}

	mmw_process_mem_free((void **)&buffer_32);
}

/** @brief mmw wave callback
 * main noise estmation process function.
 * 1、check mmw frame is valid.
 * 2、get all dop data for each rbin .
 * 3、restore auto gain.
 * 4、calculate the sum of all low speed dop bin power.
 * 5、storage the sum value in mradar_est_data.noise_est_buf.
 * 6、get 0 dop data, then find max power and max power index in 0 dop for each ant.
 * */
static int mmw_ctrl_frame_cb(void *mmw_data, void *arg) {
	
	MMW_LOG_DBG("Radar callback process noise data, frame: %d\n", ft_noise_est_task_status_obj.curr_frame_index);
	int ret = 0;
	float power = 0.f;
	struct radar_noise_est_t *pdata = (struct radar_noise_est_t *)(&mradar_est_data);

	uint8_t ant_mimo_id = 0;
	uint16_t dop_len = pdata->noise_param.doppler_len;
	uint16_t range_len = g_rbin_noise_est_param.fft_len;
	
	Complex16_RealImag *noise_dop_buf = 0;
	Complexf32_RealImag *noise_dop_buffer_f32 = 0;
	Complex16_RealImag *noise_range_buf = 0;
	Complexf32_RealImag *noise_range_buffer_f32 = 0;

	uint16_t rbin_num = pdata->noise_param.range_index_max + 1;
	uint16_t rbin_off = pdata->noise_param.range_index_dc;
	ft_noise_est_task_status_obj.curr_frame_index++;
	
	if (pdata->noise_est_buf == NULL) {
		return -1;
	}
	
	ret = noise_est_frame_check(pdata);
	if (ret < 0) {
		printf("err: ft_adc_get_mmw_config error\n");
		return -1;
	}

	if (ft_noise_est_task_status_obj.curr_frame_index <= CONFIG_MMW_VENDOR_FILTER_FRAME_NUM) {
		return 0;
	}
	
	mmw_process_mem_alloc((void **)&noise_dop_buf, dop_len * sizeof(Complex16_RealImag));
	mmw_process_mem_alloc((void **)&noise_dop_buffer_f32, dop_len * sizeof(Complexf32_RealImag));
	mmw_process_mem_alloc((void **)&noise_range_buf, rbin_num * sizeof(Complex16_RealImag));
	mmw_process_mem_alloc((void **)&noise_range_buffer_f32, rbin_num * sizeof(Complexf32_RealImag));
	if (noise_dop_buf == NULL || noise_range_buf == NULL || noise_range_buffer_f32 == NULL) {
		return -1;
	}
	mmw_motion_cube_access_open();
	MMW_LOG_DBG("ongoing collecting noise frame count %d rxnum %d rangelen %d doplen %d rbin_num %d\n", ft_noise_est_task_status_obj.curr_frame_index,
		pdata->noise_param.rx_num, pdata->noise_param.range_len, pdata->noise_param.doppler_len, rbin_num);
	
	if (rbin_num >= range_len) {
		rbin_num = range_len;
	}

	for (uint8_t tx_idx = 0; tx_idx < pdata->noise_param.tx_num; tx_idx++) {
		for (uint8_t rx_idx = 0; rx_idx < pdata->noise_param.rx_num; rx_idx++) {
			if (!RX_ANT_VAILD(rx_idx)) {
				continue;
			}
		/* 1、get all dop data for each rbin 
		 * 2、restore auto gain
		 * 3、calculate sum of all low speed dop bin power
		 * 4、storage the sum in mradar_est_data.noise_est_buf
		 * */
			for (uint16_t range_idx = 0; range_idx < pdata->noise_param.range_len; range_idx++) {
				mmw_fft_doppler(noise_dop_buf, dop_len, tx_idx, rx_idx, range_idx);
				fft_autogain_restore_f32(noise_dop_buf, noise_dop_buffer_f32, 0, range_idx, dop_len);
				power = noise_est_buf_update(noise_dop_buffer_f32, dop_len);
				pdata->noise_est_buf[(ant_mimo_id * range_len) + range_idx] += power;
			}
		
			/* get 0 dop data */
			mmw_process_fft_range_f32(noise_range_buffer_f32, noise_range_buf, rbin_num, tx_idx, rx_idx, dop_len >> 1);
		
			/* find max power and max power index in 0 dop for each ant.
			 * if max power index is different between each frame, print a waring message */
			noise_rbin_buf_update(noise_range_buffer_f32, ant_mimo_id, rbin_num, rbin_off, pdata);
			ant_mimo_id++;
		}
	}
	mmw_psic_auto_gain_clear();	
	mmw_motion_cube_access_close();
	
	/* process finish */
	if(ft_noise_est_task_status_obj.curr_frame_index >= mradar_frame.frame_num){
		mmw_noise_process_finish(pdata, &report);
		mmw_process_mem_free((void **)&noise_dop_buf);
		mmw_process_mem_free((void **)&noise_dop_buffer_f32);
		mmw_process_mem_free((void **)&noise_range_buf);
		mmw_process_mem_free((void **)&noise_range_buffer_f32);
		return -1;
	}
	
	mmw_process_mem_free((void **)&noise_dop_buf);
	mmw_process_mem_free((void **)&noise_dop_buffer_f32);
	mmw_process_mem_free((void **)&noise_range_buf);
	mmw_process_mem_free((void **)&noise_range_buffer_f32);
	return 0;
}

float noise_est_buf_update(Complexf32_RealImag *pBuf_in, uint16_t dop_num)
{
	float power = 0.0f;
	uint16_t filter_start_index = dop_num >> 2; // 16/4 = 4
	uint16_t filter_end_index = filter_start_index + (dop_num >> 1) - 1; // 4 + 16/2 = 12

	for (int dp = 0; dp < dop_num; dp++) {
		if ((dp < filter_start_index || dp > filter_end_index) || dop_num == 1) {
			power += pBuf_in[dp].real * pBuf_in[dp].real + pBuf_in[dp].imag * pBuf_in[dp].imag;
		}
	}
	return power;
}

static void noise_rbin_buf_update(Complexf32_RealImag *pBuf, uint8_t rx_idx, uint16_t rbin_num, uint16_t rbin_off, struct radar_noise_est_t *pdata)
{
	struct mmw_meas_pos_t pos;

	mmw_dsp_poweron();
	mmw_radar_find_rbin_postion(pBuf, rbin_off, rbin_num, 1, &pos);

	if (pdata->noise_param.rbin_max_pwr_idx[rx_idx] == 0) {
		pdata->noise_param.rbin_max_pwr_idx[rx_idx] = pos.rbin_max_pwr_idx;
	}
	
	if (pdata->noise_param.rbin_max_pwr_idx[rx_idx] != pos.rbin_max_pwr_idx) {
		if ((pdata->noise_param.rbin_max_pwr_idx[rx_idx] > (pos.rbin_max_pwr_idx + 1))
			|| ((pdata->noise_param.rbin_max_pwr_idx[rx_idx] + 1) < pos.rbin_max_pwr_idx)) {
			report.test_invalid_mask |= BIT(PEAK_INVAILD);
		}
		printf("error rx idx %d pos index %d power %f\n", rx_idx, pos.rbin_max_pwr_idx, pos.rbin_max_pwr);
		pos.rbin_max_pwr_idx = pdata->noise_param.rbin_max_pwr_idx[rx_idx];
		pos.rbin_max_pwr = pBuf[pos.rbin_max_pwr_idx].real * pBuf[pos.rbin_max_pwr_idx].real 
							+ pBuf[pos.rbin_max_pwr_idx].imag * pBuf[pos.rbin_max_pwr_idx].imag;
	}
	pdata->noise_param.rbin_max_pwr[rx_idx] += pos.rbin_max_pwr;

}

int mmw_radar_find_rbin_postion(Complexf32_RealImag *cube, uint16_t start_offset, uint16_t rbin_num, uint8_t rx_ant_num, struct mmw_meas_pos_t *pos)
{
	Complexf32_RealImag *rbin;
	float power = 0.f, max_power = 0.f;
	uint16_t rbinoff, max_pwr_rbin_idx = 0;

	/* find out the max power from all rx ant cube rbin */
	for (uint8_t rx_idx = 0; rx_idx < rx_ant_num; rx_idx++) {
		rbinoff = rx_idx * rbin_num;
		for (uint16_t range_idx = start_offset; range_idx < rbin_num; range_idx++) {
			rbin = &cube[rbinoff + range_idx];
			power = rbin->real * (rbin->real) + (rbin->imag) * (rbin->imag);
			if (max_power < power) {
				max_power = power;
				max_pwr_rbin_idx = range_idx;
			}
		}
	}

	pos->rbin_max_pwr_idx = max_pwr_rbin_idx;
	pos->rbin_max_pwr = max_power;
	return 0;
}


/** @brief caculate noise min/max/avge value and snr.
 * compare the diff of result and typical value with gap value
 * */
static bool mmw_radio_noise_est_pass(uint16_t rx_ant_id, float *avge_noise, struct radar_noise_est_t *mradar_est_data, struct mmw_noise_report_t *report)
{
	struct mmw_noise_factor_t *pfactor;

	/* initial nosise est param */
	bool noise_est_pass = true;
	float noise_avge_snr_db = 0.f;
	float noise_avge_snr_pwr = 0;
	float noise_avge_pwr = 0;
	float noise_max_pwr = 0;
	float noise_min_pwr = INFINITY;

	/* temp variables */
	float noise_power = 0.f;

	if (mradar_est_data->noise_est_buf == NULL || rx_ant_id >= CONFIG_MAX_RX_ANT) {
		return false;
	}

	pfactor = &mradar_est_data->factor_param[rx_ant_id];

	for (uint16_t rbin_idx = g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_START_ID]; rbin_idx <= g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_END_ID]; rbin_idx++) {
		noise_power += RBIN_ANT_PWR(mradar_est_data->noise_est_buf, g_rbin_noise_est_param.fft_len, rx_ant_id, rbin_idx);
	}
	noise_power = noise_power / (g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_END_ID] - g_rbin_noise_est_param.rbin_scop_snr[RBIN_SCOPE_OF_START_ID] + 1);
	noise_avge_snr_pwr = noise_power;
	noise_avge_snr_db = (fast_db_trans_f32(noise_avge_snr_pwr)) * 0.0625f;	
	noise_avge_snr_db = noise_avge_snr_db * 100; // uint: 0.01dB

	/* init all */
	noise_power = 0.f;
	float noise_power_tmp = 0.f;

	for (uint16_t rbin_idx = g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID]; rbin_idx <= g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID]; rbin_idx++) {
		noise_power_tmp = RBIN_ANT_PWR(mradar_est_data->noise_est_buf, g_rbin_noise_est_param.fft_len, rx_ant_id, rbin_idx);
		noise_power += noise_power_tmp;
		if (noise_max_pwr < noise_power_tmp) {
			noise_max_pwr = noise_power_tmp;
		}
		if (noise_min_pwr > noise_power_tmp) {
			noise_min_pwr = noise_power_tmp;
		}
	}

	noise_power = noise_power / (g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID] - g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID] + 1);
	noise_avge_pwr = noise_power;
	report->noise_avge_db = (fast_db_trans_f32(noise_avge_pwr)) * 0.0625f;
	report->noise_avge_db = report->noise_avge_db * 100; // uint: 0.01dB

	report->noise_low_db = (fast_db_trans_f32(noise_min_pwr)) * 0.0625f;	
	report->noise_low_db = report->noise_low_db * 100; // uint: 0.01dB

	report->noise_high_db = (fast_db_trans_f32(noise_max_pwr)) * 0.0625f;	
	report->noise_high_db = report->noise_high_db * 100; // uint: 0.01dB

	/*max peak snr verify*/
	report->peak_pwr_db = (fast_db_trans_f32(mradar_est_data->noise_param.rbin_max_pwr[rx_ant_id])) * 0.0625f;	
	report->peak_pwr_db = report->peak_pwr_db * 100; // uint: 0.01dB

	report->snr_db = report->peak_pwr_db - noise_avge_snr_db;

	if (ABS(report->snr_db - pfactor->peak_snr_avge) > massess_gap.snr) {
		noise_est_pass = false;
	}

	/* max peak power verify */
	uint16_t resol_mm = mradar_est_data->noise_param.resol_mm;
	report->peak_distance = mradar_est_data->noise_param.rbin_max_pwr_idx[rx_ant_id] * resol_mm;

	if (report->peak_distance > mradar_est_data->usr_param.range_max_mm
		|| report->peak_distance < mradar_est_data->usr_param.range_min_mm
		|| ABS(pfactor->peak_pwr_avge - report->peak_pwr_db) > massess_gap.peak_pwr) {
		noise_est_pass = false;
	} 
							  
	MMW_LOG_DBG("ant [%d], distance %dmm, high avg: %f, low avg: %f, noise avg:%f, peak pwr avg:%f, peak snr avg: %f\n", 
		rx_ant_id, report->peak_distance, report->noise_high_db, report->noise_low_db, report->noise_avge_db, report->peak_pwr_db, report->snr_db);

	/* noise aver/max/min verify */
	if (ABS(report->noise_avge_db - pfactor->pwr_avge) > massess_gap.avge
		|| ABS(report->noise_low_db - pfactor->pwr_low_avge) > massess_gap.min
		|| ABS(report->noise_high_db - pfactor->pwr_high_avge) > massess_gap.max) {
		noise_est_pass = false;
	} 
	
	*avge_noise = report->noise_avge_db;
	report->rx_id = rx_ant_id;
	report->noise_start_index = g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_START_ID];
	report->noise_end_index = g_rbin_noise_est_param.rbin_scop[RBIN_SCOPE_OF_END_ID];
	if (noise_est_pass == true) {
		report->fail = 0;
	} else {
		report->fail = 1;
	}

	mmw_noise_est_report_data(MMW_NOISE_RX_ANT_REPORT_EVT, report, &noise_est_report);

	return noise_est_pass;
}


int mmw_wave_noise_est_frame_config(struct mmw_noise_est_frame_t *pFrame)
{
	int ret = 0;

	if (pFrame == NULL) {
		return -1;
	}

	/* config for mmw chirp */
	ret = mmw_mode_cfg (pFrame->trx_mimo, pFrame->work_mode);
	if (ret) {
		printf("mmw_mode_cfg error\n");
	}

	ret = mmw_freq_cfg (pFrame->startFreq_MHz, pFrame->endFreq_MHz);
	if (ret) {
		printf("mmw_freq_cfg error\n");
	}

	ret = mmw_range_cfg(pFrame->range_mm, pFrame->resol_mm);
	if (ret) {
		printf("mmw_range_cfg error\n");
	}

	ret = mmw_velocity_cfg(pFrame->velocity_mm, pFrame->veloc_resol);
	if (ret) {
		printf("mmw_velocity_cfg error\n");
	}

	ret = mmw_interval_cfg(pFrame->intvl_period_us, pFrame->intvl_num);
	if (ret) {
		printf("mmw_interval_cfg error: %d, us %d num %d\n", ret, pFrame->intvl_period_us, pFrame->intvl_num);
	}

	ret = mmw_frame_cfg(pFrame->frame_period_ms,	pFrame->frame_num);
	if (ret) {
		printf("mmw_frame_cfg error: %d\n", ret);
	}

	ret = mmw_clutter_remove(pFrame->clutter_mode);
	if (ret) {
		printf("mmw_clutter_remove error\n");
	}
	
	MMW_LOG_DBG("mimo: %d, work_mode: %d, start_freq: %d, end_freq: %d, range_mm: %d, resol_mm: %d, vel_mm: %d, vel_res: %d, intvl_perid: %d, intvl_num: %d, frame_period: %d, frame_num: %d, clutter_mode: %d\r\n",
						pFrame->trx_mimo, pFrame->work_mode, pFrame->startFreq_MHz, pFrame->endFreq_MHz, pFrame->range_mm, pFrame->resol_mm, pFrame->velocity_mm, pFrame->veloc_resol,
						pFrame->intvl_period_us, pFrame->intvl_num, pFrame->frame_period_ms, pFrame->frame_num, pFrame->clutter_mode);
	
	/* Enable autogain */
	mmw_fft_autogain_set(1);
	return ret;
}

/** @brief noise estmation start
 *  1.config mmw wave
 *  2.init buffer struct
 *  3.config mmw callback
 *  4.creat semaphore
 *  5.mmw wave start
 *  @return return 0 on sucess
 * */
int mmw_noise_est_start(struct mmw_noise_est_frame_t *pFrame, struct radar_noise_est_t *mradar_est_data)
{
	int ret = 0;
	uint8_t int_type;	
	
	/* config mmw wave */
	mmw_wave_noise_est_frame_config(pFrame);
	/* init buffer struct */
	ret = ft_noise_est_init(pFrame);
	if (ret < 0) {
		return -1;
	}
	
	/* config mmw callback */
	if (pFrame->work_mode == MMW_WORK_MODE_2DFFT) {
		int_type = MMW_DATA_TYPE_2DFFT;
	} else {
		int_type = MMW_DATA_TYPE_1DFFT;
	}
	ret = mmw_ctrl_callback_cfg(mmw_ctrl_frame_cb, int_type, NULL);
	if (ret) {
		mmw_ctrl_callback_cfg(NULL, MMW_DATA_TYPE_DISABLE, NULL);
	}
	
	int status = OSI_SemaphoreCreate(&ft_noise_est_task_status_obj.mmw_wave_sem, 0, 1);
	if (status != OSI_STATUS_OK) {
		printf("mmw wave Semaphore create error\n");
		return -1;
	}
	/* mmw wave start */
	ret = mmw_ctrl_start();
	if (ret) {
		return -1;
	}
	return ret;
}


/** @brief set semaphore wait time, when timeout, return err 
 *  @return return 0 on success 
 * */
int mmw_noise_est_wait_done(struct radar_noise_est_t *mradar_est_data) {
	
	int err = 0;
	if (ft_noise_est_task_status_obj.running == false) {
		return 0;
	}
	uint32_t wait_time_s =  ((NOISE_FT_DEFAULT_FRAME_NUM + 1) * NOISE_FT_DEFAULT_FRAME_PERIOD) + 10;
	// LOG_IO(0x8000, 5, 1, 0);
	err = OSI_SemaphoreWait(&ft_noise_est_task_status_obj.mmw_wave_sem, wait_time_s);
	// LOG_IO(0x8000, 5, 0, 0);
	if (err != OSI_STATUS_OK) {
		mmw_ctrl_stop();
		ft_noise_est_deinit(mradar_est_data);
		printf("mmw task wait semaphore timout errcode %d\n", err);
	}
	return err;
}
