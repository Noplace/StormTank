namespace audio {
namespace synth {
namespace instruments {

class OscWaveData : public InstrumentData {
 public:
  struct {
      uint32_t phase,inc;
  } table[Polyphony];
  OscWaveData() : InstrumentData() {
    memset(table,0,sizeof(table));
  }
};

class OscWave : public InstrumentProcessor {
 public:
  enum WaveType {
   Sine,Square,Triangle,Sawtooth,Exponent
  };
  OscWave(WaveType type) : InstrumentProcessor(),osc(nullptr) {
   switch (type) {
      case Sine:      osc = new oscillators::SineOscillator();    break;
      case Square:    osc = new oscillators::SquareOscillator();    break;
      case Triangle:  osc = new oscillators::TriangleOscillator();    break;
      case Sawtooth:  osc = new oscillators::SawtoothOscillator();    break;
      case Exponent:  osc = new oscillators::ExpOscillator();    break;
      default:
        osc = new oscillators::SineOscillator();
        break;
   }
  }
  virtual ~OscWave() {
    delete osc;
  }
  InstrumentData* NewInstrumentData() {
    return new OscWaveData();
  }
  double Tick(InstrumentData* data, int note_index) {
    auto cdata = (OscWaveData*)data;
    auto result = osc->Tick(cdata->table[note_index].phase,cdata->table[note_index].inc);// + osc->Tick(phase,inc*2);
    return result;
  }
  void Update(InstrumentData* data, int note_index) {
    auto cdata = (OscWaveData*)data;
    cdata->table[note_index].inc = osc->get_increment(cdata->note_data_array[note_index].freq);
  }
  void set_sample_rate(uint32_t sample_rate) { 
    InstrumentProcessor::set_sample_rate(sample_rate); 
    osc->set_sample_rate(sample_rate);
  }
 protected:
  oscillators::Oscillator* osc;
};

}
}
}
