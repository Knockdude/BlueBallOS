#include "pcspeaker.h"
#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void pcspeaker_init(void) {
}

void pcspeaker_beep(unsigned hz) {
    if (!hz) {
        return;
    }
    uint32_t divisor = 1193180u / hz;
    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)divisor);
    outb(0x42, (uint8_t)(divisor >> 8));
    outb(0x61, (uint8_t)(inb(0x61) | 3));
}
