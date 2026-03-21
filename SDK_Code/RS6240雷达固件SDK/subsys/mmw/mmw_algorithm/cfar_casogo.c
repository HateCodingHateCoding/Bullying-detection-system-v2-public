/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


#include "mmw_ctrl.h"
#include "cfar_casogo.h"

#define CFAR_DEBUG               0

#if (CFAR_DEBUG)
CFAR_SUM_TYPE dbg_min_noise;
CFAR_SUM_TYPE dbg_max_abs;
CFAR_SUM_TYPE dbg_max_idx;
CFAR_SUM_TYPE dbg_peak_num;
void show_cfar_debug_info(void)
{
	printk("cfar peak%d max_abs[%d]=%d, min_noise %d\n",
        dbg_peak_num, dbg_max_idx, dbg_max_abs, dbg_min_noise);
    dbg_min_noise = 0;
    dbg_max_abs   = 0;
    dbg_max_idx   = 0;
    dbg_peak_num  = 0;
}
#endif

uint32_t cfar_ca(CFAR_DATA_TYPE *data_in, uint32_t data_num, uint16_t *out,
            uint32_t snr_q2, uint32_t noiseLen_log2, uint32_t guard_len, CFAR_SUM_TYPE pass_line)
{
    uint32_t idx, idx_left_next, idx_left_prev, idx_right_next,
             idx_right_prev, idx_out, idx_in;
    CFAR_SUM_TYPE thresh, sum_left, sum_right;
    uint32_t noiseLen = (1<<noiseLen_log2);

    if (data_num <= guard_len + noiseLen) {
        return 0;
    }

    /* initializations */
    idx_out   = 0;
    sum_left  = 0;
    sum_right = 0;

    for (idx = 0; idx < noiseLen; idx++) {
        sum_right += CFAR_POWER(data_in[idx + guard_len + 1U]);
    }

#if (CFAR_DEBUG)
    dbg_min_noise = sum_right;
    dbg_max_abs   = CFAR_POWER(data_in[0]);
    dbg_max_idx   = 0;
#endif

    /* One-sided comparision for the first segment */
    idx_in = 0;
    idx_left_next = 0;
    idx_right_prev = idx_in + guard_len + 1;
    idx_right_next = idx_right_prev + noiseLen;

    for (idx = 0; idx < noiseLen + guard_len; idx++) {
        thresh = CFAR_MIN_NOISE_LIMIT(sum_right>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh ||
            CFAR_POWER(data_in[idx_in]) > pass_line) {
            //thr
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_right = (sum_right + CFAR_POWER(data_in[idx_right_next])) - CFAR_POWER(data_in[idx_right_prev]);
        idx_right_next++;
        idx_right_prev++;

    #if (CFAR_DEBUG)
        if (sum_right < dbg_min_noise) {
            dbg_min_noise = sum_right;
        }
    #endif

        if (idx < noiseLen) {
            sum_left += CFAR_POWER(data_in[idx_left_next]);
            idx_left_next++;
        }
    }

    /* Two-sided comparision for the middle segment */
    idx_left_prev = 0;
    /*CFAR_CA*/
    for (idx = 0; idx < (data_num - 2U*(noiseLen + guard_len) - 1U); idx++) {
        thresh = (sum_left + sum_right)>>1;
        thresh = CFAR_MIN_NOISE_LIMIT(thresh>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh ||
            CFAR_POWER(data_in[idx_in]) > pass_line) {
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_left += CFAR_POWER(data_in[idx_left_next]) - CFAR_POWER(data_in[idx_left_prev]);
        sum_right += CFAR_POWER(data_in[idx_right_next]) - CFAR_POWER(data_in[idx_right_prev]);
        idx_left_next++;
        idx_left_prev++;
        idx_right_next++;
        idx_right_prev++;

#if (CFAR_DEBUG)
        if (sum_right < dbg_min_noise) {
            dbg_min_noise = sum_right;
        }
#endif
    }
    /*CFAR_CA*/

    /* One-sided comparision for the last segment */
    for (idx = 0; idx < (noiseLen + guard_len); idx++) {
        thresh = CFAR_MIN_NOISE_LIMIT(sum_left>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh) {
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_left += CFAR_POWER(data_in[idx_left_next]) - CFAR_POWER(data_in[idx_left_prev]);
        idx_left_next++;
        idx_left_prev++;
    }
#if (CFAR_DEBUG)
    dbg_peak_num = idx_out;
#endif
    return (idx_out);
}

uint32_t cfar_go(CFAR_DATA_TYPE *data_in, uint32_t data_num, uint16_t *out,
            uint32_t snr_q2, uint32_t noiseLen_log2, uint32_t guard_len, CFAR_SUM_TYPE pass_line)
{
    uint32_t idx, idx_left_next, idx_left_prev, idx_right_next,
             idx_right_prev, idx_out, idx_in;
    CFAR_SUM_TYPE thresh, sum_left, sum_right;
    uint32_t noiseLen = (1<<noiseLen_log2);

    if (data_num <= guard_len + noiseLen) {
        return 0;
    }

    /* initializations */
    idx_out   = 0;
    sum_left  = 0;
    sum_right = 0;

    for (idx = 0; idx < noiseLen; idx++) {
        sum_right += CFAR_POWER(data_in[idx + guard_len + 1U]);
    }

#if (CFAR_DEBUG)
    dbg_min_noise = sum_right;
    dbg_max_abs   = CFAR_POWER(data_in[0]);
    dbg_max_idx   = 0;
#endif

    /* One-sided comparision for the first segment */
    idx_in = 0;
    idx_left_next = 0;
    idx_right_prev = idx_in + guard_len + 1;
    idx_right_next = idx_right_prev + noiseLen;

    for (idx = 0; idx < noiseLen + guard_len; idx++) {
        thresh = CFAR_MIN_NOISE_LIMIT(sum_right>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh ||
            CFAR_POWER(data_in[idx_in]) > pass_line) {//thr
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_right = (sum_right + CFAR_POWER(data_in[idx_right_next])) - CFAR_POWER(data_in[idx_right_prev]);
        idx_right_next++;
        idx_right_prev++;

#if (CFAR_DEBUG)
        if (sum_right < dbg_min_noise) {
            dbg_min_noise = sum_right;
        }
#endif

        if (idx < noiseLen) {
            sum_left += CFAR_POWER(data_in[idx_left_next]);
            idx_left_next++;
        }
    }

    /* Two-sided comparision for the middle segment */
    idx_left_prev = 0;

    /* CFAR_GO */
    for (idx = 0; idx < (data_num - 2U*(noiseLen + guard_len)); idx++) {
        thresh = MAX(sum_left, sum_right);
        thresh = CFAR_MIN_NOISE_LIMIT(thresh>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh ||
            CFAR_POWER(data_in[idx_in]) > pass_line) {
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }

        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_left += CFAR_POWER(data_in[idx_left_next]) - CFAR_POWER(data_in[idx_left_prev]);
        sum_right += CFAR_POWER(data_in[idx_right_next]) - CFAR_POWER(data_in[idx_right_prev]);

        idx_left_next++;
        idx_left_prev++;
        idx_right_next++;
        idx_right_prev++;

#if (CFAR_DEBUG)
         if (sum_right < dbg_min_noise) {
            dbg_min_noise = sum_right;
         }
#endif
     }
     /* CFAR_GO */

    /* One-sided comparision for the last segment */
    for (idx = 0; idx < (noiseLen + guard_len); idx++) {
        thresh = CFAR_MIN_NOISE_LIMIT(sum_left>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh) {
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_left += CFAR_POWER(data_in[idx_left_next]) - CFAR_POWER(data_in[idx_left_prev]);
        idx_left_next++;
        idx_left_prev++;
    }
#if (CFAR_DEBUG)
    dbg_peak_num = idx_out;
#endif
    return (idx_out);
}

uint32_t cfar_so(CFAR_DATA_TYPE *data_in, uint32_t data_num, uint16_t *out,
                    uint32_t snr_q2, uint32_t noiseLen_log2, uint32_t guard_len, CFAR_SUM_TYPE pass_line)
{
    uint32_t idx, idx_left_next, idx_left_prev, idx_right_next,
             idx_right_prev, idx_out, idx_in;
    CFAR_SUM_TYPE thresh, sum_left, sum_right;
    uint32_t noiseLen = (1<<noiseLen_log2);

    if (data_num <= guard_len + noiseLen) {
        return 0;
    }

    /* initializations */
    idx_out   = 0;
    sum_left  = 0;
    sum_right = 0;

    for (idx = 0; idx < noiseLen; idx++) {
        sum_right += CFAR_POWER(data_in[idx + guard_len + 1U]);
    }

#if (CFAR_DEBUG)
    dbg_min_noise = sum_right;
    dbg_max_abs   = CFAR_POWER(data_in[0]);
    dbg_max_idx   = 0;
#endif

    /* One-sided comparision for the first segment */
    idx_in = 0;
    idx_left_next = 0;
    idx_right_prev = idx_in + guard_len + 1;
    idx_right_next = idx_right_prev + noiseLen;

    for (idx = 0; idx < noiseLen + guard_len; idx++) {
        thresh = CFAR_MIN_NOISE_LIMIT(sum_right>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh ||
            CFAR_POWER(data_in[idx_in]) > pass_line) {//thr
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_right = (sum_right + CFAR_POWER(data_in[idx_right_next])) - CFAR_POWER(data_in[idx_right_prev]);
        idx_right_next++;
        idx_right_prev++;

#if (CFAR_DEBUG)
        if (sum_right < dbg_min_noise) {
            dbg_min_noise = sum_right;
        }
#endif

        if (idx < noiseLen) {
            sum_left += CFAR_POWER(data_in[idx_left_next]);
            idx_left_next++;
        }
    }

    /* Two-sided comparision for the middle segment */
    idx_left_prev = 0;
    /*CFAR_SO*/
    for (idx = 0; idx < (data_num - 2U*(noiseLen + guard_len) - 1U); idx++) {
        thresh = MIN(sum_left, sum_right);
        thresh = CFAR_MIN_NOISE_LIMIT(thresh>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh ||
            CFAR_POWER(data_in[idx_in]) > pass_line) {
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_left += CFAR_POWER(data_in[idx_left_next]) - CFAR_POWER(data_in[idx_left_prev]);
        sum_right += CFAR_POWER(data_in[idx_right_next]) - CFAR_POWER(data_in[idx_right_prev]);
        idx_left_next++;
        idx_left_prev++;
        idx_right_next++;
        idx_right_prev++;

#if (CFAR_DEBUG)
        if (sum_right < dbg_min_noise) {
            dbg_min_noise = sum_right;
        }
#endif
    }
    /*CFAR_SO*/

    /* One-sided comparision for the last segment */
    for (idx = 0; idx < (noiseLen + guard_len); idx++) {
        thresh = CFAR_MIN_NOISE_LIMIT(sum_left>>noiseLen_log2);
        thresh = CFAR_PEAK_THRESH(thresh, snr_q2);

        if (CFAR_POWER(data_in[idx_in]) > thresh) {
            out[idx_out] = (uint16_t)idx_in;
            idx_out++;
        }
        idx_in++;

#if (CFAR_DEBUG)
        if (CFAR_POWER(data_in[idx_in]) > dbg_max_abs) {
            dbg_max_abs = CFAR_POWER(data_in[idx_in]);
            dbg_max_idx = idx_in;
        }
#endif

        sum_left += CFAR_POWER(data_in[idx_left_next]) - CFAR_POWER(data_in[idx_left_prev]);
        idx_left_next++;
        idx_left_prev++;
    }
#if (CFAR_DEBUG)
    dbg_peak_num = idx_out;
#endif
    return (idx_out);
}
