#include "synth.h"

#define SQRT_1OVER2  0.70710678118654752440084436210485

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

  memset(&tracks,0,sizeof(tracks));

  util.set_sample_rate(sample_rate_);
  buffers.pre_effects = new real_t[sample_rate_*2]; //44100 samples & channels , check with player later on!
  buffers.post_effects = new real_t[sample_rate_*2]; //44100 samples & channels , check with player later on!
  last_event = nullptr;
  samples_to_next_event = 0;

  //initialize all available instruments
  memset(instr,0,sizeof(instr));
  instr[0] = new instruments::Piano();
  instr[2] = new instruments::OscWave(instruments::OscWave::Triangle);
  instr[81] = new instruments::OscWave(instruments::OscWave::Square);
  instr[82] = new instruments::OscWave(instruments::OscWave::Sawtooth);
  for (int i=0;i<kInstrumentCount;++i) {
    if (instr[i] == nullptr)
      instr[i] = new instruments::OscWave(instruments::OscWave::Sine);
    instr[i]->set_sample_rate(sample_rate_);
  }

  for (int i=0;i<kChannelCount;++i) {
    channels[i] = new Channel(i);
    channels[i]->set_sample_rate(sample_rate_);
    channels[i]->set_instrument(instr[0]);
    channels[i]->set_silence(true);
    channels[i]->set_panning(0.5f);
    channels[i]->set_amplification(0.4f);
  }
  //percussion
  percussion_instr = new instruments::Percussion();
  percussion_instr->set_sample_rate(sample_rate_);
  channels[9]->set_instrument(percussion_instr);
  tracks = nullptr;
  

  //misc 
  pitch_bend_range = 2.0f;
  rpn_fine = rpn_coarse = 0;

  delay_unit.Initialize(uint32_t(sample_rate_*2));
  delay_unit.set_feedback(0.3f);
  delay_unit.set_delay_ms(400.0f);

  initialized_ = true;
}

void MidiSynth::Deinitialize() {
  if (initialized_ == false)
    return;
  delay_unit.Deinitialize();
  DestroyTrackData();
  for (int i=0;i<kChannelCount;++i) {
    SafeDelete(&channels[i]);
  }
  SafeDelete(&percussion_instr);
  for (int i=0;i<kInstrumentCount;++i) {
    SafeDelete(&instr[i]);
  }
  SafeDeleteArray(&buffers.post_effects);
  SafeDeleteArray(&buffers.pre_effects);
  initialized_ = false;
}

void MidiSynth::LoadMidi(char* filename) {
  player_->Stop();
  
  FILE* fp = fopen(filename,"rb");
  fseek(fp,0,SEEK_END);
  long size = ftell(fp);
  fseek(fp,0,SEEK_SET);
  auto buffer = new uint8_t[size];
  fread(buffer,1,size,fp);
  fclose(fp);
  midi::MidiLoader midifile;
  midifile.Load(buffer,size);
  
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
      tracks[track_index].event_count = midifile.eventList[track_index].size();
      tracks[track_index].event_sequence = new midi::Event[tracks[track_index].event_count];
      max_event_count = max(max_event_count,tracks[track_index].event_count);
  }
  //ResetTracks();
  //uint32_t abs_time[64];
  //memset(abs_time,0,sizeof(abs_time));
  char str[512];
  for (uint32_t event_index=0;event_index<max_event_count;++event_index) {
    int track_index=0;
    for (track_index=0;track_index<track_count_;++track_index) {
      auto& track = tracks[track_index];
      if (event_index >= tracks[track_index].event_count)
        continue;
      auto source_event = &midifile.eventList[track_index][event_index];

      switch (source_event->subtype) {
        case midi::kEventSubtypeSetTempo:
          bpm = 60000000.0 / double(source_event->data.tempo.microsecondsPerBeat);
          bps = bpm / 60.0;
          secs_per_tick = 1 / ( bps * ticks_per_beat );
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
      
      sprintf(str,"time %04.03f type : %s %d %s track %d channel %d\n",time_ms,eventnames[source_event->type],source_event->subtype,subeventnames[source_event->subtype],source_event->track,source_event->channel);
      OutputDebugString(str);

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

  delete [] buffer;
  Reset();
}


void MidiSynth::RenderSamples(uint32_t samples_count, short* data_out) {
	auto data_offset = 0;
  while (true) {
		if (samples_to_next_event != 0xFFFFFFFF && samples_to_next_event <= samples_count) {
			/* generate samplesToNextEvent samples, process event and repeat */
			auto samples_to_generate = samples_to_next_event;
			if (samples_to_generate > 0) {
        GenerateIntoBuffer(samples_to_generate,data_out,data_offset);
				data_offset += samples_to_generate * 2;
				samples_count -= samples_to_generate;
				samples_to_next_event -= samples_to_generate;
			}
      if (last_event != nullptr)
			  HandleEvent(last_event);
      last_event = GetNextEvent();
		} else {
			/* generate samples to end of buffer */
			if (samples_count > 0) {
				GenerateIntoBuffer(samples_count,data_out,data_offset);
				samples_to_next_event -= samples_count;
			}
			break;
		}
	}
}

void MidiSynth::GenerateIntoBuffer(uint32_t samples_to_generate,short* data_out,uint32_t data_offset) {
    real_t ov_left_sample=0,ov_right_sample=0;

    
    MixChannels(samples_to_generate);
    memcpy(buffers.post_effects,buffers.pre_effects,samples_to_generate*2*sizeof(float));
    //delay_unit.Process(buffers.pre_effects,buffers.post_effects,samples_to_generate);
    
    for (uint32_t i=0;i<samples_to_generate<<1;i+=2) {
      data_out[data_offset++] = short(32767.0f * min(buffers.post_effects[i],1.0f));
      data_out[data_offset++] = short(32767.0f * min(buffers.post_effects[i+1],1.0f));;
    }

    //while (samples_to_generate) {

      //global effects
      //if (apply_delay) {
      //double lpfreq = 800;
      //static double t = 0;
      //ov_left_sample = lowpass.Tick(ov_left_sample,lpfreq+sin(t)*lpfreq);
      //ov_right_sample = lowpass.Tick(ov_right_sample,lpfreq+sin(t)*lpfreq);
      //t += 0.00001;
      //delay_unit.Process(ov_left_sample,ov_right_sample,ov_left_sample,ov_right_sample);
      //}

      //clip and write to output
      ///data_out[data_offset++] = short(32767.0f * min(ov_left_sample,1.0f));
      //data_out[data_offset++] = short(32767.0f * min(ov_right_sample,1.0f));
      //--samples_to_generate;
      //player_->song_counter_ms += util.sample_time_ms_;
    //}
  }

void MidiSynth::MixChannels(uint32_t samples_to_generate) {
  
  for (uint32_t i=0;i<samples_to_generate<<1;i+=2) {
    real_t output_left=0,output_right=0;
    for (int j=0;j<kChannelCount;j+=4) { //mix all 16 channels
      //auto channel = channels[j];
      if (channels[j]->silence() == false) {
        channels[j]->Tick(output_left,output_right);
      }
      if (channels[j+1]->silence() == false) {
        channels[j+1]->Tick(output_left,output_right);
      }
      if (channels[j+2]->silence() == false) {
        channels[j+2]->Tick(output_left,output_right);
      }
      if (channels[j+3]->silence() == false) {
        channels[j+3]->Tick(output_left,output_right);
      }
    }
    buffers.pre_effects[i] = output_left;
    buffers.pre_effects[i+1] = output_right;
  }
}

midi::Event* MidiSynth::GetNextEvent() {
  uint32_t track_index=0xFFFFFFFF;
  uint32_t event_index=0xFFFFFFFF;
  uint32_t ticks_to_next_event = 0xFFFFFFFF;
  midi::Event* result = nullptr;

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
    if (rpn_coarse == 0) {
      pitch_bend_range = (real_t)event->data.controller.value;
    }
  } else if (event->data.controller.type == 0x26) { //data entry slider fine
    if (rpn_fine == 0) {
      pitch_bend_range += (real_t)event->data.controller.value / 100.0f;
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
  } else if (event->data.controller.type == 0x64) { //RPR coarse
    rpn_coarse = event->data.controller.value;
  } else if (event->data.controller.type == 0x65) { //RPR fine
    rpn_fine = event->data.controller.value;
  }
}

void MidiSynth::MidiEventPitchBend(midi::Event* event) {
  auto channel = channels[event->channel];
  real_t amount = ((event->data.pitch.value - 8192) / 16384.0f)*2*pitch_bend_range;
  channel->SetPitchBend(amount);
}

}
}

