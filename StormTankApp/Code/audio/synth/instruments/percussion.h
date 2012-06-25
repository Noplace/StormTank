namespace audio {
namespace synth {
namespace instruments {

class PercussionData : public InstrumentData {
 public:
  struct {
    bool highhat_closed;
    double highhat_amp,bassdrum_amp;
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
  double Tick(InstrumentData* data, int note_index);
  void Update(InstrumentData* data, int note_index);
  void set_sample_rate(uint32_t sample_rate) { 
    InstrumentProcessor::set_sample_rate(sample_rate); 
    inv_sr = 1 / double(sample_rate_);
    bassdrum.set_sample_rate(sample_rate_);
    hihat_osc.set_sample_rate(sample_rate_);
  }
 protected:
  double inv_sr;
  unsigned int randseed;
  oscillators::SineOscillator bassdrum;
  oscillators::SquareOscillator hihat_osc;
};

}
}
}
