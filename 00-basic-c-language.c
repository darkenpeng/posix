#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int leaning_stack_memory() {
    const size_t align = sizeof(void*);
    int result = 0;
    char stack_buffer[64] = { 0, }; // pushed 64 bytes on the stack
    void* ptr = stack_buffer;
    
    printf("current stack pointer: %p\n", ptr);

    int* first = (int*)ptr;
    *first = 123;
    printf("first: %p, %d\n", first, *first);

    ptr += align;
    
    double* second = (double*)ptr;
    *second = 3.14;
    printf("second: %p, %f\n", second, *second);

    struct my_structure {
        int a;
        int b;
        int c;
    };

    ptr += sizeof(struct my_structure) + 4;

    struct my_structure* third = (typeof(third))ptr;
    third->a = 1;
    third->b = 2;
    third->c = 3;

    printf("third: %p, %d, %d, %d\n", third, third->a, third->b, third->c);

    return result;
}

int learning_heap_memory() {
    int result = 0;
    char* heap_buffer = malloc(64);
    if (heap_buffer == NULL) {
        result = 1;
        goto exit_func;
    }

    printf("current heap pointer: %p\n", heap_buffer);

    int* first = (int*)heap_buffer;
    *first = 123;
    printf("first: %p, %d\n", first, *first);

    heap_buffer += sizeof(int);

    double* second = (double*)heap_buffer;
    *second = 3.14;
    printf("second: %p, %f\n", second, *second);

    struct my_structure {
        int a;
        int b;
        int c;
    };

    heap_buffer += sizeof(double) + 4;

    struct my_structure* third = (typeof(third))heap_buffer;
    third->a = 1;
    third->b = 2;
    third->c = 3;

    printf("third: %p, %d, %d, %d\n", third, third->a, third->b, third->c);

exit_func:
    return result;
}

// class definition
struct person {
    char* name;
    int born_year;
    int height;
    int weight;
};

// constructor
int person_init(struct person* this, const char* name, int born_year, int height, int weight) {
    int result = 0;
    this->name = malloc(strlen(name) + 1);
    if (this->name == NULL) {
        result = 1;
        goto exit_func;
    }
    strncpy(this->name, name, strlen(name));
    this->born_year = born_year;
    this->height = height;
    this->weight = weight;
exit_func:
    return result;
}

// class property
int person_get_age(struct person* this) {
    int result = 0;
    
    struct tm now = { 0, };
    time_t now_time = time(NULL);
    localtime_r(&now_time, &now);
    result = now.tm_year + 1900 - this->born_year;

    return result;
}

// destructor
int person_deinit(struct person* this) {
    int result = 0;
    free(this->name);
    return result;
}

// class to_string
int person_to_string(struct person* this, char* buffer, size_t buffer_size) {
    int result = 0;
    result = snprintf(buffer, buffer_size, "name: %s, age: %d, height: %d, weight: %d", this->name, person_get_age(this), this->height, this->weight);
    return result;
}

int learning_oop_in_c() {
    int result = 0;

    // he is John Doe, born in 1990, 180cm, 80kg
    struct person p = { 0, };

    // initialize
    person_init(&p, "John Doe", 1990, 180, 80);

    // print
    char buffer[256] = { 0, };
    person_to_string(&p, buffer, sizeof(buffer));
    printf("%s\n", buffer);

    // deinitialize
    person_deinit(&p);

    return result;
}

int main(int argc, char** argv) {
    // 0: success, 1: failure
    int result = 0;
    int func_result = 0;
    func_result = leaning_stack_memory();
    if (func_result != 0) {
        result = 1;
        goto main_exit;
    }

    func_result = learning_heap_memory();
    if (func_result != 0) {
        result = 1;
        goto main_exit;
    }

    func_result = learning_oop_in_c();
    if (func_result != 0) {
        result = 1;
        goto main_exit;
    }

main_exit:
    return result;
}