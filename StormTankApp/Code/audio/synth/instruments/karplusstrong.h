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
      lpf[i].set_sample_rate(sample_rate_);
      lpf[i].set_cutoff_freq(8000.0f);
      lpf[i].Initialize();
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
    wavetable.buf[prev_phase] = lpf[note_index].Tick(wavetable.buf[nd.phase]);

 
    
    out *= adsr[note_index].Tick();
    return out;
  }
  int SetFrequency(real_t freq, InstrumentData* data, int note_index) {
    auto cdata = (KarplusStrongData*)data;
    cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
    cdata->table[note_index].phase = 0;
    delay[note_index].M = wavetables[note_index].count = int(ceil(sample_rate_ / cdata->note_data_array[note_index].freq));
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
  union FloatBits
  {
    real_t f;
    uint32_t u;
  };

  struct {
    int count;
    real_t* buf;
  }wavetables[Polyphony];

  filters::LowPassFilter lpf[Polyphony];
  //effects::Delay delay[Polyphony];
  DelayLine delay[Polyphony];
  uint32_t randseed;
  real_t bits2float(uint32_t u) {
    FloatBits x;
    x.u = u;
    return x.f;
  }

  inline uint32_t urandom(uint32_t *seed)
  {
    *seed = *seed * 196314165 + 907633515;
    return *seed;
  }

  // uniform random float in [-1,1)
  inline real_t Random() {
    uint32_t bits = urandom(&randseed); // random 32-bit value
    real_t f = bits2float((bits >> 9) | 0x40000000); // random float in [2,4)
    return f - 3.0f; // uniform random float in [-1,1)
  }

  inline real_t Noise() {
    float r1 = (1+Random())*0.5f;
    float r2 = (1+Random())*0.5f;
    return (float) sqrt( -2.0f * log(r1)) * cos( 2.0f * XM_PI *r2);//white noise
  }

  inline void FillNoise(int index) {
    memset(wavetables[index].buf,0,sizeof(real_t)*sample_rate_/8);
    for (int i=0;i<wavetables[index].count;++i) {
      wavetables[index].buf[i] = Noise();
    }
  }
};

}
}
}


#endif