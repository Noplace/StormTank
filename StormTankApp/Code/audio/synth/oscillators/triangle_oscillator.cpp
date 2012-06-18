#include "../synth.h"


namespace audio {
namespace synth {
namespace oscillators {

TriangleOscillator::TriangleOscillator() : Oscillator() {
  table = new double[kIndexRange+1];
  int div4 = kIndexRange>>2;
  int i;
	for(i=0;i<div4;i++) {
	  table[i]                =   i / double(div4);
	  table[i+div4]           =   (div4-i) / double(div4);
	  table[i+div4+div4]      =   - i / double(div4);
	  table[i+div4+div4+div4] = - (div4-i) / double(div4);
	}
	table[kIndexRange] = 0.0;
}

TriangleOscillator::~TriangleOscillator() {
  delete [] table;
  table = nullptr;
}



}
}
}