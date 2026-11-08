#pragma once
#include <stdint.h>
void scheduler_init(void);
void scheduler_tick(void);
int task_create(void (*entry)(void));
void scheduler_yield(void);
