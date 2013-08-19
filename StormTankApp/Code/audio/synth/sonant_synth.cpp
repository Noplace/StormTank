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
#include "sonant_synth.h"


//#define OUTPUT_EVENTS

namespace audio {
namespace synth {


void SonantSynth::Initialize() {
  if (initialized_ == true)
    return;


  auto buffer_length_ms_ = 400.0f;//400ms
  auto buffer_size = uint32_t(sample_rate_*2*buffer_length_ms_*0.001);
  buffers.aux_size = buffers.main_size = buffer_size;
  buffers.main = new real_t[buffers.main_size]; //44100 samples & channels , check with player later on!
  buffers.aux = new real_t[buffers.aux_size]; //44100 samples & channels , check with player later on!

  for (int i=0;i<8;++i) {
    prog[i] = new instruments::SonantProgram();
    prog_data[i] = prog[i]->NewInstrumentData();
    prog[i]->set_instrument_data(prog_data[i]);
  }
  samples_to_next_event = 0;

  //initialize all available instruments
 
  InitializeCriticalSection(&me_lock);
  initialized_ = true;
}

void SonantSynth::Deinitialize() {
  if (initialized_ == false)
    return;
  DeleteCriticalSection(&me_lock);

  for (int i=0;i<kChannelCount;++i) {
    SafeDeleteArray(&buffers.channels[i]);
  }
  for (int i=0;i<8;++i) {
    SafeDelete(&prog_data[i]);
    SafeDelete(&prog[i]);
  }
  SafeDeleteArray(&buffers.aux);
  SafeDeleteArray(&buffers.main);
  initialized_ = false;
}

void SonantSynth::LoadSong(SonantSong* song, int endpattern, int rowlen) {
  song_ = song;
  endpattern_ = endpattern;
  rowlen_ = rowlen;
  for (int i=0;i<8;++i) {
    memcpy(&prog[i]->data,&song_->i[i],sizeof(prog[i]->data));
  }

}

void SonantSynth::RenderSamplesStereo(uint32_t samples_count, real_t* data_out) {

  static int currentpos;


    unsigned int i,b,p,r;
    unsigned char n;
    i = 8;
    //do {
        SonantInstrument *instrument = &song_->i[--i];
    //} while(1);


}


void SonantSynth::GenerateIntoBufferStereo(uint32_t samples_to_generate, real_t* data_out, uint32_t& data_offset) {
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

void SonantSynth::MixChannelsStereo(uint32_t samples_to_generate) {
  
 /*
    for (uint32_t i=0;i<samples_to_generate<<1;++i) {
      buffers.main[i] = buffers.channels[0][i]+buffers.channels[1][i]+buffers.channels[2][i]+buffers.channels[3][i] +
                        buffers.channels[4][i]+buffers.channels[5][i]+buffers.channels[6][i]+buffers.channels[7][i] +
                        buffers.channels[8][i]+buffers.channels[9][i]+buffers.channels[10][i]+buffers.channels[11][i] +
                        buffers.channels[12][i]+buffers.channels[13][i]+buffers.channels[14][i]+buffers.channels[15][i];



      ++i;
      buffers.main[i] = buffers.channels[0][i]+buffers.channels[1][i]+buffers.channels[2][i]+buffers.channels[3][i] +
                        buffers.channels[4][i]+buffers.channels[5][i]+buffers.channels[6][i]+buffers.channels[7][i] +
                        buffers.channels[8][i]+buffers.channels[9][i]+buffers.channels[10][i]+buffers.channels[11][i] +
                        buffers.channels[12][i]+buffers.channels[13][i]+buffers.channels[14][i]+buffers.channels[15][i];


      
    }*/


}



}
}

