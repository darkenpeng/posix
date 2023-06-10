#include <sys/fcntl.h>
#include <sys/unistd.h>

int main(int argc, char** argv) {
    int result = 0;
    int file_desc = open("file.txt", O_CREAT | O_RDWR, 0666);
    const char message[] = "Hello, world!\n";
    const ssize_t written_bytes = write(file_desc, message, sizeof(message) - 1);
    if (written_bytes != sizeof(message)) {
        result = 1;
        goto exit_close;
    }

    const int close_result = close(file_desc);
    if (close_result != 0) {
        result = 1;
    }

exit_close:
    return result;
}
