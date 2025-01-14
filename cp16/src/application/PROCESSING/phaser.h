#ifndef SRC_APPLICATION_PHASER_H_
#define SRC_APPLICATION_PHASER_H_

#include "preset.h"

typedef enum
{
	PHASER_MIX,
	PHASER_RATE,
	PHASER_CENTER,
	PHASER_WIDTH,
	PHASER_FEEDBACK,
	PHASER_STAGES
}PHASER_param_type_t;

void PHASER_process(float* in, float* out);
void PHASER_set_par(PHASER_param_type_t param_type, uint32_t val);

extern preset_data_t current_preset;

#endif /* SRC_APPLICATION_PHASER_H_ */
