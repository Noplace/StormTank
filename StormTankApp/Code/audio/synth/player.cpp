#include "synth.h"

#define WM_SP_QUIT 0x400
#define WM_SP_PLAY 0x401
#define WM_SP_PAUSE 0x402
#define WM_SP_STOP 0x403

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

void Player::Initialize() {
  if (initialized_ == true)
    return;

  state = 0;
  song_pos_ms = song_counter_ms = 0;
  
  memset(&tracks,0,sizeof(tracks));

  auto sample_rate = audio_interface_->wave_format().nSamplesPerSec;
  util.set_sample_rate(sample_rate);
  sample_time_ms = (1000.0/double(sample_rate));
  last_event = nullptr;
  output_buffer = new short[44100*2]; // 1 second output buffer
  samples_to_next_event = 0;
  //initialize all available instruments
  memset(instr,0,sizeof(instr));
  instr[0] = new instruments::OscWave(instruments::OscWave::Sine);
  instr[2] = new instruments::OscWave(instruments::OscWave::Triangle);
  
  instr[81] = new instruments::OscWave(instruments::OscWave::Square);
  instr[82] = new instruments::OscWave(instruments::OscWave::Sawtooth);

  for (int i=0;i<kInstrumentCount;++i) {
    if (instr[i] == nullptr)
      instr[i] = new instruments::OscWave(instruments::OscWave::Sine);

    instr[i]->set_sample_rate(sample_rate);
  }

  for (int i=0;i<kChannelCount;++i) {
    channels[i] = new Channel();
    channels[i]->set_instrument(instr[0]);
    channels[i]->set_sample_rate(sample_rate);
    channels[i]->set_silence(false);
    channels[i]->set_panning(0.5);
    channels[i]->set_amplification(0.4);
  }

  tracks = nullptr;
  
  //channels[0]->set_panning(0);
  //channels[1]->set_panning(1);
  tc_event = CreateEvent(NULL,FALSE,FALSE,NULL);
  thread_handle = CreateThread(nullptr,0,static_cast<LPTHREAD_START_ROUTINE>(PlayThread),this,0,(LPDWORD)&thread_id);//CREATE_SUSPENDED
  if (thread_handle != nullptr && tc_event != nullptr) {
    SetThreadPriority(thread_handle,THREAD_PRIORITY_ABOVE_NORMAL);
    WaitForSingleObject(tc_event,INFINITE);
  }
  initialized_ = true;
}

void Player::Deinitialize() {
  if (initialized_ == false)
    return;
  ResetEvent(tc_event);
  PostThreadMessage(thread_id,WM_SP_QUIT,0,0);
  WaitForSingleObject(tc_event,INFINITE);
  CloseHandle(thread_handle);
  CloseHandle(tc_event);
  DestroyTrackData();
  for (int i=0;i<kChannelCount;++i) {
    SafeDelete(&channels[i]);
  }
  for (int i=0;i<kInstrumentCount;++i) {
    SafeDelete(&instr[i]);
  }
  SafeDeleteArray(&output_buffer);
  initialized_ = false;
}

void Player::LoadMidi(char* filename) {
  if (state == 1)
    return;


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
  track_count = midifile.track_count;
  tracks = new Track[track_count];
  ticks_per_beat = midifile.ticksPerBeat;
  double bpm = 120; //qn per min
  double bps = bpm / 60.0;
  double secs_per_tick;
  double time_ms;
  secs_per_tick = 1 / ( bps * ticks_per_beat );
  song_length_ms = 0;   


  uint32_t max_event_count = 0;
  for (int track_index=0;track_index<track_count;++track_index) {
      auto& track = tracks[track_index];
      tracks[track_index].event_count = midifile.eventList[track_index].size();
      tracks[track_index].event_sequence = new midi::Event[tracks[track_index].event_count];
      max_event_count = max(max_event_count,tracks[track_index].event_count);
  }
  ResetTracks();
  //uint32_t abs_time[64];
  //memset(abs_time,0,sizeof(abs_time));
  //char str[512];
  for (uint32_t event_index=0;event_index<max_event_count;++event_index) {
    int track_index=0;
    for (track_index=0;track_index<track_count;++track_index) {
      auto& track = tracks[track_index];
      if (event_index >= tracks[track_index].event_count)
        continue;
      auto source_event = &midifile.eventList[track_index][event_index];

      switch (source_event->subtype) {
        case midi::kEventTypeSetTempo:
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
      memcpy(&current_event.data,&source_event->data,sizeof(current_event.data));
      //if (track_index == 1) {
      //  sprintf(str,"time %04.03f type : %s %s track %d channel %d\n",time_ms,eventnames[source_event->type],eventnames[source_event->subtype],source_event->track,source_event->channel);
      //  OutputDebugString(str);
      //}
    }
    
  } 
  //find song length
  for (int i=0;i<track_count;++i) {
    song_length_ms = max(song_length_ms,tracks[i].event_sequence[tracks[i].event_count-1].abs_time_ms);
  }

  /*char str[512];
  for (int track=0;track<midifile.track_count;++track) {
    tracks[track].event_count = midifile.eventList[track].size();
    tracks[track].event_index = 0;
    if (tracks[track].event_count == 0)
      continue;
    
    tracks[track].event_sequence = new midi::Event[tracks[track].event_count];
    uint32_t absTime = 0;
    for (uint32_t eindex=0;eindex<tracks[track].event_count;++eindex) {
      auto& current_event = tracks[track].event_sequence[eindex];
      memset(&current_event,0,sizeof(midi::Event));
      auto ptr = &midifile.eventList[track][eindex];
      absTime += ptr->deltaTime;
      time_ms = secs_per_tick * 1000 * absTime;
      current_event.abs_time_ms = time_ms;
      current_event.deltaTime = ptr->deltaTime;
      current_event.type = ptr->type;
      current_event.subtype = ptr->subtype;
      current_event.channel = ptr->channel;
      current_event.track = ptr->track;
      memcpy(&current_event.data,&ptr->data,sizeof(current_event.data));

      if (ptr->subtype == midi::kEventTypeNoteOn) {
        //current_event.command = 0x90;
        //current_event.data.note.note = (synth::Notes)ptr->data.note.noteNumber;
        //current_event.data.note.frequency = util.NoteFreq((Notes)current_event.data.note.note);
        //current_event.data.note.velocity = ptr->data.note.velocity;
      } else if  (ptr->subtype == midi::kEventTypeNoteOff) {
        //current_event.command = 0x80;
        //current_event.data.note.note = (synth::Notes)ptr->data.note.noteNumber;
        //current_event.data.note.frequency = util.NoteFreq((Notes)current_event.data.note.note);
        //current_event.data.note.velocity = ptr->data.note.velocity;
      } else if  (ptr->subtype == midi::kEventTypeEndOfTrack) {
        
      } else if  (ptr->subtype == midi::kEventTypeProgramChange) {
        //set instrument
        //current_event.command = 0xc0;
        //current_event.data.instrument.instrument = ptr->data.program.number;

        //sprintf(str,"time %04.03f type : %d %d track %d channel %d inst %x ",time_ms,ptr->type,ptr->subtype,ptr->track,ptr->channel,ptr->data.program.number);
      //sprintf(str,"%s\n",str);
      //OutputDebugString(str);

      } else if (ptr->subtype == midi::kEventTypeSetTempo) {
        bpm = 60000000.0 / double(ptr->data.tempo.microsecondsPerBeat);
        bps = bpm / 60.0;
        secs_per_tick = 1 / ( bps * ticks_per_beat );
        //sprintf(str,"time %04.03f type : %s %s track %d channel %d - %f\n",time_ms,eventnames[ptr->type],eventnames[ptr->subtype],ptr->track,ptr->channel,bpm);
        //OutputDebugString(str);
      } else if (ptr->subtype == midi::kEventTypeTimeSignature) {
        //sprintf(str,"time %04.03f time sig %d %d %d %d\n",time_ms,ptr->data.time_signature.numerator,ptr->data.time_signature.denominator,ptr->data.time_signature.metronome,ptr->data.time_signature.thirtyseconds);
        //OutputDebugString(str);
      } else {
        //if (ptr->isMeta() == 0) {
          //sprintf(str,"time %04.03f track %d - command %x - %x %x\n",time_ms,track,ptr->data[0],ptr->data[1],ptr->data[2]);
         // OutputDebugString(str);
       // }
      }
    
      //sprintf(str,"time %04.03f type : %s %s track %d channel %d\n",time_ms,eventnames[ptr->type],eventnames[ptr->subtype],ptr->track,ptr->channel);
      //OutputDebugString(str);
    }
    song_length_ms = max(song_length_ms,time_ms);
  }*/

  delete [] buffer;
  last_event = GetNextEvent();
}

void Player::Play() {
  PostThreadMessage(thread_id,WM_SP_PLAY,0,0);
}

void Player::Pause() {
  PostThreadMessage(thread_id,WM_SP_PAUSE,0,0);
}

void Player::Stop() {
  PostThreadMessage(thread_id,WM_SP_STOP,0,0);
}

double Player::GetPlaybackSeconds() {
  return song_pos_ms * (1 / 1000.0);
}



DWORD WINAPI Player::PlayThread(LPVOID lpThreadParameter) {
  auto self = (Player*)lpThreadParameter;
  utilities::Timer<double> timer;
  MSG msg;
  PeekMessage(&msg, NULL, WM_USER, WM_USER+10, PM_NOREMOVE);
  SetEvent(self->tc_event);

  uint64_t current_cycles=0,prev_cycles=0;
  double span_accumulator = 0;
  double update_time = 100.0;
  
  

  //temp
  bool apply_delay = true;
  Delay delay_unit(44100*2);
  delay_unit.set_feedback(0.3);
  delay_unit.set_delay_ms(400);
  filters::LowPass lowpass;
  lowpass.set_sample_rate(44100);


 
  
  short* song_buffer = nullptr;

  while (1) {
    if (PeekMessage(&msg, NULL, WM_USER, WM_USER+10, PM_REMOVE)) {
      if (msg.message == WM_SP_QUIT) {
        break;
      } else if (msg.message == WM_SP_PLAY) {
        if (self->state != 1) {
          prev_cycles = timer.GetCurrentCycles();
          self->state = 1;
          self->song_counter_ms = self->song_pos_ms;
          span_accumulator = update_time;
          self->audio_interface_->Play();
        }
      } else if (msg.message == WM_SP_PAUSE) {
        self->state = 2;
        //memset(self->output_buffer,0,44100*2*sizeof(short));
        //self->audio_interface_->Write(self->output_buffer,44100*1*sizeof(short));
      } else if (msg.message == WM_SP_STOP) {
        self->state = 0;
        self->ResetTracks();
        self->song_counter_ms = self->song_pos_ms = 0;
        self->audio_interface_->Stop();
      } 
    } 

    if (self->state == 1) {
      current_cycles = timer.GetCurrentCycles();
      double time_span =  (current_cycles - prev_cycles) * timer.resolution();
      if (time_span >= 250.0) //should at max output buffer length in ms
        time_span = 250.0;
 
      if (span_accumulator >= update_time) {
        uint32_t samples_to_render = uint32_t((double(update_time) * 0.001) * 44100.0);
        uint32_t buffer_size = samples_to_render * 2 * sizeof(short);
        self->RenderSamples(samples_to_render);
        self->audio_interface_->Write(self->output_buffer,buffer_size);
        span_accumulator -= update_time;
       }
      span_accumulator += time_span;
      self->song_pos_ms += time_span;
      prev_cycles = current_cycles;
    } else {
      //wait for pause/stop events with timer
    }
  }
  SetEvent(self->tc_event);
  return S_OK;
}

midi::Event* Player::GetNextEvent() {
  uint32_t track_index=0xFFFFFFFF;
  uint32_t event_index=0xFFFFFFFF;
  uint32_t ticks_to_next_event = 0xFFFFFFFF;
  midi::Event* result = nullptr;

	for (int i = 0; i < track_count; i++) {
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
		for (int i = 0; i < track_count; i++) {
			if (tracks[i].ticks_to_next_event != 0xFFFFFFFF) {
				tracks[i].ticks_to_next_event -= ticks_to_next_event;
			}
		}

	  auto beatsToNextEvent = ticks_to_next_event / double(ticks_per_beat);
		auto secondsToNextEvent = beatsToNextEvent / (bpm / 60);
		samples_to_next_event += uint32_t(secondsToNextEvent * 44100.0);
	} else {
		result = nullptr;
		samples_to_next_event = 0xFFFFFFFF;
    PostThreadMessage(thread_id,WM_SP_STOP,0,0);
	}
  return result;
}

void Player::HandleEvent(midi::Event* event) {
	switch (event->type) {
    case midi::kEventTypeMeta:
			switch (event->subtype) {
        case midi::kEventTypeSetTempo:
          bpm = 60000000.0 / event->data.tempo.microsecondsPerBeat;
        break;
			}
			break;
    case midi::kEventTypeChannel:{
      auto channel = channels[event->channel];
			switch (event->subtype) {
				case midi::kEventTypeNoteOn: {
          double frequency = util.NoteFreq((Notes)event->data.note.noteNumber);
          channel->AddNote(event->data.note.noteNumber,frequency,event->data.note.velocity / 127.0);
					break;
        }
				case midi::kEventTypeNoteOff:
					channel->RemoveNote(event->data.note.noteNumber);
					break;
        case midi::kEventTypeProgramChange:
					channel->set_instrument(instr[event->data.program.number]);
					break;
			}
			break;
    }
	}
};

void Player::RenderSamples(uint32_t samples_count) {
  //uint32_t samples_remaining = samples_count;

  auto data = output_buffer;
	auto data_offset = 0;

  auto generateIntoBuffer = [&](uint32_t samplesToGenerate,short* data,uint32_t dataOffset) {
    double ov_left_sample=0,ov_right_sample=0;
    while (samplesToGenerate) {
      MixChannels(ov_left_sample,ov_right_sample);
      //clip and write to output
      data[dataOffset++] = short(32767.0 * min(ov_left_sample,1.0));
      data[dataOffset++] = short(32767.0 * min(ov_right_sample,1.0));
      --samplesToGenerate;
      song_counter_ms += sample_time_ms;
    }
  };
  
  while (true) {
    
		if (samples_to_next_event != 0xFFFFFFFF && samples_to_next_event <= samples_count) {
			/* generate samplesToNextEvent samples, process event and repeat */
			auto samples_to_generate = samples_to_next_event;
			if (samples_to_generate > 0) {
        generateIntoBuffer(samples_to_generate, data, data_offset);
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
				generateIntoBuffer(samples_count, data, data_offset);
				samples_to_next_event -= samples_count;
			}
			break;
		}
	}
}

void Player::RenderSamples2(uint32_t samples_count) {
  
  double ov_left_sample,ov_right_sample;
  //mul by 2,inc by 2 ( because of 2 output channels)
  for (uint32_t i=0;i<samples_count<<1;i+=2) {
     
    //process midi events
    for (int track_index=0;track_index<track_count;++track_index) {
      auto& track = tracks[track_index];
      if (track.event_count == 0 || track.event_index >= track.event_count)
        continue;
      
      auto cur_event = track.getCurrentEvent();
      while ((track.event_index < track.event_count) &&
             (cur_event->abs_time_ms >= song_counter_ms)  && 
             (cur_event->abs_time_ms < (song_counter_ms+sample_time_ms))) {
        HandleEvent(cur_event);
        ++cur_event;
        ++track.event_index;
      }
    } 
    
    MixChannels(ov_left_sample,ov_right_sample);

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
    output_buffer[i] =   short(32767.0 * min(ov_left_sample,1.0));
    output_buffer[i+1] = short(32767.0 * min(ov_right_sample,1.0));


    song_counter_ms += sample_time_ms;
  }
}

void Player::MixChannels(double& output_left,double& output_right) {
  output_left=output_right=0;
  for (int j=0;j<kChannelCount;++j) { //mix all 16 channels
    double left_sample=0,right_sample=0;
    auto channel = channels[j];
    channel->Tick(left_sample,right_sample);
    output_left += left_sample;
    output_right += right_sample;
  }
}

}
}