#ifndef __REENT_H__
#define __REENT_H__

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef __STM32F4XX__
#endif

//-----------------------------------------------------------------------
typedef struct
{
   int errno ;
   unsigned int rand_state ;
} reent_t ;
//-----------------------------------------------------------------------

void reent_deinit (reent_t*);
void reent_init   (reent_t*);

extern reent_t* impure ;



#ifdef __cplusplus
     }
#endif

#endif /*__REENT_H__*/
