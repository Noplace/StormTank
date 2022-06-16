#ifndef SPECTRUM_ANALYZER_H
#define SPECTRUM_ANALYZER_H

#include <x3daudio.h>
#include "..\XDSP.h"

class SpectrumAnalyzer : public ve::shape::Shape, public audio::synth::VisualAddon {
 public:
  SpectrumAnalyzer();
  ~SpectrumAnalyzer();
  int Initialize(ve::Context* context);
  int Deinitialize();
  int Construct() {
    return S_OK;
  }
  void AddPCMData256(float* samples, uint32_t channels, double time_ms);
  int Update();
  int Draw();
  float freq_pow[256];
 private:
  const static int nBinsLog2 = 8;
  const static int nBins = 1 << nBinsLog2;
  XDSP::XMVECTOR* real_samples;
  XDSP::XMVECTOR* img_samples;
  XDSP::XMVECTOR* unity;
  float* scratch;

  double time_counter;
  int rindex;
  //graphics::ContextD3D9* gfx;
  CRITICAL_SECTION cs;
};

#endif