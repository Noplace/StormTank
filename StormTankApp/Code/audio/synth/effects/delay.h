namespace audio {
namespace synth {

class Delay {
 public:
  Delay(int max_samples) : max_samples(max_samples), buffer_index(0),feedback(0.3) {
    left_buffer = new double[max_samples];
    right_buffer = new double[max_samples];
    memset(left_buffer,0,max_samples*sizeof(double));
    memset(right_buffer,0,max_samples*sizeof(double));
    set_delay_ms(400);
  }
  ~Delay() {
    delete [] left_buffer;
    delete [] right_buffer;
  }

  void Process(double in_left_sample,double in_right_sample,double& out_left_sample,double& out_right_sample) {
    //if (buffer_index >= max_samples)
    //  buffer_index = 0;
    left_buffer[buffer_index] = in_left_sample;
    right_buffer[buffer_index] = in_right_sample;
    //int j = buffer_index - delay_index;
    //if ( j < 0)
    //  j += max_samples;

    unsigned delayed_index = ( buffer_index - delay_index + max_samples ) % max_samples;
    /*if ( j != g ){
      int a = 1;
      a++;
    }*/
    out_left_sample = left_buffer[buffer_index] = (in_left_sample + (left_buffer[delayed_index]*feedback));
    out_right_sample = right_buffer[buffer_index] = (in_right_sample + (right_buffer[delayed_index]*feedback));
    //++buffer_index;
    buffer_index = (buffer_index + 1) % max_samples;
  }

  void set_feedback(double feedback) { this->feedback = feedback; }
  void set_delay_ms(double delay_ms) { 
    this->delay_ms = delay_ms; 
    delay_index = int( delay_ms * 0.001 * 44100 ); 
  }
 private:
  double* left_buffer;
  double* right_buffer;
  double feedback,delay_ms;
  int buffer_index,max_samples;
  int delay_index;
};

}
}