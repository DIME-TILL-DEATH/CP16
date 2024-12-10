/*
 * sound_processing.cc
 *
 *  Created on: Oct 16, 2024
 *      Author: dime
 */

#include "sound_processing.h"

#include "preset.h"

#include "ADAU/adau1701.h"

#include "fades.h"
#include "DSP/filters.h"
#include "compressor.h"
#include "amp_imp.h"
#include "Reverb/reverb.h"

processing_func_ptr processing_library[NUM_MONO_MODULE_TYPES];
processing_func_ptr processing_stage[NUM_MONO_MODULE_TYPES];

void __RAMFUNC__ bypass_processing_stage(float* in_samples, float* out_samples);
//------MONO OUT-----------------------------------------------------------
void __RAMFUNC__ compressor_processing_stage(float* in_samples, float* out_samples);
void __RAMFUNC__ preamp_processing_stage(float* in_samples, float* out_samples);
void __RAMFUNC__ pa_processing_stage(float* in_samples, float* out_samples);
void __RAMFUNC__ ir_processing_stage(float* in_samples, float* out_samples);
void __RAMFUNC__ hpf_processing_stage(float* in_samples, float* out_samples);
void __RAMFUNC__ eq_processing_stage(float* in_samples, float* out_samples);
void __RAMFUNC__ lpf_processing_stage(float* in_samples, float* out_samples);
void __RAMFUNC__ gate_processing_stage(float* in_samples, float* out_samples);
//------STEREO OUT-----------------------------------------------------------
void __RAMFUNC__ early_processing_stage(float* in_samples, float* out_l_samples, float* out_r_samples);

uint8_t ir_send_position = 3;

inline float soft_clip_amp(float in);
inline float soft_clip_pre(float in);
inline float dc_block(float in);
inline float out_clip(float in);

processing_params_t processing_params;

volatile  float vol_ind_vector[3];

float __CCM_BSS__ coeff_preamp[preamp_stage * 5];
float __CCM_BSS__ stage_preamp[preamp_stage * 4];

float __CCM_BSS__ coeff_eq[eq_stage * 5];
float __CCM_BSS__ stage_eq[eq_stage * 4];

float __CCM_BSS__ coeff_presen[presence_stage * 5];
float __CCM_BSS__ stage_presen[presence_stage * 4];

float __CCM_BSS__ gate_buf[block_size];

arm_biquad_casd_df1_inst_f32 preamp_instance;
arm_biquad_casd_df1_inst_f32 eq_instance;
arm_biquad_casd_df1_inst_f32 presence_instance;

uint16_t pwm = 0;
uint16_t pwm_count;
float pwm_count_f = 0.0f;
uint16_t pwm_count_po;

volatile uint8_t rev_en = 0;
volatile uint8_t rev_en1 = 0;

void DSP_init()
{
	gate_change_preset();
	compressor_init();
	compressor_change_preset(0, 0);

	processing_library[BYPASS] = bypass_processing_stage;
	processing_library[CM] = compressor_processing_stage;
	processing_library[PR] = preamp_processing_stage;
	processing_library[PA] = pa_processing_stage;
	processing_library[IR] = ir_processing_stage;
	processing_library[HP] = hpf_processing_stage;
	processing_library[EQ] = eq_processing_stage;
	processing_library[LP] = lpf_processing_stage;
	processing_library[NG] = gate_processing_stage;

	for(int i = 0; i < NUM_MONO_MODULE_TYPES; i++)
	{
		processing_stage[i] = processing_library[i];
	}

	pa_init();


	arm_biquad_cascade_df1_init_f32(&eq_instance, eq_stage, coeff_eq, stage_eq);
	arm_biquad_cascade_df1_init_f32(&presence_instance, presence_stage , coeff_presen, stage_presen);
	arm_biquad_cascade_df1_init_f32(&preamp_instance, preamp_stage , coeff_preamp, stage_preamp);

	processing_params.pream_vol = 1.0f;
	processing_params.amp_vol = 1.0f;
	processing_params.amp_slave = 1.0f;
	processing_params.preset_volume = 1.0f;
	processing_params.ear_vol = 0.0f;

	processing_params.impulse_avaliable = 0;
}

void DSP_set_module_to_processing_stage(DSP_mono_module_type_t module_type, uint8_t stage_num)
{
	processing_stage[stage_num] = processing_library[module_type];
}

//================================Main processing routine=================================
uint8_t __CCM_BSS__ frame_part=0;
uint8_t __CCM_BSS__ aux_samples[block_size * 2][4];
uint8_t __CCM_BSS__ aux_smpl_rd_ptr=0;
uint8_t __CCM_BSS__ aux_smpl_wr_ptr=0;

bool double_buf_ptr = 0;
extern "C" void SPI2_IRQHandler()
{
	SPI_I2S_ClearITPendingBit(adau_i2s_spi_ext, SPI_I2S_IT_RXNE);

	if(frame_part==0)
	{
		adau_dma_transmit(DSP_AUXIN_ADDRESS, &aux_samples[aux_smpl_rd_ptr][0], 4);
		aux_smpl_rd_ptr++;
		if(aux_smpl_rd_ptr == block_size * 2) aux_smpl_rd_ptr=0;
	}

	if(frame_part==0) frame_part=3;
	else frame_part--;
}

float __CCM_BSS__ di_samples[block_size];
float __CCM_BSS__ ir_samples[block_size];

float __CCM_BSS__ processing_samples[block_size];

float __CCM_BSS__ mon_sample[block_size]; // pre IR
float __CCM_BSS__ out_sampleL[block_size];
float __CCM_BSS__ out_sampleR[block_size];

int32_t __CCM_BSS__ ccl[block_size];
int32_t __CCM_BSS__ ccr[block_size];


extern "C" void DMA1_Stream3_IRQHandler()
{
	GPIO_SetBits(GPIOB,GPIO_Pin_7);

	uint8_t dma_ht_fl = 0;
	//--------------------------------------------------------Start---------------------
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_HTIF3))
	{
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_HTIF3);
		dma_ht_fl = 0;
	}
	else
	{
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
		dma_ht_fl = 1;
	}

	uint8_t base_address = dma_ht_fl * block_size;
	//---------------------------Input sample convert------------------------------------
	for(uint8_t i = 0 ; i < block_size; i++)
	{
		di_samples[i] = (int32_t)(ror16(adc_data[base_address + i].left.sample)) >> 8;
		ir_samples[i] = (int32_t)(ror16(adc_data[base_address + i].right.sample)) >> 8;


		processing_samples[i] = dc_block(di_samples[i]) * 0.000000119f;
		ir_samples[i] *= 0.000000119f;

		if(current_preset.gate.on) gate_buf[i] = gate_out(processing_samples[i]);
		else gate_buf[i] = 1.0f;

		di_samples[i] *= get_fade_coef();
	}

	//---------------------------------------------Processing------------------------------------
	for(int i = 0; i < NUM_MONO_MODULE_TYPES; i++)
	{
		if(processing_stage[i]) //pointer check
			processing_stage[i](processing_samples, processing_samples);
	}

	early_processing_stage(processing_samples, out_sampleL, out_sampleR);
	//---------------------------------------------Post corrrection------------------------------

	calc_fade_step();

	for(uint8_t i = 0; i < block_size; i++)
	{
	//---------------------------------------------PWM indicator---------------------------------
		float a_ind = fabsf(out_sampleL[i]);

		if(pwm_count_f < a_ind) pwm_count_f = a_ind;
		pwm_count_po++;
		pwm_count_po &= 0x1ff;
		if(!pwm_count_po)
		{
			if(pwm_count_f > 0.9f)GPIO_SetBits(GPIOB,GPIO_Pin_14);
			else GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			pwm_count_f = 0.0f;
		}
	//----------------------------------Out conversion-----------------------------------------------


		ccl[i] = out_clip(out_sampleL[i] * processing_params.preset_volume) * 8388607.0f;// * get_fade_coef();
		ccr[i] = out_clip(out_sampleR[i] * processing_params.preset_volume) * 8388607.0f;// * get_fade_coef();


		switch(system_parameters.output_mode)
		{
			case LINE: ccl[i] = ccl[i] >> 1 ; ccr[i] = ccr[i] >> 1; break;
			case BALANCE: ccr[i] = -ccl[i]; break;
			case MONITOR: ccl[i] = out_clip(mon_sample[i] * processing_params.preset_volume) * 8388607.0f;// * get_fade_coef(); break;
		}

		dac_data[base_address + i].left.sample = ror16((uint32_t)(ccl[i] << 8));
		dac_data[base_address + i].right.sample = ror16((uint32_t)(ccr[i] << 8));

	}

	//---------------------------------------------End-------------------------------------
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}


//=============================Processing functions=====================================
void __RAMFUNC__ bypass_processing_stage(float* in_samples, float* out_samples)
{
//	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
//	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	for(uint8_t i = 0; i < block_size; i++)
		out_samples[i] = in_samples[i];
}

void __RAMFUNC__ gate_processing_stage(float* in_samples, float* out_samples)
{
	if(current_preset.gate.on)
	{
		//-------------------------------GATE processs(threshold on start)-------------------
		for(uint8_t i = 0; i < block_size; i++)
			out_samples[i] = in_samples[i] * gate_buf[i];
	}
}

void __RAMFUNC__ compressor_processing_stage(float* in_samples, float* out_samples)
{
	//------------------------------------Compressor-----------------------------------------
	if(current_preset.compressor.on)
	{
		for(uint8_t i = 0; i < block_size; i++)
			out_samples[i] = compr_out(in_samples[i]);
	}
}

void __RAMFUNC__ preamp_processing_stage(float* in_samples, float* out_samples)
{
	if(current_preset.preamp.on)
	{
		float out_biquad_samples[block_size];
		arm_biquad_cascade_df1_f32(&preamp_instance, in_samples, out_biquad_samples, block_size);

		for(uint8_t i = 0; i < block_size; i++)
			out_samples[i] = out_biquad_samples[i] * processing_params.pream_vol * 3.0f;
	}
}

void __RAMFUNC__ pa_processing_stage(float* in_samples, float* out_samples)
{
	//--------------------------------------Amplifier----------------------------------------
	if(current_preset.power_amp.on)
	{
		if(current_preset.power_amp.type != 8)
		{
			for(uint8_t i = 0; i < block_size; i++)
				out_samples[i] = soft_clip_amp(in_samples[i] * processing_params.amp_vol) * processing_params.amp_slave;

			arm_fir_f32(&pa_instance, out_samples, out_samples, block_size);
		}
	}

	//--------------------------------------Presence-------------------------------------------
	if(current_preset.power_amp.presence_on)
	{
		arm_biquad_cascade_df1_f32(&presence_instance, out_samples, out_samples, block_size);
	}
}

void __RAMFUNC__ ir_processing_stage(float* in_samples, float* out_samples)
{
	uint8_t aux_smpl_wr_ptr;
	if(aux_smpl_rd_ptr < block_size) aux_smpl_wr_ptr = block_size;
	else aux_smpl_wr_ptr = 0;

	for(int i = 0; i < block_size; i++)
	{
		mon_sample[i] = in_samples[i];

		to523(in_samples[i] * 0.3f, &aux_samples[aux_smpl_wr_ptr][0]);

		aux_smpl_wr_ptr++;

		//---------------------------------------Cab data or Dry signal-------------------------
		if(!current_preset.cab_sim_on || !processing_params.impulse_avaliable)
		{
			// bypass IR
			out_samples[i] = in_samples[i];
		}
		else
		{
			out_samples[i] = ir_samples[i];
		}
	}
}

void __RAMFUNC__ hpf_processing_stage(float* in_samples, float* out_samples)
{
	//---------------------------------------HPF-----------------------------------------------
	if(current_preset.eq1.hp_on)
	{
		for(uint8_t i=0; i<block_size; i++)
			out_samples[i] = filt_hp(in_samples[i]);
	}
	else bypass_processing_stage(in_samples, out_samples);
}

void __RAMFUNC__ eq_processing_stage(float* in_samples, float* out_samples)
{
	//------------------------------------PARAMETRIC-------------------------------------------
	if(current_preset.eq1.parametric_on)
	{
		arm_biquad_cascade_df1_f32(&eq_instance, in_samples, out_samples, block_size);
	}
	else bypass_processing_stage(in_samples, out_samples);
}

void __RAMFUNC__ lpf_processing_stage(float* in_samples, float* out_samples)
{
	//---------------------------------------LPF-----------------------------------------------
	if(current_preset.eq1.lp_on)
	{
		for(uint8_t i=0; i<block_size; i++)
			out_samples[i] = filt_lp(in_samples[i]);
	}
	else bypass_processing_stage(in_samples, out_samples);
}

void __RAMFUNC__ early_processing_stage(float* in_samples, float* out_l_samples, float* out_r_samples)
{
	for(uint8_t i = 0; i < block_size; i++)
	{
		if(current_preset.reverb.on) reverb_accum = in_samples[i] * 0.7f;
		else reverb_accum = 0.0f;

		if(!rev_en)
		{
			switch (current_preset.reverb.type)
			{
				case 0: early1();break;
				case 1: early2();break;
				case 2: early3();break;
			}
		}
		else rev_en1 = 1;

		out_l_samples[i] = in_samples[i] + ear_outL * processing_params.ear_vol;
		out_r_samples[i] = in_samples[i] + ear_outR * processing_params.ear_vol;
	}
}

//================================Processing subroutines=======================================
inline float soft_clip_amp(float in)
{
	float aaa = fabsf(in);
	if(aaa < 0.1618f) aaa *= 4.294115f;
	else aaa = 0.81f * ((aaa - 0.11f) / (fabsf(aaa - 0.11f) + 0.033f)) + 0.2f;

	if(in < 0.0f) in = -aaa;
	else in = aaa;
	return in;
}

inline float soft_clip_pre(float in)
{
	if(in > 0.0f)
	{
		if(in >= 0.21697f)
		{
			in = 0.54f * ((in - 0.12f) / (fabsf(in - 0.12f) + 0.03f)) - 0.195441f;
		}
	}
	else {
		in = -in;
		if(in >= 0.7117f) in = 0.8f * ((in - 0.53f) / (fabsf(in - 0.53f) + 0.1f)) + 0.19569f;
		in = -in;
	}
	return in;
}

float in_dc_old;
float out_dc_old;
inline float dc_block(float in)
{
	float a = in - in_dc_old + 0.995f * out_dc_old;
	in_dc_old = in;
	out_dc_old = a;
	return a;
}

inline float out_clip(float in)
{
	float a = fabsf(in);
	float b = a - 0.91838997f;
	if(a > 0.9486f)
	{
		a = 0.98f * (b/(fabsf(b) + 0.001f));
		if(in >=0.0f)in = a;
		else in = -a;
	}
	return in;
}
