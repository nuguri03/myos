void main() {
    char* video_memory = (char*) 0xB8000;

    char msg[20] = "Welcome to C kernel"; // char* msg 가 안되는 이유를 알아야 함.

    for (int k = 0; k < 80 * 25 * 2; k += 2) {
        video_memory[k] = ' ';
        video_memory[k+1] = 0x1F;
    }
    
    
    int j = 0;
    int idx = 0;
    while (msg[idx] != '\0') {
        video_memory[j++] = msg[idx++];
        video_memory[j++] = 0x1E;
    }
    
    while (1) {}
}