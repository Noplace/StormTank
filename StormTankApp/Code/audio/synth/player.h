#ifndef AUDIO_SYNTH_PLAYER_H
#define AUDIO_SYNTH_PLAYER_H

#include "../midi/midi2.h"
#include "../output/output.h"

namespace audio {
namespace synth {

/*
struct NoteEventData {
  double frequency;
  uint8_t note;
  uint8_t velocity;
};

struct InstrumentEventData {
  uint8_t instrument;
  uint8_t unused[5];
};

struct Event {
  double pos_ms;
  int channel;
  int command;
  
  struct {
    NoteEventData note;
    InstrumentEventData instrument;
  } data;
};*/


class Player {
 public:
  enum State {
    kStateStopped=0,kStatePlaying=1,kStatePaused=2
  };
  Player() : state_(kStateStopped), initialized_(false),audio_interface_(nullptr),player_event(nullptr),msg_queue_event(nullptr),thread_handle(nullptr),thread_id(0) {
    
  }
  ~Player() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void LoadMidi(char* filename);
  void Play();
  void Pause();
  void Stop();
  double GetPlaybackSeconds();
  void set_audio_interface(AudioOutputInterface* audio_interface) { audio_interface_ = (AudioOutputInterface*)audio_interface; }
 private:
  struct Track {
    midi::Event* event_sequence;
    uint32_t event_index,event_count,ticks_to_next_event;
    midi::Event* getCurrentEvent() {
      return &event_sequence[event_index];
    }
  };
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  Util util;
  Delay delay_unit;
  audio::AudioOutputInterface* audio_interface_;
  instruments::InstrumentProcessor* instr[kInstrumentCount];
  instruments::Percussion* percussion_instr;
  Channel* channels[kChannelCount];
  midi::Event* last_event;
  short* output_buffer;
  Track* tracks;
  HANDLE thread_handle,msg_queue_event,player_event;
  double song_pos_ms,song_length_ms, song_counter_ms;
  double bpm,sample_rate_;
  DWORD thread_id;
  uint32_t ticks_per_beat;
  uint32_t samples_to_next_event;
  State state_;
  int track_count_;
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
  }
  midi::Event* GetNextEvent();
  void HandleEvent(midi::Event* event);
  void RenderSamples(uint32_t samples_count, short* data_out);
  void RenderSamples2(uint32_t samples_count);
  void MixChannels(double& output_left, double& output_right);
};

}
}

#endif