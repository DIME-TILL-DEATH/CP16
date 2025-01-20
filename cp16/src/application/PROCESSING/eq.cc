/*
 * eq.cc
 *
 *  Created on: Jan 17, 2025
 *      Author: dime
 */


#include "eq.h"

const float legacyCenterFreq[] = {120.0, 360.0, 800.0, 2000.0, 6000.0};

ParametricEq::ParametricEq(eq_t* eq_data_ptr)
{
	eq_data = eq_data_ptr;
	arm_biquad_cascade_df1_init_f32(&eq_instance, EQ_STAGES, coeff, stage);
}

uint16_t ParametricEq::convertLegacyFreq(uint8_t bandNum, uint8_t freqVal)
{
	uint16_t convertedFreq = 20;
	switch(bandNum)
	{
	case 0:case 1:convertedFreq = (int8_t)freqVal + legacyCenterFreq[bandNum];break;
	case 2:convertedFreq = (int8_t)freqVal*2 + legacyCenterFreq[bandNum];break;
	case 3:convertedFreq = (int8_t)freqVal*10 + legacyCenterFreq[bandNum];break;
	case 4:convertedFreq = (int8_t)freqVal*50 + legacyCenterFreq[bandNum];break;
	}
	return convertedFreq;
}

void ParametricEq::recalcCoefficients()
{
	for (uint8_t i = 0; i < EQ_BANDS_COUNT; i++)
	{
		filterInit(i, eq_data->freq[i], eq_data->Q[i]);
		filterCalcCoefs(i, eq_data->gain[i], (ParametricEq::band_type_t)eq_data->band_type[i]);
	}

	hpf.SetHPF(eq_data->hp_freq);
	lpf.SetLPF(eq_data->lp_freq);
}

void ParametricEq::filterInit(uint8_t bandNum, uint16_t freq, int8_t q)
{
	  float w0 = 2.0f * FILT_PI * freq / 48000.0f;
	  filt_sin[bandNum] = sinf(w0);
	  filt_cos[bandNum] = cosf(w0);

	  float filt_q =  powf(200 - (q + 100) , 3.0f)*(5.0f/powf(200.0f , 3.0f)) + 0.225f;
	  filt_alpha[bandNum] = filt_sin[bandNum]/(2.0*filt_q);
}

void ParametricEq::filterCalcCoefs(uint8_t band_num, int8_t filt_gain, band_type_t band_type)
{
	float gain = filt_gain;

	float A = powf(10.0f, gain/40.0f);

	switch(band_type)
	{
		case PEAKING:
		{
			float a0 = 1.0f + filt_alpha[band_num]/A;
			coeff[0 + band_num*5] = (1 + filt_alpha[band_num] * A)/a0;	// b0
			coeff[1 + band_num*5] = (-2.0 * filt_cos[band_num])/a0;		// b1
			coeff[2 + band_num*5] = (1 - filt_alpha[band_num] * A)/a0;	// b2
			coeff[3 + band_num*5] = -coeff[1 + band_num*5]; 			// -a1
			coeff[4 + band_num*5] = -(1 - filt_alpha[band_num]/A)/a0; 	// -a2
			break;
		}
		case LOW_SHELF:
		{
			float a0 = (A+1) + (A-1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num];
			coeff[0 + band_num*5] = A * ((A + 1) - (A - 1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff[1 + band_num*5] = 2 * A * ((A - 1) - (A + 1)*filt_cos[band_num])/a0;
			coeff[2 + band_num*5] = A * ((A + 1) - (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff[3 + band_num*5] = -(-2 * ((A - 1) + (A + 1)*filt_cos[band_num])/a0);
			coeff[4 + band_num*5] = -(((A + 1) + (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0);
			break;
		}
		case HIGH_SHELF:
		{
			float a0 = (A+1) - (A-1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num];
			coeff[0 + band_num*5] = A * ((A + 1) + (A - 1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff[1 + band_num*5] = -2 * A * ((A - 1) + (A + 1)*filt_cos[band_num])/a0;
			coeff[2 + band_num*5] = A * ((A + 1) + (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff[3 + band_num*5] = -(2 * ((A - 1) - (A + 1)*filt_cos[band_num])/a0);
			coeff[4 + band_num*5] = -(((A + 1) - (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0);
			break;
		}
	}
}

void ParametricEq::process(float* in, float* out, int processing_block_size)
{
	if(eq_data->parametric_on)
	{
		arm_biquad_cascade_df1_f32(&eq_instance, in, out, processing_block_size);
	}
	else
	{
		kgp_sdk_libc::memcpy(out, in, processing_block_size);
	}

	if(eq_data->lp_on)
		for(int i = 0; i< processing_block_size; i++)
			out[i] = lpf.process(in[i]);

	if(eq_data->hp_on)
			for(int i = 0; i< processing_block_size; i++)
				out[i] = hpf.process(in[i]);
}
