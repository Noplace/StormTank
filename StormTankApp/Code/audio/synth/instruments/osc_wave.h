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
#ifndef AUDIO_SYNTH_INSTRUMENTS_OSC_WAVE_H
#define AUDIO_SYNTH_INSTRUMENTS_OSC_WAVE_H

#include "instrument.h"
#include "../oscillators/sine_oscillator.h"
#include "../oscillators/triangle_oscillator.h"
#include "../oscillators/sawtooth_oscillator.h"
#include "../oscillators/square_oscillator.h"
#include "../oscillators/exp_oscillator.h"

namespace audio {
namespace synth {
namespace instruments {

class OscWaveData : public InstrumentData {
 public:
  struct {
      uint32_t phase,inc;
  } table[Polyphony];
  OscWaveData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class OscWave : public InstrumentProcessor {
 public:
  enum WaveType {
   Sine,Square,Triangle,Sawtooth,Exponent
  };
  OscWave(WaveType type) : InstrumentProcessor(),osc(nullptr),type_(type) {

  }
  virtual ~OscWave() {
    Unload();
  }
  int Load() {
    if (loaded_ == true)
      return S_FALSE;
    switch (type_) {
      case Sine:      osc = new oscillators::SineOscillator();    break;
      case Square:    osc = new oscillators::SquareOscillator();    break;
      case Triangle:  osc = new oscillators::TriangleOscillator();    break;
      case Sawtooth:  osc = new oscillators::SawtoothOscillator();    break;
      case Exponent:  osc = new oscillators::ExpOscillator();    break;
      default:
        osc = new oscillators::SineOscillator();
        break;
    }
    osc->set_sample_rate(sample_rate_);
    //default adsr, should be instrument specific
  //default adsr, should be instrument specific
    for (int i=0;i<Polyphony;++i) {
      adsr[i].set_sample_rate(sample_rate_);
      adsr[i].set_attack_amp(0.5f);
      adsr[i].set_sustain_amp(0.3f);
      adsr[i].set_attack_time_ms(50.0f);
      adsr[i].set_decay_time_ms(8.0f);
      adsr[i].set_release_time_ms(20.5f);
    }
    loaded_ = true;
    return S_OK;
  }
  int Unload() {
    if (loaded_ == false)
      return S_FALSE;
    SafeDelete(&osc);
    loaded_ = false;
    return S_OK;
  }
  InstrumentData* NewInstrumentData() {
    return new OscWaveData();
  }
  real_t Tick(int note_index) {
    auto result = osc->Tick(cdata->table[note_index].phase,cdata->table[note_index].inc);// + osc->Tick(phase,inc*2);
    result *= adsr[note_index].Tick();
    return result;
  }
  int SetFrequency(real_t freq, int note_index) {
    cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
    cdata->table[note_index].inc = osc->get_increment(cdata->note_data_array[note_index].freq);
    return S_OK;
  }
  int NoteOn(int note_index) {
    adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
  int NoteOff(int note_index) {
    //cdata->table[note_index].inc = osc->get_increment(cdata->note_data_array[note_index].freq);
    adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
  void set_instrument_data(InstrumentData* idata) {
    cdata = (OscWaveData*)idata;
  }
 protected:
  OscWaveData* cdata;
  oscillators::Oscillator* osc;
  WaveType type_;
};

}
}
}


#endif