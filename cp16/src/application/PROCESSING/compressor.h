/*
 * compressor.h
 *
 *  Created on: Oct 15, 2024
 *      Author: dime
 */

#ifndef SRC_APPLICATION_COMPRESSOR_H_
#define SRC_APPLICATION_COMPRESSOR_H_

#include "appdefs.h"

void gate_change_preset(void);
void compressor_change_preset(int dgui, int npreset);
void compressor_init(void);
void comp_par(uint32_t val);
void gate_par(uint32_t val);
float gate_out(float efxout);
float compr_out(float efxout);

#endif /* SRC_APPLICATION_COMPRESSOR_H_ */
