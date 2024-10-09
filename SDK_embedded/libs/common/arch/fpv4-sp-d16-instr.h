#ifndef __FPV4_SP_D16_INSTR_H__
#define __FPV4_SP_D16_INSTR_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdint.h>  // needed for uint16_t type defines

static inline float __attribute__ ((always_inline)) vsqrt (float value)
		{
			float result ;
			asm volatile ( 	"vsqrt.f32 %0, %1 \r\n"
		  			: "=w" (result) : "w" (value) );
			return (result) ;
		}

static inline float __attribute__ ((always_inline)) vabs (float value)
		{
			float result ;
			asm volatile ( 	"vabs.f32 %0, %1 \r\n"
		  		: "=w" (result) : "w" (value) );
			return (result) ;
		}

static inline uint16_t __attribute__ ((always_inline)) vcs2ht (float value)
		{
			typedef union
			{
				float f ;
				struct
				{
					uint16_t l ;
					uint16_t h ;
				};
			} TRes ;
			TRes result ;
			asm volatile ( 	"vcvtt.f32.f16 %0, %1 \r\n"
		  			: "=w" (result.f) : "w" (value) );
			return (result.h) ;
		}

static inline float __attribute__ ((always_inline)) vch2st (uint16_t value)
		{
			typedef union
				{
					float f ;
					struct
					{
						uint16_t l ;
						uint16_t h ;
					};
				} TVal ;
			TVal val ;
			val.h = value ;
			float result ;
			asm volatile ( "vcvtt.f16.f32 %0, %1 \r\n"
		  			: "=w" (result) : "w" (val.f) );
			return (result) ;
		}




#ifdef __cplusplus
	}
#endif

#endif 

