#include "compressor.h"

#include "log.h"
#include "exp.h"


#define MIN_GAIN  0.00001f        // -100dB  This will help prevent evaluation of denormal numbers
#define LOG_2  0.693147f
#define LOG_10 2.302585f
#define samp_rate 46875.0f

using namespace vdt;
using namespace vdt::details;

inline float dB2rap(float dB) {
	return fast_expf((dB) * LOG_10 / 20.0f);
}
inline float rap2dB(float rap) {
	return 20.0f * fast_logf(rap) / LOG_10;
}

float lvolume;
float lvolume_db;
float thres_db;		// threshold
float knee;
float thres_mx;
float kpct;
float ratio;			// ratio
float kratio;			// ratio maximum for knee region
float eratio;			// dynamic ratio
float makeup;			// make-up gain
float makeuplin;
float outlevel;
float att, attl;
float rel, rell;
int ltimer, hold;
float lgain;
float lgain_old;
float lgain_t;
float lgain_time = 0.001f;
float lgain_t1;
float coeff_kratio;
float coeff_ratio;
float coeff_knee;
float coeff_kk;
float lpeak;
float efxout;

int tatt;			// attack time  (ms)
int trel;			// release time (ms)
int tratio;
int toutput;
int tthreshold;
int a_out;
int stereo;
int tknee;
int peak;
int clipping;

void comp_par(uint32_t val);
void compressor_init(void) {
	lvolume = 0.0f;
	lvolume_db = 0.0f;
	tthreshold = -24;
	tratio = 4;
	toutput = -10;
	tatt = 20;
	trel = 50;
	a_out = 1;
	stereo = 0;
	tknee = 30;
	lgain = 1.0f;
	lgain_old = 1.0f;
	lgain_t = 1.0f;
	ratio = 1.0;
	kpct = 0.0f;
	peak = 0;
	lpeak = 0.0f;
	rell = attl = 1.0f;

	ltimer = 0;
	hold = (int) (samp_rate * 0.00001f);  //12.5ms
	clipping = 0;

	uint8_t a[5] = { 10, 64, 20, 0, 0 };
	for (uint8_t i = 0; i < 5; i++)
		comp_par(i | (a[i] << 8));
}
void Compressor_Change(int np, int value) {
	switch (np) {
	case 0:
		tthreshold = value;
		thres_db = (float) tthreshold;
		break;  //implicit type cast int to float
	case 1:
		tratio = value;
		ratio = (float) tratio;
		break;
	case 2:
		toutput = value;
		break;
	case 3:
		tatt = value;
		att = samp_rate / (((float) value / 1000.0f) + samp_rate);
		attl = att;
		break;
	case 4:
		trel = 1;
		rel = samp_rate / (((float) 1 / 1000.0f) + samp_rate);
		rell = rel;
		break;
	case 5:
		tknee = value;
		kpct = (float) tknee / 100.1f;
		break; //knee expressed a percentage of range between thresh and zero dB
	}
	kratio = logf(ratio) / LOG_2;  //  Log base 2 relationship matches slope
	knee = -kpct * thres_db;

	coeff_kratio = 1.0 / kratio;
	coeff_ratio = 1.0 / ratio;
	coeff_knee = 1.0 / knee;

	coeff_kk = knee * coeff_kratio;

	thres_mx = thres_db + knee; //This is the value of the input when the output is at t+k
	makeup = -thres_db - knee / kratio + thres_mx / ratio;
	makeuplin = dB2rap(makeup);
	outlevel = dB2rap((float) toutput) * makeuplin * 0.05f;
}
float bu[512];
void compressor_change_preset(int dgui, int npreset) {
	for (int i = 0; i < 512; i++)
		bu[i] = 0.0f;
	//thres,ratio,out,attak,relise,aout,knee,stereo,peak
	int presets[7] = { -40, 12, 0, 0, 0, 0, 0 };
	for (int n = 0; n < 7; n++)
		Compressor_Change(n, presets[n]);
	//}

}
uint16_t bu_po;
volatile float bu_add = 0.0f;
//---------------------------------------------Compressor---------------------------------------------------------
float compr_out(float efxout) {
	bu_add -= bu[bu_po];
	bu[bu_po] = efxout * efxout;
	bu_add += bu[bu_po++];
	if (bu_po == 512)
		bu_po = 0;
	float bu_sum = sqrtf(bu_add / 512.0f);

	float ldelta = 0.0f;
	if (ltimer > hold) {
		lpeak *= 0.9998f;	//leaky peak detector.
		ltimer--; //keeps the timer from eventually exceeding max int & rolling over
	}
	ltimer++;
	if (lpeak < bu_sum) {
		lpeak = bu_sum;
		ltimer = 0;
	}
	if (lpeak > 20.0f)
		lpeak = 20.0f; //keeps limiter from getting locked up when signal levels go way out of bounds (like hundreds)
	ldelta = fabsf(lpeak);
	if (lvolume < 0.9f) {
		attl = att;
		rell = rel;
	} else {
		if (lvolume < 1.0f) {
			attl = att + ((1.0f - att) * (lvolume - 0.9f) * 10.0f);	//dynamically change attack time for limiting mode
			rell = rel / (1.0f + (lvolume - 0.9f) * 9.0f); //release time gets longer when signal is above limiting
		} else {
			attl = 1.0f;
			rell = rel * 0.1f;
		}
	}
	if (ldelta > lvolume)
		lvolume = attl * ldelta + (1.0f - attl) * lvolume;
	else
		lvolume = rell * ldelta + (1.0f - rell) * lvolume;

	lvolume_db = rap2dB(lvolume);

	if (lvolume_db < thres_db)
		lgain = outlevel;
	else {
		if (lvolume_db < thres_mx)  //knee region
				{
			eratio = 1.0f
					+ (kratio - 1.0f) * (lvolume_db - thres_db) * coeff_knee;
			lgain = outlevel
					* dB2rap(
							thres_db + (lvolume_db - thres_db) / eratio
									- lvolume_db);
		} else {
			lgain = outlevel
					* dB2rap(
							thres_db + coeff_kk
									+ (lvolume_db - thres_mx) * coeff_ratio
									- lvolume_db);
		}
	}
	if (lgain < MIN_GAIN)
		lgain = MIN_GAIN;
	lgain_t = 0.4f * lgain + 0.6f * lgain_old;
	if (lgain_t1 < lgain_t) {
		lgain_t1 += lgain_time;
		if (lgain_t1 > lgain_t)
			lgain_t1 = lgain_t;
	}
	if (lgain_t1 > lgain_t) {
		lgain_t1 -= lgain_time;
		if (lgain_t1 < lgain_t)
			lgain_t1 = lgain_t;
	}
	efxout *= lgain_t1;
	lgain_old = lgain;
	return efxout;
}
//------------------------------------------------Gate-----------------------------------------------------
#define ENV_TR 0.0001f
#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define CLOSING 4
float env = 0.0f;
float t_level;
float gate = 0.0f;
float a_rate;
float fs = 46875.0f;
float d_rate = 6.85959239e-05f;
float cut;
int state;
int hold_count = 0.0f;
int Pthreshold;
int Prange;
int Pattack;
int Pdecay;
int Phold;
void Gate_Change(int np, int value) {
	a_rate = 1000.0f / ((float) fs);
	switch (np) {
	case 0:
		Pthreshold = value;
		t_level = dB2rap((float) Pthreshold);
		break;
	case 1:
		Pdecay = value;
		d_rate = 1000.0f / ((float) Pdecay * fs);
		break;
	}
}
void gate_change_preset(void) {
	env = 0.0f;
	gate = 0.0f;
	state = CLOSED;
	hold_count = 0;
	int presets[2] = { -50, 1 };
	for (int n = 0; n < 2; n++)
		Gate_Change(n, presets[n]);
}
float gate_out(float efxout) {
	float sum;
	sum = fabsf(efxout);
	if (sum > env)
		env = sum;
	else
		env = 0.0f;  //sum * ENV_TR + env * (1.0f - ENV_TR);
	//------------------------------------------------------------------
	if (state == CLOSED) {
		if (env >= t_level)
			state = OPENING;
	} else {
		if (state == OPENING) {
			gate += a_rate;
			if (gate >= 1.0f) {
				gate = 1.0f;
				state = OPEN;
			}
		} else {
			if (state == OPEN) {
				if (env < t_level)
					state = CLOSING;
			} else {
				if (state == CLOSING) {
					gate -= d_rate;
					if (env >= t_level)
						state = OPENING;
					else {
						if (gate <= 0.0f) {
							gate = 0.0f;
							state = CLOSED;
						}
					}
				}
			}
		}
	}
	return gate;
}
void gate_par(uint32_t val) {
	uint32_t va = val >> 8;
	val &= 0xff;

	switch (val & 0xff) {
	case 0:
		Gate_Change(0, -((31 - va) * (70.0f / 31.0f) + 17.0f));
		break;
	case 1:
		Gate_Change(1, va * (1270.0f / 31.0f) + 100);
		break;
	}
}

void comp_par(uint32_t val) {
	uint32_t va = val >> 8;
	val &= 0xff;
	switch (val & 0xff) {
	case 0:
		Compressor_Change(0, -(sqrtf(va) * (50.0f / sqrtf(31.0f))));
		break;
	case 1:
		Compressor_Change(1, va * (20.0f / 127.0f) + 2.0f);
		break;
	case 2:
		Compressor_Change(2, va * (20.0f / 31.0f));
		break;
	case 3:
		lgain_time = powf((127 - va), 6.0f) * (20.0f / powf(127.0f, 6.0f))
				+ 0.03f;
		break;
	case 4:
		Compressor_Change(5, va);
		break;
	}
}
