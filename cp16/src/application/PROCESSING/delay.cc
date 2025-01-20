#include "delay.h"

#include "appdefs.h"
#include "fx_sin.h"
#include "vdt/vdt.h"

#include "simple_filter.h"

#define DELAY_BUFF_SIZE 12000
int16_t del_buf[DELAY_BUFF_SIZE];

float del_accL;
float del_accR;
float del_acc1;
float del_acc2;
float del_acc_fed;

float del_fe = 0.5f;
float del_vol = 0.5f;

int32_t del_p;
int32_t del_p1 = 8000;
int32_t del_p2 = 4000;

float del_ph;
float del_wh = 0.0f;
float del_ra = 0.00001f;

float del_wet = 1.0f;

FirstOrderFilt del_lp1;
FirstOrderFilt del_lp2;
FirstOrderFilt del_lp3;
FirstOrderFilt del_hp1;
FirstOrderFilt del_hp2;
FirstOrderFilt del_hp3;

uint32_t direc = 0;
uint8_t hpf_fl = 0;
uint8_t lpf_fl = 0;

float del_in_contr1;
float wh;
uint32_t d_p;
float fr;

PassFilt delay_aafilt;

void DELAY_init()
{
	DELAY_mem_clear();
	delay_aafilt.SetLPF(10000);
}

void SetLPF_d(float fCut)
{
	del_lp1.SetLPF(fCut);
	del_lp2.SetLPF(fCut);
	del_lp3.SetLPF(fCut);
}

void SetHPF_d(float fCut)
{
	del_hp1.SetHPF(fCut);
	del_hp2.SetHPF(fCut);
	del_hp3.SetHPF(fCut);
}

#define DECIMATION_COEFFICIENT 2
uint8_t decimation_counter = 0;
void DELAY_process(float* inl , float* inr, float* outl, float* outr)
{
	float in = delay_aafilt.process(*inl, 3);
	if(decimation_counter == DECIMATION_COEFFICIENT-1)
	{
		/*
		 * Decimation. Bandwidth of wet signal reduced by 2 (48000/2/4 = 12kHz)
		 * HPF is mandatory for antialliasing!
		 */
		decimation_counter = 0;

		in = in * 32767.0f;

		del_ph += del_ra;
		if(del_ph > 1.0f) del_ph = 0.0f;
		wh = vector_sin[(uint32_t)(del_ph * 16384.0f)] * 0.00006103515625f;
		wh *= del_wh;
		fr = del_p;
		wh = wh + fr;
		d_p = wh;
		fr = wh - d_p;

	//----------------------------------------------------------------------------------------------------
		del_acc_fed = del_acc1 = del_buf[d_p % del_p1] * (1.0f - fr) + del_buf[(d_p + 1) % del_p1] * fr;
	//----------------------------------------------------------------------------------------------------

		del_acc_fed = del_hp3.process(del_acc_fed);
		del_acc1 = del_hp1.process(del_acc1);

		del_acc_fed = del_lp3.process(del_acc_fed);
		del_acc1 = del_lp1.process(del_acc1);

	//----------------------------------------------------------------------------------------------------
		del_accL = del_acc1;
		del_accR = del_acc1;

		d_p = del_p2 + d_p;
	//----------------------------------------------------------------------------------------------------
		del_acc2 = del_buf[d_p % del_p1] * (1.0f - fr) + del_buf[(d_p + 1) % del_p1] * fr;
	//----------------------------------------------------------------------------------------------------
		del_acc2 = del_hp2.process(del_acc2);
		del_acc2 = del_lp2.process(del_acc2);

		if((!del_p2) || (del_p2 == del_p1)) del_acc2 = 0.0f;
	//----------------------------------------------------------------------------------------------------
		del_accL += del_acc2 * 0.5f;
		del_accR += del_acc2 * 0.5f;

		del_acc_fed = (del_acc_fed * del_fe);
		del_buf[del_p] = in * del_vol  + del_acc_fed;

		if(++del_p >= del_p1) del_p = 0;
	}
	else decimation_counter++;

    *outl = *inl + (del_accL * del_wet) * 1/32767.0f;
    *outr = *inr + (del_accR * del_wet) * 1/32767.0f;

	if(!current_preset.delay.on)
	{
		if(del_vol > 0.0f) del_vol -= 0.01f;
		if(del_vol < 0.0f) del_vol = 0.0f;
	}
}

void DELAY_set_par(DELAY_param_type_t param_type, uint32_t val)
{
	switch(param_type)
	{
		case DELAY_MIX: del_wet = val * (1.0f/63.0f); break;
		case DELAY_FEEDBACK:
		{
			del_fe = val * (1.0f/127.0f);
			if(del_fe > 0.5f) del_fe = 0.5f;
			break;
		}
		case DELAY_LPF: SetLPF_d(powf(val, 2.0f) * (9000.0f/powf(127.0f, 2.0f)) + 1000.0f); break;
		case DELAY_HPF: SetHPF_d(powf(val, 2.0f) * (980.0f / powf(127.0f, 2.0f)) + 20.0f); break;
	//	case 9: del_wh = powf(va, 2.0f) * (510.0f/powf(127.0f, 2.0f)); break;
	//	case 10: del_ra = powf(va, 2.0f) * (0.0002f/powf(127.0f, 2.0f)) + 0.00001f; break;
		case DELAY_TIME:
		{
			del_p1 = (val * 48000.0f/1000.0f) / DECIMATION_COEFFICIENT;

			if(del_p1 > DELAY_BUFF_SIZE) del_p1 = DELAY_BUFF_SIZE;
			del_p2 = del_p1 / 2;

			kgp_sdk_libc::memset(del_buf, 0, DELAY_BUFF_SIZE);
			break;
		}
	}
}

void DELAY_mem_clear()
{
	kgp_sdk_libc::memset(del_buf, 0, DELAY_BUFF_SIZE);
}
