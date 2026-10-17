#include "kernel/thread.h"
#include "kernel/kmalloc.h"

#define THREAD_STACK_SIZE 4096

static struct thread* current_thread = NULL;

void init_thread_system(struct thread* boot_thread) {
    boot_thread->stack_base = NULL;
    boot_thread->next = boot_thread;
    current_thread = boot_thread;
}

bool init_thread(struct thread* thread, void (*entry)(void)) {
    if (thread == NULL || entry == NULL) {
        return false;
    }

    void* stack = kmalloc(THREAD_STACK_SIZE);
    if (stack == NULL) {
        return false;
    }

    u32 top = (u32)stack + THREAD_STACK_SIZE;

    top &= ~0xF; // 16바이트 정렬

    u32* sp = (u32*)top;

    *(--sp) = 0;           // worker_entry가 반환할 주소 자리
    *(--sp) = (u32)entry;  // context_switch가 반환할 주소
    *(--sp) = 0;            // EBP
    *(--sp) = 0;            // EBX
    *(--sp) = 0;            // ESI
    *(--sp) = 0;            // EDI

    thread->stack_base = stack;
    thread->esp = (u32)sp;

    return true;
}

void yield() {
    struct thread* prev = current_thread;
    struct thread* next = prev->next;

    if (prev == next) {
        return;
    }

    current_thread = next;
    context_switch(&prev->esp, next->esp);
}