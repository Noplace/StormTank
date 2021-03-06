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
  graphics::ContextD3D9* gfx;
};


class LoadingScene : public BaseScene {
 public:
  int Initialize(MainWindow* win);
  int Deinitialize();
  int Update(double dt);
  int Draw();
 private:
  static DWORD WINAPI LoadingThread(LPVOID lpThreadParameter);
  graphics::Camera camera_;
  graphics::VertexShader vertex_shader_;
  graphics::PixelShader pixel_shader_;
  graphics::Buffer vb;
  graphics::Texture texture;
  graphics::shape::Arc arc1;
  graphics::shape::Rectangle loading_bar;
  graphics::shape::Rectangle loading_bg;
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

  graphics::Texture render_texture;
  graphics::ResourceView render_surface,old_surface;

  FontDraw font;
  graphics::Camera camera_;
  graphics::VertexShader vertex_shader_;
  graphics::PixelShader pixel_shader_;
  Oscilloscope sa;
  graphics::Buffer vb;
  graphics::Texture texture;
  graphics::shape::Arc arc1;
};


class DemoScene : public BaseScene {
 public:
  int Initialize(MainWindow* win);
  int Deinitialize();
  int Update(double dt);
  int Draw();
 private:
  graphics::ContextD3D9* gfx;
  double main_time_span;
  struct {
    char help_str[255];
    FontDraw font;
    graphics::Buffer vb;
    graphics::Texture texture;
    graphics::shape::Arc arc1;
    LPD3DXEFFECT effect;
    D3DXHANDLE world,viewprojection;
    graphics::Camera cam2d,cam3d;
  } res;
};