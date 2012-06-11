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

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  StormTankApp app(hInstance,lpCmdLine,nShowCmd);
  return app.Run();
}