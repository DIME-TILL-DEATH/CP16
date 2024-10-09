#ifndef __RAND_H__
#define __RAND_H__

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef __STM32F4XX__
   //-----------------------------------------------------------------------
   // hardware true random generator
   uint32_t rng ();
   //-----------------------------------------------------------------------
   void rng_init();
   //-----------------------------------------------------------------------
   float rng1f();
#endif

//-----------------------------------------------------------------------
void  srand(unsigned seed);
int   rand();

void  srand_r(unsigned seed);
int   rand_r();
//-----------------------------------------------------------------------

#ifdef __cplusplus
     }
#endif

#endif /*__RAND_H__*/
