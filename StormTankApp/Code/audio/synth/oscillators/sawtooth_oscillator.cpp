#include "../base.h"
#include "oscillator.h"
#include "sawtooth_oscillator.h"


namespace audio {
namespace synth {
namespace oscillators {

SawtoothOscillator::SawtoothOscillator() : Oscillator() {
  table = new double[kIndexRange+1];
	for(int i=0;i<kIndexRange;++i) {
    table[i] = 2.0*(i/double(kIndexRange-1)) - 1.0;
  }
  table[kIndexRange] = -1.0;
}

SawtoothOscillator::~SawtoothOscillator() {
  delete [] table;
  table = nullptr;
}

}
}
}