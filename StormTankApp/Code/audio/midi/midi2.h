#include <vector>

namespace midi {

/* Wrapper for accessing strings through sequential reads */
class Stream {
 public:
  
  uint8_t* str;
  void Load(uint8_t* str,uint32_t length) {
    this->str = str;
    str_length = length;
    position = 0;
  }

	uint32_t position,str_length;
	
	uint8_t* read(uint32_t length) {
		auto result = &str[position];
		position += length;
		return result;
	}
	
	/* read a big-endian 32-bit integer */
	uint32_t readInt32() {
		auto result = (
			(str[position] << 24)
			+ (str[position+1] << 16)
			+ (str[position+2] << 8)
			+ str[position+3]);
		position += 4;
		return result;
	}

	/* read a big-endian 16-bit integer */
	uint16_t readInt16() {
		auto result = (
			(str[position] << 8)
			+ str[position+1]);
		position += 2;
		return result;
	}
	
	/* read an 8-bit integer */
	uint8_t readInt8() {
		auto result = str[position];
		position += 1;
		return result;
	}
	
	bool eof() {
		return position >=str_length;
	}
	
	/* read a MIDI-style variable-length integer
		(big-endian value in groups of 7 bits,
		with top bit set to signify that another byte follows)
	*/
	uint32_t readVarInt() {
		uint32_t result = 0;
		while (true) {
			auto b = readInt8();
			if (b & 0x80) {
				result += (b & 0x7f);
				result <<= 7;
			} else {
				/* b is the last byte */
				return result + b;
			}
		}
	}
};

enum EventType {
kEventTypeMeta,
kEventTypeText,
kEventTypeCopyrightNotice,
kEventTypeTrackName,
kEventTypeInstrumentName,
kEventTypeLyrics,
kEventTypeMarker,
kEventTypeCuePoint,
kEventTypeMidiChannelPrefix,
kEventTypeEndOfTrack,
kEventTypeSetTempo,
kEventTypeSmpteOffset,
kEventTypeTimeSignature,
kEventTypeKeySignature,
kEventTypeSequencerSpecific,
kEventTypeUnknown,
kEventTypeSysEx,
kEventTypeSequenceNumber,
kEventTypeDividedSysEx,
kEventTypeChannel,
kEventTypeNoteOff,
kEventTypeNoteOn,
kEventTypeNoteAftertouch,
kEventTypeController,
kEventTypeProgramChange,
kEventTypeChannelAftertouch,
kEventTypePitchBend,
};


/*
class to parse the .mid file format
(depends on stream.js)
*/
class MidiLoader {
 public:
  struct Chunk {
    uint8_t* id;
    uint32_t length;
    uint8_t* data;
    Chunk() :id(nullptr),length(0),data(nullptr) {}
    Chunk(const Chunk& other) {
      id = other.id;
      length = other.length;
      data = other.data;
    }
    Chunk& operator =(const Chunk& other) {
      id = other.id;
      length = other.length;
      data = other.data;
      return *this;
    }
  };

  struct Event {
    uint8_t* data;
    uint32_t deltaTime;
    EventType type,subtype;
    uint8_t channel,track;
    
    uint32_t number;
    uint8_t channelprefix;
    uint8_t frameRate,hour,min,sec,frame,subframe;
    uint8_t numerator,denominator,metronome,thirtyseconds;
    uint8_t key,scale;
    uint8_t noteNumber,velocity;
    uint8_t amount,controllerType,value,programNumber;
    uint32_t microsecondsPerBeat;

    Event() : deltaTime(0),data(nullptr) {
      memset(this,0,sizeof(Event));
    }
    Event(const Event& other) {
      memcpy(this,&other,sizeof(Event));
    }
    Event& operator =(const Event& other) {
      memcpy(this,&other,sizeof(Event));
      return *this;
    }
  };
  uint32_t ticksPerBeat;
  std::vector<Event> eventList[16];
  MidiLoader() : lastEventTypeByte(0),ticksPerBeat(0) {

  }

  void Load(uint8_t* data,uint32_t length);
 protected:
	uint8_t lastEventTypeByte;
  Chunk readChunk(Stream& stream) {
    Chunk chunk;
    chunk.id = stream.read(4);
		chunk.length = stream.readInt32();
    chunk.data = stream.read(chunk.length);
    return chunk;
	}

	
	Event readEvent(Stream& stream);
};

}