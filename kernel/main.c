void main() {
    char* video_memory = (char*) 0xB8000;

    char* msg = "Welcome to C kernel";

    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i+1] = 0x1F;
    }
    
    int i = 0;
    int idx = 0;
    while (msg[idx] != '\0') {
        video_memory[i] = msg[idx++];
        video_memory[i+1] = 0x1E;
        i += 2;
    }
    
    while (1) {}
}