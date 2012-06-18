#define kIndex 8
#define kIndexRange (1<<kIndex)
#define kPrecision 24
#define kPrecisionMask 0xFFFFFF
#define kPrecisionRange 16777216.0 //(double)(1<<kPrecision);

namespace audio {
namespace synth {
namespace oscillators {

class Oscillator : public Component {
 public:
  Oscillator() : Component(), k1Div24lowerBits(1.0/(kPrecisionRange)) {

  }
  virtual ~Oscillator() {
  }
  virtual double Tick(uint32_t& phase,uint32_t inc) {
    // the 8 MSB are the index in the table in the range 0-255 
    int i = phase >> kPrecision; 
    // and the kPrecision LSB are the fractionnal part
    double frac = (phase & kPrecisionMask) * k1Div24lowerBits;
    // increment the phase for the next tick
    phase += inc; // the phase overflow itself
    return (table[i]*(1.0-frac) + table[i+1]*frac); // linear interpolation
  }
  virtual uint32_t get_increment(double frequency) {
    return (uint32_t)((256.0 * frequency / sample_rate_) * kPrecisionRange);
  }
 protected:
  double* table;
  const double k1Div24lowerBits;
};

}
}
}