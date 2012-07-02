#include "synth.h"

#define SQRT_1OVER2  0.70710678118654752440084436210485

namespace audio {
namespace synth {

const char* eventnames[] = {"Meta",
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
"Unknown",
"SysEx",
"SequenceNumber",
"DividedSysEx",
"Channel",
"NoteOff",
"NoteOn",
"NoteAftertouch",
"Controller",
"ProgramChange",
"ChannelAftertouch",
"PitchBend",
};
/*
kEventTypeMeta,
kEventTypeText,
kEventTypeCopyrightNotice,
kEventTypeTrackName,
kEventTypeInstrumentName,
kEventTypeLyrics,
kEventTypeMarker,
kEventTypeCuePoint,
kEventTypeMidiChannelPrefix,
kEventTypeEndOfTrack,
kEventTypeSetTempo,
kEventTypeSmpteOffset,
kEventTypeTimeSignature,
kEventTypeKeySignature,
kEventTypeSequencerSpecific,
kEventTypeUnknown,
kEventTypeSysEx,
kEventTypeSequenceNumber,
kEventTypeDividedSysEx,
kEventTypeChannel,
kEventTypeNoteOff,
kEventTypeNoteOn,
kEventTypeNoteAftertouch,
kEventTypeController,
kEventTypeProgramChange,
kEventTypeChannelAftertouch,
kEventTypePitchBend,*/

MidiSynth::MidiEventHandler MidiSynth::midi_main_event_handlers[5] = {
  &MidiSynth::MidiEventMeta,    &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, 
  &MidiSynth::MidiEventChannel, &MidiSynth::MidiEventUnknown, 
};

MidiSynth::MidiEventHandler MidiSynth::midi_meta_event_handlers[22] = {
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventSetTempo, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, 
};

MidiSynth::MidiEventHandler MidiSynth::midi_channel_event_handlers[22] = {
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventNoteOff, 
  &MidiSynth::MidiEventNoteOn, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventController, &MidiSynth::MidiEventProgramChange,  &MidiSynth::MidiEventUnknown,&MidiSynth::MidiEventUnknown,

};

void MidiSynth::Initialize() {
  if (initialized_ == true)
    return;

  memset(&tracks,0,sizeof(tracks));

  util.set_sample_rate(sample_rate_);

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
    channels[i]->set_panning(0.5);
    channels[i]->set_amplification(0.4);
  }
  //percussion
  percussion_instr = new instruments::Percussion();
  percussion_instr->set_sample_rate(sample_rate_);
  channels[9]->set_instrument(percussion_instr);
  tracks = nullptr;
  
  delay_unit.Initialize(uint32_t(sample_rate_*2));
  delay_unit.set_feedback(0.3);
  delay_unit.set_delay_ms(400);

  initialized_ = true;
}

void MidiSynth::Deinitialize() {
  if (initialized_ == false)
    return;

  DestroyTrackData();
  for (int i=0;i<kChannelCount;++i) {
    SafeDelete(&channels[i]);
  }
  SafeDelete(&percussion_instr);
  for (int i=0;i<kInstrumentCount;++i) {
    SafeDelete(&instr[i]);
  }
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
  double bpm = 120; //qn per min
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
      /*if (current_event.subtype == midi::kEventTypeProgramChange) {
        sprintf(str,"time %04.03f type : %s %s track %d channel %d\n",time_ms,eventnames[source_event->type],eventnames[source_event->subtype],source_event->track,source_event->channel);
        OutputDebugString(str);
        sprintf(str,"\tdata: %d\n",current_event.data.program.number);
        OutputDebugString(str);
      }
      if (current_event.subtype == midi::kEventTypeController) {
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
  ResetTracks();
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
  double frequency = util.NoteFreq((Notes)event->data.note.noteNumber);
  channel->AddNote(event->data.note.noteNumber,frequency,event->data.note.velocity / 127.0);
}
void MidiSynth::MidiEventNoteOff(midi::Event* event) {
  auto channel = channels[event->channel];
  channel->RemoveNote(event->data.note.noteNumber);
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
  if (event->data.controller.type == 0x0A) { //pan
    double pan = event->data.controller.value / 127.0;
    channel->set_panning(pan);
  }
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

void MidiSynth::MixChannels(double& output_left,double& output_right) {
  output_left=output_right=0;
  for (int j=0;j<kChannelCount;++j) { //mix all 16 channels
    auto channel = channels[j];
    if (channel->silence() == false) {
      double left_sample=0,right_sample=0;
      channel->Tick(left_sample,right_sample);
      output_left += left_sample;
      output_right += right_sample;
    }
  }
}

}
}

