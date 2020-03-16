#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <libunwind-ptrace.h>

#include <cxxabi.h>

#include <stdio.h>
#include <stdlib.h>

#include <thread>
#include <iostream>

#include <sys/syscall.h>

#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


void backtrace()
{
    unw_cursor_t cursor;
    unw_context_t context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    pid_t pid = getpid();

    void *p = _UPT_create(pid);
    unw_accessors_t ap;
    unw_addr_space_t addr = unw_create_addr_space(&ap,  __LITTLE_ENDIAN);
    unw_init_remote(&cursor, addr, p);

    int n=0;

    printf("backtrace(LWP %d)\n", pid);

    while ( unw_step(&cursor) ) {
        unw_word_t ip, sp, off;

        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);

        char symbol[256] = {"<unknown>"};
        char *name = symbol;

        if ( !unw_get_proc_name(&cursor, symbol, sizeof(symbol), &off) ) {
            int status;
            if ( (name = abi::__cxa_demangle(symbol, NULL, NULL, &status)) == 0 )
                name = symbol;
        }

        printf("#%-2d 0x%016" PRIxPTR " sp=0x%016" PRIxPTR " %s 0x%" PRIxPTR "\n",
                ++n,
                static_cast<uintptr_t>(ip),
                static_cast<uintptr_t>(sp),
                name,
                static_cast<uintptr_t>(off));

        if ( name != symbol )
            free(name);
    }
    printf("\n");

}

int fac()
{
    backtrace();
}

int main()
{
    fac();
    return 0;
}

/*
 * To compile:
        g++ -I/path/to/unwind/include -L/path/to/unwind/lib unwind_example.cpp -lunwind -lunwind-generic -lunwind-ptrace
 */
