#include "../stormtankapp.h"
//#include "../song/song2.h"

   

int LoadingScene::Initialize(MainWindow* win) {
  graphics::Scene::Initialize(win->gfx());
  BaseScene::Initialize(win);
  gfx = (graphics::ContextD3D9*)context_;

  camera_.Initialize(gfx);
  camera_.Ortho2D(0,0,gfx->width(),gfx->height());
  gfx->SetViewport(0,0,gfx->width(),gfx->height(),0,1000);
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



  //gfx->CompileShaderFromMemory(



  arc1.Initialize(gfx);
  arc1.SetTopLeft(200,200);
  arc1.SetColor0(0xffffffff);
  arc1.SetColor1(0x000000ff);
  arc1.SetParams(50,0,2,10);
  arc1.Construct();


  loading_bar.Initialize(gfx);
  loading_bar.SetTopLeft(0,0);
  loading_bar.SetParams(200,20);
  loading_bar.SetColors(XMCOLOR(0xff00ff00),XMCOLOR(0xff00ff00),XMCOLOR(0xff00ff00),XMCOLOR(0xff00ff00));
  loading_bar.Construct();





  loading_progress = 0;
  event1 = CreateEvent(nullptr,false,false,nullptr);
  thread_handle = CreateThread(nullptr,0,static_cast<LPTHREAD_START_ROUTINE>(LoadingThread),this,0,(LPDWORD)&thread_id);//CREATE_SUSPENDED
  //WaitForSingleObject(event1,INFINITE);

  
   

  return S_OK;
}

int LoadingScene::Deinitialize() {
  CloseHandle(thread_handle);
  return S_OK;
}




//XMMATRIX world_mat;

int LoadingScene::Update(double dt) {


  if (WaitForSingleObject(event1,0) == WAIT_OBJECT_0) {
    win->SwitchScene(&win->intro_scene);
  }

  static float theta =0;
  float y = 0;//100+sin(theta)*100;
  //D3DXMATRIX matIdentity;
	//D3DXMatrixIdentity(&matIdentity);
  //world_mat = XMMatrixAffineTransformation2D(XMVectorSet(1,1,0,0),XMVectorSet(0,0,0,0),0,XMVectorSet(0,y,0,0));
  

  theta += 0.3f;

    //sn.Generate(tex_data0,256,256,theta,0,5);
    //gfx->CopyToTexture(texture,tex_data0,0,256,graphics::TexturePoint(0,0),graphics::TexturePoint(0,0),256,256);

  
  arc1.SetRotate(theta);
  arc1.BuildTransform();
  int w = gfx->width();
  loading_bar.SetParams((loading_progress*(float)w)/100.0f,20);
  loading_bar.Construct();
  loading_bar.BuildTransform();

  return S_OK;
}

int LoadingScene::Draw() {

  //uint32_t offsets[1] = {0};
  //uint32_t strides[1] = {sizeof(graphics::shape::Vertex)};
  //gfx->SetVertexBuffers(0,1,&vb,strides,offsets);
  //gfx->SetPrimitiveTopology(D3DPT_TRIANGLESTRIP);
  //gfx->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&world_mat);
  //gfx->device()->SetTexture(0,(IDirect3DTexture9*)texture.data_pointer);
  //gfx->Draw(4,0);
  //gfx->device()->SetTexture(0,nullptr);
  //gfx->Draw(4,4);
  
  gfx->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&arc1.world());
  arc1.Draw();
  gfx->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&loading_bar.world());
  loading_bar.Draw();

  return S_OK;
}

DWORD WINAPI LoadingScene::LoadingThread(LPVOID lpThreadParameter) {
  auto self = (LoadingScene*)lpThreadParameter;

  ResetEvent(self->event1);
  
  self->win->midi_synth().LoadMidi("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Content\\MoonLte3.mid");
  //self->win->midi_synth().LoadMidi("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Content\\btmnjstg.mid");
  //self->win->midi_synth().LoadMidi("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Content\\minute_waltz.mid");
  //self->win->midi_synth().LoadMidi("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Content\\smb109.mid");
  //self->win->midi_synth().LoadMidi("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Content\\liangzhu.mid");
  
  //self->win->player().LoadSong(&songdata,_4K_SONANT_ROWLEN_,_4K_SONANT_ENDPATTERN_,&self->loading_progress);
  Sleep(400);
  self->win->player2().Play();
  SetEvent(self->event1);

  return S_OK;
}