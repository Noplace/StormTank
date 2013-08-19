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
#include "percussion.h"
#include "../filters/chebyshev_filter.h"

audio::synth::filters::ChebyshevFilter filter;


namespace audio {
namespace synth {
namespace instruments {

static float getFrequency(float fFrequency, float fMultiplier,
unsigned char nValue, unsigned char nLimit)
{
	if(nValue > nLimit)
	{
		nValue -= nLimit;
	}
	else
	{
		nValue = nLimit - nValue;
		fMultiplier = 1.0f / fMultiplier;
	}
 
	do
		fFrequency *= fMultiplier;
	while(--nValue);
 
	return fFrequency;
}

static float getnotefreq(unsigned char n) {
    return getFrequency(0.00390625f, 1.059463094f, n, 128);
}

static float osc_sin(float value) {
    return sin(value * 2.0f * 3.141592f);
}

class Filter {
 public:
  float low,band,q,freq;
  void Init(float freq) {
    this->freq = freq;
    float res = 240.0f;
      q = (float)res / 255.0f;
                        low = band = 0.0f;
  }

  real_t Tick(real_t rsample) {
    //5900.0f;//instrument->fx_freq;
        
          real_t f = 1.5f * sin(freq * 3.141592f / 44100.0f);
          low += f * band;
          float high = q * (rsample - band) - low;
          band += f * high;
          /*switch(instrument->fx_filter) {
              case 1: // Hipass
                  rsample = high;
                  break;
              case 2: // Lopass
                  rsample = low;
                  break;
              case 3: // Bandpass
                  rsample = band;
                  break;
              case 4: // Notch
                  rsample = low + high;
          }*/
          return low;
  }

}bassdrum2,snare;

Percussion::PercussionPieceTick Percussion::ticks[Polyphony] = {
  &Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,
  &Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,
};

int Percussion::Load() {
  if (loaded_ == true)
    return S_FALSE;
  inv_sr = 1 / real_t(sample_rate_);
  bassdrum_osc1.set_sample_rate(sample_rate_);
  bassdrum_osc2.set_sample_rate(sample_rate_);
  hihat_osc.set_sample_rate(sample_rate_); 

  snare.Init(11024.0f);
  bassdrum2.Init(5900.0f);
  //default adsr, should be instrument specific
  for (int i=0;i<Polyphony;++i) {
    adsr[i].set_sample_rate(sample_rate_);
    adsr[i].set_attack_amp(1.0f);
    adsr[i].set_sustain_amp(0.6f);
    adsr[i].set_attack_time_ms(8.0f);
    adsr[i].set_decay_time_ms(8.0f);
    adsr[i].set_release_time_ms(10.5f);
  }

  filter.Initialize(0.2f,0,0,4);

  loaded_ = true;
  return S_OK;
}

int Percussion::Unload() {
  if (loaded_ == false)
    return S_FALSE;
 
  loaded_ = false;
  return S_OK;
}

real_t Percussion::Tick(int note_index) {
  auto& table = cdata->table;
  real_t result = 0;
  
  
  //result = (this->*(ticks[note_index]))((PercussionData*)data,note_index);
  if (adsr[note_index].stage() != 0 && cdata->note_data_array[note_index].note == 46) {
    real_t value = (real_t)((randseed *= 0x15a4e35) % 255) / 255.0f;
    result = sin(value * 2.0f * XM_PI);
    result *= 0.2f*adsr[note_index].Tick();
  }

  if (adsr[note_index].stage() != 0 && 
    (cdata->note_data_array[note_index].note == 35 || cdata->note_data_array[note_index].note == 36)) {
    real_t e = adsr[note_index].Tick();
    real_t f = 180;//getnotefreq(123);
    f *= e*e;
    table.bassdrum_inc1 = bassdrum_osc1.get_increment(f);
    table.bassdrum_inc2 = bassdrum_osc1.get_increment(f);
    result += bassdrum_osc1.Tick(table.bassdrum_phase1,table.bassdrum_inc1);
    //result += bassdrum_osc2.Tick(table.bassdrum_phase2,table.bassdrum_inc2);
    real_t value = (real_t)((randseed *= 0x15a4e35) % 255) / 255.0f;
   

    result *= e;

    result = bassdrum2.Tick(result);//
    //result = filter.Tick(result);
    
  }

  //hihat
  /*table.hihat_inc[0] = bassdrum.get_increment(460.0+460.0*sin(value * 2.0f * XM_PI));
  table.hihat_phase[1] = 0;
  table.hihat_inc[1] = bassdrum.get_increment(1840.0+1840.0*sin(value * 2.0f * XM_PI));
  table.hihat_phase[2] = 0;
  table.hihat_inc[2] = bassdrum.get_increment(5404.0+5404.0*sin(value * 2.0f * XM_PI));
  table.hihat_phase[3] = 0;
  table.hihat_inc[3] = bassdrum.get_increment(3220.0+3220.0*sin(value * 2.0f * XM_PI));
  auto h1 = hihat_osc.Tick(table.hihat_phase[0],table.hihat_inc[0]) - hihat_osc.Tick(table.hihat_phase[1],table.hihat_inc[1]) - 1.0;
  auto h2 = hihat_osc.Tick(table.hihat_phase[2],table.hihat_inc[2]) - hihat_osc.Tick(table.hihat_phase[3],table.hihat_inc[3]) - 1.0;
  result = 0.4*table.highhat_amp*(h1-h2-1.0);//*sin(value * 2.0f * XM_PI);;
  table.highhat_amp = max(0,table.highhat_amp - inv_sr);
  */

  //result += 0.4f*table.bassdrum_amp*bassdrum.Tick(table.bassdrum_phase,table.bassdrum_inc);
  //table.bassdrum_amp = max(0,table.bassdrum_amp - 2*inv_sr);


  return result;
}

int Percussion::SetFrequency(real_t freq, int note_index) {
  cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
  return S_OK;
}

int Percussion::NoteOn(int note_index) {
  NoteData* nd = &cdata->note_data_array[note_index];

  if ((nd->note == 35||nd->note == 36) && nd->on == true) {
    cdata->table.bassdrum_amp = 1.0;
  }
  
  if (nd->note == 42 && nd->on == true) {
    cdata->table.highhat_closed = true;
    cdata->table.highhat_amp = 1.0;
  }

  if (nd->note == 46 && nd->on == true) {
    cdata->table.highhat_closed = true;
    cdata->table.highhat_amp = 1.0;
  }
 
  cdata->table.hihat_phase[0] = 0;
  cdata->table.hihat_inc[0] = hihat_osc.get_increment(460.0f);
  cdata->table.hihat_phase[1] = 0;
  cdata->table.hihat_inc[1] = hihat_osc.get_increment(1840.0f);
  cdata->table.hihat_phase[2] = 0;
  cdata->table.hihat_inc[2] = hihat_osc.get_increment(5404.0f);
  cdata->table.hihat_phase[3] = 0;
  cdata->table.hihat_inc[3] = hihat_osc.get_increment(7220.0f);

  adsr[note_index].NoteOn(nd->velocity);
  return S_OK;
}

int Percussion::NoteOff(int note_index) {
  NoteData* nd = &cdata->note_data_array[note_index];

  if (nd->note == 42 && nd->on == false) {
    cdata->table.highhat_closed = false;
    cdata->table.highhat_amp = 0;
  }
  if (nd->note == 46 && nd->on == false) {
    cdata->table.highhat_closed = false;
    cdata->table.highhat_amp = 0;
  }
  if ((nd->note == 35||nd->note == 36) && nd->on == false) {
    cdata->table.bassdrum_amp = 0;
  }
 
  adsr[note_index].NoteOff(nd->velocity);
  return S_OK;
}


real_t Percussion::HihatOpenTick(PercussionData* data, int note_index) {

  real_t value = (real_t)((randseed *= 0x15a4e35) % 255) / 255.0f;
  auto result = 0.6f*data->table.highhat_amp*sin(value * 2.0f * XM_PI);
  data->table.highhat_amp = max(0,data->table.highhat_amp - inv_sr);
  return result * adsr[note_index].Tick();  
}

real_t Percussion::HihatClosedTick(PercussionData* data, int note_index) {

  real_t value = (real_t)((randseed *= 0x15a4e35) % 255) / 255.0f;
  auto result = 0.6f*data->table.highhat_amp*sin(value * 2.0f * XM_PI);
  data->table.highhat_amp = max(0,data->table.highhat_amp - inv_sr);
  return result * adsr[note_index].Tick();  
}

real_t Percussion::BassDrumTick(PercussionData* data, int note_index) {
  return 0;
}

}
}
}