#include "shell/shell.h"
#include "shell/input.h"

#include "kstdio.h"
#include "driver/serial.h"
#include "driver/video.h"

void shell_run() {
    clear_vga();

    char line[128];

    while (1) {
        kprintf("myos> ");


        ssize_t len = shell_readline(line, sizeof(line));

        if (len < 0) {
            kprintf("shell: input error\n");
            serial_printf("shell: input error\n");
            return;
        }

        if (len == 0) {
            continue;
        }

        kprintf("input: %s\n", line);
        serial_printf("input: %s\n", line);
    }
}