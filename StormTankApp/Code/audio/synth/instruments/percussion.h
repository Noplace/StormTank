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
#ifndef AUDIO_SYNTH_INSTRUMENTS_PERCUSSION_H
#define AUDIO_SYNTH_INSTRUMENTS_PERCUSSION_H

#include "../oscillators/sine_oscillator.h"
#include "../oscillators/square_oscillator.h"
#include "instrument.h"

namespace audio {
namespace synth {
namespace instruments {

class PercussionData : public InstrumentData {
 public:
  struct {
    bool highhat_closed;
    real_t highhat_amp,bassdrum_amp;
    uint32_t bassdrum_phase1,bassdrum_phase2,bassdrum_inc1,bassdrum_inc2;
    uint32_t hihat_phase[4],hihat_inc[4];
  } table;
  PercussionData() : InstrumentData() {
    memset(&table,0,sizeof(table));
  }
};


class Percussion : public InstrumentProcessor {
 public:
  Percussion() : InstrumentProcessor() {
   randseed = 1;
  }
  virtual ~Percussion() {
    Unload(); 
  }
  int Load();
  int Unload();
  InstrumentData* NewInstrumentData() {
    auto result = new PercussionData();
    result->table.bassdrum_phase1 = 0;
    result->table.bassdrum_inc1 = bassdrum_osc1.get_increment(523.25f);
    result->table.bassdrum_phase2 = 0;
    result->table.bassdrum_inc2 = bassdrum_osc1.get_increment(523.25f);

    return result;
  }
  real_t Tick(InstrumentData* data, int note_index);
  int SetFrequency(real_t freq, InstrumentData* data, int note_index);
  int NoteOn(InstrumentData* data, int note_index);
  int NoteOff(InstrumentData* data, int note_index);
 protected:
  typedef real_t (Percussion::*PercussionPieceTick)(PercussionData* data, int note_index);
  static PercussionPieceTick ticks[Polyphony];
  real_t inv_sr;
  unsigned int randseed;
  oscillators::SineOscillator bassdrum_osc1,bassdrum_osc2;
  oscillators::SquareOscillator hihat_osc;
  real_t EmptyTick(PercussionData* data, int note_index) {
    return 0.0f;
  }
  real_t HihatOpenTick(PercussionData* data, int note_index);
  real_t HihatClosedTick(PercussionData* data, int note_index);
  real_t BassDrumTick(PercussionData* data, int note_index);
};

}
}
}

#endif