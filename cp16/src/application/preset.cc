/*
 * preset.cc
 *
 *  Created on: Oct 28, 2024
 *      Author: dime
 */

#include "preset.h"
//
//const uint8_t prog_data_init[128] =
//{
//	/*eq*/15, 15, 15, 15, 15,
//	/*early*/0, 0,
//	/*pres_vol*/31,
//	/*cab_on_of*/1,
//	/*delay*/0,
//	/*eq_on*/0,
//	/*er_on*/0,
//	/*amp_on*/16,/*amp_vol*/16,/*amp slave*/0,/*amp type*/0,
//	/*preamp*/25, 0, 0, 0,
//	/*gate*/ 0, 0, 0,
//	/*compressor*/ 0, 0, 0
//};

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

