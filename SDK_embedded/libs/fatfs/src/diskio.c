/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ffconf.h"          // use for _VOLUMS


/* Definitions of physical drive number for each media */
/*#define ATA		0
#define MMC		1
#define USB		2
#define EXT_DATAFLASH   3
#define SDCARD          4*/

//typedef enum { pdnInternalDataFlash = 0 , pdnExternalDataFlash, pdnSDCard, pdnInternalRAM, pdnCount } physical_drive_type_t ;


/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2012        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "ffconf.h"

storage_fn_t  storage_fn[_VOLUMES] ;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (BYTE drv)
{
        disk_set_io_fn( drv , storage_fn) ;
        DSTATUS stat ;
	int result;
	(void)result ;
	stat = storage_fn[drv].storage_initialize();
	return stat ;
}
//-------------------------------------------------------------------------
DSTATUS disk_deinitialize (BYTE drv)
{
        DSTATUS stat ;
	int result;
	(void)result ;
	stat = storage_fn[drv].storage_deinitialize();
	return stat ;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = 0 ;
	storage_fn[drv].storage_get_status();
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res ;
	res =  storage_fn[drv].storage_read (buff, (DWORD)sector, count);
	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res;
	res =  storage_fn[drv].storage_write (buff, (DWORD)sector, count);
	return res ;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res ;
	res =  storage_fn[drv].storage_ioctl (ctrl,buff);
	return res ;
}
#endif
