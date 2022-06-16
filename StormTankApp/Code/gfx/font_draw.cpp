#include "../stormtankapp.h"

int FontDraw::Initialize(ve::Context* context) {
  Component::Initialize(context);
  font = nullptr;
  sprite = nullptr;
  ve::ContextD3D9* ctxt = (graphics::ContextD3D9*)context_;
  D3DXCreateSprite(ctxt->device(),&sprite);
  color_ = 0xFFFFFFFF;
  return S_OK;
}

int FontDraw::Deinitialize() {
  SafeRelease(&font);
  SafeRelease(&sprite);
  return S_OK;
}

int FontDraw::Construct(uint32_t height, const char* facename) {
  SafeRelease(&font);
  graphics::ContextD3D9* ctxt = (graphics::ContextD3D9*)context_;
  D3DXCreateFont(ctxt->device(),height,0,FW_NORMAL,1,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,facename,&font);
  font->PreloadCharacters(0,255);
  return S_OK;
}

int FontDraw::Update() {
  if (changed_ == true) {
    ve::shape::Shape::Update();
    sprite->SetTransform((D3DXMATRIX*)&world_);
  }
  return S_OK;
}

int FontDraw::Draw(const char* text, int32_t length, uint32_t flags) {
  sprite->Begin(D3DXSPRITE_ALPHABLEND);
  font->DrawText(sprite,text,length,&rc,flags,color_);
  sprite->End();
  return S_OK;
}
