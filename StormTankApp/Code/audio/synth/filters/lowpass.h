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
#ifndef AUDIO_SYNTH_FILTERS_LOWPASS_H
#define AUDIO_SYNTH_FILTERS_LOWPASS_H
  
namespace audio {
namespace synth {
namespace filters {

class Filter : public Component {
 public:
  virtual int Initialize() = 0;
  virtual int Update() = 0;
  virtual real_t Tick(real_t) = 0;
};

class MoogFilter : public Component {
 public:
  real_t cutoff,fs,res,f,k,p,scale,r,x,y1,y2,y3,y4,oldx,oldy1,oldy2,oldy3;
  void Initialize() {
    cutoff = 200;
    res = 0.2f;
    fs = (real_t)sample_rate_;
    y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
    Update();
  }

  void Update() {
    f = 2 * cutoff / fs; //[0 - 1]
    k = 3.6f*f - 1.6f*f*f -1; //(Empirical tunning)
    p = (k+1)*0.5f;
    scale = exp((1-p))*1.386249f;
    r = res*scale;
  }
  real_t Tick(real_t sample) {
    x = sample - r*y4;
    //Four cascaded onepole filters (bilinear transform)
    y1=x*p + oldx*p - k*y1;
    y2=y1*p+oldy1*p - k*y2;
    y3=y2*p+oldy2*p - k*y3;
    y4=y3*p+oldy3*p - k*y4;
    //Clipper band limited sigmoid
    y4 = y4 - (y4*y4*y4)/6;
    oldx = x;
    oldy1 = y1;
    oldy2 = y2;
    oldy3 = y3;
    return y4;
  }
};

class LowPassFilter : public Filter {
 public:  
  LowPassFilter():y1(0),a0(0),b1(0),cutoff_freq_(0) {

  }

  int Initialize() {
    y1=0;
    Update();
    return S_OK;
  }

  int Update() {
    real_t fc = (cutoff_freq_)/(sample_rate_);
    real_t x = exp(-2*XM_PI*fc);
    a0 = 1 - x;
    b1 = x;
    return S_OK;
  }

  real_t Tick(real_t sample) {
    real_t y = a0*sample+b1*y1;
    y1 = y;
    return y;
  }

  void set_cutoff_freq(real_t cutoff_freq) { cutoff_freq_ = cutoff_freq; }
 protected:
  real_t a0,b1,cutoff_freq_;
  real_t y1;

};


class LowPass : public Component {
 public:
  double y,v0,v1,y1,y2,x1,x2;
  LowPass() :y(0),y1(0),y2(0),x1(0),x2(0), v0(0),v1(0) {
  }
  double Tick(double x,double freq) {

      double c = 1.0 / tan(XM_PI * freq / double(sample_rate_));
      double r = 0.1;

      double a1 = 1.0 / ( 1.0 + r * c + c * c);
      double a2 = 2* a1;
      double a3 = a1;
      double b1 = 2.0 * ( 1.0 - c*c) * a1;
      double b2 = ( 1.0 - r * c + c * c) * a1;
      y = a1 * x + a2 * x1 + a3 * x2 - b1*y1 - b2*y2;
      y2 = y1;
      y1 = y;
      x2 = x1;
      x1 = x;
      
    /*double RC = 1 / (2*XM_PI*freq);
    double dt = 1 / double(sample_rate_);
    double a = dt / (RC + dt);
    y = (a * x) + ((1-a) * y);*/
   //double c = 2*sin(XM_PI*freq/double(sample_rate_));

 //c = pow(0.5, (128-cutoff)   / 16.0);
  //double r = 1;//pow(0.5, (resonance+24) / 16.0);

//Loop:

   //v0 =  (1-r*c)*v0  -  (c)*v1  + (c)*x;
   //v1 =  (1-r*c)*v1  +  (c)*v0;

 // output = v1;

    
    
    //static double y = 0;
    
    return y;
  };
};

}
}
}

#endif