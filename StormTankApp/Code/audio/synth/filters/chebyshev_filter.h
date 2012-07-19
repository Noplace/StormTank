namespace audio {
namespace synth {
namespace filters {

class ChebyshevFilter {
 public:
  ChebyshevFilter() {
    memset(&a,0,sizeof(a));
    memset(&b,0,sizeof(b));
    memset(&x,0,sizeof(x));
    memset(&y,0,sizeof(y));
  }
  void Initialize(real_t fc,int lh,int pr,int np) {
    real_t ta[23],tb[23];
    a[2] = 1.0f;
    b[2] = 1.0f;
    for (int p=1;p<=np>>1;++p) {
      Calc(fc,lh,pr,np,p);
      for (int i=0;i<=22;++i) {
        ta[i] = a[i];
        tb[i] = b[i];
      }

      for (int i=2;i<=22;++i) {
        a[i] = a[0]*ta[i] + a[1]*ta[i-1] + a[2]*ta[i-2];
        b[i] = tb[i] - b[1]*tb[i-1] - b[2]*tb[i-2];
      }
    }
    b[2] = 0.0f;
    for (int i=0;i<=20;++i) {
      a[i] = a[i+2];
      b[i] = -b[i+2];
    }

    real_t sa = 0;
    real_t sb = 0;

    for (int i=0;i<=20;++i) {
      if (lh == 0) {
        sa = sa + a[i];
        sb = sb + b[i];
      } else if (lh == 1) {
        sa = sa + a[i] * pow(-1.0f,i);
        sb = sb + b[i] * pow(-1.0f,i);
      }
    }

    real_t gain_inv = (1-sb) / sa;

    for (int i=0;i<=20;++i) {
      a[i] = a[i] * gain_inv;
    }
  }
  void Deinitialize() {

  }

  real_t Tick(real_t sample) {
    x[22] = x[21];    x[21] = x[20];
    x[20] = x[19];    x[19] = x[18];
    x[18] = x[17];    x[17] = x[16];
    x[16] = x[15];    x[15] = x[14];
    x[14] = x[13];    x[13] = x[12];
    x[12] = x[11];    x[11] = x[10];
    x[10] = x[9];     x[9]  = x[8];
    x[8]  = x[7];     x[7]  = x[6];
    x[6]  = x[5];     x[5]  = x[4];
    x[4]  = x[3];     x[3]  = x[2];
    x[2]  = x[1];     x[1]  = x[0];
    x[0]  = sample;
    real_t result = a[0]*x[0];

    for (int i=1;i<20;++i) {
      result += a[i]*x[i] + b[i]*y[i];
    }

    y[22] = y[21];    y[21] = y[20];
    y[20] = y[19];    y[19] = y[18];
    y[18] = y[17];    y[17] = y[16];
    y[16] = y[15];    y[15] = y[14];
    y[14] = y[13];    y[13] = y[12];
    y[12] = y[11];    y[11] = y[10];
    y[10] = y[9];     y[9]  = y[8];
    y[8]  = y[7];     y[7]  = y[6];
    y[6]  = y[5];     y[5]  = y[4];
    y[4]  = y[3];     y[3]  = y[2];
    y[2]  = y[1];     y[1]  = y[0];
    y[0]  = result;
    return result;
  }

  void Calc(real_t fc,int lh,int pr,int np,int p) {
    real_t rp = -cos(XM_PI/(np*2) + (p-1)*XM_PI/np);
    real_t ip = sin(XM_PI/(np*2) + (p-1)*XM_PI/np);
    if (pr != 0) {
      real_t es = sqrt( pow(100 / (100-pr),2.0f) -1 );
      real_t vx = (1/np) * log( (1/es) + sqrt( (1/(es*es)) + 1) );
      real_t kx = (1/np) * log( (1/es) + sqrt( (1/(es*es)) - 1) );
      real_t KX = (exp(kx) + exp(-kx))/2;
      rp = rp * ( (exp(vx) - exp(-vx) ) /2 ) / kx;
      ip = ip * ( (exp(vx) + exp(-vx) ) /2 ) / kx;
    }
    real_t t = 2 * tan(0.5f);
    real_t w = 2*XM_PI*fc;
    real_t m = rp*rp + ip*ip;
    real_t d = 4 - (4*rp*t) + (m*t*t);
    real_t x0 = (t*t)/d;
    real_t x1 = (2*t*t)/d;
    real_t x2 = t*t/d;
    real_t y1 = (8 - (2*m*t*t))/d;
    real_t y2 = (-4 - (4*rp*t) - (m*t*t))/d;
    real_t k=0;
    if (lh == 1) {
      k = -cos(w/2 + 0.5f) / cos(w/2 - 0.5f);
    } else {
      k = -sin(0.5f - w/2) / sin(w/2 + 0.5f);
    }
    d = 1 + y1*k - y2*k*k;
    
    a[0] = (x0 - x1*k + x2*k*k)/d;
    a[1] = (-2*x0*k + x1 + x1*k*k - 2*x2*k)/d;
    a[2] = (x0*k*k - x1*k + x2)/d;
    b[1] = (2*k + y1 + y1*k*k - 2*y2*k)/d;
    b[2] = (-(k*k) - y2*k + y2)/d;
    if (lh == 1) {
      a[1] = -a[1];
      b[1] = -b[1];
    }
  }
 protected:
  real_t a[23],b[23],x[23],y[23];
};

}
}
}