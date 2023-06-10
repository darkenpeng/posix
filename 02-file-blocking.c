#include <stdlib.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>

int print_process_status() {
    int result = 0;
    ssize_t written_bytes = 0;
    ssize_t read_bytes;
    int in_desc = STDIN_FILENO;
    int out_desc = STDOUT_FILENO;

    pid_t this_pid = getpid();
    char proc_status_path[64] = { 0, };
    snprintf(proc_status_path, sizeof(proc_status_path), "/proc/%d/status", this_pid);
    int proc_status_desc = open(proc_status_path, O_RDONLY);
    if (proc_status_desc < 0) {
        result = 1;
        goto func_exit;
    }
    
    char status_buffer[2048] = { 0, };
    read_bytes = read(proc_status_desc, status_buffer, sizeof(status_buffer));
    if (read_bytes <= 0) {
        result = 1;
        goto func_exit;
    }
    written_bytes = write(out_desc, status_buffer, read_bytes);
    if (written_bytes != read_bytes) {
        result = 1;
        goto func_exit;
    }
    const int close_result = close(proc_status_desc);
    if (close_result != 0) {
        result = 1;
        goto func_exit;
    }
    

func_exit:
    return result;
}

int main(int argc, char** argv) {
    int result = 0;
    ssize_t written_bytes = 0;
    ssize_t read_bytes;
    int in_desc = STDIN_FILENO;
    int out_desc = STDOUT_FILENO;
    char buffer[1024] = { 0, };

    int print_result = print_process_status();
    if (print_result != 0) {
        result = 1;
        goto exit_close;
    }

    read_bytes = read(in_desc, buffer, sizeof(buffer));
    if (read_bytes < 0) {
        result = 1;
        goto exit_close;
    }

    written_bytes = write(out_desc, buffer, read_bytes);
    if (written_bytes != read_bytes) {
        result = 1;
        goto exit_close;
    }

exit_close:
    return result;
}
