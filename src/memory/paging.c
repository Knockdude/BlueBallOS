#include "paging.h"
#include <stdint.h>

__attribute__((aligned(4096))) static uint32_t page_directory[1024];
__attribute__((aligned(4096))) static uint32_t first_table[1024];

void paging_init(void) {
    for (int i=0;i<1024;i++) {
        first_table[i] = (i * 0x1000) | 3;
        page_directory[i] = 0;
    }
    page_directory[0] = ((uint32_t)first_table) | 3;
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory));
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000u;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}
int paging_map(uint32_t virt, uint32_t phys, uint32_t flags) {
    if (virt >= 0x00400000u) return -1;

    uint32_t page = virt >> 12;
    first_table[page] = (phys & 0xFFFFF000u) | (flags & 0x00000FFFu) | 1u;
    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
    return 0;
}

uint32_t paging_translate(uint32_t virt) {
    if (virt >= 0x00400000u) return 0;

    uint32_t entry = first_table[virt >> 12];
    if (!(entry & 1u)) return 0;
    return (entry & 0xFFFFF000u) | (virt & 0x00000FFFu);
}
