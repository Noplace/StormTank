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
  Piano() : InstrumentProcessor() {
    randseed = 1;
  }
  virtual ~Piano() {
    
  }

  InstrumentData* NewInstrumentData() {
    return new PianoData();
  }

  double Tick(InstrumentData* data, int note_index);
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
};

}
}
}
