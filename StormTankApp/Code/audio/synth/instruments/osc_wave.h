namespace audio {
namespace synth {
namespace instruments {



class OscWave : public Instrument {
 public:
  enum WaveType {
   Sine,Square,Triangle,Sawtooth,Exponent
  };
  OscWave(WaveType type) : Instrument(),osc(nullptr) {
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
  double Tick(uint32_t& phase,uint32_t inc) {
    /*double result = 0;
    for (int i=0;i<8;++i) {
      result += note_table[i].velocity*osc->Tick(note_table[i].phase,note_table[i].inc);
      //note_table[i].velocity = max(0,note_table[i].velocity-0.00001);
    }*/
    return osc->Tick(phase,inc);
  }
  void set_sample_rate(uint32_t sample_rate) { 
    sample_rate_ = sample_rate; 
    osc->set_sample_rate(sample_rate);
  }
  uint32_t get_increment(double freq) {
    return osc->get_increment(freq);
  }
 protected:
  oscillators::Oscillator* osc;
};

}
}
}
