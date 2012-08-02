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
#ifndef AUDIO_SYNTH_INSTRUMENTS_INSTRUMENT_H
#define AUDIO_SYNTH_INSTRUMENTS_INSTRUMENT_H

#include "../base.h"
#include "../adsr.h"

namespace audio {
namespace synth {
namespace instruments {

class InstrumentData {
 public:
  NoteData note_data_array[Polyphony];
  InstrumentData() {
    memset(note_data_array,0,sizeof(note_data_array));
  }
  virtual ~InstrumentData() {

  }
};

class InstrumentProcessor : public Component {
 public:
  InstrumentProcessor() : Component() , loaded_(false) {
   
  }
  virtual ~InstrumentProcessor() {

  }
  virtual int Load() = 0;
  virtual int Unload() = 0;
  virtual InstrumentData* NewInstrumentData() = 0;
  virtual real_t Tick(InstrumentData* data, int note_index) = 0;
  virtual int SetFrequency(real_t freq, InstrumentData* data, int note_index) = 0;
  virtual int NoteOn(InstrumentData* data, int note_index) = 0;
  virtual int NoteOff(InstrumentData* data, int note_index) = 0;
 protected:
  ADSR adsr[Polyphony];
  bool loaded_;
};

}
}
}

#endif