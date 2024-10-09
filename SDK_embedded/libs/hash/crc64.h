#ifndef _CRC64_H_
#define _CRC64_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  uint_least64_t crc64(const unsigned char * buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /*_CRC32_H_*/
