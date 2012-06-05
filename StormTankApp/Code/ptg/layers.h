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
//d3d9, argb 32 only for now

namespace pgt {
namespace layers {

class Blend {
 public:
   static XMCOLOR* Additive(XMCOLOR* dest, XMCOLOR* layer1, XMCOLOR* layer2, uint32_t width, uint32_t height, double t) {
     t = t < 0 ? 0 : (t > 1 ? 1 : t);
     return Operation(dest, layer1,layer2,width,height,[t](XMCOLOR* dest_line,XMCOLOR* line1,XMCOLOR* line2,uint32_t x,uint32_t y)
     {
        dest_line[x].r = (UINT((1-t)*line1[x].r + t*line2[x].r) & 0xFF);
        dest_line[x].g = (UINT((1-t)*line1[x].g + t*line2[x].g) & 0xFF);
        dest_line[x].b = (UINT((1-t)*line1[x].b + t*line2[x].b) & 0xFF);
        dest_line[x].a = (UINT((1-t)*line1[x].a + t*line2[x].a) & 0xFF); 
     });
   }
   static XMCOLOR* Sub(XMCOLOR* dest, XMCOLOR* layer1, XMCOLOR* layer2, uint32_t width, uint32_t height) {
     return Operation(dest,layer1,layer2,width,height,[](XMCOLOR* dest_line,XMCOLOR* line1,XMCOLOR* line2,uint32_t x,uint32_t y)
     {
        dest_line[x].r = (line1[x].r - line2[x].r) & 0xFF;
        dest_line[x].g = (line1[x].g - line2[x].g) & 0xFF;
        dest_line[x].b = (line1[x].b - line2[x].b) & 0xFF;
        dest_line[x].a = (line1[x].a - line2[x].a) & 0xFF; 
     });
   }
   static XMCOLOR* Mul(XMCOLOR* dest, XMCOLOR* layer1, XMCOLOR* layer2, uint32_t width, uint32_t height) {
     return Operation(dest,layer1,layer2,width,height,[](XMCOLOR* dest_line,XMCOLOR* line1,XMCOLOR* line2,uint32_t x,uint32_t y)
     {
        dest_line[x].r = (line1[x].r * line2[x].r) & 0xFF;
        dest_line[x].g = (line1[x].g * line2[x].g) & 0xFF;
        dest_line[x].b = (line1[x].b * line2[x].b) & 0xFF;
        dest_line[x].a = (line1[x].a * line2[x].a) & 0xFF; 
     });
   }
   static XMCOLOR* Div(XMCOLOR* dest, XMCOLOR* layer1, XMCOLOR* layer2, uint32_t width, uint32_t height) {
     return Operation(dest,layer1,layer2,width,height,[](XMCOLOR* dest_line,XMCOLOR* line1,XMCOLOR* line2,uint32_t x,uint32_t y)
     {
        dest_line[x].r = (line1[x].r / (line2[x].r+1)) & 0xFF;
        dest_line[x].g = (line1[x].g / (line2[x].g+1)) & 0xFF;
        dest_line[x].b = (line1[x].b / (line2[x].b+1)) & 0xFF;
        dest_line[x].a = (line1[x].a / (line2[x].a+1)) & 0xFF; 
     });
   }
 private:

  template<typename Op>
  static XMCOLOR* Operation(XMCOLOR* dest, XMCOLOR* layer1, XMCOLOR* layer2, uint32_t width, uint32_t height,Op op) {
    auto data = dest;
    for (uint32_t y=0;y<height;++y) {
      auto dest_line = (XMCOLOR*)&data[y*width];
      auto line1 = (XMCOLOR*)&layer1[y*width];
      auto line2 = (XMCOLOR*)&layer2[y*width];
      for (uint32_t x=0;x<width;++x) {
        op(dest_line,line1,line2,x,y);
      }
    }
    return data;
  }
};

}
}