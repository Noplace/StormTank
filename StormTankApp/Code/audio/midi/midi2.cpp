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
      DebugBreak();
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
        DebugBreak();
		  }
		  Stream trackStream;
      trackStream.Load(trackChunk.data,trackChunk.length);
		  while (!trackStream.eof()) {
			  Event event;
        readEvent(trackStream,event);
        event.track = i;
			  eventList[i].push_back(event);
			  //console.log(event);
		  }
      ++track_count;
	  }
  }

void MidiLoader::readEvent(Stream& stream,Event& event) {

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
            DebugBreak();
          }
					event.data.seq.number = stream.readInt16();
					return;
				case 0x01:
					event.subtype = kEventTypeText;
					stream.read(length);
					return;
				case 0x02:
					event.subtype = kEventTypeCopyrightNotice;
					stream.read(length);
					return;
				case 0x03:
					event.subtype = kEventTypeTrackName;
					stream.read(length);
					return;
				case 0x04:
					event.subtype = kEventTypeInstrumentName;
					stream.read(length);
					return;
				case 0x05:
					event.subtype = kEventTypeLyrics;
					stream.read(length);
					return;
				case 0x06:
					event.subtype = kEventTypeMarker;
					stream.read(length);
					return;
				case 0x07:
					event.subtype = kEventTypeCuePoint;
					stream.read(length);
					return;
				case 0x20:
					event.subtype = kEventTypeMidiChannelPrefix;
					if (length != 1) {
            //throw "Expected length for midiChannelPrefix event is 1, got " + length;
            DebugBreak();
          }
					event.data.cp.channelprefix = stream.readInt8();
					return;
				case 0x2f:
					event.subtype = kEventTypeEndOfTrack;
					if (length != 0) throw "Expected length for endOfTrack event is 0, got " + length;
					return;
				case 0x51:
					event.subtype = kEventTypeSetTempo;
					if (length != 3) {
            //throw "Expected length for setTempo event is 3, got " + length;
            DebugBreak();
          }
					event.data.tempo.microsecondsPerBeat = (
						(stream.readInt8() << 16)
						+ (stream.readInt8() << 8)
						+ stream.readInt8()
					);
					return;
        case 0x54: {
					event.subtype = kEventTypeSmpteOffset;
					if (length != 5) {
            //throw "Expected length for smpteOffset event is 5, got " + length;
          }
					auto hourByte = stream.readInt8();
          //uint8_t frameRate[] = {
          switch (hourByte & 0x60) {
						case 0x00: event.data.smpte_offset.frameRate = 24; break; 
            case 0x20: event.data.smpte_offset.frameRate = 25; break;
            case 0x40: event.data.smpte_offset.frameRate = 29; break;
            case 0x60: event.data.smpte_offset.frameRate = 30; break;
					}
          //[hourByte & 0x60];
					event.data.smpte_offset.hour = hourByte & 0x1f;
					event.data.smpte_offset.min = stream.readInt8();
					event.data.smpte_offset.sec = stream.readInt8();
					event.data.smpte_offset.frame = stream.readInt8();
					event.data.smpte_offset.subframe = stream.readInt8();
					return;
        }
				case 0x58:
					event.subtype = kEventTypeTimeSignature;
					if (length != 4) {
            //throw "Expected length for timeSignature event is 4, got " + length;
            DebugBreak();
          }
					event.data.time_signature.numerator = stream.readInt8();
					event.data.time_signature.denominator = uint8_t(pow(2.0, (double)stream.readInt8()));
					event.data.time_signature.metronome = stream.readInt8();
					event.data.time_signature.thirtyseconds = stream.readInt8();
					return;
				case 0x59:
					event.subtype = kEventTypeKeySignature;
					if (length != 2){
            DebugBreak();
            //throw "Expected length for keySignature event is 2, got " + length;
          }
					event.data.key_signature.key = stream.readInt8();
					event.data.key_signature.scale = stream.readInt8();
					return;
				case 0x7f:
					event.subtype = kEventTypeSequencerSpecific;
					//event.data = 
            stream.read(length);
					return;
				default:
          //DebugBreak();
					// console.log("Unrecognised meta event subtype: " + subtypeByte);
					event.subtype = kEventTypeUnknown;
					//event.data = 
            stream.read(length);
					return;
      }
			//event.data = 
        stream.read(length);
			return;
		} else if (eventTypeByte == 0xf0) {
			event.type = kEventTypeSysEx;
			auto length = stream.readVarInt();
			//event.data = 
        stream.read(length);
			return;
		} else if (eventTypeByte == 0xf7) {
			event.type = kEventTypeDividedSysEx;
			auto length = stream.readVarInt();
			//event.data = 
        stream.read(length);
			return;
		} else {
			//throw "Unrecognised MIDI event type byte: " + eventTypeByte;
      DebugBreak();
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
				event.data.note.noteNumber = param1;
				event.data.note.velocity = stream.readInt8();
				return;
			case 0x09:
				event.data.note.noteNumber = param1;
				event.data.note.velocity = stream.readInt8();
				if (event.data.note.velocity == 0) {
					event.subtype = kEventTypeNoteOff;
				} else {
					event.subtype = kEventTypeNoteOn;
				}
				return;
			case 0x0a:
				event.subtype = kEventTypeNoteAftertouch;
				event.data.note_aftertouch.noteNumber = param1;
				event.data.note_aftertouch.amount = stream.readInt8();
				return;
			case 0x0b:
				event.subtype = kEventTypeController;
				event.data.controller.type = param1;
        event.data.controller.value = stream.readInt8();
				return;
			case 0x0c:
				event.subtype = kEventTypeProgramChange;
				event.data.program.number = param1;
				return;
			case 0x0d:
				event.subtype = kEventTypeChannelAftertouch;
				event.data.channel_aftertouch.amount = param1;
				return;
			case 0x0e:
				event.subtype = kEventTypePitchBend;
				event.data.pitch.value = param1 + (stream.readInt8() << 7);
				return;
			default:
				//throw "Unrecognised MIDI event type: " + eventType
        DebugBreak();
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