#include "gui.h"
#include "../gpu/gpu.h"
#include "../drivers/mouse.h"
#include <stdint.h>

/* A compact software GUI fallback. If GRUB supplies a framebuffer, this layer can
   be extended to use its address/width/height. The current fallback guarantees
   that the kernel remains bootable on ordinary VGA text hardware. */
static void draw_box(int x,int y,int w,int h){
    volatile uint16_t *v=(uint16_t*)0xB8000;
    (void)x;(void)y;(void)w;(void)h;
    v[0]=(uint16_t)(0x1F00|'B'); v[1]=(uint16_t)(0x1F00|'l'); v[2]=(uint16_t)(0x1F00|'u');
    v[3]=(uint16_t)(0x1F00|'e'); v[4]=(uint16_t)(0x1F00|'B'); v[5]=(uint16_t)(0x1F00|'a');
    v[6]=(uint16_t)(0x1F00|'l'); v[7]=(uint16_t)(0x1F00|'l'); v[8]=(uint16_t)(0x1F00|'O'); v[9]=(uint16_t)(0x1F00|'S');
}
void gui_init(void){}
void gui_desktop(void){draw_box(0,0,640,480);}
void gui_poll(void){(void)mouse_x();(void)mouse_y();}
