/*
 * sound_processing.h
 *
 *  Created on: Oct 16, 2024
 *      Author: dime
 */

#ifndef SRC_APPLICATION_PROCESSING_SOUND_PROCESSING_H_
#define SRC_APPLICATION_PROCESSING_SOUND_PROCESSING_H_

#include "appdefs.h"

#define preamp_stage 6
#define presence_stage 1

typedef void (*processing_func_ptr)(float* in, float* out);
typedef void (*stereo_processing_func_ptr)(float* inl, float* inr, float* outl, float* outr);

#define MAX_PROCESSING_STAGES 10
typedef enum
{
	BYPASS=0,
	CM,
	PR,
	PA,
	IR,
	HP,
	EQ0,
	LP,
	NG,
	PS,
	TR,
	CH,
	PH,
	EQ1,
	//---------
	NUM_MONO_MODULE_TYPES,
	//=====================
	// Stereo moduls
	ER_MONO = 64,
	ER_STEREO,
	DELAY,
	//====================
	MASTER=128
}DSP_module_type_t;

typedef struct
{
	float pream_vol = 1.0f;
	float amp_vol = 1.0f;
	float amp_slave = 1.0f;
	float preset_volume = 1.0f;
	float ear_vol = 0.0f;

	float ir_send_volume = 1.0f;
	uint8_t impulse_avaliable = 0;
}processing_params_t;

extern bool consoleBusy;

extern processing_params_t processing_params;
extern uint8_t tuner_use;

extern uint16_t irInClips, irOutClips;
extern uint16_t inClips, outClips;

void DSP_init();
bool DSP_set_module_to_processing_stage(DSP_module_type_t module_type, uint8_t stage_num);
void DSP_config_reverb_section(DSP_module_type_t delay_processing, DSP_module_type_t reverb_processing);

constexpr float vol_ind_k[] = { 3e-3f,  2.7e-5f,   1.75e-4f,};
extern volatile  float vol_ind_vector[3];

#endif /* SRC_APPLICATION_PROCESSING_SOUND_PROCESSING_H_ */
