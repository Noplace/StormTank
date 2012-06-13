namespace audio {
namespace synth {
namespace instruments {

class SineWave : public Instrument {
 public:
  SineWave() : Instrument() {
   set_freq(0,0);
   set_freq(1,0);
   set_freq(2,0);
   set_freq(3,0);
   set_freq(4,0);
   set_freq(5,0);
   set_freq(6,0);
   set_freq(7,0);
  }
  virtual ~SineWave() {

  }
  double Tick() {
    return note_table[0].on*osc[0].Tick()+
      note_table[1].on*osc[1].Tick()+
      note_table[2].on*osc[2].Tick()+
      note_table[3].on*osc[3].Tick()+
      note_table[4].on*osc[4].Tick()+
      note_table[5].on*osc[5].Tick()+
      note_table[6].on*osc[6].Tick()+
      note_table[7].on*osc[7].Tick();
  }
  void set_sample_rate(uint32_t sample_rate) { 
    sample_rate_ = sample_rate; 
    for (int i=0;i<8;++i) {
      osc[i].set_sample_rate(sample_rate);
    }
  }
  void set_freq(int index,double freq) {
    osc[index].set_frequency(freq);
  }
 protected:
  oscillators::SineOscillator osc[8];
};

}
}
}
