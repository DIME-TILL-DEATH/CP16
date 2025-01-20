/*
 * delay.h
 *
 *  Created on: Jan 16, 2025
 *      Author: dime
 */

#ifndef SRC_APPLICATION_PROCESSING_DELAY_H_
#define SRC_APPLICATION_PROCESSING_DELAY_H_

#include "preset.h"

typedef enum
{
	DELAY_MIX,
	DELAY_TIME,
	DELAY_FEEDBACK,
	DELAY_HPF,
	DELAY_LPF
}DELAY_param_type_t;

void DELAY_init();
void DELAY_process(float* inl , float* inr, float* outl, float* outr);
void DELAY_set_par(DELAY_param_type_t param_type, uint32_t val);
void DELAY_mem_clear();

extern preset_data_t current_preset;

#endif /* SRC_APPLICATION_PROCESSING_DELAY_H_ */
