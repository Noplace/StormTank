namespace audio {
namespace synth {
namespace instruments {



class BlitWaveData : public InstrumentData {
 public:
  struct {
     uint32_t phase1,inc1;
     uint32_t phase2,inc2;
     uint32_t phase3,inc3;
     uint32_t phase4,inc4;
  } table[Polyphony];
  BlitWaveData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class BlitWave : public InstrumentProcessor {
 public:
  BlitWave();
  virtual ~BlitWave();

  InstrumentData* NewInstrumentData() {
    return new BlitWaveData();
  }

  double Tick(InstrumentData* data, int note_index);
  void Update(InstrumentData* data, int note_index);

  void set_sample_rate(uint32_t sample_rate) { 
    InstrumentProcessor::set_sample_rate(sample_rate); 
    GenerateWavetables();
  }
 protected:
   double* wavetables[128];
   void GenerateWavetables();
};

}
}
}