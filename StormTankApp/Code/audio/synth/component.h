namespace audio {
namespace synth {

class Component {
 public:
  Component() : sample_rate_(0) {}
  virtual ~Component() {}
  virtual void set_sample_rate(uint32_t sample_rate) { sample_rate_ = sample_rate; }
 protected:
  uint32_t sample_rate_;
};


}
}
