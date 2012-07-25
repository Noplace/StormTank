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
  real_t Tick(InstrumentData* data, int note_index);
  int NoteOn(InstrumentData* data, int note_index);
  int NoteOff(InstrumentData* data, int note_index);
 protected:
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