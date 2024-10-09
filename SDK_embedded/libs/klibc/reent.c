#include "reent.h"

reent_t* impure ;

void reent_deinit (reent_t* impure)
{
  // nome
}
void reent_init   (reent_t* impure)
{
  impure->errno = 0 ;
}
