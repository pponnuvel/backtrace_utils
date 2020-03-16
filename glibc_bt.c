#include <execinfo.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void print_trace(void)
{
    void *array[25];
    size_t size;
    char **strings;
    size_t i;

    int *p = 0;
    *p = 1;

    size = backtrace(array, 25);
    backtrace_symbols_fd(array, size, 2);
}

void handle(int sig)
{
    printf("Caught:%d\n", sig);
    int *q = malloc(5);
    int *q = realloc(q+100, 100);
    print_trace();

    _exit(1);
}

int main(void)
{
    signal(SIGSEGV, handle);
    signal(SIGABRT, handle);

    int *p = malloc(1);
    realloc(p + 100, 100);
}
