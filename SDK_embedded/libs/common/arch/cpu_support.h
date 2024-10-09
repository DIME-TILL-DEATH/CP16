#ifndef __CPU_SUPPORT_H__
#define __CPU_SUPPORT_H__

#if defined __CORTEX_M4F__
	#include "arch/cm4-instr.h"
#endif




// Cortex feaches


// get a number of IRQ
// result:
//      [1...255] - Handler CPU mode, result val is ISR number
//      0 - a Thread CPU Mode
inline __attribute__( ( always_inline ) ) static VectorType cortex_isr_num(void)
{
  uint32_t isr_num;
  asm volatile ("MRS %[isr_num], ipsr" : [isr_num] "=r" (isr_num) );
  return (VectorType)isr_num;
}


#endif
