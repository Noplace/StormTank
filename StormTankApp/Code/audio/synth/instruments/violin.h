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
#ifndef AUDIO_SYNTH_INSTRUMENTS_VIOLIN_H
#define AUDIO_SYNTH_INSTRUMENTS_VIOLIN_H

#include "instrument.h"
#include "pad.h"

namespace audio {
namespace synth {
namespace instruments {

class ViolinData : public PadData {
 public:
  ViolinData() : PadData() {
    
  }
};

class Violin : public Pad {
 public:
  Violin();
  virtual ~Violin() {
    Unload();
  }
  int Load();
  int Unload();
  InstrumentData* NewInstrumentData() {
    return new ViolinData();
  }
  real_t Tick(int note_index);
  int SetFrequency(real_t freq, int note_index);
  int NoteOn(int note_index);
  int NoteOff(int note_index);
  void set_instrument_data(InstrumentData* idata) {
    cdata = (ViolinData*)idata;
  }
 protected:
  ViolinData* cdata;
  virtual void CalculateHarmonics(real_t freq);
};

}
}
}


#endif