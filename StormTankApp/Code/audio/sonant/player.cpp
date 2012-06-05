#include "sonant.h"
//#include "../synth.h"
//#include "../lfo/lfo.h"
/*
  short* output_buffer;
  uint32_t offset;
  uint32_t size;
  uint32_t sample_count;
  uint32_t channels;
  synth::BasicGenerator track1_synth(44100,2,16);
  LFO lfo(44100);
  double song_time_ms=0;
  int note = (int)synth::Notes::B4;
  HANDLE thread;
  DWORD thread_id;
  */
void sonantInit();
bool sonantRender(song& songdata,int row,int pattern,int rowlen,int endpattern,short* output_buffer,int& ob_len);

#define WM_SP_QUIT 0x400
#define WM_SP_PLAY 0x401
#define WM_SP_PAUSE 0x402
#define WM_SP_STOP 0x403

void SonantPlayer::Initialize() {
  if (initialized_ == true)
    return;
  song_data = new song();
  event1 = CreateEvent(NULL,FALSE,FALSE,NULL);
  thread_handle = CreateThread(nullptr,0,static_cast<LPTHREAD_START_ROUTINE>(PlayThread),this,0,(LPDWORD)&thread_id);//CREATE_SUSPENDED
  WaitForSingleObject(event1,INFINITE);
  initialized_ = true;


  

    /*offset = 0;
    channels = 2;
    sample_count = 44100;
    size = sample_count * channels * sizeof(short);
    output_buffer = new short[sample_count * channels];
    //audio_interface_->Write(audio::output_buffer,audio::size);
    lfo.setWaveform(LFO::triangle);
    track1_synth.SetBeatsPerMinute(120);    
    */
  output_buffer = nullptr;
  row_buffer = nullptr;
}

void SonantPlayer::Deinitialize() {
  if (initialized_ == false)
    return;
  SafeDeleteArray(&row_buffer);
  PostThreadMessage(thread_id,WM_SP_QUIT,0,0);
  WaitForSingleObject(event1,INFINITE);
  CloseHandle(thread_handle);
  CloseHandle(event1);
  SafeDelete(&song_data);
  SafeDeleteArray(&output_buffer);
  initialized_ = false;
}

void SonantPlayer::LoadSong(song* song_data,int rowlen,int endpattern,int* progress_var) {
  memcpy(this->song_data,song_data,sizeof(song));
  this->rowlen = rowlen;
  this->endpattern = endpattern;
  row_buffer = new short[rowlen*2];
  row_time = double(rowlen) / 44100.0;
  current_row = 0;
  current_pattern = 0;
  sonantInit();
  SafeDeleteArray(&output_buffer);
  output_buffer = sonant_init(*song_data,rowlen,endpattern,progress_var);
}

void SonantPlayer::Play() {
  PostThreadMessage(thread_id,WM_SP_PLAY,0,0);
}

void SonantPlayer::Pause() {
  PostThreadMessage(thread_id,WM_SP_PAUSE,0,0);
}

void SonantPlayer::Stop() {
  PostThreadMessage(thread_id,WM_SP_STOP,0,0);
  current_row = 0;
  current_pattern = 0;
}



DWORD WINAPI SonantPlayer::PlayThread(LPVOID lpThreadParameter) {
  auto self = (SonantPlayer*)lpThreadParameter;
  utilities::Timer<double> timer;
  MSG msg;
  PeekMessage(&msg, NULL, WM_USER, WM_USER+10, PM_NOREMOVE);
  SetEvent(self->event1);

  uint64_t current_cycles=0,prev_cycles=0;
  double span_accumulator = 0;
  double time_lapse = 0;
  double dt = 16.66666667;
  double update_time = 50.0;

  short *ob_ptr = nullptr;

  while (1) {
    if (PeekMessage(&msg, NULL, WM_USER, WM_USER+10, PM_REMOVE)) {
      if (msg.message == WM_SP_QUIT) {
        ResetEvent(self->event1);
        break;
      } else if (msg.message == WM_SP_PLAY) {
        prev_cycles = timer.GetCurrentCycles();
        self->state = 1;
        ob_ptr = self->output_buffer;
      } else if (msg.message == WM_SP_PAUSE) {
        self->state = 2;
      } else if (msg.message == WM_SP_STOP) {
        self->state = 0;
      } 
    } 

    if (self->state == 1) {
      current_cycles = timer.GetCurrentCycles();
      double time_span =  (current_cycles - prev_cycles) * timer.resolution();
      if (time_span >= 250.0)
        time_span = 250.0;
      span_accumulator += time_span;
      while (span_accumulator >= dt) {
        span_accumulator -= dt;
        uint32_t buffer_size = (double(update_time) / 1000.0) * 44100 * 2 * 2;
        if (time_lapse >= update_time) {//self->row_time) {
          /*
          auto samples = track1_synth.SamplesPerTimeMS(time_lapse);
          double freq = track1_synth.NoteFreq((synth::Notes)note);
          lfo.setRate(freq);
          offset = 0;
          for (uint32_t i=0;i<samples;++i) {
            auto sample = short(lfo.tick() * 32767.0f);
            output_buffer[offset++] = sample;
            output_buffer[offset++] = sample;
          }
          //audio::song_time_ms += time_lapse;
          //if ((uint32_t(audio::song_time_ms) % 1000) == 0) {
          //  audio::note++;
          // }
          */



          /*int size = self->rowlen;
          sonantRender(*self->song_data,self->current_row,self->current_pattern,self->rowlen,self->endpattern,self->row_buffer,size);
          self->audio_interface_->Write(self->row_buffer,size*2*sizeof(short));
       
          self->current_row = (self->current_row + 1);
          if (self->current_row == 32) {
            self->current_row = 0;
            self->current_pattern = 0;
          }*/

          self->audio_interface_->Write(ob_ptr,buffer_size);
          ob_ptr += buffer_size>>1;

          time_lapse = 0;
        }
        time_lapse += dt;

      }
      prev_cycles = current_cycles;
    } else {
      Sleep(20);
    }
  
  }
 
  SetEvent(self->event1);
  return S_OK;
}