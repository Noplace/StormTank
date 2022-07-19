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
#pragma once

#include "../filters/all.h"
#include "instrument.h"
#include "../oscillators/sine_oscillator.h"
#include "../oscillators/triangle_oscillator.h"
#include "../oscillators/sawtooth_oscillator.h"
#include "../oscillators/square_oscillator.h"
#include "../oscillators/exp_oscillator.h"

namespace audio {
namespace synth {
namespace instruments {

class WhiteNoiseData : public InstrumentData {
public:
    struct {
        uint32_t phase, inc;
    } table[Polyphony];
    WhiteNoiseData() : InstrumentData() {
        memset(table, 0, sizeof(table));
    }
};

class WhiteNoise : public InstrumentProcessor {
public:
    LowPassFilter lowpass[Polyphony];

    WhiteNoise() : InstrumentProcessor()  {

    }
    virtual ~WhiteNoise() {
        Unload();
    }
    int Load() {
        if (loaded_ == true)
            return S_FALSE;
      
        //default adsr, should be instrument specific
      //default adsr, should be instrument specific
        for (int i = 0;i < Polyphony;++i) {
            adsr[i].set_sample_rate(sample_rate_);
            adsr[i].set_attack_amp(0.5f);
            adsr[i].set_sustain_amp(0.3f);
            adsr[i].set_attack_time_ms(50.0f);
            adsr[i].set_decay_time_ms(8.0f);
            adsr[i].set_release_time_ms(20.5f);

            lowpass[i].set_sample_rate(sample_rate_);
            lowpass[i].set_cutoff_freq(400.0f);
            lowpass[i].Update();
        }
        loaded_ = true;
        return S_OK;
    }

    int Unload() {
        if (loaded_ == false)
            return S_FALSE;
        loaded_ = false;
        return S_OK;
    }

    InstrumentData* NewInstrumentData() {
        return new WhiteNoiseData();
    }

    real_t Tick(int note_index) {
        //auto result = osc->Tick(cdata->table[note_index].phase, cdata->table[note_index].inc);// + osc->Tick(phase,inc*2);
        //result *= adsr[note_index].Tick();
        /* random number in range 0 - 1 not including 1 */
        real_t random1 = 0.f;
        real_t random2 = 0.f;
        real_t random3 = 0.f;
        /* the white noise */
        real_t noise = 0.f;
        const static int q = 12;
        const static float c1 = (1 << q) - 1;
        const static float c2 = ((int)(c1 / 3)) + 1;
        const static float c3 = 1.f / c1;
        random1 = ((float)rand() / (float)(RAND_MAX + 1));
        random2 = ((float)rand() / (float)(RAND_MAX + 1));
        random3 = ((float)rand() / (float)(RAND_MAX + 1));
        noise = (2.f * ((random1 * c2) + (random2 * c2) + (random3 * c2)) - 3.f * (c2 - 1.f)) * c3;
        noise=0.5f*lowpass[note_index].Tick(noise);
        return noise;
    }

    int SetFrequency(real_t freq, int note_index) {
        cdata->note_data_array[note_index].freq = cdata->note_data_array[note_index].base_freq = freq;
        //cdata->table[note_index].inc = osc->get_increment(cdata->note_data_array[note_index].freq);
        return S_OK;
    }

    int NoteOn(int note_index) {
        adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
        return S_OK;
    }

    int NoteOff(int note_index) {
        //cdata->table[note_index].inc = osc->get_increment(cdata->note_data_array[note_index].freq);
        adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
        return S_OK;
    }

    void set_instrument_data(InstrumentData* idata) {
        cdata = (WhiteNoiseData*)idata;
    }
protected:
    WhiteNoiseData* cdata;
};

}
}
}

