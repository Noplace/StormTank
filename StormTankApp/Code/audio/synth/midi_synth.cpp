#include "midi_synth.h"
#include "instruments/instrument.h"
#include "instruments/osc_wave.h"
#include "instruments/pad.h"
#include "instruments/piano.h"
#include "instruments/violin.h"
#include "instruments/percussion.h"
#include "instruments/blit.h"

//#define OUTPUT_EVENTS

namespace audio {
namespace synth {

const char* eventnames[] = {"Meta",
"SysEx",
"DividedSysEx",
"Channel",
"Unknown"
};
const char* subeventnames[] = {
"Text",
"CopyrightNotice",
"TrackName",
"InstrumentName",
"Lyrics",
"Marker",
"CuePoint",
"MidiChannelPrefix",
"EndOfTrack",
"SetTempo",
"SmpteOffset",
"TimeSignature",
"KeySignature",
"SequencerSpecific",
"SequenceNumber",
"NoteOff",
"NoteOn",
"NoteAftertouch",
"Controller",
"ProgramChange",
"ChannelAftertouch",
"PitchBend",
"Unknown",
};


MidiSynth::MidiEventHandler MidiSynth::midi_main_event_handlers[5] = {
  &MidiSynth::MidiEventMeta,    &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, 
  &MidiSynth::MidiEventChannel, &MidiSynth::MidiEventUnknown, 
};

MidiSynth::MidiEventHandler MidiSynth::midi_meta_event_handlers[23] = {
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventSetTempo, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,  &MidiSynth::MidiEventUnknown,
};

MidiSynth::MidiEventHandler MidiSynth::midi_channel_event_handlers[23] = {
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventNoteOff, 
  &MidiSynth::MidiEventNoteOn, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventController, &MidiSynth::MidiEventProgramChange,  &MidiSynth::MidiEventUnknown,&MidiSynth::MidiEventPitchBend, &MidiSynth::MidiEventUnknown,

};

void MidiSynth::Initialize() {
  if (initialized_ == true)
    return;


  util.set_sample_rate(sample_rate_);
  

  auto buffer_length_ms_ = 400.0f;//400ms
  auto buffer_size = uint32_t(sample_rate_*2*buffer_length_ms_*0.001);
  buffers.aux_size = buffers.main_size = buffer_size;
  buffers.main = new real_t[buffers.main_size]; //44100 samples & channels , check with player later on!
  buffers.aux = new real_t[buffers.aux_size]; //44100 samples & channels , check with player later on!

  last_event = nullptr;
  samples_to_next_event = 0;

  //initialize all available instruments
  
  instr[0] = new instruments::Piano();
  instr[2] = new instruments::OscWave(instruments::OscWave::Triangle);

  instr[40] = new instruments::Violin();

  instr[48] = new instruments::Pad();//strings1
  instr[49] = new instruments::Pad();

  instr[80] = new instruments::OscWave(instruments::OscWave::Square);
  instr[81] = new instruments::OscWave(instruments::OscWave::Sawtooth);
  for (int i=0;i<kInstrumentCount;++i) {
    if (instr[i] == nullptr)
      instr[i] = new instruments::OscWave(instruments::OscWave::Sine);
    instr[i]->set_sample_rate(sample_rate_);
  }

  instr[0]->Load();
  for (int i=0;i<kChannelCount;++i) {
    channels[i] = new Channel(i);
    channels[i]->set_sample_rate(sample_rate_);
    channels[i]->set_buffer_length(400.0f);
    channels[i]->set_instrument(instr[0]);
    channels[i]->set_silence(true);
    channels[i]->set_panning(0.5f);
    channels[i]->set_amplification(0.7f);
  }
  //percussion
  percussion_instr = new instruments::Percussion();
  percussion_instr->set_sample_rate(sample_rate_);
  percussion_instr->Load();
  channels[9]->set_instrument(percussion_instr);
 


  delay_unit.Initialize(uint32_t(sample_rate_*2));
  delay_unit.set_feedback(0.3f);
  delay_unit.set_delay_ms(400.0f);

  tracks = nullptr;

  if (mode_ == kModeTest) {
    track_count_ = 1;
    tracks = new Track[track_count_];
    bpm = 120; //qn per min
    channels[0]->set_silence(false);
    uint32_t max_event_count = 0;
    for (int track_index=0;track_index<track_count_;++track_index) {
        auto& track = tracks[track_index];
        tracks[track_index].event_count = 20;
        tracks[track_index].event_sequence = new midi::Event[tracks[track_index].event_count];
        tracks[track_index].event_index = 0;
        max_event_count = max(max_event_count,tracks[track_index].event_count);
    }
  }
  InitializeCriticalSection(&me_lock);
  initialized_ = true;
}

void MidiSynth::Deinitialize() {
  if (initialized_ == false)
    return;
  DeleteCriticalSection(&me_lock);
  delay_unit.Deinitialize();
  DestroyTrackData();
  for (int i=0;i<kChannelCount;++i) {
    SafeDelete(&channels[i]);
  }
  SafeDelete(&percussion_instr);
  for (int i=0;i<kInstrumentCount;++i) {
    SafeDelete(&instr[i]);
  }
  SafeDeleteArray(&buffers.aux);
  SafeDeleteArray(&buffers.main);
  initialized_ = false;
}

void MidiSynth::LoadMidiFromFile(const char* filename) {
  uint8_t* buffer;
  size_t size=0;
  core::io::ReadWholeFileBinary(filename,&buffer,size);
  if (size != 0) {
    LoadMidi(buffer,size);
    core::io::DestroyFileBuffer(&buffer);
  }
}

void MidiSynth::LoadMidi(uint8_t* data, size_t data_size) {
  if (mode_ == kModeTest)
    return;
  player_->Stop();
  
  midi::MidiLoader midifile;
  midifile.Load(data,data_size);
  
  DestroyTrackData();
  track_count_ = midifile.track_count;
  tracks = new Track[track_count_];
  ticks_per_beat = midifile.ticksPerBeat;
  bpm = 120; //qn per min
  double bps = bpm / 60.0;
  double secs_per_tick;
  double time_ms;
  secs_per_tick = 1 / ( bps * ticks_per_beat );
  player_->song_length_ms = 0;   


  uint32_t max_event_count = 0;
  for (int track_index=0;track_index<track_count_;++track_index) {
      auto& track = tracks[track_index];
      tracks[track_index].event_count = (uint32_t)midifile.eventList[track_index].size();
      tracks[track_index].event_sequence = new midi::Event[tracks[track_index].event_count];
      max_event_count = max(max_event_count,tracks[track_index].event_count);
  }
  //ResetTracks();
  //uint32_t abs_time[64];
  //memset(abs_time,0,sizeof(abs_time));
  
  for (uint32_t event_index=0;event_index<max_event_count;++event_index) {
    int track_index=0;
    for (track_index=0;track_index<track_count_;++track_index) {
      auto& track = tracks[track_index];
      if (event_index >= tracks[track_index].event_count)
        continue;
      auto source_event = &midifile.eventList[track_index][event_index];

      switch (source_event->subtype) {
        case midi::kEventSubtypeNoteOff:
          if (source_event->data.note.velocity == 0)
            source_event->data.note.velocity = 127;
        break;

        case midi::kEventSubtypeSetTempo:
          bpm = 60000000.0 / double(source_event->data.tempo.microsecondsPerBeat);
          bps = bpm / 60.0;
          secs_per_tick = 1 / ( bps * ticks_per_beat );
        break;

        case midi::kEventSubtypeProgramChange:
          if (source_event->channel == 9) {
            percussion_instr->Load();
          } else {
            instr[source_event->data.program.number]->Load();
          }
        break;
      }

      auto& current_event = tracks[track_index].event_sequence[event_index];
      memset(&current_event,0,sizeof(midi::Event));
      //abs_time[track_index] += source_event->deltaTime;
      double prev_abs_time_ms = event_index == 0?0.0:tracks[track_index].event_sequence[event_index-1].abs_time_ms;
      time_ms = secs_per_tick * 1000 * source_event->deltaTime;
      current_event.abs_time_ms = prev_abs_time_ms+time_ms;
      current_event.deltaTime = source_event->deltaTime;
      current_event.type = source_event->type;
      current_event.subtype = source_event->subtype;
      current_event.channel = source_event->channel;
      current_event.track = source_event->track;
      channels[source_event->channel]->set_silence(false);
      memcpy(&current_event.data,&source_event->data,sizeof(current_event.data));
      
      #ifdef OUTPUT_EVENTS
        char str[512];
        sprintf(str,"time\t%04.03f\ttype : %s\t%02d\t%s\ttrack %02d\tchannel %02d\n",time_ms,eventnames[source_event->type],source_event->subtype,subeventnames[source_event->subtype],source_event->track,source_event->channel);
        OutputDebugString(str);
      #endif
      /*if (current_event.subtype == midi::kEventSubtypeProgramChange) {
        sprintf(str,"time %04.03f type : %s %s track %d channel %d\n",time_ms,eventnames[source_event->type],eventnames[source_event->subtype],source_event->track,source_event->channel);
        OutputDebugString(str);
        sprintf(str,"\tdata: %d\n",current_event.data.program.number);
        OutputDebugString(str);
      }
      if (current_event.subtype == midi::kEventSubtypeController) {
        sprintf(str,"time %04.03f type : %s %s track %d channel %d\n",time_ms,eventnames[source_event->type],eventnames[source_event->subtype],source_event->track,source_event->channel);
        OutputDebugString(str);
        sprintf(str,"\tdata: %d %d\n",current_event.data.controller.type,current_event.data.controller.value);
        OutputDebugString(str);
      }*/
    }
    
  } 

  //find song length
  for (int i=0;i<track_count_;++i) {
    player_->song_length_ms = max(player_->song_length_ms,tracks[i].event_sequence[tracks[i].event_count-1].abs_time_ms);
  }

  Reset();
}


void MidiSynth::RenderSamplesStereo(uint32_t samples_count, real_t* data_out) {
	uint32_t data_offset = 0;
  if (mode_ == kModeTest) {
    samples_to_next_event = samples_count;
		auto samples_to_generate = samples_to_next_event;
		if (samples_to_generate > 0) {
      GenerateIntoBufferStereo(samples_to_generate,data_out,data_offset);
			data_offset += samples_to_generate * 2;
			samples_count -= samples_to_generate;
			samples_to_next_event -= samples_to_generate;
      if (mode_ == kModeTest)
        samples_to_next_event = 0xFFFFFFFF;
		}
    if (last_event != nullptr) {
			HandleEvent(last_event);
      memset(last_event,0,sizeof(midi::Event));
    }
    last_event = GetNextEvent();
  } else if (mode_ == kModeSequencer) {
    while (true) {
		  if (samples_to_next_event != 0xFFFFFFFF && samples_to_next_event <= samples_count) {
			  /* generate samplesToNextEvent samples, process event and repeat */
			  auto samples_to_generate = samples_to_next_event;
			  if (samples_to_generate > 0) {
          GenerateIntoBufferStereo(samples_to_generate,data_out,data_offset);
				  samples_count -= samples_to_generate;
				  samples_to_next_event -= samples_to_generate;
          if (mode_ == kModeTest)
            samples_to_next_event = 0xFFFFFFFF;
			  }
        if (last_event != nullptr) {
			    HandleEvent(last_event);
          if (mode_ == kModeTest)
            memset(last_event,0,sizeof(midi::Event));
        }
        last_event = GetNextEvent();

		  } else {
			  /* generate samples to end of buffer */
			  if (samples_count > 0) {
				  GenerateIntoBufferStereo(samples_count,data_out,data_offset);
				  samples_to_next_event -= samples_count;
			  }
			  break;
		  }
	  }
  }


}


void MidiSynth::GenerateIntoBufferStereo(uint32_t samples_to_generate, real_t* data_out, uint32_t& data_offset) {
  MixChannelsStereo(samples_to_generate);
  //SendToAux
  //mix with aux
  //global post processing
  //memcpy(buffers.post_effects,buffers.pre_effects,samples_to_generate*2*sizeof(real_t));
  delay_unit.Process(buffers.main,buffers.main,samples_to_generate);
  for (uint32_t i=0;i<samples_to_generate<<1;i+=2) {
    data_out[data_offset++] = buffers.main[i];
    data_out[data_offset++] = buffers.main[i+1];
  }
}

void MidiSynth::MixChannelsStereo(uint32_t samples_to_generate) {
  
  //render and mix each chanel
  auto mainbufptr = buffers.main;
  //memset(mainbufptr,0,sizeof(real_t)*samples_to_generate*2);
  for (uint32_t j=0;j<kChannelCount;j+=4) { //mix all 16 channels
    channels[j+0]->Render(samples_to_generate);
    channels[j+1]->Render(samples_to_generate);
    channels[j+2]->Render(samples_to_generate);
    channels[j+3]->Render(samples_to_generate);
    auto buf0 = channels[j+0]->buffer();
    auto buf1 = channels[j+1]->buffer();
    auto buf2 = channels[j+2]->buffer();
    auto buf3 = channels[j+3]->buffer();
    
    for (uint32_t i=0;i<samples_to_generate<<1;++i) {
      mainbufptr[i] = buf0[i]+buf1[i]+buf2[i]+buf3[i];
      ++i;
      mainbufptr[i] += buf0[i]+buf1[i]+buf2[i]+buf3[i];
    }
  }
}

midi::Event* MidiSynth::GetNextEvent() {
  midi::Event* result = nullptr;
  
  if (mode_ == kModeSequencer) {
    uint32_t track_index=0xFFFFFFFF;
    uint32_t event_index=0xFFFFFFFF;
    uint32_t ticks_to_next_event = 0xFFFFFFFF;

	  for (int i = 0; i < track_count_; i++) {
		  if (
			  tracks[i].ticks_to_next_event != 0xFFFFFFFF
			  && (ticks_to_next_event == 0xFFFFFFFF || tracks[i].ticks_to_next_event < ticks_to_next_event)
		  ) {
			  ticks_to_next_event = tracks[i].ticks_to_next_event;
			  track_index = i;
			  event_index = tracks[i].event_index;
		  }
	  }

	  if (track_index != 0xFFFFFFFF) {
		  /* consume event from that track */
		  result = &tracks[track_index].event_sequence[event_index];
		  if ((event_index+1) < tracks[track_index].event_count) {
			  tracks[track_index].ticks_to_next_event += tracks[track_index].event_sequence[event_index+1].deltaTime;
		  } else {
			  tracks[track_index].ticks_to_next_event = 0xFFFFFFFF;
		  }
		  ++tracks[track_index].event_index;
		  /* advance timings on all tracks by ticksToNextEvent */
		  for (int i = 0; i < track_count_; i++) {
			  if (tracks[i].ticks_to_next_event != 0xFFFFFFFF) {
				  tracks[i].ticks_to_next_event -= ticks_to_next_event;
			  }
		  }

	    auto beatsToNextEvent = ticks_to_next_event / double(ticks_per_beat);
		  auto secondsToNextEvent = beatsToNextEvent / (bpm / 60);
		  samples_to_next_event += uint32_t(secondsToNextEvent * sample_rate_);
	  } else {
		  result = nullptr;
		  samples_to_next_event = 0xFFFFFFFF;
      player_->Stop();
	  }
  } else if (mode_ == kModeTest) {
    EnterCriticalSection(&me_lock);
    result = &tracks[0].event_sequence[tracks[0].event_index];
    tracks[0].ticks_to_next_event = 0;

	  auto beatsToNextEvent = tracks[0].ticks_to_next_event / double(ticks_per_beat);
		auto secondsToNextEvent = beatsToNextEvent / (bpm / 60);
		samples_to_next_event += uint32_t(secondsToNextEvent * sample_rate_);
    tracks[0].event_index = (tracks[0].event_index + 1) % tracks[0].event_count;
    LeaveCriticalSection(&me_lock);
  }
  return result;
}

void MidiSynth::HandleEvent(midi::Event* event) {
  (this->*(midi_main_event_handlers[event->type]))(event);
};

void MidiSynth::MidiEventUnknown(midi::Event* event) {
  
}

void MidiSynth::MidiEventMeta(midi::Event* event) {
  (this->*(midi_meta_event_handlers[event->subtype]))(event);
}

void MidiSynth::MidiEventSetTempo(midi::Event* event) {
  bpm = 60000000.0 / event->data.tempo.microsecondsPerBeat;
}

void MidiSynth::MidiEventChannel(midi::Event* event) {
  (this->*(midi_channel_event_handlers[event->subtype]))(event);
}

void MidiSynth::MidiEventNoteOn(midi::Event* event) {
  auto channel = channels[event->channel];
  real_t frequency = util.NoteFreq((Notes)event->data.note.noteNumber);
  channel->AddNote(event->data.note.noteNumber,frequency,event->data.note.velocity / 127.0f);
}

void MidiSynth::MidiEventNoteOff(midi::Event* event) {
  auto channel = channels[event->channel];
  channel->RemoveNote(event->data.note.noteNumber,event->data.note.velocity / 127.0f);
}

void MidiSynth::MidiEventProgramChange(midi::Event* event) {
  auto channel = channels[event->channel];
  if (event->channel == 9) {
    channel->set_instrument(percussion_instr);
  } else {
		channel->set_instrument(instr[event->data.program.number]);
  }
  channel->set_silence(false);
}

void MidiSynth::MidiEventController(midi::Event* event) {
  auto channel = channels[event->channel];
  if (event->data.controller.type == 0x06) { //data entry slider coarse
    if (channel->param_coarse == 0) {
      channel->des_coarse = event->data.controller.value;
      real_t p = ((real_t)channel->des_coarse*0.5f) + ((real_t)channel->des_fine / 100.0f);
      channel->set_pitch_bend_range(p);
    }
  } else if (event->data.controller.type == 0x26) { //data entry slider fine
    if (channel->param_fine == 0) {
      channel->des_fine = event->data.controller.value;
      real_t p = ((real_t)channel->des_coarse) + ((real_t)channel->des_fine / 100.0f);
      channel->set_pitch_bend_range(p);
    }
  } else if (event->data.controller.type == 0x0A) { //pan
    real_t pan = event->data.controller.value / 127.0f;
    channel->set_panning(pan);
  } else if (event->data.controller.type == 0x49) { //sound attack time
    real_t time = event->data.controller.value / 127.0f;
    //channel->adsr[0].set
  } else if (event->data.controller.type == 0x07) { //main volume
    real_t vol = event->data.controller.value / 127.0f;
    channel->set_amplification(vol);
  } else if (event->data.controller.type == 0x64) { //RPN coarse
    channel->param_coarse = event->data.controller.value;
  } else if (event->data.controller.type == 0x65) { //RPN fine
    channel->param_fine = event->data.controller.value;
  }
}

void MidiSynth::MidiEventPitchBend(midi::Event* event) {
  auto channel = channels[event->channel];
  real_t amount = ((event->data.pitch.value - 8192) / 16384.0f)*channel->pitch_bend_range();
  channel->SetPitchBend(amount);
}

}
}

