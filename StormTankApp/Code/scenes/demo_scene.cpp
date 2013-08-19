#include "../stormtankapp.h"

struct {
  graphics::Buffer vb;
  XMMATRIX world;
}object;

int DemoScene::Initialize(MainWindow* win) {
  BaseScene::Initialize(win);
  gfx = (graphics::ContextD3D9*)win->gfx();
  res.cam2d.Initialize(gfx);
  res.cam2d.Ortho2D(0,0,640,480);
  
  gfx->SetViewport(0,0,640,480,0,1);
  res.cam3d.Initialize(gfx);
  //res.cam3d.Perspective();
  res.cam3d.projection() =  XMMatrixPerspectiveFovLH( XM_PIDIV4, 640.0f/480.0f, 1.0f, 100.0f );
  res.cam3d.view() = XMMatrixLookAtLH(XMVectorSet(0,1.0f,-20.0f,0),XMVectorSet(0,0,1.0f,0),XMVectorSet(0,1.0f,0,0));
  win->resources.gfx.effect->SetMatrix(win->resources.gfx.viewprojection,(D3DXMATRIX*)&(res.cam2d.viewprojection()));
  
  res.font.Initialize(gfx);
  res.font.Construct(16,"Arial");
  res.font.SetTopLeft(0,0);
  res.font.SetSize(640,480);
  sprintf_s(res.help_str,"");


  {
    static graphics::shape::Vertex v[4] = {
      graphics::shape::Vertex(XMFLOAT3(-10,-10,0),XMFLOAT2(0,0),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(10,-10,0),XMFLOAT2(1,0),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(-10,10,0),XMFLOAT2(0,1),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(10,10,0),XMFLOAT2(1,1),XMCOLOR(1,1,1,1),0),
    };


        
    memset(&object.vb,0,sizeof(object.vb));
    object.vb.description.byte_width = sizeof(v);
    object.vb.description.bind_flags = D3D11_BIND_VERTEX_BUFFER;
    object.vb.description.usage = 0;//D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
    gfx->CreateBuffer(object.vb,v);

    object.world = XMMatrixIdentity();

  }

  win->resources.gfx.effect->SetTechnique(win->resources.gfx.effect->GetTechnique(0));

  main_time_span = 0;
  return S_OK;
}

int DemoScene::Deinitialize() {
  res.font.Deinitialize();
  res.cam2d.Deinitialize();
  res.cam3d.Deinitialize();
  return S_OK;
}

int DemoScene::Update(double dt) {
  

  win->resources.gfx.effect->SetMatrix(win->resources.gfx.world,(D3DXMATRIX*)&(object.world));



  {
    time_t t = time(0);
    auto tsecs   = t % 60;
    auto tmins   = (t / 60) % 60;
    auto thrs   = (t / (60*60)) % 24;
    auto tdays   = (t / (60*60*24)) % 365;
    auto tyears   = 1970 + (t / (60*60*24*365));
    extern float delay;
    sprintf_s(res.help_str,"Time: %d:%03d:%02d:%02d:%02d\nScene Thread Time: %03.2f",
    tyears,tdays,thrs,tmins,tsecs,
    main_time_span);


    res.font.Update();
  }

  main_time_span += dt;
  return S_OK;
}

int DemoScene::Draw() {
  gfx->Clear(1,1);
  {
    auto effect = win->resources.gfx.effect;
    UINT passes;
    effect->Begin(&passes,0);
    effect->BeginPass(1);

    static const uint32_t offsets[1] = {0};
    static const uint32_t strides[1] = {sizeof(graphics::shape::Vertex)};
    gfx->SetVertexBuffers(0,1,&object.vb,strides,offsets);
    gfx->SetPrimitiveTopology(D3DPT_TRIANGLESTRIP);
    gfx->Draw(4,0);

    effect->EndPass();
    effect->End();
  }
  {
    res.font.Draw(res.help_str,-1,DT_LEFT);
  }
  return S_OK;
}