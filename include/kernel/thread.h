#ifndef THREAD_H
#define THREAD_H

#include "types.h"

struct thread {
    u32 esp;    // stack pointer
    void* stack_base; // stack base address
    struct thread* next;
};

// 
void context_switch(u32* old_esp, u32 new_esp);

// stack을 확보하고, context_switch가 복원할 초기 상태를 만듦
bool init_thread(struct thread* thread, void (*entry)(void));

void init_thread_system(struct thread* boot_thread);

void yield();

#endif