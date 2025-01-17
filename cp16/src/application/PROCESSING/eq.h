/*
 * eq.h
 *
 *  Created on: Jan 17, 2025
 *      Author: dime
 */

#ifndef SRC_APPLICATION_PROCESSING_EQ_H_
#define SRC_APPLICATION_PROCESSING_EQ_H_

#include "appdefs.h"
#include "simple_filter.h"
#include "preset.h"

#define FILT_PI 3.14159265358979323846f
#define EQ_STAGES 5
#define EQ_PARAMETRIC_BANDS 5

class ParametricEq
{
public:
	ParametricEq(eq_t* eq_data_ptr);

	typedef enum
	{
		PEAKING=0,
		LOW_SHELF,
		HIGH_SHELF
	}band_type_t;

	eq_t* eq_data;

	void recalcCoefficients();
	void filterInit(uint8_t bandNum, uint16_t freq, int8_t q);
	void filterCalcCoefs(uint8_t band_num, int8_t filt_gain, band_type_t band_type);

	void setHPF(float freq) {hpf.SetHPF(freq);};
	void setLPF(float freq) {lpf.SetLPF(freq);};

	void process(float* in, float* out, int processing_block_size);

private:
	arm_biquad_casd_df1_inst_f32 eq_instance;

	FirstOrderFilt hpf;
	FirstOrderFilt lpf;

	float filt_cos[EQ_PARAMETRIC_BANDS];
	float filt_sin[EQ_PARAMETRIC_BANDS];
	float filt_alpha[EQ_PARAMETRIC_BANDS];

	float coeff[EQ_STAGES * EQ_PARAMETRIC_BANDS]; // pointer to CCM data?
	float stage[EQ_STAGES * 4];
};

#endif /* SRC_APPLICATION_PROCESSING_EQ_H_ */
