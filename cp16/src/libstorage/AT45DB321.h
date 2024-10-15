#ifndef __AT45DB321_H__
#define __AT45DB321_H__

	extern void AT45DB321_Init(void);
    extern uint16_t AT45DB321_GetState(void);
    extern uint8_t AT45DB321_WaitReady(void);
    extern void AT45DB321_Buf_to_Mm(uint16_t ucBufferNum, uint16_t usPageAddr);
    extern void AT45DB321_Mm_to_Buf(uint16_t ucBufferNum, uint16_t usPageAddr);
    extern void AT45DB321_ReadBuffer(uint16_t ucBufferNum, uint8_t *pucBuffer,
            uint32_t ulReadAddr, uint32_t ulNumByteToRead);
    extern void AT45DB321_WriteBuffer(uint16_t ucBufferNum, const uint8_t *pucBuffer,
            uint32_t ulWriteAddr, uint32_t ulNumByteToWrite);
    extern void AT45DB321_SectorProtectionDi(void);
    //-----------
    extern void AT45DB081_EraseBlock(unsigned short ulBlockAddr);
    extern void AT45DB321_Buf_to_Mm_ne(uint16_t ucBufferNum, uint16_t usPageAddr);
    //----------------------------------------
    inline void __attribute__ ((always_inline)) xdelay(uint32_t i)
    {
      for (uint32_t ii = 0 ; ii < i ; ii++)NOP();
    }

    inline uint16_t __attribute__ ((always_inline)) xSPISingleDataReadWrite(uint16_t dat)
    {
        while(SPI_I2S_GetFlagStatus(flash_spi,SPI_I2S_FLAG_TXE) == 0);
        SPI_I2S_SendData(flash_spi,dat);
        while(SPI_I2S_GetFlagStatus(flash_spi,SPI_I2S_FLAG_RXNE) == 0);
        return SPI_I2S_ReceiveData(flash_spi);
    }
    inline void __attribute__ ((always_inline)) xSPIDataRead(uint8_t* adr , uint16_t num)
    {
    	for(uint16_t i = 0 ; i < num ; i++)
    	{
    		while(SPI_I2S_GetFlagStatus(flash_spi,SPI_I2S_FLAG_TXE) == 0);
    		SPI_I2S_SendData(flash_spi,0xff);
    		while(SPI_I2S_GetFlagStatus(flash_spi,SPI_I2S_FLAG_RXNE) == 0);
    		adr[i] = SPI_I2S_ReceiveData(flash_spi);
    	}
    }
    inline void __attribute__ ((always_inline)) xSPIDataWrite(const uint8_t* adr , uint16_t num)
    {
        volatile uint16_t j;
    	for(uint16_t i = 0 ; i < num ; i++)
    	{
    		while(SPI_I2S_GetFlagStatus(flash_spi,SPI_I2S_FLAG_TXE) == 0);
    		SPI_I2S_SendData(flash_spi,adr[i]);
    		while(SPI_I2S_GetFlagStatus(flash_spi,SPI_I2S_FLAG_RXNE) == 0);
    		j = SPI_I2S_ReceiveData(flash_spi);
    		(void)j ;
    	}
    }


#endif //__AT45DB321_H__

