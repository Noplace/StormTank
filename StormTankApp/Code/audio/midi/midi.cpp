#include <WinCore/windows/windows.h>
#include <stdio.h>


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


#define MThd 0x4d546864
#define MTrk 0x4d54726b

#pragma pack(push)
#pragma pack(1)
struct HeaderChunk {
  uint32_t tag;
  uint32_t length;
  uint16_t fileformat;
  uint16_t track_count;
  uint16_t time_divison;
};

struct TrackChunk {
  uint32_t tag;
  uint32_t length;
};
#pragma pack(pop)





struct Event {
  Event() : data(nullptr),time(0),command(0),channel(0),next(nullptr),prev(nullptr) {

  }
  ~Event() {
    if (data != nullptr) {
      delete [] data;
      data = nullptr;
    }
  }
  int time;
  int command;
  int channel;
  int type;
  uint8_t* data;

  bool IsSignature() {
    return (command == 0xFF) && (data[0] == 0x58);
  }
  bool IsTempo() {
    return (command == 0xFF) && (type == 0x51);
  }
  bool IsNoteOn() {
    return (command == 0x90);
  }
  bool IsNoteOff() {
    return (command == 0x90);
  }
  bool IsProgramChange() {
    return (command == 0x90);
  }

  int GetNote() {
    return data[0];
  }

  int GetVelocity() {
    return data[1];
  }

  int GetProgram() {
    return data[0];
  }

  double GetTempo() {
    int microseconds_pet_qnote = *((uint32_t*)&data[0]);
    return 60000000.0 / microseconds_pet_qnote;
  }

  Event* next;
  Event* prev;
};


Event* head;


void DeleteEvents() {
  auto* ptr = head;
  while (ptr != nullptr) {
    auto* next = ptr->next;
    delete ptr;
    ptr = next;
  }
}


void ReadMidiFile(char* filename) {
  int error = 0;
  FILE* fp = fopen(filename,"rb");
  FileReader fr;
  fr.fp = fp;

  HeaderChunk header;
  header.tag = fr.ReadNumber(4);
  if (header.tag != MThd) {
    error = 1;
    return;
  }
  header.length = fr.ReadNumber(4);
  if (header.length < 6) {
    error = 2;
    return;
  }
  header.fileformat = fr.ReadNumber(2);
  header.track_count = fr.ReadNumber(2);
  header.time_divison = fr.ReadNumber(2);
  int fps = 0;
  int ticks_per_beat = 0;
  if (header.time_divison & 0x8000) {
    fps = header.time_divison & 0x7FFF;
  } else {
    ticks_per_beat = header.time_divison & 0x7FFF;
  }

  //if (incorrect) {
    //fail 3
  //}
  if (header.length > 6) {
    fr.ReadNumber(header.length-6);
  }
  
  TrackChunk tc;


  bool new_track = true;
  bool read_track;
  uint32_t track_time = 0;

  head = new Event();

  auto* event_ptr = head;

  do
  {
    if (new_track == true) {
      tc.tag = fr.ReadNumber(4);
      if (tc.tag != MTrk) {
        error = 4;
        break;
      }
      tc.length = fr.ReadNumber(4);
      if (tc.length < 0) {
        error = 5;
        break;
      }
      //read tc.length bytes

      new_track = false;
    }

    auto event_time = fr.ReadNumber(0);
    event_ptr->time = event_time;
    track_time += event_time;

    uint8_t command = fr.ReadNumber(1);
    uint8_t channel = command & 0x0F;
    event_ptr->command = command;
    event_ptr->channel = channel;
    event_ptr->type = 0;
    switch (command & 0xF0) {
      case 0x90: {
        event_ptr->data = new uint8_t[2];
        event_ptr->data[0] = fr.ReadNumber(1);
        event_ptr->data[1] = fr.ReadNumber(1);
        //int note = fr.ReadNumber(1);
        //int velocity = fr.ReadNumber(1);
      }
      break;
      case 0xC0: {
        //int prog = fr.ReadNumber(1);
        //int unused = fr.ReadNumber(1);
        event_ptr->data = new uint8_t[1];
        event_ptr->data[0] = fr.ReadNumber(1);
      }
      break;
      case 0xF0:{
        event_ptr->type = fr.ReadNumber(1);
        event_ptr->channel = 0;
        if (event_ptr->type >= 1 && event_ptr->type <= 5) { //text events
          int strlen = fr.ReadNumber(1);
          char* str = new char[strlen+1];
          char* strp = str;
          for (int i=0;i<strlen;++i)
            *strp++ = fgetc(fp);
            *strp++ = '\0';
          event_ptr->data = (uint8_t*)str;
          int a = 1;
        }
        if (event_ptr->type == 0x2F) {
          int len = fr.ReadNumber(1);
          new_track = true;
        }
        if (event_ptr->type == 0x51) {
          event_ptr->data = new uint8_t[4];
          fr.ReadNumber(1); //len
          uint32_t temp = fr.ReadNumber(3);
          memcpy(event_ptr->data,&temp,sizeof(temp));
        }
        if (event_ptr->type == 0x58) {
          event_ptr->data = new uint8_t[4];
          fr.ReadNumber(1);
          event_ptr->data[0] = fr.ReadNumber(1);
          event_ptr->data[1] = fr.ReadNumber(1);
          event_ptr->data[2] = fr.ReadNumber(1);
          event_ptr->data[3] = fr.ReadNumber(1);
          //int len = fr.ReadNumber(1);
          //int num = fr.ReadNumber(1);
          //int den = 1 << fr.ReadNumber(1);
          //int metro_pulse = fr.ReadNumber(1);
          //int _32notes_per_qnote = fr.ReadNumber(1);
        }
      } 
      break;
    }

    read_track = !feof(fp);
    if (new_track == false) { //override the end of track event
      event_ptr->next = new Event();
      event_ptr->next->prev = event_ptr;
      event_ptr = event_ptr->next;
    }
  } while (read_track == true);
  event_ptr->next = nullptr;

  DeleteEvents();

  fclose(fp);
}

void ReadMidi(void* data_pointer, uint32_t data_length) {

}