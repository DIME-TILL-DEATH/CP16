#include "diskio.h"
#include "sdio_sd.h"


uint8_t sdcard_disk_get_lun_number()
{
	return 1 ;
}
uint32_t sdcard_disk_get_block_count()
{
	SD_CardInfo ci ;
	SD_GetCardInfo(&ci);
	return ci.CardCapacity / ci.CardBlockSize ;
}
uint32_t sdcard_disk_get_block_size()
{
	SD_CardInfo ci ;
	SD_GetCardInfo(&ci);
	return ci.CardBlockSize ;
}
//---------------------------------------------------------

static DSTATUS sd_status = STA_NOINIT ;

DSTATUS sdcard_disk_initialize ()
{
            if (  !(sd_status & STA_NOINIT) )
            	return sd_status ;
            SD_Error Status = SD_OK;
            uint8_t ddd = 0;
            while(SD_Init() != SD_OK)
              {
                 SD_DeInit();
                 ddd++;
                 if(!++ddd)
                 {
                	 Status = SD_ERROR;
                	 break;
                 }
              }
            if ( Status == SD_OK )sd_status &= ~STA_NOINIT ;

  return sd_status /*STA_NOINIT	STA_NODISK	 STA_PROTECT*/	;
}
//---------------------------------------------------------
DSTATUS sdcard_disk_status ()
{
	return sd_status /*STA_NOINIT	STA_NODISK	 STA_PROTECT*/	;
}
//---------------------------------------------------------
DRESULT sdcard_disk_read (BYTE* buffer , DWORD sector_number, uint16_t sector_count)
{
	SD_Error Status = SD_ReadMultiBlocks( buffer , sector_number*512 , 512, sector_count) ;
    Status = SD_WaitReadOperation();
    (void)Status ;
    while(SD_GetStatus() != SD_TRANSFER_OK);
	return RES_OK ;
}
//---------------------------------------------------------
DRESULT sdcard_disk_write (const BYTE* buffer, DWORD sector_number, uint16_t sector_count )
{
	SD_Error Status = SD_WriteMultiBlocks( buffer , sector_number*512 , 512, sector_count) ;
    Status = SD_WaitWriteOperation();
    (void)Status ;
    while(SD_GetStatus() != SD_TRANSFER_OK);
	return RES_OK ;
}
//---------------------------------------------------------
DRESULT sdcard_disk_ioctl (BYTE command , void* params)
{
	switch (command)
	{
		case CTRL_SYNC:
			return RES_OK ;
			break ;
		case GET_SECTOR_SIZE:
			*((unsigned int*)params) = 0;
			return RES_PARERR ;
			break ;
		case GET_SECTOR_COUNT:
			*((DWORD*)params) = sdcard_disk_get_block_count() ;
			break ;
		case GET_BLOCK_SIZE:
			*((DWORD*)params) = sdcard_disk_get_block_size() ;
			break ;
		case CTRL_ERASE_SECTOR:
			return RES_ERROR ;
		default :
			return RES_PARERR ;
	}
	return RES_OK ;
}
//---------------------------------------------------------
