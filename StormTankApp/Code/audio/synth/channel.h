namespace audio {
namespace synth {

class Channel : public Component {
 public:
  Channel(int index) : Component(),index(index),instrument_data_(nullptr),instrument_(nullptr),amplification_(1.0),silence_(false) {
    set_panning(0.5);
    current_note=0;
  }
  ~Channel() {
    SafeDelete(&instrument_data_);
  }

  void Tick(real_t& output_left_sample,real_t& output_right_sample) {
    if (silence_ == true) {
      output_left_sample = output_right_sample = 0;
    } else {
      real_t sample = 0;
      for (int i=0;i<Polyphony;++i) {
        sample += adsr[i].Tick()*instrument_->Tick(instrument_data_,i);
      }
      output_left_sample += (amplification_*pan_l*sample);
      output_right_sample += (amplification_*pan_r*sample);
    }
  }
  void AddNote(int note,real_t freq,real_t velocity) {
    auto ptr = &instrument_data_->note_data_array[current_note];
    ptr->note = note;
    ptr->freq = ptr->base_freq = freq;
    ptr->velocity = velocity;
    ptr->on = true;
    instrument_->Update(instrument_data_,current_note);
    adsr[current_note].NoteOn(ptr->velocity);
    current_note = (current_note + 1) % Polyphony;
  }
  void RemoveNote(int note,real_t velocity) {
    for (int i=0;i<Polyphony;++i) {
      auto* ptr = &instrument_data_->note_data_array[i];
      if (ptr->note == note) {
        ptr->on = false;
        ptr->note = note;
        ptr->freq = ptr->base_freq = 0;
        ptr->velocity = velocity;
        adsr[i].NoteOff(velocity);
        instrument_->Update(instrument_data_,i);
        current_note = i;
      }
    }
  }

  void SetPitchBend(real_t amount) {
    real_t e = exp(amount * log(2.0f) / 12);
    for (int i=0;i<Polyphony;++i) {
      instrument_data_->note_data_array[i].freq *= e;
      instrument_->Update(instrument_data_,i);
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
    //default adsr
    for (int i=0;i<Polyphony;++i) {
      instrument_->Update(instrument_data_,i);
      adsr[i].set_sample_rate(sample_rate_);
      adsr[i].set_attack_amp(1.0f);
      adsr[i].set_sustain_amp(0.3f);
      adsr[i].set_decay_time_ms(0.01f);//or set rate
    }
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
  void set_silence(bool silence) {    silence_ = silence;  }
 private:
  instruments::InstrumentData* instrument_data_;
  instruments::InstrumentProcessor* instrument_;
  real_t amplification_,panning_,pan_l,pan_r,pitch_bend_range;
  int index,current_note;
  bool silence_;
  ADSR adsr[Polyphony];
};

}
}