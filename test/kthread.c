#include "selftest.h"
#include "kernel/kthread.h"
#include "kernel/scheduler.h"
#include "driver/serial.h"
#include "kstdlib.h"

static struct task_struct worker_task;

static i32 worker_entry(void* data) {
    u32* shared = (u32*)data;

    for (u32 i = 1; i <= 3; i++) {
        *shared += 1;
        serial_printf("worker: %u, data=%u\n", i, *shared);
        yield();
    }

    return 42;
}

void test_thread(void) {
    struct task_struct* main_task = get_sched_current();

    if (main_task == NULL || main_task->next != main_task) {
        serial_printf("FAIL: test requires only the boot task\n");
        return;
    }

    u32* shared = kmalloc(sizeof(u32));
    if (shared == NULL) {
        serial_printf("FAIL: shared allocation\n");
        return;
    }

    *shared = 123;

    if (!kthread_create(&worker_task, worker_entry, shared)) {
        serial_printf("FAIL: kthread_create\n");
        kfree(shared);
        return;
    }

    if (!sched_add(&worker_task)) {
        serial_printf("FAIL: scheduler registration\n");

        // 등록되지 않았으므로 실행된 적 없는 스택을 직접 해제
        kfree(worker_task.stack_base);
        worker_task.stack_base = NULL;
        worker_task.esp = 0;
        worker_task.next = NULL;

        kfree(shared);
        return;
    }

    while (!worker_task.exited) {
        *shared += 1;
        serial_printf("main: %u\n", *shared);
        yield();
    }

    bool passed = true;

    if (worker_task.exit_code != 42) {
        serial_printf("FAIL: worker exit code\n");
        passed = false;
    }

    if (main_task->next != main_task) {
        serial_printf("FAIL: worker still linked\n");
        passed = false;
    }

    // 실행 목록에서 제거됐음을 확인한 뒤 회수
    if (main_task->next == main_task) {
        if (!kthread_reap(&worker_task)) {
            serial_printf("FAIL: worker stack cleanup\n");
            passed = false;
        } else {
            serial_printf("worker stack reclaimed\n");

            if (kthread_reap(&worker_task)) {
                serial_printf("FAIL: accepted repeated reap\n");
                passed = false;
            }
        }
    }

    kfree(shared);

    if (passed) {
        serial_printf("PASS: thread exit and cleanup\n");
    }
}
