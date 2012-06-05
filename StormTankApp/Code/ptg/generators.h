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
namespace generators {

class Generator {
 protected:
  template<typename Op>
    static XMCOLOR* Generate(XMCOLOR* dest_data, uint32_t width, uint32_t height,Op line_op) {
      auto data = dest_data;
      for (uint32_t y=0;y<height;++y) {
        auto dest_line = (XMCOLOR*)&data[y*width];
        for (uint32_t x=0;x<width;++x) {
          line_op(dest_line,x,y);
        }
      }
      return data;
    }
};



class Random : public Generator {
 public:
  static XMCOLOR* Generate(XMCOLOR* dest_data, uint32_t width, uint32_t height) {
    srand(rand());
    return Generator::Generate(dest_data,width,height,[](XMCOLOR* dest_line,uint32_t x,uint32_t y) {
      dest_line[x].r = (rand() % 255) & 0xff;
      dest_line[x].g = (rand() % 255) & 0xff;
      dest_line[x].b = (rand() % 255) & 0xff;
      dest_line[x].a = 0xff;
    });
  }
};

class XOR : public Generator {
 public:
  static XMCOLOR* Generate(XMCOLOR* dest_data, uint32_t width, uint32_t height) {
    return Generator::Generate(dest_data,width,height,[](XMCOLOR* dest_line,uint32_t x,uint32_t y) {
      dest_line[x].r = (x ^ y) & 0xff;
      dest_line[x].g = (x ^ y) & 0xff;
      dest_line[x].b = (x ^ y) & 0xff;
      dest_line[x].a = 0xff;
    });
  }
};

class SinePlasma : public Generator {
 public:
  static XMCOLOR* Generate(XMCOLOR* dest_data, uint32_t width, uint32_t height,float ph_x,float ph_y,float amp_x,float amp_y) {
    return Generator::Generate(dest_data,width,height,[ph_x,ph_y,amp_x,amp_y,width,height](XMCOLOR* dest_line,uint32_t x,uint32_t y) {
        auto x2 = 63.5*( sin( (ph_x+x) * ((XM_PI / (width>>1))*amp_x)) );
        auto y2 = 63.5*( cos( (ph_y+y) * ((XM_PI / (height>>1))*amp_y)) );
        auto color = (uint32_t)(127+(x2+y2));
        dest_line[x].r = (color) & 0xff;
        dest_line[x].g = (color) & 0xff;
        dest_line[x].b = (color) & 0xff;
        dest_line[x].a = 0xff;
    });
  }
};

class ChaldniPlates : public Generator {
 public:
  static XMCOLOR* Generate(XMCOLOR* dest_data, uint32_t width, uint32_t height) {
    return Generator::Generate(dest_data,width,height,[width,height](XMCOLOR* dest_line,uint32_t x,uint32_t y) {
      XMCOLOR color = 0xffffffff;
      switch (pick(width,height,x,y)) {
        case 0: color = XMCOLOR(235,0,95,255);		break;
        case 1: color = XMCOLOR(255,255,255,255);		break;
        case 2: color = XMCOLOR(255,0,05,255);		break;
        case 3: color = XMCOLOR(0,0,0,255);			break;
	    }
		
        
        dest_line[x] = color;
    });
  }
 private:
  static int calcula(uint32_t width,uint32_t height,double x, double y, double xshift, double yshift, double m, double n, double zoom, double detail){
	  double pi = 3.14159265;
	  double rx = x/width;
	  double ry = y/height;
	  double rm = m * zoom;
	  double rn = n * zoom;
    double rd = detail * (cos(rm*pi*rx)*cos(rn*pi*ry) - cos(rn*pi*rx)*cos(rm*pi*ry));
      auto r = int(rd);
      return r;
  }

  static int pick(uint32_t width,uint32_t height,uint32_t x,uint32_t y) {
    double   	easing=0.13;
	  double zoom = 1;
	  double detail = 5;
    double ex=1000;
    double ey=1000;
	  double m = ex/7.321, n = ey/7.257;
	  float  xshift = 0 , yshift = 0;

	int z = calcula(width,height,x, y, xshift, yshift, m, n, zoom, detail);
z = z % 3;
	 if (z < 0) {
	     switch (z) {
			case -0: return(1);
			case -1: return(2);
			case -2: return(3);
		      }
	        }
	else {
	switch (z) {
			case 0: return(1);
			case 1: return(2);
			case 2: return(3);
		}
	}
  }

};

}
}

#include "generators/simplex_noise.h"