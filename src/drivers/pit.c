#include "pit.h"
#include <stdint.h>
static inline void outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}
void pit_init(int hz){if(hz<1)hz=100;uint32_t d=1193180u/(uint32_t)hz;outb(0x43,0x36);outb(0x40,d&255);outb(0x40,d>>8);}
