/*
 * tremolo.h
 *
 *  Created on: Jan 13, 2025
 *      Author: dime
 */

#ifndef SRC_APPLICATION_PROCESSING_TREMOLO_H_
#define SRC_APPLICATION_PROCESSING_TREMOLO_H_

#include "preset.h"

typedef enum
{
	TREMOLO_DEPTH,
	TREMOLO_RATE,
	TREMOLO_FORM

}TREMOLO_param_type_t;

void TREMOLO_step(void);
void TREMOLO_set_par(TREMOLO_param_type_t param_type, uint32_t val);
float TREMOLO_get_volume(void);

extern preset_data_t current_preset;

#endif /* SRC_APPLICATION_PROCESSING_TREMOLO_H_ */
