
namespace audio {
namespace synth {
namespace instruments {

class PadData : public InstrumentData {
 public:
  struct {
     uint32_t phase,inc;
  } pad_table[Polyphony];
  PadData() : InstrumentData() {
    memset(pad_table,0,sizeof(pad_table));
  }
};

class Pad : public InstrumentProcessor {
 public:
  Pad();
  virtual ~Pad() {
    Unload();
  }
  virtual int Load();
  virtual int Unload();
  virtual InstrumentData* NewInstrumentData() {
    return new PadData();
  }
  virtual real_t Tick(InstrumentData* data, int note_index);
  virtual int NoteOn(InstrumentData* data, int note_index);
  virtual int NoteOff(InstrumentData* data, int note_index);
 protected:
  PadSynth padsynth;
  float* buffer[128];
  real_t harmonics_array[64];
  uint32_t sample_size_;
  virtual void CalculateHarmonics(real_t freq) {
    for (int i=0;i<64;++i) {
      harmonics_array[i] = cos(2*XM_PI*i/64.0f);
    }
  }
};

}
}
}
