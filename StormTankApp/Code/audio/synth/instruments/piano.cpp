#include "../synth.h"

namespace audio {
namespace synth {
namespace instruments {

double Piano::Tick(InstrumentData* data, int note_index) {
  auto cdata = (PianoData*)data;
  auto& nd = cdata->table[note_index];



  auto result = 0.4*osc1.Tick(nd.phase1,nd.inc1);
  result += 0.3*osc2.Tick(nd.phase2,nd.inc2);
  result += 0.2*osc3.Tick(nd.phase3,nd.inc3);
  result += 0.2*osc4.Tick(nd.phase4,nd.inc4);



  return result;
}

void Piano::Update(InstrumentData* data, int note_index) {
  auto cdata = (PianoData*)data;
  /*double value = (double)((randseed *= 0x15a4e35) % 255) / 255.0;
  uint32_t phase_shift = osc1.get_increment(value*80.0);
  cdata->table[note_index].phase1 = phase_shift; 
  cdata->table[note_index].phase2 = phase_shift;
  value = (double)((randseed *= 0x15a4e35) % 255) / 255.0;
  phase_shift = osc1.get_increment(value*80.0);
  cdata->table[note_index].phase3 = phase_shift;
  cdata->table[note_index].phase4 = phase_shift;*/
  cdata->table[note_index].inc1 = osc1.get_increment(cdata->note_data_array[note_index].freq);
  cdata->table[note_index].inc2 = osc2.get_increment(cdata->note_data_array[note_index].freq*1.5);
  cdata->table[note_index].inc3 = osc3.get_increment(cdata->note_data_array[note_index].freq*2);
  cdata->table[note_index].inc4 = osc4.get_increment(cdata->note_data_array[note_index].freq*2.5);
}

}
}
}