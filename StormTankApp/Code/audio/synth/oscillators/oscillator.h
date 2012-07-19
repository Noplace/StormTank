#define kIndex 8
#define kIndexRange (1<<kIndex)
#define kPrecision 24
#define kPrecisionMask 0xFFFFFF
#define kPrecisionRange 16777216.0f //(double)(1<<kPrecision);
#define k1Div24lowerBits (1.0f/(kPrecisionRange))

namespace audio {
namespace synth {
namespace oscillators {

class Oscillator : public Component {
 public:
  Oscillator() : Component() {

  }
  virtual ~Oscillator() {
  }
  virtual real_t Tick(uint32_t& phase,uint32_t inc) {
    // the 8 MSB are the index in the table in the range 0-255 
    int i = phase >> kPrecision; 
    // and the kPrecision LSB are the fractionnal part
    real_t frac = (phase & kPrecisionMask) * k1Div24lowerBits;
    // increment the phase for the next tick
    phase += inc; // the phase overflow itself
    return (table[i]*(1.0f-frac) + table[i+1]*frac); // linear interpolation
  }
  virtual uint32_t get_increment(real_t frequency) {
    return (uint32_t)((256.0f * frequency / sample_rate_) * kPrecisionRange);
  }
 protected:
  real_t* table;
};

}
}
}