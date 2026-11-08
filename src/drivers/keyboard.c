#include "keyboard.h"

#include <stdint.h>

#define KEY_BUFFER_SIZE 128
#define KEY_BUFFER_MASK (KEY_BUFFER_SIZE - 1)

static volatile char key_buffer[KEY_BUFFER_SIZE];
static volatile uint8_t head;
static volatile uint8_t tail;
static uint8_t left_shift;
static uint8_t right_shift;
static uint8_t caps_lock;

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void enqueue_key(char key) {
    uint8_t next = (uint8_t)(head + 1);
    if ((uint8_t)(next - tail) > KEY_BUFFER_SIZE) {
        return;
    }
    key_buffer[head & KEY_BUFFER_MASK] = key;
    head = next;
}

static char translate_scancode(uint8_t code, uint8_t shifted) {
    static const char digits[] = "1234567890";
    static const char shifted_digits[] = "!@#$%^&*()";
    static const char top_row[] = "qwertyuiop";
    static const char home_row[] = "asdfghjkl";
    static const char bottom_row[] = "zxcvbnm";

    if (code >= 0x02 && code <= 0x0B) {
        return shifted ? shifted_digits[code - 0x02] : digits[code - 0x02];
    }
    if (code >= 0x10 && code <= 0x19) {
        char key = top_row[code - 0x10];
        return shifted ? (char)(key - 'a' + 'A') : key;
    }
    if (code >= 0x1E && code <= 0x26) {
        char key = home_row[code - 0x1E];
        return shifted ? (char)(key - 'a' + 'A') : key;
    }
    if (code >= 0x2C && code <= 0x32) {
        char key = bottom_row[code - 0x2C];
        return shifted ? (char)(key - 'a' + 'A') : key;
    }

    switch (code) {
        case 0x0C: return shifted ? '_' : '-';
        case 0x0D: return shifted ? '+' : '=';
        case 0x1A: return shifted ? '{' : '[';
        case 0x1B: return shifted ? '}' : ']';
        case 0x1C: return '\n';
        case 0x27: return shifted ? ':' : ';';
        case 0x28: return shifted ? '"' : '\'';
        case 0x29: return shifted ? '~' : '`';
        case 0x2B: return shifted ? '|' : '\\';
        case 0x33: return shifted ? '<' : ',';
        case 0x34: return shifted ? '>' : '.';
        case 0x35: return shifted ? '?' : '/';
        case 0x39: return ' ';
        case 0x0E: return '\b';
        default: return 0;
    }
}

void keyboard_init(void) {
    head = 0;
    tail = 0;
    left_shift = 0;
    right_shift = 0;
    caps_lock = 0;
}

void keyboard_irq_handler(void) {
    uint8_t scancode = inb(0x60);
    uint8_t released = (uint8_t)(scancode & 0x80);
    uint8_t code = (uint8_t)(scancode & 0x7F);

    if (code == 0x2A) {
        left_shift = (uint8_t)!released;
        return;
    }
    if (code == 0x36) {
        right_shift = (uint8_t)!released;
        return;
    }
    if (released) {
        return;
    }
    if (code == 0x3A) {
        caps_lock = (uint8_t)!caps_lock;
        return;
    }

    uint8_t shifted = (uint8_t)(left_shift || right_shift);
    char key = translate_scancode(code, shifted);
    if (caps_lock && key >= 'a' && key <= 'z') {
        key = (char)(key - 'a' + 'A');
    } else if (caps_lock && key >= 'A' && key <= 'Z') {
        key = (char)(key - 'A' + 'a');
    }
    if (key) {
        enqueue_key(key);
    }
}

int keyboard_read(char *out) {
    if (!out || tail == head) {
        return 0;
    }
    *out = key_buffer[tail & KEY_BUFFER_MASK];
    tail = (uint8_t)(tail + 1);
    return 1;
}
