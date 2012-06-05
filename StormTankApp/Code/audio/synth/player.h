#include <WinCore/windows/windows.h>
#include <WinCore/timer/timer2.h>
#include <xnamath.h>
#include "../output/output.h"
#include "synth.h"

namespace audio {
namespace synth {

class Player {
 public:
  Player() : state(0), initialized_(false),audio_interface_(nullptr),tc_event(nullptr),thread_handle(nullptr),thread_id(0) {
    
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
  void set_audio_interface(AudioOutputInterface* audio_interface) { audio_interface_ = (AudioOutputInterface*)audio_interface; }
 private:
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  audio::AudioOutputInterface* audio_interface_;
  Synth synth;
  double song_time;
  HANDLE thread_handle,tc_event;
  DWORD thread_id;
  int state;
  bool initialized_;
  
};

}
}