#include "appdefs.h"
#include "adau1701.h"
//#ifdef __PA_VERSION__
#include "adau_programm_pa-rv.h"
//#else
//#include "sigma_cod_rv.h"
//#endif
#include "gpio.h"


ad_data_t adc_data[block_size * 2];
da_data_t dac_data[block_size * 2];

#define SPI3_DMA_BUFFER_SIZE 16
uint8_t spi_com_buffer[SPI3_DMA_BUFFER_SIZE];

void adau_transmit(uint16_t address, uint8_t* data, uint32_t size)
{
	GPIO_ResetBits(adau_spi_cs_port, adau_spi_cs);

	SPI_I2S_SendData(adau_com_spi, 0x00);
	while(SPI_I2S_GetFlagStatus(adau_com_spi,SPI_I2S_FLAG_TXE) == 0);

	SPI_I2S_SendData(adau_com_spi, (address>>8) & 0xFF);
	while(SPI_I2S_GetFlagStatus(adau_com_spi,SPI_I2S_FLAG_TXE) == 0);

	SPI_I2S_SendData(adau_com_spi, address & 0xFF);
	while(SPI_I2S_GetFlagStatus(adau_com_spi,SPI_I2S_FLAG_TXE) == 0);

	for(uint16_t i = 0 ; i < size ; i++)
	{
		SPI_I2S_SendData(adau_com_spi, data[i]);
		while(SPI_I2S_GetFlagStatus(adau_com_spi, SPI_I2S_FLAG_TXE) == 0);
	}

	while(SPI_I2S_GetFlagStatus(adau_com_spi, SPI_I2S_FLAG_BSY) == 1);

	GPIO_SetBits(adau_spi_cs_port, adau_spi_cs);
}

void adau_run(void)
{
	uint8_t sig_run[]={0, 0x1c};
	adau_transmit(DSP_CTRL_ADDRESS, (uint8_t*)sig_run, 2);
}

void adau_mute(void)
{
	uint8_t sig_mute[]={0, 0x14};
	adau_transmit(DSP_CTRL_ADDRESS, (uint8_t*)sig_mute, 2);
}

void adau_clear(void)
{
	uint8_t sig_clear[]={0, 0};
	adau_transmit(DSP_CTRL_ADDRESS, (uint8_t*)sig_clear, 2);
}

void adau_init_reset_pin()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(DSP_RESET_RCC_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DSP_RESET_PIN  ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
	GPIO_Init(DSP_RESET_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(DSP_RESET_PORT, DSP_RESET_PIN);
}

void adau_init_ic (void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn ;
	NVIC_Init(&NVIC_InitStructure);


	RCC_AHB1PeriphClockCmd(adau_spi_port_rcc | adau_spi_clk_port_rcc | adau_spi_cs_port_rcc |
						 adau_i2s_port_rcc | adau_i2s_d_port_rcc | adau_i2s_mclk_rcc , ENABLE);

	// init GPIO I2S interface
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = adau_i2s_ws  | adau_i2s_clk; // I2S
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed ;
	GPIO_Init(adau_i2s_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = adau_i2s_in | adau_i2s_out; // I2S_D
	GPIO_Init(adau_i2s_d_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = adau_i2s_mclk; // I2S_MCLK
	GPIO_Init(adau_i2s_mclk_port, &GPIO_InitStructure);

	// init GPIO SPI interface
	GPIO_InitStructure.GPIO_Pin = adau_spi_clk; // ADAU SPI
	GPIO_Init(adau_spi_clk_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = adau_spi_out; // ADAU SPI
	GPIO_Init(adau_spi_port, &GPIO_InitStructure);

	GPIO_SetBits(adau_spi_cs_port,adau_spi_cs);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = adau_spi_cs;     // ADAU CS
	GPIO_Init(adau_spi_cs_port, &GPIO_InitStructure);

	GPIO_PinAFConfig(adau_i2s_port, adau_i2s_ws_af, adau_i2s_af);
	GPIO_PinAFConfig(adau_i2s_port, adau_i2s_clk_af, adau_i2s_af);
	GPIO_PinAFConfig(adau_i2s_d_port, adau_i2s_in_af, adau_i2s_ext_af);
	GPIO_PinAFConfig(adau_i2s_d_port, adau_i2s_out_af, adau_i2s_af);
	GPIO_PinAFConfig(adau_i2s_mclk_port, adau_i2s_mclk_af, adau_i2s_af);
	GPIO_PinAFConfig(adau_spi_clk_port, adau_spi_clk_af, adau_spi_af);
	GPIO_PinAFConfig(adau_spi_port, adau_spi_out_af, adau_spi_af);

	RCC_PLLI2SCmd (DISABLE);
	RCC_APB1PeriphClockCmd( adau_i2s_spi_rcc, ENABLE );

	SPI_I2S_DeInit(adau_i2s_spi);
	SPI_I2S_DeInit(adau_i2s_spi_ext);

	I2S_InitTypeDef I2S_InitStructure;
	I2S_StructInit(&I2S_InitStructure);

	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
	I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_24b;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
	I2S_Init(adau_i2s_spi, &I2S_InitStructure);
	I2S_FullDuplexConfig(adau_i2s_spi_ext, &I2S_InitStructure);

	SPI_I2S_ITConfig(adau_i2s_spi_ext, SPI_I2S_IT_RXNE, ENABLE);

	SPI_I2S_DMACmd(adau_i2s_spi, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_I2S_DMACmd(adau_i2s_spi_ext, SPI_I2S_DMAReq_Rx, ENABLE);

	RCC->PLLI2SCFGR = (258 << 6) | (3 << 28); // Fs = 48000
	adau_i2s_spi->I2SPR = 3 | (1 << 8) | (1 << 9);

	I2S_Cmd (adau_i2s_spi , ENABLE );
	I2S_Cmd (adau_i2s_spi_ext , ENABLE );
	RCC_PLLI2SCmd (ENABLE);

	// DMA I2S
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_DMA1, ENABLE);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream3, DISABLE);
	DMA_Cmd(DMA1_Stream4, DISABLE);
	DMA_DeInit(DMA1_Stream3);
	DMA_DeInit(DMA1_Stream4);

	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&adau_i2s_spi_ext->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)adc_data;
	DMA_InitStructure.DMA_BufferSize = 4 * block_size * 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_Init(DMA1_Stream3, &DMA_InitStructure);

	DMA_ITConfig (DMA1_Stream3, DMA_IT_TC , ENABLE);
	DMA_ITConfig (DMA1_Stream3, DMA_IT_HT , ENABLE);
	DMA_Cmd(DMA1_Stream3, ENABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&adau_i2s_spi->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)dac_data;
	DMA_InitStructure.DMA_BufferSize = 4 * block_size * 2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream4, ENABLE);

	// SPI(control)
	adau_com_spi_rcc_c(adau_com_spi_rcc, ENABLE);
	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);

	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(adau_com_spi, &SPI_InitStructure);
	SPI_Cmd(adau_com_spi, ENABLE);
	SPI_I2S_DMACmd(adau_com_spi, SPI_I2S_DMAReq_Tx, ENABLE);

	// DMA SPI(control)
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_DMA1, ENABLE);
	DMA_StructInit(&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_DeInit(DMA1_Stream5);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&adau_com_spi->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)spi_com_buffer;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC , ENABLE);

	NVIC_DisableIRQ(SPI2_IRQn);
	NVIC_DisableIRQ(SPI3_IRQn);
	NVIC_DisableIRQ(DMA1_Stream5_IRQn);
	delay_nop(0x1ffffff);

	for(uint8_t i = 0; i<3;i++)
	{
		GPIO_ResetBits(adau_spi_cs_port,adau_spi_cs);
		delay_nop(0xff);
		GPIO_SetBits(adau_spi_cs_port,adau_spi_cs);
		delay_nop(0xff);
	}

	uint8_t* param_data_init = (uint8_t*)malloc(1024 * 4);
	kgp_sdk_libc::memset(param_data_init, 0, 1024 * 4);
	adau_transmit(DSP_DATA_ADDRESS, (uint8_t*)Param_Data_IC_1, 1024 * 4);
	free(param_data_init);

	adau_transmit(DSP_PROGRAMM_ADDRESS, (uint8_t*)adau_program_parv, 1024 * 5);
	adau_transmit(DSP_CTRL_ADDRESS, (uint8_t*)R3_HWCONFIGURATION_IC_1_Default, sizeof(R3_HWCONFIGURATION_IC_1_Default));

	adau_transmit(DSP_SAFELOAD_ADDR0_ADDRESS, (uint8_t*)HWSF_ADDR0, sizeof(HWSF_ADDR0));

	kgp_sdk_libc::memset(spi_com_buffer, 0, SPI3_DMA_BUFFER_SIZE);
}

void adau_dma_transmit(uint16_t address, const void* data, uint32_t size)
{
	GPIO_ResetBits(adau_spi_cs_port, adau_spi_cs);

	spi_com_buffer[0] = 0x00;
	spi_com_buffer[1] = (address>>8) & 0xFF;
	spi_com_buffer[2] = address & 0xFF;

	kgp_sdk_libc::memcpy(spi_com_buffer+3, data, size);
	DMA_SetCurrDataCounter(DMA1_Stream5, size+3);

	DMA_Cmd(DMA1_Stream5, ENABLE);
}

bool send_ist = 0;
extern "C" void DMA1_Stream5_IRQHandler()
{
	DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);

	while(SPI_I2S_GetFlagStatus(adau_com_spi,SPI_I2S_FLAG_BSY) == 1);
	delay_nop(0xFF);

	GPIO_SetBits(adau_spi_cs_port, adau_spi_cs);

	if(send_ist)
	{
		adau_dma_transmit(DSP_CTRL_ADDRESS, (uint8_t*)HWCONTROL_IST, sizeof(HWCONTROL_IST));
		send_ist = false;
	}
}

void adau_load_firmware(ADAU_fw_type_t fw_type)
{

}

void to523(float param_dec , uint8_t* param_hex)
{
	long param223;
	long param227;
	param223 = param_dec * (1<<23);
	param227 = param223 + (1<<27);
	param_hex[3] = (uint8_t)param227;
	param_hex[2] = (uint8_t)(param227>>8);
	param_hex[1] = (uint8_t)(param227>>16);
	param_hex[0] = (uint8_t)(param227>>24);
	param_hex[0] = param_hex[0] ^ 0x08;
}

uint8_t converted_impulse_buffer[DSP_FIR_SIZE * 4];
void dsp_upload_ir (float* cab_data)
{
	for(size_t i = 0 ; i < DSP_FIR_SIZE ; i++)
	{
		to523(cab_data[i], converted_impulse_buffer + i*4);
	}
	NVIC_DisableIRQ(SPI2_IRQn);
	while(SPI_I2S_GetFlagStatus(adau_com_spi, SPI_I2S_FLAG_BSY) == 1); //wait for SPI complete action

	adau_transmit(DSP_FIR_ADDRESS, (uint8_t*)converted_impulse_buffer, DSP_FIR_SIZE * 4);
	NVIC_EnableIRQ(SPI2_IRQn);
}

void sig_reset(bool state)
{
	if(state)
		GPIO_ResetBits(DSP_RESET_PORT, DSP_RESET_PIN);
	else
		GPIO_SetBits(DSP_RESET_PORT, DSP_RESET_PIN);
}

void sig_invert(uint8_t val)
{
	uint8_t buf[] = {0, 0x80, 0, 0};
	if(val) buf[0] = 0xff;
	adau_transmit(0x03d7, (uint8_t*)buf, 4);
}
