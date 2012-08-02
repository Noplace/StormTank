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
#ifndef AUDIO_SYNTH_UTIL_H
#define AUDIO_SYNTH_UTIL_H

namespace audio {
namespace synth {

class Util : public Component {
 public:
  Util() : Component() {
      real_t A4_freq = 440;
      chromatic_scale_freq = new real_t[note_count];
      for (int i=0;i<note_count;++i) {
        chromatic_scale_freq[i] = A4_freq * pow(_2_POW_12TH,i-69);//45 = index of A4
      }

      
  }

  virtual ~Util() {
    delete [] chromatic_scale_freq;
  }

  void set_sample_rate(uint32_t sample_rate) { 
    Component::set_sample_rate(sample_rate);
    sr_inv = 1.0f/real_t(sample_rate_);
  }

  real_t NoteFreq(Notes note) {
    return chromatic_scale_freq[note];
  }

  real_t* chromatic_scale_freq;
  real_t sr_inv;
};

}
}

#endif