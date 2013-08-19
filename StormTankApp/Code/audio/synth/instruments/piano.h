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
#ifndef AUDIO_SYNTH_INSTRUMENTS_PIANO_H
#define AUDIO_SYNTH_INSTRUMENTS_PIANO_H

#include "../oscillators/sine_oscillator.h"
#include "../oscillators/triangle_oscillator.h"
#include "../oscillators/sawtooth_oscillator.h"
#include "instrument.h"
#include "pad.h"

namespace audio {
namespace synth {
namespace instruments {

class PianoData : public PadData {
 public:
  struct {
     uint32_t phase1,inc1;
     uint32_t phase2,inc2;
     uint32_t phase3,inc3;
     uint32_t phase4,inc4;
  } piano_table[Polyphony];
  PianoData() : PadData() {
    memset(piano_table,0,sizeof(piano_table));
  }
};

class Piano : public Pad {
 public:
  Piano();
  virtual ~Piano() {
    Unload();
  }
  int Load();
  int Unload();
  InstrumentData* NewInstrumentData() {
    return new PianoData();
  }
  real_t Tick(int note_index);
  int SetFrequency(real_t freq, int note_index);
  int NoteOn(int note_index);
  int NoteOff(int note_index);
  void set_instrument_data(InstrumentData* idata) {
    Pad::set_instrument_data(idata);
    cdata = (PianoData*)idata;
  }
 protected:
  PianoData* cdata;
  oscillators::SineOscillator osc1;
  oscillators::SineOscillator osc2;
  oscillators::SawtoothOscillator osc3;
  oscillators::TriangleOscillator osc4;
  virtual void CalculateHarmonics(real_t freq);
};

}
}
}


#endif