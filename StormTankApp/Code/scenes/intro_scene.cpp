#include "../stormtankapp.h"
#include "../song/song1.h"

pgt::generators::SimplexNoise sn;
XMCOLOR* tex_data0;


/*
DWORD WINAPI IntroScene::playMusic(LPVOID lpThreadParameter) {
  utilities::Timer<double> timer;
  auto self = (IntroScene*)lpThreadParameter;
  uint64_t current_cycles=0,prev_cycles=timer.GetCurrentCycles();
  double span_accumulator = 0;
  double time_lapse = 0;
  double dt = 16.66666667;


  short* buffer = sonant_init(songdata,_4K_SONANT_ROWLEN_,_4K_SONANT_ENDPATTERN_);

  while (1) {
    current_cycles = timer.GetCurrentCycles();
    double time_span =  (current_cycles - prev_cycles) * timer.resolution();
    if (time_span >= 250.0)
      time_span = 250.0;
    span_accumulator += time_span;
    while (span_accumulator >= dt) {
      span_accumulator -= dt;

      uint32_t buffer_size = 0.050 * 44100 * 2 * 2;
      if (time_lapse >= 50) {
        //auto samples = audio::track1_synth.SamplesPerWholeNote();
        /*auto samples = audio::track1_synth.SamplesPerTimeMS(time_lapse);
        double freq = audio::track1_synth.NoteFreq((synth::Notes)audio::note);
        audio::lfo.setRate(freq);
        audio::offset = 0;
        for (uint32_t i=0;i<samples;++i) {
          auto sample = short(audio::lfo.tick() * 32767.0f);
          audio::output_buffer[audio::offset++] = sample;
          audio::output_buffer[audio::offset++] = sample;
        }*/
        //self->audio_interface_->Write(audio::output_buffer,audio::offset*sizeof(short));
    /*    
        self->audio_interface_->Write(buffer,buffer_size);
        buffer += buffer_size>>1;
        //audio::song_time_ms += time_lapse;
        //if ((uint32_t(audio::song_time_ms) % 1000) == 0) {
        //  audio::note++;
       // }

        time_lapse = 0;
      }
      time_lapse += dt;

    }

    prev_cycles = current_cycles;
  }

  

  return 0;
}*/
   

int IntroScene::Initialize(MainWindow* win) {
  Scene::Initialize(win->gfx());
  gfx = (graphics::ContextD3D9*)context_;


  camera_.Initialize(gfx);
  camera_.Ortho2D(0,0,640,480);
  gfx->SetViewport(0,0,640,480,0,1000);
  gfx->SetCamera(&camera_);
  
  D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
  gfx->device()->SetTransform(D3DTS_WORLD,&matIdentity);
	gfx->device()->SetRenderState(D3DRS_ZENABLE,  D3DZB_TRUE );
	gfx->device()->SetRenderState(D3DRS_LIGHTING, 0 );
	gfx->device()->SetRenderState(D3DRS_ALPHABLENDENABLE,1);
  

	gfx->device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gfx->device()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );  
	gfx->device()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	gfx->device()->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	gfx->device()->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	gfx->device()->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_MODULATE );

  arc1.Initialize(gfx);
  arc1.SetTopLeft(200,200);
  arc1.SetColor0(0xffffffff);
  arc1.SetColor1(0x000000ff);
  arc1.SetParams(120,0,2,10);
  arc1.Construct();
  
  {
      /*GfxVertex v[8] = {
      {0,0,0,0xffffffff,0,0},
      {100,0,0,0xffffFFFF,1,0},
      {0,100,0,0xffffFFFF,0,1},
      {100,100,0,0xffffffFF,1,1},
      {200+0,0,0,0xffffffff,0,0},
      {200+100,0,0,0xffff0000,0,0},
      {200+0,100,0,0xffff00ff,0,0},
      {200+100,100,0,0xffffff00,0,0}
    };*/
    graphics::shape::Vertex v[8] = {
      graphics::shape::Vertex(XMFLOAT3(0,0,0),XMFLOAT2(0,0),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(256,0,0),XMFLOAT2(1,0),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(0,256,0),XMFLOAT2(0,1),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(256,256,0),XMFLOAT2(1,1),XMCOLOR(1,1,1,1),0),

      graphics::shape::Vertex(XMFLOAT3(200+0,0,0),XMFLOAT2(0,0),XMCOLOR(1,1,1,0),0),
      graphics::shape::Vertex(XMFLOAT3(200+100,0,0),XMFLOAT2(0,0),XMCOLOR(1,1,0,0),0),
      graphics::shape::Vertex(XMFLOAT3(200+0,100,0),XMFLOAT2(0,0),XMCOLOR(1,1,0,0),0),
      graphics::shape::Vertex(XMFLOAT3(200+100,100,0),XMFLOAT2(0,0),XMCOLOR(1,1,1,0),0),
    };
    
    memset(&vb,0,sizeof(vb));
    vb.description.byte_width = sizeof(graphics::shape::Vertex)*8;
    vb.description.usage = 0;//D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
    gfx->CreateBuffer(vb,v);

    
    XMCOLOR* tex_data1,*tex_data2,*tex_data3;
    tex_data0 = new XMCOLOR[256*256];
    tex_data1 = new XMCOLOR[256*256];
    tex_data2 = new XMCOLOR[256*256];
    tex_data3 = new XMCOLOR[256*256];
    sn.Generate(tex_data0,256,256,0,0,1);
    gfx->CreateTexture(256,256,0,0,texture);
    pgt::generators::ChaldniPlates::Generate(tex_data0,256,256);
    pgt::generators::XOR::Generate(tex_data1,256,256);
    pgt::generators::Random::Generate(tex_data2,256,256);
    pgt::layers::Blend::Additive(tex_data3,tex_data1,tex_data2,256,256,0.5);
    gfx->CopyToTexture(texture,tex_data0,0,256,graphics::TexturePoint(0,0),graphics::TexturePoint(0,0),256,256);
    delete [] tex_data3;
    delete [] tex_data2;
    delete [] tex_data1;
    



    //audio::thread = CreateThread(nullptr,0,static_cast<LPTHREAD_START_ROUTINE>(playMusic),this,CREATE_SUSPENDED,(LPDWORD)&audio::thread_id);
    //ResumeThread(audio::thread);
    
    //int blit_sawtooth(double freq,int samples,short* out);
    //blit_sawtooth(freq,samples,audio_buffer);
   // audio_interface_->Write(audio::output_buffer,samples*2*sizeof(short));

    //ReadMidiFile("D:\\Personal\\Projects\\StormTank\\Solution\\StormTankApp\\Content\\sample.mid");

   /* {
      MidiFile  inputfile;
      inputfile.read("D:\\Personal\\Projects\\StormTank\\Solution\\StormTankApp\\Content\\smb109.mid");
      //inputfile.deltaTime();
      inputfile.absoluteTime();
      inputfile.joinTracks();
      //int eventcount = inputfile.getEventCount(0);
      //auto a0 = inputfile.getAbsoluteTickTime(0);
      

      auto ticks_per_qn = inputfile.getTicksPerQuarterNote();
      uint32_t ticks =0;
      double seconds = 0;
      synth::BasicGenerator track1_synth(44100,2,16);
      int track;
      int timeinticks;
      double timeinsecs;

      
      //for (int track = 0;track<inputfile.getTrackCount();++track) {
        int event_count = inputfile.getEventCount(0);
        
        //for (int event=0;event<event_count;++event) {
        //  auto ptr = &inputfile.getEvent(track,event);
        //  ticks += ptr->time;
        //}
        //seconds = inputfile.getTimeInSeconds(ticks);
        //short* audio_buffer = new short[2*44100*(int)ceil(seconds)];
        //ticks = 0;
        //uint32_t audio_buffer_offset = 0;
        for (int event=0;event<event_count;++event) {
          auto ptr = &inputfile.getEvent(track,event);
          if (ptr->isNoteOn()) {
            uint32_t samples = int((double(ptr->time)/ticks_per_qn)*(track1_synth.SamplesPerWholeNote()/4));
            double freq = track1_synth.NoteFreq((synth::Notes)ptr->data[1]);
            //audio_buffer_offset += track1_synth.GenerateSineWave(freq,samples,audio_buffer+audio_buffer_offset);
            int b = 1;
            char str[256];
            sprintf(str,"%d - note on %d - samples %d\n",track,ptr->data[1],samples);
            OutputDebugString(str);
          }
          if (ptr->isTimbre()) {
            char instrument = ptr->data[1];
            //set instrument
          }
          if (ptr->isTempo()) {
            char c = ptr->data[0];
            auto bpm = ptr->getTempoBPM();
            track1_synth.SetBeatsPerMinute(bpm);
            int a = 0;
          }
          ticks += ptr->time;
        }
        //audio_interface_->Write(audio_buffer,audio_buffer_offset*sizeof(short));
        //delete [] audio_buffer;
       // break;
      //}


    }*/

   
  }
  return S_OK;
}

int IntroScene::Deinitialize() {
  gfx->DestroyTexture(texture);
  delete [] tex_data0;
  return S_OK;
}




XMMATRIX world_mat;

int IntroScene::Update(double dt) {

 
  

  static float theta =0;
  float y = 0;//100+sin(theta)*100;
  //D3DXMATRIX matIdentity;
	//D3DXMatrixIdentity(&matIdentity);
  world_mat = XMMatrixAffineTransformation2D(XMVectorSet(1,1,0,0),XMVectorSet(0,0,0,0),0,XMVectorSet(0,y,0,0));
  

  theta += 0.3f;

    //sn.Generate(tex_data0,256,256,theta,0,5);
    //gfx->CopyToTexture(texture,tex_data0,0,256,graphics::TexturePoint(0,0),graphics::TexturePoint(0,0),256,256);


  arc1.BuildTransform();
  arc1.SetRotate(theta);




  return S_OK;
}

int IntroScene::Draw() {

  uint32_t offsets[1] = {0};
  uint32_t strides[1] = {sizeof(graphics::shape::Vertex)};
  gfx->SetVertexBuffers(0,1,&vb,strides,offsets);
  gfx->SetPrimitiveTopology(D3DPT_TRIANGLESTRIP);
  gfx->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&world_mat);
  gfx->device()->SetTexture(0,(IDirect3DTexture9*)texture.data_pointer);
  gfx->Draw(4,0);
  gfx->device()->SetTexture(0,nullptr);
  gfx->Draw(4,4);

  gfx->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&arc1.world());
  arc1.Draw();
  return S_OK;
}