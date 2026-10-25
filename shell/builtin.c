#include "shell/builtin.h"
#include "driver/video.h"
#include "kernel/pmm.h"
#include "lib/kstring.h"
#include "kstdio.h"

#include "driver/serial.h"

bool shell_execute_builtin(size_t argc, char** argv) {
    if (argc == 0 || argv == NULL || argv[0] == NULL) {
        return false;
    }

    if (kstrcmp(argv[0], "help") == 0) {
        if (argc != 1) {
            kprintf("usage: help\n");
            return true;
        }
        kprintf("help  - show available commands\n");
        kprintf("clear - clear the screen\n");
        kprintf("echo  - print arguments\n");
        kprintf("mem   - show physical memory statistics\n");
        return true;
    }

    if (kstrcmp(argv[0], "clear") == 0) {
        if (argc != 1) {
            kprintf("usage: clear\n");
            return true;
        }
        clear_vga();
        return true;
    }

    if (kstrcmp(argv[0], "echo") == 0) {
        for (size_t i = 1; i < argc; i++) {
            if (i > 1) {
                kprintf(" ");
            }

            kprintf("%s", argv[i]);
        }

        kprintf("\n");
        return true;
    }

    if (kstrcmp(argv[0], "mem") == 0) {
        if (argc != 1) {
            kprintf("usage: mem\n");
            return true;
        }
        const struct pmm_stats* stats = pmm_get_stats();

        kprintf("Usable:    %u KiB\n", stats->usable_pages * 4u);
        kprintf("Reserved:  %u KiB\n", stats->reserved_pages * 4u);
        kprintf("Allocated: %u KiB\n", stats->allocated_pages * 4u);
        kprintf("Free:      %u KiB\n", stats->free_pages * 4u);

        serial_printf("Usable:    %u KiB\n", stats->usable_pages * 4u);
        serial_printf("Reserved:  %u KiB\n", stats->reserved_pages * 4u);
        serial_printf("Allocated: %u KiB\n", stats->allocated_pages * 4u);
        serial_printf("Free:      %u KiB\n", stats->free_pages * 4u);

        return true;
    }

    return false;
}