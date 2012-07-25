#ifndef AUDIO_SYNTH_PLAYER_H
#define AUDIO_SYNTH_PLAYER_H

#include "../output/output.h"
#include "synth.h"

namespace audio {
namespace synth {

class VisualAddon {
 public:
  virtual void AddPCMData256(float* samples, uint32_t channels, double time_ms) = 0;
};

class Player {
 public:
  enum State {
    kStateStopped=0,kStatePlaying=1,kStatePaused=2
  };
  double song_length_ms;
  Player() : state_(kStateStopped), initialized_(false),audio_interface_(nullptr),
             visual_addon_(nullptr), tick_event(nullptr),thread_handle(nullptr),thread_id(0) {
  }
  ~Player() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void Play();
  void Pause();
  void Stop();
  void Tick() {
    SetEvent(tick_event);
  }
  double GetPlaybackSeconds();
  void set_audio_interface(output::Interface* audio_interface) { audio_interface_ = (output::Interface*)audio_interface; }
  void set_visual_addon(VisualAddon* visual_addon) { 
    EnterCriticalSection(&vis_cs);
    visual_addon_ = (VisualAddon*)visual_addon; 
    LeaveCriticalSection(&vis_cs);
  }
  void set_synth(Synth* synth) {
    synth_ = synth;
    synth_->player_ = this;
  }
 private:
  static void __stdcall callback_func(void *parm, float *buf, uint32_t len);
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  CRITICAL_SECTION cs,vis_cs;
  audio::output::Interface* audio_interface_;
  Synth* synth_;
  VisualAddon* visual_addon_;
  real_t* output_buffer;
  HANDLE thread_handle,tick_event,exit_event;
  double song_pos_ms,song_counter_ms;
  uint32_t output_buffer_size_;
  DWORD thread_id;
  State state_;
  bool initialized_;
  DWORD InstancePlayThread();
};

}
}

#endif