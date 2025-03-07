#include "appdefs.h"

#include "gpio.h"
#include "eeprom.h"
#include "AT45DB321.h"
#include "debug_led.h"

#include "cs.h"

#include "ADAU/adau1701.h"

#include "preset.h"

#include "PROCESSING/sound_processing.h"

#include "PROCESSING/amp_imp.h"
#include "PROCESSING/compressor.h"
#include "PROCESSING/eq.h"
#include "PROCESSING/fades.h"
#include "PROCESSING/filters.h"
#include "PROCESSING/tremolo.h"
#include "PROCESSING/chorus.h"
#include "PROCESSING/phaser.h"
#include "PROCESSING/delay.h"

extern ParametricEq parametricEq0;
extern ParametricEq parametricEq1;

char __CCM_BSS__ loadedCab[256]; // debug data

inline void key_check(void);

const char ver[] = FIRMWARE_VER;
const char dev[] = FIRMWARE_NAME;

uint8_t usb_flag = 0;
bool sw4_state = true;

volatile uint32_t key_buf;

extern volatile uint8_t usb_type;

void start_usb(uint8_t type);

TCSTask *CSTask;

TCSTask::TCSTask()
	:TTask() {
}

void TCSTask::Code() {
	bool started = false;
	init();

	PRESET_init();

	DSP_init();
	EEPROM_folderInit();

	adau_init_ic();

	delay_nop(0xffff);

	char version_string[FIRMWARE_STRING_SIZE] = { 0 };

	kgp_sdk_libc::strcpy(version_string, dev);
	version_string[kgp_sdk_libc::strlen(dev)] = '.';
	kgp_sdk_libc::strcpy(version_string + kgp_sdk_libc::strlen(dev) + 1, ver);

	if (kgp_sdk_libc::strcmp(version_string, system_parameters.firmware_version)) {
		system_parameters.eol_symb = '\n';
		kgp_sdk_libc::memset(system_parameters.firmware_version, 0,
				FIRMWARE_STRING_SIZE);
		kgp_sdk_libc::strcpy(system_parameters.firmware_version,
				version_string);

		EEPROM_saveSys();
	}

	adau_run();

	while (1)
	{
		if (!usb_flag)
		{
			if (GPIOA->IDR & GPIO_Pin_9)
			{
				usb_flag = 1;
				start_usb(usb_type);
				sw4_state = false;
			}
		}
		else
		{
			if (!(GPIOA->IDR & GPIO_Pin_9))
				NVIC_SystemReset();
		}

		if (!started){
			NVIC_EnableIRQ(DMA1_Stream5_IRQn);
			SPI_I2S_DMACmd(adau_com_spi, SPI_I2S_DMAReq_Tx, ENABLE);
			NVIC_EnableIRQ(SPI2_IRQn);

//			key_check();

#ifdef __LA3_MOD__
			// LA3 starting with clean channel
			bank_pres[0] = (system_parameters.la3_cln_preset & 0xF0) >> 4;
			bank_pres[1] = system_parameters.la3_cln_preset & 0x0F;
			preset_change();
#else
			key_check();
#endif

			NVIC_EnableIRQ(EXTI0_IRQn);
			NVIC_EnableIRQ(EXTI1_IRQn);
			NVIC_EnableIRQ(EXTI15_10_IRQn);

			started = true;
		}
	}
}

inline void key_check(void)
{
	static uint8_t key_buf_local = 0xff;

	if (!sw4_state)
		return;

	key_buf = GPIOB->IDR & 0xc03;
	key_buf |= key_buf >> 8;
	key_buf = ~key_buf & 0xf;

	if (key_buf_local != key_buf)
	{
		key_buf_local = key_buf;
		bank_pres[0] = key_buf_local >> 2;
		bank_pres[1] = key_buf_local & 3;

		preset_change();
	}
}

void preset_change(void){
	fade_out();
	while (!is_fade_complete());

	kgp_sdk_libc::memset(current_preset_name, 0, PRESET_NAME_LENGTH);

	save_data_t load_data;

	EEPROM_loadPreset(bank_pres[0], bank_pres[1], load_data, current_ir_link);
	kgp_sdk_libc::memcpy(&current_preset, &load_data.parametersData, sizeof(preset_data_t));
	kgp_sdk_libc::memcpy(current_preset_name, load_data.name, PRESET_NAME_LENGTH);

	ir_path_data_t link_data;
	EEPROM_getPresetCabPath(bank_pres[0], bank_pres[1], link_data);
	CS_activateIr(link_data.irLinkPath + "/" + link_data.irFileName);
	set_parameters();
	fade_in();
}

bool CS_activateIr(const emb_string &irFilePath) {
	emb_string err_msg;
	cleanCabData();
	if (EEPROM_loadIr(cab_data, irFilePath, err_msg) != true) {
		processing_params.impulse_avaliable = 0;
		led_pulse_config(1);

		kgp_sdk_libc::memset(loadedCab, 0, 256); // debug info
		return false;
	} else {
		dsp_upload_ir(cab_data);
		processing_params.impulse_avaliable = 1;
		led_pulse_config(0);

		kgp_sdk_libc::memcpy(loadedCab, irFilePath.c_str(), sizeof(irFilePath));
		return true;
	}
}

void set_parameters(void) {
	if (!current_preset.preamp.volume)
		current_preset.preamp.volume = 13;
	if (!current_preset.power_amp.slave)
		current_preset.power_amp.slave = 31;
	if (!current_preset.compressor.sustain)
		current_preset.compressor.sustain = 15;
	if (!current_preset.compressor.volume)
		current_preset.compressor.volume = 15;

	processing_params.preset_volume = powf(current_preset.volume, 2.0f) * (1.0f / powf(31.0f, 2.0f));
	processing_params.ir_send_volume = powf(current_preset.ir_send_level, 2.0f) * (1.0f / powf(31.0f, 2.0f));

	gate_par(current_preset.gate.threshold << 8);
	gate_par(1 | (current_preset.gate.decay << 8));

	comp_par(0 | (current_preset.compressor.sustain << 8));
	comp_par(2 | (current_preset.compressor.volume << 8));

	processing_params.pream_vol = powf(current_preset.preamp.volume, 2.0f)* (1.0f / powf(31.0f, 2.0f));
	preamp_param(PREAMP_LOW, current_preset.preamp.low);
	preamp_param(PREAMP_MID, current_preset.preamp.mid);
	preamp_param(PREAMP_HIGH, current_preset.preamp.high);

	processing_params.amp_vol = powf(current_preset.power_amp.volume, 2.0f)* (10.0f / powf(31.0f, 2.0f)) + 1.0f;
	processing_params.amp_slave = powf(current_preset.power_amp.slave, 4.0f)* (0.99f / powf(31.0f, 4.0f)) + 0.01f;
	pa_update_coefficients(current_preset.power_amp.type);
	set_shelf(current_preset.power_amp.presence_vol * (31.0f / 31.0f));

	parametricEq0.recalcCoefficients();
	parametricEq1.recalcCoefficients();

	TREMOLO_set_par(TREMOLO_DEPTH, current_preset.tremolo.depth);
	TREMOLO_set_par(TREMOLO_RATE, current_preset.tremolo.rate);
	TREMOLO_set_par(TREMOLO_FORM, current_preset.tremolo.form);

	CHORUS_set_par(CHORUS_MIX, current_preset.chorus.mix);
	CHORUS_set_par(CHORUS_RATE, current_preset.chorus.rate);
	CHORUS_set_par(CHORUS_WIDTH, current_preset.chorus.width);
	CHORUS_set_par(CHORUS_HPF, current_preset.chorus.hpf);

	PHASER_set_par(PHASER_MIX, current_preset.phaser.mix);
	PHASER_set_par(PHASER_FEEDBACK, current_preset.phaser.feedback);
	PHASER_set_par(PHASER_RATE, current_preset.phaser.rate);
	PHASER_set_par(PHASER_WIDTH, current_preset.phaser.width);
	PHASER_set_par(PHASER_CENTER, current_preset.phaser.center);
	PHASER_set_par(PHASER_STAGES, current_preset.phaser.stages);

	DELAY_set_par(DELAY_TIME, current_preset.delay.time);
	DELAY_set_par(DELAY_MIX, current_preset.delay.mix);
	DELAY_set_par(DELAY_FEEDBACK, current_preset.delay.feedback);
	DELAY_set_par(DELAY_HPF, current_preset.delay.hpf);
	DELAY_set_par(DELAY_LPF, current_preset.delay.lpf);

	processing_params.ear_vol = current_preset.reverb.volume * (1.0 / 31.0);

	for(int i=0; i<MAX_PROCESSING_STAGES; i++){
		DSP_set_module_to_processing_stage((DSP_module_type_t)current_preset.modules_order[i], i);
	}

	DSP_config_reverb_section((DSP_module_type_t)current_preset.reverb_config[0], (DSP_module_type_t)current_preset.reverb_config[1]);
}
