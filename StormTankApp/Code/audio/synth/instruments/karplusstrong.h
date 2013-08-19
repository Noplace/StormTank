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
#ifndef AUDIO_SYNTH_INSTRUMENTS_KARPLUSSTRONG_H
#define AUDIO_SYNTH_INSTRUMENTS_KARPLUSSTRONG_H

#include "instrument.h"
#include "../filters/lowpass.h"
#include "../misc.h"
#include "../filters/iir_filter.h"

namespace audio {
namespace synth {
namespace instruments {

class DelayLine {
 public:
  DelayLine() :buffer(nullptr),ptr(0),M(0) {

  }
  void Initialize(uint32_t max) {
    this->M = max;
    buffer = new real_t[max];
  }
  void Deinitialize() {
    delete [] buffer;
  }
  real_t Tick(real_t x)   {
    real_t y = buffer[ptr];
    buffer[ptr++] = x;         
    if (ptr >= M) { ptr -= M; } 
    return y;
  }
  uint32_t M;
  real_t* buffer;
  intptr_t ptr;
protected:

   
};

class PickDirectionLowPassFilter : public filters::IIRFilter<1,2> {
 public:
  void Update(float p) {
    a[0] = 1.0f-p;
    b[0] = 1.0f;
    b[1] = p;
  }
};

class PickPositionCombFilter : public filters::IIRFilter<100,1> {
 public:
  void Update(real_t B,real_t N) {
    memset(a,0,sizeof(a));
    a[0] = 1.0f;
    int n = int(B*N+0.5f);
    if (n>=0&&n<40)
      a[n] = 1.0f;

    b[0] = 1.0f;
  }
};

class StringDampingFilter : public filters::IIRFilter<3,1> {
 public:
  void Update() {
    real_t g0 = 0.52f,g1 = 0.8f;
    a[0] = g0;
    a[1] = g1;
    a[2] = g0;
    b[0] = 2.0f;

  }
};

class StringStiffnessAllpassFilter : public filters::IIRFilter<3,3> {
 public:
  void Update() {
    real_t c0 = 0.1f,c1 = 0.2f,c2=0.3f;
    a[0] = c2;
    a[1] = c1;
    a[2] = c0;
    b[0] = c0;
    b[1] = c1;
    b[2] = c2;
  }
};

class FirstStringTuningAllPassFilter : public filters::IIRFilter<2,2> {
 public:
  void Update() {
    real_t n = 0.2f;
    a[0] = -n;
    a[1] = 1.0f;
    b[0] = 1.0f;
    b[1] = n;
  }
};

class DynamicLevelLowPassFilter : public filters::IIRFilter<1,2> {
 public:
  void Update(real_t bw) {
    real_t L = bw;//bandwidth
    real_t T = 1.0f/sample_rate_;
    real_t x = exp(-XM_PI*L*T);
    a[0] = 1.0f-x;
    b[0] = 1.0f;
    b[1] = x;

  }
};

class LowPassFilter : public filters::IIRFilter<1,2> {
 public:
  void set_cutoff_freq(real_t cutoff_freq) { cutoff_freq_ = cutoff_freq; }
  void Update() {
    real_t fc = (cutoff_freq_)/(sample_rate_);
    real_t x = exp(-2*XM_PI*fc);
    a[0] = 1.0f-x;
    b[0] = 1.0f;
    b[1] = x;
  }
 protected:
  real_t cutoff_freq_;
};


class KarplusStrongData : public InstrumentData {
 public:
  struct {
    uint32_t phase;
  } table[Polyphony];
  KarplusStrongData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class KarplusStrong : public InstrumentProcessor {
 public:
  KarplusStrong() : InstrumentProcessor() {
    randseed = 1;
  }
  virtual ~KarplusStrong() {
    Unload();
  }
  int Load() {
    if (loaded_ == true)
      return S_FALSE;


    for (int i=0;i<Polyphony;++i) {
      wavetables[i].count = (sample_rate_/8)+10;
      wavetables[i].buf = new real_t[wavetables[i].count]; //lowest freq = 8
      memset(wavetables[i].buf,0,sizeof(real_t)*wavetables[i].count);

      Hp[i].set_sample_rate(sample_rate_);
      Hp[i].Update(0.1f);


      Hd[i].set_sample_rate(sample_rate_);
      Hd[i].Update();
      Hs[i].set_sample_rate(sample_rate_);
      Hs[i].Update();

      Hn[i].set_sample_rate(sample_rate_);
      Hn[i].Update();
      HL[i].set_sample_rate(sample_rate_);
      HL[i].Update(22100.0f);
      
      lowpass[i].set_sample_rate(sample_rate_);
      lowpass[i].set_cutoff_freq(12050.0f);
      lowpass[i].Update();
      //delay[i].set_sample_rate(sample_rate_);
      //delay[i].set_delay_ms(40.0f);
      //delay[i].set_feedback(0.7f);
      delay[i].Initialize(wavetables[i].count);
      adsr[i].set_sample_rate(sample_rate_);
      adsr[i].set_attack_amp(0.8f);
      adsr[i].set_sustain_amp(0.6f);
      adsr[i].set_attack_time_ms(10.0f);
      adsr[i].set_decay_time_ms(4.0f);
      adsr[i].set_release_time_ms(20.5f);
    }

    /*for_each_note([&](int index,float freq){
      wavetables[index].count = int(ceil(sample_rate_ / freq));
      wavetables[index].buf = new real_t[wavetables[index].count];
      FillNoise(index);
    });*/

    loaded_ = true;
    return S_OK;
  }

  int Unload() {
    if (loaded_ == false)
      return S_FALSE;

    for (int i=0;i<Polyphony;++i) {
      delay[i].Deinitialize();
      SafeDeleteArray(&wavetables[i].buf);
    }

    loaded_ = false;
    return S_OK;
  }
  InstrumentData* NewInstrumentData() {
    return new KarplusStrongData();
  }
  real_t Tick(int note_index) {
    //auto cdata = (KarplusStrongData*)data;
    auto& nd = cdata->table[note_index];
    auto& wavetable = wavetables[note_index];
    auto prev_phase = nd.phase;
    nd.phase = ((nd.phase + 1) % wavetable.count);

    
    real_t	out = wavetable.buf[nd.phase];
    //auto previous = wavetable.buf[prev_phase];
    //wavetable.buf[prev_phase] = (wavetable.buf[prev_phase]+wavetable.buf[nd.phase])*0.5f;
    wavetable.buf[prev_phase] = lowpass[note_index].Tick(wavetable.buf[nd.phase]);
    //auto s1 = Hd[note_index].Tick(wavetable.buf[nd.phase]);
    //auto s2 = Hs[note_index].Tick(wavetable.buf[nd.phase]);
    //wavetable.buf[prev_phase] = Hd[note_index].Tick(wavetable.buf[nd.phase]);
    

    //out = effects::HardClip(out*5);
    //out = effects::EnhanceHarmonics(out,);
    out *= adsr[note_index].Tick();
    return out;
  }
  int SetFrequency(real_t freq, int note_index) {
    cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
    cdata->table[note_index].phase = 0;
    delay[note_index].M = wavetables[note_index].count = int(ceil(sample_rate_ / cdata->note_data_array[note_index].freq));

     

    //HB[note_index].Update(0.2,sample_rate_/freq);
    FillNoise(note_index);//cdata->note_data_array[note_index].note);

    return S_OK;
  }
  int NoteOn(int note_index) {
    //auto cdata = (KarplusStrongData*)data;

    adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
  int NoteOff(int note_index) {
    //auto cdata = (KarplusStrongData*)data;
    adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }

  void set_instrument_data(InstrumentData* idata) {
    this->cdata = (KarplusStrongData*)idata;
  }
 protected:
  KarplusStrongData* cdata;
  struct {
    int count;
    real_t* buf;
  } wavetables[Polyphony];
  PickDirectionLowPassFilter Hp[Polyphony];
  PickPositionCombFilter     HB[Polyphony];
  FirstStringTuningAllPassFilter Hn[Polyphony];
  DynamicLevelLowPassFilter HL[Polyphony];
  StringDampingFilter Hd[Polyphony];
  StringStiffnessAllpassFilter Hs[Polyphony];
  LowPassFilter lowpass[Polyphony];
  //filters::LowPassFilter Hd[Polyphony];
  
  //effects::Delay delay[Polyphony];
  DelayLine delay[Polyphony];
  uint32_t randseed;
  

  inline void FillNoise(int index) {
    memset(wavetables[index].buf,0,sizeof(real_t)*sample_rate_/8);
    for (int i=0;i<wavetables[index].count;++i) {
      delay[index].buffer[i] = wavetables[index].buf[i] = RandomFloat(&randseed);
      //wavetables[index].buf[i] = HB[index].Tick(Hp[index].Tick(Noise(&randseed)));
    }
  }
};

}
}
}


#endif