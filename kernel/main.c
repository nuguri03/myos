#include "video.h"
#include "string.h"

void main() {
    clear_screen();
    
    i8 *msg = "Hello MYOS!";
    print_string(msg, strlen(msg));
    
    while (1) {}
}