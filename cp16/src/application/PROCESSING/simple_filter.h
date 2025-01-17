#ifndef SRC_APPLICATION_SIMPLE_FILTER_H_
#define SRC_APPLICATION_SIMPLE_FILTER_H_

#include "appdefs.h"

class FirstOrderFilt
{
	static constexpr float filt_pi2 = 6.283185307f;
	static constexpr float LpHpW = 48000.0f * 2;

public:

	FirstOrderFilt(void)
	{
		SetLPF(20000.0f);
		SetHPF(20.0f);
		in0 = in1 = out0 = out1 = 0.0f;
	}

	inline float process(float in)
	{
		in0 = in;
		out0 = in0*a0 + in1*a1 + out1*b1;
		in1 = in0;
		out1 = out0;
		return out0;
	}

	inline void SetLPF(float fCut)
	{
	    float Norm;
	    fCut *= filt_pi2;
	    Norm = 1.0f / (fCut + LpHpW);
	    b1 = (LpHpW - fCut) * Norm;
	    a0 = a1 = fCut * Norm;
	}

	inline void SetHPF(float fCut)
	{
	    float Norm;
	    fCut *= filt_pi2;
	    Norm = 1.0f / (fCut + LpHpW);
	    a0 = LpHpW * Norm;
	    a1 = -a0;
	    b1 = (LpHpW - fCut) * Norm;
	}

private:

	float in0 , in1 , out0 , out1 , a0 , a1 , b1;
};

class PassFilt
{

	FirstOrderFilt filt1;
	FirstOrderFilt filt2;
	FirstOrderFilt filt3;

public:

	inline float process(float in , uint8_t num)
	{
		switch(num){
		case 1:in = filt1.process(in);break;
		case 2:in = filt2.process(filt1.process(in));break;
		case 3:in = filt3.process(filt2.process(filt1.process(in)));break;
		}
		return in;
	}

	inline void SetLPF(float f)
	{
		filt1.SetLPF(f);
		filt2.SetLPF(f);
		filt3.SetLPF(f);
	}

	inline void SetHPF(float f)
	{
		filt1.SetHPF(f);
		filt2.SetHPF(f);
		filt3.SetHPF(f);
	}

private:

};

#endif /* SRC_APPLICATION_FIRSTSTFILT_H_ */
