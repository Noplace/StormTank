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

  void Tick(double& output_left_sample,double& output_right_sample) {
    if (silence_ == true) {
      output_left_sample = output_right_sample = 0;
    } else {
      double sample = 0;
      for (int i=0;i<Polyphony;++i) {
        if (instrument_data_->note_data_array[i].on == true) //this contradicts adsr and release
          sample += instrument_data_->note_data_array[i].velocity*instrument_->Tick(instrument_data_,i);
        /*adsr[i].Tick()**/
        //note_table[i].velocity = max(0,note_table[i].velocity-0.00001);
      }
      double left_sample = pan_l*sample;
      double right_sample = pan_r*sample;

      output_left_sample = (amplification_*left_sample);
      output_right_sample = (amplification_*right_sample);
    }
  }
  void AddNote(int note,double freq,double velocity) {
    
    auto ptr = &instrument_data_->note_data_array[current_note];
    ptr->note = note;
    ptr->freq = freq;
    ptr->velocity = velocity;
    ptr->on = true;
    instrument_->Update(instrument_data_,current_note);
    adsr[current_note].NoteOn(ptr->velocity);
    current_note = (current_note + 1) % Polyphony;

   /* auto* ptr = &instrument_data_->note_data_array[0];


    int i=Polyphony;
    while (ptr->on == true && ptr->note != note && --i) {
      ++ptr;
    }
    ptr->note = note;
    ptr->freq = freq;
    ptr->velocity = velocity;
    ptr->on = true;
    instrument_->Update(instrument_data_,Polyphony-i);
    adsr[Polyphony-i].NoteOn(ptr->velocity);*/
  }
  void RemoveNote(int note) {
    for (int i=0;i<Polyphony;++i) {
      auto* ptr = &instrument_data_->note_data_array[i];
      if (ptr->note == note) {
        ptr->on = false;
        ptr->note = note;
        ptr->freq = 0;
        ptr->velocity = 0;
        adsr[i].NoteOff(1);
        instrument_->Update(instrument_data_,i);
        current_note = i;
      }
    }
    /*auto* ptr = &instrument_data_->note_data_array[0];
    int i=Polyphony;
    while (ptr->note != note && --i) {
      ++ptr;
    }
    if (ptr->note == note) {
      ptr->on = false;
      ptr->note = note;
      ptr->freq = 0;
      ptr->velocity = 0;
      adsr[Polyphony-i].NoteOff(1);
      instrument_->Update(instrument_data_,Polyphony-i);
    }*/
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

    for (int i=0;i<Polyphony;++i) {
      instrument_->Update(instrument_data_,i);
      adsr[i].set_sample_rate(sample_rate_);
      adsr[i].SetParameters(0.5,0.3,300,100,1000);
    }
  }
  void set_amplification(double amplification) {
    amplification_ = amplification;
  }
  void set_panning(double panning) {
    panning_ = panning;
    pan_l = sqrt(1-panning_);
    pan_r = sqrt(panning_);
  }
  bool silence() { return silence_;  }
  void set_silence(bool silence) {    silence_ = silence;  }
 private:
  instruments::InstrumentData* instrument_data_;
  instruments::InstrumentProcessor* instrument_;
  double amplification_,panning_,pan_l,pan_r;
  int index,current_note;
  bool silence_;
  ADSR adsr[Polyphony];
};

}
}