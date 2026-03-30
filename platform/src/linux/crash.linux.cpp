//
// Created by cakilgan on 3/29/26.
//

#include <omnix/platform/crash.h>

#include <unistd.h>
#include <csignal>
#include <execinfo.h>

namespace ox {

    volatile sig_atomic_t g_intentional_trap = 0;

    static inline void write_str(int fd, const char* str, size_t len) {
        write(fd, str, len);
    }

    struct sig_str {
        const char* str;
        size_t len;
    };

    static constexpr sig_str signal_to_str(int sig) {
        switch(sig) {
            case SIGSEGV: return {"SIGSEGV", 7};
            case SIGABRT: return {"SIGABRT", 7};
            case SIGILL:  return {"SIGILL", 6};
            case SIGFPE:  return {"SIGFPE", 6};
            default:      return {"UNKNOWN", 7};
        }
    }

    template<size_t N>
    static inline void write_lit(int fd, const char (&str)[N]) {
        write(fd, str, N - 1);
    }

    void panic(const char* msg) {
        g_intentional_trap = 1;

        write_lit(2, "PANIC: ");

        const char* p = msg;
        while (*p) ++p;
        write_str(2, msg, p - msg);

        write_str(2, "\n", 1);

        write_lit(2,
                  "CAUTION: Intentional trap (not a real illegal instruction).\n"
                  "A runtime check failed.\n"
        );

        OX_TRAP();
    }

    void crash_handler(int sig) {
        void* stack[32];
        int size = backtrace(stack, 32);

        if (g_intentional_trap) {
            write_str(2, "=== PANIC ===\n", 14);
            write_lit(2,
                      "CAUTION: Intentional trap (not a real crash)\n"
            );
        } else {
            write_str(2, "=== CRASH ===\n", 14);
            write_lit(2,
                      "FATAL: Unexpected crash (real signal)\n"
            );
        }

        sig_str s = signal_to_str(sig);
        write_str(2, s.str, s.len);
        write_str(2, "\n", 1);

        backtrace_symbols_fd(stack, size, 2);

        _exit(1);
    }

    void install_crash_handler() {
        struct sigaction sa{};
        sa.sa_handler = crash_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;

        sigaction(SIGSEGV, &sa, nullptr);
        sigaction(SIGABRT, &sa, nullptr);
        sigaction(SIGILL , &sa, nullptr);
        sigaction(SIGFPE , &sa, nullptr);
    }

} // namespace ox
