/*
 * preset.h
 *
 *  Created on: Oct 28, 2024
 *      Author: dime
 */

#ifndef SRC_APPLICATION_PRESET_H_
#define SRC_APPLICATION_PRESET_H_

#include "DSP/sound_processing.h"

//======================Legacy================================
enum{
	/*eq*/ eq1=0, eq2, eq3, eq4, eq5,
	/*early*/ e_vol, e_t,
	/*pres_vol*/pres_lev,
	/*cab_on_off*/cab_on,
	/*eq_on*/eq_on,
	/*er_on*/er_on,
	/*amp_on*/amp_on, a_vol, amp_slave, /*amp type*/a_t,
	/*preamp*/preamp_on, preamp_vol, preamp_lo, preamp_mi, preamp_hi,
	/*gate*/gate_on, gate_th, gate_att,
	/*compressor*/compr_on, sustein, compr_vol,
	/*eq band freq*/fr1, fr2, fr3, fr4, fr5,
	/*eq band q*/q1, q2, q3, q4, q5,
	/*lohi*/lop, hip, hip_on, lop_on,
	/*presence*/pr_on, pr_vol,
	/*eq position*/eq_po,
	/*end*/pdCount
};

#define EQ_BANDS_COUNT 5

typedef struct
{
    uint8_t eq_band_vol[EQ_BANDS_COUNT];

    uint8_t early_volume;
    uint8_t early_type;
    uint8_t preset_volume;
    uint8_t cab_on;
    uint8_t eq_on;
    uint8_t early_on;

    uint8_t amp_on;
    uint8_t amp_volume;
    uint8_t amp_slave;
    uint8_t amp_type;

    uint8_t preamp_on;
    uint8_t preamp_volume;
    uint8_t preamp_low;
    uint8_t preamp_mid;
    uint8_t preamp_high;

    uint8_t gate_on;
    uint8_t gate_threshold;
    uint8_t gate_decay;

    uint8_t compressor_on;
    uint8_t compressor_sustain;
    uint8_t compressor_volume;

    uint8_t eq_freq[EQ_BANDS_COUNT];
    uint8_t eq_Q[EQ_BANDS_COUNT];

    uint8_t lp_freq;
    uint8_t hp_freq;

    uint8_t hp_on;
    uint8_t lp_on;

    uint8_t presence_on;
    uint8_t presence_vol;

    uint8_t eq_pre;

}preset_data_legacy_t;


extern preset_data_legacy_t default_legacy_preset;

//=======================New generation=======================

typedef struct
{
    uint8_t on;
    uint8_t type;
    uint8_t threshold;
    uint8_t decay;
    uint8_t reserved[4];
}gate_data_t;

typedef struct
{
    uint8_t on;
    uint8_t type;
    uint8_t sustain;
    uint8_t volume;
    uint8_t reserved[4];
}compressor_data_t;

typedef struct
{
    uint8_t on;
    uint8_t type;
    uint8_t volume;
    uint8_t low;
    uint8_t mid;
    uint8_t high;
    uint8_t reserved[2];
}preamp_data_t;

typedef struct
{
    uint8_t on;
    uint8_t type;
    uint8_t volume;
    uint8_t slave;
    uint8_t presence_on;
    uint8_t presence_vol;
    uint8_t depth;
    uint8_t reserved[1];
}pa_data_t;

typedef struct
{
    uint8_t parametric_on;
    uint8_t hp_on;
    uint8_t lp_on;

    uint8_t lp_freq;
    uint8_t hp_freq;

    int8_t gain[EQ_BANDS_COUNT];
//    uint16_t freq[EQ_BANDS_COUNT];
    uint8_t freq[EQ_BANDS_COUNT];
    uint8_t reserv[EQ_BANDS_COUNT];

    uint8_t Q[EQ_BANDS_COUNT]; //    int8_t Q[EQ_BANDS_COUNT];
    uint8_t band_type[EQ_BANDS_COUNT];
    uint8_t band_on[EQ_BANDS_COUNT];

    uint8_t reserved[5];
}eq_t; //42

typedef struct
{
    uint8_t on;
    uint8_t type;
    uint8_t volume;
    uint8_t reserved[5];
}reverb_data_t;

typedef struct
{
    uint8_t modules_order[14];
    uint8_t volume;
    uint8_t cab_sim_on;

    gate_data_t 		gate;
    compressor_data_t 	compressor;
    preamp_data_t		preamp;
    pa_data_t			power_amp;

    eq_t eq1;
    eq_t eq2;

    // Tremolo, phaser, flanger, chorus, WAH
    uint8_t reserved[22];

    reverb_data_t reverb;
}preset_data_t;

#define PRESET_NAME_LENGTH 32
typedef struct
{
    char name[PRESET_NAME_LENGTH];
    preset_data_t parametersData;
}save_data_t;

typedef struct
{
	std::emb_string irFileName;
	std::emb_string irLinkPath;
}ir_path_data_t;

extern char current_preset_name[PRESET_NAME_LENGTH];
extern ir_path_data_t current_ir_link;
extern preset_data_t current_preset;

void PRESET_init();
void preset_from_legacy(preset_data_t* dst_preset, const preset_data_legacy_t* src_preset);
void legacy_from_preset(preset_data_legacy_t* dst_preset, const preset_data_t* src_preset);

#endif /* SRC_APPLICATION_PRESET_H_ */
