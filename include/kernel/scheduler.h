#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

struct task_struct;

struct task_struct* get_sched_current();

void sched_init(struct task_struct* boot_task);
__attribute__((noreturn)) void sched_exit();

void scheduler_tick();
void yield();

bool sched_add(struct task_struct* task);

#endif