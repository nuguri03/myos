#include "shell/shell.h"
#include "shell/input.h"
#include "shell/parser.h"

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

        char *argv[9];
        ssize_t argc = shell_parse(line, argv, sizeof(argv) / sizeof(argv[0]));

        if (argc < 0) {
            kprintf("shell: too many arguments\n");
            serial_printf("shell: too many arguments\n");
            continue;
        }

        if (argc == 0) {
            continue;
        }

        for (size_t i = 0; i < (size_t)argc; i++) {
            kprintf("argv[%u] = %s\n", (unsigned int)i, argv[i]);
            serial_printf("argv[%u] = %s\n", (unsigned int)i, argv[i]);
        }
    }
}