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
#include "violin.h"

namespace audio {
namespace synth {
namespace instruments {

Violin::Violin() : Pad() {
 
}

int Violin::Load() {
  if (loaded_ == true)
    return S_FALSE;
 
  int hr = Pad::Load();
  if (hr != S_OK)
    return hr;

  //default adsr, should be instrument specific
  for (int i=0;i<Polyphony;++i) {
    adsr[i].set_sample_rate(sample_rate_);
    adsr[i].set_attack_amp(1.0f);
    adsr[i].set_sustain_amp(0.6f);
    adsr[i].set_attack_time_ms(200.0f);
    adsr[i].set_decay_time_ms(50.0f);
    adsr[i].set_release_time_ms(1300.5f);
  }

  return  hr;
}

int Violin::Unload() {
  if (loaded_ == false)
    return S_FALSE;
  
  return Pad::Unload();
}

real_t Violin::Tick(InstrumentData* data, int note_index) {
  auto cdata = (ViolinData*)data;
  real_t result = 0;
  {
    auto& nd = cdata->pad_table[note_index];
    result += buffer[cdata->note_data_array[note_index].note][nd.phase];
    nd.phase = (nd.phase + 1) % sample_size_;

  }
  result *= adsr[note_index].Tick();
  return result;
}

int Violin::SetFrequency(real_t freq, InstrumentData* data, int note_index) {
  auto cdata = (ViolinData*)data;
  cdata->pad_table[note_index].phase = rand() % sample_size_;
  return S_OK;
}

int Violin::NoteOn(InstrumentData* data, int note_index) {
  Pad::NoteOn(data,note_index);
  return S_OK;
}

int Violin::NoteOff(InstrumentData* data, int note_index) {
  Pad::NoteOff(data,note_index);
  return S_OK;
}

void Violin::CalculateHarmonics(real_t freq) {

  memset(&harmonics_array,0,sizeof(harmonics_array));

  int hcount = int(real_t(sample_rate_) / (2.0f*freq));
  hcount = min(64,hcount);
  for (int i=0;i<hcount;++i) {

    //if ((i%2)) {
    real_t x = (i/64.0f)-4.5f;
      harmonics_array[i] = exp((-x*x/2));//(1.0f / (i));
    //}

    ///harmonics_array[i] = 64.0f/(i+1);
    //if (!(i % 2))
    //  harmonics_array[i] *= 10.0f;

    //harmonics_array[i] *= (2*freq/real_t(sample_rate_));

    //else
     //harmonics_array[i] *= cos(6*XM_PI*i/64.0f);
  }


  //  harmonics_array[0] = 1.0f; 
  //harmonics_array[1] = 1.5f; 
  //harmonics_array[2] = 0.5f; 
  //harmonics_array[3] = 0.2f; 
  //harmonics_array[4] = 0.0f; 
   
}

}
}
}