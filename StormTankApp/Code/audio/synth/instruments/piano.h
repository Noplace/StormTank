namespace audio {
namespace synth {
namespace instruments {

class PianoData : public InstrumentData {
 public:
  struct {
     uint32_t phase1,inc1;
     uint32_t phase2,inc2;
     uint32_t phase3,inc3;
  } table[Polyphony];
  PianoData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class Piano : public InstrumentProcessor {
 public:
  Piano() : InstrumentProcessor() {

  }
  virtual ~Piano() {
    
  }

  InstrumentData* NewInstrumentData() {
    return new PianoData();
  }

  double Tick(InstrumentData* data, int note_index) {
    auto cdata = (PianoData*)data;
    auto& nd = cdata->table[note_index];
    auto result = osc1.Tick(nd.phase1,nd.inc1);
    result += 0.4*osc2.Tick(nd.phase2,nd.inc2);
    result += 0.3*osc3.Tick(nd.phase3,nd.inc3);
    return result;
  }

  void Update(InstrumentData* data, int note_index) {
    auto cdata = (PianoData*)data;
    cdata->table[note_index].inc1 = osc1.get_increment(cdata->note_data_array[note_index].freq);
    cdata->table[note_index].inc2 = osc2.get_increment(cdata->note_data_array[note_index].freq*2);
    cdata->table[note_index].inc3 = osc3.get_increment(cdata->note_data_array[note_index].freq*3);
  }

  void set_sample_rate(uint32_t sample_rate) { 
    InstrumentProcessor::set_sample_rate(sample_rate); 
    osc1.set_sample_rate(sample_rate);
    osc2.set_sample_rate(sample_rate);
    osc3.set_sample_rate(sample_rate);
  }
 protected:
  oscillators::SineOscillator osc1;
  oscillators::SineOscillator osc2;
  oscillators::SawtoothOscillator osc3;
};

}
}
}
