#include "appdefs.h"
#include "AT45DB321.h"
#include "hw_AT45DB321.h"

uint16_t AT45DB321_PageSize = 512;

void AT45DB321_Init(void)
{
	  RCC_AHB1PeriphClockCmd(flash_port_rcc,ENABLE);
	  GPIO_InitTypeDef  GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = flash_port_clk | flash_port_in | flash_port_out;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed ;
	  GPIO_Init(flash_port, &GPIO_InitStructure);

	  RCC_AHB1PeriphClockCmd(flash_cs_rcc,ENABLE);
	  GPIO_SetBits(flash_cs_port,flash_cs);
	  GPIO_InitStructure.GPIO_Pin =  flash_cs;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_Init(flash_cs_port, &GPIO_InitStructure);

	  GPIO_PinAFConfig(flash_port, flash_port_clk_af, flash_af);
	  GPIO_PinAFConfig(flash_port, flash_port_in_af, flash_af);
	  GPIO_PinAFConfig(flash_port, flash_port_out_af, flash_af);

	  flash_spi_rcc_c( flash_spi_rcc, ENABLE );
	  SPI_I2S_DeInit(flash_spi);
	  SPI_InitTypeDef SPI_InitStructure;
	  SPI_StructInit(&SPI_InitStructure);
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;
	  SPI_Init(flash_spi, &SPI_InitStructure);
	  SPI_Cmd(flash_spi, ENABLE);

	AT45DB321_CS1;
    xSPISingleDataReadWrite(0xFF);
    xdelay(10000);

    AT45DB321_SectorProtectionDi();
}

//*****************************************************************************
//
//! \brief Read AT45DB321 state register
//!
//! \param None
//!
//! This function is to Read AT45DB321 state register
//!
//! \return the value of state register
//
//*****************************************************************************
uint16_t AT45DB321_GetState(void)
{
	uint16_t temp;
    AT45DB321_CS0;
    xSPISingleDataReadWrite(AT45DB321_CMD_SRRD);
    temp = xSPISingleDataReadWrite(0xff);
	#ifdef __PANGEA_MINI_REV2__
    	xSPISingleDataReadWrite(0xff);
	#endif
    AT45DB321_CS1;
    return temp;
}

//*****************************************************************************
//
//! \brief To check and wait device ready
//!
//! \param None
//!
//! This function is to check and wait device ready. If the device still don't get
//! ready after 300 cycles of retry, the function will return 0 for not ready,
//! otherwise return 1 for ready.
//!
//! \return 0: not ready  ;  1: ready
//
//*****************************************************************************
uint8_t AT45DB321_WaitReady(void)
{
    uint16_t state_reg = 0x00;
    uint16_t retry = 0;
    do
    {
        state_reg = AT45DB321_GetState();
        retry++;
    }
    while((state_reg & AT45DB321_IDLE) == 0 && retry < 300);
    if(retry >= 300)return 0;
    else return 1;
}

//*****************************************************************************
//
//! \brief Main Memory Page to Buffer Transfer
//!
//! \param ucBufferNum specify the buffer 1 or 2 as the destination.
//! \param usPageAddr specify the page address which you want to read.
//!
//! This function is to transfer data from specified page address of main memory
//! to specified AT45DB321 internal buffer.
//!
//! \return None
//
//*****************************************************************************


void AT45DB321_Mm_to_Buf(uint16_t ucBufferNum, uint16_t usPageAddr)
{
    while(!AT45DB321_WaitReady());
    if (usPageAddr < AT45DB321_PAGES)
    {
        AT45DB321_CS0;
        if (ucBufferNum == AT45DB321_BUF1)
        {
            xSPISingleDataReadWrite(AT45DB321_CMD_MTB1);
        }
        else
        {
            xSPISingleDataReadWrite(AT45DB321_CMD_MTB2);
        }
        xSPISingleDataReadWrite((uint16_t)(usPageAddr >> 6));
        xSPISingleDataReadWrite((uint16_t)((usPageAddr & 0x003F) << 2));
        xSPISingleDataReadWrite(0x00);
        AT45DB321_CS1;
    }
}

//*****************************************************************************
//
//! \brief Buffer to Main Memory Page Program with Built-in Erase
//!
//! \param ucBufferNum specify the buffer 1 or 2 as the source.
//! \param usPageAddr specify the page address which you want to write.
//!
//! This function is to write data from specified internal buffer to the
//! specified main memory page address with built-in erase.
//!
//! \return None
//
//*****************************************************************************
void AT45DB321_Buf_to_Mm(uint16_t ucBufferNum, uint16_t usPageAddr)
{
    while(!AT45DB321_WaitReady());
    if (usPageAddr < AT45DB321_PAGES)
    {
        AT45DB321_CS0;
        if (ucBufferNum == AT45DB321_BUF1)xSPISingleDataReadWrite(AT45DB321_CMD_B1TMW);
        else xSPISingleDataReadWrite(AT45DB321_CMD_B2TMW);
        xSPISingleDataReadWrite((uint16_t)(usPageAddr >> 6));
        xSPISingleDataReadWrite((uint16_t)((usPageAddr & 0x003F) << 2));
        xSPISingleDataReadWrite(0x00);
        AT45DB321_CS1;
    }
}

//*****************************************************************************
//
//! \brief Read data in specified AT45DB321 internal buffer
//!
//! \param ucBufferNum specify the buffer 1 or 2 as the source.
//! \param pucBuffer to store the data read out from internal buffer
//! \param ulReadAddr specify the start address in the buffer which you want to read.
//! \param ulNumByteToRead specify how many bytes to read.
//!
//! This function is to read data from specified internal buffer. If you have
//! just a little data to be stored(less than AT45DB321_PageSize), you can temporarily
//! store them in the AT45DB321 internal buffer.It can be more fast than read write from
//! main memory
//!
//! \return None
//
//*****************************************************************************
void AT45DB321_ReadBuffer(uint16_t ucBufferNum, uint8_t *pucBuffer,
                          uint32_t ulReadAddr, uint32_t ulNumByteToRead)
{
    if (ulReadAddr + ulNumByteToRead <= AT45DB321_PageSize)
    {
        while(!AT45DB321_WaitReady());
        AT45DB321_CS0;
        if (ucBufferNum == AT45DB321_BUF1)
        {
            xSPISingleDataReadWrite(AT45DB321_CMD_BF1R);
        }
        else
        {
            xSPISingleDataReadWrite(AT45DB321_CMD_BF2R);
        }
        xSPISingleDataReadWrite(0x00);
        xSPISingleDataReadWrite(0x00);
        xSPISingleDataReadWrite(0x00);
        xSPISingleDataReadWrite(0x00);
        xSPIDataRead(pucBuffer , ulNumByteToRead);
        AT45DB321_CS1;
    }
}

//*****************************************************************************
//
//! \brief Write data to specified AT45DB321 internal buffer
//!
//! \param ucBufferNum specify the buffer 1 or 2 as the destination.
//! \param pucBuffer to store the data need to be written.
//! \param ulWriteAddr specify the start address in the buffer which you want to write.
//! \param ulNumByteToRead specify how many bytes to write.
//!
//! This function is to write data to specified internal buffer. If you have
//! just a little data to be stored(less than AT45DB321_PageSize), you can temporarily
//! store them in the AT45DB321 internal buffer.It can be more fast than read write from
//! main memory.This write function doesn't affect the content in main memory.
//!
//! \return None
//
//*****************************************************************************
void AT45DB321_WriteBuffer(uint16_t ucBufferNum, const uint8_t *pucBuffer,
                           uint32_t ulWriteAddr, uint32_t ulNumByteToWrite)
{

    if (ulWriteAddr + ulNumByteToWrite <= AT45DB321_PageSize)
    {
        while(!AT45DB321_WaitReady());
        AT45DB321_CS0;
        if (ucBufferNum == AT45DB321_BUF1)
        {
            xSPISingleDataReadWrite(AT45DB321_CMD_BF1W);
        }
        else
        {
            xSPISingleDataReadWrite(AT45DB321_CMD_BF2W);
        }
        xSPISingleDataReadWrite(0x00);
        xSPISingleDataReadWrite(0x00);
        xSPISingleDataReadWrite(0x00);
        xSPIDataWrite(pucBuffer, ulNumByteToWrite);

        AT45DB321_CS1;
    }
}
void AT45DB321_SectorProtectionDi(void)
{
    AT45DB321_CS0;
    xSPIDataWrite((uint8_t*)AT45DB321_CMD_DSSP, 4);
    AT45DB321_CS1;
}
//---------------------------------------------------------------------

void AT45DB321_EraseBlock(uint8_t usBlockAddr)
{
    //if(AT45DB081_WaitReady() == 0) return;
    //if(usBlockAddr >= AT45DB081_BLOCKS) return;
    AT45DB321_CS0;
    xSPISingleDataReadWrite(AT45DB321_CMD_ERBL);
    xSPISingleDataReadWrite((uint16_t)(usBlockAddr >> 3));
    xSPISingleDataReadWrite((uint16_t)((usBlockAddr & 0x000F) << 5));
    xSPISingleDataReadWrite(0x00);
    AT45DB321_CS1;
}
void AT45DB321_Buf_to_Mm_ne(uint16_t ucBufferNum, uint16_t usPageAddr)
{
    while(!AT45DB321_WaitReady());
    if (usPageAddr < AT45DB321_PAGES)
    {
        AT45DB321_CS0;
        if (ucBufferNum == AT45DB321_BUF1)xSPISingleDataReadWrite(AT45DB321_CMD_B1TMO);
        else xSPISingleDataReadWrite(AT45DB321_CMD_B2TMO);
        xSPISingleDataReadWrite((uint16_t)(usPageAddr >> 6));
        xSPISingleDataReadWrite((uint16_t)((usPageAddr & 0x003F) << 2));
        xSPISingleDataReadWrite(0x00);
        AT45DB321_CS1;
    }
}

