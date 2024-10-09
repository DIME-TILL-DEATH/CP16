#include "appdefs.h"
#include "filt.h"



float eq_coef[5][5];

float eq_1_buf[5];
float eq_1_coef[5];
float eq_2_buf[5];
float eq_2_coef[5];
float eq_3_buf[5];
float eq_3_coef[5];
float eq_4_buf[5];
float eq_4_coef[5];
float eq_5_buf[5];
float eq_5_coef[5];
float pres_buf[5];
float Coeffs_b[5];

float filt_cos[5];
float filt_sin[5];
float filt_alpha[5];

float fil_lp_in[2];
float fil_hp_in[2];
float fil_lp_out[2];
float fil_hp_out[2];

float a0;
float a1;
float a01;
float a11;
float b1;
float b11;

float freq[5] = {120.0,360.0,800.0,2000.0,6000.0};
float freq1[5];
volatile float filt_q;

