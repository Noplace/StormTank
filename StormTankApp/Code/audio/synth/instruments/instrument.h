namespace audio {
namespace synth {
namespace instruments {

#define Polyphony 8
class Instrument : public Component {
 public:
  Instrument() : Component() {
    
  }
  virtual ~Instrument() {

  }
  virtual double Tick(uint32_t& phase,uint32_t inc) = 0;
  virtual uint32_t get_increment(double freq) = 0;
  
  /*void CopyTo(Instrument* dest) {
    memcpy(dest->note_table,note_table,sizeof(note_table));
    dest->sample_rate_ = sample_rate_;
  }
  void CopyFrom(Instrument* source) {
    memcpy(note_table,source->note_table,sizeof(note_table));
    source->sample_rate_ = sample_rate_;
  }*/
 protected:

};

}
}
}