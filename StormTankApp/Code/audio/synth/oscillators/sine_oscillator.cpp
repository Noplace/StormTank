#include "../synth.h"


namespace audio {
namespace synth {
namespace oscillators {

static const double k1Div24lowerBits = 1.0/(kPrecisionRange);
double* SineOscillator::table = nullptr;
int SineOscillator::ref_count = 0;

SineOscillator::SineOscillator() : phase(0),inc(0) {
  if (ref_count == 0) {
    table = new double[kIndexRange+1];
	  for(auto i=0;i<=kIndexRange;i++)
	    table[i] = sin(2.0*XM_PI*(i/double(kIndexRange)));
  }
  ++ref_count;
}

SineOscillator::~SineOscillator() {
  --ref_count;
  if (ref_count == 0) {
    delete [] table;
    table = nullptr;
  }
}


double SineOscillator::Tick() {
  // the 8 MSB are the index in the table in the range 0-255 
  int i = phase >> kPrecision; 

  // and the kPrecision LSB are the fractionnal part
  double frac = (phase & kPrecisionMask) * k1Div24lowerBits;

  // increment the phase for the next tick
  phase += inc; // the phase overflow itself

  return (table[i]*(1.0-frac) + table[i+1]*frac); // linear interpolation
}

}
}
}