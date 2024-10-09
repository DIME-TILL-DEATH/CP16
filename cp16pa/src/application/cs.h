#ifndef __CS_H__

#include "appdefs.h"

enum
{
	eq1=0,
	eq2,
	eq3,
	eq4,
	eq5,

	e_vol, /*early*/
	e_t,

	pres_lev, /*pres_vol*/
	cab_on,
	eq_on,
	er_on,

	amp_on,
	a_vol,
	amp_slave,
	a_t,

	preamp_on,
	preamp_vol,
	preamp_lo,
	preamp_mi,
	preamp_hi,

	gate_on,
	gate_th,
	gate_att,

	compr_on,
	sustein,
	compr_vol,

	fr1, fr2, fr3, fr4, fr5, /*eq band freq*/
	q1, q2, q3, q4, q5, /*eq band q*/
	lop, hip, hip_on, lop_on, /*lohi*/
	pr_on, /*presence*/
	pr_vol,
	eq_po, /*eq position*/
	/*end*/
	pdCount
 };

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

inline float out_clip(float in)
{
	float a = fabsf(in);
	float b = a - 0.91838997f;
	if(a > 0.9486f)
	{
		a = 0.98f * (b/(fabsf(b) + 0.001f));
		if(in >=0.0f)in = a;
		else in = -a;
	}
	return in;
}

void Gate_Change_Preset (void);
void Compressor_Change_Preset (int dgui, int npreset);
void Compressor_init (void);
void comp_par(uint32_t val);
void gate_par(uint32_t val);
float gate_out (float efxout);
float compr_out (float efxout);
void prog_ch(void);
void param_set(void);
void sig_invert(uint8_t val);


constexpr float vol_ind_k[] = { 3e-3f,  2.7e-5f,   1.75e-4f,};
extern volatile  float vol_ind_vector[3];

#define taps_fir 384

extern uint8_t ind_en;
extern float m_vol;
extern volatile uint8_t m_vol_fl;
extern volatile uint8_t m_vol_fl1;
extern float p_vol;
extern float ear_vol;
extern float amp_vol;
extern float amp_sla;
extern float pream_vol;
extern TCSTask* CSTask ;

float convert ( uint8_t* in );
extern uint8_t m1_up[];
extern uint8_t m1_up1[];

extern volatile uint8_t rev_en;
extern volatile uint8_t rev_en1;
extern const uint8_t ver[];
extern const uint8_t dev[];

extern bool sw4_state ;

#endif /*__CS_H__*/
