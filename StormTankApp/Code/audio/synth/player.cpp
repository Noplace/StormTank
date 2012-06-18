#include "synth.h"

#define WM_SP_QUIT 0x400
#define WM_SP_PLAY 0x401
#define WM_SP_PAUSE 0x402
#define WM_SP_STOP 0x403

#define SQRT_1OVER2  0.70710678118654752440084436210485

namespace audio {
namespace synth {


void Player::Initialize() {
  if (initialized_ == true)
    return;

  state = 0;
  song_pos_ms = song_counter_ms = 0;
  
  memset(&tracks,0,sizeof(tracks));

  auto sample_rate = audio_interface_->wave_format().nSamplesPerSec;
  util.set_sample_rate(sample_rate);
  sample_time_ms = (1000.0/double(sample_rate));

  output_buffer = new short[44100*2]; // 1 second output buffer

  //initialize all available instruments
  memset(instr,0,sizeof(instr));
  instr[0] = new instruments::OscWave(instruments::OscWave::Sine);
  instr[2] = new instruments::OscWave(instruments::OscWave::Triangle);
  
  instr[81] = new instruments::OscWave(instruments::OscWave::Square);
  instr[82] = new instruments::OscWave(instruments::OscWave::Sawtooth);

  for (int i=0;i<128;++i) {
    if (instr[i] == nullptr)
      instr[i] = new instruments::OscWave(instruments::OscWave::Sine);

    instr[i]->set_sample_rate(sample_rate);
  }

  for (int i=0;i<16;++i) {
    channels[i] = new Channel();
    channels[i]->set_instrument(instr[0]);
    channels[i]->set_sample_rate(sample_rate);
    channels[i]->set_silence(false);
    channels[i]->set_panning(0.5);
    channels[i]->set_amplification(0.4);
  }
  
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
  for (int i=0;i<16;++i) {
    SafeDelete(&channels[i]);
  }
  for (int i=0;i<128;++i) {
    SafeDelete(&instr[i]);
  }

  SafeDeleteArray(&output_buffer);
  for (int i=0;i<16;++i) {
    SafeDeleteArray(&tracks[i].event_sequence);
  }
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

  auto ticks_per_qn = midifile.ticksPerBeat;
  double bpm = 120; //qn per min
  double bps = bpm / 60.0;
  double secs_per_tick;
  double time_ms;
  secs_per_tick = 1 / ( bps * ticks_per_qn );
  song_length_ms = 0;   

  char str[512];
  for (int track=0;track<16;++track) {
    tracks[track].event_count = midifile.eventList[track].size();
    tracks[track].event_index = 0;
    if (tracks[track].event_count == 0)
      continue;
    SafeDeleteArray(&tracks[track].event_sequence);
    tracks[track].event_sequence = new Event[tracks[track].event_count];
    uint32_t absTime = 0;
    for (uint32_t eindex=0;eindex<tracks[track].event_count;++eindex) {
      auto& current_event = tracks[track].event_sequence[eindex];
      memset(&current_event,0,sizeof(Event));
      auto ptr = &midifile.eventList[track][eindex];
      absTime += ptr->deltaTime;
      time_ms = secs_per_tick * 1000 * absTime;
      current_event.channel = ptr->channel;
      current_event.pos_ms = time_ms;

      if (ptr->subtype == midi::kEventTypeNoteOn) {
        current_event.command = 0x90;
        current_event.data.note.note = (synth::Notes)ptr->noteNumber;
        current_event.data.note.frequency = util.NoteFreq((Notes)current_event.data.note.note);
        current_event.data.note.velocity = ptr->velocity;
      } else if  (ptr->subtype == midi::kEventTypeNoteOff) {
        current_event.command = 0x80;
        current_event.data.note.note = (synth::Notes)ptr->noteNumber;
        current_event.data.note.frequency = util.NoteFreq((Notes)current_event.data.note.note);
        current_event.data.note.velocity = ptr->velocity;
      } else if  (ptr->subtype == midi::kEventTypeEndOfTrack) {
        
      } else if  (ptr->subtype == midi::kEventTypeProgramChange) {
        //set instrument
        current_event.command = 0xc0;
        current_event.data.instrument.instrument = ptr->programNumber;
      } else if (ptr->subtype == midi::kEventTypeSetTempo) {
        bpm = 60000000.0 / double(ptr->microsecondsPerBeat);
        bps = bpm / 60.0;
        secs_per_tick = 1 / ( bps * ticks_per_qn );
      } else {
        //if (ptr->isMeta() == 0) {
          //sprintf(str,"time %04.03f track %d - command %x - %x %x\n",time_ms,track,ptr->data[0],ptr->data[1],ptr->data[2]);
         // OutputDebugString(str);
       // }
      }
    
      sprintf(str,"time %04.03f type : %d %d track %d channel %d ",time_ms,ptr->type,ptr->subtype,ptr->track,ptr->channel);
      sprintf(str,"%s\n",str);
      OutputDebugString(str);
    }
    song_length_ms = max(song_length_ms,time_ms);
  }
  

  delete [] buffer;
}

void Player::Play() {
  PostThreadMessage(thread_id,WM_SP_PLAY,0,0);
}

void Player::Pause() {
  PostThreadMessage(thread_id,WM_SP_PAUSE,0,0);
}

void Player::Stop() {
  PostThreadMessage(thread_id,WM_SP_STOP,0,0);
  song_pos_ms = 0;
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
        }
      } else if (msg.message == WM_SP_PAUSE) {
        self->state = 2;
      } else if (msg.message == WM_SP_STOP) {
        self->state = 0;
        self->song_pos_ms = 0;
      } 
    } 

    if (self->state == 1) {
      current_cycles = timer.GetCurrentCycles();
      double time_span =  (current_cycles - prev_cycles) * timer.resolution();
      if (time_span >= 250.0)
        time_span = 250.0;
 
      if (span_accumulator >= update_time) {
        uint32_t samples_to_render = uint32_t((double(span_accumulator) * 0.001) * 44100.0);
        uint32_t buffer_size = samples_to_render * 2 * sizeof(short);

        self->RenderSamples(samples_to_render);
        self->audio_interface_->Write(self->output_buffer,buffer_size);
        if (self->song_counter_ms >= self->song_length_ms) {
          self->song_counter_ms = self->song_pos_ms = 0;

          for (int i=0;i<16;++i)
            self->tracks[i].event_index = 0;
        }
        span_accumulator -= update_time;
        Sleep(5);
      }
      span_accumulator += time_span;
      self->song_pos_ms += time_span;
      prev_cycles = current_cycles;
    }
  }
  SetEvent(self->tc_event);
  return S_OK;
}

void Player::RenderSamples(uint32_t samples_count) {
  
  double ov_left_sample,ov_right_sample;
  //mul by 2,inc by 2 ( because of 2 output channels)
  for (uint32_t i=0;i<samples_count<<1;i+=2) {
     
    //process midi events
    for (int track_index=0;track_index<16;++track_index) {
      auto& track = tracks[track_index];
      if (track.event_count == 0 || track.event_index >= track.event_count)
        continue;
      
      auto cur_event = track.getCurrentEvent();
      while ((track.event_index < track.event_count) &&
             (cur_event->pos_ms >= song_counter_ms)  && 
             (cur_event->pos_ms < (song_counter_ms+sample_time_ms))) {
        auto channel = channels[cur_event->channel];
        switch (cur_event->command) {
          case 0x90:
            channel->AddNote(cur_event->data.note.note,cur_event->data.note.frequency,cur_event->data.note.velocity / 127.0);
            break;
          case 0x80:
            channel->RemoveNote(cur_event->data.note.note);
            break;
          case 0xC0:
            channel->set_instrument(instr[cur_event->data.instrument.instrument]);
          break;
        }

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
  for (int j=0;j<16;++j) { //mix all 16 channels
    double left_sample=0,right_sample=0;
    auto channel = channels[j];
    channel->Tick(left_sample,right_sample);
    output_left += left_sample;
    output_right += right_sample;
  }
}

}
}