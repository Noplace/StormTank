struct GfxVertex {
  float x,y,z;
  uint32_t color;
  float u,v;
};

class BaseScene {
 public:
  virtual int Initialize(MainWindow* win) {
    this->win = win;
    return S_OK;
  }
  virtual int Deinitialize() = 0;
  virtual int Update(double dt) = 0;
  virtual int Draw() = 0;
 protected:
  MainWindow* win;
  ve::ContextD3D11* gfx;
};


class LoadingScene : public BaseScene {
 public:
  int Initialize(MainWindow* win);
  int Deinitialize();
  int Update(double dt);
  int Draw();
 private:
  static DWORD WINAPI LoadingThread(LPVOID lpThreadParameter);
  ve::Camera camera_;
  ve::VertexShader vertex_shader_;
  ve::PixelShader pixel_shader_;
  ve::Buffer vb;
  ve::Texture texture;
  ve::shape::Arc arc1;
  ve::shape::Rectangle loading_bar;
  ve::shape::Rectangle loading_bg;
  HANDLE thread_handle,event1;
  DWORD thread_id;
  int loading_progress;
};


class IntroScene : public BaseScene {
 public:
  int Initialize(MainWindow* win);
  int Deinitialize();
  int Update(double dt);
  int Draw();
 private:

  ve::Texture render_texture;
  ve::ResourceView render_surface,old_surface;

 // FontDraw font;
  ve::Camera camera_;
  ve::VertexShader vertex_shader_;
  ve::PixelShader pixel_shader_;
  Oscilloscope sa;
  ve::Buffer vb;
  ve::Texture texture;
  ve::shape::Arc arc1;
};


class DemoScene : public BaseScene {
 public:
  int Initialize(MainWindow* win);
  int Deinitialize();
  int Update(double dt);
  int Draw();
 private:
  ve::ContextD3D11* gfx;
  double main_time_span;
  struct {
    char help_str[255];
    //FontDraw font;
    ve::Buffer vb;
    ve::Texture texture;
    ve::shape::Arc arc1;
    //LPD3DXEFFECT effect;
    //D3DXHANDLE world,viewprojection;
    ve::Camera cam2d,cam3d;
  } res;
};