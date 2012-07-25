#include "exp_oscillator.h"

namespace audio {
namespace synth {
namespace oscillators {

ExpOscillator::ExpOscillator() : Oscillator() {
  table = new real_t[kIndexRange+1];
	int i;
  int div2 = kIndexRange >> 1;
	real_t e = exp(1.0f);
	for(i=0;i<div2;i++) {
    table[i] = 2.0f * ((exp(i/real_t(div2)) - 1.0f) / (e - 1.0f)) - 1.0f  ;
    table[i+div2] = 2.0f * ((exp((div2-i)/real_t(div2)) - 1.0f) / (e - 1.0f)) - 1.0f  ;
  }
	table[kIndexRange] = -1.0f;
}

ExpOscillator::~ExpOscillator() {
  delete [] table;
  table = nullptr;
}

}
}
}