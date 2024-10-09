#ifndef __ARCH_H__
#define __ARCH_H__

#include <stdint.h>
#include <string.h>
#include <stddef.h>

// chip family dependence headers --------------
#if defined __STM32L1XX__
        #include "stm32l1xx_conf.h"
        #include "stm32l1xx.h"
#elif defined __STM32F1XX__
	#include "stm32f10x_conf.h"
	#include "stm32f10x.h"
#elif defined __STM32F2XX__
	#include "stm32f2xx_conf.h"
	#include "stm32f2xx.h"
#elif defined __STM32F3XX__
	#include "stm32f3xx_conf.h"
	#include "stm32f3xx.h"
#elif defined __STM32F4XX__
	#include "stm32f4xx_conf.h"
	#include "stm32f4xx.h"
#endif
//----------------------------------------------

// chip cpu architecture dependence header
#include "crt.h" // vec table caps
#include "arm_math.h"
#include "arch/cpu_support.h"
#include "arch/fpu_support.h"
#include "arch/unique_id.h"


//----------------------------------------------
// general macro defines -----------------------
#include "sdkdefs.h"

#define  NOP()   asm volatile ("mov r0,r0")
#define  DELAY_NOP(T) {volatile uint32_t t = T; while (t--) NOP(); }


// suppor memory bit banding

#define  CCMDATARAM_ADDR_2_BB_ADDR(REG,BIT)  (uint32_t*)(CCMDATARAM_BB_BASE + ((  (uint32_t)&(REG) - CCMDATARAM_BASE) * 32) + (BIT * 4))
#define  SRAM_ADDR_2_BB_ADDR(REG,BIT)  (uint32_t*)(SRAM_BB_BASE + ((  (uint32_t)&(REG) - SRAM_BASE) * 32) + (BIT * 4))
#define  SRAM1_ADDR_2_BB_ADDR(REG,BIT)  (uint32_t*)(SRAM1_BB_BASE + ((  (uint32_t)&(REG) - SRAM1_BASE) * 32) + (BIT * 4))
#define  SRAM2_ADDR_2_BB_ADDR(REG,BIT)  (uint32_t*)(SRAM2_BB_BASE + ((  (uint32_t)&(REG) - SRAM2_BASE) * 32) + (BIT * 4))
#define  SRAM3_ADDR_2_BB_ADDR(REG,BIT)  (uint32_t*)(SRAM3_BB_BASE + ((  (uint32_t)&(REG) - SRAM3_BASE) * 32) + (BIT * 4))
#define  PERIPH_ADDR_2_BB_ADDR(REG,BIT)  (uint32_t*)(PERIPH_BB_BASE + ((  (uint32_t)&(REG) - PERIPH_BASE) * 32) + (BIT * 4))
#define  BKPSRAM_ADDR_2_BB_ADDR(REG,BIT)  (uint32_t*)(BKPSRAM_BB_BASE + ((  (uint32_t)&(REG) - BKPSRAM_BASE) * 32) + (BIT * 4))



#endif /*__ARCH_H__*/
