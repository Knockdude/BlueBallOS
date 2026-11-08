#pragma once
#include <stdint.h>
void gpu_init(void);
void gpu_putpixel(uint32_t *fb,int pitch,int x,int y,uint32_t c);
void gpu_rect(uint32_t *fb,int pitch,int x,int y,int w,int h,uint32_t c);
