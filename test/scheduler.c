#include "selftest.h"
#include "kernel/kthread.h"
#include "kernel/scheduler.h"
#include "driver/pit.h"
#include "driver/serial.h"

static volatile u32 counter_a;
static volatile u32 counter_b;

static i32 preempt_worker_a(void* data) {
    (void)data;
    u32 start = pit_get_ticks();

    while ((u32)(pit_get_ticks() - start) < 2000) {
        counter_a++;
    }

    return 11;
}

static i32 preempt_worker_b(void* data) {
    (void)data;
    u32 start = pit_get_ticks();

    while ((u32)(pit_get_ticks() - start) < 3000) {
        counter_b++;
    }

    return 22;
}

void test_preemption(void) {
    __asm__ volatile("cli" ::: "memory");

    struct task_struct* main_task = get_sched_current();

    static struct task_struct worker_a;
    static struct task_struct worker_b;

    if (main_task == NULL || main_task->next != main_task) {
        serial_printf("[FAIL] test requires only the boot task\n");
        return;
    }

    if (!kthread_create(&worker_a, preempt_worker_a, NULL)) {
        serial_printf("[FAIL] worker A creation\n");
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }

    if (!kthread_create(&worker_b, preempt_worker_b, NULL)) {
        serial_printf("[FAIL] worker B creation\n");
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }

    // boot task -> A -> B -> boot task
    if (!sched_add(&worker_a) || !sched_add(&worker_b)) {
        serial_printf("[FAIL] scheduler registration\n");
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }

    counter_a = 0;
    counter_b = 0;

    bool reaped_a = false;
    bool reaped_b = false;
    bool both_reaped = false;

    u32 last_tick = pit_get_ticks();
    u32 reaped_tick = 0;

    serial_printf("[TEST] preemption, exit and reap\n");

    for (;;) {
        __asm__ volatile("cli" ::: "memory");

        u32 now = pit_get_ticks();

        if (!reaped_a && worker_a.exited) {
            i32 result = worker_a.exit_code;
            reaped_a = kthread_reap(&worker_a);

            serial_printf(
                "A: exit=%d, reap=%u\n",
                result, (u32)reaped_a
            );
        }

        if (!reaped_b && worker_b.exited) {
            i32 result = worker_b.exit_code;
            reaped_b = kthread_reap(&worker_b);

            serial_printf(
                "B: exit=%d, reap=%u\n",
                result, (u32)reaped_b
            );
        }

        if ((u32)(now - last_tick) >= 1000) {
            serial_printf(
                "ticks=%u, A=%u, B=%u\n",
                now, counter_a, counter_b
            );
            last_tick = now;
        }

        if (!both_reaped && reaped_a && reaped_b) {
            both_reaped = true;
            reaped_tick = now;
        }

        // 두 스택을 회수한 뒤에도 약 1초간 타이머가 동작하는지 확인
        if (both_reaped &&
            (u32)(now - reaped_tick) >= 1000) {
            bool passed =
                worker_a.exit_code == 11 &&
                worker_b.exit_code == 22 &&
                worker_a.stack_base == NULL &&
                worker_b.stack_base == NULL &&
                main_task->next == main_task;

            serial_printf(
                "[%s] exit codes, stack cleanup, main-only timer\n",
                passed ? "PASS" : "FAIL"
            );
            return;
        }

        __asm__ volatile("sti; hlt" ::: "memory");
    }
}