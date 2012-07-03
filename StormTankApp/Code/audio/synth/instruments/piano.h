#include "../PADsynth.h"

namespace audio {
namespace synth {
namespace instruments {

class PianoData : public InstrumentData {
 public:
  struct {
     uint32_t phase1,inc1;
     uint32_t phase2,inc2;
     uint32_t phase3,inc3;
     uint32_t phase4,inc4;
  } table[Polyphony];
  PianoData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class Piano : public InstrumentProcessor {
 public:
  Piano() : InstrumentProcessor(),padsynth(256,44100,64) {
    randseed = 1;
    padsynth.setharmonic(0,0.0f);
    for (int i=1;i<64;i++) {
	    padsynth.setharmonic(i,1.0f/i);
	    if ((i%2)==0) 
        padsynth.setharmonic(i,padsynth.getharmonic(i)*2.0f);
    };
    buffer = new float*[128];
    for (int i=0;i<128;++i) {
      real_t A4_freq = 440;
      real_t two = pow(2.0f,1/12.0f);

        auto freq = A4_freq * pow(two,i-69);//45 = index of A4
      

      buffer[i] = new float[256];
      padsynth.synth(freq,40,1,buffer[i]);
    }
  }
  virtual ~Piano() {
    for (int i=0;i<128;++i) {
      delete [] buffer[i];
    }
    delete [] buffer;
  }

  InstrumentData* NewInstrumentData() {
    return new PianoData();
  }

  real_t Tick(InstrumentData* data, int note_index);
  void Update(InstrumentData* data, int note_index);

  void set_sample_rate(uint32_t sample_rate) { 
    InstrumentProcessor::set_sample_rate(sample_rate); 
    osc1.set_sample_rate(sample_rate);
    osc2.set_sample_rate(sample_rate);
    osc3.set_sample_rate(sample_rate);
    osc4.set_sample_rate(sample_rate);
  }
 protected:
  int randseed;
  oscillators::SineOscillator osc1;
  oscillators::SineOscillator osc2;
  oscillators::SawtoothOscillator osc3;
  oscillators::TriangleOscillator osc4;
  float** buffer;
  PADsynth padsynth;
};

}
}
}
