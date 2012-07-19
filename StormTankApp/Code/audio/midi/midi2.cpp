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
#include <WinCore/windows/windows.h>
#include <stdio.h>
#include <WinCore/math/math.h>
#include "midi2.h"


namespace midi {

void MidiLoader::Load(uint8_t* data,size_t length) {
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
					event.subtype = kEventSubtypeSequenceNumber;
					if (length != 2) {
            //throw "Expected length for sequenceNumber event is 2, got " + length;
            DebugBreak();
          }
					event.data.seq.number = stream.readInt16();
					return;
				case 0x01:
					event.subtype = kEventSubtypeText;
					stream.read(length);
					return;
				case 0x02:
					event.subtype = kEventSubtypeCopyrightNotice;
					stream.read(length);
					return;
				case 0x03:
					event.subtype = kEventSubtypeTrackName;
					stream.read(length);
					return;
				case 0x04:
					event.subtype = kEventSubtypeInstrumentName;
					stream.read(length);
					return;
				case 0x05:
					event.subtype = kEventSubtypeLyrics;
					stream.read(length);
					return;
				case 0x06:
					event.subtype = kEventSubtypeMarker;
					stream.read(length);
					return;
				case 0x07:
					event.subtype = kEventSubtypeCuePoint;
					stream.read(length);
					return;
				case 0x20:
					event.subtype = kEventSubtypeMidiChannelPrefix;
					if (length != 1) {
            //throw "Expected length for midiChannelPrefix event is 1, got " + length;
            DebugBreak();
          }
					event.data.cp.channelprefix = stream.readInt8();
					return;
				case 0x2f:
					event.subtype = kEventSubtypeEndOfTrack;
					if (length != 0) throw "Expected length for endOfTrack event is 0, got " + length;
					return;
				case 0x51:
					event.subtype = kEventSubtypeSetTempo;
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
					event.subtype = kEventSubtypeSmpteOffset;
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
					event.subtype = kEventSubtypeTimeSignature;
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
					event.subtype = kEventSubtypeKeySignature;
					if (length != 2){
            DebugBreak();
            //throw "Expected length for keySignature event is 2, got " + length;
          }
					event.data.key_signature.key = stream.readInt8();
					event.data.key_signature.scale = stream.readInt8();
					return;
				case 0x7f:
					event.subtype = kEventSubtypeSequencerSpecific;
					//event.data = 
            stream.read(length);
					return;
				default:
          //DebugBreak();
					// console.log("Unrecognised meta event subtype: " + subtypeByte);
					event.subtype = kEventSubtypeUnknown;
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
				event.subtype = kEventSubtypeNoteOff;
				event.data.note.noteNumber = param1;
				event.data.note.velocity = stream.readInt8();
				return;
			case 0x09:
				event.data.note.noteNumber = param1;
				event.data.note.velocity = stream.readInt8();
				if (event.data.note.velocity == 0) {
					event.subtype = kEventSubtypeNoteOff;
				} else {
					event.subtype = kEventSubtypeNoteOn;
				}
				return;
			case 0x0a:
				event.subtype = kEventSubtypeNoteAftertouch;
				event.data.note_aftertouch.noteNumber = param1;
				event.data.note_aftertouch.amount = stream.readInt8();
				return;
			case 0x0b:
				event.subtype = kEventSubtypeController;
				event.data.controller.type = param1;
        event.data.controller.value = stream.readInt8();
				return;
			case 0x0c:
				event.subtype = kEventSubtypeProgramChange;
				event.data.program.number = param1;
				return;
			case 0x0d:
				event.subtype = kEventSubtypeChannelAftertouch;
				event.data.channel_aftertouch.amount = param1;
				return;
			case 0x0e:
				event.subtype = kEventSubtypePitchBend;
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
