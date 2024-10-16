#ifndef __SIGMA_H__

#include "appdefs.h"

#define block_size 8

typedef union
{
	uint32_t sample;
	struct
	{
		uint16_t sampleL;
		uint16_t sampleH;
	};
}ad_channel_t;

typedef union
{
	uint32_t sample;
	struct
	{
		uint16_t sampleL;
		uint16_t sampleH;
	};
}da_channel_t;

typedef union
{
	int64_t val;
	struct
	{
		ad_channel_t left;
		ad_channel_t right;
	};
}ad_data_t;

typedef union
{
	int64_t val;
	struct
	{
		da_channel_t left;
		da_channel_t right;
	};
}da_data_t;

extern ad_data_t adc_data[];
extern da_data_t dac_data[];

void adau_transmit (uint8_t* adr , uint32_t size );
void adau_init_ic (void);

void sig_load (float* cab_data , uint8_t* buf);
void sig_volume(float val);

void dsp_mute(void);
void dsp_run(void);
void dsp_clear(void);

void sig_invert(uint8_t val);
void sig_reset_init();
void sig_reset(bool state);
bool sig_reset();

#endif /*__SIGMA_H__*/
