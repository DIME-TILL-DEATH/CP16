//#include "appdefs.h"
//#include "init.h"
#include "math.h"
#include <vdt/vdt.h>
#include "fx_sin.h"
//
float tr_phase;
float tr_phase1;
float tr_phase2;
float tr_rate = 0.00001f;
float tr_gen1;
float tr_gen2;
float trem_vlt = 1.0f;
float trem_vrt = 1.0f;
float trem_vl = 1.0f;
float trem_vr = 1.0f;
float trem_int;
float trem_dist = 0.01f;
uint32_t tr_typ;
uint32_t tr_lfo;
float tri_form;
float sq_form;

inline float dist_sin( float in)
{
	return ((trem_dist + 1.0f) * (in / (fabsf(in) + trem_dist)) + 1.0f) * 0.5f;
}
volatile uint32_t fl_tr1;
void trem(void)
{
	tr_phase += tr_rate;
	if(tr_phase > 1.0f)tr_phase = 0.0f;

	tr_phase1 = tr_phase + 0.5f;
	if(tr_phase1 > 1.0f)tr_phase1 = tr_phase1 - 1.0f;

	switch(tr_lfo){
	case 0:tr_gen1 = dist_sin((vector_sin[(uint32_t)(tr_phase * 16384.0f)] * 0.00006103515625f) * 2.0f - 1.0f);
	       tr_gen2 = dist_sin((vector_sin[(uint32_t)(tr_phase1 * 16384.0f)] * 0.00006103515625f) * 2.0f - 1.0f);
		break;
	case 1:if(tr_phase < sq_form)tr_gen1 = 0.0f;else tr_gen1 = 1.0f;
	       if(tr_phase1 < sq_form)tr_gen2 = 0.0f;else tr_gen2 = 1.0f;
		break;
	case 2:if(tri_form < 0.5f)
           {
			  float sq_temp = sq_form;
			  sq_temp = 1.0f - tri_form * 1.5f;
	          tr_gen1 = fast_powf(1.0f - tr_phase, sq_temp);
	          tr_gen2 = fast_powf(1.0f - tr_phase1, sq_temp);
           }
           else   {
        	  float sq_temp = sq_form;
        	  sq_temp = 1.0f - (tri_form - 0.5f) * 1.5f;
	          tr_gen1 = fast_powf(tr_phase, sq_temp);
	          tr_gen2 = fast_powf(tr_phase1, sq_temp);
           }
		break;
	}

	trem_vlt = 1.5f - tr_gen1 * trem_int;
	trem_vrt = 1.5f - tr_gen2 * trem_int;

	if(1) //prog_data[trem_on])
	{
		if(!tr_typ)trem_vrt = trem_vlt;
		if(!tr_lfo)
		{
			trem_vl = trem_vlt;
			trem_vr = trem_vrt;
		}
		else {
			if(trem_vr < trem_vrt)trem_vr += 0.01f;
			else {
				if(trem_vr > trem_vrt)trem_vr -= 0.01f;
				if(trem_vr < 0.0f)trem_vr = 0.0f;
			}
			if(trem_vl < trem_vlt)trem_vl += 0.01f;
			else {
				if(trem_vl > trem_vlt)trem_vl -= 0.01f;
				if(trem_vl < 0.0f)trem_vl = 0.0f;
			}
		}
	}
	else trem_vr = trem_vl = 1.0f;
}

uint32_t ra_temp;
void trem_par(uint32_t val)
{
	uint32_t va = val >> 8;
	val &= 0xff;
	switch(val & 0xff){
	case 1:trem_int = sqrt(va) * (1.5f/sqrt(127.0f));break;
	case 2:tr_rate = powf(va,2.0f) * (0.0003/powf(127.0f,2.0f)) + 0.000001f;break;
	case 3:tr_lfo = va;break;
	case 4:trem_dist = powf((127.0f - va),8.0f) * (20.0f / powf(127.0f,8.0f)) + 0.0005f;
	       tri_form = va * (1.0f/127.0f);sq_form = va * (0.8f/127.0f) + 0.1f;break;
	case 5:tr_typ = va;break;
	case 6:ra_temp = va << 8;break;
	case 7:ra_temp |= va;tr_rate = 1.0f / 48.0f / ra_temp;break;
	}
}

