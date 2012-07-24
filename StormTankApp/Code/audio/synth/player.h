#ifndef AUDIO_SYNTH_PLAYER_H
#define AUDIO_SYNTH_PLAYER_H

#include "../output/output.h"

namespace audio {
namespace synth {

class VisualAddon {
 public:
  virtual void AddPCMData256(float* samples, uint32_t channels, double time_ms) = 0;
};

class Player {
 public:
  typedef void (Player::*MidiEventHandler)(midi::Event* event);
  enum State {
    kStateStopped=0,kStatePlaying=1,kStatePaused=2
  };
  double song_length_ms;
  Player() : state_(kStateStopped), initialized_(false),audio_interface_(nullptr),
             visual_addon_(nullptr), player_event(nullptr),thread_handle(nullptr),thread_id(0) {
  }
  ~Player() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void Play();
  void Pause();
  void Stop();
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
  double thread_time_span;
 private:
  static void __stdcall callback_func(void *parm, float *buf, uint32_t len);
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  CRITICAL_SECTION cs,vis_cs;
  audio::output::Interface* audio_interface_;
  Synth* synth_;
  VisualAddon* visual_addon_;
  short* output_buffer;
  HANDLE thread_handle,player_event;
  double song_pos_ms,song_counter_ms,output_buffer_length_ms_;
  DWORD thread_id;
  State state_;
  int thread_msg;
  bool initialized_;
  void SendThreadMessage(int msg) {
    EnterCriticalSection(&cs);
    thread_msg = msg;
    LeaveCriticalSection(&cs);
  }
  DWORD InstancePlayThread();
};

}
}

#endif