#include "directsound.h"

#pragma comment(lib, "dsound.lib")

namespace audio {

DirectSound::DirectSound() : window_handle_(nullptr), buffer_size_(0),last_write_cursor(0) {

}

DirectSound::~DirectSound() {

}
  
int DirectSound::Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits) {
  last_write_cursor=0xffffffff;
  HRESULT hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback,&ds8,nullptr);
  if (FAILED(hr)) return S_FALSE;
  hr = ds8->SetCooperativeLevel((HWND)window_handle_,DSSCL_PRIORITY);
  if (FAILED(hr)) {
    hr = ds8->SetCooperativeLevel((HWND)window_handle_,DSSCL_NORMAL);  
    if (FAILED(hr)) {
      return S_FALSE;
    }
  }

  DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd,sizeof(DSBUFFERDESC));
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;


	hr = ds8->CreateSoundBuffer(&dsbd,&primary_buffer,nullptr) ;
  if (FAILED(hr)) return S_FALSE;

	ZeroMemory( &wave_format_, sizeof(WAVEFORMATEX) ); 
	wave_format_.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wave_format_.nChannels       =  channels; 
	wave_format_.nSamplesPerSec  = sample_rate; 
	wave_format_.wBitsPerSample  = (WORD) bits; 
	wave_format_.nBlockAlign     = (WORD) ((wave_format_.wBitsPerSample >> 3) * wave_format_.nChannels);
	wave_format_.nAvgBytesPerSec = (DWORD) (wave_format_.nSamplesPerSec * wave_format_.nBlockAlign);
  wave_format_.cbSize = 0;
	hr = primary_buffer->SetFormat(&wave_format_);
  if (FAILED(hr)) return S_FALSE;


  if (buffer_size_ == 0)
    buffer_size_ = uint32_t(wave_format_.nBlockAlign * wave_format_.nSamplesPerSec * 0.4); //400ms


	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       =   DSBCAPS_LOCSOFTWARE | DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
	dsbd.dwBufferBytes = buffer_size_;
	dsbd.lpwfxFormat   = &wave_format_;
  // Create a temporary sound buffer with the specific buffer settings.
	hr = ds8->CreateSoundBuffer(&dsbd, &secondary_buffer, NULL);
  if (FAILED(hr)) return S_FALSE;

    //hr = secondary_buffer->SetVolume(DSBVOLUME_MAX);
  //if (FAILED(hr)) return S_FALSE;

  return Play();
}

int DirectSound::Deinitialize() {
  Stop();
  SafeRelease(&secondary_buffer);
  SafeRelease(&primary_buffer);
  SafeRelease(&ds8);
  return S_OK;
}

int DirectSound::Play() {

  auto hr = primary_buffer->Play(0,0,DSBPLAY_LOOPING);
  if (FAILED(hr)) return S_FALSE;
  hr = secondary_buffer->Play(0,0,DSBPLAY_LOOPING);
  if (FAILED(hr)) return S_FALSE;
  hr = secondary_buffer->SetCurrentPosition(0);
  return hr;
}

int DirectSound::Stop() {
  auto hr = secondary_buffer->Stop();
  if (FAILED(hr)) return S_FALSE;
  hr = primary_buffer->Stop();
  if (FAILED(hr)) return S_FALSE;
  return hr;
}

uint32_t DirectSound::GetBytesBuffered() {
 unsigned long cplay,cwrite;

 if(last_write_cursor==0xffffffff) return 0;

 secondary_buffer->GetCurrentPosition(&cplay,&cwrite);

 if(cplay>buffer_size_) return buffer_size_;

 if(cplay<last_write_cursor) return last_write_cursor-cplay;
 return (buffer_size_-cplay)+last_write_cursor;
}

int DirectSound::Write(void* data_pointer, uint32_t size_bytes) {
  HRESULT hr;
  DWORD buffer_status;
  hr = secondary_buffer->GetStatus(&buffer_status);    
  if (FAILED(hr)) return S_FALSE;
  if( buffer_status & DSBSTATUS_BUFFERLOST ) {
    hr = secondary_buffer->Restore();
    if (hr != DS_OK) return S_FALSE;
    secondary_buffer->Play(0,0,DSBPLAY_LOOPING);
  }
  DWORD play_cursor,write_cursor;
  secondary_buffer->GetCurrentPosition(&play_cursor,&write_cursor);
  
  if(last_write_cursor==0xffffffff) 
    last_write_cursor=write_cursor;

  LPVOID buf_ptr1, buf_ptr2;
  DWORD buf_size1,buf_size2; 
  hr = secondary_buffer->Lock(write_cursor,size_bytes,&buf_ptr1,&buf_size1,&buf_ptr2,&buf_size2,0);
  if (hr!=DS_OK) { 
    last_write_cursor=0xffffffff;
    return S_FALSE; 
  }

  auto lpSD=(uint8_t*)buf_ptr1;
  auto dw=buf_size1;

  auto lpSS=(uint8_t*)data_pointer;
  while(dw) {*lpSD++=*lpSS++;dw--;}

  if(buf_ptr2)  {
    lpSD=(uint8_t*)buf_ptr2;
    dw=buf_size2;
    while(dw) {*lpSD++=*lpSS++;dw--;}
  }

  secondary_buffer->Unlock(buf_ptr1,buf_size1,buf_ptr2,buf_size2);

  /*last_write_cursor += size_bytes;
  if (last_write_cursor >= buffer_size_) 
    last_write_cursor -= buffer_size_;
  last_write_cursor = play_cursor;*/

  return S_OK;
}

}