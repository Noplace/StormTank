#include "percussion.h"
#include "../filters/chebyshev_filter.h"

audio::synth::filters::ChebyshevFilter filter;


namespace audio {
namespace synth {
namespace instruments {

Percussion::PercussionPieceTick Percussion::ticks[Polyphony] = {
  &Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,
  &Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,&Percussion::EmptyTick,
};

int Percussion::Load() {
  if (loaded_ == true)
    return S_FALSE;
  inv_sr = 1 / real_t(sample_rate_);
  bassdrum.set_sample_rate(sample_rate_);
  hihat_osc.set_sample_rate(sample_rate_); 


  //default adsr, should be instrument specific
  for (int i=0;i<Polyphony;++i) {
    adsr[i].set_sample_rate(sample_rate_);
    adsr[i].set_attack_amp(1.0f);
    adsr[i].set_sustain_amp(0.6f);
    adsr[i].set_attack_time_ms(8.0f);
    adsr[i].set_decay_time_ms(8.0f);
    adsr[i].set_release_time_ms(100.5f);
  }

  filter.Initialize(0.2,0,0,4);

  loaded_ = true;
  return S_OK;
}

int Percussion::Unload() {
  if (loaded_ == false)
    return S_FALSE;
 
  loaded_ = false;
  return S_OK;
}

real_t Percussion::Tick(InstrumentData* data, int note_index) {
  auto& table = ((PercussionData*)data)->table;
  real_t result = 0;
  
  
  //result = (this->*(ticks[note_index]))((PercussionData*)data,note_index);
  if (adsr[note_index].stage() != 0 && data->note_data_array[note_index].note == 46) {
    real_t value = (real_t)((randseed *= 0x15a4e35) % 255) / 255.0f;
    result = 0.5f*sin(value * 2.0f * XM_PI);
    result *= adsr[note_index].Tick();
  }

  if (adsr[note_index].stage() != 0 && data->note_data_array[note_index].note == 35) {
    result = 0.6f*bassdrum.Tick(table.bassdrum_phase,table.bassdrum_inc);
    result = filter.Tick(result);
    result *= adsr[note_index].Tick();
  }

  //hihat
  /*table.hihat_inc[0] = bassdrum.get_increment(460.0+460.0*sin(value * 2.0f * XM_PI));
  table.hihat_phase[1] = 0;
  table.hihat_inc[1] = bassdrum.get_increment(1840.0+1840.0*sin(value * 2.0f * XM_PI));
  table.hihat_phase[2] = 0;
  table.hihat_inc[2] = bassdrum.get_increment(5404.0+5404.0*sin(value * 2.0f * XM_PI));
  table.hihat_phase[3] = 0;
  table.hihat_inc[3] = bassdrum.get_increment(3220.0+3220.0*sin(value * 2.0f * XM_PI));
  auto h1 = hihat_osc.Tick(table.hihat_phase[0],table.hihat_inc[0]) - hihat_osc.Tick(table.hihat_phase[1],table.hihat_inc[1]) - 1.0;
  auto h2 = hihat_osc.Tick(table.hihat_phase[2],table.hihat_inc[2]) - hihat_osc.Tick(table.hihat_phase[3],table.hihat_inc[3]) - 1.0;
  result = 0.4*table.highhat_amp*(h1-h2-1.0);//*sin(value * 2.0f * XM_PI);;
  table.highhat_amp = max(0,table.highhat_amp - inv_sr);
  */

  //result += 0.4f*table.bassdrum_amp*bassdrum.Tick(table.bassdrum_phase,table.bassdrum_inc);
  //table.bassdrum_amp = max(0,table.bassdrum_amp - 2*inv_sr);


  return result;
}

int Percussion::NoteOn(InstrumentData* data, int note_index) {
  auto cdata = (PercussionData*)data;
  NoteData* nd = &cdata->note_data_array[note_index];

  if (nd->note == 35 && nd->on == true) {
    cdata->table.bassdrum_amp = 1.0;
  }

  
  if (nd->note == 42 && nd->on == true) {
    cdata->table.highhat_closed = true;
    cdata->table.highhat_amp = 1.0;
  }

  if (nd->note == 46 && nd->on == true) {
    cdata->table.highhat_closed = true;
    cdata->table.highhat_amp = 1.0;
  }
 

  cdata->table.bassdrum_phase = 0;
  cdata->table.bassdrum_inc = bassdrum.get_increment(40.0f);

  cdata->table.hihat_phase[0] = 0;
  cdata->table.hihat_inc[0] = bassdrum.get_increment(460.0f);
  cdata->table.hihat_phase[1] = 0;
  cdata->table.hihat_inc[1] = bassdrum.get_increment(1840.0f);
  cdata->table.hihat_phase[2] = 0;
  cdata->table.hihat_inc[2] = bassdrum.get_increment(5404.0f);
  cdata->table.hihat_phase[3] = 0;
  cdata->table.hihat_inc[3] = bassdrum.get_increment(7220.0f);

  adsr[note_index].NoteOn(nd->velocity);
  return S_OK;
}

int Percussion::NoteOff(InstrumentData* data, int note_index) {
  auto cdata = (PercussionData*)data;
  NoteData* nd = &cdata->note_data_array[note_index];

  if (nd->note == 42 && nd->on == false) {
    cdata->table.highhat_closed = false;
    cdata->table.highhat_amp = 0;
  }
  if (nd->note == 46 && nd->on == false) {
    cdata->table.highhat_closed = false;
    cdata->table.highhat_amp = 0;
  }
  if (nd->note == 35 && nd->on == false) {
    cdata->table.bassdrum_amp = 0;
  }
 
  adsr[note_index].NoteOff(nd->velocity);
  return S_OK;
}


real_t Percussion::HihatOpenTick(PercussionData* data, int note_index) {

  real_t value = (real_t)((randseed *= 0x15a4e35) % 255) / 255.0f;
  auto result = 0.6f*data->table.highhat_amp*sin(value * 2.0f * XM_PI);
  data->table.highhat_amp = max(0,data->table.highhat_amp - inv_sr);
  return result * adsr[note_index].Tick();  
}

real_t Percussion::HihatClosedTick(PercussionData* data, int note_index) {

  real_t value = (real_t)((randseed *= 0x15a4e35) % 255) / 255.0f;
  auto result = 0.6f*data->table.highhat_amp*sin(value * 2.0f * XM_PI);
  data->table.highhat_amp = max(0,data->table.highhat_amp - inv_sr);
  return result * adsr[note_index].Tick();  
}

real_t Percussion::BassDrumTick(PercussionData* data, int note_index) {
  return 0;
}

}
}
}