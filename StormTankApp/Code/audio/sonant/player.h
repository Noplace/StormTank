class SonantPlayer {
 public:
  SonantPlayer() : state(0), initialized_(false),audio_interface_(nullptr),event1(nullptr),thread_handle(nullptr),thread_id(0) {
    rowlen = endpattern = 0;
  }
  ~SonantPlayer() {
    Deinitialize();
  }
  void Initialize();
  void Deinitialize();
  void LoadSong(song* song_data,int rowlen,int endpattern,int* progress_var);
  void Play();
  void Pause();
  void Stop();
  void set_audio_interface(audio::AudioOutputInterface* audio_interface) { audio_interface_ = (audio::AudioOutputInterface*)audio_interface; }
 private:
  static DWORD WINAPI PlayThread(LPVOID lpThreadParameter);
  song* song_data;
  short* row_buffer,*output_buffer;
  audio::AudioOutputInterface* audio_interface_;
  double row_time;
  HANDLE thread_handle,event1;
  DWORD thread_id;
  int state;
  int rowlen;
  int endpattern;
  int   current_row;
  int current_pattern;

  bool initialized_;
  
};