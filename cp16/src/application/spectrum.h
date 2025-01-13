#ifndef __SPECTRUM_H__
#define __SPECTRUM_H__

#include "appdefs.h"
// #include "display.h"
 #include "dsp/fft.h"
 #include "math/window.h"

class TSpectrumTask : public TTask
{
public:
	TSpectrumTask();
	virtual ~TSpectrumTask()
	{
	} ;

	kgp_math::fft& get_fft_0() { return fft_0 ; };
	kgp_math::fft& get_fft_1() { return fft_1 ; };

	char const* note_name;
	int8_t cents;

private:
	void Code() ;

	inline void Process()
	{
		Resume() ;
	}

	void PrintSpertrum(vec* in, size_t N);
	void PrintSpertrumDB(float* in, float low_level, size_t N );
	void Sinus( float Fin, float Fs, size_t N, vec* in, size_t offset = 0 );
	void SinusRepos( kgp_math::fft& fft, float f, float Fs, size_t N, size_t offset = 0 );

	void ToneMeter();

	void ToneTable();

	void PrintNoteTable();

	void PrintBassGuitarTable();

    float inline HalfTone(float index, const float  ref_freq = 440.0f)
	{
    	  return ref_freq * vdt::fast_powf(2.0f, (index - 57.0f) / 12.0f ) ;
	}

	void inline Tone2NoteAndDiff( float tone , size_t& note, float& freq_diff, const float  ref_freq = 440.0f )
	{
		note = 0.5f + 57 + 12 * vdt::fast_logf( tone / ref_freq )/ vdt::fast_logf(2.0f) ;
		//      ^ round trip

		extern volatile float Fswe;
		Fswe = tone;

		freq_diff = tone - HalfTone(note);
	}

	size_t guitar_classic_index_table[6];
	size_t guitar_bass_index_table[6];

	kgp_math::fft fft_0;
	kgp_math::fft fft_1;

	float Kmes ;            // измеренный индекс отсчета спектра
	float tone ;
	float  freq_diff ;

	float ffts_x2_uS , uS_find_tone, note_and_diff_uS ;
};

extern TSpectrumTask* SpectrumTask ;
void SpectrumBuffsUpdate (float u);

#endif /*__SPECTRUM_H__*/
