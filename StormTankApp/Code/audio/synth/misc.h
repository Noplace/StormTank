/*****************************************************************************************************************
* Copyright (c) 2012 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/
#ifndef AUDIO_SYNTH_MISC_H
#define AUDIO_SYNTH_MISC_H

namespace audio {
namespace synth {

template<typename T1,typename T2>
union AnyCast{
  T1 in;
  T2 out;
};

typedef AnyCast<uint32_t,real_t> UIntToFloatCast;


real_t bits2float(uint32_t u) {
  UIntToFloatCast x;
  x.in = u;
  return x.out;
}

inline uint32_t RandomInt(uint32_t* seed) {
  *seed = *seed * 196314165 + 907633515;
  return *seed;
}

// uniform random float in [-1,1)
inline real_t RandomFloat(uint32_t* seed) {
  uint32_t bits = RandomInt(seed); // random 32-bit value
  real_t f = bits2float((bits >> 9) | 0x40000000); // random float in [2,4)
  return f - 3.0f; // uniform random float in [-1,1)
}

inline real_t Noise(uint32_t* seed) {
  real_t r1 = (1+RandomFloat(seed))*0.5f;
  real_t r2 = (1+RandomFloat(seed))*0.5f;
  return (real_t) sqrt( -2.0f * log(r1)) * cos( 2.0f * XM_PI *r2);//white noise
}

inline real_t GussianWhiteNoise() {
  real_t R1 = (real_t) rand() / (real_t) RAND_MAX;
  real_t R2 = (real_t) rand() / (real_t) RAND_MAX;
  return (real_t) sqrt( -2.0f * log( R1 )) * cos( 2.0f * XM_PI * R2 );
}

static real_t HardClip(real_t x) {
  if (x < -1.0f)
    return -1.0f;
  else if (x > 1.0f)
    return 1.0f;
  else
    return x;
}

static real_t SoftClip(real_t x) {
  static const real_t lim = 0.6666666666f;
  if (x < -lim)
    return -lim;
  else if (x > lim)
    return lim;
  else
    return x-((x*x*x)/3.0f);
}

__forceinline real_t EnhanceHarmonics(real_t x,real_t a,real_t b,real_t c,real_t d) {
  return ((x*x*x)*a)+((x*x)*b)+(x*c)+(d);
}


}
}

#endif