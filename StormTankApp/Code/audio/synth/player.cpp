#include "player.h"

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
  tc_event = CreateEvent(NULL,FALSE,FALSE,NULL);
  thread_handle = CreateThread(nullptr,0,static_cast<LPTHREAD_START_ROUTINE>(PlayThread),this,0,(LPDWORD)&thread_id);//CREATE_SUSPENDED
  SetThreadPriority(thread_handle,THREAD_PRIORITY_ABOVE_NORMAL);
  WaitForSingleObject(tc_event,INFINITE);
  initialized_ = true;
  state = 0;
  song_time = 0;

  
  synth.osc1.setSampleRate(audio_interface_->wave_format().nSamplesPerSec);
  synth.osc1.setWaveform(LFO::sinus);

  synth.osc1.setRate(440);
}

void Player::Deinitialize() {
  if (initialized_ == false)
    return;
  PostThreadMessage(thread_id,WM_SP_QUIT,0,0);
  WaitForSingleObject(tc_event,INFINITE);
  CloseHandle(thread_handle);
  CloseHandle(tc_event);
  initialized_ = false;
}

void Player::Play() {
  PostThreadMessage(thread_id,WM_SP_PLAY,0,0);
}

void Player::Pause() {
  PostThreadMessage(thread_id,WM_SP_PAUSE,0,0);
}

void Player::Stop() {
  PostThreadMessage(thread_id,WM_SP_STOP,0,0);
  song_time = 0;
}

double Player::GetPlaybackSeconds() {
  return song_time * (1 / 1000.0);
}

DWORD WINAPI Player::PlayThread(LPVOID lpThreadParameter) {
  auto self = (Player*)lpThreadParameter;
  utilities::Timer<double> timer;
  MSG msg;
  PeekMessage(&msg, NULL, WM_USER, WM_USER+10, PM_NOREMOVE);
  SetEvent(self->tc_event);

  short *ob_ptr = nullptr;
  uint64_t current_cycles=0,prev_cycles=0;
  double span_accumulator = 0;
  double time_lapse = 0;
  double dt = 16.66666667;
  double update_time = 50.0;
  int32_t samples_rendered;
  int32_t sample_buffer_size = 0.050 * 44100;
  int32_t current_buffer_playing_position = 0;

  while (1) {
    if (PeekMessage(&msg, NULL, WM_USER, WM_USER+10, PM_REMOVE)) {
      if (msg.message == WM_SP_QUIT) {
        ResetEvent(self->tc_event);
        break;
      } else if (msg.message == WM_SP_PLAY) {
        if (self->state != 1) {
          prev_cycles = timer.GetCurrentCycles();
          self->state = 1;
          ob_ptr = 0;
          samples_rendered = 0;
          current_buffer_playing_position = -sample_buffer_size;
        }
      } else if (msg.message == WM_SP_PAUSE) {
        self->state = 2;
      } else if (msg.message == WM_SP_STOP) {
        samples_rendered = 0;
        self->state = 0;
      } 
    } 

    if (self->state == 1) {
      current_cycles = timer.GetCurrentCycles();
      double time_span =  (current_cycles - prev_cycles) * timer.resolution();
      if (time_span >= 250.0)
        time_span = 250.0;
        if (span_accumulator >= update_time) {
          uint32_t samples_to_render = (double(span_accumulator) / 1000.0) * 44100;
          uint32_t buffer_size = samples_to_render * 2 * sizeof(short);

          auto buffer = new short[samples_to_render*2];
          for (int i=0;i<samples_to_render*2;i+=2) {
            float left_sample,right_sample;
            float pan = 0.5;
            left_sample = (1.0-sqrtf(pan))*self->synth.osc1.tick();
            right_sample = (sqrtf(pan))*left_sample;
            buffer[i] = short(32767.0 * left_sample);
            buffer[i+1] = short(32767.0 * right_sample);
          }

          self->audio_interface_->Write(buffer,buffer_size);
          delete [] buffer;
          samples_rendered += samples_to_render;

          //ob_ptr += buffer_size>>1;
          
          time_lapse = 0;
          span_accumulator = 0;
          Sleep(10);
        }
        time_lapse += dt;
        span_accumulator += time_span;
        self->song_time += time_span;
        prev_cycles = current_cycles;
      }
      
  }
 
  SetEvent(self->tc_event);
  return S_OK;
}

}
}