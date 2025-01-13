#include "chorus.h"

#include <vdt/vdt.h>
#include "fx_sin.h"

//__attribute__((section(".dtcm_data"))) float memchor[2048];
//__attribute__((section(".dtcm_data"))) float det_w[2048];

float memchor[2048];
float det_w[2048];

float phase = 0.0f ;
float phase1;
float accum_chorL;
int poschor;
float chor_rate = 50.0f;
float chor_vol = 1.0f;
float chor_with;

float vals[3];

float ch_dry;
float ch_wet;

inline void Sine (void)
{
	vals[0] = vector_sin[(uint32_t)(phase * 16383.0f)] * 0.00006103515625f;
	vals[1] = vector_sin[((uint32_t)(phase * 16383.0f) + 5461) & 0x3fff] * 0.00006103515625f;
	vals[2] = vector_sin[((uint32_t)(phase * 16383.0f) + 10922) & 0x3fff] * 0.00006103515625f;
}

inline void Tri (float in)
{
	if(in < 1.0f)
	{
		vals[0] = in;
		vals[1] = 1.0f - vals[0];
	}
	else
	{
		vals[0] = 2.0f - in;
		vals[1] = 1.0f - vals[0];
	}
}

float fil_hp_coCH[3];
float fil_hp_inCH[2];
float fil_hp_outCH[2];

float fil_hp1_coCH[3];
float fil_hp1_inCH[2];
float fil_hp1_outCH[2];

float fil_lp1_coCH[3];
float fil_lp1_inCH[2];
float fil_lp1_outCH[2];


void SetHPF_ch(float fCut)
{
    float w = 2.0f * 48000.0f;
    float Norm;
    fCut *= 2.0f * VDT_PI;
    Norm = 1.0f / (fCut + w);
    fil_hp_coCH[0] = w * Norm;
    fil_hp_coCH[1] = -fil_hp_coCH[0];
    fil_hp_coCH[2] = (w - fCut) * Norm;
}
void SetLPF_ch_mid_side(void)
{
    float w = 2.0f * 48000.0f;
    float Norm;
    float fCut = 3600.0f;
    fCut *= 2.0f * VDT_PI;
    Norm = 1.0f / (fCut + w);
    fil_lp1_coCH[2] = (w - fCut) * Norm;
    fil_lp1_coCH[0] = fil_lp1_coCH[1] = fCut * Norm;
}
void SetHPF_ch_mid_side(void)
{
    float w = 2.0f * 48000.0f;
    float Norm;
    float fCut = 250.0f;
    fCut *= 2.0f * VDT_PI;
    Norm = 1.0f / (250.0f + w);
    fil_hp1_coCH[0] = w * Norm;
    fil_hp1_coCH[1] = -fil_hp1_coCH[0];
    fil_hp1_coCH[2] = (w - 250.0f) * Norm;
}

void CHORUS_process(float* in, float* out)
{
	float in_sample = *in;

	fil_hp_inCH[0] = in_sample;
	fil_hp_outCH[0] = fil_hp_inCH[0]*fil_hp_coCH[0] + fil_hp_inCH[1]*fil_hp_coCH[1] + fil_hp_outCH[1]*fil_hp_coCH[2];
	fil_hp_inCH[1] = fil_hp_inCH[0];
	fil_hp_outCH[1] = fil_hp_outCH[0];
	in_sample = fil_hp_outCH[0];

	phase += chor_rate ;
	if ( phase > 1.0f ) phase = 0.0f;

	memchor[poschor] = in_sample;

	uint32_t pos;
	float pos_fl;
	float fra;

	phase1 = phase * 2.0f;
	Tri(phase1);
	pos_fl = vals[0] * chor_with;
	pos = pos_fl;
	fra = pos_fl - pos;

	accum_chorL = memchor[(poschor + pos) & 0x7ff] * (1.0f - fra) + memchor[(poschor + pos + 1) & 0x7ff] * fra;
	accum_chorL *= chor_vol * ch_wet;

	if(!current_preset.chorus.on)
	{
		if(chor_vol > 0.0f) chor_vol -= 0.0005f;
		else chor_vol = 0.0f;
		ch_dry = 1.0f;
	}
	else if(chor_vol < 1.0f) chor_vol += 0.0005f;

	*out = *out * ch_dry + accum_chorL;

	if(!poschor) poschor = 2048;
	poschor -= 1;
}

int cho_ra;
int cho_wh;
void chor_wi(int va)
{
	chor_with = powf(cho_wh+5, 2.0f) * (1023.0f/powf(127.0f,2.0f));
}

void chor_ra(int va)
{
	chor_rate = powf(va,2.0f) * (0.0002f/powf(127.0f,2.0f)) + 0.0000005f;
	chor_wi(cho_wh);
}

void CHORUS_set_par(CHORUS_param_type_t param_type, uint32_t val)
{
	switch(param_type)
	{
	case CHORUS_MIX:
	{
		if(val <= 63.0f)
		{
			ch_dry = 1.0f;
			ch_wet = val * (1.0f/63.0f);
		}
		else
		{
			ch_wet = 1.0f;
			ch_dry = (127.0f - val) * (1.0f/64.0);
		}
		break;
	}
	case CHORUS_RATE:cho_ra = val; chor_ra(cho_ra);break;
	case CHORUS_WIDTH: cho_wh = val; chor_wi(cho_wh);break;
	case CHORUS_HPF: SetHPF_ch(val * (980.0f/127.0f) + 20.0f);break;
	}
}
