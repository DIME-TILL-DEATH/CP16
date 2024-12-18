#include "appdefs.h"
#include "FirststFilt.h"
#include <vdt/vdt.h>

float dB2rap(float dB);
extern const int8_t def_expander[];

class Expander
{

public:

	inline Expander(void){for(uint8_t i = 0 ; i < 4 ; i++)Expander_Change(i,def_expander[i]);}

	inline float expander(float in)
	{
		delta = 0.5f * fabsf(in) - env;
	    if (delta > 0.0f)env += a_rate * delta;
		else env += d_rate * delta;
		//End envelope power detection
		if (env > tlevel) env = tlevel;
		expenv = sgain * (fast_expf(env * sfactor * tfactor) - 1.0f);		//Envelope waveshaping
	    gain = (1.0f - d_rate) * oldgain + d_rate * expenv;
	    oldgain = gain;				                                    //smooth it out a little bit
		return gain;
	}
	inline void Expander_Change (int np, int value)
	{
	  switch (np)
	    {
	    case 0:
	      tfactor = dB2rap (-((float) value));
	      tlevel = 1.0f/tfactor;
	      break;
	    case 1:
	      sfactor = dB2rap ((float)value / 2);
	      sgain = fast_expf(-sfactor);
	      break;
	    case 2:
	      a_rate = 1000.0f/((float)value * 48000.0f);
	      break;
	    case 3:
	      d_rate = 1000.0f/((float)value * 48000.0f);
	      break;
	    }
	}
private:

	float delta = 0.0f;
	float expenv = 0.0f;
	float env = 0.0f;
	float a_rate;
	float d_rate;
	float tlevel;
	float sgain;
	float sfactor;
	float tfactor;
	float gain;
	float oldgain = 0.0f;
};
class Clipp
{
	PassFilt ClippLPF;
	PassFilt ClippHPF;

public:

	inline Clipp(void)
	{
		ClippLPF.SetLPF(20000.0f);
		ClippHPF.SetHPF(20.0f);
		numL = numH = 1;
	}

    inline float assym_clip(float in , uint8_t num)
    {
    	float temp;
    	in = ClippLPF.filt(in , 1);
    	in = ClippHPF.filt(in , 1);

    	if(in >= 0.0f)
    	{
    		if(in < 1.0f)in = in - (in*in*in) * 0.333333333f;
    		else in = 0.666666667f;

    	}
    	else {
        	temp = -in;
        	if(num == 2)
        	{
        		if(temp < 1.0f)temp = temp - (temp*temp*temp) * 0.333333333f;
        		else temp = 0.666666667f;
        	}
        	else {
            	if(temp < 0.6964f)temp = temp - (temp*temp*temp) * 0.333333333f;
            	else temp = -(1.0f / (temp + 0.7f)) + 1.29995f;
        	}
        	in = -temp;
    	}
    	return in;
    }

    inline void clip_LPF(float f)
    {
    	ClippLPF.SetLPF(f);
    }

    inline void clip_HPF(float f)
    {
    	ClippHPF.SetHPF(f);
    }

private:

    uint8_t numH , numL;

};
class Distor
{
	Clipp clip1;
	Clipp clip2;

public:

	inline Distor(void)
	{
		float temp = 0.0f;
		temp = clip1.assym_clip(temp , 1);
		temp = clip2.assym_clip(temp , 1);
	}

	inline float soft_clip(float in , uint8_t num)
	{
		in = clip1.assym_clip(in , num);in = -clip2.assym_clip(in * -4.0f , num);
		return in;
	}

	inline void clip_LPF(float f , uint8_t num)
	{
		if(!num)clip1.clip_LPF(f);
		else clip2.clip_LPF(f);
	}
	inline void clip_HPF(float f , uint8_t num)
	{
		if(!num)clip1.clip_HPF(f);
		else clip2.clip_HPF(f);
	}
private:

};
