#include "../stormtankapp.h"

#define LINE_COUNT 128

Oscilloscope::Oscilloscope() {

}

Oscilloscope::~Oscilloscope() {

}

int Oscilloscope::Initialize(ve::Context* context) {
  ve::shape::Shape::Initialize(context);

  ve::shape::Vertex v[LINE_COUNT];
  float xinc = 0;
  for (int i=0;i<LINE_COUNT;++i){
    float w = 1;
    v[i] = ve::shape::Vertex(dx::XMFLOAT3(xinc,0,0), dx::XMFLOAT2(0,0), dxp::XMCOLOR(1,1,1,1),i);
    xinc += 1.0f;
  }
    
  memset(&vertex_buffer_,0,sizeof(vertex_buffer_));
  vertex_buffer_.description.byte_width = sizeof(ve::shape::Vertex)*LINE_COUNT;
  vertex_buffer_.description.bind_flags = D3D11_BIND_VERTEX_BUFFER;
  vertex_buffer_.description.usage = 0;//D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
 // context_->CreateBuffer(vertex_buffer_,v,0);
  
  Update();



  scratch = new float[nBins];
  unity = new XDSP::XMVECTOR[nBins];
  real_samples = new XDSP::XMVECTOR[nBins>>2];
  img_samples = new XDSP::XMVECTOR[nBins>>2];
  memset(img_samples,0,(nBins>>2)*sizeof(XDSP::XMVECTOR));
  memset(real_samples,0,(nBins>>2)*sizeof(XDSP::XMVECTOR));
  memset(freq_pow,0,sizeof(freq_pow));
  

  time_counter = 0;
  rindex = 0;
  XDSP::FFTInitializeUnityTable(unity,nBins);
  InitializeCriticalSection(&cs);
  
  return S_OK;
}

int Oscilloscope::Deinitialize() {
  //EnterCriticalSection(&cs);
 // context_->DestroyBuffer(vertex_buffer_);
  delete [] real_samples;
  delete [] img_samples;
  delete [] scratch;
  delete [] unity;
  //LeaveCriticalSection(&cs);
  DeleteCriticalSection(&cs);
  return S_OK;
}

void Oscilloscope::AddPCMData256(float* samples, uint32_t channels, double time_ms) {
  time_counter += time_ms;
  //if (time_counter >= 100.0) {
    for (uint32_t i=0;i<nBins;++i) {
      float a = samples[i<<1];
      freq_pow[i] = a;//(20*log10((a+1.001f)*0.5f));//((20 * log10(1+samples[i<<1])));
    }
    time_counter = 0;
  //}
}

int Oscilloscope::Update() {
  //world_ = XMMatrixAffineTransformation2D(XMVectorSet(1,1,0,0),XMVectorSet(0,0,0,0),0,XMVectorSet(0,0.0f,0,0));
  
  return ve::shape::Shape::Update();
}

int Oscilloscope::Draw() {
  //EnterCriticalSection(&cs);
  uint32_t offsets[1] = {0};
  uint32_t strides[1] = {sizeof(ve::shape::Vertex)};
 // context_->SetVertexBuffers(0,1,&vertex_buffer_,strides,offsets);
  //context_->SetIndexBuffer(index_buffer_,0);
  //context_->SetPrimitiveTopology(D3DPT_POINTLIST);
  //((graphics::ContextD3D9*)context_)->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&world_);
  //((graphics::ContextD3D9*)context_)->DrawIndexed(64*6,0,0);
  for (int i=0;i<LINE_COUNT;++i) {
    /*auto current_world_ = world_ * XMMatrixAffineTransformation2D(XMVectorSet(1,1,0,0),XMVectorSet(0,0,0,0),0,XMVectorSet(0,100.0f-100.0f*freq_pow[i],0,0));
    //((graphics::ContextD3D9*)context_)->device()->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&current_world_);
    graphics::Buffer buf;
    current_world_ = XMMatrixTranspose(current_world_);
    buf.internal_pointer = (void*)&current_world_;
    buf.description.byte_width = sizeof(current_world_);
    context_->SetConstantBuffers(graphics::kShaderTypeVertex,1,1,&buf);*/
    
  }

  context_->Draw(LINE_COUNT,0);
  //context_->ClearIndexBuffer();
  //LeaveCriticalSection(&cs);
  return S_OK;
}
