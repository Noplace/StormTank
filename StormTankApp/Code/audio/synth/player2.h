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
  enum Mode {
   kModeNormal=0, kModeLoop=1
  };
  double song_length_ms;
  Player() : state_(kStateStopped),mode_(kModeNormal), initialized_(false),audio_interface_(nullptr),
             visual_addon_(nullptr), tick_event(nullptr),thread_handle(nullptr),thread_id(0),request_stop_(false) {
  }
  ~Player() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void Play();
  void Pause();
  void Stop();
  void Reset() {
    synth_->Reset();
    song_counter_ms = 0;
  }
  void Tick() {
    SetEvent(tick_event);
  }
  void RequestStop() {
    request_stop_ = true;
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
  Mode mode() { return mode_; }
  void set_mode(Mode mode) { mode_ = mode; }
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
  Mode mode_;
  bool initialized_,request_stop_;
  DWORD InstancePlayThread();
};

}
}

#endif