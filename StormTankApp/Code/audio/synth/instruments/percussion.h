namespace audio {
namespace synth {
namespace instruments {

class PercussionData : public InstrumentData {
 public:
  struct {
    bool highhat_closed;
    real_t highhat_amp,bassdrum_amp;
    uint32_t bassdrum_phase,bassdrum_inc;
    uint32_t hihat_phase[4],hihat_inc[4];
  } table;
  PercussionData() : InstrumentData() {
    memset(&table,0,sizeof(table));
  }
};


class Percussion : public InstrumentProcessor {
 public:
  Percussion() : InstrumentProcessor() {
   randseed = 1;
  }
  virtual ~Percussion() {
    
  }
  InstrumentData* NewInstrumentData() {
    return new PercussionData();
  }
  real_t Tick(InstrumentData* data, int note_index);
  void Update(InstrumentData* data, int note_index);
  void set_sample_rate(uint32_t sample_rate) { 
    InstrumentProcessor::set_sample_rate(sample_rate); 
    inv_sr = 1 / real_t(sample_rate_);
    bassdrum.set_sample_rate(sample_rate_);
    hihat_osc.set_sample_rate(sample_rate_);
  }
 protected:
  real_t inv_sr;
  unsigned int randseed;
  oscillators::SineOscillator bassdrum;
  oscillators::SquareOscillator hihat_osc;
};

}
}
}
