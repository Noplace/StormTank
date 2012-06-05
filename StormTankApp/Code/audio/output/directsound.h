#include <WinCore/windows/windows.h>
#include <WinCore/types.h>
#include <audiodefs.h>
#include <dsound.h>
#include "audio_output_interface.h"

namespace audio {

class DirectSound : public AudioOutputInterface {
 public:
  DirectSound();
  ~DirectSound();
  int Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits);
  int Deinitialize();
  int Play();
  int Stop();
  uint32_t GetBytesBuffered();
  int Write(void* data_pointer, uint32_t size_bytes);
  void set_window_handle(HWND window_handle) { window_handle_ = window_handle; }
  void set_buffer_size(uint32_t buffer_size) { buffer_size_ = buffer_size; }
 protected:
  IDirectSound8* ds8;
  LPDIRECTSOUNDBUFFER primary_buffer;
  LPDIRECTSOUNDBUFFER secondary_buffer;
  //uint8_t* audio_data;  
  void* window_handle_;
  //uint32_t offset_;
 // uint32_t written_bytes;
  uint32_t buffer_size_;
  //uint32_t next_pos;
  DWORD last_write_cursor;
};

}