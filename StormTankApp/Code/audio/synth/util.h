namespace audio {
namespace synth {

const int note_count = 127;
enum Notes {
  A0=21,Bb0,B0,
  C1,Db1,D1,Eb1,E1,F1,Gb1,G1,Ab1,A1,Bb1,B1,
  C2,Db2,D2,Eb2,E2,F2,Gb2,G2,Ab2,A2,Bb2,B2,
  C3,Db3,D3,Eb3,E3,F3,Gb3,G3,Ab3,A3,Bb3,B3,
  C4,Db4,D4,Eb4,E4,F4,Gb4,G4,Ab4,A4,Bb4,B4,
  C5,Db5,D5,Eb5,E5,F5,Gb5,G5,Ab5,A5,Bb5,B5,
  C6,Db6,D6,Eb6,E6,F6,Gb6,G6,Ab6,A6,Bb6,B6,
  C7,Db7,D7,Eb7,E7,F7,Gb7,G7,Ab7,A7,Bb7,B7,
  C8,Db8,D8,Eb8,E8,F8,Gb8,G8,Ab8,A8,Bb8,B8,
};

class Util : public Component {
 public:
  Util() : Component() {
      double A4_freq = 440;
      double two = pow(2.0,1/12.0);
      chromatic_scale_freq = new double[note_count];
      for (int i=0;i<note_count;++i) {
        chromatic_scale_freq[i] = A4_freq * pow(two,i-69);//45 = index of A4
      }

      
  }

  virtual ~Util() {
    delete [] chromatic_scale_freq;
  }

  void set_sample_rate(uint32_t sample_rate) { 
    sample_rate_ = sample_rate;
    sr_inv = 1.0/double(sample_rate_);
  }

  double NoteFreq(Notes note) {
    return chromatic_scale_freq[note];
  }

  double* chromatic_scale_freq;
  double sr_inv;
};

}
}