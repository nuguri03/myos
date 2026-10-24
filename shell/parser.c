#include "shell/parser.h"

ssize_t shell_parse(char *line, char** argv, size_t argv_capacity) {
    if (line == NULL || argv == NULL || argv_capacity < 1) {
        return -1;
    }

    size_t argc = 0;
    char *p = line;
    argv[0] = NULL;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        if (argc >= argv_capacity - 1) {
            return -1;
        }

        argv[argc++] = p;

        while (*p != ' ' && *p != '\t' && *p != '\0') {
            p++;
        }

        if (*p != '\0') {
            *p = '\0';
            p++;
        }

        argv[argc] = NULL;
    }

    return (ssize_t)argc;
}