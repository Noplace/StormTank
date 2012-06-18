namespace audio {
namespace synth {

class Channel : public Component {
 public:
  Channel() : Component(),instrument_(nullptr),amplification_(1.0),silence_(false) {
    set_panning(0.5);
    memset(note_table,0,sizeof(note_table));
  }
  ~Channel() {

  }

  void Tick(double& output_left_sample,double& output_right_sample) {
    double sample = 0;//instrument_->Tick();
    
    for (int i=0;i<Polyphony;++i) {
      sample += note_table[i].velocity*instrument_->Tick(note_table[i].phase,note_table[i].inc);
      //note_table[i].velocity = max(0,note_table[i].velocity-0.00001);
    }
    double left_sample = pan_l*sample;
    double right_sample = pan_r*sample;

    output_left_sample = (!silence_)*(amplification_*left_sample);
    output_right_sample = (!silence_)*(amplification_*right_sample);
  }
  void AddNote(int note,double freq,double velocity) {
    auto* ptr = &note_table[0];
    int i=Polyphony;
    while (ptr->on == true && ptr->note != note && --i) {
      ++ptr;
    }
    ptr->note = note;
    ptr->freq = freq;
    ptr->velocity = velocity;
    ptr->on = true;
    ptr->inc = instrument_->get_increment(ptr->freq);
  }
  void RemoveNote(int note) {
    auto* ptr = &note_table[0];
    int i=Polyphony;
    while (ptr->note != note && --i) {
      ++ptr;
    }
    if (ptr->note == note) {
      ptr->on = false;
      ptr->note = 0;
      ptr->freq = 0;
      ptr->velocity = 0;
      ptr->inc = instrument_->get_increment(ptr->freq);
    }
  }

  instruments::Instrument* instrument() { return instrument_; }
  void set_sample_rate(uint32_t sample_rate) { 
    sample_rate_ = sample_rate; 
    if (instrument_ != nullptr)
      instrument_->set_sample_rate(sample_rate_);
  }
  void set_instrument(instruments::Instrument* instrument) {
    if (instrument_ != nullptr) {
      //instrument_->CopyTo(instrument);
    }
    instrument_ = instrument;
  }
  void set_amplification(double amplification) {
    amplification_ = amplification;
  }
  void set_panning(double panning) {
    panning_ = panning;
    pan_l = sqrt(1-panning_);
    pan_r = sqrt(panning_);
  }
  void set_silence(bool silence) {
    silence_ = silence;
  }
 private:
  instruments::Instrument* instrument_;
  double amplification_,panning_,pan_l,pan_r;
  bool silence_;
  struct {
    double freq; //Polyphony note polyphony
    double velocity;
    uint32_t phase,inc;
    int note;
    bool on;
  } note_table[Polyphony];
};

}
}