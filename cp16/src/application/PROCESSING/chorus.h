/*
 * chorus.h
 *
 *  Created on: Jan 13, 2025
 *      Author: dime
 */

#ifndef SRC_APPLICATION_PROCESSING_CHORUS_H_
#define SRC_APPLICATION_PROCESSING_CHORUS_H_


#include "preset.h"

typedef enum
{
	CHORUS_MIX,
	CHORUS_RATE,
	CHORUS_WIDTH,
	CHORUS_HPF
}CHORUS_param_type_t;

void CHORUS_process(float* in, float* out);
void CHORUS_set_par(CHORUS_param_type_t param_type, uint32_t val);

extern preset_data_t current_preset;

#endif /* SRC_APPLICATION_PROCESSING_CHORUS_H_ */
