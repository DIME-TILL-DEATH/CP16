#ifndef __SDKDEFS_H__
#define __SDKDEFS_H__

#define _BV(val) (1<<val)
#define _HB(val) (val>>8)
#define _LB(val) (val&0xff)

#if !defined NULL
        #define  NULL    (void*)0
#endif

#define __RAMFUNC__  __attribute__ ((section(".ramfunc"),noinline,long_call))
#define __CONST_DATA__ __attribute__ ((section(".const_data")))
#define __FUNC_USED__ __attribute__((used))

#define __CCM_BSS__ __attribute__ ((section(".ccm_bss")))
#define __CCM_DATA__ __attribute__ ((section(".ccm_data")))

#endif /*__SDKDEFS_H__*/
