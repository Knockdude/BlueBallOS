#include <stdint.h>

#include "interrupts/interrupts.h"
#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "memory/heap.h"
#include "memory/paging.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002u
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define COMMAND_SIZE 80

static volatile uint16_t *const vga = (volatile uint16_t *)0xB8000;
static uint8_t row;
static uint8_t column;
static volatile uint32_t uptime_ticks;

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void serial_init(void) {
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x80);
    outb(0x3F8, 0x03);
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x03);
    outb(0x3FA, 0xC7);
    outb(0x3FC, 0x0B);
}

static void serial_putc(char c) {
    uint32_t timeout = 100000;
    while (!(inb(0x3FD) & 0x20) && --timeout) {
    }
    if (timeout) {
        outb(0x3F8, (uint8_t)c);
    }
}

static void console_clear(void) {
    for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        vga[i] = 0x0700 | ' ';
    }
    row = 0;
    column = 0;
}

static void console_scroll(void) {
    for (uint32_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); ++i) {
        vga[i] = vga[i + VGA_WIDTH];
    }
    for (uint32_t i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        vga[i] = 0x0700 | ' ';
    }
    row = VGA_HEIGHT - 1;
}

static void console_putc(char c) {
    serial_putc(c);

    if (c == '\r') {
        column = 0;
        return;
    }
    if (c == '\n') {
        column = 0;
        if (++row >= VGA_HEIGHT) {
            console_scroll();
        }
        return;
    }
    if (c == '\b') {
        if (column == 0) {
            if (row == 0) {
                return;
            }
            --row;
            column = VGA_WIDTH - 1;
        } else {
            --column;
        }
        vga[row * VGA_WIDTH + column] = 0x0700 | ' ';
        return;
    }

    vga[row * VGA_WIDTH + column] = 0x0700 | (uint8_t)c;
    if (++column >= VGA_WIDTH) {
        column = 0;
        if (++row >= VGA_HEIGHT) {
            console_scroll();
        }
    }
}

static void console_write(const char *text) {
    while (*text) {
        console_putc(*text++);
    }
}

static void console_write_u32(uint32_t value) {
    char digits[10];
    uint32_t length = 0;

    do {
        digits[length++] = (char)('0' + value % 10);
        value /= 10;
    } while (value && length < sizeof(digits));

    while (length) {
        console_putc(digits[--length]);
    }
}

static int string_equal(const char *left, const char *right) {
    while (*left && *left == *right) {
        ++left;
        ++right;
    }
    return *left == *right;
}

static void shell_execute(const char *command) {
    if (string_equal(command, "help")) {
        console_write("Commands: help, about, uptime, clear\r\n");
    } else if (string_equal(command, "about")) {
        console_write("BlueBallOS x86 kernel, Multiboot v1, 32-bit protected mode.\r\n");
        console_write("This build is an educational kernel foundation, not a desktop OS.\r\n");
    } else if (string_equal(command, "uptime")) {
        console_write("Timer ticks: ");
        console_write_u32(uptime_ticks);
        console_write(" (100 Hz)\r\n");
    } else if (string_equal(command, "clear")) {
        console_clear();
    } else if (command[0]) {
        console_write("Unknown command. Type help.\r\n");
    }
}

static void shell_run(void) {
    char command[COMMAND_SIZE];
    uint32_t length = 0;

    console_write("BlueBallOS kernel ready. Type help.\r\n");
    console_write("blueballos> ");

    for (;;) {
        char key;
        __asm__ volatile ("sti; hlt" : : : "memory");

        while (keyboard_read(&key)) {
            if (key == '\n') {
                console_write("\r\n");
                command[length] = '\0';
                shell_execute(command);
                length = 0;
                console_write("blueballos> ");
            } else if (key == '\b') {
                if (length) {
                    --length;
                    console_putc('\b');
                }
            } else if (key >= 0x20 && key < 0x7F && length < COMMAND_SIZE - 1) {
                command[length++] = key;
                console_putc(key);
            }
        }
    }
}

void irq_dispatch(uint32_t vector) {
    if (vector == 32) {
        ++uptime_ticks;
    } else if (vector == 33) {
        keyboard_irq_handler();
    }
    outb(0x20, 0x20);
}

void exception_dispatch(uint32_t vector) {
    __asm__ volatile ("cli");
    console_write("\r\nKERNEL PANIC: CPU exception ");
    console_write_u32(vector);
    console_write("\r\nSystem halted.\r\n");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void kernel_main(uint32_t magic, uint32_t multiboot_info) {
    (void)multiboot_info;
    serial_init();
    console_clear();
    console_write("BlueBallOS booting...\r\n");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        console_write("Boot error: invalid Multiboot v1 bootloader signature.\r\n");
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    console_write("[ OK ] Multiboot handoff\r\n");
    paging_init();
    heap_init();
    console_write("[ OK ] Identity-mapped paging and kernel heap\r\n");

    interrupts_init();
    keyboard_init();
    pit_init(100);
    console_write("[ OK ] IDT, PIC, PIT and PS/2 keyboard IRQs\r\n");

    while (!uptime_ticks) {
        __asm__ volatile ("sti; hlt" : : : "memory");
    }
    console_write("[ OK ] PIT interrupt clock running\r\n");
    shell_run();
}