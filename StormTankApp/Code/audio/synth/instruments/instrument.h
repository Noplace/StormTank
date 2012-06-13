namespace audio {
namespace synth {

class Instrument : public Component {
 public:
  Instrument() : Component() {
    memset(note_table,0,sizeof(note_table));
  }
  virtual ~Instrument() {

  }
  virtual double Tick() = 0;
  virtual void set_freq(int index,double freq) = 0;
  virtual void add_note(int note,double freq,double velocity) {
    auto* ptr = &note_table[0];
    int i=8;
    while (ptr->on == true && ptr->note != note && --i) {
      ++ptr;
    }
    ptr->note = note;
    ptr->freq = freq;
    ptr->velocity = velocity;
    ptr->on = true;
    set_freq(8-i,ptr->freq);
  }
  virtual void remove_note(int note) {
    auto* ptr = &note_table[0];
    int i=8;
    while (ptr->note != note && --i) {
      ++ptr;
    }
    if (ptr->note == note) {
      ptr->on = false;
      ptr->note = 0;
      ptr->freq = 0;
      ptr->velocity = 0;
      set_freq(8-i,ptr->freq);
    }
  }
 protected:
  struct {
    double freq; //8 note polyphony
    double velocity;
    int note;
    bool on;
  } note_table[8];
};

}
}
