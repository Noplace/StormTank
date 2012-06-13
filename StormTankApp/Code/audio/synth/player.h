
namespace audio {
namespace synth {


struct Event {
  double pos_ms;
  int channel;
  int command;
  Notes note;

  /*union {
    struct {
      uint8_t note_on:1; 
      uint8_t note_off:1;
      uint8_t tempo:1;
      uint8_t prog_change:1;
      uint8_t etc1:1;
      uint8_t etc2:1;
      uint8_t etc3:1;
      uint8_t etc4:1;
    };
    uint8_t raw;
  } type;*/
};


class Player {
 public:
  Player() : state(0), initialized_(false),audio_interface_(nullptr),tc_event(nullptr),thread_handle(nullptr),thread_id(0) {
    
  }
  ~Player() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void LoadMidi(MidiFile* midi);
  void Play();
  void Pause();
  void Stop();
  double GetPlaybackSeconds();
  void set_audio_interface(AudioOutputInterface* audio_interface) { audio_interface_ = (AudioOutputInterface*)audio_interface; }
 private:
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  audio::AudioOutputInterface* audio_interface_;
  instruments::SineWave* sine_instr[16];
  Channel* channels[16];
  Event* event_sequence;
  HANDLE thread_handle,tc_event;
  double song_pos_ms,song_length_ms;
  Util util;
  DWORD thread_id;
  uint32_t event_index,event_count;
  int state;
  bool initialized_;

  void MixChannels(double& output_left,double& output_right);
};

}
}