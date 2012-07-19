#include "../stormtankapp.h"

#define LINE_COUNT 128

SpectrumAnalyzer::SpectrumAnalyzer() {

}

SpectrumAnalyzer::~SpectrumAnalyzer() {

}

int SpectrumAnalyzer::Initialize(graphics::Context* context) {
  graphics::shape::Shape::Initialize(context);

  graphics::shape::Vertex v[LINE_COUNT*8];
  float xinc = 0;
  for (int i=0;i<LINE_COUNT;++i){
    float w = 1;
    v[i*8+0] = graphics::shape::Vertex(XMFLOAT3(xinc,0,0),XMFLOAT2(0,0),XMCOLOR(1,0,0,1),i);
    v[i*8+1] = graphics::shape::Vertex(XMFLOAT3(w+xinc,0,0),XMFLOAT2(1,0),XMCOLOR(1,0,0,1),i);
    v[i*8+2] = graphics::shape::Vertex(XMFLOAT3(xinc,50,0),XMFLOAT2(0,1),XMCOLOR(1,1,0,1),i);
    v[i*8+3] = graphics::shape::Vertex(XMFLOAT3(w+xinc,50,0),XMFLOAT2(1,1),XMCOLOR(1,1,0,1),i);
    v[i*8+4] = graphics::shape::Vertex(XMFLOAT3(xinc,50,0),XMFLOAT2(0,0),XMCOLOR(1,1,0,1),i);
    v[i*8+5] = graphics::shape::Vertex(XMFLOAT3(w+xinc,50,0),XMFLOAT2(1,0),XMCOLOR(1,1,0,1),i);
    v[i*8+6] = graphics::shape::Vertex(XMFLOAT3(xinc,100,0),XMFLOAT2(0,1),XMCOLOR(0,1,0,1),i);
    v[i*8+7] = graphics::shape::Vertex(XMFLOAT3(w+xinc,100,0),XMFLOAT2(1,1),XMCOLOR(0,1,0,1),i);
    xinc += 2.0f;
  }
    
  memset(&vertex_buffer_,0,sizeof(vertex_buffer_));
  vertex_buffer_.description.byte_width = sizeof(graphics::shape::Vertex)*LINE_COUNT*8;
  vertex_buffer_.description.bind_flags = D3D11_BIND_VERTEX_BUFFER;
  vertex_buffer_.description.usage = 0;//D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
  context_->CreateBuffer(vertex_buffer_,v);
  
  short indices[LINE_COUNT*6];
  for (int i=0;i<LINE_COUNT;++i) {
    indices[i*6+0] = (i*4)+0;
    indices[i*6+1] = (i*4)+1;
    indices[i*6+2] = (i*4)+2;
    indices[i*6+3] = (i*4)+2;
    indices[i*6+4] = (i*4)+1;
    indices[i*6+5] = (i*4)+3;
  }
  
  memset(&index_buffer_,0,sizeof(index_buffer_));
  index_buffer_.description.byte_width = sizeof(short)*LINE_COUNT*6;
  index_buffer_.description.bind_flags = D3D11_BIND_INDEX_BUFFER;
  index_buffer_.description.usage = 0;//D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
  context_->CreateBuffer(index_buffer_,indices);
  

  Update();



  scratch = new float[nBins];
  unity = new XDSP::XVECTOR[nBins];
  real_samples = new XDSP::XVECTOR[nBins>>2];
  img_samples = new XDSP::XVECTOR[nBins>>2];
  memset(img_samples,0,(nBins>>2)*sizeof(XDSP::XVECTOR));
  memset(real_samples,0,(nBins>>2)*sizeof(XDSP::XVECTOR));
  memset(freq_pow,0,sizeof(freq_pow));
  

  time_counter = 0;
  rindex = 0;
  XDSP::FFTInitializeUnityTable(unity,nBins);
  InitializeCriticalSection(&cs);
  
  return S_OK;
}

int SpectrumAnalyzer::Deinitialize() {
  //EnterCriticalSection(&cs);
  context_->DestroyBuffer(vertex_buffer_);
  context_->DestroyBuffer(index_buffer_);
  delete [] real_samples;
  delete [] img_samples;
  delete [] scratch;
  delete [] unity;
  //LeaveCriticalSection(&cs);
  DeleteCriticalSection(&cs);
  return S_OK;
}

void SpectrumAnalyzer::AddPCMData256(float* samples, uint32_t channels, double time_ms) {
  //EnterCriticalSection(&cs);
  float* __restrict real_arr = (float* __restrict)real_samples;
  float* __restrict img_arr = (float* __restrict)img_samples;
  //for (uint32_t i=0;i<count>>1;++i) {
  for (uint32_t i=0;i<nBins;++i) {
    real_arr[i] = samples[(i*2)];// / 32767.0f;

    //hann window
    real_t multiplier = 0.5f * (1 - cos(2*XM_PI*i/(nBins-1)));
    real_arr[i] = multiplier * real_arr[i];

  }
  real_arr[0] = 0.5f * samples[0];


  //memcpy(real_samples,samples,256*sizeof(real_t));
  time_counter += time_ms;
  if (time_counter >= 10.0) {
    memset(scratch,0,nBins*sizeof(float));
    XDSP::FFT(real_samples,img_samples,unity,nBins);
    XDSP::FFTPolar(real_samples, real_samples, img_samples, nBins);
    XDSP::FFTUnswizzle((XDSP::XVECTOR*)scratch, real_samples, nBinsLog2);
   
    //scratch[0] = scratch[0]*0.5*(1+cos((2*XM_PI*0)/LINE_COUNT-1));
    //freq_pow[0] = (20 * log10(scratch[0]*5));//max(0.01f,scratch[i]*10);
    for (int i=0;i<LINE_COUNT;++i) {
      //scratch[i] = scratch[i]*0.5*(1-cos((2*XM_PI*i)/LINE_COUNT-1));
      //double freq = (double(i)/(2*LINE_COUNT))*44100.0;
      freq_pow[i] = ((20 * log10(1+scratch[i])));//max(0.01f,scratch[i]*10);
    }
    /*int nBands = 64;
    //int bandWidth = rindex / nBands;
    
		  float exponent = log( (float)( nBins / 2 ) ) / log( 2.0f );
      exponent /= (float)nBands;
      for( int j = 0; j < nBands; ++j )
      {
          float freq = 22100.0f*log10(1+(j)*10.0f/64.0f);
          int low = (int)floor( pow( 2.0f, exponent * (float)j ) );
          int high = (int)ceil( pow( 2.0f, exponent * (float)(j+1) ) );
          float energy = 0;
          float binwidth = 0;
          for( int bin = low; ( bin < high ) && ( bin < (nBins/2) ); ++bin )
          {
              energy += scratch[bin];
              binwidth += 1.0f;
          }

          const float biasScale = 4.0f / binwidth;
          freq_pow[j] = max(0.01f,energy * biasScale);
          freq_pow[j] = min(freq_pow[j],1.0f);
          //for( int x = 0; x < bandWidth; ++x )
          //{
              //((float*)pSamples)[j*bandWidth + x] = energy * biasScale;
          //}

      }*/

    time_counter = 0;
    rindex = 0;
    memset(real_samples,0,(nBins>>2)*sizeof(XDSP::XVECTOR));
    memset(img_samples,0,(nBins>>2)*sizeof(XDSP::XVECTOR));
      
  }
  //XDSP::FFTPolar(rv,rv,iv,64);
  //XDSP::FFTUnswizzle(outrv,rv,6);
  //LeaveCriticalSection(&cs);
}

int SpectrumAnalyzer::Update() {
  //world_ = XMMatrixAffineTransformation2D(XMVectorSet(1,1,0,0),XMVectorSet(0,0,0,0),0,XMVectorSet(0,0.0f,0,0));
  
  return graphics::shape::Shape::Update();
}

int SpectrumAnalyzer::Draw() {
  //EnterCriticalSection(&cs);
  uint32_t offsets[1] = {0};
  uint32_t strides[1] = {sizeof(graphics::shape::Vertex)};
  context_->SetVertexBuffers(0,1,&vertex_buffer_,strides,offsets);
  //context_->SetIndexBuffer(index_buffer_,0);
  context_->SetPrimitiveTopology(D3DPT_TRIANGLESTRIP);
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

  context_->Draw(8*LINE_COUNT,0);
  //context_->ClearIndexBuffer();
  //LeaveCriticalSection(&cs);
  return S_OK;
}
