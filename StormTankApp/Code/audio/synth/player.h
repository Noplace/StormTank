
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

#define kInstrumentCount 128
#define kChannelCount 16

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
  struct Track {
    midi::Event* event_sequence;
    uint32_t event_index,event_count,ticks_to_next_event;
    midi::Event* getCurrentEvent() {
      return &event_sequence[event_index];
    }
  };
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  audio::AudioOutputInterface* audio_interface_;
  instruments::Instrument* instr[kInstrumentCount];
  Channel* channels[kChannelCount];
  short* output_buffer;
  Track* tracks;
  int track_count;
  HANDLE thread_handle,tc_event;
  double song_pos_ms,song_length_ms, song_counter_ms;
  double sample_time_ms;
  Util util;
  DWORD thread_id;
  uint32_t ticks_per_beat;
  uint32_t samples_to_next_event;
  double bpm;
  int state;
  bool initialized_;
  midi::Event* last_event;
  void DestroyTrackData() {
    for (int i=0;i<track_count;++i)
      SafeDeleteArray(&tracks[i].event_sequence);
    SafeDeleteArray(&tracks);
    track_count = 0;
    tracks = nullptr;
  }
  void ResetTracks() {
    for (int track_index=0;track_index<track_count;++track_index) {
      auto& track = tracks[track_index];
      track.event_index = 0;      
      track.ticks_to_next_event = track.event_sequence[0].deltaTime;
    }
  }
  midi::Event* GetNextEvent();
  void HandleEvent(midi::Event* event);
  void RenderSamples(uint32_t samples_count);
  void RenderSamples2(uint32_t samples_count);
  void MixChannels(double& output_left,double& output_right);
};

}
}