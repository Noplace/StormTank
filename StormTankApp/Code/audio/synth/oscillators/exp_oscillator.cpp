#include "../synth.h"


namespace audio {
namespace synth {
namespace oscillators {

ExpOscillator::ExpOscillator() : Oscillator() {
  table = new double[kIndexRange+1];
	int i;
  int div2 = kIndexRange >> 1;
	double e = exp(1.0);
	for(i=0;i<div2;i++) {
    table[i] = 2.0 * ((exp(i/double(div2)) - 1.0) / (e - 1.0)) - 1.0  ;
    table[i+div2] = 2.0 * ((exp((div2-i)/double(div2)) - 1.0) / (e - 1.0)) - 1.0  ;
  }
	table[kIndexRange] = -1.0;
}

ExpOscillator::~ExpOscillator() {
  delete [] table;
  table = nullptr;
}

}
}
}