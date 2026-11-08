#include "ata.h"
static inline void outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static inline uint8_t inb(uint16_t p){uint8_t r;__asm__ volatile("inb %1,%0":"=a"(r):"Nd"(p));return r;}
static inline void insw(uint16_t p,void *b,uint32_t c){__asm__ volatile("cld; rep insw":"+D"(b),"+c"(c):"d"(p):"memory");}
void ata_init(void){}
int ata_read28(uint32_t lba,uint8_t count,void *buffer){
    outb(0x1F6,0xE0|((lba>>24)&0x0F)); outb(0x1F2,count);
    outb(0x1F3,lba); outb(0x1F4,lba>>8); outb(0x1F5,lba>>16); outb(0x1F7,0x20);
    for(int s=0;s<count;s++){uint8_t st;do{st=inb(0x1F7);}while(!(st&8) && !(st&1));if(st&1)return -1;insw(0x1F0,(uint8_t*)buffer+s*512,256);}
    return 0;
}
