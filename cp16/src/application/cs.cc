#include "appdefs.h"
#include "gpio.h"
#include "cs.h"

#include "ADAU/adau1701.h"
#include "eeprom.h"
#include "AT45DB321.h"
#include "debug_led.h"

#include "DSP/fades.h"
#include "DSP/filters.h"
#include "DSP/sound_processing.h"


inline void key_check(void);

const char ver[] = FIRMWARE_VER;
const char dev[] = FIRMWARE_NAME;

uint8_t usb_flag = 0;
bool sw4_state = true;

volatile uint32_t key_buf;

extern volatile uint8_t usb_type;

//volatile  float vol_ind_vector[3];

void start_usb(uint8_t type);

TCSTask* CSTask ;

TCSTask::TCSTask () : TTask()
{
}

void TCSTask::Code()
{
	init();

	DSP_init();
	flash_folder_init();

	adau_init_ic();

	delay_nop(0xffff);

#ifdef __PA_VERSION__
//	if(sys_para[2] == 2) sig_invert(1);
	if(system_parameters.output_mode == BALANCE) sig_invert(1);
#endif

	char version_string[FIRMWARE_STRING_SIZE] = {0};

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

	int a = TAPS_PA_FIR - 1;
	switch(preset_data[a_t])
	{
		case 0:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = PP_6L6[i];break;
		case 1:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = PP_EL34[i];break;
		case 2:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = SE_6L6[i];break;
		case 3:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = SE_EL34[i];break;
		case 4:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = tc_1[i];break;
		case 5:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = fender[i];break;
		case 6:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = jcm800[i];break;
		case 7:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = lc50[i];break;
		case 9:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = mes_mod[i];break;
		case 10:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = mes_vint[i];break;
		case 11:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = Pr0_Re0_5150[i];break;
		case 12:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = Pr5_Re5_5150[i];break;
		case 13:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = Pr8_Re7_5150[i];break;
		case 14:for(int i = 0 ; i < TAPS_PA_FIR ; i++)Coeffs[a--] = Pr9_Re8_5150[i];break;
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
