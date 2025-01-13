#include "tremolo.h"

#include <vdt/vdt.h>
#include "fx_sin.h"

float tr_phase;
float tr_rate = 0.00001f;
float tr_gen;

float trem_vl = 1.0f;

float trem_int;
float trem_dist = 0.01f;
uint32_t tr_lfo;

inline float dist_sin( float in)
{
	return ((trem_dist + 1.0f) * (in / (fabsf(in) + trem_dist)) + 1.0f) * 0.5f;
}
volatile uint32_t fl_tr1;

void TREMOLO_step(void)
{
	tr_phase += tr_rate;
	if(tr_phase > 1.0f) tr_phase = 0.0f;


	tr_gen = dist_sin((vector_sin[(uint32_t)(tr_phase * 16384.0f)] * 0.00006103515625f) * 2.0f - 1.0f);

	trem_vl= 1.5f - tr_gen * trem_int;
}

void TREMOLO_set_par(TREMOLO_param_type_t param_type, uint32_t val)
{
	switch(param_type)
	{
	case TREMOLO_DEPTH: trem_int = sqrt(val) * (1.5f/sqrt(127.0f));break;
	case TREMOLO_RATE: tr_rate = powf(val, 2.0f) * (0.0003/powf(127.0f, 2.0f)) + 0.000001f; break;
	case TREMOLO_FORM: trem_dist = powf((127.0f - val), 8.0f) * (20.0f / powf(127.0f, 8.0f)) + 0.0005f;
	}
}

float TREMOLO_get_volume(void)
{
	if(current_preset.tremolo.on)
	{
		return trem_vl;
	}
	else
	{
		return 1.0f;
	}
}
