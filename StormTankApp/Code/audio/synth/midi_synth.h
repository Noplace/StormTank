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
#ifndef AUDIO_SYNTH_MIDI_SYNTH_H
#define AUDIO_SYNTH_MIDI_SYNTH_H

#include <WinCore/io/io.h>
#include "../midi/midi2.h"
#include "base.h"
#include "filters/lowpass.h"
#include "effects/delay.h"
#include "channel.h"
#include "instruments/percussion.h"
#include "synth.h"
#include "player.h"

namespace audio {
namespace synth {

template<typename F>
static void for_each_note(F f) {
  real_t A4_freq = 440;
  real_t two = pow(2.0f,1/12.0f);
  for (int i=0;i<128;++i) {
    auto freq = A4_freq * pow(two,i-69);//69 = index of A4
    f(i,freq);
  }
}

class MidiSynth : public Synth {
 public:
  typedef void (MidiSynth::*MidiEventHandler)(midi::Event* event);
  enum State {
    kStateStopped=0,kStatePlaying=1,kStatePaused=2
  };
  enum Mode {
    kModeSequencer,
    kModeTest
  };
  MidiSynth() : Synth(),initialized_(false),mode_(kModeSequencer),percussion_instr(nullptr),
                last_event(nullptr),tracks(nullptr),ticks_per_beat(0),samples_to_next_event(0),bpm(0) {
    memset(&instr,0,sizeof(instr));
    memset(&channels,0,sizeof(channels));
    memset(&aux_effects,0,sizeof(aux_effects));
    memset(&main_effects,0,sizeof(main_effects));
  }
  ~MidiSynth() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void LoadMidiFromFile(const char* filename);
  void LoadMidi(uint8_t* data, size_t data_size);
  void RenderSamplesStereo(uint32_t samples_count, real_t* data_out);
  void Reset() {
    ResetTracks();
  }
  midi::Event* BeginPrepareNextEvent() {
    auto result = GetNextEvent();
    EnterCriticalSection(&me_lock);
    if (tracks[0].event_index)
      --tracks[0].event_index;
    return result;
  }
  void EndPrepareNextEvent() {
    LeaveCriticalSection(&me_lock);
  }
  void set_mode(Mode mode) {
    mode_ = mode;
  }
  effects::Effect* aux_effects[kEffectCount];
  effects::Effect* main_effects[kEffectCount];
  effects::Delay delay_unit;
 private:
  struct Track {
    midi::Event* event_sequence;
    uint32_t event_index,event_count,ticks_to_next_event;
    __forceinline midi::Event* GetCurrentEvent() {
      return &event_sequence[event_index];
    }
  };
  static const MidiEventHandler midi_main_event_handlers[5];
  static const MidiEventHandler midi_meta_event_handlers[23];
  static const MidiEventHandler midi_channel_event_handlers[23];
  void DestroyTrackData() {
    for (int i=0;i<track_count_;++i)
      SafeDeleteArray(&tracks[i].event_sequence);
    SafeDeleteArray(&tracks);
    track_count_ = 0;
    tracks = nullptr;
  }
  void ResetTracks() {
    for (int track_index=0;track_index<track_count_;++track_index) {
      auto& track = tracks[track_index];
      track.event_index = 0;      
      track.ticks_to_next_event = track.event_sequence[0].deltaTime;
    }
    last_event = GetNextEvent();
    samples_to_next_event = 0;
  }
  void GenerateIntoBufferStereo(uint32_t samples_to_generate, real_t* data_out, uint32_t& data_offset);
  void MixChannelsStereo(uint32_t samples_to_generate);
  midi::Event* GetNextEvent();
  void HandleEvent(midi::Event* event);
  void MidiEventUnknown(midi::Event* event);
  void MidiEventMeta(midi::Event* event);
  void MidiEventSetTempo(midi::Event* event);
  void MidiEventChannel(midi::Event* event);
  void MidiEventNoteOn(midi::Event* event);
  void MidiEventNoteOff(midi::Event* event);
  void MidiEventProgramChange(midi::Event* event);
  void MidiEventController(midi::Event* event);
  void MidiEventPitchBend(midi::Event* event);
  real_t chromatic_scale_freq[note_count];
  struct {
    real_t* channels[16];
    real_t* main;
    real_t* aux;
    size_t main_size;
    size_t aux_size;
  } buffers;  
  instruments::InstrumentProcessor* instr[kInstrumentCount];
  instruments::Percussion* percussion_instr;
  Channel* channels[kChannelCount];
  midi::Event* last_event;
  Track* tracks;
  CRITICAL_SECTION me_lock;
  double bpm;
  uint32_t ticks_per_beat;
  uint32_t samples_to_next_event;
  int track_count_;
  Mode mode_;
  bool initialized_;
};

}
}

#endif