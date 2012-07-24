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
#include <WinCore/windows/windows.h>
#include <WinCore/types.h>
#include <audiodefs.h>
#include <dsound.h>
#include "interface.h"

namespace audio {
namespace output {

class DirectSound : public Interface {
 public:
  DirectSound();
  ~DirectSound();
  int Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits);
  int Deinitialize();
  int Play();
  int Stop();
  uint32_t GetBytesBuffered();
  void GetCursors(uint32_t& play, uint32_t& write);
  int Write(void* data_pointer, uint32_t size_bytes);
  
  void set_buffer_size(uint32_t buffer_size) { buffer_size_ = buffer_size; }
 protected:
  IDirectSound8* ds8;
  LPDIRECTSOUNDBUFFER primary_buffer;
  LPDIRECTSOUNDBUFFER secondary_buffer;
  //uint8_t* audio_data;  
  //uint32_t offset_;
 // uint32_t written_bytes;
  uint32_t buffer_size_;
  //uint32_t next_pos;
  DWORD last_write_cursor;
};

}
}