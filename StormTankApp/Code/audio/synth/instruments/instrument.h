namespace audio {
namespace synth {
namespace instruments {

class InstrumentData {
 public:
  NoteData note_data_array[Polyphony];
  InstrumentData() {
    memset(note_data_array,0,sizeof(note_data_array));
  }
  virtual ~InstrumentData() {

  }
};

class InstrumentProcessor : public Component {
 public:
  InstrumentProcessor() : Component() {
   
  }
  virtual ~InstrumentProcessor() {

  }
  //virtual void Initialize() = 0;
  //virtual void Deinitialize() = 0;
  virtual InstrumentData* NewInstrumentData() = 0;
  virtual double Tick(InstrumentData* data, int note_index) = 0;
  virtual void Update(InstrumentData* data, int note_index) = 0;
};

}
}
}