#include "sawtooth_oscillator.h"


namespace audio {
namespace synth {
namespace oscillators {

SawtoothOscillator::SawtoothOscillator() : Oscillator() {
  table = new real_t[kIndexRange+1];
	for(int i=0;i<kIndexRange;++i) {
    table[i] = 2.0f*(i/real_t(kIndexRange-1)) - 1.0f;
  }
  table[kIndexRange] = -1.0f;
}

SawtoothOscillator::~SawtoothOscillator() {
  delete [] table;
  table = nullptr;
}

}
}
}