#ifndef __CM4_INSTR_H__
#define __CM4_INSTR_H__

#ifdef __cplusplus
	extern "C" {
#endif


static inline uint32_t __attribute__ ((always_inline)) ror16 (uint32_t value)
		{
			uint32_t result ;
			asm volatile ( "ror %0, %1 , #16 \r\n"
		  			: "=r" (result) : "r" (value));
			return (result) ;
		}

#ifdef __cplusplus
	}
#endif

#endif 
