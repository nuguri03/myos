#include "shell/shell.h"
#include "shell/input.h"
#include "shell/parser.h"
#include "shell/builtin.h"

#include "kstdio.h"
#include "driver/serial.h"
#include "driver/video.h"

static bool has_unsupported_syntax(const char* line) {
    while (*line != '\0') {
        switch (*line) {
            case '|':
            case '<':
            case '>':
            case '&':
            case '\'':
            case '"':
            case '\\':
            case '$':
            case ';':
                return true;
        }

        line++;
    }

    return false;
}

void shell_run(void) {
    clear_vga();

    char line[128];
    char* argv[9];

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

        serial_printf("input: %s\n", line);

        if (has_unsupported_syntax(line)) {
            kprintf("shell: unsupported syntax\n");
            serial_printf("shell: unsupported syntax\n");
            continue;
        }

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
            serial_printf("argv[%u] = %s\n", (unsigned int)i, argv[i]);
        }

        if (!shell_execute_builtin((size_t)argc, argv)) {
            kprintf("shell: unknown command: %s\n", argv[0]);
            serial_printf("shell: unknown command: %s\n", argv[0]);
        }
    }
}