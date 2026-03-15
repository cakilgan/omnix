//
// Created by cakilgan on 3/8/26.
//
#include <omnix/platform/macros.h>
#include <omnix/platform/detect.h>
#include <omnix/platform/assert.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <execinfo.h>



// TODO: when i create a logging api, implement a logger_assert_handler.
// * that will do the job for now
using namespace ox;
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

assert_handler ox::g_assert_handler = default_assert_handler;//
