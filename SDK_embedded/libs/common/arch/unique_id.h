/*
 * unique_id.h
 *
 *  Created on: 06 мая 2014 г.
 *      Author: klen
 */

#ifndef __UNIQUE_ID_H__
#define __UNIQUE_ID_H__

#include <stdint.h>

#if defined __STM32L1XX__
  #error "not implement in this header"
#elif defined __STM32F1XX__
  #define UNIQUE_ID_ADDR 0x1FFFF7E8
  #define UNIQUE_ID_0 (*(uint32_t*) UNIQUE_ID_ADDR)
  #define UNIQUE_ID_1 (*(uint32_t*) UNIQUE_ID_ADDR+4)
  #define UNIQUE_ID_2 (*(uint32_t*) UNIQUE_ID_ADDR+8)
  #define CHIP_FLASH_SIZE ((*(uint16_t*)0x1FFFF7E0))
  #define CHIP_RAM_SIZE   ((*(uint16_t*)0x1FFFF7E2))

#elif defined __STM32F2XX__
  #error "not implement in this header"
#elif defined __STM32F3XX__
  #error "not implement in this header"
#elif defined __STM32F2XX__ || __STM32F4XX__
  #define UNIQUE_96BIT_ID ((uint32_t*) 0x1FFF7A10)
  #define UNIQUE_ID_0 (*(uint32_t*) 0x1FFF7A10)
  #define UNIQUE_ID_1 (*(uint32_t*) 0x1FFF7A14)
  #define UNIQUE_ID_2 (*(uint32_t*) 0x1FFF7A18)
  #define CHIP_FLASH_SIZE ((*(uint16_t*)0x1FFF7A22))
  #define CHIP_RAM_SIZE   ((*(uint16_t*)0x1FFF7A24))
#endif

#endif /* __UNIQUE_ID_H__ */
