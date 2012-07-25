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