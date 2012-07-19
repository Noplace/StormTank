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
#ifndef AUDIO_OUTPUT_AUDIO_OUTPUT_INTERFACE_H
#define AUDIO_OUTPUT_AUDIO_OUTPUT_INTERFACE_H

namespace audio {
namespace output {

class Interface {
 public:
  virtual int Initialize(uint32_t sample_rate,uint8_t channels,uint8_t bits) = 0;
  virtual int Deinitialize() = 0;
  virtual int Play() = 0;
  virtual int Stop() = 0;
  virtual uint32_t GetBytesBuffered() = 0;
  virtual void GetCursors(uint32_t& play, uint32_t& write) = 0;
  virtual int Write(void* data_pointer, uint32_t size_bytes) = 0;
  const WAVEFORMATEX& wave_format() { return wave_format_; }
 protected:
  WAVEFORMATEX wave_format_;
};

}
}

#endif