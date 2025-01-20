#ifndef __FILT_H__
#define __FILT_H__
#include "appdefs.h"

#define FILT_PI 3.14159265358979323846f

extern float pres_buf[5];
extern float Coeffs_b[5];


extern float coeff_presen[];
extern float coeff_preamp[];

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


typedef enum
{
	PREAMP_LOW = 0,
	PREAMP_MID,
	PREAMP_HIGH
}preamp_param_t;

void preamp_param(preamp_param_t num, uint8_t val);


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
