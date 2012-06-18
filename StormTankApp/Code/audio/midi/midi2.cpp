#include <WinCore/windows/windows.h>
#include <stdio.h>
#include <math.h>
#include "midi2.h"


namespace midi {

void MidiLoader::Load(uint8_t* data,uint32_t length) {
    Stream stream;
    stream.Load(data,length);
	  
    auto headerChunk = readChunk(stream);
	  if (strncmp((const char*)headerChunk.id,"MThd",4) !=0 || headerChunk.length != 6) {
		  //throw "Bad .mid file - header not found";
	  }
	  Stream headerStream;
    headerStream.Load(headerChunk.data,headerChunk.length);
	  auto formatType = headerStream.readInt16();
	  auto trackCount = headerStream.readInt16();
	  auto timeDivision = headerStream.readInt16();
	
	  if (timeDivision & 0x8000) {
		  //throw "Expressing time division in SMTPE frames is not supported yet"
	  } else {
		  ticksPerBeat = timeDivision;
	  }
	
    
	  //var tracks = [];
	  for (auto i = 0; i < trackCount; i++) {
		  //tracks[i] = [];
		  auto trackChunk = readChunk(stream);
		  if (strncmp((const char*)trackChunk.id,"MTrk",4)  != 0) {
			  //throw "Unexpected chunk - expected MTrk, got "+ trackChunk.id;
        int a = 1;
		  }
		  Stream trackStream;
      trackStream.Load(trackChunk.data,trackChunk.length);
		  while (!trackStream.eof()) {
			  auto event = readEvent(trackStream);
        event.track = i;
			  eventList[i].push_back(event);
			  //console.log(event);
		  }
	  }
  }

MidiLoader::Event MidiLoader::readEvent(Stream& stream) {
	Event event;
	event.deltaTime = stream.readVarInt();
	auto eventTypeByte = stream.readInt8();
	if ((eventTypeByte & 0xf0) == 0xf0) {
		/* system / meta event */
		if (eventTypeByte == 0xff) {
			/* meta event */
			event.type = kEventTypeMeta;
			auto subtypeByte = stream.readInt8();
			auto length = stream.readVarInt();
			switch(subtypeByte) {
				case 0x00:
					event.subtype = kEventTypeSequenceNumber;
					if (length != 2) {
            //throw "Expected length for sequenceNumber event is 2, got " + length;
            int a = 1;
          }
					event.number = stream.readInt16();
					return event;
				case 0x01:
					event.subtype = kEventTypeText;
					stream.read(length);
					return event;
				case 0x02:
					event.subtype = kEventTypeCopyrightNotice;
					stream.read(length);
					return event;
				case 0x03:
					event.subtype = kEventTypeTrackName;
					stream.read(length);
					return event;
				case 0x04:
					event.subtype = kEventTypeInstrumentName;
					stream.read(length);
					return event;
				case 0x05:
					event.subtype = kEventTypeLyrics;
					stream.read(length);
					return event;
				case 0x06:
					event.subtype = kEventTypeMarker;
					stream.read(length);
					return event;
				case 0x07:
					event.subtype = kEventTypeCuePoint;
					stream.read(length);
					return event;
				case 0x20:
					event.subtype = kEventTypeMidiChannelPrefix;
					if (length != 1) {
            //throw "Expected length for midiChannelPrefix event is 1, got " + length;
            int a = 1;
          }
					event.channelprefix = stream.readInt8();
					return event;
				case 0x2f:
					event.subtype = kEventTypeEndOfTrack;
					if (length != 0) throw "Expected length for endOfTrack event is 0, got " + length;
					return event;
				case 0x51:
					event.subtype = kEventTypeSetTempo;
					if (length != 3) {
            //throw "Expected length for setTempo event is 3, got " + length;
            int a = 1;
          }
					event.microsecondsPerBeat = (
						(stream.readInt8() << 16)
						+ (stream.readInt8() << 8)
						+ stream.readInt8()
					);
					return event;
        case 0x54: {
					event.subtype = kEventTypeSmpteOffset;
					if (length != 5) {
            //throw "Expected length for smpteOffset event is 5, got " + length;
          }
					auto hourByte = stream.readInt8();
          //uint8_t frameRate[] = {
          switch (hourByte & 0x60) {
						case 0x00: event.frameRate = 24; break; 
            case 0x20: event.frameRate = 25; break;
            case 0x40: event.frameRate = 29; break;
            case 0x60: event.frameRate = 30; break;
					}
          //[hourByte & 0x60];
					event.hour = hourByte & 0x1f;
					event.min = stream.readInt8();
					event.sec = stream.readInt8();
					event.frame = stream.readInt8();
					event.subframe = stream.readInt8();
					return event;
        }
				case 0x58:
					event.subtype = kEventTypeTimeSignature;
					if (length != 4) {
            //throw "Expected length for timeSignature event is 4, got " + length;
            int a = 1;
          }
					event.numerator = stream.readInt8();
					event.denominator = uint8_t(pow(2.0, (double)stream.readInt8()));
					event.metronome = stream.readInt8();
					event.thirtyseconds = stream.readInt8();
					return event;
				case 0x59:
					event.subtype = kEventTypeKeySignature;
					if (length != 2){
            int a = 1;
            //throw "Expected length for keySignature event is 2, got " + length;
          }
					event.key = stream.readInt8();
					event.scale = stream.readInt8();
					return event;
				case 0x7f:
					event.subtype = kEventTypeSequencerSpecific;
					event.data = stream.read(length);
					return event;
				default:
					// console.log("Unrecognised meta event subtype: " + subtypeByte);
					event.subtype = kEventTypeUnknown;
					event.data = stream.read(length);
					return event;
			}
			event.data = stream.read(length);
			return event;
		} else if (eventTypeByte == 0xf0) {
			event.type = kEventTypeSysEx;
			auto length = stream.readVarInt();
			event.data = stream.read(length);
			return event;
		} else if (eventTypeByte == 0xf7) {
			event.type = kEventTypeDividedSysEx;
			auto length = stream.readVarInt();
			event.data = stream.read(length);
			return event;
		} else {
			//throw "Unrecognised MIDI event type byte: " + eventTypeByte;
      int a = 1;
		}
	} else {
		/* channel event */
		uint8_t param1;
		if ((eventTypeByte & 0x80) == 0) {
			/* running status - reuse lastEventTypeByte as the event type.
				eventTypeByte is actually the first parameter
			*/
			param1 = eventTypeByte;
			eventTypeByte = lastEventTypeByte;
		} else {
			param1 = stream.readInt8();
			lastEventTypeByte = eventTypeByte;
		}
		auto eventType = eventTypeByte >> 4;
		event.channel = eventTypeByte & 0x0f;
		event.type = kEventTypeChannel;
		switch (eventType) {
			case 0x08:
				event.subtype = kEventTypeNoteOff;
				event.noteNumber = param1;
				event.velocity = stream.readInt8();
				return event;
			case 0x09:
				event.noteNumber = param1;
				event.velocity = stream.readInt8();
				if (event.velocity == 0) {
					event.subtype = kEventTypeNoteOff;
				} else {
					event.subtype = kEventTypeNoteOn;
				}
				return event;
			case 0x0a:
				event.subtype = kEventTypeNoteAftertouch;
				event.noteNumber = param1;
				event.amount = stream.readInt8();
				return event;
			case 0x0b:
				event.subtype = kEventTypeController;
				event.controllerType = param1;
				event.value = stream.readInt8();
				return event;
			case 0x0c:
				event.subtype = kEventTypeProgramChange;
				event.programNumber = param1;
				return event;
			case 0x0d:
				event.subtype = kEventTypeChannelAftertouch;
				event.amount = param1;
				return event;
			case 0x0e:
				event.subtype = kEventTypePitchBend;
				event.value = param1 + (stream.readInt8() << 7);
				return event;
			default:
				//throw "Unrecognised MIDI event type: " + eventType
        int a = 1;
				/*
				console.log("Unrecognised MIDI event type: " + eventType);
				stream.readInt8();
				event.subtype = 'unknown';
				return event;
				*/
		}
	}
}

}

void ReadMidiFile2(char* filename) {

  FILE* fp = fopen(filename,"rb");
  fseek(fp,0,SEEK_END);
  long size = ftell(fp);
  fseek(fp,0,SEEK_SET);
  auto buffer = new uint8_t[size];
  fread(buffer,1,size,fp);
  fclose(fp);
  midi::MidiLoader midifile;
  midifile.Load(buffer,size);


  delete [] buffer;

}