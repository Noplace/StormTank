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
#ifndef AUDIO_SYNTH_INSTRUMENTS_PAD_H
#define AUDIO_SYNTH_INSTRUMENTS_PAD_H

#include "instrument.h"
#include "../padsynth/pad_synth.h"

namespace audio {
namespace synth {
namespace instruments {

class PadData : public InstrumentData {
 public:
  struct {
     uint32_t phase,inc;
  } pad_table[Polyphony];
  PadData() : InstrumentData() {
    memset(pad_table,0,sizeof(pad_table));
  }
};

class Pad : public InstrumentProcessor {
 public:
  Pad();
  virtual ~Pad() {
    Unload();
  }
  virtual int Load();
  virtual int Unload();
  virtual InstrumentData* NewInstrumentData() {
    return new PadData();
  }
  virtual real_t Tick(InstrumentData* data, int note_index);
  virtual int SetFrequency(real_t freq, InstrumentData* data, int note_index);
  virtual int NoteOn(InstrumentData* data, int note_index);
  virtual int NoteOff(InstrumentData* data, int note_index);
 protected:
  PadSynth padsynth;
  float* buffer[128];
  real_t harmonics_array[64];
  uint32_t sample_size_;
  virtual void CalculateHarmonics(real_t freq) {
    for (int i=0;i<64;++i) {
      harmonics_array[i] = cos(2*XM_PI*i/64.0f);
    }
  }
};

}
}
}


#endif