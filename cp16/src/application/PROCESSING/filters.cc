#include "../PROCESSING/filters.h"

#include "appdefs.h"

float __CCM_BSS__ pres_buf[5];
float __CCM_BSS__ Coeffs_b[5];

float __CCM_BSS__ filt_cos[5];
float __CCM_BSS__ filt_sin[5];
float __CCM_BSS__ filt_alpha[5];

float __CCM_BSS__ fil_lp_in[2];
float __CCM_BSS__ fil_hp_in[2];
float __CCM_BSS__ fil_lp_out[2];
float __CCM_BSS__ fil_hp_out[2];

float __CCM_BSS__ a0;
float __CCM_BSS__ a1;
float __CCM_BSS__ a01;
float __CCM_BSS__ a11;
float __CCM_BSS__ b1;
float __CCM_BSS__ b11;




