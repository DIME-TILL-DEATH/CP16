#ifndef __SDCARD_DISKIO_H__
#define __SDCARD_DISKIO_H__

#ifdef __cplusplus
	extern "C" {
#endif


#include <stdint.h>

/*
void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void);
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
*/

//-------------------------------------------------------------------

uint8_t sdcard_disk_get_lun_number();
uint32_t sdcard_disk_get_block_count();
uint32_t sdcard_disk_get_block_size();


DSTATUS sdcard_disk_initialize ();
DSTATUS sdcard_disk_status ();
DRESULT sdcard_disk_read ( BYTE* buffer , DWORD sector_number, uint16_t sector_count);
DRESULT sdcard_disk_write ( const BYTE* buffer, DWORD sector_number, uint16_t sector_count );
DRESULT sdcard_disk_ioctl (BYTE command , void* params);

#ifdef __cplusplus
	}
#endif

#endif  /*__SDCARD_DISKIO_H__*/
