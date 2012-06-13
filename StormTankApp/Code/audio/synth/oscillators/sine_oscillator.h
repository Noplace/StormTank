#define kIndex 8
#define kIndexRange (1<<kIndex)
#define kPrecision 24
#define kPrecisionMask 0xFFFFFF
#define kPrecisionRange 16777216.0 //(double)(1<<kPrecision);

namespace audio {
namespace synth {
namespace oscillators {

class SineOscillator : public Synth {
 public:
  SineOscillator();
  ~SineOscillator();
  double Tick();
  void set_frequency(double frequency) {
    inc =  (uint32_t)((256.0 * frequency / sample_rate_) * kPrecisionRange);
  }
 private:
  static double* table;
  static int ref_count;
  uint32_t inc;
  uint32_t phase;
};

}
}
}