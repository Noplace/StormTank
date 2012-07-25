#ifndef AUDIO_SYNTH_COMPONENT_H
#define AUDIO_SYNTH_COMPONENT_H

namespace audio {
namespace synth {

class Component {
 public:
  Component() : sample_rate_(0),sample_time_ms_(0) {}
  virtual ~Component() {}
  virtual void set_sample_rate(uint32_t sample_rate) { 
    sample_rate_ = sample_rate; 
    sample_time_ms_ = (1000.0f/sample_rate_);
  }
  real_t sample_time_ms_;
  uint32_t sample_rate_;
};


}
}

#endif