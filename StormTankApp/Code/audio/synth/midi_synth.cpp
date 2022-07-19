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
#include "midi_synth.h"
#include "instruments/instrument.h"
#include "instruments/osc_wave.h"
#include "instruments/pad.h"
#include "instruments/piano.h"
#include "instruments/violin.h"
#include "instruments/percussion.h"
#include "instruments/blit.h"
#include "instruments/karplusstrong.h"
#include "instruments/sonant_program.h"
#include "instruments/waveguide_synthesis.h"
#include "instruments/white_noise.h"
//#define OUTPUT_EVENTS

namespace audio {
namespace synth {

const char* eventnames[] = {"Meta",
"SysEx",
"DividedSysEx",
"Channel",
"Unknown"
};
const char* subeventnames[] = {
"Text",
"CopyrightNotice",
"TrackName",
"InstrumentName",
"Lyrics",
"Marker",
"CuePoint",
"MidiChannelPrefix",
"EndOfTrack",
"SetTempo",
"SmpteOffset",
"TimeSignature",
"KeySignature",
"SequencerSpecific",
"SequenceNumber",
"NoteOff",
"NoteOn",
"NoteAftertouch",
"Controller",
"ProgramChange",
"ChannelAftertouch",
"PitchBend",
"Unknown",
};


const MidiSynth::MidiEventHandler MidiSynth::midi_main_event_handlers[5] = {
  &MidiSynth::MidiEventMeta,    &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, 
  &MidiSynth::MidiEventChannel, &MidiSynth::MidiEventUnknown, 
};

const MidiSynth::MidiEventHandler MidiSynth::midi_meta_event_handlers[23] = {
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventSetTempo, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,  &MidiSynth::MidiEventUnknown,
};

const MidiSynth::MidiEventHandler MidiSynth::midi_channel_event_handlers[23] = {
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown,
  &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventNoteOff, 
  &MidiSynth::MidiEventNoteOn, &MidiSynth::MidiEventUnknown, &MidiSynth::MidiEventController, &MidiSynth::MidiEventProgramChange,  &MidiSynth::MidiEventUnknown,&MidiSynth::MidiEventPitchBend, &MidiSynth::MidiEventUnknown,

};

void MidiSynth::Initialize() {
  if (initialized_ == true)
    return;

  {
    static const real_t A4_freq = 440;
    for (int i=0;i<note_count;++i) {
      chromatic_scale_freq[i] = A4_freq * powf(_2_POW_12TH,i-69);//45 = index of A4
    }
  }


  auto buffer_length_ms_ = 400.0f;//400ms
  auto buffer_size = uint32_t(sample_rate_*2*buffer_length_ms_*0.001);
  buffers.aux_size = buffers.main_size = buffer_size;
  buffers.main = new real_t[buffers.main_size]; //44100 samples & channels , check with player later on!
  buffers.aux = new real_t[buffers.aux_size]; //44100 samples & channels , check with player later on!

  last_event = nullptr;
  samples_to_next_event = 0;

  //initialize all available instruments
  
  {
      auto inst = new instruments::KarplusStrong();//KarplusStrong();//instruments::Piano();
    /*decltype(inst->data) data = { // 0
			// Oscillator 1
			7, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			192, // Volume knob
			2, // Wave form
			// Oscillator 2
			8, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			157, // Volume knob
			3, // Wave form
			// Noise oscillator
			0, // Amount of noise to add
			// Envelope
			100, // Attack
			2727, // Sustain
			22727, // Release
			185, // Master volume knob
			// Effects
			2, // Hi/lo/bandpass or notch toggle
			11024, // FX Frequency
			240, // FX Resonance
			6, // Delay time
			132, // Delay amount
			0, // Panning frequency
			171, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			0, // LFO freq
			207, // LFO amount
			0, // LFO waveform
			
		};
    memcpy(&inst->data,&data,sizeof(data));*/
      instr[0] = nullptr;//inst;
  }
  instr[2] = new instruments::OscWave(instruments::OscWave::Triangle);

  instr[3] = new instruments::WhiteNoise();

  instr[40] = new instruments::Violin();

  instr[48] = new instruments::Pad();//strings1
  instr[49] = new instruments::Pad();

  instr[80] = new instruments::OscWave(instruments::OscWave::Square);
  instr[81] = new instruments::OscWave(instruments::OscWave::Sawtooth);
  for (int i=0;i<kInstrumentCount;++i) {
    if (instr[i] == nullptr)
      instr[i] = new instruments::OscWave(instruments::OscWave::Sine);
    instr[i]->set_sample_rate(sample_rate_);
  }


  instr[0]->Load();
  instr[2]->Load();
  instr[3]->Load();
  instr[81]->Load();
  for (int i=0;i<kChannelCount;++i) {
    buffers.channels[i] = new real_t[buffers.main_size];
    memset(buffers.channels[i],0,sizeof(real_t)*buffers.main_size);
    channels[i] = new Channel(i);
    channels[i]->set_sample_rate(sample_rate_);
    channels[i]->set_buffer(buffers.channels[i]);
    //channels[i]->set_buffer_length(400.0f);
    channels[i]->set_instrument(instr[0]);
    channels[i]->set_silence(true);
    channels[i]->set_panning(0.5f);
    channels[i]->set_amplification(0.7f);
  }
  //percussion
  percussion_instr = new instruments::Percussion();
  percussion_instr->set_sample_rate(sample_rate_);
  percussion_instr->Load();
  channels[9]->set_instrument(percussion_instr);

  delay_unit.Initialize(uint32_t(sample_rate_*2));
  delay_unit.set_feedback(0.3f);
  delay_unit.set_delay_ms(400.0f);

  tracks = nullptr;

  if (mode_ == kModeTest) {
    track_count_ = 1;
    tracks = new Track[track_count_];
    bpm = 120; //qn per min
    channels[0]->set_silence(false);
    uint32_t max_event_count = 0;
    for (int track_index=0;track_index<track_count_;++track_index) {
        auto& track = tracks[track_index];
        tracks[track_index].event_count = 20;
        tracks[track_index].event_sequence = new midi::Event[tracks[track_index].event_count];
        tracks[track_index].event_index = 0;
        max_event_count = max(max_event_count,tracks[track_index].event_count);
    }
  }
  InitializeCriticalSection(&me_lock);
  initialized_ = true;
}

void MidiSynth::Deinitialize() {
  if (initialized_ == false)
    return;
  DeleteCriticalSection(&me_lock);
  delay_unit.Deinitialize();
  DestroyTrackData();
  for (int i=0;i<kChannelCount;++i) {
    SafeDelete(&channels[i]);
    SafeDeleteArray(&buffers.channels[i]);
  }
  SafeDelete(&percussion_instr);
  for (int i=0;i<kInstrumentCount;++i) {
    SafeDelete(&instr[i]);
  }
  SafeDeleteArray(&buffers.aux);
  SafeDeleteArray(&buffers.main);
  initialized_ = false;
}

void MidiSynth::LoadMidiFromFile(const char* filename) {
  uint8_t* buffer;
  size_t size=0;
  core::io::ReadWholeFileBinary(filename,&buffer,size);
  if (size != 0) {
    LoadMidi(buffer,size);
    core::io::DestroyFileBuffer(&buffer);
  }
}

void MidiSynth::LoadMidi(uint8_t* data, size_t data_size) {
  if (mode_ == kModeTest)
    return;
  player_->Stop();
  
  midi::MidiLoader midifile;
  midifile.Load(data,data_size);
  
  DestroyTrackData();
  track_count_ = midifile.track_count;
  tracks = new Track[track_count_];
  ticks_per_beat = midifile.ticksPerBeat;
  bpm = 120; //qn per min
  double bps = bpm / 60.0;
  double secs_per_tick;
  double time_ms;
  secs_per_tick = 1 / ( bps * ticks_per_beat );
  player_->song_length_ms = 0;   


  uint32_t max_event_count = 0;
  for (int track_index=0;track_index<track_count_;++track_index) {
      auto& track = tracks[track_index];
      tracks[track_index].event_count = (uint32_t)midifile.eventList[track_index].size();
      tracks[track_index].event_sequence = new midi::Event[tracks[track_index].event_count];
      max_event_count = max(max_event_count,tracks[track_index].event_count);
  }
  //ResetTracks();
  //uint32_t abs_time[64];
  //memset(abs_time,0,sizeof(abs_time));
  
  for (uint32_t event_index=0;event_index<max_event_count;++event_index) {
    int track_index=0;
    for (track_index=0;track_index<track_count_;++track_index) {
      auto& track = tracks[track_index];
      if (event_index >= tracks[track_index].event_count)
        continue;
      auto source_event = &midifile.eventList[track_index][event_index];

      switch (source_event->subtype) {
        case midi::kEventSubtypeNoteOff:
          if (source_event->data.note.velocity == 0)
            source_event->data.note.velocity = 127;
        break;

        case midi::kEventSubtypeSetTempo:
          bpm = 60000000.0 / double(source_event->data.tempo.microsecondsPerBeat);
          bps = bpm / 60.0;
          secs_per_tick = 1 / ( bps * ticks_per_beat );
        break;

        case midi::kEventSubtypeProgramChange:

          if (source_event->channel == 9) {
            percussion_instr->Load();
          } else {
            instr[source_event->data.program.number]->Load();
          }
        break;
      }

      auto& current_event = tracks[track_index].event_sequence[event_index];
      memset(&current_event,0,sizeof(midi::Event));
      //abs_time[track_index] += source_event->deltaTime;
      double prev_abs_time_ms = event_index == 0?0.0:tracks[track_index].event_sequence[event_index-1].abs_time_ms;
      time_ms = secs_per_tick * 1000 * source_event->deltaTime;
      current_event.abs_time_ms = prev_abs_time_ms+time_ms;
      current_event.deltaTime = source_event->deltaTime;
      current_event.type = source_event->type;
      current_event.subtype = source_event->subtype;
      current_event.channel = source_event->channel;
      current_event.track = source_event->track;
      channels[source_event->channel]->set_silence(false);
      memcpy(&current_event.data,&source_event->data,sizeof(current_event.data));
      
      #ifdef OUTPUT_EVENTS
        char str[512];
        sprintf(str,"time\t%04.03f\ttype : %s\t%02d\t%s\ttrack %02d\tchannel %02d\n",time_ms,eventnames[source_event->type],source_event->subtype,subeventnames[source_event->subtype],source_event->track,source_event->channel);
        OutputDebugString(str);
      #endif
      if (current_event.subtype == midi::kEventSubtypeProgramChange) {
        char str[512];
        sprintf(str,"time %04.03f type : %s %s track %d channel %d",time_ms,eventnames[source_event->type],subeventnames[source_event->subtype],source_event->track,source_event->channel);
        OutputDebugString(str);
        sprintf(str,"[data: %d]\n",current_event.data.program.number);
        OutputDebugString(str);
      }
      /*if (current_event.subtype == midi::kEventSubtypeController) {
        sprintf(str,"time %04.03f type : %s %s track %d channel %d\n",time_ms,eventnames[source_event->type],eventnames[source_event->subtype],source_event->track,source_event->channel);
        OutputDebugString(str);
        sprintf(str,"\tdata: %d %d\n",current_event.data.controller.type,current_event.data.controller.value);
        OutputDebugString(str);
      }*/
    }
    
  } 

  //find song length
  for (int i=0;i<track_count_;++i) {
    player_->song_length_ms = max(player_->song_length_ms,tracks[i].event_sequence[tracks[i].event_count-1].abs_time_ms);
  }

  Reset();
}


void MidiSynth::RenderSamplesStereo(uint32_t samples_count, real_t* data_out) {
	uint32_t data_offset = 0;
  if (mode_ == kModeTest) {
    samples_to_next_event = samples_count;
		auto samples_to_generate = samples_to_next_event;
		if (samples_to_generate > 0) {
      GenerateIntoBufferStereo(samples_to_generate,data_out,data_offset);
			data_offset += samples_to_generate * 2;
			samples_count -= samples_to_generate;
			samples_to_next_event -= samples_to_generate;
      if (mode_ == kModeTest)
        samples_to_next_event = 0xFFFFFFFF;
		}
    if (last_event != nullptr) {
			HandleEvent(last_event);
      memset(last_event,0,sizeof(midi::Event));
    }
    last_event = GetNextEvent();
  } else if (mode_ == kModeSequencer) {
    while (true) {
		  if (samples_to_next_event != 0xFFFFFFFF && samples_to_next_event <= samples_count) {
			  /* generate samplesToNextEvent samples, process event and repeat */
			  auto samples_to_generate = samples_to_next_event;
			  if (samples_to_generate > 0) {
          GenerateIntoBufferStereo(samples_to_generate,data_out,data_offset);
				  samples_count -= samples_to_generate;
				  samples_to_next_event -= samples_to_generate;
          if (mode_ == kModeTest)
            samples_to_next_event = 0xFFFFFFFF;
			  }
        if (last_event != nullptr) {
			    HandleEvent(last_event);
          if (mode_ == kModeTest)
            memset(last_event,0,sizeof(midi::Event));
        }
        last_event = GetNextEvent();

		  } else {
			  /* generate samples to end of buffer */
			  if (samples_count > 0) {
				  GenerateIntoBufferStereo(samples_count,data_out,data_offset);
				  samples_to_next_event -= samples_count;
			  }
			  break;
		  }
	  }
  }


}


void MidiSynth::GenerateIntoBufferStereo(uint32_t samples_to_generate, real_t* data_out, uint32_t& data_offset) {
  //mix and send to aux
  MixChannelsStereo(samples_to_generate);
  //effects processing
  for (int i=0;i<5;++i) {
    if (main_effects[i] != nullptr)
      main_effects[i]->ProcessStereo(buffers.main,buffers.main,samples_to_generate);
    if (aux_effects[i] != nullptr)
      aux_effects[i]->ProcessStereo(buffers.aux,buffers.aux,samples_to_generate);
  }

  //mix with aux and send to output
  for (uint32_t i=0;i<samples_to_generate<<1;i+=2) {
    buffers.main[i] += buffers.aux[i];
    buffers.main[i+1] += buffers.aux[i+1];
    data_out[data_offset++] = buffers.main[i];
    data_out[data_offset++] = buffers.main[i+1];
  }
}

void MidiSynth::MixChannelsStereo(uint32_t samples_to_generate) {
  
    channels[0]->RenderStereo(samples_to_generate);
    channels[1]->RenderStereo(samples_to_generate);
    channels[2]->RenderStereo(samples_to_generate);
    channels[3]->RenderStereo(samples_to_generate);
    channels[4]->RenderStereo(samples_to_generate);
    channels[5]->RenderStereo(samples_to_generate);
    channels[6]->RenderStereo(samples_to_generate);
    channels[7]->RenderStereo(samples_to_generate);
    channels[8]->RenderStereo(samples_to_generate);
    channels[9]->RenderStereo(samples_to_generate);
    channels[10]->RenderStereo(samples_to_generate);
    channels[11]->RenderStereo(samples_to_generate);
    channels[12]->RenderStereo(samples_to_generate);
    channels[13]->RenderStereo(samples_to_generate);
    channels[14]->RenderStereo(samples_to_generate);
    channels[15]->RenderStereo(samples_to_generate);

    for (uint32_t i=0;i<samples_to_generate<<1;++i) {
      buffers.main[i] = buffers.channels[0][i]+buffers.channels[1][i]+buffers.channels[2][i]+buffers.channels[3][i] +
                        buffers.channels[4][i]+buffers.channels[5][i]+buffers.channels[6][i]+buffers.channels[7][i] +
                        buffers.channels[8][i]+buffers.channels[9][i]+buffers.channels[10][i]+buffers.channels[11][i] +
                        buffers.channels[12][i]+buffers.channels[13][i]+buffers.channels[14][i]+buffers.channels[15][i];

      buffers.aux[i]  = buffers.channels[0][i]*channels[0]->aux_send   + buffers.channels[1][i]*channels[1]->aux_send   + buffers.channels[2][i]*channels[2]->aux_send   + buffers.channels[3][i]*channels[3]->aux_send   +
                        buffers.channels[4][i]*channels[4]->aux_send   + buffers.channels[5][i]*channels[5]->aux_send   + buffers.channels[6][i]*channels[6]->aux_send   + buffers.channels[7][i]*channels[7]->aux_send   +
                        buffers.channels[8][i]*channels[8]->aux_send   + buffers.channels[9][i]*channels[9]->aux_send   + buffers.channels[10][i]*channels[10]->aux_send + buffers.channels[11][i]*channels[11]->aux_send +
                        buffers.channels[12][i]*channels[12]->aux_send + buffers.channels[13][i]*channels[13]->aux_send + buffers.channels[14][i]*channels[14]->aux_send + buffers.channels[15][i]*channels[15]->aux_send;

      ++i;
      buffers.main[i] = buffers.channels[0][i]+buffers.channels[1][i]+buffers.channels[2][i]+buffers.channels[3][i] +
                        buffers.channels[4][i]+buffers.channels[5][i]+buffers.channels[6][i]+buffers.channels[7][i] +
                        buffers.channels[8][i]+buffers.channels[9][i]+buffers.channels[10][i]+buffers.channels[11][i] +
                        buffers.channels[12][i]+buffers.channels[13][i]+buffers.channels[14][i]+buffers.channels[15][i];


      buffers.aux[i]  = buffers.channels[0][i]*channels[0]->aux_send   + buffers.channels[1][i]*channels[1]->aux_send   + buffers.channels[2][i]*channels[2]->aux_send   + buffers.channels[3][i]*channels[3]->aux_send   +
                        buffers.channels[4][i]*channels[4]->aux_send   + buffers.channels[5][i]*channels[5]->aux_send   + buffers.channels[6][i]*channels[6]->aux_send   + buffers.channels[7][i]*channels[7]->aux_send   +
                        buffers.channels[8][i]*channels[8]->aux_send   + buffers.channels[9][i]*channels[9]->aux_send   + buffers.channels[10][i]*channels[10]->aux_send + buffers.channels[11][i]*channels[11]->aux_send +
                        buffers.channels[12][i]*channels[12]->aux_send + buffers.channels[13][i]*channels[13]->aux_send + buffers.channels[14][i]*channels[14]->aux_send + buffers.channels[15][i]*channels[15]->aux_send;
      
    }


  /*__m128 channel_buffer[4];
  //render and mix each chanel
  auto mainbufptr = buffers.main;
  memset(mainbufptr,0,sizeof(real_t)*samples_to_generate*2);
  auto bufptr = (real_t* __restrict)channel_buffer;
  for (uint32_t j=0;j<kChannelCount;j+=4) { //mix all 16 channels

    /*
      for (uint32_t i=0;i<samples_to_generate;++i) {
        __m128 s[2] = {0,0};
        auto sample = (float* __restrict)s;
        for (int n=0;n<Polyphony;++n) {
          if (channels[j]->silence_ == false)
            sample[0] += channels[j+0]->instrument_->Tick(channels[j+0]->instrument_data_,n);
          if (channels[j+1]->silence_ == false)
            sample[1] += channels[j+1]->instrument_->Tick(channels[j+1]->instrument_data_,n);
          if (channels[j+2]->silence_ == false)
            sample[2] += channels[j+2]->instrument_->Tick(channels[j+2]->instrument_data_,n);
          if (channels[j+3]->silence_ == false)
            sample[3] += channels[j+3]->instrument_->Tick(channels[j+3]->instrument_data_,n);
          if (channels[j+4]->silence_ == false)
            sample[4] += channels[j+4]->instrument_->Tick(channels[j+4]->instrument_data_,n);
          if (channels[j+5]->silence_ == false)
            sample[5] += channels[j+5]->instrument_->Tick(channels[j+5]->instrument_data_,n);
          if (channels[j+6]->silence_ == false)
            sample[6] += channels[j+6]->instrument_->Tick(channels[j+6]->instrument_data_,n);
          if (channels[j+7]->silence_ == false)
            sample[7] += channels[j+7]->instrument_->Tick(channels[j+7]->instrument_data_,n);
        }
        __m128 pan[4],amp[2];
        auto pan_array = (float* __restrict)pan;
        auto amp_array = (float* __restrict)amp;

        pan_array[0] = channels[j]->pan_l;
        pan_array[1] = channels[j+1]->pan_l;
        pan_array[2] = channels[j+2]->pan_l;
        pan_array[3] = channels[j+3]->pan_l;
        pan_array[4] = channels[j+4]->pan_l;
        pan_array[5] = channels[j+5]->pan_l;
        pan_array[6] = channels[j+6]->pan_l;
        pan_array[7] = channels[j+7]->pan_l;
        pan_array[8] = channels[j]->pan_r;
        pan_array[9] = channels[j+1]->pan_r;
        pan_array[10] = channels[j+2]->pan_r;
        pan_array[11] = channels[j+3]->pan_r;
        pan_array[12] = channels[j+4]->pan_r;
        pan_array[13] = channels[j+5]->pan_r;
        pan_array[14] = channels[j+6]->pan_r;
        pan_array[15] = channels[j+7]->pan_r;

        amp_array[0] = channels[j]->amplification_;
        amp_array[1] = channels[j+1]->amplification_;
        amp_array[2] = channels[j+2]->amplification_;
        amp_array[3] = channels[j+3]->amplification_;
        amp_array[4] = channels[j+4]->amplification_;
        amp_array[5] = channels[j+5]->amplification_;
        amp_array[6] = channels[j+6]->amplification_;
        amp_array[7] = channels[j+7]->amplification_;
        
        auto left1 = _mm_mul_ps(pan[0],s[0]);
        auto left2 = _mm_mul_ps(pan[1],s[1]);
        left1 = _mm_mul_ps(left1,amp[0]);
        left2 = _mm_mul_ps(left2,amp[1]);
        
        auto right1 = _mm_mul_ps(pan[2],s[0]);
        auto right2 = _mm_mul_ps(pan[3],s[1]);
        right1 = _mm_mul_ps(right1,amp[0]);
        right2 = _mm_mul_ps(right2,amp[1]);

        __m128 result1,result2;
        result1 = _mm_add_ps(left1,left2);
        result2 = _mm_add_ps(right1,right2);

        *mainbufptr++ = result1.m128_f32[0]+result1.m128_f32[1]+result1.m128_f32[2]+result1.m128_f32[3];
        *mainbufptr++ = result2.m128_f32[0]+result2.m128_f32[1]+result2.m128_f32[2]+result2.m128_f32[3];

      }
    */
    /*
    channels[j+0]->Render(samples_to_generate);
    channels[j+1]->Render(samples_to_generate);
    channels[j+2]->Render(samples_to_generate);
    channels[j+3]->Render(samples_to_generate);
    auto buf0 = channels[j+0]->buffer();
    auto buf1 = channels[j+1]->buffer();
    auto buf2 = channels[j+2]->buffer();
    auto buf3 = channels[j+3]->buffer();
    
    for (uint32_t i=0;i<samples_to_generate<<1;++i) {
      mainbufptr[i] += buf0[i]+buf1[i]+buf2[i]+buf3[i];
      ++i;
      mainbufptr[i] += buf0[i]+buf1[i]+buf2[i]+buf3[i];
    }
  }*/
}

midi::Event* MidiSynth::GetNextEvent() {
  midi::Event* result = nullptr;
  
  if (mode_ == kModeSequencer) {
    uint32_t track_index=0xFFFFFFFF;
    uint32_t event_index=0xFFFFFFFF;
    uint32_t ticks_to_next_event = 0xFFFFFFFF;

	  for (int i = 0; i < track_count_; i++) {
		  if (
			  tracks[i].ticks_to_next_event != 0xFFFFFFFF
			  && (ticks_to_next_event == 0xFFFFFFFF || tracks[i].ticks_to_next_event < ticks_to_next_event)
		  ) {
			  ticks_to_next_event = tracks[i].ticks_to_next_event;
			  track_index = i;
			  event_index = tracks[i].event_index;
		  }
	  }

	  if (track_index != 0xFFFFFFFF) {
		  /* consume event from that track */
		  result = &tracks[track_index].event_sequence[event_index];
		  if ((event_index+1) < tracks[track_index].event_count) {
			  tracks[track_index].ticks_to_next_event += tracks[track_index].event_sequence[event_index+1].deltaTime;
		  } else {
			  tracks[track_index].ticks_to_next_event = 0xFFFFFFFF;
		  }
		  ++tracks[track_index].event_index;
		  /* advance timings on all tracks by ticksToNextEvent */
		  for (int i = 0; i < track_count_; i++) {
			  if (tracks[i].ticks_to_next_event != 0xFFFFFFFF) {
				  tracks[i].ticks_to_next_event -= ticks_to_next_event;
			  }
		  }

	    auto beatsToNextEvent = ticks_to_next_event / double(ticks_per_beat);
		  auto secondsToNextEvent = beatsToNextEvent / (bpm / 60);
		  samples_to_next_event += uint32_t(secondsToNextEvent * sample_rate_);
	  } else {
		  result = nullptr;
		  samples_to_next_event = 0xFFFFFFFF;
      /*if (player_->mode() == Player::kModeLoop) {
        player_->Reset();
      } else {
        player_->RequestStop();
      }*/
      player_->Stop();
	  }
  } else if (mode_ == kModeTest) {
    EnterCriticalSection(&me_lock);
    result = &tracks[0].event_sequence[tracks[0].event_index];
    tracks[0].ticks_to_next_event = 0;

	  auto beatsToNextEvent = tracks[0].ticks_to_next_event / double(ticks_per_beat);
		auto secondsToNextEvent = beatsToNextEvent / (bpm / 60);
		samples_to_next_event += uint32_t(secondsToNextEvent * sample_rate_);
    tracks[0].event_index = (tracks[0].event_index + 1) % tracks[0].event_count;
    LeaveCriticalSection(&me_lock);
  }
  return result;
}

void MidiSynth::HandleEvent(midi::Event* event) {
  (this->*(midi_main_event_handlers[event->type]))(event);
};

void MidiSynth::MidiEventUnknown(midi::Event* event) {
  
}

void MidiSynth::MidiEventMeta(midi::Event* event) {
  (this->*(midi_meta_event_handlers[event->subtype]))(event);
}

void MidiSynth::MidiEventSetTempo(midi::Event* event) {
  bpm = 60000000.0 / event->data.tempo.microsecondsPerBeat;
}

void MidiSynth::MidiEventChannel(midi::Event* event) {
  (this->*(midi_channel_event_handlers[event->subtype]))(event);
}

void MidiSynth::MidiEventNoteOn(midi::Event* event) {
  auto channel = channels[event->channel];
  real_t frequency = chromatic_scale_freq[(Notes)event->data.note.noteNumber];
  channel->AddNote(event->data.note.noteNumber,frequency,event->data.note.velocity / 127.0f);
}

void MidiSynth::MidiEventNoteOff(midi::Event* event) {
  auto channel = channels[event->channel];
  channel->RemoveNote(event->data.note.noteNumber,event->data.note.velocity / 127.0f);
}

void MidiSynth::MidiEventProgramChange(midi::Event* event) {
  auto channel = channels[event->channel];
  if (event->channel == 9) {
    channel->set_instrument(percussion_instr);
  } else {
		channel->set_instrument(instr[event->data.program.number]);
  }
  channel->set_silence(false);
}

void MidiSynth::MidiEventController(midi::Event* event) {
  auto channel = channels[event->channel];
  if (event->data.controller.type == 0x06) { //data entry slider coarse
    if (channel->param_coarse == 0) {
      channel->des_coarse = event->data.controller.value;
      real_t p = ((real_t)channel->des_coarse*0.5f) + ((real_t)channel->des_fine / 100.0f);
      channel->set_pitch_bend_range(p);
    }
  } else if (event->data.controller.type == 0x26) { //data entry slider fine
    if (channel->param_fine == 0) {
      channel->des_fine = event->data.controller.value;
      real_t p = ((real_t)channel->des_coarse) + ((real_t)channel->des_fine / 100.0f);
      channel->set_pitch_bend_range(p);
    }
  } else if (event->data.controller.type == 0x0A) { //pan
    real_t pan = event->data.controller.value / 127.0f;
    channel->set_panning(pan);
  } else if (event->data.controller.type == 0x49) { //sound attack time
    real_t time = event->data.controller.value / 127.0f;
    //channel->adsr[0].set
  } else if (event->data.controller.type == 0x07) { //main volume
    real_t vol = event->data.controller.value / 127.0f;
    channel->set_amplification(vol);
  } else if (event->data.controller.type == 0x64) { //RPN coarse
    channel->param_coarse = event->data.controller.value;
  } else if (event->data.controller.type == 0x65) { //RPN fine
    channel->param_fine = event->data.controller.value;
  }
}

void MidiSynth::MidiEventPitchBend(midi::Event* event) {
  auto channel = channels[event->channel];
  real_t amount = ((event->data.pitch.value - 8192) / 16384.0f)*channel->pitch_bend_range();
  channel->SetPitchBend(amount);
}

}
}

