#include <sys/stat.h>
#include <unistd.h>

/* System call stubs */

void _exit(int status) {
    while (1) {
        // Infinite loop - halt execution
    }
}

int _close(int file) {
    return -1; // Error for unsupported file operations
}

int _lseek(int file, int ptr, int dir) {
    return 0; // Return 0, lseek not supported
}

int _read(int file, char *ptr, int len) {
    return 0; // Return 0, read not supported
}

int _write(int file, char *ptr, int len) {
    return len; // Assume all bytes are written successfully
}

caddr_t _sbrk(int incr) {
    extern char end; // Defined in the linker script
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &end; // Initialize heap_end to end of bss
    }

    prev_heap_end = heap_end;
    heap_end += incr; // Increment the heap
    return (caddr_t)prev_heap_end;
}
