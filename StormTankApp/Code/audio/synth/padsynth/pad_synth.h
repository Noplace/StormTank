#ifndef AUDIO_SYNTH_PAD_SYNTH_H
#define AUDIO_SYNTH_PAD_SYNTH_H

#include "../base.h"

namespace audio {
namespace synth {

class PadSynth{
 public:
	PadSynth();
	~PadSynth();
  void Initialize(int N_, int samplerate_, int number_harmonics_);
  void Deinitialize();
	void setharmonic(int n, real_t value);
	real_t getharmonic(int n);
	void synth(real_t f, real_t bw, real_t bwscale, real_t* smp);
 protected:
	real_t *A;		//Amplitude of the harmonics
	real_t *freq_amp;	//Amplitude spectrum
  kiss_fft_cpx* fft_in,* fft_out;
  kiss_fft_cfg fft_cfg;
	int N;
	int samplerate_;
	int number_harmonics;
  void IFFT(real_t* smp);
	real_t profile(real_t fi, real_t bwi);
	real_t RND();
};

}
}

#endif

