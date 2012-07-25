#ifndef AUDIO_SYNTH_INSTRUMENTS_PERCUSSION_H
#define AUDIO_SYNTH_INSTRUMENTS_PERCUSSION_H

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
    uint32_t bassdrum_phase,bassdrum_inc;
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
    return new PercussionData();
  }
  real_t Tick(InstrumentData* data, int note_index);
  int NoteOn(InstrumentData* data, int note_index);
  int NoteOff(InstrumentData* data, int note_index);
 protected:
  typedef real_t (Percussion::*PercussionPieceTick)(PercussionData* data, int note_index);
  static PercussionPieceTick ticks[Polyphony];
  real_t inv_sr;
  unsigned int randseed;
  oscillators::SquareOscillator bassdrum;
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