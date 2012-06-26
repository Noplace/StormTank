#include "../synth.h"

namespace audio {
namespace synth {
namespace instruments {

BlitWave::BlitWave() : InstrumentProcessor() {
  memset(wavetables,0,sizeof(wavetables));
}

BlitWave::~BlitWave() {
  for (int i=0;i<128;++i)
    SafeDeleteArray(&wavetables[i]);
}
  
double BlitWave::Tick(InstrumentData* data, int note_index) {
  auto cdata = (BlitWaveData*)data;
  int note = cdata->note_data_array[note_index].note;
  
  double result = 0.4*wavetables[note][cdata->table[note_index].inc1];
  cdata->table[note_index].inc1 = (cdata->table[note_index].inc1 + 1) % 4096;
  return result;
}

void BlitWave::Update(InstrumentData* data, int note_index) {
  auto cdata = (BlitWaveData*)data;
  
}

void BlitWave::GenerateWavetables() {
  double A4_freq = 440;
  double two = pow(2.0,1/12.0);
  double f_s = double(sample_rate_);

  for (int i=0;i<128;++i) {
    if (wavetables[i] == nullptr)
      wavetables[i] = new double[4096];

    auto freq = A4_freq * pow(two,i-69);
    auto harmonics = uint32_t(f_s / (freq * 2));

    for (int n=0;n<4096;++n) {
      double result = 0;
      double x = 2*XM_PI*n*freq/(f_s);
      for (uint32_t i=1;i<=harmonics;++i) {
        double h = double(i);
        double m = pow(cos((i-1)*XM_PI/(2*harmonics)),2);
        result += m*(1/h)*sin(h*x);
      }
      wavetables[i][n] = result;
    }

  }

}

}
}
}