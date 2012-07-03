#ifndef AUDIO_SYNTH_MIDI_SYNTH_H
#define AUDIO_SYNTH_MIDI_SYNTH_H

#include "../midi/midi2.h"

namespace audio {
namespace synth {

class Player;

class Synth : public Component {
 public:
  struct {
    real_t* pre_effects;
    real_t* post_effects;
  } buffers;
  Player* player_;
  virtual void Initialize() = 0;
  virtual void Deinitialize() = 0;
  virtual void Reset() = 0;
  virtual void RenderSamples(uint32_t samples_count, short* data_out) = 0;
 protected:
};

class MidiSynth : public Synth {
 public:
  typedef void (MidiSynth::*MidiEventHandler)(midi::Event* event);
  enum State {
    kStateStopped=0,kStatePlaying=1,kStatePaused=2
  };
  Delay delay_unit;
  MidiSynth() : initialized_(false) {
  }
  ~MidiSynth() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void LoadMidi(char* filename);
  void RenderSamples(uint32_t samples_count, short* data_out);
  void Reset() {
    ResetTracks();
  }
  
 private:
  struct Track {
    midi::Event* event_sequence;
    uint32_t event_index,event_count,ticks_to_next_event;
    midi::Event* getCurrentEvent() {
      return &event_sequence[event_index];
    }
  };
  static MidiEventHandler midi_main_event_handlers[5];
  static MidiEventHandler midi_meta_event_handlers[23];
  static MidiEventHandler midi_channel_event_handlers[23];
  Util util;
  
  instruments::InstrumentProcessor* instr[kInstrumentCount];
  instruments::Percussion* percussion_instr;
  Channel* channels[kChannelCount];
  midi::Event* last_event;
  Track* tracks;
  double bpm;
  real_t pitch_bend_range;
  uint32_t ticks_per_beat;
  uint32_t samples_to_next_event;
  int track_count_;
  uint8_t rpn_fine,rpn_coarse;
  bool initialized_;
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
  void GenerateIntoBuffer(uint32_t samples_to_generate,short* data_out,uint32_t data_offset);
  void MixChannels(uint32_t samples_to_generate);
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
  
  
};

}
}

#endif