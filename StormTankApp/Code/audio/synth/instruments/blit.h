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
#ifndef AUDIO_SYNTH_INSTRUMENTS_BLIT_H
#define AUDIO_SYNTH_INSTRUMENTS_BLIT_H

#include "instrument.h"

namespace audio {
namespace synth {
namespace instruments {



class BlitWaveData : public InstrumentData {
 public:
  struct {
     uint32_t phase1,inc1;
     uint32_t phase2,inc2;
     uint32_t phase3,inc3;
     uint32_t phase4,inc4;
  } table[Polyphony];
  BlitWaveData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class BlitWave : public InstrumentProcessor {
 public:
  BlitWave();
  virtual ~BlitWave();

  InstrumentData* NewInstrumentData() {
    return new BlitWaveData();
  }
  int Load();
  int Unload();
  real_t Tick(InstrumentData* data, int note_index);
  void Update(InstrumentData* data, int note_index);
  int SetFrequency(real_t freq, InstrumentData* data, int note_index);
  int NoteOn(InstrumentData* data, int note_index);
  int NoteOff(InstrumentData* data, int note_index);

 protected:
   real_t* wavetables[128];
   void GenerateWavetables();
};

}
}
}

#endif