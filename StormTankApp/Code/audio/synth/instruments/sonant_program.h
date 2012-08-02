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
#ifndef AUDIO_SYNTH_INSTRUMENTS_SONANT_PROGRAM_H
#define AUDIO_SYNTH_INSTRUMENTS_SONANT_PROGRAM_H

#include "instrument.h"

namespace audio {
namespace synth {
namespace instruments {



class SonantProgramData : public InstrumentData {
 public:
  struct {
    unsigned int attack;
    unsigned int sustain;
    unsigned int release;
    double c1,c2;

    float q;
    float low,band;

    unsigned int i,icount,currentpos;
  } table[Polyphony];
  SonantProgramData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class SonantProgram : public InstrumentProcessor {
 public:
  struct Data {
    // Oscillator 1
    unsigned char   osc1_oct;       // Octave knob
    unsigned char   osc1_det;       // Detune knob
    unsigned char   osc1_detune;    // Actual detune knob
    unsigned char   osc1_xenv;      // Multiply freq by envelope
    unsigned char   osc1_vol;       // Volume knob
    unsigned char   osc1_waveform;  // Wave form
    // Oscillator 2
    unsigned char   osc2_oct;       // Octave knob
    unsigned char   osc2_det;       // Detune knob
    unsigned char   osc2_detune;    // Actual detune knob
    unsigned char   osc2_xenv;      // Multiply freq by envelope
    unsigned char   osc2_vol;       // Volume knob
    unsigned char   osc2_waveform;  // Wave form
    // Noise oscillator
    unsigned char   noise_fader;    // Amount of noise to add
    // Envelope
    unsigned int    env_attack;     // Attack
    unsigned int    env_sustain;    // Sustain
    unsigned int    env_release;    // Release
    unsigned char   env_master;     // Master volume knob
    // Effects
    unsigned char   fx_filter;      // Hi/lo/bandpass or notch toggle
            float   fx_freq;        // FX Frequency
    unsigned char   fx_resonance;   // FX Resonance
    unsigned char   fx_delay_time;  // Delay time
    unsigned char   fx_delay_amt;   // Delay amount
    unsigned char   fx_pan_freq;    // Panning frequency
    unsigned char   fx_pan_amt;     // Panning amount
    // LFO
    unsigned char   lfo_osc1_freq;  // Modify osc1 freq (FM) toggle
    unsigned char   lfo_fx_freq;    // Modify fx freq toggle
    unsigned char   lfo_freq;       // LFO freq
    unsigned char   lfo_amt;        // LFO amount
    unsigned char   lfo_waveform;   // LFO waveform
  };

  SonantProgram();
  virtual ~SonantProgram();

  InstrumentData* NewInstrumentData() {
    return new SonantProgramData();
  }
  int Load();
  int Unload();
  real_t Tick(InstrumentData* data, int note_index);
  void Update(InstrumentData* data, int note_index);
  int SetFrequency(real_t freq, InstrumentData* data, int note_index);
  int NoteOn(InstrumentData* data, int note_index);
  int NoteOff(InstrumentData* data, int note_index);

  Data data;
};

}
}
}

#endif