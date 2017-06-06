#ifndef CMCM_H
#define CMCM_H

#include <stdint.h>

// stacks for all tasks are allocated statically so tune
// these values so as to not needlessly waste RAM
#ifndef CMCM_MAX_NUM_TASKS
#define CMCM_MAX_NUM_TASKS 8
#endif

#ifndef CMCM_STACK_SIZE
#define CMCM_STACK_SIZE 2048
#endif

void cmcm_create_task(void (*handler)(void));
int cmcm_current_task(void);
void cmcm_context_switch(void);
void cmcm_yield(void);
void cmcm_delay(uint32_t ticks);
void cmcm_sleep(void);
void cmcm_wake(int task_id);

void cmcm_disable_interrupts(void);
void cmcm_enable_interrupts(void);

#include "mutex.h"
#include "queue.h"
#include "tick.h"

#endif
