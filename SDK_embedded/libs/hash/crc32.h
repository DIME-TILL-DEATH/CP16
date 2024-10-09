#ifndef _CRC32_H_
#define _CRC32_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// block version of CRC32
uint_least32_t crc32(const unsigned char * buf, size_t len);

// stream version of CRC32
void crc32_init( uint_least32_t* crc);
void crc32_update( uint_least32_t* crc, const unsigned char c );
uint_least32_t crc32_result( uint_least32_t* crc);

#ifdef __cplusplus
}
#endif

#endif /*_CRC32_H_*/
