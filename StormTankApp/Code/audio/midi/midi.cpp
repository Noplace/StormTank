#include <WinCore/windows/windows.h>
#include <stdio.h>

union be_uint32_t {
  struct {
    uint8_t bytes[4];
  };
  uint32_t raw;
  uint32_t value() {
    return (bytes[0]<<24) |  (bytes[1]<<16) |  (bytes[2]<<8) |  bytes[3];
  }
  uint32_t operator=(const be_uint32_t& other) {
    raw = other.raw;
    return value();
  }
};

union be_uint16_t {
  struct {
    uint8_t bytes[2];
  };
  uint16_t raw;
  uint16_t value() {
    return (bytes[0]<<8) |  bytes[1];
  }
  uint16_t operator=(const be_uint16_t& other) {
    raw = other.raw;
    return value();
  }


};

#define MThd 0x4d546864
#define MTrk 0x4d54726b

#pragma pack(push)
#pragma pack(1)
struct HeaderChunk {
  uint32_t tag;
  uint32_t length;
  uint16_t fileformat;
  uint16_t track_count;
  uint16_t dttpqn;
};

struct TrackChunk {
  uint32_t tag;
  uint32_t length;
};
#pragma pack(pop)

class FileReader {
 public:
  FILE* fp;
  uint32_t ReadVar()
  {
    uint32_t v=0; int c;

    if ((c = fgetc(fp)) == EOF) return -1;

    while (c & 0x80 ) {
      v = (v << 7) | (c & 0x7f);
      if ((c = fgetc(fp)) == EOF) return -1;
    }
    v = (v << 7) | c;
    return (v);
  }

  uint32_t ReadNumber(int k)
  {
    uint32_t x=0, v = 0;

    if (k == 0) return(ReadVar());

    while (k-- > 0) {
      if ((x = fgetc(fp)) == EOF) return -1;
      v = (v << 8) | x;
    }
    return v;
  }
  
};

void ReadMidiFile(char* filename) {
  FILE* fp = fopen(filename,"rb");
  FileReader fr;
  fr.fp = fp;

  HeaderChunk header;
  header.tag = fr.ReadNumber(4);
  if (header.tag != MThd) {
    //fail 1
  }
  header.length = fr.ReadNumber(4);
  if (header.length < 6) {
    //fail 2
  }
  header.fileformat = fr.ReadNumber(2);
  header.track_count = fr.ReadNumber(2);
  header.dttpqn = fr.ReadNumber(2);
  //if (incorrect) {
    //fail 3
  //}
  if (header.length > 6) {
    fr.ReadNumber(header.length-6);
  }
  
  TrackChunk tc;
  uint32_t track_time=0;
  tc.tag = fr.ReadNumber(4);
  if (tc.tag != MTrk) {
    //fail 4
  }
  tc.length = fr.ReadNumber(4);
  if (tc.length < 0) {
    //fail 5
  }

  auto fpos = fseek(fp,0,SEEK_CUR);
  bool read_track = (fseek(fp,0,SEEK_CUR) - fpos <= 19);
  //while (read_track == true)
  {
    auto event_time = fr.ReadNumber(0);
    track_time += event_time;
    uint8_t command = fr.ReadNumber(1);
    switch (command) {
      case 0xFF:{
        auto v1 = fr.ReadNumber(1);
        if (v1 < 0) {
          //fail
        }
      } 
      break;
    }
    read_track = (fseek(fp,0,SEEK_CUR) - fpos <= 19);
  }



  fclose(fp);
}

void ReadMidi(void* data_pointer, uint32_t data_length) {

}