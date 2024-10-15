/*
 * storge.h
 *
 *  Created on: Aug 30, 2014
 *      Author: s
 */

#ifndef STORGE_H_
#define STORGE_H_

#include "stdint.h"
#include "diskio.h"

#ifdef  __cplusplus
 extern "C" {
#endif
    DSTATUS storage_initialize();
    DSTATUS storage_get_status();
	uint8_t storage_get_lun_number();
	uint32_t storage_get_block_count() ;
	uint32_t storage_get_block_size() ;
	DRESULT storage_read ( uint8_t *buf, uint32_t blk_index, uint16_t blk_count);
	DRESULT storage_write ( const uint8_t *buf, uint32_t blk_index, uint16_t blk_count);
	DRESULT storage_ioctl ( uint8_t command , void* params);

#ifdef  __cplusplus
 	 }
#endif

#endif /* STORGE_H_ */
