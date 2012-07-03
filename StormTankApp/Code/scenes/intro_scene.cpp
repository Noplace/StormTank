#include "../stormtankapp.h"
#include <fstream>

pgt::generators::SimplexNoise sn;
XMCOLOR* tex_data0;
XMMATRIX world_mat;

int IntroScene::Initialize(MainWindow* win) {
  graphics::Scene::Initialize(win->gfx());
  BaseScene::Initialize(win);
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

  {
    /*FILE* fp = fopen("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Shaders\\main2d.fx","r");
    fseek(fp,0,SEEK_END);
    auto size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    
    fread(data,1,size-1,fp);
    fclose(fp);*/

    std::ios::openmode mode = std::ios::beg | std::ios::in;

    std::ifstream ifs("D:\\Personal\\Projects\\StormTank\\StormTankApp\\Shaders\\main2d.fx",mode);
 
      const std::string file_content( (std::istreambuf_iterator<char>( ifs )), std::istreambuf_iterator<char>() );
      auto size = file_content.length();
      char* data = new char[size];
      //data_pointer = HeapAlloc(manager_->heap(),0,data_length);
      memcpy(data,file_content.c_str(),size);
   
    ifs.close();

    graphics::ShaderBlob blob;
    gfx->CompileShaderFromMemory(data,size,"VS","vs_3_0",blob);
    gfx->CreateVertexShader(blob.data(),blob.size(),vertex_shader_);
    delete []data;
  }
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
    static graphics::shape::Vertex v[8] = {
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
  }
  return S_OK;
}

int IntroScene::Deinitialize() {
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
  camera_.UpdateConstantBuffer();
  
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
  sa.SetTopLeft(0,480-100);
  sa.BuildTransform();

  return S_OK;
}

int IntroScene::Draw() {
  //gfx->SetShader(vertex_shader_);
  camera_.SetConstantBuffer(0);
  //uint32_t offsets[1] = {0};
  //uint32_t strides[1] = {sizeof(graphics::shape::Vertex)};
  //gfx->SetVertexBuffers(0,1,&vb,strides,offsets);
  //gfx->SetPrimitiveTopology(D3DPT_TRIANGLESTRIP);
  //gfx->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&world_mat);
  //gfx->device()->SetTexture(0,(IDirect3DTexture9*)texture.data_pointer);
  //gfx->Draw(4,0);
  //gfx->device()->SetTexture(0,nullptr);
  //gfx->Draw(4,4);

  sa.Draw();

  //gfx->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&arc1.world());
  //arc1.Draw();

  

  {
    char caption[255];
    double song_time_secs = win->player2().GetPlaybackSeconds();
    auto song_time_mins = song_time_secs / 60.0 ;


    auto secs   = ((song_time_mins - floor(song_time_mins))*60.0);
    auto ms = (secs - floor(secs)) * 1000;
    auto mins   = floor(song_time_mins);

    sprintf_s(caption,"Song Time: %02d:%02d:%03d",uint32_t(mins),uint32_t(secs),uint32_t(ms));
    SetWindowText(this->win->handle(),caption);
  }

  return S_OK;
}