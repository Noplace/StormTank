#include "../synth.h"


namespace audio {
namespace synth {
namespace oscillators {


//double* SineOscillator::table = nullptr;
//int SineOscillator::ref_count = 0;

SineOscillator::SineOscillator() : Oscillator() {
  //if (ref_count == 0) {
    table = new double[kIndexRange+1];
	  for(auto i=0;i<=kIndexRange;i++)
	    table[i] = sin(2.0*XM_PI*(i/double(kIndexRange)));
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