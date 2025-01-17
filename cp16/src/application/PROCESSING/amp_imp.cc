/*
 * amp_imp.cc
 *
 *  Created on: Oct 29, 2024
 *      Author: dime
 */

#include "../PROCESSING/amp_imp.h"

#include "../PROCESSING/sound_processing.h"
#include "ADAU/adau1701.h"

arm_fir_instance_f32 pa_instance;

float __CCM_BSS__ coeff_pa[TAPS_PA_FIR];
float __CCM_BSS__ pa_state[TAPS_PA_FIR + BLOCK_SIZE -1];

void pa_init()
{
	arm_fir_init_f32(&pa_instance, TAPS_PA_FIR, coeff_pa, pa_state, BLOCK_SIZE);
}

void pa_update_coefficients(uint8_t amp_type)
{
	int a = TAPS_PA_FIR - 1;

	switch(amp_type)
	{
		case 0:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = PP_6L6[i]; break;
		case 1:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = PP_EL34[i]; break;
		case 2:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = SE_6L6[i]; break;
		case 3:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = SE_EL34[i]; break;
		case 4:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = tc_1[i]; break;
		case 5:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = fender[i]; break;
		case 6:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = jcm800[i]; break;
		case 7:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = lc50[i]; break;
		case 9:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = mes_mod[i]; break;
		case 10:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = mes_vint[i]; break;
		case 11:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = Pr0_Re0_5150[i]; break;
		case 12:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = Pr5_Re5_5150[i]; break;
		case 13:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = Pr8_Re7_5150[i]; break;
		case 14:for(int i = 0 ; i < TAPS_PA_FIR ; i++) coeff_pa[a--] = Pr9_Re8_5150[i]; break;
	}
}

