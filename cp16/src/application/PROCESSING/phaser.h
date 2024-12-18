#ifndef SRC_APPLICATION_PHASER_H_
#define SRC_APPLICATION_PHASER_H_

#include "appdefs.h"
#include "FirststFilt.h"

extern float phas[];

class Phaser
{

	FirststFilt ph_hpf;

public:

	inline Phaser(void)
	{
		for(uint16_t i = 0 ; i < 14 ; i++)phas[i] = 0.0f;
	}
	inline float phaser(float in)
	{
		float in_ph = in;
		in_ph = ph_hpf.filt(in_ph);
		ph_phase1 += phas_rate ;
		if ( ph_phase1 >= 1.0f ) ph_phase1 = 0.0f;
		if( ph_phase1 < 0.5f)ph_phas_ = 2.0f*ph_phase1 * phas_wide;
		else ph_phas_ =  2.0f*(1.0f-ph_phase1) * phas_wide;

		phaser_mod = ph_phas_ * (0.99f/1.0f);
		phaser_mod =  phas_range - (phaser_mod * phaser_mod);
		if (phaser_mod > 0.99f)phaser_mod = 0.99f;
		if (phaser_mod < 0.0f)phaser_mod = 0.0f;

		phas[13] = accu = in_ph + accum_phas * phas_fed;
		phas[11] = accu * -phaser_mod + phas[12];
		phas[12] = phas[11] * phaser_mod + phas[13];
		for (int i = 0;i < phas_type;i++)
		{
			phas[13] = phas[11];
			accum_phas = phas[11] = phas[13] * -phaser_mod + phas[i];
			phas[i] = phas[11] * phaser_mod + phas[13];
		}
		if(ph_on)
		{
			if(phaser_vol < 1.0f)phaser_vol += 0.01f;
			else phaser_vol = 1.0f;
		}
		else {
			if(phaser_vol > 0.0f)phaser_vol -= 0.01f;
			if(phaser_vol < 0.0f)phaser_vol = 0.0f;
		}
		in += phas[11] * ph_wet/* * phaser_vol*/;
		return in;
	}
	void phaser_par(uint32_t val)
	{
		uint32_t va = val >> 8;
		val &= 0xff;
		switch(val & 0xff){
		case 1:ph_wet = va * (1.0f/127.0f);break;
		case 2:phas_rate = powf(va,2.0f) * (0.0001f/powf(127.0f,2.0f));break;
		case 3:phas_range = fast_powf((127.0f - va),0.1f) * (0.99f/fast_powf(127.0f,0.1f));break;
		case 4:phas_wide = va * (1.0f/127.0f);break;
		case 5:phas_fed = sqrt_f32_main(va) * (0.9f/sqrt_f32_main(127.0f));break;
		case 6:phas_type = ph_st[va];break;
		case 7:ph_hpf.SetHPF(va * (980.0f/127.0f) + 20.0f);break;
		}
	}

private:

	uint8_t ph_on;
	float ph_phase1 = 0.0f ;
	float accu;
	float accum_phas;
	float ph_phas_;
	float phas_fed= 0.0f;
	float phas_wide  = 0.7f;
	float phas_range  = 0.98f;
	float phas_rate = 0.00001f;
	float phaser_mod;
	float phaser_vol;
	int phas_type = 3;
	float ph_wet;
	uint8_t ph_st[3] = {3,5,7};
};

#endif /* SRC_APPLICATION_PHASER_H_ */
