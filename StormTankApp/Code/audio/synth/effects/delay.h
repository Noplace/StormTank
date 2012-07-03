namespace audio {
namespace synth {

class Delay {
 public:
  Delay() : left_buffer(nullptr),right_buffer(nullptr), max_samples(0), buffer_index(0),feedback(0.3f) {
  }
  ~Delay() {
    Deinitialize();
  }

  void Initialize(int max_samples)  {
    this->max_samples = max_samples;
    left_buffer = new real_t[max_samples];
    right_buffer = new real_t[max_samples];
    memset(left_buffer,0,max_samples*sizeof(real_t));
    memset(right_buffer,0,max_samples*sizeof(real_t));
  }

  void Deinitialize() {
    SafeDeleteArray(&left_buffer);
    SafeDeleteArray(&right_buffer);
  }
  //2 channels only
  void Process(float* in_samples, float* out_samples, uint32_t sample_count) {
    for (uint32_t i=0;i<sample_count<<1;i+=2) {
      //if (buffer_index >= max_samples)
      //  buffer_index = 0;
      left_buffer[buffer_index] = in_samples[i];
      right_buffer[buffer_index] = in_samples[i+1];
      //int j = buffer_index - delay_index;
      //if ( j < 0)
      //  j += max_samples;

      unsigned delayed_index = ( buffer_index - delay_index + max_samples ) % max_samples;
      /*if ( j != g ){
        int a = 1;
        a++;
      }*/
      out_samples[i] = left_buffer[buffer_index] = (in_samples[i] + (left_buffer[delayed_index]*feedback));
      out_samples[i+1] = right_buffer[buffer_index] = (in_samples[i+1] + (right_buffer[delayed_index]*feedback));
      //++buffer_index;
      buffer_index = (buffer_index + 1) % max_samples;
    }
  }

  void set_feedback(real_t feedback) { this->feedback = feedback; }
  void set_delay_ms(real_t delay_ms) { 
    this->delay_ms = delay_ms; 
    delay_index = int( delay_ms * 0.001f * 44100 ); 
  }
 private:
  real_t* left_buffer;
  real_t* right_buffer;
  real_t feedback,delay_ms;
  int buffer_index,max_samples;
  int delay_index;
};

}
}