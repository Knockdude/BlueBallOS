#include "scheduler.h"
#include <stdint.h>
#define MAX_TASKS 16
typedef struct { void (*entry)(void); uint32_t ticks; uint8_t active; } task_t;
static task_t tasks[MAX_TASKS];
static int current;
void scheduler_init(void) { for(int i=0;i<MAX_TASKS;i++) tasks[i].active=0; current=0; }
int task_create(void (*entry)(void)) {
    for(int i=0;i<MAX_TASKS;i++) if(!tasks[i].active){tasks[i]=(task_t){entry,0,1};return i;}
    return -1;
}
void scheduler_tick(void) {
    tasks[current].ticks++;
    current = (current + 1) % MAX_TASKS;
}
void scheduler_yield(void) { scheduler_tick(); }
