#pragma once
#include <stdint.h>
void paging_init(void);
int paging_map(uint32_t virt, uint32_t phys, uint32_t flags);
uint32_t paging_translate(uint32_t virt);
