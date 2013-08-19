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
#include "pad.h"

namespace audio {
namespace synth {
namespace instruments {

Pad::Pad() : InstrumentProcessor(),padsynth() {
 memset(&buffer,0,sizeof(buffer));
 memset(&harmonics_array,0,sizeof(harmonics_array));
}

int Pad::Load() {
  if (loaded_ == true)
    return S_FALSE;

  /*auto choir3 = [&](real_t freq) {
    padsynth.setharmonic(0,0);
	  for (int i=1;i<64;i++) {
	      real_t ai =1.0f/i;
	      real_t formants=exp(-pow((i*freq-600.0f)/150.0f,2.0f))+exp(-pow((i*freq-900.0f)/250.0f,2.0f))+
		  exp(-pow((i*freq-2200.0f)/200.0f,2.0f))+exp(-pow((i*freq-2600.0f)/250.0f,2.0f))+
		  exp(-pow((i*freq)/3000.0f,2.0f))*0.1f;
	      ai*=formants;
        padsynth.setharmonic(i,ai*4);
    }
  };*/

  sample_size_ = 32768;
  padsynth.Initialize(sample_size_,sample_rate_,64);




  for (int i=0;i<128;++i) {
    real_t A4_freq = 440;
    real_t two = pow(2.0f,1/12.0f);
    auto freq = A4_freq * pow(two,i-69);//69 = index of A4
    buffer[i] = new float[sample_size_];
    CalculateHarmonics(freq);
    for (int j=0;j<64;j++) {
	    padsynth.setharmonic(j,harmonics_array[j]);
    };
    padsynth.synth(freq,60,1,buffer[i]);
  }

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

int Pad::Unload() {
  if (loaded_ == false)
    return S_FALSE;
  for (int i=0;i<128;++i) {
    SafeDeleteArray(&buffer[i]);
  }
  padsynth.Deinitialize();
  loaded_ = false;
  return S_OK;
}

real_t Pad::Tick( int note_index) {
  auto& nd = cdata->pad_table[note_index];

  real_t result = 0;
  result += buffer[cdata->note_data_array[note_index].note][nd.phase];
  nd.phase = (nd.phase + 1) % sample_size_;

  result *= adsr[note_index].Tick();

  return result;
}

int Pad::SetFrequency(real_t freq, int note_index) {
  cdata->pad_table[note_index].phase = rand() % 30000;
  return S_OK;
}

int Pad::NoteOn(int note_index) {
  adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
  return S_OK;
}

int Pad::NoteOff(int note_index) {
  adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
  return S_OK;
}
    


}
}
}