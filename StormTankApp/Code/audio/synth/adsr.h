namespace audio {
namespace synth {

class ADSR : public Component {
 public:
  ADSR() : Component() , stage_(0),target_(0),value_(0),attack_amp_(1.0f),sustain_amp_(0.5f),attack_rate_(0),decay_rate_(0),release_rate_(0) {
    stages_[0] = &ADSR::stage_0;
    stages_[1] = &ADSR::stage_1;
    stages_[2] = &ADSR::stage_2;
    stages_[3] = &ADSR::stage_3;
    stages_[4] = &ADSR::stage_4;
  }

  ~ADSR() {

  }

  real_t Tick() {
    (this->*stages_[stage_])();
    return value_;
  }
  void NoteOn(real_t velocity) {
    stage_ = 1;
    target_ = attack_amp_;
    attack_rate_ = velocity;
  }
  void NoteOff(real_t velocity) {
    stage_ = 4;
    release_rate_ = velocity;
  }
  void set_attack_amp(real_t attack_amp) { attack_amp_  = attack_amp; }
  void set_sustain_amp(real_t sustain_amp) { sustain_amp_  = sustain_amp; }
  void set_decay_rate(real_t decay_rate_) {
    decay_rate_ = decay_rate_;
  }
  void set_decay_time_ms(real_t decay_time_ms) {
    decay_rate_ = (1.0f - sustain_amp_) / ( decay_time_ms * 0.001f * this->sample_rate_ );
  }
 protected:
  typedef void (ADSR::*StageFunc)();
  int stage_;
  real_t value_,target_;
  real_t attack_amp_,sustain_amp_;
  real_t attack_rate_,decay_rate_,release_rate_;


  void stage_0() { value_ = 0; };
  void stage_1() {
    value_ += attack_rate_;
    if ( value_ >= target_ ) {
      value_ = target_;
      target_ = sustain_amp_;
	    stage_ = 2;
    }
  }
  void stage_2() {
    if ( value_ > sustain_amp_ ) {
      value_ -= decay_rate_;
      if ( value_ <= sustain_amp_ ) {
        value_ = sustain_amp_;
        stage_ = 3;
      }
    } else {
      value_ += decay_rate_; // attack target < sustain level
      if ( value_ >= sustain_amp_ ) {
        value_ = sustain_amp_;
        stage_ = 3;
      }
    }
  }
  void stage_3() {};
  void stage_4() {
    value_ -= release_rate_;
    if ( value_ <= 0.0 ) {
      value_ = 0.0;
      stage_ = 0;
    }
  }

  StageFunc stages_[5];

  __forceinline real_t lerp(real_t x0,real_t x1,real_t t) {
    return x0 + t*(x1-x0);
  }
  __forceinline real_t u(real_t t) {
    return t < 0 ? 0 : t;
  }
};

}
}