#include "../base.h"
#include "oscillator.h"
#include "square_oscillator.h"

namespace audio {
namespace synth {
namespace oscillators {

SquareOscillator::SquareOscillator() : Oscillator() {
  table = new double[kIndexRange+1];
  int div2 = kIndexRange >> 1;
  int i;
  for(i=0;i<div2;++i) {
	  table[i]     =  1.0;
	  table[i+div2] = -1.0;
  }
  table[kIndexRange] = 1.0;
}

SquareOscillator::~SquareOscillator() {
  delete [] table;
  table = nullptr;
}



}
}
}