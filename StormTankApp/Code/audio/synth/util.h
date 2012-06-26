#ifndef AUDIO_SYNTH_UTIL_H
#define AUDIO_SYNTH_UTIL_H

namespace audio {
namespace synth {

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
    Component::set_sample_rate(sample_rate);
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

#endif