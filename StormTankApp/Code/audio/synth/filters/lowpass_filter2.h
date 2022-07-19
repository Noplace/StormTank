#pragma once

namespace audio {
    namespace synth {
        namespace filters {



            class LowPassFilter2 : public Component {
            public:
                double y, v0, v1, y1, y2, x1, x2;
                LowPassFilter2() :y(0), y1(0), y2(0), x1(0), x2(0), v0(0), v1(0) {
                }
                double Tick(double x, double freq) {

                    double c = 1.0 / tan(XM_PI * freq / double(sample_rate_));
                    double r = 0.1;

                    double a1 = 1.0 / (1.0 + r * c + c * c);
                    double a2 = 2 * a1;
                    double a3 = a1;
                    double b1 = 2.0 * (1.0 - c * c) * a1;
                    double b2 = (1.0 - r * c + c * c) * a1;
                    y = a1 * x + a2 * x1 + a3 * x2 - b1 * y1 - b2 * y2;
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