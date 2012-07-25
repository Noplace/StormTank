#include "sine_oscillator.h"


namespace audio {
namespace synth {
namespace oscillators {


//double* SineOscillator::table = nullptr;
//int SineOscillator::ref_count = 0;

SineOscillator::SineOscillator() : Oscillator() {
  //if (ref_count == 0) {
    table = new real_t[kIndexRange+1];
	  for(auto i=0;i<=kIndexRange;++i)
	    table[i] = sin(2.0f*XM_PI*(i/real_t(kIndexRange)));
  //}
  //++ref_count;
}

SineOscillator::~SineOscillator() {
  //--ref_count;
  //if (ref_count == 0) {
    delete [] table;
    table = nullptr;
  //}
}



}
}
}