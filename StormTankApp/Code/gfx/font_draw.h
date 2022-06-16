class FontDraw : public ve::shape::Shape {
 public:
  int Initialize(ve::Context* context);
  int Deinitialize();
  int Construct(uint32_t height, const char* facename);
  int SetSize(uint32_t w,uint32_t h) {
    changed_ = true;
    rc.left =  rc.top = 0;
    rc.right = w;  
    rc.bottom = h;
    return S_OK;
  }
  int SetColor(uint32_t color) {
    color_ = color;
  }
  int Update();
  int Draw(const char* text, int32_t length, uint32_t flags);
 private:
  ID3DXFont* font;
  ID3DXSprite* sprite;
  RECT rc;
  uint32_t color_;
  int Construct() {
    return S_FALSE;
  }
  int Draw() {
    return S_FALSE;
  }
};