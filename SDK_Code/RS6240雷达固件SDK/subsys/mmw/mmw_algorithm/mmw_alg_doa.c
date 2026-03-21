/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <stdint.h>

#include "mmw_ctrl.h"
#include "mmw_alg_doa.h"
#include "mmw_alg_ant_calibration.h"
#include "csi_const_structs.h"
#include "csi_math.h"

#define angle_printk(...)

#define  ANGLE_CALC_USE_MDSP_ENABLE          1

MmwMountType_e g_mmw_mount_type = MMW_MOUNT_VERTICAL;
complex16_mdsp angle_calib_data[CALIB_TX_RX_NUM];

uint32_t mmw_gain_adjust_S16Q15(int32_t real, int32_t image)
{
    int32_t mask = ((real>>31)^real)|((image>>31)^image);

    if (mask & 0xFFFF8000) {
        if (!(mask & 0xFF800000)) {
            real  = real>>8;
            image = image>>8;
        } else if (!(mask & 0xF8000000)) {
            real  = real>>12;
            image = image>>12;
        } else {
            real  = real>>16;
            image = image>>16;
        }
    }
    return (uint32_t)((real & (0x0000FFFF)) | (image<<16));
}

void mmw_angle_calculate(const uint32_t *azim_phase, const uint32_t *elev_phase, MmwAngleInfo_t *result)
{
    uint32_t azim_phase_val;
    uint32_t elev_phase_val;
    uint32_t sinValue_Y;
    uint32_t sinValue_Z;

    if (g_mmw_mount_type != MMW_MOUNT_HORIZONTAL) { //on default.
        azim_phase_val = *azim_phase;
        elev_phase_val = *elev_phase;
    } else {
        //MMW_MOUNT_HORIZONTAL
        azim_phase_val = *elev_phase;
        elev_phase_val = *azim_phase;
        azim_phase_val = (azim_phase_val ^ 0xFFFF0000) + 0x10000; //conj due to phase_angle is negetive
    }

    /*
    * azimuth_degree = angleValue_out.azimuth >> 7;(notice:angle unit is degree)
    * elevation_degree = angleValue_out.elevation >> 7;(notice:angle unit is degree)
    * X =  (angleValue_out.sinValue_X >> 15) * R;
    * Y =  (angleValue_out.sinValue_Y >> 15) * R;
    * Z =  (angleValue_out.sinValue_Z >> 15) * R;
    */
    MDSP_FUNC_1T3R_ALL_ANGLE_ESTI(elev_phase_val, azim_phase_val,  ANGLE_CORRECT_FACTOR,
        result->elevation, result->azimuth, result->sinValue_X, sinValue_Z, sinValue_Y);
    if (g_mmw_mount_type != MMW_MOUNT_CEIL) {
        result->sinValue_Y = (int16_t)(sinValue_Y <= 32767 ? sinValue_Y : 32767);
        result->sinValue_Z = (int16_t)sinValue_Z;
    } else {
        result->sinValue_Y = (int16_t)sinValue_Z;
        result->sinValue_Z = -(int16_t)(sinValue_Y <= 32767 ? sinValue_Y : 32767);
    }
}

/*
        Rear View, X is at positive direction of azimuth,  Z is at positive direction of elevation:
                                          _
                                /\Z       /| Y
                                |        /
                                |       /
                                rx0	   /
                                |     /
                                |    /
                                |   /
                                |  /
                                | /                      \
--------------------------------rx1--------rx2------------ X
                                |O                       /
                                |
                                |
                                |
                                |
                                |
                                |
                                |
                                |
                                |
       ant_phase[0] -> ant_phase[1]
        the Y-direction is directly in front of the radar;
        the XOZ plane is the plane where the radar is located.
*/
int mmw_angle_calc_a22(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result)
{
	int32_t phase_real, phase_imag;
    uint32_t phase_azimuth, phase_elevation;
    const uint32_t *ptr_ant_phase = (uint32_t *)ant_aligned;
	MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[1], ptr_ant_phase[2]^(0xFFFF0000), phase_real, phase_imag);
	phase_azimuth = mmw_gain_adjust_S16Q15(phase_real, phase_imag);
	MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[1], ptr_ant_phase[0]^(0xFFFF0000), phase_real, phase_imag);
	phase_elevation = mmw_gain_adjust_S16Q15(phase_real, phase_imag);
	MDSP_FUNC_SCALE_MAG(phase_elevation, phase_elevation);
	MDSP_FUNC_SCALE_MAG(phase_azimuth, phase_azimuth);
    mmw_angle_calculate(&phase_azimuth, &phase_elevation, result);
    return 1;
}


/*
        Rear View, X is at positive direction of azimuth,  Z is at positive direction of elevation:
                                          _
                                /\Z       /| Y
                                |        /
                                |       /
                                rx1	   /   rx0
                                |     /
                                |    /
                                |   /
                                |  /
                                | /                                \
---------------------rx5--------rx4--------rx3--------rx2----------- X
                                |O                                 /
                                |
                                |
                                |
                                |
                                |
                                |
                                |
                                |
                                |
       ant_phase[1] -> ant_phase[0]
    the Y-direction is directly in front of the radar;
    the XOZ plane is the plane where the radar is located.
*/
int mmw_angle_calc_m42(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result)
{
    int32_t phase_real, phase_imag;
    int64_t phase_azimuth_sum_real = 0;
    int64_t phase_azimuth_sum_imag = 0;
    int64_t phase_elevation_sum_real = 0;
    int64_t phase_elevation_sum_imag = 0;
    uint32_t phase_elevation_sum, phase_azimuth_sum;
    const uint32_t *ptr_ant_phase = (uint32_t *)ant_aligned;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[3], ptr_ant_phase[2]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[4], ptr_ant_phase[3]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[5], ptr_ant_phase[4]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;


    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[3], ptr_ant_phase[0]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[4], ptr_ant_phase[1]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

#if 0
    azi_real = phase_azimuth_sum_real >> 18;	//
    azi_imag = phase_azimuth_sum_imag >> 18;	//
    phase_azimuth_sum = (azi_real & (0x0000FFFF)) | (azi_imag << 16);
//  phase_azimuth_sum = ((phase_azimuth_sum_real >> 19) & (0x0000FFFF)) | ((phase_azimuth_sum_imag >> 3) & 0xffff0000);
    dbg_printf("azi_real:%x,azi_imag:%x\n", azi_real, azi_imag);

    ele_real = phase_elevation_sum_real >> 17;
    ele_imag = phase_elevation_sum_imag >> 17;
    phase_elevation_sum = (ele_real & (0x0000FFFF)) | (ele_imag << 16);
//  phase_elevation_sum = ((phase_elevation_sum_real >> 17) & (0x0000FFFF)) | ((phase_elevation_sum_imag >> 1) & 0xffff0000);
    dbg_printf("ele_real:%x,ele_imag:%x\n", ele_real, ele_imag);
#else
    phase_azimuth_sum = mmw_gain_adjust_S16Q15(phase_azimuth_sum_real>>1, phase_azimuth_sum_imag>>1);
    phase_elevation_sum = mmw_gain_adjust_S16Q15(phase_elevation_sum_real, phase_elevation_sum_imag);
#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("azi_real=%x, azi_imag=%x, azi_u32=%x\n", (int32_t)(phase_azimuth_sum_real>>2),
               (int32_t)(phase_azimuth_sum_imag>>2), phase_azimuth_sum);
    angle_printk("ele_real=%x, ele_imag=%x, ele_u32=%x\n", (int32_t)phase_elevation_sum_real,
               (int32_t)phase_elevation_sum_imag, phase_elevation_sum);
#endif
#endif
    mmw_angle_calculate(&phase_azimuth_sum, &phase_elevation_sum, result);
    return 1;
}


/*
    Rear View, X is at positive direction of azimuth,  Z is at positive direction of elevation:
                                                      _
                                            /\Z	      /| Y
                                            |        /
                                            |       /
                                            rx0    /
                                            |     /
                                            |    /
                  rx3        rx2        rx1 /      rx4
                                            |  /
                                            | /                                \
---------------------------------rx7--------rx6--------rx5---------------------- X
                                            |O                                 /
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                   ant_phase[1] -> ant_phase[0]
    the Y-direction is directly in front of the radar;
    the XOZ plane is the plane where the radar is located.
*/
void mmw_angle_phase_calc_m43(complex16_mdsp *ant_aligned, uint32_t *azim_phase, uint32_t *elev_phase)
{
    int32_t phase_real, phase_imag;
    int64_t phase_azimuth_sum_real = 0;
    int64_t phase_azimuth_sum_imag = 0;
    int64_t phase_elevation_sum_real = 0;
    int64_t phase_elevation_sum_imag = 0;
    const uint32_t *ptr_ant_phase = (uint32_t *)ant_aligned;

    /* azimath */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[3], ptr_ant_phase[2]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[2], ptr_ant_phase[1]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[1], ptr_ant_phase[4]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[7], ptr_ant_phase[6]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[6], ptr_ant_phase[5]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    /* elevation */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[5], ptr_ant_phase[4]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[6], ptr_ant_phase[1]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    //new design of alg, remove this phase.
    //MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[1], ptr_ant_phase[0]^(0xFFFF0000), phase_real, phase_imag);
    //phase_elevation_sum_real += phase_real;
    //phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[7], ptr_ant_phase[2]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    /* Compensate 6dB for align lost */
    //phase_azimuth_sum = mmw_gain_adjust_S16Q15(phase_azimuth_sum_real>>3, phase_azimuth_sum_imag>>3);
    //phase_elevation_sum = mmw_gain_adjust_S16Q15(phase_elevation_sum_real>>1, phase_elevation_sum_imag>>1);
    *azim_phase = mmw_gain_adjust_S16Q15(phase_azimuth_sum_real>>2, phase_azimuth_sum_imag>>2);
    *elev_phase = mmw_gain_adjust_S16Q15(phase_elevation_sum_real>>1, phase_elevation_sum_imag>>1);
#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("azi_real=%x, azi_imag=%x, azi_u32=%x\n", (int32_t)(phase_azimuth_sum_real>>2),
               (int32_t)(phase_azimuth_sum_imag>>2), *azim_phase);
    angle_printk("ele_real=%x, ele_imag=%x, ele_u32=%x\n", (int32_t)phase_elevation_sum_real,
               (int32_t)phase_elevation_sum_imag, *elev_phase);
#endif
}

/*
    Rear View, X is at positive direction of azimuth,  Z is at positive direction of elevation:
                                                      _
                                            /\Z       /| Y
                                            |        /
                                            |       /
                                            rx1    /   rx5
                                            |     /
                                            |    /
                                            |   /
                                            |  /
                                            | /                                \
-----------rx3-------rx7---------rx2--------rx6--------rx0--------rx4----------- X
                                            |O                                 /
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                   ant_phase[1] -> ant_phase[0]
    the Y-direction is directly in front of the radar;
    the XOZ plane is the plane where the radar is located.
*/
void mmw_angle_elev_phase_m62(complex16_mdsp *ant_aligned, uint32_t *elev_phase)
{
    int32_t phase_real, phase_imag;
    int64_t phase_elevation_sum_real = 0;
    int64_t phase_elevation_sum_imag = 0;
    const uint32_t *ptr_ant_phase = (uint32_t *)ant_aligned;

    /* elevation */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[0], ptr_ant_phase[5]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[6], ptr_ant_phase[1]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    /* Compensate 6dB for align lost */
    //phase_azimuth_sum = mmw_gain_adjust_S16Q15(phase_azimuth_sum_real>>3, phase_azimuth_sum_imag>>3);
    //phase_elevation_sum = mmw_gain_adjust_S16Q15(phase_elevation_sum_real>>1, phase_elevation_sum_imag>>1);
    *elev_phase = mmw_gain_adjust_S16Q15(phase_elevation_sum_real, phase_elevation_sum_imag);
#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("ele_real=%x, ele_imag=%x, ele_u32=%x\n", (int32_t)phase_elevation_sum_real,
               (int32_t)phase_elevation_sum_imag, *elev_phase);
#endif
}

void mmw_angle_phase_calc_m62(complex16_mdsp *ant_aligned, uint32_t *azim_phase, uint32_t *elev_phase)
{
    int32_t phase_real, phase_imag;
    int64_t phase_azimuth_sum_real = 0;
    int64_t phase_azimuth_sum_imag = 0;
    int64_t phase_elevation_sum_real = 0;
    int64_t phase_elevation_sum_imag = 0;
    const uint32_t *ptr_ant_phase = (uint32_t *)ant_aligned;

    /* azimath */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[0], ptr_ant_phase[4]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[6], ptr_ant_phase[0]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[2], ptr_ant_phase[6]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[7], ptr_ant_phase[2]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[3], ptr_ant_phase[7]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;

    /* rx1 - rx5 */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[1], ptr_ant_phase[5]^(0xFFFF0000), phase_real, phase_imag);
    phase_azimuth_sum_real += phase_real;
    phase_azimuth_sum_imag += phase_imag;


    /* elevation */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[0], ptr_ant_phase[5]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[6], ptr_ant_phase[1]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    /* Compensate 6dB for align lost */
    //phase_azimuth_sum = mmw_gain_adjust_S16Q15(phase_azimuth_sum_real>>3, phase_azimuth_sum_imag>>3);
    //phase_elevation_sum = mmw_gain_adjust_S16Q15(phase_elevation_sum_real>>1, phase_elevation_sum_imag>>1);
    *azim_phase = mmw_gain_adjust_S16Q15(phase_azimuth_sum_real>>2, phase_azimuth_sum_imag>>2);
    *elev_phase = mmw_gain_adjust_S16Q15(phase_elevation_sum_real, phase_elevation_sum_imag);
#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("azi_real=%x, azi_imag=%x, azi_u32=%x\n", (int32_t)(phase_azimuth_sum_real>>2),
               (int32_t)(phase_azimuth_sum_imag>>2), *azim_phase);
    angle_printk("ele_real=%x, ele_imag=%x, ele_u32=%x\n", (int32_t)phase_elevation_sum_real,
               (int32_t)phase_elevation_sum_imag, *elev_phase);
#endif
}

/*
    Rear View, X is at positive direction of azimuth,  Z is at positive direction of elevation:
                                                      _
                                            /\Z       /| Y
                                            |        /
                                            |       /
                    rx7                    rx3     /
                                            |     /
                                            |    /
                    rx6                    rx2  /
                                            |  /
                                            | /                                \
--------------------rx5---------rx4--------rx1---------rx0-------------------- X
                                            |O                                 /
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                                            |
                   ant_phase[1] -> ant_phase[0]
    the Y-direction is directly in front of the radar;
    the XOZ plane is the plane where the radar is located.
*/
void mmw_angle_azim_phase_a43(complex16_mdsp *ant_aligned, uint32_t *azim_phase)
{
    int32_t phase_real, phase_imag;
    int64_t phase_elevation_sum_real = 0;
    int64_t phase_elevation_sum_imag = 0;
    const uint32_t *ptr_ant_phase = (uint32_t *)ant_aligned;
    /* elevation */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[5], ptr_ant_phase[4]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[4], ptr_ant_phase[1]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[1], ptr_ant_phase[0]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    /* Compensate 6dB for align lost */
    //*azim_phase = mmw_gain_adjust_S16Q15(phase_elevation_sum_real>>1, phase_elevation_sum_imag>>1);
    *azim_phase = mmw_gain_adjust_S16Q15(phase_elevation_sum_real, phase_elevation_sum_imag);
#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("azim_real=%x, azim__imag=%x, azim__u32=%x\n", (int32_t)phase_elevation_sum_real,
               (int32_t)phase_elevation_sum_imag, *azim_phase);
#endif
}
void mmw_angle_elev_phase_a43(complex16_mdsp *ant_aligned, uint32_t *elev_phase)
{
    int32_t phase_real, phase_imag;
    int64_t phase_elevation_sum_real = 0;
    int64_t phase_elevation_sum_imag = 0;
    const uint32_t *ptr_ant_phase = (uint32_t *)ant_aligned;
    /* elevation */
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[1], ptr_ant_phase[2]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[2], ptr_ant_phase[3]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[5], ptr_ant_phase[6]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(ptr_ant_phase[6], ptr_ant_phase[7]^(0xFFFF0000), phase_real, phase_imag);
    phase_elevation_sum_real += phase_real;
    phase_elevation_sum_imag += phase_imag;

    /* Compensate 6dB for align lost */
    //*elev_phase = mmw_gain_adjust_S16Q15(phase_elevation_sum_real>>2, phase_elevation_sum_imag>>2);
    *elev_phase = mmw_gain_adjust_S16Q15(phase_elevation_sum_real>>1, phase_elevation_sum_imag>>1);
#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("elev_real=%x, elev_imag=%x, elev_u32=%x\n", (int32_t)phase_elevation_sum_real,
               (int32_t)phase_elevation_sum_imag, *elev_phase);
#endif
}

int mmw_angle_calc_m43(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result)
{
    uint32_t phase_azim, phase_elev;
#if (FUNC_CALIB_DEBUG_EN)
    int32_t angle_start;
    int32_t time_start = csi_coret_get_value();
#endif
    mmw_angle_phase_calc_m43(ant_aligned, &phase_azim, &phase_elev);
#if (FUNC_CALIB_DEBUG_EN)
    angle_start = csi_coret_get_value();
#endif
    mmw_angle_calculate(&phase_azim, &phase_elev, result);

#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("M43 Angle-time=%d phase-time=%d\n",
        wrap32_diff(csi_coret_get_value(), time_start),
        wrap32_diff(angle_start, time_start));
#endif
    return 1;
}

int mmw_angle_calc_m62(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result)
{
    uint32_t phase_azim, phase_elev;
#if (FUNC_CALIB_DEBUG_EN)
    int32_t angle_start;
    int32_t time_start = csi_coret_get_value();
#endif
    mmw_angle_phase_calc_m62(ant_aligned, &phase_azim, &phase_elev);
#if (FUNC_CALIB_DEBUG_EN)
    angle_start = csi_coret_get_value();
#endif
    mmw_angle_calculate(&phase_azim, &phase_elev, result);

#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("M62 Angle-time=%d phase-time=%d\n",
        wrap32_diff(csi_coret_get_value(), time_start),
        wrap32_diff(angle_start, time_start));
#endif
    return 1;
}

int mmw_angle_calc_a43(complex16_mdsp *ant_aligned, MmwAngleInfo_t *result)
{
    uint32_t phase_azim, phase_elev;

#if (FUNC_CALIB_DEBUG_EN)
    int32_t angle_start;
    int32_t time_start = csi_coret_get_value();
#endif
    mmw_angle_azim_phase_a43(ant_aligned, &phase_azim);
    mmw_angle_elev_phase_a43(ant_aligned, &phase_elev);
#if (FUNC_CALIB_DEBUG_EN)
    angle_start = csi_coret_get_value();
#endif
    mmw_angle_calculate(&phase_azim, &phase_elev, result);

#if (FUNC_CALIB_DEBUG_EN)
    angle_printk("A43 Angle-time=%d phase-time=%d\n",
        wrap32_diff(csi_coret_get_value(), time_start),
        wrap32_diff(angle_start, time_start));
#endif
    return 1;
}


#define CORDIC_EXP_2PI       (1.0f)
#define CORDIC_EXP_PI        (CORDIC_EXP_2PI/2)
#define CORDIC_EXP_1_2PI     (CORDIC_EXP_2PI/4)
#define CORDIC_EXP_3_2PI     (CORDIC_EXP_2PI - CORDIC_EXP_1_2PI)

static int16_t poly_sin(float angle_float)
{
    float in2 = angle_float*angle_float;
    float out = ((74.165f*in2 - 41.125f) * in2 + 6.28f) * angle_float;

    return (int16_t)(out * 16384);
}

static inline void cordic_exp(float angle_float,  complex16_mdsp *phase)
{
    int16_t real, imag;
    angle_float = (angle_float < 0 ? angle_float + CORDIC_EXP_2PI : angle_float);
    if (angle_float >= CORDIC_EXP_3_2PI) {
        //0.75 <= angle < 1
        real = poly_sin(angle_float - CORDIC_EXP_3_2PI);
        imag = -poly_sin(CORDIC_EXP_2PI - angle_float);
    } else if (angle_float >= CORDIC_EXP_PI) {
        //0.5 <= angle < 0.75
        real = -poly_sin(CORDIC_EXP_3_2PI - angle_float);
        imag = -poly_sin(angle_float - CORDIC_EXP_PI);
    } else if (angle_float >= CORDIC_EXP_1_2PI) {
        //0.25 <= angle < 0.5
        real = -poly_sin(angle_float - CORDIC_EXP_1_2PI);
        imag = poly_sin(CORDIC_EXP_PI - angle_float);
    } else {
        //0.0 <= angle < 0.25
        real = poly_sin(CORDIC_EXP_1_2PI - angle_float);
        imag = poly_sin(angle_float);
    }
    phase->real = real;
    phase->imag = imag;
}

void mmw_angle_bin_phase(complex16_mdsp *data_dbf, uint32_t peak_idx, complex16_mdsp *phase)
{
    /* interpolate */
    int32_t left  = (int32_t)COMPLEX16_POWER(&data_dbf[(peak_idx - 1) & 0xF]);
    int32_t mid   = (int32_t)COMPLEX16_POWER(&data_dbf[peak_idx]);
    int32_t right = (int32_t)COMPLEX16_POWER(&data_dbf[(peak_idx + 1) & 0xF]);

    //a = l/2  + r/2 - mid
    //b = -l/2 + r/2
    //out = -b/2/a = (r/2 - l/2)/((mid - l)  + (mid - r))
    float comp = right - left;

    mid = (mid - left) + (mid - right);

    /* angle_q30 is idx/16 */
    float angle_float = (comp/(mid + 1) + peak_idx*2)/32; //avoid mid is 0
    cordic_exp(angle_float, phase);
#if (MPC_SHOW_DBF_FFT_DEBUG)
    calib_printk("azimuth%d %d phase %d %d\n", peak_idx,
                (int)(angle_float*10000), phase->real, phase->imag);
#endif
}

uint32_t mmw_angle_fft(complex16_mdsp *ant_aligned, complex16_mdsp *dbf_fft_out,
                           const uint8_t *ant_geometry, uint32_t dbf_ant_num)
{
    int i;
    uint32_t noise_level = 0;
    uint32_t max_abs = 0;
#if (MDSP_FFT_CALC_ENABLE)
    complex16_mdsp dbf_fft_in[MMW_DBF_FFT_NUM];
#else
    complex16_mdsp *dbf_fft_in = dbf_fft_out;
#endif
    for (i = 0; i < dbf_ant_num; ++i) {
        uint32_t abs_tmp;
        uint8_t  ant_idx  = ant_geometry[i];
        dbf_fft_in[i].real = ant_aligned[ant_idx].real;
        dbf_fft_in[i].imag = ant_aligned[ant_idx].imag;
        abs_tmp = complex16_abs(ant_aligned[ant_idx].real, ant_aligned[ant_idx].imag);
        if (abs_tmp > max_abs) {
            max_abs = abs_tmp;
        }
        noise_level += abs_tmp;
    }
    /* padding 0 for fft */
    while (i < MMW_DBF_FFT_NUM) {
        dbf_fft_in[i].real = 0;
        dbf_fft_in[i].imag = 0;
        i++;
    }

    /* Gain adjust */
    if (max_abs & (1U<<15)) {
        for (i = 0; i < dbf_ant_num; ++i) {
            dbf_fft_in[i].real = dbf_fft_in[i].real >> 1;
            dbf_fft_in[i].imag = dbf_fft_in[i].imag >> 1;
        }
    } else if (max_abs) {
        int shift = 0;
        while (!(max_abs & (1U<<14))) {
            max_abs = max_abs<<1;
            shift++;
        }
        for (i = 0; i < dbf_ant_num; ++i) {
            dbf_fft_in[i].real = dbf_fft_in[i].real << shift;
            dbf_fft_in[i].imag = dbf_fft_in[i].imag << shift;
        }
    } else {
        return 0;
    }

    /* angle fft16 */
    //mpc_show_complex_vec("fft-in", dbf_fft_in, sizeof(ant_geometry), 1);
#if (MDSP_FFT_CALC_ENABLE)
    RunComplexFFT((uint32_t *)&dbf_fft_in[0], MMW_DBF_FFT_NUM_LOG2, (uint32_t *)&dbf_fft_out[0]);
#else
    csi_cfft_q15(&csi_cfft_sR_q15_len16, (q15_t *)&dbf_fft_out[0], 0, 1);
#endif
    //mpc_show_complex_vec("fft-out", dbf_fft_out, MMW_DBF_FFT_NUM, 1);
    return noise_level;
}

uint32_t mmw_angle_cfar(complex16_mdsp *fft_out, uint32_t noise_level, complex16_mdsp *phase, uint32_t angle_max)
{
    uint32_t dbf_num = 0;
    uint8_t  max_idx[(MMW_DBF_FFT_NUM>>1) + 1];
    uint32_t max_peak[(MMW_DBF_FFT_NUM>>1) + 1] = {0};
    uint32_t max_peak_th;

    for (int i = 0; i < MMW_DBF_FFT_NUM; ++i) {
        /* noise threshold */
        uint32_t power_pre, power_next;
        uint32_t power = COMPLEX16_POWER(&fft_out[i]);

        if (power < noise_level) {
            continue ;
        }
        /* stationary detect */
        power_pre  = COMPLEX16_POWER(&fft_out[(i - 1)&(MMW_DBF_FFT_NUM - 1)]);
        power_next = COMPLEX16_POWER(&fft_out[(i + 1)&(MMW_DBF_FFT_NUM - 1)]);
        if (power < power_next || power < power_pre) {
            continue ;
        }
        /* save peaks */
        for (int j = 0; j < angle_max; ++j) {
            if (power > max_peak[j]) {
                max_peak[j + 1] = max_peak[j];
                max_idx[j + 1]  = max_idx[j];
                max_peak[j] = power;
                max_idx[j]  = i;
                break;
            }
        }
    }
    /* peak out */
    max_peak_th = max_peak[0]/MMW_DBF_PEAK_SFDR;  //10dB
    while (max_peak[dbf_num] > max_peak_th && dbf_num < angle_max) {
        mmw_angle_bin_phase(fft_out, max_idx[dbf_num], &phase[dbf_num]);
        dbf_num++;
    }
    return dbf_num;
}


int mmw_angle_dbf_m62_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {4, 0, 6, 2, 7, 3};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;

    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        return mmw_angle_cfar(&dbf_fft_out[0], noise_level, azim_phase, buf_max);
    } else {
        return 0;
    }
}
int mmw_angle_dbf_m62(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {4, 0, 6, 2, 7, 3};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;

    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        uint32_t dbf_num, angle_num = 0;
        complex16_mdsp azim_phase[MMW_DBF_MAX_PEAK];
        dbf_num = mmw_angle_cfar(&dbf_fft_out[0], noise_level, &azim_phase[0], buf_max);
        for (int i = 0; i < dbf_num; ++i) {
            uint32_t elev_phase;
            mmw_angle_elev_phase_m62(ant_aligned, &elev_phase);
            mmw_angle_calculate((uint32_t *)&azim_phase[i], &elev_phase, angles);
            angles++;
            angle_num++;
        }
        return angle_num;
    } else {
        return 0;
    }
}

int mmw_angle_dbf_m81_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {4, 5, 6, 7, 0, 1, 2, 3};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;
    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        return mmw_angle_cfar(&dbf_fft_out[0], noise_level, azim_phase, buf_max);
    } else {
        return 0;
    }
}
int mmw_angle_dbf_m81(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {4, 5, 6, 7, 0, 1, 2, 3};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;

    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        uint32_t dbf_num, angle_num = 0;
        complex16_mdsp azim_phase[MMW_DBF_MAX_PEAK];
        uint32_t elev_phase = 0x1000;

        dbf_num = mmw_angle_cfar(&dbf_fft_out[0], noise_level, &azim_phase[0], buf_max);
        for (int i = 0; i < dbf_num; ++i) {
            mmw_angle_calculate((uint32_t *)&azim_phase[i], &elev_phase, angles);
            angles++;
            angle_num++;
        }
        return angle_num;
    } else {
        return 0;
    }
}

int mmw_angle_dbf_a81_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {0, 1, 2, 3, 4, 5, 6, 7};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;
    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        return mmw_angle_cfar(&dbf_fft_out[0], noise_level, azim_phase, buf_max);
    } else {
        return 0;
    }
}

int mmw_angle_dbf_a81(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {0, 1, 2, 3, 4, 5, 6, 7};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;

    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        uint32_t dbf_num, angle_num = 0;
        complex16_mdsp azim_phase[MMW_DBF_MAX_PEAK];
        uint32_t elev_phase = 0x1000;

        dbf_num = mmw_angle_cfar(&dbf_fft_out[0], noise_level, &azim_phase[0], buf_max);
        for (int i = 0; i < dbf_num; ++i) {
            mmw_angle_calculate((uint32_t *)&azim_phase[i], &elev_phase, angles);
            angles++;
            angle_num++;
        }
        return angle_num;
    } else {
        return 0;
    }
}

int mmw_angle_dbf_a43_azim_phase(complex16_mdsp *ant_aligned, complex16_mdsp *azim_phase, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {0, 1, 4, 5};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;

    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        return mmw_angle_cfar(&dbf_fft_out[0], (noise_level>>2), azim_phase, buf_max);
    } else {
        return 0;
    }
}
int mmw_angle_dbf_a43(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max)
{
    const uint8_t ant_geometry[] = {0, 1, 4, 5};
    complex16_mdsp dbf_fft_out[MMW_DBF_FFT_NUM];
    uint32_t noise_level = 0;

    noise_level = mmw_angle_fft(&ant_aligned[0], &dbf_fft_out[0], &ant_geometry[0], sizeof(ant_geometry));
    if (noise_level) {
        uint32_t dbf_num, angle_num = 0;
        complex16_mdsp azim_phase[MMW_DBF_MAX_PEAK];
        dbf_num = mmw_angle_cfar(&dbf_fft_out[0], (noise_level>>2), &azim_phase[0], buf_max);
        for (int i = 0; i < dbf_num; ++i) {
            uint32_t elev_phase;

            mmw_angle_elev_phase_a43(ant_aligned, &elev_phase);
            mmw_angle_calculate((uint32_t *)&azim_phase[i], &elev_phase, angles);
            angles++;
            angle_num++;
        }
        return angle_num;
    } else {
        return 0;
    }
}

int mmw_angle_estim_by_data(complex16_cube *ant_data, MmwAngleInfo_t *angles)
{
    complex16_mdsp ant_aligned[MMW_ANT_CHANNEL_NUM];
#if (CONFIG_SOC_RS6130 == 0)
    mmw_cube_ant_align_2t4r(&ant_data[0], &angle_calib_data[0], &ant_aligned[0]);
#endif

#if (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_A22)
	ant_aligned[0].real = ant_data[0].real;
	ant_aligned[0].imag = ant_data[0].imag;
	ant_aligned[1].real = ant_data[1].real;
	ant_aligned[1].imag = ant_data[1].imag;
	ant_aligned[2].real = ant_data[2].real;
	ant_aligned[2].imag = ant_data[2].imag;
	mmw_angle_calc_a22(&ant_aligned[0], angles);
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_M62)
    mmw_angle_dbf_m62(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_A43)
    mmw_angle_dbf_a43(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#else
    mmw_angle_calc_m43(&ant_aligned[0], angles);
#endif
    return (int)(angles->elevation < ANGLE_ELEV_INVALID);
}

int mmw_angle_estim_by_index(uint16_t range_idx, uint16_t dop_idx, MmwAngleInfo_t *angles)
{
    complex16_cube  ant_data[MMW_ANT_CHANNEL_NUM];
    complex16_mdsp  *ant_aligned = (complex16_mdsp *)&ant_data[0];
    int ret = mmw_fft_ants(&ant_data[0], dop_idx, range_idx);

    if (ret == MMW_ERR_CODE_SUCCESS) {
#if (CONFIG_SOC_RS6130 == 0)
        mmw_cube_ant_align_2t4r(&ant_data[0], &angle_calib_data[0], &ant_aligned[0]);
#endif

#if (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_M62)
        mmw_angle_dbf_m62(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_A43)
        mmw_angle_dbf_a43(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#else
        mmw_angle_calc_m43(&ant_aligned[0], angles);
#endif
        return (int)(angles->elevation < ANGLE_ELEV_INVALID);
    } else {
        return 0;
    }
}

int mmw_angle_mount_type_set(MmwMountType_e mount_type)
{
    g_mmw_mount_type = mount_type;
    return 0;
}
MmwMountType_e mmw_angle_mount_type_get(void)
{
    return g_mmw_mount_type;
}

int mmw_angle_init(void)
{
#if (CONFIG_SOC_MULTI_CORE == 0)
	mmw_ant_calib_data_load();
#endif
#if (ANGLE_CALC_USE_MDSP_ENABLE)
    mdsp_angle_calc_init();
#endif
    return mmw_ant_calib_data_get((uint32_t *)&angle_calib_data[0], sizeof(angle_calib_data));
}

int mmw_angle_dbf(complex16_mdsp *ant_aligned, MmwAngleInfo_t *angles, uint32_t buf_max)
{
    int ret = 0;

#if (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_A22)
	ret = mmw_angle_calc_a22(&ant_aligned[0], angles);
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_M62)
    ret = mmw_angle_dbf_m62(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_A43)
    ret = mmw_angle_dbf_a43(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_M43)
    ret = 0;
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_A81)
    ret = mmw_angle_dbf_a81(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#elif (MMW_ANT_GEOMETRY_TYPE == MMW_ANT_GEOMETRY_M81)
    ret = mmw_angle_dbf_m81(&ant_aligned[0], angles, MMW_DBF_MAX_PEAK);
#endif

    return ret;
}

void * mmw_angle_calib_data_get(void)
{
    return angle_calib_data;
}
