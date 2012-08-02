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
#include "pad_synth.h"

namespace audio {
namespace synth {

PadSynth::PadSynth() : A(nullptr),  freq_amp(nullptr),  fft_in(nullptr),  fft_out(nullptr),fft_cfg(nullptr) {

};

PadSynth::~PadSynth() {
  Deinitialize();
};

void PadSynth::Initialize(int N_,int samplerate_,int number_harmonics_) {
  N=N_;
  this->samplerate_=samplerate_;
  number_harmonics=number_harmonics_;
  A=new real_t [number_harmonics];
  for (int i=0;i<number_harmonics;i++) A[i]=0.0;
  A[1]=1.0;//default, the first harmonic has the amplitude 1.0
  freq_amp=new real_t[N/2];
  fft_in = new kiss_fft_cpx[N];
  fft_out = new kiss_fft_cpx[N];
  fft_cfg = kiss_fft_alloc(N,1,0,0);
}

void PadSynth::Deinitialize() {
  SafeDeleteArray(&A);
  SafeDeleteArray(&freq_amp);
  SafeDeleteArray(&fft_in);
  SafeDeleteArray(&fft_out);
  if (fft_cfg != nullptr) {
    kiss_fft_free(fft_cfg);
    fft_cfg = nullptr;
  }
}

void PadSynth::setharmonic(int n,real_t value){
    if ((n<1)||(n>=number_harmonics)) return;
    A[n]=value;
};

real_t PadSynth::getharmonic(int n){
    if ((n<1)||(n>=number_harmonics)) return 0.0;
    return A[n];
};

void PadSynth::IFFT(real_t* smp) {
   kiss_fft(fft_cfg,fft_in,fft_out);
   for (int i=0;i<N;++i) {
     smp[i] = fft_out[i].r / float(N/2);
   }
}

real_t PadSynth::profile(real_t fi, real_t bwi){
    real_t x=fi/bwi;
    x*=x;
    if (x>14.71280603) return 0.0;//this avoids computing the e^(-x^2) where it's results are very close to zero
    return exp(-x)/bwi;
};

void PadSynth::synth(real_t f,real_t bw,real_t bwscale,real_t* smp){
  int i,nh;
  for (i=0;i<N/2;i++) 
    freq_amp[i]=0.0;//default, all the frequency amplitudes are zero
  
  for (nh=1;nh<number_harmonics;nh++){//for each harmonic
    real_t bw_Hz;//bandwidth of the current harmonic measured in Hz
    real_t bwi;
    real_t fi;
    real_t rF=f*real_t(nh);
    bw_Hz=(pow(2.0f,bw/1200.0f)-1.0f)*f*pow(real_t(nh),bwscale);
    bwi=bw_Hz/(2.0f*samplerate_);
	  fi=rF/samplerate_;
	  for (i=0;i<N/2;i++){//here you can optimize, by avoiding to compute the profile for the full frequency (usually it's zero or very close to zero)
	    real_t hprofile;
	    hprofile=profile((real_t(i)/(real_t)N)-fi,bwi);
	    freq_amp[i]+=hprofile*A[nh];
	  }
  }

  memset(fft_in,0,sizeof(kiss_fft_cpx)*N);
  for (int i=0;i<N/2;++i) {
    real_t phase=RND()*2.0f*3.14159265358979f;
    fft_in[i].r = freq_amp[i]*cos(phase);
    fft_in[i].i = freq_amp[i]*sin(phase);
  }

  IFFT(smp);

  //normalize the output
  real_t max=0.0f;
  for (i=0;i<N;i++) if (fabs(smp[i])>max) max=fabs(smp[i]);
  if (max<1e-5f) max=1e-5f;
  for (i=0;i<N;i++) smp[i]/=max*1.4142f;
};

real_t PadSynth::RND(){
  return (rand()/(RAND_MAX+1.0f));
};


}
}