#ifndef __FPU_SUPPORT_H__
#define __FPU_SUPPORT_H__


#if defined __CORTEX_M4F__
	#include "arch/fpv4-sp-d16-instr.h"
#else
        #include <math.h>
        #define vsqrt sqrtf
        #define vabs fabsf
#endif

#endif
