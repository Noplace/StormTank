namespace audio {
namespace synth {

class Channel : public Component {
 public:
  Channel() : Component(),instrument_(nullptr),amplification_(1.0),silence_(false) {
    set_panning(0.5);
  }
  ~Channel() {

  }

  void Tick(double& output_left_sample,double& output_right_sample) {
    double sample = instrument_->Tick();
    double left_sample = pan_l*sample;
    double right_sample = pan_r*sample;

    output_left_sample = (!silence_)*(amplification_*left_sample);
    output_right_sample = (!silence_)*(amplification_*right_sample);
  }
  Instrument* instrument() { return instrument_; }
  void set_sample_rate(uint32_t sample_rate) { 
    sample_rate_ = sample_rate; 
    if (instrument_ != nullptr)
      instrument_->set_sample_rate(sample_rate_);
  }
  void set_instrument(Instrument* instrument) {
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
  Instrument* instrument_;
  double amplification_,panning_,pan_l,pan_r;
  bool silence_;
};

}
}