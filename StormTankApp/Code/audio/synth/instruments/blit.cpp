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
#include "blit.h"

namespace audio {
namespace synth {
namespace instruments {

BlitWave::BlitWave() : InstrumentProcessor() {
  memset(wavetables,0,sizeof(wavetables));
}

BlitWave::~BlitWave() {
  Unload();
}

int BlitWave::Load() {
  if (loaded_ == true)
    return S_FALSE;
  GenerateWavetables();
  //default adsr, should be instrument specific
  for (int i=0;i<Polyphony;++i) {
    adsr[i].set_sample_rate(sample_rate_);
    adsr[i].set_attack_amp(1.0f);
    adsr[i].set_sustain_amp(0.6f);
    adsr[i].set_attack_time_ms(8.0f);
    adsr[i].set_decay_time_ms(8.0f);
    adsr[i].set_release_time_ms(100.5f);
  }
  loaded_ = true;
  return S_OK;
}

int BlitWave::Unload() {
  if (loaded_ == false)
    return S_FALSE;
  for (int i=0;i<128;++i)
    SafeDeleteArray(&wavetables[i]);
  loaded_ = false;
  return S_OK;
}

  
real_t BlitWave::Tick(int note_index) {
  
  int note = cdata->note_data_array[note_index].note;
  
  real_t result = 0.2f*wavetables[note][cdata->table[note_index].inc1];
  cdata->table[note_index].inc1 = (cdata->table[note_index].inc1 + 1) % 4096;
  return result*adsr[note_index].Tick();
}

int BlitWave::SetFrequency(real_t freq, int note_index) {
  cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
  cdata->table[note_index].inc1 = rand() % 4096;
  return S_OK;
}

int BlitWave::NoteOn(int note_index) {
  adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);  
  return S_OK;
}


int BlitWave::NoteOff(int note_index) {
  adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity); 
  return S_OK;
}

void BlitWave::GenerateWavetables() {
  real_t A4_freq = 440.0f;
  real_t two = pow(2.0f,1/12.0f);
  real_t f_s = real_t(sample_rate_);

  for (int note=0;note<128;++note) {
    if (wavetables[note] == nullptr)
      wavetables[note] = new real_t[4096];

    auto freq = A4_freq * pow(two,note-69.0f);
    auto harmonics = uint32_t(f_s / (freq * 2.0f));

    for (int n=0;n<4096;++n) {
      real_t result = 0.0f;
      real_t x = 2.0f*XM_PI*n*freq/(f_s);
      for (uint32_t i=1;i<=harmonics;++i) {
        real_t h = real_t(i);
        real_t m = pow(cos((i-1.0f)*XM_PI/(2.0f*harmonics)),2.0f);
        result += m*(1/h)*sin(h*x);
      }
      wavetables[note][n] = result;
    }

  }

}

}
}
}