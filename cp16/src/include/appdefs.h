#ifndef __APPDEFS_H__
#define __APPDEFS_H__

#include "sdk.h"
#include "apptypes.h"

#ifdef __PA_VERSION__
enum{
	eq1=0,eq2,eq3,eq4,eq5,/*early*/e_vol,e_t,/*pres_vol*/pres_lev,/*cab_on*/cab_on,/*eq_on*/eq_on,
   /*er_on*/er_on,/*amp_on*/amp_on,/*amplif*/a_vol,/*amplifier slave*/amp_slave,a_t,
   /*preamp_on*/preamp_on,/*preamp vol*/preamp_vol,preamp_lo,preamp_mi,preamp_hi,
   /*gate*/gate_on,gate_th,gate_att,/*compressor*/compr_on,sustein,compr_vol,
   /*eq band freq*/fr1,fr2,fr3,fr4,fr5,/*eq band q*/q1,q2,q3,q4,q5,/*lohi*/lop,hip,hip_on,lop_on,
   /*presence*/pr_on,pr_vol,/*eq position*/eq_po,/*end*/pdCount};
#else
enum{
	eq1=0,eq2,eq3,eq4,eq5,
 /*early*/e_vol,e_t,/*pres_vol*/pres_lev,/*cab_on*/cab_on,/*eq_on*/eq_on,
   /*er_on*/er_on,/*preamp_on*/preamp_on_,preamp_lo_,preamp_mi_,preamp_hi_,preamp_on,
   preamp_vol,preamp_lo,preamp_mi,preamp_hi,/*gate*/gate_on,gate_th,gate_att,
   /*compressor*/compr_on,sustein,compr_vol,
   /*eq band freq*/fr1,fr2,fr3,fr4,fr5,/*eq band q*/q1,q2,q3,q4,q5,/*lohi*/lop,hip,hip_on,lop_on,
   /*amp*/pr_on,pr_vol,www,pdCount};
#endif

inline void i2hex(uint8_t val, char* dest)
{
	static const char hexes[] = "0123456789abcdef" ;
	dest[0] = hexes[ val >> 4  ];
	dest[1] = hexes[ val & 0xf ];
	dest[2] = 0;
}



#endif /*__APPDEFS_H__*/
