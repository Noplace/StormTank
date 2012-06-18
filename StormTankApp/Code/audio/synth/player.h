
namespace audio {
namespace synth {

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
  void LoadMidi(char* filename);
  void Play();
  void Pause();
  void Stop();
  double GetPlaybackSeconds();
  void set_audio_interface(AudioOutputInterface* audio_interface) { audio_interface_ = (AudioOutputInterface*)audio_interface; }
 private:
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  audio::AudioOutputInterface* audio_interface_;
  instruments::Instrument* instr[128];
  Channel* channels[16];
  short* output_buffer;
  struct {
    Event* event_sequence;
    uint32_t event_index,event_count;
    Event* getCurrentEvent() {
      return &event_sequence[event_index];
    }
  }tracks[16];

  HANDLE thread_handle,tc_event;
  double song_pos_ms,song_length_ms, song_counter_ms;
  double sample_time_ms;
  Util util;
  DWORD thread_id;
  
  int state;
  bool initialized_;
  void RenderSamples(uint32_t samples_count);
  void MixChannels(double& output_left,double& output_right);
};

}
}