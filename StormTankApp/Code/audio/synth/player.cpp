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
  Stop();

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
  SetEvent(tick_event);
  WaitForSingleObject(thread_handle,INFINITE);
  CloseHandle(thread_handle);
  audio_interface_->Stop();
}

void Player::Stop() {
  if (state_ == kStateStopped) return;
  state_ = kStateStopped;
  SetEvent(exit_event);
  SetEvent(tick_event);
  WaitForSingleObject(thread_handle,INFINITE);
  CloseHandle(thread_handle);

  synth_->Reset();
  song_counter_ms = 0;
  audio_interface_->Stop();
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
  
  while (WaitForMultipleObjects(2, handles, FALSE, output_buffer_size_/3000) != WAIT_OBJECT_0 + 1) {
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

  return S_OK;
}


}
}