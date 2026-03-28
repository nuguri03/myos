#include "video.h"
#include "printf.h"

void main() {
    clear_screen();

    char* msg = "Hello Kernel!";

    // char* test_msg =
    //     "01: Hello MYOS! Testing screen boundaries... 0123456789abcdefghijklmnopqrstuvwxyz\n"
    //     "02: Line two of the VGA display test. We are checking if the cursor wraps well.\n"
    //     "03: Systems programming is all about controlling the hardware directly. Cool!!\n"
    //     "04: [DEBUG] Checking offset calculation... pos.y * WIDTH + pos.x should be ok.\n"
    //     "05: Writing to 0xB8000 is like painting on a digital canvas with ASCII chars.\n"
    //     "06: Let's fill this screen until it cries for a scroll function! Go go go go!!\n"
    //     "07: Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod...\n"
    //     "08: The quick brown fox jumps over the lazy dog. Classic string for any test.\n"
    //     "09: 0x07 is the default color: Light Gray on Black. Bitwise OR is our friend.\n"
    //     "10: Halfway to the bottom of the standard 80x25 VGA text mode buffer area.\n"
    //     "11: If you see this, your kstrlen and vga_print loop are working together.\n"
    //     "12: C, C++, and Rust are the pillars of low-level software. Great choice, YM!\n"
    //     "13: Does your cursor.x >= WIDTH check trigger correctly at the end of line?\n"
    //     "14: Kernel development requires patience and a lot of debugging via screen.\n"
    //     "15: Almost there. The screen should be looking pretty full by this point now.\n"
    //     "16: 1234567890!@#$%^&*()_+~`[]{};:'\",.<>/? - All symbols should be visible.\n"
    //     "17: Let's see if the next few lines trigger any hidden bugs in your logic.\n"
    //     "18: Still printing... Still printing... The buffer is getting very crowded.\n"
    //     "19: 19th line. Only 6 lines left until we hit the 25th line limit of VGA.\n"
    //     "20: Row 20. Just five more to go. Prepare for the bottom of the display!!\n"
    //     "21: Is your cursor.y being incremented correctly on every newline character?\n"
    //     "22: Penultimate lines are always the most exciting in a buffer overflow test.\n"
    //     "23: This is the 23rd line. The edge of the world is just two lines away now.\n"
    //     "24: LINE 24: THE LAST VISIBLE LINE. THE NEXT ONE WILL GO OFF THE SCREEN!!\n"
    //     "25: LINE 25: This is exactly at the bottom. Will the next char disappear?\n"
    //     "26: EXTRA LINE: If you see this without scrolling, it might overwrite line 0!\n";

    kprintf("%s\n%x\n", msg, 125);
    
    while (1) {}
}