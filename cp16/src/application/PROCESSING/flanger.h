#ifndef SRC_APPLICATION_FLANGER_H_
#define SRC_APPLICATION_FLANGER_H_

#include "appdefs.h"
#include "FirststFilt.h"
#include "fx_sin.h"

extern float memflan[];

class Flanger {

	FirststFilt fl_hpf;

public:

	inline Flanger(void) {
		for (uint16_t i = 0; i < 2048; i++)
			memflan[i] = 0.0f;
	}

	inline void flanger(float *l) {
		float in_f = *l;
		in_f = fl_hpf.filt(in_f);
		if (fl_type == 2)
			phase_f += (fl_rate * 0.3f);
		else
			phase_f += fl_rate;
		phase_f += fl_rate;
		if (phase_f > 1.0f)
			phase_f = 0.0f;

		memflan[posfl] = in_f + accum_f * fl_f;

		if (fl_type) {
			val_f[0] = vector_sin[(uint32_t)(phase_f * 16383.0f)]
					* 0.00006103515625f;
			val_f[1] = vector_sin[((uint32_t)(phase_f * 16383.0f) + 5461)
					& 0x3fff] * 0.00006103515625f;
			val_f[2] = vector_sin[((uint32_t)(phase_f * 16383.0f) + 10922)
					& 0x3fff] * 0.00006103515625f;
		} else {
			val_f[0] = phase_f * 2.0f;
			if (val_f[0] > 1.0f)
				val_f[0] = 2.0f - val_f[0];
		}
		accum_f = 0.0f;
		pos_fl = val_f[0] * fl_with;
		pos = pos_fl;
		fra = pos_fl - pos;
		pos += fl_dela;
		accum_f = memflan[(posfl + pos) & 0x7ff] * (1.0f - fra)
				+ memflan[(posfl + pos + 1) & 0x7ff] * fra;
		if (fl_type == 2) {
			pos_fl = val_f[1] * fl_with;
			pos = pos_fl;
			fra = pos_fl - pos;
			pos += fl_dela;
			accum_f += memflan[(posfl + pos) & 0x7ff] * (1.0f - fra)
					+ memflan[(posfl + pos + 1) & 0x7ff] * fra;
			pos_fl = val_f[2] * fl_with;
			pos = pos_fl;
			fra = pos_fl - pos;
			pos += fl_dela;
			accum_f += memflan[(posfl + pos) & 0x7ff] * (1.0f - fra)
					+ memflan[(posfl + pos + 1) & 0x7ff] * fra;
			accum_f *= 0.5f;
		}
		--posfl &= 0x7ff;

		if (fl_on) {
			if (fl_vol < 1.0f)
				fl_vol += 0.01f;
			else
				fl_vol = 1.0f;
		} else {
			if (fl_vol > 0.0f)
				fl_vol -= 0.01f;
			if (fl_vol < 0.0f)
				fl_vol = 0.0f;
			fl_dry = 1.0f;
		}
		*l = *l * fl_dry + accum_f * fl_wet/* * fl_vol*/;
	}

	inline void fl_param(uint32_t val) {
		uint32_t va = val >> 8;
		val &= 0xff;
		switch (val & 0xff) {
		case 0:
			fl_on = va;
			break;
		case 1:
			if (va <= 63.0f) {
				fl_dry = 1.0f;
				fl_wet = va * (1.0f / 63.0f);
			} else {
				fl_wet = 1.0f;
				fl_dry = (127.0f - va) * (1.0f / 64.0f);
			}
			break;
		case 2:
			fl_type = va;
			break;
		case 3:
			fl_rate = powf(va, 2.0f) * (0.0001f / powf(127.0f, 2.0f));
			break;
		case 4:
			fl_with = (powf(va, 2.0f) * (1023.0f / powf(127.0f, 2.0f)));
			break;
		case 5:
			fl_dela = va * (1024.0f / 127.0f);
			break;
		case 6:
			fl_f = va * (0.9f / 127.0f);
			break;
		case 7:
			fl_hpf.SetHPF(va * (980.0f / 127.0f) + 20.0f);
			break;
		}
	}

private:

	uint8_t fl_on;
	float pos_fl;
	float fra;
	uint32_t pos;
	float phase_f = 0.0f;
	float phase_t;
	float accum_f;
	int posfl;
	float fl_rate = 50.0f;
	float fl_f = 0.0f;
	float fl_vol;
	float fl_with;
	uint32_t fl_dela;
	uint32_t fl_type;
	float val_f[3];
	float fl_dry;
	float fl_wet;
};

#endif /* SRC_APPLICATION_FLANGER_H_ */
