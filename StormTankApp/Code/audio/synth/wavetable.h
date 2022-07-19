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
#ifndef AUDIO_SYNTH_WAVETABLE_H
#define AUDIO_SYNTH_WAVETABLE_H

#include "../base.h"

//#define kIndex 8
//#define kIndexRange (1<<kIndex)
//#define kPrecision 24
//#define kPrecisionMask 0xFFFFFF
//#define kPrecisionRange 16777216.0f //(double)(1<<kPrecision);
//#define k1Div24lowerBits (1.0f/(kPrecisionRange))

namespace audio {
namespace synth {

template<int kIndex=8,int kPrecision=24>
class Wavetable : public Component {
 public:
  const int kPrecisionMask;
  const int kIndexRange;
  const real_t kPrecisionRange;
  const real_t kPrecisionRangeInv;

  Wavetable() : Component(),
    table(nullptr),
    kPrecisionMask((1<<kPrecision)-1),
    kIndexRange(1<<kIndex),
    kPrecisionRange((real_t)(1<<kPrecision)),
    kPrecisionRangeInv(1.0f/(kPrecisionRange)){
   //kPrecisionMask = ((1<<kPrecision)-1);
   //kIndexRange = (1<<kIndex);
   //kPrecisionRange = (real_t)(1<<kPrecision);
  }

  virtual ~Wavetable() {

  }

  virtual real_t Tick(uint32_t& phase,uint32_t inc) {
    // the 8 MSB are the index in the table in the range 0-255 
    int i = phase >> kPrecision; 
    // and the kPrecision LSB are the fractionnal part
    real_t frac = (phase & kPrecisionMask) * kPrecisionRangeInv;
    // increment the phase for the next tick
    phase += inc; // the phase overflow itself
    return (table[i]*(1.0f-frac) + table[i+1]*frac); // linear interpolation
  }

  virtual uint32_t get_increment(real_t frequency) {
    return (uint32_t)((kIndexRange * frequency / sample_rate_) * kPrecisionRange);
  }

 protected:
  real_t* table;
};

}
}

#endif