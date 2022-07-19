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
#ifndef AUDIO_SYNTH_CHANNEL_H
#define AUDIO_SYNTH_CHANNEL_H

#include "instruments/instrument.h"

namespace audio {
namespace synth {

class MidiSynth;

class Channel : public Component {
 public:
  Channel(int index) : Component(),index(index),instrument_data_(nullptr),instrument_(nullptr),buffer_(nullptr),amplification_(1.0),silence_(false),pitch_bend_range_(2.0f),param_fine(0),param_coarse(0),des_fine(0),des_coarse(0),aux_send(0) {
    set_panning(0.5);
    current_note=0;
    memset(&effects,0,sizeof(effects));
  }
  ~Channel() {
    //SafeDeleteArray(&buffer_);
    SafeDelete(&instrument_data_);
  }
  void RenderStereo(uint32_t samples_to_generate) {
    instrument_->set_instrument_data(instrument_data_);
    auto bufptr = buffer_;
    if (silence_ == false) {
      for (uint32_t i=0;i<samples_to_generate;++i) {
        real_t sample = 0;
        for (int n=0;n<Polyphony;++n) {
          sample += instrument_->Tick(n);
        }
        *bufptr++ = (amplification_*pan_l*sample);
        *bufptr++ = (amplification_*pan_r*sample);
      }
      //effects processing
      for (int i=0;i<5;++i) {
        if (effects[i] != nullptr)
          effects[i]->ProcessStereo(buffer_,buffer_,samples_to_generate);
      }
    }
  }
  void AddNote(int note,real_t freq,real_t velocity) {
    instrument_->set_instrument_data(instrument_data_);
    auto ptr = &instrument_data_->note_data_array[current_note];
    ptr->note = note;
    ptr->velocity = velocity;
    ptr->on = true;
    instrument_->SetFrequency(freq,current_note);
    instrument_->NoteOn(current_note);
    current_note = (current_note + 1) % Polyphony;
  }
  void RemoveNote(int note,real_t velocity) {
    instrument_->set_instrument_data(instrument_data_);
    for (int i=0;i<Polyphony;++i) {
      auto* ptr = &instrument_data_->note_data_array[i];
      if (ptr->note == note && ptr->on == true) {
        ptr->on = false;
        ptr->note = note;
        ptr->velocity = velocity;
        instrument_->NoteOff(i);
        current_note = i;
      }
    }
  }
  void SetPitchBend(real_t amount) {
    instrument_->set_instrument_data(instrument_data_);
    real_t e = expf(amount * _LN_2_DIV_12);
    for (int i=0;i<Polyphony;++i) {
      auto freq = instrument_data_->note_data_array[i].freq * e;
      instrument_->SetFrequency(freq,i);
    }
  }
  instruments::InstrumentProcessor* instrument() { return instrument_; }
  void set_instrument(instruments::InstrumentProcessor* instrument) {
    instrument_ = instrument;

    if (instrument_data_ == nullptr) {
      instrument_data_ = instrument_->NewInstrumentData();
    } else {
      auto newidata = instrument_->NewInstrumentData();
      memcpy(newidata->note_data_array,instrument_data_->note_data_array,sizeof(newidata->note_data_array));
      SafeDelete(&instrument_data_);
      instrument_data_ = newidata;
      newidata = nullptr;
    }
  }
  /*void set_buffer_length(real_t buffer_length_ms) {
    buffer_size_ = uint32_t(sample_rate_*2*buffer_length_ms*0.001);
    SafeDeleteArray(&buffer_);
    buffer_ = new real_t[buffer_size_];
  }*/
  void set_amplification(real_t amplification) {
    amplification_ = amplification;
  }
  void set_panning(real_t panning) {
    panning_ = panning;
    pan_l = sqrtf(1.0f - panning_);
    pan_r = sqrtf(panning_);
  }
  bool silence() { return silence_;  }
  void set_silence(bool silence) {    
    silence_ = silence;  
    if (silence_ == true) {
     // memset(buffer_,0,sizeof(real_t)*buffer_size_);
    }
  }
  real_t pitch_bend_range() {
    return pitch_bend_range_;
  }
  void set_pitch_bend_range(real_t pitch_bend_range) {
    pitch_bend_range_ = pitch_bend_range;
  }
  real_t* buffer() {
    return buffer_;
  }
  void set_buffer(real_t* buffer) { buffer_ = buffer; }
 private:
  friend MidiSynth;
  effects::Effect* effects[kEffectCount];
  instruments::InstrumentData* instrument_data_;
  instruments::InstrumentProcessor* instrument_;
  real_t* buffer_;
  real_t amplification_,panning_,pan_l,pan_r,pitch_bend_range_,aux_send;
  //uint32_t buffer_size_;
  int index,current_note;
  uint8_t param_fine,param_coarse,des_fine,des_coarse;
  bool silence_;
};

}
}

#endif