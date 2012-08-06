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
#ifndef AUDIO_SYNTH_FILTERS_IIR_FILTER_H
#define AUDIO_SYNTH_FILTERS_IIR_FILTER_H
  
#include "../base.h"

namespace audio {
namespace synth {
namespace filters {

template <int P,int Q>
class IIRFilter : public Component {
 public:
  IIRFilter() {
    memset(a,0,sizeof(a));
    memset(b,0,sizeof(a));
    memset(x,0,sizeof(a));
    memset(y,0,sizeof(a));
  }
  virtual ~IIRFilter() {

  }
  real_t Tick(real_t x0) {
    memmove(&x[1],x,sizeof(x)-sizeof(real_t));
    x[0] = x0;

    real_t result=0;
    for (int i=0;i<P;++i) {
      result += a[i]*x[i];
    }

    for (int j=1;j<Q;++j) {
      result -= b[j]*y[j];
    }

    memmove(&y[1],y,sizeof(y)-sizeof(real_t));
    y[0] = result / b[0];
    return y[0];
  }
 protected:
  real_t a[P]; //feedback filter coefficients
  real_t x[P]; 
  real_t b[Q]; //feedforward filter coefficients
  real_t y[Q]; 
};


}
}
}

#endif