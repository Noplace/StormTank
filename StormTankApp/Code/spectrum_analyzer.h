#ifndef SPECTRUM_ANALYZER_H
#define SPECTRUM_ANALYZER_H

class SpectrumAnalyzer : public graphics::shape::Shape, public audio::synth::VisualAddon {
 public:
  SpectrumAnalyzer();
  ~SpectrumAnalyzer();
  int Initialize(graphics::Context* context);
  int Deinitialize();
  int Construct() {
    return S_OK;
  }
  void AddPCMData(short* samples, uint32_t count, uint32_t channels, double time_ms);
  int Draw();
 private:
  const static int nBinsLog2 = 8;
  const static int nBins = 1 << nBinsLog2;
  XDSP::XVECTOR* real_samples;
  XDSP::XVECTOR* img_samples;
  XDSP::XVECTOR* unity;
  float* scratch;
  float freq_pow[256];
  double time_counter;
  int rindex;
  //graphics::ContextD3D9* gfx;
  CRITICAL_SECTION cs;
};

#endif