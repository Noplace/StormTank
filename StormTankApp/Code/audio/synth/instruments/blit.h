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