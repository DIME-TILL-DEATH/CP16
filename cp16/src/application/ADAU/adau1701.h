#ifndef __SIGMA_H__

#include "appdefs.h"

#define block_size 32//8

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

typedef enum
{
	PA_CLASSIC=0,
	RV_CLASSIC
}ADAU_fw_type_t;

void adau_init_reset_pin();
void adau_init_ic();

void adau_load_firmware(ADAU_fw_type_t fw_type);

void adau_mute(void);
void adau_run(void);
void adau_clear(void);

void adau_dma_transmit(uint16_t address, uint8_t* data, uint32_t size);
void adau_transmit(uint16_t address, uint8_t* data, uint32_t size);

void sig_invert(uint8_t val);
void sig_reset(bool state);
void sig_load(float* cab_data);

#define DSP_FIR_SIZE 983
#define DSP_FIR_ADDRESS 0x0000

#define DSP_DATA_ADDRESS 0x0000
#define DSP_PROGRAMM_ADDRESS 0x0400
#define DSP_ITF0_ADDRESS 0x0800
#define DSP_CTRL_ADDRESS 0x081c

const uint8_t R3_HWCONFIGURATION_IC_1_Default[] =
{
		0x00, 0x058, //0x18, 		//2076 - dsp core control
		0x08, 				//2077 - reserved
		0x00, 0x00,	 		//2078 - serial output control
		0x00, 	    		//2079 - serial input control
		0x44, 0x40, 0x00, 	//2080 MP
		0x44, 0x04, 0x00, 	//2081 MP
		0x00, 0x00, 	  	//2082
		0x00, 0x00, 		//2083
		0x00, 0x00, 		//2084
		0x00, 0x00, 		//2085
		0x00, 0x00, 		//2086
		0x00, 0x01			//2087
};

#endif /*__SIGMA_H__*/
