#include "../base.h"
#include "oscillator.h"
#include "triangle_oscillator.h"

namespace audio {
namespace synth {
namespace oscillators {

TriangleOscillator::TriangleOscillator() : Oscillator() {
  table = new real_t[kIndexRange+1];
  int div4 = kIndexRange>>2;
  int i;
	for(i=0;i<div4;i++) {
	  table[i]                =   i / real_t(div4);
	  table[i+div4]           =   (div4-i) / real_t(div4);
	  table[i+div4+div4]      =   - i / real_t(div4);
	  table[i+div4+div4+div4] = - (div4-i) / real_t(div4);
	}
	table[kIndexRange] = 0.0f;
}

TriangleOscillator::~TriangleOscillator() {
  delete [] table;
  table = nullptr;
}



}
}
}