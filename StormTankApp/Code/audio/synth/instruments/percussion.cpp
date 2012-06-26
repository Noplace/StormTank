#include "../synth.h"

namespace audio {
namespace synth {
namespace instruments {

double Percussion::Tick(InstrumentData* data, int note_index) {
  auto& table = ((PercussionData*)data)->table;
  double result = 0;

    
  double value = (double)((randseed *= 0x15a4e35) % 255) / 255.0;
  result = 0.4*table.highhat_amp*sin(value * 2.0f * XM_PI);
  table.highhat_amp = max(0,table.highhat_amp - inv_sr);
  
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
  result += 0.4*table.bassdrum_amp*bassdrum.Tick(table.bassdrum_phase,table.bassdrum_inc);
  table.bassdrum_amp = max(0,table.bassdrum_amp - 2*inv_sr);


  return result;
}

void Percussion::Update(InstrumentData* data, int note_index) {
  auto cdata = (PercussionData*)data;
  NoteData* nd = &cdata->note_data_array[note_index];
  if (nd->note == 42 && nd->on == true) {
    cdata->table.highhat_closed = true;
    cdata->table.highhat_amp = 1.0;
  }
  if (nd->note == 42 && nd->on == false) {
    cdata->table.highhat_closed = false;
    cdata->table.highhat_amp = 0;
  }
  if (nd->note == 35 && nd->on == true) {
    cdata->table.bassdrum_amp = 1.0;
  }
  if (nd->note == 35 && nd->on == false) {
    cdata->table.bassdrum_amp = 0;
  }
  if (nd->note == 46 && nd->on == true) {
    cdata->table.highhat_closed = true;
    cdata->table.highhat_amp = 1.0;
  }
  if (nd->note == 46 && nd->on == false) {
    cdata->table.highhat_closed = false;
    cdata->table.highhat_amp = 0;
  }
  cdata->table.bassdrum_phase = 0;
  cdata->table.bassdrum_inc = bassdrum.get_increment(20);

  cdata->table.hihat_phase[0] = 0;
  cdata->table.hihat_inc[0] = bassdrum.get_increment(460.0);
  cdata->table.hihat_phase[1] = 0;
  cdata->table.hihat_inc[1] = bassdrum.get_increment(1840.0);
  cdata->table.hihat_phase[2] = 0;
  cdata->table.hihat_inc[2] = bassdrum.get_increment(5404.0);
  cdata->table.hihat_phase[3] = 0;
  cdata->table.hihat_inc[3] = bassdrum.get_increment(7220.0);

}

}
}
}