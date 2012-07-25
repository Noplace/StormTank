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
  real_t Tick(InstrumentData* data, int note_index);
  int NoteOn(InstrumentData* data, int note_index);
  int NoteOff(InstrumentData* data, int note_index);
 protected:
  virtual void CalculateHarmonics(real_t freq);
};

}
}
}


#endif