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
#include "sonant_program.h"

#define _4K_SONANT_ROWLEN_ 4593 // In sample lengths

namespace audio {
namespace synth {
namespace instruments {


// General
static float sin4k(float value) {
	return sin(value);
}
 static float cos4k(float value) {
	return cos(value);
}
static unsigned int randseed = 1;
static float randfloat4k() {
	return (float) ((randseed *= 0x15a4e35) % 255) / 255.0f;
}


// Oscillators
__forceinline static float osc_sin(float value) {
    return sin4k(value * 2.0f * 3.141592f);
}

__forceinline static float osc_square(float value) {
    if(osc_sin(value) < 0) return -1.0f;
    return 1.0f;
}

__forceinline static float osc_saw(float value) {
    float result;
  #ifdef _M_X64
    result = (value - (int64_t)value);
  #else
    __asm {
      fld1
      fld value
      fprem
      fstp st(1)
      fstp result
    }
  #endif
	return result - .5f;
}

__forceinline static float osc_tri(float value) {
    float v2 = (osc_saw(value) + .5f) * 4.0f;
    if(v2 < 2.0f) return v2 - 1.0f;
    return 3.0f - v2;
}

// Renderer
/*!
    * Returns the frequency for the supplied value.
    * @param fFrequency Initial frequency.
    * @param fMultiplier Multiplier per level.
    * @param nValue An integer value.
    * @return Frequency of @c nValue.
    */
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

__forceinline static float getnotefreq(unsigned char n) {
    return getFrequency(0.00390625f, 1.059463094f, n, 128);
}
//! Oscillator function-pointer type.
typedef float (* const oscillator_t)(float);
 
//! Array of oscillator function-pointers for branchless linear access.
static oscillator_t const afpOscillator[4] =
{
	osc_sin,
	osc_square,
	osc_saw,
	osc_tri
};

/*!
    * Returns an oscillation depending on the supplied waveform and value.
    * @param nWaveform Waveform.
    * @param fValue A floating-point value.
    * @return An oscillation depending on the supplied waveform and value.
    */
static inline float getoscoutput(unsigned char nWaveform,float fValue)
{
	return (*(afpOscillator + nWaveform))(fValue);
}


SonantProgram::SonantProgram() : InstrumentProcessor() {
  memset(&data,0,sizeof(data));
}

SonantProgram::~SonantProgram() {
  Unload();
}

int SonantProgram::Load() {
  if (loaded_ == true)
    return S_FALSE;

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

int SonantProgram::Unload() {
  if (loaded_ == false)
    return S_FALSE;

  loaded_ = false;
  return S_OK;
}

  
real_t SonantProgram::Tick(InstrumentData* data, int note_index) {
  auto cdata = (SonantProgramData*)data;
  auto& nd = cdata->table[note_index];
  //int note = cdata->note_data_array[note_index].note;
  int n = cdata->note_data_array[note_index].note+75;

  if (nd.i < nd.icount) {
    unsigned int b = nd.i + nd.currentpos;
    // LFO
    float t = getFrequency(1.0f,2.0f,this->data.lfo_freq,8) * (float) b / (float) _4K_SONANT_ROWLEN_;
    float lfor = getoscoutput(this->data.lfo_waveform,t) * (float) this->data.lfo_amt / 512.0f + .5f;
    // Envelope
    float e = 1.0f;
    if(nd.i < nd.attack) {
        e = (float) nd.i / (float) nd.attack;
    } else if(nd.i >= nd.attack + nd.sustain) {
        e -= (float) (nd.i - nd.attack - nd.sustain) / (float) nd.release;
    }
    // Oscillator 1
    t = getnotefreq(n + (this->data.osc1_oct - 8) * 12 + this->data.osc1_det) * (1.0f + .2f * (float) this->data.osc1_detune / 255.0f);
    if(this->data.lfo_osc1_freq) t += lfor;
    if(this->data.osc1_xenv) t *= e * e;
    nd.c1 += t;
    float r = getoscoutput(this->data.osc1_waveform,nd.c1);
    float rsample = r * (float) this->data.osc1_vol / 255.0f;
    // Oscillator 2
    t = getnotefreq(n + (this->data.osc2_oct - 8) * 12 + this->data.osc2_det) * (1.0f + .2f * (float) this->data.osc2_detune / 255.0f);
    if(this->data.osc2_xenv) t *= e * e;
    nd.c2 += t;
    r = getoscoutput(this->data.osc2_waveform,nd.c2);
    rsample += r * (float) this->data.osc2_vol / 255.0f
    // Noise oscillator
        + osc_sin(randfloat4k()) * (float) this->data.noise_fader / 255.0f * e;
    rsample *= e;
    // State variable filter
    float f = this->data.fx_freq;
    if(this->data.lfo_fx_freq) f *= lfor;
    f = 1.5f * sin4k(f * 3.141592f / 44100.0f);
    nd.low += f * nd.band;
    float high = nd.q * (rsample - nd.band) - nd.low;
    nd.band += f * high;
    switch(this->data.fx_filter) {
        case 1: // Hipass
            rsample = high;
            break;
        case 2: // Lopass
            rsample = nd.low;
            break;
        case 3: // Bandpass
            rsample = nd.band;
            break;
        case 4: // Notch
            rsample = nd.low + high;
    }
    t = osc_sin(getFrequency(1.0f,2.0f,this->data.fx_pan_freq,8) * (float) b / (float) _4K_SONANT_ROWLEN_) * (float) this->data.fx_pan_amt / 512.0f + .5f;
    rsample *= (float) (this->data.env_master * 156);
    //lbuffer[b] += rsample * (1.0f - t);
    //rbuffer[b] += rsample * t;
    ++nd.i;
    return rsample / 32767.0f;
  }
  ++nd.currentpos;
 return 0;
}

int SonantProgram::SetFrequency(real_t freq, InstrumentData* data, int note_index) {
  auto cdata = (SonantProgramData*)data;
  cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
  //cdata->table[note_index].inc1 = rand() % 4096;

  return S_OK;
}

int SonantProgram::NoteOn(InstrumentData* data, int note_index) {
  auto cdata = (SonantProgramData*)data;
  auto& nd = cdata->table[note_index];

  nd.attack = this->data.env_attack;
  nd.sustain = this->data.env_sustain;
  nd.release = this->data.env_release;

  nd.c1 = nd.c2 = 0;
  // State variable init
  nd.q = (float)this->data.fx_resonance / 255.0f;
  
  nd.low = nd.band = 0.0f;
  nd.i = 0;
  nd.icount = nd.attack + nd.sustain + nd.release;

  return S_OK;
}


int SonantProgram::NoteOff(InstrumentData* data, int note_index) {
  auto cdata = (SonantProgramData*)data;
  auto& nd = cdata->table[note_index];
  nd.i = nd.attack + nd.sustain;
  return S_OK;
}


}
}
}