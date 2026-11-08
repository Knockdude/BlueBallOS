#include "interrupts.h"

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t attributes;
    uint16_t offset_high;
} idt_gate_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t base;
} idt_pointer_t;

static idt_gate_t idt[256];

extern void isr_default(void);
extern void irq0_stub(void);
extern void irq1_stub(void);
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

static void idt_set_gate(uint8_t vector, void (*handler)(void)) {
    uint32_t address = (uint32_t)handler;
    idt[vector].offset_low = (uint16_t)(address & 0xFFFF);
    idt[vector].selector = 0x08;
    idt[vector].zero = 0;
    idt[vector].attributes = 0x8E;
    idt[vector].offset_high = (uint16_t)(address >> 16);
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static void pic_remap(void) {
    outb(0x20, 0x11);
    io_wait();
    outb(0xA0, 0x11);
    io_wait();
    outb(0x21, 0x20);
    io_wait();
    outb(0xA1, 0x28);
    io_wait();
    outb(0x21, 0x04);
    io_wait();
    outb(0xA1, 0x02);
    io_wait();
    outb(0x21, 0x01);
    io_wait();
    outb(0xA1, 0x01);
    io_wait();

    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);
}

void interrupts_init(void) {
    void (*const exceptions[32])(void) = {
        isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
        isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
        isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
        isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
    };

    for (uint32_t i = 0; i < 256; ++i) {
        idt_set_gate((uint8_t)i, isr_default);
    }
    for (uint32_t i = 0; i < 32; ++i) {
        idt_set_gate((uint8_t)i, exceptions[i]);
    }
    idt_set_gate(32, irq0_stub);
    idt_set_gate(33, irq1_stub);

    const idt_pointer_t pointer = {
        (uint16_t)(sizeof(idt) - 1),
        (uint32_t)idt
    };
    __asm__ volatile ("lidt %0" : : "m"(pointer));
    pic_remap();
}
