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
  real_t Tick(InstrumentData* data, int note_index) {
    auto cdata = (OscWaveData*)data;
    auto result = osc->Tick(cdata->table[note_index].phase,cdata->table[note_index].inc);// + osc->Tick(phase,inc*2);
    result *= adsr[note_index].Tick();
    return result;
  }
  int NoteOn(InstrumentData* data, int note_index) {
    auto cdata = (OscWaveData*)data;
    cdata->table[note_index].inc = osc->get_increment(cdata->note_data_array[note_index].freq);
    adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
  int NoteOff(InstrumentData* data, int note_index) {
    auto cdata = (OscWaveData*)data;
    //cdata->table[note_index].inc = osc->get_increment(cdata->note_data_array[note_index].freq);
    adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
 protected:
  oscillators::Oscillator* osc;
  WaveType type_;
};

}
}
}


#endif