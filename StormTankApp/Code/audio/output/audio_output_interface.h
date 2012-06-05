#ifndef AUDIO_OUTPUT_AUDIO_OUTPUT_INTERFACE_H
#define AUDIO_OUTPUT_AUDIO_OUTPUT_INTERFACE_H

namespace audio {

class AudioOutputInterface {
 public:
  virtual int Initialize(uint32_t sample_rate,uint8_t channels,uint8_t bits) = 0;
  virtual int Deinitialize() = 0;
  virtual int Play() = 0;
  virtual int Stop() = 0;
  virtual uint32_t GetBytesBuffered() = 0;
  virtual int Write(void*, uint32_t) = 0;
 protected:
  WAVEFORMATEX wave_format_;
};

}

#endif