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
class MainWindow: public core::windows::Window {
  public:
    LoadingScene  loading_scene;
    IntroScene    intro_scene;
    MainWindow();
    ~MainWindow();
    void Initialize();
    void Step();
    void SwitchScene(BaseScene* scene) {
      if (current_scene != nullptr) {
        current_scene->Deinitialize();
        current_scene = nullptr;
      }
      if (scene != nullptr) {
        current_scene = scene;
        current_scene->Initialize(this);
      }
    }
    graphics::ContextD3D9* gfx() { return gfx_; }
    audio::DirectSound* audio_interface() { return audio_interface_; }
    //SonantPlayer& player() { return player_; }
    audio::synth::Player& player2() { return *synth_player_; }
    utilities::Timer<double>& timer() { return timer_; }
   protected:
    int OnCreate(WPARAM wParam,LPARAM lParam);
    int OnDestroy(WPARAM wParam,LPARAM lParam);
    int OnCommand(WPARAM wParam,LPARAM lParam);
    int OnActivate(WPARAM wParam,LPARAM lParam);
  private:
    graphics::ContextD3D9*    gfx_;
    audio::DirectSound*       audio_interface_;
    BaseScene*                current_scene;
    graphics::InputLayout     input_layout;
    //SonantPlayer              player_;
    audio::synth::Player*      synth_player_;
    utilities::Timer<double>  timer_;
    struct {
      uint64_t extra_cycles;
      uint64_t current_cycles;
      uint64_t prev_cycles;
      uint64_t total_cycles;
      double render_time_span;
      double fps_time_span;
      double span_accumulator;
      uint32_t fps_counter;
      uint32_t ups_counter;
      uint32_t fps;
      uint32_t ups;
    } timing;
};