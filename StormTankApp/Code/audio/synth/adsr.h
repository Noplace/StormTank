namespace audio {
namespace synth {

class ADSR : public Component {
 public:
  ADSR() : Component(),current_time_ms_(0) {
    stage = 0;
  }
  ~ADSR() {

  }

  void SetParameters(double attack_amp, double sustain_amp, double attack_time_ms, double decay_time_ms, double release_time_ms) {
    attack_amp_  = attack_amp;
    sustain_amp_ = sustain_amp;
    attack_time_ms_   =  attack_time_ms;   
    decay_time_ms_    =  decay_time_ms;   
    release_time_ms_  =  release_time_ms; 
  }

  double Tick() {

    double t0 = attack_time_ms_;
    double t1 = t0+decay_time_ms_;
    double t2 = t1+sustain_time_ms_;//90 seconds sustain!
    double t3 = t2+release_time_ms_;

    auto stage1_func = [=](double t){
      return attack_amp_*(t*attack_velocity_)/attack_time_ms_;
    };
    auto stage2_func = [=](double t) {
      return attack_amp_-(attack_amp_+sustain_amp_)*(t*attack_velocity_)/decay_time_ms_;
    };
    auto stage3_func = [=]() {
      return sustain_amp_;
    };
    auto stage4_func = [=](double t) {
      return sustain_amp_*((release_time_ms_-t)*release_velocity_)/release_time_ms_;
    };
    double result = 0;
    if (current_time_ms_ < t0)
      result = stage1_func(current_time_ms_);
    else if (current_time_ms_ < t1)
      result = stage2_func(current_time_ms_-t0);
    else if (current_time_ms_ < t2)
      return stage3_func();
    else
      return stage4_func(current_time_ms_-t2);

    current_time_ms_ += sample_time_ms_;
  }

  void NoteOn(double velocity) {
    current_time_ms_ = 0;
    attack_velocity_ = velocity;
    sustain_time_ms_ = 90000.0;
  }
  void NoteOff(double velocity) {
    sustain_time_ms_ = current_time_ms_;
    release_velocity_ = velocity;
  }
 protected:
  int stage;
  double attack_amp_,attack_velocity_;
  double sustain_amp_,release_velocity_;
  double current_time_ms_;
  double attack_time_ms_;
  double decay_time_ms_; 
  double sustain_time_ms_;
  double release_time_ms_;

  __forceinline double lerp(double x0,double x1,double t) {
    return x0 + t*(x1-x0);
  }
  __forceinline double u(double t) {
    return t < 0 ? 0 : t;
  }
};

}
}