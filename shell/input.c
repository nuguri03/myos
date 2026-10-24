#include "shell/input.h"
#include "driver/keyboard.h"
#include "driver/video.h"

ssize_t shell_readline(char *buffer, size_t capacity) {
    if (buffer == NULL || capacity < 1) {
        return -1;
    }

    ssize_t len = 0;
    buffer[0] = '\0';

    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\0') {
            continue;
        }

        // Press Enter('\n')
        else if (c == '\n') {
            buffer[len] = '\0';
            vga_putchar('\n');
            return len;
        }
        
        // Press Backspace(DEL)
        else if (c == 127 || c == '\b') {
            if (len > 0) {
                buffer[--len] = '\0';
                vga_putchar('\b');
            }
        }

        // input
        else if (c >= ' ' && c <= '~') {
            if ((size_t)len < capacity - 1) {
                buffer[len++] = c;
                buffer[len] = '\0';
                vga_putchar(c);
            }
        }
    }
}