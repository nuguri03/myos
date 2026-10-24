#include "kernel/scheduler.h"
#include "kernel/kthread.h"

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

void sched_init(struct task_struct* boot_task) {
    boot_task->stack_base = NULL;
    boot_task->next = boot_task;
    boot_task->exited = false;
    current = boot_task;
}

__attribute__((noreturn)) void sched_exit() {
    struct task_struct* dead = current;

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

struct task_struct* get_sched_current() {
    return current;
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