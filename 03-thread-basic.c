#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <pthread.h>

struct thread_arg {
    int thread_idx;
    off_t* shared_buffer_offset;
    char* shared_buffer;
    size_t shared_buffer_size;
    pthread_mutex_t* mutex;
};

void* thread_func(void* arg) {
    struct thread_arg tharg = *(struct thread_arg*)arg;
    ssize_t write_result = 0;

    for (int cnt = 0; cnt < 10; ++cnt) {
        int printf_result = snprintf(tharg.shared_buffer + *tharg.shared_buffer_offset, tharg.shared_buffer_size, "Hello, world! This is thread %d.\n", tharg.thread_idx);
        if (printf_result < 0) {
            char error_msg[] = "Error: snprintf() failed.\n";
            write(STDERR_FILENO, error_msg, sizeof(error_msg) - 1);
            return NULL;
        }
        *tharg.shared_buffer_offset += printf_result;
        usleep(0);
    }
}

void* synchronized_thread_func(void* arg) {
    struct thread_arg tharg = *(struct thread_arg*)arg;
    ssize_t write_result = 0;

    for (int cnt = 0; cnt < 10; ++cnt) {
        if (pthread_mutex_lock(tharg.mutex) != 0) {
            char error_msg[] = "Error: pthread_mutex_lock() failed.\n";
            write(STDERR_FILENO, error_msg, sizeof(error_msg) - 1);
            return NULL;
        }
        int printf_result = snprintf(tharg.shared_buffer + *tharg.shared_buffer_offset, tharg.shared_buffer_size, "Hello, world! This is thread %d.\n", tharg.thread_idx);
        if (printf_result < 0) {
            char error_msg[] = "Error: snprintf() failed.\n";
            write(STDERR_FILENO, error_msg, sizeof(error_msg) - 1);
            return NULL;
        }
        *tharg.shared_buffer_offset += printf_result;
        if (pthread_mutex_unlock(tharg.mutex) != 0) {
            char error_msg[] = "Error: pthread_mutex_unlock() failed.\n";
            write(STDERR_FILENO, error_msg, sizeof(error_msg) - 1);
            return NULL;
        }
        usleep(0);
    }
}

int main(int argc, char** argv) {
    int result = 0;
    pthread_t threads[3] = { 0, };
    off_t shared_buffer_offset = 0;
    char shared_buffer[2048];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    struct thread_arg thread_args[3] = {
        { 0, &shared_buffer_offset, shared_buffer, sizeof(shared_buffer), &mutex },
        { 1, &shared_buffer_offset, shared_buffer, sizeof(shared_buffer), &mutex },
        { 2, &shared_buffer_offset, shared_buffer, sizeof(shared_buffer), &mutex },
    };

    pthread_attr_t thread_attr = { 0, };

    for (int idx = 0; idx < 3; ++idx) {
        const int create_result =
            pthread_create(&threads[idx], &thread_attr, thread_func, (void*)&thread_args[idx]);
        if (create_result != 0) {
            result = 1;
            goto main_exit;
        }
    }

    for (int idx = 0; idx < 3; ++idx) {
        const int join_result = pthread_join(threads[idx], NULL);
        if (join_result != 0) {
            result = 1;
            goto main_exit;
        }
    }

    const ssize_t written_bytes = write(STDOUT_FILENO, shared_buffer, *thread_args->shared_buffer_offset);
    if (written_bytes != *thread_args->shared_buffer_offset) {
        result = 1;
        goto main_exit;
    }

    void* memset_result = memset(shared_buffer, 0, sizeof(shared_buffer));
    if (memset_result == NULL) {
        result = 1;
        goto main_exit;
    }

    for (int idx = 0; idx < 3; ++idx) {
        const int create_result =
            pthread_create(&threads[idx], &thread_attr, synchronized_thread_func, (void*)&thread_args[idx]);
        if (create_result != 0) {
            result = 1;
            goto main_exit;
        }
    }

    for (int idx = 0; idx < 3; ++idx) {
        const int join_result = pthread_join(threads[idx], NULL);
        if (join_result != 0) {
            result = 1;
            goto main_exit;
        }
    }

    const ssize_t written_bytes2 = write(STDOUT_FILENO, shared_buffer, *thread_args->shared_buffer_offset);
    if (written_bytes2 != *thread_args->shared_buffer_offset) {
        result = 1;
        goto main_exit;
    }

main_exit:
    return result;
}