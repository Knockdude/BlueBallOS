#include "gpu.h"
void gpu_init(void){}
void gpu_putpixel(uint32_t *fb,int pitch,int x,int y,uint32_t c){((uint32_t*)((uint8_t*)fb+y*pitch))[x]=c;}
void gpu_rect(uint32_t *fb,int pitch,int x,int y,int w,int h,uint32_t c){
    for(int yy=0;yy<h;yy++)for(int xx=0;xx<w;xx++)gpu_putpixel(fb,pitch,x+xx,y+yy,c);
}
