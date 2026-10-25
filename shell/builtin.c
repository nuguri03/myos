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
        kprintf("help  - show available commands\n");
        kprintf("clear - clear the screen\n");
        kprintf("echo  - print arguments\n");
        return true;
    }

    if (kstrcmp(argv[0], "clear") == 0) {
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
        const struct pmm_stats* stats = pmm_get_stats();

        kprintf("Usable:    %u KB\n", stats->usable_pages * 4u);
        kprintf("Reserved:  %u KB\n", stats->reserved_pages * 4u);
        kprintf("Allocated: %u KB\n", stats->allocated_pages * 4u);
        kprintf("Free:      %u KB\n", stats->free_pages * 4u);

        serial_printf("Usable:    %u KB\n", stats->usable_pages * 4u);
        serial_printf("Reserved:  %u KB\n", stats->reserved_pages * 4u);
        serial_printf("Allocated: %u KB\n", stats->allocated_pages * 4u);
        serial_printf("Free:      %u KB\n", stats->free_pages * 4u);

        return true;
    }

    return false;
}