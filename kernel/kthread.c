#include "kernel/kthread.h"
#include "kernel/kmalloc.h"

#define THREAD_STACK_SIZE 4096

static struct task_struct* current = NULL;

void sched_init(struct task_struct* boot_task) {
    boot_task->stack_base = NULL;
    boot_task->next = boot_task;
    current = boot_task;
}

bool kthread_create(struct task_struct* task, i32 (*threadfn)(void*), void* data) {
    if (task == NULL || threadfn == NULL) {
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
    *(--sp) = (u32)threadfn;  // context_switch가 반환할 주소
    *(--sp) = 0;            // EBP
    *(--sp) = 0;            // EBX
    *(--sp) = 0;            // ESI
    *(--sp) = 0;            // EDI

    task->stack_base = stack;
    task->esp = (u32)sp;

    return true;
}

void yield() {
    struct task_struct* prev = current;
    struct task_struct* next = prev->next;

    if (prev == next) {
        return;
    }

    current = next;
    context_switch(&prev->esp, next->esp);
}