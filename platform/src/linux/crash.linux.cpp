//
// Created by cakilgan on 3/29/26.
//
#include <omnix/platform/crash.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <execinfo.h>

namespace ox {
    void panic(const char* msg) {
        write(2, "PANIC: ", 7);
        write(2, msg, strlen(msg));
        write(2, "\n", 1);
        OX_TRAP();
    }
    void crash_handler(int sig) {
        void* stack[32];
        int size = backtrace(stack, 32);

        write(2, "=== CRASH ===\n", 14);

        write(2, strsignal(sig), strlen(strsignal(sig)));
        write(2, "\n", 1);

        backtrace_symbols_fd(stack, size, 2);

        _exit(1);
    }
    void install_crash_handler() {
        signal(SIGSEGV,crash_handler);
        signal(SIGABRT,crash_handler);
        signal(SIGILL ,crash_handler);
    }
}
