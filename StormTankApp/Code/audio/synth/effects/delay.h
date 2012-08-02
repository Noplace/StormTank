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
#ifndef AUDIO_SYNTH_EFFECTS_DELAY_H
#define AUDIO_SYNTH_EFFECTS_DELAY_H

#include "effect.h"

namespace audio {
namespace synth {
namespace effects {

class Delay : public Effect {
 public:
  Delay() : left_buffer(nullptr),right_buffer(nullptr), max_samples(0), buffer_index(0),delay_ms(200),feedback(0.3f) {
  }
  ~Delay() {
    Deinitialize();
  }

  void Initialize(int max_samples)  {
    this->max_samples = max_samples;
    left_buffer = new real_t[max_samples];
    right_buffer = new real_t[max_samples];
    memset(left_buffer,0,max_samples*sizeof(real_t));
    memset(right_buffer,0,max_samples*sizeof(real_t));
  }

  void Deinitialize() {
    SafeDeleteArray(&left_buffer);
    SafeDeleteArray(&right_buffer);
  }

  void ProcessStereo(real_t* output_buffer, real_t* input_buffer, uint32_t count) {
    for (uint32_t i=0;i<count<<1;i+=2) {
      //if (buffer_index >= max_samples)
      //  buffer_index = 0;
      left_buffer[buffer_index] = input_buffer[i];
      right_buffer[buffer_index] = input_buffer[i+1];
      //int j = buffer_index - delay_index;
      //if ( j < 0)
      //  j += max_samples;

      unsigned delayed_index = ( buffer_index - delay_index + max_samples ) % max_samples;
      /*if ( j != g ){
        int a = 1;
        a++;
      }*/
      output_buffer[i] = left_buffer[buffer_index] = (input_buffer[i] + (left_buffer[delayed_index]*feedback));
      output_buffer[i+1] = right_buffer[buffer_index] = (input_buffer[i+1] + (right_buffer[delayed_index]*feedback));
      //++buffer_index;
      buffer_index = (buffer_index + 1) % max_samples;
    }
  }

  void ProcessMono(real_t* output_buffer, real_t* input_buffer, uint32_t count) {
    for (uint32_t i=0;i<count;++i) {
      left_buffer[buffer_index] = input_buffer[i];
      unsigned delayed_index = ( buffer_index - delay_index + max_samples ) % max_samples;
      output_buffer[i] = left_buffer[buffer_index] = (input_buffer[i] + (left_buffer[delayed_index]*feedback));
      buffer_index = (buffer_index + 1) % max_samples;
    }
  }

  void set_feedback(real_t feedback) { this->feedback = feedback; }
  void set_delay_ms(real_t delay_ms) { 
    this->delay_ms = delay_ms; 
    delay_index = int( delay_ms * 0.001f * sample_rate_ ); 
  }
 private:
  real_t* left_buffer;
  real_t* right_buffer;
  real_t feedback,delay_ms;
  int buffer_index,max_samples;
  int delay_index;
};

}
}
}

#endif