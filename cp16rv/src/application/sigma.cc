#include "appdefs.h"
#include "sigma.h"
#include "sigma_cod.h"
#include "init.h"

ad_data_t adc_data ;
da_data_t dac_data ;

ad_data_t& init_get_adc_data()
{
  return  adc_data;
}

da_data_t& init_get_dac_data()
{
  return  dac_data;
}

void adau_transmit (uint8_t* adr , uint32_t size )
{
  GPIO_ResetBits(adau_spi_cs_port,adau_spi_cs);
  SPI_I2S_SendData(adau_com_spi,0);
  while(SPI_I2S_GetFlagStatus(adau_com_spi,SPI_I2S_FLAG_TXE) == 0);
  for(uint16_t i = 0 ; i < size ; i++)
  {
      SPI_I2S_SendData(adau_com_spi,adr[i]);
      while(SPI_I2S_GetFlagStatus(adau_com_spi,SPI_I2S_FLAG_TXE) == 0);
  }
  while(SPI_I2S_GetFlagStatus(adau_com_spi,SPI_I2S_FLAG_BSY) == 1);
  GPIO_SetBits(adau_spi_cs_port,adau_spi_cs);
}
void dsp_run(void)
{
	adau_transmit ((uint8_t*)sig_run,4 );
}
void dsp_mute(void)
{
	adau_transmit ((uint8_t*)sig_mute_1,4 );
}
void dsp_clear(void)
{
	adau_transmit ((uint8_t*)sig_clear,4 );
}

void adau_init_ic (void)
{
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	  NVIC_InitTypeDef NVIC_InitStructure;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn ;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_Init( &NVIC_InitStructure );

	  RCC_AHB1PeriphClockCmd(adau_spi_port_rcc | adau_spi_clk_port_rcc | adau_spi_cs_port_rcc |
			                 adau_i2s_port_rcc | adau_i2s_d_port_rcc | adau_i2s_mclk_rcc , ENABLE);

	  // init I2S interface
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

	  // init SPI interface
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
	  I2S_FullDuplexConfig(adau_i2s_spi_ext,&I2S_InitStructure);
	  SPI_I2S_DMACmd (adau_i2s_spi,SPI_I2S_DMAReq_Tx,ENABLE);
	  SPI_I2S_DMACmd (adau_i2s_spi_ext ,SPI_I2S_DMAReq_Rx,ENABLE);

	  RCC->PLLI2SCFGR = (258 << 6) | (3 << 28); // Fs = 48000
	  adau_i2s_spi->I2SPR = 3 | (1 << 8) | (1 << 9);

	  I2S_Cmd (adau_i2s_spi , ENABLE );
	  I2S_Cmd (adau_i2s_spi_ext , ENABLE );
	  RCC_PLLI2SCmd (ENABLE);

	  adau_com_spi_rcc_c( adau_com_spi_rcc, ENABLE );
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

	  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_DMA1 , ENABLE);
	  DMA_InitTypeDef DMA_InitStructure;
	  DMA_StructInit(&DMA_InitStructure);

	  DMA_Cmd(DMA1_Stream3, DISABLE);
	  DMA_Cmd(DMA1_Stream4, DISABLE);
      DMA_DeInit (DMA1_Stream3 );
	  DMA_DeInit (DMA1_Stream4 );

	  DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&adau_i2s_spi_ext->DR;
	  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&adc_data;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  DMA_InitStructure.DMA_BufferSize = 4;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  DMA_Init(DMA1_Stream3, &DMA_InitStructure);
	  DMA_ITConfig ( DMA1_Stream3 , DMA_IT_TC , ENABLE);
	  DMA_Cmd(DMA1_Stream3, ENABLE);

	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&adau_i2s_spi->DR;
	  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&dac_data;
	  DMA_InitStructure.DMA_BufferSize = 4;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	  DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	  DMA_Cmd(DMA1_Stream4, ENABLE);

	  dela(0x1ffffff);
  for(uint8_t i = 0; i<3;i++)
    {
      GPIO_ResetBits(adau_spi_cs_port,adau_spi_cs);
      dela(0xff);
      GPIO_SetBits(adau_spi_cs_port,adau_spi_cs);
      dela(0xff);
    }
  adau_transmit((uint8_t*)Program_Data_init,5122);
  adau_transmit((uint8_t*)Param_Data_init,4098);
  adau_transmit((uint8_t*)R3_HWCONFIGURATION_IC_1_Default,26);
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
void sig_load (float* cab_data , uint8_t* buf)
{
  for(size_t i = 0 ; i < 984 ; i++)
    {
      to523(cab_data[i],(uint8_t *) buf + i*4 + 2);
    }
  GPIO_ResetBits(adau_spi_cs_port,adau_spi_cs);
  adau_transmit((uint8_t*)buf,3938);
}
void sig_load1 (uint8_t* buf)
{
  GPIO_ResetBits(adau_spi_cs_port,adau_spi_cs);
  adau_transmit((uint8_t*)buf,3938);
}

uint8_t vol_hex[6] = {0,1};
void sig_volume(float val)
{
  to523(val*0.3, (uint8_t *) vol_hex + 2);
  adau_transmit((uint8_t*)vol_hex , 6);
}
void sig_reset_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(DSP_RESET_RCC_GPIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = DSP_RESET_PIN  ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
    GPIO_Init(DSP_RESET_PORT, &GPIO_InitStructure);

    GPIO_ResetBits(DSP_RESET_PORT,DSP_RESET_PIN);
}

void sig_reset(bool state)
{
  if(state)
	  GPIO_ResetBits(DSP_RESET_PORT,DSP_RESET_PIN);
  else
	  GPIO_SetBits(DSP_RESET_PORT,DSP_RESET_PIN);
}

