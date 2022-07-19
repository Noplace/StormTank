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
#ifndef AUDIO_SYNTH_SONANT_SYNTH_H
#define AUDIO_SYNTH_SONANT_SYNTH_H

#include <WinCore/io/io.h>
#include "base.h"
#include "filters/all.h"
#include "effects/delay.h"
#include "channel.h"
#include "instruments/sonant_program.h"
#include "synth.h"
#include "player.h"

namespace audio {
namespace synth {


// Structs
// Columns
typedef struct {
    // Notes
    unsigned char   n[32];          // Notes (pattern length is 32)
} SonantColumn;

// Instrument
typedef struct {
    // Oscillator 1
    unsigned char   osc1_oct;       // Octave knob
    unsigned char   osc1_det;       // Detune knob
    unsigned char   osc1_detune;    // Actual detune knob
    unsigned char   osc1_xenv;      // Multiply freq by envelope
    unsigned char   osc1_vol;       // Volume knob
    unsigned char   osc1_waveform;  // Wave form
    // Oscillator 2
    unsigned char   osc2_oct;       // Octave knob
    unsigned char   osc2_det;       // Detune knob
    unsigned char   osc2_detune;    // Actual detune knob
    unsigned char   osc2_xenv;      // Multiply freq by envelope
    unsigned char   osc2_vol;       // Volume knob
    unsigned char   osc2_waveform;  // Wave form
    // Noise oscillator
    unsigned char   noise_fader;    // Amount of noise to add
    // Envelope
    unsigned int    env_attack;     // Attack
    unsigned int    env_sustain;    // Sustain
    unsigned int    env_release;    // Release
    unsigned char   env_master;     // Master volume knob
    // Effects
    unsigned char   fx_filter;      // Hi/lo/bandpass or notch toggle
            float   fx_freq;        // FX Frequency
    unsigned char   fx_resonance;   // FX Resonance
    unsigned char   fx_delay_time;  // Delay time
    unsigned char   fx_delay_amt;   // Delay amount
    unsigned char   fx_pan_freq;    // Panning frequency
    unsigned char   fx_pan_amt;     // Panning amount
    // LFO
    unsigned char   lfo_osc1_freq;  // Modify osc1 freq (FM) toggle
    unsigned char   lfo_fx_freq;    // Modify fx freq toggle
    unsigned char   lfo_freq;       // LFO freq
    unsigned char   lfo_amt;        // LFO amount
    unsigned char   lfo_waveform;   // LFO waveform
    // Patterns
             char   p[48];          // Pattern order (Maximum 32 patterns)
    // Columns
    SonantColumn          c[10];          // Columns (10 maximum)
} SonantInstrument;

// Songs
typedef struct {
    // Instruments
    SonantInstrument      i[8];           // Instruments (8 maximum)
} SonantSong;


class SonantSynth : public Synth {
 public:
  enum State {
    kStateStopped=0,kStatePlaying=1,kStatePaused=2
  };
  enum Mode {
    kModeSequencer,
    kModeTest
  };
  SonantSynth() : Synth(),initialized_(false),mode_(kModeSequencer),
                ticks_per_beat(0),samples_to_next_event(0),bpm(0) {
    memset(&prog,0,sizeof(prog));
    memset(&prog_data,0,sizeof(prog_data));
    memset(&aux_effects,0,sizeof(aux_effects));
    memset(&main_effects,0,sizeof(main_effects));
  }
  ~SonantSynth() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void LoadSong(SonantSong* song, int endpattern, int rowlen);
  void RenderSamplesStereo(uint32_t samples_count, real_t* data_out);
  void Reset() {

  }
  void set_mode(Mode mode) {
    mode_ = mode;
  }
  effects::Effect* aux_effects[kEffectCount];
  effects::Effect* main_effects[kEffectCount];
 private:
  SonantSong* song_;
  struct Track {

    uint32_t event_index,event_count,ticks_to_next_event;

  };


  void GenerateIntoBufferStereo(uint32_t samples_to_generate, real_t* data_out, uint32_t& data_offset);
  void MixChannelsStereo(uint32_t samples_to_generate);
  
  
  struct {
    real_t* channels[16];
    real_t* main;
    real_t* aux;
    size_t main_size;
    size_t aux_size;
  } buffers; 
  instruments::SonantProgram* prog[8];
  instruments::InstrumentData* prog_data[8];
  
  CRITICAL_SECTION me_lock;
  double bpm;
  uint32_t ticks_per_beat;
  uint32_t samples_to_next_event;
  int track_count_,endpattern_,rowlen_;
  Mode mode_;
  bool initialized_;
};

}
}

#endif