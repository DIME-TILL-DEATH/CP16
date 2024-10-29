#include "appdefs.h"

#include "gpio.h"
#include "eeprom.h"
#include "AT45DB321.h"
#include "debug_led.h"

#include "cs.h"

#include "ADAU/adau1701.h"

#include "preset.h"

#include "DSP/sound_processing.h"
#include "DSP/fades.h"
#include "DSP/filters.h"
#include "DSP/amp_imp.h"
#include "DSP/compressor.h"


inline void key_check(void);

const char ver[] = FIRMWARE_VER;
const char dev[] = FIRMWARE_NAME;

uint8_t usb_flag = 0;
bool sw4_state = true;

volatile uint32_t key_buf;

extern volatile uint8_t usb_type;


void start_usb(uint8_t type);

TCSTask* CSTask ;

TCSTask::TCSTask () : TTask()
{
}

void TCSTask::Code()
{
	init();

	PRESET_init();

	DSP_init();
	flash_folder_init();

	adau_init_ic();

	delay_nop(0xffff);

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

	adau_run();

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

	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	SPI_I2S_DMACmd(adau_com_spi, SPI_I2S_DMAReq_Tx, ENABLE);
	NVIC_EnableIRQ(SPI2_IRQn);
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

	load_preset();

	emb_string err_msg;
	if(load_ir(cab_data, err_msg) != true)
	{
		processing_params.impulse_avaliable = 0;
		led_pulse_config(1);
	}
	else
	{
		dsp_upload_ir(cab_data);
		processing_params.impulse_avaliable = 1;
		led_pulse_config(0);
	}
	set_parameters();
	fade_in();
}

void set_parameters(void)
{
	if(!current_preset.preamp.volume) current_preset.preamp.volume = 13;
	if(!current_preset.power_amp.slave) current_preset.power_amp.slave = 31;
	if(!current_preset.compressor.sustain) current_preset.compressor.sustain = 15;
	if(!current_preset.compressor.volume) current_preset.compressor.volume  = 15;

	processing_params.preset_volume = powf(current_preset.volume, 2.0f) * (1.0f/powf(31.0f, 2.0f));

	gate_par(	  current_preset.gate.threshold << 8);
	gate_par(1 | (current_preset.gate.decay << 8));

	comp_par(0 | (current_preset.compressor.sustain << 8));
	comp_par(2 | (current_preset.compressor.volume << 8));

	processing_params.pream_vol = powf(current_preset.preamp.volume, 2.0f) * (1.0f/powf(31.0f, 2.0f));
	preamp_param(PREAMP_LOW, current_preset.preamp.low);
	preamp_param(PREAMP_MID, current_preset.preamp.mid);
	preamp_param(PREAMP_HIGH, current_preset.preamp.high);

	processing_params.amp_vol = powf(current_preset.power_amp.volume, 2.0f) * (10.0f/powf(31.0f, 2.0f)) + 1.0f;
	processing_params.amp_slave = powf(current_preset.power_amp.slave, 4.0f) * (0.99f/powf(31.0f, 4.0f)) + 0.01f;
	pa_update_coefficients(current_preset.power_amp.type);
	set_shelf(current_preset.power_amp.presence_vol * (31.0f/31.0f));

	for(uint8_t i = 0 ; i < 5 ; i++)
	{
		filt_ini(i, current_preset.eq1.freq, current_preset.eq1.Q);
		set_filt(i, current_preset.eq1.band_vol[i]);
	}

	float low_pass = powf(195 - current_preset.eq1.lp_freq, 2.0f) * (19000.0f/powf(195.0f, 2.0f)) + 1000.0f;
	SetLPF(low_pass);

	float hi_pass = current_preset.eq1.hp_freq * (980.0f/255.0f) + 20.0f;
	SetHPF(hi_pass);

	processing_params.ear_vol = current_preset.reverb.volume * (1.0/31.0);
}
