#ifndef KTHREAD_H
#define KTHREAD_H

#include "types.h"

struct task_struct {
    u32 esp;    // stack pointer
    void* stack_base; // stack base address
    struct task_struct* next;
};

// 
void context_switch(u32* old_esp, u32 new_esp);

// 스택을 확보하고 새 커널 스레드의 초기 상태를 만드는 역할
bool kthread_create(struct task_struct* task, i32 (*threadfn)(void*), void* data);

// 기존 부팅 실행 흐름을 현재 태스크로 등록하고 실행 목록을 초기화하는 역할
void sched_init(struct task_struct* boot_thread);

void yield();

#endif