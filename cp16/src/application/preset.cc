/*
 * preset.cc
 *
 *  Created on: Oct 28, 2024
 *      Author: dime
 */

#include "preset.h"

#include "PROCESSING/filters.h"

char current_preset_name[PRESET_NAME_LENGTH];
preset_data_legacy_t default_legacy_preset;
preset_data_t current_preset;
ir_path_data_t current_ir_link;

void PRESET_init()
{
	kgp_sdk_libc::memset(&default_legacy_preset, 0, sizeof(preset_data_legacy_t));

	default_legacy_preset.eq_band_vol[0] = 15;
	default_legacy_preset.eq_band_vol[1] = 15;
	default_legacy_preset.eq_band_vol[2] = 15;
	default_legacy_preset.eq_band_vol[3] = 15;
	default_legacy_preset.eq_band_vol[4] = 15;

	default_legacy_preset.preset_volume = 20;

	default_legacy_preset.cab_on = 1;

	default_legacy_preset.amp_volume = 16;
	default_legacy_preset.amp_slave = 16;

	default_legacy_preset.preamp_volume = 25;
}

void preset_from_legacy(preset_data_t *dst_preset, const preset_data_legacy_t *src_preset) {

	kgp_sdk_libc::memset(dst_preset, 0, sizeof(preset_data_t));

	if (src_preset->eq_pre) {
		dst_preset->modules_order[0] = CM;
		dst_preset->modules_order[1] = EQ;
		dst_preset->modules_order[2] = PR;
		dst_preset->modules_order[3] = PA;
		dst_preset->modules_order[4] = IR;
		dst_preset->modules_order[5] = NG;
	} else {
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
	for (int i = 0; i < 5; i++) {

		dst_preset->eq1.gain[i] = src_preset->eq_band_vol[i] - 15;
		dst_preset->eq1.freq[i] = convertLegacyFreq(i, src_preset->eq_freq[i]);
		dst_preset->eq1.Q[i] = (int8_t)(src_preset->eq_Q[i]);
	}
	dst_preset->eq1.hp_on = src_preset->hp_on;
	dst_preset->eq1.hp_freq = src_preset->hp_freq * (980.0f / 255.0f) + 20.0f;
	dst_preset->eq1.lp_on = src_preset->lp_on;
	dst_preset->eq1.lp_freq = powf(195 - src_preset->lp_freq, 2.0f)
			* (19000.0f / powf(195.0f, 2.0f)) + 1000.0f;

	for (int i = 0; i < 5; i++) {
		dst_preset->eq2.gain[i] = 0;
		dst_preset->eq2.freq[i] = legacyCenterFreq[i];
		dst_preset->eq2.Q[i] = 0;
	}
	dst_preset->eq2.hp_freq = 20;
	dst_preset->eq2.lp_freq = 20000;

	dst_preset->cab_sim_on = src_preset->cab_on;

	dst_preset->tremolo.depth = 63;
	dst_preset->tremolo.rate = 63;
	dst_preset->tremolo.form = 0;

	dst_preset->chorus.mix = 63;
	dst_preset->chorus.rate = 31;
	dst_preset->chorus.width = 74;

	dst_preset->phaser.mix = 63;
	dst_preset->phaser.rate = 50;
	dst_preset->phaser.center = 0;
	dst_preset->phaser.width = 55;

	dst_preset->reverb.on = src_preset->early_on;
	dst_preset->reverb.type = src_preset->early_type;
	dst_preset->reverb.volume = src_preset->early_volume;
}

void legacy_from_preset(preset_data_legacy_t *dst_preset, const preset_data_t *src_preset)
{
	dst_preset->preset_volume = src_preset->volume;

	dst_preset->gate_on = src_preset->gate.on;
	dst_preset->gate_threshold = src_preset->gate.threshold;
	dst_preset->gate_decay = src_preset->gate.decay;

	dst_preset->compressor_on = src_preset->compressor.on;
	dst_preset->compressor_sustain = src_preset->compressor.sustain;
	dst_preset->compressor_volume = src_preset->compressor.volume;

	dst_preset->preamp_on = src_preset->preamp.on;
	dst_preset->preamp_volume = src_preset->preamp.volume;
	dst_preset->preamp_low = src_preset->preamp.low;
	dst_preset->preamp_mid = src_preset->preamp.mid;
	dst_preset->preamp_high = src_preset->preamp.high;

	dst_preset->amp_on = src_preset->power_amp.on;
	dst_preset->amp_volume = src_preset->power_amp.volume;
	dst_preset->amp_slave = src_preset->power_amp.slave;
	dst_preset->amp_type = src_preset->power_amp.type;

	dst_preset->presence_on = src_preset->power_amp.presence_on;
	dst_preset->presence_vol = src_preset->power_amp.presence_vol;

	dst_preset->eq_on = src_preset->eq1.parametric_on;
	for (int i = 0; i < 5; i++) {
		// backward compatibility does not support
		dst_preset->eq_band_vol[i] = 15; //src_preset->eq1.gain[i];
		dst_preset->eq_freq[i] = legacyCenterFreq[i]; //src_preset->eq1.freq[i];
		dst_preset->eq_Q[i] = 0; //src_preset->eq1.Q[i];
	}
	dst_preset->hp_on = src_preset->eq1.hp_on;
	dst_preset->hp_freq = src_preset->eq1.hp_freq;
	dst_preset->lp_on = src_preset->eq1.lp_on;
	dst_preset->lp_freq = src_preset->eq1.lp_freq;

	dst_preset->cab_on = src_preset->cab_sim_on;

	dst_preset->early_on = src_preset->reverb.on;
	dst_preset->early_type = src_preset->reverb.type;
	dst_preset->early_volume = src_preset->reverb.volume;
}
