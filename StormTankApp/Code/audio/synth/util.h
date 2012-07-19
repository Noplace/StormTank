#ifndef AUDIO_SYNTH_UTIL_H
#define AUDIO_SYNTH_UTIL_H

namespace audio {
namespace synth {

class Util : public Component {
 public:
  Util() : Component() {
      real_t A4_freq = 440;
      chromatic_scale_freq = new real_t[note_count];
      for (int i=0;i<note_count;++i) {
        chromatic_scale_freq[i] = A4_freq * pow(_2_POW_12TH,i-69);//45 = index of A4
      }

      
  }

  virtual ~Util() {
    delete [] chromatic_scale_freq;
  }

  void set_sample_rate(uint32_t sample_rate) { 
    Component::set_sample_rate(sample_rate);
    sr_inv = 1.0f/real_t(sample_rate_);
  }

  real_t NoteFreq(Notes note) {
    return chromatic_scale_freq[note];
  }

  real_t* chromatic_scale_freq;
  real_t sr_inv;
};

}
}

#endif