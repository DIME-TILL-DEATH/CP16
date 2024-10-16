#include "appdefs.h"
#include "init.h"
#include "cs.h"

#include "sigma.h"
#include "eepr.h"
#include "AT45DB321.h"
#include "debug_led.h"

#include "fades.h"

#include "filt.h"
#include "compressor.h"
#include "amp_imp.h"
#include "Reverb/reverb.h"

float amp_vol = 1.0f;
float amp_sla = 1.0f;


inline void key_check(void);
inline float soft_clip_amp(float in);
inline float soft_clip_pre(float in);
inline float dc_block(float in);

const char ver[] = FIRMWARE_VER;
const char dev[] = FIRMWARE_NAME;

float __CCM_BSS__ coeff_preamp[preamp_stage * 5];
float __CCM_BSS__ stage_preamp[preamp_stage * 4];

float __CCM_BSS__ Coeffs[taps_fir];
float __CCM_BSS__ State[taps_fir + block_size -1];

float __CCM_BSS__ coeff_eq[eq_stage * 5];
float __CCM_BSS__ stage_eq[eq_stage * 4];

float __CCM_BSS__ coeff_presen[presence_stage * 5];
float __CCM_BSS__ stage_presen[presence_stage * 4];

arm_fir_instance_f32 fir_instance ;
arm_biquad_casd_df1_inst_f32 preamp_instance;
arm_biquad_casd_df1_inst_f32 eq_instance;
arm_biquad_casd_df1_inst_f32 presence_instance;

uint8_t usb_flag = 0;
bool sw4_state = true;

volatile uint32_t key_buf;

float preset_volume = 1.0f;
float ear_vol = 0.0f; // RV was 1.0f

extern volatile uint8_t usb_type;

volatile  float vol_ind_vector[3];

void start_usb(uint8_t type);

TCSTask* CSTask ;

TCSTask::TCSTask () : TTask()
{
}

void TCSTask::Code()
{
	init();

	gate_change_preset();
	compressor_init();
	compressor_change_preset(0, 0);

	arm_fir_init_f32(&fir_instance, taps_fir, Coeffs, State, block_size);
	arm_biquad_cascade_df1_init_f32(&eq_instance, eq_stage , coeff_eq , stage_eq);
	arm_biquad_cascade_df1_init_f32(&presence_instance, presence_stage , coeff_presen, stage_presen);
	arm_biquad_cascade_df1_init_f32(&preamp_instance, preamp_stage , coeff_preamp, stage_preamp);

	flash_folder_init();

	adau_init_ic();

	delay_nop(0xffff);

#ifdef __PA_VERSION__
//	if(sys_para[2] == 2) sig_invert(1);
	if(system_parameters.output_mode == LINE) sig_invert(1);
#endif

	//extern const uint8_t ver[];
//	uint8_t temp = 0;
//	for(uint8_t i = 0 ; i < 8 ; i++)
//		if(ver[i] != sys_para[23 + i]) temp++;

	char version_string[FIRMWARE_STRING_SIZE] = {0};

	// sprintf нужен
	//		kgp_sdk_libc::emb_printf::sprintf((char*)&system_parameters.firmware_version, "%s.%s", dev, ver);
	kgp_sdk_libc::strcpy(version_string, dev);
	version_string[kgp_sdk_libc::strlen(dev)] = '.';
	kgp_sdk_libc::strcpy(version_string + kgp_sdk_libc::strlen(dev) + 1, ver);

	if(kgp_sdk_libc::strcmp(version_string, system_parameters.firmware_version))
	{
		system_parameters.eol_symb = '\n';
		kgp_sdk_libc::memset(system_parameters.firmware_version, 0, FIRMWARE_STRING_SIZE);
		kgp_sdk_libc::strcpy(system_parameters.firmware_version, version_string);

		save_sys();
	}

	dsp_run();

	while(1)
	{
		if(!usb_flag)
		{
			if(GPIOA->IDR & GPIO_Pin_9)
			{
				usb_flag = 1;
				void start_usb(uint8_t type);
				start_usb(usb_type);
				sw4_state = false ;
			}
		}
		else
		{
			if(!(GPIOA->IDR & GPIO_Pin_9))
			NVIC_SystemReset();
		}

#ifdef __LA3_MOD__
	uint8_t bp ;
	load_map0(bp);
	decode_preset( bank_pres, bp);
	preset_change();
#else
	key_check();  // управление пресетами с внешних устройств по средством gpio обычных cp16
#endif
	}
}

inline void key_check(void)
{
	static uint8_t key_buf_local = 0xff;

	if ( !sw4_state ) return ;

    key_buf = GPIOB->IDR & 0xc03;
    key_buf |= key_buf >> 8;
    key_buf = ~key_buf & 0xf;
    if(key_buf_local != key_buf)
    {
    	key_buf_local = key_buf;
		bank_pres[0] = key_buf_local >> 2;
		bank_pres[1] = key_buf_local & 3;

		preset_change();
    }
}

void preset_change(void)
{
	fade_out();
	while(!is_fade_complete());

	emb_string err_msg;
	if(load_pres(cab_data, err_msg,1) != true)
	{
		impulse_avaliable = 0;
		led_pulse_config(1);
	}
	else
	{
		sig_load(cab_data, impulse_buffer);
		impulse_avaliable = 1;
		led_pulse_config(0);
	}
	set_parameters();
	fade_in();
}

uint16_t pwm = 0;
uint16_t pwm_count;
float pwm_count_f = 0.0f;
uint16_t pwm_count_po;

int32_t __CCM_BSS__ ccl[block_size];
int32_t __CCM_BSS__ ccr[block_size];
uint32_t __CCM_BSS__ cl[block_size];
uint32_t __CCM_BSS__ cr[block_size];


float __CCM_BSS__ inp_di_sample[block_size];
float __CCM_BSS__ inp_cab_sample[block_size];
float __CCM_BSS__ processing_sample[block_size];

float __CCM_BSS__ out_sampleL[block_size];
float __CCM_BSS__ out_sampleR[block_size];

float __CCM_BSS__ out_biquad_sample[block_size];

float __CCM_BSS__ gate_buf[block_size];

//----------------------------------------------
void (*processing_stage_func[5])(uint8_t* in);


void __RAMFUNC__ eq_processing_stage(float* in_samples, float* out_samples)
{
	float out_biquad_samples[block_size];

	arm_biquad_cascade_df1_f32(&eq_instance, in_samples, out_biquad_samples, block_size);
	if(preset_data[eq_on])
	{
		for(uint8_t i = 0; i < block_size; i++)
			out_samples[i] = out_biquad_sample[i];
	}
}
//----------------------------------------------

float pream_vol = 1.0f;
uint8_t dma_ht_fl = 0;
//================================PA======================
#ifdef __PA_VERSION__

extern "C" void DMA1_Stream3_IRQHandler()
{
	//--------------------------------------------------------Start---------------------
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_HTIF3))
	{
		DMA_ClearITPendingBit( DMA1_Stream3, DMA_IT_HTIF3);
		dma_ht_fl = 0;
	}
	else
	{
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
		dma_ht_fl = 1;
	}

	//GPIO_SetBits(GPIOB,GPIO_Pin_7);

	//---------------------------Input sample convert------------------------------------
	for(uint8_t i = 0 ; i < block_size; i++)
	{
		uint8_t a = i + dma_ht_fl * block_size;

		inp_di_sample[i] = (int32_t)(ror16(adc_data[a].left.sample)) >> 8;
		inp_cab_sample[i] = (int32_t)(ror16(adc_data[a].right.sample)) >> 8;

		inp_di_sample[i] = dc_block(inp_di_sample[i]) * 0.000000119f;  //-----> Output ADC
		inp_cab_sample[i] *= 0.000000119f;                            //-----> Output CAB

		gate_buf[i] = gate_out(inp_di_sample[i]);

		if(preset_data[compr_on])
			inp_di_sample[i] = compr_out(inp_di_sample[i]);
	}

	//-------------------------------------EQ pre--------------------------------------------
	if(preset_data[eq_po])
	{
		arm_biquad_cascade_df1_f32(&eq_instance, inp_di_sample, out_biquad_sample, block_size);
		if(preset_data[eq_on])
		{
			for(uint8_t i = 0; i < block_size; i++)
				inp_di_sample[i] = out_biquad_sample[i];
		}
	}
	//-------------------------------------Fender--------------------------------------------
	if(preset_data[preamp_on])
	{
		arm_biquad_cascade_df1_f32(&preamp_instance, inp_di_sample, out_biquad_sample, block_size);
		for(uint8_t i = 0; i < block_size; i++)
			inp_di_sample[i] = out_biquad_sample[i] * pream_vol * 3.0f;
	}

	//--------------------------------------Presence-------------------------------------------
	if(preset_data[pr_on])
	{
		arm_biquad_cascade_df1_f32(&presence_instance, processing_sample, out_biquad_sample, block_size);
		for(uint8_t i = 0; i < block_size; i++)
			processing_sample[i] = out_biquad_sample[i];
	}
	//--------------------------------------Amplifier----------------------------------------
	if(preset_data[amp_on])
	{
		if(preset_data[a_t] != 8)
		{
			for(uint8_t i = 0; i < block_size; i++)
				inp_di_sample[i] = soft_clip_amp(inp_di_sample[i] * amp_vol) * amp_sla;

			arm_fir_f32(&fir_instance, inp_di_sample, out_biquad_sample, block_size);
			for(uint8_t i = 0; i < block_size; i++)
				inp_di_sample[i] = out_biquad_sample[i];
		}
	}
	//---------------------------------------Cab data or Dry signal-------------------------
	if(!preset_data[cab_on] || !impulse_avaliable)
	{
		for(uint8_t i = 0; i < block_size; i++)
			processing_sample[i] = inp_di_sample[i];
	}
	else
	{
		for(uint8_t i = 0; i < block_size; i++)
			processing_sample[i] = inp_cab_sample[i];
	}
	//--------------------------------------HPF----------------------------------------------
	if(preset_data[hip_on])
	{
		for(uint8_t i = 0; i < block_size; i++)
			processing_sample[i] = filt_hp(processing_sample[i]);
	}
	//--------------------------------------EQ-post-----------------------------------------------
	if(!preset_data[eq_po])
	{
		if(preset_data[eq_on])
		{
			arm_biquad_cascade_df1_f32(&eq_instance, processing_sample, out_biquad_sample, block_size);
			for(uint8_t i = 0; i < block_size; i++)
				processing_sample[i] = out_biquad_sample[i];
		}
	}
	//---------------------------------------LPF-----------------------------------------------
	if(preset_data[lop_on])
	{
		for(uint8_t i = 0; i < block_size; i++)
			processing_sample[i] = filt_lp(processing_sample[i]);
	}
	//----------------------------Calculate fade-------------------------------
	calc_fade_step();
	//----------------------------ER + GATE-------------------------------
	for(uint8_t i = 0; i < block_size; i++)
	{
		uint8_t a = i + dma_ht_fl * block_size; //pointer
		if(preset_data[gate_on]) processing_sample[i] *= gate_buf[i];  // gate

		if(preset_data[er_on]) reverb_accum = processing_sample[i] * 0.7f;
		else reverb_accum = 0.0f;

		if(!rev_en)
		{
			switch (preset_data[e_t])
			{
				case 0:early1();break;
				case 1:early2();break;
				case 2:early3();break;
			}
		}
		else rev_en1 = 1;

		processing_sample[i] += ear_outL * ear_vol;
	//---------------------------------------------PWM indicator---------------------------------
		float a_ind = fabsf(processing_sample[i]);

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
		ccl[i] = out_clip(inp_di_sample[i] * 0.3f) * 8388607.0f * get_fade_coef();
		ccr[i] = out_clip(processing_sample[i] * preset_volume) * 8388607.0f * get_fade_coef();

		// phones, line, inst?
//		if(sys_para[2] == 1) ccr[i] = ccr[i] >> 1;
		if(system_parameters.output_mode == BALANCE) ccr[i] = ccr[i] >> 1;

		dac_data[a].left.sample = ror16((uint32_t)(ccl[i] << 8));	// IR in ADAU1701 processing
		dac_data[a].right.sample = ror16((uint32_t)(ccr[i] << 8)); // master out

		// (Использовалось ccl, перепроверить, переделать)расчет индикации громкости входа
		vol_ind_vector[1] += vol_ind_k[0] * ( abs(ccl[i]) * vol_ind_k[2] - vol_ind_vector[1]);
		vol_ind_vector[2] += vol_ind_k[0] * ( abs(ccr[i]) * vol_ind_k[2] - vol_ind_vector[2]);
	}
	//---------------------------------------------End-------------------------------------
	//GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}

void set_parameters(void)
{
	if(!preset_data[preamp_vol])preset_data[preamp_vol] = 13;
	if(!preset_data[amp_slave])preset_data[amp_slave] = 31;
	if(!preset_data[sustein])preset_data[sustein] = 15;
	if(!preset_data[compr_vol])preset_data[compr_vol] = 15;
	gate_par(preset_data[gate_th] << 8);
	gate_par(1 | (preset_data[gate_att] << 8));
	comp_par(0 | (preset_data[sustein] << 8));
	comp_par(2 | (preset_data[compr_vol] << 8));
	preset_volume = powf(preset_data[pres_lev],2.0f)*(1.0f/powf(31.0f,2.0f));
	pream_vol = powf(preset_data[preamp_vol],2.0f)*(1.0f/powf(31.0f,2.0f));
	for(uint8_t i = 0 ; i < 3 ; i++)pre_param(i,preset_data[preamp_lo + i]);
	amp_vol = powf(preset_data[a_vol],2.0f)*(10.0f/powf(31.0f,2.0f)) + 1.0f;
	amp_sla = powf(preset_data[amp_slave],4.0f)*(0.99f/powf(31.0f,4.0f)) + 0.01f;

	int a = taps_fir - 1;
	switch(preset_data[a_t])
	{
		case 0:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = PP_6L6[i];break;
		case 1:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = PP_EL34[i];break;
		case 2:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = SE_6L6[i];break;
		case 3:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = SE_EL34[i];break;
		case 4:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = tc_1[i];break;
		case 5:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = fender[i];break;
		case 6:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = jcm800[i];break;
		case 7:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = lc50[i];break;
		case 9:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = mes_mod[i];break;
		case 10:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = mes_vint[i];break;
		case 11:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr0_Re0_5150[i];break;
		case 12:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr5_Re5_5150[i];break;
		case 13:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr8_Re7_5150[i];break;
		case 14:for(int i = 0 ; i < taps_fir ; i++)Coeffs[a--] = Pr9_Re8_5150[i];break;
	}

	for(uint8_t i = 0 ; i < 5 ; i++) filt_ini(i , preset_data + fr1 , preset_data + q1);
	for(uint8_t i = 0 ; i < 5 ; i++) set_filt(i , preset_data[eq1 + i]);

	float low_pass = powf(195 - preset_data[lop],2.0f)*(19000.0f/powf(195.0f,2.0f))+1000.0f;
	SetLPF(low_pass);
	float hi_pass = preset_data[hip]*(980.0f/255.0f)+20.0f;
	SetHPF(hi_pass);

	set_shelf(preset_data[pr_vol]*(31.0f/31.0f));
	ear_vol = preset_data[e_vol]*(1.0/31.0);
}

#else
//=====================================RV===============================
extern "C" void DMA1_Stream3_IRQHandler()
{
	//--------------------------------------------------------Start---------------------
	//GPIO_SetBits(GPIOB, GPIO_Pin_7);

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

	for(uint8_t i = 0; i<block_size; i++)
	{
		uint8_t a = i + dma_ht_fl * block_size;

		inp_di_sample[i] = (int32_t)(ror16(adc_data[a].left.sample)) >> 8;
		inp_cab_sample[i] = (int32_t)(ror16(adc_data[a].right.sample)) >> 8;

		inp_di_sample[i] *= 0.000000119f;  //-----> Output ADC
		inp_cab_sample[i] *= 0.000000119f; //-----> Output Cab                           //-----> Output CAB
	}

	//-------------------------------Cab data or Dry signal-----------------------------
	if(preset_data[cab_on] && impulse_avaliable)
	{
		for(uint8_t i=0; i<block_size; i++)
			processing_sample[i] = inp_cab_sample[i];
	}
	else
	{
		for(uint8_t i=0; i<block_size; i++)
			processing_sample[i] = inp_di_sample[i];
	}
	//--------------------------Compr + Gate--------------------------------------------

	for(uint8_t i=0; i<block_size; i++)
		gate_buf[i] = gate_out(processing_sample[i]);

	if(preset_data[compr_on])
	{
		for(uint8_t i=0; i<block_size; i++)
			inp_di_sample[i] = compr_out(processing_sample[i]);
	}
	//--------------------------------------HPF----------------------------------------------
	if(preset_data[hip_on])
	{
		for(uint8_t i=0; i<block_size; i++)
			processing_sample[i] = filt_hp(processing_sample[i]);
	}
	//--------------------------------------EQ-----------------------------------------------
//	if(!prog_data[eq_po])
//	{
	if(preset_data[eq_on])
	{
		arm_biquad_cascade_df1_f32(&eq_instance, processing_sample, out_biquad_sample, block_size);
		for(uint8_t i=0; i<block_size; i++)
			processing_sample[i] = out_biquad_sample[i];
	}
//	}
	//---------------------------------------LPF-----------------------------------------------
	if(preset_data[lop_on])
	{
		for(uint8_t i=0; i<block_size; i++)
			processing_sample[i] = filt_lp(processing_sample[i]);
	}

	//-----------------------------------Fender preamp--------------------------------------------
	if(preset_data[preamp_on])
	{
		arm_biquad_cascade_df1_f32(&preamp_instance, processing_sample, out_biquad_sample, block_size);
		for(uint8_t i = 0; i<block_size; i++)
			processing_sample[i] = out_biquad_sample[i] * pream_vol * 3.0f;
	}
	//--------------------------------------Presence-------------------------------------------
	if(preset_data[pr_on])
	{
		arm_biquad_cascade_df1_f32(&presence_instance, processing_sample, out_biquad_sample, block_size);
		for(uint8_t i=0; i<block_size; i++)
			processing_sample[i] = out_biquad_sample[i];
	}
	//----------------------------Calculate fades-------------------------------

	calc_fade_step();
	//----------------------------ER + GATE-work-------------------------------
	for(uint8_t i=0; i<block_size; i++)
	{
		uint8_t a = i + dma_ht_fl * block_size; //pointer

		if(preset_data[gate_on])
			processing_sample[i] *= gate_buf[i];  // gate

		if(preset_data[er_on]) reverb_accum = processing_sample[i] * 0.7f;
		else reverb_accum = 0.0f;

		if(!rev_en)
		{
			switch (preset_data[e_t])
			{
				case 0:early1();break;
				case 1:early2();break;
				case 2:early3();break;
			}
		}
		else rev_en1 = 1;

		out_sampleL[i] = (processing_sample[i] + ear_outL * ear_vol) * 8388607.0f;
		out_sampleR[i] = (processing_sample[i] + ear_outR * ear_vol) * 8388607.0f;

		//-----------------------------------------------PWM indicator--------------------------------
		float a_ind = fabsf(processing_sample[i]);

		if(pwm_count_f < a_ind) pwm_count_f = a_ind;
		pwm_count_po++;
		pwm_count_po &= 0x1ff;

		if(!pwm_count_po)
		{
			if(pwm_count_f > 0.9f) GPIO_SetBits(GPIOB,GPIO_Pin_14);
			else GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			pwm_count_f = 0.0f;
		}
		//--------------------------------Out conversion---------------------------------------------------
		out_sampleL[i] *= preset_volume * get_fade_coef();
		out_sampleR[i] *= preset_volume * get_fade_coef();
		ccl[i] =  -out_sampleL[i];
		ccr[i] =  -out_sampleR[i];

		// phones, line, inst?
//		switch(sys_para[2])
//		{
//			case 1:
//			{
//				ccl[i] = ccl[i] >> 1;
//				ccr[i] = ccr[i] >> 1;
//				break;
//			}
//			case 2: ccr[i] = -ccl[i];
//		}
		switch(system_parameters.output_mode)
		{
		case BALANCE:
		{
			cl[i] = ccl[i] >> 1;
			ccr[i] = ccr[i] >> 1;
			break;
		}
		case LINE:
		{
			ccr[i] = -ccl[i];
			break;
		}
		}

		ccl[i] = __SSAT(ccl[i], 24);
		ccr[i] = __SSAT(ccr[i], 24);

		cl[i] = ccl[i] << 8;
		cr[i] = ccr[i] << 8;

		dac_data[a].left.sample = ror16((uint32_t)(ccl[i] << 8));
		dac_data[a].right.sample = ror16((uint32_t)(ccr[i] << 8));

		 // расчет индикации громкости входа
		vol_ind_vector[1] += vol_ind_k[0] * ( abs(ccl[i]) * vol_ind_k[2] - vol_ind_vector[1]) ;
		vol_ind_vector[2] += vol_ind_k[0] * ( abs(ccr[i]) * vol_ind_k[2] - vol_ind_vector[2]) ;
	}
	//---------------------------------------------End-------------------------------------
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}

void set_parameters(void)
{
	if(preset_data[cab_on])
	{
		sig_volume(powf(preset_data[pres_lev],2.0)*(1.0/powf(31.0,2.0)));
		preset_volume = 1.0;
	}
	else
	{
		preset_volume = powf(preset_data[pres_lev],2.0)*(1.0/powf(31.0,2.0));
	}

	if(!preset_data[sustein]) preset_data[sustein] = 15;
	if(!preset_data[compr_vol]) preset_data[compr_vol] = 15;
	gate_par(preset_data[gate_th] << 8);
	gate_par(1 | (preset_data[gate_att] << 8));
	comp_par(0 | (preset_data[sustein] << 8));
	comp_par(2 | (preset_data[compr_vol] << 8));
	if(!preset_data[preamp_vol]) preset_data[preamp_vol] = 28;
	pream_vol = powf(preset_data[preamp_vol],2.0f)*(1.0f/powf(31.0f,2.0f));
	for(uint8_t i = 0 ; i < 3 ; i++) pre_param(i,preset_data[preamp_lo + i]);
	for(uint8_t i = 0 ; i < 5 ; i++) filt_ini(i , preset_data + fr1 , preset_data + q1);
	for(uint8_t i = 0 ; i < 5 ; i++) set_filt(i , preset_data[eq1 + i]);

	float low_pass = powf(195 - preset_data[lop],2.0f)*(19000.0f/powf(195.0f,2.0f))+1000.0f;
	SetLPF(low_pass);
	float hi_pass = preset_data[hip]*(980.0f/255.0f)+20.0f;
	SetHPF(hi_pass);

	set_shelf(preset_data[pr_vol]*(25.0f/31.0f));
	ear_vol = preset_data[e_vol]*(1.0/31.0);
}
#endif

inline float soft_clip_amp(float in)
{
	float aaa = fabsf(in);
	if(aaa < 0.1618f)aaa *= 4.294115f;
	else aaa = 0.81f * ((aaa - 0.11f) / (fabsf(aaa - 0.11f) + 0.033f)) + 0.2f;

	if(in < 0.0f)in = -aaa;
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
