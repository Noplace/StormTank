#include "../synth.h"
#include <time.h>
namespace audio {
namespace synth {
namespace instruments {

Pad::Pad() : InstrumentProcessor(),padsynth() {
 memset(&buffer,0,sizeof(buffer));
 memset(&harmonics_array,0,sizeof(harmonics_array));
}

int Pad::Load() {
  if (loaded_ == true)
    return S_FALSE;
  srand(time(0));

  /*auto choir3 = [&](real_t freq) {
    padsynth.setharmonic(0,0);
	  for (int i=1;i<64;i++) {
	      real_t ai =1.0f/i;
	      real_t formants=exp(-pow((i*freq-600.0f)/150.0f,2.0f))+exp(-pow((i*freq-900.0f)/250.0f,2.0f))+
		  exp(-pow((i*freq-2200.0f)/200.0f,2.0f))+exp(-pow((i*freq-2600.0f)/250.0f,2.0f))+
		  exp(-pow((i*freq)/3000.0f,2.0f))*0.1f;
	      ai*=formants;
        padsynth.setharmonic(i,ai*4);
    }
  };*/

  sample_size_ = 32768;
  padsynth.Initialize(sample_size_,sample_rate_,64);




  for (int i=0;i<128;++i) {
    real_t A4_freq = 440;
    real_t two = pow(2.0f,1/12.0f);
    auto freq = A4_freq * pow(two,i-69);//69 = index of A4
    buffer[i] = new float[sample_size_];
    CalculateHarmonics(freq);
    for (int j=0;j<64;j++) {
	    padsynth.setharmonic(j,harmonics_array[j]);
    };
    padsynth.synth(freq,60,1,buffer[i]);
  }

  //default adsr, should be instrument specific
  for (int i=0;i<Polyphony;++i) {
    adsr[i].set_sample_rate(sample_rate_);
    adsr[i].set_attack_amp(1.0f);
    adsr[i].set_sustain_amp(0.6f);
    adsr[i].set_attack_time_ms(8.0f);
    adsr[i].set_decay_time_ms(8.0f);
    adsr[i].set_release_time_ms(100.5f);
  }

  loaded_ = true;
  return S_OK;
}

int Pad::Unload() {
  if (loaded_ == false)
    return S_FALSE;
  for (int i=0;i<128;++i) {
    SafeDeleteArray(&buffer[i]);
  }
  padsynth.Deinitialize();
  loaded_ = false;
  return S_OK;
}

real_t Pad::Tick(InstrumentData* data, int note_index) {
  auto cdata = (PadData*)data;
  auto& nd = cdata->pad_table[note_index];

  real_t result = 0;
  result += buffer[cdata->note_data_array[note_index].note][nd.phase];
  nd.phase = (nd.phase + 1) % sample_size_;

  result *= adsr[note_index].Tick();

  return result;
}

int Pad::NoteOn(InstrumentData* data, int note_index) {
  auto cdata = (PadData*)data;
  cdata->pad_table[note_index].phase = rand() % 30000;
  adsr[note_index].NoteOn(cdata->note_data_array[note_index].velocity);
  return S_OK;
}

int Pad::NoteOff(InstrumentData* data, int note_index) {
  auto cdata = (PadData*)data;
  adsr[note_index].NoteOff(cdata->note_data_array[note_index].velocity);
  return S_OK;
}
    


}
}
}