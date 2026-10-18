#include "kernel/kthread.h"
#include "kernel/kmalloc.h"
#include "kernel/kfree.h"

#define THREAD_STACK_SIZE 4096
#define TIME_SLICE_TICKS    10

static struct task_struct* current = NULL;

// 기존 EFLAGS를 읽어 보관한 뒤 IF=0으로 설정
static inline u32 irq_save() {
    u32 flags;

    __asm__ volatile(
        "pushfl\n\t"
        "popl %0\n\t"
        "cli"
        : "=r"(flags)
        :
        : "memory"
    );

    return flags;
}

// 보관했던 EFLAGS를 복원
static inline void irq_restore(u32 flags) {
    __asm__ volatile(
        "pushl %0\n\t"
        "popfl"
        :
        : "r"(flags)
        : "memory", "cc"
    );
}

static __attribute__((noreturn)) void kthread() {
    __asm__ volatile("sti" ::: "memory");

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
    __asm__ volatile("cli" ::: "memory");

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
    u32 flags = irq_save();

    struct task_struct* prev = current;
    struct task_struct* next = prev->next;

    if (prev == next) {
        irq_restore(flags);
        return;
    }

    current = next;
    context_switch(&prev->esp, next->esp);
    irq_restore(flags);
}

void scheduler_tick() {
    if (current == NULL) {
        return;
    }

    static u32 sched_tick = 0;

    sched_tick++;
    if (sched_tick < TIME_SLICE_TICKS) {
        return;
    }

    sched_tick = 0;
    yield();
    return;
}