/*
 * preset.cc
 *
 *  Created on: Oct 28, 2024
 *      Author: dime
 */

#include "preset.h"


preset_data_legacy_t default_legacy_preset;

void PRESET_init()
{
	kgp_sdk_libc::memset(&default_legacy_preset, 0, sizeof(preset_data_legacy_t));

	default_legacy_preset.eq_band_vol[0] = 15;
	default_legacy_preset.eq_band_vol[1] = 15;
	default_legacy_preset.eq_band_vol[2] = 15;
	default_legacy_preset.eq_band_vol[3] = 15;
	default_legacy_preset.eq_band_vol[4] = 15;

	default_legacy_preset.preset_volume = 31;

	default_legacy_preset.cab_on = 1;

	default_legacy_preset.amp_volume = 16;
	default_legacy_preset.amp_slave = 16;

	default_legacy_preset.preamp_volume = 25;
}

void preset_from_legacy(preset_data_t* dst_preset, const preset_data_legacy_t* src_preset)
{
	kgp_sdk_libc::memset(dst_preset, 0, sizeof(preset_data_t));

	// EQ2 to default values (non zero)
	dst_preset->eq2.band_vol[0] = 15;
	dst_preset->eq2.band_vol[1] = 15;
	dst_preset->eq2.band_vol[2] = 15;
	dst_preset->eq2.band_vol[3] = 15;
	dst_preset->eq2.band_vol[4] = 15;

	if(src_preset->eq_pre)
	{
		dst_preset->modules_order[0] = CM;
		dst_preset->modules_order[1] = EQ;
		dst_preset->modules_order[2] = PR;
		dst_preset->modules_order[3] = PA;
		dst_preset->modules_order[4] = IR;
		dst_preset->modules_order[5] = NG;
	}
	else
	{
		dst_preset->modules_order[0] = CM;
		dst_preset->modules_order[1] = PR;
		dst_preset->modules_order[2] = PA;
		dst_preset->modules_order[3] = IR;
		dst_preset->modules_order[4] = EQ;
		dst_preset->modules_order[5] = NG;
	}

	dst_preset->volume = src_preset->preset_volume;

	dst_preset->gate.on = src_preset->gate_on;
	dst_preset->gate.threshold = src_preset->gate_threshold;
	dst_preset->gate.decay = src_preset->gate_decay;

	dst_preset->compressor.on = src_preset->compressor_on;
	dst_preset->compressor.sustain = src_preset->compressor_sustain;
	dst_preset->compressor.volume = src_preset->compressor_volume;

	dst_preset->preamp.on = src_preset->preamp_on;
	dst_preset->preamp.volume = src_preset->preamp_volume;
	dst_preset->preamp.low = src_preset->preamp_low;
	dst_preset->preamp.mid = src_preset->preamp_mid;
	dst_preset->preamp.high = src_preset->preamp_high;

	dst_preset->power_amp.on = src_preset->amp_on;
	dst_preset->power_amp.volume = src_preset->amp_volume;
	dst_preset->power_amp.slave = src_preset->amp_slave;
	dst_preset->power_amp.type = src_preset->amp_type;
	dst_preset->power_amp.presence_on = src_preset->presence_on;
	dst_preset->power_amp.presence_vol = src_preset->presence_vol;

	dst_preset->eq1.parametric_on = src_preset->eq_on;
	for(int i=0; i<5; i++)
	{
		dst_preset->eq1.band_vol[i] = src_preset->eq_band_vol[i];
		dst_preset->eq1.freq[i] = src_preset->eq_freq[i];
		dst_preset->eq1.Q[i] = src_preset->eq_Q[i];
	}
	dst_preset->eq1.hp_on = src_preset->hp_on;
	dst_preset->eq1.hp_freq = src_preset->hp_freq;
	dst_preset->eq1.lp_on = src_preset->lp_on;
	dst_preset->eq1.lp_freq = src_preset->lp_freq;

	dst_preset->cab_sim_on = src_preset->cab_on;

	dst_preset->reverb.on = src_preset->early_on;
	dst_preset->reverb.type = src_preset->early_type;
	dst_preset->reverb.volume = src_preset->early_volume;
}

