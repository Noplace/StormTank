#include "square_oscillator.h"

namespace audio {
namespace synth {
namespace oscillators {

SquareOscillator::SquareOscillator() : Oscillator() {
  table = new real_t[kIndexRange+1];
  int div2 = kIndexRange >> 1;
  int i;
  for(i=0;i<div2;++i) {
	  table[i]     =  1.0f;
	  table[i+div2] = -1.0f;
  }
  table[kIndexRange] = 1.0f;
}

SquareOscillator::~SquareOscillator() {
  delete [] table;
  table = nullptr;
}



}
}
}