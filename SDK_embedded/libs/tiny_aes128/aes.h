#ifndef _AES_H_
#define _AES_H_

//   https://github.com/kokke/tiny-AES128-C

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES128 encryption in CBC-mode of operation and handles 0-padding.
// ECB enables the basic ECB 16-byte block algorithm. Both can be enabled simultaneously.

void AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
void AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);

void AES128_CBC_encrypt_buffer(uint8_t* output, uint8_t* input, const uint32_t length, const uint8_t* key, const uint8_t* iv);
void AES128_CBC_decrypt_buffer(uint8_t* output, uint8_t* input, const uint32_t length, const uint8_t* key, const uint8_t* iv);

#ifdef __cplusplus
    }
#endif

#endif //_AES_H_
