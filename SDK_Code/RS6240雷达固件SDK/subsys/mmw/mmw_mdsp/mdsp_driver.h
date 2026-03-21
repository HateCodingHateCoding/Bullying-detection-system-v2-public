#ifndef MDSP_DRIVER_H
#define MDSP_DRIVER_H

extern void cpu_delay(uint32_t dly);

#define MDSP_BASE_ADDR          (0x50000000)

#define WRITE_REG(addr, data)   (*(volatile uint32_t*)(addr) = (data))
#define READ_REG(addr)          (*(volatile uint32_t*)(addr))

/* FUNCTION: init MDSP */
#define MDSP_CCMU_BASE                               (0x40017000)
#define MDSP_RESET_CTRL                              (MDSP_CCMU_BASE + 0)
#define MDSP_CLK_CTRL                                (MDSP_CCMU_BASE + 0x1c)

/* FUNCTION:  complex16Multiply */
#define MDSP_COMPLEX16_MULTI_MODE_IN                 (MDSP_BASE_ADDR + 0)
#define MDSP_COMPLEX16_MULTI_X_IN                    (MDSP_BASE_ADDR + 1 * 4)
#define MDSP_COMPLEX16_MULTI_Y_IN                    (MDSP_BASE_ADDR + 2 * 4)
#define MDSP_COMPLEX16_MULTI_Z_IN                    (MDSP_BASE_ADDR + 3 * 4)
#define MDSP_COMPLEX16_MULTI_MULTI_REAL_OUT          (MDSP_BASE_ADDR + 4 * 4)
#define MDSP_COMPLEX16_MULTI_MULTI_IMAG_OUT          (MDSP_BASE_ADDR + 5 * 4)
#define MDSP_COMPLEX16_MULTI_COMPLEX_OUT             (MDSP_BASE_ADDR + 6 * 4)
#define MDSP_COMPLEX16_MULTI_BUTTERFLY_1_OUT         (MDSP_BASE_ADDR + 7 * 4)
#define MDSP_COMPLEX16_MULTI_BUTTERFLY_2_OUT         (MDSP_BASE_ADDR + 8 * 4)
#define MDSP_COMPLEX16_MULTI_BUTTERFLY_3_OUT         (MDSP_BASE_ADDR + 9 * 4)

typedef union _complex16mdsp_u {
    struct {
        int16_t real;
        int16_t imag;
    };
    uint32_t value32;
} complex16_mdsp;

typedef struct _complex32mdsp_t {
    int32_t real;
    int32_t imag;
} complex32_mdsp;

#define MDSP_FUNC_COMPLEX16_MULTI_MODE0(x, y, z, real, imag, complex32, butter_fly_buf_0, butter_fly_buf_1) { \
    WRITE_REG(MDSP_COMPLEX16_MULTI_MODE_IN, 0);                          \
    WRITE_REG(MDSP_COMPLEX16_MULTI_X_IN, (x));                             \
    WRITE_REG(MDSP_COMPLEX16_MULTI_Y_IN, (y));                             \
    WRITE_REG(MDSP_COMPLEX16_MULTI_Z_IN, (z));                             \
    real = READ_REG(MDSP_COMPLEX16_MULTI_MULTI_REAL_OUT);                \
    imag = READ_REG(MDSP_COMPLEX16_MULTI_MULTI_IMAG_OUT);                \
    complex32 = READ_REG(MDSP_COMPLEX16_MULTI_COMPLEX_OUT);              \
    butter_fly_buf_0 = READ_REG(MDSP_COMPLEX16_MULTI_BUTTERFLY_1_OUT);   \
    butter_fly_buf_1 = READ_REG(MDSP_COMPLEX16_MULTI_BUTTERFLY_2_OUT);   \
}

#define MDSP_FUNC_COMPLEX16_MULTI_MODE1(x, y, z, butter_fly_buf)  {      \
    WRITE_REG(MDSP_COMPLEX16_MULTI_MODE_IN, 1);                          \
    WRITE_REG(MDSP_COMPLEX16_MULTI_X_IN, (x));                           \
    WRITE_REG(MDSP_COMPLEX16_MULTI_Y_IN, (y));                           \
    WRITE_REG(MDSP_COMPLEX16_MULTI_Z_IN, (z));                           \
    butter_fly_buf = READ_REG(MDSP_COMPLEX16_MULTI_BUTTERFLY_3_OUT);     \
}

#define MDSP_FUNC_COMPLEX16_MULTI_MODE0_FLAG_START(start) { \
    WRITE_REG(MDSP_COMPLEX16_MULTI_MODE_IN, start);         \
}

#define MDSP_FUNC_COMPLEX16_MULTI_MODE0_HALF_16BIT_RESULT(x, y, complex32)  { \
    WRITE_REG(MDSP_COMPLEX16_MULTI_X_IN, (x));                             \
    WRITE_REG(MDSP_COMPLEX16_MULTI_Y_IN, (y));                             \
    complex32 = READ_REG(MDSP_COMPLEX16_MULTI_COMPLEX_OUT);              \
}

#define MDSP_FUNC_COMPLEX16_MULTI_MODE0_TOTAL_32BIT_RESULT(x, y, real, imag)  { \
    WRITE_REG(MDSP_COMPLEX16_MULTI_X_IN, (x));                             \
    WRITE_REG(MDSP_COMPLEX16_MULTI_Y_IN, (y));                             \
    real = READ_REG(MDSP_COMPLEX16_MULTI_MULTI_REAL_OUT);                \
    imag = READ_REG(MDSP_COMPLEX16_MULTI_MULTI_IMAG_OUT);                \
}



/* FUNCTION BIT_INV */
#define MDSP_BIT_INV_B_IN                        (MDSP_BASE_ADDR + 10 * 4)
#define MDSP_BIT_INV_A_IN                        (MDSP_BASE_ADDR + 11 * 4)
#define MDSP_BIT_INV_OUT                         (MDSP_BASE_ADDR + 12 * 4)

#define MDSP_FUNC_BIT_INV(b, a, out)                              {   \
    WRITE_REG(MDSP_BIT_INV_B_IN, (b));                           \
    WRITE_REG(MDSP_BIT_INV_A_IN, (a));                           \
    out = READ_REG(MDSP_BIT_INV_OUT);                            \
}

/* FUNCTION AVALANCHE  */
#define MDSP_AVALANCHE_ID_IN                     (MDSP_BASE_ADDR + 13 * 4)
#define MDSP_AVALANCHE_DATA_IN                   (MDSP_BASE_ADDR + 14 * 4)
#define MDSP_AVALANCHE_OUT                       (MDSP_BASE_ADDR + 15 * 4)
#define MDSP_FUNC_AVALANCHE(id, in, out)                           {  \
    WRITE_REG(MDSP_AVALANCHE_ID_IN, (id));                       \
    WRITE_REG(MDSP_AVALANCHE_DATA_IN, (in));                     \
    out = READ_REG(MDSP_AVALANCHE_OUT);                          \
}

/* FUNCTION ABS_LOG */
#define MDSP_ABS_LOG_COMPLEX32_IN                    (MDSP_BASE_ADDR + 16 * 4)
#define MDSP_ABS_LOG_OUT                             (MDSP_BASE_ADDR + 17 * 4)
#define MDSP_ABS_LOG_DB_OUT                          (MDSP_BASE_ADDR + 18 * 4)
#define MDSP_FUNC_ABSLOG(in, out, db_out)                            {    \
    WRITE_REG(MDSP_ABS_LOG_COMPLEX32_IN, (in));                      \
    cpu_delay(3);                                                    \
    out = READ_REG(MDSP_ABS_LOG_OUT);                                \
    db_out = READ_REG(MDSP_ABS_LOG_DB_OUT);                          \
}


/* FUNCTION COMPLEX_ADDER */
#define MDSP_COMPLEX_ADDER_COMPLEX32_X_IN                (MDSP_BASE_ADDR + 19 * 4)
#define MDSP_COMPLEX_ADDER_COMPLEX32_Y_IN                (MDSP_BASE_ADDR + 20 * 4)
#define MDSP_COMPLEX_ADDER_COMPLEX64_ADD_REAL_OUT        (MDSP_BASE_ADDR + 21 * 4)
#define MDSP_COMPLEX_ADDER_COMPLEX64_ADD_IMAG_OUT        (MDSP_BASE_ADDR + 22 * 4)
#define MDSP_COMPLEX_ADDER_COMPLEX64_MINUS_REAL_OUT      (MDSP_BASE_ADDR + 23 * 4)
#define MDSP_COMPLEX_ADDER_COMPLEX64_MINUS_IMAG_OUT      (MDSP_BASE_ADDR + 24 * 4)
#define MDSP_COMPLEX_ADDER_COMPLEX32_ADD_HALF_OUT        (MDSP_BASE_ADDR + 25 * 4)
#define MDSP_COMPLEX_ADDER_COMPLEX32_MINUS_HALF_OUT      (MDSP_BASE_ADDR + 26 * 4)
#define MDSP_FUNC_COMPLEX_ADDER(x, y, add_real, add_imag, minus_real, minus_imag, complex_add_div, complex_minus_div) {\
    WRITE_REG(MDSP_COMPLEX_ADDER_COMPLEX32_X_IN, (x));                           \
    WRITE_REG(MDSP_COMPLEX_ADDER_COMPLEX32_Y_IN, (y));                           \
    add_real = READ_REG(MDSP_COMPLEX_ADDER_COMPLEX64_ADD_REAL_OUT);              \
    add_imag = READ_REG(MDSP_COMPLEX_ADDER_COMPLEX64_ADD_IMAG_OUT);              \
    minus_real = READ_REG(MDSP_COMPLEX_ADDER_COMPLEX64_MINUS_REAL_OUT);          \
    minus_imag = READ_REG(MDSP_COMPLEX_ADDER_COMPLEX64_MINUS_IMAG_OUT);          \
    complex_add_div = READ_REG(MDSP_COMPLEX_ADDER_COMPLEX32_ADD_HALF_OUT);       \
    complex_minus_div = READ_REG(MDSP_COMPLEX_ADDER_COMPLEX32_MINUS_HALF_OUT);   \
}

/* FUNCTION ANGLE_ESTI */
#define MDSP_ANGLE_ESTI_MODE_IN                          (MDSP_BASE_ADDR + 27 * 4)
#define MDSP_ANGLE_ESTI_COMPLEX32_X_IN                   (MDSP_BASE_ADDR + 28 * 4)
#define MDSP_ANGLE_ESTI_COMPLEX32_Y_IN                   (MDSP_BASE_ADDR + 29 * 4)
#define MDSP_ANGLE_ESTI_FACTOR_IN                        (MDSP_BASE_ADDR + 30 * 4)
#define MDSP_ANGLE_ESTI_FLAG_OUT                         (MDSP_BASE_ADDR + 31 * 4)
#define MDSP_ANGLE_ESTI_PITCH_OUT                        (MDSP_BASE_ADDR + 32 * 4)
#define MDSP_ANGLE_ESTI_AZIMUTH_OUT                      (MDSP_BASE_ADDR + 33 * 4)
#define MDSP_ANGLE_ESTI_XSIN_OUT                         (MDSP_BASE_ADDR + 34 * 4)
#define MDSP_ANGLE_ESTI_YSIN_OUT                         (MDSP_BASE_ADDR + 35 * 4)
#define MDSP_ANGLE_ESTI_ZSIN_OUT                         (MDSP_BASE_ADDR + 36 * 4)
#define MDSP_FUNC_1T3R_ALL_ANGLE_ESTI(x, y, fac, pitch, azimuth, x_sin, y_sin, z_sin) {\
    WRITE_REG(MDSP_ANGLE_ESTI_COMPLEX32_X_IN, (x));                           \
    WRITE_REG(MDSP_ANGLE_ESTI_COMPLEX32_Y_IN, (y));                           \
    WRITE_REG(MDSP_ANGLE_ESTI_FACTOR_IN, (fac));                              \
    while(0 == READ_REG(MDSP_ANGLE_ESTI_FLAG_OUT));                           \
    (pitch) = READ_REG(MDSP_ANGLE_ESTI_PITCH_OUT);                            \
    (azimuth) = READ_REG(MDSP_ANGLE_ESTI_AZIMUTH_OUT);                        \
    (x_sin) = READ_REG(MDSP_ANGLE_ESTI_XSIN_OUT);                             \
    (y_sin) = READ_REG(MDSP_ANGLE_ESTI_YSIN_OUT);                             \
    (z_sin) = READ_REG(MDSP_ANGLE_ESTI_ZSIN_OUT);                             \
}
#define MDSP_FUNC_1T2R_AZIMUTH_ANGLE_ESTI(x, y, fac, pitch, y_sin, z_sin) {\
    WRITE_REG(MDSP_ANGLE_ESTI_COMPLEX32_X_IN, (x));                           \
    WRITE_REG(MDSP_ANGLE_ESTI_COMPLEX32_Y_IN, (y));                           \
    WRITE_REG(MDSP_ANGLE_ESTI_FACTOR_IN, (fac));                              \
    while(0 == READ_REG(MDSP_ANGLE_ESTI_FLAG_OUT));                           \
    (pitch) = READ_REG(MDSP_ANGLE_ESTI_PITCH_OUT);                            \
    (y_sin) = READ_REG(MDSP_ANGLE_ESTI_YSIN_OUT);                             \
    (z_sin) = READ_REG(MDSP_ANGLE_ESTI_ZSIN_OUT);                             \
}
#define MDSP_FUNC_ANGLE_ESTI_RAD_UNIT_FLAG_START() {\
    WRITE_REG(MDSP_ANGLE_ESTI_MODE_IN, (0));                                  \
}
#define MDSP_FUNC_ANGLE_ESTI_DEGREE_UNIT_FLAG_START() {\
    WRITE_REG(MDSP_ANGLE_ESTI_MODE_IN, (1));                                  \
}
#define MDSP_FUNC_ANGLE_ESTI_CYCLE_UNIT_FLAG_START() {\
    WRITE_REG(MDSP_ANGLE_ESTI_MODE_IN, (2));                                  \
}

/* FUNCTION SCALE_MAG */
#define MDSP_SCALE_MAG_COMPLEX32_IN                      (MDSP_BASE_ADDR + 37 * 4)
#define MDSP_SCALE_MAG_COMPLEX32_SCALED_OUT              (MDSP_BASE_ADDR + 38 * 4)
#define MDSP_FUNC_SCALE_MAG(in, out) {\
    WRITE_REG(MDSP_SCALE_MAG_COMPLEX32_IN, (in));                             \
    (out) = READ_REG(MDSP_SCALE_MAG_COMPLEX32_SCALED_OUT);                    \
}

/* FUNCTION SQRT */
#define MDSP_SCALE_MAG_X_IN                              (MDSP_BASE_ADDR + 39 * 4)
#define MDSP_SCALE_MAG_SQRT_X_OUT                        (MDSP_BASE_ADDR + 40 * 4)
#define MDSP_FUNC_SQRT(in, out) {\
    WRITE_REG(MDSP_SCALE_MAG_X_IN, (in));                             \
    (out) = READ_REG(MDSP_SCALE_MAG_SQRT_X_OUT);                      \
}

static inline void mdsp_complex16_multi_mode0_init(void)
{
    MDSP_FUNC_COMPLEX16_MULTI_MODE0_FLAG_START(0);
}

/*if you want to config the unit of angle result is rad, this function must be called before all calculating angle function be called.
 * the rad unit is S16Q13.
 */
static inline void mdsp_angle_rad_unit_flag_init(void)
{
    MDSP_FUNC_ANGLE_ESTI_RAD_UNIT_FLAG_START();
}
/*if you want to config the unit of angle result is degree, this function must be called before all calculating angle function be called.
 * the degree unit is S16Q7.
 */
static inline void mdsp_angle_degree_unit_flag_init(void)
{
    MDSP_FUNC_ANGLE_ESTI_DEGREE_UNIT_FLAG_START();
}
/*if you want to config the unit of angle result is cycle, this function must be called before all calculating angle function be called.
 * the cycle unit is S16Q14.
*/
static inline void mdsp_angle_cycle_unit_flag_init(void)
{
    MDSP_FUNC_ANGLE_ESTI_CYCLE_UNIT_FLAG_START();
}

static inline void mdsp_hw_init(void)
{
    unsigned int reg = READ_REG(MDSP_RESET_CTRL);
    reg |= (1 << 7);
    WRITE_REG(MDSP_RESET_CTRL, reg);
    reg = READ_REG(MDSP_CLK_CTRL);
    reg |= (1 << 7);
    WRITE_REG(MDSP_CLK_CTRL, reg);
}

static inline void mdsp_angle_calc_init(void)
{
    mdsp_hw_init();
    mdsp_complex16_multi_mode0_init();
    mdsp_angle_degree_unit_flag_init(); //angle unit
}


#endif
