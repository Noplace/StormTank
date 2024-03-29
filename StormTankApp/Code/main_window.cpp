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
#include "stormtankapp.h"
#include "music.h"

float delay;
bool key_down[256];
int key_state[256];

  const char key_maps[10] = {
    'Z','X','C','V','B','N','M','<','>','?'
  };


MainWindow::MainWindow() : core::windows::Window() {
  current_scene = nullptr;
  memset(&key_down,0,sizeof(key_down));
  memset(&key_state,0,sizeof(key_state));
  sonant_synth_ = null;
}

MainWindow::~MainWindow() {

}


void MainWindow::Initialize() {
  PrepareClass("StormTrooperMainWindow");
  this->Create();
  SetClientSize(640,480);
  Center();
  //SetMenu(LoadMenu(NULL,MAKEINTRESOURCE(IDR_MAINMENU)));
  memset(&timing,0,sizeof(timing));
  timer_.Calibrate();

  gfx_ = new ve::ContextD3D11();
  gfx_->Initialize();
  gfx_->CreateDisplay(handle());


  /*
  //for 2d
  ve::InputElement gielements[] = 
  {
      {0, 0 , D3DDECLTYPE_FLOAT3  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, 12, D3DDECLTYPE_FLOAT2  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR   , 0},
      {0, 24, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES   , 0},
      D3DDECL_END()
  };

  gfx_->CreateInputLayout(gielements,resources.gfx.input_layout);
  gfx_->SetInputLayout(resources.gfx.input_layout);


  */
  /*
  {
    uint8_t* data;
    size_t size;
    core::io::ReadWholeFileBinary("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Shaders\\main2d.o",&data,size);
    if (data != nullptr) {
      gfx_->CreateEffectInterface((uint8_t*)data,size,(void**)&resources.gfx.effect);
    }
    core::io::DestroyFileBuffer(&data);

    resources.gfx.viewprojection = resources.gfx.effect->GetParameterByName(0,"viewprojection");
    resources.gfx.world = resources.gfx.effect->GetParameterByName(0,"world");

  }*/


  audio_interface_ = new audio::output::DirectSound();
  audio_interface_->set_window_handle(handle());
  audio_interface_->set_buffer_size((44100/10)*4);
  audio_interface_->Initialize(44100,2,16);


  //player_.set_audio_interface(audio_interface_);
  //player_.Initialize();
  
  //sonant_synth_ = new audio::synth::SonantSynth();
 // sonant_synth_->set_mode(audio::synth::SonantSynth::kModeSequencer);

 
  midi_synth_ = new audio::synth::MidiSynth();
  midi_synth_->set_mode(audio::synth::MidiSynth::kModeSequencer);
  synth_player_ = new audio::synth::Player();
  synth_player_->set_audio_interface(audio_interface_);
  synth_player_->set_synth(midi_synth_);
  //synth_player_->set_mode(audio::synth::Player::kModeLoop);
  synth_player_->Initialize();

  //sonant_synth_->LoadSong(&songdata, _4K_SONANT_ENDPATTERN_, _4K_SONANT_ROWLEN_);

  
  delay = 400.0f;
  midi_synth_->delay_unit.set_delay_ms(delay);
  
  //loading_scene.Initialize(this);
  //intro_scene.Initialize(this);
  //current_scene = &loading_scene;
  SwitchScene(&scenes.loading);//demo
  Show();
  timing.prev_cycles = timer_.GetCurrentCycles();
  //SetThreadAffinityMask(GetCurrentThread(),
}

void MainWindow::Step() {
  //synth_player_->Tick();
  //Sleep(100);
  const double dt =  1000.0 / 60.0;//16.667f;
  timing.current_cycles = timer_.GetCurrentCycles();
  double time_span =  (timing.current_cycles - timing.prev_cycles) * timer_.resolution();
  if (time_span > 250.0) //clamping time
    time_span = 250.0;

  timing.span_accumulator += time_span;
  while (timing.span_accumulator >= dt) {
    timing.span_accumulator -= dt;
    current_scene->Update(dt);
    ++timing.fps_counter;
  }


  for (int i=0;i<10;++i) {

    if (key_down[key_maps[i]] == true && key_state[key_maps[i]] == 0) {
      auto midi_event = midi_synth_->BeginPrepareNextEvent();
      midi_event->channel = 0;
      midi_event->data.note.noteNumber = (audio::synth::kNoteC3+i);
      midi_event->data.note.velocity = 127;
      midi_event->type = midi::kEventTypeChannel;
      midi_event->subtype = midi::kEventSubtypeNoteOn;
      midi_event->track = 0;
      midi_event->deltaTime = 0;
      midi_synth_->EndPrepareNextEvent();
      //synth_player_->Tick();
      OutputDebugString("sent note on\n");
      key_state[key_maps[i]] = 1;
    } else if (key_down[key_maps[i]] == false && key_state[key_maps[i]] == 1) {
      auto midi_event = midi_synth_->BeginPrepareNextEvent();
      midi_event->channel = 0;
      midi_event->data.note.noteNumber = (audio::synth::kNoteC3+i);
      midi_event->data.note.velocity = 127;
      midi_event->type = midi::kEventTypeChannel;
      midi_event->subtype = midi::kEventSubtypeNoteOff;
      midi_event->track = 0;
      midi_event->deltaTime = 0;
      midi_synth_->EndPrepareNextEvent();
      //synth_player_->Tick();
      OutputDebugString("sent note off\n");
      key_state[key_maps[i]] = 0;
    }
  }

  timing.total_cycles += timing.current_cycles-timing.prev_cycles;
  timing.prev_cycles = timing.current_cycles;

   
  if (timing.render_time_span >= 16.667) {
    //gfx_->Begin();
  //  current_scene->Draw();
   // gfx_->End();
   // gfx_->Render();
    timing.render_time_span = 0;
  }

  timing.fps_time_span += time_span;
  timing.render_time_span += time_span;
  
  if (timing.fps_time_span >= 1000.0) {
    timing.fps = timing.fps_counter;
    timing.fps_counter = 0;
    timing.fps_time_span = 0;

    //char caption[256];
    //sprintf(caption,"Freq : %0.2f MHz",nes.frequency_mhz());
    //sprintf(caption,"CPS: %llu ",nes.cycles_per_second());
    
    //sprintf_s(caption,"FPS: %02d",timing.fps);
    //SetWindowText(handle(),caption);
 
  }
  

}

int MainWindow::OnCreate(WPARAM wParam,LPARAM lParam) {
  return 0;
}

int MainWindow::OnDestroy(WPARAM wParam,LPARAM lParam) {
  //intro_scene.Deinitialize();
  //loading_scene.Deinitialize();

  SwitchScene(nullptr);
  synth_player_->Deinitialize();
  SafeDelete(&synth_player_);
  SafeDelete(&midi_synth_);
  SafeDelete(&sonant_synth_);
  //player_.Deinitialize();
  audio_interface_->Deinitialize();
  SafeDelete(&audio_interface_);

  //gfx_->DestroyEffectInterface((void**)&resources.gfx.effect);
  //gfx_->DestoryInputLayout(resources.gfx.input_layout);
  gfx_->Deinitialize();
  SafeDelete(&gfx_);
  PostQuitMessage(0);
  return 0;
}

int MainWindow::OnKeyDown(WPARAM wParam,LPARAM lParam) {
  if (wParam == 'A') {
    synth_player_->Pause();
  }

  if (wParam == 'S') {
    synth_player_->Play();
  }

  if (wParam == 'D') {
    delay = min(1000.0f,delay + 1.0f);
    midi_synth_->delay_unit.set_delay_ms(delay);
  }

  if (wParam == 'F') {
    delay = max(0.0f,delay - 1.0f);
    midi_synth_->delay_unit.set_delay_ms(delay);
  }

  key_down[wParam&0xFF] = true;
  if (wParam == 'Z') {
    
  }


  return 0;
}

int MainWindow::OnKeyUp(WPARAM wParam,LPARAM lParam) {
  key_down[wParam&0xFF] = false;

  return 0;
}

int MainWindow::OnActivate(WPARAM wParam,LPARAM lParam) {
  if ((wParam&0xFFFF) == WA_INACTIVE) {
   // synth_player_->Pause();
  } else {
    //synth_player_->Play();
  }
  return 0;
}