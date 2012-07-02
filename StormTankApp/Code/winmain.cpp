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

class StormTankApp : public core::windows::Application {
 public: 
   MainWindow win;
   StormTankApp(HINSTANCE instance , LPSTR command_line, int show_command) : Application() {
    if (RanBefore("StormTankApp") == true) {
      MessageBox(nullptr,"App already running","Error",MB_ICONWARNING|MB_OK);
      exit(0);
    }
    unsigned old_fp_state;
    #ifndef _M_X64 
      _controlfp_s(&old_fp_state, _PC_53, _MCW_PC);
      //_controlfp(_RC_NEAR, _MCW_RC);
    #endif
    win.Initialize();
   }
  ~StormTankApp() {

  }

  int Run() {
    MSG msg;
    do {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      } else {
        win.Step();
      }
    } while(msg.message!=WM_QUIT);
    return static_cast<int>(msg.wParam);
  }
};
/*
#include <x3daudio.h>
#include <XDSP.h>
void fft_test() {
  XDSP::XVECTOR unity[16];
  XDSP::FFTInitializeUnityTable(unity,64);
  
  
  XDSP::XVECTOR rv[16],iv[16],outrv[16],outiv[16];

  float* rd = (float* __restrict)rv;
  float* id = (float* __restrict)iv;
  for (int i=0;i<64;++i) {
    rd[i] = i/64.0;
    id[i] = 0;//0.1*i/64.0;
  }
  
  XDSP::FFT(rv,iv,unity,64,1);
  //XDSP::FFTPolar(rv,rv,iv,64);
  XDSP::FFTUnswizzle(outrv,rv,6);
  XDSP::FFTUnswizzle(outiv,iv,6);
 //for (int i=0;i<64;++i) {
  //  id[i] = 0;//0.1*i/64.0;
  //}
  XDSP::IFFTDeinterleaved(outrv,outiv,unity,1,6);
  int a = 1;
}*/

/*
void floatTest() {
  float r1,r2,r3,r4;
  utilities::Timer<double> timer;
  r1 = r2 = r3 = r4 = 0.0f;
  double time1 = timer.GetCurrentCycles();
  for (int i=0;i<100000000;++i) {
    r1 = r1 + 1.0f;
    r4 = sinf(r1 * 2 * XM_PI * 0.0001f);
    r3 = 12.0f;
    r2 = (r1 * r3) / r4;
  }
  double time2 = timer.GetCurrentCycles();
  double result = (time2 - time1) * timer.resolution();
  int a = 1;
}*/

float g_one = 1.0f;
float g_small_1 = FLT_EPSILON * 0.5;
float g_small_2 = DBL_EPSILON * 0.5;
float g_small_3 = DBL_EPSILON * 0.5;
 
void AddThreeAndPrint()
{
  char str[255];
    sprintf(str,"Sum = %1.16e\n", ((g_one + g_small_1) + g_small_2) + g_small_3);
    OutputDebugString(str);
}

void MulDouble(double x, double y, double* pResult)
{
    *pResult = x * y;
}
 
void MulFloat(float x, float y, float* pResult)
{
    *pResult = x * y;
}


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  //int test_padsynth();
  //test_padsynth();
  //fft_test();
  StormTankApp app(hInstance,lpCmdLine,nShowCmd);
  //AddThreeAndPrint();
  return app.Run();
}