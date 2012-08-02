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

#define SQRT_1OVER2  0.70710678118654752440084436210485


namespace audio {
namespace synth {

void Player::Initialize() {
  if (initialized_ == true)
    return;

  state_ = kStateStopped;
  song_counter_ms = 0;
  auto sample_rate = audio_interface_->wave_format().nSamplesPerSec;
  //sample_rate_ = double(sample_rate);

  //output_buffer_length_ms_ = 400.0;//400ms
  {
    output_buffer_size_ = audio_interface_->buffer_size();
    //auto size = uint32_t(sample_rate*audio_interface_->wave_format().nChannels*output_buffer_length_ms_*0.001);
    output_buffer = new real_t[output_buffer_size_]; // 1 second output buffer
  }
  synth_->set_sample_rate(sample_rate);
  synth_->Initialize();

  InitializeCriticalSection(&vis_cs);
  InitializeCriticalSectionAndSpinCount(&cs,0x100);
  tick_event = CreateEvent(NULL,FALSE,FALSE,NULL);
  exit_event = CreateEvent(NULL,FALSE,FALSE,NULL);
  initialized_ = true;
}

void Player::Deinitialize() {
  if (initialized_ == false)
    return;
  SetEvent(exit_event);
  WaitForSingleObject(thread_handle,INFINITE);
  if (thread_handle != nullptr) {
    CloseHandle(thread_handle);
    thread_handle  = nullptr;
  }

  CloseHandle(exit_event);
  CloseHandle(tick_event);
  DeleteCriticalSection(&cs);
  DeleteCriticalSection(&vis_cs);
  synth_->Deinitialize();
  SafeDeleteArray(&output_buffer);
  initialized_ = false;
}

void Player::Play() {
  if (state_ == kStatePlaying) return;
  state_ = kStatePlaying;


  WaitForSingleObject(thread_handle,INFINITE);
  if (thread_handle != nullptr) {
    CloseHandle(thread_handle);
    thread_handle  = nullptr;
  }
  ResetEvent(exit_event);
  audio_interface_->Play();
  thread_handle = CreateThread(nullptr,0,static_cast<LPTHREAD_START_ROUTINE>(PlayThread),this,0,(LPDWORD)&thread_id);//CREATE_SUSPENDED
  if (thread_handle != nullptr) {
    SetThreadPriority(thread_handle,THREAD_PRIORITY_ABOVE_NORMAL);
  }
}

void Player::Pause() {
  if (state_ == kStatePaused) return;
  state_ = kStatePaused;
  SetEvent(exit_event);
  audio_interface_->Stop();
}

void Player::Stop() {
  if (state_ == kStateStopped) return;
  state_ = kStateStopped;
  SetEvent(exit_event);
  audio_interface_->Stop();
  Reset();
}

double Player::GetPlaybackSeconds() {
  return song_counter_ms * (1 / 1000.0);
}

DWORD WINAPI Player::PlayThread(LPVOID lpThreadParameter) {
  return static_cast<Player*>(lpThreadParameter)->InstancePlayThread();
}

DWORD Player::InstancePlayThread() {
  uint32_t samples_to_render = 0;
  HANDLE handles[2] = { tick_event, exit_event };
  
  while ((WaitForMultipleObjects(2, handles, FALSE, output_buffer_size_/3000) != WAIT_OBJECT_0 + 1)&&request_stop_==false) {
    EnterCriticalSection(&cs);
    if (audio_interface_->BeginWrite(samples_to_render)==S_OK) {
      synth_->RenderSamplesStereo(samples_to_render,output_buffer);
      audio_interface_->EndWrite(output_buffer);
      double time = (samples_to_render / 44100.0) * 1000.0;
      if (this->visual_addon_ != nullptr)
        this->visual_addon_->AddPCMData256(output_buffer,2,time);
      song_counter_ms += time;
    }
    LeaveCriticalSection(&cs);
  }

  if (request_stop_ == true) {
    Stop();
    request_stop_ = false;
  }

  return S_OK;
}


}
}