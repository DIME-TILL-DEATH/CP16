#ifndef __CS_H__

#include "appdefs.h"

     enum {eq1=0,eq2,eq3,eq4,eq5,/*early*/e_vol,e_t,/*pres_vol*/pres_lev,/*cab_on*/cab_on,/*eq_on*/eq_on,
    	   /*er_on*/er_on,/*preamp_on*/preamp_on_,preamp_lo_,preamp_mi_,preamp_hi_,preamp_on,
		   preamp_vol,preamp_lo,preamp_mi,preamp_hi,/*gate*/gate_on,gate_th,gate_att,
		   /*compressor*/compr_on,sustein,compr_vol,
           /*eq band freq*/fr1,fr2,fr3,fr4,fr5,/*eq band q*/q1,q2,q3,q4,q5,/*lohi*/lop,hip,hip_on,lop_on,
           /*amp*/pr_on,pr_vol,www,pdCount};

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


void prog_ch(void);
void param_set(void);
void Gate_Change_Preset (void);
void Compressor_Change_Preset (int dgui, int npreset);
void Compressor_init (void);
void comp_par(uint32_t val);
void gate_par(uint32_t val);
float gate_out (float efxout);
float compr_out (float efxout);
extern uint8_t ind_en;
extern float m_vol;
extern volatile uint8_t m_vol_fl;
extern volatile uint8_t m_vol_fl1;
extern float p_vol;
extern float ear_vol;
extern float pream_vol;
extern TCSTask* CSTask ;

constexpr float vol_ind_k[] = { 3e-3f,  2.7e-5f,   1.75e-4f,};
extern volatile  float vol_ind_vector[3];

float convert ( uint8_t* in );
extern uint8_t m1_up[];
extern uint8_t m1_up1[];

extern volatile uint8_t rev_en;
extern volatile uint8_t rev_en1;
extern const uint8_t ver[];
extern const uint8_t dev[];

extern bool sw4_state ;

#endif /*__CS_H__*/
