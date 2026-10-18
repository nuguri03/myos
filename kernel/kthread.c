#include "kernel/kthread.h"
#include "kernel/kmalloc.h"
#include "kernel/kfree.h"

#define THREAD_STACK_SIZE 4096

static struct task_struct* current = NULL;

static __attribute__((noreturn)) void kthread() {
    i32 ret = current->threadfn(current->data);
    kthread_exit(ret);
}

void sched_init(struct task_struct* boot_task) {
    boot_task->stack_base = NULL;
    boot_task->next = boot_task;
    boot_task->exited = false;
    current = boot_task;
}

bool kthread_create(struct task_struct* task, i32 (*threadfn)(void*), void* data) {
    if (task == NULL || threadfn == NULL) {
        return false;
    }

    task->threadfn = threadfn;
    task->data = data;
    task->exit_code = 0;
    task->exited = false;

    void* stack = kmalloc(THREAD_STACK_SIZE);
    if (stack == NULL) {
        return false;
    }

    u32 top = (u32)stack + THREAD_STACK_SIZE;

    top &= ~0xF; // 16바이트 정렬

    u32* sp = (u32*)top;

    *(--sp) = 0;             // kthread는 반환하지 않음
    *(--sp) = (u32)kthread;  // threadfn 대신 래퍼에서 시작
    *(--sp) = 0;             // EBP
    *(--sp) = 0;             // EBX
    *(--sp) = 0;             // ESI
    *(--sp) = 0;             // EDI

    task->stack_base = stack;
    task->esp = (u32)sp;

    return true;
}

void kthread_exit(i32 result) {
    struct task_struct* dead = current;

    dead->exit_code = result;
    dead->exited = true;

    // lastest task dead -> STOP CPU
    if (dead->next == dead) {
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }

    struct task_struct* prev = dead;
    while (prev->next != dead) {
        prev = prev->next;
    }

    struct task_struct* next = dead->next;

    prev->next = next;
    
    current = next;
    context_switch(&dead->esp, next->esp);

    // 정상적인 흐름에서는 작동하지 않음
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}

bool kthread_reap(struct task_struct* task) {
    if (task == NULL || task == current) {
        return false;
    }

    if (!task->exited || task->stack_base == NULL) {
        return false;
    }

    kfree(task->stack_base);

    task->stack_base = NULL;
    task->esp = 0;
    task->next = NULL;

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