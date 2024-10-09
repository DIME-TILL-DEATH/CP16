
#include "appdefs.h"
#include  "storage.h"

#include "AT45DB321.h"


#define SIZE 4*(1024-1)*1024
#define BLK_SIZE  512
#define BLK_COUNT SIZE/BLK_SIZE
#define STORAGE_LUN_NBR  1


        DSTATUS storage_initialize()
	{
          return RES_OK ;
	}

        DSTATUS storage_get_status()
        {
          return RES_OK ;
        }

	uint8_t storage_get_lun_number() { return  STORAGE_LUN_NBR ; }

	uint32_t storage_get_block_count() { return BLK_COUNT ; }
	uint32_t storage_get_block_size() { return BLK_SIZE ; }


	DRESULT storage_read (uint8_t *buf, uint32_t blk_index, uint16_t blk_count)
	{
	for(uint16_t i = 0 ; i < blk_count ; i++)
			{
		      AT45DB321_Mm_to_Buf(1,blk_index + i);
		      AT45DB321_ReadBuffer(1 , buf + i*BLK_SIZE, 0 , BLK_SIZE);
			}
		return RES_OK ;
	}

	DRESULT storage_write (const uint8_t *buf, uint32_t blk_index, uint16_t blk_count)
	{
		//memcpy ( storage + blk_index*BLK_SIZE , buf , blk_count*BLK_SIZE ) ;
		for(uint16_t i = 0 ; i < blk_count ; i++)
		{
			AT45DB321_WriteBuffer(1 , buf + i*BLK_SIZE , 0 , BLK_SIZE);
			AT45DB321_Buf_to_Mm(1,blk_index + i);
			while(AT45DB321_WaitReady() == 0);
		}

		return RES_OK ;
	}


	DRESULT storage_ioctl (uint8_t command , void* params)
	{
		switch (command)
		{
			case CTRL_SYNC:
				return RES_OK ;
				break ;
			case GET_SECTOR_SIZE:
				*((unsigned int*)params) = 0 ;
				return RES_PARERR ;
				break ;
			case GET_SECTOR_COUNT:
				*((DWORD*)params) =  storage_get_block_count() ;
				break ;
			case GET_BLOCK_SIZE:
				*((DWORD*)params) =  storage_get_block_size() ;
				break ;
			case CTRL_ERASE_SECTOR:
				return RES_ERROR ;
			default :
				return RES_PARERR ;
		}
		return RES_OK ;

	}


#include "sdcard_diskio.h"

void disk_set_io_fn(BYTE drv , storage_fn_t* storage_fn)
{
  switch( drv )
  {
     case 0:
        storage_fn[drv].storage_get_status = storage_get_status ;
        storage_fn[drv].storage_initialize = storage_initialize ;
        storage_fn[drv].storage_ioctl = storage_ioctl ;
        storage_fn[drv].storage_read = storage_read ;
        storage_fn[drv].storage_write = storage_write ;
        break;/*
     case 1:
        storage_fn[drv].storage_get_status = sdcard_disk_status ;
        storage_fn[drv].storage_initialize = sdcard_disk_initialize ;
        storage_fn[drv].storage_ioctl = sdcard_disk_ioctl ;
        storage_fn[drv].storage_read = sdcard_disk_read ;
        storage_fn[drv].storage_write = sdcard_disk_write ;
        break;*/
  }
}
