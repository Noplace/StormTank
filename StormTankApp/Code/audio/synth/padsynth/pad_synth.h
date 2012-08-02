/*****************************************************************************************************************
* Copyright (c) 2012 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/
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

