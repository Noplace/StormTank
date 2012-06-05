#include <WinCore/windows/windows.h>
#include <WinCore/types.h>
#include <audiodefs.h>
#include <XAudio2.h>
#include "audio_interface.h"


namespace audio {

struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
            STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 )
            {
            }
            STDMETHOD_( void, OnVoiceProcessingPassEnd )()
            {
            }
            STDMETHOD_( void, OnStreamEnd )()
            {
            }
            STDMETHOD_( void, OnBufferStart )( void* )
            {
            }
            STDMETHOD_( void, OnBufferEnd )( void* data)
            {
             // delete [] data;
                SetEvent( hBufferEndEvent );
            }
            STDMETHOD_( void, OnLoopEnd )( void* )
            {
            }
            STDMETHOD_( void, OnVoiceError )( void*, HRESULT )
            {
            }

    HANDLE hBufferEndEvent;

            StreamingVoiceContext() : hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) )
            {
            }
    virtual ~StreamingVoiceContext()
    {
        CloseHandle( hBufferEndEvent );
    }
};

class XAudio : public AudioInterface {
 public:
  XAudio();
  ~XAudio();
  int Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits);
  int Deinitialize();
  uint32_t GetBytesBuffered();
  int Write(void* data_pointer, uint32_t size_bytes);
  void set_window_handle(HWND window_handle) { window_handle_ = window_handle; }
  void set_buffer_size(uint32_t buffer_size) { buffer_size_ = buffer_size; }
  IXAudio2SourceVoice* pSourceVoice;
  StreamingVoiceContext voiceContext;
 protected:
  void* window_handle_;
  uint32_t buffer_size_;
  IXAudio2* pXAudio2;
  IXAudio2MasteringVoice* pMasteringVoice;
};

}