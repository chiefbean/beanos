void main() {
    char* video_memory = (char*) 0xb8000;
    
    *(video_memory + 2*81) = 'y';
}
