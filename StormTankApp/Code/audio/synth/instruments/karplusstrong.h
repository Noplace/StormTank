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
  DelayLine() :D(nullptr),ptr(0),M(0) {

  }
  void Initialize(uint32_t max) {
    this->M = max;
    D = new real_t[max];
  }
  void Deinitialize() {
    delete [] D;
  }
  real_t Tick(real_t x)   {
    real_t y = D[ptr];
    D[ptr++] = x;         
    if (ptr >= M) { ptr -= M; } 
    return y;
  }
  uint32_t M;
 protected:
  real_t* D;
  intptr_t ptr;
   
};

class PickDirectionLowPassFilter : public filters::IIRFilter<1,2> {
 public:
  void Update(float p) {
    a[0] = 1.0f-p;
    b[0] = 1.0f;
    b[1] = -p;
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


class FirstStringTuningAllPassFilter : public filters::IIRFilter<2,2> {
 public:
  void Update() {
    real_t n = 0.2f;
    a[0] = -n;
    a[1] = 1.0f;
    b[0] = 1.0f;
    b[1] = -n;
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
    b[1] = -x;

  }
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
      Hd[i].set_cutoff_freq(8000.0f);
      Hd[i].Initialize();


      Hn[i].set_sample_rate(sample_rate_);
      Hn[i].Update();
      HL[i].set_sample_rate(sample_rate_);
      HL[i].Update(22100.0f);

      //delay[i].set_sample_rate(sample_rate_);
      //delay[i].set_delay_ms(40.0f);
      //delay[i].set_feedback(0.7f);
      delay[i].Initialize(wavetables[i].count);
      adsr[i].set_sample_rate(sample_rate_);
      adsr[i].set_attack_amp(0.5f);
      adsr[i].set_sustain_amp(0.3f);
      adsr[i].set_attack_time_ms(50.0f);
      adsr[i].set_decay_time_ms(8.0f);
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
  real_t Tick(InstrumentData* data, int note_index) {
    auto cdata = (KarplusStrongData*)data;
    auto& nd = cdata->table[note_index];
    auto& wavetable = wavetables[note_index];
    auto prev_phase = nd.phase;
    nd.phase = ((nd.phase + 1) % wavetable.count);
    real_t	out = wavetable.buf[nd.phase];
    auto previous = wavetable.buf[prev_phase];
    wavetable.buf[prev_phase] = Hn[note_index].Tick(Hd[note_index].Tick(wavetable.buf[nd.phase]));
    
    out = HL[note_index].Tick(out);

    //out = effects::HardClip(out*5);
    //out = effects::EnhanceHarmonics(out,);
    out *= adsr[note_index].Tick();
    return out;
  }
  int SetFrequency(real_t freq, InstrumentData* data, int note_index) {
    auto cdata = (KarplusStrongData*)data;
    cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
    cdata->table[note_index].phase = 0;
    delay[note_index].M = wavetables[note_index].count = int(ceil(sample_rate_ / cdata->note_data_array[note_index].freq));

    //HB[note_index].Update(0.2,sample_rate_/freq);
    FillNoise(note_index);//cdata->note_data_array[note_index].note);

    return S_OK;
  }
  int NoteOn(InstrumentData* data, int note_index) {
    auto cdata = (KarplusStrongData*)data;
    adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
  int NoteOff(InstrumentData* data, int note_index) {
    auto cdata = (KarplusStrongData*)data;
    adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
 protected:
  struct {
    int count;
    real_t* buf;
  } wavetables[Polyphony];

  PickDirectionLowPassFilter Hp[Polyphony];
  PickPositionCombFilter     HB[Polyphony];
  FirstStringTuningAllPassFilter Hn[Polyphony];
  DynamicLevelLowPassFilter HL[Polyphony];
  filters::LowPassFilter Hd[Polyphony];
  
  //effects::Delay delay[Polyphony];
  DelayLine delay[Polyphony];
  uint32_t randseed;
  

  inline void FillNoise(int index) {
    memset(wavetables[index].buf,0,sizeof(real_t)*sample_rate_/8);
    for (int i=0;i<wavetables[index].count;++i) {
      wavetables[index].buf[i] = Noise(&randseed);
      //wavetables[index].buf[i] = HB[index].Tick(Hp[index].Tick(Noise(&randseed)));
    }
  }
};

}
}
}


#endif