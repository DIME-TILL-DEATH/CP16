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

float freq[5] = {120.0, 360.0, 800.0, 2000.0, 6000.0};
float freq1[5];
volatile float filt_q;

uint16_t convertLegacyFreq(uint8_t bandNum, uint8_t freqVal)
{
	uint16_t convertedFreq = 20;
	switch(bandNum)
	{
	case 0:case 1:convertedFreq = (int8_t)freqVal + freq[bandNum];break;
	case 2:convertedFreq = (int8_t)freqVal*2 + freq[bandNum];break;
	case 3:convertedFreq = (int8_t)freqVal*10 + freq[bandNum];break;
	case 4:convertedFreq = (int8_t)freqVal*50 + freq[bandNum];break;
	}
	return convertedFreq;
}

