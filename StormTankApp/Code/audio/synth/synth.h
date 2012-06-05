#include <WinCore/windows/windows.h>
#include <xnamath.h>
#include "lfo/lfo.h"

double sgn(double d);

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

class BasicGenerator {
 public:
  BasicGenerator(int sample_rate,int channels,int bits) : sample_rate_(sample_rate),channels_(channels),bits_(bits) {
     range_mul = (pow(2,double(bits)) / 2.0) - 1;
     double A4 = 440;
     double two = pow(2.0,1/12.0);
     chromatic_scale_freq = new double[note_count];
     for (int i=0;i<note_count;++i) {
       chromatic_scale_freq[i] = A4 * pow(two,i-69);//45 = index of A4
     }
     channel_weights = new double[channels];
     for (int i=0;i<channels;++i)
      channel_weights[i] = 1.0;
     sr_inv = 1.0/double(sample_rate_);

     tick = 0;
  }
  virtual ~BasicGenerator() {
    delete [] chromatic_scale_freq;
    delete [] channel_weights;
  }

  template<typename T>
  T GenerateSineWaveSingle(double frequency) {
    T result = T(0);
    double af = 2*XM_PI*frequency;
    auto omega = (af);
    uint32_t buf_index = 0;
    
    //for (uint32_t si=0;si<samples;++si) {
     //t = double(si)*sr_inv;
    auto value = sin(omega*tick);
    tick += sr_inv;
    if (tick > 1.0)
      tick  = 0.0;
    result = T(range_mul*value);
    //}
    return result;
  }

  template<typename T>
  uint32_t GenerateSineWave(double frequency, uint32_t samples, T* data_out) {
    double af = 2*XM_PI*frequency;
    auto omega = (af);
    uint32_t buf_index = 0;
    double t=0;
    for (uint32_t si=0;si<samples;++si) {
      t = double(si)*sr_inv;
      auto value = sin(omega*t);
      for (int i=0;i<channels_;++i) {
        data_out[buf_index++] = T(range_mul*channel_weights[i]*value);
      }
    }
    return buf_index;
  }

  template<typename T>
  uint32_t GenerateSquareWave(double frequency, uint32_t samples, T* data_out) {
    double af = 2*XM_PI*frequency;
    auto omega = (af);
    uint32_t buf_index = 0;
    double t=0;
    for (uint32_t si=0;si<samples;++si) {
      t = double(si)*sr_inv;
      auto value = sgn(sin(omega*t));
      for (int i=0;i<channels_;++i) {
        data_out[buf_index++] = T(range_mul*channel_weights[i]*value);
      }
    }
    return buf_index;
  }

  template<typename T>
  uint32_t GenerateSawtoothWave(double frequency, uint32_t samples, T* data_out) {
    uint32_t buf_index = 0;
    double t=0;
    for (uint32_t si=0;si<samples;++si) {
      t = double(si)*sr_inv;
      double a = frequency*t;
      auto value = (2*(a-floor(a-0.5)));
      for (int i=0;i<channels_;++i) {
        data_out[buf_index++] = T(range_mul*channel_weights[i]*value);
      }
    }
    return buf_index;
  }

  template<typename T>
  uint32_t GenerateTriangleWave(double frequency, uint32_t samples, T* data_out) {
    uint32_t buf_index = 0;
    double t=0;
    double period = 1/frequency;
    double a = period*0.5;
    double a_inv = 2*frequency;
    for (uint32_t si=0;si<samples;++si) {
      t = double(si)*sr_inv;
      auto value =  (2*a_inv)*(t-a*floor((t*a_inv)+0.5))*pow(-1,floor((t*a_inv)-0.5)); 
      for (int i=0;i<channels_;++i) {
        data_out[buf_index++] = T(range_mul*channel_weights[i]*value);
      }
    }
    return buf_index;
  }

  void SetChannelWeights(double* weights) {
    memcpy(channel_weights,weights,channels_*sizeof(double));
  }
  void SetBeatsPerMinute(double bpm) {
    bpm_ = bpm;
  }
  double NoteFreq(Notes note) {
    return chromatic_scale_freq[note];
  }
  uint32_t SamplesPerWholeNote() {
   return uint32_t((sample_rate_ * 60.0) / bpm_); //samples per min
  }
  uint32_t SamplesPerTimeMS(double time_ms) {
   return uint32_t((sample_rate_ * time_ms) / 1000.0); //samples per min
  }
 protected:
  double* chromatic_scale_freq;
  double *channel_weights;
  double range_mul;
  double bpm_;
  double sr_inv;
  int sample_rate_;
  int channels_;
  int bits_;
  double tick;
};


class Synth {
 public:
  Synth() : osc1(44100) {
  }
  ~Synth() {
  }
  LFO osc1;
};

}
}