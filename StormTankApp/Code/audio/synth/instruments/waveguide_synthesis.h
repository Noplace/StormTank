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
#ifndef AUDIO_SYNTH_INSTRUMENTS_WAVEGUIDE_SYNTHESIS_H
#define AUDIO_SYNTH_INSTRUMENTS_WAVEGUIDE_SYNTHESIS_H

#include "instrument.h"
#include "../filters/lowpass.h"
#include "../misc.h"
#include "../filters/iir_filter.h"

namespace audio {
namespace synth {
namespace instruments {

/*class LowPassFilter : public filters::IIRFilter<1,2> {
 public:
  void set_cutoff_freq(real_t cutoff_freq) { cutoff_freq_ = cutoff_freq; }
  void Update() {
    real_t fc = (cutoff_freq_)/(sample_rate_);
    real_t x = exp(-2*XM_PI*fc);
    a[0] = 1.0f-x;
    b[0] = 1.0f;
    b[1] = x;
  }
 protected:
  real_t cutoff_freq_;
};*/


class WaveguideSynthesisData : public InstrumentData {
 public:
  struct {
    uint32_t lphase,rphase;
  } table[Polyphony];
  WaveguideSynthesisData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class  WaveguideSynthesis : public InstrumentProcessor {
 public:
   WaveguideSynthesis() : InstrumentProcessor() {
    randseed = 1;
  }
  virtual ~WaveguideSynthesis() {
    Unload();
  }
  int Load() {
    if (loaded_ == true)
      return S_FALSE;


    for (int i=0;i<Polyphony;++i) {
      wavetables[i].count = (sample_rate_/8)+10;//lowest freq = 8
      wavetables[i].left = new real_t[wavetables[i].count]; 
      wavetables[i].right = new real_t[wavetables[i].count];
      memset(wavetables[i].left,0,sizeof(real_t)*wavetables[i].count);
      memset(wavetables[i].right,0,sizeof(real_t)*wavetables[i].count);

      llowpass[i].set_sample_rate(sample_rate_);
      llowpass[i].set_cutoff_freq(12050.0f);
      llowpass[i].Update();
      rlowpass[i].set_sample_rate(sample_rate_);
      rlowpass[i].set_cutoff_freq(12050.0f);
      rlowpass[i].Update();

      adsr[i].set_sample_rate(sample_rate_);
      adsr[i].set_attack_amp(0.8f);
      adsr[i].set_sustain_amp(0.6f);
      adsr[i].set_attack_time_ms(10.0f);
      adsr[i].set_decay_time_ms(4.0f);
      adsr[i].set_release_time_ms(20.5f);
    }

    /*for_each_note([&](int index,float freq){
      wavetables[index].count = int(ceil(sample_rate_ / freq));
      wavetables[index].buf = new real_t[wavetables[index].count];
      FillNoise(index);
    });*/

    loaded_ = true;
    return S_OK;
  }

  int Unload() {
    if (loaded_ == false)
      return S_FALSE;

    for (int i=0;i<Polyphony;++i) {
      SafeDeleteArray(&wavetables[i].left);
      SafeDeleteArray(&wavetables[i].right);
    }

    loaded_ = false;
    return S_OK;
  }
  InstrumentData* NewInstrumentData() {
    return new WaveguideSynthesisData();
  }
  real_t Tick(int note_index) {
    //auto cdata = (KarplusStrongData*)data;
    auto& nd = cdata->table[note_index];
    auto& wavetable = wavetables[note_index];

    
		auto out = wavetable.right[nd.rphase];
		out += wavetable.left[nd.lphase];


    auto prev_lphase =  nd.lphase;
    auto prev_rphase =  nd.rphase;
    nd.lphase = ((nd.lphase + 1) % wavetable.count);
    nd.rphase = ((nd.rphase + 1) % wavetable.count);      

    wavetable.left[prev_lphase] = llowpass[note_index].Tick(wavetable.left[nd.lphase]);
    wavetable.right[prev_rphase] = rlowpass[note_index].Tick(wavetable.right[nd.rphase]);

    

    /*
    real_t odrazL, odrazR;
		odrazR = wavetable.right[wavetable.count-1];
		odrazL = wavetable.left[0];

		//posunuti praveho pole doprava - uvolni se pozice vlevo (je dvakrat)
		//to je misto pro odrazL - odraz z leftline
    int i = wavetable.count;
		//for(int i = wavetable.count; i > 0; i--){
			//wavetable.right[i-1] = wavetable.right[i-2];
      
      //wavetable.left[wavetable.count-i] = wavetable.left[wavetable.count-i+1];
		//	}

		//posunuti leveho pole doleva - uvolni se pozice vpravo
		//misto pro odrazR - odraz z rightline
      //int j = 0;
		//for(int j = 0; j < wavetable.count; j++){
			
			

		//       rightline
		//       >>>>>>>>>>>>>>>>>odrazR>
		// zamek |                      | kobylka
		//       <odrazL<<<<<<<<<<<<<<<<<
		//                       leftline

		//druhy parametr kobylky je tlumeni
		//0.4955 je OK

		odrazR = odrazR;//Kobylka(odrazR, lowpass);
		odrazL = -odrazL;


		wavetable.right[0] = odrazL;
		wavetable.left[wavetable.count-1]  = odrazR;*/

    out *= adsr[note_index].Tick();
    return out;
  }
  int SetFrequency(real_t freq, int note_index) {
    cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
    wavetables[note_index].count = int(ceil(sample_rate_ / cdata->note_data_array[note_index].freq));
    cdata->table[note_index].lphase = 0;
    cdata->table[note_index].rphase = 0;
    FillNoise(note_index);
    return S_OK;
  }
  int NoteOn(int note_index) {
    //auto cdata = (KarplusStrongData*)data;

    adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }
  int NoteOff(int note_index) {
    //auto cdata = (KarplusStrongData*)data;
    adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
    return S_OK;
  }

  void set_instrument_data(InstrumentData* idata) {
    this->cdata = (WaveguideSynthesisData*)idata;
  }
 protected:
  WaveguideSynthesisData* cdata;
  struct {
    int count;
    real_t* left;
    real_t* right;
  } wavetables[Polyphony];
  LowPassFilter llowpass[Polyphony];
  LowPassFilter rlowpass[Polyphony];
  uint32_t randseed;
  

  inline void FillNoise(int index) {
    memset(wavetables[index].left,0,sizeof(real_t)*sample_rate_/8);
    memset(wavetables[index].right,0,sizeof(real_t)*sample_rate_/8);
    for (int i=0;i<wavetables[index].count;++i) {
      real_t t = (real_t(i)/(wavetables[index].count-1));
      real_t A = sin(t*2*XM_PI)*0.3f;
      wavetables[index].left[i] = A+0.5f*RandomFloat(&randseed);
      wavetables[index].right[i] = A+0.5f*RandomFloat(&randseed);
    }
  }
};

}
}
}


#endif