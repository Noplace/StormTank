#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include <x3daudio.h>
#include <XDSP.h>

class Oscilloscope : public graphics::shape::Shape, public audio::synth::VisualAddon {
 public:
  Oscilloscope();
  ~Oscilloscope();
  int Initialize(graphics::Context* context);
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
  XDSP::XVECTOR* real_samples;
  XDSP::XVECTOR* img_samples;
  XDSP::XVECTOR* unity;
  float* scratch;

  double time_counter;
  int rindex;
  //graphics::ContextD3D9* gfx;
  CRITICAL_SECTION cs;
};

#endif