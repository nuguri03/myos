#ifndef SCHEDULER_H
#define SCHEDULER_H

struct task_struct;

struct task_struct* get_sched_current();

void sched_init(struct task_struct* boot_task);
__attribute__((noreturn)) void sched_exit();

void scheduler_tick();
void yield();

#endif