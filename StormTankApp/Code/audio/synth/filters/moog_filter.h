#pragma once

namespace audio {
    namespace synth {
        namespace filters {


            class MoogFilter : public Component {
            public:
                real_t cutoff, fs, res, f, k, p, scale, r, x, y1, y2, y3, y4, oldx, oldy1, oldy2, oldy3;
                void Initialize() {
                    cutoff = 200;
                    res = 0.2f;
                    fs = (real_t)sample_rate_;
                    y1 = y2 = y3 = y4 = oldx = oldy1 = oldy2 = oldy3 = 0;
                    Update();
                }

                void Update() {
                    f = 2 * cutoff / fs; //[0 - 1]
                    k = 3.6f * f - 1.6f * f * f - 1; //(Empirical tunning)
                    p = (k + 1) * 0.5f;
                    scale = expf((1 - p)) * 1.386249f;
                    r = res * scale;
                }

                real_t Tick(real_t sample) {
                    x = sample - r * y4;
                    //Four cascaded onepole filters (bilinear transform)
                    y1 = x * p + oldx * p - k * y1;
                    y2 = y1 * p + oldy1 * p - k * y2;
                    y3 = y2 * p + oldy2 * p - k * y3;
                    y4 = y3 * p + oldy3 * p - k * y4;
                    //Clipper band limited sigmoid
                    y4 = y4 - (y4 * y4 * y4) / 6;
                    oldx = x;
                    oldy1 = y1;
                    oldy2 = y2;
                    oldy3 = y3;
                    return y4;
                }
            };



        }
    }
}
