#include "../stormtankapp.h"
#include <fstream>
#include <time.h>
pgt::generators::SimplexNoise sn;
XMCOLOR* tex_data0;
XMMATRIX world_mat;
double main_time_span;


graphics::Texture render_texture;
graphics::ResourceView render_surface;
LPDIRECT3DSURFACE9 pBackBuffer = NULL;

int IntroScene::Initialize(MainWindow* win) {
  graphics::Scene::Initialize(win->gfx());
  BaseScene::Initialize(win);
  gfx = (graphics::ContextD3D9*)context_;
  
  //gfx->CreateTexture(640,480,0,0,render_texture);
  gfx->device()->CreateTexture(640,
                                 480,
                                 1,
                                 D3DUSAGE_RENDERTARGET,
                                 D3DFMT_A8R8G8B8,
                                 D3DPOOL_DEFAULT,
                                 (IDirect3DTexture9**)&render_texture.data_pointer,
                                 NULL);


  gfx->CreateResourceView(render_texture,render_surface);
  
  gfx->device()->GetRenderTarget(0,&pBackBuffer);

  camera_.Initialize(gfx);
  camera_.Ortho2D(0,0,640,480);
  gfx->SetViewport(0,0,640,480,-1000,1000);
  gfx->SetCamera(&camera_);
  
  D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
  gfx->device()->SetTransform(D3DTS_WORLD,&matIdentity);
	gfx->device()->SetRenderState(D3DRS_ZENABLE,  D3DZB_TRUE );
	gfx->device()->SetRenderState(D3DRS_LIGHTING, 0 );
	gfx->device()->SetRenderState(D3DRS_ALPHABLENDENABLE,1);
  
	gfx->device()->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gfx->device()->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE );  
	gfx->device()->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE );
	gfx->device()->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	gfx->device()->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	gfx->device()->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE );

	gfx->device()->SetTextureStageState(1,D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gfx->device()->SetTextureStageState(1,D3DTSS_COLORARG2, D3DTA_DIFFUSE );  
	gfx->device()->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_MODULATE );
	gfx->device()->SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	gfx->device()->SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	gfx->device()->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE );

  {
    //uint8_t* data;
    //ReadWholeFileBinary("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Shaders\\main2d.o",&data;
    /*FILE* fp = fopen("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Shaders\\main2d.o","rb");
    fseek(fp,0,SEEK_END);
    auto size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    char* data = new char[size];
    fread(data,1,size,fp);
    fclose(fp);*/
    /*
    std::ios::openmode mode = std::ios::beg | std::ios::in;
    std::ifstream ifs("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Shaders\\main2d.fx",mode);
    const std::string file_content( (std::istreambuf_iterator<char>( ifs )), std::istreambuf_iterator<char>() );
    auto size = file_content.length();
    char* data = new char[size];
    memcpy(data,file_content.c_str(),size);
    ifs.close();


    LPD3DXBUFFER buffer=nullptr,parser_errors=nullptr;
    LPD3DXEFFECTCOMPILER compiler;
    D3DXCreateEffectCompiler(data,size,nullptr,nullptr,0,&compiler,&parser_errors);
    if (parser_errors != nullptr) {
      OutputDebugString((LPCSTR)parser_errors->GetBufferPointer());
      parser_errors->Release();
    }
    compiler->CompileEffect(0,&buffer,&parser_errors);
    if (parser_errors != nullptr) {
      OutputDebugString((LPCSTR)parser_errors->GetBufferPointer());
      parser_errors->Release();
    }
    compiler->Release();
    */
    uint8_t* data;
    size_t size;
    core::io::ReadWholeFileBinary("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Shaders\\main2d.o",&data,size);

    if (data != nullptr) {
      gfx->CreateEffectInterface((uint8_t*)data,size,(void**)&effect);
    }

    core::io::DestroyFileBuffer(&data);
  }
  arc1.Initialize(gfx);
  arc1.SetTopLeft(200,200);
  arc1.SetColor0(0xffffffff);
  arc1.SetColor1(0x000000ff);
  arc1.SetParams(120,0,2,10);
  arc1.Construct();

  font.Initialize(gfx);
  font.Construct(16,"Arial");
  font.SetTopLeft(400,0);
  font.SetSize(640,480);

  {
    static graphics::shape::Vertex v[8] = {
      graphics::shape::Vertex(XMFLOAT3(0,0,1),XMFLOAT2(0,0),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(640,0,1),XMFLOAT2(1,0),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(0,480,1),XMFLOAT2(0,1),XMCOLOR(1,1,1,1),0),
      graphics::shape::Vertex(XMFLOAT3(640,480,1),XMFLOAT2(1,1),XMCOLOR(1,1,1,1),0),

      graphics::shape::Vertex(XMFLOAT3(200+0,0,0),XMFLOAT2(0,0),XMCOLOR(1,1,1,0),0),
      graphics::shape::Vertex(XMFLOAT3(200+100,0,0),XMFLOAT2(0,0),XMCOLOR(1,1,0,0),0),
      graphics::shape::Vertex(XMFLOAT3(200+0,100,0),XMFLOAT2(0,0),XMCOLOR(1,1,0,0),0),
      graphics::shape::Vertex(XMFLOAT3(200+100,100,0),XMFLOAT2(0,0),XMCOLOR(1,1,1,0),0),
    };
    
    memset(&vb,0,sizeof(vb));
    vb.description.byte_width = sizeof(graphics::shape::Vertex)*8;
    vb.description.bind_flags = D3D11_BIND_VERTEX_BUFFER;
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
    

    sa.Initialize(gfx);
    sa.Construct();
    win->player2().set_visual_addon(&sa);

    camera_.UpdateConstantBuffer();
    camera_.SetConstantBuffer(0);
    
    viewprojection = effect->GetParameterByName(0,"viewprojection");
    world = effect->GetParameterByName(0,"world");

    effect->SetTechnique(effect->GetTechnique(0));
    effect->SetMatrix(viewprojection,(D3DXMATRIX*)&(camera_.view()*camera_.projection()));
    world_mat = XMMatrixIdentity();
    effect->SetMatrix(world,(D3DXMATRIX*)&world_mat);

  }

  main_time_span = 0;
  win->player2().Play();
  return S_OK;
}

int IntroScene::Deinitialize() {
  gfx->DestroyEffectInterface((void**)&effect);
  win->player2().set_visual_addon(nullptr);
  sa.Deinitialize();
  gfx->DestroyBuffer(vb);
  arc1.Deinitialize();
  gfx->DestroyTexture(texture);
  gfx->DestroyShader(vertex_shader_);
  delete [] tex_data0;
  return S_OK;
}

int IntroScene::Update(double dt) {
  
  
  static float theta =0;
  float y = 0;//100+sin(theta)*100;
  //D3DXMATRIX matIdentity;
	//D3DXMatrixIdentity(&matIdentity);
  world_mat = XMMatrixAffineTransformation2D(XMVectorSet(1,1,0,0),XMVectorSet(0,0,0,0),0,XMVectorSet(0,y,0,0));


  //sn.Generate(tex_data0,256,256,theta,0,5);
  //gfx->CopyToTexture(texture,tex_data0,0,256,graphics::TexturePoint(0,0),graphics::TexturePoint(0,0),256,256);

  arc1.SetRotate(theta);
  arc1.Update();
  

  sa.SetTopLeft(0,480-100);
  sa.Update();

  font.Update();


  theta += 0.3f;
  main_time_span += dt;
  return S_OK;
}

int IntroScene::Draw() {
  UINT passes;
  effect->Begin(&passes,0);
  //gfx->SetShader(vertex_shader_);
  //gfx->SetShader(pixel_shader_);

  gfx->device()->SetRenderTarget(0,(IDirect3DSurface9*)render_surface.data_pointer);
  //gfx->ClearTarget();


  effect->BeginPass(3);
  effect->SetFloatArray(effect->GetParameterByName(0,"spec_pow"),sa.freq_pow,128);
  effect->SetMatrix(world,(D3DXMATRIX*)&sa.world());
  effect->CommitChanges();
  sa.Draw();
  effect->EndPass();
  

 

  

  uint32_t offsets[1] = {0};
  uint32_t strides[1] = {sizeof(graphics::shape::Vertex)};

  
  //gfx->device()->SetRenderTarget(0,(IDirect3DSurface9*)render_surface.data_pointer);
  effect->BeginPass(4);
  effect->SetTexture(effect->GetParameterByName(0,"tex1"),(IDirect3DBaseTexture9*)render_texture.data_pointer);
  gfx->SetVertexBuffers(0,1,&vb,strides,offsets);
  gfx->SetPrimitiveTopology(D3DPT_TRIANGLESTRIP);
  effect->SetMatrix(world,(D3DXMATRIX*)&world_mat);
  effect->CommitChanges();
  gfx->Draw(4,0);
  effect->EndPass();


  gfx->device()->SetRenderTarget(0,pBackBuffer);
  //gfx->device()->SetTexture(0,(IDirect3DBaseTexture9*)render_texture.data_pointer);
  
  effect->BeginPass(0);
  effect->SetTexture(effect->GetParameterByName(0,"tex1"),(IDirect3DBaseTexture9*)render_texture.data_pointer);
  gfx->SetVertexBuffers(0,1,&vb,strides,offsets);
  gfx->SetPrimitiveTopology(D3DPT_TRIANGLESTRIP);
  effect->SetMatrix(world,(D3DXMATRIX*)&world_mat);
  effect->CommitChanges();
  gfx->Draw(4,0);
  effect->EndPass();


  effect->SetMatrix(world,(D3DXMATRIX*)&arc1.world());
  //effect->CommitChanges();
  effect->BeginPass(1);

 // arc1.Draw();
  effect->EndPass();



{
    char song_info[512];
    double song_time_secs = win->player2().GetPlaybackSeconds();
    auto song_time_mins = song_time_secs / 60.0 ;


    auto secs   = ((song_time_mins - floor(song_time_mins))*60.0);
    auto ms = (secs - floor(secs)) * 1000;
    auto mins   = floor(song_time_mins);
    
    time_t t = time(0);

    auto tsecs   = t % 60;
    auto tmins   = (t / 60) % 60;
    auto thrs   = (t / (60*60)) % 24;
    auto tdays   = (t / (60*60*24)) % 365;
    auto tyears   = 1970 + (t / (60*60*24*365));
    extern float delay;
    sprintf_s(song_info,"Delay: %f\nTime: %d:%03d:%02d:%02d:%02d\nSong Time: %02d:%02d:%03d\nScene Thread Time: %f",
      delay,
      tyears,tdays,thrs,tmins,tsecs,
      uint32_t(mins),uint32_t(secs),uint32_t(ms),
      main_time_span);
    font.Draw(song_info,-1,DT_LEFT);
  }

  effect->End();
  return S_OK;
}