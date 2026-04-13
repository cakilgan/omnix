
#include <omnix/runtime.h>
#include <omnix/platform/crash.h>
#include <omnix/runtime/logger.h>
#include <omnix/runtime/setup.h>

#include <omnix/core/log.h>

#if OX_IS(PLATFORM,LINUX)
#include <omnix/platform/assert.h>

using namespace ox;
using engine::literals::operator""_k;

void logger_assert_handler(cstr expr,
                            cstr file,
                            i32 line,
                            cstr msg)
{
    fatal("ASSERTION FAILED :",
          "Expr"_k = expr,
          "File"_k = file,
          "Line"_k = line,
          "Msg"_k = msg ? msg:"none"
    );

    //warn: stacktrace disabled for now
/*
    #if OX_IS(PLATFORM,LINUX)
    void* bt[64];
    int n = backtrace(bt, 64);

    char** arr = backtrace_symbols(bt, n);
    if (arr) {
        for (int i = 0; i < n; i++) {
            fatal("stacktrace[%d]: %s", i, arr[i]);
        }
        free(arr);
    }
    #endif
*/

    std::abort();
}

assert_handler ox::g_assert_handler = logger_assert_handler;
#elif OX_IS(PLATFORM,WINDOWS)
#else
void default_assert_handler(cstr expr,
                            cstr file,
                            i32 line,
                            cstr msg)
{
    std::fprintf(stderr, "\033[1;31mASSERTION FAILED\033[0m\n");
    std::fprintf(stderr, "Expr : %s\n", expr);
    std::fprintf(stderr, "File : %s:%d\n", file, line);
    std::fprintf(stderr, "Msg  : %s\n", msg ? msg : "none");

    #if OX_IS(PLATFORM,LINUX)
    std::fprintf(stderr, "Stacktrace (top 10):\n");
    void* bt[10];
    int n = backtrace(bt, 10);
    backtrace_symbols_fd(bt, n, STDERR_FILENO);
    #endif

    std::abort();
}
assert_handler ox::g_assert_handler = default_assert_handler;
#endif
