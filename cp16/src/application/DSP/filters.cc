#include "appdefs.h"
#include "filters.h"

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

float legacyCenterFreq[5] = {120.0, 360.0, 800.0, 2000.0, 6000.0};
//float freq1[5];
//volatile float filt_q;

uint16_t convertLegacyFreq(uint8_t bandNum, uint8_t freqVal)
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

void filterInit(uint8_t bandNum, uint16_t freq, int8_t q)
{
	  float w0;
	  w0 = 2.0f * FILT_PI * freq / 48000.0f;
	  filt_sin[bandNum] = sinf(w0);
	  filt_cos[bandNum] = cosf(w0);
	  float filt_q =  powf(200 - (q + 100) , 3.0f)*(5.0f/powf(200.0f , 3.0f)) + 0.225f;
	  filt_alpha[bandNum] = filt_sin[bandNum]/(2.0*filt_q);
}

void filterCalcCoefs(uint8_t band_num, int8_t filt_gain, band_type_t band_type)
{
	float gain = filt_gain;

	float A = powf(10.0f, gain/40.0f);

	switch(band_type)
	{
		case PEAKING:
		{
			float a0 = 1.0f + filt_alpha[band_num]/A;
			coeff_eq[0 + band_num*5] = (1 + filt_alpha[band_num] * A)/a0;	// b0
			coeff_eq[1 + band_num*5] = (-2.0 * filt_cos[band_num])/a0;		// b1
			coeff_eq[2 + band_num*5] = (1 - filt_alpha[band_num] * A)/a0;	// b2
			coeff_eq[3 + band_num*5] = -coeff_eq[1 + band_num*5]; 			// -a1
			coeff_eq[4 + band_num*5] = -(1 - filt_alpha[band_num]/A)/a0; 	// -a2
			break;
		}
		case LOW_SHELF:
		{
			float a0 = (A+1) + (A-1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num];
			coeff_eq[0 + band_num*5] = A * ((A + 1) - (A - 1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[1 + band_num*5] = 2 * A * ((A - 1) - (A + 1)*filt_cos[band_num])/a0;
			coeff_eq[2 + band_num*5] = A * ((A + 1) - (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[3 + band_num*5] = -(-2 * ((A - 1) + (A + 1)*filt_cos[band_num])/a0);
			coeff_eq[4 + band_num*5] = -(((A + 1) + (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0);
			break;
		}
		case HIGH_SHELF:
		{
			float a0 = (A+1) - (A-1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num];
			coeff_eq[0 + band_num*5] = A * ((A + 1) + (A - 1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[1 + band_num*5] = -2 * A * ((A - 1) + (A + 1)*filt_cos[band_num])/a0;
			coeff_eq[2 + band_num*5] = A * ((A + 1) + (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[3 + band_num*5] = -(2 * ((A - 1) - (A + 1)*filt_cos[band_num])/a0);
			coeff_eq[4 + band_num*5] = -(((A + 1) - (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0);
			break;
		}
	}
}
