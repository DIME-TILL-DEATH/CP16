#ifndef __STDAPP_H__
#define __STDAPP_H__

#include "arch.h"

	#if defined __USE_FREERTOS__
	  #ifdef __cplusplus
  	    #include "FreeRTOS++.h"
          #else
            #include "FreeRTOS_headers.h"
          #endif
	#endif

        #ifdef __cplusplus
             #include "klibc++.h"

            #include "supc++.h"
            #include "supstl.h"
        #endif

        #include "platform_config.h"


#endif /*__STDAPP_H__*/
