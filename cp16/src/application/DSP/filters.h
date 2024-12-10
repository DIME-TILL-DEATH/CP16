#ifndef __FILT_H__
#define __FILT_H__
#include "appdefs.h"

#define FILT_PI    3.14159265358979323846f


extern float pres_buf[5];
extern float Coeffs_b[5];

extern float filt_cos[5];
extern float filt_sin[5];
extern float filt_alpha[5];

extern float fil_lp_in[2];
extern float fil_hp_in[2];
extern float fil_lp_out[2];
extern float fil_hp_out[2];

extern float a0;
extern float a1;
extern float a01;
extern float a11;
extern float b1;
extern float b11;

extern float freq[5];
extern float freq1[5];
extern volatile float filt_q;

typedef enum
{
	PREAMP_LOW = 0,
	PREAMP_MID,
	PREAMP_HIGH
}preamp_param_t;

void preamp_param(preamp_param_t num, uint8_t val);

uint16_t convertLegacyFreq(uint8_t bandNum, uint8_t freqVal);

inline void SetLPF(float fCut)
{
    float w = 2.0f * 48000.0f;
    float Norm;
    fCut *= 2.0f * FILT_PI;
    Norm = 1.0f / (fCut + w);
    b1 = (w - fCut) * Norm;
    a0 = a1 = fCut * Norm;
}

inline void SetHPF(float fCut)
{
    float w = 2.0f * 48000.0f;
    float Norm;
    fCut *= 2.0f * FILT_PI;
    Norm = 1.0f / (fCut + w);
    a01 = w * Norm;
    a11 = -a01;
    b11 = (w - fCut) * Norm;
}

//inline void filt_ini(uint8_t num , uint8_t* adr , uint8_t* adr1)
//{
//  float w0;
//
//  switch (num){
//  case 0:case 1:freq1[num] = (int8_t)adr[num] + freq[num];break;
//  case 2:freq1[num] = (int8_t)adr[num]*2 + freq[num];break;
//  case 3:freq1[num] = (int8_t)adr[num]*10 + freq[num];break;
//  case 4:freq1[num] = (int8_t)adr[num]*50 + freq[num];break;
//  }
//
//  w0 = 2.0f * FILT_PI * freq1[num] / 48000.0f;
//  filt_sin[num] = sinf(w0);
//  filt_cos[num] = cosf(w0);
//  filt_q =  powf(200 - ((int8_t)adr1[num] + 100) , 3.0f)*(5.0f/powf(200.0f , 3.0f)) + 0.225f;
//  filt_alpha[num] = filt_sin[num]/2.0*filt_q;
//}

inline void filterInit(uint8_t bandNum, uint16_t freq, int8_t q)
{
	  float w0;
	  w0 = 2.0f * FILT_PI * freq / 48000.0f;
	  filt_sin[bandNum] = sinf(w0);
	  filt_cos[bandNum] = cosf(w0);
	  filt_q =  powf(200 - (q + 100) , 3.0f)*(5.0f/powf(200.0f , 3.0f)) + 0.225f;
	  filt_alpha[bandNum] = filt_sin[bandNum]/2.0*filt_q;
}

inline float filt_proc(float in , float* buf , float* coef)
{
  buf[0] = in;
  float out = coef[0]*buf[0] + coef[1]*buf[1] + coef[2]*buf[2] - coef[3]*buf[3] - coef[4]*buf[4];
  buf[2] = buf[1];
  buf[1] = buf[0];
  buf[4] = buf[3];
  buf[3] = out;
  return out;
}

inline float filt_lp(float in)
{
  fil_lp_in[0] = in;
  fil_lp_out[0] = fil_lp_in[0]*a0 + fil_lp_in[1]*a1 + fil_lp_out[1]*b1;
  fil_lp_in[1] = fil_lp_in[0];
  fil_lp_out[1] = fil_lp_out[0];
  return fil_lp_out[0];
}

inline float filt_hp(float in)
{
  fil_hp_in[0] = in;
  fil_hp_out[0] = fil_hp_in[0]*a01 + fil_hp_in[1]*a11 + fil_hp_out[1]*b11;
  fil_hp_in[1] = fil_hp_in[0];
  fil_hp_out[1] = fil_hp_out[0];
  return fil_hp_out[0];
}


extern float coeff_eq[];
extern float coeff_presen[];
extern float coeff_preamp[];

typedef enum
{
	PEAKING=0,
	LOW_SHELF,
	HIGH_SHELF
}band_type_t;

inline void set_filt(uint8_t band_num, uint8_t filt_gain, band_type_t band_type)
{
	float gain;
	if(filt_gain < 15) gain = -(15.0f - filt_gain);
	else gain = -(15.0f - filt_gain);

	float A = powf(10.0f, gain/40.0f);

	switch(band_type)
	{
		case PEAKING:
		{
			float a0 = 1.0f + filt_alpha[band_num]/A;
			coeff_eq[0 + band_num*5] = (1 + filt_alpha[band_num] * A)/a0;
			coeff_eq[1 + band_num*5] = (-2.0 * filt_cos[band_num])/a0;
			coeff_eq[2 + band_num*5] = (1 - filt_alpha[band_num] * A)/a0;
			coeff_eq[3 + band_num*5] = -coeff_eq[1 + band_num*5]; //-coeff_eq[1 + band_num*5];
			coeff_eq[4 + band_num*5] = -(1 - filt_alpha[band_num]/A)/a0; //-(1 - filt_alpha[band_num]/A)/a0;
			break;
		}
		case LOW_SHELF:
		{
			float a0 = (A+1) + (A-1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num];
			coeff_eq[0 + band_num*5] = A * ((A + 1) - (A - 1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[1 + band_num*5] = 2 * A * ((A - 1) - (A + 1)*filt_cos[band_num])/a0;
			coeff_eq[2 + band_num*5] = A * ((A + 1) - (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[3 + band_num*5] = -2 * ((A - 1) + (A + 1)*filt_cos[band_num])/a0;
			coeff_eq[4 + band_num*5] = ((A + 1) + (A - 1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num])/a0;
			break;
		}
		case HIGH_SHELF:
		{
			float a0 = (A+1) - (A-1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num];
			coeff_eq[0 + band_num*5] = A * ((A + 1) + (A - 1)*filt_cos[band_num] + 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[1 + band_num*5] = -2 * A * ((A - 1) + (A + 1)*filt_cos[band_num])/a0;
			coeff_eq[2 + band_num*5] = A * ((A + 1) + (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0;
			coeff_eq[3 + band_num*5] = 2 * ((A - 1) - (A + 1)*filt_cos[band_num])/a0;
			coeff_eq[4 + band_num*5] = ((A + 1) - (A - 1)*filt_cos[band_num] - 2*sqrt(A)*filt_alpha[band_num])/a0;
			break;
		}
	}
}


inline float proc_shelf(float in)
{
	pres_buf[0] = in;
	float out = Coeffs_b[0]*pres_buf[0] + Coeffs_b[1]*pres_buf[1] + Coeffs_b[2]*pres_buf[2] -
			    Coeffs_b[3]*pres_buf[3] - Coeffs_b[4]*pres_buf[4];

	pres_buf[2] = pres_buf[1];
	pres_buf[1] = pres_buf[0];
	pres_buf[4] = pres_buf[3];
	pres_buf[3] = out;
	return out;
}

//inline float biquad (float in)
//{
//	float out = filt_proc(in , (float*)eq_1_buf , (float*)eq_coef[0]);
//	out = filt_proc(out , (float*)eq_2_buf , (float*)eq_coef[1]);
//	out = filt_proc(out , (float*)eq_3_buf , (float*)eq_coef[2]);
//	out = filt_proc(out , (float*)eq_4_buf , (float*)eq_coef[3]);
//	out = filt_proc(out , (float*)eq_5_buf , (float*)eq_coef[4]);
//	return out;
//}


inline void filt_set(float gain , float* adr , float q_fac , float freq)
{
	float w0 = 2.0f*FILT_PI/48000.0f;
	float AA = powf(10.0f , gain/40.0f);
	float w00 = w0 * freq;
	float cos_ = cosf(w00);
	float sin_ = sinf(w00);
	float alfa = sin_/(2.0f*q_fac);
	float a0 = 1.0f + alfa/AA ;
	float gaine = powf(10.0f , 0.0f/20.0f)/a0;
	adr[2] = (1.0f - alfa*AA)*gaine;//--B2
	adr[1] = (-2.0f*cos_)*gaine;    //--B1
	adr[0] = (1.0f + alfa*AA)*gaine;//--B0
	adr[4] = -(1.0f - alfa/AA)/a0;//-A2
	adr[3] = -(-2.0f*cos_)/a0;  //-A1
}

inline void set_shelf(float gain)
{
	float A = powf(10.0f, gain /40.0f);
	float w0 = 2.0f*FILT_PI*5000.0f/48000.0f;
	float cos_ = cosf(w0);
	float sin_ = sinf(w0);
	float alfa = sin_/2.0f * vsqrt((A+1.0f/A)*(1.0f/0.3f/*slop*/-1.0f)+2.0f);
	float a0 = (A+1.0f) - (A-1.0f)*cos_ + 2.0f*vsqrt(A)*alfa;


	coeff_presen[0] = (A*((A+1.0f)+(A-1.0f)*cos_ + 2.0f*vsqrt(A)*alfa))/a0;
	coeff_presen[1] = (-2.0f*A*((A-1.0f)+(A+1.0f)*cos_))/a0;
	coeff_presen[2] = (A*((A+1.0f)+(A-1.0f)*cos_ - 2.0f*vsqrt(A)*alfa))/a0;
	coeff_presen[3] = -(2.0f*((A-1.0f)-(A+1.0f)*cos_))/a0;
	// ����� ������? � � PA � � RV �������� ������������, ������ �� ���
	coeff_presen[4] = -((A+1.0f) - (A-1.0f)*cos_ - 2.0f*vsqrt(A)*alfa)/a0;


	Coeffs_b[0] = (A*((A+1.0f)+(A-1.0f)*cos_ + 2.0f*vsqrt(A)*alfa))/a0;
	Coeffs_b[1] = (-2.0f*A*((A-1.0f)+(A+1.0f)*cos_))/a0;
	Coeffs_b[2] = (A*((A+1.0f)+(A-1.0f)*cos_ - 2.0f*vsqrt(A)*alfa))/a0;
	Coeffs_b[3] = (2.0f*((A-1.0f)-(A+1.0f)*cos_))/a0;
	Coeffs_b[4] = ((A+1.0f) - (A-1.0f)*cos_ - 2.0f*vsqrt(A)*alfa)/a0;
}
extern float coeff_preamp[];

inline void set_shelf_hi(float gain , float* adr , float slope , float freq)
{
	float A = powf(10.0f, gain /40.0f);
	float w0 = 2.0f*FILT_PI*freq/48000.0f;
	float cos_ = cosf(w0);
	float sin_ = sinf(w0);
	float alfa = sin_/2.0f * sqrtf((A+1.0f/A)*(1.0f/slope - 1.0f)+2.0f);
	float a0 = (A+1.0f) - (A-1.0f)*cos_ + 2.0f*sqrtf(A)*alfa;

	adr[2] = (A*((A+1.0f)+(A-1.0f)*cos_ - 2.0f*sqrtf(A)*alfa))/a0; // B2
	adr[1] = (-2.0f*A*((A-1.0f)+(A+1.0f)*cos_))/a0;                // B1
	adr[0] = (A*((A+1.0f)+(A-1.0f)*cos_ + 2.0f*sqrtf(A)*alfa))/a0; // B0
	adr[4] = -((A+1.0f) - (A-1.0f)*cos_ - 2.0f*sqrtf(A)*alfa)/a0;  // A2
	adr[3] = -(2.0f*((A-1.0f)-(A+1.0f)*cos_))/a0;  				// A1
}

inline void preamp_param(preamp_param_t num,uint8_t val)
{
	int8_t va = val;
	va += 64;
	switch(num){
	case PREAMP_LOW:if(va < 64)filt_set(va * (19.0f/63.0f) - 12.0f , coeff_preamp , 0.39f , 78.0f);
		   else filt_set((va - 63) * (2.0f/64.0f) + 7.0f , coeff_preamp , 0.39f , 78.0f);
	break;
	case PREAMP_MID:if(va < 64)
		   {
		       filt_set(0.0f , coeff_preamp + 20 , 0.49f , 460.0f);
			   filt_set(va * (6.0f/63.0f) - 6.0f, coeff_preamp + 25 , 1.1f , 604.0f);
		   }
		   else {
			   filt_set((va - 63) * (6.0f/64.0f), coeff_preamp + 20 , 0.49f , 460.0f);
			   filt_set((va - 63) * (3.0f/64.0f), coeff_preamp + 25 , 0.44f , 800.0f);
		   }
    break;
	case PREAMP_HIGH:if(va < 64)
		   {
			   set_shelf_hi(va * (16.11f/63.0f) , coeff_preamp + 15 , 0.3f , 6000.0f);
			   filt_set(va * -(4.0f/63.0f) , coeff_preamp + 5 , 0.71f , 602.0f);
			   filt_set(va * (8.0f/63.0f) - 2.0f , coeff_preamp + 10 , 0.3f , 1920.0f);
		   }
	       else {
	    	   set_shelf_hi((va - 63) * (9.764f/64.0f) + 16.11 , coeff_preamp + 15 , 0.3f , 6000.0f);
	    	   filt_set((va - 63) * -(1.0f/64.0f) - 4.0f, coeff_preamp + 5 , 0.71f , (127 - va) * (236.0f/63.0f) + 366.0f);
	    	   filt_set((va - 63) * (2.0f/64.0f) + 6.0f , coeff_preamp + 10 , 0.3f , 1920.0f);
	       }
	break;
	}
}
#endif /*__FILT_H__*/
