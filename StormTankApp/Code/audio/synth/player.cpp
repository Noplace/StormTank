/*****************************************************************************************************************
* Copyright (c) 2012 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/
#include "player.h"


#define WM_SP_QUIT 0x400
#define WM_SP_PLAY 0x401
#define WM_SP_PAUSE 0x402
#define WM_SP_STOP 0x403

#define SQRT_1OVER2  0.70710678118654752440084436210485

namespace audio {
namespace synth {

static void clamp(void *dest, const float *src, int32_t nsamples)
{
  int16_t *dests = (int16_t *)dest;

  for (int32_t i=0; i < nsamples; i++)
  {
    float v = src[i] * 32768.0f;
    if (v >  32600.0f) v =  32600.0f;
    if (v < -32600.0f) v = -32600.0f;
    dests[i] = (int16_t)v;
  }
}




void Player::Initialize() {
  if (initialized_ == true)
    return;

  state_ = kStateStopped;
  song_counter_ms = 0;
  auto sample_rate = audio_interface_->wave_format().nSamplesPerSec;

  {
    output_buffer_samples_ = audio_interface_->buffer_size()/sizeof(short); //size in samples
    output_buffer_length_ms_ = output_buffer_samples_ / double(sample_rate*audio_interface_->wave_format().nChannels*0.001); //400.0;//400ms
    output_buffer = new short[output_buffer_samples_]; 
    mix_buffer = new real_t[output_buffer_samples_]; 
  }

  synth_->set_sample_rate(sample_rate);
  synth_->Initialize();

  thread_msg = 0;
  InitializeCriticalSection(&vis_cs);
  InitializeCriticalSectionAndSpinCount(&cs,0x100);
  player_event = CreateEvent(NULL,FALSE,FALSE,NULL);
  thread_handle = CreateThread(nullptr,0,static_cast<LPTHREAD_START_ROUTINE>(PlayThread),this,0,(LPDWORD)&thread_id);//CREATE_SUSPENDED
  if (thread_handle != nullptr) {
    SetThreadPriority(thread_handle,THREAD_PRIORITY_ABOVE_NORMAL);
  }

  initialized_ = true;
}

void Player::Deinitialize() {
  if (initialized_ == false)
    return;
  Stop();
  //force close
  SetEvent(player_event);
  SendThreadMessage(WM_SP_QUIT);
  WaitForSingleObject(thread_handle,INFINITE);
  CloseHandle(thread_handle);
  CloseHandle(player_event);
  DeleteCriticalSection(&cs);
  DeleteCriticalSection(&vis_cs);
  synth_->Deinitialize();
  SafeDeleteArray(&mix_buffer);
  SafeDeleteArray(&output_buffer);


  initialized_ = false;
}

void Player::Play() {
  if (state_ == kStatePlaying) return;
  SetEvent(player_event);
  SendThreadMessage(WM_SP_PLAY);
}

void Player::Pause() {
  if (state_ == kStatePaused) return;
  SendThreadMessage(WM_SP_PAUSE);
}

void Player::Stop() {
  if (state_ == kStateStopped) return;
  SendThreadMessage(WM_SP_STOP);
}

double Player::GetPlaybackSeconds() {
  return song_counter_ms * (1 / 1000.0);
}



DWORD WINAPI Player::PlayThread(LPVOID lpThreadParameter) {
  return static_cast<Player*>(lpThreadParameter)->InstancePlayThread();
}

DWORD Player::InstancePlayThread() {
  utilities::Timer timer;

  uint64_t current_cycles=0,prev_cycles=0;
  double span_accumulator = 0;
  double update_time = 20.0;
  double timer_res = timer.resolution();
  uint32_t samples_to_render = uint32_t(update_time * 0.001 * audio_interface_->wave_format().nSamplesPerSec);
  uint32_t buffer_size = samples_to_render * 2 * sizeof(short);

  thread_time_span = 0;
  while (1) {
    EnterCriticalSection(&cs);
    if (thread_msg == WM_SP_QUIT) {
        thread_msg = 0;
        break;
    }
    if (thread_msg == WM_SP_PLAY) {
      if (state_ != kStatePlaying) {
        prev_cycles = timer.GetCurrentCycles();
        state_ = kStatePlaying;
        span_accumulator = update_time;
        audio_interface_->Play();
      }
    } else if (thread_msg == WM_SP_PAUSE) {
      if (state_ != kStatePaused) {
        ResetEvent(player_event);
        state_ = kStatePaused;
        
        memset(output_buffer,0,output_buffer_samples_*sizeof(short));
        audio_interface_->Write(output_buffer,output_buffer_samples_*sizeof(short));
      }
    } else if (thread_msg == WM_SP_STOP) {
      if (state_ != kStateStopped) {
        ResetEvent(player_event);
        state_ = kStateStopped;
        synth_->Reset();
        song_counter_ms = 0;
        audio_interface_->Stop();
        thread_time_span = 0;
      }
    } 
    thread_msg = 0;
    LeaveCriticalSection(&cs);

    if (state_ == kStatePlaying) {
      current_cycles = timer.GetCurrentCycles();
      double time_span =  (current_cycles - prev_cycles) * timer_res;
      if (time_span >= output_buffer_length_ms_) //should be at max, output buffer length in ms
        time_span = output_buffer_length_ms_;
 
      while (span_accumulator >= update_time) {
        //memset(mix_buffer,0,output_buffer_samples_*sizeof(real_t));
        synth_->RenderSamplesStereo(samples_to_render,mix_buffer);
        song_counter_ms += update_time;
        if (TryEnterCriticalSection(&vis_cs) != 0) {
          if (visual_addon_ != nullptr && samples_to_render >= 256)
            visual_addon_->AddPCMData256(mix_buffer,2,update_time);
          LeaveCriticalSection(&vis_cs);
        }
        clamp(output_buffer,mix_buffer,samples_to_render<<1);

        audio_interface_->Write(output_buffer,buffer_size);
        span_accumulator -= update_time;
      }
      span_accumulator += time_span;
      thread_time_span += time_span;
      prev_cycles = current_cycles;
    } else {
      //wait for pause/stop events with timer
      WaitForSingleObject(player_event,2000);
    }
  }

  return S_OK;
}


}
}