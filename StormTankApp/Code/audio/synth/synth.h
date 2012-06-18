#include <WinCore/windows/base.h>
#include <WinCore/timer/timer2.h>
#include <xnamath.h>
#include "../midi/midi2.h"
#include "../output/output.h"
#include "component.h"
#include "util.h"
#include "oscillators/oscillator.h"
#include "oscillators/sine_oscillator.h"
#include "oscillators/square_oscillator.h"
#include "oscillators/triangle_oscillator.h"
#include "oscillators/sawtooth_oscillator.h"
#include "oscillators/exp_oscillator.h"
#include "instruments/instrument.h"
#include "instruments/osc_wave.h"
#include "filters/lowpass.h"
#include "effects/delay.h"
#include "channel.h"
#include "player.h"

double sgn(double d);

namespace audio {
namespace synth {

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

}
}