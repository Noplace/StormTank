#include "piano.h"

namespace audio {
namespace synth {
namespace instruments {


class LowPassFilter : public Component {
 public:  
  LowPassFilter() {
    x1=x2=y1=y2=0;
  }
  real_t Tick(real_t x,real_t freq) {
    real_t fc = (freq*0.5f)/(44100.0f*0.5f);
    real_t x_ = exp(-2*XM_PI*fc);
    a0 = 1 - x_;
    b1 = x_;

    real_t y = a0*x+b1*y1;
    y1 = y;
    return y;
  }
  real_t a0,b1;
  real_t x1,x2,y1,y2;

};

Piano::Piano() : Pad() {
 
}

int Piano::Load() {
  if (loaded_ == true)
    return S_FALSE;
 
  int hr = Pad::Load();
  if (hr != S_OK)
    return hr;

  osc1.set_sample_rate(sample_rate_);
  osc2.set_sample_rate(sample_rate_);
  osc3.set_sample_rate(sample_rate_);
  osc4.set_sample_rate(sample_rate_);

  //default adsr, should be instrument specific
  for (int i=0;i<Polyphony;++i) {
    adsr[i].set_sample_rate(sample_rate_);
    adsr[i].set_attack_amp(1.0f);
    adsr[i].set_sustain_amp(0.6f);
    adsr[i].set_attack_time_ms(40.0f);
    adsr[i].set_decay_time_ms(50.0f);
    adsr[i].set_release_time_ms(100.5f);
  }


  //lowpass.set_sample_rate(sample_rate_);
  //lowpass.Initialize(0.1,0,0,4);
  return hr;
}

int Piano::Unload() {
  if (loaded_ == false)
    return S_FALSE;
  
  return Pad::Unload();
}

real_t Piano::Tick(InstrumentData* data, int note_index) {
  auto cdata = (PianoData*)data;
  real_t result = 0;
  {
    auto& nd = cdata->pad_table[note_index];
    result += buffer[cdata->note_data_array[note_index].note][nd.phase];
    nd.phase = (nd.phase + 1) % sample_size_;

  }
  {
    auto& nd = cdata->piano_table[note_index];
    result += 0;//osc1.Tick(nd.phase1,nd.inc1) + osc2.Tick(nd.phase2,nd.inc2);
  }

  auto amp = adsr[note_index].Tick();
  result *= amp;
  //static float t =0;
  //result = lowpass.Tick(result);
  //result = lowpass.Tick(result,10001.0f+10000.0f*sin(t));
  //t += 0.000001f;
  return result;
}


int Piano::NoteOn(InstrumentData* data, int note_index) {
  auto cdata = (PianoData*)data;

  cdata->piano_table[note_index].inc1 = osc1.get_increment(cdata->note_data_array[note_index].freq);
  cdata->piano_table[note_index].inc2 = osc2.get_increment(cdata->note_data_array[note_index].freq*1.5f);
  cdata->piano_table[note_index].inc3 = osc3.get_increment(cdata->note_data_array[note_index].freq*2.0f);
  cdata->piano_table[note_index].inc4 = osc4.get_increment(cdata->note_data_array[note_index].freq*2.5f);
  Pad::NoteOn(data,note_index);
  return S_OK;
}

int Piano::NoteOff(InstrumentData* data, int note_index) {
  Pad::NoteOff(data,note_index);
  return S_OK;
}

void Piano::CalculateHarmonics(real_t freq) {

  memset(&harmonics_array,0,sizeof(harmonics_array));

  int hcount = int(real_t(sample_rate_) / (2.0f*freq));
  hcount = min(64,hcount);
  for (int i=0;i<hcount;++i) {

    if (!(i%2)) {
      harmonics_array[i] = (1.0f / (i*XM_PI));
    }

    ///harmonics_array[i] = 64.0f/(i+1);
    //if (!(i % 2))
    //  harmonics_array[i] *= 10.0f;

    //harmonics_array[i] *= (2*freq/real_t(sample_rate_));

    //else
     //harmonics_array[i] *= cos(6*XM_PI*i/64.0f);
  }


  //  harmonics_array[0] = 1.0f; 
  //harmonics_array[1] = 1.5f; 
  //harmonics_array[2] = 0.5f; 
  //harmonics_array[3] = 0.2f; 
  //harmonics_array[4] = 0.0f; 
   
}

}
}
}