#include "phaStagesser.h"

#include <vdt/vdt.h>
#include "fx_sin.h"

float phaser_mem[16];

uint8_t ph_on;
float ph_phase1 = 0.0f ;
float accu;
float accum_phas;
float ph_phas_;
float phas_fed = 0.0f;
float phas_wide  = 0.7f;
float phas_center  = 0.98f;
float phas_rate = 0.00001f;
float phaser_mod;
float phaser_vol;
int phas_type = 3;
float ph_wet;
uint8_t ph_st[3] = {3, 5, 7};

void PHASER_process(float* in, float* out)
{
	float in_smpl = *in;

	ph_phase1 += phas_rate;

	if ( ph_phase1 >= 1.0f ) ph_phase1 = 0.0f;

	if( ph_phase1 < 0.5f) ph_phas_ = 2.0f*ph_phase1 * phas_wide;
	else ph_phas_ =  2.0f*(1.0f-ph_phase1) * phas_wide;

	phaser_mod = ph_phas_ * (0.99f/1.0f);
	phaser_mod =  phas_center - (phaser_mod * phaser_mod);

	if (phaser_mod > 0.99f) phaser_mod = 0.99f;
	if (phaser_mod < 0.0f) phaser_mod = 0.0f;

	phaser_mem[13] = accu = in_smpl + accum_phas * phas_fed;
	phaser_mem[11] = accu * -phaser_mod + phaser_mem[12];
	phaser_mem[12] = phaser_mem[11] * phaser_mod + phaser_mem[13];

	for (int i = 0;i < phas_type; i++)
	{
		phaser_mem[13] = phaser_mem[11];
		accum_phas = phaser_mem[11] = phaser_mem[13] * -phaser_mod + phaser_mem[i];
		phaser_mem[i] = phaser_mem[11] * phaser_mod + phaser_mem[13];
	}

	if(ph_on)
	{
		if(phaser_vol < 1.0f) phaser_vol += 0.01f;
		else phaser_vol = 1.0f;
	}
	else
	{
		if(phaser_vol > 0.0f) phaser_vol -= 0.01f;
		if(phaser_vol < 0.0f) phaser_vol = 0.0f;
	}

	*out += phaser_mem[11] * ph_wet;
}

void PHASER_set_par(PHASER_param_type_t param_type, uint32_t val)
{
	switch(param_type)
	{
	case PHASER_MIX: ph_wet = val * (1.0f/127.0f);break;
	case PHASER_RATE: phas_rate = powf(val, 2.0f) * (0.0001f/powf(127.0f, 2.0f));break;
	case PHASER_CENTER: phas_center = fast_powf((127.0f - val), 0.1f) * (0.99f/fast_powf(127.0f, 0.1f));break;
	case PHASER_WIDTH: phas_wide = val * (1.0f/127.0f);break;
	case PHASER_FEEDBACK: phas_fed = sqrt(val) * (0.9f/sqrt(127.0f));break;
	case PHASER_STAGES: phas_type = ph_st[val]; break;
	}
}
