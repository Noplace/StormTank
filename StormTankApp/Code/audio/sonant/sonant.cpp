// Sonant
//  Synth system targeted for 4k intros.

//  Many thanks to Decipher for alot of the optimizations used here, and also to
//   Waffle for much of the playsound code and general help. Huge thanks is also
//   due to Gargaj, who helped me to understand alot of synth design etc.

// (C) 2008-2009 Jake Taylor [ Ferris / Youth Uprising ]

#include "sonant.h"

#pragma comment(lib, "winmm.lib")

// General

#define sin4k sinf
#define cos4k cosf


static unsigned int randseed = 1;
static float randfloat4k() {
	return (float) ((randseed *= 0x15a4e35) % 255) / 255.0f;
}

// Sound-Specific
#define WAVE_CHAN 2 // channels
#define WAVE_SPS 44100 // samples per second
#define WAVE_BITS 16 // bits per sample
#define WAVE_ALIGN WAVE_CHAN * WAVE_BITS / 8 // bytes per sample
#define WAVE_SIZE WAVE_CHAN * WAVE_SPS * 240 // buffer size in samples
#define AUDIO_CLIPAMP 32767 // audio clipping amplitude


//short* wave_buffer[WAVE_SIZE * WAVE_CHAN];
/*
static HWAVEOUT wave_handle;

static WAVEFORMATEX wave_format = {
	WAVE_FORMAT_PCM, //wFormatTag
	WAVE_CHAN, // nChannels
	WAVE_SPS, // nSamplesPerSec
	WAVE_ALIGN * WAVE_SPS, // nAvgBytesPerSec
	WAVE_ALIGN, // nBlockAlign
	WAVE_BITS // wBitsPerSample
};

static WAVEHDR wave_header = {
	(char *) 0, // lpData
	WAVE_SIZE * WAVE_CHAN * WAVE_BITS / 8, // dwBufferLength
	0, // dwBytesRecorded
	0, // dwUser
	0, // dwFlags
	0, // dwLoops
	0, // lpNext
	0 // reserved
};*/


__forceinline  int clip(int value) {
	if(value > AUDIO_CLIPAMP) return AUDIO_CLIPAMP;
	if(value < -AUDIO_CLIPAMP) return -AUDIO_CLIPAMP;
	return value;
}

// Oscillators
__forceinline float osc_sin(float value) {
    return sin4k(value * 2.0f * 3.141592f);
}

__forceinline float osc_square(float value) {
    if(osc_sin(value) < 0) return -1.0f;
    return 1.0f;
}

__forceinline float osc_saw(float value) {
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
/*	asm(
        "fld1;"
        "fld %1;"
        "fprem;"
        "fstp %%st(1);"
        "fstp %0;"
        : "=m" (result)
        : "m" (value)
	);*/
	return result - .5f;
}

__forceinline  float osc_tri(float value) {
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
static float getFrequency(float fFrequency, float fMultiplier,unsigned char nValue, unsigned char nLimit)
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




struct state {
  char note;
  bool new_note;
  unsigned int attack;
  unsigned int sustain;
  unsigned int release;
  double c1,c2;
  // State variable init
  float q;
  float low,band;
  unsigned int i;
};

state inst_state[8];
float note_freq[256];
void sonantInit() {
  memset(inst_state,0,sizeof(inst_state));
  inst_state[0].new_note = true;
  inst_state[1].new_note = true;
  inst_state[2].new_note = true;
  inst_state[3].new_note = true;
  inst_state[4].new_note = true;
  inst_state[5].new_note = true;
  inst_state[6].new_note = true;
  inst_state[7].new_note = true;

  for (int i=0;i<=256;++i)
  note_freq[i] = getnotefreq(i);
}

float generateSample(state& state,instrument* instrument,unsigned int currentpos,int rowlen,float& pan) {
  
  unsigned int b = state.i + currentpos;
  // LFO
  float t = getFrequency(1.0f,2.0f,instrument->lfo_freq,8) * (float) b / (float) rowlen;
  float lfor = getoscoutput(instrument->lfo_waveform,t) * (float) instrument->lfo_amt / 512.0f + .5f;
  // Envelope
  float e = 1.0f;
  if(state.i < state.attack) {
      e = (float) state.i / (float) state.attack;
  } else if(state.i >= state.attack + state.sustain) {
      e -= (float) (state.i - state.attack - state.sustain) / (float) state.release;
  }
  // Oscillator 1
  t = getnotefreq(state.note + (instrument->osc1_oct - 8) * 12 + instrument->osc1_det) * (1.0f + .2f * (float) instrument->osc1_detune / 255.0f);
  if(instrument->lfo_osc1_freq) t += lfor;
  if(instrument->osc1_xenv) t *= e * e;
  state.c1 += t;
  float r = getoscoutput(instrument->osc1_waveform,state.c1);
  float rsample = r * (float) instrument->osc1_vol / 255.0f;
  // Oscillator 2
  t = getnotefreq(state.note + (instrument->osc2_oct - 8) * 12 + instrument->osc2_det) * (1.0f + .2f * (float) instrument->osc2_detune / 255.0f);
  if(instrument->osc2_xenv) t *= e * e;
  state.c2 += t;
  r = getoscoutput(instrument->osc2_waveform,state.c2);
  rsample += r * (float) instrument->osc2_vol / 255.0f
  // Noise oscillator
      + osc_sin(randfloat4k()) * (float) instrument->noise_fader / 255.0f * e;
  rsample *= e;
  // State variable filter
  float f = instrument->fx_freq;
  if(instrument->lfo_fx_freq) f *= lfor;
  f = 1.5f * sin4k(f * 3.141592f / 44100.0f);
  state.low += f * state.band;
  float high = state.q * (rsample - state.band) - state.low;
  state.band += f * high;
  switch(instrument->fx_filter) {
    case 1: // Hipass
        rsample = high;
        break;
    case 2: // Lopass
        rsample = state.low;
        break;
    case 3: // Bandpass
        rsample = state.band;
        break;
    case 4: // Notch
        rsample = state.low + high;
  }
  t = osc_sin(getFrequency(1.0f,2.0f,instrument->fx_pan_freq,8) * (float) b / (float) rowlen) * (float) instrument->fx_pan_amt / 512.0f + .5f;
  rsample *= (float) (instrument->env_master * 156);
  pan = t;
  return rsample;
}

// Init/play audio
bool sonantRender(song& songdata,int row,int pattern,int rowlen,int endpattern,short* output_buffer,int& ob_len) {
    // Init
    // Parse note data
    unsigned int inst_index,p,r;
    inst_index = 8;
    
    for (int i=0;i<ob_len*2;++i) {
      output_buffer[i] = 0;
    }

    do {
      instrument *instrument = &songdata.i[--inst_index];
      auto& state = inst_state[inst_index];
      unsigned int currentpos = rowlen*row;
      int p = pattern;
      int r = row;
      int cp = instrument->p[p];
      if (cp) {
        state.note = instrument->c[cp - 1].n[r];
        if (state.note) {
          if (state.new_note == true) {
            state.attack = instrument->env_attack;
            state.sustain = instrument->env_sustain;
            state.release = instrument->env_release;
            state.c1 = state.c2 = 0;
            // State variable init
            state.q = (float) instrument->fx_resonance / 255.0f;
            state.low = state.band = 0.0f;
            state.i = state.attack + state.sustain + state.release;
            state.new_note = false;
          }
          ob_len = state.i > ob_len ? ob_len : state.i;
          int ob_i = 0;
          int count = ob_len;
          do {
            float pan;
            auto rsample = generateSample(state,instrument,currentpos,rowlen,pan);
            output_buffer[ob_i++] += short(rsample * (1.0f - pan));
            output_buffer[ob_i++] += short(rsample * pan);
            --inst_state[inst_index].i;
          } while(--count);
          if (inst_state[inst_index].i == 0) {
            inst_state[inst_index].new_note = true;
          }
        }
      }
      //currentpos += rowlen;
     
    } while(inst_index);

    return false;
}



// Init/play audio
short* sonant_init(song& songdata,int rowlen,int endpattern,int* progress_var) {
  
    float* lbuffer = new float[WAVE_SIZE*2];
    float* rbuffer = new float[WAVE_SIZE*2];
    short* wave_buffer = new short[WAVE_SIZE * WAVE_CHAN];
    // Init
    // Parse note data
    unsigned int i,b,p,r;
    unsigned char n;
    i = 8;
    *progress_var = 0;
    do {
        instrument *instrument = &songdata.i[--i];
        b = WAVE_SIZE;
        do lbuffer[b] = rbuffer[b] = 0; while(--b);
        unsigned int currentpos = 0;
        for(p = 0;p < endpattern - 1;++p) { // Patterns
            for(r = 0;r < 32;++r) { // Rows
                int cp = instrument->p[p];
                if(cp) {
                    n = instrument->c[cp - 1].n[r];
#ifdef _4K_SONANT_FASTFORWARD_
                    if(p >= _4K_SONANT_FASTFORWARD_)
#endif
                    if(n) {
                        unsigned int attack = instrument->env_attack;
                        unsigned int sustain = instrument->env_sustain;
                        unsigned int release = instrument->env_release;
                        double c1,c2;
                        c1 = c2 = 0;
                        // State variable init
                        float q = (float) instrument->fx_resonance / 255.0f;
                        float low,band;
                        low = band = 0.0f;
                        unsigned int i = attack + sustain + release;
                        do {
                            unsigned int b = i + currentpos;
                            // LFO
                            float t = getFrequency(1.0f,2.0f,instrument->lfo_freq,8) * (float) b / (float) rowlen;
                            float lfor = getoscoutput(instrument->lfo_waveform,t) * (float) instrument->lfo_amt / 512.0f + .5f;
                            // Envelope
                            float e = 1.0f;
                            if(i < attack) {
                                e = (float) i / (float) attack;
                            } else if(i >= attack + sustain) {
                                e -= (float) (i - attack - sustain) / (float) release;
                            }
                            // Oscillator 1
                            t = note_freq[n + (instrument->osc1_oct - 8) * 12 + instrument->osc1_det] * (1.0f + .2f * (float) instrument->osc1_detune / 255.0f);
                            if(instrument->lfo_osc1_freq) t += lfor;
                            if(instrument->osc1_xenv) t *= e * e;
                            c1 += t;
                            float r = getoscoutput(instrument->osc1_waveform,c1);
                            float rsample = r * (float) instrument->osc1_vol / 255.0f;
                            // Oscillator 2
                            t = note_freq[n + (instrument->osc2_oct - 8) * 12 + instrument->osc2_det] * (1.0f + .2f * (float) instrument->osc2_detune / 255.0f);
                            if(instrument->osc2_xenv) t *= e * e;
                            c2 += t;
                            r = getoscoutput(instrument->osc2_waveform,c2);
                            rsample += r * (float) instrument->osc2_vol / 255.0f
                            // Noise oscillator
                                + osc_sin(randfloat4k()) * (float) instrument->noise_fader / 255.0f * e;
                            rsample *= e;
                            // State variable filter
                            float f = instrument->fx_freq;
                            if(instrument->lfo_fx_freq) f *= lfor;
                            f = 1.5f * sin4k(f * 3.141592f / 44100.0f);
                            low += f * band;
                            float high = q * (rsample - band) - low;
                            band += f * high;
                            switch(instrument->fx_filter) {
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
                            }
                            t = osc_sin(getFrequency(1.0f,2.0f,instrument->fx_pan_freq,8) * (float) b / (float) rowlen) * (float) instrument->fx_pan_amt / 512.0f + .5f;
                            rsample *= (float) (instrument->env_master * 156);
                            lbuffer[b] += rsample * (1.0f - t);
                            rbuffer[b] += rsample * t;
                        } while(--i);
                    }
                }
#ifdef _4K_SONANT_FASTFORWARD_
                if(p >= _4K_SONANT_FASTFORWARD_)
#endif
                currentpos += rowlen;
            }
        }
        *progress_var += 6;
        // Delay
        p = instrument->fx_delay_time * rowlen / 2;
        for(b = 0;b < WAVE_SIZE - p;++b) {
            float da = (float) instrument->fx_delay_amt / 255.0f;
            lbuffer[b + p] += rbuffer[b] * da;
            rbuffer[b + p] += lbuffer[b] * da;
        }
        // Write to buffer
        b = WAVE_SIZE * WAVE_CHAN;
        do {
            wave_buffer[b] = clip(wave_buffer[b] + (int) lbuffer[b / 2]);
            wave_buffer[--b] = clip(wave_buffer[b] + (int) rbuffer[b / 2]);
        } while(--b);
        *progress_var += 6;
    } while(i);
    // Play
    //waveOutOpen(&wave_handle,WAVE_MAPPER,&wave_format,0,0,0);
    //waveOutPrepareHeader(wave_handle,&wave_header,sizeof(WAVEHDR));
    //waveOutWrite(wave_handle,&wave_header,sizeof(WAVEHDR));
    delete [] lbuffer;
    *progress_var += 2;
    delete [] rbuffer;
    *progress_var += 2;
    return wave_buffer;
}

static inline int sonant_getrowpos(int row,int rowlen) {
    return row * rowlen * 1000 / WAVE_SPS;
}

static inline int sonant_getpatternpos(int pattern,int rowlen) {
    return sonant_getrowpos(pattern,rowlen) * 32;
}
