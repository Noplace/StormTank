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

class Synth : public Component {
 public:
  Synth() : Component(), frequency_(0.0) {}
  virtual ~Synth() {}
  virtual double Tick() = 0;
  virtual void set_frequency(double frequency) {
    frequency_ = frequency;
  }
 protected:
  double frequency_;
};

}
}
