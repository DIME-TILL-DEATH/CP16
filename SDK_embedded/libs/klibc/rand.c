#include "arch.h"
#include "reent.h"


#ifdef __STM32F4XX__
//-----------------------------------------------------------------------
// hardware true random generator
uint32_t rng ()
{
  /* Wait until one RNG number is ready */
  while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET) {}
  /* Get a 32bit Random number */
  return  RNG_GetRandomNumber();
}
//-----------------------------------------------------------------------
float rng1f()
{
  return 1.0f * rng () / ((float)0xffffffff) ;
}
//-----------------------------------------------------------------------
void rng_init()
{
  // reset hw
  RNG_DeInit();

  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
  RNG_Cmd(ENABLE);
}
//-----------------------------------------------------------------------
#endif

unsigned int rand_state ;

//-----------------------------------------------------------------------
void  srand(unsigned seed)
{
  rand_state = seed % (1<<31) ;
}
//-----------------------------------------------------------------------
int  rand()
{
  rand_state = ( 1103515245 * rand_state + 12345 )% (1<<31) ;
  return rand_state ;
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
void  srand_r(unsigned seed)
{
  impure->rand_state = seed % (1<<31) ;
}
//-----------------------------------------------------------------------
int  rand_r()
{
  impure->rand_state = ( 1103515245 * impure->rand_state + 12345 )% (1<<31) ;
  return impure->rand_state ;
}
//-----------------------------------------------------------------------

