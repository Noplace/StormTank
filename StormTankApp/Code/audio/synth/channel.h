#ifndef AUDIO_SYNTH_CHANNEL_H
#define AUDIO_SYNTH_CHANNEL_H

#include "instruments/instrument.h"

namespace audio {
namespace synth {

class Channel : public Component {
 public:
  uint8_t param_fine,param_coarse,des_fine,des_coarse;
  Channel(int index) : Component(),index(index),instrument_data_(nullptr),instrument_(nullptr),buffer_(nullptr),amplification_(1.0),silence_(false),pitch_bend_range_(2.0f),param_fine(0),param_coarse(0),des_fine(0),des_coarse(0) {
    set_panning(0.5);
    current_note=0;
  }
  ~Channel() {
    SafeDeleteArray(&buffer_);
    SafeDelete(&instrument_data_);
  }

  void Render(uint32_t samples_to_generate) {
    auto bufptr = buffer_;
    if (silence_ == false) {
      for (uint32_t i=0;i<samples_to_generate;++i) {
        real_t sample = 0;
        for (int n=0;n<Polyphony;++n) {
          sample += instrument_->Tick(instrument_data_,n);
        }
        *bufptr++ = (amplification_*pan_l*sample);
        *bufptr++ = (amplification_*pan_r*sample);
      }
    }
  }

  void AddNote(int note,real_t freq,real_t velocity) {
    auto ptr = &instrument_data_->note_data_array[current_note];
    ptr->note = note;
    ptr->freq = ptr->base_freq = freq;
    ptr->velocity = velocity;
    ptr->on = true;
    instrument_->NoteOn(instrument_data_,current_note);
    current_note = (current_note + 1) % Polyphony;
  }

  void RemoveNote(int note,real_t velocity) {
    for (int i=0;i<Polyphony;++i) {
      auto* ptr = &instrument_data_->note_data_array[i];
      if (ptr->note == note && ptr->on == true) {
        ptr->on = false;
        ptr->note = note;
        ptr->freq = ptr->base_freq = 0;
        ptr->velocity = velocity;
        instrument_->NoteOff(instrument_data_,i);
        current_note = i;
      }
    }
  }

  void SetPitchBend(real_t amount) {
    real_t e = exp(amount * _LN_2_DIV_12);
    for (int i=0;i<Polyphony;++i) {
      instrument_data_->note_data_array[i].freq *= e;
      instrument_->NoteOn(instrument_data_,i);//todo: new function to change freq
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
  void set_buffer_length(real_t buffer_length_ms) {
    buffer_size_ = uint32_t(sample_rate_*2*buffer_length_ms*0.001);
    SafeDeleteArray(&buffer_);
    buffer_ = new real_t[buffer_size_];
  }
  void set_amplification(real_t amplification) {
    amplification_ = amplification;
  }
  void set_panning(real_t panning) {
    panning_ = panning;
    pan_l = sqrt(1.0f-panning_);
    pan_r = sqrt(panning_);
  }
  bool silence() { return silence_;  }
  void set_silence(bool silence) {    
    silence_ = silence;  
    if (silence_ == true) {
      memset(buffer_,0,sizeof(real_t)*buffer_size_);
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
 private:
  instruments::InstrumentData* instrument_data_;
  instruments::InstrumentProcessor* instrument_;
  real_t* buffer_;
  size_t buffer_size_;
  real_t amplification_,panning_,pan_l,pan_r,pitch_bend_range_;
  int index,current_note;
  bool silence_;
};

}
}

#endif