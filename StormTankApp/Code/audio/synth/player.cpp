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
  song_pos_ms = 0;
  event_count = 0;
  event_sequence = nullptr;
  auto sample_rate = audio_interface_->wave_format().nSamplesPerSec;
  util.set_sample_rate(sample_rate);
  
  for (int i=0;i<16;++i) {
    sine_instr[i] = new instruments::SineWave();
    channels[i] = new Channel();
    channels[i]->set_instrument(sine_instr[i]);
    channels[i]->set_sample_rate(sample_rate);
    channels[i]->set_silence(false);
    channels[i]->set_panning(0.5);
    channels[i]->set_amplification(0.4);
  }
  


  //synth.osc1.setWaveform(LFO::sinus);
  //synth.osc1.setRate(440);
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
    delete channels[i];
    delete sine_instr[i];
  }
  SafeDeleteArray(&event_sequence);
  initialized_ = false;
}

void Player::LoadMidi(MidiFile* midi) {
  if (state == 1)
    return;

  event_count = midi->getEventCount(0);
  auto ticks_per_qn = midi->getTicksPerQuarterNote();
  double bpm = 120; //qn per min
  double bps = bpm / 60.0;
  double secs_per_tick;
  secs_per_tick = 1 / ( bps * ticks_per_qn );
  
  //bps = bpm / 60
  // bps  bpqn
  //seconds per tick

  SafeDeleteArray(&event_sequence);
  event_sequence = new Event[event_count];
  char str[256];
  for (uint32_t eindex=0;eindex<event_count;++eindex) {
    memset(&event_sequence[eindex],0,sizeof(Event));
    auto ptr = &midi->getEvent(0,eindex);
    int track = ptr->track;
    int time = ptr->time;
    double time_ms = secs_per_tick * 1000 * time;
    event_sequence[eindex].channel = track-1;
    event_sequence[eindex].pos_ms = time_ms;

    if (ptr->isNoteOn()) {
      auto note = (synth::Notes)ptr->data[1];
      double freq = util.NoteFreq(note);
      auto vel = ptr->data[2];
      event_sequence[eindex].command = 0x90;
      event_sequence[eindex].note = note;

   
      //sprintf(str,"time %04.03f track %d - note on %d\n",time_ms,track,ptr->data[1]);
      //OutputDebugString(str);
    } else if  (ptr->isNoteOff()) {
      auto note = (synth::Notes)ptr->data[1];
      event_sequence[eindex].command = 0x80;
      event_sequence[eindex].note = note;
      
      //sprintf(str,"time %04.03f track %d - note off %d\n",time_ms,track,ptr->data[1]);
      //OutputDebugString(str);
    } else if  (ptr->isTimbre()) {
      char instrument = ptr->data[1];
      //set instrument
    } else if (ptr->isTempo()) {
      char c = ptr->data[0];
      bpm = ptr->getTempoBPM();
      bps = bpm / 60.0;
      secs_per_tick = 1 / ( bps * ticks_per_qn );
      int a = 0;
    } else {
      if (ptr->isMeta() == 0) {
        sprintf(str,"time %04.03f track %d - command %x - %x %x\n",time_ms,track,ptr->data[0],ptr->data[1],ptr->data[2]);
        OutputDebugString(str);
      }
    }

  }

  song_length_ms = event_sequence[event_count-1].pos_ms;
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
  double update_time = 50.0;
  double song_counter_ms = 0;
  int32_t sample_buffer_size = int32_t(update_time * 0.001 * 44100);


  auto output_buffer = new short[44100*2];

  //temp
  bool apply_delay = true;
  Delay delay_unit(44100*2);
  delay_unit.set_feedback(0.3);
  delay_unit.set_delay_ms(400);




  while (1) {
    if (PeekMessage(&msg, NULL, WM_USER, WM_USER+10, PM_REMOVE)) {
      if (msg.message == WM_SP_QUIT) {
        break;
      } else if (msg.message == WM_SP_PLAY) {
        if (self->state != 1) {
          prev_cycles = timer.GetCurrentCycles();
          self->state = 1;
          song_counter_ms = self->song_pos_ms;
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

        const double sample_time_ms = (1000.0/44100.0);
        for (uint32_t i=0;i<samples_to_render<<1;i+=2) {
          
          //process events
          auto cur_event = &self->event_sequence[self->event_index];
          while (cur_event->pos_ms >= song_counter_ms && cur_event->pos_ms < (song_counter_ms+sample_time_ms)) {

            auto channel = self->channels[cur_event->channel];
            
            if (cur_event->command == 0x90) {
              channel->instrument()->add_note(cur_event->note,self->util.NoteFreq(cur_event->note),1);
            }
            if (cur_event->command == 0x80) {
              channel->instrument()->remove_note(cur_event->note);
            }

            if (self->event_index < self->event_count ) {
              ++cur_event;
              ++self->event_index;
            }
          }
          

          double ov_left_sample,ov_right_sample;
          self->MixChannels(ov_left_sample,ov_right_sample);

          //global effects
          if (apply_delay) {
            //delay_unit.Process(ov_left_sample,ov_right_sample,ov_left_sample,ov_right_sample);
          }
          
          //clip and write to output
          output_buffer[i] =   short(32767.0 * min(ov_left_sample,1.0));
          output_buffer[i+1] = short(32767.0 * min(ov_right_sample,1.0));


          song_counter_ms += sample_time_ms;
        }

        self->audio_interface_->Write(output_buffer,buffer_size);
        if (song_counter_ms >= self->song_length_ms) {
          song_counter_ms = self->song_pos_ms = 0;
          self->event_index = 0;
        }
        span_accumulator = 0;
        Sleep(5);
      }
      span_accumulator += time_span;
      self->song_pos_ms += time_span;
      prev_cycles = current_cycles;
    }
  }
  delete [] output_buffer; 
  SetEvent(self->tc_event);
  return S_OK;
}

void Player::MixChannels(double& output_left,double& output_right) {
  output_left=output_right=0;
  for (int j=0;j<1;++j) { //mix all 16 channels
    double left_sample=0,right_sample=0;
    auto channel = channels[j];
    channel->Tick(left_sample,right_sample);
    output_left += left_sample;
    output_right += right_sample;
  }
}

}
}