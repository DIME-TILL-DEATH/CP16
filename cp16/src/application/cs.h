#ifndef __CS_H__

#include "appdefs.h"



//#ifdef __PA_VERSION__
//enum{
//	eq1=0,eq2,eq3,eq4,eq5,/*early*/e_vol,e_t,/*pres_vol*/pres_lev,/*cab_on*/cab_on,/*eq_on*/eq_on,
//   /*er_on*/er_on,/*amp_on*/amp_on,/*amplif*/a_vol,/*amplifier slave*/amp_slave,a_t,
//   /*preamp_on*/preamp_on,/*preamp vol*/preamp_vol,preamp_lo,preamp_mi,preamp_hi,
//   /*gate*/gate_on,gate_th,gate_att,/*compressor*/compr_on,sustein,compr_vol,
//   /*eq band freq*/fr1,fr2,fr3,fr4,fr5,/*eq band q*/q1,q2,q3,q4,q5,/*lohi*/lop,hip,hip_on,lop_on,
//   /*presence*/pr_on,pr_vol,/*eq position*/eq_po,/*end*/pdCount};
//#else
//enum{
//	eq1=0,eq2,eq3,eq4,eq5,
// /*early*/e_vol,e_t,/*pres_vol*/pres_lev,/*cab_on*/cab_on,/*eq_on*/eq_on,
//   /*er_on*/er_on,/*preamp_on*/preamp_on_,preamp_lo_,preamp_mi_,preamp_hi_,preamp_on,
//   preamp_vol,preamp_lo,preamp_mi,preamp_hi,/*gate*/gate_on,gate_th,gate_att,
//   /*compressor*/compr_on,sustein,compr_vol,
//   /*eq band freq*/fr1,fr2,fr3,fr4,fr5,/*eq band q*/q1,q2,q3,q4,q5,/*lohi*/lop,hip,hip_on,lop_on,
//   /*amp*/pr_on,pr_vol,www,pdCount};
//#endif

class TCSTask : public TTask
{
public:
  TCSTask () ;
  inline void Give()
     {
        sem-> Give () ;
     }
  inline void GiveFromISR()
    {
       BaseType_t HigherPriorityTaskWoken ;
       sem-> GiveFromISR ( &HigherPriorityTaskWoken) ;
       if ( HigherPriorityTaskWoken )
    	   TScheduler::Yeld();
    }

  inline void DisplayAccess( bool val )
  {
    DispalyAccess = val ;
    if ( val )
       Give();
  }

  inline bool DisplayAccess()
  {
    return DispalyAccess ;
  }


  private:
     void Code() ;

     TSemaphore* sem ;
     bool DispalyAccess ;

};

//inline float out_clip(float in)
//{
//	float a = fabsf(in);
//	float b = a - 0.91838997f;
//	if(a > 0.9486f)
//	{
//		a = 0.98f * (b/(fabsf(b) + 0.001f));
//		if(in >=0.0f)in = a;
//		else in = -a;
//	}
//	return in;
//}

void preset_change(void);
void set_parameters(void);

extern uint8_t ind_en;
//extern float preset_volume;
//extern float ear_vol;
//
//extern float amp_vol;
//extern float amp_sla;
//
//extern float pream_vol;
extern TCSTask* CSTask ;

//constexpr float vol_ind_k[] = { 3e-3f,  2.7e-5f,   1.75e-4f,};
//extern volatile  float vol_ind_vector[3];

float convert ( uint8_t* in );
extern uint8_t m1_up[];
extern uint8_t m1_up1[];

extern volatile uint8_t rev_en;
extern volatile uint8_t rev_en1;
extern const char ver[];
extern const char dev[];

extern bool sw4_state ;

#endif /*__CS_H__*/
