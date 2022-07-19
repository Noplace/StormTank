#pragma once

namespace audio {
    namespace synth {
        namespace filters {

            class LowPassFilter1 : public Filter {
            public:
                LowPassFilter1() :y1(0), a0(0), b1(0), cutoff_freq_(0) {

                }

                int Initialize() {
                    y1 = 0;
                    Update();
                    return S_OK;
                }

                int Update() {
                    real_t fc = (cutoff_freq_) / (sample_rate_);
                    real_t x = expf(-2 * XM_PI * fc);
                    a0 = 1 - x;
                    b1 = x;
                    return S_OK;
                }

                real_t Tick(real_t sample) {
                    real_t y = a0 * sample + b1 * y1;
                    y1 = y;
                    return y;
                }

                void set_cutoff_freq(real_t cutoff_freq) { cutoff_freq_ = cutoff_freq; }
            protected:
                real_t a0, b1, cutoff_freq_;
                real_t y1;

            };

        }
    }
}